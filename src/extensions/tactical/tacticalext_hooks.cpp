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
#include "tibsun_globals.h"
#include "dsurface.h"
#include "textprint.h"
#include "wwfont.h"
#include "scenario.h"
#include "session.h"
#include "colorscheme.h"
#include "voc.h"
#include "iomap.h"
#include "cell.h"
#include "tag.h"
#include "tagtype.h"
#include "object.h"
#include "foot.h"
#include "waypoint.h"
#include "fatal.h"
#include "vinifera_globals.h"
#include "vinifera_util.h"
#include "vinifera_gitinfo.h"
#include "mapedit.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <timeapi.h>


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

    color = DSurface::RGBA_To_Pixel(0,255,0);
    color_black = DSurface::RGBA_To_Pixel(0,0,0);

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

    color_black = DSurface::RGBA_To_Pixel(0,0,0);

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

    color_black = DSurface::RGBA_To_Pixel(0,0,0);

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
 *  Draws the developer mode overlay.
 * 
 *  @authors: CCHyper
 */
static void Tactical_Draw_Debug_Overlay()
{
    RGBClass rgb_black(0,0,0);
    unsigned color_black = DSurface::RGBA_To_Pixel(0, 0, 0);
    ColorScheme *text_color = ColorScheme::As_Pointer("White");

    int padding = 2;

    char buffer[256];
    std::snprintf(buffer, sizeof(buffer),
        "[%s]  %3d  %3d  0x%08X",
        strupr(Scen->ScenarioName),
        Session.DesiredFrameRate,
        FramesPerSecond,
        CurrentObjects.Count() == 1 ? CurrentObjects.Fetch_Head() : 0
    );

    /**
     * Fetch the text occupy area.
     */
    Rect text_rect;
    GradFont6Ptr->String_Pixel_Rect(buffer, &text_rect);

    /**
     *  Fill the background area.
     */
    Rect fill_rect;
    fill_rect.X = 160; // Width of Options tab, so we draw from there.
    fill_rect.Y = 0;
    fill_rect.Width = text_rect.Width+(padding+1);
    fill_rect.Height = 16; // Tab bar height
    CompositeSurface->Fill_Rect(fill_rect, color_black);

    /**
     *  Move rects into position.
     */
    text_rect.X = fill_rect.X+padding;
    text_rect.Y = 0;
    text_rect.Width += padding;
    text_rect.Height += 3;

    /**
     *  Draw the overlay text.
     */
    Fancy_Text_Print(buffer, CompositeSurface, &CompositeSurface->Get_Rect(),
        &Point2D(text_rect.X, text_rect.Y), text_color, COLOR_TBLACK, TextPrintType(TPF_6PT_GRAD|TPF_NOSHADOW));
}


/**
 *  Draws the overlay for frame step mode.
 * 
 *  @authors: CCHyper
 */
static void Tactical_Draw_FrameStep_Overlay()
{
    RGBClass rgb_black(0,0,0);
    unsigned color_black = DSurface::RGBA_To_Pixel(0, 0, 0);
    ColorScheme *text_color = ColorScheme::As_Pointer("White");

    int padding = 2;

    const char *text = "Frame Step Mode Enabled";

    /**
     * Fetch the text occupy area.
     */
    Rect text_rect;
    GradFont6Ptr->String_Pixel_Rect(text, &text_rect);

    /**
     *  Fill the background area.
     */
    Rect fill_rect;
    fill_rect.X = TacticalRect.X+TacticalRect.Width-text_rect.Width-(padding+1);
    fill_rect.Y = 16; // Tab bar height
    fill_rect.Width = text_rect.Width+(padding+1);
    fill_rect.Height = 16;
    CompositeSurface->Fill_Rect(fill_rect, color_black);

    /**
     *  Move rects into position.
     */
    text_rect.X = TacticalRect.X+TacticalRect.Width-1;
    text_rect.Y = fill_rect.Y;
    text_rect.Width += padding;
    text_rect.Height += 3;

    /**
     *  Draw the overlay text.
     */
    Fancy_Text_Print(text, CompositeSurface, &CompositeSurface->Get_Rect(),
        &Point2D(text_rect.X, text_rect.Y), text_color, COLOR_TBLACK, TextPrintType(TPF_RIGHT|TPF_6PT_GRAD|TPF_NOSHADOW));
}


