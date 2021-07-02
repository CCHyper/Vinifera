/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TECHNOTYPEEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended TechnoTypeClass class.
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
#include "technotypeext.h"
#include "technotype.h"
#include "ccini.h"
#include "imagecollection.h"
#include "filepcx.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Provides the map for all TechnoTypeClass extension instances.
 */
ExtensionMap<TechnoTypeClass, TechnoTypeClassExtension> TechnoTypeClassExtensions;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
TechnoTypeClassExtension::TechnoTypeClassExtension(TechnoTypeClass *this_ptr) :
    Extension(this_ptr),

    CloakSound(VOC_NONE),
    UncloakSound(VOC_NONE),
    IsCameoDataPCX(false),
    CameoDataPCX(nullptr)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("TechnoTypeClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //DEV_DEBUG_WARNING("TechnoTypeClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
TechnoTypeClassExtension::TechnoTypeClassExtension(const NoInitClass &noinit) :
    Extension(noinit)
{
    IsInitialized = false;
}


/**
 *  Class deconstructor.
 *  
 *  @author: CCHyper
 */
TechnoTypeClassExtension::~TechnoTypeClassExtension()
{
    //DEV_DEBUG_TRACE("TechnoTypeClassExtension deconstructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //DEV_DEBUG_WARNING("TechnoTypeClassExtension deconstructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = false;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT TechnoTypeClassExtension::Load(IStream *pStm)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("TechnoTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    HRESULT hr = Extension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) TechnoTypeClassExtension(NoInitClass());

    /**
     *  We need to reload the "Cameo" key because TechnoTypeClass does
     *  not store the entry value. 
     */
    char cameo_buffer[32];
    ArtINI.Get_String(ThisPtr->IniName, "Cameo", cameo_buffer, sizeof(cameo_buffer));
    if (std::strlen(cameo_buffer) > 0) {

        char buff[32+4];
        std::snprintf(buff, sizeof(buff), "%s.PCX", cameo_buffer);

        /**
         *  
         */
        CCFileClass pcxfile(buff);
        if (pcxfile.Is_Available()) {

            /**
             *  Image collection required lowercase filename.
             */
            strlwr(buff);

            PCX_HEADER pcxhdr;
            pcxfile.Read(&pcxhdr, sizeof(pcxhdr));

            bool loaded = false;

            switch (pcxhdr.BitsPixelPlane) {
                case 8:
                    loaded = ImageCollection.Load_Paletted_PCX(buff);
                    break;

                case 16:
                    loaded = ImageCollection.Load_PCX(buff, 2);
                    break;

                default:
                    break;
            };

            if (loaded) {

                /**
                 *  
                 */
                CameoDataPCX = ImageCollection.Get_Image_Surface(buff);

                IsCameoDataPCX = (CameoDataPCX != nullptr);
            }
        }
    }
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT TechnoTypeClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("TechnoTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

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
int TechnoTypeClassExtension::Size_Of() const
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("TechnoTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void TechnoTypeClassExtension::Detach(TARGET target, bool all)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("TechnoTypeClassExtension::Detach - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void TechnoTypeClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("TechnoTypeClassExtension::Compute_CRC - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
}


/**
 *  Fetches the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
bool TechnoTypeClassExtension::Read_INI(CCINIClass &ini)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("TechnoTypeClassExtension::Read_INI - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //DEV_DEBUG_WARNING("TechnoTypeClassExtension::Read_INI - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    const char *ini_name = ThisPtr->Name();

    if (!ini.Is_Present(ini_name)) {
        return false;
    }
    
    CloakSound = ini.Get_VocType(ini_name, "CloakSound", CloakSound);
    UncloakSound = ini.Get_VocType(ini_name, "UncloakSound", UncloakSound);

    /**
     *  We need to reload the "Cameo" key because TechnoTypeClass does
     *  not store the entry value. 
     */
    char cameo_buffer[32];
    ArtINI.Get_String(ini_name, "Cameo", cameo_buffer, sizeof(cameo_buffer));
    if (std::strlen(cameo_buffer) > 0) {

        char buff[32+4];
        std::snprintf(buff, sizeof(buff), "%s.PCX", cameo_buffer);

        /**
         *  
         */
        CCFileClass pcxfile(buff);
        if (pcxfile.Is_Available()) {

            /**
             *  Image collection required lowercase filename.
             */
            strlwr(buff);

            PCX_HEADER pcxhdr;
            pcxfile.Read(&pcxhdr, sizeof(pcxhdr));

            bool loaded = false;

            switch (pcxhdr.BitsPixelPlane) {
                case 8:
                    loaded = ImageCollection.Load_Paletted_PCX(buff);
                    break;

                case 16:
                    loaded = ImageCollection.Load_PCX(buff, 2);
                    break;

                default:
                    break;
            };

            if (loaded) {

                /**
                 *  
                 */
                CameoDataPCX = ImageCollection.Get_Image_Surface(buff);

                IsCameoDataPCX = (CameoDataPCX != nullptr);
            }
        }
    }

    return true;
}
