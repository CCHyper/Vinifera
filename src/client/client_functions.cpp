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
#include "houseext.h"
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
#include "drop.h"
#include "statbtn.h"
#include "tactical.h"
#include "utracker.h"
#include "bsurface.h"
#include "spritecollection.h"
#include "filepcx.h"
#include "drop.h"
#include "statbtn.h"
#include "ownrdraw.h"
#include "iomap.h"
#include "sessionext.h"
#include "wwfont.h"
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

#define TXT_CLIENT_INALIDCLIENTSETTINGS       "Client: Invalid client setings!\n"
#define TXT_CLIENT_INALIDGAMESETTINGS         "Client: Invalid game settings!\n"
#define TXT_CLIENT_INALIDGAMEID               "Client: Invalid game id!\n"
#define TXT_CLIENT_INALIDLAUNCHOPTION         "Client: Invalid launch mode!\n"


#define GADGET_OBSERVER_DROPDOWN 1000
#define GADGET_OBSERVER_RESET 1001
#define GADGET_OBSERVER_PLAYER_ONE 1002
#define GADGET_OBSERVER_PLAYER_TWO 1003
#define GADGET_OBSERVER_PLAYER_THREE 1004
#define GADGET_OBSERVER_PLAYER_FOUR 1005
#define GADGET_OBSERVER_PLAYER_FIVE 1006
#define GADGET_OBSERVER_PLAYER_SIX 1007
#define GADGET_OBSERVER_PLAYER_SEVEN 1008
#define GADGET_OBSERVER_PLAYER_EIGHT 1009


static void Client_Debug_Log_Settings()
{
#ifndef NDEBUG
    DEBUG_INFO("\n");

    DEBUG_INFO("Client: Dumping game settings...\n");

    DEBUG_INFO("  ScenarioName:         %s\n", Client::GameSettings.ScenarioName);
    DEBUG_INFO("  Description:          %s\n", Client::GameSettings.Description);

    DEBUG_INFO("  IsWDTGame:            %s\n", Client::GameSettings.IsWDTGame ? "true" : "false");
    DEBUG_INFO("  IsQuickMatchGame:     %s\n", Client::GameSettings.IsQuickMatchGame ? "true" : "false");
    DEBUG_INFO("  IsMultiplayerGame:    %s\n", Client::GameSettings.IsMultiplayerGame ? "true" : "false");
    DEBUG_INFO("  IsSkirmishGame:       %s\n", Client::GameSettings.IsSkirmishGame ? "true" : "false");
    DEBUG_INFO("  IsNormalGame:         %s\n", Client::GameSettings.IsNormalGame ? "true" : "false");

    DEBUG_INFO("  IsFirestormAddon:     %s\n", Client::GameSettings.IsFirestormAddon ? "true" : "false");

    if (!Client::GameSettings.IsNormalGame) {
        DEBUG_INFO("  IsShortGame:          %s\n", Client::GameSettings.IsShortGame ? "true" : "false");
        DEBUG_INFO("  IsMultiEngineer:      %s\n", Client::GameSettings.IsMultiEngineer ? "true" : "false");
        DEBUG_INFO("  IsMCVRedeploy:        %s\n", Client::GameSettings.IsMCVRedeploy ? "true" : "false");
        DEBUG_INFO("  IsCrateGoodies:       %s\n", Client::GameSettings.IsCrateGoodies ? "true" : "false");
        DEBUG_INFO("  IsBridgeDestruction:  %s\n", Client::GameSettings.IsBridgeDestruction ? "true" : "false");
        DEBUG_INFO("  IsTiberiumGrows:      %s\n", Client::GameSettings.IsTiberiumGrows ? "true" : "false");
        DEBUG_INFO("  IsFogOfWar:           %s\n", Client::GameSettings.IsFogOfWar ? "true" : "false");
        DEBUG_INFO("  IsShroudRegrows:      %s\n", Client::GameSettings.IsShroudRegrows ? "true" : "false");

        DEBUG_INFO("  IsAutoDeployMCV:      %s\n", Client::GameSettings.IsAutoDeployMCV ? "true" : "false");
        DEBUG_INFO("  IsPrePlacedConYards:  %s\n", Client::GameSettings.IsPrePlacedConYards ? "true" : "false");
        DEBUG_INFO("  IsBuildOffAlly:       %s\n", Client::GameSettings.IsBuildOffAlly ? "true" : "false");
    }

    DEBUG_INFO("  Seed:                 %08d\n", Client::GameSettings.Seed);

    DEBUG_INFO("\n");

    if (!Client::GameSettings.IsNormalGame && !Client::GameSettings.IsWDTGame) {

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
                DEBUG_INFO("    IsObserver:   %s\n", player.IsObserver ? "true" : "false");
            }
        }
        DEBUG_INFO("\n");

#if 0
        DEBUG_INFO("Client: Dumping observer data...\n");
        for (int i = 0; i < Client::GameSettings.Observers.Count(); ++i) {
            Client::ObserverSettingsType &observer = *Client::GameSettings.Observers[i];
            if (observer.IsSlotActive) {
                DEBUG_INFO("  Observer%d (%d):\n", i+1, i);
                DEBUG_INFO("    Name:   %s\n", observer.PlayerName);
                if (Client::GameSettings.IsMultiplayerGame) {
                    DEBUG_INFO("    IsHost:       %s\n", observer.IsHost ? "true" : "false");
                }
                DEBUG_INFO("    IsLocal:      %s\n", observer.IsLocal ? "true" : "false");
            }
        }
        DEBUG_INFO("\n");
#endif

    }

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
         *  Format: "-CLIENT=<client>,<settings>,<gameid>,<exit>,<skip>,<dev_mode>"
         * 
         *  Example: "-CLIENT=RESOURCES\MOD_SETTINGS.INI,RESOURCES\CLIENT_GAME.INI,999,1,1,0"
         */
        if (std::strstr(string, "-CLIENT")) {

            char *tmpstr;
            char *str = (char *)&string[std::strlen("-CLIENT")+1]; // +1 to skip the "=" char.

            /**
             *  Fetch settings, scenario and exit mode.
             *  We do some error checking here as we don't want to launch the normal game
             *  if client-launch game actually failed. We show an error message instead.
             */

            tmpstr = std::strtok(str, ",");
            if (!tmpstr || !CDFileClass(tmpstr).Is_Available()) {
                DEBUG_ERROR(TXT_CLIENT_INALIDCLIENTSETTINGS);
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal(TXT_CLIENT_INALIDCLIENTSETTINGS);
            }
            std::strncpy(ClientSettingsFilename, tmpstr, sizeof(ClientSettingsFilename));

            tmpstr = std::strtok(nullptr, ",");
            if (!tmpstr || !CDFileClass(tmpstr).Is_Available()) {
                DEBUG_ERROR(TXT_CLIENT_INALIDGAMESETTINGS);
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal(TXT_CLIENT_INALIDGAMESETTINGS);
            }
            std::strncpy(GameSettingsFilename, tmpstr, sizeof(GameSettingsFilename));

#if 0
            tmpstr = std::strtok(nullptr, ",");
            if (!tmpstr) {
                DEBUG_ERROR(TXT_CLIENT_INALIDGAMEID);
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal(TXT_CLIENT_INALIDGAMEID);
            }
            GameID = std::strtol(tmpstr, nullptr, 10);

            tmpstr = std::strtok(nullptr, ",");
            if (!tmpstr) {
                DEBUG_ERROR(TXT_CLIENT_INALIDLAUNCHOPTION);
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal(TXT_CLIENT_INALIDLAUNCHOPTION);
            }
            IsExitOnGameFinish = std::strtol(tmpstr, nullptr, 10) == 1 ? true : false;

            tmpstr = std::strtok(nullptr, ",");
            if (!tmpstr) {
                DEBUG_ERROR(TXT_CLIENT_INALIDLAUNCHOPTION);
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal(TXT_CLIENT_INALIDLAUNCHOPTION);
            }
            IsSkipStartupMovies = std::strtol(tmpstr, nullptr, 10) == 1 ? true : false;

            tmpstr = std::strtok(nullptr, ",");
            if (!tmpstr) {
                DEBUG_ERROR(TXT_CLIENT_INALIDLAUNCHOPTION);
                ShowCursor(TRUE);
                MessageBox(nullptr, TXT_CLIENT_RUN_ERROR, "Vinifera", MB_OK);
                Fatal(TXT_CLIENT_INALIDLAUNCHOPTION);
            }
            Vinifera_DeveloperMode = std::strtol(tmpstr, nullptr, 10) == 1 ? true : false;
#endif

            DEBUG_INFO("Client: Starting game launched from the client.\n");

            return true;

        }

    }

    return false;
}


