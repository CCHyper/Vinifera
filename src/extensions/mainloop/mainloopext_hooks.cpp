/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MAINLOOPEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the intercepting Main_Loop().
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
#include "mainloopext_hooks.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "iomap.h"
#include "layer.h"
#include "logic.h"
#include "tactical.h"
#include "session.h"
#include "house.h"
#include "ccfile.h"
#include "ccini.h"
#include "saveload.h"
#include "addon.h"
#include "language.h"
#include "wstring.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-517
 * 
 *  Saves the current mission so it can be used as a mission checkpoint.
 * 
 *  author: CCHyper
 */
static void Save_Mission()
{
    char scenname[PATH_MAX+4];
    char savename[64];

    /**
     *  Build the save file name.
     */
    std::strncpy(scenname, Scen->ScenarioName, PATH_MAX);
    scenname[PATH_MAX] = '\0';
    char *ext = std::strrchr(scenname, '.');
    if (ext) ext[0] = '\0';
    std::strcat(scenname, ".SAV");

    Wstring scen_name = Scen->ScenarioName;
    Wstring scen_desc = Scen->Description;


    /**
     *  We need to fix up the campaign mission names with prefixes as they are
     *  not consistent between the mission briefing and the scenario description.
     *  
     *  Currently only handles the EN release.
     */


    if (Addon_Enabled(ADDON_FIRESTORM)) {

        /**
         *  Fix the Firestorm mission names.
         */
        if (scen_name == "FSGDI01.MAP"
            && (scen_desc == "GDI 01:Recover the Tacitus" || scen_desc == "Kodiak Down!")) {
            scen_desc = "(Firestorm) GDI 01: Recover the Tacitus";

        } else if (scen_name == "FSGDI02.MAP"
            && (scen_desc == "GDI 02:Party Crashers" || scen_desc == "Party Crashers")) {
            scen_desc = "(Firestorm) GDI 02: Party Crashers";
    
        } else if (scen_name == "FSGDI03.MAP"
            && (scen_desc == "GDI 03:Quell the Civilian Riot" || scen_desc == "The Tratos Riots")) {
            scen_desc = "(Firestorm) GDI 03: Quell the Civilian Riot";
    
        } else if (scen_name == "FSGDI04.MAP" && scen_desc == "GDI 04: In the Box") {
            scen_desc = "(Firestorm) GDI 04: In the Box";
    
        } else if (scen_name == "FSGDI05.MAP" && scen_desc == "GDI 05: Dogma Day Afternoon") {
            scen_desc = "(Firestorm) GDI 05: Dogma Day Afternoon";
    
        } else if (scen_name == "FSGDI06.MAP"
            && (scen_desc == "GDI 06:Escape from CABAL" || scen_desc == "Escape from CABAL")) {
            scen_desc = "(Firestorm) GDI 06: Escape from CABAL";
    
        } else if (scen_name == "FSGDI07.MAP" && scen_desc == "GDI 07: The Cyborgs are Coming") {
            scen_desc = "(Firestorm) GDI 07: The Cyborgs are Coming";
    
        } else if (scen_name == "FSGDI08.MAP"
            && (scen_desc == "GDI 08:Factory Recall" || scen_desc == "Factory Recall")) {
            scen_desc = "(Firestorm) GDI 08: Factory Recall";
    
        } else if (scen_name == "FSGDI09.MAP" && scen_desc == "GDI 09:Core of the Problem") {
            scen_desc = "(Firestorm) GDI 09: Core of the Problem";

        } else if (scen_name == "FSNOD01.MAP"
            && (scen_desc == "NOD 01:Operation Reboot" || scen_desc == "Operation Reboot")) {
            scen_desc = "(Firestorm) NOD 01: Operation Reboot";
    
        } else if (scen_name == "FSNOD02.MAP"
            && (scen_desc == "NOD 02:Seeds of Destruction" || scen_desc == "Seeds of Destruction")) {
            scen_desc = "(Firestorm) NOD 02: Seeds of Destruction";
    
        } else if (scen_name == "FSNOD03.MAP" && scen_desc == "NOD 03:Tratos’ Final Act") {
            scen_desc = "(Firestorm) NOD 03: Tratos's Final Act";
    
        } else if (scen_name == "FSNOD04.MAP"
            && (scen_desc == "NOD 04:Mutant Extermination" || scen_desc == "Mutant Extermination")) {
            scen_desc = "(Firestorm) NOD 04: Mutant Extermination";
    
        } else if (scen_name == "FSNOD05.MAP"
            && (scen_desc == "NOD 05:Escape from CABAL" || scen_desc == "Escape from CABAL")) {
            scen_desc = "(Firestorm) NOD 05: Escape from CABAL";
    
        } else if (scen_name == "FSNOD06.MAP" && scen_desc == "NOD 06: The Needs of the Many!") {
            scen_desc = "(Firestorm) NOD 06: The Needs of the Many!";
    
        } else if (scen_name == "FSNOD07.MAP" && scen_desc == "NOD 07: Determined Retribution") {
            scen_desc = "(Firestorm) NOD 07: Determined Retribution";
    
        } else if (scen_name == "FSNOD08.MAP" && scen_desc == "NOD 08: Harvester Hunting") {
            scen_desc = "(Firestorm) NOD 08: Harvester Hunting";
    
        } else if (scen_name == "FSNOD09.MAP" && scen_desc == "NOD 09:Core of the Problem") {
            scen_desc = "(Firestorm) NOD 09: Core of the Problem";
        }

    } else {

        /**
         *  Fix up the demo/tutorial mission names.
         */
        if (scen_name == "TSDEMO.MAP" && scen_desc == "TS demo mission #1 : Initiation") {
            scen_desc = "Tutorial 1: Initiation";
    
        } else if (scen_name == "TSDEMO2.MAP" && scen_desc == "TS demo mission #2 : Clean Sweep") {
            scen_desc = "Tutorial 2: Clean Sweep";


        /**
         *  Fix the original mission names.
         */
        } else if (scen_name == "GDI1A.MAP" && scen_desc == "Reinforce Phoenix Base") {
            scen_desc = "GDI 1: Reinforce Phoenix Base";
    
        } else if (scen_name == "GDI2A.MAP" && scen_desc == "Secure The Region") {
            scen_desc = "GDI 2: Secure the Region";
    
        } else if (scen_name == "GDI3A.MAP" && scen_desc == "Secure Crash Site") {
            scen_desc = "GDI 3A: Locate and Secure Crash Site";                 // From Prima's Official Guide
    
        } else if (scen_name == "GDI3B.MAP"
            && (scen_desc == "Capture Train Station" || scen_desc == "Secure the Region")) {
            scen_desc = "GDI 3B: Capture the Train Station";
    
        } else if (scen_name == "GDI4A.MAP" && scen_desc == "Defend Crash Site") {
            scen_desc = "GDI 4: Defend the Crash Site";
    
        } else if (scen_name == "GDI5A.MAP" && scen_desc == "Destroy Radar Array") {
            scen_desc = "GDI 5A: Destroy the Radar Array";
    
        } else if (scen_name == "GDI5B.MAP" && scen_desc == "Rescue Tratos") {
            scen_desc = "GDI 5B: Rescue Tratos (A)";
    
        } else if (scen_name == "GDI5C.MAP" && scen_desc == "Rescue Tratos") {
            scen_desc = "GDI 5C: Rescue Tratos (B)";
    
        } else if (scen_name == "GDI6A.MAP" && scen_desc == "Destroy Vega's Dam") {
            scen_desc = "GDI 6A: Destroy Vega's Dam";
    
        } else if (scen_name == "GDI6B.MAP" && scen_desc == "Destroy Vega's Base") {
            scen_desc = "GDI 6B: Destroy Vega's Base";
    
        } else if (scen_name == "GDI7A.MAP" && scen_desc == "Capture Hammerfest Base") {
            scen_desc = "GDI 7: Recapture Hammerfest Base";                     // From Prima's Official Guide
    
        } else if (scen_name == "GDI8A.MAP"
            && (scen_desc == "Retrieve Disrupter Crystals" || scen_desc == "Retrieval of Disrupter Crystals")) {
            scen_desc = "GDI 8: Retrieval of Disrupter Crystals";    
    
        } else if (scen_name == "GDI9A.MAP"
            && (scen_desc == "Rescue Prisoners" || scen_desc == "Rescue the Prisoners")) {
            scen_desc = "GDI 9A: Rescue the Prisoners";
    
        } else if (scen_name == "GDI9B.MAP"
            && (scen_desc == "Destroy Chemical Supply" || scen_desc == "Destroy Chemical Supply Station")) {
            scen_desc = "GDI 9B: Destroy Chemical Supply Station";
    
        } else if (scen_name == "GDI9C.MAP"
            && (scen_desc == "Mine Power Grid" || scen_desc == "Mine the Power Grid")) {
            scen_desc = "GDI 9C: Mine the Power Grid";
    
        } else if (scen_name == "GDI9D.MAP"
            && (scen_desc == "Destroy Chemical Missile Plant" || scen_desc == "Destroy the Chemical Missile Plant")) {
            scen_desc = "GDI 9D: Destroy the Chemical Missile Plant";
    
        } else if (scen_name == "GDI10A.MAP" && scen_desc == "Destroy Prototype Facility") {
            scen_desc = "GDI 10A: Destroy the Prototype Facility (A)";          // ?? Locate and Destroy Prototype Manufacturing Facility
    
        } else if (scen_name == "GDI10B.MAP" && scen_desc == "Destroy Prototype Facility") {
            scen_desc = "GDI 10B: Destroy the Prototype Facility (B)";          // ?? Locate and Destroy Prototype Manufacturing Facility
    
        } else if (scen_name == "GDI11A.MAP" && scen_desc == "Weather the Storm") {
            scen_desc = "GDI 11: Weather the Storm";
    
        } else if (scen_name == "GDI12A.MAP" && scen_desc == "Final Conflict") {
            scen_desc = "GDI 12: Final Conflict";

        } else if (scen_name == "NOD1A.MAP" && scen_desc == "The Messiah Returns") {
            scen_desc = "NOD 1: The Messiah Returns";
    
        } else if (scen_name == "NOD2A.MAP" && scen_desc == "Retaliation") {
            scen_desc = "NOD 2: Retaliation";
    
        } else if (scen_name == "NOD3A.MAP"
            && (scen_desc == "Detroy Hassan's Temple")                          // Yeah, spelling error in MISSION.INI...
            || scen_desc == "Destroy Hassan's Temple and Capture Him") {
            scen_desc = "NOD 3A: Destroy Hassan's Temple";                      // ?? Destroy Hassan's Temple and Capture Him
    
        } else if (scen_name == "NOD3B.MAP" && scen_desc == "Free Rebel Commander") {
            scen_desc = "NOD 3B: Free Rebel Commander";                         // ?? Free the Rebel Nod Commander
    
        } else if (scen_name == "NOD4A.MAP" && scen_desc == "Eviction Notice") {
            scen_desc = "NOD 4A: Eviction Notice";
    
        } else if (scen_name == "NOD4B.MAP" && scen_desc == "Blackout") {
            scen_desc = "NOD 4B: Blackout";
    
        } else if (scen_name == "NOD5A.MAP" && scen_desc == "Salvage Operation") {
            scen_desc = "NOD 5: Salvage Operation";
    
        } else if (scen_name == "NOD6A.MAP" && scen_desc == "Sheep's Clothing") {
            scen_desc = "NOD 6A: Sheep's Clothing";
    
        } else if (scen_name == "NOD6B.MAP"
            && (scen_desc == "Capture Umagon" || scen_desc == "Locate and Capture Umagon")) {
            scen_desc = "NOD 6B: Locate and Capture Umagon (A)";
    
        } else if (scen_name == "NOD6C.MAP" && scen_desc == "Capture Umagon") {
            scen_desc = "NOD 6C: Locate and Capture Umagon (B)";
    
        } else if (scen_name == "NOD7A.MAP"
            && (scen_desc == "Destroy GDI Research Facility" || scen_desc == "Destroy the Research Facility")) {
            scen_desc = "NOD 7A: Destroy the GDI Research Facility";
    
        } else if (scen_name == "NOD7B.MAP"
            && (scen_desc == "Escort Bio-toxin Trucks" || scen_desc == "Escort the Bio-toxin Trucks")) {
            scen_desc = "NOD 7B: Escort the Bio-toxin Trucks";

        } else if (scen_name == "NOD8A.MAP"
            && (scen_desc == "Villainess in Distress" || scen_desc == "Villainess In Distress")) {
            scen_desc = "NOD 8: Villainess in Distress";                        // ?? Rescue Oxanna
    
        } else if (scen_name == "NOD9A.MAP"
            && (scen_desc == "Establish Nod Presence" || scen_desc == "Re-establish Nod Presence")) {
            scen_desc = "NOD 9A: Establish Nod Presence";                       // Reestablish Nod Presence
    
        } else if (scen_name == "NOD9B.MAP"
            && (scen_desc == "Protect Waste Convoys" || scen_desc == "Protect the Waste Convoys")) {
            scen_desc = "NOD 9B: Protect the Waste Convoys";
    
        } else if (scen_name == "NOD10A.MAP"
            && (scen_desc == "Destroy Mammoth Mk.II Prototype" || scen_desc == "Destroy Prototype Facility")) {
            scen_desc = "NOD 10A: Destroy the Mammoth Mk.II Prototype";

        } else if (scen_name == "NOD11A.MAP" && scen_desc == "Capture Jake McNeil") {
            scen_desc = "NOD 11: Capture Jake McNeil";
    
        } else if (scen_name == "NOD12A.MAP" && scen_desc == "A New Beginning") {
            scen_desc = "NOD 12A: A New Beginning";
    
        } else if (scen_name == "NOD12B.MAP" && scen_desc == "Illegal Data Transfer") {
            scen_desc = "NOD 12B: Illegal Data Transfer";                       // ?? Pulling the Shark's Teeth
        }

    }

    /**
     *  Format the mission description. We use square brackets to signify
     *  this is a auto save and not a user made save.
     */
    std::snprintf(savename, sizeof(savename), "[%s]", scen_desc);

    /**
     *  Now save it!
     */
    Save_Game(scenname, savename);

    DEBUG_INFO("Mission \"%s\" (%s) saved.\n", Scen->ScenarioName, scen_desc);
}


