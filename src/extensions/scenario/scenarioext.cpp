/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SCENARIOEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended ScenarioClass class.
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
#include "scenarioext.h"
#include "tibsun_globals.h"
#include "tibsun_defines.h"
#include "noinit.h"
#include "swizzle.h"
#include "vinifera_saveload.h"
#include "asserthandler.h"
#include "debughandler.h"


ScenarioClassExtension *ScenarioExtension = nullptr;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
ScenarioClassExtension::ScenarioClassExtension(ScenarioClass *this_ptr)
{
    //EXT_DEBUG_TRACE("ScenarioClassExtension constructor - 0x%08X\n", (uintptr_t)(This()));

    /**
     *  This copies the behavior of the games ScenarioClass.
     */
    Init_Clear();
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
ScenarioClassExtension::ScenarioClassExtension(const NoInitClass &noinit)
{
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
ScenarioClassExtension::~ScenarioClassExtension()
{
    //EXT_DEBUG_TRACE("ScenarioClassExtension destructor - 0x%08X\n", (uintptr_t)(This()));
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *
 *  As ScenarioClassExtension is static data, we do not need to request
 *  pointer remap of "ThisPtr" after loading has finished.
 *  
 *  @author: CCHyper
 */
HRESULT ScenarioClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("ScenarioClassExtension::Load - 0x%08X\n", (uintptr_t)(This()));

    if (!pStm) {
        return E_POINTER;
    }

    /**
     *  Load the unique id for this class.
     */
    ULONG id = 0;
    HRESULT hr = pStm->Read(&id, sizeof(ULONG), nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    new (this) ScenarioClassExtension(NoInitClass());

    /**
     *  x
     */
    VINIFERA_SWIZZLE_REGISTER_POINTER(id, this, "this");

    /**
     *  Read this classes instance binary blob data.
     */
    hr = pStm->Read(this, Size_Of(), nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT ScenarioClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("ScenarioClassExtension::Save - 0x%08X\n", (uintptr_t)(This()));

    if (!pStm) {
        return E_POINTER;
    }

    /**
     *  x
     */
    ULONG id = (ULONG)this;
    HRESULT hr = pStm->Write(&id, sizeof(id), nullptr);
    if (FAILED(hr)) {
        return hr;
    }
    
    /**
     *  Write this class instance as a binary blob.
     */
    hr = pStm->Write(this, Size_Of(), nullptr);
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
int ScenarioClassExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("ScenarioClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(This()));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void ScenarioClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("ScenarioClassExtension::Detach - 0x%08X\n", (uintptr_t)(This()));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void ScenarioClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("ScenarioClassExtension::Compute_CRC - 0x%08X\n", (uintptr_t)(This()));
}


/**
 *  Initialises any values for this instance.
 *  
 *  @author: CCHyper
 */
void ScenarioClassExtension::Init_Clear()
{
    //EXT_DEBUG_TRACE("ScenarioClassExtension::Init - 0x%08X\n", (uintptr_t)(This()));
}
