/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          UICONTROL.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         UI controls and overrides.
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
#include "uicontrol.h"
#include "ccini.h"
#include "asserthandler.h"
#include "debughandler.h"


UIControlsClass *UIControls = nullptr;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
UIControlsClass::UIControlsClass() :
    /**
     *  #issue-541
     * 
     *  The health bar graphics "Y" position on selection boxes is off by 1 pixel.
     * 
     *  @author: CCHyper
     */
    UnitHealthBarDrawPos(-25, -16), // Y was -15
    InfantryHealthBarDrawPos(-24, -5),
    IsTextLabelOutline(true),
    TextLabelBackgroundTransparency(50)
{
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
UIControlsClass::UIControlsClass(const NoInitClass &noinit)
{
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
UIControlsClass::~UIControlsClass()
{
}


/**
 *  Process the UI controls from INI.
 *  
 *  @author: CCHyper
 */
bool UIControlsClass::Read_INI(CCINIClass &ini)
{
    static char const * const INGAME = "Ingame";
    static const char * const SIDEBAR = "Sidebar";

    UnitHealthBarDrawPos = ini.Get_Point(INGAME, "UnitHealthBarPos", UnitHealthBarDrawPos);
    InfantryHealthBarDrawPos = ini.Get_Point(INGAME, "InfantryHealthBarPos", InfantryHealthBarDrawPos);

    IsTextLabelOutline = ini.Get_Bool(INGAME, "TextLabelOutline", IsTextLabelOutline);
    TextLabelBackgroundTransparency = ini.Get_Int_Clamp(INGAME, "TextLabelBackgroundTransparency", 0, 100, TextLabelBackgroundTransparency);

    /**
     *  Sidebar.
     */
    int MAX_BUILDABLES = ini.Get_Int(SIDEBAR, "MAX_BUILDABLES", 75);             // Maximum number of object types in sidebar.
    int MAX_VISIBLE = ini.Get_Int(SIDEBAR, "MAX_VISIBLE", 4);                      // Number of object slots visible at any one time.
    int COLUMNS = ini.Get_Int(SIDEBAR, "COLUMNS", 2);                          // Number of side strips on sidebar.

    int SIDE_WIDTH = ini.Get_Int(SIDEBAR, "SIDE_WIDTH", 168);                     // Width of the entire sidebar (in pixels).
    int SIDE_HEIGHT = ini.Get_Int(SIDEBAR, "SIDE_HEIGHT", 200-(7+70));             // Height of the entire sidebar (in pixels).

    int SIDE_X = ini.Get_Int(SIDEBAR, "SIDE_X", 320-80);                      // The X position of sidebar upper left corner.
    int SIDE_Y = ini.Get_Int(SIDEBAR, "SIDE_Y", 7+70);                        // The Y position of sidebar upper left corner.

    int OBJECT_HEIGHT = ini.Get_Int(SIDEBAR, "OBJECT_HEIGHT", 51);                   // Pixel height of each buildable object.
    int OBJECT_WIDTH = ini.Get_Int(SIDEBAR, "OBJECT_WIDTH", 64);                    // Pixel width of each buildable object.

    int STRIP_WIDTH = ini.Get_Int(SIDEBAR, "STRIP_WIDTH", 35);                     // Width of strip (not counting border lines).

    int SCROLL_RATE = ini.Get_Int(SIDEBAR, "SCROLL_RATE", 12);                     // The pixel jump while scrolling (larger is faster).

    int SBUTTON_WIDTH = ini.Get_Int(SIDEBAR, "SBUTTON_WIDTH", 16);                   // Width of the mini-scroll button.
    int SBUTTON_HEIGHT = ini.Get_Int(SIDEBAR, "SBUTTON_HEIGHT",  12);                 // Height of the mini-scroll button.

    int LEFT_EDGE_OFFSET = ini.Get_Int(SIDEBAR, "LEFT_EDGE_OFFSET", 2);                 // Offset from left edge for building shapes.
    int TEXT_X_OFFSET = ini.Get_Int(SIDEBAR, "TEXT_X_OFFSET", 18);                   // X offset to print "ready" text.
    int TEXT_Y_OFFSET = ini.Get_Int(SIDEBAR, "TEXT_Y_OFFSET", 15);                   // Y offset to print "ready" text.

    int UP_X_OFFSET = ini.Get_Int(SIDEBAR, "UP_X_OFFSET", 2);                      // Scroll up arrow coordinates.
    int UP_Y_OFFSET = ini.Get_Int(SIDEBAR, "UP_Y_OFFSET", MAX_VISIBLE * OBJECT_HEIGHT + 1);
    int DOWN_X_OFFSET = ini.Get_Int(SIDEBAR, "DOWN_X_OFFSET", 18);                   // Scroll down arrow coordinates.
    int DOWN_Y_OFFSET = ini.Get_Int(SIDEBAR, "DOWN_Y_OFFSET", UP_Y_OFFSET);          //int(MAX_VISIBLE)*int(OBJECT_HEIGHT)+1,

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
    int BUTTON_FOUR_X = ini.Get_Int(SIDEBAR, "BUTTON_FOUR_X", SIDE_X+78);         // Right button X coordinate.
    int BUTTON_FOUR_Y = ini.Get_Int(SIDEBAR, "BUTTON_FOUR_Y", SIDE_Y+2);          // Right button Y coordinate.

    return true;
}
