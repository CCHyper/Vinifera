/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          DSAUDIO8.CPP
 *
 *  @author        CCHyper
 *
 *  @contributors  tomsons26
 *
 *  @brief         Main header file for the Direct Sound audio interface.
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
#include "dsaudio8.h"
#include "dsaudio8_util.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "ccfile.h"
#include "ini.h"
#include "wwaud.h"
#include "soscodec.h"
#include "dsaudio.h"
#include "endiantype.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Global instance of the new DirectSound8 audio engine.
 */
DirectSound8AudioClass DirectSound8Audio;


/**
 *  
 */
static HWND Audio_hWnd = nullptr;
static int Audio_BitsPerSample = 8;
static bool Audio_IsStereo = false;
static int Audio_Rate = 22050;
static unsigned Audio_NumSampleTrackers = 16;


DirectSound8AudioClass::tSampleTrackerType::tSampleTrackerType(int index) :
    Active(false),
    Loading(false),
    IsScore(false),
    Original(nullptr),
    PlayBuffer(nullptr),
    PlaybackRate(0),
    BitSize(0),
    Stereo(0),
    DataLength(0),
    DestPtr(nullptr),
    MoreSource(false),
    field_25(false),
    OneShot(false),
    field_27(false),
    Source(nullptr),
    Remainder(0),
    Priority(0),
    Handle(0),
    Volume(0),
    DSVolume(0),
    Reducer(0),
    Compression(SCOMP_NONE),
    Service(0),
    Callback(nullptr),
    QueueBuffer(nullptr),
    QueueSize(0),
    Odd(0),
    FilePending(0),
    FilePendingSize(0),
    FileHandle(nullptr),
    FileBuffer(nullptr),
    sSOSInfo(),
    BufferMutex(INVALID_HANDLE_VALUE)
{
    ZeroMemory(&BufferFormat, sizeof(BufferFormat));
    ZeroMemory(&BufferDesc, sizeof(BufferDesc));

    char buffer[128];
    std::snprintf(buffer, sizeof(buffer), "Secondary Audio Buffer Mutex %d", index);
    BufferMutex = CreateMutex(nullptr, FALSE, TEXT(buffer));
}


