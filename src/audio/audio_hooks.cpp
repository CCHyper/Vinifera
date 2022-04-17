/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the new audio driver interface.
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
#include "audio_hooks.h"
#include "audio_driver.h"
#include "audio_util.h"
#include "audio_newtheme.h"
#include "tspp_audio_intercept.h"
#include "vinifera_globals.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "voc.h"
#include "vox.h"
#include "ini.h"
#include "rules.h"
#include "stimer.h"
#include "credits.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Has the new theme engine been hooked?
 */
bool NewThemeClassHooked = false;

extern NewThemeClass NewTheme;


/**
 *  If we are not replacing AHandle, we still need to init DSAudio due to
 *  the VQA player still using DirectSound. This define controls that.
 */
//#define VINIFERA_AUDIO_HOOK_AHANDLE 1


/**
 *  Handy macros for defining the audio driver patches.
 * 
 *  @author: CCHyper
 */
#define AUDIO_ENGINE_INIT_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Driver_Init(MainWindow, AUDIO_DEFAULT_BITS, AUDIO_DEFAULT_STEREO, AUDIO_DEFAULT_RATE, AUDIO_DEFAULT_NUM_TRACKERS); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_END_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Driver_End(); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_IS_AVAILABLE_PATCH(label, return_false_addr, return_true_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        if (Audio_Driver_Is_Available()) { \
            goto return_true; \
        } else { \
            goto return_false; \
        } \
    return_true: \
        _asm { popad } \
        JMP(return_true_addr); \
    return_false: \
        _asm { popad } \
        JMP(return_false_addr); \
    }

#define AUDIO_ENGINE_FOCUS_LOSS_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Driver_Focus_Loss(); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_FOCUS_RESTORE_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Driver_Focus_Restore(); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_STOP_SAMPLE_PATCH(label, handle_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(int, handle, handle_reg); \
        _asm { pushad } \
        Audio_Driver_Stop_Sample(handle); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_SAMPLE_STATUS_PATCH(label, handle_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(int, handle, handle_reg); \
        static bool status; \
        _asm { pushad } \
        status = Audio_Driver_Sample_Status(handle); \
        _asm { popad } \
        _asm { mov al, status } \
        JMP_REG(ecx, ret_addr); \
    }

#define AUDIO_ENGINE_IS_SAMPLE_PLAYING_PATCH(label, sample_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(void *, sample, sample_reg); \
        static bool status; \
        _asm { pushad } \
        status = Audio_Driver_Is_Sample_Playing(sample); \
        _asm { popad } \
        _asm { mov al, status } \
        JMP_REG(ecx, ret_addr); \
    }

#define AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(label, sample_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(void *, sample, sample_reg); \
        _asm { pushad } \
        Audio_Driver_Stop_Sample_Playing(sample); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_PLAY_SAMPLE_PATCH(label, sample_reg, priority_reg, volume_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(void *, sample, sample_reg); \
        GET_REGISTER_STATIC(int, priority, sample_reg); \
        GET_REGISTER_STATIC(int, volume, volume_reg); \
        _asm { pushad } \
        Audio_Driver_Play_Sample(sample, priority, volume); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(label, sample_reg, priority, volume_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(void *, sample, sample_reg); \
        GET_REGISTER_STATIC(int, volume, volume_reg); \
        _asm { pushad } \
        Audio_Driver_Play_Sample(sample, priority, volume); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_FILE_STREAM_SAMPLE_VOL_PATCH(label, filename_reg, volume_reg, real_time_start, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(char *, filename, filename_reg); \
        GET_REGISTER_STATIC(int, volume, volume_reg); \
        static int handle; \
        _asm { pushad } \
        handle = Audio_Driver_Play(filename, volume, real_time_start); \
        _asm { popad } \
        _asm { mov eax, handle } \
        JMP_REG(ecx, ret_addr); \
    }

#define AUDIO_ENGINE_SOUND_CALLBACK_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        Audio_Driver_Sound_Callback(); \
        JMP(ret_addr); \
    }
    
#define AUDIO_ENGINE_SET_VOLUME_ALL_PATCH(label, volume_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(int, volume, volume_reg); \
        _asm { pushad } \
        Audio_Driver_Set_Volume_All(volume); \
        _asm { popad } \
        JMP(ret_addr); \
    }

//#define AUDIO_ENGINE_SET_VOLUME_PERCENT_ALL_PATCH(label, volume_reg, ret_addr)
#define AUDIO_ENGINE_SET_VOLUME_PERCENT_ALL_PATCH(label, volume_percent, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Driver_Set_Volume_Percent_All(volume_percent); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_SET_HANDLE_VOLUME_PATCH(label, handle_reg, volume_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(int, handle, handle_reg); \
        GET_REGISTER_STATIC(int, volume, volume_reg); \
        _asm { pushad } \
        Audio_Driver_Set_Handle_Volume(handle, volume); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_SET_SAMPLE_VOLUME_PATCH(label, sample_reg, volume_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(void *, sample, sample_reg); \
        GET_REGISTER_STATIC(int, volume, volume_reg); \
        _asm { pushad } \
        Audio_Driver_Set_Sample_Volume(sample, volume); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_FADE_SAMPLE_PATCH(label, handle_reg, ticks, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(int, handle, handle_reg); \
        _asm { pushad } \
        Audio_Driver_Fade_Sample(handle, ticks); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_START_PRIMARY_SOUND_BUFFER_PATCH(label, force, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Driver_Start_Primary_Sound_Buffer(force); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_STOP_PRIMARY_SOUND_BUFFER_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Driver_Stop_Primary_Sound_Buffer(); \
        _asm { popad } \
        JMP(ret_addr); \
    }


/**
 *  The following definitions create intercept patches for various audio engine calls.
 */
AUDIO_ENGINE_INIT_PATCH(_WinMain_Init_Audio_1_Patch, 0x006013C8);
AUDIO_ENGINE_INIT_PATCH(_WinMain_Init_Audio_2_Patch, 0x006016B3);

AUDIO_ENGINE_END_PATCH(_WinMain_End_Audio_Patch, 0x00601A1C);
AUDIO_ENGINE_END_PATCH(_Emergency_Exit_End_Audio_Patch, 0x0060248A);

AUDIO_ENGINE_IS_AVAILABLE_PATCH(_Call_Back_Is_Available_Patch_1, 0x00462CA0, 0x00462C7E);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_GameSettingsClass_Is_Available_Patch_1, 0x004AAAFB, 0x004AAADF);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_Init_Bulk_Data_Is_Available_Patch_1, 0x004E465F, 0x004E4626);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_Main_Loop_Is_Available_Patch_1, 0x00508AF4, 0x00508ADF);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_MenuOptionsDialog_Is_Available_Patch_1, 0x0050A773, 0x0050A76C);

