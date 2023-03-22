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
    IsEligibleForAllyBuilding(false)
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

    const char *ini_name = Name();
    const char *graphic_name = Graphic_Name();

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

    /**
     *  The following lines are reloading various animation keys as they are
     *  incorrect loaded with "IniName" instead of "GraphicName". As a result
     *  if the building uses Image= with a string that does not match its actual
     *  name (i.e. PROC, the Refinery), then these keys are not loaded from
     *  the section specified on Image=.
     */
    This()->field_580[BANIM_SPECIAL_ONE].Location.X = ini.Get_Int(graphic_name, "SpecialAnimX", This()->field_580[BANIM_SPECIAL_ONE].Location.X);
    This()->field_580[BANIM_SPECIAL_ONE].Location.Y = ini.Get_Int(graphic_name, "SpecialAnimY", This()->field_580[BANIM_SPECIAL_ONE].Location.Y);
    This()->field_580[BANIM_SPECIAL_ONE].ZAdjust = ini.Get_Int(graphic_name, "SpecialAnimZAdjust", This()->field_580[BANIM_SPECIAL_ONE].ZAdjust);
    This()->field_580[BANIM_SPECIAL_ONE].YSort = ini.Get_Int(graphic_name, "SpecialAnimYSort", This()->field_580[BANIM_SPECIAL_ONE].YSort);
    This()->field_580[BANIM_SPECIAL_ONE].Powered = ini.Get_Bool(graphic_name, "SpecialAnimPowered", This()->field_580[BANIM_SPECIAL_ONE].Powered);
    This()->field_580[BANIM_SPECIAL_ONE].PoweredLight = ini.Get_Bool(graphic_name, "SpecialAnimPoweredLight", This()->field_580[BANIM_SPECIAL_ONE].PoweredLight);

    This()->field_580[BANIM_SPECIAL_TWO].Location.X = ini.Get_Int(graphic_name, "SpecialAnimTwoX", This()->field_580[BANIM_SPECIAL_TWO].Location.X);
    This()->field_580[BANIM_SPECIAL_TWO].Location.Y = ini.Get_Int(graphic_name, "SpecialAnimTwoY", This()->field_580[BANIM_SPECIAL_TWO].Location.Y);
    This()->field_580[BANIM_SPECIAL_TWO].ZAdjust = ini.Get_Int(graphic_name, "SpecialAnimTwoZAdjust", This()->field_580[BANIM_SPECIAL_TWO].ZAdjust);
    This()->field_580[BANIM_SPECIAL_TWO].YSort = ini.Get_Int(graphic_name, "SpecialAnimTwoYSort", This()->field_580[BANIM_SPECIAL_TWO].YSort);
    This()->field_580[BANIM_SPECIAL_TWO].Powered = ini.Get_Bool(graphic_name, "SpecialAnimTwoPowered", This()->field_580[BANIM_SPECIAL_TWO].Powered);
    This()->field_580[BANIM_SPECIAL_TWO].PoweredLight = ini.Get_Bool(graphic_name, "SpecialAnimTwoPoweredLight", This()->field_580[BANIM_SPECIAL_TWO].PoweredLight);

    This()->field_580[BANIM_SPECIAL_THREE].Location.X = ini.Get_Int(graphic_name, "SpecialAnimThreeX", This()->field_580[BANIM_SPECIAL_THREE].Location.X);
    This()->field_580[BANIM_SPECIAL_THREE].Location.Y = ini.Get_Int(graphic_name, "SpecialAnimThreeY", This()->field_580[BANIM_SPECIAL_THREE].Location.Y);
    This()->field_580[BANIM_SPECIAL_THREE].ZAdjust = ini.Get_Int(graphic_name, "SpecialAnimThreeZAdjust", This()->field_580[BANIM_SPECIAL_THREE].ZAdjust);
    This()->field_580[BANIM_SPECIAL_THREE].YSort = ini.Get_Int(graphic_name, "SpecialAnimThreeYSort", This()->field_580[BANIM_SPECIAL_THREE].YSort);
    This()->field_580[BANIM_SPECIAL_THREE].Powered = ini.Get_Bool(graphic_name, "SpecialAnimThreePowered", This()->field_580[BANIM_SPECIAL_THREE].Powered);
    This()->field_580[BANIM_SPECIAL_THREE].PoweredLight = ini.Get_Bool(graphic_name, "SpecialAnimThreePoweredLight", This()->field_580[BANIM_SPECIAL_THREE].PoweredLight);

    This()->field_580[BANIM_PRODUCTION].Location.X = ini.Get_Int(graphic_name, "ProductionAnimX", This()->field_580[BANIM_PRODUCTION].Location.X);
    This()->field_580[BANIM_PRODUCTION].Location.Y = ini.Get_Int(graphic_name, "ProductionAnimY", This()->field_580[BANIM_PRODUCTION].Location.Y);
    This()->field_580[BANIM_PRODUCTION].ZAdjust = ini.Get_Int(graphic_name, "ProductionAnimZAdjust", This()->field_580[BANIM_PRODUCTION].ZAdjust);
    This()->field_580[BANIM_PRODUCTION].YSort = ini.Get_Int(graphic_name, "ProductionAnimYSort", This()->field_580[BANIM_PRODUCTION].YSort);
    This()->field_580[BANIM_PRODUCTION].Powered = ini.Get_Bool(graphic_name, "ProductionAnimPowered", This()->field_580[BANIM_PRODUCTION].Powered);
    This()->field_580[BANIM_PRODUCTION].PoweredLight = ini.Get_Bool(graphic_name, "ProductionAnimPoweredLight", This()->field_580[BANIM_PRODUCTION].PoweredLight);

    This()->field_580[BANIM_PRE_PRODUCTION].Location.X = ini.Get_Int(graphic_name, "PreProductionAnimX", This()->field_580[BANIM_PRE_PRODUCTION].Location.X);
    This()->field_580[BANIM_PRE_PRODUCTION].Location.Y = ini.Get_Int(graphic_name, "PreProductionAnimY", This()->field_580[BANIM_PRE_PRODUCTION].Location.Y);
    This()->field_580[BANIM_PRE_PRODUCTION].ZAdjust = ini.Get_Int(graphic_name, "PreProductionAnimZAdjust", This()->field_580[BANIM_PRE_PRODUCTION].ZAdjust);
    This()->field_580[BANIM_PRE_PRODUCTION].YSort = ini.Get_Int(graphic_name, "PreProductionAnimYSort", This()->field_580[BANIM_PRE_PRODUCTION].YSort);
    This()->field_580[BANIM_PRE_PRODUCTION].Powered = ini.Get_Bool(graphic_name, "PreProductionAnimPowered", This()->field_580[BANIM_PRE_PRODUCTION].Powered);
    This()->field_580[BANIM_PRE_PRODUCTION].PoweredLight = ini.Get_Bool(graphic_name, "PreProductionAnimPoweredLight", This()->field_580[BANIM_PRE_PRODUCTION].PoweredLight);

    This()->field_580[BANIM_TURRET].Location.X = ini.Get_Int(graphic_name, "TurretAnimX", This()->field_580[BANIM_TURRET].Location.X);
    This()->field_580[BANIM_TURRET].Location.Y = ini.Get_Int(graphic_name, "TurretAnimY", This()->field_580[BANIM_TURRET].Location.Y);
    This()->field_580[BANIM_TURRET].ZAdjust = ini.Get_Int(graphic_name, "TurretAnimZAdjust", This()->field_580[BANIM_TURRET].ZAdjust);
    This()->field_580[BANIM_TURRET].YSort = ini.Get_Int(graphic_name, "TurretAnimYSort", This()->field_580[BANIM_TURRET].YSort);
    This()->field_580[BANIM_TURRET].Powered = ini.Get_Bool(graphic_name, "TurretAnimPowered", This()->field_580[BANIM_TURRET].Powered);
    This()->field_580[BANIM_TURRET].PoweredLight = ini.Get_Bool(graphic_name, "TurretPoweredLight", This()->field_580[BANIM_TURRET].PoweredLight);
    
    return true;
}
