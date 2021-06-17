/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENAL_AUDIO.CPP
 *
 *  @authors       OmniBlade, CCHyper, tomsons26
 *
 *  @brief         OpenAL audio interface.
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
#include "openal_audio.h"
#include "openal_load_dll.h"
#include "openal_util.h"
#include "openal_globals.h"
#include "wwaud.h"
#include "tibsun_globals.h"
#include "debughandler.h"


OpenALAudioClass::SampleTrackerType::SampleTrackerType() :
    IsActive(false),
    IsLoading(false),
    IsScore(false),
    IsSpeech(false),
    Original(nullptr),
    MoreSource(false),
    IsOneShot(false),
    Source(nullptr),
    Remainder(0),
    Priority(0),
    Volume(0),
    Reducer(0),
    Compression(SCOMP_NONE),
    IsNeedingService(false),
    Callback_Func(nullptr),
    FilePending(0),
    FilePendingSize(0),
    Odd(0),
    QueueBuffer(nullptr),
    QueueSize(0),
    FileHandle(nullptr),
    FileBuffer(nullptr),
    sSOSInfo(),
    OpenALSource(),
    OpenALFormat(),
    OpenALFrequency(),
    OpenALAudioBuffers()
{
}

OpenALAudioClass::SampleTrackerType::~SampleTrackerType()
{
    Original = nullptr;
    Source = nullptr;

    QueueBuffer = nullptr;
    FileBuffer = nullptr;

    /**
     *  Close the streaming file handle.
     */
    FileHandle->Close();
    delete FileHandle;
    FileHandle = nullptr;
    
    /**
     *  Cleanup OpenAL buffers.
     */

    ALint processed_count = -1;
    alSourceStop(OpenALSource);
    alGetSourcei(OpenALSource, AL_BUFFERS_PROCESSED, &processed_count);

    while (processed_count-- > 0) {
        ALuint tmp;
        alSourceUnqueueBuffers(OpenALSource, 1, &tmp);
    }

    alDeleteBuffers(OPENAL_BUFFER_COUNT, OpenALAudioBuffers);
}


/**
 *  Class constructor
 */
// DONE ========================================================================
OpenALAudioClass::OpenALAudioClass() :
    AudioDone(true),
    OpenALDevice(nullptr),
    OpenALContext(nullptr),
    MagicNumber(AUD_CHUNK_MAGIC_ID),
    UncompBuffer(nullptr),
    FileStreamBuffer(nullptr),
    StreamBufferSize(BUFFER_CHUNK_SIZE + 128),
    StreamBufferCount(STREAM_BUFFER_COUNT),
    SoundVolume(VOLUME_MAX),
    ScoreVolume(VOLUME_MAX),
    field_380(false),
    SampleTracker()
{
}


/**
 *  Class destructor.
 */
// DONE ========================================================================
OpenALAudioClass::~OpenALAudioClass()
{
    End();
}


/**
 *  
 */
// DONE ========================================================================
bool OpenALAudioClass::Init(HWND hWnd, int tracker_count)
{
    ALCenum error;

    if (!OpenALInitialised) {
        DEBUG_ERROR("OpenAL: OpenAL library is not initialised!\n");
        return false;
    }

    OpenALDevice = alcOpenDevice(nullptr);
    if (OpenALDevice == nullptr) {
        DEBUG_ERROR("OpenAL: Failed to open device!\n");
        //DEBUG_ERROR("OpenAL Error: %s", Get_OpenAL_Error(alcGetError(nullptr)));
        return false;
    }

    OpenALContext = alcCreateContext(OpenALDevice, nullptr);
    if (OpenALContext == nullptr || !alcMakeContextCurrent(OpenALContext)) {
        DEBUG_ERROR("OpenAL: Failed to create context - %s!\n", Get_OpenAL_Error(alcGetError(OpenALDevice)));
        alcCloseDevice(OpenALDevice);
        OpenALContext = nullptr;
        return false;
    }

    UncompBuffer = (unsigned char *)std::malloc(UNCOMP_BUFFER_SIZE);
    if (UncompBuffer == nullptr) {
        DEBUG_ERROR("OpenAL: Failed to allocate UncompBuffer!\n");
        return false;
    }

    /**
     *  Init the audio trackers.
     */
    Init_Trackers(tracker_count);

    AudioDone = false;

    return true;
}


/**
 *  
 */
void OpenALAudioClass::End()
{
    if (OpenALContext == nullptr) {
        for (int i = 0; i < SampleTracker.Count(); ++i) {
            Stop_Sample(i);
            alDeleteSources(1, &SampleTracker[i]->OpenALSource);
        }
    }
    SampleTracker.Count();

    if (UncompBuffer != nullptr) {
        std::free((void *)UncompBuffer);
        UncompBuffer = nullptr;
    }

    if (FileStreamBuffer != nullptr) {
        std::free((void *)FileStreamBuffer);
        FileStreamBuffer = nullptr;
    }

    alcMakeContextCurrent(nullptr);

    alcDestroyContext(OpenALContext);
    OpenALContext = nullptr;

    alcCloseDevice(OpenALDevice);
    OpenALDevice = nullptr;

    AudioDone = true;
}


