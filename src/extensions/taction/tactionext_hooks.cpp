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
#include "tactionext_functions.h"
#include "taction.h"
#include "vinifera_defines.h"
#include "house.h"
#include "housetype.h"
#include "object.h"
#include "objecttype.h"
#include "trigger.h"
#include "triggertype.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Returns the name of the TActionType.
 * 
 *  @author: CCHyper
 */
static const char *TActionClass_New_Action_Name(int action)
{
    if (action < TACTION_COUNT) {
        return TActionClass::Action_Name(TActionType(action));
    }

    switch (action) {

        case TACTION_CREDITS:
            return "Give credits...";

        case TACTION_CREDITS_HOUSE:
            return "Give credits to house...";

        default:
            return "<invalid>";
    }
}


/**
 *  Returns the description of the TActionType.
 * 
 *  @author: CCHyper
 */
static const char *TActionClass_New_Action_Description(int action)
{
    if (action < TACTION_COUNT) {
        return TActionClass::Action_Description(TActionType(action));
    }

    switch (action) {

        case TACTION_CREDITS:
            return "Gives credits to the owner of the trigger.";

        case TACTION_CREDITS_HOUSE:
            return "Gives credits to the specified house.";

        default:
            return "<invalid>";
    }
}


/**
 *  This patch extends the TActionClass operator.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TAction_Operator_Extend_Switch_Patch)
{
    GET_REGISTER_STATIC(bool, success, al);
    GET_REGISTER_STATIC(TActionClass *, this_ptr, esi);

    /**
     *  Function arguments
     */
    GET_STACK_STATIC(HouseClass *, house, esp, 0x1C4);
    GET_STACK_STATIC(ObjectClass *, object, esp, 0x1C8);
    GET_STACK_STATIC(TriggerClass *, trigger, esp, 0x1CC);
    GET_STACK_STATIC(Cell *, cell, esp, 0x1D0);

#if 0
    /**
     *  Helper info for debugging when adding new actions.
     */
    DEV_DEBUG_INFO("TActionClass::operator()\n");
    if (house) DEV_DEBUG_INFO("  House: \"%s\"\n", house->Class->Name());
    if (object) DEV_DEBUG_INFO("  Object: \"%s\"\n", object->Name());
    if (trigger) DEV_DEBUG_INFO("  Trigger: \"%s\"\n", trigger->Class_Of()->Name());
    if (cell && *cell) DEV_DEBUG_INFO("  Cell: %d,%d\n", cell->X, cell->Y);
#endif

    /**
     *  Skip null actions.
     */
    if (this_ptr->Action == TACTION_NONE) {
        goto do_nothing;
    }

    /**
     *  Handle the original TActionTypes.
     */
    if (this_ptr->Action < TACTION_COUNT) {
        DEV_DEBUG_INFO("Executing action: \"%s\"\n", TActionClass::Action_Name(this_ptr->Action));
        goto taction_switch;
    }

    /**
     *  New TActionType switch.
     */
    if (this_ptr->Action < NEW_TACTION_COUNT) {
        DEV_DEBUG_INFO("Executing new action: \"%s\"\n", TActionClass_New_Action_Name(this_ptr->Action));
    }

    switch (this_ptr->Action) {

        /**
         *  #issue-158
         * 
         *  Gives credits to the owner of the trigger.
         * 
         *  @author: CCHyper
         */
        case TACTION_CREDITS:
            success = TAction_Give_Credits(this_ptr, house, object, trigger, cell);
            break;

        /**
         *  #issue-158
         * 
         *  Gives credits to the specified house.
         * 
         *  @author: CCHyper
         */
        case TACTION_CREDITS_HOUSE:
            success = TAction_Give_Credits_To_House(this_ptr, house, object, trigger, cell);
            break;

        /**
         *  Unexpected TActionType.
         */
        default:
            goto do_nothing;
    }

    /**
     *  The default case, return doing nothing.
     */
do_nothing:
    _asm { mov al, success }
    JMP(0x0061A9C5);

    /**
     *  The switch case for the original TActionTypes
     */
taction_switch:
    _asm { mov esi, this_ptr }
    _asm { mov edx, [esi+0x1C] } // this->Action
    _asm { dec edx } 
    JMP_REG(eax, 0x00619141);
}


/**
 *  Main function for patching the hooks.
 */
void TActionClassExtension_Hooks()
{
    Patch_Jump(0x00619134, &_TAction_Operator_Extend_Switch_Patch);
}
