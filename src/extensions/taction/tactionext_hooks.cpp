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

        case TACTION_RESHROUD_MAP_AT:
            return "";

        case TACTION_SET_TECH_LEVEL:
            return "";

        case TACTION_CREATE_CRATE:
            return "";

        case TACTION_PAUSE_GAME_FOR:
            return "";

        case TACTION_JUMP_WAYPOINT:
            return "";

        case TACTION_PLAY_MOVIE_PAUSE:
            return "";

        case TACTION_CLEAR_SMUDGES:
            return "";

        case TACTION_DESTROY_ALL_OF:
            return "";

        case TACTION_DESTROY_BUILDINGS:
            return "";

        case TACTION_DESTROY_UNITS:
            return "";

        case TACTION_CREATE_BUILDING:
            return "";

        case TACTION_SET_SPECIAL_CHARGE:
            return "";

        case TACTION_FLASH_BUILDINGS:
            return "";

        case TACTION_SET_SPECIAL_RECHARGE:
            return "";

        case TACTION_RESET_SPECIAL_RECHARGE:
            return "";

        case TACTION_SPECIAL_RESET:
            return "";

        case TACTION_PREFERRED_TARGET_SET:
            return "";

        case TACTION_PREFERRED_TARGET_CLEAR:
            return "";

        case TACTION_CENTER_BASE_SET:
            return "";

        case TACTION_CENTER_BASE_CLEAR:
            return "";

        case TACTION_BLACKOUT_RADAR:
            return "";

        case TACTION_RETINT_RED:
            return "";

        case TACTION_RETINT_GREEN:
            return "";

        case TACTION_RETINT_BLUE:
            return "";

        case TACTION_JUMP_CAMERA_HOME:
            return "";

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

        case TACTION_RESHROUD_MAP_AT:
            return "";

        case TACTION_SET_TECH_LEVEL:
            return "";

        case TACTION_CREATE_CRATE:
            return "";

        case TACTION_PAUSE_GAME_FOR:
            return "";

        case TACTION_JUMP_WAYPOINT:
            return "";

        case TACTION_PLAY_MOVIE_PAUSE:
            return "";

        case TACTION_CLEAR_SMUDGES:
            return "";

        case TACTION_DESTROY_ALL_OF:
            return "";

        case TACTION_DESTROY_BUILDINGS:
            return "";

        case TACTION_DESTROY_UNITS:
            return "";

        case TACTION_CREATE_BUILDING:
            return "";

        case TACTION_SET_SPECIAL_CHARGE:
            return "";

        case TACTION_FLASH_BUILDINGS:
            return "";

        case TACTION_SET_SPECIAL_RECHARGE:
            return "";

        case TACTION_RESET_SPECIAL_RECHARGE:
            return "";

        case TACTION_SPECIAL_RESET:
            return "";

        case TACTION_PREFERRED_TARGET_SET:
            return "";

        case TACTION_PREFERRED_TARGET_CLEAR:
            return "";

        case TACTION_CENTER_BASE_SET:
            return "";

        case TACTION_CENTER_BASE_CLEAR:
            return "";

        case TACTION_BLACKOUT_RADAR:
            return "";

        case TACTION_RETINT_RED:
            return "";

        case TACTION_RETINT_GREEN:
            return "";

        case TACTION_RETINT_BLUE:
            return "";

        case TACTION_JUMP_CAMERA_HOME:
            return "";

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
         *  #issue-328
         * 
         *  The follow are all from Red Alert 2 and Yuri's Revenge.
         * 
         *  @author: CCHyper
         */
        case TACTION_RESHROUD_MAP_AT:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_SET_TECH_LEVEL:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_CREATE_CRATE:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_PAUSE_GAME_FOR:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_JUMP_WAYPOINT:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_PLAY_MOVIE_PAUSE:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_CLEAR_SMUDGES:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_DESTROY_ALL_OF:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_DESTROY_BUILDINGS:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_DESTROY_UNITS:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_CREATE_BUILDING:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_SET_SPECIAL_CHARGE:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_FLASH_BUILDINGS:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_SET_SPECIAL_RECHARGE:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_RESET_SPECIAL_RECHARGE:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_SPECIAL_RESET:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_PREFERRED_TARGET_SET:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_PREFERRED_TARGET_CLEAR:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_CENTER_BASE_SET:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_CENTER_BASE_CLEAR:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_BLACKOUT_RADAR:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_RETINT_RED:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_RETINT_GREEN:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_RETINT_BLUE:
            //success = TAction_(this_ptr, house, object, trigger, cell);
            break;

        case TACTION_JUMP_CAMERA_HOME:
            //success = TAction_(this_ptr, house, object, trigger, cell);
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
 *  R
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TAction_Read_INI_New_ParamTypes_Patch_1)
{
    GET_REGISTER_STATIC(int, value, eax);
    GET_REGISTER_STATIC(unsigned, param_type, ebp);
    GET_REGISTER_STATIC(TActionClass *, this_ptr, esi);

    /**
     *  Handle the original param types.
     */
    if (param_type <= 4) {
        goto paramtype_switch;
    }

    /**
     *  New param type switch.
     */
    switch (param_type) {

        /**
         *  Param type 5 means the data param is an integer, which can
         *  be used any way we like. This new type is just to handle the
         *  new action types we add.
         */
        case 5:
            this_ptr->Data.Value = value;

        /**
         *  Unexpected param type.
         */
        default:
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
    JMP(0x00618FC3);
}


/**
 *  R
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TAction_Read_INI_New_ParamTypes_Patch_2)
{
    GET_REGISTER_STATIC(int, value, eax);
    GET_REGISTER_STATIC(TActionClass *, this_ptr, esi);

    /**
     *  New param type switch.
     */
    switch (value) {

        /**
         *  Param types 0-3 means the last entry of a action is a waypoint.
         */
        //case 0:
        //case 1:
        //case 2:
        //case 3:
            _asm { mov eax, value }
            JMP(0x006190EC);

        /**
         *  Param type 4 means the last entry of a action is a integer.
         */
        //case 4:
            _asm { mov eax, value }
            JMP(0x006190DB);

        /**
         *  Param type 5 means the last entry of a action is a integer. We reuse
         *  the waypoint member of TActionClass and are limited to USHORT_MAX (65535).
         */
        //case 5:
        default:
            //unsigned short *member = (unsigned short *)&this_ptr->Location;
            *(unsigned short *)&this_ptr->Location = value;
            break;

        //default:
        //    goto function_return;
    }

    /**
     *  Return from the function.
     */
function_return:
    JMP(0x006190F6);
}


/**
 *  Main function for patching the hooks.
 */
void TActionClassExtension_Hooks()
{
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

    Patch_Jump(0x00619134, &_TAction_Operator_Extend_Switch_Patch);



    Patch_Jump(0x00618FB8, &_TAction_Read_INI_New_ParamTypes_Patch_1);
    Patch_Jump(0x006190D6, &_TAction_Read_INI_New_ParamTypes_Patch_2);


}