bool OpenALAudioClass::Init_Trackers(int new_tracker_count)
{
    ALCenum error;

    new_tracker_count = std::max(8, new_tracker_count);

    /**
     *  Clear the previously allocated trackers.
     */
    SampleTracker.Clear();

    /**
     *  Create playback buffer trackers.
     */
    DEBUG_INFO("OpenAL: Changing tracker count to '%d'.\n", new_tracker_count);
    for (int i = 0; i < new_tracker_count; ++i) {

        SampleTrackerType *st = new SampleTrackerType;
        if (!st) {
            DEBUG_ERROR("OpenAL: Failed to create sample tracker '%d'!\n", i);
            return false;
        }
        
        //alGenBuffers(OPENAL_BUFFER_COUNT, st->OpenALAudioBuffers);
        //if ((error = alcGetError(OpenALDevice)) != AL_NO_ERROR) {
        //    DEBUG_ERROR("OpenAL: Failed to create context - %s!\n", Get_OpenAL_Error(error));
        //}

        alGenSources(1, &st->OpenALSource);
        if ((error = alcGetError(OpenALDevice)) != AL_NO_ERROR) {
            DEBUG_ERROR("OpenAL: Failed to create context - %s!\n", Get_OpenAL_Error(error));
            return false;
        }

        /**
         *  Set the default buffer format.
         */
        st->OpenALFrequency = 22050;
        st->OpenALFormat = Get_OpenAL_Format(16, 1);

        /**
         *  Add the new tracker.
         */
        SampleTracker.Add(st);
    }

    return true;
}


/**
 *  
 */
int OpenALAudioClass::Play_Sample(const void *sample, int priority, int volume)
{
    return Play_Sample_Handle(sample, priority, volume, Get_Free_Sample_Handle(priority));
}


/**
 *  
 */
