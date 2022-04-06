/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          OPENAL_AUDIO.CPP
 *
 *  @author        OmniBlade, CCHyper
 *
 *  @contributors  tomsons26
 *
 *  @brief         Main header file for the OpenAL audio interface.
 *
 *  @license       ProjectTSYR is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 ProjectTSYR is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "openal_audio.h"

#include "openal_load_dll.h"
#include "openal_globals.h"
#include "openal_util.h"

#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "ccfile.h"
#include "wwaud.h"
#include "soscodec.h"
#include "dsaudio.h"
#include "asserthandler.h"
#include "debughandler.h"


static OpenALAudioClass *OpenAL_This = nullptr;


/**
 *  Size of temp HMI low memory staging buffer.
 */
//SECONDARY_BUFFER_SIZE = (1024*32), // 32kb
#define SECONDARY_BUFFER_SIZE 65536 // 32kb of data in each buffer

#define STREAM_BUFFER_COUNT 16

/**
 *  Max chunk size in auds.
 */
#define MAX_AUD_CHUNK_SIZE 512
#define AUD_MARGIN 50

/**
 *  Uncompress buffer chunk size. 4 is the max size data would grow
 *  assuming we are decompressing an IMA based codec (IMA compresses to about / 4 the size).
 */
#define UNCOMP_CHUNK_SIZE (MAX_AUD_CHUNK_SIZE * 4) + AUD_MARGIN

#define VOLUME_SCALE 256


#define NUM_BUFFERS 2

#define BUFFER_CHUNK_SIZE 8192


typedef struct SampleTrackerStruct
{
    SampleTrackerStruct() :
        IsActive(false),
        IsLoading(false),
        IsScore(false),
        IsMoreSource(false),
        IsOneShot(false),
        IsLooping(false),
        IsPaused(false),
        IsNeedService(false),
        Original(nullptr),
        Source(0),
        Format(0),
        Frequency(0),
        AudioBuffers(),
        DataLength(0),
        DestPtr(nullptr),
        SourcePtr(nullptr),
        Remainder(0),
        Priority(0),
        Volume(1.0f),
        Reducer(0.0f),
        Callback(nullptr),
        QueueBuffer(nullptr),
        QueueSize(0),
        Odd(0),
        FilePending(0),
        FilePendingSize(0),
        FileHandle(nullptr),
        FileBuffer(nullptr)
    {
    }

    ~SampleTrackerStruct()
    {
        Delete_Source();
    }

    /**
     *  x
     * 
     *  @warning: Do not call this from anywhere outside of the constructor!
     */
    bool Create_Source(ALCdevice *device, int rate, int bits, bool stereo)
    {
        ASSERT(device != nullptr);

        /**
         *  Generate a Source to playback the Buffers.
         */
        alCall(device, alGenSources(1, &Source));

        Frequency = rate;
        Format = Get_OpenAL_Format(bits, stereo ? 2 : 1);

        return true;
    }
    
    /**
     *  x
     */
    void Delete_Source()
    {
        /**
         *  Clean up the Source and Buffers.
         */
        alSourceStop(Source);
        alDeleteSources(1, &Source);

        alDeleteBuffers(NUM_BUFFERS, AudioBuffers);
    }
    
    /**
     *  x
     */
    void Reset()
    {
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
        Frequency = 0;
        //AudioBuffers
        DataLength = 0;
        DestPtr = nullptr;
        SourcePtr = nullptr;
        Remainder = 0;
        Priority = 0;
        Volume = 1.0f;
        Reducer = 0.0f;
        Callback = nullptr;
        QueueBuffer = nullptr;
        QueueSize = 0;
        Odd = 0;
        FilePending = 0;
        FilePendingSize = 0;
        FileHandle = nullptr;
        FileBuffer = nullptr;
    }

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
     *  This flags whether this sample structure is active or not.
     */
    bool IsActive;

    /**
     *  Is this sample tracker considered dead (used when reassigning a tracker)?
     */
    bool IsDead;

    /**
     *  This flags whether the sample is loading or has been started.
     */
    bool IsLoading;

    /**
     *  If this sample is really to be considered a score rather than
     *  a sound effect, then special rules apply. These largely fall into
     *  the area of volume control.
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
    int Volume;

    /**
     *  Amount to reduce volume per tick when fading out.
     */
    int Reducer;

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
     *  A point in space that is the source of this sound.
     */
    ALuint Source;

    /**
     *  The format of the audio stream.
     */  
    ALenum Format;

    /**
     *  The Frequency of the audio stream.
     */            
    ALuint Frequency;

    /**
     *  A set of buffers.
     */
    ALuint AudioBuffers[NUM_BUFFERS];

    /**
        *  This is the compression that the sound data is using.
        */
    SCompressType Compression;

    /**
        *  The following structure is used if the sample if compressed using
        *  the sos 16 bit compression codec.
        */
    _tagCOMPRESS_INFO2 sSOSInfo;

} SampleTrackerStruct;


/**
 *  x
 * 
 *  @author: CCHyper
 */
OpenALAudioClass::OpenALAudioClass() :
    AudioDone(false),
    StreamBufferSize(8320),                       // TODO: How is this size set?
    UncompBuffer(nullptr /*new unsigned char[UNCOMP_CHUNK_SIZE]*/),
    FileStreamBuffer(nullptr /*new unsigned char[StreamBufferSize * STREAM_BUFFER_COUNT]*/),
    IsUsingFileStreamBuffer(false),
    SoundVolume(VOLUME_MAX),
    ScoreVolume(VOLUME_MAX),
    Device(nullptr),
    Context(nullptr),
    SampleTracker()
{
    OpenAL_This = this;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
OpenALAudioClass::~OpenALAudioClass()
{
    if (!AudioDone) {
        End();
    }
}


/**
 *  x
 * 
 *  @author: OmniBlade, CCHyper
 */
bool OpenALAudioClass::Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers)
{
    ALCenum alerror;
    ALCboolean alres;

    if (Device) {
        return true;
    }

    Device = alcOpenDevice(nullptr);
    if (!Device) {
        DEBUG_ERROR("OpenAL: Failed to open audio device!\n");
        Device = nullptr;
        return false;
    }

    /**
     *  Create the OpenAL context.
     */
    Context = alcCreateContext(Device, nullptr);
    if (!Context) {
        DEBUG_ERROR("OpenAL: Failed to create audio context!\n");
        alcCloseDevice(Device);
        Device = nullptr;
        Context = nullptr;
        return false;
    }

    alres = alcMakeContextCurrent(Context);
    if (!alres) {
        DEBUG_ERROR("OpenAL: Failed to make context current!\n");
        alcCloseDevice(Device);
        Device = nullptr;
        Context = nullptr;
        return false;
    }

    UncompBuffer = new unsigned char[UNCOMP_CHUNK_SIZE];
    ChunkBuffer = new unsigned char[BUFFER_CHUNK_SIZE];

    /**
     *  Init the audio sample trackers.
     */
    num_trackers = std::clamp<unsigned>(num_trackers, 5, 32);

    DEBUG_INFO("OpenAL: Allocating '%d' sample trackers.\n", num_trackers);

    for (int index = 0; index < num_trackers; ++index) {

        SampleTrackerStruct *st = new SampleTrackerStruct;
        ASSERT(st != nullptr);

        /**
         *  Generate a Source to playback the Buffers.
         */
        alGenSources(1, &st->Source);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return false;
        }

        st->Frequency = rate;
        st->Format = Get_OpenAL_Format(bits_per_sample, stereo ? 2 : 1);

        st->FileHandle = nullptr;
        st->QueueBuffer = nullptr;

        SampleTracker.Add(st);
    }

    AudioDone = false;

    return true;
}


/**
 *  x
 * 
 *  @author: OmniBlade, CCHyper
 */
void OpenALAudioClass::End()
{
    ALCenum alerror;

    if (Device) {

        for (int index = 0; index < SampleTracker.Count(); ++index) {

            SampleTrackerStruct *st = SampleTracker[index];
            if (!st) {
                continue;
            }

            Stop_Sample(index);
            
            if (OpenALImportsLoaded) {
                alDeleteSources(1, &st->Source);
                if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                    DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                    return;
                }
                st->Source = 0;
            }

            if (st->FileBuffer) {
                if (st->FileBuffer != FileStreamBuffer) {
                    delete st->FileBuffer;
                }
                st->FileBuffer = nullptr;
            }
        }
    }

    if (OpenALImportsLoaded) {
        alcMakeContextCurrent(nullptr);

        alcDestroyContext(Context);
        Context = 0;

        alcCloseDevice(Device);
        Device = nullptr;
    }

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

    AudioDone = true;
}


#if 0
/**
 *  x
 * 
 *  @author: OmniBlade, CCHyper
 */
void OpenALAudioClass::Stop_Sample(int handle)
{
    ALCenum alerror;

    if (!Device || AudioDone) {
        return;
    }

    if (handle < 0 && handle >= SampleTracker.Count()) {
        DEBUG_WARNING("OpenAL: Invalid handle in Stop_Sample()!\n");
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        DEBUG_WARNING("OpenAL: Null sample tracker!\n");
        return;
    }

    if (st->IsActive || st->IsLoading) {

        st->IsActive = false;
        st->Priority = 0;

        if (!st->IsScore) {
            st->Original = nullptr;
        }

        if (!st->IsLoading) {

            ALint processed_count = -1;

            /**
             *  Stop the source.
             */
            alSourceStop(st->Source);
            if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                return;
            }

            alGetSourcei(st->Source, AL_BUFFERS_PROCESSED, &processed_count);
            if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                return;
            }

            while (processed_count-- > 0) {
                ALuint tmp = 0;
                alSourceUnqueueBuffers(st->Source, 1, &tmp);
                if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                    DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                    return;
                }
            }

            /**
             *  Clean up buffers.
             */
            alDeleteBuffers(NUM_BUFFERS, st->AudioBuffers);
            if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                return;
            }
        }

        st->IsLoading = false;

        st->Close_File();

        if (st->FileBuffer) {
            if (st->FileBuffer == FileStreamBuffer) {
                IsUsingFileStreamBuffer = false;
            } else {
                delete st->FileBuffer;
                st->FileBuffer = nullptr;
            }
        }

        st->IsLoading = false;
        st->Priority = 0;
        st->FilePending = 0;
        st->FilePendingSize = 0;
        st->QueueBuffer = nullptr;
        st->Callback = nullptr;
    }
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: OmniBlade, CCHyper
 */
