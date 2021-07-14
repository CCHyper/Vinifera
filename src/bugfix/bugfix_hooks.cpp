/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BUGFIX_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for all bug fixes.
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
#include "bugfix_hooks.h"
#include "bugfixes.h"

#include "tibsun_globals.h"
#include "vinifera_util.h"
#include "wwmouse.h"
#include "campaign.h"
#include "scenario.h"
#include "playmovie.h"
#include "ccfile.h"
#include "cd.h"
#include "vqa.h"
#include "movie.h"
#include "dsurface.h"
#include "options.h"
#include "language.h"
#include "theme.h"
#include "endgame.h"
#include "dropship.h"
#include "msgbox.h"
#include "command.h"
#include "loadoptions.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-46
 * 
 *  Fixes bug where the game difficulty gets reset, but not reassigned
 *  after restarting a mission.
 * 
 *  This also handles the case where the EndGame instance is re-initialised.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_EndGameClass_Constructor_Set_Difficulty_Patch)
{
    GET_REGISTER_STATIC(EndGameClass *, this_ptr, edx);

    /**
     *  The EndGameClass constructor initialises Difficulty to NORMAL.
     *  This patch uses the ScenarioClass Difficulty if set at this point.
     */
    if (Scen && Scen->Difficulty != -1) {
        this_ptr->Difficulty = Scen->Difficulty;
    }

    //DEBUG_INFO("EndGameClass constructor.\n");

    /**
     *  Stolen bytes/code.
     */
    _asm { mov eax, this_ptr }
    _asm { pop edi }
    _asm { ret }
}

DECLARE_PATCH(_Select_Game_Set_EndGameClass_Difficulty_Patch)
{
    DEBUG_INFO("Scen->Difficulty = %d\n", Scen->Difficulty);
    DEBUG_INFO("Scen->CDifficulty = %d\n", Scen->CDifficulty);

    /**
     *  Assign the ScenarioClass Difficulty. This is done to ensure
     *  the difficulty is restored after game restart.
     */
    DEBUG_INFO("Setting EndGame difficulty to %d.\n", Scen->Difficulty);
    EndGame.Difficulty = Scen->Difficulty;

    /**
     *  Stolen bytes/code.
     */
    Theme.Stop(true);

    JMP(0x004E2AE3);
}

DECLARE_PATCH(_EndGameClass_Record_Debug_Patch)
{
    GET_REGISTER_STATIC(EndGameClass *, this_ptr, esi);

    DEBUG_INFO("Recording end game information...\n");
    DEBUG_INFO("  Credits: %d\n", this_ptr->Credits);
    DEBUG_INFO("  MissionTimer: %d\n", this_ptr->MissionTimer);
    DEBUG_INFO("  Difficulty: %d\n", this_ptr->Difficulty);
    DEBUG_INFO("  Stage: %d\n", this_ptr->Stage);

    /**
     *  Stolen bytes/code.
     */
    _asm { pop esi }
    _asm { ret }
}

DECLARE_PATCH(_EndGameClass_Apply_Debug_Patch)
{
    GET_REGISTER_STATIC(EndGameClass *, this_ptr, edi);

    DEBUG_INFO("Applying end game information...\n");
    DEBUG_INFO("  Credits: %d\n", this_ptr->Credits);
    DEBUG_INFO("  MissionTimer: %d\n", this_ptr->MissionTimer);
    DEBUG_INFO("  Difficulty: %d\n", this_ptr->Difficulty);
    DEBUG_INFO("  Stage: %d\n", this_ptr->Stage);

    /**
     *  Stolen bytes/code.
     */
    Scen->Stage = this_ptr->Stage;

    _asm { pop edi }
    _asm { pop esi }
    _asm { add esp, 0x0C }
    _asm { ret }
}

static void _Set_Difficulty_On_Game_Restart_Patch()
{
    Patch_Jump(0x00493881, &_EndGameClass_Constructor_Set_Difficulty_Patch);
    Patch_Jump(0x004E2AD7, &_Select_Game_Set_EndGameClass_Difficulty_Patch);
}

static void _EndGameClass_Debug_Output_Patches()
{
    /**
     *  Patches to log the current state of EndGameClass.
     */
    Patch_Jump(0x00493919, &_EndGameClass_Record_Debug_Patch);
    Patch_Jump(0x004939F1, &_EndGameClass_Apply_Debug_Patch);
    Patch_Jump(0x00493A07, 0x004939F1);
    Patch_Jump(0x00493A18, 0x004939F1);
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

static void _Scroll_Sidebar_InGame_Check_Patch()
{
    Patch_Jump(0x00685F69, &_Main_Window_Procedure_Scroll_Sidebar_Check_Patch);
}


/**
 *  #issue-212
 * 
 *  Changes the default value of "IsScoreShuffle" to "true".
 * 
 *  @author: CCHyper
 */
static void _OptionsClass_Constructor_IsScoreShuffle_Default_Patch()
{
    Patch_Byte(0x005899F1+1, 0x50); // "cl" (zero) to "dl" (1)
}


/**
 *  #issue-8
 *  
 *  Fixes MultiMission "MaxPlayers" incorrectly loaded with "MinPlayers".
 * 
 *  @author: CCHyper
 */
static void _MultiMission_Constructor_MaxPlayers_Typo_Patch()
{
    static const char *TEXT_MAXPLAYERS = "MaxPlayers";
    Patch_Dword(0x005EF124+1, (uintptr_t)TEXT_MAXPLAYERS); // +1 skips "push" opcode
    Patch_Dword(0x005EF5E4+1, (uintptr_t)TEXT_MAXPLAYERS); // +1 skips "push" opcode
}


/**
 *  #issue-262
 * 
 *  In certain cases, the mouse might not be shown on the Dropship Loadout menu.
 *  This patch fixes that by showing the mouse regardless of its current state.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Start_Scenario_Dropship_Loadout_Show_Mouse_Patch)
{
    /**
     *  issue-284
     * 
     *  Play a background theme during the loadout menu.
     * 
     *  @author: CCHyper
     */
    if (!Theme.Still_Playing()) {

        /**
         *  If DSHPLOAD is defined in THEME.INI, play that, otherwise default
         *  to playing the TS Maps theme.
         */
        ThemeType theme = Theme.From_Name("DSHPLOAD");
        if (theme == THEME_NONE) {
            theme = Theme.From_Name("MAPS");
        }

        Theme.Play_Song(theme);
    }

    WWMouse->Release_Mouse();
    WWMouse->Show_Mouse();

    Dropship_Loadout();

    WWMouse->Hide_Mouse();
    WWMouse->Capture_Mouse();

    if (Theme.Still_Playing()) {
        Theme.Stop(true); // Smoothly fade out the track.
    }

    JMP(0x005DB3C0);
}

static void _Dropship_Loadout_Show_Mouse_Patch()
{
    Patch_Jump(0x005DB3BB, &_Start_Scenario_Dropship_Loadout_Show_Mouse_Patch);
}


/**
 *  Scale up the input rect to the desired width and height, while maintaining the aspect ratio.
 * 
 *  @author: CCHyper
 */
static bool Scale_Video_Rect(Rect &rect, int max_width, int max_height, bool maintain_ratio = false)
{
    /**
     *  No need to scale the rect if it is larger than the max width/height
     */
    bool smaller = rect.Width < max_width && rect.Height < max_height;
    if (!smaller) {
        return false;
    }

    /**
     *  This is a workaround for edge case issues with some versions
     *  of cnc-ddraw. This ensures the available draw area is actually
     *  the resolution the user defines, not what the cnc-ddraw forces
     *  the primary surface to.
     */
    int surface_width = std::clamp(HiddenSurface->Width, 0, Options.ScreenWidth);
    int surface_height = std::clamp(HiddenSurface->Height, 0, Options.ScreenHeight);

    if (maintain_ratio) {

        double dSurfaceWidth = surface_width;
        double dSurfaceHeight = surface_height;
        double dSurfaceAspectRatio = dSurfaceWidth / dSurfaceHeight;

        double dVideoWidth = rect.Width;
        double dVideoHeight = rect.Height;
        double dVideoAspectRatio = dVideoWidth / dVideoHeight;
    
        /**
         *  If the aspect ratios are the same then the screen rectangle
         *  will do, otherwise we need to calculate the new rectangle.
         */
        if (dVideoAspectRatio > dSurfaceAspectRatio) {
            int nNewHeight = (int)(surface_width/dVideoWidth*dVideoHeight);
            int nCenteringFactor = (surface_height - nNewHeight) / 2;
            rect.X = 0;
            rect.Y = nCenteringFactor;
            rect.Width = surface_width;
            rect.Height = nNewHeight;

        } else if (dVideoAspectRatio < dSurfaceAspectRatio) {
            int nNewWidth = (int)(surface_height/dVideoHeight*dVideoWidth);
            int nCenteringFactor = (surface_width - nNewWidth) / 2;
            rect.X = nCenteringFactor;
            rect.Y = 0;
            rect.Width = nNewWidth;
            rect.Height = surface_height;

        } else {
            rect.X = 0;
            rect.Y = 0;
            rect.Width = surface_width;
            rect.Height = surface_height;
        }

    } else {
        rect.X = 0;
        rect.Y = 0;
        rect.Width = surface_width;
        rect.Height = surface_height;
    }

    return true;
}


