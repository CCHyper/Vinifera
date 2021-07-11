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
#include "vinifera_globals.h"
#include "tracker.h"
#include "tspp_assert.h"


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


const VesselTypeClass &VesselTypeClass::As_Reference(VesselType type)
{
    TSPP_ASSERT(type != VESSEL_NONE && type < VesselTypes.Count());
    return *VesselTypes[type];
}


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