bool OpenALAudioClass::Sample_Status(int handle)
{
    ALCenum alerror;

    if (!Device || AudioDone) {
        return false;
    }

    if (handle < 0 && handle >= SampleTracker.Count()) {
        DEBUG_WARNING("OpenAL: Invalid handle in Sample_Status()!\n");
        return false;
    }

    SampleTrackerStruct *st = SampleTracker[handle];
    if (!st) {
        return false;
    }

    if (st->IsLoading) {
        return true;
    }

    if (!st->IsActive) {
        return false;
    }

    if (st->Source) {

        /**
         *  Get Source state.
         */
        ALint source_status;
        alGetSourcei(st->Source, AL_SOURCE_STATE, &source_status);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return false;
        }
        
        return source_status == AL_PLAYING || source_status == AL_PAUSED;
    }

    return false;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioClass::Is_Sample_Playing(AudioSample sample) const
{
    if (AudioDone || !sample) {
        return false;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];
        if (!st || st->Original != sample) {
            continue;
        }
        if (Sample_Status(index)) {
            return true;
        }
    }

    return false;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Stop_Sample_Playing(const void *sample)
{
    if (AudioDone || !sample) {
        return;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];
        if (!st || st->Original != sample) {
            continue;
        }
        if (st->IsLooping) {
            DEV_DEBUG_WARNING("OpenAL: Use Stop_Sample_Looping()!\n");
            continue;
        }
        if (Sample_Status(index)) {
            Stop_Sample(index);
        }
    }
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioClass::Get_Free_Sample_Handle(int priority)
{
    int index = INVALID_AUDIO_HANDLE;

    /**
     *  Walk the tracker backwards and find a free tracker handle. 
     */
    for (index = (SampleTracker.Count()-1); index >= 0; --index) {
        SampleTrackerStruct *st = SampleTracker[index];
        if (!st) {
            continue;
        }
        if (st->IsLooping || st->IsPaused) {
            continue;
        }
        if (!st->IsActive && !st->IsLoading) {
            break;
        }
    }

    /**
     *  We failed to find a free inactive handle, now find one with a
     *  lower priority than us so we can steal it.
     */
    if (index == INVALID_AUDIO_HANDLE) {
        for (index = 0; index < SampleTracker.Count(); ++index) {
            SampleTrackerStruct *st = SampleTracker[index];

            if (!st) {
                continue;
            }

            /**
             *  #BUGFIX: Don't steal score trackers!
             */
            if (st->IsScore) {
                continue;
            }

            /**
             *  Skip looping or paused samples.
             */
            if (st->IsLooping || st->IsPaused) {
                continue;
            }

            if (priority > st->Priority && priority < PRIORITY_MAX) {
                break;
            }
        }

        /**
         *  Index is out of range, was not able to steal a handle. Return error.
         */
        if (index >= SampleTracker.Count()) {
            return INVALID_AUDIO_HANDLE;
        }

        Stop_Sample(index);
    }

    /**
     *  Still not able to find a free handle? Return error.
     */
    if (index == INVALID_AUDIO_HANDLE) {
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerStruct *st = SampleTracker[index];

    if (!st) {
        return INVALID_AUDIO_HANDLE;
    }

    st->Close_File();

    if (st->Original && !st->IsScore) {
        st->Original = nullptr;
    }

    st->IsScore = false;

    DEBUG_INFO("OpenAL: Found free sample tracker '%d'.\n", index);

    return index;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioClass::Play_Sample(AudioSample sample, int priority, int volume)
{
    return Play_Sample_Handle(sample, priority, volume, Get_Free_Sample_Handle(priority));
}
#endif


/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioClass::File_Play_Sample(const char *filename, int priority, int volume)
{
    if (!Device || AudioDone) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!filename) {
        return INVALID_AUDIO_HANDLE;
    }

    int handle = Get_Free_Sample_Handle(PRIORITY_MAX);
    if (handle == INVALID_AUDIO_HANDLE) {
        DEBUG_ERROR("OpenAL: Unable to find free handle for \"%s\"!\n", filename);
        return INVALID_AUDIO_HANDLE;
    }

    FileClass *fh = Get_File_Handle(filename);
    if (!fh) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!fh->Is_Available()) {
        DEBUG_ERROR("OpenAL: File \"%s\" is not available!\n", fh->File_Name());
        return INVALID_AUDIO_HANDLE;
    }

    if (!fh->Open(FILE_ACCESS_READ)) {
        DEBUG_ERROR("OpenAL: Unable to open file \"%s\"!\n", fh->File_Name());
        return INVALID_AUDIO_HANDLE;
    }








    // TODO



    return INVALID_AUDIO_HANDLE;
}


#if 0
/**
 *  x
 * 
 *  @author: OmniBlade, CCHyper
 */
