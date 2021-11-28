/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          FMOD_AHANDLE.CPP
 *
 *  @author        CCHyper
 *
 *  @contributors  
 *
 *  @brief         
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
#include "fmod_ahandle.h"
#include "fmod_audio.h"
#include "debughandler.h"


FMOD_AHandle AudioHandles[MAX_AUDIO_HANDLES];


// 006A9640, 006A99F4, 006A9A0D calls stream handler (00407450)
// 006AFD81 calls (004072D0)
// 006AFDC4 calls (004072D0)


/**
 *  Possible call-back command values.
 */
enum VQACommandType
{
    VQACMD_NOTHING = 0,
    VQACMD_INIT = 1,		// Prepare the stream for a session.
    VQACMD_CLEANUP = 2,		// Terminate stream session.
    VQACMD_OPEN = 3,		// Open the stream handle.
    VQACMD_CLOSE = 4,		// Close the stream handle.
    VQACMD_READ = 5,		// Read bytes from stream.
    VQACMD_WRITE = 6,		// Write bytes to stream.
    VQACMD_SEEK = 7,		// Seek on stream.
    VQACMD_EOF = 8,
    VQACMD_SIZE = 9
};

enum VQAMemoryHandlerType
{
    VQAMEMORY_0 = 0,
    VQAMEMORY_1 = 1,
    VQAMEMORY_ALLOC = 2,
    VQAMEMORY_FREE = 3,
    VQAMEMORY_REF_INC = 4,
    VQAMEMORY_REF_DEC = 5,
    VQAMEMORY_FREESIZE = 6
};

enum VQAErrorType
{
    VQAERR_OK = 0,
    VQAERR_FORCEDRW = -25,
    VQAERR_SETBUFFR = -24,
    VQAERR_SETLOOP = -23,
    VQAERR_SKIPDRAW = -22,
    VQAERR_NOAHANDL = -21,
    VQAERR_BADBLOCK = -20,
    VQAERR_AUDSYNC = -19,
    VQAERR_NOAUDSIZ = -18,
    VQAERR_NOCONFIG = -17,
    VQAERR_NORATE = -16,
    VQAERR_NOTIMER = -15,
    VQAERR_PAUSED = -14,
    VQAERR_AUDIO = -13,
    VQAERR_VIDEO = -12,
    VQAERR_SLEEPING = -11,
    VQAERR_NOT_TIME = -10,
    VQAERR_NOBUFFER = -9,
    VQAERR_NOMEM = -8,
    VQAERR_NOTVQA = -7,
    VQAERR_SEEK = -6,
    VQAERR_WRITE = -5,
    VQAERR_READ = -4,
    VQAERR_OPEN = -3,
    VQAERR_EOF = -2,
    VQAERR_NONE = -1
};


//0x004072D0
int __stdcall FMOD_AHandle::func_4072D0(VQAHandle *handle, void *a2)
{
    return 0;
}


//0x00407450
int __stdcall FMOD_AHandle::Stream_Handler(VQAHandle *handle, int action, void *buffer, int nbytes)
{
    int error = 1;

    /**
     *  Perform the action specified by the stream command.
     */
    switch (action) {

        case 1:
            DEBUG_INFO("CASE 1\n");
            //handle->Config_TimerCallback = func_408200();
            //handle->Config_RefreshRate = 60;
            error = 0;
            break;

        case 2: // START?
            DEBUG_INFO("CASE 2\n");
            error = func_407650(handle, buffer, nbytes);
            break;

        case 3: // STOP?
            DEBUG_INFO("CASE 3\n");
            error = func_407980(handle);
            break;

        case 4: // START?
            DEBUG_INFO("CASE 4\n");
            error = func_407B20(handle);
            break;

        case 5: // COPY?
            DEBUG_INFO("CASE 5\n");
            error = func_407D30(handle, buffer, nbytes);
            break;

        case 6: // PAUSE?
            DEBUG_INFO("CASE 6\n");
            error = 0;
            break;

        case 7:
            DEBUG_INFO("CASE 7\n");
            error = 0;
            break;

        case 8: // RESUME?
            DEBUG_INFO("CASE 8\n");
            error = 0;
            break;

        default:
            break;
    };

    return error;
}


