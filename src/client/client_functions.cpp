/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CLIENT_FUNCTIONS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Various globals for the client system.
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
#include "client_functions.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "tibsun_functions.h"
#include "cncnet5_globals.h"
#include "cncnet5_wspudp.h"
#include "vinifera_globals.h"
#include "session.h"
#include "scenario.h"
#include "scenarioini.h"
#include "iomap.h"
#include "house.h"
#include "housetype.h"
#include "theme.h"
#include "addon.h"
#include "rules.h"
#include "campaign.h"
#include "colorscheme.h"
#include "wwmouse.h"
#include "wwkeyboard.h"
#include "queue.h"
#include "event.h"
#include "cctooltip.h"
#include "dsurface.h"
#include "rawfile.h"
#include "ccini.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <winsock2.h>
#include <ctime>


/**
 *  Enforce the 8 player limit?
 */
//#define ENFORCE_MAX_PLAYERS


#define TXT_CLIENT_RUN_ERROR            "Failed to launch a client game due to invalid settings!\n" \
                                        "\nThe game will now exit.\n"

#define TXT_CLIENT_INALIDSETTINGS       "Client: Invalid settings file!\n"
#define TXT_CLIENT_INALIDSCENARIO       "Client: Invalid scenario file!\n"
#define TXT_CLIENT_INALIDLAUNCH         "Client: Invalid launch mode!\n"
#define TXT_CLIENT_INALIDGAMEID         "Client: Invalid game id!\n"

#define CLIENT_RESOURCES_DIR            ".\\Resources\\"
#define CLIENT_SETTINGS_FILENAME        "ClientDefinitions.ini"


static void Client_Debug_Log_Settings()
{
#ifndef NDEBUG
    DEBUG_INFO("\n");

    DEBUG_INFO("Client: Dumping game settings...\n");

    DEBUG_INFO("  ScenarioName:         %s\n", Client::GameSettings.ScenarioName);
    DEBUG_INFO("  Description:          %s\n", Client::GameSettings.Description);

    DEBUG_INFO("  IsNormalGame:         %s\n", Client::GameSettings.IsNormalGame ? "true" : "false");
    DEBUG_INFO("  IsSkirmishGame:       %s\n", Client::GameSettings.IsSkirmishGame ? "true" : "false");
    DEBUG_INFO("  IsFirestormAddon:     %s\n", Client::GameSettings.IsFirestormAddon ? "true" : "false");

    DEBUG_INFO("  IsShortGame:          %s\n", Client::GameSettings.IsShortGame ? "true" : "false");
    DEBUG_INFO("  IsMultiEngineer:      %s\n", Client::GameSettings.IsMultiEngineer ? "true" : "false");
    DEBUG_INFO("  IsMCVRedeploy:        %s\n", Client::GameSettings.IsMCVRedeploy ? "true" : "false");
    DEBUG_INFO("  IsCrateGoodies:       %s\n", Client::GameSettings.IsCrateGoodies ? "true" : "false");
    DEBUG_INFO("  IsBridgeDestruction:  %s\n", Client::GameSettings.IsBridgeDestruction ? "true" : "false");
    DEBUG_INFO("  IsTiberiumGrows:      %s\n", Client::GameSettings.IsTiberiumGrows ? "true" : "false");
    DEBUG_INFO("  IsFogOfWar:           %s\n", Client::GameSettings.IsFogOfWar ? "true" : "false");
    DEBUG_INFO("  IsShroudRegrows:      %s\n", Client::GameSettings.IsShroudRegrows ? "true" : "false");

    DEBUG_INFO("  Seed:                 %08d\n", Client::GameSettings.Seed);

    DEBUG_INFO("\n");

    DEBUG_INFO("Client: Dumping player data...\n");
    for (int i = 0; i < Client::GameSettings.Players.Count(); ++i) {
        Client::PlayerSettingsType &player = *Client::GameSettings.Players[i];
        if (player.IsSlotActive) {
            DEBUG_INFO("  Multi%d (%d):\n", i+1, i);
            DEBUG_INFO("    PlayerName:   %s\n", player.PlayerName);
            if (Client::GameSettings.IsMultiplayerGame) {
                DEBUG_INFO("    IsHost:       %s\n", player.IsHost ? "true" : "false");
            }
            DEBUG_INFO("    IsLocal:      %s\n", player.IsLocal ? "true" : "false");
            DEBUG_INFO("    IsHuman:      %s\n", player.IsHuman ? "true" : "false");
        }
    }
    DEBUG_INFO("\n");

    DEBUG_INFO("Client: Dumping observer data...\n");
    for (int i = 0; i < Client::GameSettings.Observers.Count(); ++i) {
        Client::ObserverSettingsType &observer = *Client::GameSettings.Observers[i];
        if (observer.IsSlotActive) {
            DEBUG_INFO("  Observer%d (%d):\n", i+1, i);
            DEBUG_INFO("    Name:   %s\n", observer.Name);
            if (Client::GameSettings.IsMultiplayerGame) {
                DEBUG_INFO("    IsHost:       %s\n", observer.IsHost ? "true" : "false");
            }
            DEBUG_INFO("    IsLocal:      %s\n", observer.IsLocal ? "true" : "false");
        }
    }
    DEBUG_INFO("\n");
#endif
}


