/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENAL_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the OpenAL audio engine.
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
#include "openal_hooks.h"
#include "openal_globals.h"
#include "openal_audio.h"
#include "tibsun_globals.h"
#include "dsaudio.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/** ============================================================================
 *  Audio_Init intercepts
 */
DECLARE_PATCH(_WinMain_Audio_Init_Intercept_1)
{
    DEBUG_INFO("About to call Audio Init...\n");

    if (OpenALInitDirectSound) {
        Audio.Init(MainWindow, 16, false, 22050);
        DEBUG_INFO("DirectSound intialised.\n");
    }

    if (OpenALInitialised) {
        OpenALAudio.Init(MainWindow);
        DEBUG_INFO("OpenAL intialised.\n");
    }

    DEBUG_INFO("Audio Init Done!\n");

    JMP(0x006013C8);
}

DECLARE_PATCH(_WinMain_Audio_Init_Intercept_2)
{
    DEBUG_INFO("About to call Audio Init...\n");

    if (OpenALInitDirectSound) {
        Audio.Init(MainWindow, 16, false, 22050);
        DEBUG_INFO("DirectSound intialised.\n");
    }

    if (OpenALInitialised) {
        OpenALAudio.Init(MainWindow);
        DEBUG_INFO("OpenAL intialised.\n");
    }

    DEBUG_INFO("Audio Init Done!\n");

    JMP(0x006016B3);
}


/** ============================================================================
 *  Audio_End intercepts
 */
DECLARE_PATCH(_WinMain_Audio_End)
{
    if (OpenALInitDirectSound) {
        Audio.End();
        DEBUG_INFO("DirectSound shutdown.\n");
    }

    if (OpenALInitialised) {
        OpenALAudio.End();
        DEBUG_INFO("OpenAL shutdown.\n");
    }

    JMP(0x00601A1C);
}

DECLARE_PATCH(_Emergency_Exit_Audio_End)
{
    if (OpenALInitDirectSound) {
        Audio.End();
        DEBUG_INFO("DirectSound shutdown.\n");
    }

    if (OpenALInitialised) {
        OpenALAudio.End();
        DEBUG_INFO("OpenAL shutdown.\n");
    }

    JMP(0x0060248A);
}


/** ============================================================================
 *  Stop_Sample intercepts
 */
DECLARE_PATCH(_MapSelect_Smth__Stop_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, eax);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample(handle);
    //}

    if (OpenALInitialised) {
        OpenALAudio.Stop_Sample(handle);
    }

    JMP_REG(ecx, 0x0055404D);
}

DECLARE_PATCH(_MapSelect_Update__Stop_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, eax);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample(handle);
    //}

    if (OpenALInitialised) {
        OpenALAudio.Stop_Sample(handle);
    }

    JMP_REG(ecx, 0x005544C9);
}

DECLARE_PATCH(_MapSelect_Sample_Vol__Stop_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, eax);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample(handle);
    //}

    if (OpenALInitialised) {
        OpenALAudio.Stop_Sample(handle);
    }

    JMP_REG(ecx, 0x005545EC);
}

DECLARE_PATCH(_MapSelect_Stop_Sample__Stop_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, eax);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample(handle);
    //}

    if (OpenALInitialised) {
        OpenALAudio.Stop_Sample(handle);
    }

    JMP_REG(ecx, 0x00554672);
}

DECLARE_PATCH(_ThemeClass_Play_Song__Stop_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, ecx);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample(handle);
    //}

    if (OpenALInitialised) {
        OpenALAudio.Stop_Sample(handle);
    }

    JMP(0x00644048);
}

DECLARE_PATCH(_ThemeClass_Stop__Stop_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, eax);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample(handle);
    //}

    if (OpenALInitialised) {
        OpenALAudio.Stop_Sample(handle);
    }

    JMP(0x00644238);
}

DECLARE_PATCH(_ThemeClass_Suspend__Stop_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, ecx);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample(handle);
    //}

    if (OpenALInitialised) {
        OpenALAudio.Stop_Sample(handle);
    }

    JMP_REG(edx, 0x0064429A);
}

// 0067E8FB
// 0067FCA9
// 0067FD89
// 0067FE5C
// 0067FE96
// 00680063
// 006800F1
// 0068018C
// 006801C6

/** ============================================================================
 *  Sample_Status intercepts
 */
//DECLARE_PATCH(_)
//{
//}