int OpenALAudioClass::Play_Sample_Handle(const void *sample, int priority, int volume, int handle)
{
    if (AudioDone) {  
        return INVALID_AUDIO_HANDLE;
    }

    if (sample == nullptr) {
        return INVALID_AUDIO_HANDLE;
    }

    if (handle == INVALID_AUDIO_HANDLE) {
        return INVALID_AUDIO_HANDLE;
    }

    /**
     *  Read in the sample's header.
     */
    AUDHeaderType raw_header;
    std::memcpy(&raw_header, sample, sizeof(raw_header));

    /**
     *  Make sure the AUD format is expected.
     */
    if (SCompressType(raw_header.Compression) != SCOMP_SOS) {
        DEBUG_INFO("OpenAL: Unsupported AUD format!\n");
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerType *st = SampleTracker[handle];

    /**
     *  Set up basic sample tracker info.
     */
    st->Original = sample;
    st->OriginalSize = raw_header.Size + sizeof(AUDHeaderType);
    st->Priority = priority;
    st->Compression = SCompressType(raw_header.Compression);
    st->Odd = 0;
    st->Reducer = 0;
    st->QueueBuffer = nullptr;
    st->QueueSize = 0;
    st->Remainder = raw_header.Size;
    st->Source = ((unsigned char *)(sample)) + sizeof(AUDHeaderType);
    st->IsNeedingService = false;

    int desired_bits = (raw_header.Flags & 2) ? 16 : 8;
    int desired_channels = (raw_header.Flags & 1) ? 2 : 1;

    /**
     *  Compression is ADPCM so we need to init it's stream info.
     */
    st->sSOSInfo.wChannels = desired_channels;
    st->sSOSInfo.wBitSize = desired_bits;
    st->sSOSInfo.dwCompSize = raw_header.Size;
    st->sSOSInfo.dwUnCompSize = raw_header.Size * (st->sSOSInfo.wBitSize / 4);
    sosCODEC2InitStream(&st->sSOSInfo);

    /**
     *  If the loaded sample doesn't match the sample tracker we need to adjust the tracker.
     */
    ALenum desired_format =  Get_OpenAL_Format(desired_bits, desired_channels);

    if (raw_header.Rate != st->OpenALFrequency || desired_format != st->OpenALFormat) {

        st->IsActive = false;
        st->IsNeedingService = false;
        st->MoreSource = false;

        DEBUG_INFO("OpenAL: Changing sample tracker format.\n");

        /**
         *  Set the new sample info.
         */
        st->OpenALFrequency = raw_header.Rate;
        st->OpenALFormat = desired_format;
    }

    ALint source_status;
    alGetSourcei(st->OpenALSource, AL_SOURCE_STATE, &source_status);

    /**
     *  If the sample is already playing, stop it and delete buffers.
     */
    if (source_status != AL_STOPPED) {

        DEBUG_INFO("OpenAL: Sample already stopped, deleting buffers.\n");

        st->IsActive = false;
        st->IsNeedingService = false;
        st->MoreSource = false;

        ALint processed_count = -1;
        alSourceStop(st->OpenALSource);
        alGetSourcei(st->OpenALSource, AL_BUFFERS_PROCESSED, &processed_count);

        while (processed_count-- > 0) {
            ALuint tmp;
            alSourceUnqueueBuffers(st->OpenALSource, 1, &tmp);
        }

        alDeleteBuffers(OPENAL_BUFFER_COUNT, st->OpenALAudioBuffers);
    }

    /**
     *  Create a new buffer.
     */
    alGenBuffers(OPENAL_BUFFER_COUNT, st->OpenALAudioBuffers);

    int buffer_index = 0;
    while (buffer_index < OPENAL_BUFFER_COUNT) {

        int bytes_read = Sample_Copy(st, &st->Source, &st->Remainder, &st->QueueBuffer, &st->QueueSize,
                                     ChunkBuffer, BUFFER_CHUNK_SIZE, st->Compression, nullptr, nullptr);

        if (bytes_read > 0) {
            alBufferData(st->OpenALAudioBuffers[buffer_index++], st->OpenALFormat, ChunkBuffer, bytes_read, st->OpenALFrequency);
        }

        if (bytes_read == BUFFER_CHUNK_SIZE) {
            st->MoreSource = true;
            st->IsOneShot = false;
        } else {
            st->MoreSource = false;
            st->IsOneShot = true;
            break;
        }
    }

    alSourceQueueBuffers(st->OpenALSource, buffer_index, st->OpenALAudioBuffers);
    st->IsNeedingService = true;

    st->Volume = volume;

    alSourcef(st->OpenALSource, AL_GAIN, ((SoundVolume * st->Volume) / 256) / 256.0f);

    if (!Start_Primary_Sound_Buffer(false)) {
        return INVALID_AUDIO_HANDLE;
    }

    return Attempt_To_Play_Buffer(handle);
}


/**
 *  
 */
// DONE ========================================================================
int OpenALAudioClass::Attempt_To_Play_Buffer(int handle)
{
    ALCenum error;

    SampleTrackerType *st = SampleTracker[handle];

    //if (st->IsActive) {
    //    return handle;
    //}

    alSourcePlay(st->OpenALSource);
    if ((error = alcGetError(OpenALDevice)) != AL_NO_ERROR) {
        DEBUG_ERROR("OpenAL: Failed to play - %s!\n", Get_OpenAL_Error(error));
    }

    /**
     *  Playback has started so we set some needed sample tracker values.
     */
    st->IsActive = true;

    return handle;
}


// DONE ========================================================================
int OpenALAudioClass::Get_Free_Sample_Handle(int priority)
{
    SampleTrackerType *st;
    int free_index = INVALID_AUDIO_HANDLE;
    bool found_slot = false;

    /**
     *  Find a free tracker slot.
     */
    for (int index = SampleTracker.Count()-1; index >= 0; --index) {
        st = SampleTracker[index];
        if (priority == PRIORITY_MAX && st->IsScore) {
            found_slot = true;
            free_index = index;
            break;
        }
        if (st->IsScore || st->IsSpeech) {
            continue;
        }
        if (!st->IsActive && !st->IsLoading) {
            found_slot = true;
            free_index = index;
            break;
        }
    }

    /**
     *  If a free slot was not found, find a low priority slot we can free up.
     */
    if (!found_slot) {
        for (int index = 0; index < SampleTracker.Count(); ++index) {
            st = SampleTracker[index];
            if (priority == PRIORITY_MAX && st->IsScore) {
                found_slot = true;
                free_index = index;
                break;
            }
            if (st->IsScore || st->IsSpeech) {
                continue;
            }
            if (st->Priority < priority) {
                found_slot = true;
                free_index = index;
                break;
            }
        }
    }

    if (!found_slot) {
        return INVALID_AUDIO_HANDLE;
    }

    /**
     *  Kill this sample, it's our handle now!
     */
    Stop_Sample(free_index);

    st = SampleTracker[free_index];

    /**
     *  Free up the tracker ready to be reused.
     */
    if (st->FileHandle != nullptr) {
        st->FileHandle->Close();
        delete st->FileHandle;
        st->FileHandle = nullptr;
    }
    if (st->Original) {
        if (!st->IsScore) {
            st->Original = nullptr;
        }
    }
    st->IsScore = false;
    st->IsSpeech = false;

    /**
     *  Return the free tracker slot index.
     */
    return free_index;
}


int OpenALAudioClass::File_Stream_Sample_Vol(char const *filename, int volume, bool real_time_start)
{
    if (AudioDone) {
        return INVALID_AUDIO_HANDLE;
    }

    if (filename == nullptr) {
        return INVALID_AUDIO_HANDLE;
    }

    if (FileStreamBuffer == nullptr) {
        FileStreamBuffer = (unsigned char *)std::malloc((unsigned long)(StreamBufferSize * StreamBufferCount));
        for (int i = 0; i < SampleTracker.Count(); ++i) {
            SampleTrackerType *st = SampleTracker[i];
            st->FileBuffer = FileStreamBuffer;
        }
    }
    
    if (FileStreamBuffer == nullptr) {
        return INVALID_AUDIO_HANDLE;
    }

    CCFileClass *fh = new CCFileClass(filename);

    if (!fh->Is_Available() && !fh->Open()) {
        DEBUG_ERROR("OpenAL: Unable to open file \"%s\"!\n", filename);
        delete fh;
        return INVALID_AUDIO_HANDLE;
    }

    unsigned char *stream_ptr = nullptr;

    /**
     *  Fetch a free tracker handle, PRIORITY_MAX will give us 
     */
    int handle = Get_Free_Sample_Handle(PRIORITY_MAX);

    /**
     *  Get the streaming buffer.
     */
    if (field_380) {
        stream_ptr = new unsigned char [16 * StreamBufferSize];
    } else {
        stream_ptr = FileStreamBuffer;
    }

    if (!stream_ptr) {
        DEBUG_ERROR("OpenAL: Unable to obtain streaming buffer!\n");
        delete fh;
        return INVALID_AUDIO_HANDLE;
    }

    if (handle < SampleTracker.Count()) {
        SampleTrackerType *st = SampleTracker[handle];

        st->FilePending = 0;
        st->FilePendingSize = 0;
        st->IsScore = true;
        st->IsLoading = real_time_start;
        st->Volume = volume;
        st->FileHandle = fh;
        st->FileBuffer = stream_ptr;
        st->Priority = PRIORITY_MAX;

        File_Stream_Preload(handle);

        return handle;
    }

    return INVALID_AUDIO_HANDLE;
}


// DONE ========================================================================
int OpenALAudioClass::Stream_Sample_Vol(void *buffer, int size, bool (*callback)(short, short *, void **, int *), int volume, int handle)
{
    if (AudioDone || buffer == nullptr || size == 0) {
        return INVALID_AUDIO_HANDLE;
    }

    AUDHeaderType header;
    std::memcpy(&header, buffer, sizeof(header));
    int oldsize = header.Size;
    header.Size = size - sizeof(header);
    std::memcpy(buffer, &header, sizeof(header));

    int playid = Play_Sample_Handle(buffer, PRIORITY_MAX, volume, handle);

    header.Size = oldsize;
    std::memcpy(buffer, &header, sizeof(header));

    if (playid == INVALID_AUDIO_HANDLE) {
        return INVALID_AUDIO_HANDLE;
    }

    SampleTrackerType *st = SampleTracker[playid];
    st->Callback_Func = callback;
    st->Odd = 0;

    return playid;
}


// DONE ========================================================================
void OpenALAudioClass::File_Stream_Preload(int handle)
{
    SampleTrackerType *st = SampleTracker[handle];

    int maxnum = (StreamBufferCount/2) + 4;
    int num = st->IsLoading ? std::min<int>(st->FilePending+2, maxnum) : maxnum;

    int i = 0;

    for (i = st->FilePending; i < num; ++i) {

        int size = st->FileHandle->Read(static_cast<char *>(st->FileBuffer) + i * StreamBufferSize, StreamBufferSize);

        if (size > 0) {
            st->FilePendingSize = size;
            ++st->FilePending;
        }

        if (size < StreamBufferSize) {
            break;
        }
    }

    Maintenance_Callback();

    if (StreamBufferSize > st->FilePendingSize || i == maxnum) {

        int stream_size = st->FilePending == 1 ? st->FilePendingSize : StreamBufferSize;

        Stream_Sample_Vol(st->FileBuffer, stream_size, &File_Callback, st->Volume, handle);

        st->IsLoading = false;

        --st->FilePending;

        if (st->FilePending == 0) {
            st->Odd = 0;
            st->QueueBuffer = nullptr;
            st->QueueSize = 0;
            st->FilePendingSize = 0;

            st->Callback_Func = nullptr;

            st->FileHandle->Close();
            delete st->FileHandle;
            st->FileHandle = nullptr;

        } else {
            st->Odd = 2;
            --st->FilePending;

            if (st->FilePendingSize != StreamBufferSize) {
                st->FileHandle->Close();
                delete st->FileHandle;
                st->FileHandle = nullptr;
            }

            st->QueueBuffer = static_cast<char *>(st->FileBuffer) + StreamBufferSize;
            st->QueueSize = st->FilePending == 0 ? st->FilePendingSize : StreamBufferSize;
        }
    }
}


void OpenALAudioClass::Set_Handle_Volume(int handle, int volume)
{
    SampleTrackerType *st = SampleTracker[handle];

    if (st->IsActive) {
        if (Sample_Status(handle)) {
            if (st->IsScore) {
                alSourcef(st->OpenALSource, AL_GAIN, ((ScoreVolume * st->Volume) / 256) / 256.0f);
            } else {
                alSourcef(st->OpenALSource, AL_GAIN, ((SoundVolume * st->Volume) / 256) / 256.0f);
            }
        }
    }
}


void OpenALAudioClass::Set_Sample_Volume(const void *sample, int volume)
{
    int handle = INVALID_AUDIO_HANDLE;

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerType *st = SampleTracker[i];
        if (/*st->IsActive &&*/ st->Original == sample) {
            handle = i;
            break;
        }
    }

    if (handle != INVALID_AUDIO_HANDLE) {
        Set_Handle_Volume(handle, volume);
    }
}


