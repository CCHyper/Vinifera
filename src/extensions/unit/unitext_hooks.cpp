/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          UNITEXT_HOOKS.H
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended UnitClass.
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
#include "vinifera_globals.h"
#include "unit.h"
#include "unittype.h"
#include "target.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-#6
 * 
 *  A "quality of life" patch for harvesters so they auto harvest
 *  when they have just been kicked out of the war factory.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_UnitClass_Per_Cell_Process_AutoHarvest_Assign_Harvest_Mission_Patch)
{
    GET_REGISTER_STATIC(UnitClass *, this_ptr, ebp);
    GET_REGISTER_STATIC(TARGET, target, esi);
    static BuildingClass *building_contact;
    static UnitTypeClass *unittype;

    /**
     *  Is the unit we are processing a harvester?
     */
    unittype = reinterpret_cast<UnitTypeClass *>(this_ptr->Class_Of());
    if (unittype->IsToHarvest || unittype->IsToVeinHarvest) {

        /**
         *  Order the unit to harvest.
         */
        this_ptr->Assign_Mission(MISSION_HARVEST);

        goto continue_check_scatter;
    }

    /**
     *  Stolen bytes/code from here on, continues function flow.
     */

    /**
     *  Find out if the target is a building. (flagged to not use dynamic_cast).
     */
continue_function:
    building_contact = Target_As_Building(target, false);

    /**
     *  This is real ugly, but we replace the dynamic_cast in the original
     *  location and we need to return to just after its stack fixup.
     */
    _asm { mov ebp, this_ptr }
    _asm { mov ecx, [ebp+0x0EC] } // this->House
    _asm { mov eax, building_contact }

    JMP_REG(edx, 0x006517DB);

continue_check_scatter:
    _asm { mov ebp, this_ptr }
    JMP_REG(ecx, 0x0065194E);
}



#include "scenarioini.h"
#include "tibsun_inline.h"
DECLARE_PATCH(_MinimapCrash)
{
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, esi);
    GET_REGISTER_STATIC(unsigned, _ebx, ebx);

    static Cell cell;

    cell.X = this_ptr->Coord.X / CELL_LEPTON_W;
    cell.Y = this_ptr->Coord.Y / CELL_LEPTON_H;

    if (this_ptr->IsInLimbo && this_ptr->field_207) {
        DEBUG_WARNING("Writing map...\n");
        Write_Scenario_INI("verylikley_minimapcrash.map", true);
        DEBUG_WARNING("Very likley object causing radar crash? Coord: %d,%d,%d Cell: %d,%d\n",
            this_ptr->Coord.X, this_ptr->Coord.Y, this_ptr->Coord.Z, cell.X, cell.Y);
        JMP_REG(eax, 0x0063A46E);
    }

    if (this_ptr->IsInLimbo) {
        DEBUG_WARNING("Writing map...\n");
        Write_Scenario_INI("possible_minimapcrash.map", true);
        DEBUG_WARNING("Possible object causing radar crash? Coord: %d,%d,%d Cell: %d,%d\n",
            this_ptr->Coord.X, this_ptr->Coord.Y, this_ptr->Coord.Z, cell.X, cell.Y);
        JMP_REG(eax, 0x0063A46E);
    }

    if (!this_ptr->field_207) {
        JMP_REG(eax, 0x0063A46E);
    }

    _asm { mov esi, this_ptr }
    _asm { mov ebx, _ebx }
    //_asm { xor bl, bl }
    JMP_REG(ecx, 0x0063A44A);
}


/**
 *  Main function for patching the hooks.
 */
void UnitClassExtension_Hooks()
{
    Patch_Jump(0x006517BE, &_UnitClass_Per_Cell_Process_AutoHarvest_Assign_Harvest_Mission_Patch);

    Patch_Jump(0x0063A440, &_MinimapCrash);
}
