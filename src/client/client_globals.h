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
#include "session.h"


class RawFileClass;
class CCINIClass;
class HouseClass;
class DropListClass;
class StaticButtonClass;
class ControlClass;
class BSurface;


/**
 *  Enforce the 8 player limit?
 */
//#define ENFORCE_MAX_PLAYERS


#define TXT_CLIENT_RUN_ERROR            "Failed to launch a client game due to invalid settings!\n" \
                                        "\nThe game will now exit.\n"

#ifndef NDEBUG
#define TXT_CLIENT_DEBUGGER            "Attach debugger now or continue!\n"
#endif

#define TXT_CLIENT_INALIDSETTINGS       "Client: Invalid settings file!\n"
#define TXT_CLIENT_INALIDSCENARIO       "Client: Invalid scenario file!\n"
#define TXT_CLIENT_INALIDLAUNCH         "Client: Invalid launch mode!\n"
#define TXT_CLIENT_INALIDGAMEID         "Client: Invalid game id!\n"

#define CLIENT_RESOURCES_DIR            ".\\Resources\\"
#define CLIENT_SETTINGS_FILENAME        "VINIFERA.INI"


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


typedef enum ObserverOverlayModeType
{
    OVERLAY_NONE,
    OVERLAY_BASIC,
    OVERLAY_ECONOMY,
    OVERLAY_PRODUCTION,
    OVERLAY_SUPPORTPOWERS,
    OVERLAY_COMBAT,
    OVERLAY_ARMY,
    OVERLAY_CONNECTION,
} ObserverOverlayModeType;


