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
 *  @brief         Contains the hooks for the new audio engine.
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
 *  If we are not replacing AHandle, we still need to init DSAudio due to
 *  the VQA player still using DirectSound. This define controls that.
 */
//#define VINIFERA_AUDIO_HOOK_AHANDLE 1


/**
 *  Default audio driver values.
 */
#define AUDIO_DEFAULT_BITS 16
#define AUDIO_DEFAULT_STEREO 1
#define AUDIO_DEFAULT_RATE 48000
#define AUDIO_DEFAULT_NUM_TRACKERS 16


/**
 *  If we are not replacing AHandle, we still need to init DirectSound audio
 *  engine due to the VQA player still using DirectSound. This define also
 *  controls if the following functions should call the DirectSound engine
 *  in that case.
 */
//#define VINIFERA_AUDIO_HOOK_AHANDLE 1

static bool Direct_Sound_Init()
{
#ifndef VINIFERA_AUDIO_HOOK_AHANDLE
    DEV_DEBUG_INFO("Audio[VQ]: Initialising DirectSound.\n");
    return Audio.Init(MainWindow, 16, false, 22050);
#else
    return true;
#endif
}

static void Direct_Sound_End()
{
#ifndef VINIFERA_AUDIO_HOOK_AHANDLE
    DEV_DEBUG_INFO("Audio[VQ]: Shutting down DirectSound.\n");
    Audio.End();
#endif
}

static bool Direct_Sound_Is_Available()
{
#ifndef VINIFERA_AUDIO_HOOK_AHANDLE
    return Audio.Is_Available();
#else
    return true;
#endif
}

static bool Direct_Sound_Start()
{
#ifndef VINIFERA_AUDIO_HOOK_AHANDLE
    DEV_DEBUG_INFO("Audio[VQ]: Starting DirectSound.\n");
    return Audio.Start_Primary_Sound_Buffer(true);
#else
    return true;
#endif
}

static void Direct_Sound_Stop()
{
#ifndef VINIFERA_AUDIO_HOOK_AHANDLE
    DEV_DEBUG_INFO("Audio[VQ]: Stopping DirectSound.\n");
    Audio.Stop_Primary_Sound_Buffer();
#endif
}


/**
 *  These wrappers are required due to the stack being used when calling
 *  a static function.
 */