/**
 *  #issue-292
 * 
 *  Videos stretch to the whole screen size and ignore the video aspect ratio.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Play_Movie_Scale_By_Ratio_Patch)
{
    GET_REGISTER_STATIC(MovieClass *, this_ptr, esi);
    static Rect stretched_rect;

    /**
     *  Calculate the stretched rect for this video, maintaining the video ratio.
     */
    stretched_rect = this_ptr->VideoRect;
    if (Scale_Video_Rect(stretched_rect, HiddenSurface->Width, HiddenSurface->Height, true)) {

        /**
         *  Stretched rect calculated, assign it to the movie instance.
         */
        this_ptr->StretchRect = stretched_rect;

        DEBUG_INFO("Stretching movie - VideoRect: %d,%d -> StretchRect: %d,%d\n",
                this_ptr->VideoRect.Width, this_ptr->VideoRect.Height,
                this_ptr->StretchRect.Width, this_ptr->StretchRect.Height);

        /*DEBUG_GAME("Stretching movie %dx%d -> %dx%d\n",
            this_ptr->VideoRect.Width, this_ptr->VideoRect.Height, this_ptr->StretchRect.Width, this_ptr->StretchRect.Height);*/
    }

    JMP(0x00563805);
}

static void _Scale_Movies_By_Ratio_Patch()
{
    Patch_Jump(0x00563795, &_Play_Movie_Scale_By_Ratio_Patch);
}


/**
 *  #issue-187
 *  
 *  Fixes incorrect spelling of "Loser" on the multiplayer score screen debug output.
 * 
 *  @author: CCHyper
 */
static void _MultiScore_Tally_Score_Fix_Loser_Typo_Patch()
{
    static const char *TEXT_LOSER = "Loser";
    Patch_Dword(0x00568A05+1, (uintptr_t)TEXT_LOSER); // +1 skips "mov eax," opcode
}


/**
 *  #issue-287
 * 
 *  Main menu transition videos incorrectly scale up when "StretchMovies=true".
 * 
 *  @author: CCHyper
 */
static void _Dont_Stretch_Main_Menu_Video_Patch()
{
    /**
     *  Change Play_Movie "stretch_allowed" arg to false.
     */
    Patch_Byte(0x0057FF34+1, 0); // TS_TITLE.VQA
    Patch_Byte(0x0057FECF+1, 0); // FS_TITLE.VQA
}


/**
 *  #issue-269
 * 
 *  Adds a "Load Game" button to the dialog shown on mission lose.
 * 
 *  @author: CCHyper
 */
static bool _Save_Games_Available()
{
    return LoadOptionsClass().Read_Save_Files();
}

static bool _Do_Load_Dialog()
{
    return LoadOptionsClass().Load_Dialog();
}

DECLARE_PATCH(_Do_Lose_Create_Lose_WWMessageBox)
{
    static int ret;

    /**
     *  Show the message box.
     */
retry_dialog:
    ret = Vinifera_Do_WWMessageBox(Text_String(TXT_TO_REPLAY), Text_String(TXT_YES), Text_String(TXT_NO), "Load Game");
    switch (ret) {
        default:
        case 0: // User pressed "Yes"
            JMP(0x005DCE1A);

        case 1: // User pressed "No"
            JMP(0x005DCE56);

        case 2: // User pressed "Load Game"
        {
#ifdef RELEASE
            /**
             *  If no save games are available, notify the user and return back
             *  and reissue the main dialog.
             */
            if (!_Save_Games_Available()) {
                Vinifera_Do_WWMessageBox("No saved games available.", Text_String(TXT_OK));
                goto retry_dialog;
            }

            /**
             *  Show the load game dialog.
             */
            ret = _Do_Load_Dialog();
            if (ret) {
                Theme.Stop();
                JMP(0x005DCE48);
            }
#else
            /**
             *  We disable loading in non-release builds.
             */
            Vinifera_Do_WWMessageBox("No saved games available.", Text_String(TXT_OK));
#endif

            /**
             *  Reissue the dialog if the user pressed cancel on the load dialog.
             */
            goto retry_dialog;
        }
    };
}

static void _Show_Load_Game_On_Mission_Failure()
{
    Patch_Jump(0x005DCDFD, &_Do_Lose_Create_Lose_WWMessageBox);
}


/**
 *  #issue-95
 * 
 *  Patch for handling the campaign intro movies
 *  for "The First Decade" and "Freeware TS" installations.
 * 
 *  @author: CCHyper
 */
static bool Play_Intro_Movie(CampaignType campaign_id)
{
    /**
     *  Catch any cases where we might be starting a non-campaign scenario.
     */
    if (campaign_id == CAMPAIGN_NONE) {
        return false;
    }

    /**
     *  Only handle campaigns with either DISK_GDI (0) or DISK_NOD (1) set.
     */
    int cd_num = Campaigns[campaign_id]->WhichCD;
    if (cd_num >= 0 && cd_num < 2) {

        /**
         *  And make sure its only the first mission of this campaign.
         */
        if (Scen->Scenario == 1) {

            /**
             *  Finally, make sure this is the first map of each factions campaign.
             */
            if (std::strcmp(Scen->ScenarioName, "GDI1A.MAP") == 0
             || std::strcmp(Scen->ScenarioName, "NOD1A.MAP") == 0) {

                /**
                 *  "The First Decade" and "Freeware TS" installations reshuffle
                 *  the movie files due to all mix files being local now and a
                 *  primitive "no-cd" added;
                 *  
                 *  MOVIES01.MIX -> INTRO.VQA (GDI) is now INTR0.VQA
                 *  MOVIES02.MIX -> INTRO.VQA (NOD) is now INTR1.VQA
                 * 
                 *  Build the movies filename based on the current campaigns desired CD (see DiskID enum). 
                 */
                char filename[12];
                std::snprintf(filename, sizeof(filename), "INTR%d.VQA", cd_num);

                /**
                 *  Now play the movie if it is found, falling back to original behavior otherwise.
                 */
                if (CCFileClass(filename).Is_Available()) {
                    DEBUG_INFO("About to play %s.\n", filename);
                    Play_Movie(filename);

                } else {
                    DEBUG_INFO("About to play INTRO.VQA.\n");
                    Play_Movie("INTRO.VQA");
                }

                return true;

            }

        }

    }

    return false;
}

DECLARE_PATCH(_Start_Scenario_Intro_Movie_Patch)
{
    GET_REGISTER_STATIC(CampaignType, campaign_id, ebx);
    GET_REGISTER_STATIC(char *, name, ebp);

    Play_Intro_Movie(campaign_id);

read_scenario:
    //JMP(0x005DB319);

    /**
     *  The First Decade" and "Freeware TS" EXE's actually have patched code at
     *  the address 0x005DB319, so lets handle the debug log print ourself and
     *  jump back at a safe location.
     */
    DEBUG_GAME("Reading scenario: %s\n", name);
    JMP(0x005DB327);
}


/**
 *  #issue-95
 * 
 *  Patch for handling the campaign intro movies for "The First Decade"
 *  and "Freeware TS" installations when selecting "Intro / Sneak Peak" on
 *  the main menu.
 * 
 *  @author: CCHyper
 */
static void Play_Intro_SneakPeak_Movies()
{
    /**
     *  Backup the current volume.
     */
    //int disk = CD::RequiredCD;

    /**
     *  Find out what movies are available locally.
     */
    bool intro_available = CCFileClass("INTRO.VQA").Is_Available();
    bool intr0_available = CCFileClass("INTR0.VQA").Is_Available();
    bool sizzle_available = CCFileClass("SIZZLE1.VQA").Is_Available();

    bool movie_pair_available = (intro_available && sizzle_available) || (intr0_available && sizzle_available);

    /**
     *  If at least one of the movie pairs were found, we can go ahead and play
     *  them, otherwise set the required disk to GDI and request it if not present.
     */
    if (movie_pair_available || (CD::Set_Required_CD(DISK_GDI), CD().Is_Available(DISK_GDI))) {
        
        /**
         *  Play the intro movie (GDI).
         * 
         *  If the renamed intro is found play that, otherwise falling back to original behavior.
         */
        if (intr0_available) {
            DEBUG_INFO("About to play INTR0.VQA.\n");
            Play_Movie("INTR0.VQA");

            /**
             *  Also attempt to play the NOD intro, just because its a nice improvement.
             */
            DEBUG_INFO("About to play INTR1.VQA.\n");
            Play_Movie("INTR1.VQA");
    
        } else {
        
            DEBUG_INFO("About to play INTRO.VQA.\n");
            Play_Movie("INTRO.VQA");
        }

        /**
         *  Play the sizzle/showreel. This exists loosely on both disks, so we tell
         *  the VQA playback to not use the normal mix file handler.
         */
        VQA_Clear_Option(OPTION_USE_MIX_HANDLER);
        DEBUG_INFO("About to play SIZZLE1.VQA.\n");
        Play_Movie("SIZZLE1.VQA");
        VQA_Set_Option(OPTION_USE_MIX_HANDLER);

    } else {
        DEBUG_WARNING("Failed to find Intro and Sizzle movies!\n");
    }

    /**
     *  Restore the previous volume.
     */
    //CD::Set_Required_CD(disk);
    //CD().Force_Available(disk);
}

