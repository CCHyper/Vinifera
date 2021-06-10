/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          INITEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains any hooks for the game init process.
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
#include "initext_hooks.h"
#include "initext_functions.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "special.h"
#include "playmovie.h"
#include "cd.h"
#include "newmenu.h"
#include "addon.h"
#include "command.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"










#include "session.h"
#include "scenario.h"
#include "house.h"
#include "housetype.h"
#include "side.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "rules.h"


/**
 *  #issue-218
 * 
 *  This collection of patches are changes that allow sidebar and speech assets
 *  for new sides.
 * 
 *  We abuse SessionClass::IsGDI and ScenarioClass::IsGDI in these patches
 *  to store the current players HouseType so it can be used to fetch the
 *  SideType from it for loading the assets. This also means this bugfix
 *  works without extending any of the games classes.
 * 
 *  @warning: This does mean we are limited to 255 unique houses!
 * 
 *  @author: CCHyper
 */
static void Set_Session_House() { Session.IsGDI = (unsigned char)Session.Players.Fetch_Head()->Player.House; }
DECLARE_PATCH(_Select_Game_PreStart_SetPlayerHouse_Patch)
{
    /**
     *  This patch removes the code that sets the "IsGDI" member of SessionClass
     *  bool based on if the house name matched "GDI" or not and stores
     *  the player HouseType directly.
     */

    /**
     *  We abuse SessionClass::IsGDI to store the player house. The "head"
     *  of the Players vector is "us", the local human player.
     * 
     *  Accessing unions trashes the stack, so this operation is wrapped.
     */
    Set_Session_House();

#if 0
    /**
     *  Original game code.
     */
    static HouseTypeClass *housetype;

    housetype = HouseTypes[Session.Players[0]->Player.House];
    Session.IsGDI = strcmpi("GDI", housetype->Name()) == 0;
#endif

    JMP(0x004E2D13);
}

DECLARE_PATCH(_Get_All_Remove_PrepForSide_Patch)
{
    static HouseTypeClass *housetype;

    DEBUG_INFO("Calling Prep_For_Side()...\n");

    /**
     *  Fetch the houses side type and use this to decide which assets to load.
     */
    housetype = HouseTypes[HousesType(Scen->IsGDI)];
    if (!Prep_For_Side(housetype->Side)) {

        DEBUG_WARNING("Prep_For_Side(%d) failed! Trying with side 0...\n", housetype->Side);

        /**
         *  Try once again but with the Side 0 (GDI) assets.
         */
        if (!Prep_For_Side(SIDE_GDI)) {
            DEBUG_ERROR("Prep_For_Side() failed!\n");
            goto return_label;
        }

    }

    DEV_DEBUG_INFO("Prep_For_Side(%d) sucessfull.\n", housetype->Side);

continue_function:
    JMP(0x005D6C6C);

return_label:
    JMP(0x005D6C61);
}

DECLARE_PATCH(_Get_All_Remove_PrepSpeechForSide_Patch)
{
    static HouseTypeClass *housetype;
    static SideType speech_side;

    DEBUG_INFO("Calling Prep_Speech_For_Side()...\n");

    /**
     *  Fetch the houses side type and use this to decide which assets to load.
     */
    housetype = HouseTypes[HousesType(Scen->IsGDI)];
    speech_side = Scen->SpeechSide != SIDE_NONE ? Scen->SpeechSide : housetype->Side;
    if (!Prep_Speech_For_Side(speech_side)) {

        DEBUG_WARNING("Prep_Speech_For_Side(%d) failed! Trying with side 0...\n", speech_side);

        /**
         *  Try once again but with the Side 0 (GDI) assets.
         */
        if (!Prep_Speech_For_Side(SIDE_GDI)) {
            DEBUG_ERROR("Prep_Speech_For_Side() failed!\n");
            goto return_label;
        }
    }

    DEV_DEBUG_INFO("Prep_Speech_For_Side(%d) sucessfull.\n", speech_side);

continue_function:
    JMP(0x005D6DEF);

return_label:
    JMP(0x005D6DC3);
}

