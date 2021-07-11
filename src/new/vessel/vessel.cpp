/*******************************************************************************
/*                     O P E N  S O U R C E  --  T S + +                      **
/*******************************************************************************
 *
 *  @project       TS++
 *
 *  @file          BUILDING.H
 *
 *  @authors       CCHyper, tomsons26
 *
 *  @brief         Unit game object class.
 *
 *  @license       TS++ is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 TS++ is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "vessel.h"
#include "vesseltype.h"
#include "tracker.h"
#include "tibsun_globals.h"
#include "vinifera_globals.h"
#include "vinifera_defines.h"
#include "house.h"
#include "team.h"


VesselClass::VesselClass(VesselTypeClass *classof, HouseClass *house) :
    FootClass(house)
{
}


VesselClass::VesselClass(const NoInitClass &noinit) :
    FootClass(noinit)
{
}


VesselClass::~VesselClass()
{
    if (GameActive && Class) {

        if (House->Can_Build(Class) == -1) {
            House->IsRecalcNeeded = true;
        }

        if (Team) {
            Team->Remove(this);
            Team = nullptr;
        }

        House->Tracking_Remove(this);

        while (Cargo.Is_Something_Attached()) {
            delete Cargo.Detach_Object();
        }

        Limbo();
    }

    Detach_This_From_All(this);

    Vessels.Delete(this);
    index_80E8C8.Remove(Fetch_ID());


    IsActive = false;
}


void VesselClass::Detach(TARGET target, bool all)
{
    FootClass::Detach(target, all);
}


RTTIType VesselClass::Kind_Of() const
{
    return RTTIType(RTTI_VESSEL);
}


int VesselClass::Size_Of(bool firestorm) const
{
    return sizeof(VesselClass);
}


void VesselClass::Compute_CRC(WWCRCEngine &crc) const
{
    FootClass::Compute_CRC(crc);
}


void VesselClass::AI()
{
    FootClass::AI();
}


void VesselClass::Read_INI(CCINIClass &ini)
{
}


void VesselClass::Write_INI(CCINIClass &ini)
{
}
