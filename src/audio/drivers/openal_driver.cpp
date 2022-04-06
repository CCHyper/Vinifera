/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENAL_DRIVER.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         
 *
 *  @license       Vinifera is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 Vinifera is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "openal_driver.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "openal_globals.h"
#include "openal_util.h"
#include "vorbis_globals.h"
#include "vorbis_util.h"
#include "audio_util.h"
#include "ccfile.h"
#include "ramfile.h"

#define MINIMP3_IMPLEMENTATION // Required for minimp3 lib.
#include <minimp3.h>

#include <vorbisfile.h>
#include "wwaud.h"

#include "dsaudio.h"                        // Required for some enums and types.

#include "debughandler.h"
#include "asserthandler.h"

#include <iostream>
#include <fstream>


//#define OGG_TEST_STD_FSTREAM
#define OGG_TEST_CC_FILE


#define TEST_SOUND_FILENAME "TESTSOUND.OGG"
#define TEST_STREAM_FILENAME "TESTMUSIC.OGG"


// https://github.com/ptitSeb/stuntcarremake/blob/master/sound_openal.cpp
// https://github.com/seblef/DeathChase/blob/bb24f9adce681f3833c4c6a415c6b3beecd098b1/DeathEngine/Core/SoundSource.cpp
// https://ffainelli.github.io/openal-example/
// https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/
// https://indiegamedev.net/2020/02/25/the-complete-guide-to-openal-with-c-part-2-streaming-audio/
// https://indiegamedev.net/2020/01/16/how-to-stream-ogg-files-with-openal-in-c/
// https://web.archive.org/web/20060618124137/http://www.devmaster.net/articles/openal-ogg-file/
// https://web.archive.org/web/20060616033727/http://www.devmaster.net/articles.php?catID=6
// https://github.com/Warzone2100/warzone2100/blob/master/lib/sound/openal_track.cpp


static int StreamBufferSize;

static unsigned char *UncompBuffer;
        
static unsigned char *ChunkBuffer;

static unsigned char *FileStreamBuffer;
static bool IsUsingFileStreamBuffer;


typedef enum OpenALAudioDriverEnums {
    /**
     *  Numbers of OpenAL buffers (1 is playing, 2+ are queued).
     */
    NUM_BUFFERS = 2,

    MIN_SAMPLE_TRACKERS = 6,
    MAX_SAMPLE_TRACKERS = 32,

    STREAM_BUFFER_COUNT = 16,

    /**
     *  Max chunk size in auds.
     */
    MAX_AUD_CHUNK_SIZE = 512,
    AUD_MARGIN = 50,

    /**
     *  
     */
    BUFFER_CHUNK_SIZE = 8192,

    /**
     *  Uncompress buffer chunk size. 4 is the max size data would grow
     *  assuming we are decompressing an IMA based codec (IMA compresses to about / 4 the size).
     */
    UNCOMP_CHUNK_SIZE = (MAX_AUD_CHUNK_SIZE * 4) + AUD_MARGIN,

    /**
     *  Size of temp HMI low memory staging buffer.
     */
    //SECONDARY_BUFFER_SIZE = (1024*32), // 32kb
    SECONDARY_BUFFER_SIZE = 65536, // 32kb of data in each buffer



    OGG_BUFFER_SIZE = (4096 * 8) // How big a Ogg chunk we want to read from the stream on each update.

} OpenALAudioDriverEnums;


/**
 *  Each tracker will be one of these depending on the sample format.
 */
typedef enum SampleTrackerTypeEnum
{
    TRACKER_AUD,
    TRACKER_OGG,
    TRACKER_MP3,
    TRACKER_WAV,

    TRACKER_NONE = -1,
} SampleTrackerTypeEnum;


/**
 *  Returns the tracker type required from the requested sample.
 */
SampleTrackerTypeEnum Find_Sample_Tracker_Type(AudioSample sample)
{
    if (Sample_Is_Wav(sample)) {
        return TRACKER_WAV;

    } else if (Sample_Is_Ogg(sample)) {
        return TRACKER_OGG;

    } else if (Sample_Is_Mp3(sample)) {
        return TRACKER_MP3;

    } else if (Sample_Is_Aud(sample)) {
        return TRACKER_AUD;
    }

    return TRACKER_NONE;
}


/**
 *  Define the sample control structure which helps us to handle feeding
 *  data to the sound interrupt.
 */
