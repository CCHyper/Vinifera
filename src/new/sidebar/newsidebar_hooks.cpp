/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          NEWSIDEBAR_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for NewSidebarClass.
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
#include "newsidebar_hooks.h"
#include "newsidebar.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


#include "ini.h"
#include "rawfile.h"
static bool Sidebar_Read_INI()
{
    RawFileClass file("UI.INI");
    INIClass ini(file);

    static const char * const SIDEBAR = "SIDEBAR";

    //MAX_BUILDABLES = 75,             // Maximum number of object types in sidebar.
    int COLUMNS = ini.Get_Int(SIDEBAR, "COLUMNS", 2);                          // Number of side strips on sidebar.

    int SIDE_WIDTH = ini.Get_Int(SIDEBAR, "SIDE_WIDTH", 168);                     // Width of the entire sidebar (in pixels).
    int SIDE_HEIGHT = ini.Get_Int(SIDEBAR, "SIDE_HEIGHT", 200-(7+70));             // Height of the entire sidebar (in pixels).

    int SIDE_X = ini.Get_Int(SIDEBAR, "SIDE_X", 320-80);                      // The X position of sidebar upper left corner.
    int SIDE_Y = ini.Get_Int(SIDEBAR, "SIDE_Y", 7+70);                        // The Y position of sidebar upper left corner.

    int OBJECT_HEIGHT = ini.Get_Int(SIDEBAR, "OBJECT_HEIGHT", 51);                   // Pixel height of each buildable object.
    int OBJECT_WIDTH = ini.Get_Int(SIDEBAR, "OBJECT_WIDTH", 64);                    // Pixel width of each buildable object.
    int STRIP_WIDTH = ini.Get_Int(SIDEBAR, "STRIP_WIDTH", 35);                     // Width of strip (not counting border lines).
    int MAX_VISIBLE = ini.Get_Int(SIDEBAR, "MAX_VISIBLE", 4);                      // Number of object slots visible at any one time.
    int SCROLL_RATE = ini.Get_Int(SIDEBAR, "SCROLL_RATE", 12);                     // The pixel jump while scrolling (larger is faster).
    int UP_X_OFFSET = ini.Get_Int(SIDEBAR, "UP_X_OFFSET", 2);                      // Scroll up arrow coordinates.
    int UP_Y_OFFSET = ini.Get_Int(SIDEBAR, "UP_Y_OFFSET", MAX_VISIBLE * OBJECT_HEIGHT + 1);
    int DOWN_X_OFFSET = ini.Get_Int(SIDEBAR, "DOWN_X_OFFSET", 18);                   // Scroll down arrow coordinates.
    int DOWN_Y_OFFSET = ini.Get_Int(SIDEBAR, "DOWN_Y_OFFSET", UP_Y_OFFSET);          //int(MAX_VISIBLE)*int(OBJECT_HEIGHT)+1,
    int SBUTTON_WIDTH = ini.Get_Int(SIDEBAR, "SBUTTON_WIDTH", 16);                   // Width of the mini-scroll button.
    int SBUTTON_HEIGHT = ini.Get_Int(SIDEBAR, "SBUTTON_HEIGHT",  12);                 // Height of the mini-scroll button.
    int LEFT_EDGE_OFFSET = ini.Get_Int(SIDEBAR, "LEFT_EDGE_OFFSET", 2);                 // Offset from left edge for building shapes.
    int TEXT_X_OFFSET = ini.Get_Int(SIDEBAR, "TEXT_X_OFFSET", 18);                   // X offset to print "ready" text.
    int TEXT_Y_OFFSET = ini.Get_Int(SIDEBAR, "TEXT_Y_OFFSET", 15);                   // Y offset to print "ready" text.

    int TOP_HEIGHT = ini.Get_Int(SIDEBAR, "TOP_HEIGHT", 13);                      // Height of top section (with repair/sell buttons).
    int COLUMN_ONE_X = ini.Get_Int(SIDEBAR, "COLUMN_ONE_X", (320-80)+8);            // Sidestrip upper left coordinates...
    int COLUMN_ONE_Y = ini.Get_Int(SIDEBAR, "COLUMN_ONE_Y", SIDE_Y+TOP_HEIGHT);
    int COLUMN_TWO_X = ini.Get_Int(SIDEBAR, "COLUMN_TWO_X", (320-80)+8+((80-16)/2)+3);
    int COLUMN_TWO_Y = ini.Get_Int(SIDEBAR, "COLUMN_TWO_Y", 7+70+13);

    int BUTTON_ONE_WIDTH = ini.Get_Int(SIDEBAR, "BUTTON_ONE_WIDTH", 32);                // Button width.
    int BUTTON_TWO_WIDTH = ini.Get_Int(SIDEBAR, "BUTTON_TWO_WIDTH", 20);                // Button width.
    int BUTTON_THREE_WIDTH = ini.Get_Int(SIDEBAR, "BUTTON_THREE_WIDTH", 20);              // Button width.
    int BUTTON_FOUR_WIDTH = ini.Get_Int(SIDEBAR, "BUTTON_FOUR_WIDTH", 20);              // Button width.

    int BUTTON_HEIGHT = ini.Get_Int(SIDEBAR, "BUTTON_HEIGHT", 9);                   // Button height.

    int BUTTON_ONE_X = ini.Get_Int(SIDEBAR, "BUTTON_ONE_X", SIDE_X+2);              // Left button X coordinate.
    int BUTTON_ONE_Y = ini.Get_Int(SIDEBAR, "BUTTON_ONE_Y", SIDE_Y+2);              // Left button Y coordinate.
    int BUTTON_TWO_X = ini.Get_Int(SIDEBAR, "BUTTON_TWO_X", SIDE_X+36);             // Right button X coordinate.
    int BUTTON_TWO_Y = ini.Get_Int(SIDEBAR, "BUTTON_TWO_Y", SIDE_Y+2);              // Right button Y coordinate.
    int BUTTON_THREE_X = ini.Get_Int(SIDEBAR, "BUTTON_THREE_X", SIDE_X+58);         // Right button X coordinate.
    int BUTTON_THREE_Y = ini.Get_Int(SIDEBAR, "BUTTON_THREE_Y", SIDE_Y+2);          // Right button Y coordinate.
    int BUTTON_THREE_X = ini.Get_Int(SIDEBAR, "BUTTON_THREE_X", SIDE_X+78);         // Right button X coordinate.
    int BUTTON_THREE_Y = ini.Get_Int(SIDEBAR, "BUTTON_THREE_Y", SIDE_Y+2);          // Right button Y coordinate.

    return true;
}


