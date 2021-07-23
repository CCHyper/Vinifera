/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BUILDINGEXT_FUNCTIONS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         
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
#include "buildingext_functions.h"
#include "buildingtypeext.h"
#include "buildingtype.h"
#include "building.h"
#include "unit.h"
#include "unittype.h"
#include "house.h"
#include "housetype.h"
#include "tibsun_inline.h"


void Building_FreeUnit(BuildingClass *this_ptr)
{
    if (!this_ptr) {
        return;
    }

    BuildingTypeClass *buildingtype = this_ptr->Class;

    bool has_custom_coord = false;
    bool freeunit_placed = false;

    BuildingTypeClassExtension *buildingtypeext = BuildingTypeClassExtensions.find(buildingtype);
    if (buildingtypeext) {

        /**
         *  
         */
        if (buildingtypeext->FreeUnitCoordinate != TPoint3D<int>(0,0,0)) {
            has_custom_coord = true;
        }
    }

    UnitClass *unit = new UnitClass(buildingtype->FreeUnit, this_ptr->House);
    if (unit != nullptr) {

        /**
         *  If this building has a custom free unit exit coord defined then
         *  first attempt to place it down at that location offset.
         */
        if (has_custom_coord) {

            Coordinate coord = this_ptr->Center_Coord();
            
            /**
             *  Custom free unit coord is offset from the center of the
             *  the building, so adjust it.
             */
            coord.X += buildingtypeext->FreeUnitCoordinate.X;
            coord.Y += buildingtypeext->FreeUnitCoordinate.Y;
            coord.Z += buildingtypeext->FreeUnitCoordinate.Z;

            /**
             *  Try to place down the harvester at the coord specified.
             */
            if (unit->Unlimbo(coord, Facing_Dir(buildingtypeext->FreeUnitDir))) {

                /**
                 *  
                 */
                freeunit_placed = true;
            }

        }

        /**
         *  Original FreeUnit placement logic.
         */
        if (freeunit_placed) {

            Cell cell = Coord_Cell(Adjacent_Cell(this_ptr->Center_Coord(), DIR_W));
            Coordinate coord = Cell_Coord(cell, true);
        
            /**
             *  Try to place down the harvesters. If it could not be placed, then
             *  try to place it in a nearby location.
             */
            if (!unit->Unlimbo(coord, DIR_W)) {

                /**
                 *  Check multiple times for clear locations.
                 */
                cell = unit->Nearby_Location(this_ptr);
                if (!unit->Unlimbo(coord, DIR_SW)) {

                    cell = unit->Nearby_Location(this_ptr);
                    unit->Unlimbo(coord, DIR_SW);

                }

            }

        }

        /**
         *  If the harvester could still not be placed, then refund the
         *  money to the owner and then bail.
         */
        if (unit->IsInLimbo) {
            this_ptr->House->Refund_Money(unit->Class->Cost_Of());
            delete unit;

        } else {
        
            if (unit->Class->IsToHarvest || unit->Class->IsToVeinHarvest) {
                unit->Assign_Mission(MISSION_HARVEST);

            } else if (buildingtypeext && buildingtypeext->FreeUnitMission != MISSION_NONE) {
                unit->Assign_Mission(buildingtypeext->FreeUnitMission);

            } else {
                unit->Assign_Mission(MISSION_GUARD);
            }

            unit->Commence();
        }

    } else {

        /**
         *  If the harvester could not be created in the first place, then give
         *  the full refund price to the owning player.
         */
        this_ptr->House->Refund_Money(buildingtype->FreeUnit->Cost_Of());
    }
}
