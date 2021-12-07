/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ROCKETLOCOMOTION.CPP
 *
 *  @authors       CCHyper, tomsons26
 *
 *  @brief         Rocket locomotion.
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
#include "rocketlocomotion.h"
#include "tibsun_inline.h"
#include "tibsun_globals.h"
#include "iomap.h"
#include "cell.h"
#include "foot.h"
#include "tactical.h"
#include "wwmath.h"
#include "debughandler.h"


/**
 *  Retrieves the class identifier (CLSID) of the object.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP RocketLocomotionClass::GetClassID(CLSID *pClassID)
{    
    if (pClassID == nullptr) {
        return E_POINTER;
    }

    *pClassID = __uuidof(this);

    return S_OK;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 * 
 *  @author: CCHyper
 * 
 *  @param      pStm           An IStream pointer to the stream from which the object should be loaded.
 */
IFACEMETHODIMP RocketLocomotionClass::Load(IStream *pStm)
{
    HRESULT hr = LocomotionClass::Locomotion_Load(pStm);
    if (SUCCEEDED(hr)) {
        // Insert any data to be loaded here.
    }

    return hr;
}


/**
 *  Class default constructor.
 * 
 *  @author: CCHyper
 */
RocketLocomotionClass::RocketLocomotionClass() :                                // DONE
    LocomotionClass(),
    DestinationCoord(),
    field_24(),
    field_34(),
    field_44(),
    field_48(0),
    field_4C(0),
    field_50(true),
    field_51(false),
    field_54(0.0f),
    field_58(0)
{
}


/**
 *  Class destructor.
 * 
 *  @author: CCHyper
 */
RocketLocomotionClass::~RocketLocomotionClass()                                 // DONE
{
}


/**
 *  Sees if object is moving.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(bool) RocketLocomotionClass::Is_Moving()                        // DONE
{
    return DestinationCoord;
}


/**
 *  Fetches destination coordinate.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(Coordinate) RocketLocomotionClass::Destination()                // DONE
{
    return DestinationCoord;
}


/**
 *  Fetch voxel draw matrix.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(Matrix3D) RocketLocomotionClass::Draw_Matrix(int *key)
{
    return LocomotionClass::Draw_Matrix(key);   // TODO
}


/**
 *  Process movement of object.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(bool) RocketLocomotionClass::Process()
{
    return Is_Moving();   // TODO
}


/**
 *  Instruct to move to location specified.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) RocketLocomotionClass::Move_To(Coordinate to)
{
   // TODO
}


/**
 *  Stop moving at first opportunity.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) RocketLocomotionClass::Stop_Moving()                      // DONE
{
    // empty
}


/**
 *  What display layer is it located in.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(LayerType) RocketLocomotionClass::In_Which_Layer()              // DONE
{
    return LAYER_AIR;
}


/**
 *  Is it actually moving across the ground this very second?
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(bool) RocketLocomotionClass::Is_Moving_Now()
{
    return false;   // TODO
}


/**
 *  x
 * 
 *  @author: x
 */
Coordinate RocketLocomotionClass::func_00661FE0(int a1)
{
    x = (LinkedTo->PrimaryFacing.Current()->un.Facing - 0x3FFF) * -0.00009587672516830327;
    *a2 = v13;

    v13 = (WWMath::Cos(field_54) * a1);

    Coordinate retval;
    retval.X = (WWMath::Cos(x) * a1 + LinkedTo->Coord.X);
    retval.Y = (LinkedTo->Coord.Y - WWMath::Sin(x) * a1);
    retval.Z = (WWMath::Sin(field_54) * a1 + LinkedTo->Coord.Z);
    return retval;
}


/**
 *  x
 * 
 *  @author: x
 */
bool RocketLocomotionClass::func_006620F0(RocketStruct &a1)
{
    Coordinate tmp;
    tmp.X = LinkedTo->Coord.X;
    tmp.Y = LinkedTo->Coord.Y;
    tmp.Z = LinkedTo->Coord.Z;

    // inlined func_00661FE0?
    WWMath::Cos(field_54);
    x = (LinkedTo->PrimaryFacing.Current()->un.Facing - 0x3FFF) * -0.00009587672516830327;
    WWMath::Sin(x);
    WWMath::Cos(x);
    v5 = WWMath::Sin(field_54);

    v6 = (v5 * a1.BodyLength + tmp.Z);

    if (v6 > DestinationCoord.Z) {
        CellClass *cell = LinkedTo->Coord_CellClass();
        if (!cell || !(*cell->Bitfield2 & 0x100) || cell->Center_Coord().Z != DestinationCoord.Z || v6 > (HighBridgeHeightLeptons_661E20 + DestinationCoord.Z)) {
            if (LinkedTo->Get_Height() > 0) {
                return false;
            }
        }
    }

    func_00663030();

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
float RocketLocomotionClass::Get_Angle()                                        // DONE, need confirming         // get angle to target? 
{
    Coordinate coord = DestinationCoord - LinkedTo->Coord;
    float dist = coord.Length();
    return dist <= 0.0f ? -1.570796326794897 : WWMath::Atan(coord.Z / dist);    // TODO what is "-1.570796326794897"
}


/**
 *  x
 * 
 *  @author: x
 */
void RocketLocomotionClass::func_00663030()
{

}
