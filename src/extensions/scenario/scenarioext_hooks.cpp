/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SCENARIOEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended ScenarioClass.
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
#include "scenarioext_hooks.h"
#include "scenarioext_functions.h"
#include "client_functions.h"
#include "client_globals.h"
#include "tibsun_globals.h"
#include "vinifera_util.h"
#include "language.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-
 * 
 *  
 * 
 *  @author: CCHyper
 */
static void Assign_Houses_Intercept()
{
    /**
     *  Is the client mode enabled?
     */
    if (Client::IsActive) {
        if (!Client::Assign_Houses()) {
            DEBUG_WARNING("Client: Assign_Houses failed!\n");
            Vinifera_Do_WWMessageBox("Failed to assign houses to players!", Text_String(TXT_OK));
            Fatal("Client: Assign_Houses failed!\n");
        }
        return;
    }

    /**
     *  
     */
    Vinifera_Assign_Houses();
}


/**
 *  Main function for patching the hooks.
 */
void ScenarioClassExtension_Hooks()
{
    /**
     *  Hooks in the new Assign_Houses() function.
     * 
     *  @author: CCHyper
     */
    //Patch_Call(0x005E08E3, &Vinifera_Assign_Houses);
    Patch_Call(0x005E08E3, &Assign_Houses_Intercept);

    /**
     *  #issue-338
     * 
     *  Hooks in the new Create_Units() function.
     * 
     *  @author: CCHyper
     */
    Patch_Call(0x005DD320, &Vinifera_Create_Units);
}