void OpenALAudioClass::Set_Volume_All(int volume)
{
    SoundVolume = std::min<int>(volume, VOLUME_MAX);

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        Set_Handle_Volume(i, volume);
    }
}


void OpenALAudioClass::Adjust_Volume_All(int vol_percent)
{
    SoundVolume = std::min<int>((vol_percent * SoundVolume / 100), VOLUME_MAX);

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        Set_Handle_Volume(i, (vol_percent * SoundVolume / 100));
    }
}


int OpenALAudioClass::Set_Sound_Vol(int volume)
{
    int oldvol = SoundVolume;
    SoundVolume = std::min<int>(volume, VOLUME_MAX);

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerType *st = SampleTracker[i];

        /**
         *  Change the volume of non-score trackers only.
         */
        if (st->IsActive && !st->IsScore) {
            alSourcef(st->OpenALSource, AL_GAIN, ((SoundVolume * st->Volume) / 256) / 256.0f);
        }
    }

    return oldvol;
};


int OpenALAudioClass::Set_Score_Vol(int volume)
{
    int old = ScoreVolume;
    ScoreVolume = std::min<int>(volume, VOLUME_MAX);

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerType *st = SampleTracker[i];

        /**
         *  Change the volume of score trackers only.
         */
        if (st->IsActive && st->IsScore) {
            alSourcef(st->OpenALSource, AL_GAIN, ((ScoreVolume * st->Volume) / 256) / 256.0f);
        }
    }

    return old;
};


