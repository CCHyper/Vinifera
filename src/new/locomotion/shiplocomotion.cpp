/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SHIPLOCOMOTION.CPP
 *
 *  @authors       CCHyper
 *
 *  @brief         Ship locomotion.
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
#include "shiplocomotion.h"
#include "tibsun_inline.h"
#include "tibsun_globals.h"
#include "iomap.h"
#include "cell.h"
#include "foot.h"
#include "unit.h"
#include "technotype.h"
#include "tactical.h"
#include "wwmath.h"
#include "debughandler.h"


/**
 *  Retrieves pointers to the supported interfaces on an object.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP ShipLocomotionClass::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    return LocomotionClass::QueryInterface(riid, ppvObj);
}


/**
 *  Increments the reference count for an interface pointer to a COM object.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(ULONG) ShipLocomotionClass::AddRef()
{
    return LocomotionClass::AddRef();
}


/**
 *  Decrements the reference count for an interface on a COM object.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(ULONG) ShipLocomotionClass::Release()
{
    return LocomotionClass::Release();
}


/**
 *  Retrieves the class identifier (CLSID) of the object.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP ShipLocomotionClass::GetClassID(CLSID *pClassID)
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
IFACEMETHODIMP ShipLocomotionClass::Load(IStream *pStm)
{
    HRESULT hr = LocomotionClass::Locomotion_Load(pStm);
    if (SUCCEEDED(hr)) {
        // Insert any data to be loaded here.
    }

    return hr;
}


/**
 *  Saves an object to the specified stream.
 * 
 *  @author: CCHyper
 * 
 *  @param      pStm           An IStream pointer to the stream into which the object should be saved.
 * 
 *  @param      fClearDirty    Indicates whether to clear the dirty flag after the save is complete.
 */
IFACEMETHODIMP ShipLocomotionClass::Save(IStream *pStm, BOOL fClearDirty)
{
    HRESULT hr = LocomotionClass::Save(pStm, fClearDirty);
    if (SUCCEEDED(hr)) {
        // Insert any data to be saved here.
    }

    return hr;
}


/**
 *  Retrieves the size of the stream needed to save the object.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(LONG) ShipLocomotionClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
    if (pcbSize == nullptr) {
        return E_POINTER;
    }

    HRESULT hr = LocomotionClass::GetSizeMax(pcbSize);

    return S_OK;
}


/**
 *  Class default constructor.
 * 
 *  @author: CCHyper
 */
ShipLocomotionClass::ShipLocomotionClass() :
    //LocomotionClass()
    DriveLocomotionClass()
{
}


/**
 *  Class destructor.
 * 
 *  @author: CCHyper
 */
ShipLocomotionClass::~ShipLocomotionClass()
{
}


/**
 *  Links object to locomotor.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP ShipLocomotionClass::Link_To_Object(void *object)
{
    HRESULT hr = LocomotionClass::Link_To_Object(object);

    if (SUCCEEDED(hr)) {
        DEV_DEBUG_INFO("ShipLocomotionClass - Sucessfully linked to \"%s\"\n", LinkedTo->Name());
    }

    return hr;
}


#if 0
/**
 *  Sees if object is moving.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(bool) ShipLocomotionClass::Is_Moving()
{
    return false;   // TODO
}
#endif


/**
 *  Fetches destination coordinate.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(Coordinate) ShipLocomotionClass::Destination()           // DONE
{
    return DestinationCoord;
}


/**
 *  Fetches immediate (next cell) destination coordinate.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(Coordinate) ShipLocomotionClass::Head_To_Coord()         // DONE
{
    if (!HeadToCoord) {
        return Linked_To()->Get_Coord();
    }
    return HeadToCoord;
}


#if 0
/**
 *  Fetch voxel draw matrix.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(Matrix3D) ShipLocomotionClass::Draw_Matrix(int *key)
{
    return LocomotionClass::Draw_Matrix(key);   // TODO
}
#endif


#if 0
/**
 *  Fetch shadow draw matrix.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(Matrix3D) ShipLocomotionClass::Shadow_Matrix(int *key)
{
    return LocomotionClass::Shadow_Matrix(key);   // TODO
}
#endif


/**
 *  Z adjust control value.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(int) ShipLocomotionClass::Z_Adjust()                     // DONE
{
    return 0;
}


/**
 *  Z gradient control value.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(ZGradientType) ShipLocomotionClass::Z_Gradient()         // DONE
{
    return ZGRAD_90DEG;
}


#if 0
/**
 *  Process movement of object.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(bool) ShipLocomotionClass::Process()
{
    return Is_Moving();   // TODO
}
#endif


/**
 *  Instruct to move to location specified.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) ShipLocomotionClass::Move_To(Coordinate to)        // DONE
{
    /**
     *  Object is still under the influence of an emp, do not move.
     */
    if (Linked_To()->EMPFramesRemaining > 0) {
        return;
    }

    DestinationCoord = to;

    if (Map[to].Bit2_16) {
        DestinationCoord.Z += BridgeCellHeight;
    }
}