static char Player_Buffer[32];
DECLARE_PATCH(_Read_Scenario_INI_Singleplayer_Patch)
{
    LEA_STACK_STATIC(char *, player_buffer, esp, 0x6C);
    
    strncpy(Player_Buffer, player_buffer, sizeof(Player_Buffer));

    /**
     *  Store the players house to used for loading sides assets later on.
     */
    //Scen->IsGDI = (unsigned char)Session.IsGDI;
    //Scen->SpeechSide = (unsigned char)Session.IsGDI;

    JMP(0x005DD784);
}

DECLARE_PATCH(_Read_Scenario_INI_Multiplayer_Patch)
{
    /**
     *  Store the players house to used for loading sides assets later on.
     */
    Scen->IsGDI = (unsigned char)Session.IsGDI;
    //Scen->SpeechSide = (unsigned char)Session.IsGDI;

    JMP(0x005DD784);
}

DECLARE_PATCH(_Read_Scenario_INI_PrepForSide_Patch)
{
    GET_REGISTER_STATIC(CCINIClass *, ini, ebp)
    static HouseTypeClass *housetype;
    static SideType speech_side;

    /**
     *  This moves the calls to Prep_For_Side and Prep_Speech_For_Side to
     *  after the initialisation of the houses so we can load the Side from
     *  the players chosen house type.
     */

    /**
     *  Stolen bytes/code.
     */
    Scen->Read_Basic(*ini);








    /**
     *  If this is a campaign session, load the house from the "Player" value.
     */
    if (Session.Type == GAME_NORMAL) {

        /**
         *  Fetch the houses side type and use this to decide which assets to load.
         */
        housetype = (HouseTypeClass *)HouseTypeClass::As_Pointer(Player_Buffer);

    } else {

        /**
         *  Fetch the houses side type and use this to decide which assets to load.
         */
        housetype = HouseTypes[HousesType(Scen->IsGDI)];
    }

    /**
     *  Debugging code.
     */
    if (Vinifera_DeveloperMode) {

        DEV_DEBUG_INFO("About to prepare for...\n");
        DEV_DEBUG_INFO("  House \"%s\" (%d) with Side \"%s\" (%d)\n",
            housetype->Name(), housetype->Get_Heap_ID(),
            Sides[housetype->Side]->Name(), Sides[housetype->Side]->Get_Heap_ID());
        
        DEV_DEBUG_INFO("Side info:\n");
        for (int i = 0; i < Sides.Count(); ++i) {
            static SideClass *side;
            side = Sides[i];
            DEV_DEBUG_INFO("  Side \"%s\" (%d), Houses.Count %d\n", side->IniName, i, side->Houses.Count());
            for (int i = 0; i < side->Houses.Count(); ++i) {
               DEV_DEBUG_INFO("    Houses %d = %s (%d)\n", i, HouseTypes[side->Houses[i]]->Name(), side->Houses[i]);
            }
        }
    }

    DEBUG_INFO("Calling Prep_For_Side()...\n");
    if (!Prep_For_Side(housetype->Side)) {

        DEBUG_WARNING("Prep_For_Side(%d) failed! Trying with side 0...\n", housetype->Side);

        /**
         *  Try once again but with the Side 0 (GDI) assets.
         */
        if (!Prep_For_Side(SIDE_GDI)) {
            DEBUG_ERROR("Prep_For_Side() failed!\n");
            goto return_label;
        }
    }

    DEV_DEBUG_INFO("Prep_For_Side(%d) sucessfull.\n", housetype->Side);

    DEBUG_INFO("Calling Prep_Speech_For_Side()...\n");
    speech_side = Scen->SpeechSide != SIDE_NONE ? Scen->SpeechSide : housetype->Side;
    if (!Prep_Speech_For_Side(speech_side)) {

        DEBUG_WARNING("Prep_Speech_For_Side(%d) failed! Trying with side 0...\n", speech_side);

        /**
         *  Try once again but with the Side 0 (GDI) assets.
         */
        if (!Prep_Speech_For_Side(SIDE_GDI)) {
            DEBUG_ERROR("Prep_Speech_For_Side() failed!\n");
            goto return_label;
        }
    }

    DEV_DEBUG_INFO("Prep_Speech_For_Side(%d) sucessfull.\n", speech_side);





    /**
     *  Reprocess rules
     */
    DEBUG_INFO("Rule->Initialize()\n");
    Rule->Initialize(*RuleINI);

    DEBUG_INFO("Rule->Process()\n");
    Rule->Process(*ini);

    DEBUG_INFO("HouseClass::Read_Scenario_INI()\n");
    HouseClass::Read_Scenario_INI(*ini);



    /**
     *  Continue function flow.
     */
continue_function:
    JMP(0x005DD956);

    /**
     *  Function return.
     */
return_label:
    JMP(0x005DD94B);
}


