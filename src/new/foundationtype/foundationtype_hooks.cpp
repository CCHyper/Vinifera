/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FOUNDATIONTYPE_HOOKS.H
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for FoundationTypeClass.
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
#include "foundationtype_hooks.h"
#include "foundationtype.h"
#include "building.h"
#include "buildingtype.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  x
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(BuildingTypeClass_Occupy_List_OccupyList_Patch)
{
    GET_REGISTER_STATIC(BuildingTypeClass *, this_ptr, esi);

    _asm { pop esi }



    JMP(0x004E7BB0);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
DECLARE_PATCH(BuildingClass_Clear_Factory_Bib_ExitList_Patch)
{
    GET_REGISTER_STATIC(BuildingClass *, this_ptr, esi);
    static Cell exit_cell;
    static const FoundationTypeClass *foundation;

    foundation = FoundationTypeClass::As_Pointer(this_ptr->Class->Size);

    exit_cell.X = foundation->Exit_Cell().X;
    exit_cell.Y = foundation->Exit_Cell().Y;

    _asm { mov edi, exit_cell }

    JMP(0x0042FF9A);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
DECLARE_PATCH(BuildingClass_Mission_Unload_ExitList_Patch)
{
    GET_REGISTER_STATIC(BuildingClass *, this_ptr, ebp);
    static Cell exit_cell;
    static const FoundationTypeClass *foundation;

    foundation = FoundationTypeClass::As_Pointer(this_ptr->Class->Size);

    exit_cell.X = foundation->Exit_Cell().X;
    exit_cell.Y = foundation->Exit_Cell().Y;

    _asm { mov esi, exit_cell }

    _asm { mov edx, [ebp+0x0] }
    _asm { mov ecx, ebp }

    JMP(0x0043356B);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
DECLARE_PATCH(BuildingClass_Find_Exit_Cell_ExitList_Patch)
{
    GET_REGISTER_STATIC(BuildingClass *, this_ptr, ebx);
    static Cell *exit_list;
    static const FoundationTypeClass *foundation;

    foundation = FoundationTypeClass::As_Pointer(this_ptr->Class->Size);

    exit_list = foundation->Exit_List();

    _asm { mov edi, exit_list }

    JMP(0x00434260);
}


/**
 *  Main function for patching the hooks.
 */
void FoundationTypeClass_Hooks()
{
    //Patch_Jump(0x0043FF0F, &BuildingTypeClass_Occupy_List_OccupyList_Patch);

    Patch_Jump(0x0042FF8D, &BuildingClass_Clear_Factory_Bib_ExitList_Patch);
    Patch_Jump(0x0043355D, &BuildingClass_Mission_Unload_ExitList_Patch);
    Patch_Jump(0x0043425A, &BuildingClass_Find_Exit_Cell_ExitList_Patch);
}