bool Client::Parse_Command_Line(int argc, char *argv[])
{
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
         *  Format: "-CLIENT=<settings>,<map>,<gameid>,<exit>,<skip>,<dev_mode>"
         */
        if (std::strstr(string, "-CLIENT")) {

            char *tmpstr;
            char *str = (char *)&string[std::strlen("-CLIENT")];

#if 0
            /**
             *  Fetch settings, scenario and exit mode.
             *  We do some error checking here as we don't want to launch the normal game
             *  if client-launch game actually failed. We show an error message instead.
             */
            char *settingsstr = std::strtok(str, ",");
            SettingsFile.Set_Name(settingsstr);
            if (!SettingsFile.Is_Available()) {
                DEBUG_ERROR(TXT_CLIENT_INALIDSETTINGS);
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal(TXT_CLIENT_INALIDSETTINGS);
            }
            SettingsFile.Open();
            SettingsINI.Load(SettingsFile, false);

            char *mapstr = std::strtok(nullptr, ",");
            ScenarioFile.Set_Name(mapstr);
            if (!ScenarioFile.Is_Available()) {
                DEBUG_ERROR(TXT_CLIENT_INALIDSCENARIO);
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal(TXT_CLIENT_INALIDSCENARIO);
            }
            ScenarioFile.Open();
            ScenarioINI.Load(ScenarioFile, false);

            char *modestr = std::strtok(nullptr, ",");
            if (!modestr) {
                DEBUG_ERROR(TXT_CLIENT_INALIDLAUNCH);
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal(TXT_CLIENT_INALIDLAUNCH);
            }
            ExitOnGameFinish = (std::strtol(modestr, nullptr, 10) == 1) ? true : false;

            char *idstr = std::strtok(nullptr, ",");
            if (!idstr) {
                DEBUG_ERROR(TXT_CLIENT_INALIDGAMEID);
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal(TXT_CLIENT_INALIDGAMEID);
            }
            GameID = std::strtol(idstr, nullptr, 10);

            /**
             *  Load the settings now before any other initialisation is done.
             */
            bool loaded = Read_Settings_INI(SettingsINI);
            if (!loaded) {
                DEBUG_ERROR("Client: Failed to read settings!\n");
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal("Client: Failed to read settings!\n");
            }
#endif

            tmpstr = std::strtok(str, ",");
            IsExitOnGameFinish = std::strtol(tmpstr, nullptr, 10) == 1 ? true : false;

            tmpstr = std::strtok(str, ",");
            IsSkipStartupMovies = std::strtol(tmpstr, nullptr, 10) == 1 ? true : false;

            tmpstr = std::strtok(str, ",");
            Vinifera_DeveloperMode = std::strtol(tmpstr, nullptr, 10) == 1 ? true : false;

            /**
             *  Settings and scenario file successfully loaded, set the system to active.
             */
            IsActive = true;

            DEBUG_INFO("Client: Starting game launched from the client.\n");
            //DEBUG_INFO("Client: %s %s %d %d\n", SettingsFile.File_Name(), ScenarioFile.File_Name(), ExitOnGameFinish, GameID);

            return true;

        }

    }

    return false;
}


bool Client::Read_Client_Settings(CCINIClass &ini)
{
    DEBUG_INFO("Client: Loading Client settings.\n");

    static char const * const GENERAL = "General";
    if (!ini.Is_Present(GENERAL)) {
        return false;
    }

    IsRunFromClientOnly = ini.Get_Bool(GENERAL, "RunFromClientOnly", IsRunFromClientOnly);
    IsExitOnGameFinish = ini.Get_Bool(GENERAL, "ExitOnGameFinish", IsExitOnGameFinish);
    IsSkipStartupMovies = ini.Get_Bool(GENERAL, "SkipStartupMovies", IsSkipStartupMovies);

    /**
     *  this allows Mod developers that might wish to enable the developer using an
     *  option in the front-end client.
     */
    Vinifera_DeveloperMode = ini.Get_Bool(GENERAL, "DeveloperMode", Vinifera_DeveloperMode);

    return true;
}


bool Client::Read_CnCNet_Settings(CCINIClass &ini)
{
    DEBUG_INFO("Client: Loading CnCNet settings.\n");

    static char const * const CNCNET = "CnCNet5";
    if (!ini.Is_Present(CNCNET)) {
        return false;
    }

    CnCNet5::TunnelInfo.ID = ini.Get_Int(CNCNET, "ID", -1);
    CnCNet5::TunnelInfo.IP = ini.Get_Int(CNCNET, "IP", -1);
    CnCNet5::TunnelInfo.Port = ini.Get_Int(CNCNET, "Port", -1);
    CnCNet5::TunnelInfo.PortHack = ini.Get_Bool(CNCNET, "PortHack", false);

    return true;
}