bool Client::Read_Client_Startup_Settings()
{
    DEV_DEBUG_INFO("Client: Loading Client startup settings.\n");

    RawFileClass file(ClientStartupSettingsFilename);
    if (!file.Is_Available()) {
        return false;
    }

    INIClass ini;
    if (!ini.Load(file)) {
        return false;
    }

    static char const * const GENERAL = "General";
    if (!ini.Is_Present(GENERAL)) {
        return false;
    }

    /**
     *  This stops stops the game from being launched directly if the mod developer wishes so.
     */
    IsRunFromClientOnly = ini.Get_Bool(GENERAL, "RunFromClientOnly", IsRunFromClientOnly);

    /**
     *  This allows Mod developers that might wish to enable the developer using an
     *  option in the front-end client.
     */
    IsDeveloperModeEnabled = ini.Get_Bool(GENERAL, "DeveloperMode", Vinifera_DeveloperMode);

    /**
     *  Debug console can only be used when developer mode is active.
     */
    if (IsDeveloperModeEnabled) {
        IsDebugConsoleEnabled = ini.Get_Bool(GENERAL, "DebugConsole", IsDebugConsoleEnabled);
    }

    return true;
}


bool Client::Read_Client_Settings(CCINIClass &ini)
{
    DEBUG_INFO("Client: Loading Client settings.\n");

    static char const * const GENERAL = "General";
    if (!ini.Is_Present(GENERAL)) {
        return false;
    }

    IsExitOnGameFinish = ini.Get_Bool(GENERAL, "ExitOnGameFinish", IsExitOnGameFinish);
    IsSkipStartupMovies = ini.Get_Bool(GENERAL, "SkipStartupMovies", IsSkipStartupMovies);

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

    GameSettings.IsWDTGame = ini.Get_Bool(SETTINGS, "WDTGame", GameSettings.IsWDTGame);
    GameSettings.IsQuickMatchGame = ini.Get_Bool(SETTINGS, "QuickMatchGame", GameSettings.IsQuickMatchGame);
    GameSettings.IsMultiplayerGame = ini.Get_Bool(SETTINGS, "MultiplayerGame", GameSettings.IsMultiplayerGame);
    GameSettings.IsNormalGame = ini.Get_Bool(SETTINGS, "NormalGame", GameSettings.IsNormalGame);
    GameSettings.IsSkirmishGame = ini.Get_Bool(SETTINGS, "SkirmishGame", GameSettings.IsSkirmishGame);

    if (!GameSettings.IsNormalGame) {
        GameSettings.IsFirestormAddon = ini.Get_Bool(SETTINGS, "Firestorm", GameSettings.IsFirestormAddon);
    }

    GameSettings.IsSkipScore = ini.Get_Bool(SETTINGS, "SkipScore", GameSettings.IsSkipScore);

    /**
     *  Force the priority of game modes (only one mode can be active at once).
     * 
     *  1) IsWDTGame
     *  1) IsQuickMatchGame
     *  2) IsMultiplayerGame
     *  3) IsSkirmishGame
     *  4) IsNormalGame
     */
    if (GameSettings.IsWDTGame) {
        GameSettings.IsQuickMatchGame = false;
        GameSettings.IsMultiplayerGame = false;
        GameSettings.IsSkirmishGame = false;
        GameSettings.IsNormalGame = false;
        GameSettings.Campaign = CAMPAIGN_NONE;

    } else if (GameSettings.IsQuickMatchGame) {
        GameSettings.IsWDTGame = false;
        GameSettings.IsMultiplayerGame = false;
        GameSettings.IsSkirmishGame = false;
        GameSettings.IsNormalGame = false;
        GameSettings.Campaign = CAMPAIGN_NONE;

    } else if (GameSettings.IsMultiplayerGame) {
        GameSettings.IsWDTGame = false;
        GameSettings.IsQuickMatchGame = false;
        GameSettings.IsSkirmishGame = false;
        GameSettings.IsNormalGame = false;
        GameSettings.Campaign = CAMPAIGN_NONE;

    } else if (GameSettings.IsSkirmishGame) {
        GameSettings.IsWDTGame = false;
        GameSettings.IsQuickMatchGame = false;
        GameSettings.IsMultiplayerGame = false;
        GameSettings.IsNormalGame = false;
        GameSettings.Campaign = CAMPAIGN_NONE;

    } else if (GameSettings.IsNormalGame) {
        GameSettings.IsWDTGame = false;
        GameSettings.IsQuickMatchGame = false;
        GameSettings.IsMultiplayerGame = false;
        GameSettings.IsSkirmishGame = false;
    }

    /**
     *  Load the campaign type.
     */
    if (GameSettings.IsNormalGame) {
        GameSettings.Campaign = (CampaignType)ini.Get_Int(SETTINGS, "Campaign", GameSettings.Campaign);
        GameSettings.IsOneTimeOnly = ini.Get_Bool(SETTINGS, "OneTimeOnly", GameSettings.IsOneTimeOnly);
        GameSettings.IsSkipMapSelect = ini.Get_Bool(SETTINGS, "SkipMapSelect", GameSettings.IsSkipMapSelect);
        GameSettings.IsEndOfGame = ini.Get_Bool(SETTINGS, "EndOfGame", GameSettings.IsEndOfGame);
        GameSettings.PlayerDifficulty = (DiffType)ini.Get_Int_Clamp(SETTINGS, "PlayerDifficulty", DIFF_EASY, DIFF_HARD, GameSettings.PlayerDifficulty);
        GameSettings.ComputerDifficulty = (DiffType)ini.Get_Int_Clamp(SETTINGS, "ComputerDifficulty", DIFF_EASY, DIFF_HARD, GameSettings.ComputerDifficulty);

        /**
         *  Read the global variable names.
         */
        Scen->Read_Global_INI(ini);
    }

    /**
     *  Load World Domination Tour options.
     */
    if (GameSettings.IsWDTGame) {
        // TODO
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

        GameSettings.IsAutoDeployMCV = ini.Get_Bool(SETTINGS, "AutoDeployMCV", GameSettings.IsAutoDeployMCV);
        GameSettings.IsPrePlacedConYards = ini.Get_Bool(SETTINGS, "PrePlacedConYards", GameSettings.IsPrePlacedConYards);
        GameSettings.IsBuildOffAlly = ini.Get_Bool(SETTINGS, "BuildOffAlly", GameSettings.IsBuildOffAlly);
    }

    /**
     *  Load the custom loading screen filename.
     */
    ini.Get_String(SETTINGS, "LoadingScreenPrefix", GameSettings.LoadingScreenPrefix, sizeof(GameSettings.LoadingScreenPrefix));
    if (GameSettings.LoadingScreenPrefix[0] != '\0') {
        DEBUG_INFO("Client: Loading screen set to \"%s\".\n", GameSettings.LoadingScreenPrefix);
    }

    /**
     *  Load the random seed for synchronising multiplayer games.
     */
    if (GameSettings.IsNormalGame) {
        GameSettings.Seed = std::time(nullptr);
    } else {
        GameSettings.Seed = ini.Get_Int(SETTINGS, "Seed", GameSettings.Seed);
    }

    /**
     *  Load the scenario filename and its description.
     */
    ini.Get_String(SETTINGS, "Description", "<none>", GameSettings.Description, sizeof(GameSettings.Description));
    ini.Get_String(SETTINGS, "Scenario", "", GameSettings.ScenarioName, sizeof(GameSettings.ScenarioName));

    /**
     *  Is the requested scenario actually a map pack?
     */
    if (std::strstr(GameSettings.ScenarioName, ".MMX")) {
        GameSettings.ScenarioIsMixfile = true;
    }

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
        if (!GameSettings.PlayerCount && !GameSettings.ObserverCount) {
            DEBUG_ERROR("Client: No players or observers detected!\n");
            return false;
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

            PlayerSettingsType *client_player = new PlayerSettingsType;

            std::strncpy(client_player->PlayerName, buffer, sizeof(client_player->PlayerName));
            std::strncpy(client_player->IniName, buffer, sizeof(client_player->IniName));

            ini.Get_String(buffer, "Name", "No name", client_player->PlayerName, sizeof(client_player->PlayerName));

            DEBUG_INFO("Client: Processing player \"%s\" (\"%s\").\n", client_player->PlayerName, client_player->IniName);

            /**
             *  Is this a human player (non-AI)?
             */
            client_player->IsHuman = ini.Get_Bool(buffer, "Human", client_player->IsHuman);

            /**
             *  Check to make sure no other human players have been set in a Skirmish game.
             */
            if (GameSettings.IsSkirmishGame) {
                if (i > 0 && client_player->IsHuman) {
                    DEBUG_ERROR("Client: Skirmish games can only have one human player!\n");
                    return false;
                }
            }

            /**
             *  Is this player the local human player (used to set PlayerPtr)?
             */
            client_player->IsLocal = ini.Get_Bool(buffer, "Local", client_player->IsLocal);

            if (GameSettings.IsMultiplayerGame || GameSettings.IsQuickMatchGame) {

                /**
                 *  Is this player the host of the game?
                 */
                client_player->IsHost = ini.Get_Bool(buffer, "Host", client_player->IsHost);
            }

            /**
             *  
             */
            client_player->IsPlayerControl = ini.Get_Bool(buffer, "PlayerControl", client_player->IsPlayerControl);

            /**
             *  Set the difficulty for the AI houses.
             */
            if (!client_player->IsHuman) {
                client_player->Difficulty = (DiffType)ini.Get_Int(buffer, "Difficulty", client_player->Difficulty);
            } else {
                client_player->Difficulty = DIFF_NORMAL;
            }

            /**
             *  Set the starting waypoint location.
             */
            char waypt_buffer[32];
            ini.Get_String(buffer, "StartWaypoint", "<none>", waypt_buffer, sizeof(waypt_buffer));
            if (!stricmp(waypt_buffer, "<random>")) {
                client_player->StartLocation = WAYPT_RANDOM;

            } else {

                client_player->StartLocation = (WaypointEnum)ini.Get_Int(buffer, "StartWaypoint", WAYPOINT_NONE);
                if (client_player->StartLocation == -1) {
                    DEBUG_ERROR("Client: Player \"%s\" has an invalid starting waypoint!\n", client_player->IniName);
                    return false;
                }
            }

            /**
             *  
             */
            client_player->Side = ini.Get_SideType(buffer, "Side", client_player->Side);
            if (client_player->Side == SIDE_NONE) {
                DEBUG_ERROR("Client: Player \"%s\" has an invalid side!\n", client_player->IniName);
                return false;
            }
            
            /**
             *  Set the players house type.
             */
            char house_buffer[32];
            ini.Get_String(buffer, "House", "<none>", house_buffer, sizeof(house_buffer));
            if (!stricmp(house_buffer, "<random>")) {
                client_player->House = HOUSE_RANDOM;

            } else {

                client_player->House = ini.Get_HousesType(buffer, "House", client_player->House);
                if (client_player->House == HOUSE_NONE) {
                    DEBUG_ERROR("Client: Player \"%s\" has an invalid house!\n", client_player->IniName);
                    return false;
                }
            }

            /**
             *  Set the players remap color.
             */
            char color_buffer[32];
            ini.Get_String(buffer, "Color", "<none>", color_buffer, sizeof(color_buffer));
            if (!stricmp(color_buffer, "<none>") || !stricmp(color_buffer, "<random>")) {
                client_player->ColorScheme = COLORSCHEME_RANDOM;

            } else {

                client_player->ColorScheme = ini.Get_ColorSchemeType(buffer, "Color", client_player->ColorScheme);
                if (client_player->ColorScheme == COLORSCHEME_NONE) {
                    DEBUG_ERROR("Client: Player \"%s\" has an invalid color!\n", client_player->IniName);
                    //client_player->ColorScheme = COLORSCHEME_RANDOM;
                    return false;
                }
            }

            /**
             *  
             */
            client_player->Team = (ClientTeamType)ini.Get_Int(buffer, "Team", client_player->Team);
            if (client_player->Team == TEAM_NONE || client_player->Team >= TEAM_COUNT) {
                DEBUG_ERROR("Client: Player \"%s\" has an invalid team id!\n", client_player->IniName);
                return false;
            }

            /**
             *  
             */
            ini.Get_String(buffer, "Allies", "<none>", client_player->Allies, sizeof(client_player->Allies));

            /**
             *  This slot is valid and active.
             */
            client_player->IsSlotActive = true;

            /**
             *  Add the player to vector.
             */
            GameSettings.Players.Add(client_player);
        }

        /**
         *  Load observer data.
         */
        if (GameSettings.IsSkirmishGame || GameSettings.IsMultiplayerGame) {

            for (int i = 0; i < GameSettings.ObserverCount; ++i) {

                char buffer[16];
                std::snprintf(buffer, sizeof(buffer), "Observer%02d", i+1); // Sections should start from 1 (not zero).
                if (!ini.Is_Present(buffer)) {
                    DEBUG_ERROR("Client: Failed to find section for \"%s\"! (expected players %d).\n", buffer, GameSettings.ObserverCount);
                    return false;
                }

                PlayerSettingsType *client_observer = new PlayerSettingsType;

                std::strncpy(client_observer->PlayerName, buffer, sizeof(client_observer->PlayerName));
                std::strncpy(client_observer->IniName, buffer, sizeof(client_observer->IniName));

                ini.Get_String(buffer, "Name", "No name", client_observer->PlayerName, sizeof(client_observer->PlayerName));

                DEBUG_INFO("Client: Processing observer \"%s\" (\"%s\").\n", client_observer->PlayerName, client_observer->IniName);

                /**
                 *  Is this observer the local player (used to set ObserverPlayerPtr)?
                 */
                client_observer->IsLocal = ini.Get_Bool(buffer, "Local", client_observer->IsLocal);

                /**
                 *  Is this observer actually the host of the game?
                 */
                client_observer->IsHost = ini.Get_Bool(buffer, "Host", client_observer->IsHost);

                /**
                 *  
                 */
                client_observer->IsObserver = true;

                /**
                 *  This slot is valid and active.
                 */
                client_observer->IsSlotActive = true;

                /**
                 *  Add the observer player to vector.
                 */
                GameSettings.Players.Add(client_observer);
            }
        }

        /**
         *  Sanity check before we continue.
         */
        if (!GameSettings.Players.Count()) {
            DEBUG_ERROR("Client: Failed to load any player info!\n");
            return false;
        }

        /**
         *  Local player sanity check, there should only be one (either observer or player)!
         */
        int local_count = 0;
        for (int i = 0; i < GameSettings.Players.Count(); ++i) {
            PlayerSettingsType &client_player = *GameSettings.Players[i];
            if (client_player.IsLocal) {
                ++local_count;
            }
        }
        if (local_count > 1) {
            DEBUG_ERROR("Client: More than one local player detected!\n");
            return false;
        }

        /**
         *  Check the for any starting color duplicates.
         */
        for (int i = 0; i < GameSettings.Players.Count(); ++i) {

            PlayerSettingsType &client_player = *GameSettings.Players[i];

            if (!client_player.IsSlotActive) {
                continue;
            }

            /**
             *  Skip observer players.
             */
            if (client_player.IsObserver) {
                continue;
            }

            for (int j = 0; j < GameSettings.Players.Count(); ++j) {

                PlayerSettingsType &existing_player = *GameSettings.Players[j];

                /**
                 *  Skip ourself.
                 */
                if (i == j) {
                    continue;
                }

                /**
                 *  Skip random assignments.
                 */
                if (client_player.ColorScheme == COLORSCHEME_RANDOM || existing_player.ColorScheme == COLORSCHEME_RANDOM) {
                    continue;
                }

                if (client_player.ColorScheme == existing_player.ColorScheme) {
                    DEBUG_ERROR("Client: Duplicate player colors detected (\"%s\" and \"%s\")!\n",
                        client_player.IniName, existing_player.IniName);
                    return false;
                }
            }

        }

        /**
         *  Starting location sanity check for conflicting starting locations.
         */
        for (int i = 0; i < GameSettings.Players.Count(); ++i) {

            PlayerSettingsType &client_player = *GameSettings.Players[i];

            if (!client_player.IsSlotActive) {
                continue;
            }

            /**
             *  Skip observer players.
             */
            if (client_player.IsObserver) {
                continue;
            }

            if (client_player.StartLocation == WAYPOINT_NONE) {
                DEBUG_ERROR("Client: Invalid starting location for \"%s\"!\n", client_player.IniName);
                return false;
            }

            for (int j = 0; j < GameSettings.Players.Count(); ++j) {

                PlayerSettingsType &existing_player = *GameSettings.Players[j];

                /**
                 *  Skip ourself.
                 */
                if (i == j) {
                    continue;
                }

                /**
                 *  Skip random assignments.
                 */
                if (client_player.StartLocation == WAYPT_RANDOM || existing_player.StartLocation == WAYPT_RANDOM) {
                    continue;
                }

                if (client_player.StartLocation == existing_player.StartLocation) {
                    DEBUG_ERROR("Client: Duplicate starting locations detected (\"%s\" and \"%s\")!\n",
                        client_player.IniName, existing_player.IniName);
                    return false;
                }
            }
        }

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
    if (!GameSettings.IsNormalGame && !GameSettings.IsWDTGame) {
        if (!Read_CnCNet_Settings(game_ini)) {
            DEBUG_ERROR("Client: Failed to load CnCNet settings!\n");
            return false;
        }
    }
    if (!Read_Game_Settings(game_ini)) {
        DEBUG_ERROR("Client: Failed to load game settings!\n");
        return false;
    }

    /**
     *  Developer mode will cause an out-of-sync in multiplayer games.
     */
    if (Vinifera_DeveloperMode && (GameSettings.IsMultiplayerGame || GameSettings.IsQuickMatchGame)) {
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
        //DEBUG_WARNING("Client: Unable to find settings!\n");
        return false;
    }

    client_ini.Load(client_file, false);

    /**
     *  Load the client settings.
     */
    if (!Read_Client_Settings(client_ini)) {
        DEBUG_ERROR("Client: Failed to load settings!\n");
        return false;
    }
    
    DEBUG_INFO("Client: System active!\n");

    if (Vinifera_DeveloperMode) {
        DEBUG_INFO("Client: Developer Mode enabled.\n");
    }

    /**
     *  Settings loaded OK, client mode is active.
     */
    IsActive = true;

    /**
     *  
     */
    Vinifera_SkipStartupMovies = Client::IsSkipStartupMovies;
    Vinifera_ExitAfterSkip = Client::IsExitOnGameFinish;

    return true;
}