int OpenALAudioClass::Get_Playing_Sample_Handle(const void *sample)
{
    int handle = INVALID_AUDIO_HANDLE;

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerType *st = SampleTracker[i];
        if (st->IsActive && st->Original == sample) {
            handle = i;
            break;
        }
    }

    return handle;
}


int OpenALAudioClass::Get_Sample_Handle(const void *sample)
{
    int handle = INVALID_AUDIO_HANDLE;

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerType *st = SampleTracker[i];
        if (st->Original == sample) {
            handle = i;
            break;
        }
    }

    return handle;
}


// DONE ========================================================================
void OpenALAudioClass::Fade_Sample(int handle, int ticks)
{
    if (Sample_Status(handle)) {

        SampleTrackerType *st = SampleTracker[handle];

        if (ticks > 0 && !st->IsLoading) {
            st->Reducer = ((st->Volume / ticks) + 1);

        } else {
            Stop_Sample(handle);
        }
    }
}


// DONE ========================================================================
void OpenALAudioClass::Stop_Sample(int handle)
{
    if (AudioDone) {
        return;
    }

    if (handle == INVALID_AUDIO_HANDLE) {
        return;
    }
    
    if (handle >= SampleTracker.Count()) {
        return;
    }

    SampleTrackerType *st = SampleTracker[handle];

    if (st->IsActive || st->IsLoading) {

        st->IsActive = false;

        if (!st->IsLoading) {

            ALint processed_count = -1;
            alSourceStop(st->OpenALSource);
            alGetSourcei(st->OpenALSource, AL_BUFFERS_PROCESSED, &processed_count);

            while (processed_count-- > 0) {
                ALuint tmp;
                alSourceUnqueueBuffers(st->OpenALSource, 1, &tmp);
            }

            alDeleteBuffers(OPENAL_BUFFER_COUNT, st->OpenALAudioBuffers);
        }

        if (st->FileHandle) {
            st->FileHandle->Close();
            delete st->FileHandle;
            st->FileHandle = nullptr;
        }

        st->IsLoading = false;
        st->Priority = 0;
        st->FilePending = 0;
        st->FilePendingSize = 0;
        st->QueueBuffer = nullptr;
        st->Callback_Func = nullptr;
    }
}


// DONE ========================================================================
void OpenALAudioClass::Free_Sample(const void *sample)
{
    if (sample != nullptr) {
        std::free((void *)sample);
    }
}