DECLARE_PATCH(_MovieClass_Is_Available_Patch_1)
{
    _asm { mov [esp+0x10], ebx }

    _asm { pushad }
    if (Audio_Driver_Is_Available()) {
        goto return_true;
    } else {
        goto return_false;
    }
return_true:
    _asm { popad }
    JMP(0x005641BE);
return_false:
    _asm { popad }
    JMP(0x005641B4);
}

AUDIO_ENGINE_IS_AVAILABLE_PATCH(_MapSelect_Is_Available_Patch_1, 0x0056E7B8, 0x0056E65F);

DECLARE_PATCH(_MapStage_Is_Available_Patch_1)
{
    GET_REGISTER_STATIC(void *, this_ptr, ecx);
    _asm { pushad }
    if (Audio_Driver_Is_Available()) {
        goto return_true;
    } else {
        goto return_false;
    }
return_true:
    _asm { popad }
    _asm { mov ebx, this_ptr }
    JMP(0x0056E9A8);
return_false:
    _asm { popad }
    _asm { mov ebx, this_ptr }
    JMP(0x0056E9D3);
}

AUDIO_ENGINE_IS_AVAILABLE_PATCH(_MSSfxEntry_Is_Available_Patch_1, 0x0056F39C, 0x0056F2AE);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_MSEngine_Is_Available_Patch_1, 0x005714C5, 0x00571480);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_MSFont_Is_Available_Patch_1, 0x005722D7, 0x005721CF);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_MSSfx_Is_Available_Patch_1, 0x00574C8A, 0x00574C0E);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_OwnerDraw_Is_Available_Patch_1, 0x00594295, 0x00594273);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ScoreClass_Is_Available_Patch_1, 0x005E35BA, 0x005E3564);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ScoreClass_Is_Available_Patch_2, 0x005E3706, 0x005E36B0);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ScoreClass_Is_Available_Patch_3, 0x005E3E6B, 0x005E3E15);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ScoreClass_Is_Available_Patch_4, 0x005E4437, 0x005E43E1);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ScoreClass_Is_Available_Patch_5, 0x005E5835, 0x005E57CD);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ScoreClass_Is_Available_Patch_6, 0x005E61EB, 0x005E5FA5);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ScoreClass_Is_Available_Patch_7, 0x005E61C6, 0x005E6170);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ScoreFontClass_Is_Available_Patch_1, 0x005E67E5, 0x005E6790);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ScoreFontClass_Is_Available_Patch_2, 0x005E6A31, 0x005E68ED);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ScoreSoundClass_Is_Available_Patch_1, 0x005E739A, 0x005E731E);

DECLARE_PATCH(_SoundControlsClass_Is_Available_Patch_1)
{
    _asm { mov eax, 0x0080C5E8 }
    _asm { mov [eax], 0 }

    _asm { pushad }
    if (Audio_Driver_Is_Available()) {
        goto return_true;
    } else {
        goto return_false;
    }
return_true:
    _asm { popad }
    _asm { mov byte ptr [esp+0x84], 1 }
    JMP(0x005FC55A);
return_false:
    _asm { popad }
    _asm { mov byte ptr [esp+0x84], 0 }
    JMP(0x005FC552);
}

AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ThemeClass_Scan_Is_Available_Patch_1, 0x00643D35, 0x00643CA1);

DECLARE_PATCH(_ThemeClass_AI_Is_Available_Patch_1)
{
    _asm { push esi }
    _asm { mov esi, ecx }

    _asm { pushad }
    if (Audio_Driver_Is_Available()) {
        goto return_true;
    } else {
        goto return_false;
    }
return_true:
    _asm { popad }
    JMP_REG(edx, 0x00643DDE);
return_false:
    _asm { popad }
    JMP_REG(edx, 0x00643E6F);
}

DECLARE_PATCH(_ThemeClass_AI_Is_Available_Patch_2)
{
    _asm { pushad }
    if (Audio_Driver_Is_Available()) {
        goto return_true;
    } else {
        goto return_false;
    }
return_true:
    _asm { popad }
    JMP(0x00643E05);
return_false:
    _asm { popad }
    JMP(0x00643E1B);
}

AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ThemeClass_Queue_Song_Is_Available_Patch_1, 0x00643FDB, 0x00643F4A);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ThemeClass_Queue_Song_Is_Available_Patch_2, 0x00643FDB, 0x00643FA5);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ThemeClass_Play_Song_Is_Available_Patch_1, 0x00644118, 0x0064400D);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ThemeClass_Stop_Is_Available_Patch_1, 0x0064424C, 0x006441BC);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ThemeClass_Suspend_Is_Available_Patch_1, 0x006442AD, 0x0064426E);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_ThemeClass_Still_Playing_Is_Available_Patch_1, 0x006442EE, 0x006442CB);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_VocClass_Is_Available_Patch_1, 0x00664AEF, 0x00664A90);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_VocClass_Is_Available_Patch_2, 0x00664B93, 0x00664B3A);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_VocClass_Is_Available_Patch_3, 0x00664C4E, 0x00664BEF);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_VocClass_Is_Available_Patch_4, 0x00664D02, 0x00664CA9);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_VocClass_Is_Available_Patch_5, 0x00664EA5, 0x00664E45);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_Speak_Is_Available_Patch_1, 0x00665935, 0x0066583D);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_Speak_AI_Is_Available_Patch_1, 0x00665AE3, 0x0066596F); 
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_Is_Speaking_Is_Available_Patch_1, 0x00665B67, 0x00665B40);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_Set_Speech_Volume_Is_Available_Patch_1, 0x00665BB6, 0x00665B9E);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_WorldDominationTour_Is_Available_Patch_1, 0x0067F986, 0x0067F796);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_WorldDominationTour_Is_Available_Patch_2, 0x0067FBCE, 0x0067FA26);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_Focus_Restore_Is_Available_Patch_1, 0x00685A2B, 0x00685A1F);
AUDIO_ENGINE_IS_AVAILABLE_PATCH(_Main_Window_Procedure_Is_Available_Patch_1, 0x00685ED5, 0x00685ECB);

//AUDIO_ENGINE_STOP_SAMPLE_PATCH(_ThemeClass_Play_Song_Stop_Sample_Patch_1, ecx, 0x00644048);
//AUDIO_ENGINE_STOP_SAMPLE_PATCH(_ThemeClass_Stop_Stop_Sample_Patch_1, eax, 0x00644238);
//AUDIO_ENGINE_STOP_SAMPLE_PATCH(_ThemeClass_Suspend_Stop_Sample_Patch_1, ecx, 0x0064429A);
//AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_ThemeClass_AI_Sample_Status_Patch_1, eax, 0x00643E17);
//AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_ThemeClass_Queue_Song_Sample_Status_Patch_2, eax, 0x00643FC8);
//AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_ThemeClass_Stop_Sample_Status_Patch_3, eax, 0x006441F4);
//AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_ThemeClass_Still_Playing_Sample_Status_Patch_4, ecx, 0x006442ED);
// 
//AUDIO_ENGINE_FILE_STREAM_SAMPLE_VOL_PATCH(_ThemeClass_Play_Song_File_Stream_Sample_Vol_Patch_1, eax, ecx, true, 0x006440C6);

AUDIO_ENGINE_SOUND_CALLBACK_PATCH(_Call_Back_Sound_Callback_Patch_1, 0x00462C91);
AUDIO_ENGINE_SOUND_CALLBACK_PATCH(_OwnerDraw_Window_Procedure_Sound_Callback_Patch_1, 0x00594286);
AUDIO_ENGINE_SOUND_CALLBACK_PATCH(_ThemeClass_AI_Sound_Callback_Patch_1, 0x00643E6F);

//AUDIO_ENGINE_SET_HANDLE_VOLUME_PATCH(_ThemeClass_Set_Volume_Set_Handle_Volume_Patch_1, ecx, eax, 0x00644436);

//AUDIO_ENGINE_FADE_SAMPLE_PATCH(_ThemeClass_Queue_Song_Fade_Sample_Patch_1, eax, 60, 0x00643FDB);
//AUDIO_ENGINE_FADE_SAMPLE_PATCH(_ThemeClass_Stop_Fade_Sample_Patch_1, ecx, 60, 0x00644218);

AUDIO_ENGINE_FOCUS_RESTORE_PATCH(_Focus_Restore_Start_Primary_Sound_Buffer_Patch_1, 0x00685A2B);

AUDIO_ENGINE_FOCUS_LOSS_PATCH(_Focus_Loss_Stop_Primary_Sound_Buffer_Patch_1, 0x006859C4);
AUDIO_ENGINE_FOCUS_LOSS_PATCH(_Main_Window_Procedure_Stop_Primary_Sound_Buffer_Patch_1, 0x00685ED5);

DECLARE_PATCH(_Create_Main_Window_Set_Focus_Loss_Function)
{
    //Audio_Driver_Set_Focus_Loss_Function(&Focus_Loss);
    JMP(0x00686330);
}

//DECLARE_PATCH(_ThemeClass_Play_Song_Set_Low_Impact_1)
//{
//    Audio_Driver_Set_Stream_Low_Impact(true);
//    JMP(0x00644086);
//}

//DECLARE_PATCH(_ThemeClass_Play_Song_Set_Low_Impact_2)
//{
//    Audio_Driver_Set_Stream_Low_Impact(false);
//    JMP(0x006440CF);
//}


/**
 *  Forces a playback limit on the credits tick sound, otherwise it runs wild.
 * 
 *  @author: CCHyper
 */
//bool Fix_Credit_Tick_Sfx = false;
DECLARE_PATCH(_CreditsClass_Graphic_Logic_Limit_Sfx_Patch)
{
    GET_REGISTER_STATIC(CreditClass *, this_ptr, 0x00471493);
    static CDTimerClass<MSTimerClass> _delay = 35;

    if (/*!Fix_Credit_Tick_Sfx ||*/ _delay.Expired()) {
        if (this_ptr->IsUp) {
            Sound_Effect(Rule->CreditTicks[0], 0, 0.5f);
        } else {
            Sound_Effect(Rule->CreditTicks[1], 0, 0.5f);
        }
        _delay = 35;
        _delay.Start();
    }

    JMP(0x004714CC);
}


/**
 *  Patch in the new theme handler.
 */