int OpenALAudioClass::Play_Sample_Handle(const void *sample, int priority, int volume, int handle)
{
    ALCenum alerror;

    if (!Device || AudioDone) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!sample) {
        return INVALID_AUDIO_HANDLE;
    }

    if (handle < 0 && handle >= SampleTracker.Count()) {
        DEBUG_WARNING("OpenAL: Invalid handle in Play_Sample_Handle()!\n");
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerStruct *st = SampleTracker[handle];

    if (!st) {
        return INVALID_AUDIO_HANDLE;
    }

    /**
     *  Is the same a Mp3 file?
     */
    if (Sample_Is_Mp3(sample)) {

        Mp3SampleTrackerStruct *mp3_st = (Mp3SampleTrackerStruct *)st;

    
    /**
     *  Is the same a Ogg Vorbis file?
     */
    } else if (VorbisImportsLoaded && Sample_Is_Ogg(sample)) {

        OggSampleTrackerStruct *ogg_st = (OggSampleTrackerStruct *)st;

        ogg_st->OggVorbisCallbacks.read_func = mmio_ogg_read_func;
        ogg_st->OggVorbisCallbacks.seek_func = mmio_ogg_seek_func;
        ogg_st->OggVorbisCallbacks.close_func = mmio_ogg_close_func;
        ogg_st->OggVorbisCallbacks.tell_func = mmio_ogg_tell_func;

        ogg_file oggfile;
        oggfile.filePtr = (char *)sample;
        oggfile.curPtr = (char *)sample;
        oggfile.fileSize = 9999;            // TODO, need to get file size.

        if (ogg_st->OggVorbisFile) {
            delete ogg_st->OggVorbisFile;
            ogg_st->OggVorbisFile = nullptr;
        }

        ogg_st->OggVorbisFile = new OggVorbis_File;
        ASSERT(ogg_st->OggVorbisFile != nullptr);

        /**
         *  Create an OggVorbis file stream.
         */
        int result = ov_open_callbacks(&oggfile, ogg_st->OggVorbisFile, nullptr, -1, ogg_st->OggVorbisCallbacks);
        if (result != 0) {
            switch (result) {
                case OV_EREAD:
                    DEBUG_INFO("OpenAL: A read from media returned an error.\n");
                    break;
                case OV_ENOTVORBIS:
                    DEBUG_INFO("OpenAL: Bitstream is not Vorbis data.\n");
                    break;
                case OV_EVERSION:
                    DEBUG_INFO("OpenAL: Vorbis version mismatch.\n");
                    break;
                case OV_EBADHEADER:
                    DEBUG_INFO("OpenAL: Invalid Vorbis bitstream header.\n");
                    break;
                case OV_EFAULT:
                    DEBUG_INFO("OpenAL: Internal logic fault.\n");
                    break;
                default:
                    DEBUG_INFO("OpenAL: Unknown Ogg open error.\n");
                    break;
            };
            return INVALID_AUDIO_HANDLE;
        }

        ogg_uint32_t ulFrequency = 0;
        ogg_uint32_t ulFormat = 0;
        ogg_uint32_t ulChannels = 0;
        ogg_uint32_t ulBufferSize = 0;

        /**
         *  Get some information about the file (Channels, Format, and Frequency).
         */
        ogg_st->OggVorbisInfo = ov_info(ogg_st->OggVorbisFile, -1);
        if (!ogg_st->OggVorbisInfo) {
            DEBUG_INFO("OpenAL: Error getting info from Ogg file.\n");
            return INVALID_AUDIO_HANDLE;
        }

        // #NOTE: The Buffer Size must be an exact multiple of the BlockAlignment ...

        ulFrequency = ogg_st->OggVorbisInfo->rate;
        ulChannels = ogg_st->OggVorbisInfo->channels;

        if (ogg_st->OggVorbisInfo->channels == 1) {
            DEBUG_INFO("OpenAL: Ogg file is MONO.\n");
            ulFormat = AL_FORMAT_MONO16;
            // Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
            ulBufferSize = ulFrequency >> 1;
            ulBufferSize -= (ulBufferSize % 2);

        } else if (ogg_st->OggVorbisInfo->channels == 2) {
            DEBUG_INFO("OpenAL: Ogg file is STEREO.\n");
            ulFormat = AL_FORMAT_STEREO16;
            // Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
            ulBufferSize = ulFrequency;
            ulBufferSize -= (ulBufferSize % 4);

        } else {
            DEBUG_WARNING("OpenAL: Unsupport Ogg format!\n");
            return INVALID_AUDIO_HANDLE;
        }

        ogg_st->Original = sample;
        ogg_st->Odd = 0;
        ogg_st->Reducer = 0;
        ogg_st->QueueBuffer = 0;
        ogg_st->QueueSize = 0;
        ogg_st->Priority = priority;
        ogg_st->IsNeedService = false;
        //ogg_st->Remainder = aud_hdr.Size;  // TODO, maybe oggfile.fileSize ?

        ogg_st->Frequency = ulFrequency;
        ogg_st->Format = ulFormat;


    /**
     *  Is the same a Wetwood AUD file?
     */
    } else if (Sample_Is_Aud(sample)) {

        SampleTrackerStruct *st = (SampleTrackerStruct *)st;

        AUDHeaderType aud_hdr;
        CopyMemory(&aud_hdr, sample, sizeof(AUDHeaderType));

        aud_hdr.Rate = std::clamp<unsigned short>(aud_hdr.Rate, 22050, 48000);

        st->Compression = (SCompressType)aud_hdr.Compression;

        if (st->Compression != SCOMP_SOS) {
            DEBUG_WARNING("OpenAL: Unsupported Westwood AUD codec!\n");
            return INVALID_AUDIO_HANDLE;
        }

        st->Original = sample;
        st->Odd = 0;
        st->Reducer = 0;
        st->QueueBuffer = 0;
        st->QueueSize = 0;
        st->Priority = priority;
        st->IsNeedService = false;
        st->Remainder = aud_hdr.Size;
        st->SourcePtr = (void *)(static_cast<const unsigned char *>(sample) + sizeof(AUDHeaderType));

        int bits = (aud_hdr.Flags & 2) ? 16 : 8;
        int channels = (aud_hdr.Flags & 1) ? 2 : 1;

        st->sSOSInfo.wChannels = channels;
        st->sSOSInfo.wBitSize = bits;
        st->sSOSInfo.dwCompSize = aud_hdr.Size;
        st->sSOSInfo.dwUnCompSize = aud_hdr.Size * (st->sSOSInfo.wBitSize / 4);
        sosCODEC2InitStream(&st->sSOSInfo);

        /**
         *  If the requested sample is a different format from the last sample, then
         *  recreate the sound buffer with the new format properties.
         */
        if (aud_hdr.Rate != st->Frequency || Get_OpenAL_Format(bits, channels) != st->Format) {

            DEBUG_INFO("OpenAL: Changing sample tracker '%d' format:\n", handle);
            DEBUG_INFO("  Frequency: %d (was %d)\n", aud_hdr.Rate, st->Frequency);
            DEBUG_INFO("  BitsPerSample: %d\n", (aud_hdr.Flags & 2) ? 16 : 8);
            DEBUG_INFO("  Channels: %s\n", (aud_hdr.Flags & 1) ? "stereo" : "mono");

            st->IsActive = false;
            st->IsNeedService = false;
            st->IsMoreSource = false;

            st->Frequency = aud_hdr.Rate;
            st->Format = Get_OpenAL_Format(bits, channels);
        }

    
    /**
     *  Unexpected file format.
     */
    } else {
        DEBUG_ERROR("OpenAL: Unexpected file format!\n");
        return INVALID_AUDIO_HANDLE;
    }

    /**
     *  Get Source state.
     */
    ALint source_status;
    alGetSourcei(st->Source, AL_SOURCE_STATE, &source_status);
    if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
        DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
        return INVALID_AUDIO_HANDLE;
    }

    /**
     *  If the previous sample is playing, stop it.
     */
    if (source_status != AL_STOPPED) {

        st->IsActive = false;
        st->IsNeedService = false;
        st->IsMoreSource = false;

        ALint processed_count = -1;

        /**
         *  Stop the source.
         */
        alSourceStop(st->Source);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return INVALID_AUDIO_HANDLE;
        }

        alGetSourcei(st->Source, AL_BUFFERS_PROCESSED, &processed_count);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return INVALID_AUDIO_HANDLE;
        }

        while (processed_count-- > 0) {
            ALuint tmp;
            alSourceUnqueueBuffers(st->Source, 1, &tmp);
            if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                return INVALID_AUDIO_HANDLE;
            }
        }

        alDeleteBuffers(NUM_BUFFERS, st->AudioBuffers);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return INVALID_AUDIO_HANDLE;
        }
    }

    /**
     *  Generate some AL Buffers for streaming.
     */
    alGenBuffers(NUM_BUFFERS, st->AudioBuffers);

    int bytes_read = 0;
    int buffer_index = 0;
    while (buffer_index < NUM_BUFFERS) {

        switch (st->TrackerType) {
            case TRACKER_AUD:
            {
                SampleTrackerStruct *st = (SampleTrackerStruct*)st;
                bytes_read = Aud_Sample_Copy(st,
                                             &st->SourcePtr,
                                             &st->Remainder,
                                             (void **)&st->QueueBuffer,
                                             &st->QueueSize,
                                             ChunkBuffer,
                                             BUFFER_CHUNK_SIZE,
                                             st->Compression);
                break;
            }
            case TRACKER_OGG:
            {
                OggSampleTrackerStruct *ogg_st = (OggSampleTrackerStruct *)st;
                bytes_read = ogg_vorbis_decode(ogg_st->OggVorbisFile,
                                               (char *)ChunkBuffer, 
                                                BUFFER_CHUNK_SIZE,
                                                ogg_st->OggVorbisInfo->channels);
                break;
            }
            default:
                break;
        };

        if (bytes_read > 0) {
            alBufferData(st->AudioBuffers[buffer_index++], st->Format, ChunkBuffer, bytes_read, st->Frequency);
            if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                return INVALID_AUDIO_HANDLE;
            }
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

    st->Volume = volume;
    alSourcef(st->Source, AL_GAIN, ((SoundVolume * st->Volume) / VOLUME_SCALE) / float(VOLUME_SCALE));
    if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
        DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
        return INVALID_AUDIO_HANDLE;
    }

    if (!Start_Primary_Sound_Buffer()) {
        DEBUG_WARNING("OpenAL: Unable to start primary sound buffer!\n");
        return INVALID_AUDIO_HANDLE;
    }

    /**
     *  Start playing the Source.
     */
    alSourcePlay(st->Source);
    if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
        DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
        return INVALID_AUDIO_HANDLE;
    }

    /**
     *  Tracker is primed and ready to go!
     */
    st->IsActive = true;

    return handle;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: OmniBlade, CCHyper
 */
