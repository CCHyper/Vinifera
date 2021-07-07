/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CLIENT_GLOBALS.H
 *
 *  @author        CCHyper
 *
 *  @brief         Various globals for the client front-end system.
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
#pragma once

#include "always.h"
#include "cncnet5_globals.h"
#include "tibsun_defines.h"


class RawFileClass;
class CCINIClass;
class HouseClass;


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


/**
 *  Pick a random house.
 */
#define HOUSE_RANDOM HousesType(-2)
#define HOUSE_OBSERVER HousesType(-3)

/**
 *  Pick a random color.
 */
#define COLORSCHEME_RANDOM ColorSchemeType(-2)

/**
 *  Pick a random waypoint.
 */
#define WAYPT_NONE WaypointEnum(-1)
#define WAYPT_RANDOM WaypointEnum(-2)

namespace Client
{

typedef enum ClientTeamType
{
    TEAM_A,
    TEAM_B,
    TEAM_C,
    TEAM_D,
    TEAM_E,
    TEAM_F,
    TEAM_G,
    TEAM_H,

    TEAM_COUNT
} ClientTeamType;


typedef struct PlayerSettingsType
{
    PlayerSettingsType() :
        IsSlotActive(false),
        IsHost(false),
        IsLocal(false),
        IsHuman(false),
        IsPlayerControl(false),
        //IsObserver(false),
        Side(SIDE_NONE),
        House(HOUSE_NONE),
        Difficulty(DIFF_NORMAL),
        StartLocation(WAYPT_RANDOM),
        Team(ClientTeamType(-1)),
        ColorScheme(COLORSCHEME_NONE),
        Port(-1),
        HousePtr(nullptr)
    {
        std::memset(PlayerName, 0, sizeof(PlayerName));
        std::memset(IniName, 0, sizeof(IniName));
        std::memset(Allies, 0, sizeof(Allies));
        std::memset(Address, 0, sizeof(Address));
    }

    ~PlayerSettingsType()
    {
    }

    /**
     *  Player handle.
     */
    char PlayerName[16];

    /**
     *  The ini name.
     */
    char IniName[16];

    /**
     *  Is this slot active?
     */
    bool IsSlotActive;

    /**
     *  Is this player the host? (otherwise client).
     */
    bool IsHost;

    /**
     *  Is this player... me?
     */
    bool IsLocal;

    /**
     *  Is this player human? (will be an AI otherwise).
     */
    bool IsHuman;
    
    /**
     *  
     */
    bool IsPlayerControl;

    /**
     *  
     */
    //bool IsObserver;

    SideType Side;
    HousesType House;

    /**
     *  Difficulty level (AI only).
     */
    DiffType Difficulty;

    /**
     *  Starting location waypoint cell.
     */
    WaypointEnum StartLocation;

    /**
     *  Houses I am allied with.
     */
    //unsigned Allies;
    char Allies[256];

    /**
     *  What team am I assigned to?
     */
    ClientTeamType Team;

    /**
     *  Player colour scheme.
     */
    ColorSchemeType ColorScheme;

    /**
     *  Network settings.
     */
    char Address[128];
    unsigned Port;

    /**
     *  Pointer to the created house instance.
     * 
     *  This allows us to setup the alliances and also fetch all sorts
     *  of useful information during the game (likes stats etc.)
     */
    HouseClass *HousePtr;
};


typedef struct ObserverSettingsType
{
    ObserverSettingsType() :
        IsSlotActive(false),
        IsHost(false),
        IsLocal(false),
        Port(-1),
        HousePtr(nullptr)
    {
        std::memset(Name, 0, sizeof(Name));
        std::memset(Address, 0, sizeof(Address));
    }

    ~ObserverSettingsType()
    {
    }

    /**
     *  Observer handle.
     */
    char Name[16];

    /**
     *  Is this slot active?
     */
    bool IsSlotActive;

    /**
     *  Is this observer the host? (otherwise client).
     */
    bool IsHost;

    /**
     *  Is this observer... me?
     */
    bool IsLocal;

    /**
     *  Network settings.
     */
    char Address[128];
    unsigned Port;

    /**
     *  Pointer to the created observer house instance.
     */
    HouseClass *HousePtr;
};


typedef struct GameSettingsType
{
    GameSettingsType() :
        Players(),
        Observers(),
        IsMultiplayerGame(false),
        IsNormalGame(false),
        IsSkirmishGame(false),
        IsFirestormAddon(false),
        Campaign(CAMPAIGN_NONE),
        IsBases(false),
        IsShortGame(false),
        IsMultiEngineer(false),
        IsMCVRedeploy(false),
        IsCrateGoodies(false),
        IsBridgeDestruction(false),
        IsTiberiumGrows(false),
        IsFogOfWar(false),
        IsShroudRegrows(false),
        IsHarvesterTruce(false),
        IsAlliesAllowed(false),
        Credits(0),
        TechLevel(0),
        UnitCount(0),
        PlayerCount(0),
        AIPlayerCount(0),
        ObserverCount(0),
        GameSpeed(0),
        MaxAhead(0),
        FrameSendRate(0),
        CommProtocol(0),
        Seed(0)
    {
        std::memset(ScenarioName, 0, sizeof(ScenarioName));
        std::memset(Description, 0, sizeof(Description));
    }

    ~GameSettingsType()
    {
        Players.Clear();
        Observers.Clear();
    }

    /**
     *  Vector of player settings data.
     * 
     *  #WARNING:
     *  The order in which player info is added to this vector will
     *  be mirrored when creating the player nodes!
     */
    DynamicVectorClass<PlayerSettingsType *> Players;

    /**
     *  Vector of player settings data.
     * 
     *  #WARNING:
     *  The order in which player info is added to this vector will
     *  be mirrored when creating the player nodes!
     */
    DynamicVectorClass<ObserverSettingsType *> Observers;

    /**
     *  Scenario filename.
     */
    char ScenarioName[256];

    /**
     *  Scenario description name from the client.
     */
    char Description[256];

    /**
     *  Game options.
     */
    bool IsMultiplayerGame;     // Is this a online multiplayer game?
    bool IsNormalGame;          // Is this a campaign game?
    bool IsSkirmishGame;        // Is this a skirmish game?

    bool IsFirestormAddon;      // Is the Firestorm addon enabled?

    /**
     *  Campaign index for singleplayer.
     */
    CampaignType Campaign;

    /**
     *  Game session options.
     */
    bool IsBases;               // Is base building enabled?
    bool IsShortGame;           // Is this a "short game"?
    bool IsMultiEngineer;       // Is the multi engineer logic enabled?
    bool IsMCVRedeploy;         // Can the player undeploy/redeploy the mcv?
    bool IsCrateGoodies;        // Are goodie/bonus crates enabled?
    bool IsBridgeDestruction;   // Can bridges be destroyed?
    bool IsTiberiumGrows;       // Does tiberium regenerate?
    bool IsFogOfWar;            // Is "fog of war" enabled?
    bool IsShroudRegrows;       // Does the shoud regrow over time?
    bool IsHarvesterTruce;      // Harvesters are immune from damage?
    bool IsAlliesAllowed;       // Are alliances allowed to be created/broken during the game?
    bool IsFixedAlliance;       // 

    unsigned Credits;
    unsigned TechLevel;
    unsigned UnitCount;
    unsigned PlayerCount;
    unsigned AIPlayerCount;
    unsigned ObserverCount;

    /**
     *  From OPTIONS.H
     *  
     *  This is actually the delay between game frames expressed as 1/60 of
     *  a second. The default value is 4 (1/15 second).
     */
    unsigned GameSpeed;

    unsigned MaxAhead;
    unsigned FrameSendRate;
    unsigned CommProtocol;

    /**
     *  Randomiser seed, should be the same for all players!
     */
    long Seed;
};

/**
 *  Is the client mode active?
 */
extern bool IsActive;

/**
 *  Can the game be run from the client only?
 */
extern bool IsRunFromClientOnly;

/**
 *  Quit the game when the game has finished? (otherwise it will return the main menu).
 */
extern bool IsExitOnGameFinish;

/**
 *  Skip the startup movies?
 */
extern bool IsSkipStartupMovies;

/**
 *  The CNCNet game id. This is only used in startup checks, the settings ini
 *  should set the correct game id, and it should be identical to this for debugging purposes.
 */
extern int GameID;

/**
 *  This is the observer house that the local human player is currently "playing" as.
 */
extern HouseClass *ObserverPlayerPtr;

/**
 *  Scenario file instance.
 */
extern RawFileClass ScenarioFile;
extern CCINIClass ScenarioINI;

/**
 *  Settings file instance.
 */
extern RawFileClass SettingsFile;
extern CCINIClass SettingsINI;

extern const char *GameSettingsFilename;
extern const char *ClientSettingsFilename;

/**
 *  Game settings and options.
 */
extern GameSettingsType GameSettings;

}; // namespace Client
