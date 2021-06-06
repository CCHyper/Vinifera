/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MAPSEEDEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended MapSeedClass.
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
#include "mapseedext_hooks.h"
#include "tibsun_util.h"
#include "scenarioini.h"
#include "miscutil.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


#define TEXT_SAVE_RAND_MAP "Would you like to save the randomly generated map as a .MAP file?"


/**
 *  #issue-256
 * 
 *  Save random generated maps with the .MAP extension.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_MapSeedClass_Dialog_Proc_Skip_Generate_Patch)
{
	/**
	 *  This patch skips the generating without a preview (used by the internal map editor).
	 */
	JMP(0x0053A5FE);
}

DECLARE_PATCH(_MapSeedClass_Dialog_Proc_Skip_Editor_Check_Patch)
{
	/**
	 *  This patch skips the internal map editor check and allows writing to "RandMap.Map".
	 */
	JMP(0x0053A622);
}

DECLARE_PATCH(_MapSeedClass_Dialog_Proc_Skip_Preview_Check_Patch)
{
	/**
	 *  This patch skips the check for a previous preview.
	 */
	JMP(0x0053A60C);
}

DECLARE_PATCH(_MapSeedClass_Dialog_Proc_Filename_Patch)
{
#if 0
    /**
     *  If in developer mode, ask if the user wishes to save the generated random map.
     */
    if (!Vinifera_DeveloperMode) {
	    JMP(0x0053A644);
    }
#endif

    /**
     *  Ask the user if they wish to save this map as a .MAP file.
     */
    if (Simple_YesNo_WWMessageBox(TEXT_SAVE_RAND_MAP)) {

        static char buffer[128];

        /**
         *  Generate a unique filename with the current date and time.
         */
        static int day ;
        static int month;
        static int year;
        static int hour;
        static int min;
        static int sec;
        Get_Full_Time(day, month, year, hour, min, sec);
        std::snprintf(buffer, sizeof(buffer), "RAND_%02u-%02u-%04u_%02u-%02u-%02u.MAP", day, month, year, hour, min, sec);

        DEBUG_INFO("Saving random map...");

        Write_Scenario_INI(buffer, true);

        DEBUG_INFO(" COMPLETE!\n");

        DEBUG_INFO("Filename: %s\n", buffer);
    }

	JMP(0x0053A644);
}


/**
 *  Main function for patching the hooks.
 */
void MapSeedClassExtension_Hooks()
{
	Patch_Jump(0x0053A5E6, &_MapSeedClass_Dialog_Proc_Skip_Generate_Patch);
	Patch_Jump(0x0053A619, &_MapSeedClass_Dialog_Proc_Skip_Editor_Check_Patch);
	Patch_Jump(0x0053A638, &_MapSeedClass_Dialog_Proc_Filename_Patch);
	Patch_Jump(0x0053A5FE, &_MapSeedClass_Dialog_Proc_Skip_Preview_Check_Patch);
}
