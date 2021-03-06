/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_UTIL.CPP
 *
 *  @authors       CCHyper
 *
 *  @brief         Various utility functions.
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
#include "vinifera_util.h"
#include "vinifera_gitinfo.h"
#include "tspp_gitinfo.h"
#include "vinifera_const.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "colorscheme.h"
#include "textprint.h"
#include "dsurface.h"
#include "wwfont.h"
#include "msgbox.h"
#include "minidump.h"
#include "winutil.h"
#include <cstdio>


/**
 *  Returns the Vinifera version info as string.
 * 
 *  The "~" character is added if there are changes made locally before the build was produced.
 * 
 *  @author: CCHyper
 */
const char *Vinifera_Version_String()
{
    static char _buffer[512] { '\0' };

    if (_buffer[0] == '\0') {
        std::snprintf(_buffer, sizeof(_buffer), "Vinifera%s: %s %s %s%s %s",
            Vinifera_DeveloperMode ? " (Dev)" : "",
            Vinifera_Git_Branch(), Vinifera_Git_Author(),
            Vinifera_Git_Uncommitted_Changes() ? "~" : "", Vinifera_Git_Hash_Short(), Vinifera_Git_DateTime());
    }

    return _buffer;
}


/**
 *  Returns the TS++ version info as string.
 * 
 *  The "~" character is added if there are changes made locally before the build was produced.
 * 
 *  @author: CCHyper
 */
const char *TSpp_Version_String()
{
    static char _buffer[512] { '\0' };

    if (_buffer[0] == '\0') {
        std::snprintf(_buffer, sizeof(_buffer), "TS++: %s %s %s%s %s",
            TSPP_Git_Branch(), TSPP_Git_Author(),
            TSPP_Git_Uncommitted_Changes() ? "~" : "", TSPP_Git_Hash_Short(), TSPP_Git_DateTime());
    }

    return _buffer;
}


/**
 *  Draws the version info to the input surface.
 * 
 *  @note: This function will draw the text relative to the bottom right of the surface. 
 * 
 *  @author: CCHyper
 */
