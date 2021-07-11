/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VESSELTYPE_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the new VesselTypeClass.
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
#include "vesseltype_hooks.h"
#include "unittype.h"
#include "vesseltype.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


// TODO: 004114D7
// TODO: 0044BB52
// TODO: 0044C9B8
// TODO: 00492308
// TODO: 0044962D
// TODO: 004631C7
// TODO: 005B6DFD
// TODO: 005D485A
// TODO: 005D5836

// RTTI checks
// 0058815D
// 0042EDB4
// 00439D1C
// 004C1800
// 004CB798
// 00587DDC
// 0058853F
// 00616D1A
// 004CA14D


/**
 *  blah
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_One_Time_Intercept_Patch)
{
    UnitTypeClass::One_Time();

    VesselTypeClass::One_Time();

    JMP(0x004E486F);
}


/**
 *  Main function for patching the hooks.
 */
void VesselTypeClass_Hooks()
{
    Patch_Jump(0x004E486A, &_One_Time_Intercept_Patch);
}
