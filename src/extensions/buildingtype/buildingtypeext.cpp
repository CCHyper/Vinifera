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
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Provides the map for all BuildingTypeClass extension instances.
 */
ExtensionMap<BuildingTypeClass, BuildingTypeClassExtension> BuildingTypeClassExtensions;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
BuildingTypeClassExtension::BuildingTypeClassExtension(BuildingTypeClass *this_ptr) :
    Extension(this_ptr),
    GateUpSound(VOC_NONE),
    GateDownSound(VOC_NONE),
    ProduceCashStartup(0),
    ProduceCashAmount(0),
    ProduceCashDelay(0),
    ProduceCashBudget(0),
    IsStartupCashOneTime(false),
    IsResetBudgetOnCapture(false),
    IsEligibleForAllyBuilding(false),
    FactoryExitCell(-1,-1)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("BuildingTypeClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
BuildingTypeClassExtension::BuildingTypeClassExtension(const NoInitClass &noinit) :
    Extension(noinit)
{
    IsInitialized = false;
}


/**
 *  Class deconstructor.
 *  
 *  @author: CCHyper
 */
BuildingTypeClassExtension::~BuildingTypeClassExtension()
{
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension deconstructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("BuildingTypeClassExtension deconstructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = false;

    delete [] ThisPtr->OccupyList;
    delete [] ThisPtr->ExitList;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT BuildingTypeClassExtension::Load(IStream *pStm)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    HRESULT hr = Extension::Load(pStm);
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
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

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
int BuildingTypeClassExtension::Size_Of() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void BuildingTypeClassExtension::Detach(TARGET target, bool all)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Detach - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void BuildingTypeClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Compute_CRC - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    crc(IsEligibleForAllyBuilding);
}


/**
 *  Fetches the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
bool BuildingTypeClassExtension::Read_INI(CCINIClass &ini)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("BuildingTypeClassExtension::Read_INI - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    EXT_DEBUG_WARNING("BuildingTypeClassExtension::Read_INI - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    char buffer[16] = { '\0' };
    const char *ini_name = ThisPtr->Name();
    const char *graphic_name = ThisPtr->Graphic_Name();

    if (!ini.Is_Present(ini_name)) {
        return false;
    }

    //if (!ArtINI.Is_Present(graphic_name)) {
    //    return false;
    //}

    GateUpSound = ini.Get_VocType(ini_name, "GateUpSound", GateUpSound);
    GateDownSound = ini.Get_VocType(ini_name, "GateDownSound", GateDownSound);

    ProduceCashStartup = ini.Get_Int(ini_name, "ProduceCashStartup", ProduceCashStartup);
    ProduceCashAmount = ini.Get_Int(ini_name, "ProduceCashAmount", ProduceCashAmount);
    ProduceCashDelay = ini.Get_Int(ini_name, "ProduceCashDelay", ProduceCashDelay);
    ProduceCashBudget = ini.Get_Int(ini_name, "ProduceCashBudget", ProduceCashBudget);
    IsStartupCashOneTime = ini.Get_Int(ini_name, "ProduceCashStartupOneTime", IsStartupCashOneTime);
    IsResetBudgetOnCapture = ini.Get_Bool(ini_name, "ProduceCashResetOnCapture", IsResetBudgetOnCapture);

    IsEligibleForAllyBuilding = ini.Get_Bool(ini_name, "EligibleForAllyBuilding",
                                                    ThisPtr->IsConstructionYard ? true : IsEligibleForAllyBuilding);
    
    if (ArtINI.Get_String(graphic_name, "Foundation", buffer, sizeof(buffer)) > 0 && !strcmpi(buffer, "custom")) {
        Read_Foundation(ini, graphic_name);
    }

    if (ini.Get_String(ini_name, "Foundation", buffer, sizeof(buffer)) > 0 && !strcmpi(buffer, "custom")) {
        Read_Foundation(ini, ini_name);
    }

    return true;
}


/**
 *  @author: CCHyper
 * 
 *  Expected ini format;
 *    Foundation=custom
 *    FoundationSize=4,4
 *    FoundationOccupyLength=16
 *    FoundationExitLength=20
 *    FoundationCell0=0,0
 *    FoundationCell1=1,0
 *    FoundationCell2=2,0
 *    FoundationCell3=2,0
 *    FoundationCell4=0,1
 *    FoundationCell5=1,1
 *    etc.
 * 
 *  Optional;
 *   FoundationFactoryExitCell=3,1
 */
bool BuildingTypeClassExtension::Read_Foundation(CCINIClass &ini, const char *section)
{
    if (ini.Is_Present(section, "FoundationSize")) {
        return false;
    }

    TPoint2D<int> size = ini.Get_Point(section, "FoundationSize", TPoint2D<int>(0,0));
    int occupy_length = ini.Get_Int(section, "FoundationOccupyLength", 0);
    int exit_length = ini.Get_Int(section, "FoundationExitLength", 0);

    if ((!size.X || !size.Y) || !occupy_length || !exit_length) {
        return false;
    }

    FactoryExitCell = ini.Get_Cell(section, "FoundationFactoryExitCell", FactoryExitCell);

    /**
     *  By default, war factories are hardcoded to use the 11th exit cell.
     */
    if (ThisPtr->ToBuild == RTTI_UNITTYPE && exit_length < 10) {
        DEBUG_WARNING("Factory \"%s\" with RTTI_UNITTYPE can not have a custom exit list of less than 10!", ThisPtr->Name());
        return false;
    }

    /**
     *  Invalidate normal building size if we have a custom one set.
     */
    ThisPtr->Size = BSIZE_NONE;
    delete ThisPtr->OccupyList;
    delete ThisPtr->ExitList;

    ThisPtr->OccupyList = new Cell [occupy_length+1];
    ASSERT(ThisPtr->OccupyList != nullptr);

    ThisPtr->ExitList = new Cell [exit_length+1];
    ASSERT(ThisPtr->ExitList != nullptr);

    char buffer[32];
    for (int i = 0; i < occupy_length; ++i) {
        std:snprintf(buffer, sizeof(buffer), "FoundationOccupyCell%d", i);
        ThisPtr->OccupyList[i] = ini.Get_Cell(section, buffer, Cell(0,0));
    }
    for (int i = 0; i < exit_length; ++i) {
        std:snprintf(buffer, sizeof(buffer), "FoundationExitCell%d", i);
        ThisPtr->ExitList[i] = ini.Get_Cell(section, buffer, Cell(0,0));
    }

    /**
     *  Terminate the lists.
     */
    ThisPtr->OccupyList[occupy_length] = Cell(REFRESH_EOL, REFRESH_EOL);
    ThisPtr->ExitList[exit_length] = Cell(REFRESH_EOL, REFRESH_EOL);

#ifndef NDEBUG
    DEBUG_INFO("\"%s\" - CustomOccupy:", ThisPtr->Name());
    for (int i = 0; i <= occupy_length; ++i) {
        DEBUG_INFO("  X:%d Y:%d", ThisPtr->OccupyList[i].X,  ThisPtr->OccupyList[i].Y);
    }
    DEBUG_INFO("\"%s\" - CustomExit:", ThisPtr->Name());
    for (int i = 0; i <= exit_length; ++i) {
        DEBUG_INFO("  X:%d Y:%d", ThisPtr->ExitList[i].X,  ThisPtr->ExitList[i].Y);
    }
#endif

#ifdef FOUNDATION_SORT_LISTS
    auto cell_comp_func = [](const Cell &a, const Cell &b)
    {
        if (a.Y != b.Y) {
            return a.Y < b.Y;
        }
        return a.X < b.X;
    };

    /**
     *  Sort the lists.
     */
    std::qsort(ThisPtr->OccupyList, occupy_length, sizeof(Cell), cell_comp_func);
    std::qsort(ThisPtr->OccupyList, occupy_length, sizeof(Cell), cell_comp_func);
#endif

    return true;
}
