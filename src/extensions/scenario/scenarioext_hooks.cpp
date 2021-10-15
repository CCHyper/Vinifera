/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SCENARIOEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended ScenarioClass.
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
#include "scenarioext_hooks.h"
#include "scenarioext_init.h"
#include "scenarioext_functions.h"
#include "scenarioext.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "house.h"
#include "housetype.h"
#include "side.h"
#include "campaign.h"
#include "multiscore.h"
#include "scenario.h"
#include "session.h"
#include "rules.h"
#include "ccfile.h"
#include "ccini.h"
#include "addon.h"
#include "progressscreen.h"
#include "language.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Reimplements the loading screen setup routine.
 * 
 *  @author: CCHyper
 */
void Draw_Loading_Screen()
{
    int image_width = 640;
    int image_height = 400;

    int load_filename_height = 400;
    char prefix = 'A';

    Rect drawrect;
    Point2D textpos;

    bool solo = Session.Singleplayer_Game();

    int player_count = 1;
    if (!solo) {
        player_count = Session.Players.Count();
    }

    /**
     *  For the campaign, we abuse the required CD to get the desired Side.
     */
    SideType side = SIDE_GDI;
    if (Session.Type == GAME_NORMAL) {
        side = SideType(Campaigns[Scen->CampaignID]->WhichCD);

    /**
     *  The first player in the player array is always the local player, so
     *  fetch our player info and the house we are assigned as.
     */
    } else {
        HousesType house = Session.Players.Fetch_Head()->Player.House;

        HouseTypeClass *housetype = HouseTypes[house];

        side = housetype->Side;

        /**
         *  Workaround because NOD has Side=GDI in unmodded Tiberian Sun.
         * 
         *  Match criteria;
         *   - HouseTypes name is "Nod"
         *   - HouseType "Nod" is index 1
         *   - Side is GDI (index 0)
         *   - Side GDI (index 0) name is "GDI"
         *   - Side 1 name is "Nod"
         */
        if (strcmpi("Nod", housetype->Name()) == 0
         && housetype->Get_Heap_ID() == HOUSE_NOD
         && housetype->Side == SIDE_GDI
         && strcmpi(Sides[housetype->Side]->Name(), "GDI") == 0
         && strcmpi(Sides[SIDE_NOD]->Name(), "Nod") == 0) {

            DEBUG_WARNING("Loading Screen: House \"%s\" (%d) has \"Side=GDI\", forcing Side to \"Nod\"!\n",
                housetype->Name(), housetype->Get_Heap_ID());

            /**
             *  Force the Side to SIDE_NOD, fixing the issue so vanilla
             *  Tiberian Sun works with our new loading screen system.
             */
            side = SIDE_NOD;
        }
    }

    /**
     *  Sanity check the side type.
     */
    if (side == SIDE_NONE || side >= Sides.Count()) {
        side = SIDE_GDI;
    }

    /**
     *  To retain compatibility with the original game, the first two must
     *  remain hardcoded as their order was reverse in the original code.
     */
    if (side == SIDE_GDI) {
        prefix = Percent_Chance(50) ? 'C' : 'D';

    } else if (side == SIDE_NOD) {
        prefix = Percent_Chance(50) ? 'A' : 'B';
    }

    /**
     *  Set the progress text draw positions (resolves #issue-294).
     */
    if (ScreenRect.Width >= 640 && ScreenRect.Height == 400) {

        if (side == SIDE_GDI) {
            textpos.X = solo ? 435 : 435;
            textpos.Y = solo ? 157 : 157;

        } else if (side == SIDE_NOD) {
            textpos.X = solo ? 436 : 436;
            textpos.Y = solo ? 161 : 161;

        /**
         *  All other sides (uses the GDI offsets).
         */
        } else {
            textpos.X = solo ? 435 : 435;
            textpos.Y = solo ? 157 : 157;
        }

        image_width = 640;
        image_height = 400;

        load_filename_height = 400;

    } else if (ScreenRect.Width >= 640 && ScreenRect.Height == 480) {

        if (side == SIDE_GDI) {
            textpos.X = solo ? 435 : 435;
            textpos.Y = solo ? 195 : 195;

        } else if (side == SIDE_NOD) {
            textpos.X = solo ? 436 : 436;
            textpos.Y = solo ? 200 : 200;
            
        /**
         *  All other sides (uses the GDI offsets).
         */
        } else {
            textpos.X = solo ? 435 : 435;
            textpos.Y = solo ? 195 : 195;
        }

        image_width = 640;
        image_height = 480;

        load_filename_height = 480;

    } else if (ScreenRect.Width >= 800 && ScreenRect.Height >= 600) {

        if (side == SIDE_GDI) {
            textpos.X = solo ? 563 : 563;
            textpos.Y = solo ? 252 : 252;

        } else if (side == SIDE_NOD) {
            textpos.X = solo ? 565 : 565;
            textpos.Y = solo ? 258 : 258;
            
        /**
         *  All other sides (uses the GDI offsets).
         */
        } else {
            textpos.X = solo ? 563 : 563;
            textpos.Y = solo ? 252 : 252;
        }

        image_width = 800;
        image_height = 600;

        load_filename_height = 600;
    }

    /**
     *  Adjust the position of the text so it is correct for widescreen resolutions.
     */
    textpos.X += (ScreenRect.Width - image_width) / 2;
    textpos.Y += (ScreenRect.Height - image_height) / 2;

    /**
     *  Build the loading screen filename. (Format: LOAD[screen width][side char].PCX)
     */
    char loadname[16];
    std::snprintf(loadname, sizeof(loadname), "LOAD%d%c.PCX", load_filename_height, prefix);

    DEV_DEBUG_INFO("Loading Screen: \"%s\"\n", loadname);

    /**
     *  If this is a tournament game, format the game id.
     */
    char gamenamebuffer[128];
    const char *gamename = nullptr;
#if 0
    if (Session.Type == GAME_INTERNET && Wolapi_Tournament) {
        std::snprintf(gamenamebuffer, sizeof(gamenamebuffer), Text_String(TXT_GAME_ID), Wolapi_GamesPlayed);
        gamename = gamenamebuffer;
    }
#endif

    const char *progress_name = player_count <= 1 ? "PROGBAR.SHP" : "PROGBARM.SHP";

    /**
     *  Initialise and draw the loading screen.
     */
    ProgressScreen.Init(100.0, player_count);

    ProgressScreen.Draw_Graphics(progress_name, loadname, gamename, textpos.X, textpos.Y);
    ProgressScreen.Draw_Bars_And_Text();
}


