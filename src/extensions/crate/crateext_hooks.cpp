/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CRATEEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended CrateClass.
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
#include "crateext_hooks.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "crate.h"
#include "rules.h"
#include "overlay.h"
#include "rulesext.h"
#include "iomap.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or destructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
class CrateClassExt final : public CrateClass
{
    public:
        static bool _Put_Crate(Cell &cell);
        static bool _Get_Crate(Cell &cell);
};


/**
 *  Generates crate overlay at cell specified.  
 * 
 *  @author: 10/14/1996 JLB - Red Alert source code.
 *           CCHyper - Adjustments for Tiberian Sun.
 */
bool CrateClassExt::_Put_Crate(Cell & cell)
{
    int old = ScenarioInit;
    ScenarioInit = 0;

    if (Map.In_Radar(cell)) {
        CellClass * cellptr = &Map[cell];

        while (cellptr->Overlay != OVERLAY_NONE && !cellptr->Is_Clear_To_Build(SPEED_FLOAT) && !cellptr->Is_Clear_To_Build(SPEED_FOOT)) {
            cell = Map.Pick_Random_Location();

            if (RulesExtension && Percent_Chance(100 * RulesExtension->WaterCrateChance)) {
                cell = Map.Nearby_Location(cell, SPEED_FLOAT);
            } else {
                cell = Map.Nearby_Location(cell, SPEED_TRACK);
            }
            cellptr = &Map[cell];
        }

        if (RulesExtension && cellptr->Is_Clear_To_Build(SPEED_FLOAT)) {
            new OverlayClass((OverlayTypeClass *)RulesExtension->WaterCrateImage, cell);
        } else {
            new OverlayClass(Rule->WoodCrateImg, cell);
        }
        ScenarioInit = old;
        return true;
    }

    ScenarioInit = old;
    return false;
}


/**
 *  #issue-x
 * 
 *  Adds a check for WaterCrateImage.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_CrateClass_Get_Crate_WaterCrate_Patch)
{
    GET_REGISTER_STATIC(OverlayTypeClass *, overlaytype, eax);

    if (overlaytype != Rule->WoodCrateImage && overlaytype != Rule->SteelCrateImage) {
        goto return_false;
    }

    if (RulesExtension) {
        if (RulesExtension->WaterCrateImage) {
            goto return_false;
        }
    }

crate_check_match:
    JMP(0x00470BBC);

return_false:
    JMP(0x00470CB1);
}


/**
 *  Main function for patching the hooks.
 */
void CrateClassExtension_Hooks()
{
    Patch_Jump(0x00470BA2, &_CrateClass_Get_Crate_WaterCrate_Patch);
}
