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
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  was 0x8000, now double the size.
 */
#define AudioBufferSize (1024*32)//((1024*32)*2)


/**
 *  Instances of the format and buffer descriptions for the trackers.
 */
static WAVEFORMATEX SampleTrackerFormats[5];
static DSBUFFERDESC SampleTrackerDescs[5];


/**
 *  
 * 
 *  
 * 
 *  
 */
DECLARE_PATCH(_DirectSoundAudioClass_Play_Sample_Handle_Patch)
{
    GET_REGISTER_STATIC(DirectSoundAudioClass *, this_ptr, ebp);
    GET_REGISTER_STATIC(SampleTrackerType *, tracker, esi);
    LEA_STACK_STATIC(AUDHeaderType *, raw_header, esp, 0x24);
    GET_STACK_STATIC(int, handle, esp, 0x58);
    static HRESULT res;
    static WAVEFORMATEX *fmt;
    static DSBUFFERDESC *dsc;

    DEV_DEBUG_INFO("DSAudio [%d]: New format:\n", handle);
    DEV_DEBUG_INFO("  Rate: %d\n", raw_header->Rate);
    DEV_DEBUG_INFO("  BitsPerSample: %d\n", (raw_header->Flags & 2) ? 16 : 8);
    DEV_DEBUG_INFO("  Channels: %s\n", (raw_header->Flags & 1) ? "stereo" : "mono");

    fmt = &SampleTrackerFormats[handle];
    std::memcpy(fmt, this_ptr->PrimaryBuffFormat, sizeof(WAVEFORMATEX));

    if ((raw_header->Flags & 1)) {
        fmt->nChannels = 2;
    } else {
        fmt->nChannels = 1;
    }

    if ((raw_header->Flags & 2)) {
        fmt->wBitsPerSample = 16;
    } else {
        fmt->wBitsPerSample = 8;
    }

    fmt->nSamplesPerSec = raw_header->Rate;
    fmt->nBlockAlign = (fmt->nChannels * fmt->wBitsPerSample) / 8;
    fmt->nAvgBytesPerSec = fmt->nBlockAlign * fmt->nSamplesPerSec;
    
    dsc = &SampleTrackerDescs[handle];
    std::memcpy(dsc, this_ptr->PrimaryBufferDesc, sizeof(DSBUFFERDESC));
    dsc->lpwfxFormat = &SampleTrackerFormats[handle];
    dsc->dwFlags = DSBCAPS_CTRLVOLUME;
    dsc->dwBufferBytes = AudioBufferSize;

    if (tracker->PlayBuffer) {
        tracker->PlayBuffer->Release();
        tracker->PlayBuffer = nullptr;
    }

    res = this_ptr->SoundObject->CreateSoundBuffer(dsc, &tracker->PlayBuffer, nullptr);

    __asm { xor edi, edi }
    __asm { mov eax, res }

    JMP_REG(ecx, 0x00488C4E);
}


DECLARE_PATCH(_DirectSoundAudioClass_Play_Sample_Handle_Patch_2)
{
    GET_REGISTER_STATIC(SampleTrackerType *, tracker, esi);
    GET_STACK_STATIC(void *, play_buffer_ptr, esp, 0x14);
    GET_REGISTER_STATIC(void *, dest, edx);

    tracker->DestPtr = dest;

    if (tracker->DestPtr == (void *)AudioBufferSize) {
        tracker->MoreSource = true;
        tracker->Service = true;
        tracker->OneShot = false;
    } else {
        tracker->MoreSource = false;
        tracker->OneShot = true;
        tracker->Service = true;
        std::memset(static_cast<unsigned char *>(play_buffer_ptr) + (unsigned)tracker->DestPtr, 0, AudioBufferSize);
    }

    JMP(0x00488F1C);
}


/**
 *  Main function for patching the hooks.
 */