void OpenALAudioClass::Sound_Maintenance_Callback()
{
    ALCenum alerror;

    if (AudioDone) {
        return;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {

        SampleTrackerStruct *st = SampleTracker[index];

        /**
         *  Skip inactive trackers.
         */
        if (!st || !st->IsActive) {
            continue;
        }

        // TODO: OpenAL handles these buffers?
        if (st->IsLooping || st->IsPaused) {
            continue;
        }

        /**
         *  Has this sample been flagged for buffer refill?
         */
        if (st->IsNeedService) {

            if (!st->IsMoreSource) {

                /**
                 *  Get Source state.
                 */
                ALint source_status = 0;
                alGetSourcei(st->Source, AL_SOURCE_STATE, &source_status);
                if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                    DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                    return;
                }

                if (source_status != AL_PLAYING && source_status != AL_PAUSED) {
                    st->IsNeedService = false;
                    Stop_Sample(index);
                }

            } else {

                /**
                 *  Request the number of OpenAL Buffers that have been processed (played) on the Source.
                 */
                ALint processed_buffers;
                alGetSourcei(st->Source, AL_BUFFERS_PROCESSED, &processed_buffers);
                if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                    DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                    return;
                }

                /**
                 *  For each processed buffer, remove it from the Source Queue, read
                 *  next chunk of audio data from disk, fill buffer with new data, and
                 *  add it to the Source Queue.
                 */
                int bytes_copied = 0;
                while (processed_buffers > 0) {

                    switch (st->TrackerType) {
                        case TRACKER_AUD:
                        {
                            SampleTrackerStruct *st = (SampleTrackerStruct *)st;
                            bytes_copied = Aud_Sample_Copy(st,
                                                           &st->SourcePtr,
                                                           &st->Remainder,
                                                           (void **)&st->QueueBuffer,
                                                           &st->QueueSize,
                                                           ChunkBuffer,
                                                           BUFFER_CHUNK_SIZE,
                                                           st->Compression);
                            break;
                        }
                        case TRACKER_OGG:
                        {
                            OggSampleTrackerStruct *ogg_st = (OggSampleTrackerStruct *)st;
                            bytes_copied = ogg_vorbis_decode(ogg_st->OggVorbisFile,
                                                             (char *)ChunkBuffer, 
                                                             BUFFER_CHUNK_SIZE,
                                                             ogg_st->OggVorbisInfo->channels);
                            break;
                        }
                        default:
                            break;
                    };

                    if (bytes_copied > 0) {

                        /**
                         *  Remove the Buffer from the Queue.
                         *  ('buffer' contains the Buffer ID for the unqueued Buffer).
                         */
                        ALuint buffer = 0;
                        alSourceUnqueueBuffers(st->Source, 1, &buffer);
                        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                            return;
                        }

                        /**
                         *  Copy audio data to Buffer.
                         */
                        alBufferData(buffer, st->Format, ChunkBuffer, bytes_copied, st->Frequency);
                        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                            return;
                        }

                        /**
                         *  Queue Buffer on the Source.
                         */
                        alSourceQueueBuffers(st->Source, 1, &buffer);
                        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
                            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
                            return;
                        }

                        --processed_buffers;
                    }

                    if (bytes_copied != BUFFER_CHUNK_SIZE && st->FilePending == 0) {
                        //st->IsMoreSource = false;
                    }

                    if (bytes_copied != BUFFER_CHUNK_SIZE) {
                        break;
                    }
                }
            }
        }

        if (!st->QueueBuffer && st->FilePending != 0) {
            st->QueueBuffer = &st->FileBuffer[StreamBufferSize * (st->Odd % STREAM_BUFFER_COUNT)];
            --st->FilePending;
            ++st->Odd;

            if (st->FilePending != 0) {
                st->QueueSize = StreamBufferSize;
            } else {
                st->QueueSize = st->FilePendingSize;
            }
        }

        if (!st->IsActive) {
            continue;
        }

        /**
         *  Adjust the volume for any samples that are fading out.
         */
        if (st->Reducer && st->Volume) {
            if (st->Reducer >= st->Volume) {
                st->Volume = VOLUME_MIN;
            } else {
                st->Volume -= st->Reducer;
            }

            if (!st->IsScore) {
                alSourcef(st->Source, AL_GAIN, ((SoundVolume * st->Volume) / VOLUME_SCALE) / float(VOLUME_SCALE));
            } else {
                alSourcef(st->Source, AL_GAIN, ((ScoreVolume * st->Volume) / VOLUME_SCALE) / float(VOLUME_SCALE));
            }
        }
    }
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioClass::Stream_Sample_Vol(void *buffer, int size, bool(*callbackptr)(short, short*, void**, int*), int volume, int handle)
{
    if (!Device || AudioDone) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!buffer || size <= 0) {
        return INVALID_AUDIO_HANDLE;
    }

    AUDHeaderType *header = (AUDHeaderType *)buffer;
    int oldsize = header->Size;
    header->Size -= sizeof(AUDHeaderType);

    int playid = Play_Sample_Handle(buffer, PRIORITY_MAX, volume, handle);

    header->Size = oldsize;

    if (playid == INVALID_AUDIO_HANDLE) {
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerStruct *st = SampleTracker[playid];

    if (!st) {
        return INVALID_AUDIO_HANDLE;
    }

    st->Odd = 0;
    st->Callback = callbackptr;

    return playid;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::File_Stream_Preload(int handle)
{
    SampleTrackerStruct *st = SampleTracker[handle];

    if (!st) {
        return;
    }

    int max_reads = st->FilePending + 2;
    if (!st->IsLoading || max_reads >= 12) {
        max_reads = (STREAM_BUFFER_COUNT/2) + 4;
    }

    if (!st->FileBuffer) {
        DEBUG_INFO("OpenAL: FileBuffer is null!\n");
        return;
    }

    int pending = 0;

    for (pending = st->FilePending; pending < max_reads; ++pending) {
        int data_read = st->FileHandle->Read(&st->FileBuffer[pending * StreamBufferSize], StreamBufferSize);
    
        if (data_read > 0) {
            st->FilePendingSize = data_read;
            ++st->FilePending;
        }
    
        if (data_read < StreamBufferSize) {
            break;
        }
    }

    //Sound_Maintenance_Callback();

    /**
     *  If we are on the last chunk of file or we have filled up our buffer.
     */
    if (StreamBufferSize > st->FilePendingSize || pending == max_reads) {

        int stream_size = st->FilePending == 1 ? st->FilePendingSize : StreamBufferSize;

        Stream_Sample_Vol(st->FileBuffer, stream_size, File_Callback, st->Volume, handle);

        --st->FilePending;
        st->IsLoading = false;

        if (st->FilePending) {

            st->Odd = 2;
            --st->FilePending;

            if (st->FilePendingSize != StreamBufferSize) {
                st->FileHandle->Close();
                if (st->FileHandle != nullptr) {
                    delete st->FileHandle;
                    st->FileHandle = nullptr;
                }
            }

            st->QueueBuffer = st->FileBuffer + StreamBufferSize;
            st->QueueSize = st->FilePending > 0 ? StreamBufferSize : st->FilePendingSize;

        } else {

            st->Odd = 0;
            st->QueueBuffer = 0;
            st->QueueSize = 0;
            st->FilePendingSize = 0;
            st->Callback = nullptr;

            st->FileHandle->Close();

            if (st->FileHandle != nullptr) {
                delete st->FileHandle;
                st->FileHandle = nullptr;
            }

        } 
    }
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: OmniBlade, CCHyper
 */
int OpenALAudioClass::File_Stream_Sample_Vol(const char *filename, int volume, bool real_time_start)
{
    DEBUG_INFO("OpenAL: File_Stream_Sample_Vol with \"%s\".\n", filename);

    if (!Device || AudioDone) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!filename) {
        return INVALID_AUDIO_HANDLE;
    }

    int handle = Get_Free_Sample_Handle(PRIORITY_MAX);
    if (handle == INVALID_AUDIO_HANDLE) {
        DEBUG_ERROR("OpenAL: Unable to find free handle for \"%s\"!\n", filename);
        return INVALID_AUDIO_HANDLE;
    }

    FileClass *fh = Get_File_Handle(filename);
    if (!fh) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!fh->Is_Available()) {
        DEBUG_ERROR("OpenAL: File \"%s\" is not available!\n", fh->File_Name());
        return INVALID_AUDIO_HANDLE;
    }

    if (!fh->Open(FILE_ACCESS_READ)) {
        DEBUG_ERROR("OpenAL: Unable to open file \"%s\"!\n", fh->File_Name());
        return INVALID_AUDIO_HANDLE;
    }

    unsigned char *stream_ptr = nullptr;
    if (IsUsingFileStreamBuffer) {
        stream_ptr = new unsigned char [STREAM_BUFFER_COUNT * StreamBufferSize];
    } else {
        stream_ptr = FileStreamBuffer;
        IsUsingFileStreamBuffer = true;
    }
        
    if (!stream_ptr) {
        delete fh;
        DEBUG_ERROR("OpenAL: Unable to obtain streaming buffer!\n");
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerStruct *st = SampleTracker[handle];

    if (!st) {
        return INVALID_AUDIO_HANDLE;
    }

    DEV_DEBUG_INFO("OpenAL: Free sample tracker '%d' chosen for score \"%s\".\n", handle, filename);

    st->FilePending = 0;
    st->FilePendingSize = 0;
    st->IsScore = true;
    st->IsLoading = real_time_start;
    st->Volume = volume;
    st->FileHandle = fh;
    st->FileBuffer = stream_ptr;

    File_Stream_Preload(handle);

    return handle;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: OmniBlade, CCHyper
 */
void OpenALAudioClass::Sound_Callback()
{
    if (!Device || AudioDone) {
        return;
    }

    Sound_Maintenance_Callback();

    /**
     *  Process all trackers.
     */
    for (int index = 0; index < SampleTracker.Count(); ++index) {

        SampleTrackerStruct *st = SampleTracker[index];

        if (!st) {
            continue;
        }

        /**
         *  Is this tracker pending a load?
         */
        if (st->IsLoading) {
            File_Stream_Preload(index);

            /**
             *  We are done, next tracker...
             */
            continue;
        }

        /**
         *  Is this sample inactive? Perform a cleanup.
         */
        if (!st->IsActive) {
            st->Close_File();

            /**
             *  We are done, next tracker...
             */
            continue;
        }
            
        /**
         *  Has this sample volume been reduced to 0?
         */
        if (st->Reducer && !st->Volume) {

            /**
             *  If so, stop it.
             */
            Stop_Sample(index);

            /**
             *  We are done, next tracker...
             */
            continue;
        }

        /**
         *  Process any pending files.
         */
        if (!st->QueueBuffer || st->FileHandle && st->FilePending < (STREAM_BUFFER_COUNT-3)) {
            if (st->Callback) {
                if (!st->Callback(index, &st->Odd, (void **)&st->QueueBuffer, &st->QueueSize)) {

                    /**
                     *  No files pending, so the file callback is not required anymore.
                     */
                    st->Callback = nullptr;
                }
            }

            /**
             *  We are done, next tracker...
             */
            continue;
        }

    }
}
#endif


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Set_Volume_All(int volume)
{
    ALCenum alerror;

    Set_Sound_Vol(volume);

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];

        if (!st) {
            continue;
        }

        if (!Sample_Status(index)) {
            continue;
        }

        st->Volume = volume;
        alSourcef(st->Source, AL_GAIN, ((SoundVolume * st->Volume) / VOLUME_SCALE) / float(VOLUME_SCALE));
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return;
        }
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioClass::Set_Volume_Percent_All(int vol_percent)
{
    ALCenum alerror;

    int newvol = vol_percent * SoundVolume / 100;
    int oldvol = Set_Sound_Vol(newvol);

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];

        if (!st) {
            continue;
        }

        if (!Sample_Status(index)) {
            continue;
        }

        st->Volume = newvol;
        alSourcef(st->Source, AL_GAIN, ((SoundVolume * st->Volume) / VOLUME_SCALE) / float(VOLUME_SCALE));
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            break;
        }
    }

    return oldvol;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Set_Handle_Volume(int handle, int volume)
{
    ALCenum alerror;

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];

    if (!st) {
        return;
    }

    if (st->IsActive) {
        st->Volume = volume;
        alSourcef(st->Source, AL_GAIN, ((SoundVolume * st->Volume) / VOLUME_SCALE) / float(VOLUME_SCALE));
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return;
        }
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Set_Sample_Volume(const void *sample, int volume)
{
    ALCenum alerror;

    if (!sample) {
        return;
    }

    int handle = Get_Playing_Sample_Handle(sample);
    if (handle == INVALID_AUDIO_HANDLE) {
        return;
    }

    Set_Handle_Volume(handle, volume);
}


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Fade_Sample(int handle, int ticks)
{
    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];

    if (!st) {
        return;
    }

    if (ticks > 0 && !st->IsLoading) {
        st->Reducer = (st->Volume / (ticks + 1));
    } else {
        Stop_Sample(handle);
    }
}
#endif


