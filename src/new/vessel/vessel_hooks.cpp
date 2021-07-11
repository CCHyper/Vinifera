/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VESSEL_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the new VesselClass.
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
#include "vessel_hooks.h"
#include "tibsun_functions.h"
#include "unit.h"
#include "vessel.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


// TODO: 005B560A
// TODO: 005B5FAD
// TODO: 005D4882
// TODO: 005D57B0
// TODO: 0060A1BB
// TODO: 


/**
 *  blah
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Read_Scenario_INI_Intercept_Patch)
{
    GET_REGISTER_STATIC(CCINIClass *, ini, ebp);

    UnitClass::Read_INI(*ini);

    Call_Back();

    VesselClass::Read_INI(*ini);

    Call_Back();

    JMP(0x005DDB35);
}


/**
 *  blah
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Write_Scenario_INI_Intercept_Patch)
{
    LEA_STACK_STATIC(CCINIClass *, ini, esp, 0x0C);

    UnitClass::Write_INI(*ini);

    VesselClass::Write_INI(*ini);

    JMP(0x005DE14A);
}


/**
 *  Main function for patching the hooks.
 */
void VesselClass_Hooks()
{
    Patch_Jump(0x005DDB29, &_Read_Scenario_INI_Intercept_Patch);
    Patch_Jump(0x005DE141, &_Write_Scenario_INI_Intercept_Patch);
}