/**
 *  Stop moving at first opportunity.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) ShipLocomotionClass::Stop_Moving()                 // NEEDS CONFIRMING
{
    if (DestinationCoord) {

        if (Linked_To()->Techno_Type_Class()->IsTrain) {

            UnitClass *unit_linked = reinterpret_cast<UnitClass *>(Linked_To());
            if (unit_linked->IsFollowing) {
                    
                UnitClass *follower = unit_linked->FollowingMe;
                while (follower) {
                    follower->Locomotor_Ptr()->Stop_Moving();
                    follower = follower->FollowingMe;
                }
            }
        }

    }

    // Huh?
    //SpeedAccum = SpeedAccum >= 0.3 ? 0.3 : SpeedAccum;
    field_50 = std::min(field_50, 0.3);

    DestinationCoord = 0;
}


/**
 *  Try to face direction specified.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) ShipLocomotionClass::Do_Turn(DirStruct coord)      // DONE
{
    Linked_To()->PrimaryFacing.Set_Desired(coord);
}


/**
 *  Object is appearing in the world.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) ShipLocomotionClass::Unlimbo()                     // DONE
{
    /**
     *  Set the objects ramp for redraw.
     */
    Force_New_Slope(Linked_To()->Get_Cell_Ptr()->Ramp);
}


#if 0
/**
 *  Force drive track -- special case only.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) ShipLocomotionClass::Force_Track(int track, Coordinate coord)
{
    // TODO
}
#endif


/**
 *  What display layer is it located in.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(LayerType) ShipLocomotionClass::In_Which_Layer()         // DONE
{
    return LAYER_GROUND;
}


/**
 *  Force a voxel unit to a given slope. Used in cratering.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) ShipLocomotionClass::Force_New_Slope(int ramp)     // DONE
{
    CurrentRamp = TileRampType(ramp);
    PreviousRamp = TileRampType(ramp);
    RampTransitionTimer = 0;
}


#if 0
/**
 *  Is it actually moving across the ground this very second?
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(bool) ShipLocomotionClass::Is_Moving_Now()
{
    if (Linked_To()->PrimaryFacing.Is_Rotating()) {
        return true;
    }

    if (Is_Moving()) {
        return HeadToCoord && Apparent_Speed() > 0;
    }

    return false;   // TODO
}
#endif


/**
 *  Lifts all occupation bits associated with the object off the map.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) ShipLocomotionClass::Mark_All_Occupation_Bits(int mark) // NEEDS CONFIRMING
{
    if (HeadToCoord) {
        Mark_Track(HeadToCoord, MarkType(mark));
    }
}


/**
 *  Is this object in the process of moving into this coord.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(bool) ShipLocomotionClass::Is_Moving_Here(Coordinate to) // DONE
{
    if (Linked_To()->PrimaryFacing.Is_Rotating()) {
        return true;
    }
    return Is_Moving() && HeadToCoord && Linked_To()->Current_Speed() > 0;
}


#if 0
/**
 *  Will this object jump tracks?
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(bool) ShipLocomotionClass::Will_Jump_Tracks()
{
    return false;   // TODO
}
#endif


#if 0
/**
 *  Locks the locomotor from being deleted.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) ShipLocomotionClass::Lock()
{
   // TODO
}
#endif


#if 0
/**
 *  Unlocks the locomotor from being deleted.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(void) ShipLocomotionClass::Unlock()
{
   // TODO
}
#endif


/**
 *  Queries internal variables.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(int) ShipLocomotionClass::Get_Track_Number()             // DONE
{
    return TrackNumber;
}


/**
 *  Queries internal variables.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(int) ShipLocomotionClass::Get_Track_Index()              // DONE
{
    return TrackIndex;
}


/**
 *  Queries internal variables.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP_(int) ShipLocomotionClass::Get_Speed_Accum()              // DONE
{
    return SpeedAccum;
}


/***********************************************************************************************
 * DriveClass::Smooth_Turn -- Handles the low level coord calc for smooth turn logic.          *
 *                                                                                             *
 *    This routine calculates the new coordinate value needed for the                          *
 *    smooth turn logic. The adjustment and flag values must be                                *
 *    determined prior to entering this routine.                                               *
 *                                                                                             *
 * INPUT:   adj      -- The adjustment coordinate as lifted from the                           *
 *                      correct smooth turn table.                                             *
 *                                                                                             *
 *          dir      -- Pointer to dir for possible modification                               *
 *                      according to the flag bits.                                            *
 *                                                                                             *
 * OUTPUT:  Returns with the coordinate the unit should positioned to.                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/14/1994 JLB : Created.                                                                 *
 *   07/13/1994 JLB : Converted to member function.                                            *
 *=============================================================================================*/
