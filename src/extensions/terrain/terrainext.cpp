/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TERRAINEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended TerrainClass class.
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
#include "terrainext.h"
#include "terrain.h"
#include "terraintype.h"
#include "terraintypeext.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "iomap.h"
#include "cell.h"
#include "combat.h"
#include "rules.h"
#include "options.h"
#include "wwcrc.h"
#include "lightsource.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Provides the map for all TerrainClass extension instances.
 */
ExtensionMap<TerrainClass, TerrainClassExtension> TerrainClassExtensions;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
TerrainClassExtension::TerrainClassExtension(TerrainClass *this_ptr) :
    Extension(this_ptr),
    LightSource(nullptr),
    IsBlossoming(false),
    IsBarnacled(false),
    IsSporing(false)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TerrainClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("TerrainClassExtension constructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    IsInitialized = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
TerrainClassExtension::TerrainClassExtension(const NoInitClass &noinit) :
    Extension(noinit)
{
    IsInitialized = false;
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
TerrainClassExtension::~TerrainClassExtension()
{
    //EXT_DEBUG_TRACE("TerrainClassExtension destructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("TerrainClassExtension destructor - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    if (LightSource) {
        LightSource->Disable();
        delete LightSource;
        LightSource = nullptr;
    }

    IsInitialized = false;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT TerrainClassExtension::Load(IStream *pStm)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TerrainClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    HRESULT hr = Extension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) TerrainClassExtension(NoInitClass());

    LightSource = nullptr;
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT TerrainClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TerrainClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

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
int TerrainClassExtension::Size_Of() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TerrainClassExtension::Size_Of - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void TerrainClassExtension::Detach(TARGET target, bool all)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TerrainClassExtension::Detach - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void TerrainClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("TerrainClassExtension::Compute_CRC - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));
}


static bool T1()
{
    return (std::abs(Scen->RandomNumber()) % 1000000) * 0.000001 < 0.75;
}


static bool T2()
{
    return (std::abs(Scen->RandomNumber()) % 1000000) / (double)1000000 < 0.75;
}


static bool T3()
{
    return ((Scen->RandomNumber(0, INT_MAX-1) / (double)(INT_MAX-1))) < 0.75;
}


/**
 *  Process the terrain object AI.
 * 
 *  @author: CCHyper
 */
void TerrainClassExtension::AI()
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("TerrainClassExtension::AI - Name: %s (0x%08X)\n", ThisPtr->Name(), (uintptr_t)(ThisPtr));

    T1();
    T2();
    T3();

    ThisPtr->ObjectClass::AI();

    if (ThisPtr->Class->IsAnimated) {
        if (ThisPtr->Fetch_Rate() == 0) {
            if (Probability_Of(ThisPtr->Class->AnimationProbability)) {
                ThisPtr->Set_Rate(ThisPtr->Class->AnimationRate);
            }
        }
    }

    if (ThisPtr->Graphic_Logic()) {

        /**
         *  If the terrain object is in the process of crumbling, then when at the
         *  last stage of the crumbling animation, delete the terrain object.
         */
        if (ThisPtr->IsCrumbling && ThisPtr->Fetch_Stage() == ThisPtr->Class->Get_Image_Data()->Get_Frame_Count()-1) {
            ThisPtr->entry_E4();
            return;
        }

        if (ThisPtr->Class->IsSpawnsTiberium) {
            if (ThisPtr->Class->IsAnimated) {
                if (ThisPtr->Fetch_Stage() == ThisPtr->Class->Get_Image_Data()->Get_Frame_Count()/2) {
                    ThisPtr->Set_Rate(0);
                    Map[ThisPtr->Get_Coord()].Spread_Tiberium(true);
                }
            }
        }
    }

    if (ThisPtr->IsOnFire) {
        if (Percent_Chance(1)) {
            CellClass *mycell = &Map[ThisPtr->Get_Coord()];
            for (FacingType facing = FACING_FIRST; facing < FACING_COUNT; ++facing) {
                TerrainClass *terrain = mycell->Adjacent_Cell(facing).Cell_Terrain();
                if (terrain && !terrain->IsOnFire && Scen->RandomNumber(0, 0x7FFFFFFE) * 4.656612877414201e-10 < Rule->TreeFlammability) {
                    terrain->Catch_Fire();
                }
            }
        }
    }
}