DirectSound8AudioClass::tSampleTrackerType::~tSampleTrackerType()
{
    Original = nullptr;
    DestPtr = nullptr;
    Source = nullptr;
    QueueBuffer = nullptr;
    FileBuffer = nullptr;

    PlayBuffer->Release();
    PlayBuffer = nullptr;

    FileHandle->Close();
    FileHandle = nullptr;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
DirectSound8AudioClass::DirectSound8AudioClass() :
    AudioFocusLossFunction(nullptr),
    StreamLowImpact(false),
    MagicNumber(AUD_CHUNK_MAGIC_ID),
    UncompBuffer(new char[UNCOMP_CHUNK_SIZE]),
    StreamBufferSize(8320),                                                     // TODO: How is this size set?
    FileStreamBuffer(new char[StreamBufferSize * STREAM_BUFFER_COUNT]),
    field_380(false),
    SoundVolume(VOLUME_MAX),
    SoundObject(nullptr),
    PrimaryBufferPtr(nullptr),
    AudioTimerHandle(0),
    AudioTimerResolution(0),
    PrimaryBufferDesc(new DSBUFFERDESC),
    PrimaryBufferFormat(new WAVEFORMATEX),
    TimerMutex(CreateMutex(nullptr, FALSE, TEXT("Audio Timer Mutex"))),
    GlobalAudioMutex(CreateMutex(nullptr, FALSE, TEXT("Global Audio Mutex"))),
    SampleTracker(),
    AudioDone(false)
{
    ZeroMemory(&PrimaryBufferDesc, sizeof(DSBUFFERDESC));
    ZeroMemory(&PrimaryBufferFormat, sizeof(WAVEFORMATEX));
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
DirectSound8AudioClass::~DirectSound8AudioClass()
{
    if (!AudioDone) {
        End();
    }

    Lock_Global_Mutex();

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        Lock_Secondary_Mutex(index);
    }

    if (UncompBuffer) {
        delete UncompBuffer;
        UncompBuffer = nullptr;
    }

    if (FileStreamBuffer) {
        delete FileStreamBuffer;
        FileStreamBuffer = nullptr;
    }

    if (SoundObject) {
        SoundObject->Release();
        SoundObject = nullptr;
    }

    if (PrimaryBufferPtr) {
        PrimaryBufferPtr->Release();
        PrimaryBufferPtr = nullptr;
    }

    if (PrimaryBufferFormat) {
        delete PrimaryBufferFormat;
        PrimaryBufferFormat = nullptr;
    }

    if (PrimaryBufferDesc) {
        delete PrimaryBufferDesc;
        PrimaryBufferDesc = nullptr;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        Unlock_Secondary_Mutex(index);
        CloseHandle(SampleTracker[index]->BufferMutex);
    }
    SampleTracker.Clear();

    Unlock_Global_Mutex();
    CloseHandle(GlobalAudioMutex);

    /**
     *  #BUGFIX:
     *  Timer mutex was not released in original code.
     */
    Unlock_Timer_Mutex();
    CloseHandle(TimerMutex);
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Init(HWND hWnd, int bits_per_sample, bool stereo, int rate)
{
    HRESULT dsresult;
    LPDIRECTSOUNDBUFFER temporary_buffer = nullptr;

    /**
     *  Store init values for future use.
     */
    Audio_hWnd = hWnd;
    Audio_BitsPerSample = bits_per_sample;
    Audio_IsStereo = stereo;
    Audio_Rate = rate;

    if (SoundObject) {
        return true;
    }

    Lock_Global_Mutex();

    /**
     *  Create the IDirectSound interface.
     */
    dsresult = DirectSoundCreate((LPCGUID)&DSDEVID_DefaultPlayback, &SoundObject, nullptr);
    if (FAILED(dsresult)) {
        DEBUG_ERROR("Failed to create direct sound object. Error code 0x%08X\n", dsresult);
        DS_Message_Box(hWnd, dsresult, "Warning! - Failed to create direct sound object.\n");
        Unlock_Global_Mutex();
        return false;
    }

    /**
     *  Set the DirectSound cooperation level.
     */
    dsresult = SoundObject->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
    if (FAILED(dsresult)) {
        DEBUG_ERROR("Failed to set cooperative level. Error code 0x%08X.\n", dsresult);
        DS_Message_Box(hWnd, dsresult, "Warning! - Failed to set cooperative level.\n");
        SoundObject->Release();
        SoundObject = nullptr;
        Unlock_Global_Mutex();
        return false;
    }

    /**
     *  Set up buffer description structure.
     */
    DSBUFFERDESC buffer_desc;
    ZeroMemory(&buffer_desc, sizeof(DSBUFFERDESC));
    buffer_desc.dwSize = sizeof(DSBUFFERDESC);
    buffer_desc.dwFlags = DSBCAPS_PRIMARYBUFFER/*|DSBCAPS_CTRLVOLUME*/;
    buffer_desc.lpwfxFormat = nullptr;

    /**
     *  Create the primary DirectSound buffer.
     */
    dsresult = SoundObject->CreateSoundBuffer(&buffer_desc, &temporary_buffer, nullptr);
    if (FAILED(dsresult)) {
        DEBUG_ERROR("Failed to create the primary sound buffer. Error code 0x%08X.\n", dsresult);
        DS_Message_Box(hWnd, dsresult, "Warning! - Failed to create the primary sound buffer.\n");
        SoundObject->Release();
        SoundObject = nullptr;
        Unlock_Global_Mutex();
        return false;
    }
    
    CopyMemory(&PrimaryBufferDesc, &buffer_desc, sizeof(DSBUFFERDESC));

    /**
     *  
     */
    dsresult = temporary_buffer->QueryInterface(IID_IDirectSoundBuffer, (LPVOID *)&PrimaryBufferPtr);
    if (FAILED(dsresult)) {
        DEBUG_ERROR("Failed to obtain the primary sound buffer. Error code 0x%08X.\n", dsresult);
        DS_Message_Box(hWnd, dsresult, "Warning! - Failed to create the primary sound buffer.\n");
        SoundObject->Release();
        SoundObject = nullptr;
        Unlock_Global_Mutex();
        return false;
    }

    temporary_buffer->Release();

    /**
     *  Set up wave format structure.
     */
    WAVEFORMATEX buffer_format;
    ZeroMemory(&buffer_format, sizeof(WAVEFORMATEX));
    buffer_format.nChannels = stereo ? 2 : 1;                  // Number of interleaved channels
    buffer_format.nSamplesPerSec = rate;                       // Sampling rate (blocks per second)
    buffer_format.wBitsPerSample = bits_per_sample;            // Bits per sample
    buffer_format.nBlockAlign = buffer_format.nChannels * buffer_format.wBitsPerSample / 8;     // Data block size (bytes)
    buffer_format.nAvgBytesPerSec = buffer_format.nBlockAlign * buffer_format.nSamplesPerSec;   // Data rate
    buffer_format.cbSize = 0;                                  // Size of the extension (0 or 22)
    buffer_format.wFormatTag = WAVE_FORMAT_PCM;                // Format code

    /**
     *  Set the primary buffer format.
     */
    dsresult = PrimaryBufferPtr->SetFormat(&buffer_format);
    if (FAILED(dsresult)) {
        DEBUG_ERROR("Failed to set primary buffer format! Disabling audio. Error code %x.\n", dsresult);
        DS_Message_Box(hWnd, dsresult, "Warning! - Your sound card does not meet the products audio requirements.");
        SoundObject->Release();
        SoundObject = nullptr;
        PrimaryBufferPtr->Release();
        PrimaryBufferPtr = nullptr;
        Unlock_Global_Mutex();
        return false;
    }

    CopyMemory(&PrimaryBufferFormat, &buffer_format, sizeof(WAVEFORMATEX));
    //CopyMemory(&PrimaryBufferFormat.cbSize, &buffer_format.cbSize, sizeof(PrimaryBufferFormat.cbSize));

    /**
     *  Start the primary buffer playing.
     */
    dsresult = PrimaryBufferPtr->Play(0, 0, DSBPLAY_LOOPING);
    if (FAILED(dsresult)) {
        DEBUG_ERROR("Failed to start primary sound buffer. Error code 0x%08X.\n", dsresult);
        DS_Message_Box(hWnd, dsresult, "Warning! - Failed to start primary sound buffer.\n");
        SoundObject->Release();
        SoundObject = nullptr;
        PrimaryBufferPtr->Release();
        PrimaryBufferPtr = nullptr;
        Unlock_Global_Mutex();
        return false;
    }

    /**
     *  Set up the highest resolution timer we can manage.
     */
    TIMECAPS tc;
    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR) {
        AudioTimerResolution = std::min<uint32_t>(std::max<uint32_t>(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);

    } else {
        DEBUG_WARNING("Failed to obtain timer resolution caps, using default resolution.\n");
        AudioTimerResolution = 25;
    }

    DEBUG_INFO("Audio timer resolution is %d milliseconds\n", AudioTimerResolution);

    /**
     *  Set the timer resolution.
     */
    timeBeginPeriod(AudioTimerResolution);

    AudioTimerHandle = timeSetEvent(TIMER_DELAY, TIMER_RESOLUTION, Sound_Timer_Callback, NULL, TIME_CALLBACK_FUNCTION|TIME_PERIODIC);




    RawFileClass file("SUN.INI");
    INIClass ini;
    ini.Load(file);
    Audio_NumSampleTrackers = ini.Get_Int("Audio", "SampleTrackers", Audio_NumSampleTrackers);




    /**
     *  Init the audio sample trackers.
     */
    Audio_NumSampleTrackers = std::clamp<unsigned>(Audio_NumSampleTrackers, 4+1, MAX_SFX);

    DEBUG_INFO("DSound: Allocating '%d' sample trackers.\n", Audio_NumSampleTrackers);

    for (int index = 0; index < Audio_NumSampleTrackers; ++index) {

        SampleTrackerType *st = new SampleTrackerType(index);
        ASSERT(st != nullptr);

        /**
         *  Set up buffer description structure.
         */
        DSBUFFERDESC buffer_desc;
        ZeroMemory(&buffer_desc, sizeof(DSBUFFERDESC));
        buffer_desc.dwSize = sizeof(DSBUFFERDESC);
        buffer_desc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLPAN|DSBCAPS_CTRLFREQUENCY;
        buffer_desc.dwBufferBytes = SECONDARY_BUFFER_SIZE;
        //buffer_desc.lpwfxFormat = nullptr;

        /**
         *  Copy the format from the primary buffer.
         */
        CopyMemory(&PrimaryBufferFormat, &st->BufferFormat, sizeof(WAVEFORMATEX));
        buffer_desc.lpwfxFormat = &st->BufferFormat;

        /**
         *  Create the secondary buffer for this tracker.
         */
        dsresult = SoundObject->CreateSoundBuffer(&buffer_desc, &temporary_buffer, nullptr);
        if (FAILED(dsresult)) {
            DEBUG_ERROR("Failed to create a secondary sound buffer [%d]! Error code 0x%08X.\n", index, dsresult);
            DS_Message_Box(Audio_hWnd, dsresult, "Warning! - Failed to create a secondary sound buffer.\n");
            SoundObject->Release();
            SoundObject = nullptr;
            PrimaryBufferPtr->Release();
            PrimaryBufferPtr = nullptr;
            Unlock_Global_Mutex();
            return false;
        }

        /**
         *  
         */
        dsresult = temporary_buffer->QueryInterface(IID_IDirectSoundBuffer, (LPVOID *)&st->PlayBuffer);
        if (FAILED(dsresult)) {
            DEBUG_ERROR("Failed to obtain a secondary sound buffer. Error code 0x%08X.\n", dsresult);
            DS_Message_Box(hWnd, dsresult, "Warning! - Failed to create a secondary sound buffer.\n");
            SoundObject->Release();
            SoundObject = nullptr;
            PrimaryBufferPtr->Release();
            PrimaryBufferPtr = nullptr;
            Unlock_Global_Mutex();
            return false;
        }

        temporary_buffer->Release();
        
        /**
         *  
         */
        st->PlaybackRate = Audio_Rate;
        st->Stereo = Audio_IsStereo;
        st->BitSize = Audio_BitsPerSample == 16 ? 2 : 0;
        st->FileHandle = nullptr;
        st->QueueBuffer = nullptr;

        SampleTracker.Add(st);
    }





    ini.Put_Int("Audio", "SampleTrackers", Audio_NumSampleTrackers);
    ini.Save(file);





    AudioDone = false;

    return true;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::End()
{
    Lock_Timer_Mutex();

    if (AudioTimerHandle) {
        timeKillEvent(AudioTimerHandle);
        AudioTimerHandle = 0;
        timeEndPeriod(AudioTimerResolution);
        AudioTimerResolution = 0;
    }

    Unlock_Timer_Mutex();

    if (SoundObject && PrimaryBufferPtr) {

        for (int index = 0; index < SampleTracker.Count(); ++index) {

            SampleTrackerType &st = *SampleTracker[index];

            Lock_Secondary_Mutex(index);

            if (st.PlayBuffer) {
                Stop_Sample(index);
                st.PlayBuffer->Stop();
                st.PlayBuffer->Release();
                st.PlayBuffer = nullptr;
            }

            if (st.FileBuffer) {
                if (st.FileBuffer != FileStreamBuffer) {
                    delete st.FileBuffer;
                }
                st.FileBuffer = nullptr;
            }
            
            Unlock_Secondary_Mutex(index);
        }
    }

    Lock_Global_Mutex();

    if (PrimaryBufferPtr) {
        PrimaryBufferPtr->Stop();
        PrimaryBufferPtr->Release();
        PrimaryBufferPtr = nullptr;
    }

    if (SoundObject) {
        SoundObject->Release();
        SoundObject = nullptr;
    }

    if (UncompBuffer) {
        delete UncompBuffer;
        UncompBuffer = nullptr;
    }

    if (FileStreamBuffer) {
        delete FileStreamBuffer;
        FileStreamBuffer = nullptr;
    }

    field_380 = false;
    AudioDone = true;

    Unlock_Global_Mutex();
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Stop_Sample(int handle)
{
    if (!SoundObject || !AudioDone) {
        return;
    }

    if (handle < 0 && handle >= SampleTracker.Count()) {
        return;
    }

    SampleTrackerType &st = *SampleTracker[handle];

    Lock_Secondary_Mutex(handle);

    if (st.Active || st.Loading) {
        st.Priority = 0;

        if (!st.IsScore) {
            st.Original = nullptr;
        }

        if (!st.Loading) {
            st.PlayBuffer->Stop();
        }

        st.Active = false;
        st.Loading = false;

        if (st.FileHandle) {
            st.FileHandle->Close();
            delete st.FileHandle;
            st.FileHandle = nullptr;
        }

        if (st.FileBuffer) {
            if (st.FileBuffer == FileStreamBuffer) {
                field_380 = false;
            } else {
                delete st.FileBuffer;
                st.FileBuffer = nullptr;
            }
        }

        st.Loading = 0;
        st.Priority = 0;
        st.FilePending = 0;
        st.FilePendingSize = 0;
        st.QueueBuffer = nullptr;
        st.Callback = nullptr;
    }

    Unlock_Secondary_Mutex(handle);
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Sample_Status(int handle)
{
    if (!SoundObject || !AudioDone) {
        return false;
    }

    if (handle < 0 && handle >= SampleTracker.Count()) {
        DEBUG_WARNING("DSAudio: Invalid handle in Sample_Status()!\n");
        return false;
    }

    SampleTrackerType &st = *SampleTracker[handle];

    if (st.Loading || !st.Active) {
        return true;
    }

    if (st.PlayBuffer) {

        Lock_Secondary_Mutex(handle);

        DWORD status;
        HRESULT dsresult = st.PlayBuffer->GetStatus(&status);

        Unlock_Secondary_Mutex(handle);

        if (FAILED(dsresult)) {
            DEBUG_WARNING("DSAudio: GetStatus failed on secondary buffer %d\n", handle);
            return false;
        }
        
        /**
         *  Possible #BUGFIX:
         *  The Buffer has to be set as looping to qualify as playing.
         */
        //return (status & DSBSTATUS_PLAYING) && (status & DSBSTATUS_LOOPING);
        return (status & (DSBSTATUS_PLAYING|DSBSTATUS_LOOPING)) != 0;
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Is_Sample_Playing(const void *sample)
{
    if (AudioDone || sample == nullptr) {
        return false;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerType &st = *SampleTracker[index];
        if (st.Original == sample) {
            if (Sample_Status(index)) {
                return true;
            }
        }
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Stop_Sample_Playing(const void *sample)
{
    if (AudioDone || sample == nullptr) {
        return;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerType &st = *SampleTracker[index];
        if (st.Original == sample) {
            if (Sample_Status(index)) {
                Stop_Sample(index);
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Play_Sample(const void *sample, int priority, int volume)
{
    return Play_Sample_Handle(sample, priority, volume, Get_Free_Sample_Handle(priority));
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::File_Stream_Sample_Vol(char const *filename, int volume, bool real_time_start)
{
    if (AudioDone || !SoundObject) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!filename) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!CCFileClass(filename).Is_Available()) {
        return INVALID_AUDIO_HANDLE;
    }

    int handle = Get_Free_Sample_Handle(PRIORITY_MAX);
    if (handle == INVALID_AUDIO_HANDLE) {
        DEBUG_ERROR("DSAudio[%d]: Unable to find free handle!\n");
        return INVALID_AUDIO_HANDLE;
    }

    CCFileClass *fh = new CCFileClass(filename);

    if (!fh) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!fh->Is_Available()) {
        DEBUG_ERROR("DSAudio[%d]: File not available!\n", handle);
        return INVALID_AUDIO_HANDLE;
    }

    if (!fh->Open(FILE_ACCESS_READ)) {
        DEBUG_ERROR("DSAudio[%d]: Unable to open file!\n", handle);
        return INVALID_AUDIO_HANDLE;
    }

    void *stream_ptr = nullptr;
    if (field_380) {
        stream_ptr = new unsigned char [STREAM_BUFFER_COUNT * StreamBufferSize];
    } else {
        stream_ptr = FileStreamBuffer;
        field_380 = true;
    }
        
    if (!stream_ptr) {
        delete fh;
        DEBUG_ERROR("DSAudio[%d]: Unable to obtain streaming buffer!\n", handle);
        return INVALID_AUDIO_HANDLE;
    }
        
    Lock_Secondary_Mutex(handle);

    SampleTrackerType &st = *SampleTracker[handle];

    st.FilePending = 0;
    st.FilePendingSize = 0;
    st.IsScore = true;
    st.Loading = real_time_start;
    st.DSVolume = volume;
    st.Volume = volume * 128;
    st.FileHandle = fh;
    st.FileBuffer = stream_ptr;

    Lock_Secondary_Mutex(handle);

    File_Stream_Preload(handle);

    return handle;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Fade_Sample(int handle, int ticks)
{
    Lock_Secondary_Mutex(handle);

    if (Sample_Status(handle)) {
        SampleTrackerType &st = *SampleTracker[handle];
        if (ticks > 0 && !st.Loading) {
            st.Reducer = (st.Volume / (ticks + 1));
        } else {
            Stop_Sample(handle);
        }
    }

    Unlock_Secondary_Mutex(handle);
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Get_Playing_Sample_Handle(const void *sample)
{
    int handle = INVALID_AUDIO_HANDLE;

    if (!AudioDone) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!sample) {
        return INVALID_AUDIO_HANDLE;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerType &st = *SampleTracker[index];
        if (st.Original == sample) {
            if (Sample_Status(index)) {
                return index;
            }
        }
    }

    return INVALID_AUDIO_HANDLE;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Lock_Global_Mutex()
{
    if (WaitForSingleObject(GlobalAudioMutex, MUTEX_TIMEOUT) == WAIT_TIMEOUT) {
        DEBUG_WARNING("DirectSound8Audio: Probable deadlock occurred on GlobalAudioMutex.\n");
        return false;
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Unlock_Global_Mutex()
{
    if (ReleaseMutex(GlobalAudioMutex) == FALSE) {
        DEBUG_WARNING("DirectSound8Audio: Failed to release GlobalAudioMutex.\n");
        return false;
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Lock_Timer_Mutex()
{
    if (WaitForSingleObject(TimerMutex, MUTEX_TIMEOUT) == WAIT_TIMEOUT) {
        DEBUG_WARNING("DirectSound8Audio: Probable deadlock occurred on TimerMutex.\n");
        return false;
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Unlock_Timer_Mutex()
{
    if (ReleaseMutex(TimerMutex) == FALSE) {
        DEBUG_WARNING("DirectSound8Audio: Failed to release TimerMutex.\n");
        return false;
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Lock_Secondary_Mutex(int handle)
{
    if (WaitForSingleObject(SampleTracker[handle]->BufferMutex, MUTEX_TIMEOUT) != WAIT_TIMEOUT) {
        return true;
    }
    DEBUG_WARNING("DirectSound8Audio: Probable deadlock occurred on secondary buffer [%d].\n", handle);
    return false;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Unlock_Secondary_Mutex(int handle)
{
    if (ReleaseMutex(SampleTracker[handle]->BufferMutex) != FALSE) {
        return true;
    }
    DEBUG_WARNING("DirectSound8Audio: Failed to release secondary buffer [%d].\n", handle);
    return false;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Lock_Mutex()
{
    DEBUG_INFO("DirectSound8Audio: Taking ownership of all audio mutexes\n");

    if (!Lock_Timer_Mutex()) {
        return false;
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        if (WaitForSingleObject(SampleTracker[index]->BufferMutex, MUTEX_TIMEOUT) == WAIT_TIMEOUT) {
            ReleaseMutex(SampleTracker[index]->BufferMutex);
            return false;
        }
    }

    if (WaitForSingleObject(GlobalAudioMutex, MUTEX_TIMEOUT) != WAIT_TIMEOUT) {
        return true;
    }

    for (int index = SampleTracker.Count()-1; index >= 0; --index) {
        Unlock_Secondary_Mutex(index);
    }

    Unlock_Timer_Mutex();

    return true;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Unlock_Mutex()
{
    Unlock_Global_Mutex();

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        Unlock_Secondary_Mutex(index);
    }

    Unlock_Timer_Mutex();

    DEBUG_INFO("DirectSound8Audio: Released ownership of all audio mutexes\n");

    return true;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Gain_All(int a1)
{
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Mute_All()
{
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Set_Handle_Volume(int handle, int volume)
{
    if (!Sample_Status(handle)) {
        return;
    }

    Lock_Secondary_Mutex(handle);

    SampleTrackerType &st = *SampleTracker[handle];

    if (st.Active) {
        volume = std::min<int32_t>(volume, VOLUME_MAX);
        if (volume * SoundVolume / VOLUME_MAX) {
            DEBUG_INFO("Setting volume on tracker %d, from %d to %d", handle, st.Volume, volume);
            if (FAILED(st.PlayBuffer->SetVolume(Convert_HMI_To_Direct_Sound_Volume(volume)))) {
                DEBUG_INFO("Failed to set volume! Handle %d.\n", handle);
            }
            st.Volume = volume * 128;
        }
    }

    Unlock_Secondary_Mutex(handle);
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Set_Sample_Volume(const void *sample, int volume)
{
    if (sample == nullptr) {
        return;
    }

    int handle = Get_Playing_Sample_Handle(sample);
    if (handle != INVALID_AUDIO_HANDLE) {

        Lock_Secondary_Mutex(handle);

        SampleTrackerType &st = *SampleTracker[handle];
        if (st.Active) {
            volume = std::min<int32_t>(volume, VOLUME_MAX);
            if (volume * SoundVolume / VOLUME_MAX) {
                DEBUG_INFO("Setting volume on tracker %d, from %d to %d", handle, st.Volume, volume);
                if ( FAILED(st.PlayBuffer->SetVolume(Convert_HMI_To_Direct_Sound_Volume(volume))) ) {
                    DEBUG_INFO("Failed to set volume! Handle %d.\n", handle);
                }
                st.Volume = volume * 128;
            }
        }

        Unlock_Secondary_Mutex(handle);
    }
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Set_Sound_Vol(int volume)
{
    int oldvol = SoundVolume;
    SoundVolume = volume;
    return oldvol;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Set_Score_Vol(int volume)
{
    int old = ScoreVolume;
    ScoreVolume = volume;

    for (int i = 0; i < SampleTracker.Count(); ++i) {
        SampleTrackerType &st = *SampleTracker[i];

        if (st.IsScore & st.Active) {
            st.PlayBuffer->SetVolume(
                Convert_HMI_To_Direct_Sound_Volume((ScoreVolume * (st.Volume / 128)) / 256));
        }
    }

    return old;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::sub_48A250(int handle, int volume)
{
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::sub_48A340(int handle, int volume)
{
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Set_Volume_All(int vol)
{
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Adjust_Volume_All(int vol_percent)
{
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Start_Primary_Sound_Buffer(bool forced)
{
    if (!SoundObject || !PrimaryBufferPtr) {
        return false;
    }

    if (!AudioDone || !GameInFocus) {
        return false;
    }  
        
    Lock_Global_Mutex();

    bool started = false;
    HRESULT dsresult = 0;

    if (forced) {
        dsresult = PrimaryBufferPtr->Play(0, 0, DSBPLAY_LOOPING);
        started = true;

    } else {
        DWORD status = 0;
        if (SUCCEEDED(PrimaryBufferPtr->GetStatus(&status))) {
            if (!(status & DSBSTATUS_PLAYING) && !(status & DSBSTATUS_LOOPING)) {
                dsresult = PrimaryBufferPtr->Play(0, 0, DSBPLAY_LOOPING);
            }

            started = true;
        }
    }

    if (FAILED(dsresult)) {
        DEBUG_ERROR("Failed to play primary sound buffer. Error code 0x%08X.\n", dsresult);
        DS_Message_Box(nullptr, dsresult, "Warning! - Failed to play primary sound buffer!\n");
    }

    Unlock_Global_Mutex();

    return started;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Stop_Primary_Sound_Buffer()
{
    Lock_Global_Mutex();

    if (PrimaryBufferPtr) {
        HRESULT dsresult;
        for (int attempts = 0; attempts < 4 && dsresult == DSERR_BUFFERLOST; ++attempts) {
            dsresult = PrimaryBufferPtr->Stop();
            if (FAILED(dsresult)) {
                DEBUG_ERROR("Failed to stop primary sound buffer. Error code 0x%08X.\n", dsresult);
                DS_Message_Box(nullptr, dsresult, "Warning! - Failed to stop primary sound buffer!\n");
            }
        }
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        Stop_Sample(index);
    }

    Unlock_Global_Mutex();
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Sound_Callback()
{
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Get_Free_Sample_Handle(int priority)
{
    int index;

    for (index = SampleTracker.Count() - 1; index >= 0; --index) {

        Lock_Secondary_Mutex(index);

        SampleTrackerType &st = *SampleTracker[index];

        if (!st.Active && !st.Loading) {
            break;
        }

        Unlock_Secondary_Mutex(index);
    }

    if ( index < 0 ) {

        for (index = 0; index < SampleTracker.Count(); ++index) {

            SampleTrackerType &st = *SampleTracker[index];

            if (priority > st.Priority && priority < PRIORITY_MAX) {
                break;
            }
        }

        if (index < SampleTracker.Count()) {

            Stop_Sample(index);

            if (index != INVALID_AUDIO_HANDLE) {

                SampleTrackerType &st = *SampleTracker[index];

                if (st.FileHandle) {
                    st.FileHandle->Close();
                    delete st.FileHandle;
                    st.FileHandle = nullptr;
                }

                if (st.Original) {
                    st.Original = nullptr;
                }
            }
            return index;
        }
        return INVALID_AUDIO_HANDLE;

    } else if (index != INVALID_AUDIO_HANDLE) {

        SampleTrackerType &st = *SampleTracker[index];

        if (st.FileHandle) {
            st.FileHandle->Close();
            delete st.FileHandle;
            st.FileHandle = nullptr;
        }

        if (st.Original) {
            st.Original = nullptr;
        }
    }

    return index;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Play_Sample_Handle(const void *sample, int priority, int volume, int handle)
{
    if (handle == INVALID_AUDIO_HANDLE) {
        return INVALID_AUDIO_HANDLE;
    }
    if (SoundObject == nullptr) {
        return INVALID_AUDIO_HANDLE;
    }
    if (AudioDone) {
        return INVALID_AUDIO_HANDLE;
    }
    if (sample == nullptr) {
        return INVALID_AUDIO_HANDLE;
    }

    DWORD status;
    HRESULT dsresult;

    SampleTrackerType &st = *SampleTracker[handle];

    AUDHeaderType aud_hdr;
    CopyMemory(&aud_hdr, sample, sizeof(AUDHeaderType));

    aud_hdr.Rate = std::clamp<unsigned short>(aud_hdr.Rate, 20000, 48000);

    st.Compression = (SCompressType)aud_hdr.Compression;
    st.Original = sample;
    st.Odd = 0;
    st.Reducer = 0;
    st.QueueBuffer = 0;
    st.QueueSize = 0;
    st.Priority = priority;
    st.Service = false;
    st.Remainder = aud_hdr.Size;
    st.Source = (void *)(static_cast<const unsigned char *>(sample) + sizeof(AUDHeaderType));

    if (st.Compression == SCOMP_SOS) {
        st.sSOSInfo.wChannels = (aud_hdr.Flags & 1) ? 2 : 1;
        st.sSOSInfo.wBitSize = (aud_hdr.Flags & 2) ? 16 : 8;
        st.sSOSInfo.dwCompSize = aud_hdr.Size;
        st.sSOSInfo.dwUnCompSize = aud_hdr.Size * (st.sSOSInfo.wBitSize / 4);
        sosCODEC2InitStream(&st.sSOSInfo);
    }

    /**
     *  If the requested sample is a different format from the last sample, then
     *  recreate the sound buffer with the new format properties.
     */
    if (aud_hdr.Rate != st.PlaybackRate
    || (aud_hdr.Flags & 2) != (st.BitSize & 2)
    || (aud_hdr.Flags & 1) != (st.Stereo & 1)) {

        DEBUG_INFO("DirectSound8Audio [%d]: Changing sample tracker format:\n", handle);
        DEBUG_INFO("  Rate: %d\n", aud_hdr.Rate);
        DEBUG_INFO("  BitsPerSample: %d\n", (aud_hdr.Flags & 2) ? 16 : 8);
        DEBUG_INFO("  Channels: %s\n", (aud_hdr.Flags & 1) ? "stereo" : "mono");

        st.Active = false;
        st.Service = false;
        st.MoreSource = false;

        do {
            dsresult = st.PlayBuffer->GetStatus(&status);
            if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st.PlayBuffer)) {
                return INVALID_AUDIO_HANDLE;
            }
        } while (dsresult == DSERR_BUFFERLOST);

        if (status & (DSBSTATUS_PLAYING | DSBSTATUS_LOOPING)
            && (st.PlayBuffer->Stop(), dsresult == DSERR_BUFFERLOST) && !Attempt_Audio_Restore(st.PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }

        if (st.PlayBuffer) {
            st.PlayBuffer->Release();
            st.PlayBuffer = nullptr;
        }

        WAVEFORMATEX buffformat;
        DSBUFFERDESC bufferdesc;

        buffformat.nSamplesPerSec = aud_hdr.Rate;
        buffformat.nChannels = (aud_hdr.Flags & 1) + 1;
        buffformat.wBitsPerSample = aud_hdr.Flags & 2 ? 16 : 8;
        buffformat.nBlockAlign = buffformat.nChannels * buffformat.wBitsPerSample / 8;
        buffformat.nAvgBytesPerSec = buffformat.nBlockAlign * buffformat.nSamplesPerSec;



        //
        // query for updated interface
        //
        dsresult = SoundObject->QueryInterface(IID_IDirectSoundBuffer, (LPVOID *)&st.PlayBuffer);
        if (FAILED(dsresult)) {
            DEBUG_ERROR("Failed to obtain a secondary sound buffer. Error code 0x%08X.\n", dsresult);
            DS_Message_Box(Audio_hWnd, dsresult, "Warning! - Failed to obtain a secondary sound buffer.\n");
            SoundObject->Release();
            SoundObject = nullptr;
            Unlock_Global_Mutex();
            return false;
        }



        if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st.PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }

        if (dsresult != DS_OK && dsresult != DSERR_BUFFERLOST) {
            st.PlaybackRate = 0;
            st.Stereo = false;
            st.BitSize = 0;
            return INVALID_AUDIO_HANDLE;
        }

        CopyMemory(&st.BufferFormat, &buffformat, sizeof(WAVEFORMATEX));
        CopyMemory(&st.BufferDesc, &bufferdesc, sizeof(DSBUFFERDESC));

        st.PlaybackRate = aud_hdr.Rate;
        st.Stereo = (aud_hdr.Flags & 1) ? 2 : 1;
        st.BitSize = (aud_hdr.Flags & 2) ? 16 : 8;
    }

    do {
        dsresult = st.PlayBuffer->GetStatus(&status);
        if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st.PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }
    } while (dsresult == DSERR_BUFFERLOST);

    if (status & (DSBSTATUS_PLAYING|DSBSTATUS_LOOPING)) {
        st.Active = false;
        st.Service = false;
        st.MoreSource = false;
        st.PlayBuffer->Stop();
    }

    LPVOID play_buffer_ptr;
    DWORD lock_length1;
    LPVOID dummy_buffer_ptr;
    DWORD lock_length2;

    do {
        dsresult = st.PlayBuffer->Lock(
            0, BUFFER_CHUNK_SIZE, &play_buffer_ptr, &lock_length1, &dummy_buffer_ptr, &lock_length2, 0);
        if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st.PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }
    } while (dsresult == DSERR_BUFFERLOST);

    if (dsresult != DS_OK) {
        DEBUG_INFO("DirectSound8Audio: Bad sample format!\n");

        st.PlaybackRate = 0;
        st.Stereo = false;
        st.BitSize = 0;

        if (st.FileHandle) {
            st.FileHandle->Close();
            if (st.FileHandle) {
                delete st.FileHandle;
            }
            st.FileHandle = nullptr;
        }
        if (st.FileBuffer) {
            if (st.FileBuffer == FileStreamBuffer) {
                field_380 = false;
            } else {
                delete st.FileBuffer;
            }
            st.FileBuffer = nullptr;
        }

        st.Loading = false;
        st.Priority = 0;
        st.FilePending = 0;
        st.FilePendingSize = 0;
        st.QueueBuffer = nullptr;
        st.Callback = nullptr;

        return INVALID_AUDIO_HANDLE;
    }

    st.DestPtr = (void *)Sample_Copy(&st,
        &st.Source,
        &st.Remainder,
        &st.QueueBuffer,
        &st.QueueSize,
        play_buffer_ptr,
        BUFFER_CHUNK_SIZE,
        st.Compression);

    if (st.DestPtr == (void *)BUFFER_CHUNK_SIZE) {
        st.MoreSource = true;
        st.Service = true;
        st.OneShot = false;
    } else {
        st.MoreSource = false;
        st.OneShot = true;
        st.Service = true;

        int v47 = (BUFFER_CHUNK_SIZE/4);
        if (aud_hdr.UncompSize < (v47*3)) {
            v47 = aud_hdr.UncompSize;
        }

        int v50 = std::max(BUFFER_CHUNK_SIZE - v47, v47);

        ZeroMemory(static_cast<char *>(play_buffer_ptr) + (unsigned)st.DestPtr, v50);
    }

    st.PlayBuffer->Unlock(play_buffer_ptr, lock_length1, dummy_buffer_ptr, lock_length2);
    st.DSVolume = volume;
    st.Volume = volume * 128;

    do {
        st.PlayBuffer->SetVolume(Convert_HMI_To_Direct_Sound_Volume((volume * SoundVolume) / 256));
        if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st.PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }
    } while (dsresult == DSERR_BUFFERLOST);

    if (!Start_Primary_Sound_Buffer(false)) {
        return INVALID_AUDIO_HANDLE;
    }

    do {
        dsresult = st.PlayBuffer->SetCurrentPosition(0);
        if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st.PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }
    } while (dsresult == DSERR_BUFFERLOST);

    do {
        dsresult = st.PlayBuffer->Play(0, 0, DSBPLAY_LOOPING);

        if (dsresult == DS_OK) {

            st.Active = true;
            st.Handle = handle;

            return st.Handle;
        }

        if (dsresult != DSERR_BUFFERLOST) {
            st.Active = false;
            break;
        }

        if (!Attempt_Audio_Restore(st.PlayBuffer)) {
            break;
        }

    } while (dsresult == DSERR_BUFFERLOST);

    return INVALID_AUDIO_HANDLE;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Stream_Sample_Vol(void *buffer, int size, bool(*callbackptr)(short, short*, void**, int*), int volume, int handle)
{
    if (AudioDone || !SoundObject) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!buffer|| size <= 0) {
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

    Lock_Secondary_Mutex(playid);

    SampleTrackerType &st = *SampleTracker[playid];

    st.Odd = 0;
    st.Callback = callbackptr;

    Unlock_Secondary_Mutex(playid);

    return playid;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::File_Stream_Preload(int handle)
{
    Lock_Secondary_Mutex(handle);

    SampleTrackerType &st = *SampleTracker[handle];

    int maxnum = (st.Loading
                    ? std::min(st.FilePending + 2, (STREAM_BUFFER_COUNT/2) + 4)
                    : (STREAM_BUFFER_COUNT/2) + 4);
    int pending = 0;

    for (pending = st.FilePending; pending < maxnum; ++pending) {
        int num = st.FileHandle->Read(static_cast<unsigned char *>(st.FileBuffer) + pending * StreamBufferSize, StreamBufferSize);

        if (num > 0) {
            st.FilePendingSize = num;
            ++st.FilePending;
        }

        if (num < StreamBufferSize) {
            break;
        }
    }

    /**
     *  If we are on the last chunk of file or we have filled up our buffer.
     */
    if (StreamBufferSize > st.FilePendingSize || pending == maxnum) {

        int stream_size = st.FilePending == 1 ? st.FilePendingSize : StreamBufferSize;

        Stream_Sample_Vol(st.FileBuffer, stream_size, File_Callback, st.Volume, handle);

        --st.FilePending;
        st.Loading = false;

        if (!st.FilePending) {

            st.Odd = 0;
            st.QueueBuffer = nullptr;
            st.QueueSize = 0;
            st.FilePendingSize = 0;
            st.Callback = nullptr;

            if (st.FileHandle) {
                st.FileHandle->Close();
                delete st.FileHandle;
                st.FileHandle = nullptr;
            }

        } else {

            --st.FilePending;
            st.Odd = 2;

            if (st.FilePendingSize != StreamBufferSize) {
                if (st.FileHandle) {
                    st.FileHandle->Close();
                    delete st.FileHandle;
                    st.FileHandle = nullptr;
                }
            }

            st.QueueBuffer = static_cast<unsigned char *>(st.FileBuffer) + StreamBufferSize;
            st.QueueSize = st.FilePending > 0 ? StreamBufferSize : st.FilePendingSize;

        } 
    }

    Unlock_Secondary_Mutex(handle);
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Sample_Copy(SampleTrackerType* st, void **source, int *src_size, void **alternate, int *alt_size, void* dest, int dst_size, int scomp)
{
    ASSERT(st != nullptr);

    int datasize = 0;

    if (!st) {
        return 0;
    }

    if (scomp == SCOMP_NONE) {
        datasize = Simple_Copy(source, src_size, alternate, alt_size, &dest, dst_size);
        return datasize;
    }

    while (dst_size > 0) {

        unsigned short frame_size;
        unsigned short dcomp_size;
        unsigned magic_number;

        void *frame_size_ptr = &frame_size;
        void *dcomp_size_ptr = &dcomp_size;
        void *magic_number_ptr = &magic_number;

        if (Simple_Copy(source, src_size, alternate, alt_size, &frame_size_ptr, sizeof(frame_size)) < sizeof(frame_size)) {
            break;
        }

        if (Simple_Copy(source, src_size, alternate, alt_size, &dcomp_size_ptr, sizeof(dcomp_size)) < sizeof(dcomp_size) || dcomp_size > dst_size) {
            break;
        }

        if (Simple_Copy(source, src_size, alternate, alt_size, &magic_number_ptr, sizeof(magic_number)) < sizeof(magic_number) || magic_number != MagicNumber) {
            break;
        }

        if (frame_size == dcomp_size) {
            if (Simple_Copy(source, src_size, alternate, alt_size, &dest, frame_size) < dcomp_size) {
                break;
            }

        } else {

            void *uptr = UncompBuffer;
            if (Simple_Copy(source, src_size, alternate, alt_size, &uptr, frame_size) < frame_size) {
                break;
            }

            ASSERT(UncompBuffer != nullptr);
            ASSERT(dest != nullptr);

            switch (scomp) {

                case SCOMP_WESTWOOD:
                    //Westwood_UnZap(UncompBuffer, dest, dcomp_size);
                    break;

                case SCOMP_SOS:
                {
                    _tagCOMPRESS_INFO2 *sosinfo = &st->sSOSInfo;

                    sosinfo->lpSource = (BYTE *)UncompBuffer;
                    sosinfo->lpDest = (BYTE *)dest;

                    sosCODEC2DecompressData(sosinfo, dcomp_size);

                    break;
                }

                default:
                    DEBUG_WARNING("Unsupported compression format!\n");
                    break;

            };

            dest = static_cast<unsigned char *>(dest) + dcomp_size;
        }

        datasize += dcomp_size;
        dst_size -= dcomp_size;
    }

    return datasize;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Simple_Copy(void **source, int *src_size, void **alternate, int *alt_size, void **dest, int dst_size)
{
    int out = 0;

    if (*src_size == 0) {
        *source = *alternate;
        *src_size = *alt_size;
        *alternate = nullptr;
        *alt_size = 0;
    }

    if (*source == nullptr || *src_size == 0) {
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
    out = Simple_Copy(source, src_size, alternate, alt_size, dest, (dst_size - s)) + s;

    return out;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Attempt_Audio_Restore(LPDIRECTSOUNDBUFFER buffer)
{
    if (AudioDone || !SoundObject || !buffer) {
        return false;
    }

    if (AudioFocusLossFunction) {
        AudioFocusLossFunction();
    }

    HRESULT dsresult;

    for (int attempts = 0; attempts < 2; ++attempts) {
        Restore_Sound_Buffers();
        DWORD play_status;
        dsresult = buffer->GetStatus(&play_status);
        if (dsresult != DSERR_BUFFERLOST) {
            return true;
        }
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
int DirectSound8AudioClass::Attempt_To_Play_Buffer(int handle)
{
    HRESULT dsresult;

    SampleTrackerType &st = *SampleTracker[handle];

    do {
        dsresult = st.PlayBuffer->Play(0, 0, DSBPLAY_LOOPING);

        if (dsresult == DS_OK) {
            st.Active = true;
            //st.DontTouch = false;
            st.Handle = handle;
            return st.Handle;
        }

        if (dsresult != DSERR_BUFFERLOST) {
            st.Active = false;
            break;
        }

        if (!Attempt_Audio_Restore(st.PlayBuffer)) {
            break;
        }

    } while (dsresult == DSERR_BUFFERLOST);

    return INVALID_AUDIO_HANDLE;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::Set_Primary_Buffer_Format()
{
    if (!SoundObject || !PrimaryBufferPtr) {
        return false;
    }

    return PrimaryBufferPtr->SetFormat(PrimaryBufferFormat) == DS_OK;
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Restore_Sound_Buffers()
{
    Lock_Global_Mutex();

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        Lock_Secondary_Mutex(index);
    }

    if (PrimaryBufferPtr) {
        PrimaryBufferPtr->Restore();
    }

    for (int index = 0; index < SampleTracker.Count(); ++index) {
        SampleTrackerType &st = *SampleTracker[index];
        if (st.PlayBuffer) {
            st.PlayBuffer->Restore();
            Unlock_Secondary_Mutex(index);
        }
    }

    Unlock_Global_Mutex();
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
bool DirectSound8AudioClass::File_Callback(short handle, short *odd, void **buffer, int *size)
{
    if (handle == INVALID_AUDIO_HANDLE) {
        return false;
    }

    DirectSound8Audio.Lock_Secondary_Mutex(handle);

    SampleTrackerType &st = *DirectSound8Audio.SampleTracker[handle];

    if (!st.Active) {
        return false;
    }

    if (!st.FileBuffer) {
        return false;
    }

    if (!*buffer && st.FilePending) {

        *buffer = static_cast<unsigned char *>(st.FileBuffer)
            + DirectSound8Audio.StreamBufferSize * (*odd % STREAM_BUFFER_COUNT);

        --st.FilePending;

        ++*odd;

        *size = st.FilePending == 0 ? st.FilePendingSize : DirectSound8Audio.StreamBufferSize;
    }

    if (st.FilePending < (STREAM_BUFFER_COUNT/2) && st.FileHandle) {

        if (STREAM_BUFFER_COUNT-2 != st.FilePending) {

            /**
             *  Fill empty buffers.
             */
            for (int num_empty_buffers = (STREAM_BUFFER_COUNT-2) - st.FilePending;
                num_empty_buffers && st.FileHandle; --num_empty_buffers) {

                void *tofill = static_cast<unsigned char *>(st.FileBuffer)
                    + DirectSound8Audio.StreamBufferSize * ((st.FilePending + *odd) % STREAM_BUFFER_COUNT);

                int psize = st.FileHandle->Read(tofill, DirectSound8Audio.StreamBufferSize);

                if (psize != DirectSound8Audio.StreamBufferSize) {
                    st.FileHandle->Close();
                    delete st.FileHandle;
                    st.FileHandle = nullptr;
                }

                if (psize > 0) {
                    st.FilePendingSize = psize;
                    ++st.FilePending;
                }
            }
        }

        if (!st.QueueBuffer && st.FilePending) {

            st.QueueBuffer = static_cast<unsigned char *>(st.FileBuffer)
                + DirectSound8Audio.StreamBufferSize * (st.Odd % STREAM_BUFFER_COUNT);

            --st.FilePending;
            ++st.Odd;
            st.QueueSize = st.FilePending > 0 ? DirectSound8Audio.StreamBufferSize : st.FilePendingSize;
        }
    }

    DirectSound8Audio.Unlock_Secondary_Mutex(handle);

    if (st.FilePending) {
        return true;
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Sound_Maintenance_Callback()
{
    HRESULT ret;
    DWORD play_cursor;
    DWORD write_cursor;

    for (int index = 0; index < SampleTracker.Count(); ++index) {

        DirectSound8Audio.Lock_Secondary_Mutex(index);

        SampleTrackerType &st = *DirectSound8Audio.SampleTracker[index];

        if (!st.Active) {
            continue;
        }

        if (st.Service) {

            DWORD dest_ptr = (DWORD)st.DestPtr;

            ret = st.PlayBuffer->GetCurrentPosition(&play_cursor, &write_cursor);

            if (ret != S_OK) {
                if (ret == DSERR_BUFFERLOST) {
                    if (DirectSound8Audio.AudioFocusLossFunction) {
                        DirectSound8Audio.AudioFocusLossFunction();
                    }
                }
                DirectSound8Audio.Unlock_Secondary_Mutex(index);
                DEBUG_ERROR("DirectSound8Audio[%d]: GetCurrentPosition failed!\n", index);
                return;
            }

            if (!st.MoreSource) {

                if ((dest_ptr <= play_cursor && (play_cursor - dest_ptr) < 8192)
                 || (!st.OneShot && play_cursor < dest_ptr && 24576 < (play_cursor - dest_ptr))) {

                    st.PlayBuffer->Stop();
                    st.Service = false;
                    DirectSound8Audio.Stop_Sample(index);
                }

            } else {

                void *play_buffer_ptr;
                void *wrapped_buffer_ptr;
                DWORD play_lock_len;
                DWORD wrapped_lock_len;
                
                if (play_cursor < dest_ptr || (24576 < play_cursor) && !dest_ptr) {
                    if ((dest_ptr - play_cursor) < 8193) {
                        
                        ret = st.PlayBuffer->Lock(dest_ptr,
                                                2 * SECONDARY_BUFFER_SIZE,
                                                &play_buffer_ptr,
                                                &play_lock_len,
                                                &wrapped_buffer_ptr,
                                                &wrapped_lock_len,
                                                0);

                        if (ret == S_OK) {

                            int bytes_copied = DirectSound8Audio.Sample_Copy(&st,
                                                                &st.Source,
                                                                &st.Remainder,
                                                                &st.QueueBuffer,
                                                                &st.QueueSize,
                                                                play_buffer_ptr,
                                                                SECONDARY_BUFFER_SIZE,
                                                                st.Compression);

                            // We didn't have enough data left to finish filling the chunk so fill with silence.
                            if (bytes_copied != SECONDARY_BUFFER_SIZE) {
                                st.MoreSource = false;

                                if (st.BitSize != 0) {
                                    ZeroMemory(static_cast<unsigned char *>(play_buffer_ptr) + bytes_copied,
                                        SECONDARY_BUFFER_SIZE - bytes_copied);
                                } else {
                                    memset(static_cast<unsigned char *>(play_buffer_ptr) + bytes_copied,
                                        0x80,
                                        SECONDARY_BUFFER_SIZE - bytes_copied);
                                }
                            }

                            // This block silences the next block, ensures we don't play random garbage with buffer underruns.
                            if (dest_ptr == (SECONDARY_BUFFER_SIZE - SECONDARY_BUFFER_SIZE)) {
                                if (wrapped_buffer_ptr != nullptr && wrapped_lock_len != 0) {
                                    if (st.BitSize != 0) {
                                        ZeroMemory(wrapped_buffer_ptr, wrapped_lock_len);
                                    } else {
                                        memset(wrapped_buffer_ptr, 0x80, wrapped_lock_len);
                                    }
                                }
                            } else {
                                if (st.BitSize != 0) {
                                    ZeroMemory(static_cast<unsigned char *>(play_buffer_ptr) + SECONDARY_BUFFER_SIZE, SECONDARY_BUFFER_SIZE);
                                } else {
                                    memset(static_cast<unsigned char *>(play_buffer_ptr) + SECONDARY_BUFFER_SIZE, 0x80, SECONDARY_BUFFER_SIZE);
                                }
                            }

                            dest_ptr = (dest_ptr - bytes_copied);
                            st.DestPtr = (void *)dest_ptr;

                            /**
                             *  If we reached the end of the buffer, loop back around.
                             */
                            if (dest_ptr >= SECONDARY_BUFFER_SIZE) {
                                dest_ptr = (dest_ptr - SECONDARY_BUFFER_SIZE);
                                st.DestPtr = (void *)dest_ptr;
                            }

                            st.PlayBuffer->Unlock(play_buffer_ptr, play_lock_len, wrapped_buffer_ptr, wrapped_lock_len);
                        }
                    }
                }
            }
        }

        if (!st.QueueBuffer && st.FilePending != 0) {
            st.QueueBuffer = static_cast<unsigned char *>(st.FileBuffer)
                + DirectSound8Audio.StreamBufferSize * (st.Odd % STREAM_BUFFER_COUNT);
            --st.FilePending;
            ++st.Odd;

            if (st.FilePending != 0) {
                st.QueueSize = DirectSound8Audio.StreamBufferSize;
            } else {
                st.QueueSize = st.FilePendingSize;
            }
        }

        if (!st.Active) {
            continue;
        }

        if (st.Reducer && st.Volume) {
            if (st.Reducer >= st.Volume) {
                st.Volume = VOLUME_MIN;
            } else {
                st.Volume -= st.Reducer;
            }

            /*if (st.IsScore) {
                st.PlayBuffer->SetVolume(
                    Convert_HMI_To_Direct_Sound_Volume(DirectSound8Audio.ScoreVolume * (st.Volume / 128)) / 256);
            } else {*/
                st.PlayBuffer->SetVolume(
                    Convert_HMI_To_Direct_Sound_Volume(DirectSound8Audio.SoundVolume * (st.Volume / 128)) / 256);
            //}
        }

        DirectSound8Audio.Unlock_Secondary_Mutex(index);
    }
}


////////////////////////////////////////////////////////////////////////////////
// DONE
////////////////////////////////////////////////////////////////////////////////
void DirectSound8AudioClass::Sound_Timer_Callback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    //DEBUG_INFO("In Sound_Timer_Callback()\n");

    if (!DirectSound8Audio.AudioDone) {
        if (WaitForSingleObject(DirectSound8Audio.TimerMutex, 0) == WAIT_OBJECT_0) {
            DirectSound8Audio.Sound_Maintenance_Callback();
            ReleaseMutex(DirectSound8Audio.TimerMutex);
        }
    }
}










#if 0
// https://dev.ds-servers.com/yura9889/engine/-/blob/master/source/score/sound.h
enum AudioFormatType
{
    FORMAT_AUD,
    FORMAT_WAVE,
    FORMAT_OGG,
};

struct CSoundWaveHeader
{
    char	RiffSig[4];
    int32_t	ChunkSize;
    char	Sig[4];
    char	FormatSig[4];
    int32_t	FormatChunkSize;
    int16_t	FormatTag;
    int16_t	Channels;
    int32_t	SampleRate;
    int32_t	BytesPerSec;
    int16_t	BlockAlign;
    int16_t	BitsPerSample;
    char	DataSig[4];
    int32_t	DataSize;
};


int DirectSound8AudioClass::What_File_Format(const void* file)
{
    FILE *fp;

    if (!(fp = fopen(file, "rb")))
        return SOUND_FILEFORMAT_UNKNOWN;

    CSoundWaveHeader Hdr;
    fseek(fp, 0, SEEK_SET);
    fread(&Hdr, 1, sizeof(CSoundWaveHeader), fp);

    if (memcmp(Hdr.RiffSig, "RIFF", 4) == 0 || memcmp(Hdr.Sig, "WAVE", 4) == 0 || memcmp(Hdr.FormatSig, "fmt ", 4) == 0) {
        fclose(fp);
        return FORMAT_WAVE;
    }

    if (strcmp(fp->_base, "OggS") == 0) {
        fclose(fp);
        return FORMAT_OGG;
    }

    fclose(fp);

    return FORMAT_AUD;

}
#endif