/**
 *  Main function for patching the hooks.
 */
void NewSidebarClassExtension_Hooks()
{
    /**
     *  Functions.
     */
    //Patch_Jump(0x005F2310, &NewSidebarClass::SBGadgetClass::SBGadgetClass);
    //Patch_Jump(0x005F2330, &NewSidebarClass::SidebarClass);
    //Patch_Jump(0x005F2510, &NewSidebarClass::SidebarClass);
    Patch_Jump(0x005F25E0, &NewSidebarClass::Set_Cameo_Text);
    Patch_Jump(0x005F2610, &NewSidebarClass::One_Time);
    Patch_Jump(0x005F2660, &NewSidebarClass::Init_Clear);
    Patch_Jump(0x005F2720, &NewSidebarClass::Init_IO);
    Patch_Jump(0x005F2900, &NewSidebarClass::Init_For_House);
    //Patch_Jump(0x005F2B00, &NewSidebarClass_reinitpos_5F2B00);
    Patch_Jump(0x005F2C30, &NewSidebarClass::Which_Column);
    Patch_Jump(0x005F2C50, &NewSidebarClass::Factory_Link);
    Patch_Jump(0x005F2CD0, &NewSidebarClass::Activate_Repair);
    Patch_Jump(0x005F2D40, &NewSidebarClass::Activate_Upgrade);
    Patch_Jump(0x005F2DB0, &NewSidebarClass::Activate_Demolish);
    Patch_Jump(0x005F2E20, &NewSidebarClass::Add);
    Patch_Jump(0x005F2E90, &NewSidebarClass::Scroll);
    Patch_Jump(0x005F30F0, &NewSidebarClass::Scroll_Page);
    Patch_Jump(0x005F3560, &NewSidebarClass::Draw_It);
    //Patch_Jump(0x005F38C0, &NewSidebarClass__Blit_Sidebar_5F38C0);
    Patch_Jump(0x005F3C70, &NewSidebarClass::AI);
    Patch_Jump(0x005F3E20, &NewSidebarClass::Recalc);
    Patch_Jump(0x005F3E60, &NewSidebarClass::Activate);
    //Patch_Jump(0x005F4180, &NewSidebarClass::StripClass::StripClass);
    Patch_Jump(0x005F4210, &NewSidebarClass::StripClass::One_Time);
    Patch_Jump(0x005F4230, &NewSidebarClass::StripClass::Get_Special_Cameo);
    Patch_Jump(0x005F4260, &NewSidebarClass::StripClass::Init_Clear);
    Patch_Jump(0x005F42A0, &NewSidebarClass::StripClass::Init_IO);
    Patch_Jump(0x005F4450, &NewSidebarClass::StripClass::Activate);
    Patch_Jump(0x005F4560, &NewSidebarClass::StripClass::Deactivate);
    Patch_Jump(0x005F4630, &NewSidebarClass::StripClass::Add);
    Patch_Jump(0x005F46B0, &NewSidebarClass::StripClass::Scroll);
    Patch_Jump(0x005F4760, &NewSidebarClass::StripClass::Scroll_Page);
    Patch_Jump(0x005F48F0, &NewSidebarClass::StripClass::Flag_To_Redraw);
    Patch_Jump(0x005F4910, &NewSidebarClass::StripClass::AI);
    Patch_Jump(0x005F4E40, &NewSidebarClass::StripClass::Help_Text);
    Patch_Jump(0x005F4F10, &NewSidebarClass::StripClass::Draw_It);
    Patch_Jump(0x005F5610, &NewSidebarClass::StripClass::Recalc);
    //Patch_Jump(0x005F5950, &NewSidebarClass::StripClass::SelectClass::SelectClass);
    Patch_Jump(0x005F5980, &NewSidebarClass::StripClass::SelectClass::Set_Owner);
    Patch_Jump(0x005F59A0, &NewSidebarClass::StripClass::SelectClass::Action);
    Patch_Jump(0x005F5EF0, &NewSidebarClass::SBGadgetClass::Action);
    Patch_Jump(0x005F5F10, &NewSidebarClass::StripClass::Factory_Link);
    Patch_Jump(0x005F5F70, &NewSidebarClass::Abandon_Production);
    Patch_Jump(0x005F5FB0, &NewSidebarClass::StripClass::Abandon_Production);
    Patch_Jump(0x005F6030, &NewSidebarClass::Zoom_Mode_Control);
    Patch_Jump(0x005F6080, &NewSidebarClass::entry_84);
    Patch_Jump(0x005F6620, &NewSidebarClass::Help_Text);
    Patch_Jump(0x005F6670, &NewSidebarClass::StripClass::Max_Visible);
    //Patch_Jump(0x005F66E0, &NewPrint_Cameo_Text);
    Patch_Jump(0x005F68A0, &NewSidebarClass::Load);
    Patch_Jump(0x005F68B0, &NewSidebarClass::Save);
    //Patch_Jump(0x005F68C0, &NewSidebarClass::StripClass::SelectClass::`scalar deleting destructor);
    //Patch_Jump(0x005F68E0, &NewSidebarClass::SBGadgetClass::`scalar deleting destructor);
    //Patch_Jump(0x005F6900, &NewStageClass::StageClass);
    //Patch_Jump(0x005F6920, &NewBuildType__BuildType);

    /**
     *  Globals
     */
}
