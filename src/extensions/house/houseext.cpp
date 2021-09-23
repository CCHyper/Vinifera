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
#include "target.h"
#include "tibsun_globals.h"
#include "swizzle.h"
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
    Extension(this_ptr),
    CloningVats()
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
    Extension(noinit),
    CloningVats()
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

    CloningVats.Clear();

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

    /**
     *  Working count.
     */
    int count;

    /**
     *  Clear vectors to prepare for loading.
     */
    CloningVats.Clear();

    /**
     *  Read in vectors from the stream.
     */
    pStm->Read(&count, sizeof(count), nullptr);
    for (int i = 0; i < count; ++i) {
        BuildingClass *ptr;
        if (SUCCEEDED(pStm->Read(&ptr, sizeof(uintptr_t), nullptr))) {
            CloningVats.Add(ptr);
        }
    }

    /**
     *  Swizzle the pointers.
     */
    for (int i = 0; i < count; ++i) {
        SwizzleManager.Swizzle((void **)&CloningVats[i]);
    }
    
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

    /**
     *  Working count.
     */
    int count;

    /**
     *  Write vectors to the stream.
     */
    count = CloningVats.Count();
    pStm->Write(&count, sizeof(count), nullptr);
    for (int i = 0; i < count; ++i) {
        BuildingClass *ptr = CloningVats[i];
        pStm->Write(&ptr, sizeof(uintptr_t), nullptr);
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

    /**
     *  Remove this target from any trackers.
     */
    switch (target->What_Am_I()) {
        case RTTI_BUILDING:
        {
            BuildingClass *building = Target_As_Building(target);

            CloningVats.Delete(building);

            break;
        }

        default:
            break;
    }
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
