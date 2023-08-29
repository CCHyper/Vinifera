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
#include "audio_theme.h"
#include "audio_vox.h"
#include "audio_voc.h"
#include "theme.h"
#include "addon.h"
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


#ifdef USE_MINIAUDIO
extern AudioThemeClass AudioTheme;
#endif


/**
 *  These wrappers are required due to the stack being used when calling
 *  the Theme functions function.
 */
void Theme_Queue_Song(ThemeType theme)
{
#ifdef USE_MINIAUDIO
    AudioTheme.Queue_Song(theme);
#else
    Theme.Queue_Song(theme);
#endif
}

bool Theme_Play_Song(ThemeType theme)
{
#ifdef USE_MINIAUDIO
    return AudioTheme.Play_Song(theme);
#else
    return Theme.Play_Song(theme);
#endif
}

void Theme_Stop(bool fade)
{
#ifdef USE_MINIAUDIO
    AudioTheme.Stop(fade);
#else
    Theme.Stop(fade);
#endif
}

void Theme_Suspend()
{
#ifdef USE_MINIAUDIO
    AudioTheme.Suspend();
#else
    Theme.Suspend();
#endif
}

void Theme_Resume()
{
#ifdef USE_MINIAUDIO
    AudioTheme.Resume();
#else
    // do nothing...
#endif
}

bool Theme_Is_Paused()
{
#ifdef USE_MINIAUDIO
    return AudioTheme.Is_Paused();
#else
    return Theme.What_Is_Playing() != THEME_NONE;
#endif
}

ThemeType Theme_What_Is_Playing()
{
#ifdef USE_MINIAUDIO
    return AudioTheme.What_Is_Playing();
#else
    return Theme.What_Is_Playing();
#endif
}

int Theme_Max_Themes()
{
#ifdef USE_MINIAUDIO
    return AudioTheme.Max_Themes();
#else
    return Theme.Max_Themes();
#endif
}

bool Theme_Is_Allowed(ThemeType theme)
{
#ifdef USE_MINIAUDIO
    return AudioTheme.Is_Allowed(theme);
#else
    return Theme.Is_Allowed(theme);
#endif
}

const char * Theme_Full_Name(ThemeType theme)
{
#ifdef USE_MINIAUDIO
    return AudioTheme.Full_Name(theme);
#else
    return Theme.Full_Name(theme);
#endif
}

void Theme_Clear()
{
#ifdef USE_MINIAUDIO
    AudioTheme.Clear();
#else
    Theme.Clear();
#endif
}

void Theme_Scan()
{
#ifdef USE_MINIAUDIO
    AudioTheme.Scan();
#else
    Theme.Scan();
#endif
}

int Theme_Process(CCINIClass &ini)
{
#ifdef USE_MINIAUDIO
    return AudioTheme.Process(ini);
#else
    return Theme.Process(ini);
#endif
}

bool Theme_Fill_In_All()
{
#ifdef USE_MINIAUDIO
    CCFileClass theme_file("THEME.INI");
    CCINIClass theme_ini;

    if (!theme_file.Is_Available()) {
        DEBUG_WARNING("Failed to find THEME.INI!\n");
        return false;
    }
    if (!theme_ini.Load(theme_file, false)) {
        DEBUG_WARNING("Failed to load THEME.INI!\n");
        return false;
    }
    if (!AudioTheme.Fill_In_All(theme_ini)) {
        DEBUG_WARNING("Fill_In_All(THEME.INI) returned false!\n");
        return false;
    }

    if (Addon_Installed(ADDON_FIRESTORM)) {
        theme_file.Set_Name("THEME01.INI");
        theme_ini.Clear();

        if (!theme_ini.Load(theme_file, false)) {
            DEBUG_WARNING("Failed to load THEME01.INI!\n");
            return false;
        }
        if (!AudioTheme.Fill_In_All(theme_ini)) {
            DEBUG_WARNING("Fill_In_All(THEME01.INI) returned false!\n");
            return false;
        }
    }

#endif

    return true;
}