void DSAudioExtension_Hooks()
{
    //Patch_Jump(0x006013B0, &_WinMain_Init_Audio_1_Patch);
    //Patch_Jump(0x0060169B, &_WinMain_Init_Audio_2_Patch);


    //Patch_Jump(0x00488360, &_DirectSoundAudioClass_Init_Patch);
    Patch_Jump(0x00488C36, &_DirectSoundAudioClass_Play_Sample_Handle_Patch);
    //Patch_Jump(0x00488ECE, &_DirectSoundAudioClass_Play_Sample_Handle_Patch_2);

    //Patch_Jump(0x00488990, DSAudioClassFake::_Play_Sample_Handle);


     // remove clamp
    //Patch_Byte_Range(0x00488A34, 0x90, 4);
    //Patch_Jump(0x00488A4A, 0x00488A59);


#if 1
    Patch_Dword(0x00488E9C + 1, (AudioBufferSize/4));
    Patch_Dword(0x00488EF4 + 1, (AudioBufferSize/4));
    Patch_Dword(0x00488EFF + 1, (AudioBufferSize/4));
    Patch_Dword(0x00489344 + 2, (AudioBufferSize/4));
    Patch_Dword(0x0048939E + 1, (AudioBufferSize/4));
    Patch_Dword(0x004893BB + 2, (AudioBufferSize/4));
    Patch_Dword(0x004893C3 + 1, (AudioBufferSize/4));
    Patch_Dword(0x00489414 + 1, (AudioBufferSize/4)/4);
    Patch_Dword(0x00489419 + 2, (AudioBufferSize/4));
    Patch_Dword(0x00489466 + 2, (AudioBufferSize/4));

    Patch_Dword(0x0048830A + 4, AudioBufferSize);
    Patch_Dword(0x00488E27 + 1, AudioBufferSize);
    Patch_Dword(0x00488EE5 + 1, AudioBufferSize);
    Patch_Dword(0x00489429 + 1, AudioBufferSize);

    Patch_Dword(0x00489432 + 1, -AudioBufferSize);

    Patch_Dword(0x00487A29 + 1, ((512*4)+50)); // UncompBuffer
    Patch_Dword(0x00487A47 + 3, ((AudioBufferSize/2)+128)); // StreamBufferSize
    Patch_Dword(0x00487A3F + 1, (((AudioBufferSize/2)+128)*16)); // FileStreamBuffer

    Patch_Dword(0x00489380 + 1, (AudioBufferSize/2));

    Patch_Dword(0x00488EA1 + 1, ((AudioBufferSize/4)*3));
    Patch_Dword(0x00489352 + 1, ((AudioBufferSize/4)*3));
    Patch_Dword(0x004893E4 + 2, ((AudioBufferSize/4)*3));
    Patch_Dword(0x0048947B + 1, ((AudioBufferSize/4)*3));




    Patch_Byte(0x004882C4, 0x52);
    Patch_Byte(0x004882C4 + 1, 0x90);
#endif
    //Patch_Byte(0x00488260 + 7, 10); // Sounds mix interval (was 25).
    //Patch_Byte(0x004882C4 + 1, 10); // Sounds mix interval (was 25).




#if 0
    Patch_Byte(0x004882C4, 0x52);
    Patch_Byte(0x004882C4 + 1, 0x90);

    Patch_Byte(0x00488270 + 2, 25);
    Patch_Byte(0x00488277 + 1, 25);
    Patch_Byte(0x00488284 + 2, 25);
    Patch_Byte(0x00488289 + 1, 25);
#endif
}



























#if 0




DECLARE_PATCH(_WinMain_Init_Audio_1_Patch)
{
    Audio.Init(MainWindow, 16, true, 48000);

    JMP(0x006013C8);
}

DECLARE_PATCH(_WinMain_Init_Audio_2_Patch)
{
    Audio.Init(MainWindow, 16, true, 48000);

    JMP(0x006016B3);
}



class DSAudioClassFake : public DirectSoundAudioClass
{
    public:
        int _Play_Sample_Handle(void *sample, int priority, int volume, int handle);