static bool Audio_Driver_Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers) { return Direct_Sound_Init() && Audio_Driver()->Init(hWnd, bits_per_sample, stereo, rate, num_trackers); }
static void Audio_Driver_End() { Direct_Sound_End(); Audio_Driver()->End(); }
static bool Audio_Driver_Is_Available() { return Direct_Sound_Is_Available() && Audio_Driver()->Is_Available(); }
static bool Audio_Driver_Is_Enabled() { return Audio_Driver()->Is_Enabled(); }
static void Audio_Driver_Enabled() { Audio_Driver()->Enable(); }
static void Audio_Driver_Disable() { Audio_Driver()->Disable(); }
static void Audio_Driver_Stop_Sample(int handle) { return Audio_Driver()->Stop_Handle_Playing((AudioHandle)handle); }
static bool Audio_Driver_Sample_Status(int handle) { return Audio_Driver()->Is_Handle_Playing((AudioHandle)handle); }
static bool Audio_Driver_Is_Sample_Playing(const void *sample) { return Audio_Driver()->Is_Sample_Playing((AudioSample)sample); }
static void Audio_Driver_Stop_Sample_Playing(const void *sample) { Audio_Driver()->Stop_Sample_Playing((AudioSample)sample); }
static int Audio_Driver_Play_Sample(const void *sample, int priority = PRIORITY_MAX, int volume = VOLUME_MAX) { return (int)Audio_Driver()->Play_Sample((AudioSample)sample, priority, volume); }
static int Audio_Driver_File_Play_Sample(const char *filename, int priority = PRIORITY_MAX, int volume = VOLUME_MAX) { return (int)Audio_Driver()->Play_File(Wstring(filename), priority, volume); }
static int Audio_Driver_File_Stream_Sample_Vol(const char *filename, int volume, bool real_time_start = false) { return (int)Audio_Driver()->Stream_File(Wstring(filename), volume, real_time_start); }
static void Audio_Driver_Fade_Sample(int handle, int ticks = 60) { Audio_Driver()->Fade_Sample((AudioHandle)handle, ticks); }
//static int Audio_Driver_Get_Playing_Sample_Handle(const void *sample) { return (int)Audio_Driver()->Get_Playing_Sample_Handle((AudioSample)sample); }
//static void Audio_Driver_Set_Handle_Loop(int handle, bool loop) { Audio_Driver()->Set_Handle_Loop((AudioHandle)handle, loop); }
//static void Audio_Driver_Set_Sample_Loop(const void *sample, bool loop) { Audio_Driver()->Set_Sample_Loop((AudioSample)sample, loop); }
//static void Audio_Driver_Stop_Handle_Looping(int handle) { Audio_Driver()->Stop_Handle_Looping((AudioHandle)handle); }
//static void Audio_Driver_Stop_Sample_Looping(const void *sample) { Audio_Driver()->Stop_Sample_Looping((AudioSample)sample); }
//static void Audio_Driver_Set_Score_Handle_Pause(int handle) { Audio_Driver()->Set_Score_Handle_Pause((AudioHandle)handle); }
//static void Audio_Driver_Set_Score_Handle_Resume(int handle) { Audio_Driver()->Set_Score_Handle_Resume((AudioHandle)handle); }
//static void Audio_Driver_Set_Handle_Pitch(int handle, float pitch) { Audio_Driver()->Set_Handle_Pitch((AudioHandle)handle, pitch); }
//static void Audio_Driver_Set_Sample_Pitch(const void *sample, float pitch) { Audio_Driver()->Set_Sample_Pitch((AudioSample)sample, pitch); }
//static void Audio_Driver_Set_Handle_Pan(int handle, float pan) { Audio_Driver()->Set_Handle_Pan((AudioHandle)handle, pan); }
//static void Audio_Driver_Set_Sample_Pan(const void *sample, float pan) { Audio_Driver()->Set_Sample_Pan((AudioSample)sample, pan); }
static void Audio_Driver_Set_Handle_Volume(int handle, int volume) { Audio_Driver()->Set_Handle_Volume((AudioHandle)handle, volume); }
static void Audio_Driver_Set_Sample_Volume(const void *sample, int volume) { Audio_Driver()->Set_Sample_Volume((AudioSample)sample, volume); }
static int Audio_Driver_Set_Sound_Vol(int volume) { return Audio_Driver()->Set_Sound_Volume(volume); }
static int Audio_Driver_Set_Score_Vol(int volume) { return Audio_Driver()->Set_Score_Volume(volume); }
static void Audio_Driver_Set_Volume_All(int volume) { Audio_Driver()->Set_Volume_All(volume); }
static int Audio_Driver_Set_Volume_Percent_All(int vol_percent) { return Audio_Driver()->Set_Volume_Percent_All(vol_percent); }
//static long Audio_Driver_Sample_Length(const void *sample) { return Audio_Driver()->Sample_Length((AudioSample)sample); }
static bool Audio_Driver_Start_Primary_Sound_Buffer(bool forced = false) { return Direct_Sound_Start() && Audio_Driver()->Start_Primary_Sound_Buffer(forced); }
static void Audio_Driver_Stop_Primary_Sound_Buffer() { Direct_Sound_Stop(); Audio_Driver()->Stop_Primary_Sound_Buffer(); }
static void Audio_Driver_Sound_Callback() { Audio_Driver()->Sound_Callback(); }
//static int Audio_Driver_Get_Free_Sample_Handle(int priority) { return (int)Audio_Driver()->Get_Free_Sample_Handle(priority); }
static int Audio_Driver_Play_Sample_Handle(const void *sample, int priority, int volume, int handle) { return (int)Audio_Driver()->Play_Sample_Handle((AudioSample)sample, priority, volume, (AudioHandle)handle); }
//static int Audio_Driver_Stream_Sample_Vol(void *buffer, int size, bool (*callbackptr)(short, short *, void **, int *), int volume, int handle) { return (int)Audio_Driver()->Stream_Sample_Vol(buffer, size, callbackptr, volume, (AudioHandle)handle); }
static void Audio_Driver_Set_Focus_Loss_Function(void (*func)()) { Audio_Driver()->Set_Focus_Loss_Function(func); }
static void Audio_Driver_Set_Stream_Low_Impact(bool set) { Audio_Driver()->Set_Stream_Low_Impact(set); }