typedef struct PlayerSettingsType
{
    PlayerSettingsType() :
        IsSlotActive(false),
        IsHost(false),
        IsLocal(false),
        IsHuman(false),
        IsPlayerControl(false),
        IsObserver(false),
        Side(SIDE_NONE),
        House(HOUSE_NONE),
        Difficulty(DIFF_NORMAL),
        StartLocation(WAYPT_RANDOM),
        Team(ClientTeamType(-1)),
        ColorScheme(COLORSCHEME_NONE),
        Port(-1),
        HousePtr(nullptr),
        IconSurface(nullptr)
    {
        std::memset(PlayerName, 0, sizeof(PlayerName));
        std::memset(IniName, 0, sizeof(IniName));
        std::memset(Allies, 0, sizeof(Allies));
        std::memset(Address, 0, sizeof(Address));
    }

    ~PlayerSettingsType()
    {
        //delete HousePtr;
        HousePtr = nullptr;

        delete IconSurface;
        IconSurface = nullptr;
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
    bool IsObserver;

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

    /**
     *  
     */
    BSurface *IconSurface;
};


#if 0
typedef struct ObserverSettingsType
{
    ObserverSettingsType() :
        IsSlotActive(false),
        IsHost(false),
        IsLocal(false),
        Port(-1),
        HousePtr(nullptr)
    {
        std::memset(PlayerName, 0, sizeof(PlayerName));
        std::memset(Address, 0, sizeof(Address));
    }

    ~ObserverSettingsType()
    {
    }

    /**
     *  Observer handle.
     */
    char PlayerName[16];

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
#endif


typedef struct GameSettingsType
{
    GameSettingsType() :
        Players(),
        //Observers(),
        IsWDTGame(false),
        IsQuickMatchGame(false),
        IsMultiplayerGame(false),
        IsNormalGame(true),
        IsSkirmishGame(false),
        IsFirestormAddon(false),
        Campaign(CAMPAIGN_NONE),
        PlayerDifficulty(DIFF_NORMAL),
        ComputerDifficulty(DIFF_NORMAL),
        IsOneTimeOnly(false),
        IsSkipScore(false),
        IsSkipMapSelect(false),
        IsEndOfGame(false),
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
        Seed(0),
        ScenarioIsMixfile(false)
    {
        std::memset(ScenarioName, 0, sizeof(ScenarioName));
        std::memset(Description, 0, sizeof(Description));
        std::memset(LoadingScreenPrefix, 0, sizeof(LoadingScreenPrefix));
    }

    ~GameSettingsType()
    {
        Players.Clear();
        //Observers.Clear();
    }

    /**
     *  Vector of player settings data.
     * 
     *  #WARNING:
     *  The order in which player info is added to this vector will
     *  be mirrored when creating the player nodes!
     */
    DynamicVectorClass<PlayerSettingsType *> Players;

#if 0
    /**
     *  Vector of player settings data.
     * 
     *  #WARNING:
     *  The order in which player info is added to this vector will
     *  be mirrored when creating the player nodes!
     */
    DynamicVectorClass<ObserverSettingsType *> Observers;
#endif

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
    bool IsWDTGame;             // Is this a World Domination Tour game?
    bool IsQuickMatchGame;      // Is this a online quick-match game?
    bool IsMultiplayerGame;     // Is this a online multiplayer game?
    bool IsNormalGame;          // Is this a campaign game?
    bool IsSkirmishGame;        // Is this a skirmish game?

    bool IsFirestormAddon;      // Is the Firestorm addon enabled?

    /**
     *  Campaign index for singleplayer campaign.
     */
    CampaignType Campaign;

    /**
     *  This is the difficulty setting of the game (Only used for campaigns).
     */
    DiffType PlayerDifficulty;     // For human player.
    DiffType ComputerDifficulty;   // For computer players.

    /**
     *  If this is to be a one time only mission such that when it is completed
     */
    bool IsOneTimeOnly;

    /**
     *  If the score screen (and "mission accomplished" voice) is to be skipped when
     *  this scenario is finished, then this flag will be true.
     */
    bool IsSkipScore;

    /**
     *  If the map selection is to be skipped then this flag will be true. If this
     *  isn't a one time only scenario, then the next scenario will have the same
     *  name as the current one but will be for variation "B".
     */
    bool IsSkipMapSelect;

    /**
     *  If this scenario is to be the last mission of the game (for this side), then
     *  this is the ending of the game, print the credits and exit.
     */
    bool IsEndOfGame;

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
     *  Extended game session options.
     */
    bool IsAutoDeployMCV;
    bool IsPrePlacedConYards;
    bool IsBuildOffAlly;

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
     *  World Domination Tour settings
     */
    unsigned WDTHost[128];
    unsigned WDTPort;

    /**
     *  
     */
    char LoadingScreenPrefix[32];

    /**
     *  Randomiser seed, should be the same for all players!
     */
    long Seed;

    /**
     *  
     */
    bool ScenarioIsMixfile;
};


/**
 *  Copy of the NodeNameTag struct from session.h with added members.
 */
#pragma pack(1)
typedef struct ClientNodeNameTag
{
    char Name[MPLAYER_NAME_MAX];
    IPXAddressClass Address;
    union {
        struct {
            unsigned char IsOpen;
            unsigned char Addon;
            unsigned long LastTime;
        } Game;
        struct {
            char field_1E[23];
            HousesType House;
            PlayerColorType Color;
            HousesType ID;
            int ProcessTime;
            int field_45;
            int Clan;
            int IsObserver;             // Added for Client use.
        } Player;
        struct {
            unsigned long LastTime;
            unsigned char LastChance;
            PlayerColorType Color;
        } Chat;
    };
} ClientNodeNameType;
#pragma pack()


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
//extern HouseClass *ObserverPlayerPtr;

/**
 *  
 */
extern ObserverOverlayModeType ObserverOverlayMode;

/**
 *  
 */
extern DropListClass *ObserverModeDropList;
extern StaticButtonClass *ObserverResetButton;
extern ControlClass *PlayerButtons[MAX_PLAYERS];

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

extern char ClientStartupSettingsFilename[PATH_MAX];
extern char GameSettingsFilename[PATH_MAX];
extern char ClientSettingsFilename[PATH_MAX];

/**
 *  Startup settings.
 */
extern bool IsDeveloperModeEnabled;
extern bool IsDebugConsoleEnabled;

/**
 *  Game settings and options.
 */
extern GameSettingsType GameSettings;

}; // namespace Client
