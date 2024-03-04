/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DSAUDIOEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended audio class.
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
#include "dsaudioext_hooks.h"
#include "tibsun_globals.h"
#include "dsaudio.h"
#include "wwaud.h"
#include "soscodec.h"
#include "language.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or destructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
class DirectSoundAudioClassExt final : public DirectSoundAudioClass
{
    public:
        bool Init(HWND window, int bits_per_sample, bool stereo, int rate);
        int Play_Sample_Handle(void const *sample, int priority, int volume, int id);
};

/**
 *  
 */
//static DirectSoundAudioClassExt NewDSAudio;




#define LOCK_GLOBAL_MUTEX() \
    if (!Lock_Global_Mutex()) { \
        DEBUG_ERROR("Warning: Probable deadlock occurred on GlobalAudioMutex. %s, line %d\n", __FILE__, __LINE__); \
    } \

#define UNLOCK_GLOBAL_MUTEX() Unlock_Global_Mutex()


#define LOCK_SECONDARY_MUTEX(_handle) \
    if (WaitForSingleObject(SecondaryBufferMutexes[_handle], MUTEX_TIMEOUT) == WAIT_TIMEOUT) { \
        DEBUG_ERROR("Warning: Probable deadlock occurred on secondary buffer mutex %d. %s, line %d\n", _handle, __FILE__, __LINE__); \
    } \

#define UNLOCK_SECONDARY_MUTEX(_handle) ReleaseMutex(SecondaryBufferMutexes[_handle]);


#define LOCK_STREAMING_SECONDARY_MUTEX(_handle) \
    if (WaitForSingleObject(SecondaryBufferMutexes[_handle], MUTEX_TIMEOUT) == WAIT_TIMEOUT) { \
        DEBUG_ERROR("Warning: Probable deadlock occurred on streaming secondary buffer mutex %d. %s, line %d\n", _handle, __FILE__, __LINE__); \
    } \

#define UNLOCK_STREAMING_SECONDARY_MUTEX(_handle) ReleaseMutex(SecondaryBufferMutexes[_handle]);


#define _LOCK_SECONDARY_MUTEX(_handle) \
    if (WaitForSingleObject(NewDSAudio.SecondaryBufferMutexes[_handle], DirectSoundAudioClassExt::MUTEX_TIMEOUT) == WAIT_TIMEOUT) { \
        DEBUG_ERROR("Warning: Probable deadlock occurred on secondary buffer mutex %d. %s, line %d\n", _handle, __FILE__, __LINE__); \
    } \

#define _UNLOCK_SECONDARY_MUTEX(_handle) ReleaseMutex(NewDSAudio.SecondaryBufferMutexes[_handle]);


#define LOCK_ALL_MUTEX() \
    if (WaitForMultipleObjects(MUTEX_COUNT, AllAudioMutexes, true, DirectSoundAudioClassExt::MUTEX_TIMEOUT) == WAIT_TIMEOUT) { \
        DEBUG_ERROR("Warning: Probable deadlock occurred on multiple audio mutexes. %s, line %d\n", __FILE__, __LINE__); \
    } \