void Vinifera_Draw_Version_Text(XSurface *surface, bool pre_init)
{
    if (!surface) {
        return;
    }
    
#ifndef RELEASE
    static Point2D warning_pos;
#endif
    static Point2D version_pos;

    Rect surfrect = surface->Get_Rect();

    TextPrintType style = (TPF_RIGHT|TPF_NOSHADOW|TPF_8POINT);
    ColorScheme *color_white = ColorScheme::As_Pointer("White");
    ColorScheme *color_yellow = ColorScheme::As_Pointer("Yellow");

#ifndef RELEASE
#if defined(NIGHTLY)
    ColorType nightly_color = COLOR_WHITE;
    ColorType nightly_back_color = COLOR_BLUE;
#elif defined(PREVIEW)
    ColorType preview_color = COLOR_WHITE;
    ColorType preview_back_color = COLOR_PURPLE;
#else
    ColorType warning_color = COLOR_YELLOW;
    ColorType warning_back_color = COLOR_RED;
#endif
#endif
    ColorType version_color = COLOR_WHITE;
    ColorType back_color = COLOR_BLACK;

    /**
     *  This is just to retrieve the font height, we don't need to know
     *  the width as we print with right alignment.
     */
    WWFontClass *font = Font_Ptr(style);
    font->Set_X_Spacing(2);

    Rect print_rect;
    font->String_Pixel_Rect("X", &print_rect);

    int offset = 3; // Pixels from edge
        
#ifndef RELEASE
    int space = 0; // Line space
    warning_pos.X = surfrect.Width-offset;
    warning_pos.Y = surfrect.Height-offset-(print_rect.Height*1);
    version_pos.X = surfrect.Width-offset;
    version_pos.Y = surfrect.Height-offset-(print_rect.Height*2)-space;
#else
    version_pos.X = surfrect.Width-offset;
    version_pos.Y = surfrect.Height-offset-(print_rect.Height*1);
#endif

    /**
     *  So, we need to draw the strings slightly differently if this is being drawn
     *  before the games initialisation process has finished. This is because the
     *  ColorSchemes has not be initialised yet.
     */
    if (pre_init) {

        /**
         *  Draw the version string.
         */
        Simple_Text_Print(Vinifera_Version_String(), surface, &surfrect, &version_pos, NormalDrawer, version_color, back_color, style);

#ifndef RELEASE
        /**
         *  Draw the warning string.
         */
    #if defined(NIGHTLY)
        Simple_Text_Print(TXT_VINIFERA_NIGHTLY_BUILD, surface, &surfrect, &warning_pos, NormalDrawer, nightly_color, nightly_back_color, style);
    #elif defined(PREVIEW)
        Simple_Text_Print(TXT_VINIFERA_PREVIEW_BUILD, surface, &surfrect, &warning_pos, NormalDrawer, preview_color, preview_back_color, style);
    #else
        Simple_Text_Print(Vinifera_Git_Uncommitted_Changes() ? TXT_VINIFERA_LOCAL_BUILD : TXT_VINIFERA_UNOFFICIAL_BUILD,
            surface, &surfrect, &warning_pos, NormalDrawer, warning_color, warning_back_color, style);
    #endif
#endif

    } else {

        /**
         *  Draw the version string.
         */
        Fancy_Text_Print(Vinifera_Version_String(), surface, &surfrect, &version_pos, color_white, back_color, style);

#ifndef RELEASE
        /**
         *  Draw the warning string.
         */
    #if defined(NIGHTLY)
        Fancy_Text_Print(TXT_VINIFERA_NIGHTLY_BUILD, surface, &surfrect, &warning_pos, color_white, nightly_back_color, style);
    #elif defined(PREVIEW)
        Fancy_Text_Print(TXT_VINIFERA_PREVIEW_BUILD, surface, &surfrect, &warning_pos, color_white, preview_back_color, style);
    #else
        Fancy_Text_Print(Vinifera_Git_Uncommitted_Changes() ? TXT_VINIFERA_LOCAL_BUILD : TXT_VINIFERA_UNOFFICIAL_BUILD,
            surface, &surfrect, &warning_pos, color_yellow, warning_back_color, style);
    #endif
#endif

    }
}


/**
 *  Write a mini dump file for analysis.
 */
bool Vinifera_Generate_Mini_Dump()
{
    MessageBox(MainWindow,
        "A crash dump will now be generated that can be sent to the\n"
        "developers for further analysis.\n\n"
        "Please note: This may take some time depending on the options\n"
        "set by the crash dump generator, please be patient and allow\n"
        "this process to finish. You will be notified when it is complete.\n\n",
        "Crash dump", 
        MB_OK|MB_ICONQUESTION);

    GenerateFullCrashDump = false; // We don't need a full memory dump.
    bool res = Create_Mini_Dump(nullptr, Get_Module_File_Name());

    if (res) {
        char buffer[512];
        std::snprintf(buffer, sizeof(buffer),
            "Crash dump file generated successfully.\n\n"
            "Please make sure you package DEBUG_<date-time>.LOG\n"
            "and EXCEPT_<date-time>.LOG along with this crash dump file!\n\n"
            "Filename:\n\"%s\" \n", MinidumpFilename);
        MessageBox(MainWindow, buffer, "Crash dump", MB_OK);
        return true;
    }

    MessageBox(MainWindow, "Failed to create crash dump!\n\n", "Crash dump", MB_OK|MB_ICONASTERISK);
    return false;
}


/**
 *  Shows a in-game message box.
 * 
 *  This has been made its own function because we can not allocate on the stack with
 *  our patches, so this handles all that within this function scope.
 * 
 *  @author: CCHyper
 */
int Vinifera_Do_WWMessageBox(const char *msg, const char *btn1, const char *btn2, const char *btn3)
{
    return WWMessageBox().Process(msg, 0, btn1, btn2, btn3);
}