    public:
        bool Attempt_Audio_Restore(LPDIRECTSOUNDBUFFER);
        int Sample_Copy(SampleTrackerType *, void **, int *, void **, int *, void *, int , int);
        int Simple_Copy(void **, int *, void **, int *, void **, int);
};

#include "tspp.h"
DEFINE_IMPLEMENTATION(bool DSAudioClassFake::Attempt_Audio_Restore(LPDIRECTSOUNDBUFFER), 0x004890F0);
DEFINE_IMPLEMENTATION(int DSAudioClassFake::Sample_Copy(SampleTrackerType *, void **, int *, void **, int *, void *, int , int), 0x0048A750);
DEFINE_IMPLEMENTATION(int DSAudioClassFake::Simple_Copy(void **, int *, void **, int *, void **, int), 0x0048A690);
DEFINE_IMPLEMENTATION(int Convert_HMI_To_Direct_Sound_Volume(int), 0x004879C0);



#if 0
static tWAVEFORMATEX DSBuffFormat;
static DSBUFFERDESC BufferDesc;
static void *DumpBuffer;

#include "tspp.h"
DEFINE_IMPLEMENTATION(void func_6856E0(void *, void *), 0x006856E0);


DECLARE_PATCH(_DirectSoundAudioClass_Init_Patch)
{
    GET_REGISTER_STATIC(DirectSoundAudioClass *, this_ptr, ebp);
    LEA_STACK_STATIC(DSBUFFERDESC *, desc, esp, 0x30 - 0x4);
    
    std::memcpy(&BufferDesc, desc, sizeof(BufferDesc));
    BufferDesc.dwSize = sizeof(BufferDesc);

    DEBUG_INFO("Initialized BufferDesc with:\n");
    DEBUG_INFO("  dwSize %d\n", BufferDesc.dwSize);
    DEBUG_INFO("  dwFlags %d\n", BufferDesc.dwFlags);
    DEBUG_INFO("  dwBufferBytes %d\n", BufferDesc.dwBufferBytes);
    DEBUG_INFO("  dwReserved %d\n", BufferDesc.dwReserved);
    DEBUG_INFO("  lpwfxFormat %X\n", BufferDesc.lpwfxFormat);

    std::memcpy(&DSBuffFormat, desc->lpwfxFormat, sizeof(DSBuffFormat));
    BufferDesc.dwSize = sizeof(DSBuffFormat);

    BufferDesc.lpwfxFormat = &DSBuffFormat;

    DEBUG_INFO("Initialized DSBuffFormat with:\n");
    DEBUG_INFO("  wFormatTag %d\n", DSBuffFormat.wFormatTag);
    DEBUG_INFO("  nChannels %d\n", DSBuffFormat.nChannels);
    DEBUG_INFO("  nSamplesPerSec %d\n", DSBuffFormat.nSamplesPerSec);
    DEBUG_INFO("  nAvgBytesPerSec %d\n", DSBuffFormat.nAvgBytesPerSec);
    DEBUG_INFO("  nBlockAlign %d\n", DSBuffFormat.nBlockAlign);
    DEBUG_INFO("  wBitsPerSample %d\n", DSBuffFormat.wBitsPerSample);
    DEBUG_INFO("  cbSize %d\n", DSBuffFormat.cbSize);

    /**
     *  Stolen bytes/code.
     */
    ReleaseMutex(this_ptr->GlobalAudioMutex);

    JMP(0x0048836D);
}