bool DirectSoundAudioClassExt::Init(HWND window, int bits_per_sample, bool stereo, int rate)
{
    int index;
    int sample=1;
    short old_bits_per_sample;
    short old_block_align;
    long old_bytes_per_sec;
    DWORD old_channels;
    HRESULT res;

    DSBUFFERDESC BufferDesc;
    WAVEFORMATEX DsBuffFormat;

    if (!SoundObject) {

        LOCK_GLOBAL_MUTEX();

        res = DirectSoundCreate (nullptr,&SoundObject,nullptr);
        if (res != DS_OK) {
            DEBUG_ERROR("Failed to create direct sound object. Error code %d\n", res);
            Print_Sound_Error(Text_String(TXT_DSOUND_CANT_CREATE), window);
            UNLOCK_GLOBAL_MUTEX();
            return false;
        }

        res = SoundObject->SetCooperativeLevel( window, DSSCL_PRIORITY);
        if (res != DS_OK) {
            DEBUG_ERROR("Failed to set cooperative level. Error code %d\n", res);
            Print_Sound_Error(Text_String(TXT_DSOUND_NO_COOP), window);
            SoundObject->Release();
            SoundObject = nullptr;
            UNLOCK_GLOBAL_MUTEX();
            return false;
        }

        memset (&BufferDesc , 0 , sizeof(DSBUFFERDESC));
        BufferDesc.dwSize=sizeof(DSBUFFERDESC);
        BufferDesc.dwFlags=DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;

        memset (&DsBuffFormat , 0 , sizeof(WAVEFORMATEX));
        DsBuffFormat.wFormatTag = WAVE_FORMAT_PCM;
        DsBuffFormat.nChannels = (unsigned short) (1 + stereo);
        DsBuffFormat.nSamplesPerSec = rate;
        DsBuffFormat.wBitsPerSample = (short) bits_per_sample;
        DsBuffFormat.nBlockAlign = (unsigned short)( (DsBuffFormat.wBitsPerSample/8) * DsBuffFormat.nChannels);
        DsBuffFormat.nAvgBytesPerSec= DsBuffFormat.nSamplesPerSec * DsBuffFormat.nBlockAlign;
        DsBuffFormat.cbSize = 0;

        memcpy (PrimaryBufferDesc , &BufferDesc , sizeof(DSBUFFERDESC));
        memcpy (PrimaryBuffFormat , &DsBuffFormat , sizeof(WAVEFORMATEX));

        res = SoundObject->CreateSoundBuffer(PrimaryBufferDesc, &PrimaryBufferPtr, nullptr);
        if (res != DS_OK) {
            DEBUG_ERROR("Failed to create the primary sound buffer. Error code %d\n", res);
            Print_Sound_Error(Text_String(TXT_DSOUND_NO_PRIMARY), window);
            SoundObject->Release();
            SoundObject = nullptr;
            UNLOCK_GLOBAL_MUTEX();
            return false;
        }

        //TODO, a bug seems to be introduced in TS here this seems wrong
        old_channels = DsBuffFormat.nChannels;
        old_bits_per_sample = DsBuffFormat.wBitsPerSample;
        old_block_align = DsBuffFormat.nBlockAlign;
        old_bytes_per_sec = DsBuffFormat.nAvgBytesPerSec;

        if (!Set_Primary_Buffer_Format()) {
            
            DEBUG_ERROR("Failed to set primary buffer format\n");

            //TODO, a bug seems to be introduced in TS here this seems wrong
            if (DsBuffFormat.wBitsPerSample == 16 || stereo == true) {

                DEBUG_ERROR("Trying an 8 bit primary buffer format\n");
                
                //TODO, a bug seems to be introduced in TS here this seems wrong
                //old_bits_per_sample = DsBuffFormat.wBitsPerSample;
                //old_block_align = DsBuffFormat.nBlockAlign;
                //old_bytes_per_sec = DsBuffFormat.nAvgBytesPerSec;

                DsBuffFormat.wBitsPerSample = 8;
                DsBuffFormat.nBlockAlign = (unsigned short)( (DsBuffFormat.wBitsPerSample/8) * DsBuffFormat.nChannels);
                DsBuffFormat.nAvgBytesPerSec= DsBuffFormat.nSamplesPerSec * DsBuffFormat.nBlockAlign;
                
                memcpy (PrimaryBufferDesc , &BufferDesc , sizeof(DSBUFFERDESC));
                memcpy (PrimaryBuffFormat , &DsBuffFormat , sizeof(WAVEFORMATEX));

                if (!Set_Primary_Buffer_Format()){
                    
                    DEBUG_ERROR("Failed to set primary buffer format\n");

                    if (stereo == true) {

                        DEBUG_ERROR("Trying a mono primary buffer format\n");

                        DsBuffFormat.nBlockAlign = DsBuffFormat.wBitsPerSample >> 3;
                        DsBuffFormat.nAvgBytesPerSec = DsBuffFormat.nSamplesPerSec * DsBuffFormat.nBlockAlign;
                        DsBuffFormat.nChannels = 1;
                        
                        *PrimaryBufferDesc = BufferDesc;
                        *PrimaryBuffFormat = DsBuffFormat;
                    }

                    if (!Set_Primary_Buffer_Format()) {
                        DEBUG_ERROR("Failed to set any primary buffer format. Disabling audio.\n");
                        PrimaryBufferPtr->Release();
                        PrimaryBufferPtr = nullptr;
                        SoundObject->Release();
                        SoundObject = nullptr;
                        Print_Sound_Error(Text_String(TXT_DSOUND_INCOMPAT), window);
                        UNLOCK_GLOBAL_MUTEX();
                        return false;
                    }
                }
            }
        }
        //TODO, a bug seems to be introduced in TS here this seems wrong

        DsBuffFormat.nChannels = old_channels;
        DsBuffFormat.wBitsPerSample = old_bits_per_sample;
        DsBuffFormat.nBlockAlign = old_block_align;
        DsBuffFormat.nAvgBytesPerSec = old_bytes_per_sec;

        res = PrimaryBufferPtr->Play(0,0,DSBPLAY_LOOPING);
        if (res != DS_OK) {
            DEBUG_ERROR("Failed to start primary sound buffer. Error code %d\n", 0);
            Print_Sound_Error(Text_String(TXT_DSOUND_NO_PRIMARY), window);
            PrimaryBufferPtr->Release();
            PrimaryBufferPtr = nullptr;
            SoundObject->Release();
            SoundObject = nullptr;
            UNLOCK_GLOBAL_MUTEX();
            return false;
        }

        TIMECAPS tc;
        if (timeGetDevCaps(&tc, sizeof(tc)) != TIMERR_NOERROR) {
            DEBUG_ERROR("Error - Failed to obtain timer resolution caps\n");
            TimerResolution = WORST_TIMER_RESOLUTION;
        } else {
            TimerResolution = std::min(std::max(tc.wPeriodMin, (unsigned int)TARGET_TIMER_RESOLUTION), tc.wPeriodMax);
        }

        DEBUG_INFO("Audio timer resolution is %d milliseconds\n", TimerResolution);
        
        timeBeginPeriod(TimerResolution);
        SoundTimerHandle = timeSetEvent ( 1000/MAINTENANCE_RATE , 1 , Sound_Timer_Callback , 0 , TIME_PERIODIC);
        AudioDone = FALSE;

        BufferDesc.dwFlags=DSBCAPS_CTRLVOLUME;
        BufferDesc.dwBufferBytes=SECONDARY_BUFFER_SIZE;
        BufferDesc.lpwfxFormat = (LPWAVEFORMATEX) &DsBuffFormat;

        for (index = 0; index < MAX_SFX; index++) {
            SampleTrackerType *st = &SampleTracker[index];
            SoundObject->CreateSoundBuffer (&BufferDesc , &st->PlayBuffer , nullptr);
            st->PlaybackRate = rate;
            st->Stereo = (stereo) ? AUD_FLAG_STEREO : 0;
            st->BitSize = (bits_per_sample == 16) ? AUD_FLAG_16BIT : 0;
            st->FileHandle = nullptr;
            st->QueueBuffer = nullptr;
            st->FileBuffer = nullptr;
        }

        UNLOCK_GLOBAL_MUTEX();
    }

    return true;
}


