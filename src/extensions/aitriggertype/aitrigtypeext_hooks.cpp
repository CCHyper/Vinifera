/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AITRIGGERTYPEEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended AITriggerTypeClass.
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
#include "aitrigtypeext_hooks.h"
#include "aitrigtypeext_init.h"
#include "aitrigtypeext.h"
#include "aitrigtype.h"
#include "extension.h"
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
DECLARE_PATCH(_AITriggerTypeClass_Process_Extend_Switch_Patch)
{
    GET_REGISTER_STATIC(AITriggerTypeClass *, this_ptr, esi);
    static AITriggerTypeClassExtension *this_ptr_ext;

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
    DEV_DEBUG_INFO("AITriggerTypeClass::operator()\n");
#endif

    /**
     *  Skip null events.
     */
    if (this_ptr->Type == AITRIGGEREVENT_NONE) {
        goto do_nothing;
    }

    /**
     *  Handle the original event types.
     */
    if (this_ptr->Type < AITRIGGEREVENT_COUNT) {
        DEV_DEBUG_INFO("Executing event: \"%s\"\n", TActionClass::Action_Name(this_ptr->Action));
        goto event_switch;
    }

    /**
     *  New event types switch.
     */
    if (this_ptr->Type < NEW_TACTION_COUNT) {
        DEV_DEBUG_INFO("Executing new event: \"%s\"\n", TActionClass_New_Action_Name(this_ptr->Action));
    }

    this_ptr_ext = Extension::Fetch<AITriggerTypeClassExtension>(this_ptr);

    switch (this_ptr->Type) {

        /**
         *  #issue-158
         * 
         *  x
         * 
         *  @author: CCHyper
         */
        case AITRIGGEREVENT_CHECK_CIVILIAN:
            success = this_ptr_ext->Check_Civilian_Owns(nullptr, ?);
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
    JMP(0x0061A9C5);

    /**
     *  The switch case for the original event type.
     */
event_switch:
    _asm { mov esi, this_ptr }
    _asm { mov edx, [esi+0x60] } // this->Type
    JMP_REG(edi, 0x00410A4A);
}


/**
 *  Main function for patching the hooks.
 */
void AITriggerTypeClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    AITriggerTypeClassExtension_Init();

    Patch_Jump(0x00410A35, &_AITriggerTypeClass_Process_Extend_Switch_Patch);
}
