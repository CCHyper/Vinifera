/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_GLOBALS.H
 *
 *  @authors       CCHyper
 *
 *  @brief         Vinifera global values.
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


extern bool Vinifera_DeveloperMode;

extern char Vinifera_DebugDirectory[PATH_MAX];
extern char Vinifera_ScreenshotDirectory[PATH_MAX];


/**
 *  Defines and constants.
 */
#define TEXT_SCRNCAP				"Screen Capture"
#define TEXT_SCRNCAP_INI			"ScreenCapture"
#define TEXT_SCRNCAP_DESC			"Takes a snapshot of the game screen (Saved as 'SCRN_<date-time>.PNG.)"
#define TEXT_PLACEBLD				"Place Building"
#define TEXT_PLACEBLD_INI			"ManualPlace"
#define TEXT_PLACEBLD_DESC			"Enter the manual placement mode when a building is complete and pending on the sidebar."
#define TEXT_PREVTHEME				"Music: Previous Track"
#define TEXT_PREVTHEME_INI			"PrevTheme"
#define TEXT_PREVTHEME_DESC			"Play the previous music track in the jukebox."
#define TEXT_NEXTTHEME				"Music: Next Track"
#define TEXT_NEXTTHEME_INI			"NextTheme"
#define TEXT_NEXTTHEME_DESC			"Play the next music track in the jukebox."
#define TEXT_SCROLL_NE				"Scroll North-East"
#define TEXT_SCROLL_NE_INI			"ScrollNorthEast"
#define TEXT_SCROLL_NE_DESC			"Scroll tactical map to the north-east."
#define TEXT_SCROLL_SE				"Scroll South-East"
#define TEXT_SCROLL_SE_INI			"ScrollSouthEast"
#define TEXT_SCROLL_SE_DESC			"Scroll tactical map to the south-east."
#define TEXT_SCROLL_SW				"Scroll South-West"
#define TEXT_SCROLL_SW_INI			"ScrollSouthWest"
#define TEXT_SCROLL_SW_DESC			"Scroll tactical map to the south-west."
#define TEXT_SCROLL_NW				"Scroll North-West"
#define TEXT_SCROLL_NW_INI			"ScrollNorthWest"
#define TEXT_SCROLL_NW_DESC			"Scroll tactical map to the north-west."
#define TEXT_MEMDUMP				"Memory Dump"
#define TEXT_MEMDUMP_INI			"MemoryDump"
#define TEXT_MEMDUMP_DESC			"Produces a mini-dump of the memory for analysis."
#define TEXT_		"Dump Heap CRCs"
#define TEXT_?_INI		"DumpHeapCRC"
#define TEXT_?_DESC			"Dumps all the current game objects as CRCs to the log output."
#define TEXT_		"Instant Build (Player)"
#define TEXT_?_INI		"InstantBuild"
#define TEXT_?_DESC			"Toggles the instant build cheat for the player."
#define TEXT_		"Instant Build (AI)"
#define TEXT_?_INI		"AIInstantBuild"
#define TEXT_?_DESC			"Toggles the instant build cheat for the AI."
#define TEXT_TOWIN					"To Win"
#define TEXT_TOWIN_INI				"ForceWin"
#define TEXT_TOWIN_DESC				"Forces the player to win the current game session."
#define TEXT_TOLOSE					"To Lose"
#define TEXT_TOLOSE_INI				"ForceLose"
#define TEXT_TOLOSE_DESC			"Forces the player to lose the current game session."
#define TEXT_TODIE					"To Die"
#define TEXT_TODIE_INI				"ForceDie"
#define TEXT_TODIE_DESC				"Forces the player to blowup, loosing the current game session."
#define TEXT_CAPTUREOBJ				"Capture Object"
#define TEXT_CAPTUREOBJ_INI			"CaptureObject"
#define TEXT_CAPTUREOBJ_DESC		"Take ownership of any selected objects."
#define TEXT_		"Special Weapons"
#define TEXT_?_INI		"SpecialWeapons"
#define TEXT_?_DESC			"Grants all available special weapons to the player."
#define TEXT_FREEMONEY				"Free Money"
#define TEXT_FREEMONEY_INI			"FreeMoney"
#define TEXT_FREEMONEY_DESC			"Gives free money to the player."
#define TEXT_LIGHTNINGBOLT			"Lightning Bolt"
#define TEXT_LIGHTNINGBOLT_INI		"LightningBolt"
#define TEXT_LIGHTNINGBOLT_DESC		"Fires a lightning bolt at the current mouse location."
#define TEXT_IONBLAST				"Ion Blast"
#define TEXT_IONBLAST_INI			"IonBlast"
#define TEXT_IONBLAST_DESC			"Fires an ion blast bolt at the current mouse location."
#define TEXT_EXPLOSION				"Explosion"
#define TEXT_EXPLOSION_INI			"Explosion"
#define TEXT_EXPLOSION_DESC			"Spawns a explosion at the mouse location."
#define TEXT_SUPEREXPLOSION			"Super Explosion"
#define TEXT_SUPEREXPLOSION_INI		"SuperExplosion"
#define TEXT_SUPEREXPLOSION_DESC	"Spawns a large explosion at the mouse location."
#define TEXT_BAILOUT				"Bail Out"
#define TEXT_BAILOUT_INI			"BailOut"
#define TEXT_BAILOUT_DESC			"Exits the game to the desktop."
#define TEXT_IONSTORM				"Ion Storm"
#define TEXT_IONSTORM_INI			"IonStorm"
#define TEXT_IONSTORM_DESC			"Toggles an ion storm on/off."
#define TEXT_		"Scenario Snapshot"
#define TEXT_?_INI		"MapSnapshot"
#define TEXT_?_DESC			"Saves a snapshot of the current scenario state (Saved as 'SCEN_<date-time>.MAP.)."
#define TEXT_		"Delete Selected"
#define TEXT_?_INI		"DeleteObject"
#define TEXT_?_DESC			"Removes the selected object(s) from the game world."
#define TEXT_		"Spawn All"
#define TEXT_?_INI		"SpawnAll"
#define TEXT_?_DESC			"Spawn all buildable units and structures at mouse location."
#define TEXT_DAMAGE					"Damage"
#define TEXT_DAMAGE_INI				"Damage"
#define TEXT_DAMAGE_DESC			"Apply damage to all selected objects."
#define TEXT_		"Toggle Elite"
#define TEXT_?_INI		"ToggleElite"
#define TEXT_?_DESC			"Toggle the elite status of the selected objects."
#define TEXT_		"Build Cheat"
#define TEXT_?_INI		"BuildCheat"
#define TEXT_?_DESC			"Unlock all available build options for the player house."
#define TEXT_		"Toggle Shroud"
#define TEXT_?_INI		"ToggleShroud"
#define TEXT_?_DESC			"Toggles the visibility of the map shroud."
#define TEXT_HEAL					"Heal"
#define TEXT_HEAL_INI				"Heal"
#define TEXT_HEAL_DESC				"Heal the selected objects."
#define TEXT_INERT					"Toggle Inert"
#define TEXT_INERT_INI				"ToggleInert"
#define TEXT_INERT_DESC				"Toggles if weapons are inert or not."
#define TEXT_		"Dump AI Base Nodes"
#define TEXT_?_INI		"DumpAIBaseNodes"
#define TEXT_?_DESC			"Dumps all the current AI house base node info to the log output."
#define TEXT_		"Toggle Berzerk"
#define TEXT_?_INI		"ToggleBerzerk"
#define TEXT_?_DESC			"Toggles the berzerk state of the selected infantry."
#define TEXT_GROWSHADOW				"Encroach Shadow"
#define TEXT_GROWSHADOW_INI			"EncroachShadow"
#define TEXT_GROWSHADOW_DESC		"Increase the shroud darkness by one step (cell)."
#define TEXT_GROWFOG				"Encroach Fog"
#define TEXT_GROWFOG_INI			"EncroachFog"
#define TEXT_GROWFOG_DESC			"Increase the fog of war by one step (cell)."
#define TEXT_ALLIANCE				"Toggle Alliance"
#define TEXT_ALLIANCE_INI			"ToggleAlly"
#define TEXT_ALLIANCE_DESC			"Toggles alliance with the selected objects house."
#define TEXT_ADDPOWER				"Add Power"
#define TEXT_ADDPOWER_INI			"AddPower"
#define TEXT_ADDPOWER_DESC			"Adds 2000 power units to the player."
#define TEXT_PLACECRATE				"Place Crate"
#define TEXT_PLACECRATE_INI			"PlaceCrate"
#define TEXT_PLACECRATE_DESC		"Places a random crate at the mouse location."
#define TEXT_CURSOR_DEBUG			"Cursor Position"
#define TEXT_CURSOR_DEBUG_INI		"CursorPosition"
#define TEXT_CURSOR_DEBUG_DESC		"Displays cell coordinates of the mouse cursor."
#define TEXT_FRAMESTEP				"Toggle Frame Step"
#define TEXT_FRAMESTEP_INI			"ToggleFrameStep"
#define TEXT_FRAMESTEP_DESC			"Toggle frame step mode to step through the game frame-by-frame (for inspection)."
#define TEXT_STEPONE				"Step Forward 1 Frame"
#define TEXT_STEPONE_INI			"Step1Frame"
#define TEXT_STEPONE_DESC			"Frame Step Only: Step forward 1 frame."
#define TEXT_STEPFIVE				"Step Forward 5 Frames"
#define TEXT_STEPFIVE_INI			"Step5Frame"
#define TEXT_STEPFIVE_DESC			"Frame Step Only: Step forward 5 frames."
#define TEXT_STEPTEN				"Step Forward 10 Frames"
#define TEXT_STEPTEN_INI			"Step10Frames"
#define TEXT_STEPTEN_DESC			"Frame Step Only: Step forward 10 frames."
#define TEXT_AICONTROL				"Toggle AI Control"
#define TEXT_AICONTROL_INI			"ToggleAIControl"
#define TEXT_AICONTROL_DESC			"Toggles AI control of the player house."
#define TEXT_DEVELOPER				"Developer"
#define TEXT_S_S					"%s: %s"


/**
 *  Developer mode globals.
 */
extern bool Vinifera_Developer_InstantBuild;
extern bool Vinifera_Developer_AIInstantBuild;
extern bool Vinifera_Developer_BuildCheat;
extern bool Vinifera_Developer_Unshroud;
extern bool Vinifera_Developer_ShowCursorPosition;
extern bool Vinifera_Developer_FrameStep;
extern int Vinifera_Developer_FrameStepCount;
extern bool Vinifera_Developer_AIControl;


/**
 *  Various globals.
 */
extern bool Vinifera_SkipWWLogoMovie;
extern bool Vinifera_SkipStartupMovies;


/**
 *  Skip to menus.
 */
extern bool Vinifera_SkipToTSMenu;
extern bool Vinifera_SkipToFSMenu;
extern bool Vinifera_SkipToLAN;
extern bool Vinifera_SkipToSkirmish;
extern bool Vinifera_SkipToCampaign;
extern bool Vinifera_SkipToInternet;
extern bool Vinifera_ExitAfterSkip;