int DirectSoundAudioClassExt::Play_Sample_Handle(void const *sample, int priority, int volume, int id)
{
    AUDHeaderType RawHeader;
    SampleTrackerType *st = nullptr;
    LPVOID play_buffer_ptr;
    LPVOID dummy_buffer_ptr;
    DWORD lock_length1;
    DWORD lock_length2;
    DWORD play_status;
    HRESULT return_code;
    int retries=0;

    DSBUFFERDESC BufferDesc;

    if (id == INVALID_AUDIO_HANDLE || !SoundObject || AudioDone || !sample) {
        return INVALID_AUDIO_HANDLE;
    }

    LOCK_SECONDARY_MUTEX(id);

    bool unk = false;
    st = &SampleTracker[id];

    if (st->Original == sample && st->OneShot && st->PlayBuffer != nullptr) {
        unk = true;
    }

    memcpy((void *)&RawHeader, (void *)sample, sizeof(RawHeader));

    if (RawHeader.Rate <24000 && RawHeader.Rate >20000) RawHeader.Rate = 22050;

    st->Compression = (SCompressType) ((unsigned char)RawHeader.Compression);
    st->Original = sample;
    //st->OriginalSize = RawHeader.Size + sizeof(RawHeader);
    st->Priority = (short)priority;
//    st->DontTouch = TRUE;
    st->Odd = 0;
    st->Reducer = 0;
//    st->Restart = FALSE;
    st->QueueBuffer = nullptr;
    st->QueueSize = 0;
    st->Remainder = RawHeader.Size;
    st->Source = Audio_Add_Long_To_Pointer((void *)sample, sizeof(RawHeader));
    st->Service = FALSE;

    if (st->Compression == SCOMP_SOS) {
        st->sSOSInfo.wChannels = (RawHeader.Flags & AUD_FLAG_STEREO) ? 2  : 1;
        st->sSOSInfo.wBitSize = (RawHeader.Flags & AUD_FLAG_16BIT)  ? 16 : 8;
        st->sSOSInfo.dwCompSize = RawHeader.Size;
        st->sSOSInfo.dwUnCompSize = RawHeader.Size * ( st->sSOSInfo.wBitSize / 4 );
        if (st->sSOSInfo.wBitSize == 16 && st->sSOSInfo.wChannels == 1) {
            sosCODECInitStream(&st->sSOSInfo);
        } else {
            General_sosCODECInitStream(&st->sSOSInfo);
        }
    }

    if (!unk) {

        if (!st->PlayBuffer || ( RawHeader.Rate != st->PlaybackRate ) ||
            ( ( RawHeader.Flags & AUD_FLAG_16BIT ) != ( st->BitSize & AUD_FLAG_16BIT ) ) ||
            ( ( RawHeader.Flags & AUD_FLAG_STEREO) != ( st->Stereo & AUD_FLAG_STEREO ) ) ) {
            DEBUG_ERROR("DSAudio [%d]: Changing sample format\n", id);

            st->Active=true;
            st->Service=false;
            st->MoreSource=false;

            if (st->PlayBuffer) {
                do {
                    return_code = st->PlayBuffer->GetStatus ( &play_status );

                    if (return_code==DSERR_BUFFERLOST){
                        if (!Attempt_Audio_Restore(st->PlayBuffer)) {
                            ReleaseMutex(SecondaryBufferMutexes[id]);
                            return INVALID_AUDIO_HANDLE;
                        }
                    }
                }while (return_code == DSERR_BUFFERLOST);

                if (play_status & (DSBSTATUS_PLAYING | DSBSTATUS_LOOPING) ){
                    return_code = st->PlayBuffer->Stop();
                    if (return_code==DSERR_BUFFERLOST){
                        if (!Attempt_Audio_Restore(st->PlayBuffer)) {
                            ReleaseMutex(SecondaryBufferMutexes[id]);
                            return INVALID_AUDIO_HANDLE;
                        }
                    }
                }

                st->PlayBuffer->Release();
                st->PlayBuffer=nullptr;
            }

            //Removed from TS
            /*
            DsBuffFormat.nSamplesPerSec    = (unsigned short int) RawHeader.Rate;
            DsBuffFormat.nChannels            = (RawHeader.Flags & AUD_FLAG_STEREO) ? 2 : 1 ;
            DsBuffFormat.wBitsPerSample    = (RawHeader.Flags & AUD_FLAG_16BIT) ? 16 : 8 ;
            DsBuffFormat.nBlockAlign    = (short) ((DsBuffFormat.wBitsPerSample/8) * DsBuffFormat.nChannels);
            DsBuffFormat.nAvgBytesPerSec= DsBuffFormat.nSamplesPerSec * DsBuffFormat.nBlockAlign;
            */

            do {
                return_code= SoundObject->CreateSoundBuffer (&BufferDesc , &st->PlayBuffer , nullptr);
                if (return_code==DSERR_BUFFERLOST){
                    if (!Attempt_Audio_Restore(st->PlayBuffer)) {
                        ReleaseMutex(SecondaryBufferMutexes[id]);
                        return INVALID_AUDIO_HANDLE;
                    }
                }
            } while (return_code == DSERR_BUFFERLOST);

            if (return_code!=DS_OK && return_code!=DSERR_BUFFERLOST){
                DEBUG_ERROR("DSAudio [%d]: Bad sample format!\n", id);
                st->PlaybackRate = 0;
                st->Stereo = 0;
                st->BitSize = 0;
                if (st->FileHandle != nullptr) {
                    st->FileHandle->Close();
                    delete st->FileHandle;
                    st->FileHandle = nullptr;
                }
                if (st->FileBuffer != nullptr) {
                
                    if (st->FileBuffer != FileStreamBuffer) {
                        delete st->FileBuffer;
                    } else {
                        field_380 = false;
                    }

                    st->FileBuffer = nullptr;
                }

                st->Loading = false;
                st->Priority = 0;
                st->FilePending = 0;
                st->FilePendingSize = 0;
                st->QueueBuffer = nullptr;
                st->Callback = nullptr;
                ReleaseMutex(SecondaryBufferMutexes[id]);
                return INVALID_AUDIO_HANDLE;
            }

            st->PlaybackRate = RawHeader.Rate;
            st->Stereo = RawHeader.Flags & AUD_FLAG_STEREO;
            st->BitSize = RawHeader.Flags & AUD_FLAG_16BIT;
        }
    }

    do {
        return_code = st->PlayBuffer->GetStatus ( &play_status );
        if (return_code==DSERR_BUFFERLOST){
            if (!Attempt_Audio_Restore(st->PlayBuffer)) {
                ReleaseMutex(SecondaryBufferMutexes[id]);
                return INVALID_AUDIO_HANDLE;
            }
        }
    } while (return_code==DSERR_BUFFERLOST);

    if (play_status & (DSBSTATUS_PLAYING | DSBSTATUS_LOOPING) ){
        st->Active=false;
        st->Service=false;
        st->MoreSource=false;
        st->PlayBuffer->Stop();
    }
    
    audio_6856E0((char *)st->Source, st->Remainder);

    if (unk){
        st->Remainder = false;
        st->MoreSource = false;
        st->OneShot = true;
        st->Service = true;
    } else {

        do {
            return_code = st->PlayBuffer->Lock ( 0 ,
                                            SECONDARY_BUFFER_SIZE,
                                            &play_buffer_ptr,
                                            &lock_length1,
                                            &dummy_buffer_ptr,
                                            &lock_length2,
                                            0 );
            if (return_code==DSERR_BUFFERLOST) {
                if (!Attempt_Audio_Restore(st->PlayBuffer)) {
                    ReleaseMutex(SecondaryBufferMutexes[id]);
                    return INVALID_AUDIO_HANDLE;
                }
            }
        } while (return_code==DSERR_BUFFERLOST);

        if (return_code != DS_OK) {
            ReleaseMutex(SecondaryBufferMutexes[id]);
            return INVALID_AUDIO_HANDLE;
        }

        int size = SECONDARY_BUFFER_SIZE*1/4;
        if (RawHeader.UncompSize < SECONDARY_BUFFER_SIZE*3/4) {
            size = RawHeader.UncompSize;
        }

        st->DestPtr=(void*)Sample_Copy (     st,
                                    &st->Source,
                                    (long *)&st->Remainder,
                                    &st->QueueBuffer,
                                    (long *)&st->QueueSize,
                                    play_buffer_ptr,
                                    size,
                                    (SCompressType)st->Compression);

        if ((long)st->Remainder > 0) {
            st->MoreSource=TRUE;
            st->Service=TRUE;
            st->OneShot=FALSE;

        } else {

            st->MoreSource=FALSE;
            st->OneShot=TRUE;
            st->Service=TRUE;

            int left = SECONDARY_BUFFER_SIZE - size;
            if (SECONDARY_BUFFER_SIZE - size > SECONDARY_BUFFER_SIZE/4) {
                left = SECONDARY_BUFFER_SIZE/4;
            }
            memset ( (char*)( (unsigned)play_buffer_ptr + (unsigned)st->DestPtr ), 0 , left);
        }
        
        st->PlayBuffer->Unlock(    play_buffer_ptr,
                                lock_length1,
                                dummy_buffer_ptr,
                                lock_length2);
    }

    st->PlayBuffer->SetVolume ( Convert_HMI_To_Direct_Sound_Volume( ( SoundVolume*volume)/255) );
    st->Volume = volume;
    st->DSVolume = volume << 7;

    if (!Start_Primary_Sound_Buffer(FALSE)){
        ReleaseMutex(SecondaryBufferMutexes[id]);
        return INVALID_AUDIO_HANDLE;
    }

    do {
        return_code = st->PlayBuffer->SetCurrentPosition (0);
        if (return_code==DSERR_BUFFERLOST){
            if (!Attempt_Audio_Restore(st->PlayBuffer)){
                ReleaseMutex(SecondaryBufferMutexes[id]);
                return INVALID_AUDIO_HANDLE;
            }
        }
    } while (return_code==DSERR_BUFFERLOST);

    do
    {
        return_code = st->PlayBuffer->Play (0,0,DSBPLAY_LOOPING);

        switch (return_code){

            case DS_OK :
                st->Active=TRUE;
                st->Handle=(int)id;
                ReleaseMutex(SecondaryBufferMutexes[id]);
                return st->Handle;

            case DSERR_BUFFERLOST :
                if (!Attempt_Audio_Restore(st->PlayBuffer)) { 
                    ReleaseMutex(SecondaryBufferMutexes[id]);
                    return INVALID_AUDIO_HANDLE;
                }
                break;

            default:
                st->Active=FALSE;
                ReleaseMutex(SecondaryBufferMutexes[id]);
                return INVALID_AUDIO_HANDLE;
        }
    } while (return_code==DSERR_BUFFERLOST);

    ReleaseMutex(SecondaryBufferMutexes[id]);
    return st->Handle;
}


/**
 *  Main function for patching the hooks.
 */
void DirectSoundAudioClassExtExtension_Hooks()
{
    Patch_Jump(0x00487C50, &DirectSoundAudioClassExt::Init);
    Patch_Jump(0x00488990, &DirectSoundAudioClassExt::Play_Sample_Handle);
}
