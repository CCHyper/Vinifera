/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SUPEREXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended SuperClass.
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
#include "superext_hooks.h"
#include "superext_init.h"
#include "superext.h"
#include "super.h"
#include "supertype.h"
#include "supertypeext.h"
#include "vinifera_defines.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  This patch extends the switch case in SuperClass::Place.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_SuperClass_Place_Extend_Switch_Patch)
{
    GET_REGISTER_STATIC(SuperClass *, this_ptr, esi);
    GET_STACK_STATIC(Cell *, cell, esp, 0x94);
    GET_STACK_STATIC(bool, player, esp, 0x98);
    static SuperWeaponTypeClass *supertype;
    static SuperClassExtension *superext;
    static SuperWeaponTypeClassExtension *supertypeext;

#ifndef NDEBUG
    /**
     *  Helper info for debugging when adding new special types.
     */
    DEV_DEBUG_INFO("SuperClass::Place()\n");
    if (cell && *cell) DEV_DEBUG_INFO("  Cell: %d,%d\n", cell->X, cell->Y);
    DEV_DEBUG_INFO("  Player: %s\n", player ? "yes" : "no");
#endif

    supertype = this_ptr->Class;

    superext = SuperClassExtensions.find(this_ptr);
    supertypeext = SuperWeaponTypeClassExtensions.find(supertype);

    /**
     *  Skip null specials.
     */
    if (supertype->Type == SPECIAL_NONE) {
        goto do_nothing;
    }

    /**
     *  Handle the original SpecialWeaponTypes.
     */
    if (supertype->Type < SPECIAL_COUNT) {
        DEV_DEBUG_INFO("Placing special.\n");
        goto super_switch;
    }

    /**
     *  New SpecialWeaponType switch.
     */
    if (supertype->Type < NEW_SPECIAL_COUNT) {
        DEV_DEBUG_INFO("Placing new special.\n");
    }

    switch (supertype->Type) {

        /**
         *  Unexpected SpecialWeaponType.
         */
        default:
            goto do_nothing;

        case SPECIAL_CREDITS:
            superext->Place_Credits(*cell, player);
            break;
    }

    /**
     *  The default case, return doing nothing.
     */
do_nothing:
    JMP(0x0060C6D5);

    /**
     *  The switch case for the original SpecialWeaponTypes
     */
super_switch:
    _asm { mov esi, this_ptr }
    _asm { mov ebx, [esi+0x14] } // this->Class
    _asm { mov eax, [ebx+0x7C] } // Class->Type
    JMP_REG(ecx, 0x0060BF8F);
}


/**
 *  Main function for patching the hooks.
 */
void SuperClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    SuperClassExtension_Init();

    Patch_Jump(0x0060BF80, &_SuperClass_Place_Extend_Switch_Patch);
}