DECLARE_PATCH(_DirectSoundAudioClass_Play_Sample_Handle_Patch)
{
    GET_REGISTER_STATIC(DirectSoundAudioClass *, this_ptr, ebp);
    GET_REGISTER_STATIC(SampleTrackerType *, st, esi);
    GET_STACK_STATIC(int, handle, esp, 0x58);
    LEA_STACK_STATIC(AUDHeaderType *, raw_header, esp, 0x24);
    static HRESULT res;
    static DWORD status;

    /**
     *  Within the branch when aud file and tracker info do not match.
     */



    DEBUG_INFO("DSAudio [%d]: Changing sample tracker format to %d,%d,%s\n",
        handle, raw_header->Rate, (raw_header->Flags & 2) ? 16 : 8, ((raw_header->Flags & 1) ? "stereo" : "mono"));



    st->Active = false;
    st->Service = false;
    st->MoreSource = false;


    // Query the playback status.
    do {
        res = st->PlayBuffer->GetStatus(&status);
        if (res == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st->PlayBuffer)) {
            DEBUG_INFO("DSAudio [%d]: Unable to get PlayBuffer status!\n", handle);
            return INVALID_AUDIO_HANDLE;
        }
    } while (res == DSERR_BUFFERLOST);



    // Stop the sample if its already playing.
    // TODO: Investigate this, logics here are possibly wrong.
    // - What happens when we call Restore when we have stopped the the buffer?
    // - Stop return isn't checked, in TS it checks for DSERR_BUFFERLOST, but thats not a valid Stop return.
    if (status & (DSBSTATUS_PLAYING | DSBSTATUS_LOOPING)
        && (st->PlayBuffer->Stop(), dsresult == DSERR_BUFFERLOST) && !Attempt_Audio_Restore(st->PlayBuffer)) {
        DebugInfo("DSAudio [%d]: Play_Sample_Handle - Unable to stop buffer!\n", handle);
        return INVALID_AUDIO_HANDLE;
    }


            func_6856E0(st->Source, st->Remainder);



    DEBUG_INFO("Initialized raw_header with:\n");
    DEBUG_INFO("  Rate %d\n", raw_header->Rate);
    DEBUG_INFO("  Size %d\n", raw_header->Size);
    DEBUG_INFO("  UncompSize %d\n", raw_header->UncompSize);
    DEBUG_INFO("  Flags %d\n", raw_header->Flags);
    DEBUG_INFO("  Compression %d\n", raw_header->Compression);


    if (st->PlayBuffer) {
        st->PlayBuffer->Release();
        st->PlayBuffer = nullptr;
    }

    DSBuffFormat.nSamplesPerSec = raw_header->Rate;
    DSBuffFormat.nChannels = (raw_header->Flags & 1) ? 2 : 1;
    DSBuffFormat.wBitsPerSample = (raw_header->Flags & 2) ? 16 : 8;
    DSBuffFormat.nBlockAlign = DSBuffFormat.nChannels * DSBuffFormat.wBitsPerSample / 8;
    DSBuffFormat.nAvgBytesPerSec = DSBuffFormat.nBlockAlign * DSBuffFormat.nSamplesPerSec;


    res = this_ptr->SoundObject->CreateSoundBuffer(&BufferDesc, &st->PlayBuffer, nullptr);

    
#if 0
    if (res == DSERR_BUFFERLOST) {
        JMP(0x00488BC0);
    }
    
    // We failed to create the buffer, bail!
    if (res != DS_OK && res != DSERR_BUFFERLOST) {
        st->PlaybackRate = 0;
        st->Stereo = false;
        st->BitSize = 0;
        //DEBUG_INFO("Play_Sample_Handle - Bad sample format!");
        JMP(0x00488BC0);
    }
    
    // Set the new sample info.
    st->PlaybackRate = raw_header->Rate;
    st->Stereo = raw_header->Flags & 1;
    st->BitSize = raw_header->Flags & 2;
    
    if (res != DS_OK){
        DEBUG_WARNING("CreateSoundBuffer returned %d\n", res);
    } else {
        DEBUG_INFO("CreateSoundBuffer is OK\n");
    }
#endif
    
    
    DEBUG_INFO("CreateSoundBuffer PlayBuffer is %x\n", st->PlayBuffer);
    
    SET_REGISTER(eax, res);
    __asm xor edi, edi

    JMP(0x00488C4E);
}
#endif