DECLARE_PATCH(_Main_Loop_Initial_Auto_Save_Patch)
{
    /**
     *  Perform a save of the current mission if we are in a singleplayer game
     *  and before the first frame logic pass has been performed.
     */
    if (Vinifera_AutoMissionSaveEnabled) {
        if (Session.Type == GAME_NORMAL && Frame == 0) {
            Save_Mission();
        }
    }

    //DEV_DEBUG_INFO("Before Logic.AI\n");

    Logic.AI();

    //DEV_DEBUG_INFO("After Logic.AI\n");

    JMP(0x005091A0);
}


/**
 *  This patch stops EVENT_OPTIONS from being created when frame step
 *  mode is enabled. This is because we need to handle it differently
 *  due to us not processing any event while in frame step mode.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Queue_Options_Frame_Step_Check_Patch)
{
    _asm { sub esp, 0x30 }

    if (Vinifera_Developer_FrameStep) {
        goto function_return;
    }

    if (!PlayerPtr->IsToWin && !PlayerPtr->IsToLose && !PlayerPtr->IsToDie) {
        goto create_event;
    }

function_return:
    JMP_REG(ecx, 0x005B1171);

create_event:
    _asm { mov ecx, PlayerPtr } // Second dereference required due to the global reference in TS++.
    _asm { mov eax, [ecx] }
    JMP_REG(ecx, 0x005B1116);
}


static void Before_Main_Loop()
{
}


static void After_Main_Loop()
{
}


/**
 *  Main loop for the frame step mode. This should only handle basic
 *  input, redraw the map and update the scroll position.
 * 
 *  @author: CCHyper
 */
