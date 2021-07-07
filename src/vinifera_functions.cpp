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
#include "vinifera_newdel.h"
#include "cncnet4.h"
#include "cncnet4_globals.h"
#include "cncnet5_globals.h"
#include "client_functions.h"
#include "tibsun_globals.h"
#include "ccfile.h"
#include "cd.h"
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

    bool menu_skip = false;

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

        /**
         *  Skip the startup videos.
         */
        if (stricmp(string, "-NO_STARTUP_VIDEO") == 0) {
            DEBUG_INFO("  - Skipping startup videos.\n");
            Vinifera_SkipStartupMovies = true;
            continue;
        }

        /**
         *  Skip directly to Tiberian Sun menu.
         */
        if (stricmp(string, "-SKIP_TO_TS_MENU") == 0) {
            DEBUG_INFO("  - Skipping to Tiberian Sun menu.\n");
            Vinifera_SkipToTSMenu = true;
            menu_skip = true;
            continue;
        }

        /**
         *  Skip directly to Firestorm menu.
         */
        if (stricmp(string, "-SKIP_TO_FS_MENU") == 0) {
            DEBUG_INFO("  - Skipping to Firestorm menu.\n");
            Vinifera_SkipToFSMenu = true;
            menu_skip = true;
            continue;
        }

        /**
         *  Skip directly to a specific game mode dialog.
         */
        if (stricmp(string, "-SKIP_TO_LAN") == 0) {
            DEBUG_INFO("  - Skipping to LAN dialog.\n");
            Vinifera_SkipToLAN = true;
            menu_skip = true;
            continue;
        }

        if (stricmp(string, "-SKIP_TO_CAMPAIGN") == 0) {
            DEBUG_INFO("  - Skipping to campaign dialog.\n");
            Vinifera_SkipToCampaign = true;
            menu_skip = true;
            continue;
        }

        if (stricmp(string, "-SKIP_TO_SKIRMISH") == 0) {
            DEBUG_INFO("  - Skipping to skirmish dialog.\n");
            Vinifera_SkipToSkirmish = true;
            menu_skip = true;
            continue;
        }

        if (stricmp(string, "-SKIP_TO_INTERNET") == 0) {
            DEBUG_INFO("  - Skipping to internet dialog.\n");
            Vinifera_SkipToInternet = true;
            menu_skip = true;
            continue;
        }

        /**
         *  Exit the game after the dialog we skipped to has been canceled?
         */
        if (stricmp(string, "-EXIT_AFTER_SKIP") == 0) {
            DEBUG_INFO("  - Skipping to Firestorm menu.\n");
            Vinifera_ExitAfterSkip = true;
            menu_skip = true;
        }

        /**
         *  #issue-513
         * 
         *  Re-implements the file search path override logic of "-CD" from Red Alert.
         */
        if (std::strstr(string, "-CD")) {
            CCFileClass::Set_Search_Drives(&string[3]);
            CD::IsFilesLocal = true;
            continue;
        }

    }

	/**
	 *  Parse any command line parameters for the Client.
	 */
	Client::Parse_Command_Line(argc, argv);

    if (argc > 1) {
        DEBUG_INFO("Finished parsing command line arguments.\n");
    }

    /**
     *  Firestorm has priority over Tiberian Sun.
     */
    if (Vinifera_SkipToTSMenu && Vinifera_SkipToFSMenu) {
        Vinifera_SkipToTSMenu = false;
    }

    /**
     *  If any of the menu skip commands have been set then
     *  we also need to skip the startup movies.
     */
    if (menu_skip) {
        Vinifera_SkipStartupMovies = true;
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
    /**
     *  Initialise the CnCNet4 system.
     */
    if (!CnCNet4::Init()) {
        CnCNet4::IsEnabled = false;
        DEBUG_WARNING("Failed to initialise CnCNet4, continuing without CnCNet4 support!\n");
    }

    /**
     *  Disable CnCNet4 if CnCNet5 is active, they can not co-exist.
     */
    if (CnCNet4::IsEnabled && CnCNet5::IsActive) {
        CnCNet4::Shutdown();
        CnCNet4::IsEnabled = false;
    }

    return true;
}


/**
 *  This function will get called on application shutdown, allowing you to
 *  perform any memory cleanup or shutdown of new systems.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Shutdown()
{
	/**
	 *  Shutdown the client system.
	 */
	Client::Shutdown();

    DEV_DEBUG_INFO("Shutdown - New Count: %d, Delete Count: %d\n", Vinifera_New_Count, Vinifera_Delete_Count);

    return true;
}