void NewTheme_Hooks()
{
#ifndef NDEBUG
    DEV_DEBUG_INFO("Patching in NewThemeClass.\n");
#endif

    Patch_Call(0x004E0AF0, &NewThemeClass::Process);
    Patch_Call(0x004E0ADE, &NewThemeClass::Clear);
    Patch_Call(0x00601C83, &NewThemeClass::Clear);
    Patch_Call(0x0044ECBB, &NewThemeClass::Scan);
    Patch_Call(0x004E0AFA, &NewThemeClass::Scan);
    Patch_Call(0x00643C70, &NewThemeClass::Scan);
    Patch_Call(0x0044B3FD, &NewThemeClass::Base_Name);
    Patch_Call(0x005FC77C, &NewThemeClass::Full_Name);
    Patch_Call(0x00462C96, &NewThemeClass::AI);
    Patch_Call(0x0059428B, &NewThemeClass::AI);
    Patch_Call(0x0044E954, &NewThemeClass::Queue_Song);
    Patch_Call(0x00491617, &NewThemeClass::Queue_Song);
    Patch_Call(0x00491A6B, &NewThemeClass::Queue_Song);
    Patch_Call(0x00491AFC, &NewThemeClass::Queue_Song);
    Patch_Call(0x004B7A85, &NewThemeClass::Queue_Song);
    Patch_Call(0x004E28C2, &NewThemeClass::Queue_Song);
    Patch_Call(0x004E29C3, &NewThemeClass::Queue_Song);
    Patch_Call(0x00508AEF, &NewThemeClass::Queue_Song);
    Patch_Call(0x0056454E, &NewThemeClass::Queue_Song);
    Patch_Call(0x005DB40B, &NewThemeClass::Queue_Song);
    Patch_Call(0x005DC966, &NewThemeClass::Queue_Song);
    Patch_Call(0x005DCCB2, &NewThemeClass::Queue_Song);
    Patch_Call(0x005DCEE1, &NewThemeClass::Queue_Song);
    Patch_Call(0x005DCFEF, &NewThemeClass::Queue_Song);
    Patch_Call(0x005E4D08, &NewThemeClass::Queue_Song);
    Patch_Call(0x005FC343, &NewThemeClass::Queue_Song);
    Patch_Call(0x005FC4BB, &NewThemeClass::Queue_Song);
    Patch_Call(0x00619F07, &NewThemeClass::Queue_Song);
    Patch_Call(0x0061C039, &NewThemeClass::Queue_Song);
    Patch_Call(0x00622EBC, &NewThemeClass::Queue_Song);
    Patch_Call(0x006268A0, &NewThemeClass::Queue_Song);
    Patch_Call(0x0067A041, &NewThemeClass::Queue_Song);
    Patch_Call(0x0067CA72, &NewThemeClass::Queue_Song);
    Patch_Call(0x00687F1E, &NewThemeClass::Queue_Song);
    Patch_Call(0x00687F5F, &NewThemeClass::Queue_Song);
    Patch_Call(0x00687F6D, &NewThemeClass::Queue_Song);
    Patch_Call(0x0068C0C9, &NewThemeClass::Queue_Song);
    Patch_Call(0x0068C0E4, &NewThemeClass::Queue_Song);
    Patch_Call(0x004B779D, &NewThemeClass::Play_Song);
    Patch_Call(0x004E1FA2, &NewThemeClass::Play_Song);
    Patch_Call(0x004E25D8, &NewThemeClass::Play_Song);
    Patch_Call(0x004ECC28, &NewThemeClass::Play_Song);
    Patch_Call(0x004ECE8B, &NewThemeClass::Play_Song);
    Patch_Call(0x005538C8, &NewThemeClass::Play_Song);
    Patch_Call(0x005C034D, &NewThemeClass::Play_Song);
    Patch_Call(0x005DB3FD, &NewThemeClass::Play_Song);
    Patch_Call(0x005E2F36, &NewThemeClass::Play_Song);
    Patch_Call(0x0067C9D3, &NewThemeClass::Play_Song);
    Patch_Call(0x00685B90, &NewThemeClass::Play_Song);
    Patch_Call(0x005FC76F, &NewThemeClass::Track_Length);
    Patch_Call(0x0044E887, &NewThemeClass::Stop);
    Patch_Call(0x0044E9A7, &NewThemeClass::Stop);
    Patch_Call(0x0049154A, &NewThemeClass::Stop);
    Patch_Call(0x00491BD6, &NewThemeClass::Stop);
    Patch_Call(0x004E26CC, &NewThemeClass::Stop);
    Patch_Call(0x004E2759, &NewThemeClass::Stop);
    Patch_Call(0x004E2A47, &NewThemeClass::Stop);
    Patch_Call(0x004E2ADE, &NewThemeClass::Stop);
    Patch_Call(0x004E2B5B, &NewThemeClass::Stop);
    Patch_Call(0x004E2B77, &NewThemeClass::Stop);
    Patch_Call(0x004E2C49, &NewThemeClass::Stop);
    Patch_Call(0x004E2DAD, &NewThemeClass::Stop);
    Patch_Call(0x004E2DF5, &NewThemeClass::Stop);
    Patch_Call(0x004E2E7B, &NewThemeClass::Stop);
    Patch_Call(0x004ECA23, &NewThemeClass::Stop);
    Patch_Call(0x004ECD18, &NewThemeClass::Stop);
    Patch_Call(0x00553C03, &NewThemeClass::Stop);
    Patch_Call(0x005C0330, &NewThemeClass::Stop);
    Patch_Call(0x005DB221, &NewThemeClass::Stop);
    Patch_Call(0x005DB343, &NewThemeClass::Stop);
    Patch_Call(0x005E2F1C, &NewThemeClass::Stop);
    Patch_Call(0x005E4B5B, &NewThemeClass::Stop);
    Patch_Call(0x005FC338, &NewThemeClass::Stop);
    Patch_Call(0x00687F12, &NewThemeClass::Stop);
    Patch_Call(0x00687F53, &NewThemeClass::Stop);
    Patch_Call(0x006859A3, &NewThemeClass::Suspend);
    Patch_Call(0x00685EB4, &NewThemeClass::Suspend);
    Patch_Call(0x00491A5B, &NewThemeClass::Still_Playing);
    Patch_Call(0x004E2B10, &NewThemeClass::Still_Playing);
    Patch_Call(0x004E2B4B, &NewThemeClass::Still_Playing);
    Patch_Call(0x004E2E27, &NewThemeClass::Still_Playing);
    Patch_Call(0x004E2E6B, &NewThemeClass::Still_Playing);
    Patch_Call(0x00589AD6, &NewThemeClass::Still_Playing);
    Patch_Call(0x00687F02, &NewThemeClass::Still_Playing);
    Patch_Call(0x005FC75C, &NewThemeClass::Is_Allowed);
    Patch_Call(0x0044B3C5, &NewThemeClass::From_Name);
    Patch_Call(0x0049160C, &NewThemeClass::From_Name);
    Patch_Call(0x004B7792, &NewThemeClass::From_Name);
    Patch_Call(0x004E1F83, &NewThemeClass::From_Name);
    Patch_Call(0x004E1F97, &NewThemeClass::From_Name);
    Patch_Call(0x004E25B9, &NewThemeClass::From_Name);
    Patch_Call(0x004E25CD, &NewThemeClass::From_Name);
    Patch_Call(0x004E28A3, &NewThemeClass::From_Name);
    Patch_Call(0x004E28B7, &NewThemeClass::From_Name);
    Patch_Call(0x004E29A4, &NewThemeClass::From_Name);
    Patch_Call(0x004E29B8, &NewThemeClass::From_Name);
    Patch_Call(0x004E8708, &NewThemeClass::From_Name);
    Patch_Call(0x004E871C, &NewThemeClass::From_Name);
    Patch_Call(0x004E8748, &NewThemeClass::From_Name);
    Patch_Call(0x004E875C, &NewThemeClass::From_Name);
    Patch_Call(0x004ECC1D, &NewThemeClass::From_Name);
    Patch_Call(0x005538BD, &NewThemeClass::From_Name);
    Patch_Call(0x005E2F2B, &NewThemeClass::From_Name);
    Patch_Call(0x0067C9C8, &NewThemeClass::From_Name);
    Patch_Call(0x00589AC4, &NewThemeClass::Set_Volume);
    Patch_Call(0x00589FE3, &NewThemeClass::Set_Volume);
    Patch_Call(0x0058A580, &NewThemeClass::Set_Volume);
    
    Patch_Dword(0x0044B3C0+1, (uintptr_t)&NewTheme); // CCINIClass::Get_ThemeType
    Patch_Dword(0x0044B3F8+1, (uintptr_t)&NewTheme); // CCINIClass::Put_ThemeType
    Patch_Dword(0x0044E882+1, (uintptr_t)&NewTheme); // CD::Insert_Disk
    Patch_Dword(0x0044E94F+1, (uintptr_t)&NewTheme); // CD::Insert_Disk
    Patch_Dword(0x0044E9A1+1, (uintptr_t)&NewTheme); // CD::Init_Swap
    Patch_Dword(0x0044ECB6+1, (uintptr_t)&NewTheme); // CD::Init_Swap
    Patch_Dword(0x00462C91+1, (uintptr_t)&NewTheme); // Call_Back
    Patch_Dword(0x00491545+1, (uintptr_t)&NewTheme); // Slide_Show
    Patch_Dword(0x00491607+1, (uintptr_t)&NewTheme); // Slide_Show
    Patch_Dword(0x00491611+1, (uintptr_t)&NewTheme); // Slide_Show
    Patch_Dword(0x00491A56+1, (uintptr_t)&NewTheme); // Slide_Show
    Patch_Dword(0x00491A66+1, (uintptr_t)&NewTheme); // Slide_Show
    Patch_Dword(0x00491AF7+1, (uintptr_t)&NewTheme); // Slide_Show
    Patch_Dword(0x00491BC6+1, (uintptr_t)&NewTheme); // Slide_Show
    Patch_Dword(0x004B7789+1, (uintptr_t)&NewTheme); // GraphicMenu_Process_Loop
    Patch_Dword(0x004B7798+1, (uintptr_t)&NewTheme); // GraphicMenu_Process_Loop
    Patch_Dword(0x004B7A80+1, (uintptr_t)&NewTheme); // GraphicMenu_Process_Loop
    Patch_Dword(0x004E0AD9+1, (uintptr_t)&NewTheme); // Init_Game
    Patch_Dword(0x004E0AEA+1, (uintptr_t)&NewTheme); // Init_Game
    Patch_Dword(0x004E0AF5+1, (uintptr_t)&NewTheme); // Init_Game
    Patch_Dword(0x004E1F7E+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E1F92+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E1F9D+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E25B4+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E25C8+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E25D3+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E26B8+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2754+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E289E+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E28B2+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E28BD+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E299F+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E29B3+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E29BE+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2A40+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2AD9+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2B05+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2B46+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2B56+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2B72+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2C44+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2DA8+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2DF0+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2E1C+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2E66+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E2E76+1, (uintptr_t)&NewTheme); // Select_Game
    Patch_Dword(0x004E452D+1, (uintptr_t)&NewTheme); // Init_Secondary_Mixfiles
    Patch_Dword(0x004E8703+1, (uintptr_t)&NewTheme); // Get_Menu_Theme
    Patch_Dword(0x004E8717+1, (uintptr_t)&NewTheme); // Get_Menu_Theme
    Patch_Dword(0x004E8743+1, (uintptr_t)&NewTheme); // Get_MapSelect_Theme
    Patch_Dword(0x004E8757+1, (uintptr_t)&NewTheme); // Get_MapSelect_Theme
    Patch_Dword(0x004ECA0D+1, (uintptr_t)&NewTheme); // IonStorm_Start
    Patch_Dword(0x004ECC13+1, (uintptr_t)&NewTheme); // IonStorm_Start
    Patch_Dword(0x004ECC22+1, (uintptr_t)&NewTheme); // IonStorm_Start
    Patch_Dword(0x004ECD13+1, (uintptr_t)&NewTheme); // IonStorm_Stop
    Patch_Dword(0x004ECE86+1, (uintptr_t)&NewTheme); // IonStorm_Stop
    Patch_Dword(0x00508AEA+1, (uintptr_t)&NewTheme); // Main_Loop
    Patch_Dword(0x005538B8+1, (uintptr_t)&NewTheme); // MapSelect
    Patch_Dword(0x005538C3+1, (uintptr_t)&NewTheme); // MapSelect
    Patch_Dword(0x00553BFE+1, (uintptr_t)&NewTheme); // MapSelect
    Patch_Dword(0x00564549+1, (uintptr_t)&NewTheme); // Movie_Play
    Patch_Dword(0x00589ABF+1, (uintptr_t)&NewTheme); // OptionsClass::Set_Score_Volume
    Patch_Dword(0x00589AD1+1, (uintptr_t)&NewTheme); // OptionsClass::Set_Score_Volume
    Patch_Dword(0x00589FDE+1, (uintptr_t)&NewTheme); // OptionsClass::Load_Settings
    Patch_Dword(0x0058A57B+1, (uintptr_t)&NewTheme); // OptionsClass::Set
    Patch_Dword(0x00594286+1, (uintptr_t)&NewTheme); // OwnerDraw_Window_Procedure
    Patch_Dword(0x005C032B+1, (uintptr_t)&NewTheme); // Restate_Mission
    Patch_Dword(0x005C0347+1, (uintptr_t)&NewTheme); // Restate_Mission
    Patch_Dword(0x005DB21A+1, (uintptr_t)&NewTheme); // Start_Scenario
    Patch_Dword(0x005DB33E+1, (uintptr_t)&NewTheme); // Start_Scenario
    Patch_Dword(0x005DB3F8+1, (uintptr_t)&NewTheme); // Start_Scenario
    Patch_Dword(0x005DB406+1, (uintptr_t)&NewTheme); // Start_Scenario
    Patch_Dword(0x005DC95F+1, (uintptr_t)&NewTheme); // Do_Win
    Patch_Dword(0x005DCCAB+1, (uintptr_t)&NewTheme); // Do_Lose
    Patch_Dword(0x005DCED4+1, (uintptr_t)&NewTheme); // Do_Restart
    Patch_Dword(0x005DCFE8+1, (uintptr_t)&NewTheme); // Do_Abort
    Patch_Dword(0x005E2F17+1, (uintptr_t)&NewTheme); // ScoreClass::Presentation
    Patch_Dword(0x005E2F26+1, (uintptr_t)&NewTheme); // ScoreClass::Presentation
    Patch_Dword(0x005E2F31+1, (uintptr_t)&NewTheme); // ScoreClass::Presentation
    Patch_Dword(0x005E4B56+1, (uintptr_t)&NewTheme); // ScoreClass::Presentation
    Patch_Dword(0x005E4D03+1, (uintptr_t)&NewTheme); // ScoreClass::Presentation
    Patch_Dword(0x005FC331+1, (uintptr_t)&NewTheme); // SoundControlsClass::Process
    Patch_Dword(0x005FC33E+1, (uintptr_t)&NewTheme); // SoundControlsClass::Process
    Patch_Dword(0x005FC4B6+1, (uintptr_t)&NewTheme); // SoundControlsClass::Process
    Patch_Dword(0x005FC757+1, (uintptr_t)&NewTheme); // SoundControlsClass::Process
    Patch_Dword(0x005FC76A+1, (uintptr_t)&NewTheme); // SoundControlsClass::Process
    Patch_Dword(0x005FC775+1, (uintptr_t)&NewTheme); // SoundControlsClass::Process
    Patch_Dword(0x00601C72+1, (uintptr_t)&NewTheme); // Game_Shutdown
    Patch_Dword(0x00619F02+1, (uintptr_t)&NewTheme); // TActionClass::operator()
    Patch_Dword(0x0061C033+1, (uintptr_t)&NewTheme); // TActionClass::TAction_Play_Music
    Patch_Dword(0x00622EB7+1, (uintptr_t)&NewTheme); // TeamClass::AI
    Patch_Dword(0x0062689B+1, (uintptr_t)&NewTheme); // TeamClass::TMission_Play_Music
    Patch_Dword(0x0067A03C+1, (uintptr_t)&NewTheme); // WorldDominationTour::Selection::DTOR
    Patch_Dword(0x0067C9C3+1, (uintptr_t)&NewTheme); // WDT_Init_Menu
    Patch_Dword(0x0067C9CE+1, (uintptr_t)&NewTheme); // WDT_Init_Menu
    Patch_Dword(0x0067CA6D+1, (uintptr_t)&NewTheme); // WDT
    Patch_Dword(0x00685999+1, (uintptr_t)&NewTheme); // Focus_Loss
    Patch_Dword(0x00685B8A+1, (uintptr_t)&NewTheme); // Focus_Restore
    Patch_Dword(0x00685EAA+1, (uintptr_t)&NewTheme); // Main_Window_Procedure
    Patch_Dword(0x00687EFD+1, (uintptr_t)&NewTheme); // WOL
    Patch_Dword(0x00687F0D+1, (uintptr_t)&NewTheme); // WOL
    Patch_Dword(0x00687F17+1, (uintptr_t)&NewTheme); // WOL
    Patch_Dword(0x00687F4E+1, (uintptr_t)&NewTheme); // WOL
    Patch_Dword(0x00687F58+1, (uintptr_t)&NewTheme); // WOL
    Patch_Dword(0x00687F68+1, (uintptr_t)&NewTheme); // WOL
    Patch_Dword(0x0068C0C4+1, (uintptr_t)&NewTheme); // WOL_Start
    Patch_Dword(0x0068C0DF+1, (uintptr_t)&NewTheme); // WOL_Start

    Patch_Dword(0x0044E877+2, (uintptr_t)&NewTheme.Score); // CD::Insert_Disk
    Patch_Dword(0x0044E997+1, (uintptr_t)&NewTheme.Score); // CD::Init_Swap
    Patch_Dword(0x004ECA19+1, (uintptr_t)&NewTheme.Score); // IonStorm_Start
    Patch_Dword(0x00508ADF+2, (uintptr_t)&NewTheme.Score); // Main_Loop
    Patch_Dword(0x005C0324+2, (uintptr_t)&NewTheme.Score); // Restate_Mission
    Patch_Dword(0x005FC7E8+2, (uintptr_t)&NewTheme.Score); // SoundControlsClass::Process
    Patch_Dword(0x00685994+1, (uintptr_t)&NewTheme.Score); // Focus_Loss
    Patch_Dword(0x00685EA5+1, (uintptr_t)&NewTheme.Score); // Main_Window_Procedure
    Patch_Dword(0x00687F2E+2, (uintptr_t)&NewTheme.Score); // WOL
    Patch_Dword(0x00687F3D+2, (uintptr_t)&NewTheme.Score); // WOL
    Patch_Dword(0x0068C09B+2, (uintptr_t)&NewTheme.Score); // WOL start
    Patch_Dword(0x0068C0CE+2, (uintptr_t)&NewTheme.Score); // WOL start

    Patch_Dword(0x00589A67+1, (uintptr_t)&NewTheme.IsRepeat); // OptionsClass::Set_Repeat
    Patch_Dword(0x0058A019+1, (uintptr_t)&NewTheme.IsRepeat); // OptionsClass::Load_Settings
    Patch_Dword(0x0058A59E+1, (uintptr_t)&NewTheme.IsRepeat); // OptionsClass::Set
    Patch_Dword(0x0067C9D8+2, (uintptr_t)&NewTheme.IsRepeat); // WDT init menu

    Patch_Dword(0x00589A37+1, (uintptr_t)&NewTheme.IsShuffle); // OptionsClass::Set_Shuffle
    Patch_Dword(0x0058A058+1, (uintptr_t)&NewTheme.IsShuffle); // OptionsClass::Load_Settings
    Patch_Dword(0x0058A5C5+1, (uintptr_t)&NewTheme.IsShuffle); // OptionsClass::Set
    Patch_Dword(0x0068C99C+1, (uintptr_t)&NewTheme.IsShuffle); // Join_WOL_Lobby
    Patch_Dword(0x0068C9A1+2, (uintptr_t)&NewTheme.IsShuffle); // Join_WOL_Lobby
    Patch_Dword(0x0068CFBD+2, (uintptr_t)&NewTheme.IsShuffle); // Join_WOL_Lobby

    /**
     *  Fixes a bug where themes would restart ofter focus loss and regain. The
     *  new audio driver system handles the focus loss and gain, so this is no
     *  longer required.
     * 
     *  @author: CCHyper
     */
    Patch_Jump(0x00685994, 0x006859A8);
    Patch_Jump(0x00685B84, 0x00685B95);

    NewThemeClassHooked = true;
}