/** ============================================================================
 *  Is_Sample_Playing intercepts
 */
DECLARE_PATCH(_Speak_AI__Is_Sample_Playing_Intercept)
{
    GET_REGISTER_STATIC(const void *, sample, ecx);
    static bool is_playing;

    //if (OpenALInitDirectSound) {
    //    is_playing = Audio.Is_Sample_Playing(sample);
    //}

    if (OpenALInitialised) {
        is_playing = OpenALAudio.Is_Sample_Playing(sample);
    }
    
    _asm { mov al, is_playing }
    JMP_REG(esi, 0x0066599E);
}

DECLARE_PATCH(_Speak_AI__Is_Speaking_Intercept)
{
    GET_REGISTER_STATIC(const void *, sample, ecx);
    static bool is_playing;

    //if (OpenALInitDirectSound) {
    //    is_playing = Audio.Is_Sample_Playing(sample);
    //}

    if (OpenALInitialised) {
        is_playing = OpenALAudio.Is_Sample_Playing(sample);
    }

    _asm { mov al, is_playing }
    JMP_REG(ebx, 0x00665B60);
}


/** ============================================================================
 *  Stop_Sample_Playing intercepts
 */
#if 0
DECLARE_PATCH(_Stop_Speaking___Intercept)
{
    GET_REGISTER_STATIC(const void *, sample, ecx);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample_Playing(sample);
    //}

    if (OpenALInitialised) {
        Audio.Stop_Sample_Playing(sample);
    }
    
    //JMP_REG(ecx, 0x);
}

DECLARE_PATCH(_Stop_Speaking___Intercept)
{
    GET_REGISTER_STATIC(const void *, sample, ecx);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample_Playing(sample);
    //}

    if (OpenALInitialised) {
        Audio.Stop_Sample_Playing(sample);
    }
    
    //JMP_REG(ecx, 0x);
}

DECLARE_PATCH(_Stop_Speaking___Intercept)
{
    GET_REGISTER_STATIC(const void *, sample, ecx);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample_Playing(sample);
    //}

    if (OpenALInitialised) {
        Audio.Stop_Sample_Playing(sample);
    }
    
    //JMP_REG(ecx, 0x);
}

DECLARE_PATCH(_Stop_Speaking___Intercept)
{
    GET_REGISTER_STATIC(const void *, sample, ecx);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample_Playing(sample);
    //}

    if (OpenALInitialised) {
        Audio.Stop_Sample_Playing(sample);
    }
    
    //JMP_REG(ecx, 0x);
}

DECLARE_PATCH(_Stop_Speaking___Intercept)
{
    GET_REGISTER_STATIC(const void *, sample, ecx);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample_Playing(sample);
    //}

    if (OpenALInitialised) {
        Audio.Stop_Sample_Playing(sample);
    }
    
    //JMP_REG(ecx, 0x);
}

DECLARE_PATCH(_Stop_Speaking___Intercept)
{
    GET_REGISTER_STATIC(const void *, sample, ecx);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample_Playing(sample);
    //}

    if (OpenALInitialised) {
        Audio.Stop_Sample_Playing(sample);
    }
    
    //JMP_REG(ecx, 0x);
}
#endif

DECLARE_PATCH(_ScoreSoundClass_Stop__Stop_Sample_Playing_Intercept)
{
    GET_REGISTER_STATIC(const void *, sample, eax);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample_Playing(sample);
    //}

    if (OpenALInitialised) {
        Audio.Stop_Sample_Playing(sample);
    }
    
    JMP_REG(eax, 0x005E73C5);
}

DECLARE_PATCH(_Stop_Speaking__Stop_Sample_Playing_Intercept)
{
    GET_REGISTER_STATIC(const void *, sample, ecx);

    //if (OpenALInitDirectSound) {
    //    Audio.Stop_Sample_Playing(sample);
    //}

    if (OpenALInitialised) {
        Audio.Stop_Sample_Playing(sample);
    }
    
    JMP_REG(ecx, 0x00665B11);
}


/** ============================================================================
 *  Play_Sample intercepts
 */
DECLARE_PATCH(_Call_Back__Play_Sample_Intercept)
{
    if (OpenALInitDirectSound) {
        if (Audio.Is_Available() && GameInFocus) {
            Audio.Sound_Callback();
        }
    }

    if (OpenALInitialised) {
        if (OpenALAudio.Is_Available() && GameInFocus) {
            OpenALAudio.Sound_Callback();
        }
    }

    JMP_REG(ecx, 0x00462C91);
}