/**
 *  This function will get called right before the games "Init_Game" fuction,
 *  allowing you to perform any action that would effect the game initialisation process.
 * 
 *  Related issues;
 *    issue-514: Adds various search paths for loading files locally.
 * 
 *  @author: CCHyper
 */
int Vinifera_Init_Game(int argc, char *argv[])
{
    DWORD rc;
    DynamicVectorClass<const char *> search_paths;

    char *new_path = new char [_MAX_PATH * 100];
    new_path[0] = '\0';

    /**
     *  If -CD has been defined, set the root directory as highest priority.
     */
    if (CD::IsFilesLocal) {
        search_paths.Add(".");
    }

    /**
     *  If the game was spawned by a external client, add the search folders
     *  where we expect the game settings to be.
     */
    search_paths.Add("Client");
    search_paths.Add("Resources");

    /**
     *  Add various local search drives to loading of files locally.
     */
    search_paths.Add("INI");
    search_paths.Add("MIX");
    search_paths.Add("SHP");
    search_paths.Add("AUD");
    search_paths.Add("PCX");
    search_paths.Add("MAPS");
    search_paths.Add("MAPS\\MULTIPLAYER");
    search_paths.Add("MAPS\\MISSION");
    search_paths.Add("MOVIES");
    search_paths.Add("MUSIC");
    search_paths.Add("SOUNDS");

    /**
     *  Load additional paths from the user environment vars.
     * 
     *  @note: Path must end in "\" otherwise this will fail.
     */
    char movies_var_buff[PATH_MAX];
    rc = GetEnvironmentVariable("TIBSUN_MOVIES", movies_var_buff, sizeof(movies_var_buff));
    if (rc && rc < sizeof(movies_var_buff)) {
        DEV_DEBUG_INFO("Found TIBSUN_MOVIES EnvVar: \"%s\".\n", movies_var_buff);
        search_paths.Add(movies_var_buff);
    }
    char music_var_buff[PATH_MAX];
    rc = GetEnvironmentVariable("TIBSUN_MUSIC", music_var_buff, sizeof(music_var_buff));
    if (rc && rc < sizeof(music_var_buff)) {
        DEV_DEBUG_INFO("Found TIBSUN_MUSIC EnvVar: \"%s\".\n", music_var_buff);
        search_paths.Add(music_var_buff);
    }
    char path_var_buff[PATH_MAX];
    rc = GetEnvironmentVariable("TIBSUN_FILES", path_var_buff, sizeof(path_var_buff));
    if (rc && rc < sizeof(path_var_buff)) {
        DEV_DEBUG_INFO("Found TIBSUN_FILES EnvVar: \"%s\".\n", path_var_buff);
        search_paths.Add(path_var_buff);
    }

    /**
     *  Current path (perhaps set set with -CD) should go next.
     */
    if (CCFileClass::RawPath[0] != '\0') {
        search_paths.Add(CCFileClass::RawPath);
    }

    /**
     *  Add search drives for the CD contents.
     */
    search_paths.Add("TS1");
    search_paths.Add("CD1");
    search_paths.Add("GDI");
    search_paths.Add("TS2");
    search_paths.Add("CD2");
    search_paths.Add("NOD");
    search_paths.Add("TS3");
    search_paths.Add("CD3");
    search_paths.Add("FIRESTORM");

    /**
     *  Build the search path string.
     */
    for (int i = 0; i < search_paths.Count(); ++i) {
        if (i != 0) std::strcat(new_path, ";");
        std::strcat(new_path, search_paths[i]);
    }

    /**
     *  Clear the current path ready to be set.
     */
    CCFileClass::Clear_Search_Drives();
    CCFileClass::Reset_Raw_Path();

    /**
     *  Set the new search drive path.
     */
    CCFileClass::Set_Search_Drives(new_path);

    delete [] new_path;

    DEBUG_INFO("SearchPath: %s\n", CCFileClass::RawPath);

    /**
     *  Initialise the Client system.
     */
    bool client = Client::Startup();
    if (!client) {
        DEBUG_WARNING("Failed to initialise Client, continuing without Client support!\n");
    }

    if (Client::IsActive) {

        /**
         *  The front-end client is most likely for a mod or redistribution of
         *  Tiberian Sun so all the files will be local. Enabling this will
         *  tell the game to skip the CD initialisation routines.
         */
        CD::IsFilesLocal = true;

        if (Client::IsRunFromClientOnly && !CDFileClass(Client::GameSettingsFilename).Is_Available()) {
            MessageBox(MainWindow, "This version of Tiberian Sun can only be launched via the Client!", "Tiberian Sun", MB_OK|MB_ICONEXCLAMATION);
            return 1; // Skips Init_Game error dialog.
        }
    }

    return EXIT_SUCCESS;
}
