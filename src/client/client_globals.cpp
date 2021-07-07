/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CLIENT_GLOBALS.CPP
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
#include "client_globals.h"
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


bool Client::IsActive = false;
bool Client::IsRunFromClientOnly = false;
bool Client::IsExitOnGameFinish = false;
bool Client::IsSkipStartupMovies = false;

int Client::GameID = -1;

//HouseClass *Client::ObserverPlayerPtr = nullptr;
Client::ObserverOverlayModeType Client::ObserverOverlayMode = Client::OVERLAY_BASIC;

DropListClass *Client::ObserverModeDropList = nullptr;
StaticButtonClass *Client::ObserverResetButton = nullptr;
ControlClass *Client::PlayerButtons[MAX_PLAYERS];

RawFileClass Client::ScenarioFile;
CCINIClass Client::ScenarioINI;

RawFileClass Client::SettingsFile;
CCINIClass Client::SettingsINI;

#ifndef NDEBUG
char Client::ClientStartupSettingsFilename[PATH_MAX] = { "CLIENT_STARTUP_SETTINGS.INI" };
char Client::ClientSettingsFilename[PATH_MAX] = { "CLIENT_SETTINGS.INI" };
//char Client::GameSettingsFilename[PATH_MAX] = { "SKIRMISH_GAME.INI" };
char Client::GameSettingsFilename[PATH_MAX] = { "CAMPAIGN_GAME.INI" };
//char Client::GameSettingsFilename[PATH_MAX] = { "MULTIPLAYER_GAME.INI" };
//char Client::GameSettingsFilename[PATH_MAX] = { "WDT_GAME.INI" };
//char Client::GameSettingsFilename[PATH_MAX] = { "QM_GAME.INI" };
#else
char Client::ClientStartupSettingsFilename[PATH_MAX] = { CLIENT_RESOURCES_DIR CLIENT_SETTINGS_FILENAME };
char Client::ClientSettingsFilename[PATH_MAX] = { '\0' };
char Client::GameSettingsFilename[PATH_MAX] = { '\0' };
#endif

bool Client::IsDeveloperModeEnabled = false;
bool Client::IsDebugConsoleEnabled = false;

Client::GameSettingsType Client::GameSettings;
