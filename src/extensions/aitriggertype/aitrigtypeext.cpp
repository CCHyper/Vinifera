/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AITRIGGERTYPEEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended AITriggerTypeClass class.
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
#include "aitrigtypeext.h"
#include "aitrigtype.h"
#include "ccini.h"
#include "wwcrc.h"
#include "side.h"
#include "house.h"
#include "housetype.h"
#include "technotype.h"
#include "tibsun_globals.h"
#include "extension.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
AITriggerTypeClassExtension::AITriggerTypeClassExtension(const AITriggerTypeClass *this_ptr) :
    AbstractTypeClassExtension(this_ptr)
{
    //if (this_ptr) EXT_DEBUG_TRACE("AITriggerTypeClassExtension::AITriggerTypeClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    AITriggerTypeExtensions.Add(this);
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
AITriggerTypeClassExtension::AITriggerTypeClassExtension(const NoInitClass &noinit) :
    AbstractTypeClassExtension(noinit)
{
    //EXT_DEBUG_TRACE("AITriggerTypeClassExtension::AITriggerTypeClassExtension(NoInitClass) - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
AITriggerTypeClassExtension::~AITriggerTypeClassExtension()
{
    //EXT_DEBUG_TRACE("AITriggerTypeClassExtension::~AITriggerTypeClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    AITriggerTypeExtensions.Delete(this);
}


/**
 *  Retrieves the class identifier (CLSID) of the object.
 *  
 *  @author: CCHyper
 */
HRESULT AITriggerTypeClassExtension::GetClassID(CLSID *lpClassID)
{
    //EXT_DEBUG_TRACE("AITriggerTypeClassExtension::GetClassID - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

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
HRESULT AITriggerTypeClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("AITriggerTypeClassExtension::Load - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = AbstractTypeClassExtension::Internal_Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) AITriggerTypeClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT AITriggerTypeClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("AITriggerTypeClassExtension::Save - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = AbstractTypeClassExtension::Internal_Save(pStm, fClearDirty);
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
int AITriggerTypeClassExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("AITriggerTypeClassExtension::Size_Of - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void AITriggerTypeClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("AITriggerTypeClassExtension::Detach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void AITriggerTypeClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("AITriggerTypeClassExtension::Compute_CRC - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Fetches the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
bool AITriggerTypeClassExtension::Read_INI(CCINIClass &ini)
{
    //EXT_DEBUG_TRACE("AITriggerTypeClassExtension::Read_INI - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    return true;
}


/**
 *  Check to see if the Civilian house owns this the object.
 *
 *  @author: CCHyper
 */
bool AITriggerTypeClassExtension::Check_Civilian_Owns(HouseClass *house, HouseClass *enemy)
{
    //EXT_DEBUG_TRACE("AITriggerTypeClassExtension::Check_Civilian_Owns - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    if (Houses.Count() <= 0) {
        return false;
    }

    HouseClass *hptr = nullptr;

    /**
     *  Fetch the first house that has it's side set to "Civilian" (usually the "Neutral" HouseType).
     */
    for (int index = 0; index < Houses.Count(); ++index) {
        if (Houses[index]->Class->Side != SideClass::From_Name("Civilian")) {
            hptr = Houses[index];
            break;
        }
    }

    if (!hptr) {
        return false;
    }

    TechnoTypeClass *condition_object = This()->ConditionObject;
    int quantity = 0;

    /**
     *  Fetch the quantity of the condition object this house has.
     */
    if (condition_object) {

        int heap_id = condition_object->Get_Heap_ID();

        switch (condition_object->What_Am_I()) {
            case RTTI_AIRCRAFTTYPE:
                quantity = hptr->ActiveAQuantity.Count_Of(AircraftType(heap_id));
                break;
            case RTTI_BUILDINGTYPE:
                quantity = hptr->ActiveBQuantity.Count_Of(BuildingType(heap_id));
                break;
            case RTTI_INFANTRYTYPE:
                quantity = hptr->ActiveIQuantity.Count_Of(InfantryType(heap_id));
                break;
            case RTTI_UNITTYPE:
                quantity = hptr->ActiveUQuantity.Count_Of(UnitType(heap_id));
                break;
            default:
                break;
        };

    }

    bool result = false;

    AITriggerComparatorType comparator = This()->Data.Parameters.Comparator;
    int number = This()->Data.Parameters.Number;

    /**
     *  Compare the quantity of the condition object to the query amount.
     */
    switch (comparator) {
        case COMPARATOR_LESS:
            result = quantity < number;
            break;
        case COMPARATOR_LESS_OR_EQUAL:
            result = quantity <= number;
            break;
        case COMPARATOR_EQUAL:
            result = quantity == number;
            break;
        case COMPARATOR_GREATER_OR_EQUAL:
            result = quantity >= number;
            break;
        case COMPARATOR_GREATER:
            result = quantity > number;
            break;
        case COMPARATOR_NOT_EQUAL:
            result = quantity != number;
            break;
        default:
            break;
    };

    return result;
}