/** ============================================================================
 *  File_Stream_Sample_Vol intercepts
 */
//DECLARE_PATCH(_)
//{
//}


/** ============================================================================
 *  Sound_Callback intercepts
 */
DECLARE_PATCH(_Call_Back__Sound_Callback_Intercept)
{
    if (OpenALInitDirectSound) {
        if (Audio.Is_Available() && GameInFocus) {
            Audio.Sound_Callback();
        }
    }

    if (OpenALInitialised) {
        if (OpenALAudio.Is_Available() && GameInFocus) {
            OpenALAudio.Sound_Callback();
        }
    }

    JMP_REG(ecx, 0x00462C91);
}

DECLARE_PATCH(_OwnerDraw_DialogProc__Sound_Callback_Intercept)
{
    if (OpenALInitDirectSound) {
        if (Audio.Is_Available() && GameInFocus) {
            Audio.Sound_Callback();
        }
    }

    if (OpenALInitialised) {
        if (OpenALAudio.Is_Available() && GameInFocus) {
            OpenALAudio.Sound_Callback();
        }
    }

    JMP_REG(ecx, 0x00594286);
}

DECLARE_PATCH(_ThemeClass_AI__Sound_Callback_Intercept)
{
    if (OpenALInitDirectSound) {
        Audio.Sound_Callback();
    }

    if (OpenALInitialised) {
        OpenALAudio.Sound_Callback();
    }

    JMP_REG(ecx, 0x00643E6F);
}


/** ============================================================================
 *  Set_Volume_All intercepts
 */
//DECLARE_PATCH(_)
//{
//}


/** ============================================================================
 *  Adjust_Volume_All intercepts
 */
//DECLARE_PATCH(_)
//{
//}


/** ============================================================================
 *  Set_Handle_Volume intercepts
 */
//DECLARE_PATCH(_)
//{
//}


/** ============================================================================
 *  Set_Sample_Volume intercepts
 */
//DECLARE_PATCH(_)
//{
//}


/** ============================================================================
 *  Fade_Sample intercepts
 */
DECLARE_PATCH(_MapSelect_Update_1__Fade_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, eax);

    if (OpenALInitDirectSound) {
        Audio.Fade_Sample(handle, 20);
    }

    if (OpenALInitialised) {
        Audio.Fade_Sample(handle, 20);
    }

    JMP_REG(ecx, 0x00554240);
}

DECLARE_PATCH(_MapSelect_Update_2__Fade_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, eax);

    if (OpenALInitDirectSound) {
        Audio.Fade_Sample(handle, 20);
    }

    if (OpenALInitialised) {
        Audio.Fade_Sample(handle, 20);
    }

    JMP_REG(ecx, 0x00554435);
}

DECLARE_PATCH(_MapSelect_Fade_Sample__Fade_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, eax);

    if (OpenALInitDirectSound) {
        Audio.Fade_Sample(handle, 20);
    }

    if (OpenALInitialised) {
        Audio.Fade_Sample(handle, 20);
    }

    JMP_REG(ecx, 0x0055456F);
}

DECLARE_PATCH(_MapSelect_Stop_Sample__Fade_Sample_Intercept)
{
    GET_REGISTER_STATIC(int, handle, eax);

    if (OpenALInitDirectSound) {
        Audio.Fade_Sample(handle, 20);
    }

    if (OpenALInitialised) {
        Audio.Fade_Sample(handle, 20);
    }

    JMP_REG(ecx, 0x00554665);
}

//Patch_Jump(0x00643FCE, &_ThemeClass_Queue_Song__Fade_Sample_Intercept);
DECLARE_PATCH(_ThemeClass_Queue_Song__Fade_Sample_Intercept_)
{
    GET_REGISTER_STATIC(int, handle, eax);

    if (OpenALInitDirectSound) {
        Audio.Fade_Sample(handle, 60);
    }

    if (OpenALInitialised) {
        Audio.Fade_Sample(handle, 60);
    }

    JMP_REG(ecx, 0x00554665);
}

//Patch_Jump(0x0064420B, &_ThemeClass_Stop__Fade_Sample_Intercept);
DECLARE_PATCH(_ThemeClass_Stop__Fade_Sample_Intercept)
{
}


