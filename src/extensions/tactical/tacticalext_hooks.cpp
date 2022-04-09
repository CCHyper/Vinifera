/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TACTICALEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended Tactical class.
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
#include "tacticalext_hooks.h"
#include "tacticalext_init.h"
#include "tacticalext.h"
#include "tactical.h"
#include "voc.h"
#include "laserdraw.h"
#include "ebolt.h"
#include "vinifera_globals.h"
#include "vinifera_util.h"
#include "extension_globals.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <timeapi.h>

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-315
 * 
 *  Set the waypoint number text for all theaters to be "White" (14).
 *  TEMPERATE was "White" (14) and SNOW was "Black" (12).
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Tactical_Draw_Waypoint_Paths_Text_Color_Patch)
{
    _asm { mov eax, 14 }

    JMP_REG(ecx, 0x00616FEB);
}


/**
 *  #issue-348
 * 
 *  The animation speed of Waypoint lines is not normalised and subjective to
 *  the game speed setting. This patch adjusts the animation using the system
 *  timer and makes the animation speed consistent across all game speeds.
 * 
 *  @authors: CCHyper
 */
DECLARE_PATCH(_Tactical_Draw_Rally_Points_NormaliseLineAnimation_Patch)
{
    GET_STACK_STATIC8(bool, blit, esp, 0x70);
    LEA_STACK_STATIC(Point2D *, start_pos, esp, 0x1C);
    LEA_STACK_STATIC(Point2D *, end_pos, esp, 0x14);

    /**
     *  5 pixels on, 3 off, 5 pixels on, 3 off.
     */
    static bool _pattern[16] = { true, true, true, true, true, false, false, false, true, true, true, true, true, false, false, false };
    
    static int time;
    static int offset;
    static unsigned color;
    static unsigned color_black;

    /**
     *  Adjust the offset of the line pattern.
     */
    time = timeGetTime();
    offset = (-time / 32) & (ARRAYSIZE(_pattern)-1);

    color = DSurface::RGB_To_Pixel(0,255,0);
    color_black = DSurface::RGB_To_Pixel(0,0,0);

#if 0
    /**
     *  Draw the line line with the desired pattern.
     */
    TempSurface->entry_48(*start_pos, *end_pos, color, _pattern, offset, blit);
#endif

    /**
     *  #issue-351
     * 
     *  Thicken the rally point lines so they are easier to see in contrast to the terrain.
     * 
     *  @authors: CCHyper
     */

    /**
     *  Draw the drop shadow line.
     */
    start_pos->Y += 2;
    end_pos->Y += 2;
    TempSurface->entry_48(*start_pos, *end_pos, color_black, _pattern, offset, blit);

    /**
     *  Draw two lines, offset by one pixel from each other, giving the
     *  impression that it is double the thickness.
     */
    --start_pos->Y;
    --end_pos->Y;
    TempSurface->entry_48(*start_pos, *end_pos, color, _pattern, offset, blit);

    --start_pos->Y;
    --end_pos->Y;
    TempSurface->entry_48(*start_pos, *end_pos, color, _pattern, offset, blit);

    JMP(0x00616EFD);
}


/**
 *  #issue-348
 * 
 *  The animation speed of Rally Point lines is not normalised and subjective to
 *  the game speed setting. This patch adjusts the animation using the system
 *  timer and makes the animation speed consistent across all game speeds.
 * 
 *  @authors: CCHyper
 */