static void Tactical_Editor_Draw_Occupiers()
{
    unsigned color_dark_blue = DSurface::RGBA_To_Pixel(0, 0, 128);
    unsigned color_white = DSurface::RGBA_To_Pixel(255, 255, 255);

    ColorScheme *text_color = ColorScheme::As_Pointer("White");

    /**
     *  Reset the cell iterator.
     */
    Map.Iterator_Reset();

    /**
     *  Iterate over all the map cells.
     */
    for (CellClass *cell = Map.Iterator_Next_Cell(); cell != nullptr; cell = Map.Iterator_Next_Cell()) {

        Rect cellrect = cell->Get_Rect();

        /**
         *  Determine if the cell draw rect is within the viewport.
         */
        Rect intersect = Intersect(cellrect, TacticalRect);
        if (intersect.Is_Valid()) {

            /**
             *  Get the center point of the cell.
             */
            Point2D cell_center;
            //cell_center.X = cellrect.X + CELL_PIXEL_W/2;
            //cell_center.Y = cellrect.Y + CELL_PIXEL_H/2;
            cell_center.X = cellrect.X + cellrect.Width/2;
            cell_center.Y = cellrect.Y + cellrect.Height/2;

            /**
             *  This is a workaround for objects on the high bridges, which use the alternative occupier ptr.
             */
            ObjectClass *occupier = cell->OccupierPtr;
            if (cell->field_74) {
                occupier = cell->field_74;
            }

            /**
             *  If the cell has an occupier, draw its attached tag marker.
             */
            if (occupier && occupier->Tag) {

                const char *string = occupier->Tag->Class_Of()->Full_Name();

#if 0
                /**
                 *  Fixup the position based on its current coord (as infantry have sub cell locations).
                 */
                if (occupier->What_Am_I() == RTTI_INFANTRY) { 
                    Rect object_rect = occupier->entry_118();
                    cell_center.X = object_rect.X + object_rect.Width/2;
                    cell_center.Y = object_rect.Y + object_rect.Height/2;
                }
#endif

                /**
                 *  Fetch the text occupy rect.
                 */
                Rect text_rect;
                EditorFont->String_Pixel_Rect(string, &text_rect);

                /**
                 *  Move into position.
                 */
                text_rect.X = cell_center.X+10;
                text_rect.Y = (cell_center.Y-text_rect.Height)-33;
                text_rect.Width += 4;
                text_rect.Height += 2;

                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X-3, cell_center.Y-3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X+4, cell_center.Y-4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, cell_center.Y-11), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y-11), Point2D(cell_center.X+11, cell_center.Y-22), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X+10, cell_center.Y- 22), Point2D(cell_center.X+10, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, color_dark_blue);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print(string,
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X+1, text_rect.Y+1), text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW));
            }
        }
    }
}


static void Tactical_Editor_Draw_CellTags()
{
    unsigned color_dark_red = DSurface::RGBA_To_Pixel(128, 0, 0);
    unsigned color_white = DSurface::RGBA_To_Pixel(255, 255, 255);

    ColorScheme *text_color = ColorScheme::As_Pointer("White");

    /**
     *  Reset the cell iterator.
     */
    Map.Iterator_Reset();

    /**
     *  Iterate over all the map cells.
     */
    for (CellClass *cell = Map.Iterator_Next_Cell(); cell != nullptr; cell = Map.Iterator_Next_Cell()) {

        Rect cellrect = cell->Get_Rect();

        /**
         *  Determine if the cell draw rect is within the viewport.
         */
        Rect intersect = Intersect(cellrect, TacticalRect);
        if (intersect.Is_Valid()) {

            /**
             *  Get the center point of the cell.
             */
            Point2D cell_center;
            //cell_center.X = cellrect.X + CELL_PIXEL_W/2;
            //cell_center.Y = cellrect.Y + CELL_PIXEL_H/2;
            cell_center.X = cellrect.X + cellrect.Width/2;
            cell_center.Y = cellrect.Y + cellrect.Height/2;

            /**
             *  Draw the cell tag marker.
             */
            if (cell->CellTag) {

                const char *string = cell->CellTag->Class_Of()->Full_Name();

                /**
                 *  Fetch the text occupy rect.
                 */
                Rect text_rect;
                EditorFont->String_Pixel_Rect(string, &text_rect);

                /**
                 *  Move into position.
                 */
                text_rect.X = cell_center.X;
                text_rect.Y = (cell_center.Y-text_rect.Height)-33;
                text_rect.Width += 4;
                text_rect.Height += 2;

                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X-3, cell_center.Y-3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X+4, cell_center.Y-4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);
            
                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, color_dark_red);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print(string,
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X+1, text_rect.Y+1), text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW));
            }

        }
    }
}


