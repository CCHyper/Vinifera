/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TECHNOEXT_FUNCTIONS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the supporting functions for the extended TechnoClass.
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
#include "technoext_functions.h"
#include "technoext.h"
#include "technotypeext.h"
#include "techno.h"
#include "building.h"
#include "ebolt.h"
#include "target.h"
#include "drawshape.h"
#include "tactical.h"
#include "tibsun_inline.h"
#include "debughandler.h"
#include "asserthandler.h"


// TODO, move to WeaponType.
static const ShapeFileStruct * LightningShapes = nullptr;


/**
 *  Creates a electric bolt zap from the firing techno to the target.
 * 
 *  @author: CCHyper
 */
EBoltClass *TechnoClassExtension_Electric_Zap(TechnoClass *this_ptr, TARGET target, int which, const WeaponTypeClass *weapontype, Coordinate &source_coord)
{
    ASSERT(this_ptr != nullptr);

    EBoltClass *ebolt = new EBoltClass;
    if (!ebolt) {
        return nullptr;
    }

    int z_adj = 0;

    if (Is_Target_Building(target)) {
        Coordinate source = this_ptr->Render_Coord();

        Point2D p1 = TacticalMap->func_60F150(source);
        Point2D p2 = TacticalMap->func_60F150(source_coord);

        z_adj = p2.Y - p1.Y;

        if (z_adj > 0) {
            z_adj = 0;
        }
    }

    Coordinate target_coord = Is_Target_Object(target) ?
        reinterpret_cast<ObjectClass *>(target)->Target_Coord() : // #TODO: Should be Target_As_Object.
        target->entry_5C();

    /**
     *  Spawn the electric bolt.
     */
    ebolt->Create(source_coord, target_coord, z_adj);

    return ebolt;
}


/**
 *  Creates an instance of the electric bolt from the firing techno to the target.
 * 
 *  @author: CCHyper
 */
EBoltClass *TechnoClassExtension_Electric_Bolt(TechnoClass *this_ptr, TARGET target)
{
    ASSERT(this_ptr != nullptr);

    WeaponSlotType which = this_ptr->What_Weapon_Should_I_Use(target);
    const WeaponTypeClass *weapontype = this_ptr->Get_Weapon(which)->Weapon;
    Coordinate fire_coord = this_ptr->Fire_Coord(which);

    EBoltClass *ebolt = TechnoClassExtension_Electric_Zap(this_ptr, target, which, weapontype, fire_coord);
    if (ebolt) {
        if (this_ptr->IsActive) {

            TechnoClassExtension *technoext;
            technoext = TechnoClassExtensions.find(this_ptr);
            if (technoext) {

                /**
                 *  Remove existing electric bolt from the object.
                 */
                if (technoext->ElectricBolt) {
                    technoext->ElectricBolt->Flag_To_Delete();
                    technoext->ElectricBolt = nullptr;
                }

                if (!technoext->ElectricBolt) {
                    technoext->ElectricBolt = ebolt;
                    technoext->ElectricBolt->Set_Properties(this_ptr, weapontype, which);
                }
            }
        }
    }

    return ebolt;
}


/**
 *  Creates a electric bolt zap (using the legacy style from RA1) from the
 *  firing techno to the target.
 * 
 *  @author: CCHyper (Modified for Vinifera)
 */
bool TechnoClassExtension_Legacy_Electric_Bolt(TechnoClass *this_ptr, TARGET target, int which, const WeaponTypeClass *weapontype, Coordinate &source_coord)
{
    ASSERT(this_ptr != nullptr);

    static int _shape[]={ 2, 3, 1, 0, 2, 3, 1, 0};
    static int  _xadd[8][8]={
        { 0, 8, 8, 8, 0, 0, 0, 0},
        { 0, 8, 8, 8, 0, 0, 0, 0},
        { 0, 8, 8, 8, 0, 0, 0, 0},
        { 0, 8, 8, 8, 0, 0, 0, 0},
        { 0, 8, 8, 8, 0, 0, 0, 0},
        {-8, 0, 0, 0,-8,-8,-8,-8},
        {-8, 0, 0, 0,-8,-8,-8,-8},
        {-8, 0, 0, 0,-8,-8,-8,-8}
    };
    static int  _yadd[8][8]={
        {-8,-8,-8, 0, 0, 0,-8,-8},
        {-8,-8,-8, 0, 0, 0,-8,-8},
        { 0, 0, 0, 8, 8, 8, 0, 0},
        { 0, 0, 0, 8, 8, 8, 0, 0},
        { 0, 0, 0, 8, 8, 8, 0, 0},
        { 0, 0, 0, 8, 8, 8, 0, 0},
        { 0, 0, 0, 8, 8, 8, 0, 0},
        {-8,-8,-8, 0, 0, 0,-8,-8}
    };

	if (!LightningShapes) {
		LightningShapes = (const ShapeFileStruct *)MFCC::Retrieve("LITNING.SHP");
	}

    bool gonnadraw = false;
	int x = 0;
	int y = 0;
	int x1 = 0;
	int y1 = 0;
    int z_adj = 0;

    if (this_ptr->What_Am_I() == RTTI_BUILDING) {
        reinterpret_cast<BuildingClass *>(this_ptr)->IsCharging = false;
    }

    if (Is_Target_Building(target)) {

        Coordinate source = this_ptr->Render_Coord();

        Point2D p1 = TacticalMap->func_60F150(source);
        Point2D p2 = TacticalMap->func_60F150(source_coord);

		x = p1.X;
		y = p1.Y;
		x1 = p2.X;
		y1 = p2.Y;

        z_adj = p2.Y - p1.Y;

        if (z_adj > 0) {
            z_adj = 0;
        }

        gonnadraw = true;
    }

    Coordinate target_coord = Is_Target_Object(target) ?
        reinterpret_cast<ObjectClass *>(target)->Target_Coord() : // #TODO: Should be Target_As_Object.
        target->entry_5C();

	int savex = x;
	int savey = y;
	if (gonnadraw) {
		for (int shots = 0; shots < 3; shots++) {
			x = savex;
			y = savey;
			int lastfacing = 0;
			while (Distance(Point2D(x, y), Point2D(x1, y1)) > 8) {

				/*
				**	Determine true (0..7) facing from current position to
				**	destination (actually the source coordinate of the zap).
				*/
				int facing = Dir_Facing(Desired_Facing8(Point2D(x, y), Point2D(x1, y1)));

				/*
				** If there's quite a bit of distance to go,
				** we may vary the desired facing to give the
				** bolt some randomness.
				*/
				if (Distance(Point2D(x, y), Point2D(x1, y1)) > 40) {
					switch (Sim_Random_Pick(1, 3 + ((shots==0) ? 3 : 0))) {
						case 1:
							facing++;
							break;

						case 2:
							facing--;
							break;

						default:
							break;
					}
					facing &= 7;
				}

				/*
				** Now that we have the direction of the bolt,
				** draw it and move the x & y coords in the right
				** direction for the next piece.
				*/
				x += _xadd[facing][lastfacing];
				y += _yadd[facing][lastfacing];

				CC_Draw_Shape(CompositeSurface, NormalDrawer,
					LightningShapes, _shape[facing]+(shots ? 4 : 0), &Point2D(x, y),
					(window != WINDOW_PARTIAL) ? window : WINDOW_TACTICAL, SHAPE_TRANS50|SHAPE_CENTER, z_adj);

				lastfacing = facing;
			}
		}
	}

	return gonnadraw;
}