DECLARE_PATCH(_Tactical_Draw_Waypoint_Paths_NormaliseLineAnimation_Patch)
{
    GET_REGISTER_STATIC(unsigned, color, eax);
    GET_STACK_STATIC8(bool, blit, esp, 0x90);
    LEA_STACK_STATIC(Point2D *, start_pos, esp, 0x34);
    LEA_STACK_STATIC(Point2D *, end_pos, esp, 0x3C);

    /**
     *  5 pixels on, 3 off, 5 pixels on, 3 off.
     */
    static bool _pattern[16] = { true, true, true, true, true, false, false, false, true, true, true, true, true, false, false, false };

    static int time;
    static int offset;
    static unsigned color_black;

    /**
     *  Adjust the offset of the line pattern (this animates a little slower than rally points).
     */
    time = timeGetTime();
    offset = (-time / 64) & (ARRAYSIZE(_pattern)-1);

    color_black = DSurface::RGB_To_Pixel(0,0,0);

#if 0
    /**
     *  Draw the line line with the desired pattern.
     */
    TempSurface->entry_48(*start_pos, *end_pos, color, _pattern, offset, blit);
#endif

    /**
     *  #issue-351
     * 
     *  Thicken the waypoint path lines so they are easier to see in contrast to the terrain.
     * 
     *  @authors: CCHyper
     */

    /**
     *  Draw the drop shadow line.
     */
    start_pos->Y += 2;
    end_pos->Y += 2;
    TempSurface->entry_48(*start_pos, *end_pos, color_black, _pattern, offset, blit);

    /**
     *  Draw two lines, offset by one pixel from each other, giving the
     *  impression that it is double the thickness.
     */
    --start_pos->Y;
    --end_pos->Y;
    TempSurface->entry_48(*start_pos, *end_pos, color, _pattern, offset, blit);

    --start_pos->Y;
    --end_pos->Y;
    TempSurface->entry_48(*start_pos, *end_pos, color, _pattern, offset, blit);

    JMP(0x00617307);
}


/**
 *  #issue-351
 * 
 *  Thicken the waypoint path lines so they are easier to see in contrast to the terrain.
 * 
 *  @authors: CCHyper
 */
DECLARE_PATCH(_Tactical_Draw_Waypoint_Paths_DrawNormalLine_Patch)
{
    GET_REGISTER_STATIC(unsigned, color, eax);
    GET_STACK_STATIC8(bool, blit, esp, 0x90);
    LEA_STACK_STATIC(Point2D *, start_pos, esp, 0x34);
    LEA_STACK_STATIC(Point2D *, end_pos, esp, 0x3C);

    static unsigned color_black;

    color_black = DSurface::RGB_To_Pixel(0,0,0);

    /**
     *  Draw the drop shadow line.
     */
    start_pos->Y += 2;
    end_pos->Y += 2;
    TempSurface->entry_4C(*start_pos, *end_pos, color_black);

    /**
     *  Draw two lines, offset by one pixel from each other, giving the
     *  impression that it is double the thickness.
     */
    --start_pos->Y;
    --end_pos->Y;
    TempSurface->entry_4C(*start_pos, *end_pos, color);

    --start_pos->Y;
    --end_pos->Y;
    TempSurface->entry_4C(*start_pos, *end_pos, color);

    JMP(0x00617307);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
static Cell cellpos;
DECLARE_PATCH(_Tactical_Draw_Placement_Object_Preview_Patch)
{
    GET_STACK_STATIC(int, pos_x, esp, 0x14);
    GET_REGISTER_STATIC(int, pos_y, edi);

    cellpos.X = pos_x;
    cellpos.Y = pos_y;

    if (!Debug_Map) {
        TacticalMapExtension->Draw_Placement_Preview(cellpos);
    }

    /**
     *  Stolen bytes/code.
     */
    _asm { mov ecx, dword ptr ds:0x007494FC } // restore register ECX with "Display.PendingObject".

    JMP(0x0061203A);
}


/**
 *  This patch intercepts the post effects rendering process for Tactical
 *  allowing us to draw any new effects/systems.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Tactical_Render_Post_Effects_Patch)
{
    GET_REGISTER_STATIC(Tactical *, this_ptr, ebp);

    /**
     *  Stolen bytes/code.
     */
    LaserDrawClass::Draw_All();

    /**
     *  Draw any new post effects here.
     */
    TacticalMapExtension->Render_Post();

    JMP(0x00611AFE);
}