typedef struct SampleTrackerStruct
{
    SampleTrackerStruct(int index = -1, bool is_score = false) :
        TrackerIndex(index),
        TrackerType(TRACKER_NONE),
        Filename(),
        IsActive(false),
        IsLoading(false),
        IsScore(is_score),
        IsMoreSource(false),
        IsOneShot(false),
        IsLooping(false),
        IsPaused(false),
        IsNeedService(false),
        Original(nullptr),
        Source(0),
        Format(0),
        SampleRate(0),
        AudioBuffers(),
        DataLength(0),
        DestPtr(nullptr),
        SourcePtr(nullptr),
        Remainder(0),
        Priority(is_score ? PRIORITY_MAX : PRIORITY_MIN),
        Volume(1.0f),
        Reducer(0.0f),
        Pitch(1.0f),
        Velocity(),
        Position(),
        Callback(nullptr),
        QueueBuffer(nullptr),
        QueueSize(0),
        Odd(0),
        FilePending(0),
        FilePendingSize(0),
        FileHandle(nullptr),
        FileBuffer(nullptr)
    {
        std::memset(&SampleInfo, 0, sizeof(SampleInfo));
    }

    ~SampleTrackerStruct()
    {
        Release();
    }

    /**
     *  x
     * 
     *  @warning: Do not call this from anywhere outside of the constructor!
     */
    bool Create(ALCdevice *device, int rate, int bits, bool stereo)
    {
        ASSERT(device != nullptr);

        /**
         *  Generate a Source to playback the Buffers.
         */
        alCall(alGenSources(1, &Source));

        SampleRate = rate;
        Format = Get_OpenAL_Format(bits, stereo ? 2 : 1);

        return true;
    }
    
    /**
     *  x
     */
    void Release()
    {
        /**
         *  Clean up the OpenAL source and buffers.
         */
        alSourceStop(Source);
        alDeleteSources(1, &Source);
        alDeleteBuffers(NUM_BUFFERS, AudioBuffers);

        switch (TrackerType) {
            case TRACKER_OGG:
                /**
                 *  Close the OggVorbis decoding stream.
                 */
                ov_clear(&SampleInfo.Ogg.Stream);
                break;
        };

        std::memset(&SampleInfo, 0, sizeof(SampleInfo));
        
        Filename.Release_Buffer();

        delete FileHandle;
        FileHandle = nullptr;
    }

    void Generate_Buffers()
    {
        alCall(alGenBuffers(NUM_BUFFERS, AudioBuffers));
    }

    /**
     *  Reset the source
     */
    void Reset_Source()
    {
        alCall(alSourcef(Source, AL_PITCH, 1.0f));
        alCall(alSourcef(Source, AL_GAIN, 1.0f));
        alCall(alSource3f(Source, AL_POSITION, 0.0f, 0.0f, 0.0f));
        alCall(alSource3f(Source, AL_VELOCITY, 0.0f, 0.0f, 0.0f));
        alCall(alSourcei(Source, AL_LOOPING, AL_FALSE));
    }
    
#if 0
    /**
     *  x
     */
    void Reset()
    {
        TrackerType = TRACKER_NONE;
        Filename.Release_Buffer();
        IsActive = false;
        IsLoading = false;
        IsScore = false;
        IsMoreSource = false;
        IsOneShot = false;
        IsLooping = false;
        IsPaused = false;
        IsNeedService = false;
        Original = nullptr;
        Source = 0;
        Format = 0;
        SampleRate = 0;
        //AudioBuffers
        DataLength = 0;
        DestPtr = nullptr;
        SourcePtr = nullptr;
        Remainder = 0;
        Priority = 0;
        Volume = 1.0f;
        Reducer = 0.0f;
        Pitch = 1.0f;
        for (int i = 0; i < 3; ++i) {
            Velocity[i] = 0.0f;
        }
        for (int i = 0; i < 3; ++i) {
            Position[i] = 0.0f;
        }
        Callback = nullptr;
        QueueBuffer = nullptr;
        QueueSize = 0;
        Odd = 0;
        FilePending = 0;
        FilePendingSize = 0;
        FileHandle = nullptr;
        FileBuffer = nullptr;
        
        /**
         *  Reset AUD info.
         */
        Aud.Compression = SCOMP_NONE;
        ZeroMemory(&Aud.sSOSInfo, sizeof(Aud.sSOSInfo));
        
        /**
         *  Reset Ogg info.
         */
        //ZeroMemory(&Ogg, sizeof(Ogg));
        
        /**
         *  Reset Mp3 info.
         */
        //ZeroMemory(&Mp3, sizeof(Mp3));
        
        /**
         *  Reset Wav info.
         */
        //ZeroMemory(&Wav, sizeof(Wav));
    }
#endif

#if 0
    void Clear()
    {
        Filename.Release_Buffer();

        IsActive = false;
        IsLoading = false;
        IsScore = false;

        Original = nullptr;
        DestPtr = nullptr;
        SourcePtr = nullptr;
        QueueBuffer = nullptr;
        FileBuffer = nullptr;

        Close_File();

        Aud.Compression = SCOMP_NONE;
        ZeroMemory(&Aud.sSOSInfo, sizeof(Aud.sSOSInfo));

        //ZeroMemory(&Ogg, sizeof(Ogg));

        //ZeroMemory(&Mp3, sizeof(Mp3));

        //ZeroMemory(&Wav, sizeof(Wav));
    }
#endif

    /**
     *  x
     */
    void Close_File()
    {
        if (FileHandle) {
            FileHandle->Close();
            delete FileHandle;
            FileHandle = nullptr;
        }
    }

    /**
     *  Play the source.
     */
    void Play()
    {
        alCall(alSourcePlay(Source));

        IsActive = true;
    }

    /**
     *  Stop this sample from playing.
     */
    bool Stop(bool force = false)
    {
        ALint status;
        alGetSourcei(Source, AL_SOURCE_STATE, &status);

        if (force || (IsActive && status == AL_STOPPED)) {
 
            ALuint buffer = 0;
            ALint queued = -1;
            ALint processed = -1;

            alCall(alSourceStop(Source));
        
            alCall(alGetSourcei(Source, AL_BUFFERS_QUEUED, &queued));
            alCall(alGetSourcei(Source, AL_BUFFERS_PROCESSED, &processed));

            if (processed > 0) {
                // // unqueue all processed buffers
                while (processed-- > 0) {
                    alCall(alSourceUnqueueBuffers(Source, 1, &buffer));
                }
            } else if (queued > 0) {
                //while (queued-- > 0) {
                //    alCall(alSourceUnqueueBuffers(Source, 1, &buffer));
                //}
            }

            // delete all but the first buffer, reuse first for new data.
            alCall(alDeleteBuffers(NUM_BUFFERS - 1, &AudioBuffers[1]));

            IsActive = false;

            if (!IsScore) {
                Original = nullptr;
            }

            IsLoading = false;
            Priority = PRIORITY_MIN;
            FilePending = 0;
            FilePendingSize = 0;
            QueueBuffer = nullptr;
            Callback = nullptr;

            if (FileBuffer) {
                if (FileBuffer == FileStreamBuffer) {
                    IsUsingFileStreamBuffer = false;
                } else {
                    delete FileBuffer;
                    FileBuffer = nullptr;
                }
            }

            Close_File();

            return true;
        }

        return false;
    }
    
    /**
     *  Query the playing state of the source.
     */
    bool Is_Playing()
    {
        ALint status;
        alCall(alGetSourcei(Source, AL_SOURCE_STATE, &status));
    
        return status == AL_PLAYING || status == AL_PAUSED;
    }
    
    /**
     *  Has a filename been set for this tracker?
     */
    bool Is_Filename_Set() const { return Filename.Is_Not_Empty(); }

    void Set_Volume(ALfloat new_volume)
    {
        Volume = new_volume;
        alCall(alSourcef(Source, AL_GAIN, new_volume));
    }

    void Set_Pitch(ALfloat new_pitch)
    {
        Pitch = new_pitch;
        alCall(alSourcef(Source, AL_PITCH, new_pitch));
    }

    void Set_Position(ALfloat x, ALfloat y, ALfloat z)
    {
        Position[0] = x;
        Position[1] = y;
        Position[2] = z;
        alCall(alSource3f(Source, AL_POSITION, x, y, z));
    }

    void Set_Velocity(ALfloat x, ALfloat y, ALfloat z)
    {
        Velocity[0] = x;
        Velocity[1] = y;
        Velocity[2] = z;
        alCall(alSource3f(Source, AL_VELOCITY, x, y, z));
    }

    void Set_Looping(ALboolean loop)
    {
        IsLooping = loop;
        alCall(alSourcei(Source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE));
    }

    /**
     *  
     */
    int TrackerIndex;
    
    /**
     *  
     */
    SampleTrackerTypeEnum TrackerType;

    /**
     *  The filename of the sample currently playing (if available).
     */
    Wstring Filename;

    /**
     *  This flags whether this sample structure is actively playing or not.
     */
    bool IsActive;

    /**
     *  Is this sample tracker considered dead (used when reassigning a tracker)?
     */
    //bool IsDead;

    /**
     *  This flags whether the sample is loading or has been started.
     */
    bool IsLoading;

    /**
     *  If this sample is really to be considered a score rather than
     *  a sound effect, then special rules apply. These largely fall into
     *  the area of volume control and priority.
     */
    bool IsScore;

    /**
     *  This flag indicates that there is more source data
     *  to copy to the play buffer.
     */
    bool IsMoreSource;

    /**
     *  This flag indicates that the entire sample fitted inside the
     *  play buffer so no additional loading was required.
     */
    bool IsOneShot;

    /**
     *  
     */
    bool IsLooping;

    /**
     *  
     */
    bool IsPaused;

    /**
     *  This flag indicates whether this sample needs servicing.
     *  Servicing entails filling one of the empty low buffers.
     */
    bool IsNeedService;

    /**
     *  This is the original sample pointer. It is used to control the sample based on
     *  pointer rather than handle. The handle method is necessary when more than one
     *  sample could be playing simultaneously. The pointer method is necessary when
     *  the dealing with a sample that may have stopped behind the programmer's back and
     *  this occurrence is not otherwise determinable. It is also used in
     *  conjunction with original size to unlock a sample which has been DPMI
     *  locked.
     */
    const void *Original;
    int OriginalSize;

    /**
     *  The number of bytes in the buffer that has been filled but is not
     *  yet playing.  This value is normally the size of the buffer,
     *  except for the case of the last bit of the sample.
     */
    int DataLength;

    /**
     *  Pointer into the play buffer for writing the next
     *  chunk of sample to.
     */
    void *DestPtr;

    /**
     *  Pointer to the sound data that has not yet been copied
     *  to the playback buffers.
     */
    void *SourcePtr;

    /**
     *  This is the number of bytes remaining in the source data as
     *  pointed to by the "Source" element.
     */
    int Remainder;

    /**
     *  Samples maintain a priority which is used to determine
     *  which sounds live or die when the maximum number of
     *  sounds are being played.
     */
    int Priority;

    /**
     *  This is the current volume of the sample as it is being played.
     */
    ALfloat Volume;

    /**
     *  Amount to reduce volume per tick when fading out.
     */
    ALfloat Reducer;

    /**
     *  
     */
    ALfloat Pitch;

    /**
     *  
     */
    ALfloat Position[3];

    /**
     *  
     */
    ALfloat Velocity[3];

    /**
     *  Streaming control handlers.
     */
    bool (*Callback)(short, short *, void **, int *);

    // Pointer to continued sample data.
    unsigned char *QueueBuffer;

    // Size of queue buffer attached.
    int QueueSize;

    // Block number tracker (0..StreamBufferCount-1).
    short Odd;

    // Number of buffers already filled ahead.
    int FilePending;

    // Number of bytes in last filled buffer.
    int FilePendingSize;

    /**
     *  The file variables are used when streaming directly off of the
     *  hard drive.
     */

    // Streaming file handle (ERROR = not in use).
    FileClass *FileHandle;

    // Temporary streaming buffer (allowed to be freed).
    unsigned char *FileBuffer;

    /**
     *  The the source of this sound.
     */
    ALuint Source;

    /**
     *  The internal format of the audio stream.
     */  
    ALenum Format;

    /**
     *  The frequency of the audio stream.
     */            
    ALuint SampleRate;

    /**
     *  
     */            
    ALuint SampleCount;

    /**
     *  A set of buffers.
     */
    ALuint AudioBuffers[NUM_BUFFERS];

    /**
     *  The following info structures are for the various supported sample types.
     */
    union {

        /**
         *  Westwood AUD
         */
        struct AudStruct {

            /**
             *  This is the compression that the sound data is using.
             */
            SCompressType Compression;

            /**
             *  This is a copy of the sample file header.
             */
            //AUDHeaderType Header;

            /**
             *  The following structure is used if the sample if compressed using
             *  the sos 16 bit compression codec.
             */
            _tagCOMPRESS_INFO2 sSOSInfo;

        } Aud;

        /**
         *  Ogg Vorbis
         */
        struct OggStruct {
        
            /**
             *  Internal Vorbis file identifier.
             */
            OggVorbis_File Stream;

            /**
             *  File stream handle for when we need to load the file ourselves.
             */
    //      std::ifstream *FileStream;
            FileClass *FileStream;
        
            /**
             *  
             */
            mmio_file_struct MemoryStream;

            /**
             *  Internal meta data.
             */
            vorbis_info Info;
        
            /**
             *  
             */
            vorbis_comment Comment;
        
            /**
             *  
             */
            ov_callbacks Callbacks;

            // used by the callbacks to understand where in the audio data that playback is up to.
            //ALsizei SizeConsumed = 0;

            // 
            //int CurrentSection;

            // the total length of the audio data.
            //int Duration;

        } Ogg;

        /**
         *  MPEG Audio Layer III
         */
        struct Mp3Struct {

            int tmp;

        } Mp3;

        /**
         *  WAVE
         */
        struct WavStruct {

            int tmp;

        } Wav;

    } SampleInfo;

} SampleTrackerStruct;


/**
 *  x
 * 
 *  @author: CCHyper
 */
OpenALAudioDriver::OpenALAudioDriver() :
    AudioDriver("OpenAL"),
    IsInitialised(false),
    IsEnabled(false),
    IsReverseChannels(false),
    SoundVolume(1.0f),
    ScoreVolume(1.0f),
    Device(nullptr),
    Context(nullptr),
    SampleTracker()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
