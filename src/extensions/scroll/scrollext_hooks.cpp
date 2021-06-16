/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SCROLLEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended ScrollClass.
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
#include "displayext_hooks.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "tibsun_util.h"
#include "display.h"
#include "house.h"
#include "iomap.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


static bool Is_KeySelect_Down()
{
    return WWKeyboard->Down(Options.KeySelect1) || WWKeyboard->Down(Options.KeySelect2);
}

/**
 *  #issue-165
 * 
 *  This patch allows the user to set a waypoint path loop without the "shift"
 *  key held down (KeySelect[1-2]).
 *  
 *  @author: CCHyper (based on research by dkeeton)
 *  
 */
DECLARE_PATCH(_ScrollClass_What_Action_Remove_Waypoint_Loop_Key_Modifier_Check_Patch)
{
    /**
     *  No access to "this" pointer in this function, so use "Map" global.
     */

    static bool shiftdown;

#if 0
    /**
     *  Original code.
     */
    shiftdown = Is_KeySelect_Down();
#else
    /**
     *  Force the shift key check to true, allowing the player to finish
     *  a waypoint path with a loop back to the first waypoint (0) with
     *  the shift key held down.
     */
    shiftdown = true;
#endif

    _asm { mov bl, shiftdown }

    JMP(0x005E8B83);
}


/**
 *  Main function for patching the hooks.
 */
void ScrollClassExtension_Hooks()
{
    Patch_Jump(0x005E8B53, &_ScrollClass_What_Action_Remove_Waypoint_Loop_Key_Modifier_Check_Patch);
}
