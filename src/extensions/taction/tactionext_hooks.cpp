/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TACTIONEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended TriggerClass.
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
#include "tactionext_hooks.h"
#include "tibsun_globals.h"
#include "trigger.h"
#include "triggertype.h"
#include "scenario.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-299
 * 
 *  Fixes the issue with the current difficulty not being checked
 *  when enabling triggers.
 * 
 *  @see: TriggerClass and TriggerTypeClass for the other parts of this fix.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TActionClass_Operator_Enable_Trigger_For_Difficulty_Patch)
{
    GET_REGISTER_STATIC(int, trigger_index, edi);
    static TriggerClass *trigger;

    /**
     *  This is direct port of the code from Red Alert 2, which looks to fix this issue.
     */

    /**
     *  We need to re-fetch the trigger from the vector as the
     *  register is reused by this point.
     */
    trigger = Triggers[trigger_index];
    if (trigger) {

        /**
         *  Set this trigger to be disabled if it is marked as disabled
         *  for this current mission difficulty.
         */
        if (Scen->Difficulty == DIFF_EASY && !trigger->Class->Easy
         || Scen->Difficulty == DIFF_NORMAL && !trigger->Class->Normal
         || Scen->Difficulty == DIFF_HARD && !trigger->Class->Hard) {

            trigger->Disable();

        } else {

            trigger->Enable();
        }
    }

    JMP(0x0061A611);
}


/**
 *  Main function for patching the hooks.
 */
void TActionClassExtension_Hooks()
{
    Patch_Jump(0x0061A60C, &_TActionClass_Operator_Enable_Trigger_For_Difficulty_Patch);
}