/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioClass::Get_Playing_Sample_Handle(const void *sample)
{
    if (AudioDone) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!sample) {
        return INVALID_AUDIO_HANDLE;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];

        if (!st) {
            continue;
        }

        if (st->Original == sample) {
            if (Sample_Status(index)) {
                return index;
            }
        }
    }

    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Set_Handle_Loop(int handle, bool loop)
{
    ALCenum alerror;

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];

    if (!st) {
        return;
    }

    if (st->IsActive) {
        alSourcef(st->Source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return;
        }
        st->IsLooping = loop;
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Set_Sample_Loop(const void *sample, bool loop)
{
    ALCenum alerror;

    if (!sample) {
        return;
    }

    int handle = Get_Playing_Sample_Handle(sample);
    if (handle == INVALID_AUDIO_HANDLE) {
        return;
    }

    Set_Handle_Loop(handle, loop);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Stop_Handle_Looping(int handle)
{
    // TODO
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Stop_Sample_Looping(const void* sample)
{
    // TODO
}


void OpenALAudioClass::Set_Score_Handle_Pause(int handle)
{
    ALCenum alerror;

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];

    if (!st) {
        return;
    }

#ifndef NDEBUG
    if (!st->IsScore) {
        DEV_DEBUG_WARNING("OpenAL: Sample '%s' is not a score tracker!\n", handle);
        return;
    }
#endif

    if (st->IsActive) {
        alSourcePause(st->Source);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return;
        }
        st->IsPaused = true;
    }
}


void OpenALAudioClass::Set_Score_Handle_Resume(int handle)
{
    ALCenum alerror;

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];

    if (!st) {
        return;
    }

#ifndef NDEBUG
    if (!st->IsScore) {
        DEV_DEBUG_WARNING("OpenAL: Sample '%s' is not a score tracker!\n", handle);
        return;
    }
#endif

    if (st->IsActive) {
        alSourcePlay(st->Source);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return;
        }
        st->IsPaused = false;
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Set_Handle_Pitch(int handle, float pitch)
{
    ALCenum alerror;

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];

    if (!st) {
        return;
    }

    if (st->IsActive) {
        alSourcef(st->Source, AL_PITCH, pitch);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return;
        }
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Set_Sample_Pitch(const void *sample, float pitch)
{
    ALCenum alerror;

    if (!sample) {
        return;
    }

    int handle = Get_Playing_Sample_Handle(sample);
    if (handle == INVALID_AUDIO_HANDLE) {
        return;
    }

    Set_Handle_Pitch(handle, pitch);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Set_Handle_Pan(int handle, float pan)
{
    ALCenum alerror;

    if (!Sample_Status(handle)) {
        return;
    }

    SampleTrackerStruct *st = SampleTracker[handle];

    if (!st) {
        return;
    }

    if (st->IsActive) {

        // TODO

        float left_gain = pan;
        float right_gain = pan;

        /**
         *  https://github.com/kcat/openal-soft/issues/194
         */
        ALfloat newpan;
        newpan = (WWMath::Acosf(left_gain) + WWMath::Asinf(right_gain)) / ((float)WWMATH_PI); // average angle in [0,1]
        newpan = 2 * newpan - 1; // convert to [-1, 1]
        newpan = newpan * 0.5f;  // 0.5 = sin(30') for a +/- 30 degree arc

        alSourcei(st->Source, AL_SOURCE_RELATIVE, AL_TRUE);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return;
        }

        alSource3f(st->Source, AL_POSITION, newpan, 0.0f, -WWMath::Sqrtf(1.0f - newpan*newpan));
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return;
        }
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OpenALAudioClass::Set_Sample_Pan(const void *sample, float pan)
{
    ALCenum alerror;

    if (!sample) {
        return;
    }

    int handle = Get_Playing_Sample_Handle(sample);
    if (handle == INVALID_AUDIO_HANDLE) {
        return;
    }

    Set_Handle_Pan(handle, pan);
}


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioClass::Set_Sound_Vol(int volume)
{
    ALCenum alerror;

    int oldvol = SoundVolume;
    SoundVolume = volume;

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];

        if (!st || !st->IsActive) {
            continue;
        }

        if (st->IsScore) {
            continue;
        }

        alSourcef(st->Source, AL_GAIN, ((ScoreVolume * st->Volume) / VOLUME_SCALE) / float(VOLUME_SCALE));
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            break;
        }
    }

    return oldvol;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
int OpenALAudioClass::Set_Score_Vol(int volume)
{
    ALCenum alerror;

    int oldvol = ScoreVolume;
    ScoreVolume = volume;

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerStruct *st = SampleTracker[index];

        if (!st || !st->IsActive) {
            continue;
        }

        if (!st->IsScore) {
            continue;
        }

        alSourcef(st->Source, AL_GAIN, ((ScoreVolume * st->Volume) / VOLUME_SCALE) / float(VOLUME_SCALE));
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            break;
        }
    }

    return oldvol;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: OmniBlade
 */
long OpenALAudioClass::Sample_Length(const void *sample)
{
    if (!sample) {
        return 0;
    }

    AUDHeaderType aud_hdr;
    CopyMemory(&aud_hdr, sample, sizeof(aud_hdr));
    unsigned time = aud_hdr.UncompSize;

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
};
#endif


#if 0
/**
 *  x
 * 
 *  @author: OmniBlade, CCHyper
 */
bool OpenALAudioClass::Start_Primary_Sound_Buffer(bool forced)
{
    ALCenum alerror;

    if (!Device || AudioDone || !GameInFocus) {
        return false;
    }

    alcProcessContext(Context);
    if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
        DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
        return false;
    }
    
    return true;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: OmniBlade, CCHyper
 */