/**
 *  Patch to intercept and replace the loading screen setup.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Read_Scenario_Loading_Screen_Patch)
{
    Draw_Loading_Screen();

    JMP(0x005DBD4A);
}


/**
 *  Process additions to the Rules data from the input file.
 * 
 *  @author: CCHyper
 */
static bool Rule_Addition(const char *fname, bool with_digest = false)
{
    CCFileClass file(fname);
    if (!file.Is_Available()) {
        return false;
    }

    CCINIClass ini;
    if (!ini.Load(file, with_digest)) {
        return false;
    }

    DEBUG_INFO("Calling Rule->Addition() with \"%s\" overrides.\n", fname);

    Rule->Addition(ini);

    return true;
}


/**
 *  #issue-#671
 * 
 *  Add loading of MPLAYER.INI to override Rules data for multiplayer games.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Read_Scenario_INI_MPlayer_INI_Patch)
{
    if (Session.Type != GAME_NORMAL && Session.Type != GAME_WDT) {

        /**
         *  Process the multiplayer ini overrides.
         */
        Rule_Addition("MPLAYER.INI");
        if (Addon_Enabled(ADDON_FIRESTORM)) { 
            Rule_Addition("MPLAYERFS.INI");
        }

    }

    /**
     *  Update the progress screen bars.
     */
    Session.Loading_Callback(42);

    /**
     *  Stolen bytes/code.
     */
    Call_Back();

    JMP(0x005DD8DA);
}


/**
 *  #issue-522
 * 
 *  These patches make the multiplayer score screen to honour the value of
 *  "IsSkipScore" from ScenarioClass.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Do_Win_Skip_MPlayer_Score_Screen_Patch)
{
    /**
     *  Stolen bytes/code.
     */
    ++Session.GamesPlayed;

    if (!Scen->IsSkipScore) {
        MultiScore::Presentation();
    }

    JMP(0x005DC9DF);
}

DECLARE_PATCH(_Do_Lose_Skip_MPlayer_Score_Screen_Patch)
{
    /**
     *  Stolen bytes/code.
     */
    ++Session.GamesPlayed;

    if (!Scen->IsSkipScore) {
        MultiScore::Presentation();
    }

    JMP(0x005DCD9D);
}


/**
 *  Main function for patching the hooks.
 */
void ScenarioClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    ScenarioClassExtension_Init();

    /**
     *  For compatibility with the TS Client we need to remove
     *  these two reimplementations as they conflict with the spawner.
     */
#if !defined(TS_CLIENT)
    /**
     *  Hooks in the new Assign_Houses() function.
     * 
     *  @author: CCHyper
     */
    Patch_Call(0x005E08E3, &Vinifera_Assign_Houses);

    /**
     *  #issue-338
     * 
     *  Hooks in the new Create_Units() function.
     * 
     *  @author: CCHyper
     */
    Patch_Call(0x005DD320, &Vinifera_Create_Units);
#endif

    Patch_Jump(0x005DC9D4, &_Do_Win_Skip_MPlayer_Score_Screen_Patch);
    Patch_Jump(0x005DCD92, &_Do_Lose_Skip_MPlayer_Score_Screen_Patch);
    Patch_Jump(0x005DD8D5, &_Read_Scenario_INI_MPlayer_INI_Patch);
    Patch_Jump(0x005DBA8B, &_Read_Scenario_Loading_Screen_Patch);
}