// DONE ========================================================================
bool OpenALAudioClass::Sample_Status(int handle)
{
    if (AudioDone) {
        return false;
    }

    if (handle == INVALID_AUDIO_HANDLE && handle >= SampleTracker.Count()) {
        return false;
    }

    SampleTrackerType *st = SampleTracker[handle];

    if (st->IsLoading) {
        return true;
    }

    if (!st->IsActive) {
        return false;
    }

    ALint val;
    alGetSourcei(st->OpenALSource, AL_SOURCE_STATE, &val);

    return val == AL_PLAYING;
}


// DONE
bool OpenALAudioClass::Is_Sample_Playing(const void *sample)
{
    if (AudioDone || sample == nullptr) {
        return false;
    }

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerType *st = SampleTracker[i];
        if (st->Original == sample) {
            if (Sample_Status(i)) {
                return true;
            }
        }
    }

    return false;
}


// DONE ========================================================================
void OpenALAudioClass::Stop_Sample_Playing(const void *sample)
{
    if (sample == nullptr) {
        return;
    }

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerType *st = SampleTracker[i];
        if (st->Original == sample) {
            Stop_Sample(i);
            break;
        }
    }
}


#if 0
// DONE ========================================================================
void *OpenALAudioClass::Load_Sample(const char *filename)
{
    if (!filename) {
        return 0;
    }

    CCFileClass file(filename);
    if (!file.Is_Available()) {
        return 0;
    }

    file.Open(FILE_ACCESS_READ);

    if (!file.Is_Open()) {
        return 0;
    }
    
    void *data = nullptr;
    int data_size = file.Size() + sizeof(AUDHeaderType);
    data = std::malloc(data_size);

    if (data != nullptr) {
        Sample_Read(&file, data, data_size);
    }

    return data;
}
#endif


#if 0
// DONE ========================================================================
long OpenALAudioClass::Load_Sample_Into_Buffer(const char *filename, void *buffer, long size)
{
    if (buffer == nullptr || size == 0) {
        return 0;
    }

    if (!filename) {
        return 0;
    }

    CCFileClass file(filename);
    if (!file.Is_Available()) {
        return 0;
    }

    file.Open(FILE_ACCESS_READ);

    if (!file.Is_Open()) {
        return 0;
    }

    int sample_size = Sample_Read(&file, buffer, size);

    return sample_size;
}
#endif


#if 0
// DONE ========================================================================
long OpenALAudioClass::Sample_Read(FileClass *file, void* buffer, long size)
{
    if (buffer == nullptr || file == nullptr) {
        return 0;
    }

    if (size <= sizeof(AUDHeaderType)) {
        return 0;
    }

    AUDHeaderType header;

    int actual_bytes_read = file->Read(&header, sizeof(AUDHeaderType));
    int to_read = std::min<unsigned>(size - sizeof(AUDHeaderType), header.Size);

    actual_bytes_read += file->Read(static_cast<char*>(buffer) + sizeof(AUDHeaderType), to_read);

    std::memcpy(buffer, &header, sizeof(AUDHeaderType));

    return actual_bytes_read;
}
#endif


#if 0
// DONE ========================================================================
long OpenALAudioClass::Sample_Length(const void *sample)
{
    if (sample == nullptr) {
        return 0;
    }

    AUDHeaderType header;
    std::memcpy(&header, sample, sizeof(header));
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
}
#endif


// DONE ========================================================================
void OpenALAudioClass::Restore_Sound_Buffers()
{
    DEBUG_INFO("OpenAL: Restore Sound Buffers.\n");
}


// DONE ========================================================================
bool OpenALAudioClass::Set_Primary_Buffer_Format()
{
    DEBUG_INFO("OpenAL: Set Primary Buffer Format.\n");

    return true;
}


// DONE ========================================================================
bool OpenALAudioClass::Start_Primary_Sound_Buffer(bool forced)
{
    ALCenum error;

    DEBUG_INFO("OpenAL: Start Primary Sound Buffer.\n");

    if (OpenALContext == nullptr || !GameInFocus) {
        return false;
    }

    /**
     *  Start processing the OpenAL context.
     */
    alcProcessContext(OpenALContext);
    if ((error = alcGetError(OpenALDevice)) != AL_NO_ERROR) {
        DEBUG_ERROR("OpenAL: Failed to process context - %s!\n", Get_OpenAL_Error(error));
    }

    return true;
}


// DONE ========================================================================
void OpenALAudioClass::Stop_Primary_Sound_Buffer()
{
    ALCenum error;

    DEBUG_INFO("OpenAL: Stop Primary Sound Buffer.\n");

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        Stop_Sample(i);
    }

    if (OpenALContext == nullptr) {
        return;
    }

    /**
     *  Suspend the OpenAL context.
     */
    alcSuspendContext(OpenALContext);
    if ((error = alcGetError(OpenALDevice)) != AL_NO_ERROR) {
        DEBUG_ERROR("OpenAL: Failed to suspend context - %s!\n", Get_OpenAL_Error(error));
    }
}


