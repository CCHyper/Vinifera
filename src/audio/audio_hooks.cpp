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
#ifdef USE_MINIAUDIO

#include "audio_hooks.h"
#include "audio_util.h"
#include "audio_theme.h"
#include "audio_vox.h"
#include "audio_voc.h"
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


extern AudioThemeClass AudioTheme;


extern AudioVocClass *VocEmblem;


#ifdef USE_MINIAUDIO
//#define VINIFERA_REMOVE_DIRECTSOUND 1
//#define VINIFERA_REMOVE_VQA_AHANDLE 1
#endif


/**
 *  These wrappers are required due to the stack being used
 *  when calling the Audio static function.
 * 
 *  Also, these handle DirectSound if it is required to be
 *  enabled alongside the new audio engine (for VQA playback).
 */
static bool Audio_Init(HWND hWnd)
{
    return
#ifndef VINIFERA_REMOVE_DIRECTSOUND
           Audio.Init(hWnd, 16, false, 22050) &&
#endif
           AudioManager.Init(hWnd);
}

static void Audio_End()
{
#ifndef VINIFERA_REMOVE_DIRECTSOUND
    Audio.End();
#endif
    AudioManager.End();
}

static bool Audio_Is_Available()
{
    return
#ifndef VINIFERA_REMOVE_DIRECTSOUND
           Audio.Is_Available() &&
#endif
           AudioManager.Is_Available();
}

static bool Audio_Is_Enabled()
{
    return
#ifndef VINIFERA_REMOVE_DIRECTSOUND
        Audio.Is_Available() &&
#endif
        AudioManager.Is_Enabled();
}

static void Audio_Enable()
{
#ifndef VINIFERA_REMOVE_DIRECTSOUND
    Audio.Start_Primary_Sound_Buffer();
#endif
    AudioManager.Enable();
}

static void Audio_Disable()
{
#ifndef VINIFERA_REMOVE_DIRECTSOUND
    Audio.Stop_Primary_Sound_Buffer();
#endif
    AudioManager.Disable();
}

static void Audio_Focus_Loss()
{
#ifndef VINIFERA_REMOVE_DIRECTSOUND
    Audio.Start_Primary_Sound_Buffer(true);
#endif
    AudioManager.Focus_Loss();
}

static void Audio_Focus_Restore()
{
#ifndef VINIFERA_REMOVE_DIRECTSOUND
    Audio.Stop_Primary_Sound_Buffer();
#endif
    AudioManager.Focus_Restore();
}

static bool Audio_Start_Engine(bool forced)
{
    return
#ifndef VINIFERA_REMOVE_DIRECTSOUND
          Audio.Start_Primary_Sound_Buffer(true) &&
#endif
          AudioManager.Start_Engine(forced);
}

static void Audio_Stop_Engine()
{
#ifndef VINIFERA_REMOVE_DIRECTSOUND
    Audio.Stop_Primary_Sound_Buffer();
#endif
    AudioManager.Stop_Engine();
}

static void Audio_Sound_Callback()
{
#ifndef VINIFERA_REMOVE_DIRECTSOUND
    Audio.Sound_Callback();
#endif
    AudioManager.Sound_Callback();
}

static LPDIRECTSOUND Audio_Get_Sound_Object()
{
#ifndef VINIFERA_REMOVE_DIRECTSOUND
    return LPDIRECTSOUND(AudioManager.Get_DirectSound_Object());
#else
    return Audio.Get_Sound_Object();
#endif
}


/**
 *  Handy macros for defining the audio driver patches.
 * 
 *  @author: CCHyper
 */
#define AUDIO_ENGINE_INIT_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Init(MainWindow); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_END_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_End(); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_IS_AVAILABLE_PATCH(label, return_false_addr, return_true_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        if (Audio_Is_Available()) { \
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
        Audio_Focus_Loss(); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_FOCUS_RESTORE_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Focus_Restore(); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_STOP_SAMPLE_PATCH(label, handle_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(int, handle, handle_reg); \
        _asm { pushad } \
        Audio_Stop_Sample(handle); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_SAMPLE_STATUS_PATCH(label, handle_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(int, handle, handle_reg); \
        static bool status; \
        _asm { pushad } \
        status = Audio_Sample_Status(handle); \
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
        status = Audio_Is_Sample_Playing(sample); \
        _asm { popad } \
        _asm { mov al, status } \
        JMP_REG(ecx, ret_addr); \
    }

#define AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(label, sample_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(void *, sample, sample_reg); \
        _asm { pushad } \
        Audio_Stop_Sample_Playing(sample); \
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
        Audio_Play_Sample(sample, priority, volume); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(label, sample_reg, priority, volume_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(void *, sample, sample_reg); \
        GET_REGISTER_STATIC(int, volume, volume_reg); \
        _asm { pushad } \
        Audio_Play_Sample(sample, priority, volume); \
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
        handle = Audio_Play(filename, volume, real_time_start); \
        _asm { popad } \
        _asm { mov eax, handle } \
        JMP_REG(ecx, ret_addr); \
    }

#define AUDIO_ENGINE_SOUND_CALLBACK_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        Audio_Sound_Callback(); \
        JMP(ret_addr); \
    }
    