static void *DumpBuffer;
#define     BUFFER_CHUNK_SIZE       (8192 * 4)

int DSAudioClassFake::_Play_Sample_Handle(void *sample, int priority, int volume, int handle)
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

    SampleTrackerType *st = &SampleTracker[handle];

    AUDHeaderType aud_hdr;
    memcpy(&aud_hdr, sample, sizeof(AUDHeaderType));

    aud_hdr.Rate = std::clamp<unsigned short>(aud_hdr.Rate, 20000, 48000);

    st->Compression = aud_hdr.Compression;
    st->Original = sample;
    st->Odd = 0;
    st->Reducer = 0;
    st->QueueBuffer = 0;
    st->QueueSize = 0;
    st->Priority = priority;
    st->Service = false;
    st->Remainder = aud_hdr.Size;
    st->Source = static_cast<unsigned char *>(sample) + sizeof(AUDHeaderType);

    if (st->Compression == 99) {
        st->sSOSInfo.wChannels = (aud_hdr.Flags & 1) ? 2 : 1;
        st->sSOSInfo.wBitSize = (aud_hdr.Flags & 2) ? 16 : 8;
        st->sSOSInfo.dwCompSize = aud_hdr.Size;
        st->sSOSInfo.dwUnCompSize = aud_hdr.Size * (st->sSOSInfo.wBitSize / 4);
        sosCODEC2InitStream(&st->sSOSInfo);
    }

    if (aud_hdr.Rate != st->PlaybackRate
    || (aud_hdr.Flags & 2) != (st->BitSize & 2)
    || (aud_hdr.Flags & 1) != (st->Stereo & 1)) {

        DEBUG_INFO("DSAudio [%d]: Changing sample tracker format to %d,%d,%s\n",
            handle, aud_hdr.Rate, (aud_hdr.Flags & 2) ? 16 : 8, ((aud_hdr.Flags & 1) ? "stereo" : "mono"));

        st->Active = false;
        st->Service = false;
        st->MoreSource = false;

        do {
            dsresult = st->PlayBuffer->GetStatus(&status);
            if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st->PlayBuffer)) {
                return INVALID_AUDIO_HANDLE;
            }
        } while (dsresult == DSERR_BUFFERLOST);

        if (status & (DSBSTATUS_PLAYING | DSBSTATUS_LOOPING)
            && (st->PlayBuffer->Stop(), dsresult == DSERR_BUFFERLOST) && !Attempt_Audio_Restore(st->PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }

        if (st->PlayBuffer) {
            st->PlayBuffer->Release();
            st->PlayBuffer = nullptr;
        }

        tWAVEFORMATEX buffformat;
        DSBUFFERDESC bufferdesc;

        buffformat.nSamplesPerSec = aud_hdr.Rate;
        buffformat.nChannels = (aud_hdr.Flags & 1) + 1;
        buffformat.wBitsPerSample = aud_hdr.Flags & 2 ? 16 : 8;
        buffformat.nBlockAlign = buffformat.nChannels * buffformat.wBitsPerSample / 8;
        buffformat.nAvgBytesPerSec = buffformat.nBlockAlign * buffformat.nSamplesPerSec;

        dsresult = SoundObject->CreateSoundBuffer(&bufferdesc, &st->PlayBuffer, nullptr);
        if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st->PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }

        if (dsresult != DS_OK && dsresult != DSERR_BUFFERLOST) {
            st->PlaybackRate = 0;
            st->Stereo = false;
            st->BitSize = 0;
            return INVALID_AUDIO_HANDLE;
        }

        st->PlaybackRate = aud_hdr.Rate;
        st->Stereo = (aud_hdr.Flags & 1) ? 2 : 1;
        st->BitSize = (aud_hdr.Flags & 2) ? 16 : 8;
    }

    do {
        dsresult = st->PlayBuffer->GetStatus(&status);
        if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st->PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }
    } while (dsresult == DSERR_BUFFERLOST);

    if (status & (DSBSTATUS_PLAYING|DSBSTATUS_LOOPING)) {
        st->Active = false;
        st->Service = false;
        st->MoreSource = false;
        st->PlayBuffer->Stop();
    }

    LPVOID play_buffer_ptr;
    DWORD lock_length1;
    LPVOID dummy_buffer_ptr;
    DWORD lock_length2;

    do {
        dsresult = st->PlayBuffer->Lock(
            0, BUFFER_CHUNK_SIZE, &play_buffer_ptr, &lock_length1, &dummy_buffer_ptr, &lock_length2, 0);
        if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st->PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }
    } while (dsresult == DSERR_BUFFERLOST);

    if (dsresult != DS_OK) {
        DEBUG_INFO("DSAudio: Bad sample format!\n");

        st->PlaybackRate = 0;
        st->Stereo = 0;
        st->BitSize = 0;

        if (st->FileHandle) {
            st->FileHandle->Close();
            if (st->FileHandle) {
                delete st->FileHandle;
            }
            st->FileHandle = nullptr;
        }
        if (st->FileBuffer) {
            if (st->FileBuffer == FileStreamBuffer) {
                field_380 = false;
            } else {
                delete st->FileBuffer;
            }
            st->FileBuffer = nullptr;
        }

        st->Loading = false;
        st->Priority = 0;
        st->FilePending = 0;
        st->FilePendingSize = 0;
        st->QueueBuffer = nullptr;
        st->Callback = nullptr;

        return INVALID_AUDIO_HANDLE;
    }

    st->DestPtr = (void *)Sample_Copy(st,
        &st->Source,
        &st->Remainder,
        &st->QueueBuffer,
        &st->QueueSize,
        play_buffer_ptr,
        BUFFER_CHUNK_SIZE,
        st->Compression);

    if (st->DestPtr == (void *)BUFFER_CHUNK_SIZE) {
        st->MoreSource = true;
        st->Service = true;
        st->OneShot = false;
    } else {
        st->MoreSource = false;
        st->OneShot = true;
        st->Service = true;

        int v47 = (BUFFER_CHUNK_SIZE/4);
        if (aud_hdr.UncompSize < (v47*3)) {
            v47 = aud_hdr.UncompSize;
        }

        int v50 = std::max(BUFFER_CHUNK_SIZE - v47, v47);

        std::memset((static_cast<unsigned char *>(play_buffer_ptr) + static_cast<unsigned char *>(st->DestPtr)), 0, v50);
    }

    st->PlayBuffer->Unlock(play_buffer_ptr, lock_length1, dummy_buffer_ptr, lock_length2);
    st->DSVolume = volume;
    st->Volume = volume * 128;

    do {
        st->PlayBuffer->SetVolume(Convert_HMI_To_Direct_Sound_Volume((volume * SoundVolume) / 256));
        if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st->PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }
    } while (dsresult == DSERR_BUFFERLOST);

    if (!Start_Primary_Sound_Buffer(false)) {
        return INVALID_AUDIO_HANDLE;
    }

    do {
        dsresult = st->PlayBuffer->SetCurrentPosition(0);
        if (dsresult == DSERR_BUFFERLOST && !Attempt_Audio_Restore(st->PlayBuffer)) {
            return INVALID_AUDIO_HANDLE;
        }
    } while (dsresult == DSERR_BUFFERLOST);

    do {
        dsresult = st->PlayBuffer->Play(0, 0, DSBPLAY_LOOPING);

        if (dsresult == DS_OK) {

            st->Active = true;
            st->Handle = handle;

            return st->Handle;
        }

        if (dsresult != DSERR_BUFFERLOST) {
            st->Active = false;
            break;
        }

        if (!Attempt_Audio_Restore(st->PlayBuffer)) {
            break;
        }

    } while (dsresult == DSERR_BUFFERLOST);

    return INVALID_AUDIO_HANDLE;
}



#endif

