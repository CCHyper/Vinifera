/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          WEAPONTYPEEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended TeamTypeClass class.
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
#include "teamtypeext.h"
#include "teamtype.h"
#include "ccini.h"
#include "tibsun_globals.h"
#include "waypoint.h"
#include "scenario.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Provides the map for all TeamTypeClass extension instances.
 */
ExtensionMap<TeamTypeClass, TeamTypeClassExtension> TeamTypeClassExtensions;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
TeamTypeClassExtension::TeamTypeClassExtension(TeamTypeClass *this_ptr) :
    Extension(this_ptr),
    IsUseTransportOrigin(false),
    TransportOrigin(-1)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TeamTypeClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("TeamTypeClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
TeamTypeClassExtension::TeamTypeClassExtension(const NoInitClass &noinit) :
    Extension(noinit)
{
    IsInitialized = false;
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
TeamTypeClassExtension::~TeamTypeClassExtension()
{
    //EXT_DEBUG_TRACE("TeamTypeClassExtension destructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("TeamTypeClassExtension destructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = false;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT TeamTypeClassExtension::Load(IStream *pStm)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TeamTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    HRESULT hr = Extension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) TeamTypeClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT TeamTypeClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TeamTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

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
int TeamTypeClassExtension::Size_Of() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TeamTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void TeamTypeClassExtension::Detach(TARGET target, bool all)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TeamTypeClassExtension::Detach - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void TeamTypeClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TeamTypeClassExtension::Compute_CRC - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    crc(IsUseTransportOrigin);
    crc(TransportOrigin);
}


/**
 *  Fetches the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
bool TeamTypeClassExtension::Read_INI(CCINIClass &ini)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TeamTypeClassExtension::Read_INI - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    EXT_DEBUG_WARNING("TeamTypeClassExtension::Read_INI - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    char buffer[256];

    const char *ini_name = ThisPtr->Name();

    if (!ini.Is_Present(ini_name)) {
        return false;
    }

    ini.Get_Bool(ini_name, "UseTransportOrigin", IsUseTransportOrigin);

    if (ini.Get_String(ini_name, "TransportWaypoint", Waypoint_As_String(TransportOrigin), buffer, sizeof(buffer)) > 0 ) {
        TransportOrigin = Waypoint_From_String(buffer);
    }

    return true;
}


/**
 *  x 
 *  
 *  @author: CCHyper
 */
Cell TeamTypeClassExtension::Get_Transport_Origin() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TeamTypeClassExtension::Get_Transport_Origin - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    EXT_DEBUG_WARNING("TeamTypeClassExtension::Get_Transport_Origin - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    Cell origin(0,0);

    if (TransportOrigin != -1) {
        origin = Scen->Get_Waypoint_Location(TransportOrigin);
    }

    return origin;
}
