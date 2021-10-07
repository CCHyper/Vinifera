/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TEAMEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended TeamClass.
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
#include "teamext_hooks.h"
#include "vinifera_defines.h"
#include "team.h"
#include "teamext.h"
#include "teamtype.h"
#include "script.h"
#include "scripttype.h"
#include "scripttypeext.h"
#include "cell.h"
#include "iomap.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  This patch extends the ScriptMissionType switch in TeamClass::AI.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TeamClass_AI_Extend_Switch_Patch)
{
    GET_STACK_STATIC(bool, a3, esp, 0x10);
    GET_REGISTER_STATIC(TeamClass *, this_ptr, esi);
    GET_REGISTER_STATIC(ScriptMissionClass *, scriptmission, ecx);

    /**
     *  Stolen bytes/code. (inlined ScriptMissionClass copy constructor).
     */
    _asm { mov eax, [ecx+0x0] }
    _asm { mov [esp+0x20], eax }    // script.Mission
    _asm { mov ecx, [ecx+0x4] }
    _asm { mov [esp+0x24], ecx }    // script.Data

#ifndef NDEBUG
    /**
     *  Helper info for debugging when adding new script missions.
     */
    DEV_DEBUG_INFO("TeamClass::AI()\n");
#endif

    /**
     *  Skip null team script missions.
     */
    if (scriptmission->Mission == SMISSION_NONE) {
        goto do_nothing;
    }

    /**
     *  Handle the original ScriptMissionType's.
     */
    if (scriptmission->Mission < SMISSION_COUNT) {
#ifndef NDEBUG
        DEV_DEBUG_INFO("Executing team script mission: \"%s\"\n", ScriptMissionClass::Mission_Name(scriptmission->Mission));
#endif
        goto smission_switch;
    }

    /**
     *  New ScriptMissionType switch.
     */
    if (scriptmission->Mission < NEW_SMISSION_COUNT) {
        DEV_DEBUG_INFO("Executing new team script mission: \"%s\"\n", ScriptMissionClassExtension::Mission_Name(scriptmission->Mission));
    }

    /**
     *  Execute the new trigger action.
     */
    TeamClassExtension::Switch(this_ptr, scriptmission, a3);

    /**
     *  The default case, return doing nothing.
     */
do_nothing:
    JMP(0x00622B20);

    /**
     *  The switch case for the original ScriptMissionType's
     */
smission_switch:
    _asm { mov ecx, [scriptmission] }
    _asm { mov esi, this_ptr }
    _asm { mov edx, [esi+0x1C] } // scriptmission->Mission
    JMP_REG(ecx, 0x006229D0);
}


/**
 *  #issue-196
 * 
 *  Fixes incorrect cell calculation for the MOVECELL script.
 * 
 *  The original code used outdated code from Red Alert to calculate
 *  the cell position on the map.
 * 
 *  @author: CCHyper (based on research by E1Elite)
 */
DECLARE_PATCH(_TeamClass_AI_MoveCell_FixCellCalc_Patch)
{
    GET_STACK_STATIC(unsigned, argument, esp, 0x24);
    static CellClass *cell;
    static Cell tmpcell;

    /**
     *  Get the cell X and Y position from the script argument.
     */
    tmpcell.X = argument % 1000;
    tmpcell.Y = argument / 1000;

    /**
     *  Fetch the map cell. Added pointer check to make sure the
     *  script didn't have an invalid position.
     */
    cell = &Map[tmpcell];
    if (!cell) {
        goto coordinate_move;
    }

    /**
     *  The Assign_Mission_Target call pushes EAX into the stack
     *  for the cell argument.
     */
    _asm { mov eax, cell }

assign_mission_target:
    JMP_REG(ecx, 0x00622B5F);

coordinate_move:
    JMP(0x00622B19);
}


/**
 *  Main function for patching the hooks.
 */
void TeamClassExtension_Hooks()
{
    Patch_Jump(0x00622B2C, &_TeamClass_AI_MoveCell_FixCellCalc_Patch);
    Patch_Jump(0x006229BA, &_TeamClass_AI_Extend_Switch_Patch);
}