OpenALAudioDriver::~OpenALAudioDriver()
{
    End();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioDriver::Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels)
{
    if (IsInitialised) {
        DEV_DEBUG_INFO("OpenAL: Audio driver is already initialised!\n");
        return true;
    }

    ALenum alerror;

    /**
     *  Open a device instance with the default preferred device.
     */
    ALCdevice *device = alcOpenDevice(nullptr);
    if (!device) {
        DEBUG_ERROR("OpenAL: Failed to open audio device!\n");
        return false;
    }
    Device = device;

    DEBUG_INFO("OpenAL: Information start:\n");

    // Check for EAX 2.0 support
    DEBUG_INFO("  EAX2.0 support = %s\n", alIsExtensionPresent("EAX2.0") ? "true" : "false");

    if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_TRUE) {
        DEBUG_INFO("  Default playback: %s\n", alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER));
        DEBUG_INFO("  Default capture: %s\n", alcGetString(nullptr, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER));
        DEBUG_INFO("  --- Devices list ---\n");
        // print all playback devices
        const char * s = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
        while (s && *s != '\0') {
            DEBUG_INFO("    Playback: %s\n", s); 
            s += std::strlen(s)+1;
        }
        // print all capture devices
        s = alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER);
        while (s && *s != '\0') { 
            DEBUG_INFO("    Capture: %s\n", s); 
            s += std::strlen(s)+1;
        }
    } else {
        DEBUG_WARNING("  No device enumeration available!\n");
    }

    DEBUG_INFO("  Default device: %s\n", alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER));
    DEBUG_INFO("  Default capture device: %s\n", alcGetString(nullptr, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER));

    /**
     *  Create the context for our device.
     */
    ALCcontext *context = alcCreateContext(Device, nullptr);
    if (!context) {
        DEBUG_ERROR("OpenAL: Failed to create audio context!\n");
        alcCloseDevice(Device);
        Device = nullptr;
        return false;
    }
    Context = context;

    ALCboolean alres = alcMakeContextCurrent(Context);
    if (!alres) {
        DEBUG_ERROR("OpenAL: Failed to make context current!\n");
        alcCloseDevice(Device);
        Device = nullptr;
        Context = nullptr;
        return false;
    }

    ALint version_major;
    ALint version_minor;
    alCall(alcGetIntegerv(Device, ALC_MAJOR_VERSION, 1, &version_major));
    alCall(alcGetIntegerv(Device, ALC_MINOR_VERSION, 1, &version_minor));

    //DEBUG_INFO("  ALC version: %d.%d\n", (int)version_major, (int)version_minor);
    //DEBUG_INFO("  ALC extensions: %s\n", alcGetString(Device, ALC_EXTENSIONS));

    if ((alerror = alGetError()) != AL_NO_ERROR) {
        DEBUG_ERROR("OpenAL: %s!\n", Get_OpenAL_Error(alerror));
        return false;
    }

    DEBUG_INFO("  Vendor string: %s\n", alGetString(AL_VENDOR));
    DEBUG_INFO("  Renderer string: %s\n", alGetString(AL_RENDERER));
    DEBUG_INFO("  Version string: %s\n", alGetString(AL_VERSION));
    //DEBUG_INFO("  OpenAL extensions: %s\n", alGetString(AL_EXTENSIONS));

    if ((alerror = alGetError()) != AL_NO_ERROR) {
        DEBUG_ERROR("OpenAL: %s!\n", Get_OpenAL_Error(alerror));
        return false;
    }

    DEBUG_INFO("OpenAL: Information end.\n");

    /**
     *  Allocate any required buffers.
     */
    UncompBuffer = new unsigned char[UNCOMP_CHUNK_SIZE];
    ChunkBuffer = new unsigned char[BUFFER_CHUNK_SIZE];

    StreamBufferSize = 8320;    // TODO: How is this size calcuated?
    FileStreamBuffer = nullptr;
    IsUsingFileStreamBuffer = false;


    /**
     *  Init the secondary sample trackers.
     */
    num_trackers = std::clamp<unsigned>(num_trackers, MIN_SAMPLE_TRACKERS, MAX_SAMPLE_TRACKERS);

    DEBUG_INFO("OpenAL: Allocating '%d' sample trackers.\n", num_trackers);

    for (int index = 0; index < num_trackers; ++index) {

        /**
         *  The first sample tracker is always reserved for the music stream.
         */
        bool is_score = (index == 0);

        SampleTrackerStruct *st = new SampleTrackerStruct(index, is_score);
        ASSERT(st != nullptr);

        if (!st->Create(Device, rate, bits_per_sample, stereo)) {
            DEBUG_ERROR("OpenAL: Failed to create source for sample tracker '%d'!\n", index);
            return false;
        }

        SampleTracker.Add(st);
    }

    /**
     *  Set the listener position.
     */
    //alCall(alListener3f(AL_GAIN, 1.0f));
    alCall(alListener3f(AL_POSITION, 0.0f, 0.0f, 1.0f));
    //alCall(alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f));

    /**
     *  Store and log listener initial volume and position.
     */
    alCall(alGetListenerf(AL_GAIN, &ListenerVolume));
    alCall(alGetListenerfv(AL_POSITION, (ALfloat *)&ListenerPosition));
    //alCall(alGetListenerfv(AL_VELOCITY, (ALfloat *)&ListenerVeloctiy));

    DEBUG_INFO("OpenAL: Listener volume: %f.\n", ListenerVolume);
    DEBUG_INFO("OpenAL: Listener position: X:%f Y:%f Z:%f.\n", ListenerPosition[0], ListenerPosition[1], ListenerPosition[2]);
    //DEBUG_INFO("OpenAL: Listener velocity: X:%f Y:%f Z:%f.\n", ListenerVeloctiy[0], ListenerVeloctiy[1], ListenerVeloctiy[2]);

    /**
     *  
     */
    IsReverseChannels = reverse_channels;

    /**
     *  
     */
    IsInitialised = true;
    IsEnabled = true;

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::End()
{
    if (Device) {

        for (int index = 0; index < SampleTracker.Count(); ++index) {

            SampleTrackerStruct *st = SampleTracker[index];
            if (!st) {
                continue;
            }

            Stop_Handle_Playing(index);
           
            alCall(alDeleteSources(1, &st->Source));

            st->Source = 0;

            if (st->FileBuffer) {
                if (st->FileBuffer != FileStreamBuffer) {
                    delete st->FileBuffer;
                }
                st->FileBuffer = nullptr;
            }
        }
    }

    alcMakeContextCurrent(nullptr);

    alcDestroyContext(Context);
    Context = 0;

    alcCloseDevice(Device);
    Device = nullptr;

    if (UncompBuffer) {
        delete UncompBuffer;
        UncompBuffer = nullptr;
    }

    if (FileStreamBuffer) {
        delete FileStreamBuffer;
        FileStreamBuffer = nullptr;
    }

    SampleTracker.Clear();

    IsUsingFileStreamBuffer = false;

    IsInitialised = false;
    IsEnabled = false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioDriver::Is_Available() const
{
    return IsInitialised && IsEnabled;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioDriver::Is_Enabled() const
{
    return IsEnabled;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Enable()
{
    Start_Primary_Sound_Buffer(true);

    IsEnabled = true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Disable()
{
    Stop_Primary_Sound_Buffer();

    IsEnabled = false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Stop_Handle_Playing(AudioHandle handle)
{
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    ASSERT(st != nullptr);

    if (!st->IsActive && !st->IsLoading) {
        DEV_DEBUG_WARNING("OpenAL: Attempting to stop a inactive sample tracker!\n");
        return;
    }

    st->Stop(true);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioDriver::Is_Handle_Playing(AudioHandle handle) const
{
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return false;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    ASSERT(st != nullptr);

    if (!st->IsActive || !st->Source) {
        return false;
    }

    if (st->IsLoading) {
        return true;
    }

    return st->Is_Playing();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioDriver::Is_Sample_Playing(AudioSample sample) const
{
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return false;
    }

    if (!sample) {
        return false;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];
        ASSERT(st != nullptr);
        if (st->Original == sample) {
            return st->Is_Playing();
        }
    }

    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioDriver::Is_Handle_Looping(AudioHandle handle) const
{
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return false;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    ASSERT(st != nullptr);

    if (!st->IsActive || !st->Source) {
        return false;
    }

    return st->IsLooping;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Stop_Sample_Playing(AudioSample sample)
{
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!sample) {
        return;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];
        ASSERT(st != nullptr);
        if (st->Original == sample) {
            st->Stop(true);
            break;
        }
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioHandle OpenALAudioDriver::Play_Sample(AudioSample sample, int priority, int volume)
{
#if 0
#if 0
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return INVALID_AUDIO_HANDLE;
    }
#endif

    return Play_Sample_Handle(sample, priority, volume, Get_Free_Sample_Handle(priority));
#endif





    SampleTrackerStruct *st = nullptr;

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        ALint status;
        alCall(alGetSourcei(SampleTracker[i]->Source, AL_SOURCE_STATE, &status));
        if (!SampleTracker[i]->IsActive) {
            st = SampleTracker[i];
            DEBUG_INFO("OpenAL: Play_Sample - Using sample tracker '%d'.\n", st->TrackerIndex);
            break;
        }
    }

    if (!st) {
        DEBUG_ERROR("OpenAL: Play_Sample - Unable to find free sample tracker!\n");
        return INVALID_AUDIO_HANDLE;
    }



    st->Release();



    //st->TrackerType = Find_Sample_Tracker_Type(sample);
    st->TrackerType = TRACKER_OGG;



    // Really bad kludge, but use the sample pointer as the name.
    char pbuf[16];
    std::snprintf(pbuf, sizeof(pbuf), "%p", sample);
    st->Filename = pbuf;

    st->Priority = priority;

    st->Original = sample;

    st->Set_Pitch(1.0f);
    st->Set_Volume(Vol255_To_OpenAL_Volume(volume));





    switch (st->TrackerType) {

        default:
            DEBUG_WARNING("OpenAL: Sample type is unsupported!\n");
            break;

        case TRACKER_AUD:
        {
            DEBUG_INFO("OpenAL: Sample type is 'AUD'.\n");

            int header_size;
            unsigned short sample_rate;
            int size;
            int uncomp_size;
            unsigned char flags;
            unsigned char compression;

            AUD_Read_Reader(sample, header_size, sample_rate, size, uncomp_size, flags, compression);

            int bits_per_sample = (flags & 2) ? 16 : 8;
            int channels = (flags & 1) ? 2 : 1;

            st->OriginalSize = size + header_size;
            st->SourcePtr = (unsigned char *)sample + header_size;
            st->Remainder = size;

            st->SampleRate = sample_rate;
            st->SampleCount = sample_rate * channels;
            st->Format = Get_OpenAL_Format(bits_per_sample, channels);

            st->SampleInfo.Aud.Compression = SCompressType(compression);

            st->SampleInfo.Aud.sSOSInfo.wChannels = (flags & 1) + 1;
            st->SampleInfo.Aud.sSOSInfo.wBitSize = flags & 2 ? 16 : 8;
            st->SampleInfo.Aud.sSOSInfo.dwCompSize = size;
            st->SampleInfo.Aud.sSOSInfo.dwUnCompSize = size * (st->SampleInfo.Aud.sSOSInfo.wBitSize / 4);
            sosCODEC2InitStream(&st->SampleInfo.Aud.sSOSInfo);

            int buffer_index = 0;
            int bytes_read = 0;

            while (buffer_index < NUM_BUFFERS) {

                bytes_read = AUD_Sample_Copy(st->SampleInfo.Aud.Compression,
                                             &st->SampleInfo.Aud.sSOSInfo,
                                             &st->SourcePtr,
                                             &st->Remainder,
                                             (void **)&st->QueueBuffer,
                                             &st->QueueSize,
                                             ChunkBuffer,
                                             BUFFER_CHUNK_SIZE,
                                             UncompBuffer);

                if (bytes_read > 0) {
                    DEBUG_INFO("OpenAL: AUD - alBufferData bufidx:%d.\n", buffer_index);
                    alCall(alBufferData(st->AudioBuffers[buffer_index++], st->Format, ChunkBuffer, bytes_read, st->SampleRate));
                }

                if (bytes_read == BUFFER_CHUNK_SIZE) {
                    st->IsMoreSource = true;
                    st->IsOneShot = false;
                } else {
                    st->IsMoreSource = false;
                    st->IsOneShot = true;
                    break;
                }

            }

            break;
        }

        case TRACKER_WAV:
        {
            DEBUG_INFO("OpenAL: Sample type is 'WAV'.\n");

            short channels;
            int sample_rate;
            short bits_per_sample;
            int file_size;
            int data_size;

            WAV_Get_Info(sample, channels, sample_rate, bits_per_sample, file_size, data_size);

            st->OriginalSize = file_size;
            st->SourcePtr = (void *)sample;
            st->Remainder = data_size;

            st->SampleRate = sample_rate;
            st->SampleCount = sample_rate * channels;
            st->Format = Get_OpenAL_Format(bits_per_sample, channels);

            int buffer_index = 0;
            int bytes_read = 0;

            while (buffer_index < NUM_BUFFERS) {

                // TODO: needs to copy chunk size capped by "size".
                std::memcpy(&st->SourcePtr,
                            ChunkBuffer,
                            BUFFER_CHUNK_SIZE);

                if (bytes_read > 0) {
                    DEBUG_INFO("OpenAL: WAV - alBufferData bufidx:%d.\n", buffer_index);
                    alCall(alBufferData(st->AudioBuffers[buffer_index++], st->Format, ChunkBuffer, bytes_read, st->SampleRate));
                }

                if (bytes_read == BUFFER_CHUNK_SIZE) {
                    st->IsMoreSource = true;
                    st->IsOneShot = false;
                } else {
                    st->IsMoreSource = false;
                    st->IsOneShot = true;
                    break;
                }

            }

            break;
        }

        case TRACKER_OGG:
        {
            DEBUG_INFO("OpenAL: Sample type is 'OGG'.\n");

#if defined(OGG_TEST_STD_FSTREAM)

            st->Filename = TEST_SOUND_FILENAME;

            st->Ogg.FileStream = new std::ifstream;
            ASSERT(st->Ogg.FileStream != nullptr);

            st->Ogg.FileStream->open(st->Filename.Peek_Buffer(), std::ios::binary);
            if(!st->Ogg.FileStream->is_open()) {
                DEBUG_ERROR("OpenAL: Failed to open \"%s\"!\n", st->Filename.Peek_Buffer());
                return INVALID_AUDIO_HANDLE;
            }

            st->Ogg.FileStream->seekg(0, std::ios_base::beg);
            st->Ogg.FileStream->ignore(std::numeric_limits<std::streamsize>::max());
            int file_size = st->Ogg.FileStream->gcount();
            st->Ogg.FileStream->clear();
            st->Ogg.FileStream->seekg(0,std::ios_base::beg);

            st->Ogg.Callbacks.read_func = std_ogg_read_func;
            st->Ogg.Callbacks.close_func = std_ogg_close_func;
            st->Ogg.Callbacks.seek_func = std_ogg_seek_func;
            st->Ogg.Callbacks.tell_func = std_ogg_tell_func;

            void *file_stream = (const char *)st->Ogg.FileStream;
            const char *stream_data = nullptr;
            const int stream_data_size = -1;

#elif defined(OGG_TEST_CC_FILE)

            st->Filename = TEST_SOUND_FILENAME;

            st->SampleInfo.Ogg.FileStream = new CCFileClass(st->Filename.Peek_Buffer());
            ASSERT(st->SampleInfo.Ogg.FileStream != nullptr);

            st->SampleInfo.Ogg.FileStream->Set_Name(TEST_SOUND_FILENAME);
            if (!st->SampleInfo.Ogg.FileStream->Open(FILE_ACCESS_READ)) {
                DEBUG_ERROR("OpenAL: Failed to open \"%s\"!\n", st->Filename.Peek_Buffer());
                return INVALID_AUDIO_HANDLE;
            }

            int file_size = st->SampleInfo.Ogg.FileStream->Size();

            if (dynamic_cast<CCFileClass *>(st->SampleInfo.Ogg.FileStream)) {
                st->SampleInfo.Ogg.Callbacks.read_func = mix_ogg_read_func;
                st->SampleInfo.Ogg.Callbacks.close_func = mix_ogg_close_func;
                st->SampleInfo.Ogg.Callbacks.seek_func = mix_ogg_seek_func;
                st->SampleInfo.Ogg.Callbacks.tell_func = mix_ogg_tell_func;

            } else if (dynamic_cast<RAMFileClass *>(st->SampleInfo.Ogg.FileStream)) {
                st->SampleInfo.Ogg.Callbacks.read_func = ram_ogg_read_func;
                st->SampleInfo.Ogg.Callbacks.close_func = ram_ogg_close_func;
                st->SampleInfo.Ogg.Callbacks.seek_func = ram_ogg_seek_func;
                st->SampleInfo.Ogg.Callbacks.tell_func = ram_ogg_tell_func;

            } else if (dynamic_cast<RawFileClass *>(st->SampleInfo.Ogg.FileStream)) {
                st->SampleInfo.Ogg.Callbacks.read_func = raw_ogg_read_func;
                st->SampleInfo.Ogg.Callbacks.close_func = raw_ogg_close_func;
                st->SampleInfo.Ogg.Callbacks.seek_func = raw_ogg_seek_func;
                st->SampleInfo.Ogg.Callbacks.tell_func = raw_ogg_tell_func;

            } else {
                // Hmm?
            }
            
            void *file_stream = (void *)st->SampleInfo.Ogg.FileStream;
            const char *stream_data = nullptr;
            const int stream_data_size = -1;

#else

            // TODO: Possible we might have to limit Ogg to Music only as we need filesize...
            
            st->SampleInfo.Ogg.MemoryStream.curr_ptr = (char *)sample;
            st->SampleInfo.Ogg.MemoryStream.file_ptr = (char *)sample;
            st->SampleInfo.Ogg.MemoryStream.file_size = 0;     // TODO, somehow...

            void *file_stream = &st->SampleInfo.Ogg.MemoryStream;

            const char *stream_data = (const char *)sample;
            const int stream_data_size = st->SampleInfo.Ogg.MemoryStream.file_size;

            int file_size = st->SampleInfo.Ogg.MemoryStream.file_size;

            st->SampleInfo.Ogg.Callbacks.read_func = mmio_ogg_read_func;
            st->SampleInfo.Ogg.Callbacks.close_func = mmio_ogg_close_func;
            st->SampleInfo.Ogg.Callbacks.seek_func = mmio_ogg_seek_func;
            st->SampleInfo.Ogg.Callbacks.tell_func = mmio_ogg_tell_func;

#endif

            //st->Ogg.SizeConsumed = 0;

            /**
             *  
             */
            vorbisCall(ov_open_callbacks(file_stream, &st->SampleInfo.Ogg.Stream, stream_data, stream_data_size, st->SampleInfo.Ogg.Callbacks));

            /**
             *  Fetch some information about the vorbis file.
             */
            const vorbis_info *v_info = ov_info(&st->SampleInfo.Ogg.Stream, -1);
            const vorbis_comment *v_comment = ov_comment(&st->SampleInfo.Ogg.Stream, -1);

            CopyMemory(&st->SampleInfo.Ogg.Info, v_info, sizeof(vorbis_info));
            CopyMemory(&st->SampleInfo.Ogg.Comment, v_comment, sizeof(vorbis_comment));

            const long v_pcm_total = (long)ov_pcm_total(&st->SampleInfo.Ogg.Stream, -1);
            const double v_time_total = ov_time_total(&st->SampleInfo.Ogg.Stream, -1);

            st->OriginalSize = file_size;
            st->SourcePtr = (unsigned char *)sample;
            st->Remainder = file_size;

            st->SampleRate = v_info->rate;
            st->SampleCount = v_pcm_total * v_info->channels;
            st->Format = Get_OpenAL_Format(16, v_info->channels);

            //st->Ogg.Duration = ov_time_total(&st->Ogg.Stream, -1);

#ifndef NDEBUG
            DEBUG_INFO("Vorbis: Version: %s.\n", v_info->version);
            DEBUG_INFO("Vorbis: Bitstream is %d channel, %ldHz.\n", v_info->channels, v_info->rate);
            DEBUG_INFO("Vorbis: Bitrate - Upper:%d Avg:%d Lower:%d Wnd:%d.\n", v_info->bitrate_upper, v_info->bitrate_nominal, v_info->bitrate_lower, v_info->bitrate_window);
            DEBUG_INFO("Vorbis: Decoded length: %ld samples.\n", v_pcm_total);
            DEBUG_INFO("Vorbis: Total time: %lf.\n", v_time_total);
            DEBUG_INFO("Vorbis: Encoded by: %s.\n", v_comment->vendor);
            if (v_comment->comments > 0) {
                DEBUG_INFO("Vorbis: Comments:\n");
                for (int i = 0; i < v_comment->comments; i++) {
                    DEBUG_INFO("  \"%s\".\n", v_comment->user_comments[i]);
                }
            }
#endif

            break;
        }

        case TRACKER_MP3:
        {
            DEBUG_INFO("OpenAL: Sample type is 'MP3'.\n");

            DEBUG_WARNING("OpenAL: Mp3 is currently unsupported!\n");

            break;
        }

    };



    st->Play();




    ALint state = AL_PLAYING;
    while (state == AL_PLAYING) {
        alCall(alGetSourcei(st->Source, AL_SOURCE_STATE, &state));
        DEBUG_INFO("OpenAL: AL_PLAYING.\n");
    }

    


    st->IsActive = false; // temp while testing!



    /**
     *  
     */
    return 0; //INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioHandle OpenALAudioDriver::Play_File(Wstring &filename, int priority, int volume)
{
#if 0
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return INVALID_AUDIO_HANDLE;
    }

    // TODO
#endif

    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioHandle OpenALAudioDriver::Stream_Sample(AudioSample sample, int volume, bool real_time_start)
{
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioHandle OpenALAudioDriver::Stream_File(Wstring &filename, int volume, bool real_time_start)
{
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return INVALID_AUDIO_HANDLE;
    }

    AudioHandle handle = Get_Free_Sample_Handle(PRIORITY_MAX);
    if (handle == INVALID_AUDIO_HANDLE) {
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    ASSERT(st != nullptr);
    
    /**
     *  
     */
    st->Stop(true);
    
    /**
     *  
     */
    st->Reset_Source();

    /**
     *  Create new audio buffers.
     */
    st->Generate_Buffers();

    //st->TrackerType = Find_Sample_Tracker_Type(sample);
    st->TrackerType = TRACKER_OGG;

    st->Filename = TEST_SOUND_FILENAME; //filename;

    st->Priority = PRIORITY_MAX;

    st->Set_Pitch(1.0f);
    st->Set_Volume(Vol255_To_OpenAL_Volume(volume));

    st->IsScore = true;

    st->FilePending = 0;
    st->FilePendingSize = 0;

    st->IsLoading = real_time_start;


    /**
     *  
     */
    switch (st->TrackerType) {

        default:
            DEBUG_WARNING("OpenAL: Sample type is unsupported!\n");
            break;

        case TRACKER_AUD:
        {
            DEBUG_INFO("OpenAL: Sample type is 'AUD'.\n");

            FileClass *fh = new CCFileClass(filename.Peek_Buffer());
            if (!fh || !fh->Is_Available()) {
                return INVALID_AUDIO_HANDLE;
            }

            st->FileHandle = fh;

            int header_size;
            unsigned short sample_rate;
            int size;
            int uncomp_size;
            unsigned char flags;
            unsigned char compression;

            void *sample = nullptr;

            AUD_Read_Reader(sample, header_size, sample_rate, size, uncomp_size, flags, compression);

            int bits_per_sample = (flags & 2) ? 16 : 8;
            int channels = (flags & 1) ? 2 : 1;

            st->OriginalSize = size + header_size;
            st->SourcePtr = (unsigned char *)sample + header_size;
            st->Remainder = size;

            st->SampleRate = sample_rate;
            st->SampleCount = sample_rate * channels;
            st->Format = Get_OpenAL_Format(bits_per_sample, channels);

            st->SampleInfo.Aud.Compression = SCompressType(compression);

            st->SampleInfo.Aud.sSOSInfo.wChannels = (flags & 1) + 1;
            st->SampleInfo.Aud.sSOSInfo.wBitSize = flags & 2 ? 16 : 8;
            st->SampleInfo.Aud.sSOSInfo.dwCompSize = size;
            st->SampleInfo.Aud.sSOSInfo.dwUnCompSize = size * (st->SampleInfo.Aud.sSOSInfo.wBitSize / 4);
            sosCODEC2InitStream(&st->SampleInfo.Aud.sSOSInfo);

            int buffer_index = 0;
            int bytes_read = 0;

            while (buffer_index < NUM_BUFFERS) {

                bytes_read = AUD_Sample_Copy(st->SampleInfo.Aud.Compression,
                                             &st->SampleInfo.Aud.sSOSInfo,
                                             &st->SourcePtr,
                                             &st->Remainder,
                                             (void **)&st->QueueBuffer,
                                             &st->QueueSize,
                                             ChunkBuffer,
                                             BUFFER_CHUNK_SIZE,
                                             UncompBuffer);

                if (bytes_read > 0) {
                    DEBUG_INFO("OpenAL: AUD - alBufferData bufidx:%d.\n", buffer_index);
                    alCall(alBufferData(st->AudioBuffers[buffer_index++], st->Format, ChunkBuffer, bytes_read, st->SampleRate));
                }

                if (bytes_read == BUFFER_CHUNK_SIZE) {
                    st->IsMoreSource = true;
                    st->IsOneShot = false;
                } else {
                    st->IsMoreSource = false;
                    st->IsOneShot = true;
                    break;
                }

            }

            if (!FileStreamBuffer) {
                FileStreamBuffer = new unsigned char [(StreamBufferSize * STREAM_BUFFER_COUNT)];

                for (int i = 0; i < SampleTracker.Count(); ++i) {
                    SampleTracker[i]->FileBuffer = FileStreamBuffer;
                }
            }

            if (!FileStreamBuffer) {
                return INVALID_AUDIO_HANDLE;
            }
    
            File_Stream_Preload(handle);

            break;
        }

        case TRACKER_WAV:
        {
            DEBUG_INFO("OpenAL: Sample type is 'WAV'.\n");

            FileClass *fh = new CCFileClass(filename.Peek_Buffer());
            if (!fh || !fh->Is_Available()) {
                return INVALID_AUDIO_HANDLE;
            }

            st->FileHandle = fh;

            short channels;
            int sample_rate;
            short bits_per_sample;
            int file_size;
            int data_size;

            void * sample = nullptr;

            WAV_Get_Info(sample, channels, sample_rate, bits_per_sample, file_size, data_size);

            st->OriginalSize = file_size;
            st->SourcePtr = (void *)sample;
            st->Remainder = data_size;

            st->SampleRate = sample_rate;
            st->SampleCount = sample_rate * channels;
            st->Format = Get_OpenAL_Format(bits_per_sample, channels);

            int buffer_index = 0;
            int bytes_read = 0;

            while (buffer_index < NUM_BUFFERS) {

                // TODO: needs to copy chunk size capped by "size".
                std::memcpy(&st->SourcePtr,
                            ChunkBuffer,
                            BUFFER_CHUNK_SIZE);

                if (bytes_read > 0) {
                    DEBUG_INFO("OpenAL: WAV - alBufferData bufidx:%d.\n", buffer_index);
                    alCall(alBufferData(st->AudioBuffers[buffer_index++], st->Format, ChunkBuffer, bytes_read, st->SampleRate));
                }

                if (bytes_read == BUFFER_CHUNK_SIZE) {
                    st->IsMoreSource = true;
                    st->IsOneShot = false;
                } else {
                    st->IsMoreSource = false;
                    st->IsOneShot = true;
                    break;
                }

            }

            break;
        }

        case TRACKER_OGG:
        {
            DEBUG_INFO("OpenAL: Sample type is 'OGG'.\n");

            st->SampleInfo.Ogg.FileStream = new CCFileClass(st->Filename.Peek_Buffer());
            ASSERT(st->SampleInfo.Ogg.FileStream != nullptr);

            if (!st->SampleInfo.Ogg.FileStream->Open(FILE_ACCESS_READ)) {
                DEBUG_ERROR("OpenAL: Failed to open \"%s\"!\n", st->Filename.Peek_Buffer());
                return INVALID_AUDIO_HANDLE;
            }

            int file_size = st->SampleInfo.Ogg.FileStream->Size();

            if (dynamic_cast<CCFileClass *>(st->SampleInfo.Ogg.FileStream)) {
                st->SampleInfo.Ogg.Callbacks.read_func = mix_ogg_read_func;
                st->SampleInfo.Ogg.Callbacks.close_func = mix_ogg_close_func;
                st->SampleInfo.Ogg.Callbacks.seek_func = mix_ogg_seek_func;
                st->SampleInfo.Ogg.Callbacks.tell_func = mix_ogg_tell_func;

            } else if (dynamic_cast<RAMFileClass *>(st->SampleInfo.Ogg.FileStream)) {
                st->SampleInfo.Ogg.Callbacks.read_func = ram_ogg_read_func;
                st->SampleInfo.Ogg.Callbacks.close_func = ram_ogg_close_func;
                st->SampleInfo.Ogg.Callbacks.seek_func = ram_ogg_seek_func;
                st->SampleInfo.Ogg.Callbacks.tell_func = ram_ogg_tell_func;

            } else if (dynamic_cast<RawFileClass *>(st->SampleInfo.Ogg.FileStream)) {
                st->SampleInfo.Ogg.Callbacks.read_func = raw_ogg_read_func;
                st->SampleInfo.Ogg.Callbacks.close_func = raw_ogg_close_func;
                st->SampleInfo.Ogg.Callbacks.seek_func = raw_ogg_seek_func;
                st->SampleInfo.Ogg.Callbacks.tell_func = raw_ogg_tell_func;

            } else {
                DEBUG_ERROR("OpenAL: Invalid Vorbis file stream!\n");
                return INVALID_AUDIO_HANDLE;
            }

            //st->Ogg.SizeConsumed = 0;

            /**
             *  
             */
            vorbisCall(ov_open_callbacks(st->SampleInfo.Ogg.FileStream, &st->SampleInfo.Ogg.Stream, nullptr, -1, st->SampleInfo.Ogg.Callbacks));

            /**
             *  Fetch some information about the vorbis file.
             */
            const vorbis_info *v_info = ov_info(&st->SampleInfo.Ogg.Stream, -1);
            const vorbis_comment *v_comment = ov_comment(&st->SampleInfo.Ogg.Stream, -1);

            CopyMemory(&st->SampleInfo.Ogg.Info, v_info, sizeof(vorbis_info));
            CopyMemory(&st->SampleInfo.Ogg.Comment, v_comment, sizeof(vorbis_comment));

            const long v_pcm_total = (long)ov_pcm_total(&st->SampleInfo.Ogg.Stream, -1);
            const double v_time_total = ov_time_total(&st->SampleInfo.Ogg.Stream, -1);

            //st->Original = sample;
            st->OriginalSize = file_size;
            //st->SourcePtr = (unsigned char *)sample;
            st->Remainder = file_size;

            st->SampleRate = v_info->rate;
            st->SampleCount = v_pcm_total * v_info->channels;
            st->Format = Get_OpenAL_Format(16, v_info->channels);

            //st->Ogg.Duration = ov_time_total(&st->Ogg.Stream, -1);

#ifndef NDEBUG
            /**
             *  Log file meta data to the debugger.
             */
            DEBUG_INFO("Vorbis: Version: %s.\n", v_info->version);
            DEBUG_INFO("Vorbis: Bitstream is %d channel, %ldHz.\n", v_info->channels, v_info->rate);
            DEBUG_INFO("Vorbis: Bitrate - Upper:%d Avg:%d Lower:%d Wnd:%d.\n", v_info->bitrate_upper, v_info->bitrate_nominal, v_info->bitrate_lower, v_info->bitrate_window);
            DEBUG_INFO("Vorbis: Decoded length: %ld samples.\n", v_pcm_total);
            DEBUG_INFO("Vorbis: Total time: %lf.\n", v_time_total);
            DEBUG_INFO("Vorbis: Encoded by: %s.\n", v_comment->vendor);
            if (v_comment->comments > 0) {
                DEBUG_INFO("Vorbis: Comments:\n");
                for (int i = 0; i < v_comment->comments; i++) {
                    DEBUG_INFO("  \"%s\".\n", v_comment->user_comments[i]);
                }
            }
#endif

            break;
        }

        case TRACKER_MP3:
        {
            DEBUG_INFO("OpenAL: Sample type is 'MP3'.\n");

            DEBUG_WARNING("OpenAL: Mp3 is currently unsupported!\n");

            break;
        }

    };
    
    /**
     *  
     */
    st->Play();

    /**
     *  
     */
    return st->TrackerIndex;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioDriver::Pause_Handle(AudioHandle handle)
{
    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioDriver::Resume_Handle(AudioHandle handle)
{
    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Fade_Sample(AudioHandle handle, int ticks)
{
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!Is_Handle_Playing(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    ASSERT(st != nullptr);

    /**
     *  
     */
    if (ticks == 0 || st->IsLoading) {
        Stop_Handle_Playing(handle);
    } else {
        st->Reducer = (st->Volume / (ticks + 1)); // TODO: convert to new float system.
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioHandle OpenALAudioDriver::Get_Playing_Sample_Handle(AudioSample sample) const
{
#if 0
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return INVALID_AUDIO_HANDLE;
    }

    // TODO

    return INVALID_AUDIO_HANDLE;
#endif

    return INVALID_AUDIO_HANDLE;
}


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Handle_Loop(AudioHandle handle, bool loop)
{
#if 0
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        return;
    }

    if (!st->IsActive) {
        return;
    }

    alCall(alSourcef(st->Source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE));

    st->IsLooping = loop;
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Sample_Loop(AudioSample sample, bool loop)
{
#if 0
    if (!sample) {
        return;
    }

    AudioHandle handle = Get_Playing_Sample_Handle(sample);
    if (handle == INVALID_AUDIO_HANDLE) {
        return;
    }

    Set_Handle_Loop(handle, loop);
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Stop_Handle_Looping(AudioHandle handle)
{
#if 0
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    // TODO
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Stop_Sample_Looping(AudioSample sample)
{
#if 0
    // TODO
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Score_Handle_Pause(AudioHandle handle)
{
#if 0
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        return;
    }

    if (!st->IsActive) {
        return;
    }

    if (!st->IsScore) {
        DEV_DEBUG_WARNING("OpenAL: Sample '%s' is not a score tracker!\n", handle);
        return;
    }

    alCall(alSourcePause(st->Source));

    st->IsPaused = true;
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Score_Handle_Resume(AudioHandle handle)
{
#if 0
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        return;
    }

    if (!st->IsActive) {
        return;
    }

    if (!st->IsScore) {
        DEV_DEBUG_WARNING("OpenAL: Sample '%s' is not a score tracker!\n", handle);
        return;
    }

    alCall(alSourcePlay(st->Source));

    st->IsPaused = false;
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Handle_Pitch(AudioHandle handle, float pitch)
{
#if 0
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        return;
    }

    if (!st->IsActive) {
        return;
    }

    alCall(alSourcef(st->Source, AL_PITCH, pitch));

    st->Pitch = pitch;
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Sample_Pitch(AudioSample sample, float pitch)
{
#if 0
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!sample) {
        return;
    }

    AudioHandle handle = Get_Playing_Sample_Handle(sample);
    if (handle == INVALID_AUDIO_HANDLE) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        return;
    }

    if (!st->IsActive) {
        return;
    }

    // TODO
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Handle_Pan(AudioHandle handle, float pan)
{
#if 0
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        return;
    }

    if (!st->IsActive) {
        return;
    }

    // TODO
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Sample_Pan(AudioSample sample, float pan)
{
#if 0
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!sample) {
        return;
    }

    AudioHandle handle = Get_Playing_Sample_Handle(sample);
    if (handle == INVALID_AUDIO_HANDLE) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        return;
    }

    if (!st->IsActive) {
        return;
    }

    // TODO
#endif
}
#endif


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Handle_Volume(AudioHandle handle, int volume)
{
#if 0
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        return;
    }

    if (!st->IsActive) {
        return;
    }

    // TODO
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Sample_Volume(AudioSample sample, int volume)
{
#if 0
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    if (!sample) {
        return;
    }

    AudioHandle handle = Get_Playing_Sample_Handle(sample);
    if (handle == INVALID_AUDIO_HANDLE) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        return;
    }

    if (!st->IsActive) {
        return;
    }

    // TODO
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioDriver::Set_Sound_Volume(int volume)
{
#if 0
    float oldvol = SoundVolume;
    SoundVolume = Vol255_To_OpenAL_Volume(volume);

    for (int index = 0; index < SampleTracker.Count(); ++index) {

        SampleTrackerStruct *st = SampleTracker[index];
        if (!st || !st->IsActive) {
            continue;
        }

        if (st->IsScore) {
            continue;
        }

        st->Set_Volume(SoundVolume * st->Volume);
    }

    return OpenAL_Volume_To_Vol255(oldvol);
#endif

    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioDriver::Set_Score_Volume(int volume)
{
#if 0
    float oldvol = ScoreVolume;
    ScoreVolume = Vol255_To_OpenAL_Volume(volume);

    for (int index = 0; index < SampleTracker.Count(); ++index) {

        SampleTrackerStruct *st = SampleTracker[index];
        if (!st || !st->IsActive) {
            continue;
        }

        if (!st->IsScore) {
            continue;
        }

        st->Set_Volume(ScoreVolume * st->Volume);
    }

    return OpenAL_Volume_To_Vol255(oldvol);
#endif

    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Set_Volume_All(int volume)
{
#if 0
    Set_Sound_Vol(volume);

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];

        if (!st) {
            continue;
        }

        if (!Sample_Status(index)) {
            continue;
        }

        st->Set_Volume(Vol255_To_OpenAL_Volume(SoundVolume *volume));
    }
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioDriver::Set_Volume_Percent_All(int vol_percent)
{
#if 0
    float newvol = vol_percent * SoundVolume / 100;   // TODO: convert to new float volume system.
    float oldvol = Set_Sound_Vol(newvol);

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];

        if (!st) {
            continue;
        }

        if (!Sample_Status(index)) {
            continue;
        }

        st->Set_Volume(newvol);
    }

    return OpenAL_Volume_To_Vol255(oldvol);
#endif

    return 0;
}


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
long OpenALAudioDriver::Sample_Length(AudioSample sample)
{
#if 0
    if (!sample) {
        return -1;
    }

    long time = -1;

    if (Sample_Is_Aud(sample)) {

        AUDHeaderType aud_hdr;
        CopyMemory(&aud_hdr, sample, sizeof(aud_hdr));

        time = aud_hdr.UncompSize;

        if (aud_hdr.Flags & 2) {
            time /= 2;
        }

        if (aud_hdr.Flags & 1) {
            time /= 2;
        }

        if (aud_hdr.Rate / 60 > 0) {
            time /= aud_hdr.Rate / 60;
        }

        return time;

    } else if (Sample_Is_Wav(sample)) {

        // TODO

    } else if (Sample_Is_Ogg(sample)) {

        //time = ov_time_total(&st->VorbisFile, -1);

    } else if (Sample_Is_Mp3(sample)) {

        // TODO

    }

    return time;
#endif

    return -1;
}
#endif


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioDriver::Start_Primary_Sound_Buffer(bool forced)
{
    if (!GameInFocus) {
        return false;
    }

    if (!Context) {
        return false;
    }

    /**
     *  Signal the OpenAL context to begin processing.
     */
    //alcProcessContext(Context);

    // TODO: Better to gain than process?
    alCall(alListenerf(AL_GAIN, ListenerVolume));

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Stop_Primary_Sound_Buffer()
{
    /**
     *  Stop all playing samples.
     */
    //for (int i = 0; i < SampleTracker.Count(); ++i) {
    //    Stop_Sample(i);
    //}
    
    /**
     *  Signal the OpenAL context to stop processing.
     */
    //if (Context) {
    //    alcSuspendContext(Context);
    //}

    // TODO: Better to mute than stop processing?
    alCall(alGetListenerf(AL_GAIN, &ListenerVolume));
    alCall(alListenerf(AL_GAIN, 0.0f));
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Sound_Maintenance_Callback()
{
#if 0
    for (int i = 0; i < SampleTracker.Count(); ++i) {

        SampleTrackerStruct *st = SampleTracker[i];

        if (st->IsActive) { // If this tracker needs processing and isn't already marked as being processed, then process it.

            DEBUG_INFO("OpenAL: ******* Processing '%s' *******\n", st->Filename.Peek_Buffer());

            if (st->IsNeedService) {
                // Do we have more data in this tracker to play?
                if (st->IsMoreSource) {
                    ALint processed_buffers;

                    // Work out if we have any space to buffer more data right now.
                    alGetSourcei(st->Source, AL_BUFFERS_PROCESSED, &processed_buffers);

                    int bytes_copied = 0;

                    while (processed_buffers > 0 && st->IsMoreSource) {

                        switch (st->TrackerType) {
                            default:
                                break;
                            case TRACKER_AUD:
                                bytes_copied = AUD_Sample_Copy(st,
                                                             &st->SourcePtr,
                                                             &st->Remainder,
                                                             (void **)&st->QueueBuffer,
                                                             &st->QueueSize,
                                                             ChunkBuffer,
                                                             BUFFER_CHUNK_SIZE,
                                                             UncompBuffer);
                                break;
                        };

                        if (bytes_copied != BUFFER_CHUNK_SIZE) {
                            st->IsMoreSource = false;
                        }

                        if (bytes_copied > 0) {
                            ALuint buffer;
                            alSourceUnqueueBuffers(st->Source, 1, &buffer);
                            alBufferData(buffer, st->Format, ChunkBuffer, bytes_copied, st->SampleRate);
                            alSourceQueueBuffers(st->Source, 1, &buffer);
                            --processed_buffers;
                        }
                    }
                } else {
                    ALint status;
                    alGetSourcei(st->Source, AL_SOURCE_STATE, &status);

                    if (status != AL_PLAYING) {
                        st->IsNeedService = 0;
                        Stop_Sample(i);
                    }
                }
            }

            if (!st->QueueBuffer && st->FilePending != 0) {
                st->QueueBuffer = static_cast<unsigned char*>(st->FileBuffer)
                                  + StreamBufferSize * (st->Odd % STREAM_BUFFER_COUNT);
                --st->FilePending;
                ++st->Odd;

                if (st->FilePending != 0) {
                    st->QueueSize = StreamBufferSize;
                } else {
                    st->QueueSize = st->FilePendingSize;
                }
            }
        }
    }

    // Perform any volume modifications that need to be made.
    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerStruct * st = SampleTracker[i];
        if (st->IsActive && st->Reducer > 0.0f && st->Volume > 0.0f) {
            if (st->Reducer >= st->Volume) {
                st->Volume = 0.0f;
            } else {
                st->Volume -= st->Reducer;
            }

            if (st->IsScore) {
                st->Set_Volume(ScoreVolume * st->Volume);
            } else {
                st->Set_Volume(SoundVolume * st->Volume);
            }
        }
    }
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::Sound_Callback()
{
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    for (int i = 0; i < SampleTracker.Count(); ++i) {

        SampleTrackerStruct *st = SampleTracker[i];


        ALint _processed = -1;
        alCall(alGetSourcei(st->Source, AL_BUFFERS_PROCESSED, &_processed));
        if (_processed <= 0) {
            continue;
        }









        // Do we have more data in this tracker to play?
        if (st->IsMoreSource) {

            ALuint buffer = 0;
            ALint processed = -1;

            // Work out if we have any space to buffer more data right now.
            alCall(alGetSourcei(st->Source, AL_BUFFERS_PROCESSED, &processed));

            switch (st->TrackerType) {
                default:
                    DEBUG_WARNING("OpenAL: Sample type is unsupported!\n");
                    break;
                case TRACKER_AUD:
                {
                    //DEBUG_INFO("OpenAL: Sample type is 'AUD'.\n");

                    int bytes_copied = 0;

                    while (processed > 0 && st->IsMoreSource) {

                        bytes_copied = AUD_Sample_Copy(st->SampleInfo.Aud.Compression,
                                                       &st->SampleInfo.Aud.sSOSInfo,
                                                       &st->SourcePtr,
                                                       &st->Remainder,
                                                       (void **)&st->QueueBuffer,
                                                       &st->QueueSize,
                                                       ChunkBuffer,
                                                       BUFFER_CHUNK_SIZE,
                                                       UncompBuffer);

                        if (bytes_copied != BUFFER_CHUNK_SIZE) {
                            st->IsMoreSource = false;
                        }

                        if (bytes_copied > 0) {
                            alCall(alSourceUnqueueBuffers(st->Source, 1, &buffer));
                            alCall(alBufferData(buffer, st->Format, ChunkBuffer, bytes_copied, st->SampleRate));
                            alCall(alSourceQueueBuffers(st->Source, 1, &buffer));
                            --processed;
                        }
                    }

                    break;
                }
                case TRACKER_WAV:
                {
                    //DEBUG_INFO("OpenAL: Sample type is 'WAV'.\n");
                    break;
                }
                case TRACKER_OGG:
                {
                    //DEBUG_INFO("OpenAL: Sample type is 'OGG'.\n");
                    break;
                }
                case TRACKER_MP3:
                {
                    //DEBUG_INFO("OpenAL: Sample type is 'MP3'.\n");

                    DEBUG_WARNING("OpenAL: Mp3 is currently unsupported!\n");

                    break;
                }
            };

            continue;
        }







        st->Stop();


    }










#if 0
    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return;
    }

    Sound_Maintenance_Callback();

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerStruct *st = SampleTracker[i];

        // Is a load pending?
        if (st->IsLoading) {
            File_Stream_Preload(i);
            // We are done with this sample.
            continue;
        }

        // Is this sample inactive?
        if (!st->IsActive) {
            // If so, we close the handle.
            if (st->FileHandle != nullptr) {
                st->Close_File();
            }
            // We are done with this sample.
            continue;
        }

        // Has it been faded Is the volume 0?
        if (st->Reducer > 0.0f && !st->Volume == 0.0f) {
            // If so stop it.
            Stop_Sample(i);

            // We are done with this sample.
            continue;
        }

        // Process pending files.
        if (st->QueueBuffer == nullptr
            || st->FileHandle != nullptr && STREAM_BUFFER_COUNT - 3 > st->FilePending) {

            File_Callback(i, &st->Odd, (void **)&st->QueueBuffer, &st->QueueSize);
            //if (st->Callback != nullptr) {
            //    if (!st->Callback(i, &st->Odd, (void **)&st->QueueBuffer, &st->QueueSize)) {
            //        // No files are pending so pending file callback not needed anymore.
            //        st->Callback = nullptr;
            //    }
            //}

            // We are done with this sample.
            continue;
        }
    }
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioHandle OpenALAudioDriver::Get_Free_Sample_Handle(int priority)
{
    ASSERT(priority >= PRIORITY_MIN && priority <= PRIORITY_MAX);

    int found_index = 0;

    for (found_index = SampleTracker.Count() - 1; found_index >= 0; --found_index) {
        SampleTrackerStruct *st = SampleTracker[found_index];
        if (!st->IsActive && !st->IsLoading) {
            if (!st->IsScore) {
                break;
            }
        }
    }

    if (found_index < 0) {
        for (found_index = 0; found_index < SampleTracker.Count() && SampleTracker[found_index]->Priority > priority; ++found_index) {
            ;
        }

        if (found_index == SampleTracker.Count()) {
            return INVALID_AUDIO_HANDLE;
        }

        Stop_Handle_Playing(found_index);
    }

    if (found_index == INVALID_AUDIO_HANDLE) {
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerStruct *free_st = SampleTracker[found_index];
    ASSERT(free_st != nullptr);

    free_st->Close_File();

    if (free_st->Original) {
        if (!free_st->IsScore) {
            free_st->Original = nullptr;
        }
    }

    free_st->IsScore = false;

    return found_index;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioHandle OpenALAudioDriver::Play_Sample_Handle(AudioSample sample, int priority, int volume, AudioHandle handle)
{
#if 0
    ASSERT(sample != nullptr);
    ASSERT(priority >= PRIORITY_MIN && priority <= PRIORITY_MAX);
    ASSERT(volume >= VOLUME_MIN && volume <= VOLUME_MAX);
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerStruct *st = SampleTracker[handle];


    /**
     *  Reset the sample tracker so it's ready for the new sample.
     */
    st->IsDead = true;




    ALint status;
    alGetSourcei(st->Source, AL_SOURCE_STATE, &status);

    // If the sample is already playing stop it.
    if (status == AL_PLAYING || status == AL_PAUSED) {
        st->IsActive = false;
        st->IsNeedService = 0;
        st->IsMoreSource = false;

        ALint processed_count = -1;
        alSourceStop(st->Source);
        alGetSourcei(st->Source, AL_BUFFERS_PROCESSED, &processed_count);

        while (processed_count-- > 0) {
            ALuint tmp;
            alSourceUnqueueBuffers(st->Source, 1, &tmp);
        }

        alDeleteBuffers(NUM_BUFFERS, st->AudioBuffers);
    }




    if (Sample_Is_Aud(sample)) {

        // Read in the sample's header.
        AUDHeaderType raw_header;
        CopyMemory(&raw_header, sample, sizeof(AUDHeaderType));

        /**
         *  Keep a copy of the sample header.
         */
        CopyMemory(&st->Aud.Header, sample, sizeof(AUDHeaderType));

    //    // We don't support anything lower than 20000 hz.
    //    if (raw_header.Rate < 24000 && raw_header.Rate > 20000) {
    //        raw_header.Rate = 22050;
    //    }

        st->Aud.Compression = SCompressType(raw_header.Compression);

        //
        // Set up basic sample tracker info.
        //
        st->TrackerType = TRACKER_AUD;

        // Really bad kludge, but use the sample pointer as the name.
        char pbuf[16];
        std::snprintf(pbuf, sizeof(pbuf), "%p", sample);
        st->Filename = pbuf;

        st->Original = sample;
        st->Odd = 0;
        st->Reducer = 0.0f;
        //st->Restart = false;
        st->QueueBuffer = nullptr;
        st->QueueSize = 0;
        st->OriginalSize = raw_header.Size + sizeof(AUDHeaderType);
        st->Priority = priority;
        st->IsNeedService = 0;
        st->Remainder = raw_header.Size;
        st->SourcePtr = (unsigned char *)sample + sizeof(AUDHeaderType);

        // Compression is ADPCM so we need to init it's stream info.
        if (st->Aud.Compression == SCOMP_SOS) {
            st->Aud.sSOSInfo.wChannels = (raw_header.Flags & 1) + 1;
            st->Aud.sSOSInfo.wBitSize = raw_header.Flags & 2 ? 16 : 8;
            st->Aud.sSOSInfo.dwCompSize = raw_header.Size;
            st->Aud.sSOSInfo.dwUnCompSize = raw_header.Size * (st->Aud.sSOSInfo.wBitSize / 4);
            sosCODEC2InitStream(&st->Aud.sSOSInfo);
        }

        // If the loaded sample doesn't match the sample tracker we need to adjust the tracker.
        if (raw_header.Rate != st->SampleRate
            || Get_OpenAL_Format((raw_header.Flags & 2) ? 16 : 8, (raw_header.Flags & 1) ? 2 : 1) != st->Format) {
            st->IsActive = false;
            st->IsNeedService = 0;
            st->IsMoreSource = false;

            // Set the new sample info.
            st->SampleRate = raw_header.Rate;
            st->Format = Get_OpenAL_Format((raw_header.Flags & 2) ? 16 : 8, (raw_header.Flags & 1) ? 2 : 1);
        }


        st->IsDead = false;


    } else if (Sample_Is_Wav(sample)) {

        DEBUG_WARNING("OpenAL: Wav support not implemented!\n");

    } else if (Sample_Is_Ogg(sample)) {

        DEBUG_WARNING("OpenAL: Ogg support not implemented!\n");

    } else if (Sample_Is_Mp3(sample)) {

        DEBUG_WARNING("OpenAL: Mp3 support not implemented!\n");

    }


    // Bad check to see if the sample was loaded.
    if (st->IsDead) {
        //st->Clear();
        return INVALID_AUDIO_HANDLE;
    }


    // Create new buffers.
    alGenBuffers(NUM_BUFFERS, st->AudioBuffers);

    int buffer_index = 0;
    int bytes_read = 0;

    while (buffer_index < NUM_BUFFERS) {

        switch (st->TrackerType) {
            default:
                break;
            case TRACKER_AUD:
                bytes_read = AUD_Sample_Copy(st,
                                             &st->SourcePtr,
                                             &st->Remainder,
                                             (void **)&st->QueueBuffer,
                                             &st->QueueSize,
                                             ChunkBuffer,
                                             BUFFER_CHUNK_SIZE,
                                             UncompBuffer);
                break;
        };

        if (bytes_read > 0) {
            alBufferData(st->AudioBuffers[buffer_index++], st->Format, ChunkBuffer, bytes_read, st->SampleRate);
        }

        if (bytes_read == BUFFER_CHUNK_SIZE) {
            st->IsMoreSource = true;
            st->IsOneShot = false;
        } else {
            st->IsMoreSource = false;
            st->IsOneShot = true;
            break;
        }
    }

    alSourceQueueBuffers(st->Source, buffer_index, st->AudioBuffers);

    st->IsNeedService = true;

    st->Volume = Vol255_To_OpenAL_Volume(volume);

    alSourcef(st->Source, AL_GAIN, (SoundVolume * st->Volume));

    if (!Start_Primary_Sound_Buffer()) {
        //CCDebugString("Play_Sample_Handle - Can't start primary buffer!");
        return INVALID_AUDIO_HANDLE;
    }

    alSourcePlay(st->Source);

    // Playback was started so we set some needed sample tracker values.
    st->IsActive = true;

    return handle;
#endif

    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioHandle OpenALAudioDriver::Stream_Sample_Callback(void *buffer, int size, bool (*callbackptr)(short, short *, void **, int *), int volume, AudioHandle handle)
{
#if 0
    ASSERT(buffer != nullptr);
    ASSERT(size > 0);

    if (!Is_Available()) {
        DEV_DEBUG_WARNING("OpenAL: Driver was unavailable when calling \"%s\"!\n", __FUNCTION__);
        return INVALID_AUDIO_HANDLE;
    }

    int playid = INVALID_AUDIO_HANDLE;

    if (Sample_Is_Aud(buffer)) {

        AUDHeaderType header;
        CopyMemory(&header, buffer, sizeof(header));

        int oldsize = header.Size;
        header.Size = size - sizeof(header);
        CopyMemory(buffer, &header, sizeof(header));

        playid = Play_Sample_Handle(buffer, PRIORITY_MAX, volume, handle);

        header.Size = oldsize;
        CopyMemory(buffer, &header, sizeof(header));


    } else if (Sample_Is_Wav(buffer)) {

    } else if (Sample_Is_Ogg(buffer)) {

    } else if (Sample_Is_Mp3(buffer)) {

    }

    if (playid == INVALID_AUDIO_HANDLE) {
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerStruct *st = SampleTracker[playid];

    //st->Callback = callbackptr;
    st->Odd = 0;

    return playid;
#endif

    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioDriver::File_Callback(short handle, short *odd, void **buffer, int *size)
{
#if 0
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    SampleTrackerStruct *st = SampleTracker[handle];

    if (st->FileBuffer == nullptr) {
        return false;
    }

    if (*buffer == nullptr && st->FilePending) {
        *buffer =
            static_cast<unsigned char*>(st->FileBuffer) + StreamBufferSize * (*odd % STREAM_BUFFER_COUNT);
        --st->FilePending;
        ++*odd;
        *size = st->FilePending == 0 ? st->FilePendingSize : StreamBufferSize;
    }

    //Sound_Maintenance_Callback();

    int count = StreamLowImpact ? STREAM_BUFFER_COUNT / 2 : STREAM_BUFFER_COUNT - 3;

    if (count > st->FilePending && st->FileHandle != nullptr) {
        if (STREAM_BUFFER_COUNT - 2 != st->FilePending) {
            // Fill empty buffers.
            for (int num_empty_buffers = STREAM_BUFFER_COUNT - 2 - st->FilePending;
                 num_empty_buffers && st->FileHandle != nullptr;
                 --num_empty_buffers) {
                // Buffer to fill with data.
                void* tofill =
                    static_cast<unsigned char*>(st->FileBuffer)
                    + StreamBufferSize * ((st->FilePending + *odd) % STREAM_BUFFER_COUNT);

                int psize = st->FileHandle->Read(tofill, StreamBufferSize);

                if (psize != StreamBufferSize) {
                    st->Close_File();
                }

                if (psize > 0) {
                    st->FilePendingSize = psize;
                    ++st->FilePending;
                    //Sound_Maintenance_Callback();
                }
            }
        }

        if (st->QueueBuffer == nullptr && st->FilePending) {
            st->QueueBuffer = static_cast<unsigned char*>(st->FileBuffer)
                              + StreamBufferSize * (st->Odd % STREAM_BUFFER_COUNT);
            --st->FilePending;
            ++st->Odd;
            st->QueueSize = st->FilePending > 0 ? StreamBufferSize : st->FilePendingSize;
        }

        //OpenAL_This->Sound_Maintenance_Callback();
    }

    if (st->FilePending) {
        return true;
    }

    return false;
#endif

    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioDriver::File_Stream_Preload(AudioHandle handle)
{
#if 0
    ASSERT(handle >= 0 && handle < SampleTracker.Count());

    SampleTrackerStruct *st = SampleTracker[handle];
    int maxnum = (STREAM_BUFFER_COUNT / 2) + 4;
    int num = st->IsLoading ? std::min<int>(st->FilePending + 2, maxnum) : maxnum;

    int i = 0;
    
    //DEBUG_INFO("File_Stream_Preload - Before Read\n");

    for (i = st->FilePending; i < num; ++i) {
        int size = st->FileHandle->Read(static_cast<unsigned char*>(st->FileBuffer) + i * StreamBufferSize,
                                        StreamBufferSize);

        if (size > 0) {
            st->FilePendingSize = size;
            ++st->FilePending;
        }

        if (size < StreamBufferSize) {
            break;
        }
    }

    //DEBUG_INFO("File_Stream_Preload - After Read\n");

    //Sound_Maintenance_Callback();

    //DEBUG_INFO("File_Stream_Preload - After Sound_Maintenance_Callback\n");

    if (StreamBufferSize > st->FilePendingSize || i == maxnum) {
        int old_vol = SoundVolume;

        int stream_size = st->FilePending == 1 ? st->FilePendingSize : StreamBufferSize;
        
        //DEBUG_INFO("File_Stream_Preload - Before Stream_Sample_Vol\n");

        SoundVolume = ScoreVolume;
        Stream_Sample_Vol(st->FileBuffer, stream_size, nullptr /*File_Callback*/, st->Volume, handle);

        //DEBUG_INFO("File_Stream_Preload - After Stream_Sample_Vol\n");

        SoundVolume = old_vol;

        st->IsLoading = false;
        --st->FilePending;

        if (st->FilePending == 0) {
            st->Odd = 0;
            st->QueueBuffer = 0;
            st->QueueSize = 0;
            st->FilePendingSize = 0;
            st->Callback = nullptr;
            st->Close_File();
        } else {
            st->Odd = 2;
            --st->FilePending;

            if (st->FilePendingSize != StreamBufferSize) {
                st->Close_File();
            }

            st->QueueBuffer = static_cast<unsigned char*>(st->FileBuffer) + StreamBufferSize;
            st->QueueSize = st->FilePending == 0 ? st->FilePendingSize : StreamBufferSize;
        }
    }
#endif
}