#include "colorscheme.h"
static void Print_Tactical_Info()
{
    static ColorScheme * TextColor = nullptr;
    if (!TextColor) {
        TextColor = ColorScheme::As_Pointer("White");
        ASSERT(TextColor != nullptr);
    }

    // Format print string.
    char buffer[4096];

    // Get primary surface rect.
    Rect surfrect = PrimarySurface->Get_Rect();
    Rect comprect = CompositeSurface->Get_Rect();

    // Draw background rect.
    int rect_width = 400; // Needed, otherwise the box jitters about due to Coord value change.
    int rect_height = 100;
    Rect boxrect(comprect.X, comprect.Y + 16, rect_width, rect_height);
    //CompositeSurface->Fill_Rect(boxrect, MAKE_RGB565(0, 0, 0));
    RGBClass rgb{0, 0, 0};

    int lines = 20; // Update this when you add a line!
    boxrect.Height = lines * 24 + 4; // 4 == header spacing.

    CompositeSurface->Fill_Rect_Trans(boxrect, rgb, 50);

    // Draw position.
    Point2D drawpos;
    drawpos.X = 1;
    drawpos.Y = 17;

    Fancy_Text_Print("Tactical",
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y /*= 10*/),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    // Make a space under the title.
    drawpos.Y += 4;

    //
    // Tactical
    //
    std::snprintf(buffer, sizeof(buffer), "LastAIFrame: %d", TacticalMap->LastAIFrame);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_5C: %d,%d", TacticalMap->field_5C.X, TacticalMap->field_5C.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_64: %d,%d", TacticalMap->field_64.X, TacticalMap->field_64.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "MoveFrom: %d,%d", TacticalMap->MoveFrom.X, TacticalMap->MoveFrom.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_84: %d,%d", TacticalMap->MoveTo.X, TacticalMap->MoveTo.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_D18: %d,%d", TacticalMap->field_D18.X, TacticalMap->field_D18.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_D20: %d,%d", TacticalMap->field_D20.X, TacticalMap->field_D20.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_D28: %d,%d", TacticalMap->field_D28.X, TacticalMap->field_D28.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "TacticalRect: %d,%d,%d,%d",
        TacticalMap->field_D34.X, TacticalMap->field_D34.Y, TacticalMap->field_D34.Width, TacticalMap->field_D34.Height);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "Band: %d,%d,%d,%d",
        TacticalMap->Band.X, TacticalMap->Band.Y, TacticalMap->Band.Width, TacticalMap->Band.Height);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_D54: %d", TacticalMap->field_D54);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    //std::snprintf(buffer, sizeof(buffer), "field_98: %d", TacticalMap->field_98);
    //Fancy_Text_Print(buffer,
    //    CompositeSurface,
    //    &surfrect,
    //    &Point2D(drawpos.X, drawpos.Y += 12),
    //    TextColor,
    //    COLOR_TBLACK,
    //    TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_78: %d", TacticalMap->field_78);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_6C: %d", TacticalMap->field_6C);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "ZoomFactor: %lf", TacticalMap->ZoomFactor);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "MoveRate: %lf", TacticalMap->MoveRate);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "MoveFactor: %lf", TacticalMap->MoveFactor);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    drawpos.Y += 4;

    std::snprintf(buffer, sizeof(buffer),
        "field_D64: [0] %lf, %lf, %lf, %lf\n"
        "           [1] %lf, %lf, %lf, %lf\n"
        "           [2] %lf, %lf, %lf, %lf\n"
        "           [3] %lf, %lf, %lf, %lf\n",
        TacticalMap->field_D64[0].X, TacticalMap->field_D64[0].Y, TacticalMap->field_D64[0].Z, TacticalMap->field_D64[0].W,
        TacticalMap->field_D64[1].X, TacticalMap->field_D64[1].Y, TacticalMap->field_D64[1].Z, TacticalMap->field_D64[1].W,
        TacticalMap->field_D64[2].X, TacticalMap->field_D64[2].Y, TacticalMap->field_D64[2].Z, TacticalMap->field_D64[2].W,
        TacticalMap->field_D64[3].X, TacticalMap->field_D64[3].Y, TacticalMap->field_D64[3].Z, TacticalMap->field_D64[3].W
    );
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    drawpos.Y += 12 * 3 - 6;

    std::snprintf(buffer, sizeof(buffer),
        "field_D94: [0] %lf, %lf, %lf, %lf\n"
        "           [1] %lf, %lf, %lf, %lf\n"
        "           [2] %lf, %lf, %lf, %lf\n"
        "           [3] %lf, %lf, %lf, %lf\n",
        TacticalMap->field_D94[0].X, TacticalMap->field_D94[0].Y, TacticalMap->field_D94[0].Z, TacticalMap->field_D94[0].W,
        TacticalMap->field_D94[1].X, TacticalMap->field_D94[1].Y, TacticalMap->field_D94[1].Z, TacticalMap->field_D94[1].W,
        TacticalMap->field_D94[2].X, TacticalMap->field_D94[2].Y, TacticalMap->field_D94[2].Z, TacticalMap->field_D94[2].W,
        TacticalMap->field_D94[3].X, TacticalMap->field_D94[3].Y, TacticalMap->field_D94[3].Z, TacticalMap->field_D94[3].W
    );
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    drawpos.Y += 12 * 3;
}