void OpenALAudioClass::Stop_Primary_Sound_Buffer()
{
    ALCenum alerror;

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        Stop_Sample(index);
    }

    if (Context) {
        alcSuspendContext(Context);
        if ((alerror = alcGetError(Device)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: %s!", Get_OpenAL_Error(alerror));
            return;
        }
    }
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper, tomsons26
 */
int OpenALAudioClass::Aud_Sample_Copy(SampleTrackerStruct *st, void **source, int *src_size, void **alternate, int *alt_size, void *dest, int dst_size, SCompressType scomp)
{
    ASSERT(st != nullptr);

    int datasize = 0;

    if (st->TrackerType != TRACKER_AUD) {
        return 0;
    }

    SampleTrackerStruct *st = (SampleTrackerStruct *)st;

    /**
     *  There is no compression or it doesn't match any of the supported
     *  compressions so we just copy the data over.
     */
    if (scomp == SCOMP_NONE || (scomp != SCOMP_WESTWOOD && scomp != SCOMP_SOS)) {
        datasize = Aud_Simple_Copy(source, src_size, alternate, alt_size, &dest, dst_size);
        return datasize;
    }

    while (dst_size > 0) {

        unsigned short frame_size = 0;
        unsigned short dcomp_size = 0;
        unsigned magic_number = 0;

        void *frame_size_ptr = &frame_size;
        void *dcomp_size_ptr = &dcomp_size;
        void *magic_number_ptr = &magic_number;

        if (Aud_Simple_Copy(source, src_size, alternate, alt_size, &frame_size_ptr, sizeof(frame_size)) < sizeof(frame_size)) {
            break;
        }

        if (Aud_Simple_Copy(source, src_size, alternate, alt_size, &dcomp_size_ptr, sizeof(dcomp_size)) < sizeof(dcomp_size) || dcomp_size > dst_size) {
            break;
        }

        if (Aud_Simple_Copy(source, src_size, alternate, alt_size, &magic_number_ptr, sizeof(magic_number)) < sizeof(magic_number) || magic_number != AUD_CHUNK_MAGIC_ID) {
            break;
        }

        if (frame_size == dcomp_size) {
            if (Aud_Simple_Copy(source, src_size, alternate, alt_size, &dest, frame_size) < dcomp_size) {
                break;
            }

        } else {

            void *uptr = UncompBuffer;
            if (Aud_Simple_Copy(source, src_size, alternate, alt_size, &uptr, frame_size) < frame_size) {
                break;
            }

            ASSERT(UncompBuffer != nullptr);
            ASSERT(dest != nullptr);

            switch (scomp) {

                case SCOMP_SOS:
                {
                    _tagCOMPRESS_INFO2 *sosinfo = &st->sSOSInfo;

                    sosinfo->lpSource = (BYTE *)UncompBuffer;
                    sosinfo->lpDest = (BYTE *)dest;

                    sosCODEC2DecompressData(sosinfo, dcomp_size);

                    break;
                }

                case SCOMP_WESTWOOD:
                    DEBUG_WARNING("OpenAL: Westwood sliding is unsupported!\n");
                    break;

                default:
                    DEBUG_WARNING("OpenAL: Unsupported compression format!\n");
                    break;

            };

            dest = static_cast<unsigned char *>(dest) + dcomp_size;
        }

        datasize += dcomp_size;
        dst_size -= dcomp_size;
    }

    return datasize;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper, tomsons26
 */
int OpenALAudioClass::Aud_Simple_Copy(void **source, int *src_size, void **alternate, int *alt_size, void **dest, int dst_size)
{
    int out = 0;

    if (*src_size == 0) {
        *source = *alternate;
        *src_size = *alt_size;
        *alternate = nullptr;
        *alt_size = 0;
    }

    if (!*source || *src_size == 0) {
        return out;
    }

    int s = dst_size;

    if (*src_size < dst_size) {
        s = *src_size;
    }

    CopyMemory(*dest, *source, s);

    *source = static_cast<unsigned char *>(*source) + s;
    *src_size -= s;

    *dest = static_cast<unsigned char *>(*dest) + s;
    out = s;

    if ((dst_size - s) == 0) {
        return out;
    }

    *source = *alternate;
    *src_size = *alt_size;
    *alternate = nullptr;
    *alt_size = 0;

    out = Aud_Simple_Copy(source, src_size, alternate, alt_size, dest, (dst_size - s)) + s;

    return out;
}
#endif


/**
 *  Returns a handle to the requested file.
 * 
 *  @author: CCHyper
 */
FileClass *OpenALAudioClass::Get_File_Handle(const char *filename)
{
    /**
     *  Search for the file in the additional search paths.
     */
    if (Vinifera_MusicPath_EnvVar[0] != '\0') {
        Wstring fname = Wstring(Vinifera_MusicPath_EnvVar) + Wstring(filename);
        if (RawFileClass(fname.Peek_Buffer()).Is_Available()) {
            DEBUG_INFO("OpenAL: Get_File_Handle found file \"%s\" in Enviroment path.\n", filename);
            FileClass *fh = new RawFileClass(fname.Peek_Buffer());
            return fh;
        }
    }
    if (Vinifera_MusicPath[0] != '\0') {
        Wstring fname = Wstring(Vinifera_MusicPath) + Wstring(filename);
        if (RawFileClass(fname.Peek_Buffer()).Is_Available()) {
            DEBUG_INFO("OpenAL: Get_File_Handle found file \"%s\" in Music path.\n", filename);
            FileClass *fh = new RawFileClass(fname.Peek_Buffer());
            return fh;
        }
    }

    /**
     *  Search for the file in the mix files.
     */
    if (CCFileClass(filename).Is_Available()) {
        DEBUG_INFO("OpenAL: Get_File_Handle found file \"%s\" in a mix file.\n", filename);
        FileClass *fh = new CCFileClass(filename);
        return fh;
    }

    return nullptr;
}


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OpenALAudioClass::File_Callback(short handle, short *odd, void **buffer, int *size)
{
    if (handle == INVALID_AUDIO_HANDLE) {
        return false;
    }

    SampleTrackerStruct *st = OpenAL_This->SampleTracker[handle];

    if (!st || !st->IsActive) {
        return false;
    }

    if (!st->FileBuffer) {
        return false;
    }

    if (!*buffer && st->FilePending) {
        *buffer = &st->FileBuffer[OpenAL_This->StreamBufferSize * (*odd % STREAM_BUFFER_COUNT)];
        --st->FilePending;
        ++*odd;
        *size = st->FilePending == 0 ? st->FilePendingSize : OpenAL_This->StreamBufferSize;
    }

    //OpenAL_This->Sound_Maintenance_Callback();

    if (st->FilePending < (STREAM_BUFFER_COUNT/2) && st->FileHandle) {

        if ((STREAM_BUFFER_COUNT-2) != st->FilePending) {

            /**
             *  Fill empty buffers.
             */
            for (int num_empty_buffers = (STREAM_BUFFER_COUNT-2) - st->FilePending;
                 num_empty_buffers && st->FileHandle; --num_empty_buffers) {

                /**
                 *  Buffer to fill with data.
                 */
                void *tofill = &st->FileBuffer[OpenAL_This->StreamBufferSize * ((st->FilePending + *odd) % STREAM_BUFFER_COUNT)];

                int psize = st->FileHandle->Read(tofill, OpenAL_This->StreamBufferSize);

                if (psize != OpenAL_This->StreamBufferSize) {
                    st->Close_File();
                }

                if (psize > 0) {
                    st->FilePendingSize = psize;
                    ++st->FilePending;
                    //OpenAL_This->Sound_Maintenance_Callback();
                }
            }
        }

        if (!st->QueueBuffer && st->FilePending) {
            st->QueueBuffer = &st->FileBuffer[OpenAL_This->StreamBufferSize * (st->Odd % STREAM_BUFFER_COUNT)];
            --st->FilePending;
            ++st->Odd;
            st->QueueSize = st->FilePending > 0 ? OpenAL_This->StreamBufferSize : st->FilePendingSize;
        }

        //OpenAL_This->Sound_Maintenance_Callback();
    }

    if (st->FilePending) {
        return true;
    }

    return false;
}
#endif























//
//
// VANILLA CONQUER CODE
//
//


int OpenALAudioClass::Aud_Simple_Copy(void** source, int* ssize, void** alternate, int* altsize, void** dest, int size)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    int out = 0;

    if (*ssize == 0) {
        *source = *alternate;
        *ssize = *altsize;
        *alternate = nullptr;
        *altsize = 0;
    }

    if (*source == nullptr || *ssize == 0) {
        return out;
    }

    int s = size;

    if (*ssize < size) {
        s = *ssize;
    }

    memcpy(*dest, *source, s);
    *source = static_cast<char*>(*source) + s;
    *ssize -= s;
    *dest = static_cast<char*>(*dest) + s;
    out = s;

    if ((size - s) == 0) {
        return out;
    }

    *source = *alternate;
    *ssize = *altsize;
    *alternate = nullptr;
    *altsize = 0;

    out = Aud_Simple_Copy(source, ssize, alternate, altsize, dest, (size - s)) + s;

    return out;
}

int OpenALAudioClass::Aud_Sample_Copy(SampleTrackerStruct* st,
                void** source,
                int* ssize,
                void** alternate,
                int* altsize,
                void* dest,
                int size,
                SCompressType scomp)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    int datasize = 0;

    // There is no compression or it doesn't match any of the supported compressions so we just copy the data over.
    if (scomp == SCOMP_NONE || (scomp != SCOMP_WESTWOOD && scomp != SCOMP_SOS)) {
        return Aud_Simple_Copy(source, ssize, alternate, altsize, &dest, size);
    }

    _tagCOMPRESS_INFO2 *sosinfo = &st->sSOSInfo;

    while (size > 0) {
        uint16_t fsize;
        uint16_t dsize;
        unsigned magic;

        void* fptr = &fsize;
        void* dptr = &dsize;
        void* mptr = &magic;

        // Verify and seek over the chunk header.
        if (Aud_Simple_Copy(source, ssize, alternate, altsize, &fptr, sizeof(fsize)) < sizeof(fsize)) {
            break;
        }

        if (Aud_Simple_Copy(source, ssize, alternate, altsize, &dptr, sizeof(dsize)) < sizeof(dsize) || dsize > size) {
            break;
        }

        if (Aud_Simple_Copy(source, ssize, alternate, altsize, &mptr, sizeof(magic)) < sizeof(magic)
            || magic != AUD_CHUNK_MAGIC_ID) {
            break;
        }

        if (fsize == dsize) {
            // File size matches size to decompress, so there's nothing to do other than copy the buffer over.
            if (Aud_Simple_Copy(source, ssize, alternate, altsize, &dest, fsize) < dsize) {
                return datasize;
            }
        } else {
            // Else we need to decompress it.
            void* uptr = UncompBuffer;

            if (Aud_Simple_Copy(source, ssize, alternate, altsize, &uptr, fsize) < fsize) {
                return datasize;
            }

            if (scomp == SCOMP_WESTWOOD) {
                //Aud_Audio_Unzap(UncompBuffer, dest, dsize);
            } else {
                sosinfo->lpSource = (unsigned char *)UncompBuffer;
                sosinfo->lpDest = (unsigned char*)dest;

                sosCODEC2DecompressData(sosinfo, dsize);
            }

            dest = reinterpret_cast<char*>(dest) + dsize;
        }

        datasize += dsize;
        size -= dsize;
    }

    return datasize;
}

#if 0
int OpenALAudioClass::File_Stream_Sample(const char* filename, bool real_time_start)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    return File_Stream_Sample_Vol(filename, VOLUME_MAX, real_time_start);
}
#endif

int OpenALAudioClass::Stream_Sample_Vol(void* buffer, int size, bool (*callback)(short, short*, void**, int*), int volume, int handle)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (AudioDone || buffer == nullptr || size == 0) {
        return INVALID_AUDIO_HANDLE;
    }

    AUDHeaderType header;
    memcpy(&header, buffer, sizeof(header));
    int oldsize = header.Size;
    header.Size = size - sizeof(header);
    memcpy(buffer, &header, sizeof(header));
    int playid = Play_Sample_Handle(buffer, PRIORITY_MAX, volume, handle);
    header.Size = oldsize;
    memcpy(buffer, &header, sizeof(header));

    if (playid == INVALID_AUDIO_HANDLE) {
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerStruct* st = SampleTracker[playid];
    st->Callback = callback;
    st->Odd = 0;

    return playid;
}

