/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          HOUSEEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended HouseClass class.
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
#include "houseext.h"
#include "house.h"
#include "building.h"
#include "buildingtype.h"
#include "buildingtypeext.h"
#include "ionstorm.h"
#include "voc.h"
#include "ccini.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Provides the map for all HouseTypeClass extension instances.
 */
ExtensionMap<HouseClass, HouseClassExtension> HouseClassExtensions;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
HouseClassExtension::HouseClassExtension(HouseClass *this_ptr) :
    Extension(this_ptr)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("HouseClassExtension constructor - 0x%08X\n", (uintptr_t)(ThisPtr));
    //DEV_DEBUG_WARNING("HouseClassExtension constructor - 0x%08X\n", (uintptr_t)(ThisPtr));

    IsInitialized = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
HouseClassExtension::HouseClassExtension(const NoInitClass &noinit) :
    Extension(noinit)
{
    IsInitialized = false;
}


/**
 *  Class deconstructor.
 *  
 *  @author: CCHyper
 */
HouseClassExtension::~HouseClassExtension()
{
    //DEV_DEBUG_TRACE("HouseClassExtension deconstructor - 0x%08X\n", (uintptr_t)(ThisPtr));
    //DEV_DEBUG_WARNING("HouseClassExtension deconstructor - 0x%08X\n", (uintptr_t)(ThisPtr));

    IsInitialized = false;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT HouseClassExtension::Load(IStream *pStm)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("HouseClassExtension::Load - 0x%08X\n", (uintptr_t)(ThisPtr));

    HRESULT hr = Extension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) HouseClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT HouseClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("HouseClassExtension::Save - 0x%08X\n", (uintptr_t)(ThisPtr));

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
int HouseClassExtension::Size_Of() const
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("HouseClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(ThisPtr));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void HouseClassExtension::Detach(TARGET target, bool all)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("HouseClassExtension::Detach - 0x%08X\n", (uintptr_t)(ThisPtr));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void HouseClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("HouseClassExtension::Compute_CRC - 0x%08X\n", (uintptr_t)(ThisPtr));
}


// TODO: Move when working!
#include "iomap.h"
#include "session.h"
#include "tibsun_globals.h"
void Map_Reset_Shroud(MapClass *this_ptr, HouseClass *house)
{
    HouseClassExtension *houseext;
    houseext = HouseClassExtensions.find(house);
    if (!houseext) {
        return;
    }

    if (house) {
        houseext->IsMapClear = false;
    }

    if (house == PlayerPtr) {
        if (!house) {
            DEBUG_WARNING("We tried to reset the shroud without an owner!\n");
        }

    } else if (house) {
        return;
    }

    /**
     *  Reset the cell iterator.
     */
    Map.Iterator_Reset();

    /**
     *  Iterate over all the map cells and flag them as unrevealed.
     */
    for (CellClass *cell = Map.Iterator_Next_Cell(); cell != nullptr; cell = Map.Iterator_Next_Cell()) {
        // TODO: Whats going on here?
        cell->IsMapped = false;
        cell->IsVisible = false;
    }

    Map.All_To_Look();
    PlayerPtr->IsVisionary = false;
    this_ptr->Flag_To_Redraw(2);
}

void Map_Clear_Shroud(MapClass *this_ptr, HouseClass *house)
{
    HouseClassExtension *houseext;
    houseext = HouseClassExtensions.find(house);
    if (!houseext) {
        return;
    }

    if (house) {
        houseext->IsMapClear = true;
    }

    if (house == PlayerPtr) {
        if (!house) {
            DEBUG_WARNING("We tried to clear the shroud without an owner!\n");
        }

    } else if (house) {
        return;
    }

    bool v1 = true;
    if (Session.Type != GAME_IPX && Session.Type != GAME_INTERNET) {
        v1 = true;
    }

    if (!PlayerPtr->IsVisionary) {

        PlayerPtr->IsVisionary = true;

        /**
         *  Reset the cell iterator.
         */
        Map.Iterator_Reset();

        /**
         *  Iterate over all the map cells and flag them as revealed.
         */
        for (CellClass *cell = Map.Iterator_Next_Cell(); cell != nullptr; cell = Map.Iterator_Next_Cell()) {
            // TODO: Whats going on here?
            cell->IsMapped = true;
            cell->IsVisible = true;
        }

        Map.All_To_Look();
        this_ptr->Flag_To_Redraw(2);
    }
}


/**
 *  Handles the state of the spy satellite for the house.
 *  
 *  @author: CCHyper
 */
void HouseClassExtension::Update_SpySats()
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("HouseClassExtension::Update_SpySats - 0x%08X\n", (uintptr_t)(ThisPtr));
    
    ThisPtr->field_56D = false;

    //if (ThisPtr != PlayerPtr) {
    //    return;
    //}

    // TODO: Add option to override ion storms?
    //if (IonStorm_Is_Active()) {
    //    return;
    //}

    if (ThisPtr->Drain > ThisPtr->Power) {
        return;
    }

    /**
     *  Iterate over all buildings in the game world.
     */
    for (int i = 0; i < Buildings.Count(); ++i) {

        BuildingClass *building = Buildings[i];
        if (!building) {
            continue;
        }

        BuildingTypeClassExtension *buildingtypeext;
        buildingtypeext = BuildingTypeClassExtensions.find(building->Class);
        if (!buildingtypeext) {
            continue;
        }

        /**
         *  Skip any buildings not active in the game world.
         */
        if (building->IsInLimbo || !building->IsDown) {
            continue;
        }

        /**
         *  
         */
        if (Session.Type != GAME_NORMAL) {

            if (ThisPtr != PlayerPtr) {
            }

        } else if (!ThisPtr->IsHuman && !ThisPtr->IsPlayerControl) {
            continue;
        }

        /**
         *  
         */
        if (!building->IsDiscoveredByPlayer && Session.Type == GAME_NORMAL) {
            continue;
        }

        /**
         *  If the building is deconstructing, skip it.
         */
        if (building->Mission == MISSION_DECONSTRUCTION || building->MissionQueue == MISSION_DECONSTRUCTION) {
            continue;
        }

        /**
         *  And finally skip any buildings that are not spy sats.
         */
        if (!buildingtypeext->IsSpySat) {
            continue;
        }

        /**
         *  
         */
        if (IsSpySatActive) {
            Map_Reset_Shroud(&Map, ThisPtr);
            IsSpySatActive = false;

            if (ThisPtr == PlayerPtr) {
                Sound_Effect(buildingtypeext->SpySatDeactivationSound);
            }

        } else {
            Map_Clear_Shroud(&Map, ThisPtr);
            IsSpySatActive = true;

            if (ThisPtr == PlayerPtr) {
                Sound_Effect(buildingtypeext->SpySatActivationSound);
            }
        }

        // Maybe?
        break;

    }
}
