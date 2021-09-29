/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AIRCRAFTEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended AircraftClass.
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
#include "aircraftext_hooks.h"
#include "aircraftext_init.h"
#include "aircraft.h"
#include "aircraftext.h"
#include "aircrafttype.h"
#include "aircrafttypeext.h"
#include "object.h"
#include "target.h"
#include "unit.h"
#include "unittype.h"
#include "unittypeext.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-589
 * 
 *  
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AircraftClass_AI_Acquire_Airborne_Targets_Patch)
{
    GET_REGISTER_STATIC(AircraftClass *, this_ptr, ebp);
    static AircraftTypeClassExtension *aircrafttypeext;
    static AircraftClass *tarcom_aircraft;
    static AircraftTypeClassExtension *tarcom_aircrafttypeext;
    static bool can_target;

    /**
     *  Original case.
     *  If the target is airborne, flag to clear the target.
     */
    if (this_ptr->TarCom->In_Air()) {
        can_target = false;
    }

    aircrafttypeext = AircraftTypeClassExtensions.find(this_ptr->Class);
    if (aircrafttypeext) {

        /**
         *  
         */
        if (aircrafttypeext->IsCanAcquireAirborne && this_ptr->TarCom->In_Air()) {
            can_target = true;
        }

        /**
         *  If the target is 
         */
        tarcom_aircraft = Target_As_Aircraft(this_ptr->TarCom);
        if (tarcom_aircraft && this_ptr->TarCom->In_Air()) {

            /**
             *  Check to make sure the target aircraft is allowed to be attacked while airborne.
             */
            tarcom_aircrafttypeext = AircraftTypeClassExtensions.find(tarcom_aircraft->Class);
            if (tarcom_aircrafttypeext && !tarcom_aircrafttypeext->IsCanBeAirborneAcquired) {
                can_target = false;
            }

        }

    }

    /**
     *  
     */
    if (can_target) {
        goto assign_target_checks;
    }

    /**
     *  We can not attack this target, clear the tarcom.
     */
assign_null_target:
    JMP(0x00409119);

    /**
     *  Continue target validation checks.
     */
assign_target_checks:
    JMP(0x00409127);
}


/**
 *  #issue-208
 * 
 *  Check if the target unit is "totable" before we attempt to pick it up.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AircraftClass_What_Action_Is_Totable_Patch)
{
    GET_REGISTER_STATIC(AircraftClass *, this_ptr, esi);
    GET_REGISTER_STATIC(ObjectClass *, target, edi);
    GET_REGISTER_STATIC(ActionType, action, ebx);
    static UnitClass *target_unit;
    static UnitTypeClassExtension *unittypeext;

    /**
     *  Code before this patch checks for if this aircraft
     *  is a carryall and if it is owned by a player (non-AI).
     */

    /**
     *  Make sure the mouse is over something.
     */
    if (action != ACTION_NONE) {

        /**
         *  Target is a unit?
         */
        if (target->What_Am_I() == RTTI_UNIT) {

            target_unit = reinterpret_cast<UnitClass *>(target);

            /**
             *  Fetch the unit type extension instance if available.
             */
            unittypeext = UnitTypeClassExtensions.find(target_unit->Class);
            if (unittypeext) {

                /**
                 *  Can this unit be toted/picked up by us?
                 */
                if (!unittypeext->IsTotable) {

                    /**
                     *  If not, then show the "no move" mouse.
                     */
                    action = ACTION_NOMOVE;

                    goto failed_tote_check;

                }
            }
        }
    }

    /**
     *  Stolen code.
     */
    if (action != ACTION_NONE && action != ACTION_SELECT) {
        goto action_self_check;
    }

    /**
     *  Passes our tote check, continue original carryall checks.
     */
passes_tote_check:
    _asm { mov ebx, action }
    _asm { mov edi, target }
    JMP_REG(ecx, 0x0040B826);

    /**
     *  Undeploy/unload check.
     */
action_self_check:
    _asm { mov ebx, action }
    _asm { mov edi, target }
    JMP(0x0040B8C2);

    /**
     *  We cant pick this unit up, so continue to evaluate the target.
     */
failed_tote_check:
    _asm { mov ebx, action }
    _asm { mov edi, target }
    JMP(0x0040B871);
}


/**
 *  #issue-469
 * 
 *  Fixes a bug where IsCloakable has no effect on Aircrafts. This was
 *  because the TechnoType value was not copied to the Aircraft instance
 *  when it is created.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AircraftClass_Init_IsCloakable_BugFix_Patch)
{
    GET_REGISTER_STATIC(AircraftClass *, this_ptr, esi);
    GET_REGISTER_STATIC(AircraftTypeClass *, aircrafttype, eax);

    /**
     *  Stolen bytes/code.
     */
    this_ptr->Strength = aircrafttype->MaxStrength;
    this_ptr->Ammo = aircrafttype->MaxAmmo;

    /**
     *  This is the line that was missing (maybe it was by design?).
     */
    this_ptr->IsCloakable = aircrafttype->IsCloakable;

    JMP_REG(ecx, 0x004088AA);
}


/**
 *  Main function for patching the hooks.
 */
void AircraftClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    AircraftClassExtension_Init();

    Patch_Jump(0x00408898, &_AircraftClass_Init_IsCloakable_BugFix_Patch);
    Patch_Jump(0x0040B819, &_AircraftClass_What_Action_Is_Totable_Patch);
    Patch_Jump(0x00409110, &_AircraftClass_AI_Acquire_Airborne_Targets_Patch);
}