static void Tactical_Editor_Draw_Waypoints()
{
    unsigned color_dark_green = DSurface::RGBA_To_Pixel(0, 128, 0);
    unsigned color_white = DSurface::RGBA_To_Pixel(255, 255, 255);
    unsigned color_black = DSurface::RGBA_To_Pixel(0, 0, 0);

    ColorScheme *text_color = ColorScheme::As_Pointer("White");

    /**
     *  Reset the cell iterator.
     */
    Map.Iterator_Reset();

    /**
      *  Draw the waypoint markers.
    */
    for (int index = 0; index < WAYPT_COUNT; ++index) {

        /**
         *  Fetch cell pointer for this waypoint if it exits.
         */
        CellClass *cell = Scen->Get_Waypoint_Cell(index);
        if (cell->IsWaypoint) {

            Rect cellrect = cell->Get_Rect();

            /**
             *  Get the center point of the cell.
             */
            Point2D cell_center;
            //cell_center.X = cellrect.X + CELL_PIXEL_W/2;
            //cell_center.Y = cellrect.Y + CELL_PIXEL_H/2;
            cell_center.X = cellrect.X + cellrect.Width/2;
            cell_center.Y = cellrect.Y + cellrect.Height/2;

            /**
             *  Determine if the cell draw rect is within the viewport.
             */
            Rect intersect = Intersect(cellrect, TacticalRect);
            if (intersect.Is_Valid()) {

                const char *string = Waypoint_As_String(index);

                /**
                 *  Fetch the text occupy rect.
                 */
                Rect text_rect;
                EditorFont->String_Pixel_Rect(string, &text_rect);

                /**
                 *  Move into position.
                 */
                text_rect.X = cell_center.X;
                text_rect.Y = (cell_center.Y-text_rect.Height)-21;
                text_rect.Width += 4;
                text_rect.Height += 2;

                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X-3, cell_center.Y-3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X+4, cell_center.Y-4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);
            
                unsigned rect_color = color_black;

                /**
                 *  Give special waypoints a different colour.
                 */
                if (index == WAYPT_HOME || index == WAYPT_REINF || index == WAYPT_SPECIAL) {
                    rect_color = color_dark_green;
                }

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print(string,
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X+1, text_rect.Y+1), text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW));
            }
        }
    }
}


static void Tactical_Editor_Draw_Text()
{
    RGBClass rgb_black(0,0,0);
    unsigned color_black = DSurface::RGBA_To_Pixel(0, 0, 0);
    ColorScheme *text_color = ColorScheme::As_Pointer("White");

    Rect text_rect;
    Rect fill_rect;

    char buffer[128];

{
    std::snprintf(buffer, sizeof(buffer), "Tiberium = %d", Map.TotalValue);

    /**
     * Fetch the text occupy area.
     */
    GradFont6Ptr->String_Pixel_Rect(buffer, &text_rect);

    /**
     *  Fill the background area.
     */
    fill_rect.X = TacticalRect.X+TacticalRect.Width-text_rect.Width-2;
    fill_rect.Y = TacticalRect.Y;
    fill_rect.Width = text_rect.Width+4;
    fill_rect.Height = 16;
    CompositeSurface->Fill_Rect(fill_rect, color_black);

    /**
     *  Move rects into position.
     */
    text_rect.X = TacticalRect.X+TacticalRect.Width-text_rect.Width-1;
    text_rect.Y = fill_rect.Y;
    text_rect.Width += 2;
    text_rect.Height += 3;

    /**
     *  Display the total value of all Tiberium on the map.
     */
    Fancy_Text_Print(buffer, CompositeSurface, &CompositeSurface->Get_Rect(),
        &Point2D(text_rect.X, text_rect.Y), text_color, COLOR_BLACK, TextPrintType(TPF_6PT_GRAD|TPF_NOSHADOW));
}

{
    std::snprintf(buffer, sizeof(buffer), "Percent = %d", Scen->Percent);

    /**
     * Fetch the text occupy area.
     */
    GradFont6Ptr->String_Pixel_Rect(buffer, &text_rect);

    /**
     *  Fill the background area.
     */
    fill_rect.X = TacticalRect.X+TacticalRect.Width-text_rect.Width   -160   -2;
    fill_rect.Y = TacticalRect.Y;
    fill_rect.Width = text_rect.Width+4;
    fill_rect.Height = 16;
    CompositeSurface->Fill_Rect(fill_rect, color_black);

    /**
     *  Move rects into position.
     */
    text_rect.X = TacticalRect.X+TacticalRect.Width-text_rect.Width   -160   -1;
    text_rect.Y = fill_rect.Y;
    text_rect.Width += 2;
    text_rect.Height += 3;

    /**
     *  Display the total value of all Tiberium on the map.
     */
    Fancy_Text_Print(buffer, CompositeSurface, &CompositeSurface->Get_Rect(),
        &Point2D(text_rect.X, text_rect.Y), text_color, COLOR_BLACK, TextPrintType(TPF_6PT_GRAD|TPF_NOSHADOW));
}



    /*
    **    Update the text labels
    */
    if (CurrentObjects.Count()) {

#if 0
        /*
        **    Display the object's name & ID
        */
        label = Text_String(CurrentObjects.Fetch_Head()->Full_Name());
        sprintf(buf, "%s (%d)", label, CurrentObjects.Fetch_Head()->As_Target());

        /*
        **    print the label
        */
        Fancy_Text_Print(buf, 160, 0,
            &ColorRemaps[PCOLOR_BROWN], TBLACK,
            TPF_CENTER | TPF_NOSHADOW | TPF_EFNT);
#endif
    }

}