Coordinate ShipLocomotionClass::Smooth_Turn(Coordinate adj, DirType &dir)   // DONE
{
    DirType workdir = dir;
    int x;
    int y;
    int temp;
    TrackControlType flags = TrackControl[TrackNumber].Flag;

    x = adj.X;
    y = adj.Y;

    if (flags & F_T) {
        temp = x;
        x = y;
        y = temp;
        workdir = (DirType)(DIR_W - workdir);
    }

    if (flags & F_X) {
        x = -x;
        workdir = (DirType)-workdir;
    }

    if (flags & F_Y) {
        y = -y;
        workdir = (DirType)(DIR_S - workdir);
    }

    dir = workdir;

    return Coordinate(HeadToCoord.X + x, HeadToCoord.Y + y);
}


#if 0
/**
 *  Smooth turn track tables. These are coordinate offsets from the center
 *  of the destination cell. These are the raw tracks that are modified
 *  by negating the X and Y portions as necessary. Also for reverse travelling
 *  direction, the track list can be processed backward.
 *
 *  Track 1  = N
 *  Track 2  = NE
 *  Track 3  = N->NE 45 deg (double path consumption)
 *  Track 4  = N->E 90 deg (double path consumption)
 *  Track 5  = NE->SE 90 deg (double path consumption)
 *  Track 6  = NE->N 45 deg (double path consumption)
 *  Track 7  = N->NE (facing change only)
 *  Track 8  = NE->E (facing change only)
 *  Track 9  = N->E (facing change only)
 *  Track 10 = NE->SE (facing change only)
 *  Track 11 = back up into refinery
 *  Track 12 = drive out of refinery
 *  Track 13 = drive out of war factory
 */
ShipLocomotionClass::TrackType const ShipLocomotionClass::Track1[24] = {
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},        // Track jump check here.
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},

    {Coordinate(0,0), DIR_N}
};

ShipLocomotionClass::TrackType const ShipLocomotionClass::Track2[] = {
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},        // Track jump check here.
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},

    {Coordinate(0,0), DIR_NE}
};

ShipLocomotionClass::TrackType const ShipLocomotionClass::Track3[] = {
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},        // Jump entry point here.
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), (DirType)1},
    {Coordinate(TODO), (DirType)3},
    {Coordinate(TODO), (DirType)4},
    {Coordinate(TODO), (DirType)5},
    {Coordinate(TODO), (DirType)7},
    {Coordinate(TODO), (DirType)8},
    {Coordinate(TODO), (DirType)9},
    {Coordinate(TODO), (DirType)11},
    {Coordinate(TODO), (DirType)12},
    {Coordinate(TODO), (DirType)13},        // Center cell processing here.
    {Coordinate(TODO), (DirType)15},
    {Coordinate(TODO), (DirType)16},
    {Coordinate(TODO), (DirType)17},
    {Coordinate(TODO), (DirType)19},
    {Coordinate(TODO), (DirType)20},
    {Coordinate(TODO), (DirType)21},
    {Coordinate(TODO), (DirType)23},
    {Coordinate(TODO), (DirType)24},
    {Coordinate(TODO), (DirType)25},
    {Coordinate(TODO), (DirType)27},
    {Coordinate(TODO), (DirType)28},
    {Coordinate(TODO), (DirType)29},
    {Coordinate(TODO), (DirType)31},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},        // Track jump check here.
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},

    {Coordinate(0,0), DIR_NE}
};

