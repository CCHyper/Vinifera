/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          INFANTRYEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended InfantryClass.
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
#include "houseext_hooks.h"
#include "infantry.h"
#include "infantrytype.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-82
 * 
 *  This removes the explosion when a Jumpjet infantry is killed while in the air.
 * 
 *  @author: CCHyper
 * 
 *  @update: Remove this patch as this effect is desired (Jumpjet jet pack exploding).
 */
#if 0
DECLARE_PATCH(_InfantryClass_Take_Damage_JumpJet_Remove_Explosion_Patch)
{
    JMP(0x004D282A);
}
#endif


/**
 *  #issue-82
 * 
 *  
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Take_Damage_JumpJet_Do_Tumble_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, esi);
    static const InfantryTypeClass *infantrytype;

    infantrytype = reinterpret_cast<const InfantryTypeClass *>(this_ptr->Class_Of());


    // jumpjet loco process handle crashing

    // YR 005185F9

    if (infantrytype->IsJumpJet) {
        if (!Infantry_Crash()) { // YR 004DEBB0
        }

        this_ptr->entry_E4();
        goto return_IS_DEAD;

    } else {
        this_ptr->entry_E4();
        goto return_IS_DEAD;
    }

    /**
     *  Stolen code.
     */
    //if (!infantrytype->IsJumpJet) {
    //    goto normal_infantry;
    //}

    //this_ptr->Do_Action(DO_TUMBLE, true);

return_IS_DEAD:
    JMP(0x004D2834);

//detach:
//    JMP(0x004D282A);
//
//do_explode_anim:
//    JMP(0x004D279D);
//    
//normal_infantry:
//    JMP(0x004D27C6);
}


/**
 *  #issue-80
 * 
 *  Fixes the bug where the Jumpjet uses the wrong DoType when idle on the
 *  ground. This was because the original code did not check if the infantry
 *  was actually in the air or not and always assumed it was on the ground, as
 *  a result it was always setting DO_STAND_READY.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Doing_AI_JumpJet_Idle_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, esi);
    static const InfantryTypeClass *infantrytype;

    infantrytype = reinterpret_cast<const InfantryTypeClass *>(this_ptr->Class_Of());

    /**
     *  Stolen code.
     * 
     *  If infantry is prone, set DO_PRONE.
     */
    if (this_ptr->IsProne) {
        JMP(0x004D8B12);
    }

    if (infantrytype->IsJumpJet) {

        /**
         *  This is a Jumpjet infantry, make sure its in the air before
         *  assigning the hover graphic sequence.
         */
        if (this_ptr->In_Air()) {
            this_ptr->Do_Action(DO_HOVER, true);

        } else {
            this_ptr->Do_Action(DO_STAND_READY, true);
        }

    /**
     *  Handle normal infantry.
     */
    } else {
        this_ptr->Do_Action(DO_STAND_READY, true);
    }

    JMP(0x004D8CA1);
}


/**
 *  #issue-80
 * 
 *  Fixes the bug where the Jumpjet uses the wrong DoType when on the
 *  ground and in between firing rounds. This was because the original code
 *  did not check if the infantry was actually in the air and assumed it always
 *  is, thus setting DO_STAND_READY.
 * 
 *  @warning: This patch is within a branch that has already checked if the
 *            infantry is firing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_AI_JumpJet_Idle_Between_Firing_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, esi);
    static const InfantryTypeClass *infantrytype;

    infantrytype = reinterpret_cast<const InfantryTypeClass *>(this_ptr->Class_Of());

    if (infantrytype->IsJumpJet) {

        /**
         *  This is a Jumpjet infantry, make sure its in the air before
         *  assigning the hover graphic sequence.
         */
        if (this_ptr->In_Air()) {
            this_ptr->Do_Action(DO_HOVER);

        } else {
            this_ptr->Do_Action(DO_STAND_READY);
        }

    /**
     *  Handle normal infantry.
     */
    } else {
        this_ptr->Do_Action(DO_STAND_READY);
    }

    /**
     *  Stolen code.
     * 
     *  Clear the firing flag.
     */
    this_ptr->IsFiring = false;

    JMP(0x004D50E0);
}


/**
 *  #issue-80
 * 
 *  Fixes the bug where the Jumpjet uses the wrong DoType when not moving
 *  but actually in the air.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Movement_AI_JumpJet_Not_Moving_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, ebp);
    static const InfantryTypeClass *infantrytype;

    infantrytype = reinterpret_cast<const InfantryTypeClass *>(this_ptr->Class_Of());

    if (infantrytype->IsJumpJet) {

        /**
         *  This is a Jumpjet infantry, make sure its in the air before
         *  assigning the hover graphic sequence.
         */
        if (this_ptr->In_Air()) {
            this_ptr->Do_Action(DO_HOVER);

        } else {
            this_ptr->Do_Action(DO_STAND_READY);
        }

    /**
     *  Handle normal infantry.
     */
    } else {
        this_ptr->Do_Action(DO_STAND_READY);
    }

    JMP(0x004D9087);
}


/**
 *  #issue-81
 * 
 *  Fixes the bug where the Jumpjet uses the wrong DoType when firing on
 *  the ground. This was because the original code did not check if the infantry
 *  was actually in the air and assumed it always is, thus setting DO_FIREFLY.
 * 
 *  @warning: This patch is within a branch that has already checked if the
 *            infantry is controlled by the Jumpjet locomotor!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Firing_AI_JumpJet_In_Air_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, ebp);

    /**
     *  Make sure its in the air before assigning the hover firing graphic sequence.
     */
    if (this_ptr->In_Air()) {
        this_ptr->Do_Action(DO_FIREFLY);
    } else {
        this_ptr->Do_Action(DO_FIRE_WEAPON);
    }

    JMP(0x004D8933);
}


/**
 *  Main function for patching the hooks.
 */
void InfantryClassExtension_Hooks()
{
    Patch_Jump(0x004D88FA, &_InfantryClass_Firing_AI_JumpJet_In_Air_Patch);
    Patch_Jump(0x004D8C83, &_InfantryClass_Doing_AI_JumpJet_Idle_Patch);
    Patch_Jump(0x004D50C9, &_InfantryClass_AI_JumpJet_Idle_Between_Firing_Patch);
    Patch_Jump(0x004D9076, &_InfantryClass_Movement_AI_JumpJet_Not_Moving_Patch);
    //Patch_Jump(0x004D279D, &_InfantryClass_Take_Damage_JumpJet_Remove_Explosion_Patch);
    Patch_Jump(0x004D282A, &_InfantryClass_Take_Damage_JumpJet_Do_Tumble_Patch);
}