bool Client::Read_Game_Settings(CCINIClass &ini)
{
    DEBUG_INFO("Client: Loading game settings.\n");

    static char const * const SETTINGS = "GameSettings";
    if (!ini.Is_Present(SETTINGS)) {
        return false;
    }

    GameSettings.IsMultiplayerGame = ini.Get_Bool(SETTINGS, "MultiplayerGame", GameSettings.IsMultiplayerGame);
    GameSettings.IsNormalGame = ini.Get_Bool(SETTINGS, "NormalGame", GameSettings.IsNormalGame);
    GameSettings.IsSkirmishGame = ini.Get_Bool(SETTINGS, "SkirmishGame", GameSettings.IsSkirmishGame);

    GameSettings.IsFirestormAddon = ini.Get_Bool(SETTINGS, "Firestorm", GameSettings.IsFirestormAddon);

    /**
     *  Load the campaign type.
     */
    if (GameSettings.IsNormalGame) {
        GameSettings.Campaign = (CampaignType)ini.Get_Int(SETTINGS, "Campaign", GameSettings.Campaign);
    }

    /**
     *  Force the priority of game modes (only one mode can be active at once).
     * 
     *  1) IsMultiplayerGame
     *  2) IsSkirmishGame
     *  3) IsNormalGame
     * 
     */
    if (GameSettings.IsMultiplayerGame) {
        GameSettings.IsNormalGame = false;
        GameSettings.IsSkirmishGame = false;
        GameSettings.Campaign = CAMPAIGN_NONE;

    } else if (GameSettings.IsSkirmishGame) {
        GameSettings.IsMultiplayerGame = false;
        GameSettings.IsNormalGame = false;
        GameSettings.Campaign = CAMPAIGN_NONE;

    } else if (GameSettings.IsNormalGame) {
        GameSettings.IsMultiplayerGame = false;
        GameSettings.IsSkirmishGame = false;
    }

    /**
     *  Load multiplayer/skirmish options.
     */
    if (!GameSettings.IsNormalGame) {
        GameSettings.IsShortGame = ini.Get_Bool(SETTINGS, "ShortGame", GameSettings.IsShortGame);
        GameSettings.IsMultiEngineer = ini.Get_Bool(SETTINGS, "MultiEngineer", GameSettings.IsMultiEngineer);
        GameSettings.IsMCVRedeploy = ini.Get_Bool(SETTINGS, "MCVRedeploy", GameSettings.IsMCVRedeploy);
        GameSettings.IsCrateGoodies = ini.Get_Bool(SETTINGS, "CrateGoodies", GameSettings.IsCrateGoodies);
        GameSettings.IsBridgeDestruction = ini.Get_Bool(SETTINGS, "BridgeDestruction", GameSettings.IsBridgeDestruction);
        GameSettings.IsTiberiumGrows = ini.Get_Bool(SETTINGS, "TiberiumGrows", GameSettings.IsTiberiumGrows);
        GameSettings.IsFogOfWar = ini.Get_Bool(SETTINGS, "FogOfWar", GameSettings.IsFogOfWar);
        GameSettings.IsShroudRegrows = ini.Get_Bool(SETTINGS, "ShroudRegrows", GameSettings.IsShroudRegrows);
        GameSettings.IsHarvesterTruce = ini.Get_Bool(SETTINGS, "HarvesterTruce", GameSettings.IsHarvesterTruce);
        GameSettings.IsAlliesAllowed = ini.Get_Bool(SETTINGS, "AlliesAllowed", GameSettings.IsAlliesAllowed);
        GameSettings.IsFixedAlliance = ini.Get_Bool(SETTINGS, "FixedAlliance", GameSettings.IsFixedAlliance);

        GameSettings.Credits = ini.Get_Int(SETTINGS, "Credits", GameSettings.Credits);
        GameSettings.TechLevel = ini.Get_Int(SETTINGS, "TechLevel", GameSettings.TechLevel);
        GameSettings.UnitCount = ini.Get_Int(SETTINGS, "UnitCount", GameSettings.UnitCount);
        GameSettings.PlayerCount = ini.Get_Int(SETTINGS, "Players", GameSettings.PlayerCount);
        GameSettings.AIPlayerCount = ini.Get_Int(SETTINGS, "AIPlayers", GameSettings.AIPlayerCount);
        GameSettings.ObserverCount = ini.Get_Int(SETTINGS, "Observers", GameSettings.ObserverCount);
    }

    /**
     *  Load the random seed for synchronising multiplayer games.
     */
    if (GameSettings.IsMultiplayerGame) {
        GameSettings.Seed = ini.Get_Int(SETTINGS, "Seed", GameSettings.Seed);
    } else {
        GameSettings.Seed = std::time(nullptr);
    }

    /**
     *  Load the scenario filename and its description.
     */
    ini.Get_String(SETTINGS, "Description", "<none>", GameSettings.Description, sizeof(GameSettings.Description));
    ini.Get_String(SETTINGS, "Scenario", "", GameSettings.ScenarioName, sizeof(GameSettings.ScenarioName));

    /**
     *  Only load multiplayer info for non-campaign games.
     */
    if (!GameSettings.IsNormalGame) {

        int total_players = GameSettings.PlayerCount + GameSettings.AIPlayerCount;

        /**
         *  Player check.
         */
#ifdef ENFORCE_MAX_PLAYERS
        if (total_players > MAX_PLAYERS) {
            DEBUG_ERROR("Client: Total player count exceeds MAX_PLAYERS!\n");
            return false;
        }
#endif

        /**
         *  Player and Observer check.
         */
        if (!GameSettings.PlayerCount && GameSettings.ObserverCount) {
            DEBUG_ERROR("Client: No players or observers detected!\n");
            return false;
        }

        /**
         *  Load observer data.
         */
        if (GameSettings.IsMultiplayerGame) {

            for (int i = 0; i < GameSettings.ObserverCount; ++i) {

                char buffer[16];
                std::snprintf(buffer, sizeof(buffer), "Observer%02d", i+1); // Sections should start from 1 (not zero).
                if (!ini.Is_Present(buffer)) {
                    DEBUG_ERROR("Client: Failed to find section for \"%s\"! (expected players %d).\n", buffer, GameSettings.ObserverCount);
                    return false;
                }

                ObserverSettingsType *observer = new ObserverSettingsType;

                std::strncpy(observer->Name, buffer, sizeof(observer->Name));

                ini.Get_String(buffer, "Name", "No name", observer->Name, sizeof(observer->Name));

                DEBUG_INFO("Client: Processing observer \"%s\" (\"%s\").\n", observer->Name, buffer);

                /**
                 *  Is this observer the local player (used to set ObserverPlayerPtr)?
                 */
                observer->IsLocal = ini.Get_Bool(buffer, "Local", observer->IsLocal);

                /**
                 *  Is this observer actually the host of the game?
                 */
                observer->IsHost = ini.Get_Bool(buffer, "Host", observer->IsHost);

                /**
                 *  This slot is valid and active.
                 */
                observer->IsSlotActive = true;

                /**
                 *  Add the observer player to vector.
                 */
                GameSettings.Observers.Add(observer);

            }
        }

        /**
         *  Load player data.
         */
        for (int i = 0; i < total_players; ++i) {

            char buffer[16];
            std::snprintf(buffer, sizeof(buffer), "Multi%02d", i+1); // Sections should start from 1 (not zero).
            if (!ini.Is_Present(buffer)) {
                DEBUG_ERROR("Client: Failed to find section for \"%s\"! (expected players %d).\n", buffer, total_players);
                return false;
            }

            PlayerSettingsType *player = new PlayerSettingsType;

            std::strncpy(player->IniName, buffer, sizeof(player->IniName));

            ini.Get_String(buffer, "Name", "No name", player->PlayerName, sizeof(player->PlayerName));

            DEBUG_INFO("Client: Processing player \"%s\" (\"%s\").\n", player->PlayerName, buffer);

            /**
             *  Is this a human player (non-AI)?
             */
            player->IsHuman = ini.Get_Bool(buffer, "Human", player->IsHuman);

            /**
             *  Check to make sure no other human players have been set in a Skirmish game.
             */
            if (GameSettings.IsSkirmishGame) {
                if (i > 0 && player->IsHuman) {
                    DEBUG_ERROR("Client: Skirmish games can only have one human player!\n");
                    return false;
                }
            }

            if (GameSettings.IsMultiplayerGame) {

                /**
                 *  Is this player the local player (used to set PlayerPtr)?
                 */
                player->IsLocal = ini.Get_Bool(buffer, "Local", player->IsLocal);

                /**
                 *  Is this player the host of the game?
                 */
                player->IsHost = ini.Get_Bool(buffer, "Host", player->IsHost);

                if (i > 0 && player->IsLocal) {
                    DEBUG_ERROR("Client: Multiplayer games can only have one local player!\n");
                    return false;
                }
            }

            /**
             *  
             */
            player->IsPlayerControl = ini.Get_Bool(buffer, "PlayerControl", player->IsPlayerControl);

            /**
             *  Set the difficulty for the AI houses.
             */
            if (!player->IsHuman) {
                player->Difficulty = (DiffType)ini.Get_Int(buffer, "Difficulty", player->Difficulty);
            } else {
                player->Difficulty = DIFF_NORMAL;
            }

            /**
             *  Set the starting waypoint location.
             */
            char waypt_buffer[32];
            ini.Get_String(buffer, "StartWaypoint", "<none>", waypt_buffer, sizeof(waypt_buffer));
            if (!stricmp(waypt_buffer, "<random>")) {
                player->StartLocation = WAYPT_RANDOM;

            } else {

                player->StartLocation = (WaypointEnum)ini.Get_Int(buffer, "StartWaypoint", WAYPOINT_NONE);
                if (player->StartLocation == -1) {
                    DEBUG_ERROR("Client: Player \"%s\" has an invalid starting waypoint!\n", player->IniName);
                    return false;
                }
            }

            /**
             *  
             */
            player->Side = ini.Get_SideType(buffer, "Side", player->Side);
            if (player->Side == SIDE_NONE) {
                DEBUG_ERROR("Client: Player \"%s\" has an invalid side!\n", player->IniName);
                return false;
            }
            
            /**
             *  Set the players house type.
             */
            char house_buffer[32];
            ini.Get_String(buffer, "House", "<none>", house_buffer, sizeof(house_buffer));
            if (!stricmp(house_buffer, "<random>")) {
                player->House = HOUSE_RANDOM;

            } else {

                player->House = ini.Get_HousesType(buffer, "House", player->House);
                if (player->House == HOUSE_NONE) {
                    DEBUG_ERROR("Client: Player \"%s\" has an invalid house!\n", player->IniName);
                    return false;
                }
            }

            /**
             *  Set the players remap color.
             */
            char color_buffer[32];
            ini.Get_String(buffer, "Color", "<none>", color_buffer, sizeof(color_buffer));
            if (!stricmp(house_buffer, "<random>")) {
                player->ColorScheme = COLORSCHEME_RANDOM;

            } else {

                player->ColorScheme = ini.Get_ColorSchemeType(buffer, "Color", player->ColorScheme);
                if (player->ColorScheme == COLORSCHEME_NONE) {
                    DEBUG_ERROR("Client: Player \"%s\" has an invalid color!\n", player->IniName);
                    return false;
                }
            }

            /**
             *  
             */
            player->Team = (ClientTeamType)ini.Get_Int(buffer, "Team", player->Team);
            if (player->Team == TEAM_NONE || player->Team >= TEAM_COUNT) {
                DEBUG_ERROR("Client: Player \"%s\" has an invalid team id!\n", player->IniName);
                return false;
            }

            /**
             *  
             */
            ini.Get_String(buffer, "Allies", "<none>", player->Allies, sizeof(player->Allies));

            /**
             *  This slot is valid and active.
             */
            player->IsSlotActive = true;

            /**
             *  Add the player to vector.
             */
            GameSettings.Players.Add(player);
        }

        /**
         *  Sanity check before we continue.
         */
        if (!GameSettings.Players.Count()) {
            DEBUG_ERROR("Client: Failed to load any player info!\n");
            return false;
        }

        /**
         *  Check the for any starting colour duplicates.
         */
        for (int i = 0; i < GameSettings.Players.Count(); ++i) {

            PlayerSettingsType &player = *GameSettings.Players[i];

            if (!player.IsSlotActive) {
                continue;
            }

            /**
             *  Skip observer players.
             */
            //if (player.IsObserver) {
            //    continue;
            //}

            // #TODO

        }

        /**
         *  Starting location sanity check for conflicting starting locations.
         */
        for (int i = 0; i < GameSettings.Players.Count(); ++i) {

            PlayerSettingsType &player = *GameSettings.Players[i];

            if (!player.IsSlotActive) {
                continue;
            }

            /**
             *  Skip observer players.
             */
            //if (player.IsObserver) {
            //    continue;
            //}

            if (player.StartLocation == WAYPOINT_NONE) {
                DEBUG_ERROR("Client: Invalid starting location for \"%s\"!\n", player.IniName);
                return false;
            }

            for (int j = 0; j < GameSettings.Players.Count(); ++j) {

                PlayerSettingsType &existing_player = *GameSettings.Players[j];

                /**
                 *  Skip ourselves.
                 */
                if (i == j) {
                    continue;
                }

                /**
                 *  Skip random assignments.
                 */
                if (player.StartLocation == WAYPT_RANDOM || existing_player.StartLocation == WAYPT_RANDOM) {
                    continue;
                }

                if (player.StartLocation == existing_player.StartLocation) {
                    DEBUG_ERROR("Client: Duplicate starting locations detected (\"%s\" and \"%s\")!\n",
                        player.IniName, existing_player.IniName);
                    return false;
                }
            }
        }

        /**
         *  Fix up any observer players, they need special treatment.
         */
        //for (int i = 0; i < GameSettings.Players.Count(); ++i) {
        //
        //    ClientPlayerSettingsType &player = *GameSettings.Players[i];
        //
        //    if (!player.IsSlotActive) {
        //        continue;
        //    }
        //
        //    if (player.IsObserver) {
        //        player.IsPlayerControl = false;
        //        player.Side = SIDE_NONE;
        //        player.House = HOUSE_NONE;
        //        player.Difficulty = DIFF_NORMAL;
        //        player.StartLocation = WAYPT_NONE;
        //        player.Team = TEAM_NONE;
        //        player.ColorScheme = COLORSCHEME_NONE;
        //    }
        //}

    }

    DEBUG_INFO("Client: Game settings loaded OK.\n");

    return true;
}