// Start Audio?
//0x00407650
int FMOD_AHandle::func_407650(VQAHandle *handle, void *buffer, int nbytes)
{
#if 0
    DEBUG_INFO("FMOD: Opening VQ audio handler\n");

    DEBUG_INFO("FMOD: Current thread ID is %08x\n", GetCurrentThreadId());

    if (Audio.Is_Available()) {

        int free_index = 0;
        v5 = AudioHandles;
        do
        {
        if ( !v5->SomeBool? )
        break;
        v5 = (v5 + 0xBC);
        ++free_index?;
        }
        while ( v5 < &AudioHandles[0].field_BC );

        if ( free_index? != 1 && a3 == 16 ) {

            handle->_audio_handle = free_index;
            FMOD_AHandle &ah = AudioHandles[handle->_audio_handle];

            ZeroMemory(&ah, sizeof(FMOD_AHandle));

            ah.field_0 = true;

            ah.Volume? = handle->__Config_volume?;

            InitializeCriticalSection(&ah.CriticalSection);

            int v7 = handle->Config_field_A0;
            if ( v7 == -1 )
            {
                v8 = handle->Config_FrameRate;
                v9 = handle->__FPS;
                if ( v8 == v9 ) {
                    ah.field_8 = *a2;
                } else {
                    ah.field_8 = v8 * *a2 / v9;
                }
            } else {
                ah.field_8 = v7;
            }

            memset(dscaps, 0, sizeof(dscaps));
            dscaps[0] = sizeof(DSCAPS);

            //DEBUG_INFO("FMOD: Audio.Lock_Mutex\n");
            //Audio.Lock_Mutex();
        }
    }
#endif

    return VQAERR_AUDIO;
}


//0x00407980
int FMOD_AHandle::func_407980(VQAHandle *handle)
{
#if 0
    DEBUG_INFO("FMOD: Closing VQ audio handler\n");

    FMOD_AHandle &ah = AudioHandles[handle->_audio_handle];

    if (ah.field_0) {

        DEBUG_INFO("FMOD: Stop_Audio_Handler\n");
        if (ah.BufferPtr) {

            EnterCriticalSection(&ah.CriticalSection);

            ah.BufferPtr->Stop();
            ah.BufferPtr->Release();
            ah.BufferPtr = nullptr;

            ah.field_40 = 0;
            ah.field_44 = 0;
            ah.field_48 = 0;
            ah.field_4C = 0;

            LeaveCriticalSection(&ah.CriticalSection);
        }

        DEBUG_INFO("FMOD: Calling timeKillEvent\n");
        timeKillEvent(ah.TimerHandle);

        ah.TimerHandle = 0;

        DEBUG_INFO("FMOD: Calling timeEndPeriod\n");
        timeEndPeriod(16);

        ah.field_0 = false;

        if (ah.field_BC) {
            DEBUG_INFO("FMOD: Changing primary buffer format back to original\n");
            ah.field_BC = false;
            //DEBUG_INFO("FMOD: Audio.Lock_Mutex()\n");
            //Audio.Lock_Mutex(&Audio);
        }

        DEBUG_INFO("FMOD: Deleting the critical section object\n");
        DeleteCriticalSection(&ah.CriticalSection);
    }

    DEBUG_INFO("FMOD: VQ audio handler closed OK\n");
#endif
    return VQAERR_NONE;
}


