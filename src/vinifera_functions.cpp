/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_FUNCTIONS.CPP
 *
 *  @authors       CCHyper
 *
 *  @brief         General functions.
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
#include "vinifera_functions.h"
#include "vinifera_globals.h"
#include "debughandler.h"
#include <string>


/**
 *  Parses the command line parameters.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Parse_Command_Line(int argc, char *argv[])
{
	if (argc > 1) {
		DEBUG_INFO("Parsing command line arguments...\n");
	}

	/**
	 *  Iterate over all command line params.
	 */
	for (int index = 1; index < argc; index++) {

		char arg_string[512];

		char *src = argv[index];
		char *dest = arg_string; 
		for (int i= 0; i < std::strlen(argv[index]); ++i) {
			if (*src == '\"') {
				src++;
			} else {
				*dest++ = *src++;
			}
		}
		*dest++ = '\0';

		char *string = arg_string; // Pointer to current argument.
		strupr(string);

		/**
		 *  Add all new command line params here.
		 */

		/**
		 *  Mod developer mode.
		 */
		if (stricmp(string, "-DEVELOPER") == 0) {
			DEBUG_INFO("  - Developer mode enabled.\n");
			Vinifera_DeveloperMode = true;
			continue;
		}

	}

	if (argc > 1) {
		DEBUG_INFO("Finished parsing command line arguments.\n");
	}

	return true;
}


/**
 *  This function will get called on application startup, allowing you to
 *  perform any action that would effect the game initialisation process.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Startup()
{
	return true;
}
