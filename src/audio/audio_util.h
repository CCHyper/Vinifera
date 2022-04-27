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
#include "always.h"
#include "tibsun_defines.h"
#include "wstring.h"
#include "dsaudio.h"
#include "debughandler.h"


class CCFileClass;
class INIClass;


/**
 *  Default audio driver values.
 */
#define AUDIO_DEFAULT_BITS 16
#define AUDIO_DEFAULT_STEREO 1
#define AUDIO_DEFAULT_RATE 48000
#define AUDIO_DEFAULT_NUM_TRACKERS 5


/**
 *  
 */
extern Wstring AudioDriverName;
extern unsigned int AudioMaxSimultaneousSounds;
extern bool AudioCrossfadeMusic;


/**
 *  
 */
void Theme_Queue_Song(ThemeType theme);
bool Theme_Play_Song(ThemeType theme);
void Theme_Stop();
void Theme_Pause();
void Theme_UnPause();
bool Theme_Is_Paused();
ThemeType Theme_What_Is_Playing();
int Theme_Max_Themes();
bool Theme_Is_Allowed(ThemeType theme);
const char * Theme_Full_Name(ThemeType theme);


/**
 *  Utility functions for converting the integer audio volume to and from float.
 */
unsigned int Audio_fVolume_To_iVolume(float vol);
float Audio_iVolume_To_fVolume(unsigned int vol);


/**
 *  
 */
bool Audio_Read_INI(INIClass &ini);


/**
 *  These wrappers are required due to the stack being used when calling
 *  a static function.
 */
bool Audio_Driver_Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers);
void Audio_Driver_End();
bool Audio_Driver_Is_Available();
bool Audio_Driver_Is_Enabled();
void Audio_Driver_Enabled();
void Audio_Driver_Disable();
void Audio_Driver_Focus_Loss();
void Audio_Driver_Focus_Restore();
bool Audio_Driver_In_Focus();
bool Audio_Driver_Start_Engine(bool forced = false);
void Audio_Driver_Stop_Engine();
void Audio_Driver_Sound_Callback();
