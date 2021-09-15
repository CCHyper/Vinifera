/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          HOUSEEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended HouseClass.
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
#include "houseext_init.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "house.h"
#include "housetype.h"
#include "technotype.h"
#include "super.h"
#include "unittype.h"
#include "rules.h"
#include "rulesext.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


// harvunit 004C171E
// harvunit 004C1672
// harvunit 004C0F64
// harvunit 004C0D71
// harvunit 004C0B7B
// harvunit 004C0AFB
// harvunit 004BD0C7
// harvunit 004BCF46
// harvunit 004BAF50
// harvunit 004BAF0A
// harvunit 004A7A4B
// harvunit 004581A1
// harvunit 0045817A


/**
 *  #issue-177
 * 
 *  Allow the game to check BaseUnit for all pertinent entries for "Short Game".
 * 
 *  #NOTE: The code before this patch already checks if the house has
 *         any buildings first.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_HouseClass_AI_Short_Game_BaseUnit_Patch)
{
    GET_REGISTER_STATIC(HouseClass *, this_ptr, esi);
    static UnitTypeClass *unittype;
    static UnitType unit;
    static int count;

    /**
     *  Fetch the extended rules class instance and make sure value entries
     *  have been loaded.
     */
    if (RulesExtension && RulesExtension->BaseUnit.Count() > 0) {

        /**
         *  Fetch the first buildable base unit from the new base unit entry
         *  and get the current count of that unit that this house owns.
         */
        unittype = this_ptr->Get_First_Ownable(RulesExtension->BaseUnit);
        if (unittype) {
            unit = (UnitType)unittype->Get_Heap_ID();
            count = this_ptr->UQuantity.Count_Of(unit);
        }

    /**
     *  Fallback to the original code.
     */
    } else {

        /**
         *  Get the current count of the base unit that this house owns.
         */
        unit = (UnitType)Rule->BaseUnit->Get_Heap_ID();
        count = this_ptr->UQuantity.Count_Of(unit);

    }

    /**
     *  If no ownable base units were found, blow up the house.
     */
    if (!count) {
        goto blowup_house;
    }

    /**
     *  
     */
continue_function:
    JMP_REG(eax, 0x004BCF6E);

    /**
     *  Blows up the house, marking the house as defeated.
     */
blowup_house:
    JMP_REG(ecx, 0x004BCF60);
}


/**
 *  Patch for InstantSuperRechargeCommandClass
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_HouseClass_Super_Weapon_Handler_InstantRecharge_Patch)
{
    GET_REGISTER_STATIC(HouseClass *, this_ptr, edi);
    GET_REGISTER_STATIC(SuperClass *, special, esi);
    static bool is_player;

    is_player = false;
    if (this_ptr == PlayerPtr) {
        is_player = true;
    }

    if (Vinifera_DeveloperMode) {

        if (!special->IsReady) {

            /**
             *  If AIInstantBuild is toggled on, make sure this is a non-human AI house.
             */
            if (Vinifera_Developer_AIInstantSuperRecharge
                && !this_ptr->Is_Human_Control() && this_ptr != PlayerPtr) {

                special->Forced_Charge(is_player);

            /**
             *  If InstantBuild is toggled on, make sure the local player is a human house.
             */
            } else if (Vinifera_Developer_InstantSuperRecharge
                && this_ptr->Is_Human_Control() && this_ptr == PlayerPtr) {
                
                special->Forced_Charge(is_player);

            /**
             *  If the AI has taken control of the player house, it needs a special
             *  case to handle the "player" instant recharge mode.
             */
            } else if (Vinifera_Developer_InstantSuperRecharge) {
                if (Vinifera_Developer_AIControl && this_ptr == PlayerPtr) {
                    
                    special->Forced_Charge(is_player);
                }
            }

        }

    }

    /**
     *  Stolen bytes/code.
     */
    if (!special->AI(is_player)) {
        goto continue_function;
    }

add_to_sidebar:
    JMP(0x004BD320);

continue_function:
    JMP(0x004BD332);
}


/**
 *  Patch for BuildCheatCommandClass
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_HouseClass_Can_Build_BuildCheat_Patch)
{
    GET_REGISTER_STATIC(HouseClass *, this_ptr, ebp);
    GET_REGISTER_STATIC(int, vector_count, ecx);
    GET_STACK_STATIC(TechnoTypeClass *, objecttype, esp, 0x30);

    if (Vinifera_DeveloperMode && Vinifera_Developer_BuildCheat) {

        /**
         *  AI houses have access to everything, so we can just
         *  filter to the human houses only.
         */
        if (this_ptr->IsHuman && this_ptr->IsPlayerControl) {

            /**
             *  Check that the object has this house set as one of its owners.
             *  if true, force this 
             */
            if (((1 << this_ptr->Class->ID) & objecttype->Get_Ownable()) != 0) {
                //DEBUG_INFO("Forcing \"%s\" available.\n", objecttype->IniName);
                goto return_true;
            }
        }
    }

    /**
     *  Stolen bytes/code.
     */
original_code:
    _asm { xor eax, eax }
    _asm { mov [esp+0x34], eax }

    _asm { mov ecx, vector_count }
    _asm { test ecx, ecx }

    _asm { mov ecx, 0x004BBD2E }; // Need to use ECX as EAX is used later on.
    _asm { jmp ecx };

return_true:
    JMP(0x004BBD17);
}


/**
 *  Main function for patching the hooks.
 */
void HouseClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    HouseClassExtension_Init();

    Patch_Jump(0x004BBD26, &_HouseClass_Can_Build_BuildCheat_Patch);
    Patch_Jump(0x004BD30B, &_HouseClass_Super_Weapon_Handler_InstantRecharge_Patch);
    Patch_Jump(0x004BBD26, &_HouseClass_Can_Build_BuildCheat_Patch);
    Patch_Jump(0x004BCEE7, &_HouseClass_AI_Short_Game_BaseUnit_Patch);
}