bool Client::Shutdown()
{
    //CDFileClass(GameSettingsFilename).Delete();
    //CDFileClass(ClientSettingsFilename).Delete();

    Dump_End_Game_Info();

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

    if (GameSettings.IsMultiplayerGame || GameSettings.IsQuickMatchGame) {
        Session.Type = GAME_INTERNET;

    } else if (GameSettings.IsNormalGame) {
        Session.Type = GAME_NORMAL;

    } else if (GameSettings.IsWDTGame) {
        Session.Type = GAME_WDT;
        Session.field_4 = true;

    } else if (GameSettings.IsSkirmishGame) {
        Session.Type = GAME_SKIRMISH;

    } else {
        DEBUG_ERROR("Client: No game mode detected!\n");
        return false;
    }

    if (Session.Type != GAME_NORMAL || Session.Type != GAME_WDT) {

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

        if (SessionExtension) {
            SessionExtension->ExtOptions.IsAutoDeployMCV = GameSettings.IsAutoDeployMCV;
            SessionExtension->ExtOptions.IsPrePlacedConYards = GameSettings.IsPrePlacedConYards;
            SessionExtension->ExtOptions.IsBuildOffAlly = GameSettings.IsBuildOffAlly;
        }

    }

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
    if (Session.Type == GAME_INTERNET || Session.Type == GAME_WDT) {

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

        if (Session.Type == GAME_WDT) {
            // TODO
        }
    }

    /**
     *  Enable firestorm mode if requested.
     */
    if (Session.Type != GAME_NORMAL) {
        if (GameSettings.IsFirestormAddon && Addon_Present()) {

            Addon_4071C0(-1);

            Addon_407190(1);
            Set_Required_Addon(ADDON_FIRESTORM);
        }
    }

    /**
     *  Stop any theme that might be playing (you never know...).
     */
    Theme.Stop();

    /**
     *  Start a campaign mission.
     */
    bool started = false;
    if (Session.Type == GAME_NORMAL) {

        /**
         *  Force init the battle campaigns.
         */
        if (!Campaigns.Count()) {
            Init_Campaigns();
        }

        /**
         *  We was passed in a campaign index.
         */
        if (Campaigns.Count() > 0 && GameSettings.Campaign != CAMPAIGN_NONE) {

            CampaignClass *campaign = Campaigns[GameSettings.Campaign];
            ASSERT(campaign != nullptr);

            //std::strncpy(Scen->ScenarioName, campaign->Scenario, sizeof(Scen->ScenarioName));
            //std::strncpy(Scen->Description, campaign->Description, sizeof(Scen->Description));

            started = Start_Scenario(nullptr, true, GameSettings.Campaign);

        /**
         *  We was passed in a custom scenario name.
         */
        } else {
            std::strncpy(Scen->ScenarioName, GameSettings.ScenarioName, sizeof(Scen->ScenarioName));
            std::strncpy(Scen->Description, GameSettings.Description, sizeof(Scen->Description));

            DEBUG_INFO("Client: About to call Start_Scenario with \"%s\".\n", Scen->ScenarioName);

            started = Start_Scenario(Scen->ScenarioName, true);
        }

    /**
     *  All other game modes.
     */
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
     *  Are we to skip the score screen? We need to do this after the scenario
     *  has been read because the client can override whatever the scenario defines.
     */
    if (Session.Type != GAME_WDT) {
        Scen->IsSkipScore = GameSettings.IsSkipScore;
    }

    /**
     *  Is this a one-time scenario? We need to do this after the scenario
     *  has been read because the client can override whatever the scenario defines.
     */
    if (Session.Type == GAME_NORMAL) {

        if (GameSettings.Campaign == CAMPAIGN_NONE) {
            /**
             *  This is a one-time mission, we can skip the map selection.
             */
            Scen->IsOneTimeOnly = true;
            Scen->IsNoMapSel = true;
            Scen->IsEndOfGame = GameSettings.IsEndOfGame;
        } else {
            Scen->IsOneTimeOnly = false;
            Scen->IsNoMapSel = GameSettings.IsSkipMapSelect;
            Scen->IsEndOfGame = false;
        }
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

    Map.Flag_To_Redraw(2);

    Call_Back();

    /**
     *  Map reveal for observer or allies, multiplayer only.
     */
    if (Session.Type != GAME_NORMAL && Session.Type != GAME_WDT) {

        /**
         *  Reveal the whole map to the observer on game start.
         */
        HouseClassExtension *houseext;
        houseext = HouseClassExtensions.find(PlayerPtr);
        if (houseext && houseext->IsObserver) {

            Map.Reveal_The_Map();

            /**
             *  Set the view position to the center of the map (normally).
             */
            Coordinate coord = Scen->Get_Waypoint_Coord(WAYPT_HOME);
            coord.Z = Map.Get_Cell_Height(coord);
            TacticalMap->Set_Tactical_Position(coord);

        } else {

#if 1
            /**
             *  This is a real kludge to make sure allies are revealed to each other
             *  when the game starts, otherwise only allied units that begin to move
             *  reveal the map (which could be quite some time with AI players).
             */
            for (int j = 0; j < GameSettings.Players.Count(); ++j) {

                PlayerSettingsType &client_player = *GameSettings.Players[j];

                /**
                 *  Skip over ourself.
                 */
                if (PlayerPtr == client_player.HousePtr) {
                    continue;
                }

                if (client_player.HousePtr->Is_Ally(PlayerPtr)) {
                    Map.Sight_From(Coord_Cell(client_player.HousePtr->Center), 9, PlayerPtr, false);
                    //Map.entry_94(Coord_Cell(client_player.HousePtr->Center), PlayerPtr);
                    DEBUG_INFO("  Revealed base of player \"%s\" at %d,%d to player \"%s\".\n",
                        client_player.PlayerName,
                        Coord_Cell(client_player.HousePtr->Center).X, Coord_Cell(client_player.HousePtr->Center).Y,
                        PlayerPtr->IniName);
                }
            }
#endif


#if 0
            for (int i = 0; i < Session.Players.Count(); ++i) {
                HousesType house_type = Get_Client_Node_Tag(i)->Player.ID;
                HouseClass *house = HouseClass::As_Pointer(house_type);

                if (house == PlayerPtr || PlayerPtr->Is_Ally(house)) {
                    Map.Sight_From(Coord_Cell(house->Center), 9, house, false);
                    DEBUG_INFO("  Revealed base of \"%s\" to \"%s\".\n",
                        house->IniName, PlayerPtr->IniName);
                }
            }
#endif

        }

    }

#ifndef NDEBUG
    /**
     *  Halt the game on the first frame for inspection.
     */
    if (Session.Singleplayer_Game()) {
        Vinifera_Developer_FrameStep = !Vinifera_Developer_FrameStep;
        Vinifera_Developer_FrameStepCount = 0;
    }
#endif

    Map.Render();

    return GameActive;
}


/**
 *  Main Loop function for handling any client systems.
 * 
 *  @author: CCHyper
 */
bool Client::Main_Loop()
{
#if 1
    if (IsActive) {

        /**
         *  Update the observer display, unless we're inside a dialog.
         */
        HouseClassExtension *houseext;
        houseext = HouseClassExtensions.find(PlayerPtr);
        if (houseext && houseext->IsObserver) {

            /**
             *  Draw the observer overlay buttons.
             */
            ObserverModeDropList->Draw_Me(true);
            ObserverResetButton->Draw_Me(true);

            if (SpecialDialog == SDLG_NONE && GameInFocus) {

                KeyNumType input;
                //int x;
                //int y;

                //Map.Input(input, x, y);
                //input = GScreenClass::Buttons->Input();

                if (input != KN_NONE) {

                    switch (input) {

                        case KN_BUTTON|GADGET_OBSERVER_DROPDOWN:
                            ObserverOverlayMode = (ObserverOverlayModeType)ObserverModeDropList->Current_Index();
                            break;

                        case KN_BUTTON|GADGET_OBSERVER_RESET:
                            ObserverOverlayMode = OVERLAY_BASIC;
                            break;

                        /**
                         *  Jump to the houses center coord.
                         */
                        case KN_BUTTON|GADGET_OBSERVER_PLAYER_ONE:
                        {
                            Coordinate coord = GameSettings.Players[0]->HousePtr->Center;
                            coord.Z = Map.Get_Cell_Height(coord);
                            TacticalMap->Set_Tactical_Position(coord);
                            break;
                        }

                        case KN_BUTTON|GADGET_OBSERVER_PLAYER_TWO:
                        {
                            Coordinate coord = GameSettings.Players[1]->HousePtr->Center;
                            coord.Z = Map.Get_Cell_Height(coord);
                            TacticalMap->Set_Tactical_Position(coord);
                            break;
                        }

                        case KN_BUTTON|GADGET_OBSERVER_PLAYER_THREE:
                        {
                            Coordinate coord = GameSettings.Players[2]->HousePtr->Center;
                            coord.Z = Map.Get_Cell_Height(coord);
                            TacticalMap->Set_Tactical_Position(coord);
                            break;
                        }

                        case KN_BUTTON|GADGET_OBSERVER_PLAYER_FOUR:
                        {
                            Coordinate coord = GameSettings.Players[3]->HousePtr->Center;
                            coord.Z = Map.Get_Cell_Height(coord);
                            TacticalMap->Set_Tactical_Position(coord);
                            break;
                        }

                        case KN_BUTTON|GADGET_OBSERVER_PLAYER_FIVE:
                        {
                            Coordinate coord = GameSettings.Players[4]->HousePtr->Center;
                            coord.Z = Map.Get_Cell_Height(coord);
                            TacticalMap->Set_Tactical_Position(coord);
                            break;
                        }

                        case KN_BUTTON|GADGET_OBSERVER_PLAYER_SIX:
                        {
                            Coordinate coord = GameSettings.Players[5]->HousePtr->Center;
                            coord.Z = Map.Get_Cell_Height(coord);
                            TacticalMap->Set_Tactical_Position(coord);
                            break;
                        }

                        case KN_BUTTON|GADGET_OBSERVER_PLAYER_SEVEN:
                        {
                            Coordinate coord = GameSettings.Players[6]->HousePtr->Center;
                            coord.Z = Map.Get_Cell_Height(coord);
                            TacticalMap->Set_Tactical_Position(coord);
                            break;
                        }

                        case KN_BUTTON|GADGET_OBSERVER_PLAYER_EIGHT:
                        {
                            Coordinate coord = GameSettings.Players[7]->HousePtr->Center;
                            coord.Z = Map.Get_Cell_Height(coord);
                            TacticalMap->Set_Tactical_Position(coord);
                            break;
                        }

                        default:
                            break;

                    };

                }

            }

            Map.Buttons->Draw_All(true);

            Map.Flag_To_Redraw(2);
        }

    }
#endif

    return true;
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

    /**
     *  Create player nodes.
     */
    for (int i = 0; i < GameSettings.Players.Count(); ++i) {

        PlayerSettingsType &client_player = *GameSettings.Players[i];

        if (!client_player.IsSlotActive) {
            continue;
        }

        ClientNodeNameType *node = new ClientNodeNameType;
        if (!node) {
            return false;
        }

        std::memset(node, 0, sizeof(ClientNodeNameType));

        /**
         *  Set the player house name.
         */
        if (client_player.PlayerName[0] != '\0') {
            std::strncpy(node->Name, client_player.PlayerName, MPLAYER_NAME_MAX-1);
        } else {
            char buffer[MPLAYER_NAME_MAX];
            if (client_player.IsObserver) {
                std::snprintf(buffer, sizeof(buffer), "Observer %d", i);
            } else {
                std::snprintf(buffer, sizeof(buffer), "Player %d", i);
            }
        }

        node->Player.House = client_player.House;
        node->Player.Color = PCOLOR_FIRST;  // This isn't used in Client mode.

        node->Player.IsObserver = client_player.IsObserver;

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
        Add_Client_Node_Tag(node);
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

    /**
     *  Build a list of available colors for the random picker.
     */
    struct ColorPickStruct
    {
        ColorSchemeType Color;     // The color scheme.
        bool Assigned;             // Has this color already been assigned?
    };
    DynamicVectorClass<ColorPickStruct *> random_picked_colors;
    for (ColorSchemeType i = ColorSchemeType(0); i < ColorSchemes.Count(); ++i) {
        if (ColorSchemes[i]->field_310 != 1) {
            ColorPickStruct *pick = new ColorPickStruct;
            pick->Color = i;
            pick->Assigned = false;
            random_picked_colors.Add(pick);
        }
    }
    
    if (Session.Players.Count() > 0) {
        DEBUG_INFO("  Assigning players (%d)...\n", Session.Players.Count()-GameSettings.ObserverCount);
    }

    //if (GameSettings.ObserverCount > 0) {
    //    DEBUG_INFO("  Assigning observers (%d)...\n", GameSettings.ObserverCount);
    //}

    /**
     *  Assign each player in 'Players' to a multiplayer house. Players will
     *  be sorted by their chosen color value (this value must be unique among
     *  all the players).
     */
    for (int i = 0; i < Session.Players.Count(); ++i) {

        ClientNodeNameType &node = *Get_Client_Node_Tag(i);
        PlayerSettingsType &client_player = *GameSettings.Players[i];

        house = node.Player.IsObserver ? HOUSE_OBSERVER : node.Player.House;

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

        if (house == HOUSE_OBSERVER) {
            house = HOUSE_FIRST;
        }

        /**
         *  Update the client player nodes with the chosen house.
         */
        node.Player.House = house;
        client_player.House = house;

        /**
         *  Create the new house instance for this player.
         */
        housep = new HouseClass(HouseTypeClass::As_Pointer(house));
        ASSERT(housep);

        std::memset((char *)housep->IniName, 0, MPLAYER_NAME_MAX);
        std::strncpy((char *)housep->IniName, node.Name, MPLAYER_NAME_MAX-1);

        housep->IsHuman = node.Player.IsObserver ? true : client_player.IsHuman;
        housep->IsPlayerControl = node.Player.IsObserver ? false : client_player.IsPlayerControl;

        housep->Control.TechLevel = node.Player.IsObserver ? -1 : GameSettings.TechLevel;

        color = node.Player.IsObserver ? ColorScheme::From_Name("LightGrey") : client_player.ColorScheme;

        /**
         *  Choose a random color for this player if required.
         */
        if (color == COLORSCHEME_RANDOM) {
            while (true) {
                ColorPickStruct *pick = random_picked_colors[Random_Pick(0, random_picked_colors.Count()-1)];
                if (!pick->Assigned) {
                    pick->Assigned = true;
                    color = pick->Color;
                    break;
                }
            }
            //DEV_DEBUG_INFO("    Random color \"%s\" chosen for \"%s\".\n",
            //    ColorSchemes[color]->Name, client_player.IniName);
        }

        housep->Init_Data((PlayerColorType)node.Player.Color, node.Player.House, Session.Options.Credits);

        housep->RemapColor = color;
        housep->Init_Remap_Color();

        /**
         *  Only computer houses need a handicap.
         */
        if (!housep->IsHuman) {

            housep->IsHuman = false;
            //housep->IsStarted = true;

            if (Session.Type != GAME_NORMAL && Session.Type != GAME_WDT) {
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
        if (client_player.IsLocal) {

            PlayerPtr = housep;
            housep->IsPlayerControl = true;

            /**
             *  If this player slot is for an observer, flag the house instance
             *  so special logic is applied (overlay, sidebar, etc).
             */
            if (/*node.Player.House == HOUSE_OBSERVER &&*/ node.Player.IsObserver) {
                HouseClassExtension *houseext;
                houseext = HouseClassExtensions.find(PlayerPtr);
                if (houseext) {
                    houseext->IsObserver = true;
                }

                housep->IsPlayerControl = false;
            }
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

        if (node.Player.IsObserver) {
            DEBUG_INFO("    Assigned observer \"%s\" to slot %d.\n", node.Name, i);
        } else {
            DEBUG_INFO("    Assigned player \"%s\" (House: \"%s\", ID: %d, Color: \"%s\") to slot %d.\n",
                node.Name, housep->Class->Name(), node.Player.ID, ColorSchemes[housep->RemapColor]->Name, i);
        }
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

        ClientNodeNameType &node = *Get_Client_Node_Tag(i);
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
         *  Observers exist, but not on the map, skip these.
         */
        if (node.Player.IsObserver) {
            continue;
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
            PlayerSettingsType &client_player = *GameSettings.Players[i];

            /**
             *  Observers exist, but not on the map, skip these.
             */
            if (client_player.IsObserver) {
                continue;
            }

            if (client_player.Team == team) {
                DEBUG_INFO("      %s \"%s\"\n", client_player.IniName, client_player.PlayerName);
            }
        }
    }

#if 0
    /**
     *  
     */
    for (int i = 0; i < GameSettings.Observers.Count(); ++i) {

        ObserverSettingsType &client_observer = *GameSettings.Observers[i];

        /**
         *  If this player is an observer and is for myself, set up ObserverPlayerPtr.
         */
        if (((GameSettings.IsMultiplayerGame || GameSettings.IsQuickMatchGame) && client_observer.IsLocal)
          || (GameSettings.IsSkirmishGame && i == 0)) {

            housep = new HouseClass();

            std::memset((char *)housep->IniName, 0, MPLAYER_NAME_MAX);
            std::strncpy((char *)housep->IniName, "Observer", MPLAYER_NAME_MAX-1);

            ObserverPlayerPtr = housep;

            housep->IsHuman = true;
            housep->IsPlayerControl = true;

            housep->Control.TechLevel = -1;

            ColorSchemeType color = ColorScheme::From_Name("LightGrey");

            housep->Init_Data((PlayerColorType)color, HOUSE_FIRST, 0);

            housep->RemapColor = color;
            housep->Init_Remap_Color();

            housep->Assign_Handicap(DIFF_NORMAL);

            DEBUG_INFO("    Assigned observer \"%s\" (House: \"%s\", ID: %d, Color: \"%s\") to slot %d.\n",
                node.Name, housep->Class->Name(), node.Player.ID, ColorSchemes[housep->RemapColor]->Name, i);

            break;
        }

    }
#endif

    /**
     *  Just in case the PCX's have not been loaded, one time load them now.
     */
    static bool _onetime = false;
    if (!_onetime && !SpriteCollection.ImageDictionary.EntryCount) {
        OwnerDraw::Load_Graphics();
        _onetime = true;
    }

    /**
     *  Assign the players with their respective side graphic.
     */
    for (int i = 0; i < GameSettings.Players.Count(); ++i) {

        PlayerSettingsType &client_player = *GameSettings.Players[i];

        if (!client_player.IsSlotActive) {
            continue;
        }

        /**
         *  Skip observer players.
         */
        if (client_player.IsObserver) {
            continue;
        }

        /**
         *  Draw the house icons for the players.
         */
        BSurface *icon_surface = nullptr;

        switch (client_player.House) {      // #TODO: Should be side.
            case 0: // GDI
                icon_surface = SpriteCollection.Get_Image_Surface("gdii.pcx");
                break;
            case 1: // NOD
                icon_surface = SpriteCollection.Get_Image_Surface("nodi.pcx");
                break;
            default:
                icon_surface = SpriteCollection.Get_Image_Surface("obsi.pcx");
                break;
        };

        /**
         *  Copy the loaded graphic into the icon surface.
         */
        if (icon_surface) {
            client_player.IconSurface = new BSurface(icon_surface->Get_Width(),
                                                     icon_surface->Get_Height(),
                                                     icon_surface->Get_Bytes_Per_Pixel());
            client_player.IconSurface->Copy_From(*icon_surface);
        }

    }

    if (!PlayerPtr) {
        DEBUG_ERROR("Client: Local player not assigned!\n");
        return false;
    }

    HouseClassExtension *player_houseext;
    player_houseext = HouseClassExtensions.find(PlayerPtr);
    if (player_houseext) {
        if (GameSettings.ObserverCount > 0 && !player_houseext->IsObserver) {
            DEBUG_ERROR("Client: Local observer not assigned!\n");
            return false;
        }
    }

    DEBUG_INFO("Client: Assign_Houses(exit)\n");

    return true;
}


bool Client::Dump_End_Game_Info()
{
    // TODO 0x005687A0

    return true;
}


const char *Client::Name_From_Team(ClientTeamType team)
{
    static const char *_team_names[TEAM_COUNT] = {
        "Team A", "Team B", "Team C", "Team D",
        "Team E", "Team F", "Team G", "Team H"
    };

    return team != ClientTeamType(-1) && team < TEAM_COUNT ? _team_names[team] : "<invalid>";
}


bool Client::Add_Client_Node_Tag(Client::ClientNodeNameType *node)
{
    return Session.Players.Add((NodeNameType *)node);
}


bool Client::Remove_Client_Node_Tag(Client::ClientNodeNameType *node)
{
    return Session.Players.Delete((NodeNameType *)node);
}


Client::ClientNodeNameType *Client::Get_Client_Node_Tag(int index)
{
    ASSERT(index < Session.Players.Count());
    return (Client::ClientNodeNameType *)Session.Players[index];
}


void Client::Draw_Observer_Overlay()
{
    /**
     *  Text printing colors.
     */
    RGBClass rgb_black(0,0,0);
    unsigned color_black = DSurface::RGBA_To_Pixel(0, 0, 0);
    ColorScheme *white_color = ColorScheme::As_Pointer("White");
    ColorScheme *grey_color = ColorScheme::As_Pointer("Grey");
    ColorScheme *red_color = ColorScheme::As_Pointer("Red");
    ColorScheme *green_color = ColorScheme::As_Pointer("Green");
    

    /**
     *  Text printing styles.
     */
    TextPrintType style = TPF_DROPSHADOW;
    TextPrintType text_font = TPF_3POINT;
    TextPrintType text_style = TextPrintType(text_font|style);
    TextPrintType text_style_left = TextPrintType(text_font|style);
    TextPrintType text_style_right = TextPrintType(text_font|TPF_RIGHT|style);
    TextPrintType text_style_center = TextPrintType(text_font|TPF_CENTER|style);

    static WWFontClass *fontptr = nullptr;
    if (!fontptr) {
        fontptr = Font_Ptr(text_font);
    }
    int font_height = fontptr->Get_Char_Height();


    /**
     *  Various draw positions and sizes.
     */
    int info_y_offset = 16;

    Point2D infopos(TacticalRect.X+4,TacticalRect.Y+4+info_y_offset);      // Start drawing position.

    int text_y_pad = 3;     // Padding

    int row_height = font_height+(text_y_pad*2);       // The height of each player info row.
    int row_text_y_offset = infopos.Y+(row_height*2+text_y_pad);

    //int frame_update_rate = 10;       // Update the information every 'N' frames.

    int house_info_width = 615;


    /**
     *  Draw destination.
     */
    DSurface *draw_surface = CompositeSurface;
    Rect draw_rect = CompositeSurface->Get_Rect();


    /**
     *  Position values for column info.
     * 
     *  @note: Be sure to not go past 472 (640 - Sidebar width) as its the minimum supported width.
     */
    enum {
        COLUMN_PLAYER_ICON_XPOS = 2,

        COLUMN_PLAYER_NAME_XPOS = 20,

        COLUMN_BASIC_CREDITS_XPOS = 180,
        COLUMN_BASIC_HARVESTED_XPOS = 260,
        COLUMN_BASIC_POWER_XPOS = 335,
        COLUMN_BASIC_KILLS_XPOS = 375,
        COLUMN_BASIC_DEATHS_XPOS = 435,
        COLUMN_BASIC_DESTROYED_XPOS = 515,
        COLUMN_BASIC_LOST_XPOS = 560,
        COLUMN_BASIC_SCORE_XPOS = 610,

        COLUMN_ECONOMY_,

        COLUMN_PRODUCTION_,

        COLUMN_SUPPORTPOWERS_,

        COLUMN_COMBAT_UNITSKILLED_XPOS,
        COLUMN_COMBAT_UNITSLOST_XPOS,
        COLUMN_COMBAT_BUILDINGKILLED_XPOS,
        COLUMN_COMBAT_BUILDINGLOST_XPOS,
        COLUMN_COMBAT_ARMYVALUE_XPOS,

        COLUMN_ARMY_,
    };


    /**
     *  Create the observer gadgets.
     */
    static bool _button_onetime = false;
    static char droplist_buffer[32];
    if (!_button_onetime) {

        ObserverModeDropList = new DropListClass(
            GADGET_OBSERVER_DROPDOWN, droplist_buffer, sizeof(droplist_buffer),
            text_style, infopos.X, infopos.Y, 160, row_height,
            MFCC::Retrieve("EBTN-UP.SHP"),
            MFCC::Retrieve("EBTN-DN.SHP")
        );

        ObserverResetButton = new StaticButtonClass(
            GADGET_OBSERVER_RESET, "Reset", text_style, infopos.X+160+4, infopos.Y, 30, row_height);

#ifdef ENFORCE_MAX_PLAYERS
        for (int i = 0; i < Session.Players.Count() && i < MAX_PLAYERS; ++i) {
#else
        for (int i = 0; i < Session.Players.Count(); ++i) {
#endif
            if (!Get_Client_Node_Tag(i)->Player.IsObserver) {
                PlayerButtons[i] = new ControlClass(GADGET_OBSERVER_PLAYER_ONE+i, infopos.X, infopos.Y+(row_height*(i+1)), 50, row_height);
            }
        }

        ObserverModeDropList->Add_Item("None");
        ObserverModeDropList->Add_Item("Basic");
        ObserverModeDropList->Add_Item("Economy");
        ObserverModeDropList->Add_Item("Production");
        ObserverModeDropList->Add_Item("Support Powers");
        ObserverModeDropList->Add_Item("Combat");
        ObserverModeDropList->Add_Item("Army");

        ObserverModeDropList->Set_Selected_Index(OVERLAY_BASIC);

#if 0
        Map.Add_A_Button(*ObserverModeDropList);
        Map.Add_A_Button(*ObserverResetButton);
#ifdef ENFORCE_MAX_PLAYERS
        for (int i = 0; i < Session.Players.Count() && i < MAX_PLAYERS; ++i) {
#else
        for (int i = 0; i < Session.Players.Count(); ++i) {
#endif
            if (!Get_Client_Node_Tag(i)->Player.IsObserver) {
                Map.Add_A_Button(*PlayerButtons[i]);
            }
        }
#endif

        _button_onetime = true;
    }


    /**
     *  Depending on the overlay mode, draw the required column text.
     */
    switch (ObserverOverlayMode) {

        /**
         *  Draw nothing.
         */
        case OVERLAY_NONE:
        {
            break;
        }

        /**
         *  
         */
        case OVERLAY_BASIC:
        {
            Rect column_background(
                infopos.X,
                infopos.Y,
                house_info_width,
                row_height
                );

            draw_surface->Fill_Rect_Trans(column_background, rgb_black, 50);

            /**
             *  Column titles
             */
            Fancy_Text_Print("Player",
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_PLAYER_NAME_XPOS, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style);

            Fancy_Text_Print("Credits",
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_CREDITS_XPOS, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Harvested",
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_HARVESTED_XPOS, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Power",
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_POWER_XPOS, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Kills",
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_KILLS_XPOS, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Deaths",
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_DEATHS_XPOS, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Destroyed",
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_DESTROYED_XPOS, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Lost",
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_LOST_XPOS, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Score",
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_SCORE_XPOS, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            /**
             *  
             */

            break;
        }

        /**
         *  
         */
        case OVERLAY_ECONOMY:
        {
            break;
        }

        /**
         *  
         */
        case OVERLAY_PRODUCTION:
        {
            break;
        }

        /**
         *  
         */
        case OVERLAY_SUPPORTPOWERS:
        {
            break;
        }

        /**
         *  
         */
        case OVERLAY_COMBAT:
        {
            Rect column_background(
                infopos.X,
                infopos.Y,
                house_info_width,
                row_height
                );

            CompositeSurface->Fill_Rect_Trans(column_background, rgb_black, 50);

            /**
             *  Column titles
             */
            Fancy_Text_Print("Player",
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_PLAYER_NAME_XPOS, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style);

            Fancy_Text_Print("Units Killed",
                draw_surface, &draw_rect, &Point2D(infopos.X+65, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Units Lost",
                draw_surface, &draw_rect, &Point2D(infopos.X+85, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Bldg Killed",
                draw_surface, &draw_rect, &Point2D(infopos.X+105, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Bldg Lost",
                draw_surface, &draw_rect, &Point2D(infopos.X+125, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            Fancy_Text_Print("Army Value",
                draw_surface, &draw_rect, &Point2D(infopos.X+145, infopos.Y+text_y_pad),
                white_color, COLOR_TBLACK, text_style_right);

            break;
        }

        /**
         *  
         */
        case OVERLAY_ARMY:
        {
            break;
        }
        
        /**
         *  
         */
        case OVERLAY_CONNECTION:
        {
            break; // Ping, RTT (round trip time), packet loss
        }

    };

    
    /**
     *  Scratch buffer for anything.
     */
    char scratch_buffer[256];

    /**
     *  Draw the player info.
     */
    for (int i = 0; i < GameSettings.Players.Count(); ++i) {

        PlayerSettingsType &client_player = *GameSettings.Players[i];

        if (!client_player.IsSlotActive) {
            continue;
        }

        /**
         *  Observers are not real players.
         */
        if (client_player.IsObserver) {
            continue;
        }

        HouseClass *housep = client_player.HousePtr;

        int row_text_y_pos = infopos.Y+(row_height*(i+1))+text_y_pad;


        /**
         *  Draw the player info background.
         */
        Rect house_rect(
            infopos.X,
            infopos.Y+(row_height*(i+1)),
            house_info_width,
            row_height
            );

        unsigned house_color = DSurface::RGB_To_Pixel(
            housep->RemapColorRGB.Red,
            housep->RemapColorRGB.Green,
            housep->RemapColorRGB.Blue);

        CompositeSurface->Fill_Rect_Trans(house_rect, housep->RemapColorRGB, 50);


        /**
         *  Draw the player name.
         */
        if (!housep->IsDefeated) {
            Fancy_Text_Print(client_player.PlayerName,
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_PLAYER_NAME_XPOS, infopos.Y+(row_height*(i+1)+text_y_pad)),
                white_color, COLOR_TBLACK, text_style);
        } else {
            char buffer[32];
            std::snprintf(buffer, sizeof(buffer), "%s", client_player.PlayerName);
            Fancy_Text_Print(buffer,
                draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_PLAYER_NAME_XPOS, infopos.Y+(row_height*(i+1)+text_y_pad)),
                grey_color, COLOR_TBLACK, text_style);
        }


        /**
         *  Depending on the overlay mode, draw the required information.
         */
        switch (ObserverOverlayMode) {

            /**
             *  Draw nothing.
             */
            case OVERLAY_NONE:
            {
                break;
            }

            /**
             *  
             */
            case OVERLAY_BASIC:
            {//Cash, Power, Kills, Deaths, Destroyed, Lost, Score, APM
                /**
                 *  Credits
                 */
                Fancy_Text_Print("$%d",
                    draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_CREDITS_XPOS, row_text_y_pos),
                    (housep->Credits <= 0) ? red_color : white_color,
                    COLOR_TBLACK, text_style_right, housep->Credits);

                /**
                 *  Harvested
                 */
                Fancy_Text_Print("$%d",
                    draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_HARVESTED_XPOS, row_text_y_pos),
                    white_color,
                    COLOR_TBLACK, text_style_right, housep->Tiberium.Get_Total_Value());
                
                /**
                 *  Power
                 */
                Fancy_Text_Print("%d/%d",
                    draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_POWER_XPOS, row_text_y_pos),
                    (housep->Power == 0 && housep->Drain == 0) ? white_color : (housep->Drain <= housep->Power) ? green_color : red_color,
                    COLOR_TBLACK, text_style_right, housep->Drain, housep->Power);

                /**
                 *  Kills
                 */
                int kills_total = 0;
#ifdef ENFORCE_MAX_PLAYERS
                for (int index = 0; index < Session.Players.Count() /*&& h < MAX_PLAYERS*/; ++index) {
#else
                for (int index = 0; index < Session.Players.Count(); ++index) {
#endif
                    if (Get_Client_Node_Tag(index)->Player.IsObserver) continue;

                    kills_total += housep->UnitsKilled[index];
                    kills_total += housep->BuildingsKilled[index];
                }
                Fancy_Text_Print("%d",
                    draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_KILLS_XPOS, row_text_y_pos),
                    white_color, COLOR_TBLACK, text_style_right, kills_total);

                /**
                 *  Deaths
                 */
                /*int deaths_total = housep->DestroyedAircraft->Get_Total_Of_All()
                                    + housep->DestroyedInfantry->Get_Total_Of_All()
                                    + housep->DestroyedUnits->Get_Total_Of_All()
                                    + housep->DestroyedBuildings->Get_Total_Of_All();
                Fancy_Text_Print("%d",
                    draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_DEATHS_XPOS, row_text_y_pos),
                    white_color, COLOR_TBLACK, text_style_right, deaths_total);*/

                /**
                 *  Destroyed
                 */
                int destroyed_total = housep->DestroyedAircraft->Get_Total_Of_All()
                                    + housep->DestroyedInfantry->Get_Total_Of_All()
                                    + housep->DestroyedUnits->Get_Total_Of_All()
                                    + housep->DestroyedBuildings->Get_Total_Of_All();
                Fancy_Text_Print("%d",
                    draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_DESTROYED_XPOS, row_text_y_pos),
                    white_color, COLOR_TBLACK, text_style_right, destroyed_total);
                
                /**
                 *  Lost
                 */
                int lost_total = housep->UnitsLost + housep->BuildingsLost;
                Fancy_Text_Print("%d",
                    draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_LOST_XPOS, row_text_y_pos),
                    white_color, COLOR_TBLACK, text_style_right, lost_total);

                /**
                 *  Score
                 */
                Fancy_Text_Print("%d",
                    draw_surface, &draw_rect, &Point2D(infopos.X+COLUMN_BASIC_SCORE_XPOS, row_text_y_pos),
                    white_color, COLOR_TBLACK, text_style_right, housep->PointTotal);

                break;
            }

            /**
             *  
             */
            case OVERLAY_ECONOMY:
            {
                break;
            }

            /**
             *  
             */
            case OVERLAY_PRODUCTION:
            {
                break;
            }

            /**
             *  
             */
            case OVERLAY_SUPPORTPOWERS:
            {
                break;
            }

            /**
             *  
             */
            case OVERLAY_COMBAT:
            {
                Fancy_Text_Print("Units Killed",
                    CompositeSurface, &draw_rect, &Point2D(infopos.X+65, infopos.Y+text_y_pad),
                    white_color, COLOR_TBLACK, TextPrintType(TPF_3POINT|TPF_RIGHT|TPF_DROPSHADOW));

                Fancy_Text_Print("Units Lost",
                    CompositeSurface, &draw_rect, &Point2D(infopos.X+85, infopos.Y+text_y_pad),
                    white_color, COLOR_TBLACK, TextPrintType(TPF_3POINT|TPF_RIGHT|TPF_DROPSHADOW));

                Fancy_Text_Print("Bldg Killed",
                    CompositeSurface, &draw_rect, &Point2D(infopos.X+105, infopos.Y+text_y_pad),
                    white_color, COLOR_TBLACK, TextPrintType(TPF_3POINT|TPF_RIGHT|TPF_DROPSHADOW));

                Fancy_Text_Print("Bldg Lost",
                    CompositeSurface, &draw_rect, &Point2D(infopos.X+125, infopos.Y+text_y_pad),
                    white_color, COLOR_TBLACK, TextPrintType(TPF_3POINT|TPF_RIGHT|TPF_DROPSHADOW));

                Fancy_Text_Print("Army Value",
                    CompositeSurface, &draw_rect, &Point2D(infopos.X+145, infopos.Y+text_y_pad),
                    white_color, COLOR_TBLACK, TextPrintType(TPF_3POINT|TPF_RIGHT|TPF_DROPSHADOW));

                break;
            }

            /**
             *  
             */
            case OVERLAY_ARMY:
            {
                break;
            }
        
            /**
             *  
             */
            case OVERLAY_CONNECTION:
            {
                break; // Ping, RTT (round trip time), packet loss
            }

        };

        /**
         *  Draw the side icon for this player house.
         */
        if (client_player.IconSurface) {
            Rect icon_rect(infopos.X+COLUMN_PLAYER_ICON_XPOS,
                           infopos.Y+(row_height*(i+1)+((row_height-client_player.IconSurface->Height)/2)),
                           client_player.IconSurface->Get_Width(),
                           client_player.IconSurface->Get_Height());

            SpriteCollection.Draw_Trans(icon_rect, *draw_surface, *client_player.IconSurface);
        }

    }

}
