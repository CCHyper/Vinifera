/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SESSIONEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended SessionClass class.
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
#include "sessionext.h"
#include "tibsun_globals.h"
#include "noinit.h"
#include "options.h"
#include "ccini.h"
#include "rawfile.h"
#include "voc.h"
#include "rules.h"
#include "swizzle.h"
#include "vinifera_saveload.h"
#include "asserthandler.h"
#include "debughandler.h"


SessionClassExtension *SessionExtension = nullptr;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
SessionClassExtension::SessionClassExtension(SessionClass *this_ptr) :
    ExtOptions()
{
    //EXT_DEBUG_TRACE("SessionClassExtension constructor - 0x%08X\n", (uintptr_t)(This()));

   /**
     *  Initialises the default game options.
     */
    ExtOptions.IsAutoDeployMCV = false;
    ExtOptions.IsPrePlacedConYards = false;
    ExtOptions.IsBuildOffAlly = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
SessionClassExtension::SessionClassExtension(const NoInitClass &noinit)
{
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
SessionClassExtension::~SessionClassExtension()
{
    //EXT_DEBUG_TRACE("SessionClassExtension destructor - 0x%08X\n", (uintptr_t)(This()));
}


HRESULT SessionClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("SessionClassExtension::Load - 0x%08X\n", (uintptr_t)(This()));

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

    new (this) SessionClassExtension(NoInitClass());

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


HRESULT SessionClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("SessionClassExtension::Save - 0x%08X\n", (uintptr_t)(This()));

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
int SessionClassExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("SessionClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(This()));

    return sizeof(*this);
}


void SessionClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
}


/**
 *  Fetches the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
void SessionClassExtension::Read_MultiPlayer_Settings()
{
    //EXT_DEBUG_TRACE("SessionClassExtension::Read_MultiPlayer_Settings - 0x%08X\n", (uintptr_t)(This()));
}


/**
 *  Saves the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
void SessionClassExtension::Write_MultiPlayer_Settings()
{
    //EXT_DEBUG_TRACE("SessionClassExtension::Write_MultiPlayer_Settings - 0x%08X\n", (uintptr_t)(This()));
}
