/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VESSELTYPE.CPP
 *
 *  @authors       CCHyper
 *
 *  @brief         Class for vessel types.
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
#include "vesseltype.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "vinifera_globals.h"
#include "vessel.h"
#include "house.h"
#include "housetype.h"
#include "rules.h"
#include "tracker.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  Retrieves the class identifier (CLSID) of the object.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP VesselTypeClass::GetClassID(CLSID *pClassID)
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
IFACEMETHODIMP VesselTypeClass::Load(IStream *pStm)
{
    HRESULT hr = TechnoTypeClass::Load(pStm);
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
IFACEMETHODIMP VesselTypeClass::Save(IStream *pStm, BOOL fClearDirty)
{
    HRESULT hr = TechnoTypeClass::Save(pStm, fClearDirty);
    if (SUCCEEDED(hr)) {
        // Insert any data to be saved here.
    }

    return hr;
}


/***********************************************************************************************
 * VesselTypeClass::VesselTypeClass -- Constructor for unit types.                             *
 *                                                                                             *
 *    This is the constructor for the vessel static data. Each vessels is assign a specific    *
 *    variation. This class elaborates what the variation actually is.                         *
 *                                                                                             *
 * INPUT:   bla bla bla... see below                                                           *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/14/1996 JLB : Created                                                                  *
 *=============================================================================================*/
VesselTypeClass::VesselTypeClass(const char *ini_name) :
    TechnoTypeClass(ini_name, SPEED_FLOAT)
{
    Create_ID();
    
    VesselTypes.Add(this);
}


VesselTypeClass::VesselTypeClass(const NoInitClass &noinit) :
    TechnoTypeClass(noinit)
{
}


/***********************************************************************************************
 * VesselClass::~VesselClass -- Destructor for vessel objects.                                 *
 *                                                                                             *
 *    The destructor will destroy the vessel and ensure that it is properly removed from the   *
 *    game engine.                                                                             *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/14/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
VesselTypeClass::~VesselTypeClass()
{
    Detach_This_From_All(this);

    VesselTypes.Delete(this);
}


RTTIType VesselTypeClass::Kind_Of() const
{
    return RTTIType(RTTI_VESSELTYPE);
}


int VesselTypeClass::Size_Of(bool firestorm) const
{
    return sizeof(VesselTypeClass);
}


int VesselTypeClass::entry_34() const
{
    return 0;
}


void VesselTypeClass::Compute_CRC(WWCRCEngine &crc) const
{
    TechnoTypeClass::Compute_CRC(crc);
}


int VesselTypeClass::Get_Heap_ID() const
{
    return int(Type);
}


/***********************************************************************************************
 * UnitTypeClass::Read_INI -- Fetch the unit type data from the INI database.                  *
 *                                                                                             *
 *    This routine will find the section in the INI database for this unit type object and     *
 *    then fill in the override values specified.                                              *
 *                                                                                             *
 * INPUT:   ini   -- Reference to the INI database that will be examined.                      *
 *                                                                                             *
 * OUTPUT:  bool; Was the section for this unit found in the database and the data extracted?  *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/19/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
bool VesselTypeClass::Read_INI(CCINIClass &ini)
{
    if (!TechnoTypeClass::Read_INI(ini)) {
        return false;
    }

    return true;
}


Coordinate VesselTypeClass::Coord_Fixup(Coordinate *coord) const
{
    return Coordinate();
}


/***********************************************************************************************
 * VesselTypeClass::Dimensions -- Fetches the pixel width and height of this vessel type.      *
 *                                                                                             *
 *    This routine is used to fetch the width and height of this vessel type. These dimensions *
 *    are not specific to any particular facing. Rather, they are only for the generic vessel  *
 *    size.                                                                                    *
 *                                                                                             *
 * INPUT:   width, height  -- Reference to the integers that are to be initialized with the    *
 *                            pixel width and height of this vessel type.                      *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/20/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
Point3D VesselTypeClass::Pixel_Dimensions() const
{
    return Point3D();
}


/***********************************************************************************************
 * VesselTypeClass::Dimensions -- Fetches the pixel width and height of this vessel type.      *
 *                                                                                             *
 *    This routine is used to fetch the width and height of this vessel type. These dimensions *
 *    are not specific to any particular facing. Rather, they are only for the generic vessel  *
 *    size.                                                                                    *
 *                                                                                             *
 * INPUT:   width, height  -- Reference to the integers that are to be initialized with the    *
 *                            pixel width and height of this vessel type.                      *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/20/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
Point3D VesselTypeClass::Lepton_Dimensions() const
{
    return Point3D();
}


/***********************************************************************************************
 * VesselTypeClass::Create_And_Place -- Creates a vessel and places it at location.            *
 *                                                                                             *
 *    This routine is used to create a vessel and then place it down upon the                  *
 *    map.                                                                                     *
 *                                                                                             *
 * INPUT:   cell  -- The location to place this vessel down upon.                              *
 *                                                                                             *
 *          house -- The house to assign this vessel's ownership to.                           *
 *                                                                                             *
 * OUTPUT:  bool; Was the vessel successfully created and placed down upon the map?            *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/20/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
bool VesselTypeClass::Create_And_Place(Cell &cell, HouseClass *house) const
{
    VesselClass *vessel = new VesselClass(this, house);
    if (vessel != nullptr) {
        return vessel->Unlimbo(Cell_Coord(cell), Random_Pick(DIR_N, DIR_MAX));
    }
    delete vessel;
    return false;
}


/***********************************************************************************************
 * VesselTypeClass::Create_One_Of -- Creates a vessel object that matches this vessel type.    *
 *                                                                                             *
 *    This routine is called when the type of vessel is known (by way of a VesselTypeClass)    *
 *    and a corresponding vessel object needs to be created.                                   *
 *                                                                                             *
 * INPUT:   house -- Pointer to the owner that this vessel will be assigned to.                *
 *                                                                                             *
 * OUTPUT:  Returns with a pointer to the vessel object created. If no vessel could be         *
 *          created, then NULL is returned.                                                    *
 *                                                                                             *
 * WARNINGS:   The vessel is created in a limbo state. It must first be placed down upon       *
 *             the map before it starts to function.                                           *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/20/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
ObjectClass *const VesselTypeClass::Create_One_Of(HouseClass *house) const
{
    return new VesselClass(this, house);
}


/***********************************************************************************************
 * UnitTypeClass::Repair_Step -- Determines the repair step rate.                              *
 *                                                                                             *
 *    This routine will determine how many strength points get healed for each "step". The     *
 *    cost to repair one step is determine from the Repair_Cost() function.                    *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of health points repaired for each "step".                 *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   04/03/1995 BWG : Created.                                                                 *
 *=============================================================================================*/
int VesselTypeClass::Repair_Step() const
{
    return Rule->RepairStep;
}


/***********************************************************************************************
 * VesselTypeClass::Turret_Adjust -- Adjust turret offset according to facing specified.       *
 *                                                                                             *
 *    This routine will determine the pixel adjustment necessary for a turret. The direction   *
 *    specified is what the vessel body is facing.                                             *
 *                                                                                             *
 * INPUT:   dir   -- The presumed direction of the body facing for the vessel.                 *
 *                                                                                             *
 *          x,y   -- The center pixel position for the vessel. These values should be          *
 *                   adjusted (they are references) to match the adjusted offset for the       *
 *                   turret.                                                                   *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/20/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
const TPoint3D<int> VesselTypeClass::Turret_Adjust(DirType dir, TPoint3D<int> &pos) const
{
    TPoint3D<int> newpos;

    return newpos;
}


const VesselTypeClass &VesselTypeClass::As_Reference(VesselType type)
{
    TSPP_ASSERT(type != VESSEL_NONE && type < VesselTypes.Count());
    return *VesselTypes[type];
}



/***********************************************************************************************
 * VesselTypeClass::As_Reference -- Converts a vessel type into a VesselTypeClass reference.   *
 *                                                                                             *
 *    This routine will fetch a reference to the vessel type that corresponds to the vessel    *
 *    type specified.                                                                          *
 *                                                                                             *
 * INPUT:   type  -- The vessel type number to convert.                                        *
 *                                                                                             *
 * OUTPUT:  Returns with a reference to the vessel type class that corresponds to the vessel   *
 *          type specified.                                                                    *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/20/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
const VesselTypeClass *VesselTypeClass::As_Pointer(VesselType type)
{
    TSPP_ASSERT(type != VESSEL_NONE && type < VesselTypes.Count());
    return type != VESSEL_NONE && type < VesselTypes.Count() ? VesselTypes[type] : nullptr;
}


const VesselTypeClass &VesselTypeClass::As_Reference(const char *name)
{
    return As_Reference(From_Name(name));
}


const VesselTypeClass *VesselTypeClass::As_Pointer(const char *name)
{
    return As_Pointer(From_Name(name));
}



/***********************************************************************************************
 * VesselTypeClass::From_Name -- Converts a name into a vessel type.                           *
 *                                                                                             *
 *    Use this routine to convert an ASCII version of a vessel type into the corresponding     *
 *    VesselType id value. Typical use of this would be to parse the INI file.                 *
 *                                                                                             *
 * INPUT:   name  -- Pointer to the ASCII name to be converted into a vessel type.             *
 *                                                                                             *
 * OUTPUT:  Returns with the vessel type number that matches the string specified.             *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/20/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
VesselType VesselTypeClass::From_Name(const char *name)
{
    TSPP_ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return VESSEL_NONE;
    }

    if (name != nullptr) {
        for (VesselType index = VESSEL_FIRST; index < VesselTypes.Count(); ++index) {
            if (!strcasecmp(As_Reference(index).Name(), name)) {
                return index;
            }
        }
    }

    return VESSEL_NONE;
}


const char *VesselTypeClass::Name_From(VesselType type)
{
    return (type != VESSEL_NONE && type < VesselTypes.Count() ? As_Reference(type).Name() : "<none>");
}


const VesselTypeClass *VesselTypeClass::Find_Or_Make(const char *name)
{
    TSPP_ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return nullptr;
    }

    for (VesselType index = VESSEL_FIRST; index < VesselTypes.Count(); ++index) {
        if (!strcasecmp(VesselTypes[index]->Name(), name)) {
            return VesselTypes[index];
        }
    }

    VesselTypeClass *ptr = new VesselTypeClass(name);
    TSPP_ASSERT(ptr != nullptr);
    return ptr;
}


/***********************************************************************************************
 * VesselTypeClass::One_Time -- Performs one time initialization for vessel types.             *
 *                                                                                             *
 *    This routine will load in the vessel shape data. It should be called only once at the    *
 *    beginning of the game.                                                                   *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Only call this once.                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/20/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void VesselTypeClass::One_Time()
{
}