static bool FrameStep_Main_Loop()
{
    //DEV_DEBUG_INFO("FrameStep_Main_Loop(enter)\n");

    if (GameActive) {

        Call_Back();

        /**
         *  Update the display, unless we're inside a dialog.
         */
        if (SpecialDialog == SDLG_NONE && GameInFocus) {

            Map.Flag_To_Redraw(2);

            KeyNumType input;
            int x;
            int y;

            Map.Input(input, x, y);
            if (input != KN_NONE) {
                Keyboard_Process(input);

                /**
                 *  Kludge to allow the options dialog to open.
                 */
                if (input == KN_ESC || input == KN_SPACE) {
                    SpecialDialog = SDLG_OPTIONS;
                }

            }

            Map.Render();
            TacticalMap->AI();
        }

    }

    Sleep(1);

    //DEV_DEBUG_INFO("FrameStep_Main_Loop(exit)\n");

    return !GameActive;
}


static bool Main_Loop_Intercept()
{
    bool ret = false;

    /**
     *  Frame step mode enabled but no frames to process, so just perform
     *  a basic redraw and update of the screen, no game logic.
     */
    if (Vinifera_Developer_FrameStep && !Vinifera_Developer_FrameStepCount) {

        ret = FrameStep_Main_Loop();

    /**
     *  This is basically the original main loop, but now encapsulated by
     *  the frame step logic to allow us to process the requested frames.
     */
    } else if ((Vinifera_Developer_FrameStep && Vinifera_Developer_FrameStepCount > 0)
           || (!Vinifera_Developer_FrameStep && !Vinifera_Developer_FrameStepCount)) {

        //DEV_DEBUG_INFO("Before Main_Loop()\n");

        Before_Main_Loop();

        /**
         *  The games main loop function.
         */
        ret = Main_Loop();

        After_Main_Loop();

        //DEV_DEBUG_INFO("After Main_Loop()\n");

        /**
         *  Decrement the frame step count.
         */
        if (Vinifera_Developer_FrameStep && Vinifera_Developer_FrameStepCount > 0) {
            --Vinifera_Developer_FrameStepCount;
        }

    }

    return ret;
}

/**
 *  Main function for patching the hooks.
 */
void MainLoop_Hooks()
{
    Patch_Call(0x00462A8E, &Main_Loop_Intercept);
    Patch_Call(0x00462A9C, &Main_Loop_Intercept);
    Patch_Call(0x005A0B85, &Main_Loop_Intercept);
    Patch_Jump(0x005B10F0, &_Queue_Options_Frame_Step_Check_Patch);
    Patch_Jump(0x00509196, &_Main_Loop_Initial_Auto_Save_Patch);
}
