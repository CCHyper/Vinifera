/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BUILDINGTYPEEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended BuildingTypeClass class.
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
#include "buildingtypeext.h"
#include "buildingtype.h"
#include "tibsun_defines.h"
#include "tibsun_globals.h"
#include "convert.h"
#include "ccfile.h"
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
BuildingTypeClassExtension::BuildingTypeClassExtension(const BuildingTypeClass *this_ptr) :
    TechnoTypeClassExtension(this_ptr),
    GateUpSound(VOC_NONE),
    GateDownSound(VOC_NONE),
    ProduceCashStartup(0),
    ProduceCashAmount(0),
    ProduceCashDelay(0),
    ProduceCashBudget(0),
    IsStartupCashOneTime(false),
    IsResetBudgetOnCapture(false),
    IsEligibleForAllyBuilding(false),
    PlacementCursorImage(nullptr),
    PlacementCursorStartFrame(0),
    PlacementCursorEndFrame(0),
    PlacementCursorRate(0),
    PlacementCursorDrawer(nullptr),
    PlacementCursorTranslucentLevelOverride(CURSOR_TRANSLUCENT_NOT_SET),
    PlacementCursorXYAdjust(0,0),
    IsPlacementCursorRemap(true)
{
    //if (this_ptr) EXT_DEBUG_TRACE("BuildingTypeClassExtension::BuildingTypeClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    BuildingTypeExtensions.Add(this);
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
BuildingTypeClassExtension::BuildingTypeClassExtension(const NoInitClass &noinit) :
    TechnoTypeClassExtension(noinit)
{
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::BuildingTypeClassExtension(NoInitClass) - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
BuildingTypeClassExtension::~BuildingTypeClassExtension()
{
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::~BuildingTypeClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    PlacementCursorImage = nullptr;

    delete PlacementCursorDrawer;
    PlacementCursorDrawer = nullptr;

    BuildingTypeExtensions.Delete(this);
}


/**
 *  Retrieves the class identifier (CLSID) of the object.
 *  
 *  @author: CCHyper
 */
HRESULT BuildingTypeClassExtension::GetClassID(CLSID *lpClassID)
{
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::GetClassID - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

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
HRESULT BuildingTypeClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Load - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = TechnoTypeClassExtension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) BuildingTypeClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT BuildingTypeClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Save - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

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
int BuildingTypeClassExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void BuildingTypeClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Detach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void BuildingTypeClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Compute_CRC - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    crc(IsEligibleForAllyBuilding);
}


/**
 *  Fetches the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
bool BuildingTypeClassExtension::Read_INI(CCINIClass &ini)
{
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Read_INI - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    if (!TechnoTypeClassExtension::Read_INI(ini)) {
        return false;
    }

    char buffer[256];

    const char *ini_name = Name();

    GateUpSound = ini.Get_VocType(ini_name, "GateUpSound", GateUpSound);
    GateDownSound = ini.Get_VocType(ini_name, "GateDownSound", GateDownSound);

    ProduceCashStartup = ini.Get_Int(ini_name, "ProduceCashStartup", ProduceCashStartup);
    ProduceCashAmount = ini.Get_Int(ini_name, "ProduceCashAmount", ProduceCashAmount);
    ProduceCashDelay = ini.Get_Int(ini_name, "ProduceCashDelay", ProduceCashDelay);
    ProduceCashBudget = ini.Get_Int(ini_name, "ProduceCashBudget", ProduceCashBudget);
    IsStartupCashOneTime = ini.Get_Int(ini_name, "ProduceCashStartupOneTime", IsStartupCashOneTime);
    IsResetBudgetOnCapture = ini.Get_Bool(ini_name, "ProduceCashResetOnCapture", IsResetBudgetOnCapture);

    IsEligibleForAllyBuilding = ini.Get_Bool(ini_name, "EligibleForAllyBuilding",
                                                    This()->IsConstructionYard ? true : IsEligibleForAllyBuilding);

    ini.Get_String(ini_name, "PlacementCursorImage", buffer, sizeof(buffer));
    if (buffer[0] != '\0') {
        std::strcat(buffer, ".SHP");
        PlacementCursorImage = (const ShapeFileStruct *)MFCC::Retrieve(buffer);
        ASSERT_PRINT(PlacementCursorImage != nullptr, "Building %s must have a valid image set for custom placement cursors!", This()->Name());
        
        PlacementCursorEndFrame = PlacementCursorImage->Get_Frame_Count()-1;

    } else if (This()->Image) {

        /**
         *  
         */
        PlacementCursorImage = This()->Image;
        PlacementCursorEndFrame = (PlacementCursorImage->Get_Frame_Count()/2)-1;
    }

    ini.Get_String(ini_name, "PlacementCursorPalette", buffer, sizeof(buffer));
    if (buffer[0] != '\0') {
        std::strcat(buffer, ".PAL");
        PlacementCursorDrawer = ConvertClass::Create_Drawer(buffer, &GamePalette, PrimarySurface);
        ASSERT_PRINT(PlacementCursorDrawer != nullptr, "Building %s must have a valid palette set for custom placement cursors!", This()->Name());
    }

    ini.Get_String(ini_name, "PlacementCursorGoodPalette", buffer, sizeof(buffer));
    if (buffer[0] != '\0') {
        std::strcat(buffer, ".PAL");
        PlacementCursorGoodDrawer = ConvertClass::Create_Drawer(buffer, &GamePalette, PrimarySurface);
        ASSERT_PRINT(PlacementCursorGoodDrawer != nullptr, "Building %s must have a valid palette set for custom placement cursors!", This()->Name());
    }

    ini.Get_String(ini_name, "PlacementCursorBadPalette", buffer, sizeof(buffer));
    if (buffer[0] != '\0') {
        std::strcat(buffer, ".PAL");
        PlacementCursorBadDrawer = ConvertClass::Create_Drawer(buffer, &GamePalette, PrimarySurface);
        ASSERT_PRINT(PlacementCursorBadDrawer != nullptr, "Building %s must have a valid palette set for custom placement cursors!", This()->Name());
    }

    PlacementCursorStartFrame = ini.Get_Int(ini_name, "PlacementCursorStartFrame", PlacementCursorStartFrame);
    PlacementCursorEndFrame = ini.Get_Int(ini_name, "PlacementCursorEndFrame", PlacementCursorEndFrame);
    PlacementCursorRate = ini.Get_Int(ini_name, "PlacementCursorRate", PlacementCursorRate);
    PlacementCursorTranslucentLevelOverride = (BPCTranslucentType)ini.Get_Int(ini_name, "PlacementCursorTLucentOverride", PlacementCursorTranslucentLevelOverride);
    IsPlacementCursorRemap = ini.Get_Bool(ini_name, "PlacementCursorRemap", IsPlacementCursorRemap);
    PlacementCursorXYAdjust = ini.Get_Point(ini_name, "PlacementCursorXYAdjust", PlacementCursorXYAdjust);

    return true;
}