// DONE ========================================================================
void OpenALAudioClass::Sound_Callback()
{
    if (AudioDone) {
        return;
    }

    /**
     *  Update sample buffers.
     */
    Maintenance_Callback();

    /**
     *  Update all trackers.
     */
    for (int i = 0; i < SampleTracker.Count(); ++i) {

        SampleTrackerType *st = SampleTracker[i];

        /**
         *  Is this tracker pending a load?
         */
        if (st->IsLoading) {
            File_Stream_Preload(i);

            /**
             *  We are done, next tracker...
             */
            continue;
        }

        /**
         *  Is this sample inactive? Perform a cleanup.
         */
        if (!st->IsActive) {
            // If so, we close the handle.
            if (st->FileHandle != nullptr) {
                st->FileHandle->Close();
                st->FileHandle = nullptr;
            }

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
            Stop_Sample(i);

            /**
             *  We are done, next tracker...
             */
            continue;
        }

        /**
         *  Process any pending files.
         */
        if ((st->QueueBuffer == nullptr || st->FileHandle != nullptr)
            && st->FilePending < (StreamBufferCount-3)) {

            if (st->Callback_Func) {

                if (!st->Callback_Func(i, &st->Odd, &st->QueueBuffer, &st->QueueSize)) {

                    /**
                     *  No files pending, so the file callback is not needed anymore.
                     */
                    st->Callback_Func = nullptr;
                }
            }

            /**
             *  We are done, next tracker...
             */
            continue;
        }

    }

}


void OpenALAudioClass::Maintenance_Callback()
{
    if (AudioDone) {
        return;
    }

    for (int i = 0; i < SampleTracker.Count(); ++i) {

        SampleTrackerType *st = SampleTracker[i];

        if (!st->IsActive) {
            continue;
        }

        // If this tracker needs processing and isn't already marked as being processed, then process it.
        if (st->IsNeedingService) {

            // Do we have more data in this tracker to play?
            if (st->MoreSource) {
                ALint processed_buffers;

                // Work out if we have any space to buffer more data right now.
                alGetSourcei(st->OpenALSource, AL_BUFFERS_PROCESSED, &processed_buffers);

                while (processed_buffers > 0 && st->MoreSource) {
                    int bytes_copied = Sample_Copy(st,
                                                    &st->Source,
                                                    &st->Remainder,
                                                    &st->QueueBuffer,
                                                    &st->QueueSize,
                                                    ChunkBuffer,
                                                    BUFFER_CHUNK_SIZE,
                                                    st->Compression,
                                                    nullptr,
                                                    nullptr);

                    if (bytes_copied != BUFFER_CHUNK_SIZE) {
                        st->MoreSource = false;
                    }

                    if (bytes_copied > 0) {
                        ALuint buffer;
                        alSourceUnqueueBuffers(st->OpenALSource, 1, &buffer);
                        alBufferData(buffer, st->OpenALFormat, ChunkBuffer, bytes_copied, st->OpenALFrequency);
                        alSourceQueueBuffers(st->OpenALSource, 1, &buffer);
                        --processed_buffers;
                    }
                }

            } else {
                ALint source_status;
                alGetSourcei(st->OpenALSource, AL_SOURCE_STATE, &source_status);

                if (source_status != AL_PLAYING) {
                    st->IsNeedingService = false;
                    Stop_Sample(i);
                }
            }
        }

        if (!st->QueueBuffer && st->FilePending != 0) {
            st->QueueBuffer = static_cast<char *>(st->FileBuffer) + StreamBufferSize * (st->Odd % StreamBufferCount);
            --st->FilePending;
            ++st->Odd;

            if (st->FilePending != 0) {
                st->QueueSize = StreamBufferSize;
            } else {
                st->QueueSize = st->FilePendingSize;
            }
        }

    }

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerType *st = SampleTracker[i];
        if (st->IsActive && st->Reducer > 0 && st->Volume > 0) {
            if (st->Reducer >= st->Volume) {
                st->Volume = VOLUME_MIN;
            } else {
                st->Volume -= st->Reducer;
            }

            if (st->IsScore) {
                alSourcef(st->OpenALSource, AL_GAIN, ((ScoreVolume * st->Volume) / 256) / 256.0f);
            } else {
                alSourcef(st->OpenALSource, AL_GAIN, ((SoundVolume * st->Volume) / 256) / 256.0f);
            }
        }
    }
}


