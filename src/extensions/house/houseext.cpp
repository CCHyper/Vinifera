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
#include "vinifera_globals.h"
#include "vinifera_defines.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "session.h"
#include "vessel.h"
#include "vesseltype.h"
#include "housetype.h"
#include "team.h"
#include "teamtype.h"
#include "rules.h"
#include "ccini.h"
#include "asserthandler.h"
#include "debughandler.h"
#include <algorithm>


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
    //EXT_DEBUG_TRACE("HouseClassExtension constructor - 0x%08X\n", (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("HouseClassExtension constructor - 0x%08X\n", (uintptr_t)(ThisPtr));

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
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
HouseClassExtension::~HouseClassExtension()
{
    //EXT_DEBUG_TRACE("HouseClassExtension destructor - 0x%08X\n", (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("HouseClassExtension destructor - 0x%08X\n", (uintptr_t)(ThisPtr));

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
    //EXT_DEBUG_TRACE("HouseClassExtension::Load - 0x%08X\n", (uintptr_t)(ThisPtr));

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
    //EXT_DEBUG_TRACE("HouseClassExtension::Save - 0x%08X\n", (uintptr_t)(ThisPtr));

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
    //EXT_DEBUG_TRACE("HouseClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(ThisPtr));

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
    //EXT_DEBUG_TRACE("HouseClassExtension::Detach - 0x%08X\n", (uintptr_t)(ThisPtr));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void HouseClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("HouseClassExtension::Compute_CRC - 0x%08X\n", (uintptr_t)(ThisPtr));
}


/**
 *  
 *  
 *  @author: CCHyper
 */
VesselTypeClass *HouseClassExtension::Get_First_Ownable(TypeList<VesselTypeClass *> &list) const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("HouseClassExtension::AI_Vessel - 0x%08X\n", (uintptr_t)(ThisPtr));

    return reinterpret_cast<VesselTypeClass *>(ThisPtr->Get_First_Ownable((TypeList<TechnoTypeClass *> &)list));
}


/***********************************************************************************************
 * HouseClass::AI_Unit -- Determines what unit to build next.                                  *
 *                                                                                             *
 *    This routine handles the general case of determining what units to build next.           *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the number of games frames to delay before calling this routine again.*
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   09/29/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int HouseClassExtension::AI_Vessel()
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("HouseClassExtension::AI_Vessel - 0x%08X\n", (uintptr_t)(ThisPtr));

    if (BuildVessel != VESSEL_NONE) {
        return TICKS_PER_SECOND;
    }

    /**
     *  #BUGFIX:
     * 
     *  100-unit bug!
     */
    //int counter[100];
    int counter[1000];
    std::memset(counter, 0, sizeof(counter));

    VesselType bestlist[sizeof(counter) * 2]; // #BUGFIX: See "counter" comment above.
    std::memset(counter, 0, sizeof(counter));

    /*
    **	Build a list of the maximum of each type we wish to produce. This will be
    **	twice the number required to fill all teams.
    */
    int index;
    for (index = 0; index < Teams.Count(); ++index) {
        TeamClass * tptr = Teams[index];
        if (tptr) {
            TeamTypeClass const * team = tptr->Class;
            int val = tptr->field_40;
            if (((team->IsReinforcable && !tptr->IsFullStrength)
            || (!tptr->IsForcedActive && !tptr->IsHasBeen && !tptr->JustAltered)) && team->House == ThisPtr) {
                DynamicVectorClass<const TechnoTypeClass *> typelist;
                tptr->func_627800(typelist);
                for (int subindex = 0; subindex < typelist.Count(); subindex++) {
                    if (const_cast<TechnoTypeClass *>(typelist[subindex])->What_Am_I() == RTTI_VESSELTYPE) {
                        const VesselTypeClass *vtypeptr = (const VesselTypeClass *)typelist[subindex];
                        
                        // TODO
                        VesselType v14 = bestlist[vtypeptr->Type];
                        ++counter[vtypeptr->Type];
                        int *v15 = &bestlist[vtypeptr->Type];
                        if (val < v14) {
                            *v15 = val;
                        }

                    }
                }
            }
        }
    }

    /*
    **	Team types that are flagged as prebuilt, will always try to produce enough
    **	to fill one team of this type regardless of whether there is a team active
    **	of that type.
    */
    //for (index = 0; index < TeamTypes.Count(); ++index) {
    //    TeamTypeClass const * team = TeamTypes[index];
    //    if (team) {
    //        if (team->House == ThisPtr->Class->House && team->IsPrebuilt && (!team->IsAutocreate || ThisPtr->IsAlerted)) {
    //            for (int subindex = 0; subindex < team->ClassCount; subindex++) {
    //                if (team->Members[subindex].Class->What_Am_I() == RTTI_VESSELTYPE) {
    //                    int subtype = ((VesselTypeClass const *)(team->Members[subindex].Class))->Type;
    //                    counter[subtype] = std::max(counter[subtype], team->Members[subindex].Quantity);
    //                }
    //            }
    //        }
    //    }
    //}

    /*
    **	Reduce the theoretical maximum by the actual number of objects currently
    **	in play.
    */
    for (int vindex = 0; vindex < Vessels.Count(); vindex++) {
        const VesselClass * vessel = Vessels[vindex];
        if (vessel != NULL && vessel->Is_Recruitable(ThisPtr) && counter[vessel->Class->Type] > 0) {
            counter[vessel->Class->Type]--;
        }
    }

    /*
    **	Pick to build the most needed object but don't consider those object that
    **	can't be built because of scenario restrictions or insufficient cash.
    */
    int bestval = -1;
    int bestcount = 0;
    VesselType v10 = VESSEL_NONE;
    int v27 = 0x7FFFFFFF;
    for (VesselType vtype = VESSEL_FIRST; vtype < VesselTypes.Count(); ++vtype) {
        if (counter[vtype] > 0 && ThisPtr->Can_Build(&VesselTypeClass::As_Reference(vtype), ThisPtr->Class->House) && VesselTypeClass::As_Reference(vtype).Cost_Of() <= ThisPtr->Available_Money()) {
            if (bestval == -1 || bestval < counter[vtype]) {
                bestval = counter[vtype];
                bestcount = 0;
            }
            bestlist[bestcount++] = vtype;
            if (v10 == VESSEL_NONE || bestlist[vtype] < v27) { // TODO: needs investigating.
                v10 = vtype;
                v27 = bestlist[vtype];
            }
        }
    }

    // TODO: Need to find out if this is an inline.
    if (Rule->FillEarliestTeamProbability[ThisPtr->Difficulty] * 0.01 > Random_Pick(0, 0x7FFFFFFE) * 4.656612877414201) {
        BuildVessel = v10;
        return TICKS_PER_SECOND;
    }

    /*
    **	The vessel type to build is now known. Fetch a pointer to the techno type class.
    */
    if (bestcount) {
        BuildVessel = bestlist[Random_Pick(0, bestcount-1)];
    }

    return TICKS_PER_SECOND;
}