ShipLocomotionClass::TrackType const ShipLocomotionClass::Track4[] = {
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), (DirType)1},
    {Coordinate(TODO), (DirType)1},
    {Coordinate(TODO), (DirType)2},
    {Coordinate(TODO), (DirType)3},
    {Coordinate(TODO), (DirType)4},
    {Coordinate(TODO), (DirType)5},        // Track entry here.
    {Coordinate(TODO), (DirType)8},
    {Coordinate(TODO), (DirType)12},
    {Coordinate(TODO), (DirType)16},
    {Coordinate(TODO), (DirType)20},
    {Coordinate(TODO), (DirType)23},
    {Coordinate(TODO), (DirType)27},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), (DirType)36},
    {Coordinate(TODO), (DirType)39},
    {Coordinate(TODO), (DirType)43},
    {Coordinate(TODO), (DirType)47},
    {Coordinate(TODO), (DirType)51},
    {Coordinate(TODO), (DirType)54},
    {Coordinate(TODO), (DirType)57},
    {Coordinate(TODO), (DirType)60},        // Track jump here.
    {Coordinate(TODO), (DirType)62},
    {Coordinate(TODO), (DirType)63},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), (DirType)66},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},

    {Coordinate(0,0), DIR_E}
};

ShipLocomotionClass::TrackType const ShipLocomotionClass::Track5[] = {
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},        // Track entry here.
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), (DirType)35},
    {Coordinate(TODO), (DirType)38},
    {Coordinate(TODO), (DirType)41},
    {Coordinate(TODO), (DirType)44},
    {Coordinate(TODO), (DirType)47},
    {Coordinate(TODO), (DirType)50},
    {Coordinate(TODO), (DirType)53},
    {Coordinate(TODO), (DirType)56},
    {Coordinate(TODO), (DirType)59},
    {Coordinate(TODO), (DirType)62},
    {Coordinate(TODO), (DirType)66},
    {Coordinate(TODO), (DirType)69},
    {Coordinate(TODO), (DirType)72},
    {Coordinate(TODO), (DirType)75},
    {Coordinate(TODO), (DirType)78},
    {Coordinate(TODO), (DirType)81},
    {Coordinate(TODO), (DirType)84},
    {Coordinate(TODO), (DirType)87},
    {Coordinate(TODO), (DirType)90},
    {Coordinate(TODO), (DirType)93},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},        // Track jump check here.
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},

    {Coordinate(0,0), DIR_SE}
};

ShipLocomotionClass::TrackType const ShipLocomotionClass::Track6[] = {
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},        // Jump entry point here.
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), (DirType)35},
    {Coordinate(TODO), (DirType)37},
    {Coordinate(TODO), (DirType)40},
    {Coordinate(TODO), (DirType)43},
    {Coordinate(TODO), (DirType)45},
    {Coordinate(TODO), (DirType)48},
    {Coordinate(TODO), (DirType)51},
    {Coordinate(TODO), (DirType)53},
    {Coordinate(TODO), (DirType)56},
    {Coordinate(TODO), (DirType)59},
    {Coordinate(TODO), (DirType)61},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},        // Track jump check here.
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},

    {Coordinate(0,0), DIR_E}
};

ShipLocomotionClass::TrackType const ShipLocomotionClass::Track7[] = {
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), (DirType)4},
    {Coordinate(TODO), (DirType)8},
    {Coordinate(TODO), (DirType)12},
    {Coordinate(TODO), (DirType)16},
    {Coordinate(TODO), (DirType)19},
    {Coordinate(TODO), (DirType)22},
    {Coordinate(TODO), (DirType)23},
    {Coordinate(TODO), (DirType)24},
    {Coordinate(TODO), (DirType)25},
    {Coordinate(TODO), (DirType)26},
    {Coordinate(TODO), (DirType)27},
    {Coordinate(TODO), (DirType)28},
    {Coordinate(TODO), (DirType)29},
    {Coordinate(TODO), (DirType)30},
    {Coordinate(TODO), (DirType)30},
    {Coordinate(TODO), (DirType)30},
    {Coordinate(TODO), (DirType)30},
    {Coordinate(TODO), (DirType)31},
    {Coordinate(TODO), (DirType)31},
    {Coordinate(TODO), (DirType)31},
    {Coordinate(TODO), (DirType)31},
    {Coordinate(TODO), (DirType)31},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), DIR_NE},

    {Coordinate(0,0), DIR_NE}
};