/**
 *  #issue-305
 * 
 *  Fixes bug where the sidebar mouse wheel scrolling "error" sound
 *  can be heard at the main menu.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Main_Window_Procedure_Scroll_Sidebar_Check_Patch)
{
    GET_STACK_STATIC(UINT, wParam, esp, 0x14);
    static bool _mouse_wheel_scolling;

    /**
     *  The code before this patch checks for WM_MOUSEWHEEL.
     */

    /**
     *  We are not currently playing a scenario, no need to execute this command.
     */
    if (!bool_007E4040 && !bool_007E48FC) {
        goto message_handler;
    }

    /**
     *  Are we currently executing a scroll command? This is required because
     *  the Main_Window_Procedure function runs at a Windows level.
     */
    if (_mouse_wheel_scolling) {
        goto message_handler;
    }

    _mouse_wheel_scolling = true;

    /**
     *  Execute the command based on the direction of the mouse wheel.
     */
    if ((wParam & 0x80000000) == 0) {
        CommandClass::Activate_From_Name("SidebarUp");
    } else {
        CommandClass::Activate_From_Name("SidebarDown");
    }

    _mouse_wheel_scolling = false;

executed:
    JMP_REG(eax, 0x00685F9C);

message_handler:
    JMP_REG(ecx, 0x00685FA0);
}


/**
 *  #issue-513
 * 
 *  Patch to add check for CD::IsFilesLocal to make sure -CD really
 *  was set by the user.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Init_CDROM_Access_Local_Files_Patch)
{
    _asm { add esp, 4 }

    /**
     *  If there are search drives specified then all files are to be
     *  considered local.
     */
    if (CCFileClass::Is_There_Search_Drives()) {
        
        /**
         *  Double check that the game was launched with -CD.
         */
        if (CD::IsFilesLocal) {

            /**
             *  This is a workaround to ensure the mix loading code passes.
             */
            //CD::Set_Required_CD(DISK_GDI);

            goto files_local;
        }
    }

    /**
     *  Continue to initialise the CD-ROM code.
     */
init_cdrom:
    JMP(0x004E0471);

    /**
     *  Flag files as being local, no CD-ROM init.
     */
files_local:
    JMP(0x004E06F5);
}


static bool CCFile_Is_Available(const char *filename)
{
    return CCFileClass(filename).Is_Available();
}


/**
 *  #issue-478
 * 
 *  
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Init_Game_Skip_Startup_Movies_Patch)
{
    if (Vinifera_SkipStartupMovies) {
        DEBUG_INFO("Skipping startup movies.\n");
        goto skip_loading_screen;
    }

    if (Special.IsFromInstall) {
        DEBUG_GAME("Playing first time intro sequence.\n");
        Play_Movie("EVA.VQA", THEME_NONE, true, true, true);
    }

    if (!Vinifera_SkipWWLogoMovie) {
        DEBUG_GAME("Playing startup movies.\n");
        Play_Movie("WWLOGO.VQA", THEME_NONE, true, true, true);
    } else {
        DEBUG_INFO("Skipping startup movie.\n");
    }

    if (!NewMenuClass::Get()) {
        if (CCFile_Is_Available("FS_TITLE.VQA")) {
            Play_Movie("FS_TITLE.VQA", THEME_NONE, true, false, true);
        } else {
            Play_Movie("STARTUP.VQA", THEME_NONE, true, false, true);
        }
    }

loading_screen:
    _asm { or ebx, 0xFFFFFFFF }
    JMP(0x004E0848);

skip_loading_screen:
    JMP(0x004E084D);
}


#if defined(TS_CLIENT)
/**
 *  Forces Firestorm addon as Present (installed).
 * 
 *  @author: CCHyper
 */
static bool Vinifera_Addon_Present()
{
    /**
     *  Tiberian Sun is installed and enabled.
     */
    InstalledMode = 1;
    EnabledMode = 1;

    DEBUG_INFO("Forcing Firestorm addon as installed.");

    /**
     *  Firestorm is installed.
     */
    InstalledMode |= 2;

    return true;
}
#endif