//0x00407B20
int FMOD_AHandle::func_407B20(VQAHandle *handle)
{
#if 0
    FMOD_AHandle &ah = AudioHandles[handle->_audio_handle];

    if (ah.field_0) {

        EnterCriticalSection(&ah.CriticalSection);

        if (ah.BufferPtr) {
            ah.BufferPtr->Stop();
            ah.BufferPtr->Release();
            ah.BufferPtr = nullptr;
        }

        at.__bufsize? = 2 * handle->Config_HMIBufSize;
        at.BufferDesc.dwSize = 0;
        at.BufferDesc.dwFlags = 0;
        at.BufferDesc.dwBufferBytes = 0;
        at.BufferDesc.dwReserved = 0;
        at.BufferDesc.lpwfxFormat = 0;
        at.BufferDesc.dwBufferBytes = ah.__bufsize?;
        at.BufferDesc.dwSize = sizeof(DSBUFFERDESC);
        at.BufferDesc.dwFlags = 0x10080;
        at.BufferDesc.lpwfxFormat = &ah.SecondaryBuffFormat;
        at.BufferDesc.wFormatTag = 0;
        at.BufferDesc.nSamplesPerSec = 0;
        at.BufferDesc.nAvgBytesPerSec = 0;
        at.BufferDesc.nBlockAlign = 0;
        at.BufferDesc.cbSize = 0;
        at.BufferDesc.wFormatTag = 1;
        ah.SecondaryBuffFormat.wBitsPerSample = at.BitsPerSample?;
        at.BuffFormat.nBlockAlign = at.Channels? * (at.BitsPerSample? >> 3);
        at.BuffFormat.nSamplesPerSec = at.SamplesPerSec?;
        at.BuffFormat.nChannels = at.Channels?;
        at.BuffFormat.nAvgBytesPerSec = at.SamplesPerSec? * at.BuffFormat.nBlockAlign;

        //Audio.Lock_Mutex();
    }

    if (ah.field_0 || !ah.BufferPtr) {
        return VQAERR_AUDIO;
    }

    EnterCriticalSection(&ah.CriticalSection);

    if (ah.BufferPtr->Play(0, 0, DSBPLAY_LOOPING)) {
        LeaveCriticalSection(&ah.CriticalSection);
        return VQAERR_AUDIO;
    }



    
    LeaveCriticalSection(&ah.CriticalSection);
#endif

    return VQAERR_NONE;
}


//0x00407D30
int FMOD_AHandle::func_407D30(VQAHandle *handle, void *buffer, int nbytes)
{
    return VQAERR_NONE;
}


// Stop?
//0x00407EF0
int FMOD_AHandle::func_407EF0(VQAHandle *handle)
{
#if 0
    FMOD_AHandle &ah = AudioHandles[handle->_audio_handle];
    EnterCriticalSection(&ah.CriticalSection);
    if (ah.field_0) {
        if (ah.BufferPtr) {
            ah.BufferPtr->Stop();
        }
    }
    ah.field_28_1 = true;
    LeaveCriticalSection(&ah.CriticalSection);
#endif
    return VQAERR_NONE;
}


//0x00407F40
int FMOD_AHandle::func_407F40(VQAHandle *handle)
{
    return VQAERR_NONE;
}


//0x0407FE0
int FMOD_AHandle::Stop_Audio_Handler(VQAHandle *vqhandle)
{
    return VQAERR_NONE;
}


// NOT NEEDED
//0x00408060
void __stdcall FMOD_AHandle::SoundTimerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
}


// NOT NEEDED
//0x00408200
unsigned int FMOD_AHandle::func_408200()
{
    return 0;
}


// Pause Audio?
//0x00408210
void FMOD_AHandle::func_408210()
{
    FMOD_AHandle &ah = AudioHandles[0];

    if (!ah.field_0) {
        return;
    }

    if (ah.BufferPtr) {
        EnterCriticalSection(&ah.CriticalSection);
        ah.BufferPtr->Stop();
        ah.field_28_1 = true;
        LeaveCriticalSection(&ah.CriticalSection);
    }
}


// Resume Audio?
//0x00408260
void FMOD_AHandle::func_408260()
{
    FMOD_AHandle &ah = AudioHandles[0];

    if (!ah.field_0) {
        return;
    }

    if (ah.BufferPtr) {
        EnterCriticalSection(&ah.CriticalSection);
        ah.BufferPtr->Play(0, 0, DSBPLAY_LOOPING);
        ah.field_28_1 = false;
        LeaveCriticalSection(&ah.CriticalSection);
    }
}


// Open Audio?
//0x004082B0
int FMOD_AHandle::func_4082B0()
{
    return 1;
}


// Close Audio?
//0x004082C0
int FMOD_AHandle::func_4082C0()
{
    return 1;
}
