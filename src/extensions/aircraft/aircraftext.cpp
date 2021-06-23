/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AIRCRAFTEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended AircraftClass class.
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
#include "aircraftext.h"
#include "aircraft.h"
#include "aircrafttype.h"
#include "aircrafttypeext.h"
#include "wwcrc.h"
#include "rules.h"
#include "drawshape.h"
#include "matrix3d.h"
#include "tibsun_inline.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Provides the map for all AircraftClass extension instances.
 */
ExtensionMap<AircraftClass, AircraftClassExtension> AircraftClassExtensions;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
AircraftClassExtension::AircraftClassExtension(AircraftClass *this_ptr) :
    Extension(this_ptr)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("AircraftClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("AircraftClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
AircraftClassExtension::AircraftClassExtension(const NoInitClass &noinit) :
    Extension(noinit)
{
    IsInitialized = false;
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
AircraftClassExtension::~AircraftClassExtension()
{
    //EXT_DEBUG_TRACE("AircraftClassExtension destructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("AircraftClassExtension destructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = false;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT AircraftClassExtension::Load(IStream *pStm)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("AircraftClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    HRESULT hr = Extension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) AircraftClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT AircraftClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("AircraftClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    HRESULT hr = Extension::Save(pStm, fClearDirty);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}


/**
 *  Return the raw size of class data for save/load purposes.
 *  
 *  @author: CCHyper
 */
int AircraftClassExtension::Size_Of() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("AircraftClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void AircraftClassExtension::Detach(TARGET target, bool all)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("AircraftClassExtension::Detach - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void AircraftClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("AircraftClassExtension::Compute_CRC - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
}
















// Replace "Class->Rotation" with new Facings= loaded from ArtINI for aircrafts

// use turret image for voxel rotors?





/**
 *  Renders an aircraft object with shape graphics at the location specified.
 * 
 *  @author: CCHyper
 */
void AircraftClassExtension::Draw_Shape(Point2D &point, Rect &bounds)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("AircraftClassExtension::Draw_Shape - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

	/**
	 *  Verify the legality of the unit class.
	 */
    const ShapeFileStruct *shapefile = (const ShapeFileStruct *)ThisPtr->Get_Image_Data();
    if (!shapefile) {
        return;
    }

    ShapeFlagsType flags = SHAPE_NORMAL|SHAPE_CENTER;

    int shapenum = Shape_Number(ThisPtr->PrimaryFacing, ThisPtr->Class->Rotation);
    int shadow_shapenum = shapenum + shapefile->Get_Frame_Count()/2;

	/**
	 *  Helicopters that are flying have a "bobbing" effect.
	 */
	int jitter = 0;
	if (ThisPtr->Get_Height() == Rule->FlightLevel /*&& ThisPtr->Get_Speed() < 3*/) {
		static int _jitter[] = { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, -1, -1, -1, 0 };
		jitter = _jitter[Frame % ARRAY_SIZE(_jitter)];
	}

	/**
	 *  Special manual shadow draw code.
	 */
	if (ThisPtr->Visual_Character() <= VISUAL_DARKEN) {
		CC_Draw_Shape(TempSurface, NormalDrawer, shapefile, shadow_shapenum, &Point2D(point.X+1, point.Y+2), &bounds,
                        SHAPE_NORMAL|SHAPE_CENTER, 0, 0, ThisPtr->Get_Z_Gradient());
	}

    point.Y += jitter;

    /**
     *  Draw the root body of the aircraft.
     */
    ThisPtr->Draw_Object(shapefile, shapenum, Point2D(point.X, point.Y + jitter), bounds, DIR_N,
        256, 0, ThisPtr->Get_Z_Gradient(), false, bounds.Width, nullptr, 0, 0, 0, SHAPE_NORMAL|SHAPE_CENTER);
}


/**
 *  Draw shape rotor blades on the aircraft.
 * 
 *  @author: CCHyper
 */
void AircraftClassExtension::Draw_Shape_Rotors(Point2D &point, Rect &bounds)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("AircraftClassExtension::Draw_Shape_Rotors - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    
    //const AircraftTypeClass *aircrafttype = ThisPtr->Class;
    //if (!aircrafttype->IsTurretEquipped) {
    //    return;
    //}

    const ShapeFileStruct *left_rotor = (const ShapeFileStruct *)AircraftTypeClass::LRotorData;
    const ShapeFileStruct *right_rotor = (const ShapeFileStruct *)AircraftTypeClass::RRotorData;
    if (!left_rotor || !right_rotor) {
        return;
    }

    Point2D draw_point = point;
    ShapeFlagsType flags = SHAPE_NORMAL|SHAPE_CENTER;
    int height = ThisPtr->Get_Height();

    int shapenum = 0;

    // TODO: Handle the visual of it increasing in speed by setting height levels to change at?
    /**
     *  The rotor shape number depends on whether the helicopter is idling
     *  or not. A landed helicopter uses slow moving "idling" blades.
     */
    if (height > 0 && ThisPtr->Locomotor_Ptr()->Is_Moving()) {
        shapenum = (ThisPtr->Fetch_Stage() % 8) + 4;
        flags |= SHAPE_TRANS50;

    } else if (height > 0) {
        shapenum = ThisPtr->Fetch_Stage() % 4;
        flags |= SHAPE_TRANS50;

    } else {
        shapenum = (ThisPtr->Fetch_Stage() % 8) + 2;
        flags |= SHAPE_TRANS75;
    }



    // TEMP: enable dual rotors somehow
    if (false) {

        static int _stretch[FACING_COUNT] = { 8, 9, 10, 9, 8, 9, 10, 9 };

        /**
         *  Dual rotors offset along flight axis.
         */
        draw_point.Y -= Lepton_To_Pixel(height);

        FacingType face = Dir_Facing(ThisPtr->SecondaryFacing.Current().Get_Dir());

        /**
         *  TODO: Draw front?
         */
        Move_Point(draw_point, ThisPtr->SecondaryFacing.Current().Get_Dir(), _stretch[face]);

        ThisPtr->Draw_Object(right_rotor, shapenum, draw_point, bounds, DIR_N,
            256, 0, ThisPtr->Get_Z_Gradient(), false, bounds.Width, nullptr, 0, 0, 0, flags);

        /**
         *  TODO: Draw back?
         */
        Move_Point(draw_point, ThisPtr->SecondaryFacing.Current().Get_Dir()+DIR_S, _stretch[face]*2);

        ThisPtr->Draw_Object(left_rotor, shapenum, draw_point, bounds, DIR_N,
            256, 0, ThisPtr->Get_Z_Gradient(), false, bounds.Width, nullptr, 0, 0, 0, flags);

    } else {

        draw_point.Y -= (Lepton_To_Pixel(height) - 2);

        /**
         *  Single rotor centered about shape.
         */
        ThisPtr->Draw_Object(right_rotor, shapenum, draw_point, bounds, DIR_N,
            256, 0, ThisPtr->Get_Z_Gradient(), false, bounds.Width, nullptr, 0, 0, 0, flags);

    }
}


/**
 *  Fetch the shape number to use for the desired facing.
 * 
 *  @author: CCHyper
 */
int AircraftClassExtension::Shape_Number(FacingClass &facing, int facings_count)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("AircraftClassExtension::Shape_Number - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

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
