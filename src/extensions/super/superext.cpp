/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SUPEREXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended SuperClass class.
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
#include "superext.h"
#include "super.h"
#include "tibsun_inline.h"
#include "tibsun_globals.h"
#include "rules.h"
#include "infantry.h"
#include "infantrytype.h"
#include "iomap.h"
#include "wwcrc.h"
#include "extension.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
SuperClassExtension::SuperClassExtension(const SuperClass *this_ptr) :
    AbstractClassExtension(this_ptr),
    FlashTimeEnd(0),
    TimerFlashState(false),
    DropPodTypes()
{
    //if (this_ptr) EXT_DEBUG_TRACE("SuperClassExtension::SuperClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    SuperExtensions.Add(this);
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
SuperClassExtension::SuperClassExtension(const NoInitClass &noinit) :
    AbstractClassExtension(noinit)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::SuperClassExtension(NoInitClass) - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
SuperClassExtension::~SuperClassExtension()
{
    //EXT_DEBUG_TRACE("SuperClassExtension::~SuperClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    SuperExtensions.Delete(this);
}


/**
 *  Retrieves the class identifier (CLSID) of the object.
 *  
 *  @author: CCHyper
 */
HRESULT SuperClassExtension::GetClassID(CLSID *lpClassID)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::GetClassID - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

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
HRESULT SuperClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Load - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = AbstractClassExtension::Internal_Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) SuperClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT SuperClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Save - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = AbstractClassExtension::Internal_Save(pStm, fClearDirty);
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
int SuperClassExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Size_Of - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void SuperClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Detach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void SuperClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Compute_CRC - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    crc(DropPodTypes.Count());
}


/**
 *  x
 *  
 *  @author: CCHyper
 */
void SuperClassExtension::Place_Drop_Pods(Cell &cell)
{
    //EXT_DEBUG_TRACE("SuperClassExtension::Place_Drop_Pods - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    int count = Random_Pick(Rule->DropPodInfantryMinimum, Rule->DropPodInfantryMaximum);

    const InfantryTypeClass *infantry_one = InfantryTypeClass::As_Pointer("E1");
    const InfantryTypeClass *infantry_two = InfantryTypeClass::As_Pointer("E2");

    Cell place_cell = cell;

    int v4 = 3 * count;

    if (count > 0) {

        for (int i = count; count > 0; --count) {

            InfantryTypeClass *inftype = const_cast<InfantryTypeClass *>(Percent_Chance(50) ? infantry_two : infantry_one);

            InfantryClass *inf = reinterpret_cast<InfantryClass *>(inftype->Create_One_Of(This()->House));
            ASSERT(inf != nullptr);

            Cell cell = Map.Nearby_Location(place_cell, SPEED_FOOT, -1, MZONE_INFANTRY);
            CellClass *cellptr = &Map[cell];

            if (inf->Can_Enter_Cell(cellptr)) {

                inf->Veterancy.Set_Elite(true);
                inf->Piggyback_DropPod_Locomotor();


                if (inf->IsInLimbo) {
                    inf->Look();
                    inf->Assign_Mission(MISSION_GUARD_AREA);
                    inf->Commence();
                }



                if (inf->IsInLimbo) {
                    inf->entry_E4();
                }

                place_cell = Adjacent_Cell(place_cell, dir);

            } else {
                inf->entry_E4();
            }

        }

    }
}
