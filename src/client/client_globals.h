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
#include "tibsun_defines.h"
#include "cncnet5_globals.h"


class RawFileClass;
class CCINIClass;
class HouseClass;


/**
 *  Pick a random house.
 */
#define HOUSE_RANDOM HousesType(-2)

/**
 *  Pick a random color.
 */
#define COLORSCHEME_RANDOM ColorSchemeType(-2)

/**
 *  Pick a random waypoint.
 */
#define WAYPT_RANDOM WaypointEnum(-2)


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


typedef struct ClientPlayerSettingsType
{
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

typedef struct ClientGameSettingsType
{
    ClientGameSettingsType() :
        Players(),
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
        GameSpeed(0),
        MaxAhead(0),
        FrameSendRate(0),
        CommProtocol(0),
        Seed(0)
    {
        std::memset(ScenarioName, 0, sizeof(ScenarioName));
        std::memset(Description, 0, sizeof(Description));
    }

    ~ClientGameSettingsType()
    {
        Players.Clear();
    }

    /**
     *  Vector of player settings data.
     * 
     *  #WARNING:
     *  The order in which player info is added to this vector will
     *  be mirrored when creating the player nodes!
     */
    DynamicVectorClass<ClientPlayerSettingsType *> Players;

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


namespace Client
{

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
 *  Scenario file instance.
 */
extern RawFileClass ScenarioFile;
extern CCINIClass ScenarioINI;

/**
 *  Settings file instance.
 */
extern RawFileClass SettingsFile;
extern CCINIClass SettingsINI;

/**
 *  Game settings and options.
 */
extern ClientGameSettingsType GameSettings;

bool Parse_Command_Line(int argc, char *argv[]);

bool Read_CnCNet_Settings(CCINIClass &ini);
bool Read_Client_Settings(CCINIClass &ini);
bool Read_Game_Settings(CCINIClass &ini);

bool Init();
bool Shutdown();

bool Start_Game();

bool Create_Player_Nodes();
bool Assign_Houses();

const char *Name_From_Team(ClientTeamType team);

}; // namespace Client
