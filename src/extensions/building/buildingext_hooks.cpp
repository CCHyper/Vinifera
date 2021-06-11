/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BUILDINGEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended BuildingClass.
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
#include "buildingext_hooks.h"
#include "buildingext_init.h"
#include "buildingext.h"
#include "building.h"
#include "buildingtypeext.h"
#include "buildingtype.h"
#include "house.h"
#include "housetype.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"
#include "vinifera_util.h"


/**
 *  #issue-26
 * 
 *  Adds functionality for the produce cash per-frame logic.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_BuildingClass_AI_ProduceCash_Patch)
{
    GET_REGISTER_STATIC(BuildingClass *, this_ptr, esi);
    static BuildingClassExtension *ext_ptr;
    static BuildingTypeClassExtension *exttype_ptr;

    /**
     *  Find the extension instances.
     */
    ext_ptr = BuildingClassExtensions.find(this_ptr);
    exttype_ptr = BuildingTypeClassExtensions.find(this_ptr->Class);
    if (!ext_ptr || !exttype_ptr) {
        goto original_code;
    }

#if 0
    /**
     *  Debugging code.
     * 
     *  Only updates player owned buildings.
     */
    if (this_ptr->House != PlayerPtr) {
        goto original_code;
    }
#endif

    /**
     *  Is this building able to produce cash?
     */
    if (!ext_ptr->IsBudgetDepleted && exttype_ptr->ProduceCashAmount != 0) {

        /**
         *  Check if this building requires power to produce cash.
         */
        if (reinterpret_cast<BuildingTypeClass const *>(this_ptr->Class_Of())->IsPowered) {

            /**
             *  Stop the timer if the building is offline or has low power.
             */
            if (ext_ptr->ProduceCashTimer.Is_Active() && !this_ptr->Is_Powered_On()) {
                ext_ptr->ProduceCashTimer.Stop();
            }

            /**
             *  Restart the timer is if it previously stopped due to low power or is offline.
             */
            if (!ext_ptr->ProduceCashTimer.Is_Active() && this_ptr->Is_Powered_On()) {
                ext_ptr->ProduceCashTimer.Start();
            }

        }

        /**
         *  Are we ready to hand out some cash?
         */
        if (ext_ptr->ProduceCashTimer.Is_Active() && ext_ptr->ProduceCashTimer.Expired()) {

            /**
             *  Is the owner a passive house? If so, they should not be receiving cash.
             */
            if (!this_ptr->House->Class->IsMultiplayPassive) {

                int amount = exttype_ptr->ProduceCashAmount;

                /**
                 *  Check we have not depleted our budget first.
                 */
                if (ext_ptr->CurrentProduceCashBudget > 0) {

                    /**
                     *  Adjust the budget tracker (if one as been set).
                     */
                    if (ext_ptr->CurrentProduceCashBudget != -1) {
                        ext_ptr->CurrentProduceCashBudget = std::max<int>(0, ext_ptr->CurrentProduceCashBudget - std::abs(amount));
                    }

                    /**
                     *  Has the budget been spent?
                     */
                    ext_ptr->IsBudgetDepleted = (ext_ptr->CurrentProduceCashBudget <= 0);
                }

                /**
                 *  Check if we need to drain cash from the house or provide a bonus.
                 */
                if (!ext_ptr->IsBudgetDepleted && amount != 0) {
                    if (amount < 0) {
                        this_ptr->House->Spend_Money(amount);
                    } else {
                        this_ptr->House->Refund_Money(amount);
                    }
                }

            }

            /**
             *  Reset the delay timer.
             */
            ext_ptr->ProduceCashTimer = exttype_ptr->ProduceCashDelay;
        }
    }

    /**
     *  Stolen bytes/code here.
     */
original_code:

    /**
     *  Animation per frame update.
     */
    this_ptr->Animation_AI();

    JMP(0x00429A9D);
}


/**
 *  #issue-26
 * 
 *  Grants cash bonus and starts the cash timer on building capture.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_BuildingClass_Captured_ProduceCash_Patch)
{
    GET_REGISTER_STATIC(BuildingClass *, this_ptr, esi);
    GET_STACK_STATIC(HouseClass *, newowner, esp, 0x58);
    static BuildingClassExtension *ext_ptr;
    static BuildingTypeClassExtension *exttype_ptr;

    /**
     *  Find the extension instances.
     */
    ext_ptr = BuildingClassExtensions.find(this_ptr);
    exttype_ptr = BuildingTypeClassExtensions.find(this_ptr->Class);
    if (!ext_ptr || !exttype_ptr) {
        goto original_code;
    }

    /**
     *  Is the owner a passive/neutral house? Only they can provide the capture bonus.
     */
    if (this_ptr->House->Class->IsMultiplayPassive) {

        /**
         *  Should this building produce a cash bonus on capture?
         *  If so, grant it and start the cycle timer.
         */
        if (exttype_ptr->ProduceCashStartup > 0) {
            newowner->Refund_Money(exttype_ptr->ProduceCashStartup);

            ext_ptr->ProduceCashTimer = exttype_ptr->ProduceCashDelay;
            ext_ptr->ProduceCashTimer.Start();
        }
    }

    /**
     *  Stolen bytes/code here.
     */
original_code:
    if (this_ptr->Class->IsCloakGenerator) {
        newowner->field_4F0 = true;
    }

    JMP(0x0042F68E);
}


/**
 *  #issue-26
 * 
 *  Starts the cash timer on building placement complete (the "grand opening").
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_BuildingClass_Grand_Opening_ProduceCash_Patch)
{
    GET_STACK_STATIC8(bool, captured, esp, 0x40);
    GET_REGISTER_STATIC(BuildingClass *, this_ptr, esi);
    static BuildingClassExtension *ext_ptr;
    static BuildingTypeClassExtension *exttype_ptr;

    /**
     *  Stolen bytes/code here.
     */
    if (this_ptr->HasOpened) {
        if (!captured) {
            goto function_return;
        }
        goto has_opened_else;
    }

    /**
     *  Find the extension instances.
     */
    ext_ptr = BuildingClassExtensions.find(this_ptr);
    exttype_ptr = BuildingTypeClassExtensions.find(this_ptr->Class);
    if (!ext_ptr || !exttype_ptr) {
        goto continue_function;
    }

    /**
     *  Start the cash delay timer.
     */
    if (exttype_ptr->ProduceCashAmount > 0) {

        ext_ptr->ProduceCashTimer = exttype_ptr->ProduceCashDelay;
        ext_ptr->ProduceCashTimer.Start();

        if (exttype_ptr->ProduceCashBudget > 0) {
            ext_ptr->CurrentProduceCashBudget = exttype_ptr->ProduceCashBudget;
        }
    }

    /**
     *  Continue function flow (HasOpened == false).
     */
continue_function:
    JMP(0x0042E197);

    /**
     *  Function return.
     */
function_return:
    JMP(0x0042E9DF);

    /**
     *  Else case from "HasOpened" check.
     */
has_opened_else:
    JMP(0x0042E4C7);
}


/**
 *  Main function for patching the hooks.
 */
void BuildingClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    BuildingClassExtension_Init();

    /**
     *  #issue-26
     * 
     *  Produce Cash
     */
    Patch_Jump(0x00429A96, _BuildingClass_AI_ProduceCash_Patch);
    Patch_Jump(0x0042F67D, _BuildingClass_Captured_ProduceCash_Patch);
    Patch_Jump(0x0042E179, _BuildingClass_Grand_Opening_ProduceCash_Patch);
}