ShipLocomotionClass::TrackType const ShipLocomotionClass::Track8[] = {
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), (DirType)36},
    {Coordinate(TODO), (DirType)40},
    {Coordinate(TODO), (DirType)44},
    {Coordinate(TODO), (DirType)46},
    {Coordinate(TODO), (DirType)48},
    {Coordinate(TODO), (DirType)50},
    {Coordinate(TODO), (DirType)52},
    {Coordinate(TODO), (DirType)54},
    {Coordinate(TODO), (DirType)56},
    {Coordinate(TODO), (DirType)58},
    {Coordinate(TODO), (DirType)60},
    {Coordinate(TODO), (DirType)62},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), DIR_E},

    {Coordinate(0,0), DIR_E}
};

ShipLocomotionClass::TrackType const ShipLocomotionClass::Track9[] = {
    {Coordinate(TODO), DIR_N},
    {Coordinate(TODO), (DirType)2},
    {Coordinate(TODO), (DirType)4},
    {Coordinate(TODO), (DirType)6},
    {Coordinate(TODO), (DirType)9},
    {Coordinate(TODO), (DirType)11},
    {Coordinate(TODO), (DirType)13},
    {Coordinate(TODO), (DirType)16},
    {Coordinate(TODO), (DirType)18},
    {Coordinate(TODO), (DirType)20},
    {Coordinate(TODO), (DirType)22},
    {Coordinate(TODO), (DirType)24},
    {Coordinate(TODO), (DirType)26},
    {Coordinate(TODO), (DirType)28},
    {Coordinate(TODO), (DirType)30},
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), (DirType)34},
    {Coordinate(TODO), (DirType)36},
    {Coordinate(TODO), (DirType)39},
    {Coordinate(TODO), (DirType)41},
    {Coordinate(TODO), (DirType)43},
    {Coordinate(TODO), (DirType)45},
    {Coordinate(TODO), (DirType)48},
    {Coordinate(TODO), (DirType)50},
    {Coordinate(TODO), (DirType)52},
    {Coordinate(TODO), (DirType)54},
    {Coordinate(TODO), (DirType)56},
    {Coordinate(TODO), (DirType)58},
    {Coordinate(TODO), (DirType)62},
    {Coordinate(TODO), DIR_E},

    {Coordinate(0,0), DIR_E}
};

ShipLocomotionClass::TrackType const ShipLocomotionClass::Track10[] = {
    {Coordinate(TODO), DIR_NE},
    {Coordinate(TODO), (DirType)37},
    {Coordinate(TODO), (DirType)42},
    {Coordinate(TODO), (DirType)47},
    {Coordinate(TODO), (DirType)52},
    {Coordinate(TODO), (DirType)57},
    {Coordinate(TODO), (DirType)60},
    {Coordinate(TODO), (DirType)62},
    {Coordinate(TODO), DIR_E},
    {Coordinate(TODO), (DirType)68},
    {Coordinate(TODO), (DirType)70},
    {Coordinate(TODO), (DirType)72},
    {Coordinate(TODO), (DirType)74},
    {Coordinate(TODO), (DirType)76},
    {Coordinate(TODO), (DirType)78},
    {Coordinate(TODO), (DirType)80},
    {Coordinate(TODO), (DirType)82},
    {Coordinate(TODO), (DirType)84},
    {Coordinate(TODO), (DirType)86},
    {Coordinate(TODO), (DirType)88},
    {Coordinate(TODO), (DirType)90},
    {Coordinate(TODO), (DirType)92},
    {Coordinate(TODO), (DirType)94},
    {Coordinate(TODO), (DirType)95},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(TODO), DIR_SE},
    {Coordinate(0,0), DIR_SE}
};


/**
 *  Backup harvester into refinery.
 */
ShipLocomotionClass::TrackType const ShipLocomotionClass::Track11[] = { // CONFIRMED
    {Coordinate(0, 256), DIR_SW},
    {Coordinate(8, 243), DIR_SW},
    {Coordinate(16, 229), DIR_SW},
    {Coordinate(24, 214), DIR_SW},
    {Coordinate(32, 200), DIR_SW},
    {Coordinate(40, 185), DIR_SW},
    {Coordinate(48, 171), DIR_SW},
    {Coordinate(56, 156), DIR_SW},
    {Coordinate(64, 141), DIR_SW},
    {Coordinate(72, 127), DIR_SW},
    {Coordinate(80, 113), DIR_SW},
    {Coordinate(88, 100), DIR_SW},
    {Coordinate(96, 85), DIR_SW},

    {Coordinate(0, 0), DIR_SW}
};


/**
 *  Drive back into refinery.
 */