/**
 *  Handy macros for defining the OpenAL patches.
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
        handle = Audio_Driver_File_Stream_Sample_Vol(filename, volume, real_time_start); \
        _asm { popad } \
        _asm { mov eax, handle } \
        JMP_REG(ecx, ret_addr); \
    }

#define AUDIO_ENGINE_SOUND_CALLBACK_PATCH(label, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        Audio_Driver_Sound_Callback(); \
        _asm { popad } \
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

AUDIO_ENGINE_STOP_SAMPLE_PATCH(_MapSelect_Stop_Sample_Patch_1, eax, 0x0055404D);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_MapSelect_Stop_Sample_Patch_2, eax, 0x005544C9);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_MapSelect_Stop_Sample_Patch_3, eax, 0x005545EC);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_MapSelect_Stop_Sample_Patch_4, eax, 0x00554672);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_ThemeClass_Play_Song_Stop_Sample_Patch_1, ecx, 0x00644048);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_ThemeClass_Stop_Stop_Sample_Patch_1, eax, 0x00644238);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_ThemeClass_Suspend_Stop_Sample_Patch_1, ecx, 0x0064429A);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_WorldDominationTour_Voices_DTOR_Stop_Sample_Patch_1, eax, 0x0067E900);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_1, eax, 0x0067FCAE);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_2, edx, 0x0067FD8E);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_3, eax, 0x0067FE61);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_4, edx, 0x0067FE9B);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_5, eax, 0x00680068);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_6, eax, 0x006800F6);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_WorldDominationTour_Voices_Anim_Stop_Sample_Patch_1, eax, 0x00680191);
AUDIO_ENGINE_STOP_SAMPLE_PATCH(_WorldDominationTour_Voices_Anim_Stop_Sample_Patch_2, edx, 0x006801CB);

AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_ThemeClass_AI_Sample_Status_Patch_1, eax, 0x00643E17);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_ThemeClass_Queue_Song_Sample_Status_Patch_2, eax, 0x00643FC8);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_ThemeClass_Stop_Sample_Status_Patch_3, eax, 0x006441F4);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_ThemeClass_Still_Playing_Sample_Status_Patch_4, ecx, 0x006442ED);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Voices_Sample_Sample_Status_Patch_1, eax, 0x0067F897);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Voices_Sample_Sample_Status_Patch_2, eax, 0x0067FB4B);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Voices_Sample_Sample_Status_Patch_3, eax, 0x0067FC3F);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Voices_Sample_Sample_Status_Patch_4, eax, 0x0067FC9C);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Voices_Sample_Sample_Status_Patch_5, eax, 0x0067FD7C);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Voices_Sample_Sample_Status_Patch_6, eax, 0x0067FE89);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Voices_Sample_Sample_Status_Patch_7, eax, 0x0067FF33);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Voices_Sample_Sample_Status_Patch_8, eax, 0x00680056);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Voices_Sample_Sample_Status_Patch_9, eax, 0x00680093);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Anim_Sample_Sample_Status_Patch_1, eax, 0x006801B9);
AUDIO_ENGINE_SAMPLE_STATUS_PATCH(_WorldDominationTour_Anim_Sample_Sample_Status_Patch_2, eax, 0x00680326);

AUDIO_ENGINE_IS_SAMPLE_PLAYING_PATCH(_Speak_AI_Is_Sample_Playing_Patch_1, ecx, 0x0066599E);
AUDIO_ENGINE_IS_SAMPLE_PLAYING_PATCH(_Is_Speaking_Is_Sample_Playing_Patch_1, ecx, 0x00665B60);

AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(_MapSelect_Stop_Sample_Playing_Patch_1, eax, 0x0056E729);
AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(_MapSelect_Stop_Sample_Playing_Patch_2, eax, 0x0056E82A);
AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(_MSSfxEntry_Stop_Sample_Playing_Patch_1, eax, 0x0056F3C5);
AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(_MSFont_Stop_Sample_Playing_Patch_1, eax, 0x00571F8D);
AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(_MSSfx_DTOR_Stop_Sample_Playing_Patch_1, eax, 0x00574CB5);
AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(_ScoreClass_Presentation_Stop_Sample_Playing_Patch_1, eax, 0x005E4B85);
AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(_ScoreFontClass_DTOR_Stop_Sample_Playing_Patch_1, eax, 0x005E68AB);
AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(_ScoreSoundClass_DTOR_Stop_Sample_Playing_Patch_1, eax, 0x005E73C5);
AUDIO_ENGINE_STOP_SAMPLE_PLAYING_PATCH(_Stop_Speaking_Stop_Sample_Playing_Patch_1, ecx, 0x00665B11);

AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_MSWordAnim_Play_Sample_Patch_1, esi, 10, eax, 0x0056D064);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_MSWordAnim_Play_Sample_Patch_2, edi, 10, eax, 0x0056D16A);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_MSSfxEntry_Play_Sample_Patch_1, esi, PRIORITY_MAX, eax, 0x0056F417);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_MSFont_Play_Sample_Patch_1, esi, 10, eax, 0x00572576);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_MSSfx_Play_Sample_Patch_1, eax, PRIORITY_MAX, ecx, 0x00574CFC);

DECLARE_PATCH(_OwnerDraw_Window_Procedure_Play_Sample_Patch_1)
{
    GET_REGISTER_STATIC(int, volume, eax);
    static const void *sample;
    sample = MFCC::Retrieve("EMBLEM.AUD");
    Audio_Driver_Play_Sample(sample, PRIORITY_MAX, volume);
    JMP(0x00593DF9);
}

AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_ScoreClass_Play_Sample_Patch_1, edi, PRIORITY_MAX, eax, 0x005E35BA);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_ScoreClass_Play_Sample_Patch_2, edi, PRIORITY_MAX, eax, 0x005E3706);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_ScoreClass_Play_Sample_Patch_3, esi, PRIORITY_MAX, eax, 0x005E3E6B);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_ScoreClass_Play_Sample_Patch_4, esi, PRIORITY_MAX, eax, 0x005E4437);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_ScoreClass_Play_Sample_Patch_5, ebx, PRIORITY_MAX, eax, 0x005E5835);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_ScoreClass_Input_Name_Play_Sample_Patch_1, edi, PRIORITY_MAX, eax, 0x005E6006);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_ScoreClass_Input_Name_Play_Sample_Patch_2, edi, PRIORITY_MAX, eax, 0x005E61C6);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_ScoreFontClass_Play_Sample_Patch_1, esi, PRIORITY_MAX, eax, 0x005E67E5);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_ScoreFontClass_Play_Sample_Patch_2, esi, PRIORITY_MAX, eax, 0x005E6BC4);

// Volume scales priority.
static int Voc_Calculate_Play_Priority(VocClass *voc, float vol) { return voc->Priority * vol; }

DECLARE_PATCH(_VocClass_Play_Sample_Patch_1)
{
    GET_REGISTER_STATIC(VocClass *, this_ptr, edi);
    GET_REGISTER_STATIC(void *, sample, esi);
    GET_REGISTER_STATIC(int, volume, eax);
    static int priority;
    static float fvolume;
    _asm { sub esp, 0x4 } // compensate for fpu push on call.
    _asm { fstp dword ptr [fvolume] } // load value of st(0) (which is previous calculation of volume) into "fvolume".
    priority = Voc_Calculate_Play_Priority(this_ptr, fvolume);
    _asm { add esp, 0x4 }
    Audio_Driver_Play_Sample(sample, priority, volume);
    JMP(0x00664AEA);
}

DECLARE_PATCH(_VocClass_Play_Sample_Patch_2)
{
    GET_REGISTER_STATIC(VocClass *, this_ptr, edi);
    GET_REGISTER_STATIC(void *, sample, esi);
    GET_REGISTER_STATIC(int, volume, eax);
    static int priority;
    static float fvolume;
    _asm { sub esp, 0x4 } // compensate for fpu push on call.
    _asm { fstp dword ptr [fvolume] } // load value of st(0) (which is previous calculation of volume) into "fvolume".
    priority = Voc_Calculate_Play_Priority(this_ptr, fvolume);
    _asm { add esp, 0x4 }
    Audio_Driver_Play_Sample(sample, priority, volume);
    JMP(0x00664B8E);
}

DECLARE_PATCH(_VocClass_Play_Sample_Patch_3)
{
    GET_REGISTER_STATIC(VocClass *, this_ptr, esi);
    GET_REGISTER_STATIC(void *, sample, edi);
    GET_REGISTER_STATIC(int, volume, eax);
    static int priority;
    static float fvolume;
    _asm { sub esp, 0x4 } // compensate for fpu push on call.
    _asm { fstp dword ptr [fvolume] } // load value of st(0) (which is previous calculation of volume) into "fvolume".
    priority = Voc_Calculate_Play_Priority(this_ptr, fvolume);
    _asm { add esp, 0x4 }
    Audio_Driver_Play_Sample(sample, priority, volume);
    JMP(0x00664C49);
}

DECLARE_PATCH(_VocClass_Play_Sample_Patch_4)
{
    GET_REGISTER_STATIC(VocClass *, this_ptr, esi);
    GET_REGISTER_STATIC(void *, sample, edi);
    GET_REGISTER_STATIC(int, volume, eax);
    static int priority;
    static float fvolume;
    _asm { sub esp, 0x4 } // compensate for fpu push on call.
    _asm { fstp dword ptr [fvolume] } // load value of st(0) (which is previous calculation of volume) into "fvolume".
    priority = Voc_Calculate_Play_Priority(this_ptr, fvolume);
    _asm { add esp, 0x4 }
    Audio_Driver_Play_Sample(sample, priority, volume);
    JMP(0x00664CFD);
}

DECLARE_PATCH(_VocClass_Play_Sample_Patch_5)
{
    GET_REGISTER_STATIC(VocClass *, this_ptr, esi);
    GET_REGISTER_STATIC(void *, sample, edi);
    GET_REGISTER_STATIC(int, volume, eax);
    static int priority;
    static float fvolume;
    _asm { sub esp, 0x4 } // compensate for fpu push on call.
    _asm { fstp dword ptr [fvolume] } // load value of st(0) (which is previous calculation of volume) into "fvolume".
    priority = Voc_Calculate_Play_Priority(this_ptr, fvolume);
    _asm { add esp, 0x4 }
    Audio_Driver_Play_Sample(sample, priority, volume);
    JMP(0x00664E9F);
}

AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_Speak_AI_Play_Sample_Patch_1, esi, PRIORITY_MAX, edx, 0x00665AD3);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_WorldDominationTour_Voices_Sample_Play_Sample_Patch_1, ebp, PRIORITY_MAX, eax, 0x0067FD17);
AUDIO_ENGINE_PLAY_SAMPLE_PRIORITY_PATCH(_WorldDominationTour_Voices_Sample_Play_Sample_Patch_2, edi, PRIORITY_MAX, eax, 0x006800CF);

DECLARE_PATCH(_MapSelect_File_Stream_Sample_Vol_Patch_1)
{
    GET_REGISTER_STATIC(char *, filename, ebx);
    static int handle;
    handle = Audio_Driver_File_Stream_Sample_Vol(filename, VOLUME_MAX, false);
    _asm { mov eax, handle }
    JMP_REG(ecx, 0x0055405F);
}

DECLARE_PATCH(_MapSelect_File_Stream_Sample_Vol_Patch_2)
{
    GET_REGISTER_STATIC(char *, filename, eax);
    static int handle;
    handle = Audio_Driver_File_Stream_Sample_Vol(filename, VOLUME_MAX, false);
    _asm { mov eax, handle }
    JMP_REG(ecx, 0x00554602);
}

AUDIO_ENGINE_FILE_STREAM_SAMPLE_VOL_PATCH(_ThemeClass_Play_Song_File_Stream_Sample_Vol_Patch_1, eax, ecx, true, 0x006440C6);

AUDIO_ENGINE_SOUND_CALLBACK_PATCH(_Call_Back_Sound_Callback_Patch_1, 0x00462C91);
AUDIO_ENGINE_SOUND_CALLBACK_PATCH(_OwnerDraw_Window_Procedure_Sound_Callback_Patch_1, 0x00594286);
AUDIO_ENGINE_SOUND_CALLBACK_PATCH(_ThemeClass_AI_Sound_Callback_Patch_1, 0x00643E6F);

AUDIO_ENGINE_SET_VOLUME_ALL_PATCH(_RadarClass_Play_Movie_Set_Volume_All_Patch_1, eax, 0x005BCE33);

AUDIO_ENGINE_SET_VOLUME_PERCENT_ALL_PATCH(_RadarClass_Play_Movie_Set_Volume_Percent_All_Patch_1, 40, 0x005BCC6C); // was 50%

AUDIO_ENGINE_SET_HANDLE_VOLUME_PATCH(_ThemeClass_Set_Volume_Set_Handle_Volume_Patch_1, ecx, eax, 0x00644436);

static void * Get_Speech_Buffer(int index) { return SpeechBuffer[index]; }

DECLARE_PATCH(_Set_Speech_Volume_Set_Sample_Volume_Patch_1)
{
    GET_REGISTER_STATIC(int, volume, ecx);
    GET_REGISTER_STATIC(int, _index, eax);
    static void *sample;
    sample = SpeechBuffer[_index];
    Audio_Driver_Set_Sample_Volume(sample, volume);
    JMP(0x00665BB6);
}

AUDIO_ENGINE_FADE_SAMPLE_PATCH(_MapSelect_Fade_Sample_Patch_1, eax, 20, 0x00554240);
AUDIO_ENGINE_FADE_SAMPLE_PATCH(_MapSelect_Fade_Sample_Patch_2, eax, 20, 0x00554435);
AUDIO_ENGINE_FADE_SAMPLE_PATCH(_MapSelect_Fade_Sample_Patch_3, eax, 20, 0x0055456F);
AUDIO_ENGINE_FADE_SAMPLE_PATCH(_MapSelect_Fade_Sample_Patch_4, eax, 20, 0x00554665);
AUDIO_ENGINE_FADE_SAMPLE_PATCH(_ThemeClass_Queue_Song_Fade_Sample_Patch_1, eax, 60, 0x00643FDB);
AUDIO_ENGINE_FADE_SAMPLE_PATCH(_ThemeClass_Stop_Fade_Sample_Patch_1, ecx, 60, 0x00644218);

AUDIO_ENGINE_START_PRIMARY_SOUND_BUFFER_PATCH(_Focus_Restore_Start_Primary_Sound_Buffer_Patch_1, true, 0x00685A2B);

AUDIO_ENGINE_STOP_PRIMARY_SOUND_BUFFER_PATCH(_Focus_Loss_Stop_Primary_Sound_Buffer_Patch_1, 0x006859C4);
AUDIO_ENGINE_STOP_PRIMARY_SOUND_BUFFER_PATCH(_Main_Window_Procedure_Stop_Primary_Sound_Buffer_Patch_1, 0x00685ED5);

DECLARE_PATCH(_Create_Main_Window_Set_Focus_Loss_Function)
{
    Audio_Driver_Set_Focus_Loss_Function(&Focus_Loss);
    JMP(0x00686330);
}

DECLARE_PATCH(_ThemeClass_Play_Song_Set_Low_Impact_1)
{
    Audio_Driver_Set_Stream_Low_Impact(true);
    JMP(0x00644086);
}

DECLARE_PATCH(_ThemeClass_Play_Song_Set_Low_Impact_2)
{
    Audio_Driver_Set_Stream_Low_Impact(false);
    JMP(0x006440CF);
}


/**
 *  Forces a playback limit on the credits tick sound, otherwise it runs wild.
 * 
 *  @author: CCHyper
 */
