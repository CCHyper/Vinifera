/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BUILDINGEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended BuildingClass class.
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
#include "buildingext.h"
#include "building.h"
#include "buildingtype.h"
#include "buildingtypeext.h"
#include "weapontype.h"
#include "tibsun_inline.h"
#include "house.h"
#include "housetype.h"
#include "tacticalext.h"
#include "wwcrc.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Provides the map for all BuildingClass extension instances.
 */
ExtensionMap<BuildingClass, BuildingClassExtension> BuildingClassExtensions;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
BuildingClassExtension::BuildingClassExtension(BuildingClass *this_ptr) :
    Extension(this_ptr),

    /**
     *  #issue-26
     * 
     *  Members for the Produce Cash logic.
     */
    ProduceCashTimer(),
    CurrentProduceCashBudget(-1),
    IsCaptureOneTimeCashGiven(false),
    IsBudgetDepleted(false)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("BuildingClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
BuildingClassExtension::BuildingClassExtension(const NoInitClass &noinit) :
    Extension(noinit)
{
    IsInitialized = false;
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
BuildingClassExtension::~BuildingClassExtension()
{
    //EXT_DEBUG_TRACE("BuildingClassExtension destructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("BuildingClassExtension destructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = false;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT BuildingClassExtension::Load(IStream *pStm)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    HRESULT hr = Extension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) BuildingClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT BuildingClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

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
int BuildingClassExtension::Size_Of() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void BuildingClassExtension::Detach(TARGET target, bool all)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingClassExtension::Detach - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void BuildingClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingClassExtension::Compute_CRC - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    /**
     *  #issue-26
     * 
     *  Members for the Produce Cash logic.
     */
    crc(ProduceCashTimer());
}


/**
 *  x
 *  
 *  @author: CCHyper
 */
void BuildingClassExtension::Draw_Radial_Indicator() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingClassExtension::Draw_Radial_Indicator - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    BuildingTypeClassExtension *buildingtypeext = BuildingTypeClassExtensions.find(ThisPtr->Class);

    if (!ThisPtr->House->Is_Player_Control()) {
        return;
    }

    int range = Get_Radial_Indicator_Range();
    if (!range) {
        return;
    }

    if (ThisPtr->Class->IsSensorArray || ThisPtr->Class->IsCloakGenerator) {

        if (!ThisPtr->IsPowerOn) {
            return;
        }

        Tactical_Draw_Radial(ThisPtr->Center_Coord(), ThisPtr->Class->RadialColor, float(range), true, true, false, true);
    }
}


/**
 *  x
 *  
 *  @author: CCHyper
 */
void BuildingClassExtension::Draw_Weapon_Range_Indicator() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingClassExtension::Draw_Weapon_Range_Indicator - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    BuildingTypeClassExtension *buildingtypeext = BuildingTypeClassExtensions.find(ThisPtr->Class);

    if (!ThisPtr->House->Is_Player_Control()) {
        return;
    }

    int range = Get_Weapon_Indicator_Range();
    if (!range) {
        return;
    }

    if (buildingtypeext && buildingtypeext->IsShowRangeIndicator) {
        Tactical_Draw_Radial(ThisPtr->Center_Coord(), ThisPtr->House->RemapColorRGB, float(range), false, true, false, true);
    }
}


/**
 *  x
 *  
 *  @author: CCHyper
 */
int BuildingClassExtension::Get_Radial_Indicator_Range() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingClassExtension::Get_Radial_Indicator_Range - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    int range = 0;

    if (ThisPtr->Class->IsSensorArray || ThisPtr->Class->IsCloakGenerator) {
        range = ThisPtr->Class->CloakRadiusInCells;
    }

    return range;
}


/**
 *  x
 *  
 *  @author: CCHyper
 */
int BuildingClassExtension::Get_Weapon_Indicator_Range() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingClassExtension::Get_Weapon_Indicator_Range - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    int range = 0;

    BuildingTypeClassExtension *buildingtypeext = BuildingTypeClassExtensions.find(ThisPtr->Class);
    if (buildingtypeext) {
        
        WeaponInfoStruct *winfo = ThisPtr->Get_Weapon(WEAPON_SLOT_PRIMARY);
        ASSERT(winfo != nullptr);
        if (winfo->Weapon && winfo->Weapon->Range > 0) {
            range = winfo->Weapon->Range / 256;
        }

    }

    return range;
}