bool Client::Init()
{
    CDFileClass game_file(GameSettingsFilename);
    CCINIClass game_ini;

    if (!game_file.Is_Available()) {
        DEBUG_ERROR("Client: Failed to find game settings!\n");
        return false;
    }

    game_ini.Load(game_file, false);

    /**
     *  Load the client session settings.
     */
    if (!Read_CnCNet_Settings(game_ini)) {
        DEBUG_ERROR("Client: Failed to load CnCNet settings!\n");
        return false;
    }
    if (!Read_Game_Settings(game_ini)) {
        DEBUG_ERROR("Client: Failed to load game settings!\n");
        return false;
    }

    /**
     *  Developer mode will cause an out-of-sync in multiplayer games.
     */
    if (Vinifera_DeveloperMode && GameSettings.IsMultiplayerGame) {
        DEBUG_WARNING("Client: Disabling Developer Mode, it can not be enabled in multiplayer games!\n");
        Vinifera_DeveloperMode = false;
        //return false;
    }

    /**
     *  Create the session player nodes.
     */
    Create_Player_Nodes();

    /**
     *  Output the loaded settings to the debug log.
     */
    if (Vinifera_DeveloperMode) {
        Client_Debug_Log_Settings();
    }

    return true;
}


bool Client::Startup()
{
    CDFileClass client_file(ClientSettingsFilename);
    CCINIClass client_ini;

    if (!client_file.Is_Available()) {
        DEBUG_ERROR("Client: Failed to find Client settings!\n");
        return false;
    }

    client_ini.Load(client_file, false);

    /**
     *  Load the client settings.
     */
    if (!Read_Client_Settings(client_ini)) {
        DEBUG_ERROR("Client: Failed to load Client settings!\n");
        return false;
    }

    if (Vinifera_DeveloperMode) {
        DEBUG_INFO("Client: Developer Mode enabled.\n");
    }

    /**
     *  Settings loaded OK, client mode is active.
     */
    IsActive = true;

    return true;
}


