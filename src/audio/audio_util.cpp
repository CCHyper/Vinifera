/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_UTIL.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Various audio utility functions.
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
#include "audio_util.h"
#include "tibsun_globals.h"
#include "vinifera_globals.h"
#include "audio_driver.h"
#include "audio_theme.h"
#include "audio_vox.h"
#include "audio_voc.h"
#include "theme.h"
#include "dsaudio.h"
#include "wwaud.h"
#include "wstring.h"
#include "ramfile.h"
#include "ccfile.h"
#include "ccini.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <algorithm>


Wstring AudioDriverName = "DirectSound";
unsigned int AudioMaxSimultaneousSounds = AUDIO_DEFAULT_NUM_TRACKERS; // Original value from the TibSun DirectSound audio engine.
bool AudioCrossfadeMusic = false;


extern NewThemeClass NewTheme;
extern bool NewThemeClassHooked;


void Theme_Queue_Song(ThemeType theme)
{
    if (NewThemeClassHooked) {
        NewTheme.Queue_Song(theme);
    } else {
        Theme.Queue_Song(theme);
    }
}

bool Theme_Play_Song(ThemeType theme)
{
    if (NewThemeClassHooked) {
        return NewTheme.Play_Song(theme);
    } else {
        return Theme.Play_Song(theme) != INVALID_AUDIO_HANDLE;
    }
}

void Theme_Stop()
{
    if (NewThemeClassHooked) {
        NewTheme.Stop();
    } else {
        Theme.Stop();
    }
}

void Theme_Pause()
{
    if (NewThemeClassHooked) {
        NewTheme.Pause();
    }
}

void Theme_UnPause()
{
    if (NewThemeClassHooked) {
        NewTheme.UnPause();
    }
}

bool Theme_Is_Paused()
{
    if (NewThemeClassHooked) {
        return NewTheme.Is_Paused();
    }

    return false;
}

ThemeType Theme_What_Is_Playing()
{
    if (NewThemeClassHooked) {
        return NewTheme.What_Is_Playing();
    } else {
        return Theme.What_Is_Playing();
    }
}

int Theme_Max_Themes()
{
    if (NewThemeClassHooked) {
        return NewTheme.Max_Themes();
    } else {
        return Theme.Max_Themes();
    }
}

bool Theme_Is_Allowed(ThemeType theme)
{
    if (NewThemeClassHooked) {
        return NewTheme.Is_Allowed(theme);
    } else {
        return Theme.Is_Allowed(theme);
    }
}

const char * Theme_Full_Name(ThemeType theme)
{
    if (NewThemeClassHooked) {
        return NewTheme.Full_Name(theme);
    } else {
        return Theme.Full_Name(theme);
    }
}


/**
 *  x
 */
static bool Direct_Sound_Init()
{
#ifndef VINIFERA_AUDIO_HOOK_AHANDLE
    DEV_DEBUG_INFO("Audio: Initialising DirectSound.\n");
    return Audio.Init(MainWindow, 16, false, 22050);
#else
    return true;
#endif
}

static void Direct_Sound_End()
{
#ifndef VINIFERA_AUDIO_HOOK_AHANDLE
    DEV_DEBUG_INFO("Audio: Shutting down DirectSound.\n");
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

static bool Direct_Sound_Start_Engine()
{
#ifndef VINIFERA_AUDIO_HOOK_AHANDLE
    DEV_DEBUG_INFO("Audio: Starting DirectSound.\n");
    return Audio.Start_Primary_Sound_Buffer(true);
#else
    return true;
#endif
}

static void Direct_Sound_Stop_Engine()
{
#ifndef VINIFERA_AUDIO_HOOK_AHANDLE
    DEV_DEBUG_INFO("Audio: Stopping DirectSound.\n");
    Audio.Stop_Primary_Sound_Buffer();
#endif
}


/**
 *  Utility functions for converting the integer audio volume to and from float.
 */
unsigned int Audio_fVolume_To_iVolume(float vol)
{
    vol = std::clamp(vol, 0.0f, 1.0f);
    return (vol * 255);
}

float Audio_iVolume_To_fVolume(unsigned int vol)
{
    return float(vol) / 255.0f;
}


/**
 *  
 */
bool Audio_Read_INI(INIClass &ini)
{
    static char const * const AUDIO = "Audio";

    char buffer[128];

    ini.Get_String(AUDIO, "Driver", AudioDriverName.Peek_Buffer(), buffer, sizeof(buffer));
    AudioDriverName = buffer;

    AudioMaxSimultaneousSounds = ini.Get_Int_Clamp(AUDIO, "MaxSimultaneousSounds", 4, 64, AudioMaxSimultaneousSounds);

    AudioCrossfadeMusic = ini.Get_Bool(AUDIO, "CrossfadeMusic", AudioCrossfadeMusic);

    return AudioDriverName.Is_Not_Empty();
}


/**
 *  These wrappers are required due to the stack being used when calling
 *  the Audio_Driver static function.
 */
bool Audio_Driver_Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers)
{
    return Direct_Sound_Init() && Audio_Driver()->Init(hWnd, bits_per_sample, stereo, rate, num_trackers);
}

void Audio_Driver_End()
{
    Direct_Sound_End();
    Audio_Driver()->End();
}

bool Audio_Driver_Is_Available()
{
    return Direct_Sound_Is_Available() && Audio_Driver()->Is_Available();
}

bool Audio_Driver_Is_Enabled()
{
    return Audio_Driver()->Is_Enabled();
}

void Audio_Driver_Enabled()
{
    Audio_Driver()->Enable();
}

void Audio_Driver_Disable()
{
    Audio_Driver()->Disable();
}

void Audio_Driver_Focus_Loss()
{
    Direct_Sound_Start_Engine();
    Audio_Driver()->Focus_Loss();
}

void Audio_Driver_Focus_Restore()
{
    Direct_Sound_Stop_Engine();
    Audio_Driver()->Focus_Restore();
}

bool Audio_Driver_In_Focus()
{
    return Audio_Driver()->In_Focus();
}

bool Audio_Driver_Start_Engine(bool forced)
{
    return Direct_Sound_Start_Engine() && Audio_Driver()->Start_Engine(forced);
}

void Audio_Driver_Stop_Engine()
{
    Direct_Sound_Stop_Engine();
    Audio_Driver()->Stop_Engine();
}

void Audio_Driver_Sound_Callback()
{
    Audio_Driver()->Sound_Callback();
}