bool OpenALAudioClass::File_Callback(short id, short* odd, void** buffer, int* size)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (id == INVALID_AUDIO_HANDLE) {
        return false;
    }

    SampleTrackerStruct* st = OpenAL_This->SampleTracker[id];

    if (st->FileBuffer == nullptr) {
        return false;
    }

    if (*buffer == nullptr && st->FilePending) {
        *buffer =
            static_cast<unsigned char*>(st->FileBuffer) + OpenAL_This->StreamBufferSize * (*odd % STREAM_BUFFER_COUNT);
        --st->FilePending;
        ++*odd;
        *size = st->FilePending == 0 ? st->FilePendingSize : OpenAL_This->StreamBufferSize;
    }

    //OpenAL_This->Sound_Maintenance_Callback();

    int count = /* OpenAL_This->StreamLowImpact ? STREAM_BUFFER_COUNT / 2 : */ STREAM_BUFFER_COUNT - 3;

    if (count > st->FilePending && st->FileHandle != nullptr) {
        if (STREAM_BUFFER_COUNT - 2 != st->FilePending) {
            // Fill empty buffers.
            for (int num_empty_buffers = STREAM_BUFFER_COUNT - 2 - st->FilePending;
                 num_empty_buffers && st->FileHandle != nullptr;
                 --num_empty_buffers) {
                // Buffer to fill with data.
                void* tofill =
                    static_cast<unsigned char*>(st->FileBuffer)
                    + OpenAL_This->StreamBufferSize * ((st->FilePending + *odd) % STREAM_BUFFER_COUNT);

                int psize = st->FileHandle->Read(tofill, OpenAL_This->StreamBufferSize);

                if (psize != OpenAL_This->StreamBufferSize) {
                    st->Close_File();
                }

                if (psize > 0) {
                    st->FilePendingSize = psize;
                    ++st->FilePending;
                    //OpenAL_This->Sound_Maintenance_Callback();
                }
            }
        }

        if (st->QueueBuffer == nullptr && st->FilePending) {
            st->QueueBuffer = static_cast<unsigned char*>(st->FileBuffer)
                              + OpenAL_This->StreamBufferSize * (st->Odd % STREAM_BUFFER_COUNT);
            --st->FilePending;
            ++st->Odd;
            st->QueueSize = st->FilePending > 0 ? OpenAL_This->StreamBufferSize : st->FilePendingSize;
        }

        //OpenAL_This->Sound_Maintenance_Callback();
    }

    if (st->FilePending) {
        return true;
    }

    return false;
}

void OpenALAudioClass::File_Stream_Preload(int index)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    SampleTrackerStruct* st = SampleTracker[index];
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
        Stream_Sample_Vol(st->FileBuffer, stream_size, File_Callback, st->Volume, index);

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
}

int OpenALAudioClass::File_Stream_Sample_Vol(char const* filename, int volume, bool real_time_start)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (filename == nullptr || !CCFileClass(filename).Is_Available()) {
        return INVALID_AUDIO_HANDLE;
    }

    if (FileStreamBuffer == nullptr) {
        FileStreamBuffer = (unsigned char *)malloc((unsigned long)(StreamBufferSize * STREAM_BUFFER_COUNT));

        for (int i = 0; i < SampleTracker.Count(); ++i) {
            SampleTracker[i]->FileBuffer = FileStreamBuffer;
        }
    }

    if (FileStreamBuffer == nullptr) {
        return INVALID_AUDIO_HANDLE;
    }

    FileClass *fh = new CCFileClass(filename);

    if (fh == nullptr) {
        return INVALID_AUDIO_HANDLE;
    }

    int handle = Get_Free_Sample_Handle(PRIORITY_MAX);

    if (handle < SampleTracker.Count()) {
        SampleTrackerStruct* st = SampleTracker[handle];
        st->IsScore = true;
        st->FilePending = 0;
        st->FilePendingSize = 0;
        st->IsLoading = real_time_start;
        st->Volume = volume;
        st->FileHandle = fh;
        File_Stream_Preload(handle);
        return handle;
    }

    return INVALID_AUDIO_HANDLE;
};

void OpenALAudioClass::Sound_Callback()
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    Sound_Maintenance_Callback();

    if (!AudioDone) {
        //Sound_Maintenance_Callback();

        for (int i = 0; i < SampleTracker.Count(); ++i) {
            SampleTrackerStruct* st = SampleTracker[i];

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
            if (st->Reducer && !st->Volume) {
                // If so stop it.
                Stop_Sample(i);

                // We are done with this sample.
                continue;
            }

            // Process pending files.
            if (st->QueueBuffer == nullptr
                || st->FileHandle != nullptr && STREAM_BUFFER_COUNT - 3 > st->FilePending) {
                if (st->Callback != nullptr) {
                    if (!st->Callback(i, &st->Odd, (void **)&st->QueueBuffer, &st->QueueSize)) {
                        // No files are pending so pending file callback not needed anymore.
                        st->Callback = nullptr;
                    }
                }

                // We are done with this sample.
                continue;
            }
        }
    }
};

void OpenALAudioClass::Sound_Maintenance_Callback()
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (AudioDone) {
        return;
    }

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerStruct* st = (SampleTrackerStruct *)SampleTracker[i];
        if (st->IsActive) { // If this tracker needs processing and isn't already marked as being processed, then process it.
            if (st->IsNeedService) {
                // Do we have more data in this tracker to play?
                if (st->IsMoreSource) {
                    ALint processed_buffers;

                    // Work out if we have any space to buffer more data right now.
                    alGetSourcei(st->Source, AL_BUFFERS_PROCESSED, &processed_buffers);

                    while (processed_buffers > 0 && st->IsMoreSource) {
                        int bytes_copied = Aud_Sample_Copy(st,
                                                       (void **)&st->SourcePtr,
                                                       &st->Remainder,
                                                       (void **)&st->QueueBuffer,
                                                       &st->QueueSize,
                                                       ChunkBuffer,
                                                       BUFFER_CHUNK_SIZE,
                                                       st->Compression);

                        if (bytes_copied != BUFFER_CHUNK_SIZE) {
                            st->IsMoreSource = false;
                        }

                        if (bytes_copied > 0) {
                            ALuint buffer;
                            alSourceUnqueueBuffers(st->Source, 1, &buffer);
                            alBufferData(buffer, st->Format, ChunkBuffer, bytes_copied, st->Frequency);
                            alSourceQueueBuffers(st->Source, 1, &buffer);
                            --processed_buffers;
                        }
                    }
                } else {
                    ALint source_status;
                    alGetSourcei(st->Source, AL_SOURCE_STATE, &source_status);

                    if (source_status != AL_PLAYING) {
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
        if (st->IsActive && st->Reducer > 0 && st->Volume > 0) {
            if (st->Reducer >= st->Volume) {
                st->Volume = VOLUME_MIN;
            } else {
                st->Volume -= st->Reducer;
            }

            if (!st->IsScore) {
                alSourcef(st->Source, AL_GAIN, ((SoundVolume * st->Volume) / 256) / 256.0f);
            } else {
                alSourcef(st->Source, AL_GAIN, ((ScoreVolume * st->Volume) / 256) / 256.0f);
            }
        }
    }
};

#if 0
void* OpenALAudioClass::Load_Sample(char const* filename)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (filename == nullptr || !Find_File(filename)) {
        return nullptr;
    }

    void* data = nullptr;
    int handle = Open_File(filename, 1);

    if (handle != INVALID_FILE_HANDLE) {
        int data_size = File_Size(handle) + sizeof(AUDHeaderType);
        data = malloc(data_size);

        if (data != nullptr) {
            //Sample_Read(handle, data, data_size);

            AUDHeaderType header;
            int actual_bytes_read = Read_File(fh, &header, sizeof(AUDHeaderType));
            int to_read = std::min<unsigned>(size - sizeof(AUDHeaderType), header.Size);

            actual_bytes_read += Read_File(fh, static_cast<char*>(buffer) + sizeof(AUDHeaderType), to_read);

            memcpy(buffer, &header, sizeof(AUDHeaderType));
        }

        Close_File(handle);
        Misc = data_size;
    }

    return data;
};
#endif

#if 0
long OpenALAudioClass::Load_Sample_Into_Buffer(char const* filename, void* buffer, long size)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (buffer == nullptr || size == 0 || DigiHandle == INVALID_AUDIO_HANDLE || !filename
        || !Find_File(filename)) {
        return 0;
    }

    int handle = Open_File(filename, 1);

    if (handle == INVALID_FILE_HANDLE) {
        return 0;
    }

    int sample_size = Sample_Read(handle, buffer, size);
    Close_File(handle);
    return sample_size;
}
#endif

#if 0
long OpenALAudioClass::Sample_Read(int fh, void* buffer, long size)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (buffer == nullptr || fh == INVALID_AUDIO_HANDLE || size <= sizeof(AUDHeaderType)) {
        return 0;
    }

    AUDHeaderType header;
    int actual_bytes_read = Read_File(fh, &header, sizeof(AUDHeaderType));
    int to_read = std::min<unsigned>(size - sizeof(AUDHeaderType), header.Size);

    actual_bytes_read += Read_File(fh, static_cast<char*>(buffer) + sizeof(AUDHeaderType), to_read);

    memcpy(buffer, &header, sizeof(AUDHeaderType));

    return actual_bytes_read;
};
#endif

#if 0
bool OpenALAudioClass::Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    ALCenum error;
    ALCdevice* device = alcOpenDevice(nullptr);

    if (device == nullptr) {

        //CCDebugString("Error occured getting OpenAL device.\n");

        return false;
    }

    Context = alcCreateContext(device, nullptr);
    if (Context == nullptr || !alcMakeContextCurrent(Context)) {
        //CCDebugString("OpenAL failed to make audio context current.\n");
        alcCloseDevice(device);
        Context = nullptr;
        return false;
    }

    UncompBuffer = malloc(UNCOMP_BUFFER_SIZE);

    if (UncompBuffer == nullptr) {
        //CCDebugString("Audio_Init - Failed to allocate UncompBuffer.");
        return false;
    }

    // Create placback buffers for all trackers.
    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerStruct* st = SampleTracker[i];

        // Gen buffers on audio start?
        // alGenBuffers(NUM_BUFFERS, st->AudioBuffers);

        if ((error = alcGetError(Device)) != AL_NO_ERROR) {
            //CCDebugString(Get_OpenAL_Error(error));
            return false;
        }

        alGenSources(1, &st->Source);

        if ((error = alcGetError(Device)) != AL_NO_ERROR) {
            //CCDebugString(Get_OpenAL_Error(error));
            return false;
        }

        st->Frequency = rate;
        st->Format = Get_OpenAL_Format(bits_per_sample, stereo ? 2 : 1);
    }

    AudioDone = false;

    return true;
};
#endif

