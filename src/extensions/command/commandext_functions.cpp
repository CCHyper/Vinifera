/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          COMMANDEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended command class.
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
#include "commandext_functions.h"
#include "tibsun_defines.h"
#include "tibsun_globals.h"
#include "theme.h"
#include "rules.h"
#include "tacticalext.h"
#include "tactical.h"
#include "extension_globals.h"
#include "audio_util.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  
 */
static CDTimerClass<MSTimerClass> _theme_command_cooldown = SECONDS_TO_MILLISECONDS(2);


/**
 *  Skips to the previous available music track allowed.
 * 
 *  @author: CCHyper
 */
bool Prev_Theme_Command()
{
    if (!_theme_command_cooldown.Expired()) {
        return false;
    }

    ThemeType theme = Theme_What_Is_Playing();

    /**
     *  Iterate backward from the current theme and find the next available
     *  music track we can play.
     */
    while (theme >= THEME_FIRST) {

        --theme;

        if (theme < THEME_FIRST) {
            theme = ThemeType(Theme_Max_Themes())-1;
        }

        if (Theme_Is_Allowed(theme)) {
            break;
        }

    }

    /**
     *  Queue the track for playback. We need to stop the track first
     *  otherwise Queue_Song() will fade the track out.
     */
    Theme_Stop();
    Theme_Queue_Song(theme);

    /**
     *  Print the chosen music track name on the screen.
     */
    TacticalMapExtension->InfoTextTimer.Stop();

    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "Now Playing: %s", Theme_Full_Name(theme));

    TacticalMapExtension->Set_Info_Text(buffer);
    TacticalMapExtension->IsInfoTextSet = true;

    TacticalMapExtension->InfoTextPosition = InfoTextPosType::BOTTOM_LEFT;

    //TacticalMapExtension->InfoTextNotifySound = Rule->OptionsChanged;
    //TacticalMapExtension->InfoTextNotifySoundVolume = 0.5f;

    TacticalMapExtension->InfoTextTimer = SECONDS_TO_MILLISECONDS(4);
    TacticalMapExtension->InfoTextTimer.Start();

    _theme_command_cooldown = SECONDS_TO_MILLISECONDS(2);
    
    return true;
}


/**
 *  Skips to the next available music track allowed.
 * 
 *  @author: CCHyper
 */
bool Next_Theme_Command()
{
    if (!_theme_command_cooldown.Expired()) {
        return false;
    }

    ThemeType theme = Theme_What_Is_Playing();

    /**
     *  Iterate forward from the current theme and find the next available
     *  music track we can play.
     */
    while (theme < ThemeType(Theme_Max_Themes())) {

        ++theme;

        if (theme >= ThemeType(Theme_Max_Themes())) {
            theme = ThemeType(THEME_FIRST);
        }

        if (Theme_Is_Allowed(theme)) {
            break;
        }

    }

    /**
     *  Queue the track for playback. We need to stop the track first
     *  otherwise Queue_Song() will fade the track out.
     */
    Theme_Stop();
    Theme_Queue_Song(theme);

    /**
     *  Print the chosen music track name on the screen.
     */
    TacticalMapExtension->InfoTextTimer.Stop();

    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "Now Playing: %s", Theme_Full_Name(theme));

    TacticalMapExtension->Set_Info_Text(buffer);
    TacticalMapExtension->IsInfoTextSet = true;
    
    TacticalMapExtension->InfoTextPosition = InfoTextPosType::BOTTOM_LEFT;

    //TacticalMapExtension->InfoTextNotifySound = Rule->OptionsChanged;
    //TacticalMapExtension->InfoTextNotifySoundVolume = 0.5f;

    TacticalMapExtension->InfoTextTimer = SECONDS_TO_MILLISECONDS(4);
    TacticalMapExtension->InfoTextTimer.Start();

    _theme_command_cooldown = SECONDS_TO_MILLISECONDS(2);

    return true;
}

/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Play_Pause_Theme_Command()
{
    static bool _is_paused = false;

    Wstring msg = "";

    if (!_is_paused) {
        Theme_Suspend();
        msg = "Music: Pause";

    } else {
        Theme_Resume();

        //ThemeType theme = Theme_What_Is_Playing();

        //char buffer[256];
        //std::snprintf(buffer, sizeof(buffer), "Music: Resume : %s", Theme_Full_Name(theme));

        msg = "Music: Resume";
    }

    _is_paused = !_is_paused;

    /**
     *  Print the chosen music track name on the screen.
     */
    TacticalMapExtension->InfoTextTimer.Stop();

    TacticalMapExtension->Set_Info_Text(msg.Peek_Buffer());
    TacticalMapExtension->IsInfoTextSet = true;

    TacticalMapExtension->InfoTextPosition = InfoTextPosType::BOTTOM_LEFT;

    //TacticalMapExtension->InfoTextNotifySound = Rule->OptionsChanged;
    //TacticalMapExtension->InfoTextNotifySoundVolume = 0.5f;

    TacticalMapExtension->InfoTextTimer = SECONDS_TO_MILLISECONDS(4);
    TacticalMapExtension->InfoTextTimer.Start();

    return true;
}