ShipLocomotionClass::TrackType const ShipLocomotionClass::Track12[] = { // CONFIRMED
    {Coordinate(96,-171), DIR_SW},
    {Coordinate(88,-156), DIR_SW},
    {Coordinate(80,-143), DIR_SW},
    {Coordinate(72,-129), DIR_SW},
    {Coordinate(64,-115), DIR_SW},
    {Coordinate(56,-100), DIR_SW},
    {Coordinate(48,-85),  DIR_SW},
    {Coordinate(40,-71),  DIR_SW},
    {Coordinate(32,-56),  DIR_SW},
    {Coordinate(24,-42),  DIR_SW},
    {Coordinate(16,-27),  DIR_SW},
    {Coordinate(8,-13),   DIR_SW},

    {Coordinate(0,0), DIR_SW}
};


/**
 *  Drive out of weapon's factory.
 */
ShipLocomotionClass::TrackType const ShipLocomotionClass::Track13[] = { // CONFIRMED
    {Coordinate(-670, -68), DIR_S},
    {Coordinate(-660, -67), DIR_S},
    {Coordinate(-650, -66), DIR_S},
    {Coordinate(-639, -65), DIR_S},
    {Coordinate(-630, -64), DIR_S},
    {Coordinate(-620, -63), DIR_S},
    {Coordinate(-610, -62), DIR_S},
    {Coordinate(-600, -61), DIR_S},
    {Coordinate(-590, -60), DIR_S},
    {Coordinate(-580, -59), DIR_S},
    {Coordinate(-570, -58), DIR_S},
    {Coordinate(-560, -57), DIR_S},
    {Coordinate(-550, -56), DIR_S},
    {Coordinate(-540, -55), DIR_S},
    {Coordinate(-530, -54), DIR_S},
    {Coordinate(-520, -53), DIR_S},
    {Coordinate(-510, -52), DIR_S},
    {Coordinate(-500, -51), DIR_S},
    {Coordinate(-490, -50), DIR_S},
    {Coordinate(-480, -49), DIR_S},
    {Coordinate(-470, -48), DIR_S},
    {Coordinate(-460, -47), DIR_S},
    {Coordinate(-450, -46), DIR_S},
    {Coordinate(-440, -45), DIR_S},
    {Coordinate(-430, -44), DIR_S},
    {Coordinate(-420, -43), DIR_S},
    {Coordinate(-410, -42), DIR_S},
    {Coordinate(-400, -41), DIR_S},
    {Coordinate(-390, -40), DIR_S},
    {Coordinate(-380, -39), DIR_S},
    {Coordinate(-370, -38), DIR_S},
    {Coordinate(-360, -37), DIR_S},
    {Coordinate(-350, -36), DIR_S},
    {Coordinate(-340, -35), DIR_S},
    {Coordinate(-330, -34), DIR_S},
    {Coordinate(-320, -33), DIR_S},
    {Coordinate(-310, -32), DIR_S},
    {Coordinate(-300, -31), DIR_S},
    {Coordinate(-290, -30), DIR_S},
    {Coordinate(-280, -29), DIR_S},
    {Coordinate(-270, -28), DIR_S},
    {Coordinate(-260, -27), DIR_S},
    {Coordinate(-250, -26), DIR_S},
    {Coordinate(-240, -25), DIR_S},
    {Coordinate(-230, -24), DIR_S},
    {Coordinate(-220, -23), DIR_S},
    {Coordinate(-210, -22), DIR_S},
    {Coordinate(-200, -21), DIR_S},
    {Coordinate(-190, -20), DIR_S},
    {Coordinate(-180, -19), DIR_S},
    {Coordinate(-170, -18), DIR_S},
    {Coordinate(-160, -17), DIR_S},
    {Coordinate(-150, -16), DIR_S},
    {Coordinate(-140, -15), DIR_S},
    {Coordinate(-130, -14), DIR_S},
    {Coordinate(-120, -13), DIR_S},
    {Coordinate(-110, -12), DIR_S},
    {Coordinate(-100, -11), DIR_S},
    {Coordinate(-90, -10),  DIR_S},
    {Coordinate(-80, -9),   DIR_S},
    {Coordinate(-70, -8),   DIR_S},
    {Coordinate(-60, -7),   DIR_S},
    {Coordinate(-50, -6),   DIR_S},
    {Coordinate(-40, -5),   DIR_S},
    {Coordinate(-30, -4),   DIR_S},
    {Coordinate(-20, -3),   DIR_S},
    {Coordinate(-10, -2),   DIR_S},
    {Coordinate(0, -1),     DIR_S},

    {Coordinate(0,0), DIR_S}
};