bool Client::Shutdown()
{
#ifdef NDEBUG
    CDFileClass(GameSettingsFilename).Delete();
#endif
    return true;
}


/**
 *  Starts a game session.
 *  
 *  @author: CCHyper
 */
bool Client::Start_Game()
{
    GameActive = true;

    DoList.Init();
    OutList.Init();

    Frame = 0;

    Scen->MissionTimer = 0;
    Scen->MissionTimer.Stop();

    Scen->CDifficulty = DIFF_NORMAL;
    Scen->Difficulty = DIFF_NORMAL;

    PlayerWins = false;
    PlayerLoses = false;
    PlayerRestarts = false;
    PlayerAborts = false;

    Map.Set_Cursor_Shape(nullptr);
    Map.PendingObjectPtr = nullptr;
    Map.PendingObject = nullptr;
    Map.PendingHouse = HOUSE_NONE;

    Session.ProcessTicks = 0;
    Session.ProcessFrames = 0;
    Session.DesiredFrameRate = 60;

    Session.ObiWan = false;

    NewMaxAheadFrame1 = 0;
    NewMaxAheadFrame2 = 0;

    Debug_Map = false;
    Debug_Unshroud = false;

    /**
     *  Clear any developer mode globals.
     */
    Vinifera_Developer_AIInstantBuild = false;
    Vinifera_Developer_InstantBuild = false;
    Vinifera_Developer_BuildCheat = false;
    Vinifera_Developer_Unshroud = false;
    Vinifera_Developer_FrameStep = false;
    Vinifera_Developer_FrameStepCount = 0;
    Vinifera_Developer_AIControl = false;

    /**
     *  Set default mouse shape
     */
    Map.Set_Default_Mouse(MOUSE_NORMAL, false);

    if (ToolTipHandler) {
        ToolTipHandler->Set_Active(false);
    }

    if (GameSettings.IsMultiplayerGame) {
        Session.Type = GAME_INTERNET;

    } else if (GameSettings.IsNormalGame) {
        Session.Type = GAME_NORMAL;

    } else {
        Session.Type = GAME_SKIRMISH;
    }

    Session.NumPlayers = GameSettings.PlayerCount;
    Session.Options.AIPlayers = GameSettings.AIPlayerCount;

    Session.Options.ShortGame = GameSettings.IsShortGame;
    Session.Options.CrapEngineers = GameSettings.IsMultiEngineer;
    Session.Options.RedeployMCV = GameSettings.IsMCVRedeploy;
    Session.Options.Goodies = GameSettings.IsCrateGoodies;
    Session.Options.BridgeDestruction = GameSettings.IsBridgeDestruction;
    Session.Options.FogOfWar = GameSettings.IsFogOfWar;
    Session.Options.HarvesterTruce = GameSettings.IsHarvesterTruce;
    Session.Options.AlliesAllowed = GameSettings.IsAlliesAllowed;

    Special.IsTSpread = GameSettings.IsTiberiumGrows;
    //Special.IsTSpread = GameSettings.IsTiberiumSpreads;
    //Special.TiberiumExplosive = GameSettings.TiberiumExplosive;
    //Special.IsVisceroids = GameSettings.IsVisceroids;
    //Special.IonStorms = GameSettings.IsIonStorms;
    //Special.Meteorites = GameSettings.IsMeteorites;
    Special.IsShadowGrow = GameSettings.IsShroudRegrows;
    //Special.InitialVeteran = GameSettings.IsInitialVeteran;
    Special.FixedAlliance = GameSettings.IsFixedAlliance;

    /**
     *  Don't carry stray keystrokes into game.
     */
    WWKeyboard->Clear();

    /**
     *  Initialize the random number generator(s).
     */
    CustomSeed = GameSettings.Seed;
    Init_Random();

    /**
     *  If this is a internet game, setup connections to other players.
     */
    if (Session.Type == GAME_INTERNET) {

        Session.MaxAhead = GameSettings.MaxAhead;
        //Session.MaxMaxAhead = GameSettings.MaxMaxAhead;
        //Session.LatencyFudge = GameSettings.LatencyFudge;
        //Session.RequestedFPS = GameSettings.RequestedFPS;
        Session.FrameSendRate = GameSettings.FrameSendRate;
        Session.CommProtocol = GameSettings.CommProtocol;

        /**
         *  Enable the CnCNet5 interface.
         */
        CnCNet5::IsActive = true;

        if (!PacketTransport) {
            PacketTransport = new CnCNet5UDPInterfaceClass(CnCNet5::TunnelInfo.ID,
                                                           CnCNet5::TunnelInfo.IP,
                                                           CnCNet5::TunnelInfo.Port,
                                                           CnCNet5::TunnelInfo.PortHack);
        }
        ASSERT(PacketTransport);

        //Init_Network();
        //Ipx.Init();

        Session.Create_Connections();

        //Ipx.Set_Timing(&IPXManagerClass_this, 60, -1, 600, 1);
    }

    /**
     *  Enable firestorm mode if requested.
     */
    if (Addon_Present() && GameSettings.IsFirestormAddon) {

        Addon_4071C0(-1);

        Addon_407190(1);
        Set_Required_Addon(ADDON_FIRESTORM);
    }

    /**
     *  Stop any theme that might be playing (you never know...).
     */
    Theme.Stop();

    /**
     *  
     */
    bool started = false;
    if (Session.Type == GAME_NORMAL) {

        if (!Campaigns.Count()) {
            Init_Campaigns();
        }

        if (Campaigns.Count() > 0 && GameSettings.Campaign != CAMPAIGN_NONE) {

            CampaignClass *campaign = Campaigns[GameSettings.Campaign];

            //std::strncpy(Scen->ScenarioName, campaign->Scenario, sizeof(Scen->ScenarioName));
            //std::strncpy(Scen->Description, campaign->Description, sizeof(Scen->Description));

            started = Start_Scenario(nullptr, true, GameSettings.Campaign);

        } else {
            //std::strncpy(Scen->ScenarioName, GameSettings.ScenarioName, sizeof(Scen->ScenarioName));
            //std::strncpy(Scen->Description, GameSettings.Description, sizeof(Scen->Description));
            started = Start_Scenario(Scen->ScenarioName, true);
        }

    } else {

        /**
         *  Copy scenario name.
         */
        std::strncpy(Scen->ScenarioName, GameSettings.ScenarioName, sizeof(Scen->ScenarioName));
        std::strncpy(Scen->Description, GameSettings.Description, sizeof(Scen->Description));

        DEBUG_INFO("Client: About to call Start_Scenario with \"%s\".\n", Scen->ScenarioName);

        started = Start_Scenario(Scen->ScenarioName, false);
    }

    if (!started) {
        DEBUG_ERROR("Client: Failed to start scenario \"%s\"!\n", Scen->ScenarioName);
        return false;
    }

    /**
     *  Force the map to render one frame.
     */

    Call_Back();

    WWMouse->Hide_Mouse();

    HiddenSurface->Clear();
    GScreenClass::Blit(true, HiddenSurface);

    WWMouse->Show_Mouse();

    Map.Override_Mouse_Shape(MOUSE_NO_MOVE);
    Map.Revert_Mouse_Shape();

    Map.Activate(1);

    Map.Flag_To_Redraw(true);

    Call_Back();

    Map.Render();

    return GameActive;
}


