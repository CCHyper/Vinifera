/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          INFANTRYEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended InfantryClass class.
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
#include "infantryext.h"
#include "infantry.h"
#include "infantrytype.h"
#include "infantrytypeext.h"
#include "vinifera_defines.h"
#include "voc.h"
#include "wwcrc.h"
#include "extension.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
InfantryClassExtension::InfantryClassExtension(const InfantryClass *this_ptr) :
    FootClassExtension(this_ptr)
{
    //if (this_ptr) EXT_DEBUG_TRACE("InfantryClassExtension::InfantryClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    InfantryExtensions.Add(this);
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
InfantryClassExtension::InfantryClassExtension(const NoInitClass &noinit) :
    FootClassExtension(noinit)
{
    //EXT_DEBUG_TRACE("InfantryClassExtension::InfantryClassExtension(NoInitClass) - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
InfantryClassExtension::~InfantryClassExtension()
{
    //EXT_DEBUG_TRACE("InfantryClassExtension::~InfantryClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    InfantryExtensions.Delete(this);
}


/**
 *  Retrieves the class identifier (CLSID) of the object.
 *  
 *  @author: CCHyper
 */
HRESULT InfantryClassExtension::GetClassID(CLSID *lpClassID)
{
    //EXT_DEBUG_TRACE("InfantryClassExtension::GetClassID - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

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
HRESULT InfantryClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("InfantryClassExtension::Load - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = FootClassExtension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) InfantryClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT InfantryClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("InfantryClassExtension::Save - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = FootClassExtension::Save(pStm, fClearDirty);
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
int InfantryClassExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("InfantryClassExtension::Size_Of - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void InfantryClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("InfantryClassExtension::Detach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void InfantryClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("InfantryClassExtension::Compute_CRC - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void InfantryClassExtension::Doing_Sound_AI()
{
    //EXT_DEBUG_TRACE("InfantryClassExtension::Doing_Sound_AI - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    InfantryTypeClassExtension *inftypeext = Extension::Fetch<InfantryTypeClassExtension>(This()->Class);

    /**
     *  Has the stage animation timer just changed (did we switch to a new DoType)?
     */
    if (IsStageChanged) {

        const InfantryTypeClassExtension::DoInfoStruct &do_info = inftypeext->Do_Controls(This()->Doing);

        /**
         *  x
         */
        for (int index = 0; index < InfantryTypeClassExtension::DO_SOUND_COUNT; ++index) {

            /**
             *  x
             */
            if (do_info.Sounds[index].Frame == -1) continue;
            if (do_info.Sounds[index].Sound == VOC_NONE) continue;

            DEV_DEBUG_INFO("%s  Frame:%d Sound:%s.\n", SequenceName[This()->Doing], do_info.Sounds[index].Frame, VocClass::INI_Name_From(do_info.Sounds[index].Sound));

            /**
             *  x
             */
            //int frame_count = std::max<unsigned>(1, do_info.Count);
            if (This()->Fetch_Stage() % do_info.Count == do_info.Sounds[index].Frame) {

#ifndef NDEBUG
                DEV_DEBUG_INFO("Playing sound \"%s\" for DoType \"%s\".\n", VocClass::INI_Name_From(do_info.Sounds[index].Sound), SequenceName[This()->Doing]);
#endif

                /**
                 *  x
                 */
                Play_Sound_Effect(do_info.Sounds[index].Sound, This()->Coord);
            }

        }

    }

}
