#include "mapedit_functions.h"
#include "mapedit.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "iomap.h"
#include "tactical.h"
#include "options.h"
#include "dsurface.h"
#include "wwmouse.h"
#include "wwkeyboard.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  A mini-main loop for map edit mode only.
 * 
 *  @author: CCHyper
 */
bool Map_Edit_Loop()
{
    //DEV_DEBUG_INFO("Map_Edit_Loop(enter)\n");

    if (!Debug_Map) {
        return false;
    }

    if (GameActive) {

        /**
         *  Update the display, unless we're inside a dialog.
         */
        if (SpecialDialog == SDLG_NONE && GameInFocus) {

            Map.Flag_To_Redraw(2);

            /**
             *  Get user input (keys, mouse clicks).
             */
            KeyNumType input;
            int x;
            int y;

            Map.Input(input, x, y);

            /**
             *  Process keypress.
             */
            if (input != KN_NONE) {

                /**
                 *  
                 */
                int scroll_distance = 21;
                bool scroll = false;

                if (WWKeyboard->Down(Options.KeySelect1) || WWKeyboard->Down(Options.KeySelect2)) {
                    scroll_distance = scroll_distance * 2.5f;
                    scroll = true;
                }

                if (WWKeyboard->Down(Options.KeyForceAttack1) || WWKeyboard->Down(Options.KeyForceAttack2)) {
                    if (Map.MapSize.Width <= Map.MapSize.Height) {
                        scroll_distance = Map.MapSize.Height * 256;
                    } else {
                        scroll_distance = Map.MapSize.Width * 256;
                    }
                    scroll = true;
                }

                if (scroll) {
                    if (input == KN_E_LEFT) {
                        Map.Scroll_Map(FACING_W, scroll_distance);
                    }
                    if (input == KN_E_RIGHT) {
                        Map.Scroll_Map(FACING_E, scroll_distance);
                    }
                    if (input == KN_E_UP) {
                        Map.Scroll_Map(FACING_N, scroll_distance);
                    }
                    if (input == KN_E_DOWN) {
                        Map.Scroll_Map(FACING_S, scroll_distance);
                    }
                    input = KN_NONE;
                }

                switch (input) {
                    case KN_ESC:
                    case KN_SPACE:
                        SpecialDialog = SDLG_OPTIONS;
                        break;

                    /**
                     *  Block use of the arrow keys, the interfere with the gadgets.
                     */
                    case KN_LEFT:
                    case KN_RIGHT:
                    case KN_UP:
                    case KN_DOWN:
                        break;

                    default:
                        Keyboard_Process(input);
                        break;
                };
            }

            Map.Render();
            TacticalMap->AI();
        }

    }


    /**
     *  Maintains sound effects and Theme.AI() for music.
     */
    Call_Back();

    Sleep(1);

    //DEV_DEBUG_INFO("Map_Edit_Loop(exit)\n");

    return !GameActive;
}


/**
 *  Toggle between the editor and game view (reallocating surfaces if required).
 * 
 *  @author: CCHyper
 */
static void Toggle_Tactical_View(bool flag)
{
    static int _tab_height = 16;
    static int _sidebar_width = 168;

    if (flag) {

        Rect hidden(0, 0, Options.ScreenWidth-_sidebar_width, Options.ScreenHeight);
        Rect comp(0, 0, Options.ScreenWidth, Options.ScreenHeight);
        Rect tile(0, 0, Options.ScreenWidth, Options.ScreenHeight);
        Rect sidebar(0, 0, _sidebar_width, Options.ScreenHeight);
        Allocate_Surfaces(&hidden, &comp, &tile, &sidebar);

        Rect view(0, 0, Options.ScreenWidth, Options.ScreenHeight);
        Map.Set_View_Dimensions(view);

        Sleep(2);

    } else {

        Rect hidden(0, 0, Options.ScreenWidth-_sidebar_width, Options.ScreenHeight);
        Rect comp(0, 0, Options.ScreenWidth-_sidebar_width, Options.ScreenHeight);
        Rect tile(0, 0, Options.ScreenWidth-_sidebar_width, Options.ScreenHeight);
        Rect sidebar(0, 0, _sidebar_width, Options.ScreenHeight);
        Allocate_Surfaces(&hidden, &comp, &tile, &sidebar);

        Rect view(0, _tab_height, Options.ScreenWidth-_sidebar_width, Options.ScreenHeight-_tab_height);
        Map.Set_View_Dimensions(view);

        Sleep(2);
    }
}


/**
 *  Enables/disables the scenario editor mode.
 * 
 *  @author: CCHyper
 */
bool Go_Editor(bool flag)
{
    static bool _input_locked = false;

    WWKeyboard->Clear();

    /**
     *  Toggle the tactical view area.
     */
    Toggle_Tactical_View(flag);

    /**
     *  Go into Scenario Editor mode.
     */
    if (flag) {

        Debug_Map = true;
        Debug_Unshroud = true;

        _input_locked = UserInputLocked;
        UserInputLocked = false;
        Scen->UserInputLocked = false;

        /**
         *  Un-select any selected objects.
         */
        Unselect_All();
        Map.Abort_Drag_Select();

        /**
         *  Turn off the sidebar if it's on.
         */
        Map.Activate(0);

        /**
         *  Reset the map's Button list for the new mode.
         */
        Map.Init_IO();

        /**
         *  Force a complete redraw of the screen.
         */
        HiddenSurface->Clear();

        Map.Flag_To_Redraw(2);
        Map.Render();

    /**
     *  Go into normal game mode.
     */
    } else {

        Debug_Map = false;
        Debug_Unshroud = false;

        UserInputLocked = _input_locked;
        Scen->UserInputLocked = _input_locked;

        /**
         *  Un-select any selected objects.
         */
        Unselect_All();
        Map.Abort_Drag_Select();

        /**
         *  Turn on the sidebar if it's off.
         */
        Map.Activate(1);

        /**
         *  Reset the map's Button list for the new mode.
         */
        Map.Init_IO();

        /**
         *  Force a complete redraw of the screen.
         */
        HiddenSurface->Clear();

        Map.Flag_To_Redraw(2);
        Map.Render();
    }

    GScreenClass::Blit(true, HiddenSurface);

    return true;
}