#define AUDIO_ENGINE_SET_VOLUME_ALL_PATCH(label, volume_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(int, volume, volume_reg); \
        _asm { pushad } \
        Audio_Set_Volume_All(volume); \
        _asm { popad } \
        JMP(ret_addr); \
    }

//#define AUDIO_ENGINE_SET_VOLUME_PERCENT_ALL_PATCH(label, volume_reg, ret_addr)
#define AUDIO_ENGINE_SET_VOLUME_PERCENT_ALL_PATCH(label, volume_percent, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Set_Volume_Percent_All(volume_percent); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_SET_HANDLE_VOLUME_PATCH(label, handle_reg, volume_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(int, handle, handle_reg); \
        GET_REGISTER_STATIC(int, volume, volume_reg); \
        _asm { pushad } \
        Audio_Set_Handle_Volume(handle, volume); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_SET_SAMPLE_VOLUME_PATCH(label, sample_reg, volume_reg, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(void *, sample, sample_reg); \
        GET_REGISTER_STATIC(int, volume, volume_reg); \
        _asm { pushad } \
        Audio_Set_Sample_Volume(sample, volume); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_FADE_SAMPLE_PATCH(label, handle_reg, ticks, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(int, handle, handle_reg); \
        _asm { pushad } \
        Audio_Fade_Sample(handle, ticks); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_START_PRIMARY_SOUND_BUFFER_PATCH(label, force, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Start_Primary_Sound_Buffer(force); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define AUDIO_ENGINE_STOP_PRIMARY_SOUND_BUFFER_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Stop_Primary_Sound_Buffer(); \
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
    if (Audio_Is_Available()) {
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
    if (Audio_Is_Available()) {
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
    if (Audio_Is_Available()) {
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
    if (Audio_Is_Available()) {
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
    if (Audio_Is_Available()) {
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
    JMP(0x00686330);
}


/**
 *  Forces a playback limit on the credits tick sound, otherwise it runs wild, brother!
 * 
 *  @author: CCHyper
 */
constexpr int AUDIO_CREDIT_TICK_DELAY = 20;
DECLARE_PATCH(_CreditsClass_Graphic_Logic_Limit_Sfx_Patch)
{
    GET_REGISTER_STATIC(CreditClass *, this_ptr, edi);
    static CDTimerClass<MSTimerClass> _delay = AUDIO_CREDIT_TICK_DELAY;

    if (_delay.Expired()) {
        if (this_ptr->IsUp) {
            Sound_Effect(Rule->CreditTicks[0], 0, 0.5f);
        } else {
            Sound_Effect(Rule->CreditTicks[1], 0, 0.5f);
        }
        _delay = AUDIO_CREDIT_TICK_DELAY;
        _delay.Start();
    }

    JMP(0x004714CC);
}


/**
 *  Load the speech file assets (e.g. for preloading).
 */
DECLARE_PATCH(_Prep_Speech_For_Side_Preload_Files_Patch)
{
#ifdef USE_MINIAUDIO
    AudioVoxClass::Scan();
#endif

    _asm { mov al, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { add esp, 0x0A8 }
    _asm { retn }
}





DECLARE_PATCH(AHandle_Patch_1)
{
    if (!Audio_Is_Available()) {
        goto exit_function;
    }

continue_function:
    JMP(0x00407690);

exit_function:
    JMP(0x00407971);
}





/**
 *  x
 */
DECLARE_PATCH(_OwnerDraw_Window_Procedure_Play_EMBLEM_Patch)
{
    /**
     *  x
     */
    if (VocEmblem) {
        VocEmblem->Play(0.10f);
    }

    JMP(0x00593DF9);
}

#endif // USE_MINIAUDIO


/**
 *  Main function for patching the hooks.
 */
void Audio_Hooks()
{
#ifdef USE_MINIAUDIO

    /**
     *  Remove the initialsation of the original DirectSound audio engine.
     */
#ifdef VINIFERA_REMOVE_DIRECTSOUND
    Patch_Byte(0x00487990, 0xC3); // patch "retn" in __dyanmic_init for DSAudio.
    Patch_Byte(0x004879B0, 0xC3); // patch "retn" in __dyanmic_init, _atexit for DSAudio.
#endif

    /**
     *  Install the TS++ wrappers. These allow calls to Sound_Effect to work correctly.
     */
    //TSPP_Play_Sample_Function_Ptr = Audio_Play_Sample;
    //TSPP_Is_Sample_Playing_Function_Ptr = Audio_Is_Sample_Playing;

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

    /**
     *  Replace ThemeClass with the new AudioThemeClass.
     */
    Patch_Call(0x004E0AF0, &AudioThemeClass::Process);
    Patch_Call(0x004E0ADE, &AudioThemeClass::Clear);
    Patch_Call(0x00601C83, &AudioThemeClass::Clear);
    Patch_Call(0x0044ECBB, &AudioThemeClass::Scan);
    Patch_Call(0x004E0AFA, &AudioThemeClass::Scan);
    Patch_Call(0x004E4539, &AudioThemeClass::Scan);
    Patch_Call(0x0044B3FD, &AudioThemeClass::Base_Name);
    Patch_Call(0x005FC77C, &AudioThemeClass::Full_Name);
    Patch_Call(0x00462C96, &AudioThemeClass::AI);
    Patch_Call(0x0059428B, &AudioThemeClass::AI);
    Patch_Call(0x0044E954, &AudioThemeClass::Queue_Song);
    Patch_Call(0x00491617, &AudioThemeClass::Queue_Song);
    Patch_Call(0x00491A6B, &AudioThemeClass::Queue_Song);
    Patch_Call(0x00491AFC, &AudioThemeClass::Queue_Song);
    Patch_Call(0x004B7A85, &AudioThemeClass::Queue_Song);
    Patch_Call(0x004E28C2, &AudioThemeClass::Queue_Song);
    Patch_Call(0x004E29C3, &AudioThemeClass::Queue_Song);
    Patch_Call(0x00508AEF, &AudioThemeClass::Queue_Song);
    Patch_Call(0x0056454E, &AudioThemeClass::Queue_Song);
    Patch_Call(0x005DB40B, &AudioThemeClass::Queue_Song);
    Patch_Call(0x005DC966, &AudioThemeClass::Queue_Song);
    Patch_Call(0x005DCCB2, &AudioThemeClass::Queue_Song);
    Patch_Call(0x005DCEE1, &AudioThemeClass::Queue_Song);
    Patch_Call(0x005DCFEF, &AudioThemeClass::Queue_Song);
    Patch_Call(0x005E4D08, &AudioThemeClass::Queue_Song);
    Patch_Call(0x005FC343, &AudioThemeClass::Queue_Song);
    Patch_Call(0x005FC4BB, &AudioThemeClass::Queue_Song);
    Patch_Call(0x00619F07, &AudioThemeClass::Queue_Song);
    Patch_Call(0x0061C039, &AudioThemeClass::Queue_Song);
    Patch_Call(0x00622EBC, &AudioThemeClass::Queue_Song);
    Patch_Call(0x006268A0, &AudioThemeClass::Queue_Song);
    Patch_Call(0x0067A041, &AudioThemeClass::Queue_Song);
    Patch_Call(0x0067CA72, &AudioThemeClass::Queue_Song);
    Patch_Call(0x00687F1E, &AudioThemeClass::Queue_Song);
    Patch_Call(0x00687F5F, &AudioThemeClass::Queue_Song);
    Patch_Call(0x00687F6D, &AudioThemeClass::Queue_Song);
    Patch_Call(0x0068C0C9, &AudioThemeClass::Queue_Song);
    Patch_Call(0x0068C0E4, &AudioThemeClass::Queue_Song);
    Patch_Call(0x004B779D, &AudioThemeClass::Play_Song);
    Patch_Call(0x004E1FA2, &AudioThemeClass::Play_Song);
    Patch_Call(0x004E25D8, &AudioThemeClass::Play_Song);
    Patch_Call(0x004ECC28, &AudioThemeClass::Play_Song);
    Patch_Call(0x004ECE8B, &AudioThemeClass::Play_Song);
    Patch_Call(0x005538C8, &AudioThemeClass::Play_Song);
    Patch_Call(0x005C034D, &AudioThemeClass::Play_Song);
    Patch_Call(0x005DB3FD, &AudioThemeClass::Play_Song);
    Patch_Call(0x005E2F36, &AudioThemeClass::Play_Song);
    Patch_Call(0x0067C9D3, &AudioThemeClass::Play_Song);
    Patch_Call(0x00685B90, &AudioThemeClass::Play_Song);
    Patch_Call(0x005FC76F, &AudioThemeClass::Track_Length);
    Patch_Call(0x0044E887, &AudioThemeClass::Stop);
    Patch_Call(0x0044E9A7, &AudioThemeClass::Stop);
    Patch_Call(0x0049154A, &AudioThemeClass::Stop);
    Patch_Call(0x00491BD6, &AudioThemeClass::Stop);
    Patch_Call(0x004E26CC, &AudioThemeClass::Stop);
    Patch_Call(0x004E2759, &AudioThemeClass::Stop);
    Patch_Call(0x004E2A47, &AudioThemeClass::Stop);
    //Patch_Call(0x004E2ADE, &AudioThemeClass::Stop);     // Conflicts with _Select_Game_Set_EndGameClass_Difficulty_Patch.
    Patch_Call(0x004E2B5B, &AudioThemeClass::Stop);
    Patch_Call(0x004E2B77, &AudioThemeClass::Stop);
    Patch_Call(0x004E2C49, &AudioThemeClass::Stop);
    Patch_Call(0x004E2DAD, &AudioThemeClass::Stop);
    Patch_Call(0x004E2DF5, &AudioThemeClass::Stop);
    Patch_Call(0x004E2E7B, &AudioThemeClass::Stop);
    Patch_Call(0x004ECA23, &AudioThemeClass::Stop);
    Patch_Call(0x004ECD18, &AudioThemeClass::Stop);
    Patch_Call(0x00553C03, &AudioThemeClass::Stop);
    Patch_Call(0x005C0330, &AudioThemeClass::Stop);
    Patch_Call(0x005DB221, &AudioThemeClass::Stop);
    Patch_Call(0x005DB343, &AudioThemeClass::Stop);
    Patch_Call(0x005E2F1C, &AudioThemeClass::Stop);
    Patch_Call(0x005E4B5B, &AudioThemeClass::Stop);
    Patch_Call(0x005FC338, &AudioThemeClass::Stop);
    Patch_Call(0x00687F12, &AudioThemeClass::Stop);
    Patch_Call(0x00687F53, &AudioThemeClass::Stop);
    Patch_Call(0x006859A3, &AudioThemeClass::Suspend);
    Patch_Call(0x00685EB4, &AudioThemeClass::Suspend);
    Patch_Call(0x00491A5B, &AudioThemeClass::Still_Playing);
    Patch_Call(0x004E2B10, &AudioThemeClass::Still_Playing);
    Patch_Call(0x004E2B4B, &AudioThemeClass::Still_Playing);
    Patch_Call(0x004E2E27, &AudioThemeClass::Still_Playing);
    Patch_Call(0x004E2E6B, &AudioThemeClass::Still_Playing);
    Patch_Call(0x00589AD6, &AudioThemeClass::Still_Playing);
    Patch_Call(0x00687F02, &AudioThemeClass::Still_Playing);
    Patch_Call(0x005FC75C, &AudioThemeClass::Is_Allowed);
    Patch_Call(0x0044B3C5, &AudioThemeClass::From_Name);
    Patch_Call(0x0049160C, &AudioThemeClass::From_Name);
    Patch_Call(0x004B7792, &AudioThemeClass::From_Name);
    Patch_Call(0x004E1F83, &AudioThemeClass::From_Name);
    Patch_Call(0x004E1F97, &AudioThemeClass::From_Name);
    Patch_Call(0x004E25B9, &AudioThemeClass::From_Name);
    Patch_Call(0x004E25CD, &AudioThemeClass::From_Name);
    Patch_Call(0x004E28A3, &AudioThemeClass::From_Name);
    Patch_Call(0x004E28B7, &AudioThemeClass::From_Name);
    Patch_Call(0x004E29A4, &AudioThemeClass::From_Name);
    Patch_Call(0x004E29B8, &AudioThemeClass::From_Name);
    Patch_Call(0x004E8708, &AudioThemeClass::From_Name);
    Patch_Call(0x004E871C, &AudioThemeClass::From_Name);
    Patch_Call(0x004E8748, &AudioThemeClass::From_Name);
    Patch_Call(0x004E875C, &AudioThemeClass::From_Name);
    Patch_Call(0x004ECC1D, &AudioThemeClass::From_Name);
    Patch_Call(0x005538BD, &AudioThemeClass::From_Name);
    Patch_Call(0x005E2F2B, &AudioThemeClass::From_Name);
    Patch_Call(0x0067C9C8, &AudioThemeClass::From_Name);
    Patch_Call(0x00589AC4, &AudioThemeClass::Set_Volume);
    Patch_Call(0x00589FE3, &AudioThemeClass::Set_Volume);
    Patch_Call(0x0058A580, &AudioThemeClass::Set_Volume);
    
    Patch_Dword(0x0044B3C0+1, (uintptr_t)&AudioTheme); // CCINIClass::Get_ThemeType
    Patch_Dword(0x0044B3F8+1, (uintptr_t)&AudioTheme); // CCINIClass::Put_ThemeType
    Patch_Dword(0x0044E882+1, (uintptr_t)&AudioTheme); // CD::Insert_Disk
    Patch_Dword(0x0044E94F+1, (uintptr_t)&AudioTheme); // CD::Insert_Disk
    Patch_Dword(0x0044E9A1+1, (uintptr_t)&AudioTheme); // CD::Init_Swap
    Patch_Dword(0x0044ECB6+1, (uintptr_t)&AudioTheme); // CD::Init_Swap
    Patch_Dword(0x00462C91+1, (uintptr_t)&AudioTheme); // Call_Back
    Patch_Dword(0x00491545+1, (uintptr_t)&AudioTheme); // Slide_Show
    Patch_Dword(0x00491607+1, (uintptr_t)&AudioTheme); // Slide_Show
    Patch_Dword(0x00491611+1, (uintptr_t)&AudioTheme); // Slide_Show
    Patch_Dword(0x00491A56+1, (uintptr_t)&AudioTheme); // Slide_Show
    Patch_Dword(0x00491A66+1, (uintptr_t)&AudioTheme); // Slide_Show
    Patch_Dword(0x00491AF7+1, (uintptr_t)&AudioTheme); // Slide_Show
    Patch_Dword(0x00491BC6+1, (uintptr_t)&AudioTheme); // Slide_Show
    Patch_Dword(0x004B7789+1, (uintptr_t)&AudioTheme); // GraphicMenu_Process_Loop
    Patch_Dword(0x004B7798+1, (uintptr_t)&AudioTheme); // GraphicMenu_Process_Loop
    Patch_Dword(0x004B7A80+1, (uintptr_t)&AudioTheme); // GraphicMenu_Process_Loop
    Patch_Dword(0x004E0AD9+1, (uintptr_t)&AudioTheme); // Init_Game
    Patch_Dword(0x004E0AEA+1, (uintptr_t)&AudioTheme); // Init_Game
    Patch_Dword(0x004E0AF5+1, (uintptr_t)&AudioTheme); // Init_Game
    Patch_Dword(0x004E1F7E+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E1F92+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E1F9D+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E25B4+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E25C8+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E25D3+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E26B8+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2754+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E289E+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E28B2+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E28BD+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E299F+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E29B3+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E29BE+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2A40+1, (uintptr_t)&AudioTheme); // Select_Game
    //Patch_Dword(0x004E2AD9+1, (uintptr_t)&AudioTheme); // Select_Game     // Conflicts with _Select_Game_Set_EndGameClass_Difficulty_Patch.
    Patch_Dword(0x004E2B05+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2B46+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2B56+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2B72+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2C44+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2DA8+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2DF0+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2E1C+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2E66+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E2E76+1, (uintptr_t)&AudioTheme); // Select_Game
    Patch_Dword(0x004E452D+1, (uintptr_t)&AudioTheme); // Init_Secondary_Mixfiles
    Patch_Dword(0x004E8703+1, (uintptr_t)&AudioTheme); // Get_Menu_Theme
    Patch_Dword(0x004E8717+1, (uintptr_t)&AudioTheme); // Get_Menu_Theme
    Patch_Dword(0x004E8743+1, (uintptr_t)&AudioTheme); // Get_MapSelect_Theme
    Patch_Dword(0x004E8757+1, (uintptr_t)&AudioTheme); // Get_MapSelect_Theme
    Patch_Dword(0x004ECA0D+1, (uintptr_t)&AudioTheme); // IonStorm_Start
    Patch_Dword(0x004ECC13+1, (uintptr_t)&AudioTheme); // IonStorm_Start
    Patch_Dword(0x004ECC22+1, (uintptr_t)&AudioTheme); // IonStorm_Start
    Patch_Dword(0x004ECD13+1, (uintptr_t)&AudioTheme); // IonStorm_Stop
    Patch_Dword(0x004ECE86+1, (uintptr_t)&AudioTheme); // IonStorm_Stop
    Patch_Dword(0x00508AEA+1, (uintptr_t)&AudioTheme); // Main_Loop
    Patch_Dword(0x005538B8+1, (uintptr_t)&AudioTheme); // MapSelect
    Patch_Dword(0x005538C3+1, (uintptr_t)&AudioTheme); // MapSelect
    Patch_Dword(0x00553BFE+1, (uintptr_t)&AudioTheme); // MapSelect
    Patch_Dword(0x00564549+1, (uintptr_t)&AudioTheme); // Movie_Play
    Patch_Dword(0x00589ABF+1, (uintptr_t)&AudioTheme); // OptionsClass::Set_Score_Volume
    Patch_Dword(0x00589AD1+1, (uintptr_t)&AudioTheme); // OptionsClass::Set_Score_Volume
    Patch_Dword(0x00589FDE+1, (uintptr_t)&AudioTheme); // OptionsClass::Load_Settings
    Patch_Dword(0x0058A57B+1, (uintptr_t)&AudioTheme); // OptionsClass::Set
    Patch_Dword(0x00594286+1, (uintptr_t)&AudioTheme); // OwnerDraw_Window_Procedure
    Patch_Dword(0x005C032B+1, (uintptr_t)&AudioTheme); // Restate_Mission
    Patch_Dword(0x005C0347+1, (uintptr_t)&AudioTheme); // Restate_Mission
    Patch_Dword(0x005DB21A+1, (uintptr_t)&AudioTheme); // Start_Scenario
    Patch_Dword(0x005DB33E+1, (uintptr_t)&AudioTheme); // Start_Scenario
    Patch_Dword(0x005DB3F8+1, (uintptr_t)&AudioTheme); // Start_Scenario
    Patch_Dword(0x005DB406+1, (uintptr_t)&AudioTheme); // Start_Scenario
    Patch_Dword(0x005DC95F+1, (uintptr_t)&AudioTheme); // Do_Win
    Patch_Dword(0x005DCCAB+1, (uintptr_t)&AudioTheme); // Do_Lose
    Patch_Dword(0x005DCED4+1, (uintptr_t)&AudioTheme); // Do_Restart
    Patch_Dword(0x005DCFE8+1, (uintptr_t)&AudioTheme); // Do_Abort
    Patch_Dword(0x005E2F17+1, (uintptr_t)&AudioTheme); // ScoreClass::Presentation
    Patch_Dword(0x005E2F26+1, (uintptr_t)&AudioTheme); // ScoreClass::Presentation
    Patch_Dword(0x005E2F31+1, (uintptr_t)&AudioTheme); // ScoreClass::Presentation
    Patch_Dword(0x005E4B56+1, (uintptr_t)&AudioTheme); // ScoreClass::Presentation
    Patch_Dword(0x005E4D03+1, (uintptr_t)&AudioTheme); // ScoreClass::Presentation
    Patch_Dword(0x005FC331+1, (uintptr_t)&AudioTheme); // SoundControlsClass::Process
    Patch_Dword(0x005FC33E+1, (uintptr_t)&AudioTheme); // SoundControlsClass::Process
    Patch_Dword(0x005FC4B6+1, (uintptr_t)&AudioTheme); // SoundControlsClass::Process
    Patch_Dword(0x005FC757+1, (uintptr_t)&AudioTheme); // SoundControlsClass::Process
    Patch_Dword(0x005FC76A+1, (uintptr_t)&AudioTheme); // SoundControlsClass::Process
    Patch_Dword(0x005FC775+1, (uintptr_t)&AudioTheme); // SoundControlsClass::Process
    Patch_Dword(0x00601C72+1, (uintptr_t)&AudioTheme); // Game_Shutdown
    Patch_Dword(0x00619F02+1, (uintptr_t)&AudioTheme); // TActionClass::operator()
    Patch_Dword(0x0061C033+1, (uintptr_t)&AudioTheme); // TActionClass::TAction_Play_Music
    Patch_Dword(0x00622EB7+1, (uintptr_t)&AudioTheme); // TeamClass::AI
    Patch_Dword(0x0062689B+1, (uintptr_t)&AudioTheme); // TeamClass::TMission_Play_Music
    Patch_Dword(0x0067A03C+1, (uintptr_t)&AudioTheme); // WorldDominationTour::Selection::DTOR
    Patch_Dword(0x0067C9C3+1, (uintptr_t)&AudioTheme); // WDT_Init_Menu
    Patch_Dword(0x0067C9CE+1, (uintptr_t)&AudioTheme); // WDT_Init_Menu
    Patch_Dword(0x0067CA6D+1, (uintptr_t)&AudioTheme); // WDT
    Patch_Dword(0x00685999+1, (uintptr_t)&AudioTheme); // Focus_Loss
    Patch_Dword(0x00685B8A+1, (uintptr_t)&AudioTheme); // Focus_Restore
    Patch_Dword(0x00685EAA+1, (uintptr_t)&AudioTheme); // Main_Window_Procedure
    Patch_Dword(0x00687EFD+1, (uintptr_t)&AudioTheme); // WOL
    Patch_Dword(0x00687F0D+1, (uintptr_t)&AudioTheme); // WOL
    Patch_Dword(0x00687F17+1, (uintptr_t)&AudioTheme); // WOL
    Patch_Dword(0x00687F4E+1, (uintptr_t)&AudioTheme); // WOL
    Patch_Dword(0x00687F58+1, (uintptr_t)&AudioTheme); // WOL
    Patch_Dword(0x00687F68+1, (uintptr_t)&AudioTheme); // WOL
    Patch_Dword(0x0068C0C4+1, (uintptr_t)&AudioTheme); // WOL_Start
    Patch_Dword(0x0068C0DF+1, (uintptr_t)&AudioTheme); // WOL_Start

    Patch_Dword(0x0044E877+2, (uintptr_t)&AudioTheme.Score); // CD::Insert_Disk
    Patch_Dword(0x0044E997+1, (uintptr_t)&AudioTheme.Score); // CD::Init_Swap
    Patch_Dword(0x004ECA19+1, (uintptr_t)&AudioTheme.Score); // IonStorm_Start
    Patch_Dword(0x00508ADF+2, (uintptr_t)&AudioTheme.Score); // Main_Loop
    Patch_Dword(0x005C0324+2, (uintptr_t)&AudioTheme.Score); // Restate_Mission
    Patch_Dword(0x005FC7E8+2, (uintptr_t)&AudioTheme.Score); // SoundControlsClass::Process
    Patch_Dword(0x00685994+1, (uintptr_t)&AudioTheme.Score); // Focus_Loss
    Patch_Dword(0x00685EA5+1, (uintptr_t)&AudioTheme.Score); // Main_Window_Procedure
    Patch_Dword(0x00687F2E+2, (uintptr_t)&AudioTheme.Score); // WOL
    Patch_Dword(0x00687F3D+2, (uintptr_t)&AudioTheme.Score); // WOL
    Patch_Dword(0x0068C09B+2, (uintptr_t)&AudioTheme.Score); // WOL start
    Patch_Dword(0x0068C0CE+2, (uintptr_t)&AudioTheme.Score); // WOL start

    Patch_Dword(0x00589A67+1, (uintptr_t)&AudioTheme.IsRepeat); // OptionsClass::Set_Repeat
    Patch_Dword(0x0058A019+1, (uintptr_t)&AudioTheme.IsRepeat); // OptionsClass::Load_Settings
    Patch_Dword(0x0058A59E+1, (uintptr_t)&AudioTheme.IsRepeat); // OptionsClass::Set
    Patch_Dword(0x0067C9D8+2, (uintptr_t)&AudioTheme.IsRepeat); // WDT init menu

    Patch_Dword(0x00589A37+1, (uintptr_t)&AudioTheme.IsShuffle); // OptionsClass::Set_Shuffle
    Patch_Dword(0x0058A058+1, (uintptr_t)&AudioTheme.IsShuffle); // OptionsClass::Load_Settings
    Patch_Dword(0x0058A5C5+1, (uintptr_t)&AudioTheme.IsShuffle); // OptionsClass::Set
    Patch_Dword(0x0068C99C+1, (uintptr_t)&AudioTheme.IsShuffle); // Join_WOL_Lobby
    Patch_Dword(0x0068C9A1+2, (uintptr_t)&AudioTheme.IsShuffle); // Join_WOL_Lobby
    Patch_Dword(0x0068CFBD+2, (uintptr_t)&AudioTheme.IsShuffle); // Join_WOL_Lobby

    // This voodoo is required as we don't have access to ActiveCount of DynamicVectorClass.
    Patch_Dword(0x005FC747+1, ((uintptr_t)&AudioTheme.Themes) + (sizeof(DynamicVectorClass<void*>)-8)); // SoundControlsClass::Process
    Patch_Dword(0x005FC7F7+1, ((uintptr_t)&AudioTheme.Themes) + (sizeof(DynamicVectorClass<void*>)-8)); // SoundControlsClass::Process

    /**
     *  Replace the speech handler with the new AudioVoxClass.
     */
    Patch_Jump(0x00665800, &AudioVoxClass::Speak);
    Patch_Jump(0x00665940, &AudioVoxClass::AI);
    Patch_Jump(0x00665AF0, &AudioVoxClass::Stop_Speaking);
    Patch_Jump(0x00665B20, &AudioVoxClass::Is_Speaking);
    Patch_Jump(0x00665B70, &AudioVoxClass::Set_Speech_Volume);
    Patch_Jump(0x00665BC0, &AudioVoxClass::Set_Speech_Allowed);
    Patch_Jump(0x00665BD0, &AudioVoxClass::Is_Speech_Allowed);
    Patch_Jump(0x004E86E0, &_Prep_Speech_For_Side_Preload_Files_Patch);
    
    /**
     *  Replace VocClass with the new AudioVocClass.
     */
    Patch_Jump(0x00664BA0, static_cast<int (*)(VocType, int, float)>(&AudioVocClass::Play));
    Patch_Jump(0x00664C60, static_cast<int (*)(VocType, float)>(&AudioVocClass::Play));
    Patch_Jump(0x00664D10, static_cast<int (*)(VocType, Coordinate &)>(&AudioVocClass::Play));
    Patch_Jump(0x00664EC0, &AudioVocClass::Process);
    Patch_Jump(0x00665080, &AudioVocClass::Clear);
    Patch_Jump(0x00665100, &AudioVocClass::From_Name);
    Patch_Jump(0x00665140, &AudioVocClass::Voc_From_Name);
    Patch_Jump(0x006651C0, &AudioVocClass::VocType_From_Voc);

    /**
     *  Replace audio driver interface for the VQA player (AHandle).
     */
#ifdef VINIFERA_REMOVE_VQA_AHANDLE

    Patch_Jump(0x00407673, &AHandle_Patch_1); // to stop movies playing for now.

    Patch_Jump(0x00407765, 0x0040776F); // remove DirectSoundAudioClass::Lock_Mutex call
    Patch_Jump(0x0040784C, 0x00407856); // remove DirectSoundAudioClass::Stop_Primary_Sound_Buffer call
    Patch_Jump(0x004078CD, 0x004078D9); // remove DirectSoundAudioClass::Start_Primary_Sound_Buffer call
    Patch_Jump(0x004078E6, 0x004078F0); // remove DirectSoundAudioClass::Unlock_Mutex call
    Patch_Jump(0x00407A87, 0x00407A91); // remove DirectSoundAudioClass::Lock_Mutex call
    Patch_Jump(0x00407A9E, 0x00407AA8); // remove DirectSoundAudioClass::Stop_Primary_Sound_Buffer call
    Patch_Jump(0x00407AC5, 0x00407AD0); // remove DirectSoundAudioClass::Stop_Primary_Sound_Buffer call
    Patch_Jump(0x00407ADD, 0x00407AE7); // remove DirectSoundAudioClass::Unlock_Mutex call

    Patch_Byte_Range(0x00407C7D, 0x90, 5); // remove DirectSoundAudioClass::Lock_Mutex call
    Patch_Byte_Range(0x00407C8C, 0x90, 5); // remove DirectSoundAudioClass::Lock_Mutex call

    Patch_Jump(0x00407CA0, 0x00407CAA); // remove DirectSoundAudioClass::Unlock_Mutex call

    //Patch_Jump(0x004072D0, &VQAAudioHandleClass::Timer_Handler);
    //Patch_Jump(0x00407450, &VQAAudioHandleClass::Stream_Handler);
    //Patch_Jump(0x00408200, &VQAAudioHandleClass::Get_System_Time);
//    Patch_Jump(0x004082B0, &VQAAudioHandleClass::Open_Audio);
//    Patch_Jump(0x004082C0, &VQAAudioHandleClass::Close_Audio);

#endif

    /**
     *  #BUGFIX: Fudge to normalise the credits tick sound rate, otherwise it
     *           will spam the tick sound in rapid succession.
     */
    Patch_Jump(0x00471493, &_CreditsClass_Graphic_Logic_Limit_Sfx_Patch);

    /**
     *  #BUGFIX:
     *  Fixes a bug where themes would restart ofter focus loss and regain. The
     *  new audio manager system handles the focus loss and gain, so this is no
     *  longer required.
     * 
     *  This patch removes calls to ThemeClass::Suspend and ThemeClass::Play_Song.
     */
    Patch_Jump(0x00685994, 0x006859A8); // Focus_Loss
    Patch_Jump(0x00685EA5, 0x00685EB9); // inlined Focus_Loss
    Patch_Jump(0x00685B84, 0x00685B95); // Focus_Restore

    /**
     *  Replace various direct calls to Play_Sample with calls to the AudioVocClass interface.
     */
    Patch_Jump(0x00593DAC, &_OwnerDraw_Window_Procedure_Play_EMBLEM_Patch);

#endif

    /**
     *  #issue-x
     * 
     *  This patch removes the loading of THEME(01).INI and the calls to Theme.Process() and
     *  Theme.Scan(). This logic has now been moved to the post Init_Game process as the rules
     *  needs to be initialised before the theme class tries to read the theme control data
     *  which references a yet to be initialised SideType.
     */
    //Patch_Jump(0x004E09C8, 0x004E0B0C);
}