bool OpenALAudioClass::File_Callback(short handle, short *odd, void **buffer, int *size)
{
    if (handle == INVALID_AUDIO_HANDLE) {
        return false;
    }

    SampleTrackerType *st = SampleTracker[handle];

    if (st->FileBuffer == nullptr) {
        return false;
    }

    if (*buffer == nullptr && st->FilePending) {
        *buffer = static_cast<char*>(st->FileBuffer) + StreamBufferSize * (*odd % StreamBufferCount);
        --st->FilePending;
        ++*odd;
        *size = st->FilePending == 0 ? st->FilePendingSize : StreamBufferSize;
    }

    Maintenance_Callback();

    if (st->FilePending < (StreamBufferCount/2) && st->FileHandle != nullptr) {

        if (StreamBufferCount-2 != st->FilePending) {

            /**
             *  Fill empty buffers.
             */
            for (int num_empty_buffers = (StreamBufferCount-2) - st->FilePending; num_empty_buffers && st->FileHandle != nullptr; --num_empty_buffers) {

                /**
                 *  Buffer to fill with data.
                 */
                void *tofill = static_cast<char *>(st->FileBuffer) + StreamBufferSize * ((st->FilePending + *odd) % StreamBufferCount);

                int psize = st->FileHandle->Read(tofill, StreamBufferSize);

                if (psize != StreamBufferSize) {
                    st->FileHandle->Close();
                    delete st->FileHandle;
                    st->FileHandle = nullptr;
                }

                if (psize > 0) {
                    st->FilePendingSize = psize;
                    ++st->FilePending;
                    Maintenance_Callback();
                }
            }
        }

        if (st->QueueBuffer == nullptr && st->FilePending) {
            st->QueueBuffer = static_cast<char *>(st->FileBuffer) + StreamBufferSize * (st->Odd % StreamBufferCount);
            --st->FilePending;
            ++st->Odd;
            st->QueueSize = st->FilePending > 0 ? StreamBufferSize : st->FilePendingSize;
        }

        Maintenance_Callback();
    }

    if (st->FilePending) {
        return true;
    }

    return false;
}


// DONE ========================================================================
int OpenALAudioClass::Simple_Copy(void **source, int *ssize, void **alternate, int *altsize, void **dest, int size)
{
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

    std::memcpy(*dest, *source, s);
    *source = static_cast<char *>(*source) + s;
    *ssize -= s;
    *dest = static_cast<char *>(*dest) + s;
    out = s;

    if ((size - s) == 0) {
        return out;
    }

    *source = *alternate;
    *ssize = *altsize;
    *alternate = nullptr;
    *altsize = 0;

    out = Simple_Copy(source, ssize, alternate, altsize, dest, (size - s)) + s;

    return out;
}


// DONE ========================================================================
int OpenALAudioClass::Sample_Copy(SampleTrackerType *st, void **source, int *ssize, void **alternate, int *altsize, void *dest, int size, SCompressType scomp, void *trailer, short *trailersize)
{
    if (!st) {
        return 0;
    }

    /**
     *  No compression, do a simple copy.
     */
    if (scomp == SCOMP_NONE) {
        return Simple_Copy(source, ssize, alternate, altsize, &dest, size);
    }

    /**
     *  If the compression does not match one we know, do a simple copy.
     */
    if (scomp != SCOMP_SOS) {
        return Simple_Copy(source, ssize, alternate, altsize, &dest, size);
    }

    int datasize = 0;

    while (size > 0) {
        short fsize;
        short dsize;
        unsigned magic;

        void* fptr = &fsize;
        void* dptr = &dsize;
        void* mptr = &magic;

        /**
         *  Verify and seek over the chunk header.
         */
        if (Simple_Copy(source, ssize, alternate, altsize, &fptr, sizeof(fsize)) < sizeof(fsize)) {
            break;
        }

        if (Simple_Copy(source, ssize, alternate, altsize, &dptr, sizeof(dsize)) < sizeof(dsize) || dsize > size) {
            break;
        }

        if (Simple_Copy(source, ssize, alternate, altsize, &mptr, sizeof(magic)) < sizeof(magic) || magic != MagicNumber) {
            break;
        }

        if (fsize == dsize) {

            /**
             *  File size matches size to decompress, so there's nothing to do other than copy the buffer over.
             */
            if (Simple_Copy(source, ssize, alternate, altsize, &dest, fsize) < dsize) {
                return datasize;
            }

        } else {

            /**
             *  Else we need to decompress it.
             */
            void *uptr = UncompBuffer;

            if (Simple_Copy(source, ssize, alternate, altsize, &uptr, fsize) < fsize) {
                return datasize;
            }

            _tagCOMPRESS_INFO2 *s = &st->sSOSInfo;
            s->lpSource = (unsigned char *)UncompBuffer;
            s->lpDest = (unsigned char *)dest;

            sosCODEC2DecompressData(s, dsize);

            dest = reinterpret_cast<char *>(dest) + dsize;
        }

        datasize += dsize;
        size -= dsize;
    }

    return datasize;
}
