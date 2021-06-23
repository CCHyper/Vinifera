/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AIRCRAFTEXT_FUNCTIONS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the supporting functions for the extended AircraftClass.
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
#include "aircraftext_functions.h"
#include "tibsun_inline.h"
#include "aircraft.h"
#include "aircrafttype.h"
#include "drawshape.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"



// Replace "Class->Rotation" with new Facings= loaded from ArtINI for aircrafts

// use turret image for voxel rotors?



void Move_Point(Point2D &xy, DirType dir, int distance)
{
    double radians = (double)(dir - 0x3FFF) * -0.00009587672516830327;

    xy.Y -= (WWMath::Sin(radians) * distance);
    xy.X += (WWMath::Cos(radians) * distance);
}




/**
 *  Fetch the shape number to use for the desired facing.
 * 
 *  @author: CCHyper
 */
int Aircraft_Shape_Number(FacingClass &facing, int facings_count)
{
    int shapenum = 0;

    /**
     *  Fetch the current facing value in the required units.
     */
    switch (facings_count) {

        case 8:
            shapenum = Dir_To_8(facing.Current());
            break;

        case 16:
            shapenum = Dir_To_16(facing.Current());
            break;

        case 32:
            shapenum = Dir_To_32(facing.Current());
            break;

        case 64:
            shapenum = Dir_To_64(facing.Current());
            break;

        default:
            shapenum = 0;
            break;
    };

    return shapenum;
}


/**
 *  Renders an aircraft object with shape graphics at the location specified.
 * 
 *  @author: CCHyper
 */
void Aircraft_Draw_Shape(AircraftClass *this_ptr, Point2D &point, Rect &bounds)
{
    if (!this_ptr) {
        DEBUG_WARNING("Null pointer in Aircraft_Draw_Shape!\n");
        return;
    }

    const ShapeFileStruct *shapefile = (const ShapeFileStruct *)this_ptr->Get_Image_Data();
    if (!shapefile) {
        return;
    }

    ShapeFlagsType flags = SHAPE_NORMAL|SHAPE_CENTER;

    int shapenum = Aircraft_Shape_Number(this_ptr->PrimaryFacing, this_ptr->Class->Rotation);

    /**
     *  Draw the root body of the aircraft.
     */
    this_ptr->Draw_Object(shapefile, shapenum, point, bounds, DIR_N,
        256, 0, this_ptr->Get_Z_Gradient(), false, bounds.Width, nullptr, 0, 0, 0, flags);
}


/**
 *  Draw rotor blades on the aircraft.
 * 
 *  @author: CCHyper
 */
void Aircraft_Draw_Shape_Rotors(AircraftClass *this_ptr, Point2D &point, Rect &bounds)
{
    if (!this_ptr) {
        DEBUG_WARNING("Null pointer in Aircraft_Draw_Shape_Rotors!\n");
        return;
    }

    int shapenum;

    const ShapeFileStruct *left_rotor = (const ShapeFileStruct *)AircraftTypeClass::LRotorData;
    const ShapeFileStruct *right_rotor = (const ShapeFileStruct *)AircraftTypeClass::RRotorData;

    if (!left_rotor || !right_rotor) {
        return;
    }

    Point2D draw_point = point;
    ShapeFlagsType flags = SHAPE_NORMAL|SHAPE_CENTER;
    int height = this_ptr->Get_Height();

    /**
     *  The rotor shape number depends on whether the helicopter is idling
     *  or not. A landed helicopter uses slow moving "idling" blades.
     */
    if (height == 0) {
        shapenum = (this_ptr->Fetch_Stage() % 8) + 4;
        flags |= SHAPE_TRANS75;
    } else {
        shapenum = this_ptr->Fetch_Stage() % 4;
        flags |= SHAPE_TRANS50;
    }

    if (true) {

        int _stretch[FACING_COUNT] = { 8, 9, 10, 9, 8, 9, 10, 9 };

        /**
         *  Dual rotors offset along flight axis.
         */
        draw_point.Y -= Lepton_To_Pixel(height);

        FacingType face = Dir_Facing(this_ptr->SecondaryFacing.Current().Get_Dir());
        Move_Point(draw_point, this_ptr->SecondaryFacing.Current().Get_Dir(), _stretch[face]);

        this_ptr->Draw_Object(right_rotor, shapenum, draw_point, bounds, DIR_N,
            256, 0, this_ptr->Get_Z_Gradient(), false, bounds.Width, nullptr, 0, 0, 0, flags);

        Move_Point(draw_point, this_ptr->SecondaryFacing.Current().Get_Dir()+DIR_S, _stretch[face]*2);

        this_ptr->Draw_Object(left_rotor, shapenum, draw_point, bounds, DIR_N,
            256, 0, this_ptr->Get_Z_Gradient(), false, bounds.Width, nullptr, 0, 0, 0, flags);

    } else {

        draw_point.Y -= (Lepton_To_Pixel(height) - 2);

        /**
         *  Single rotor centered about shape.
         */
        this_ptr->Draw_Object(right_rotor, shapenum, draw_point, bounds, DIR_N,
            256, 0, this_ptr->Get_Z_Gradient(), false, bounds.Width, nullptr, 0, 0, 0, flags);

    }
}


#include "tspp.h"
DEFINE_IMPLEMENTATION(Matrix3D &Voxel_Matrix(), 0x00754BE0);

/**
 *  Renders an aircraft object with shape graphics at the location specified.
 * 
 *  @author: CCHyper
 */
void Aircraft_Draw_Voxel_Rotors(AircraftClass *this_ptr, Point2D &point, Rect &bounds)
{
    if (!this_ptr) {
        DEBUG_WARNING("Null pointer in Aircraft_Draw_Voxel_Rotors!\n");
        return;
    }






    Point2D draw_point = this_ptr->Locomotion()->Draw_Point();
    Point2D shadow_point = this_ptr->Locomotion()->Shadow_Point();




    Matrix3D draw_matrix = this_ptr->Locomotion()->Draw_Matrix();
    Matrix3D shadow_matrix = this_ptr->Locomotion()->Shadow_Matrix();

    Matrix3D d_m = draw_matrix * Voxel_Matrix();
    Matrix3D s_m = shadow_matrix * Voxel_Matrix();











    // 004A5D10 draw shadow

    //this_ptr->entry_328();






}


void Aircraft_Draw_Shadow(AircraftClass *this_ptr, Point2D &point, Rect &bounds)
{
}


void Aircraft_Draw_Rotor_Shadow(AircraftClass *this_ptr, Point2D &point, Rect &bounds)
{
}
