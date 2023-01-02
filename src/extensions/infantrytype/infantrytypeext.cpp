/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          INFANTRYTYPEEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended InfantryTypeClass class.
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
#include "infantrytypeext.h"
#include "infantrytype.h"
#include "tibsun_globals.h"
#include "voc.h"
#include "ccini.h"
#include "wwcrc.h"
#include "extension.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
InfantryTypeClassExtension::InfantryTypeClassExtension(const InfantryTypeClass *this_ptr) :
    TechnoTypeClassExtension(this_ptr),
    IsMechanic(false),
    IsOmniHealer(false)
{
    //if (this_ptr) EXT_DEBUG_TRACE("InfantryTypeClassExtension::InfantryTypeClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    /**
     *  Create a new instance of do info.
     * 
     *  Moved from Read_INI().
     */
    //delete This()->DoControls;
    This()->DoControls = (::DoInfoStruct *)new InfantryTypeClassExtension::DoInfoStruct[NEW_DO_COUNT];
    ASSERT_FATAL(This()->DoControls != nullptr);

    InfantryTypeExtensions.Add(this);
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
InfantryTypeClassExtension::InfantryTypeClassExtension(const NoInitClass &noinit) :
    TechnoTypeClassExtension(noinit)
{
    //EXT_DEBUG_TRACE("InfantryTypeClassExtension::InfantryTypeClassExtension(NoInitClass) - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
InfantryTypeClassExtension::~InfantryTypeClassExtension()
{
    //EXT_DEBUG_TRACE("InfantryTypeClassExtension::~InfantryTypeClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    InfantryTypeExtensions.Delete(this);
}


/**
 *  Retrieves the class identifier (CLSID) of the object.
 *  
 *  @author: CCHyper
 */
HRESULT InfantryTypeClassExtension::GetClassID(CLSID *lpClassID)
{
    //EXT_DEBUG_TRACE("InfantryTypeClassExtension::GetClassID - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

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
HRESULT InfantryTypeClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("InfantryTypeClassExtension::Load - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = TechnoTypeClassExtension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) InfantryTypeClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT InfantryTypeClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("InfantryTypeClassExtension::Save - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = TechnoTypeClassExtension::Save(pStm, fClearDirty);
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
int InfantryTypeClassExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("InfantryTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void InfantryTypeClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("InfantryTypeClassExtension::Detach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void InfantryTypeClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("InfantryTypeClassExtension::Compute_CRC - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    crc(IsMechanic);
    crc(IsOmniHealer);
}


/**
 *  Fetches the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
bool InfantryTypeClassExtension::Read_INI(CCINIClass &ini)
{
    //EXT_DEBUG_TRACE("InfantryTypeClassExtension::Read_INI - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    if (!TechnoTypeClassExtension::Read_INI(ini)) {
        return false;
    }

    const char *ini_name = Name();

    IsMechanic = ini.Get_Bool(ini_name, "Mechanic", IsMechanic);
    IsOmniHealer = ini.Get_Bool(ini_name, "OmniHealer", IsOmniHealer);

    Read_Sequence_INI();

    return true;
}


/**
 *  Read the animation sequence from the ini database.
 *
 *  @author: CCHyper
 */
void InfantryTypeClassExtension::Read_Sequence_INI()
{
    //EXT_DEBUG_TRACE("InfantryTypeClassExtension::Read_Sequence_INI - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    char sequence_buf[64];
    const char *graphic_name = Graphic_Name();

    if (ArtINI.Get_String(graphic_name, "Sequence", sequence_buf, sizeof(sequence_buf)) > 0) {

        /**
         *  Iterate over all the DoType's, reading the animation control for each.
         */
        for (DoType do_type = DO_FIRST; do_type < NEW_DO_COUNT; ++do_type) {

            InfantryTypeClassExtension::DoInfoStruct &do_info = const_cast<InfantryTypeClassExtension::DoInfoStruct &>(Do_Controls(do_type));
            const char *do_name = nullptr;

            /**
             *  #issue-635
             * 
             *  Support for the new DoTypes. Each of the new animation types must
             *  be initialised to a fallback value set before reading them from the
             *  ini database. This is because they are implemented with the expectation
             *  that they are defined correctly.
             */
            switch (do_type) {
                case DO_PARADROP:
                    This()->DoControls[DO_PARADROP] = This()->DoControls[DO_STAND_READY];
                    do_name = "Paradrop";
                    break;

                /**
                 *  All original DoTypes.
                 */
                default:
                    do_name = SequenceName[do_type];
                    break;
            };

            char entry[64];
            char buffer[64];
            char *tok = nullptr;

            if (ArtINI.Get_String(sequence_buf, do_name, buffer, sizeof(buffer)) > 0) {

                /**
                 *  Read the animation controls for this DoType.
                 */

                char facing_buf[3] = { '\0' };
#if 0
                // Original code, but "finish_buf" contains garbage memory if "Finish" is not defined.
                std::sscanf(buffer, "%d,%d,%d,%s", &do_info.Frame, &do_info.Count, &do_info.Jump, &facing_buf);
#else

                /**
                 *  The starting frame number of this animation.
                 */
                tok = std::strtok(buffer, ",");
                ASSERT(tok != nullptr);
                do_info.Frame = std::strtoul(tok, nullptr, 10);

                /**
                 *  The number of frames of this animation. If this number
                 *  is zero then no animation is present.
                 */
                tok = std::strtok(nullptr, ",");
                ASSERT(tok != nullptr);
                do_info.Count = std::strtoul(tok, nullptr, 10);

                /**
                 *  The multiplier by the infantry facing to reach the facing
                 *  specific animation start.
                 */
                tok = std::strtok(nullptr, ",");
                ASSERT(tok != nullptr);
                int jump = std::strtoul(tok, nullptr, 10);

                /**
                 *  If the Jump number is zero, then there is no facing specific modifier.
                 */
                if (jump > 0) {

                    do_info.Jump = std::strtoul(tok, nullptr, 10);

                    /**
                     *  Fetch the facing modifier.
                     */
                    tok = std::strtok(nullptr, ",");
                    //ASSERT(tok != nullptr);           // Facing modifier is optional.
                    if (tok) {
                        std::strncpy(facing_buf, tok, std::strlen(tok));
                    }

                }
#endif

                /**
                 *  Fetch the facing modifier from the string representation.
                 * 
                 *  NOTE: Added string length check to make sure buffer is a valid string.
                 */
                if (std::strlen(facing_buf) > 0) {
                    if (!std::strcmp("N", facing_buf)) {
                        do_info.Finish = FACING_N;

                    } else if (!std::strcmp("NE", facing_buf)) {
                        do_info.Finish = FACING_NE;

                    } else if (!std::strcmp("E", facing_buf)) {
                        do_info.Finish = FACING_E;

                    } else if (!std::strcmp("SE", facing_buf)) {
                        do_info.Finish = FACING_S;

                    } else if (!std::strcmp("S", facing_buf)) {
                        do_info.Finish = FACING_S;

                    } else if (!std::strcmp("SW", facing_buf)) {
                        do_info.Finish = FACING_SW;

                    } else if (!std::strcmp("W", facing_buf)) {
                        do_info.Finish = FACING_W;

                    } else if (!std::strcmp("NW", facing_buf)) {
                        do_info.Finish = FACING_NE;

                    }
                }

            }

            /**
             *  #issue-635
             *
             *  x
             */
            std::snprintf(entry, sizeof(entry), "%sSounds", do_name);
            if (ArtINI.Get_String(sequence_buf, entry, buffer, sizeof(buffer)) > 0) {
                
                /**
                 *  The frame number the sound should play on.
                 */
                tok = std::strtok(buffer, ",:");
                ASSERT(tok != nullptr);

                int sound_index = 0;

                while (tok && sound_index < DO_SOUND_COUNT) {

                    /**
                     *  x
                     */
                    int frame = std::strtoul(tok, nullptr, 10);
                    ASSERT(frame > 0);

                    /**
                     *  x
                     */
                    tok = std::strtok(nullptr, ",:");
                    ASSERT(tok != nullptr);
                    VocType sound = VocClass::From_Name(tok);

                    /**
                     *  x
                     */
                    if (frame > 0 && sound != VOC_NONE) {
                        do_info.Sounds[sound_index].Frame = std::clamp<unsigned>(frame, 0, do_info.Count);
                        do_info.Sounds[sound_index].Sound = sound;
                    }

                    /**
                     *  Fetch the next sound entry.
                     */
                    tok = std::strtok(nullptr, ",:");

                    ++sound_index;
                }

            }

#ifndef NDEBUG
            for (int i = 0; i < DO_SOUND_COUNT; ++i) {
                DEV_DEBUG_INFO("[%d] %s Frame %d Sound %d\n", i, do_name, do_info.Sounds[0].Frame, do_info.Sounds[0].Sound);
            }
#endif

        }

    } else {
        DEBUG_WARNING("InfantryType \"%s\" does not have a valid Sequence!\n", Name());
    }
}