/**
 *  Main function for patching the hooks.
 */
void Audio_Hooks()
{
    /**
     *  Install the TS++ wrappers.
     */
    //TSPP_Play_Sample_Function_Ptr = Audio_Driver_Play_Sample;
    //TSPP_Is_Sample_Playing_Function_Ptr = Audio_Driver_Is_Sample_Playing;

    /**
     *  Hook the audio interface into the game.
     */
    Patch_Jump(0x006013B0, &_WinMain_Init_Audio_1_Patch);
    Patch_Jump(0x0060169B, &_WinMain_Init_Audio_2_Patch);

    Patch_Jump(0x00601A12, &_WinMain_End_Audio_Patch);
    Patch_Jump(0x00602480, &_Emergency_Exit_End_Audio_Patch);

    Patch_Jump(0x00462C6C, &_Call_Back_Is_Available_Patch_1);
    Patch_Jump(0x004AAACB, &_GameSettingsClass_Is_Available_Patch_1);
    Patch_Jump(0x004E4615, &_Init_Bulk_Data_Is_Available_Patch_1);
    Patch_Jump(0x00508ACD, &_Main_Loop_Is_Available_Patch_1);
    Patch_Jump(0x0050A758, &_MenuOptionsDialog_Is_Available_Patch_1);
    Patch_Jump(0x0056419F, &_MovieClass_Is_Available_Patch_1);
    Patch_Jump(0x0056E646, &_MapSelect_Is_Available_Patch_1);
    Patch_Jump(0x0056E99B, &_MapStage_Is_Available_Patch_1);
    Patch_Jump(0x0056F296, &_MSSfxEntry_Is_Available_Patch_1);
    Patch_Jump(0x0057146E, &_MSEngine_Is_Available_Patch_1);
    Patch_Jump(0x005721B7, &_MSFont_Is_Available_Patch_1);
    Patch_Jump(0x00574BF8, &_MSSfx_Is_Available_Patch_1);
    Patch_Jump(0x00594261, &_OwnerDraw_Is_Available_Patch_1);
    Patch_Jump(0x005E3552, &_ScoreClass_Is_Available_Patch_1);
    Patch_Jump(0x005E369E, &_ScoreClass_Is_Available_Patch_2);
    Patch_Jump(0x005E3E03, &_ScoreClass_Is_Available_Patch_3);
    Patch_Jump(0x005E43CF, &_ScoreClass_Is_Available_Patch_4);
    Patch_Jump(0x005E57BB, &_ScoreClass_Is_Available_Patch_5);
    Patch_Jump(0x005E5F8B, &_ScoreClass_Is_Available_Patch_6);
    Patch_Jump(0x005E615E, &_ScoreClass_Is_Available_Patch_7);
    Patch_Jump(0x005E677E, &_ScoreFontClass_Is_Available_Patch_1);
    Patch_Jump(0x005E68D8, &_ScoreFontClass_Is_Available_Patch_2);
    Patch_Jump(0x005E7308, &_ScoreSoundClass_Is_Available_Patch_1);
    Patch_Jump(0x005FC531, &_SoundControlsClass_Is_Available_Patch_1);
    //Patch_Jump(0x00643C87, &_ThemeClass_Scan_Is_Available_Patch_1);
    //Patch_Jump(0x00643DC0, &_ThemeClass_AI_Is_Available_Patch_1);
    //Patch_Jump(0x00643DFD, &_ThemeClass_AI_Is_Available_Patch_2);
    //Patch_Jump(0x00643F30, &_ThemeClass_Queue_Song_Is_Available_Patch_1);
    //Patch_Jump(0x00643F93, &_ThemeClass_Queue_Song_Is_Available_Patch_2);
    //Patch_Jump(0x00643FF2, &_ThemeClass_Play_Song_Is_Available_Patch_1);
    //Patch_Jump(0x006441A0, &_ThemeClass_Stop_Is_Available_Patch_1);
    //Patch_Jump(0x0064425C, &_ThemeClass_Suspend_Is_Available_Patch_1);
    //Patch_Jump(0x006442B9, &_ThemeClass_Still_Playing_Is_Available_Patch_1);
    Patch_Jump(0x00664A7E, &_VocClass_Is_Available_Patch_1);
    Patch_Jump(0x00664B28, &_VocClass_Is_Available_Patch_2);
    Patch_Jump(0x00664BDD, &_VocClass_Is_Available_Patch_3);
    Patch_Jump(0x00664C97, &_VocClass_Is_Available_Patch_4);
    Patch_Jump(0x00664E33, &_VocClass_Is_Available_Patch_5);
    Patch_Jump(0x00665823, &_Speak_Is_Available_Patch_1);
    Patch_Jump(0x00665955, &_Speak_AI_Is_Available_Patch_1);
    Patch_Jump(0x00665B2E, &_Is_Speaking_Is_Available_Patch_1);
    Patch_Jump(0x00665B8C, &_Set_Speech_Volume_Is_Available_Patch_1);
    Patch_Jump(0x0067F77C, &_WorldDominationTour_Is_Available_Patch_1);
    Patch_Jump(0x0067FA0C, &_WorldDominationTour_Is_Available_Patch_2);
    Patch_Jump(0x00685A12, &_Focus_Restore_Is_Available_Patch_1);
    Patch_Jump(0x00685EB9, &_Main_Window_Procedure_Is_Available_Patch_1);

    //Patch_Jump(0x0064403D, &_ThemeClass_Play_Song_Stop_Sample_Patch_1);
    //Patch_Jump(0x0064422D, &_ThemeClass_Stop_Stop_Sample_Patch_1);
    //Patch_Jump(0x0064428F, &_ThemeClass_Suspend_Stop_Sample_Patch_1);

    //Patch_Jump(0x00643E0C, &_ThemeClass_AI_Sample_Status_Patch_1);
    //Patch_Jump(0x00643FBD, &_ThemeClass_Queue_Song_Sample_Status_Patch_2);
    //Patch_Jump(0x006441E9, &_ThemeClass_Stop_Sample_Status_Patch_3);
    //Patch_Jump(0x006442E2, &_ThemeClass_Still_Playing_Sample_Status_Patch_4);

    //Patch_Jump(0x006440B9, &_ThemeClass_Play_Song_File_Stream_Sample_Vol_Patch_1);

    Patch_Jump(0x00462C87, &_Call_Back_Sound_Callback_Patch_1);
    Patch_Jump(0x0059427C, &_OwnerDraw_Window_Procedure_Sound_Callback_Patch_1);
    Patch_Jump(0x00643E65, &_ThemeClass_AI_Sound_Callback_Patch_1);

    //Patch_Jump(0x0064442A, &_ThemeClass_Set_Volume_Set_Handle_Volume_Patch_1);

    //Patch_Jump(0x00643FCE, &_ThemeClass_Queue_Song_Fade_Sample_Patch_1);
    //Patch_Jump(0x0064420B, &_ThemeClass_Stop_Fade_Sample_Patch_1);

    Patch_Jump(0x00685A1F, &_Focus_Restore_Start_Primary_Sound_Buffer_Patch_1);

    Patch_Jump(0x006859BA, &_Focus_Loss_Stop_Primary_Sound_Buffer_Patch_1);
    Patch_Jump(0x00685ECB, &_Main_Window_Procedure_Stop_Primary_Sound_Buffer_Patch_1);

    Patch_Jump(0x00686326, &_Create_Main_Window_Set_Focus_Loss_Function);

    //Patch_Jump(0x00644080, &_ThemeClass_Play_Song_Set_Low_Impact_1);
    //Patch_Jump(0x006440C8, &_ThemeClass_Play_Song_Set_Low_Impact_2);

#ifdef VINIFERA_AUDIO_HOOK_AHANDLE
    /**
     *  Hooks for the audio driver interface for the VQA player (AHandle).
     */
#endif

    /**
     *  #BUGFIX: To normalise the credits tick sound if required.
     */
    Patch_Jump(0x00471493, &_CreditsClass_Graphic_Logic_Limit_Sfx_Patch);
}
