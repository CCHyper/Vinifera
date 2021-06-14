/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SCENARIOEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended ScenarioClass.
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
#include "scenarioext_hooks.h"
#include "tibsun_defines.h"
#include "tibsun_globals.h"
#include "ccfile.h"
#include "scenario.h"
#include "session.h"
#include "wwmouse.h"
#include "restate.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


#define TXT_START_MISSION "Start Mission"


static bool _mission_start = false;


static void Show_Mission_Briefing()
{
    static char buffer[25];

    /**
     *  If there's no briefing movie, restate the mission at the beginning.
     */
    if (Scen->BriefMovie != VQ_NONE) {
        std::snprintf(buffer, sizeof(buffer), "%s.VQA", Movies[Scen->BriefMovie]);
    }

    if (Session.Type == GAME_NORMAL && Scen->BriefMovie == VQ_NONE || !CCFileClass(buffer).Is_Available()) {

        /**
         *  Make sure the mouse is visible before showing the restatement.
         */
        while (WWMouse->Get_Mouse_State()) {
            WWMouse->Show_Mouse();
        }

        /**
         *  
         */
        _mission_start = true;
        Restate_Mission(Scen);
        _mission_start = false;
    }
}

/**
 *  
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Start_Scenario_BriefMovie_RestateMission_Patch)
{
    /**
     *  
     */
    //if (Scen->IsShowBriefing) {
        Show_Mission_Briefing();
    //}

    /**
     *  Continue to showing the Dropship Loadout menu (if enabled).
     */
    _asm { mov eax, Scen }
    JMP(0x005DB3B1);
}


/**
 *  Main function for patching the hooks.
 */
void ScenarioClassExtension_Hooks()
{
    Patch_Jump(0x005DB37F, &_Start_Scenario_BriefMovie_RestateMission_Patch);
}
