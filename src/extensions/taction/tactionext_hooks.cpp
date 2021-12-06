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
#include "tactionext.h"
#include "taction.h"
#include "tibsun_defines.h"
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
 *  This patch extends the TActionClass operator.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TActionClass_Operator_Extend_Switch_Patch)
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

#ifndef NDEBUG
    /**
     *  Helper info for debugging when adding new actions.
     */
    DEV_DEBUG_INFO("TActionClass::operator(%d)\n", this_ptr->ID);
    if (house) {
        DEV_DEBUG_INFO("  House: \"%s\"\n", house->Class->Name());
        if (house->Is_Player()) {
            DEV_DEBUG_INFO("    IsPlayer: true\n");
        } else {
            DEV_DEBUG_INFO("    IsPlayer: false\n");
        }
    }
    if (object) {
        DEV_DEBUG_INFO("  Object: \"%s\"\n", object->Name());
        DEV_DEBUG_INFO("    Coord: %s\n", object->Get_Coord().As_String());
    }
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
#ifndef NDEBUG
        DEV_DEBUG_INFO("  Executing action: \"%s\"\n", TActionClass::Action_Name(this_ptr->Action));
#endif
        goto taction_switch;
    }

    /**
     *  New TActionType switch.
     */
    if (this_ptr->Action < EXT_TACTION_COUNT) {
#ifndef NDEBUG
        DEV_DEBUG_INFO("  Executing new action: \"%s\"\n", TActionClassExtension::Action_Name(this_ptr->Action));
#endif

        /**
         *  Execute the new trigger action.
         */
        TActionClassExtension::Execute(this_ptr, house, object, trigger, cell);
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
 *  R
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TActionClass_Read_INI_New_ParamTypes_Patch_1)
{
    GET_REGISTER_STATIC(int, value, eax);
    GET_REGISTER_STATIC(unsigned, param_type, ebp);
    GET_REGISTER_STATIC(TActionClass *, this_ptr, esi);

    /**
     *  Handle the original param types.
     */
    if (param_type < 5) {
        goto paramtype_switch;


    /**
     *  New param types.
     * 
     */

    /**
     *  Param type 5 means the data param is an integer, which can
     *  be used any way we like. This new type is just to handle any
     *  new action types we add.
     */
    } else if (param_type == 5) {
        this_ptr->Data.Value = value;
        goto continue_parsing;
    }

    /**
     *  The default case.
     */
continue_parsing:
    JMP(0x0061906F);

    /**
     *  The switch case for the original param types
     */
paramtype_switch:
    _asm { mov edx, value }
    JMP(0x00618FC3);
}


/**
 *  R
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TActionClass_Read_INI_New_ParamTypes_Patch_2)
{
    GET_REGISTER_STATIC(int, param_type, ebp);
    GET_REGISTER_STATIC(const char *, tok, eax);
    GET_REGISTER_STATIC(TActionClass *, this_ptr, esi);

    /**
     *  Param types 0-3 means the last entry of a action is a waypoint.
     */
    if (param_type == 0
     || param_type == 1
     || param_type == 2
     || param_type == 3) {

        goto param_is_waypoint;

    /**
     *  Param type 4 means the last entry of a action is a integer.
     */
    } else if (param_type == 4) {
        goto param_is_integer;


    /**
     *  New param types.
     */

    /**
     *  Param type 5 means the last entry of a action is a integer. We reuse
     *  the waypoint member of TActionClass and are limited to USHORT_MAX (65535).
     */
    } else if (param_type == 5) {
        *(unsigned short *)&this_ptr->Location = std::atoi(tok);
    }

    /**
     *  Return from the function.
     */
function_return:
    JMP(0x006190F6);

param_is_integer:
    _asm { mov eax, tok }
    JMP_REG(ecx, 0x006190DB);

param_is_waypoint:
    _asm { mov eax, tok }
    JMP_REG(ecx, 0x006190EC);
}


/**
 *  Main function for patching the hooks.
 */
void TActionClassExtension_Hooks()
{
    Patch_Jump(0x00619134, &_TActionClass_Operator_Extend_Switch_Patch);

    //Patch_Jump(0x00618FB8, &_TActionClass_Read_INI_New_ParamTypes_Patch_1);
    //Patch_Jump(0x006190D6, &_TActionClass_Read_INI_New_ParamTypes_Patch_2);

    /**
     *  #issue-674
     * 
     *  Fixes a bug where the game would crash when TACTION_WAKEUP_GROUP was
     *  executed but the game was not able to match the Group to the triggers
     *  group. This was because the game was searching the Foots vector with
     *  the count of the Technos vector, and in cases where the Group did
     *  not match, the game would crash trying to search out of bounds.
     * 
     *  @author: CCHyper
     */
    Patch_Dword(0x00619552+2, (0x007E4820+4)); // Foot vector to Technos vector.
}
