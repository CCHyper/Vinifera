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
#include "aircraftext_functions.h"
#include "aircraft.h"
#include "aircrafttype.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-
 * 
 *  
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AircraftClass_Draw_It_Draw_Shape_Draw_Rotors_Patch)
{
    GET_REGISTER_STATIC(AircraftClass *, this_ptr, ebp);
    GET_STACK_STATIC(Point2D *, point, esp, 0xCC);
    GET_STACK_STATIC(Rect *, bounds, esp, 0xD0);
    static AircraftTypeClass *aircrafttype;

    aircrafttype = this_ptr->Class;

    /**
     *  All the drawing code before this depends on IsVoxel, so checking
     *  this is false gives us a if/else scenario so we can draw the shape
     *  graphics here.
     */
    if (!aircrafttype->IsVoxel) {
        Aircraft_Draw_Shape(this_ptr, *point, *bounds);
    }

    /**
     *  If this aircraft is equipped with rotor blades, then draw them at this time.
     */
    if (this_ptr->Class->IsRotorEquipped) {
        this_ptr->Draw_Rotors(*point, *bounds);
    }

    /**
     *  This draws any overlay graphics on the aircraft.
     */
    this_ptr->FootClass::Draw_It(*point, *bounds);

    /**
     *  Function return.
     */
    _asm { pop esi }
    JMP(0x00408F88);
}


/**
 *  #issue-
 * 
 *  
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AircraftClass_Draw_Rotors_Patch)
{
    GET_REGISTER_STATIC(AircraftClass *, this_ptr, ecx);
    GET_STACK_STATIC(Point2D *, point, esp, 0x04);
    GET_STACK_STATIC(Rect *, bounds, esp, 0x08);
    static AircraftTypeClass *aircrafttype;

    _asm { pushad }

    aircrafttype = this_ptr->Class;

    if (aircrafttype->IsVoxel) {
        Aircraft_Draw_Voxel_Rotors(this_ptr, *point, *bounds);
    } else {
        Aircraft_Draw_Shape_Rotors(this_ptr, *point, *bounds);
    }

    _asm { popad }

    /**
     *  Function return.
     */
    JMP(0x00408FA8);
}


/**
 *  Main function for patching the hooks.
 */
void AircraftClassExtension_Hooks()
{
    Patch_Jump(0x00408F77, &_AircraftClass_Draw_It_Draw_Shape_Draw_Rotors_Patch);
    Patch_Jump(0x00408FA0, &_AircraftClass_Draw_Rotors_Patch);
}
