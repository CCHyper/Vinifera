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
#include "vinifera_globals.h"
#include "wstring.h"
#include "dsaudio.h"
#include "debughandler.h"


class CCINIClass;


/**
 *  x
 */
#define AUDIO_DEBUG_INFO(x, ...) if (Vinifera_AudioDebug) { DEBUG_INFO(x, ##__VA_ARGS__); } 
#define AUDIO_DEBUG_WARNING(x, ...) if (Vinifera_AudioDebug) { DEBUG_WARNING(x, ##__VA_ARGS__); } 
#define AUDIO_DEBUG_ERROR(x, ...) if (Vinifera_AudioDebug) { DEBUG_ERROR(x, ##__VA_ARGS__); } 


/**
 *  These wrappers are required due to the stack being used when calling
 *  a static function.
 */
void Theme_Queue_Song(ThemeType theme);
bool Theme_Play_Song(ThemeType theme);
void Theme_Stop(bool fade);
void Theme_Suspend();
void Theme_Resume();
bool Theme_Is_Paused();
ThemeType Theme_What_Is_Playing();
int Theme_Max_Themes();
bool Theme_Is_Allowed(ThemeType theme);
const char * Theme_Full_Name(ThemeType theme);
void Theme_Clear();
void Theme_Scan();
int Theme_Process(CCINIClass &ini);
bool Theme_Fill_In_All();