/**
 *  There are a limited basic number of tracks that a vehicle can follow. These
 *  are they. Each track can be interpreted differently but this is controlled
 *  by the TrackControl structure elaborated elsewhere.
 */
ShipLocomotionClass::RawTrackType const ShipLocomotionClass::RawTracks[13] = {  // CONFIRMED
    {Track1,  -1,  0,  -1},
    {Track2,  -1,  0,  -1},
    {Track3,  37,  12, 22},
    {Track4,  26,  11, 19},
    {Track5,  45,  15, 31},
    {Track6,  44,  16, 27},
    {Track7,  -1,  0,  -1},
    {Track8,  -1,  0,  -1},
    {Track9,  -1,  0,  -1},
    {Track10, -1,  0,  -1},
    {Track11, -1,  0,  -1},
    {Track12, -1,  0,  -1},
    {Track13, -1,  0,  -1}
};
#endif


/**
 *  Smooth turning control table. Given two directions in a path list, this
 *  table determines which track to use and what modifying operations need
 *  be performed on the track data.
 */
ShipLocomotionClass::TurnTrackType const ShipLocomotionClass::TrackControl[67] = {
    {1,  0,  0, 0, DIR_N,   F_},                                                        // 0-0
    {3,  7,  0, 0, DIR_NE,  F_D},                                                       // 0-1 (raw chart)
    {4,  9,  0, 0, DIR_E,   F_D},                                                       // 0-2 (raw chart)
    {0,  0,  0, 0, DIR_SE,  F_},                                                        // 0-3 !
    {0,  0,  0, 0, DIR_S,   F_},                                                        // 0-4 !
    {0,  0,  0, 0, DIR_SW,  F_},                                                        // 0-5 !
    {4,  9,  0, 0, DIR_W,   (ShipLocomotionClass::TrackControlType)(F_X|F_D)},          // 0-6
    {3,  7,  0, 0, DIR_NW,  (ShipLocomotionClass::TrackControlType)(F_X|F_D)},          // 0-7
    {6,  8,  0, 0, DIR_N,   (ShipLocomotionClass::TrackControlType)(F_T|F_X|F_Y|F_D)},  // 1-0
    {2,  0,  0, 0, DIR_NE,  F_},                                                        // 1-1 (raw chart)
    {6,  8,  0, 0, DIR_E,   F_D},                                                       // 1-2 (raw chart)
    {5,  10, 0, 0, DIR_SE,  F_D},                                                       // 1-3 (raw chart)
    {0,  0,  0, 0, DIR_S,   F_},                                                        // 1-4 !
    {0,  0,  0, 0, DIR_SW,  F_},                                                        // 1-5 !
    {0,  0,  0, 0, DIR_W,   F_},                                                        // 1-6 !
    {5,  10, 0, 0, DIR_NW,  (ShipLocomotionClass::TrackControlType)(F_T|F_X|F_Y|F_D)},  // 1-7
    {4,  9,  0, 0, DIR_N,   (ShipLocomotionClass::TrackControlType)(F_T|F_X|F_Y|F_D)},  // 2-0
    {3,  7,  0, 0, DIR_NE,  (ShipLocomotionClass::TrackControlType)(F_T|F_X|F_Y|F_D)},  // 2-1
    {1,  0,  0, 0, DIR_E,   (ShipLocomotionClass::TrackControlType)(F_T|F_X)},          // 2-2
    {3,  7,  0, 0, DIR_SE,  (ShipLocomotionClass::TrackControlType)(F_T|F_X|F_D)},      // 2-3
    {4,  9,  0, 0, DIR_S,   (ShipLocomotionClass::TrackControlType)(F_T|F_X|F_D)},      // 2-4
    {0,  0,  0, 0, DIR_SW,  F_},                                                        // 2-5 !
    {0,  0,  0, 0, DIR_W,   F_},                                                        // 2-6 !
    {0,  0,  0, 0, DIR_NW,  F_},                                                        // 2-7 !
    {0,  0,  0, 0, DIR_N,   F_},                                                        // 3-0 !
    {5,  10, 0, 0, DIR_NE,  (ShipLocomotionClass::TrackControlType)(F_Y|F_D)},          // 3-1
    {6,  8,  0, 0, DIR_E,   (ShipLocomotionClass::TrackControlType)(F_Y|F_D)},          // 3-2
    {2,  0,  0, 0, DIR_SE,  F_Y},                                                       // 3-3
    {6,  8,  0, 0, DIR_S,   (ShipLocomotionClass::TrackControlType)(F_T|F_X|F_D)},      // 3-4
    {5,  10, 0, 0, DIR_SW,  (ShipLocomotionClass::TrackControlType)(F_T|F_X|F_D)},      // 3-5
    {0,  0,  0, 0, DIR_W,   F_},                                                        // 3-6 !
    {0,  0,  0, 0, DIR_NW,  F_},                                                        // 3-7 !
    {0,  0,  0, 0, DIR_N,   F_},                                                        // 4-0 !
    {0,  0,  0, 0, DIR_NE,  F_},                                                        // 4-1 !
    {4,  9,  0, 0, DIR_E,   (ShipLocomotionClass::TrackControlType)(F_Y|F_D)},          // 4-2
    {3,  7,  0, 0, DIR_SE,  (ShipLocomotionClass::TrackControlType)(F_Y|F_D)},          // 4-3
    {1,  0,  0, 0, DIR_S,   F_Y},                                                       // 4-4
    {3,  7,  0, 0, DIR_SW,  (ShipLocomotionClass::TrackControlType)(F_X|F_Y|F_D)},      // 4-5
    {4,  9,  0, 0, DIR_W,   (ShipLocomotionClass::TrackControlType)(F_X|F_Y|F_D)},      // 4-6
    {0,  0,  0, 0, DIR_NW,  F_},                                                        // 4-7 !
    {0,  0,  0, 0, DIR_N,   F_},                                                        // 5-0 !
    {0,  0,  0, 0, DIR_NE,  F_},                                                        // 5-1 !
    {0,  0,  0, 0, DIR_E,   F_},                                                        // 5-2 !
    {5,  10, 0, 0, DIR_SE,  (ShipLocomotionClass::TrackControlType)(F_T|F_D)},          // 5-3
    {6,  8,  0, 0, DIR_S,   (ShipLocomotionClass::TrackControlType)(F_T|F_D)},          // 5-4
    {2,  0,  0, 0, DIR_SW,  F_T},                                                       // 5-5
    {6,  8,  0, 0, DIR_W,   (ShipLocomotionClass::TrackControlType)(F_X|F_Y|F_D)},      // 5-6
    {5,  10, 0, 0, DIR_NW,  (ShipLocomotionClass::TrackControlType)(F_X|F_Y|F_D)},      // 5-7
    {4,  9,  0, 0, DIR_N,   (ShipLocomotionClass::TrackControlType)(F_T|F_Y|F_D)},      // 6-0
    {0,  0,  0, 0, DIR_NE,  F_},                                                        // 6-1 !
    {0,  0,  0, 0, DIR_E,   F_},                                                        // 6-2 !
    {0,  0,  0, 0, DIR_SE,  F_},                                                        // 6-3 !
    {4,  9,  0, 0, DIR_S,   (ShipLocomotionClass::TrackControlType)(F_T|F_D)},          // 6-4
    {3,  7,  0, 0, DIR_SW,  (ShipLocomotionClass::TrackControlType)(F_T|F_D)},          // 6-5
    {1,  0,  0, 0, DIR_W,   F_T},                                                       // 6-6
    {3,  7,  0, 0, DIR_NW,  (ShipLocomotionClass::TrackControlType)(F_T|F_Y|F_D)},      // 6-7
    {6,  8,  0, 0, DIR_N,   (ShipLocomotionClass::TrackControlType)(F_T|F_Y|F_D)},      // 7-0
    {5,  10, 0, 0, DIR_NE,  (ShipLocomotionClass::TrackControlType)(F_T|F_Y|F_D)},      // 7-1
    {0,  0,  0, 0, DIR_E,   F_},                                                        // 7-2 !
    {0,  0,  0, 0, DIR_SE,  F_},                                                        // 7-3 !
    {0,  0,  0, 0, DIR_S,   F_},                                                        // 7-4 !
    {5,  10, 0, 0, DIR_SW,  (ShipLocomotionClass::TrackControlType)(F_X|F_D)},          // 7-5
    {6,  8,  0, 0, DIR_W,   (ShipLocomotionClass::TrackControlType)(F_X|F_D)},          // 7-6
    {2,  0,  0, 0, DIR_NW,  F_X},                                                       // 7-7

    {11, 11, 0, 0, DIR_SW, F_},                                                         // Backup harvester into refinery.
    {12, 12, 0, 0, DIR_SW, F_},                                                         // Drive back into refinery.
    {13, 13, 0, 0, DIR_SW, F_}                                                          // Drive out of weapons factory.
};