/**
 *  Add the player information for the session lists.
 * 
 *  @warning: This should be called right before Client::Assign_Houses()!
 * 
 *  @author: CCHyper
 */
bool Client::Create_Player_Nodes()
{
    /**
     *  Clear out any existing entries.
     */
    Session.Players.Clear();

    for (int i = 0; i < GameSettings.Players.Count(); ++i) {

        PlayerSettingsType &player = *GameSettings.Players[i];

        if (!player.IsSlotActive) {
            continue;
        }

        NodeNameType *node = new NodeNameType;
        if (!node) {
            return false;
        }

        std::memset(node, 0, sizeof(NodeNameType));

        /**
         *  Set the player house name.
         */
        if (player.PlayerName[0] != '\0') {
            std::strncpy(node->Name, player.PlayerName, MPLAYER_NAME_MAX-1);
        }

        node->Player.House = player.House;
        node->Player.Color = PCOLOR_FIRST;  // This isn't used in Client mode.

#if 0
        NetNumType net;
        NetNodeType node;

        node->Address = IPAddressClass(net, node);


        currentNode->Address.sin_family = 0;
        currentNode->Address.sin_port = 0;
        currentNode->Address.sin_addr.s_addr = NodeId;
        std::memset(currentNode->Address.sin_zero, 0, 8);

        AddressList[NodeId - 1].ip = inet_addr(player.Address);
#endif

        /**
         *  Add to vector.
         */
        Session.Players.Add(node);
    }

    return true;
}


