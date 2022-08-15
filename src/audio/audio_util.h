/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_UTIL.H
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
#pragma once

#include "always.h"
#include "tibsun_defines.h"
#include "wstring.h"
#include "dsaudio.h"
#include "debughandler.h"

#ifdef USE_FMOD_AUDIO
#include <fmod.hpp>
#include <fmod_errors.h>
#endif


/**
 *  Utility functions for converting the integer audio volume to and from float.
 */
unsigned int Audio_fVolume_To_iVolume(float vol);
float Audio_iVolume_To_fVolume(unsigned int vol);

#ifdef USE_FMOD_AUDIO
/**
 *  Utility functions for converting from game priority to FMOD priority.
 */
unsigned char Audio_To_FMOD_Priority(unsigned char priority);
unsigned char Audio_From_FMOD_Priority(unsigned char priority);
#endif


/**
 *  These wrappers are required due to the stack being used when calling
 *  a static function.
 */
void Theme_Queue_Song(ThemeType theme);
bool Theme_Play_Song(ThemeType theme);
void Theme_Stop();
void Theme_Suspend();
void Theme_Resume();
bool Theme_Is_Paused();
ThemeType Theme_What_Is_Playing();
int Theme_Max_Themes();
bool Theme_Is_Allowed(ThemeType theme);
const char * Theme_Full_Name(ThemeType theme);


/**
 *  These wrappers are required due to the stack being used when calling
 *  a static function.
 */
bool Audio_Init(HWND hWnd);
void Audio_End();
bool Audio_Is_Available();
bool Audio_Is_Enabled();
void Audio_Enabled();
void Audio_Disable();
void Audio_Focus_Loss();
void Audio_Focus_Restore();
bool Audio_Start_Engine(bool forced = false);
void Audio_Stop_Engine();
void Audio_Sound_Callback();
LPDIRECTSOUND Audio_Get_Sound_Object();


void Audio_Wait_For_Threads();


#ifdef USE_FMOD_AUDIO
#ifndef NDEBUG
#define FMOD_ERRCHECK(_result) \
    if (_result != FMOD_OK) { \
        ASSERT_PRINT(true, FMOD_ErrorString(_result)); \
    }
#else
#define FMOD_ERRCHECK(_result) \
    if (_result != FMOD_OK) { \
        DEBUG_ERROR("FMOD Error - %s, File: %s, Line %d\n", FMOD_ErrorString(_result), __FILE__, __LINE__); \
    }
#endif
#endif