static void Tactical_Editor_Draw_Overlay()
{
    Tactical_Editor_Draw_Occupiers();
    Tactical_Editor_Draw_CellTags();
    Tactical_Editor_Draw_Waypoints();
    Tactical_Editor_Draw_Text();
}


/**
 *  Draw the overlay information text if set.
 * 
 *  @author: CCHyper
 */
static void Tactical_Draw_Information_Text()
{
    RGBClass rgb_black(0,0,0);
    unsigned color_black = DSurface::RGBA_To_Pixel(0, 0, 0);
    ColorScheme *text_color = ColorScheme::As_Pointer("White");

    int padding = 2;

    if (!TacticalExtension) {
        return;
    }

    const char *text = TacticalExtension->InfoTextBuffer.Peek_Buffer();
    if (!text) {
        return;
    }

    /**
     * Fetch the text occupy area.
     */
    Rect text_rect;
    GradFont6Ptr->String_Pixel_Rect(text, &text_rect);

    Rect fill_rect;

    TextPrintType style = TacticalExtension->InfoTextStyle;
    int pos_x = 0;
    int pos_y = 0;

    switch (TacticalExtension->InfoTextPosition) {

        default:
        case InfoTextPosType::TOP_LEFT:
            pos_x = TacticalRect.X;
            pos_y = TacticalRect.Y;
            
            /**
             *  Move rects into position.
             */
            fill_rect.X = pos_x;
            fill_rect.Y = pos_y;
            fill_rect.Width = text_rect.Width+(padding+1)+2;
            fill_rect.Height = text_rect.Height+1;

            text_rect.X = fill_rect.X+2;
            text_rect.Y = fill_rect.Y;
            text_rect.Width += padding;
            text_rect.Height += 3;

            break;

        case InfoTextPosType::TOP_RIGHT:
            pos_x = TacticalRect.X+TacticalRect.Width-text_rect.Width;
            pos_y = TacticalRect.Y;
            
            /**
             *  Move rects into position.
             */
            fill_rect.X = pos_x-5;
            fill_rect.Y = pos_y;
            fill_rect.Width = TacticalRect.X+TacticalRect.Width-text_rect.Width+3;
            fill_rect.Height = text_rect.Height+1;

            text_rect.X = TacticalRect.X+TacticalRect.Width-2;
            text_rect.Y = fill_rect.Y;
            text_rect.Width += padding;
            text_rect.Height += 3;

            style |= TPF_RIGHT;
            break;

        case InfoTextPosType::BOTTOM_LEFT:
            pos_x = 0;
            pos_y = TacticalRect.Y+TacticalRect.Height-text_rect.Height;
            
            /**
             *  Move rects into position.
             */
            fill_rect.X = pos_x;
            fill_rect.Y = pos_y;
            fill_rect.Width = text_rect.Width+(padding+1)+2;
            fill_rect.Height = text_rect.Height+1;

            text_rect.X = fill_rect.X+2;
            text_rect.Y = fill_rect.Y;
            text_rect.Width += padding;
            text_rect.Height += 3;

            break;

        case InfoTextPosType::BOTTOM_RIGHT:
            pos_x = TacticalRect.X+TacticalRect.Width-text_rect.Width;
            pos_y = TacticalRect.Y+TacticalRect.Height-text_rect.Height;

            /**
             *  Move rects into position.
             */
            fill_rect.X = pos_x-5;
            fill_rect.Y = pos_y;
            fill_rect.Width = TacticalRect.X+TacticalRect.Width-text_rect.Width+3;
            fill_rect.Height = text_rect.Height+1;

            text_rect.X = TacticalRect.X+TacticalRect.Width-2;
            text_rect.Y = fill_rect.Y;
            text_rect.Width += padding;
            text_rect.Height += 3;

            style |= TPF_RIGHT;

            break;

    };

    /**
     *  Fill the background area.
     */
    CompositeSurface->Fill_Rect_Trans(fill_rect, rgb_black, 50);

    /**
     *  Draw the overlay text.
     */
    Fancy_Text_Print(text, CompositeSurface, &CompositeSurface->Get_Rect(),
        &Point2D(text_rect.X, text_rect.Y), text_color, COLOR_TBLACK, style);
}