/** ============================================================================
 *  Start_Primary_Sound_Buffer intercepts
 */
DECLARE_PATCH(_Focus_Restore__Start_Primary_Sound_Buffer_Intercept)
{
    _asm { add esp, 0x4 }
    
    if (OpenALInitDirectSound) {
        if (Audio.Is_Available()) {
            Audio.Start_Primary_Sound_Buffer(true);
        }
    }

    if (OpenALInitialised) {
        if (OpenALAudio.Is_Available()) {
            OpenALAudio.Start_Primary_Sound_Buffer(true);
        }
    }

    JMP_REG(ecx, 0x00685A2B);
}


/** ============================================================================
 *  Stop_Primary_Sound_Buffer intercepts
 */
DECLARE_PATCH(_Main_Window_Procedure__Stop_Primary_Sound_Buffer_Intercept)
{
    if (OpenALInitDirectSound) {
        if (Audio.Is_Available()) {
            Audio.Sound_Callback();
        }
    }

    if (OpenALInitialised) {
        if (OpenALAudio.Is_Available()) {
            OpenALAudio.Sound_Callback();
        }
    }

    JMP_REG(ecx, 0x00462C91);
}


/** ============================================================================
 *  Is_Available intercepts
 */
static bool dsound_available;
static bool openal_available;

// 004AAACB
// 004E4615
DECLARE_PATCH(_Main_Loop__Is_Available_Intercept)
{
    dsound_available = false;
    openal_available = false;

    if (OpenALInitDirectSound && Audio.Is_Available()) {
        dsound_available = true;
    }

    if (OpenALInitialised && OpenALAudio.Is_Available()) {
        openal_available = true;
    }

    if (openal_available || (dsound_available && openal_available)) {
        goto is_available;
    }

no_available:
    JMP_THIS(ecx, 0x00508AF4);

is_available:
    JMP_THIS(ecx, 0x00508ADF);
}

// 0050A758
// 0056419F
// 0056E646
// 0056E990
// 0056F296
// 0057146E
// 005721B7
// 00574BF8
DECLARE_PATCH(_OwnerDraw_DialogProc__Is_Available_Intercept)
{
}

// 005E3552
// 005E369E
// 005E3E03
// 005E43CF
// 005E57BB
// 005E5F8B
// 005E615E
// 005E677E
// 005E68D0
// 005E7308
// 005FC531
// 00643C87
// 00643DC0
// 00643F30
// 00643F93
// 00643FF2
// 006441A0
// 0064425C
// 006442B9
// 00664A7E
// 00664B28
// 00664BDD
// 00664C97
// 00664E33
// 00665823
// 00665955
// 0067F77C
// 0067FA0C

















/**
 *  Main function for patching the hooks.
 */
