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
#include "audio_manager.h"
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
#include "critsection.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <algorithm>
#include <iostream>


#ifdef USE_FMOD_AUDIO
extern FMODThemeClass FMODTheme;
#endif


/**
 *  Utility functions for converting the integer audio volume to and from float.
 * 
 *  @author: CCHyper
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


#ifdef USE_FMOD_AUDIO
/**
 *  Utility functions for converting from game priority to FMOD priority.
 * 
 *  @author: CCHyper
 */
unsigned char Audio_To_FMOD_Priority(unsigned char priority)
{
    priority &= 0xFF;
    priority ^= 0xFF;
    return priority;
}

unsigned char Audio_From_FMOD_Priority(unsigned char priority)
{
    priority &= 0xFF;
    priority ^= 0xFF;
    return priority;
}
#endif


/**
 *  These wrappers are required due to the stack being used when calling
 *  the Theme functions function.
 */
void Theme_Queue_Song(ThemeType theme)
{
#ifdef USE_FMOD_AUDIO
    FMODTheme.Queue_Song(theme);
#else
    Theme.Queue_Song(theme);
#endif
}

bool Theme_Play_Song(ThemeType theme)
{
#ifdef USE_FMOD_AUDIO
    return FMODTheme.Play_Song(theme);
#else
    return Theme.Play_Song(theme);
#endif
}

void Theme_Stop()
{
#ifdef USE_FMOD_AUDIO
    FMODTheme.Stop();
#else
#endif
}

void Theme_Suspend()
{
#ifdef USE_FMOD_AUDIO
    FMODTheme.Suspend();
#else
    Theme.Suspend();
#endif
}

void Theme_Resume()
{
#ifdef USE_FMOD_AUDIO
    FMODTheme.Resume();
#else
    // do nothing...
#endif
}

bool Theme_Is_Paused()
{
#ifdef USE_FMOD_AUDIO
    return FMODTheme.Is_Paused();
#else
    return Theme.What_Is_Playing() != THEME_NONE;
#endif
}

ThemeType Theme_What_Is_Playing()
{
#ifdef USE_FMOD_AUDIO
    return FMODTheme.What_Is_Playing();
#else
    return Theme.What_Is_Playing();
#endif
}

int Theme_Max_Themes()
{
#ifdef USE_FMOD_AUDIO
    return FMODTheme.Max_Themes();
#else
    return Theme.Max_Themes();
#endif
}

bool Theme_Is_Allowed(ThemeType theme)
{
#ifdef USE_FMOD_AUDIO
    return FMODTheme.Is_Allowed(theme);
#else
    return Theme.Is_Allowed(theme);
#endif
}

const char * Theme_Full_Name(ThemeType theme)
{
#ifdef USE_FMOD_AUDIO
    return FMODTheme.Full_Name(theme);
#else
    return Theme.Full_Name(theme);
#endif
}


#if 0
/**
 *  Wrappers to the games DirectSound system.
 */
static bool Direct_Sound_Init(HWND hWnd)
{
    DEV_DEBUG_INFO("Audio: Initialising DirectSound.\n");
    return Audio.Init(hWnd, 16, false, 22050);
}

static void Direct_Sound_End()
{
    DEV_DEBUG_INFO("Audio: Shutting down DirectSound.\n");
    Audio.End();
}

static bool Direct_Sound_Is_Available()
{
    return Audio.Is_Available();
}

static bool Direct_Sound_Start_Engine()
{
    DEV_DEBUG_INFO("Audio: Starting DirectSound.\n");
    return Audio.Start_Primary_Sound_Buffer(true);
}

static void Direct_Sound_Stop_Engine()
{
    DEV_DEBUG_INFO("Audio: Stopping DirectSound.\n");
    Audio.Stop_Primary_Sound_Buffer();
}
#endif


/**
 *  These wrappers are required due to the stack being used when calling
 *  the Audio static function.
 */
bool Audio_Init(HWND hWnd)
{
#ifdef USE_FMOD_AUDIO
    return Audio.Init(hWnd, 16, false, 22050) && AudioManager.Init(hWnd);
#else
    return Audio.Init(hWnd, 16, false, 22050);
#endif
}

void Audio_End()
{
#ifdef USE_FMOD_AUDIO
    Audio.End();
    AudioManager.End();
#else
    Audio.End();
#endif
}

bool Audio_Is_Available()
{
#ifdef USE_FMOD_AUDIO
    return /*Audio.Is_Available() &&*/ AudioManager.Is_Available();
#else
    return Audio.Is_Available();
#endif
}

bool Audio_Is_Enabled()
{
#ifdef USE_FMOD_AUDIO
    return /*Audio.Is_Available() &&*/ AudioManager.Is_Enabled();
#else
    return Audio.Is_Available();
#endif
}

void Audio_Enabled()
{
#ifdef USE_FMOD_AUDIO
    Audio.Start_Primary_Sound_Buffer();
    AudioManager.Enable();
#else
    Audio.Start_Primary_Sound_Buffer();
#endif
}

void Audio_Disable()
{
#ifdef USE_FMOD_AUDIO
    Audio.Stop_Primary_Sound_Buffer();
    AudioManager.Disable();
#else
    Audio.Stop_Primary_Sound_Buffer();
#endif
}

void Audio_Focus_Loss()
{
#ifdef USE_FMOD_AUDIO
    Audio.Start_Primary_Sound_Buffer(true);
    AudioManager.Focus_Loss();
#else
    Audio.Start_Primary_Sound_Buffer(true);
#endif
}

void Audio_Focus_Restore()
{
#ifdef USE_FMOD_AUDIO
    Audio.Stop_Primary_Sound_Buffer();
    AudioManager.Focus_Restore();
#else
    Audio.Stop_Primary_Sound_Buffer();
#endif
}

bool Audio_Start_Engine(bool forced)
{
#ifdef USE_FMOD_AUDIO
    return Audio.Start_Primary_Sound_Buffer(true) && AudioManager.Start_Engine(forced);
#else
    return Audio.Start_Primary_Sound_Buffer(true);
#endif
}

void Audio_Stop_Engine()
{
#ifdef USE_FMOD_AUDIO
    Audio.Stop_Primary_Sound_Buffer();
    AudioManager.Stop_Engine();
#else
    Audio.Stop_Primary_Sound_Buffer();
#endif
}

void Audio_Sound_Callback()
{
#ifdef USE_FMOD_AUDIO
    Audio.Sound_Callback();
    AudioManager.Sound_Callback();
#else
    Audio.Sound_Callback();
#endif
}

LPDIRECTSOUND Audio_Get_Sound_Object()
{
#ifdef USE_FMOD_AUDIO
    return LPDIRECTSOUND(AudioManager.Get_Output_Handle());
#else
    return Audio.Get_Sound_Object();
#endif
}


/**
 *  Wait for the scanning threads to finish.
 */
void Audio_Wait_For_Threads()
{
#ifdef USE_FMOD_AUDIO
    //FMODVocClass::Wait_For_Scan_Thread();
    //FMODVoxClass::Wait_For_Scan_Thread();
    //FMODThemeClass::Wait_For_Scan_Thread();
#endif
}
