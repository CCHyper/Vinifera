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
#include "vinifera_globals.h"
#include "theme.h"
#include "rules.h"
#include "tacticalext.h"
#include "tactical.h"
#include "iomap.h"
#include "techno.h"
#include "foot.h"
#include "house.h"
#include "ttimer.h"
#include "stimer.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Skips to the previous available music track allowed.
 * 
 *  @author: CCHyper
 */
bool Prev_Theme_Command()
{
    ThemeType theme = Theme.What_Is_Playing();

    /**
     *  Iterate backward from the current theme and find the next available
     *  music track we can play.
     */
    while (theme >= THEME_FIRST) {

        --theme;

        if (theme < THEME_FIRST) {
            theme = ThemeType(Theme.Max_Themes());
        }

        if (Theme.Is_Allowed(theme)) {
            break;
        }

    }

    /**
     *  Queue the track for playback. We need to stop the track first
     *  otherwise Queue_Song() will fade the track out.
     */
    Theme.Stop();
    Theme.Queue_Song(theme);

    /**
     *  Print the chosen music track name on the screen.
     */
    if (TacticalExtension) {

        TacticalExtension->InfoTextTimer.Stop();

        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "Now Playing: %s", Theme.ThemeClass::Full_Name(theme));

        TacticalExtension->InfoTextBuffer = buffer;
        TacticalExtension->IsInfoTextSet = true;

        TacticalExtension->InfoTextPosition = InfoTextPosType::BOTTOM_LEFT;

        //TacticalExtension->InfoTextNotifySound = Rule->OptionsChanged;
        //TacticalExtension->InfoTextNotifySoundVolume = 0.5f;

        TacticalExtension->InfoTextTimer = SECONDS_TO_MILLISECONDS(4);
        TacticalExtension->InfoTextTimer.Start();
    }

    return true;
}


/**
 *  Skips to the next available music track allowed.
 * 
 *  @author: CCHyper
 */
bool Next_Theme_Command()
{
    ThemeType theme = Theme.What_Is_Playing();

    /**
     *  Iterate forward from the current theme and find the next available
     *  music track we can play.
     */
    while (theme < ThemeType(Theme.Max_Themes())) {

        ++theme;

        if (theme >= ThemeType(Theme.Max_Themes())) {
            theme = ThemeType(THEME_FIRST);
        }

        if (Theme.Is_Allowed(theme)) {
            break;
        }

    }

    /**
     *  Queue the track for playback. We need to stop the track first
     *  otherwise Queue_Song() will fade the track out.
     */
    Theme.Stop();
    Theme.Queue_Song(theme);

    /**
     *  Print the chosen music track name on the screen.
     */
    if (TacticalExtension) {

        TacticalExtension->InfoTextTimer.Stop();

        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "Now Playing: %s", Theme.ThemeClass::Full_Name(theme));

        TacticalExtension->InfoTextBuffer = buffer;
        TacticalExtension->IsInfoTextSet = true;
        
        TacticalExtension->InfoTextPosition = InfoTextPosType::BOTTOM_LEFT;

        //TacticalExtension->InfoTextNotifySound = Rule->OptionsChanged;
        //TacticalExtension->InfoTextNotifySoundVolume = 0.5f;

        TacticalExtension->InfoTextTimer = SECONDS_TO_MILLISECONDS(4);
        TacticalExtension->InfoTextTimer.Start();
    }

    return true;
}


/**
 *  Select members of the specified team id. This function also handles
 *  centering the tactical view on the current team if called within the
 *  last execution window.
 * 
 *  @author: CCHyper
 */
bool Select_Team_Command(int group_id)
{
    static CDTimerClass<MSTimerClass> _timer(1000);

    static int _last_group_id = -1; // The group id of the last team this command was executed on.
    static int window_ms = 1000; //800 // double press window in milliseconds.

    Map.Power_Mode_Control(0);
    Map.Waypoint_Mode_Control(0, false);
    Map.Repair_Mode_Control(0);
    Map.Sell_Mode_Control(0);

    int current_group_id = group_id;

    /**
     *  If the user has executed this command for a second time and it
     *  is within the window of the previous execution.
     */
    if (!_timer.Expired() && current_group_id == _last_group_id) {
        
        int techno_count = Technos.Count()-1;
        while (--techno_count) {

            if (techno_count < 0) {
                return Center_On_Team_Command(current_group_id);
            }

            TechnoClass *techno = Technos[techno_count];
            if (techno && !techno->IsInLimbo) {
                if (techno->House->Is_Player_Control()) {
                    if (techno->Group != (current_group_id-1)) {
                        if (techno->IsSelected) {
                            break;
                        }
                    }
                }
            }

        }

    }

    /**
     *  Reset the count down timer for the double press window.
     */
    _timer = window_ms;
    _timer.Start();

    /**
     *  Store this group id, we might be performing a double press.
     */
    _last_group_id = current_group_id;

    /**
     *  Make sure another team or unit is not selected first.
     */
    if (CurrentObjects.Count() > 0) {
        ObjectClass *object = CurrentObjects.Fetch_Head();
        if (!object->Is_Foot() || reinterpret_cast<FootClass *>(object)->Group != (current_group_id-1)) {
            Unselect_All();
        }
    }

    /**
     *  Iterate over all the technos on the map and select all that match the rule.
     */
    for (int i = 0; i < Technos.Count(); ++i) {
        TechnoClass *techno = Technos[i];
        if (techno) {
            if (!techno->IsInLimbo
             && techno->Group == (current_group_id-1)
             && techno->House->Is_Player_Control()
             && !techno->IsSelected) {

                techno->Select();
                AllowVoice = false;
            }
        }
    }

    TechnoClass::Reset_Action_Line_Timer();
    AllowVoice = false;

	return true;
}


/**
 *  Centers the tactical view on the current team.
 * 
 *  @author: CCHyper
 */
bool Center_On_Team_Command(int group_id)
{
    Map.Power_Mode_Control(0);
    Map.Waypoint_Mode_Control(0, false);
    Map.Repair_Mode_Control(0);
    Map.Sell_Mode_Control(0);

    /**
     *  Make sure another team or unit is not selected first.
     */
    if (CurrentObjects.Count() > 0) {
        ObjectClass *object = CurrentObjects.Fetch_Head();
        if (!object->Is_Foot() || reinterpret_cast<FootClass *>(object)->Group != (group_id-1)) {
            Unselect_All();
        }
    }

    /**
     *  Iterate over all the technos on the map and select all that match the rule.
     */
    for (int i = 0; i < Technos.Count(); ++i) {
        TechnoClass *techno = Technos[i];
        if (techno) {
            if (!techno->IsInLimbo
             && techno->Group == (group_id-1)
             && techno->House->Is_Player_Control()
             && !techno->IsSelected) {

                techno->Select();
                AllowVoice = false;
            }
        }
    }

    /**
     *  Now center the tactical view.
     */
    Map.Center_On_Selection();

    Map.Flag_To_Redraw(true);
    AllowVoice = true;

	return true;
}