void OpenAL_Audio_Hooks()
{
/** ============================================================================
 *  Audio_Init intercepts                                                   DONE
 */
    Patch_Jump(0x006013B0, &_WinMain_Audio_Init_Intercept_1);
    Patch_Jump(0x0060169B, &_WinMain_Audio_Init_Intercept_2);

/** ============================================================================
 *  Audio_End intercepts                                                    DONE
 */
    Patch_Jump(0x00601A12, &_WinMain_Audio_End);
    Patch_Jump(0x00602480, &_Emergency_Exit_Audio_End);

/** ============================================================================
 *  Stop_Sample intercepts
 */
    Patch_Jump(0x00554042, &_MapSelect_Smth__Stop_Sample_Intercept);
    Patch_Jump(0x005544BE, &_MapSelect_Update__Stop_Sample_Intercept);
    Patch_Jump(0x005545E1, &_MapSelect_Sample_Vol__Stop_Sample_Intercept);
    Patch_Jump(0x00554667, &_MapSelect_Stop_Sample__Stop_Sample_Intercept);
    Patch_Jump(0x0064403D, &_ThemeClass_Play_Song__Stop_Sample_Intercept);
// 0067E8FB
// 0067FCA9
// 0067FD89
// 0067FE5C
// 0067FE96
// 00680063
// 006800F1
// 0068018C
// 006801C6

/** ============================================================================
 *  Sample_Status intercepts
 */

/** ============================================================================
 *  Is_Sample_Playing intercepts
 */

/** ============================================================================
 *  Stop_Sample_Playing intercepts
 */
    //Patch_Jump(0x0056E71E, &_Stop_Speaking___Intercept);
    //Patch_Jump(0x0056E81F, &_Stop_Speaking___Intercept);
    //Patch_Jump(0x0056F3BA, &_Stop_Speaking___Intercept);
    //Patch_Jump(0x00571F82, &_Stop_Speaking___Intercept);
    //Patch_Jump(0x00574CAA, &_Stop_Speaking___Intercept);
    //Patch_Jump(0x005E4B7A, &_Stop_Speaking___Intercept);
    //Patch_Jump(0x005E68A0, &_Stop_Speaking___Intercept);
    Patch_Jump(0x005E73BA, &_ScoreSoundClass_Stop__Stop_Sample_Playing_Intercept);
    Patch_Jump(0x00665B06, &_Stop_Speaking__Stop_Sample_Playing_Intercept);

/** ============================================================================
 *  Play_Sample intercepts
 */
// 0056D056
// 0056D15C
// 0056F406
// 00572568
// 00574CEB
// 00593DEE
// 005E35A9
// 005E36F5
// 005E3E5A
// 005E4426
// 005E5824
// 005E5FF5
// 005E61B5
// 005E67D4
// 005E6BB3
// 00664ADC
// 00664B80
// 00664C3B
// 00664CEF
// 00664E91
// 00665AC2
// 0067FD06
// 006800BE

/** ============================================================================
 *  File_Stream_Sample_Vol intercepts
 */
// 0055404D
// 005545EC
// 006440B6

/** ============================================================================
 *  Sound_Callback intercepts                                               DONE
 */
    Patch_Jump(0x00462C6C, &_Call_Back__Sound_Callback_Intercept);
    Patch_Jump(0x00594261, &_OwnerDraw_DialogProc__Sound_Callback_Intercept);
    Patch_Jump(0x00643E65, &_ThemeClass_AI__Sound_Callback_Intercept);

/** ============================================================================
 *  Set_Volume_All intercepts
 */
// 005BCE28

/** ============================================================================
 *  Adjust_Volume_All intercepts
 */
// 005BCC60

/** ============================================================================
 *  Set_Handle_Volume intercepts
 */
// 0064442A

/** ============================================================================
 *  Set_Sample_Volume intercepts
 */
// 00665B9E

/** ============================================================================
 *  Fade_Sample intercepts
 */
    //Patch_Jump(0x00554233, &_MapSelect_Update_1__Fade_Sample_Intercept);
    //Patch_Jump(0x00554428, &_MapSelect_Update_2__Fade_Sample_Intercept);
    //Patch_Jump(0x00554562, &_MapSelect_Fade_Sample__Fade_Sample_Intercept);
    //Patch_Jump(0x00554658, &_MapSelect_Stop_Sample__Fade_Sample_Intercept);
    //Patch_Jump(0x00643FCE, &_ThemeClass_Queue_Song__Fade_Sample_Intercept);
    //Patch_Jump(0x0064420B, &_ThemeClass_Stop__Fade_Sample_Intercept);

/** ============================================================================
 *  Start_Primary_Sound_Buffer intercepts                                   DONE
 */
    Patch_Jump(0x00685A0A, &_Focus_Restore__Start_Primary_Sound_Buffer_Intercept);

/** ============================================================================
 *  Stop_Primary_Sound_Buffer intercepts                                    DONE
 */
    Patch_Jump(0x00685EB9, &_Main_Window_Procedure__Stop_Primary_Sound_Buffer_Intercept); // Inlined Focus_Loss

/** ============================================================================
 *  Is_Available intercepts
 */
// 004AAACB
// 004E4615
    Patch_Jump(0x00508ACD, &_Main_Loop__Is_Available_Intercept);
// 0050A758
// 0056419F
// 0056E646
// 0056E990
// 0056F296
// 0057146E
// 005721B7
// 00574BF8
    Patch_Jump(0x00594261, &_OwnerDraw_DialogProc__Is_Available_Intercept);
// 005E3552
// 005E369E
// 005E3E03
// 005E43CF
// 005E57BB
// 005E5F8B
// 005E615E
// 005E677E
// 005E68D0
// 005E7308
// 005FC531
// 00643C87
// 00643DC0
// 00643F30
// 00643F93
// 00643FF2
// 006441A0
// 0064425C
// 006442B9
// 00664A7E
// 00664B28
// 00664BDD
// 00664C97
// 00664E33
// 00665823
// 00665955
// 0067F77C
// 0067FA0C
}