/**
 *  This patch intercepts the end of the rendering process for Tactical.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Tactical_Render_Patch)
{
    GET_REGISTER_STATIC(Tactical *, this_ptr, ebp);

    /**
     *  If the scenario editor mode is active, draw overlays.
     */
    if (Debug_Map) {
        Tactical_Editor_Draw_Overlay();
    }

    /**
     *  If the developer mode is active, draw the developer overlay.
     */
    if (Vinifera_DeveloperMode) {
        Tactical_Draw_Debug_Overlay();

        if (Vinifera_Developer_FrameStep) {
            Tactical_Draw_FrameStep_Overlay();
        }

    }

#ifndef RELEASE
    /**
     *  Draw the version number on screen for non-release builds.
     * 
     *  @note: This must be last in the draw order!
     */
    Vinifera_Draw_Version_Text(CompositeSurface);
#endif

    if (TacticalExtension) {

        /**
         *  Has custom screen text been set?
         */
        if (TacticalExtension->IsInfoTextSet) {

            /**
             *  Draw it to the screen.
             */
            Tactical_Draw_Information_Text();
            
            /**
             *  Play the one time notification sound if defined.
             */
            if (TacticalExtension->InfoTextNotifySound != VOC_NONE) {
                Sound_Effect(TacticalExtension->InfoTextNotifySound, TacticalExtension->InfoTextNotifySoundVolume);
                TacticalExtension->InfoTextNotifySound = VOC_NONE;
            }
            
            /**
             *  If the screen timer has expired, disable drawing.
             */
            if (TacticalExtension->InfoTextTimer.Expired()) {
                TacticalExtension->InfoTextTimer.Stop();
                TacticalExtension->IsInfoTextSet = false;
                TacticalExtension->InfoTextNotifySound = VOC_NONE;
                TacticalExtension->InfoTextPosition = TOP_LEFT;
            }       
        }
    }

    /**
     *  Stolen bytes/code.
     */
original_code:
    this_ptr->Draw_Screen_Text(this_ptr->ScreenText);

    this_ptr->field_D30 = false;
    this_ptr->IsToRedraw = false;

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

    Patch_Jump(0x00611BCB, &_Tactical_Render_Patch);

    Patch_Jump(0x00616E9A, &_Tactical_Draw_Rally_Points_NormaliseLineAnimation_Patch);
    Patch_Jump(0x006172DB, &_Tactical_Draw_Waypoint_Paths_NormaliseLineAnimation_Patch);
    Patch_Jump(0x00617327, &_Tactical_Draw_Waypoint_Paths_DrawNormalLine_Patch);

    /**
     *  #issue-351
     * 
     *  Changes the waypoint number text to have a stroke/outline.
     * 
     *  @authors: CCHyper
     */
    Patch_Dword(0x006171C8+1, (TPF_CENTER|TPF_EFNT|TPF_FULLSHADOW));
}
