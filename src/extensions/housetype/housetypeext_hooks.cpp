/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          HOUSETYPEEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended HouseTypeClass.
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
#include "housetypeext_hooks.h"
#include "housetypeext_init.h"
#include "housetypeext.h"
#include "housetype.h"
#include "side.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-218
 * 
 *  This is a real kludge to ensure the original games works correctly
 *  with our new side asset loading system.
 * 
 *  The only reason this is needed is because house "Nod" in RULES.INI
 *  has "Side=GDI" set, which with the new system means Nod gets GDI
 *  sidebar and speech.
 * 
 *  Do I want to do this? No... Do I need to do this? Yes...
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_HouseTypeClass_Read_INI_Side_Kludge_Patch)
{
    /**
     *  Vanilla Tiberian Sun sides.
     */
    #define SIDE_GDI SideType(0)
    #define SIDE_NOD SideType(1)
    #define HOUSE_GDI HousesType(0)
    #define HOUSE_NOD HousesType(1)

    /**
     *  Stolen bytes/code.
     */
    _asm { mov [ebx+0x68], eax }; // Set this->Side with return from ini.Get_SideType("Side");

    GET_STACK_STATIC(SideType, side_default, esp, 0x14);
    GET_REGISTER_STATIC(HouseTypeClass *, this_ptr, ebx);

    /**
     *  Match criteria;
     *   - HouseTypes name is "Nod"
     *   - HouseType "Nod" is index 1
     *   - Side is GDI (index 0)
     *   - Side GDI (index 0) name is "GDI"
     *   - Side 1 name is "Nod"
     * 
     *  This must be done before the sides are added to the vector!
     */
    if (strcmpi("Nod", this_ptr->Name()) == 0
     && this_ptr->Get_Heap_ID() == HOUSE_NOD
     && this_ptr->Side == SIDE_GDI
     && strcmpi(Sides[this_ptr->Side]->Name(), "GDI") == 0
     && strcmpi(Sides[SIDE_NOD]->Name(), "Nod") == 0) {

        DEBUG_WARNING("House \"%s\" (%d) has \"Side=GDI\", forcing Side to \"Nod\"!\n",
            this_ptr->Name(), this_ptr->Get_Heap_ID());

        /**
         *  For this HouseType's Side to SIDE_NOD, fixing the issue
         *  so vanilla Tiberian Sun works with our new loading system.
         */
        this_ptr->Side = SIDE_NOD;
    }

    /**
     *  Add the side to the list of Sides.
     */
side_add:
    _asm { mov ebx, this_ptr };
    _asm { mov eax, [ebx+0x68] }; // this->Side
    _asm { mov edx, side_default };

    JMP_REG(ecx, 0x004CE165); // Jump to "cmp eax, edx"

    #undef HOUSE_NOD
    #undef HOUSE_GDI
    #undef SIDE_NOD
    #undef SIDE_GDI
}


/**
 *  Main function for patching the hooks.
 */
void HouseTypeClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    HouseTypeClassExtension_Init();

    Patch_Jump(0x004CE15E, &_HouseTypeClass_Read_INI_Side_Kludge_Patch);
}