#if 0
void OpenALAudioClass::End()
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (Context == nullptr) {
        for (int i = 0; i < SampleTracker.Count(); ++i) {
            Stop_Sample(i);
            alDeleteSources(1, &SampleTracker[i]->Source);
        }
    }

    if (FileStreamBuffer != nullptr) {
        free((void*)FileStreamBuffer);
        FileStreamBuffer = nullptr;
    }

    ALCdevice* device = alcGetContextsDevice(Context);

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(Context);
    alcCloseDevice(device);

    if (UncompBuffer != nullptr) {
        free((void*)UncompBuffer);
        UncompBuffer = nullptr;
    }

    AudioDone = true;
};
#endif

void OpenALAudioClass::Stop_Sample(int index)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (index < SampleTracker.Count() && !AudioDone) {
        SampleTrackerStruct* st = SampleTracker[index];

        if (st->IsActive || st->IsLoading) {
            st->IsActive = false;

            if (!st->IsScore) {
                st->Original = nullptr;
            }

            st->Priority = 0;

            if (!st->IsLoading) {
                ALint processed_count = -1;
                alSourceStop(st->Source);
                alGetSourcei(st->Source, AL_BUFFERS_PROCESSED, &processed_count);

                while (processed_count-- > 0) {
                    ALuint tmp;
                    alSourceUnqueueBuffers(st->Source, 1, &tmp);
                }

                alDeleteBuffers(NUM_BUFFERS, st->AudioBuffers);
            }

            st->IsLoading = false;

            if (st->FileHandle != nullptr) {
                st->Close_File();
            }

            st->QueueBuffer = nullptr;
        }
    }
};

bool OpenALAudioClass::Sample_Status(int index)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (index < 0) {
        return false;
    }

    if (AudioDone) {
        return false;
    }

    if (index >= SampleTracker.Count()) {
        return false;
    }

    SampleTrackerStruct* st = SampleTracker[index];

    if (st->IsLoading) {
        return true;
    }

    if (!st->IsActive) {
        return false;
    }

    ALint val;
    alGetSourcei(st->Source, AL_SOURCE_STATE, &val);

    return val == AL_PLAYING;
};

bool OpenALAudioClass::Is_Sample_Playing(const void* sample)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (AudioDone || sample == nullptr) {
        return false;
    }

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        if (sample == SampleTracker[i]->Original && Sample_Status(i)) {
            return true;
        }
    }

    return false;
};

void OpenALAudioClass::Stop_Sample_Playing(const void* sample)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (sample != nullptr) {
        for (int i = 0; i < SampleTracker.Count(); ++i) {
            if (SampleTracker[i]->Original == sample) {
                Stop_Sample(i);
                break;
            }
        }
    }
};

int OpenALAudioClass::Play_Sample(const void* sample, int priority, int volume)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    return Play_Sample_Handle(sample, priority, volume, Get_Free_Sample_Handle(priority));
};

int OpenALAudioClass::Play_Sample_Handle(const void* sample, int priority, int volume, int id)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (!AudioDone) {
        if (sample == nullptr) {
            return INVALID_AUDIO_HANDLE;
        }

        if (id == INVALID_AUDIO_HANDLE) {
            return INVALID_AUDIO_HANDLE;
        }

        SampleTrackerStruct* st = (SampleTrackerStruct *)SampleTracker[id];

        // Read in the sample's header.
        AUDHeaderType raw_header;
        memcpy(&raw_header, sample, sizeof(raw_header));

//        // We don't support anything lower than 20000 hz.
//        if (raw_header.Rate < 24000 && raw_header.Rate > 20000) {
//            raw_header.Rate = 22050;
//        }

        // Set up basic sample tracker info.
        st->Compression = SCompressType(raw_header.Compression);
        st->Original = sample;
        st->Odd = 0;
        st->Reducer = 0;
        //st->Restart = false;
        st->QueueBuffer = nullptr;
        st->QueueSize = 0;
        st->OriginalSize = raw_header.Size + sizeof(AUDHeaderType);
        st->Priority = priority;
        st->IsNeedService = 0;
        st->Remainder = raw_header.Size;
        st->SourcePtr = (unsigned char *)sample + sizeof(AUDHeaderType);

        // Compression is ADPCM so we need to init it's stream info.
        if (st->Compression == SCOMP_SOS) {
            st->sSOSInfo.wChannels = (raw_header.Flags & 1) + 1;
            st->sSOSInfo.wBitSize = raw_header.Flags & 2 ? 16 : 8;
            st->sSOSInfo.dwCompSize = raw_header.Size;
            st->sSOSInfo.dwUnCompSize = raw_header.Size * (st->sSOSInfo.wBitSize / 4);
            sosCODEC2InitStream(&st->sSOSInfo);
        }

        // If the loaded sample doesn't match the sample tracker we need to adjust the tracker.
        if (raw_header.Rate != st->Frequency
            || Get_OpenAL_Format((raw_header.Flags & 2) ? 16 : 8, (raw_header.Flags & 1) ? 2 : 1) != st->Format) {
            st->IsActive = false;
            st->IsNeedService = 0;
            st->IsMoreSource = false;

            // Set the new sample info.
            st->Frequency = raw_header.Rate;
            st->Format = Get_OpenAL_Format((raw_header.Flags & 2) ? 16 : 8, (raw_header.Flags & 1) ? 2 : 1);
        }

        ALint source_status;
        alGetSourcei(st->Source, AL_SOURCE_STATE, &source_status);

        // If the sample is already playing stop it.
        if (source_status != AL_STOPPED) {
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

        alGenBuffers(NUM_BUFFERS, st->AudioBuffers);
        int buffer_index = 0;

        while (buffer_index < NUM_BUFFERS) {

            int bytes_read = Aud_Sample_Copy(st,
                                         &st->SourcePtr,
                                         &st->Remainder,
                                         (void **)&st->QueueBuffer,
                                         &st->QueueSize,
                                         ChunkBuffer,
                                         BUFFER_CHUNK_SIZE,
                                         st->Compression);

            if (bytes_read > 0) {
                alBufferData(st->AudioBuffers[buffer_index++], st->Format, ChunkBuffer, bytes_read, st->Frequency);
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
        st->IsNeedService = 1;

        st->Volume = volume;

        alSourcef(st->Source, AL_GAIN, ((SoundVolume * st->Volume) / 256) / 256.0f);

        if (!Start_Primary_Sound_Buffer(false)) {
            //CCDebugString("Play_Sample_Handle - Can't start primary buffer!");
            return INVALID_AUDIO_HANDLE;
        }

        alSourcePlay(st->Source);

        // Playback was started so we set some needed sample tracker values.
        st->IsActive = true;

        return id;
    }

    return INVALID_AUDIO_HANDLE;
};

int OpenALAudioClass::Set_Sound_Vol(int volume)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    int oldvol = SoundVolume;
    SoundVolume = volume;
    return oldvol;
};

int OpenALAudioClass::Set_Score_Vol(int volume)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    int old = ScoreVolume;
    ScoreVolume = volume;

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerStruct* st = (SampleTrackerStruct *)SampleTracker[i];

        if (st->IsScore & st->IsActive) {
            alSourcef(st->Source, AL_GAIN, ((ScoreVolume * st->Volume) / 256) / 256.0f);
        }
    }

    return old;
};

void OpenALAudioClass::Fade_Sample(int index, int ticks)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (Sample_Status(index)) {
        SampleTrackerStruct* st = SampleTracker[index];

        if (ticks > 0 && !st->IsLoading) {
            st->Reducer = ((st->Volume / ticks) + 1);
        } else {
            Stop_Sample(index);
        }
    }
};

int OpenALAudioClass::Get_Free_Sample_Handle(int priority)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    int index = 0;

    for (index = SampleTracker.Count() - 1; index >= 0; --index) {
        if (!SampleTracker[index]->IsActive && !SampleTracker[index]->IsLoading) {
            if (!SampleTracker[index]->IsScore) {
                break;
            }
        }
    }

    if (index < 0) {
        for (index = 0; index < SampleTracker.Count() && SampleTracker[index]->Priority > priority; ++index) {
            ;
        }

        if (index == SampleTracker.Count()) {
            return INVALID_AUDIO_HANDLE;
        }

        Stop_Sample(index);
    }

    if (index == INVALID_AUDIO_HANDLE) {
        return INVALID_AUDIO_HANDLE;
    }

    if (SampleTracker[index]->FileHandle != nullptr) {
        SampleTracker[index]->Close_File();
    }

    if (SampleTracker[index]->Original) {
        if (!SampleTracker[index]->IsScore) {
            SampleTracker[index]->Original = 0;
        }
    }

    SampleTracker[index]->IsScore = false;
    return index;
};

long OpenALAudioClass::Sample_Length(const void* sample)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (sample == nullptr) {
        return 0;
    }

    AUDHeaderType header;
    memcpy(&header, sample, sizeof(header));
    unsigned time = header.UncompSize;

    if (header.Flags & 2) {
        time /= 2;
    }

    if (header.Flags & 1) {
        time /= 2;
    }

    if (header.Rate / 60 > 0) {
        time /= header.Rate / 60;
    }

    return time;
};

bool OpenALAudioClass::Start_Primary_Sound_Buffer(bool forced)
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    if (Context == nullptr || !GameInFocus) {
        return false;
    }

    alcProcessContext(Context);

    return true;
};

void OpenALAudioClass::Stop_Primary_Sound_Buffer()
{
    //DEBUG_INFO("%s\n", __FUNCTION__);

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        Stop_Sample(i);
    }

    if (Context != nullptr) {
        alcSuspendContext(Context);
    }
};