/**
 *  Main function for patching the hooks.
 */
void GameInit_Hooks()
{
    Patch_Jump(0x004E0786, &_Init_Game_Skip_Startup_Movies_Patch);
    Patch_Jump(0x004E0461, &_Init_CDROM_Access_Local_Files_Patch);
    Patch_Jump(0x004E3D20, &Vinifera_Init_Bootstrap_Mixfiles);
    Patch_Jump(0x004E4120, &Vinifera_Init_Secondary_Mixfiles);
    Patch_Jump(0x004E7EB0, &Vinifera_Prep_For_Side);
    Patch_Jump(0x00686190, &Vinifera_Create_Main_Window);

    /**
     *  #issue-110
     * 
     *  Unable to load startup mix files is no longer a fatal error. These
     *  patches change the checks in Init_Bulk_Data to skip the cache process
     *  and continue initialisation.
     */
    Patch_Word(0x004E4601, 0x5C74); // jz 0x004E49B7 -> jz 0x004E465F
    Patch_Byte_Range(0x004E4601+2, 0x90, 4);
    Patch_Word(0x004E460F, 0x4E74); // jz 0x004E49B7 -> jz 0x004E465F
    Patch_Byte_Range(0x004E460F+2, 0x90, 4);
    Patch_Word(0x004E4641, 0x1C74); // jz 0x004E49B7 -> jz 0x004E465F
    Patch_Byte_Range(0x004E4641+2, 0x90, 4);
    Patch_Byte_Range(0x004E4657, 0x90, 8);

    /**
     *  #issue-494
     * 
     *  Fixes a bug where FSMENU would play instead of INTRO in Tiberian Sun
     *  mode after returning to the main menu from a game.
     * 
     *  This was a because the game was checking if the Firestorm addon was
     *  installed rather than if it was the currently active game mode.
     */
    Patch_Call(0x004E1F70, &Addon_Enabled);
    Patch_Call(0x004E25A6, &Addon_Enabled);
    Patch_Call(0x004E2890, &Addon_Enabled);
    Patch_Call(0x004E2991, &Addon_Enabled);
    Patch_Call(0x004E86F5, &Addon_Enabled);
    Patch_Call(0x004E8735, &Addon_Enabled);

    Patch_Jump(0x00685F69, &_Main_Window_Procedure_Scroll_Sidebar_Check_Patch);

#if defined(TS_CLIENT)
    /**
     *  TS Client file structure assumes Firestorm is always installed and enabled.
     */
    Patch_Jump(0x00407050, &Vinifera_Addon_Present);
#endif

    Patch_Jump(0x004E2CE4, &_Select_Game_PreStart_SetPlayerHouse_Patch);
    Patch_Jump(0x005D6C46, &_Get_All_Remove_PrepForSide_Patch);
    Patch_Jump(0x005D6DAA, &_Get_All_Remove_PrepSpeechForSide_Patch);

    /**
     *  Changes the default value of ScenarioClass 0x1D91 (IsGDI) from "1" to "0". This is
     *  because we now use it as a HouseType index, and need it to default to the first index.
     */
    Patch_Byte(0x005DAFD0+6, 0x00); // +6 skips the opcode.

    /**
     *  Changes the default value of SessionClass 0x1D91 (IsGDI) from "1" to "0".. This is
     *  because we now use it as a HouseType index, and need it to default to the first index.
     */
    Patch_Byte(0x005ED06B+1, 0x85); // changes "dl" (1) to "al" (0)

    /**
     *  Patch out the existing Prep_For_Side and Prep_Speech_For_Side calls.
     */
    Patch_Jump(0x005DD784, 0x005DD7B6); // Prep_For_Side
    Patch_Jump(0x005DD818, 0x005DD845); // Prep_Speech_For_Side

    /**
     *  These fix the initial values in Read_Scenario_INI
     */
    Patch_Jump(0x005DD720, _Read_Scenario_INI_Singleplayer_Patch);
    Patch_Jump(0x005DD75B, _Read_Scenario_INI_Multiplayer_Patch);

    Patch_Jump(0x005DD93B, &_Read_Scenario_INI_PrepForSide_Patch);
}