/**
 *  This patch intercepts the end of the rendering process for Tactical
 *  for drawing any overlay or graphics.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Tactical_Render_Overlay_Patch)
{
    GET_REGISTER_STATIC(Tactical *, this_ptr, ebp);

    /**
     *  If the developer mode is active, draw the developer overlay.
     */
    if (Vinifera_DeveloperMode) {

        TacticalMapExtension->Draw_Debug_Overlay();

        if (Vinifera_Developer_FrameStep) {
            TacticalMapExtension->Draw_FrameStep_Overlay();
        }
    }

#ifndef NDEBUG
    /**
     *  Various developer only debugging.
     */
    //Tactical_Debug_Draw_Facings();
#endif

#ifndef RELEASE
    /**
     *  Draw the version number on screen for non-release builds.
     * 
     *  @note: This must be last in the draw order!
     */
    Vinifera_Draw_Version_Text(CompositeSurface);
#endif

    /**
     *  Has custom screen text been set?
     */
    if (TacticalMapExtension->IsInfoTextSet) {

        /**
         *  Draw it to the screen.
         */
        TacticalMapExtension->Draw_Information_Text();
        
        /**
         *  Play the one time notification sound if defined.
         */
        if (TacticalMapExtension->InfoTextNotifySound != VOC_NONE) {
            Sound_Effect(TacticalMapExtension->InfoTextNotifySound, TacticalMapExtension->InfoTextNotifySoundVolume);
            TacticalMapExtension->InfoTextNotifySound = VOC_NONE;
        }
        
        /**
         *  If the screen timer has expired, disable drawing.
         */
        if (TacticalMapExtension->InfoTextTimer.Expired()) {
            TacticalMapExtension->InfoTextTimer.Stop();
            TacticalMapExtension->IsInfoTextSet = false;
            std::memset(TacticalMapExtension->InfoTextBuffer, 0, sizeof(TacticalMapExtension->InfoTextBuffer));
            TacticalMapExtension->InfoTextNotifySound = VOC_NONE;
            TacticalMapExtension->InfoTextPosition = TOP_LEFT;
        }       
    }

    /**
     *  Stolen bytes/code.
     */
original_code:
    this_ptr->Draw_Screen_Text(this_ptr->ScreenText);

    this_ptr->field_D30 = false;
    this_ptr->IsToRedraw = false;

    Print_Tactical_Info();

    JMP(0x00611BE4);
}


/**
 *  Main function for patching the hooks.
 */
void TacticalExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    TacticalExtension_Init();

    Patch_Jump(0x00611AF9, &_Tactical_Render_Post_Effects_Patch);
    Patch_Jump(0x00611BCB, &_Tactical_Render_Overlay_Patch);

    Patch_Jump(0x00616E9A, &_Tactical_Draw_Rally_Points_NormaliseLineAnimation_Patch);
    Patch_Jump(0x006172DB, &_Tactical_Draw_Waypoint_Paths_NormaliseLineAnimation_Patch);
    Patch_Jump(0x00617327, &_Tactical_Draw_Waypoint_Paths_DrawNormalLine_Patch);

    Patch_Jump(0x00612034, &_Tactical_Draw_Placement_Object_Preview_Patch);

    /**
     *  #issue-351
     * 
     *  Changes the waypoint number text to have a stroke/outline.
     * 
     *  @authors: CCHyper
     */
    Patch_Dword(0x006171C8+1, (TPF_CENTER|TPF_EFNT|TPF_FULLSHADOW));
    Patch_Jump(0x00616FDA, &_Tactical_Draw_Waypoint_Paths_Text_Color_Patch);
}