/**
 *  Assigns all players to a multiplayer house slot.
 * 
 *  @warning: This function assumes you have already filled the 'Players' vector
 *            and they have been correctly initialised. See Fill_Players_List().
 * 
 *  @author: CCHyper
 */
bool Client::Assign_Houses()
{
    if (!IsActive) {
        DEBUG_ERROR("Client: Client system is not active!\n");
        return false;
    }

    HouseClass *housep;
    HouseClass *with;
    HouseTypeClass *housetype;
    HousesType house;
    ColorSchemeType color;

    /**
     *  Clear any existing player house instance.
     */
    delete PlayerPtr;
    PlayerPtr = nullptr;

    DEBUG_INFO("Client: Assign_Houses(enter)\n");

    if (!Session.Players.Count()) {
        DEBUG_ERROR("  Players.Count is zero!\n");
        return false;
    }
    
    if (Session.Players.Count() > 0) {
        DEBUG_INFO("  Assigning players (%d)...\n", Session.Players.Count());
    }

    /**
     *  Assign each player in 'Players' to a multiplayer house. Players will
     *  be sorted by their chosen color value (this value must be unique among
     *  all the players).
     */
    for (int i = 0; i < Session.Players.Count(); ++i) {

        NodeNameType &node = *Session.Players[i];
        PlayerSettingsType &client_player = *GameSettings.Players[i];

        house = node.Player.House;
        if (house == HOUSE_RANDOM) {
            while (true) {
                house = (HousesType)Random_Pick(0, HouseTypes.Count()-1);
                if (HouseTypes[house]->IsMultiplay) {
                    break;
                }
            }
            //DEV_DEBUG_INFO("    Random house \"%s\" chosen for \"%s\".\n",
            //    HouseTypeClass::Name_From(house), client_player.IniName);
        }

        housep = new HouseClass(HouseTypeClass::As_Pointer(house));

        std::memset((char *)housep->IniName, 0, MPLAYER_NAME_MAX);
        std::strncpy((char *)housep->IniName, node.Name, MPLAYER_NAME_MAX-1);

        housep->IsHuman = client_player.IsHuman;
        housep->IsPlayerControl = client_player.IsPlayerControl;

        housep->Control.TechLevel = GameSettings.TechLevel;

        housep->Init_Data((PlayerColorType)node.Player.Color, node.Player.House, Session.Options.Credits);

        color = client_player.ColorScheme;
        if (color == COLORSCHEME_RANDOM) {

            while (true) {
                color = (ColorSchemeType)Random_Pick(0, ColorSchemes.Count()-1);
                if (ColorSchemes[color]->field_310 != 1) {
                    break;
                }
            }
            //DEV_DEBUG_INFO("    Random color \"%s\" chosen for \"%s\".\n",
            //    ColorSchemes[color]->Name, client_player.IniName);
        }

        housep->RemapColor = color;
        housep->Init_Remap_Color();

        /**
         *  Only computer houses need a handicap.
         */
        if (!client_player.IsHuman) {

            housep->IsHuman = false;
            //housep->IsStarted = true;

            if (Session.Type != GAME_NORMAL) {
                housep->IQ = Rule->MaxIQ;
            }

            DiffType difficulty = client_player.Difficulty;

            if (Rule->IsCompEasyBonus && difficulty > DIFF_EASY) {
                difficulty = (DiffType)(difficulty - 1);
            }
            housep->Assign_Handicap(difficulty);

        } else {
            housep->IsHuman = true;
            housep->Assign_Handicap(DIFF_NORMAL);
        }

        /**
         *  If this player is for myself, set up PlayerPtr.
         */
        if ((GameSettings.IsMultiplayerGame && client_player.IsLocal)
         || (GameSettings.IsSkirmishGame && i == 0)) {

            PlayerPtr = housep;
            housep->IsPlayerControl = true;
        }

        /**
         *  Record where we placed this player.
         */
        node.Player.ID = HousesType(housep->ID);

        /**
         *  Store a pointer to this house in the client player data so
         *  we can track its stats during the game and setup initial alliances.
         */
        client_player.HousePtr = housep;

        DEBUG_INFO("    Assigned player \"%s\" (House: \"%s\", ID: %d, Color: \"%s\") to slot %d.\n",
            node.Name, housep->Class->Name(), node.Player.ID, ColorSchemes[housep->RemapColor]->Name, i);
    }

    /**
     *  Create Neutral and Special houses as they must exist!
     * 
     *  #BUGFIX:
     *  Added checks to make sure the houses exist before blindly
     *  attempting to create a instance of them.
     */
    
    house = HouseTypeClass::From_Name("Neutral");
    if (house != HOUSE_NONE) {
        DEBUG_INFO("  Creating Neutral house...\n");

        housep = new HouseClass(HouseTypes[house]);
        housep->RemapColor = ColorScheme::From_Name("LightGrey");
        housep->Init_Remap_Color();
    }

    house = HouseTypeClass::From_Name("Special");
    if (house != HOUSE_NONE) {
        DEBUG_INFO("  Creating Special house...\n");
        
        housep = new HouseClass(HouseTypes[house]);
        housep->RemapColor = ColorScheme::From_Name("LightGrey");
        housep->Init_Remap_Color();
    }

    DEBUG_INFO("Client: Alliances...\n");

    /**
     *  Loop over all player slots and setup alliances.
     */
    for (int i = 0; i < Session.Players.Count(); ++i) {

        NodeNameType &node = *Session.Players[i];
        PlayerSettingsType &client_player = *GameSettings.Players[i];

        housep = Houses[node.Player.ID];
        if (!housep) {
            DEBUG_ERROR("Client: Invalid house when setting up alliances!\n");
            return false;
        }

        if (housep->ID != client_player.HousePtr->ID) {
            DEBUG_ERROR("Client: Player info mismatch when setting up alliances!\n");
            return false;
        }

        /**
         *  Loop over all players and setup alliances.
         */
        char *ally_name = std::strtok(client_player.Allies, ",");
        while (ally_name) {

            for (int j = 0; j < GameSettings.Players.Count(); ++j) {

                PlayerSettingsType &with_player = *GameSettings.Players[j];

                if (!std::strcmp(ally_name, with_player.IniName)) {

                    with = with_player.HousePtr;

                    /**
                     *  Ally the two houses with each other.
                     */
                    housep->Make_Ally(with);
                    housep->Control.Allies |= 1 << with->ID;

                    //DEV_DEBUG_INFO("  Player \"%s\" has allied with \"%s\".\n",
                    //    client_player.IniName, with_player.IniName);
                    DEBUG_INFO("  Player \"%s\" has allied with \"%s\".\n",
                        client_player.PlayerName, with_player.PlayerName);
                }

            }

            ally_name = std::strtok(nullptr, ",");
        }

    }

    /**
     *  Log team setups.
     */
    DEBUG_INFO("Client: Teams...\n");
    for (int team = 0; team < TEAM_COUNT; ++team) {

        DEBUG_INFO("    %s\n", Name_From_Team(ClientTeamType(team)));
        for (int i = 0; i < GameSettings.Players.Count(); ++i) {
            PlayerSettingsType &player = *GameSettings.Players[i];
            if (player.Team == team) {
                DEBUG_INFO("      %s \"%s\"\n", player.IniName, player.PlayerName);
            }
        }
    }

    if (!PlayerPtr) {
        DEBUG_ERROR("Client: Local player not assigned!\n");
        return false;
    }

    /**
     *  
     */
    for (int i = 0; i < GameSettings.Observers.Count(); ++i) {

        ObserverSettingsType &client_observer = *GameSettings.Observers[i];

        /**
         *  If this player is an observer and is for myself, set up ObserverPlayerPtr.
         */
        if ((GameSettings.IsMultiplayerGame && client_observer.IsLocal)
         || (GameSettings.IsSkirmishGame && i == 0)) {

            ObserverPlayerPtr = PlayerPtr;
        }

    }

    if (GameSettings.ObserverCount > 0 && !ObserverPlayerPtr) {
        DEBUG_ERROR("Client: Local observer not assigned!\n");
        return false;
    }

    DEBUG_INFO("Client: Assign_Houses(exit)\n");

    return true;
}


const char *Client::Name_From_Team(ClientTeamType team)
{
    static const char *_team_names[TEAM_COUNT] = {
        "Team A", "Team B", "Team C", "Team D",
        "Team E", "Team F", "Team G", "Team H"
    };

    return team != TEAM_NONE && team < TEAM_COUNT ? _team_names[team] : "<invalid>";
}