DECLARE_PATCH(_Select_Game_Intro_SneakPeak_Movies_Patch)
{
    Play_Intro_SneakPeak_Movies();

    JMP(0x004E288B);
}

static void _Intro_Movie_Patches()
{
    Patch_Jump(0x005DB2DE, &_Start_Scenario_Intro_Movie_Patch);
    Patch_Jump(0x004E2796, &_Select_Game_Intro_SneakPeak_Movies_Patch);
}


/**
 *  #issue-244
 * 
 *  Changes the default value of "AllowHiResModes" to "true".
 * 
 *  #NOTE: This should be moved to the OptionsClass extension when implemented!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_OptionsClass_Constructor_AllowHiResModes_Default_Patch)
{
    _asm { mov [eax+25h], 1 }
    _asm { ret }
}

static void _AllowHiResModes_Default_Patch()
{
    //Patch_Byte(0x005899D6+1, 0x50); // "cl" (zero) to "dl" (1)
    Patch_Jump(0x00589A12, &_OptionsClass_Constructor_AllowHiResModes_Default_Patch);
}


/**
 *  Main function for patching the hooks.
 */
void BugFix_Hooks()
{
    _AllowHiResModes_Default_Patch();
    _Intro_Movie_Patches();
    _Show_Load_Game_On_Mission_Failure();
    _Dont_Stretch_Main_Menu_Video_Patch();
    _MultiScore_Tally_Score_Fix_Loser_Typo_Patch();
    _Scale_Movies_By_Ratio_Patch();
    _Dropship_Loadout_Show_Mouse_Patch();
    _MultiMission_Constructor_MaxPlayers_Typo_Patch();
    _OptionsClass_Constructor_IsScoreShuffle_Default_Patch();
    _Scroll_Sidebar_InGame_Check_Patch();
    _Set_Difficulty_On_Game_Restart_Patch();
    _EndGameClass_Debug_Output_Patches();




#define INCREASE_MAP_SIZE_LIMIT 1

#if INCREASE_MAP_SIZE_LIMIT

#define NEW_MAP_W 1024
#define NEW_MAP_H 1024
#define NEW_MAP_CELL_TOTAL (NEW_MAP_W*NEW_MAP_H)


/**
 *  "MAP_CELL_W and MAP_CELL_H" to "NEW_MAP_W and NEW_MAP_H".
 */
Patch_Dword(0x0046E90E+1, NEW_MAP_W);
Patch_Dword(0x0046E923+1, NEW_MAP_H);
Patch_Dword(0x004767DA+1, NEW_MAP_W); // Actually optimised use of W and H.
Patch_Dword(0x0047988B+1, NEW_MAP_W); // Actually optimised use of W and H.
Patch_Dword(0x0047A64E+2, NEW_MAP_W); // is this W or H?
Patch_Dword(0x0047A664+1, NEW_MAP_W*1000); // needs confirming what this is for.
Patch_Word(0x00492B77+2, NEW_MAP_W);
Patch_Word(0x00492B8A+3, NEW_MAP_H);
Patch_Word(0x0049301E+2, NEW_MAP_W);
Patch_Word(0x00493029+3, NEW_MAP_H);
Patch_Word(0x005017C1+2, NEW_MAP_W);
Patch_Word(0x005017D4+3, NEW_MAP_H);
Patch_Dword(0x0050F2F2+1, NEW_MAP_W); // Actually optimised use of W and H.
Patch_Dword(0x0050F6C0+1, NEW_MAP_W); // Actually optimised use of W and H.
Patch_Dword(0x0050FDE4+2, NEW_MAP_W); // Actually optimised use of W and H.     ?????
Patch_Dword(0x0052D539+1, NEW_MAP_H);
Patch_Dword(0x0052D54D+1, NEW_MAP_H);

// map gen stuff?
// 0053FAF8 ????????
// 00542515 ??????
// 0054261D ?????
// 0054265A ?????
// 005433BE ????
// 005433C9 ?????
// 0054346B ?????
// 00543505 ?????
// 00543575 ?????
// 00543586 ?????
// 00545549 ?????

Patch_Dword(0x0058C0D3+2, NEW_MAP_W);
Patch_Dword(0x0058C0E4+1, NEW_MAP_H);
Patch_Dword(0x0058C21A+2, NEW_MAP_W);
Patch_Dword(0x0058C223+2, NEW_MAP_H);
Patch_Dword(0x0058C344+2, NEW_MAP_W);
Patch_Dword(0x0058C34D+2, NEW_MAP_H);
Patch_Dword(0x0058C43B+2, NEW_MAP_W);
Patch_Dword(0x0058C444+2, NEW_MAP_H);
Patch_Dword(0x005FADDB+2, NEW_MAP_W);
Patch_Dword(0x005FADE8+2, NEW_MAP_H);


/**
 *  Bit packing of X/Y.
 * 
 *  Changes 9 (512) to 10 (1024) for shl, shr, sar.
 */
Patch_Byte(0x0050F780+2, 10); //            ?Set_Map_Dimensions@MapClass@@UAEXAAVRect@@_NH1@Z            sar     eax, 9
Patch_Byte(0x00510799+2, 10); //            ?Set_Map_Dimensions@MapClass@@UAEXAAVRect@@_NH1@Z            sar     eax, 9
Patch_Byte(0x0050F1CA+2, 10); //            ?Is_Visible@MapClass@@UAGHUCellStruct@@@Z            shl     eax, 9
Patch_Byte(0x0050F28C+2, 10); //            ??AMapClass@@QBEABVCellClass@@ABUCellStruct@@@Z            shl     eax, 9
Patch_Byte(0x0050FEDD+2, 10); //            ?Set_Map_Dimensions@MapClass@@UAEXAAVRect@@_NH1@Z            shl     eax, 9
Patch_Byte(0x00510EE1+2, 10); //            ?Sight_From@MapClass@@QAEXAAUCellStruct@@HPAVHouseClass@@_N222@Z            shl     eax, 9
Patch_Byte(0x00511451+2, 10); //            ?Overpass@MapClass@@QAEJXZ            shl     eax, 9
Patch_Byte(0x005114CB+2, 10); //            ?Overpass@MapClass@@QAEJXZ            shl     eax, 9
Patch_Byte(0x00511599+2, 10); //            MapClass_511570            shl     eax, 9
Patch_Byte(0x0051164A+2, 10); //            MapClass_511570            shl     eax, 9
Patch_Byte(0x00511745+2, 10); //            MapClass_511570            shl     eax, 9
Patch_Byte(0x0051179B+2, 10); //            MapClass_511570            shl     eax, 9
Patch_Byte(0x005117F1+2, 10); //            MapClass_511570            shl     eax, 9
Patch_Byte(0x00511968+2, 10); //            MapClass_511570            shl     eax, 9
Patch_Byte(0x005119BE+2, 10); //            MapClass_511570            shl     eax, 9
Patch_Byte(0x00511A14+2, 10); //            MapClass_511570            shl     eax, 9
Patch_Byte(0x00511BD3+2, 10); //            MapClass_511570            shl     eax, 9
Patch_Byte(0x00511D31+2, 10); //            MapClass_511570            shl     eax, 9
Patch_Byte(0x00511EA9+2, 10); //            MapClass_511E80            shl     eax, 9
Patch_Byte(0x00511F5A+2, 10); //            MapClass_511E80            shl     eax, 9
Patch_Byte(0x00512055+2, 10); //            MapClass_511E80            shl     eax, 9
Patch_Byte(0x005120AB+2, 10); //            MapClass_511E80            shl     eax, 9
Patch_Byte(0x00512101+2, 10); //            MapClass_511E80            shl     eax, 9
Patch_Byte(0x00512278+2, 10); //            MapClass_511E80            shl     eax, 9
Patch_Byte(0x005122CE+2, 10); //            MapClass_511E80            shl     eax, 9
Patch_Byte(0x00512324+2, 10); //            MapClass_511E80            shl     eax, 9
Patch_Byte(0x005124E3+2, 10); //            MapClass_511E80            shl     eax, 9
Patch_Byte(0x00512647+2, 10); //            MapClass_511E80            shl     eax, 9
Patch_Byte(0x005129C1+2, 10); //            MapClass_5127A0            shl     eax, 9
Patch_Byte(0x00512AB8+2, 10); //            MapClass_5127A0            shl     eax, 9
Patch_Byte(0x00512B0E+2, 10); //            MapClass_5127A0            shl     eax, 9
Patch_Byte(0x00512C8C+2, 10); //            MapClass_512BE0            shl     eax, 9
Patch_Byte(0x00512ED9+2, 10); //            MapClass_512BE0            shl     eax, 9
Patch_Byte(0x0051304D+2, 10); //            MapClass_512BE0            shl     eax, 9
Patch_Byte(0x0051314C+2, 10); //            MapClass_5130A0            shl     eax, 9
Patch_Byte(0x00513399+2, 10); //            MapClass_5130A0            shl     eax, 9
Patch_Byte(0x0051350D+2, 10); //            MapClass_5130A0            shl     eax, 9
Patch_Byte(0x00513D12+2, 10); //            W?Read_Binary$:MapClass$n(rn$Straw$$)i            shl     eax, 9
Patch_Byte(0x00513DA8+2, 10); //            W?Read_Binary$:MapClass$n(rn$Straw$$)i            shl     eax, 9
Patch_Byte(0x00513E24+2, 10); //            W?Read_Binary$:MapClass$n(rn$Straw$$)i            shl     eax, 9
Patch_Byte(0x00513EFB+2, 10); //            W?Read_Binary_Pack_2$:MapClass$n(rn$Straw$$)i            shl     eax, 9
Patch_Byte(0x00514000+2, 10); //            W?Read_Binary_Pack_3$:MapClass$n(rn$Straw$$)i            shl     eax, 9
Patch_Byte(0x0051411B+2, 10); //            W?Read_Binary_Pack_4$:MapClass$n(rn$Straw$$)i            shl     eax, 9
Patch_Byte(0x0051423B+2, 10); //            W?Read_Binary_Pack_5$:MapClass$n(rn$Straw$$)i            shl     eax, 9
Patch_Byte(0x0051449B+2, 10); //            ?Cell_Threat@MapClass@@QAEHAAUCellStruct@@PAVHouseClass@@@Z            shl     eax, 9
Patch_Byte(0x005145E4+2, 10); //            ?Remove_Crate@MapClass@@QAE_NAAUCellStruct@@@Z            shl     eax, 9
Patch_Byte(0x0051467E+2, 10); //            ?Remove_Crate@MapClass@@QAE_NAAUCellStruct@@@Z            shl     eax, 9
Patch_Byte(0x00514986+2, 10); //            ?Close_Object@MapClass@@QBEPAVObjectClass@@AAUCoordStruct@@@Z            shl     eax, 9
Patch_Byte(0x00515832+2, 10); //            MapClass_CanLocationBeReached            shl     eax, 9
Patch_Byte(0x0051589C+2, 10); //            MapClass_CanLocationBeReached            shl     eax, 9
Patch_Byte(0x005161EA+2, 10); //            ?Nearby_Location@MapClass@@QBE?AUCellStruct@@AAU2@W4SpeedType@@HW4MZoneType@@_NHH3333U2@@Z            shl     eax, 9
Patch_Byte(0x0051622E+2, 10); //            ?Nearby_Location@MapClass@@QBE?AUCellStruct@@AAU2@W4SpeedType@@HW4MZoneType@@_NHH3333U2@@Z            shl     eax, 9
Patch_Byte(0x005162C2+2, 10); //            ?Nearby_Location@MapClass@@QBE?AUCellStruct@@AAU2@W4SpeedType@@HW4MZoneType@@_NHH3333U2@@Z            shl     eax, 9
Patch_Byte(0x00516474+2, 10); //            ?Nearby_Location@MapClass@@QBE?AUCellStruct@@AAU2@W4SpeedType@@HW4MZoneType@@_NHH3333U2@@Z            shl     eax, 9
Patch_Byte(0x005164BE+2, 10); //            ?Nearby_Location@MapClass@@QBE?AUCellStruct@@AAU2@W4SpeedType@@HW4MZoneType@@_NHH3333U2@@Z            shl     eax, 9
Patch_Byte(0x00516683+2, 10); //            ?Nearby_Location@MapClass@@QBE?AUCellStruct@@AAU2@W4SpeedType@@HW4MZoneType@@_NHH3333U2@@Z            shl     eax, 9
Patch_Byte(0x00516827+2, 10); //            ?Nearby_Location@MapClass@@QBE?AUCellStruct@@AAU2@W4SpeedType@@HW4MZoneType@@_NHH3333U2@@Z            shl     eax, 9
Patch_Byte(0x00516C25+2, 10); //            MapClass_Is_Area_Clear?_516BE0            shl     eax, 9
Patch_Byte(0x00516DB6+2, 10); //            MapClass_516DA0            shl     eax, 9
Patch_Byte(0x00516F2D+2, 10); //            MapClass_516DA0            shl     eax, 9
Patch_Byte(0x00516FA6+2, 10); //            MapClass_516F90            shl     eax, 9
Patch_Byte(0x005170DE+2, 10); //            MapClass_516F90            shl     eax, 9
Patch_Byte(0x0051718A+2, 10); //            MapClass_517150            shl     eax, 9
Patch_Byte(0x0051729A+2, 10); //            MapClass_517260            shl     eax, 9
Patch_Byte(0x005173A9+2, 10); //            MapClass_517370            shl     eax, 9
Patch_Byte(0x005174AA+2, 10); //            MapClass_517370            shl     eax, 9
Patch_Byte(0x005174F8+2, 10); //            MapClass_517370            shl     eax, 9
Patch_Byte(0x00517546+2, 10); //            MapClass_517370            shl     eax, 9
Patch_Byte(0x0051758C+2, 10); //            MapClass_517370            shl     eax, 9
Patch_Byte(0x00517608+2, 10); //            MapClass_517370            shl     eax, 9
Patch_Byte(0x00517654+2, 10); //            MapClass_517370            shl     eax, 9
Patch_Byte(0x00517749+2, 10); //            MapClass_517710            shl     eax, 9
Patch_Byte(0x0051784A+2, 10); //            MapClass_517710            shl     eax, 9
Patch_Byte(0x00517898+2, 10); //            MapClass_517710            shl     eax, 9
Patch_Byte(0x005178E6+2, 10); //            MapClass_517710            shl     eax, 9
Patch_Byte(0x0051792C+2, 10); //            MapClass_517710            shl     eax, 9
Patch_Byte(0x005179A8+2, 10); //            MapClass_517710            shl     eax, 9
Patch_Byte(0x005179F4+2, 10); //            MapClass_517710            shl     eax, 9
Patch_Byte(0x00517AEA+2, 10); //            MapClass_517AB0            shl     eax, 9
Patch_Byte(0x00517BFA+2, 10); //            MapClass_517BC0            shl     eax, 9
Patch_Byte(0x00517D09+2, 10); //            MapClass_517CD0            shl     eax, 9
Patch_Byte(0x00517E12+2, 10); //            MapClass_517CD0            shl     eax, 9
Patch_Byte(0x00517E5F+2, 10); //            MapClass_517CD0            shl     eax, 9
Patch_Byte(0x00517EAD+2, 10); //            MapClass_517CD0            shl     eax, 9
Patch_Byte(0x00517EFA+2, 10); //            MapClass_517CD0            shl     eax, 9
Patch_Byte(0x00517F6B+2, 10); //            MapClass_517CD0            shl     eax, 9
Patch_Byte(0x00517FB5+2, 10); //            MapClass_517CD0            shl     eax, 9
Patch_Byte(0x005180D9+2, 10); //            MapClass_5180A0            shl     eax, 9
Patch_Byte(0x005181E2+2, 10); //            MapClass_5180A0            shl     eax, 9
Patch_Byte(0x0051822F+2, 10); //            MapClass_5180A0            shl     eax, 9
Patch_Byte(0x0051827D+2, 10); //            MapClass_5180A0            shl     eax, 9
Patch_Byte(0x005182CA+2, 10); //            MapClass_5180A0            shl     eax, 9
Patch_Byte(0x0051833B+2, 10); //            MapClass_5180A0            shl     eax, 9
Patch_Byte(0x00518385+2, 10); //            MapClass_5180A0            shl     eax, 9
Patch_Byte(0x0051850D+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518579+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x005185D3+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518629+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x005186F5+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x0051880F+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518943+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x005189BE+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518A15+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518A6A+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518B62+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518BE9+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518C9B+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518CF1+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518D47+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518E3F+2, 10); //            MapClass_RepairBridge_DirA            shl     eax, 9
Patch_Byte(0x00518F12+2, 10); //            MapClass_518F00            shl     eax, 9
Patch_Byte(0x00518F9E+2, 10); //            MapClass_518F00            shl     eax, 9
Patch_Byte(0x005190C5+2, 10); //            MapClass_518F00            shl     eax, 9
Patch_Byte(0x00519353+2, 10); //            MapClass_518F00            shl     eax, 9
Patch_Byte(0x005193DF+2, 10); //            MapClass_518F00            shl     eax, 9
Patch_Byte(0x005194BA+2, 10); //            MapClass_519470            shl     eax, 9
Patch_Byte(0x005196A9+2, 10); //            MapClass_519470            shl     eax, 9
Patch_Byte(0x005198DC+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519A22+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519A61+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519ACC+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519B15+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519B63+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519BB3+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519C5B+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519CA1+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519F55+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519F92+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x00519FF9+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x0051A03C+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x0051A092+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x0051A0E3+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x0051A14B+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x0051A18E+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x0051A1DA+2, 10); //            MapClass_5198C0            shl     eax, 9
Patch_Byte(0x0051A69A+2, 10); //            MapClass_51A660            shl     eax, 9
Patch_Byte(0x0051A7AA+2, 10); //            MapClass_51A770            shl     eax, 9
Patch_Byte(0x0051A8B9+2, 10); //            MapClass_51A880            shl     eax, 9
Patch_Byte(0x0051A9BA+2, 10); //            MapClass_51A880            shl     eax, 9
Patch_Byte(0x0051AA08+2, 10); //            MapClass_51A880            shl     eax, 9
Patch_Byte(0x0051AA56+2, 10); //            MapClass_51A880            shl     eax, 9
Patch_Byte(0x0051AA9C+2, 10); //            MapClass_51A880            shl     eax, 9
Patch_Byte(0x0051AB18+2, 10); //            MapClass_51A880            shl     eax, 9
Patch_Byte(0x0051AB64+2, 10); //            MapClass_51A880            shl     eax, 9
Patch_Byte(0x0051AC59+2, 10); //            MapClass_51AC20            shl     eax, 9
Patch_Byte(0x0051AD5A+2, 10); //            MapClass_51AC20            shl     eax, 9
Patch_Byte(0x0051ADA8+2, 10); //            MapClass_51AC20            shl     eax, 9
Patch_Byte(0x0051ADF6+2, 10); //            MapClass_51AC20            shl     eax, 9
Patch_Byte(0x0051AE3C+2, 10); //            MapClass_51AC20            shl     eax, 9
Patch_Byte(0x0051AEB8+2, 10); //            MapClass_51AC20            shl     eax, 9
Patch_Byte(0x0051AF04+2, 10); //            MapClass_51AC20            shl     eax, 9
Patch_Byte(0x0051AFFA+2, 10); //            MapClass_51AFC0            shl     eax, 9
Patch_Byte(0x0051B10A+2, 10); //            MapClass_51B0D0            shl     eax, 9
Patch_Byte(0x0051B219+2, 10); //            MapClass_51B1E0            shl     eax, 9
Patch_Byte(0x0051B322+2, 10); //            MapClass_51B1E0            shl     eax, 9
Patch_Byte(0x0051B36F+2, 10); //            MapClass_51B1E0            shl     eax, 9
Patch_Byte(0x0051B3BD+2, 10); //            MapClass_51B1E0            shl     eax, 9
Patch_Byte(0x0051B40A+2, 10); //            MapClass_51B1E0            shl     eax, 9
Patch_Byte(0x0051B47B+2, 10); //            MapClass_51B1E0            shl     eax, 9
Patch_Byte(0x0051B4C5+2, 10); //            MapClass_51B1E0            shl     eax, 9
Patch_Byte(0x0051B5E9+2, 10); //            MapClass_51B5B0            shl     eax, 9
Patch_Byte(0x0051B6F2+2, 10); //            MapClass_51B5B0            shl     eax, 9
Patch_Byte(0x0051B73F+2, 10); //            MapClass_51B5B0            shl     eax, 9
Patch_Byte(0x0051B78D+2, 10); //            MapClass_51B5B0            shl     eax, 9
Patch_Byte(0x0051B7DA+2, 10); //            MapClass_51B5B0            shl     eax, 9
Patch_Byte(0x0051B84B+2, 10); //            MapClass_51B5B0            shl     eax, 9
Patch_Byte(0x0051B895+2, 10); //            MapClass_51B5B0            shl     eax, 9
Patch_Byte(0x0051BA1A+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051BA83+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051BADD+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051BB33+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051BD29+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051BE74+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051BEF8+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051BF52+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051BFA8+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051C0A1+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051C128+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051C1DA+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051C230+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051C286+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051C37E+2, 10); //            MapClass_RepairBridge_DirB            shl     eax, 9
Patch_Byte(0x0051C4B8+2, 10); //            MapClass_Bridge_Destroyed_51C440            shl     eax, 9
Patch_Byte(0x0051C52A+2, 10); //            MapClass_Bridge_Destroyed_51C440            shl     eax, 9
Patch_Byte(0x0051C58F+2, 10); //            MapClass_Bridge_Destroyed_51C440            shl     eax, 9
Patch_Byte(0x0051C5F3+2, 10); //            MapClass_Bridge_Destroyed_51C440            shl     eax, 9
Patch_Byte(0x0051C65E+2, 10); //            MapClass_Bridge_Destroyed_51C440            shl     eax, 9
Patch_Byte(0x0051C6BF+2, 10); //            MapClass_Bridge_Destroyed_51C440            shl     eax, 9
Patch_Byte(0x0051C772+2, 10); //            MapClass_51C760            shl     eax, 9
Patch_Byte(0x0051C7FE+2, 10); //            MapClass_51C760            shl     eax, 9
Patch_Byte(0x0051C925+2, 10); //            MapClass_51C760            shl     eax, 9
Patch_Byte(0x0051CBB3+2, 10); //            MapClass_51C760            shl     eax, 9
Patch_Byte(0x0051CC3F+2, 10); //            MapClass_51C760            shl     eax, 9
Patch_Byte(0x0051CD18+2, 10); //            MapClass_51CCD0            shl     eax, 9
Patch_Byte(0x0051CEFB+2, 10); //            MapClass_51CCD0            shl     eax, 9
Patch_Byte(0x0051D112+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D232+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D279+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D2E2+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D32D+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D37B+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D3CB+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D47E+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D4C4+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D762+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D7A7+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D80E+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D852+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D8A8+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D8F9+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D965+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D9AD+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051D9FA+2, 10); //            MapClass_51D100            shl     eax, 9
Patch_Byte(0x0051DEBC+2, 10); //            ?Detach@MapClass@@UAEXPAVAbstractClass@@_N@Z            shl     eax, 9
Patch_Byte(0x0051E241+2, 10); //            ?Iterator_Next_Cell@MapClass@@QAEPAVCellClass@@XZ            shl     eax, 9
Patch_Byte(0x0051E39B+2, 10); //            ?In_Radar@MapClass@@QBE_NAAUCellStruct@@_N@Z            shl     eax, 9
Patch_Byte(0x0051E57F+2, 10); //            MapClass_51E560            shl     eax, 9
Patch_Byte(0x0051E73D+2, 10); //            MapClass_51E630            shl     eax, 9
Patch_Byte(0x0051E7BB+2, 10); //            MapClass_51E7A0            shl     eax, 9
Patch_Byte(0x0051E8F0+2, 10); //            MapClass_51E7A0            shl     eax, 9
Patch_Byte(0x0051E951+2, 10); //            MapClass_51E7A0            shl     eax, 9
Patch_Byte(0x0051E994+2, 10); //            MapClass_51E7A0            shl     eax, 9
Patch_Byte(0x0051EB3B+2, 10); //            MapClass_51EB30            shl     eax, 9
Patch_Byte(0x0051EC4E+2, 10); //            MapClass_SmoothIce_51EBD0            shl     eax, 9
Patch_Byte(0x0051ED13+2, 10); //            MapClass_SmoothIce_51EBD0            shl     eax, 9
Patch_Byte(0x0051ED55+2, 10); //            MapClass_SmoothIce_51EBD0            shl     eax, 9
Patch_Byte(0x0051EF6C+2, 10); //            MapClass_SmoothIce_51EBD0            shl     eax, 9
Patch_Byte(0x0051EFC9+2, 10); //            MapClass_SmoothIce_51EBD0            shl     eax, 9
Patch_Byte(0x0051F06D+2, 10); //            MapClass_Ice_Stuff_51F020            shl     eax, 9
Patch_Byte(0x0051F10C+2, 10); //            MapClass_Ice_Stuff_51F020            shl     eax, 9
Patch_Byte(0x0051F158+2, 10); //            MapClass_Ice_Stuff_51F020            shl     eax, 9
Patch_Byte(0x0051F197+2, 10); //            MapClass_Ice_Stuff_51F020            shl     eax, 9
Patch_Byte(0x0051F1D6+2, 10); //            MapClass_Ice_Stuff_51F020            shl     eax, 9
Patch_Byte(0x0051F36B+2, 10); //            MapClass_Ice_Stuff_51F020            shl     eax, 9
Patch_Byte(0x0051F406+2, 10); //            MapClass_Ice_Stuff_51F020            shl     eax, 9
Patch_Byte(0x0051F43B+2, 10); //            MapClass_Ice_Stuff_51F020            shl     eax, 9
Patch_Byte(0x0051F474+2, 10); //            MapClass_Ice_Stuff_51F020            shl     eax, 9
Patch_Byte(0x0051F5E8+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F65A+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F69D+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F6E0+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F71F+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F764+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F7A5+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F7E6+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F828+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F8FE+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F953+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F98D+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051F9C2+2, 10); //            MapClass_Ice_Stuff_51F5B0            shl     eax, 9
Patch_Byte(0x0051FBF2+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0051FCBB+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0051FD0C+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0051FD4F+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0051FD8E+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0051FDD3+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0051FE15+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0051FE5C+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0051FEA3+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x00520146+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0052017F+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x005201CE+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0052020F+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x00520244+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x0052027D+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x00520473+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x005204A8+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x005204E1+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     eax, 9
Patch_Byte(0x00520A80+2, 10); //            MapClass_Object_Moving_Over_Ice?_520900            shl     eax, 9
Patch_Byte(0x00520B2C+2, 10); //            MapClass_Object_Moving_Over_Ice?_520900            shl     eax, 9
Patch_Byte(0x00520BF7+2, 10); //            MapClass_Object_Moving_Over_Ice?_520900            shl     eax, 9
Patch_Byte(0x00521091+2, 10); //            MapClass_Grow_Ice?_520F40            shl     eax, 9
Patch_Byte(0x00521104+2, 10); //            MapClass_Grow_Ice?_520F40            shl     eax, 9
Patch_Byte(0x00521176+2, 10); //            MapClass_Grow_Ice?_520F40            shl     eax, 9
Patch_Byte(0x005211B5+2, 10); //            MapClass_Grow_Ice?_520F40            shl     eax, 9
Patch_Byte(0x005217BE+2, 10); //            MapClass%sub_521760            shl     eax, 9
Patch_Byte(0x0052181C+2, 10); //            MapClass%sub_521760            shl     eax, 9
Patch_Byte(0x00521879+2, 10); //            MapClass%sub_521760            shl     eax, 9
Patch_Byte(0x005218D6+2, 10); //            MapClass%sub_521760            shl     eax, 9
Patch_Byte(0x00521DF2+2, 10); //            MapClass%sub_521D70            shl     eax, 9
Patch_Byte(0x00521E44+2, 10); //            MapClass%sub_521D70            shl     eax, 9
Patch_Byte(0x00521EF6+2, 10); //            MapClass%sub_521D70            shl     eax, 9
Patch_Byte(0x00521F4A+2, 10); //            MapClass%sub_521D70            shl     eax, 9
Patch_Byte(0x0052203E+2, 10); //            MapClass%sub_521D70            shl     eax, 9
Patch_Byte(0x005220D5+2, 10); //            MapClass%sub_521D70            shl     eax, 9
Patch_Byte(0x00522128+2, 10); //            MapClass%sub_521D70            shl     eax, 9
Patch_Byte(0x005221CF+2, 10); //            MapClass%sub_521D70            shl     eax, 9
Patch_Byte(0x005223F4+2, 10); //            MapClass%sub_522350            shl     eax, 9
Patch_Byte(0x0052242D+2, 10); //            MapClass%sub_522350            shl     eax, 9
Patch_Byte(0x00522474+2, 10); //            MapClass%sub_522350            shl     eax, 9
Patch_Byte(0x005224B3+2, 10); //            MapClass%sub_522350            shl     eax, 9
Patch_Byte(0x005224F2+2, 10); //            MapClass%sub_522350            shl     eax, 9
Patch_Byte(0x00522578+2, 10); //            MapClass%sub_522350            shl     eax, 9
Patch_Byte(0x005225B3+2, 10); //            MapClass%sub_522350            shl     eax, 9
Patch_Byte(0x00522CAA+2, 10); //            MapClass%sub_522BE0            shl     eax, 9
Patch_Byte(0x00522D01+2, 10); //            MapClass%sub_522BE0            shl     eax, 9
Patch_Byte(0x00522D6C+2, 10); //            MapClass%sub_522BE0            shl     eax, 9
Patch_Byte(0x00522DC0+2, 10); //            MapClass%sub_522BE0            shl     eax, 9
Patch_Byte(0x00522E2C+2, 10); //            MapClass%sub_522BE0            shl     eax, 9
Patch_Byte(0x00522E7F+2, 10); //            MapClass%sub_522BE0            shl     eax, 9
Patch_Byte(0x00522EEB+2, 10); //            MapClass%sub_522BE0            shl     eax, 9
Patch_Byte(0x00522F3E+2, 10); //            MapClass%sub_522BE0            shl     eax, 9
Patch_Byte(0x00523C1B+2, 10); //            MapClass%sub_523B50            shl     eax, 9
Patch_Byte(0x00523F20+2, 10); //            MapClass_Reduce_Tiberium            shl     eax, 9
Patch_Byte(0x00523FA2+2, 10); //            MapClass%sub_523F80            shl     eax, 9
Patch_Byte(0x00523FE1+2, 10); //            MapClass%sub_523F80            shl     eax, 9
Patch_Byte(0x005240C1+2, 10); //            MapClass%sub_5240A0            shl     eax, 9
Patch_Byte(0x00524101+2, 10); //            MapClass%sub_5240A0            shl     eax, 9
Patch_Byte(0x005241C2+2, 10); //            MapClass%sub_5241B0            shl     eax, 9
Patch_Byte(0x00524421+2, 10); //            MapClass%sub_524400            shl     eax, 9
Patch_Byte(0x00524462+2, 10); //            MapClass%sub_524400            shl     eax, 9
Patch_Byte(0x005244A3+2, 10); //            MapClass%sub_524400            shl     eax, 9
Patch_Byte(0x00524941+2, 10); //            MapClass%sub_524920            shl     eax, 9
Patch_Byte(0x00524982+2, 10); //            MapClass%sub_524920            shl     eax, 9
Patch_Byte(0x005249C3+2, 10); //            MapClass%sub_524920            shl     eax, 9
Patch_Byte(0x00524E7E+2, 10); //            MapClass%sub_524E40            shl     eax, 9
Patch_Byte(0x00524EF8+2, 10); //            MapClass%sub_524E40            shl     eax, 9
Patch_Byte(0x00524F9E+2, 10); //            MapClass%sub_524F60            shl     eax, 9
Patch_Byte(0x00525018+2, 10); //            MapClass%sub_524F60            shl     eax, 9
Patch_Byte(0x005250BE+2, 10); //            MapClass%sub_525080            shl     eax, 9
Patch_Byte(0x00525107+2, 10); //            MapClass%sub_525080            shl     eax, 9
Patch_Byte(0x0052514C+2, 10); //            MapClass%sub_525080            shl     eax, 9
Patch_Byte(0x005255F3+2, 10); //            MapClass%sub_5255B0            shl     eax, 9
Patch_Byte(0x00525632+2, 10); //            MapClass%sub_5255B0            shl     eax, 9
Patch_Byte(0x00525673+2, 10); //            MapClass%sub_5255B0            shl     eax, 9
Patch_Byte(0x00525AA2+2, 10); //            MapClass%sub_525A90            shl     eax, 9
Patch_Byte(0x00525D46+2, 10); //            MapClass%sub_525CD0            shl     eax, 9
Patch_Byte(0x00525D88+2, 10); //            MapClass%sub_525CD0            shl     eax, 9
Patch_Byte(0x00525DC4+2, 10); //            MapClass%sub_525CD0            shl     eax, 9
Patch_Byte(0x0052622C+2, 10); //            MapClass%sub_5261B0            shl     eax, 9
Patch_Byte(0x00526266+2, 10); //            MapClass%sub_5261B0            shl     eax, 9
Patch_Byte(0x00526697+2, 10); //            MapClass%sub_526690            shl     eax, 9
Patch_Byte(0x00526B34+2, 10); //            MapClass_5266E0            shl     eax, 9
Patch_Byte(0x00526E77+2, 10); //            MapClass_Collapse_Cliff            shl     eax, 9
Patch_Byte(0x00526FE6+2, 10); //            MapClass_Collapse_Cliff            shl     eax, 9
Patch_Byte(0x005270D1+2, 10); //            MapClass_Collapse_Cliff            shl     eax, 9
Patch_Byte(0x005275A7+2, 10); //            MapClass_Collapse_Cliff            shl     eax, 9
Patch_Byte(0x0052771A+2, 10); //            MapClass_Collapse_Cliff            shl     eax, 9
Patch_Byte(0x0052777C+2, 10); //            MapClass_Collapse_Cliff            shl     eax, 9
Patch_Byte(0x005277FE+2, 10); //            MapClass_Collapse_Cliff            shl     eax, 9
Patch_Byte(0x0052891B+2, 10); //            MapClass%sub_5288E0            shl     eax, 9
Patch_Byte(0x005289D0+2, 10); //            MapClass%sub_5288E0            shl     eax, 9
Patch_Byte(0x00528A0D+2, 10); //            MapClass%sub_5288E0            shl     eax, 9
Patch_Byte(0x00529080+2, 10); //            MapClass%sub_528F80            shl     eax, 9
Patch_Byte(0x0052916E+2, 10); //            MapClass%sub_529160            shl     eax, 9
Patch_Byte(0x005293B2+2, 10); //            MapClass%sub_5293A0            shl     eax, 9
Patch_Byte(0x0052A3A2+2, 10); //            MapClass%sub_52A380            shl     eax, 9
Patch_Byte(0x0052A702+2, 10); //            MapClass%sub_52A6E0            shl     eax, 9
Patch_Byte(0x0052AFA5+2, 10); //            sub_52AE50            shl     eax, 9
Patch_Byte(0x0052B093+2, 10); //            sub_52AE50            shl     eax, 9
Patch_Byte(0x0052B8D5+2, 10); //            MapClass_Cell_Is_Shrouded            shl     eax, 9
Patch_Byte(0x0052B961+2, 10); //            MapClass_Cell_Is_Shrouded            shl     eax, 9
Patch_Byte(0x0052BA3A+2, 10); //            MapClass_Is_Viewable?            shl     eax, 9
Patch_Byte(0x0052BAC4+2, 10); //            MapClass_Is_Viewable?            shl     eax, 9
Patch_Byte(0x0052BB35+2, 10); //            MapClass_52BB10            shl     eax, 9
Patch_Byte(0x0052BCE0+2, 10); //            MapClass_52BCA0            shl     eax, 9
Patch_Byte(0x0052C219+2, 10); //            MapClass_52C080            shl     eax, 9
Patch_Byte(0x0052C25F+2, 10); //            MapClass_52C080            shl     eax, 9
Patch_Byte(0x0052C35D+2, 10); //            MapClass_52C2E0            shl     eax, 9
Patch_Byte(0x0052C47B+2, 10); //            MapClass_52C450            shl     eax, 9
Patch_Byte(0x0052C62F+2, 10); //            MapClass_52C610            shl     eax, 9
Patch_Byte(0x0052C6C0+2, 10); //            MapClass_52C690            shl     eax, 9
Patch_Byte(0x0052C891+2, 10); //            MapClass_52C690            shl     eax, 9
Patch_Byte(0x0052C945+2, 10); //            MapClass_Repair_Bridge_Hut_52C900            shl     eax, 9
Patch_Byte(0x0052C9A3+2, 10); //            MapClass_Repair_Bridge_Hut_52C900            shl     eax, 9
Patch_Byte(0x0052CA20+2, 10); //            MapClass_Repair_Bridge_Hut_52C900            shl     eax, 9
Patch_Byte(0x0052CAC8+2, 10); //            MapClass_Repair_Bridge_Hut_52C900            shl     eax, 9
Patch_Byte(0x0052CCC8+2, 10); //            MapClass_Repair_Bridge_Hut_52C900            shl     eax, 9
Patch_Byte(0x0052CD47+2, 10); //            MapClass_Repair_Bridge_Hut_52C900            shl     eax, 9
Patch_Byte(0x0052CDB9+2, 10); //            MapClass_Repair_Bridge_Hut_52C900            shl     eax, 9
Patch_Byte(0x0052CE2E+2, 10); //            MapClass_Repair_Bridge_Hut_52C900            shl     eax, 9
Patch_Byte(0x0052D3A9+2, 10); //            MapClass_52D2E0            shl     eax, 9
Patch_Byte(0x0052D496+2, 10); //            MapClass_52D2E0            shl     eax, 9
Patch_Byte(0x00612F85+2, 10); //            Tactical_612F00            shl     eax, 9
Patch_Byte(0x005484B7+2, 10); //            sub_547F40            shl     ebp, 9
Patch_Byte(0x00548C6C+2, 10); //            Generate_Rough            shl     ebp, 9
Patch_Byte(0x0052B815+2, 10); //            MapClass_52B7E0            shl     ebx, 9
Patch_Byte(0x0041A8A1+2, 10); //            AStarClass_Regular_FindPath_41A880            shl     ecx, 9
Patch_Byte(0x0041ABE9+2, 10); //            AStarClass_Regular_FindPath_41A880            shl     ecx, 9
Patch_Byte(0x00511117+2, 10); //            ?Place_Down@MapClass@@QAEXAAUCellStruct@@PAVObjectClass@@@Z            shl     ecx, 9
Patch_Byte(0x00511257+2, 10); //            ?Pick_Up@MapClass@@QAEXAAUCellStruct@@PAVObjectClass@@@Z            shl     ecx, 9
Patch_Byte(0x005127EC+2, 10); //            MapClass_5127A0            shl     ecx, 9
Patch_Byte(0x00512905+2, 10); //            MapClass_5127A0            shl     ecx, 9
Patch_Byte(0x00512967+2, 10); //            MapClass_5127A0            shl     ecx, 9
Patch_Byte(0x00512E1A+2, 10); //            MapClass_512BE0            shl     ecx, 9
Patch_Byte(0x00512E80+2, 10); //            MapClass_512BE0            shl     ecx, 9
Patch_Byte(0x00512F93+2, 10); //            MapClass_512BE0            shl     ecx, 9
Patch_Byte(0x00512FF4+2, 10); //            MapClass_512BE0            shl     ecx, 9
Patch_Byte(0x005132DA+2, 10); //            MapClass_5130A0            shl     ecx, 9
Patch_Byte(0x00513340+2, 10); //            MapClass_5130A0            shl     ecx, 9
Patch_Byte(0x00513453+2, 10); //            MapClass_5130A0            shl     ecx, 9
Patch_Byte(0x005134B4+2, 10); //            MapClass_5130A0            shl     ecx, 9
Patch_Byte(0x00519C15+2, 10); //            MapClass_5198C0            shl     ecx, 9
Patch_Byte(0x0051BBFA+2, 10); //            MapClass_RepairBridge_DirB            shl     ecx, 9
Patch_Byte(0x0051D42D+2, 10); //            MapClass_51D100            shl     ecx, 9
Patch_Byte(0x0051E164+2, 10); //            MapClass_GetCoordFloorHeight            shl     ecx, 9
Patch_Byte(0x0051F3C5+2, 10); //            MapClass_Ice_Stuff_51F020            shl     ecx, 9
Patch_Byte(0x0052043C+2, 10); //            MapClass_Water_Stuff_51FBC0            shl     ecx, 9
Patch_Byte(0x005252CD+2, 10); //            MapClass%sub_525080            shl     ecx, 9
Patch_Byte(0x005262A1+2, 10); //            MapClass%sub_5261B0            shl     ecx, 9
Patch_Byte(0x00526A57+2, 10); //            MapClass_5266E0            shl     ecx, 9
Patch_Byte(0x00526B6E+2, 10); //            MapClass_5266E0            shl     ecx, 9
Patch_Byte(0x00527048+2, 10); //            MapClass_Collapse_Cliff            shl     ecx, 9
Patch_Byte(0x0052BEA7+2, 10); //            MapClass_52BE40            shl     ecx, 9
Patch_Byte(0x0052C73A+2, 10); //            MapClass_52C690            shl     ecx, 9
Patch_Byte(0x0052C76A+2, 10); //            MapClass_52C690            shl     ecx, 9
Patch_Byte(0x0052D6A7+2, 10); //            MapClass_52D2E0            shl     ecx, 9
Patch_Byte(0x0052D817+2, 10); //            MapClass_BuildingToOverlay            shl     ecx, 9
Patch_Byte(0x0052DA07+2, 10); //            MapClass_BuildingToWall            shl     ecx, 9
Patch_Byte(0x00459919+2, 10); //            ?Load@CellClass@@UAGJPAUIStream@@@Z            shl     edi, 9
Patch_Byte(0x0041A8B2+2, 10); //            AStarClass_Regular_FindPath_41A880            shl     edx, 9
Patch_Byte(0x0049DB36+2, 10); //            W?$ct:FoggedObjectClass$n(uiii)_            shl     edx, 9
Patch_Byte(0x0049DD66+2, 10); //            W?$ct:FoggedObjectClass$n(uiii)__0            shl     edx, 9
Patch_Byte(0x0049DFBC+2, 10); //            W?$ct:FoggedObjectClass$n(ii)_            shl     edx, 9
Patch_Byte(0x0049E48A+2, 10); //            W?$ct:FoggedObjectClass$n(i)_            shl     edx, 9
Patch_Byte(0x0049E62C+2, 10); //            W?$dt:FoggedObjectClass$n()_            shl     edx, 9
Patch_Byte(0x0049F584+2, 10); //            W?Load$:FoggedObjectClass$n(Iii)i            shl     edx, 9
Patch_Byte(0x0050F237+2, 10); //            ??AMapClass@@QBEABVCellClass@@ABUCoordStruct@@@Z            shl     edx, 9
Patch_Byte(0x0050F2D1+2, 10); //            MapClass::Valid_Cell            shl     edx, 9
Patch_Byte(0x00510E70+2, 10); //            ?Sight_From@MapClass@@QAEXAAUCellStruct@@HPAVHouseClass@@_N222@Z            shl     edx, 9
Patch_Byte(0x00511D7C+2, 10); //            MapClass_511570            shl     edx, 9
Patch_Byte(0x00512692+2, 10); //            MapClass_511E80            shl     edx, 9
Patch_Byte(0x0051957F+2, 10); //            MapClass_519470            shl     edx, 9
Patch_Byte(0x00520FEE+2, 10); //            MapClass_Grow_Ice?_520F40            shl     edx, 9
Patch_Byte(0x00510408+2, 10); //            ?Set_Map_Dimensions@MapClass@@UAEXAAVRect@@_NH1@Z            shl     esi, 9
Patch_Byte(0x0051CDDD+2, 10); //            MapClass_51CCD0            shl     esi, 9
Patch_Byte(0x0052252E+2, 10); //            MapClass%sub_522350            shl     esi, 9
Patch_Byte(0x00523A63+2, 10); //            MapClass%sub_523990            shl     esi, 9
Patch_Byte(0x005272C0+2, 10); //            MapClass_Collapse_Cliff            shl     esi, 9
Patch_Byte(0x005279EF+2, 10); //            MapClass_Collapse_Cliff            shl     esi, 9
Patch_Byte(0x0052C122+2, 10); //            MapClass_52C080            shl     esi, 9
Patch_Byte(0x0053C043+2, 10); //            MapSeedClass_53BEC0            shr     eax, 9         // ???????



/**
 *  
 */
Patch_Dword(0x0051078B+2, (NEW_MAP_W-1));    // and     edx, 1FFh
Patch_Dword(0x0050F778+2, (NEW_MAP_W-1));    // and     edx, 1FFh
Patch_Word(0x0050F765+2, (NEW_MAP_W-1));    // and     ecx, 800001FFh
Patch_Word(0x00510778+2, (NEW_MAP_W-1));    // and     esi, 800001FFh
Patch_Dword(0x004767DF+1, (-NEW_MAP_W));    // mov     esi, 0FFFFFE00h
Patch_Dword(0x0050F76E+2, (-NEW_MAP_W));    // or      ecx, 0FFFFFE00h
Patch_Dword(0x00510781+2, (-NEW_MAP_W));    // or      esi, 0FFFFFE00h


/**
 *  "MAP_CELL_TOTAL" to "NEW_MAP_CELL_TOTAL".
 */
Patch_Dword(0x0050F1D1+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0050F294+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0050F308+6, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0050F663+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0050F677+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0050F6E6+2, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0050FEE4+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00510E77+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00510EE8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511132+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511162+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511272+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005112A2+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511458+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005114D2+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005115A0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511655+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051174C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005117A2+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005117F8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051196F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005119C5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511A1B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511BDA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511D3C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511D83+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511EB0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00511F65+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051205C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005120B2+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512108+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051227F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005122D5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051232B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005124EA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512652+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512699+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512827+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512915+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051296E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005129C8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512ABF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512B15+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512C9F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512E2A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512E87+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512EE0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512FA3+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00512FFB+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00513054+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051315F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005132EA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00513347+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005133A0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00513463+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005134BB+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00513514+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00513D19+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00513DAF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00513E2B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00513F02+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00514007+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00514122+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00514242+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005144A2+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005145EB+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00514685+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051498D+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00515839+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005158A3+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005161F7+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00516235+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005162EF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051647F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005164C5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051668E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00516832+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00516C2C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00516DBD+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00516F38+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00516FAD+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005170E9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517195+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005172A5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005173B0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005174B1+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005174FF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051754D+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517598+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051760F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051765B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517750+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517851+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051789F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005178ED+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517938+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005179AF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005179FB+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517AF5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517C05+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517D10+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517E19+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517E66+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517EB4+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517F01+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517F72+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00517FBC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005180E0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005181E9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518236+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518284+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005182D1+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518342+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051838C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518514+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518580+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005185DA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518630+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518700+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051882C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051894A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005189C5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518A1C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518A71+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518B69+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518BF0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518CA2+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518CF8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518D4E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518E46+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518F19+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00518FAF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005190CC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051935A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005193E6+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005194C1+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051958A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005196BC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005198E4+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519A29+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519A68+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519AD8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519B1C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519B6A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519BBA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519C21+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519C62+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519CAD+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519F5C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00519F99+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A005+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A048+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A099+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A0EA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A152+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A195+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A1E1+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A6A5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A7B5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A8C0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051A9C1+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051AA0F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051AA5D+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051AAA8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051AB1F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051AB6B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051AC60+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051AD61+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051ADAF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051ADFD+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051AE48+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051AEBF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051AF0B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B005+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B115+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B220+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B329+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B376+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B3C4+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B411+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B482+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B4CC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B5F0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B6F9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B746+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B794+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B7E1+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B852+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051B89C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051BA21+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051BA8A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051BAE4+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051BB3A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051BC05+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051BD46+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051BE7B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051BF03+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051BF59+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051BFAF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C0A8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C12F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C1E1+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C237+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C28D+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C385+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C4BF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C531+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C596+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C5FA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C665+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C6C6+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C779+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C80F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051C92C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051CBBA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051CC46+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051CD1F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051CDE8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051CF14+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D11B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D239+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D280+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D2E9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D334+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D382+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D3D2+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D43E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D485+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D4D0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D769+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D7AE+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D815+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D85E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D8AF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D900+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D96C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051D9B4+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051DA01+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051DEC3+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051E3A2+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051E586+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051E744+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051E7C2+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051E901+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051E962+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051E9A5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051EB42+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051EC55+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051ED1A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051ED5C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051EF73+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051EFD0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F077+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F113+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F15F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F19E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F1DD+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F372+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F3CC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F40D+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F442+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F47B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F5EF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F661+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F6A4+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F6E7+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F726+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F76B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F7AC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F7ED+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F82F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F905+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F95A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F994+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051F9C9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051FBF9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051FCC8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051FD13+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051FD56+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051FD95+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051FDDA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051FE1C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051FE63+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0051FEAA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00520151+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00520186+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005201D9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00520216+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052024B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00520284+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00520443+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052047A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005204AF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005204E8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00520A87+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00520B33+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00520BFE+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00520FFB+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052105D+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00521098+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052110B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052117D+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005211BC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005217C5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00521823+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00521880+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005218DD+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00521DF9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00521E4B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00521EFD+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00521F51+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00522045+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005220DC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052212F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005221D6+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005223FB+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00522439+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052247B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005224BE+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005224FE+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052253C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052257F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005225BA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00522CB1+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00522D08+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00522D73+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00522DC7+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00522E33+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00522E86+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00522EF2+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00522F45+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00523C22+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00523F27+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00523FA9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00523FE8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005240C8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00524108+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005241C9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052442E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00524469+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005244AA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052494E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00524989+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005249CA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00524E89+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00524F03+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00524FA9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00525023+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005250C5+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052510E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00525153+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005252D4+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005255FA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00525639+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052567A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00525AA9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00525D4D+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00525D8F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00525DCB+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00526233+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00526271+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005262AD+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052669E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00526B3B+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00526B77+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00526E7E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00526FED+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00527056+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005270D8+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005272C7+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005275AE+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00527721+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00527783+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00527805+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005279F6+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00528922+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005289D7+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00528A14+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052908A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x00529175+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x005293B9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052A3A9+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052A709+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052AFAC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052B09A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052B824+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052B8DC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052B968+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052BA41+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052BACB+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052BB3C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052BCEC+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052BEAE+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C129+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C16C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C220+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C266+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C36E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C48A+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C636+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C6C7+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C741+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C771+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C898+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C94C+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052C9AA+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052CA27+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052CACF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052CCCF+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052CD4E+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052CDC0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052CE35+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052D3B0+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052D49D+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052D825+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052D86F+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052DA16+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052DA4D+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0052DA80+1, NEW_MAP_CELL_TOTAL);
Patch_Dword(0x0058E4E5+1, NEW_MAP_CELL_TOTAL);

#endif
}
