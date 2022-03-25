/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SUPEREXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended SuperClass class.
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
#include "superext.h"
#include "super.h"
#include "supertype.h"
#include "supertypeext.h"
#include "target.h"
#include "techno.h"
#include "building.h"
#include "tibsun_inline.h"
#include "wwcrc.h"
#include "extension.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
SuperClassExtension::SuperClassExtension(const SuperClass *this_ptr) :
    AbstractClassExtension(this_ptr),
    FlashTimeEnd(0),
    TimerFlashState(false),
    LaunchSite(nullptr)
{
    //if (this_ptr) EXT_DEBUG_TRACE("SuperClassExtension::SuperClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    SuperExtensions.Add(this);
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
SuperClassExtension::SuperClassExtension(const NoInitClass &noinit) :
    AbstractClassExtension(noinit)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::SuperClassExtension(NoInitClass) - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
SuperClassExtension::~SuperClassExtension()
{
    //EXT_DEBUG_TRACE("SuperClassExtension::~SuperClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    SuperExtensions.Delete(this);
}


/**
 *  Retrieves the class identifier (CLSID) of the object.
 *  
 *  @author: CCHyper
 */
HRESULT SuperClassExtension::GetClassID(CLSID *lpClassID)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::GetClassID - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    if (lpClassID == nullptr) {
        return E_POINTER;
    }

    *lpClassID = __uuidof(this);

    return S_OK;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT SuperClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Load - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = AbstractClassExtension::Internal_Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) SuperClassExtension(NoInitClass());

    SwizzleManager.Swizzle((void **)LaunchSite);
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT SuperClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Save - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = AbstractClassExtension::Internal_Save(pStm, fClearDirty);
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
int SuperClassExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Size_Of - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void SuperClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Detach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void SuperClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Compute_CRC - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  
 *  
 *  @author: CCHyper
 */
bool SuperClassExtension::Targeting_Range_Check(TARGET target) const
{
    SuperWeaponTypeClass *supertype = ThisPtr->Class;
    SuperWeaponTypeClassExtension *supertypeext = SuperWeaponTypeClassExtensions.find(supertype);
    if (!supertypeext) {
        return false;
    }

    /**
     *  If this building is not subject to launch site checks, then it always
     *  passes the range checks.
     */
    if (!supertypeext->IsSubjectToLaunchSite) {
        return true;
    }
    
    /**
     *  Illegal targets are an error.
     */
    if (!Target_Legal(target)) {
        return false;
    }
    
    /**
     *  Invalid launch site is an error.
     */
    if (!LaunchSite) {
        return false;
    }

    Coordinate check_coord = target->Center_Coord();
    TechnoClass *target_techno = Is_Target_Techno(target) ? Target_As_Techno(target) : nullptr;

    LEPTON lepton_range_min = Cell_To_Lepton(supertypeext->TargetCellRangeMinimum);
    LEPTON lepton_range_max = Cell_To_Lepton(supertypeext->TargetCellRangeMaximum);

    /**
     *  Invalid range values set.
     */
    if ((lepton_range_min == 0 && lepton_range_max == 0) || lepton_range_min > lepton_range_max) {
        DEV_DEBUG_WARNING("Invalid range values for special \"%s\" (Min: %d, Max: %d)!\n", supertype->Name());
        return false;
    }

    /**
     *  Calculate if the distance between the launch site and the target is
     *  within the specified range.
     */
    bool in_range = Is_Distance_Within_Range(LaunchSite->Center_Coord(), check_coord, lepton_range_min, lepton_range_max);

    return in_range;
}
