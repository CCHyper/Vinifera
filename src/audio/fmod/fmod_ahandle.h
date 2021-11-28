/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          FMOD_AHANDLE.H
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
#pragma once

#include "always.h"
#include "dsaudio.h"
#include "fmod_audio.h"


struct VQAHandle;

enum {
    MAX_AUDIO_HANDLES = 1
};

class FMOD_AHandle
{
    public:
        //FMOD_AHandle() : BufferDesc(), BuffFormat() {}
        ~FMOD_AHandle() {}

        // TODO: Move statics to cpp statics when done

        static int __stdcall func_4072D0(VQAHandle *handle, void *a2);
        static int __stdcall Stream_Handler(VQAHandle *handle, int action, void *buffer, int nbytes);
        static int func_407650(VQAHandle *handle, void *buffer, int nbytes);
        static int func_407980(VQAHandle *handle);
        static int func_407B20(VQAHandle *handle);
        static int func_407D30(VQAHandle *handle, void *buffer, int nbytes);
        static int func_407EF0(VQAHandle *handle);
        static int func_407F40(VQAHandle *handle);
        static int Stop_Audio_Handler(VQAHandle *handle);
        static void __stdcall SoundTimerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
        static unsigned int func_408200();
        static void func_408210();
        static void func_408260();
        static int func_4082B0();
        static int func_4082C0();

    private:
        bool field_0; // is allocated? is active?
        int field_4; // volume?
        short field_8; // samples per sec?
        char field_A; // channels?
        char field_B; // bits per sample?
        int field_C; // 
        int field_10; // 
        int field_14; // 
        int field_18; // 
        int field_1C; // pause adjust?
        int field_20; // paused frame?
        int field_24; // 

        //int field_28; // flags?
        bool field_28_1 : 1;            // paused?
        bool field_28_2 : 1;
        bool field_28_4 : 1;
        bool field_28_8 : 1;
        bool field_28_16 : 1;

        int field_2C; // 
        int field_30; // 
        int field_34; // 
        int field_38; // 
        int field_3C; // 
        int field_40; // 
        int field_44; // 
        int field_48; // pointer
        int field_4C; // 
        int field_50; // 
        int field_54; // 
        int field_58; // 
        int field_5C; // 
        UINT TimerHandle; // 0x60
        DSBUFFERDESC BufferDesc; // 0x64
        WAVEFORMATEX BuffFormat; // 0x78
        LPDIRECTSOUNDBUFFER BufferPtr;
        int field_90; // buffer size?
        int field_94;
        int field_98;
        int field_9C; // write cursor?
        CRITICAL_SECTION CriticalSection; // 0xA0
        int field_B8; // lock level?
        bool field_BC;
};


extern FMOD_AHandle AudioHandles[MAX_AUDIO_HANDLES];