bool Fix_Credit_Tick_Sfx = false;
DECLARE_PATCH(_CreditsClass_Graphic_Logic_Limit_Sfx_Patch)
{
    GET_REGISTER_STATIC(CreditClass *, this_ptr, 0x00471493);
    static CDTimerClass<MSTimerClass> _delay = 35;

    if (!Fix_Credit_Tick_Sfx || _delay.Expired()) {
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
 *  Main function for patching the hooks.
 */
void Audio_Hooks()
{
    /**
     *  Install the TS++ wrappers.
     */
    TSPP_Play_Sample_Function_Ptr = Audio_Driver_Play_Sample;
    TSPP_Is_Sample_Playing_Function_Ptr = Audio_Driver_Is_Sample_Playing;

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
    Patch_Jump(0x00643C87, &_ThemeClass_Scan_Is_Available_Patch_1);
    Patch_Jump(0x00643DC0, &_ThemeClass_AI_Is_Available_Patch_1);
    Patch_Jump(0x00643DFD, &_ThemeClass_AI_Is_Available_Patch_2);
    Patch_Jump(0x00643F30, &_ThemeClass_Queue_Song_Is_Available_Patch_1);
    Patch_Jump(0x00643F93, &_ThemeClass_Queue_Song_Is_Available_Patch_2);
    Patch_Jump(0x00643FF2, &_ThemeClass_Play_Song_Is_Available_Patch_1);
    Patch_Jump(0x006441A0, &_ThemeClass_Stop_Is_Available_Patch_1);
    Patch_Jump(0x0064425C, &_ThemeClass_Suspend_Is_Available_Patch_1);
    Patch_Jump(0x006442B9, &_ThemeClass_Still_Playing_Is_Available_Patch_1);
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

    Patch_Jump(0x00554042, &_MapSelect_Stop_Sample_Patch_1);
    Patch_Jump(0x005544BE, &_MapSelect_Stop_Sample_Patch_2);
    Patch_Jump(0x005545E1, &_MapSelect_Stop_Sample_Patch_3);
    Patch_Jump(0x00554667, &_MapSelect_Stop_Sample_Patch_4);
    Patch_Jump(0x0064403D, &_ThemeClass_Play_Song_Stop_Sample_Patch_1);
    Patch_Jump(0x0064422D, &_ThemeClass_Stop_Stop_Sample_Patch_1);
    Patch_Jump(0x0064428F, &_ThemeClass_Suspend_Stop_Sample_Patch_1);
    Patch_Jump(0x0067E8F5, &_WorldDominationTour_Voices_DTOR_Stop_Sample_Patch_1);
    Patch_Jump(0x0067FCA3, &_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_1);
    Patch_Jump(0x0067FD83, &_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_2);
    Patch_Jump(0x0067FE56, &_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_3);
    Patch_Jump(0x0067FE90, &_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_4);
    Patch_Jump(0x0068005D, &_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_5);
    Patch_Jump(0x006800EB, &_WorldDominationTour_Voices_Sample_Stop_Sample_Patch_6);
    Patch_Jump(0x00680186, &_WorldDominationTour_Voices_Anim_Stop_Sample_Patch_1);
    Patch_Jump(0x006801C0, &_WorldDominationTour_Voices_Anim_Stop_Sample_Patch_2);

    Patch_Jump(0x00643E0C, &_ThemeClass_AI_Sample_Status_Patch_1);
    Patch_Jump(0x00643FBD, &_ThemeClass_Queue_Song_Sample_Status_Patch_2);
    Patch_Jump(0x006441E9, &_ThemeClass_Stop_Sample_Status_Patch_3);
    Patch_Jump(0x006442E2, &_ThemeClass_Still_Playing_Sample_Status_Patch_4);
    Patch_Jump(0x0067F88C, &_WorldDominationTour_Voices_Sample_Sample_Status_Patch_1);
    Patch_Jump(0x0067FB40, &_WorldDominationTour_Voices_Sample_Sample_Status_Patch_2);
    Patch_Jump(0x0067FC34, &_WorldDominationTour_Voices_Sample_Sample_Status_Patch_3);
    Patch_Jump(0x0067FC91, &_WorldDominationTour_Voices_Sample_Sample_Status_Patch_4);
    Patch_Jump(0x0067FD71, &_WorldDominationTour_Voices_Sample_Sample_Status_Patch_5);
    Patch_Jump(0x0067FE7E, &_WorldDominationTour_Voices_Sample_Sample_Status_Patch_6);
    Patch_Jump(0x0067FF28, &_WorldDominationTour_Voices_Sample_Sample_Status_Patch_7);
    Patch_Jump(0x0068004B, &_WorldDominationTour_Voices_Sample_Sample_Status_Patch_8);
    Patch_Jump(0x00680088, &_WorldDominationTour_Voices_Sample_Sample_Status_Patch_9);
    Patch_Jump(0x006801AE, &_WorldDominationTour_Anim_Sample_Sample_Status_Patch_1);
    Patch_Jump(0x0068031B, &_WorldDominationTour_Anim_Sample_Sample_Status_Patch_2);

    Patch_Jump(0x00665993, &_Speak_AI_Is_Sample_Playing_Patch_1);
    Patch_Jump(0x00665B55, &_Is_Speaking_Is_Sample_Playing_Patch_1);

    Patch_Jump(0x0056E71E, &_MapSelect_Stop_Sample_Playing_Patch_1);
    Patch_Jump(0x0056E81F, &_MapSelect_Stop_Sample_Playing_Patch_2);
    Patch_Jump(0x0056F3BA, &_MSSfxEntry_Stop_Sample_Playing_Patch_1);
    Patch_Jump(0x00571F82, &_MSFont_Stop_Sample_Playing_Patch_1);
    Patch_Jump(0x00574CAA, &_MSSfx_DTOR_Stop_Sample_Playing_Patch_1);
    Patch_Jump(0x005E4B7A, &_ScoreClass_Presentation_Stop_Sample_Playing_Patch_1);
    Patch_Jump(0x005E68A0, &_ScoreFontClass_DTOR_Stop_Sample_Playing_Patch_1);
    Patch_Jump(0x005E73BA, &_ScoreSoundClass_DTOR_Stop_Sample_Playing_Patch_1);
    Patch_Jump(0x00665B06, &_Stop_Speaking_Stop_Sample_Playing_Patch_1);

    Patch_Jump(0x0056D056, &_MSWordAnim_Play_Sample_Patch_1);
    Patch_Jump(0x0056D15C, &_MSWordAnim_Play_Sample_Patch_2);
    Patch_Jump(0x0056F406, &_MSSfxEntry_Play_Sample_Patch_1);
    Patch_Jump(0x00572568, &_MSFont_Play_Sample_Patch_1);
    Patch_Jump(0x00574CEB, &_MSSfx_Play_Sample_Patch_1);
    Patch_Jump(0x00593DDE, &_OwnerDraw_Window_Procedure_Play_Sample_Patch_1);
    Patch_Jump(0x005E35A9, &_ScoreClass_Play_Sample_Patch_1);
    Patch_Jump(0x005E36F5, &_ScoreClass_Play_Sample_Patch_2);
    Patch_Jump(0x005E3E5A, &_ScoreClass_Play_Sample_Patch_3);
    Patch_Jump(0x005E4426, &_ScoreClass_Play_Sample_Patch_4);
    Patch_Jump(0x005E5824, &_ScoreClass_Play_Sample_Patch_5);
    Patch_Jump(0x005E5FF5, &_ScoreClass_Input_Name_Play_Sample_Patch_1);
    Patch_Jump(0x005E61B5, &_ScoreClass_Input_Name_Play_Sample_Patch_2);
    Patch_Jump(0x005E67D4, &_ScoreFontClass_Play_Sample_Patch_1);
    Patch_Jump(0x005E6BB3, &_ScoreFontClass_Play_Sample_Patch_2);
    Patch_Jump(0x00664ACE, &_VocClass_Play_Sample_Patch_1);
    Patch_Jump(0x00664B72, &_VocClass_Play_Sample_Patch_2);
    Patch_Jump(0x00664C2D, &_VocClass_Play_Sample_Patch_3);
    Patch_Jump(0x00664CE1, &_VocClass_Play_Sample_Patch_4);
    Patch_Jump(0x00664E83, &_VocClass_Play_Sample_Patch_5);
    Patch_Jump(0x00665AC2, &_Speak_AI_Play_Sample_Patch_1);
    Patch_Jump(0x0067FD06, &_WorldDominationTour_Voices_Sample_Play_Sample_Patch_1);
    Patch_Jump(0x006800BE, &_WorldDominationTour_Voices_Sample_Play_Sample_Patch_2);

    Patch_Jump(0x0055404D, &_MapSelect_File_Stream_Sample_Vol_Patch_1);
    Patch_Jump(0x005545F0, &_MapSelect_File_Stream_Sample_Vol_Patch_2);
    Patch_Jump(0x006440B9, &_ThemeClass_Play_Song_File_Stream_Sample_Vol_Patch_1);

    Patch_Jump(0x00462C87, &_Call_Back_Sound_Callback_Patch_1);
    Patch_Jump(0x0059427C, &_OwnerDraw_Window_Procedure_Sound_Callback_Patch_1);
    Patch_Jump(0x00643E65, &_ThemeClass_AI_Sound_Callback_Patch_1);

    Patch_Jump(0x005BCE28, &_RadarClass_Play_Movie_Set_Volume_All_Patch_1);

    Patch_Jump(0x005BCC60, &_RadarClass_Play_Movie_Set_Volume_Percent_All_Patch_1);

    Patch_Jump(0x0064442A, &_ThemeClass_Set_Volume_Set_Handle_Volume_Patch_1);

    Patch_Jump(0x00665BA3, &_Set_Speech_Volume_Set_Sample_Volume_Patch_1);

    Patch_Jump(0x00554233, &_MapSelect_Fade_Sample_Patch_1);
    Patch_Jump(0x00554428, &_MapSelect_Fade_Sample_Patch_2);
    Patch_Jump(0x00554562, &_MapSelect_Fade_Sample_Patch_3);
    Patch_Jump(0x00554658, &_MapSelect_Fade_Sample_Patch_4);
    Patch_Jump(0x00643FCE, &_ThemeClass_Queue_Song_Fade_Sample_Patch_1);
    Patch_Jump(0x0064420B, &_ThemeClass_Stop_Fade_Sample_Patch_1);

    Patch_Jump(0x00685A1F, &_Focus_Restore_Start_Primary_Sound_Buffer_Patch_1);

    Patch_Jump(0x006859BA, &_Focus_Loss_Stop_Primary_Sound_Buffer_Patch_1);
    Patch_Jump(0x00685ECB, &_Main_Window_Procedure_Stop_Primary_Sound_Buffer_Patch_1);

    Patch_Jump(0x00686326, &_Create_Main_Window_Set_Focus_Loss_Function);

    Patch_Jump(0x00644080, &_ThemeClass_Play_Song_Set_Low_Impact_1);
    Patch_Jump(0x006440C8, &_ThemeClass_Play_Song_Set_Low_Impact_2);

#ifdef VINIFERA_AUDIO_HOOK_AHANDLE
    /**
     *  Hooks for the OpenAL audio engine for the VQA player (AHandle).
     */

    // Removes the global initialisation of the DirectSound audio engine.
    Patch_Byte(0x00487990, 0xC3); // asm "ret"
    Patch_Byte(0x004879B0, 0xC3); // asm "ret"

    //Patch_Dword(0x00407765+1, (uintptr_t)&OpenALAudio);      // AHandle
    //Patch_Dword(0x0040784C+1, (uintptr_t)&OpenALAudio);      // AHandle
    //Patch_Dword(0x004078CD+1, (uintptr_t)&OpenALAudio);      // AHandle
    //Patch_Dword(0x004078E6+1, (uintptr_t)&OpenALAudio);      // AHandle
    //Patch_Dword(0x00407A87+1, (uintptr_t)&OpenALAudio);      // AHandle
    //Patch_Dword(0x00407A9E+1, (uintptr_t)&OpenALAudio);      // AHandle
    //Patch_Dword(0x00407AC6+1, (uintptr_t)&OpenALAudio);      // AHandle
    //Patch_Dword(0x00407ADD+1, (uintptr_t)&OpenALAudio);      // AHandle
    //Patch_Dword(0x00407C7D+1, (uintptr_t)&OpenALAudio);      // AHandle

    //Patch_Dword(0x00407683+1, (uintptr_t)&OpenALAudio.AudioDone);     // AHandle

    //Patch_Dword(0x004077F2+1, (uintptr_t)&OpenALAudio.PrimaryBufferPtr);      // AHandle
    //Patch_Dword(0x004078A5+1, (uintptr_t)&OpenALAudio.PrimaryBufferPtr);      // AHandle
    //Patch_Dword(0x00407AB2+1, (uintptr_t)&OpenALAudio.PrimaryBufferPtr);      // AHandle

    //Patch_Dword(0x00407673+1, (uintptr_t)&OpenALAudio.OpenALDevice);      // AHandle
    //Patch_Dword(0x0040776F+1, (uintptr_t)&OpenALAudio.OpenALDevice);      // AHandle

    //Patch_Jump(0x004879C0, &Convert_HMI_To_Direct_Sound_Volume);              // Not needed

    //Patch_Call(0x004078D4, &Start_Primary_Sound_Buffer);        // AHandle
    //Patch_Call(0x00407ACB, &Start_Primary_Sound_Buffer);        // AHandle

    //Patch_Call(0x00407851, &Stop_Primary_Sound_Buffer);        // AHandle
    //Patch_Call(0x00407AA3, &Stop_Primary_Sound_Buffer);        // AHandle

    // #NOTE: Seems like we only these two need hooking.
    Patch_Dword(0x0066B464+6, (uintptr_t)&OpenAL_AHandle::func_4072D0);
    Patch_Dword(0x0066B48D+6, (uintptr_t)&OpenAL_AHandle::Stream_Handler);

    //Patch_Jump(0x00407650, &OpenAL_AHandle::func_407650);     // Not needed
    //Patch_Jump(0x00407980, &OpenAL_AHandle::func_407980);     // Not needed
    //Patch_Jump(0x00407B20, &OpenAL_AHandle::func_407B20);     // Not needed
    //Patch_Jump(0x00407D30, &OpenAL_AHandle::func_407D30);     // Not needed
    //Patch_Jump(0x00407EF0, &OpenAL_AHandle::func_407EF0);     // Not needed
    //Patch_Jump(0x00407F40, &OpenAL_AHandle::func_407F40);     // Not needed
    //Patch_Jump(0x00407FE0, &OpenAL_AHandle::Stop_Audio_Handler);     // Not needed
    //Patch_Jump(0x00408060, &OpenAL_AHandle::Sound_Timer_Callback);   // Not needed
    //Patch_Jump(0x00408200, &OpenAL_AHandle::func_408200);     // no need to patch, handles timer fine.
    //Patch_Jump(0x00408210, &OpenAL_AHandle::func_408210);     // Not needed
    //Patch_Jump(0x00408260, &OpenAL_AHandle::func_408260);     // Not needed
    //Patch_Jump(0x004082B0, &OpenAL_AHandle::func_4082B0);     // Not needed
    //Patch_Jump(0x004082C0, &OpenAL_AHandle::func_4082C0);     // Not needed
#endif

    /**
     *  #BUGFIX: To normalise the credits tick sound if required.
     */
    Patch_Jump(0x00471493, &_CreditsClass_Graphic_Logic_Limit_Sfx_Patch);
}
