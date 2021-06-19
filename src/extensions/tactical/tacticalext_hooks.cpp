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
#include "fatal.h"
#include "vinifera_util.h"
#include "vinifera_gitinfo.h"
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



#include "wwmouse.h"
#include "tibsun_util.h"
#include "object.h"
#include "techno.h"
#include "foot.h"
#include "infantry.h"
#include "infantrytype.h"
#include "building.h"
#include "buildingtype.h"
#include "unit.h"
#include "unittype.h"
#include "aircraft.h"
#include "aircrafttype.h"
#include "iomap.h"

ColorScheme * TextColor = nullptr;

static void Print_Current_Object()
{
    // Format print string.
    char buffer[4096];

    // Get primary surface rect.
    Rect surfrect = PrimarySurface->Get_Rect();
    Rect comprect = CompositeSurface->Get_Rect();

    // Get rect of the font space.
    //Rect fontrect;
    //Font_Ptr(TPF_VCR)->String_Pixel_Rect(buffer, &fontrect);

    // Draw background rect.
    int rect_width = 200; // Needed, otherwise the box jitters about due to Coord value change.
    int rect_height = 100;
    Rect boxrect(comprect.X + comprect.Width - /*fontrect.Width*/rect_width, comprect.Y + 16/*(comprect.Y + comprect.Height) - fontrect.Height    rect_height*/, /*fontrect.Width + 2*/rect_width, /*fontrect.Height*/rect_height);
    //CompositeSurface->Fill_Rect_14(&boxrect, MAKE_RGB565(0, 0, 0));
    RGBClass rgb{0, 0, 0};

    int lines = 8; // Update this when you add a line!
    boxrect.Height = lines * 12 + 4; // 4 == header spacing.

    // Initial draw position.
    Point2D drawpos;
    drawpos.X = comprect.X + comprect.Width - 3;// - /*fontrect.Width*/rect_width;
    //drawpos.Y = comprect.Y + comprect.Height - /*fontrect.Height*/rect_height + 3;
    drawpos.Y = 17;

    // Check we have a object selected.
    if (CurrentObjects.Count() != 1) {
        boxrect.Height = 12;
        CompositeSurface->Fill_Rect_Trans(boxrect, rgb, 50);

        Fancy_Text_Print("< Select Object To See Info >",
            CompositeSurface,
            &surfrect,
            &Point2D(drawpos.X, drawpos.Y /*= 10*/),
            TextColor,
            COLOR_TBLACK,
            TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
        return;
    }

    CompositeSurface->Fill_Rect_Trans(boxrect, rgb, 50);

    Fancy_Text_Print("Current Object",
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y /*= 10*/),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

//    // Underline title.
//    Point2D start(drawpos.X - 4, drawpos.Y + 12);
//    Point2D end(drawpos.X - 40, drawpos.Y + 12);
//    CompositeSurface->Draw_Line(&start, &end, DSurface::Pack_RGB(255, 255, 255));
//
    // Make a space under the title.
    drawpos.Y += 4;

    ObjectClass *object = CurrentObjects.Fetch_Head();

    //
    // Abstract
    //
    std::snprintf(buffer, sizeof(buffer), "ID: %d", object->Fetch_ID());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "HeapID: %d", object->Get_Heap_ID());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsDirty: %s", object->Is_Dirty() ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    //
    // Object
    //
    std::snprintf(buffer, sizeof(buffer), "Strength: %d", object->Strength);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "Layer: %d", object->Layer);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "Coord: %d,%d,%d", object->Coord.X, object->Coord.Y, object->Coord.Z);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "Cell: %d,%d", object->Coord.X / 256, object->Coord.Y / 256);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
}



static void Print_Current_Techno()
{
    // Format print string.
    char buffer[4096];

    // Get primary surface rect.
    Rect surfrect = PrimarySurface->Get_Rect();
    Rect comprect = CompositeSurface->Get_Rect();

    // Get rect of the font space.
    //Rect fontrect;
    //Font_Ptr(TPF_VCR)->String_Pixel_Rect(buffer, &fontrect);

    // Draw background rect.
    int rect_width = 200; // Needed, otherwise the box jitters about due to Coord value change.
    int rect_height = 100;
    Rect boxrect(comprect.X + comprect.Width - /*fontrect.Width*/rect_width, comprect.Y + 16/*(comprect.Y + comprect.Height) - fontrect.Height    rect_height*/, /*fontrect.Width + 2*/rect_width, /*fontrect.Height*/rect_height);
    //CompositeSurface->Fill_Rect_14(&boxrect, MAKE_RGB565(0, 0, 0));
    RGBClass rgb{0, 0, 0};

    int lines = 8; // Update this when you add a line!
    boxrect.Height = lines * 12 + 4; // 4 == header spacing.

    // Initial draw position.
    Point2D drawpos;
    drawpos.X = comprect.X + comprect.Width - 3;// - /*fontrect.Width*/rect_width;
    //drawpos.Y = comprect.Y + comprect.Height - /*fontrect.Height*/rect_height + 3;
    drawpos.Y = 17;

    // Check we have a object selected.
    if (CurrentObjects.Count() != 1) {
        boxrect.Height = 12;
        CompositeSurface->Fill_Rect_Trans(boxrect, rgb, 50);

        Fancy_Text_Print("< Select Techno To See Info >",
            CompositeSurface,
            &surfrect,
            &Point2D(drawpos.X, drawpos.Y /*= 10*/),
            TextColor,
            COLOR_TBLACK,
            TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
        return;
    }

    CompositeSurface->Fill_Rect_Trans(boxrect, rgb, 50);

    Fancy_Text_Print("Techno",
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y /*= 10*/),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    // Make a space under the title.
    drawpos.Y += 4;

    const TechnoClass *techno = dynamic_cast<const TechnoClass *>(CurrentObjects.Fetch_Head());

    std::snprintf(buffer, sizeof(buffer), "ActLike: %d", techno->ActLike);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "ArmorBias: %lf", techno->ArmorBias);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "FirepowerBias: %lf", techno->FirepowerBias);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "RadarPos: %d,%d", techno->RadarPos.X, techno->RadarPos.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "SpiedBy: %d", techno->SpiedBy);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "Group: %d", techno->Group);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    //ArchiveTarget

    std::snprintf(buffer, sizeof(buffer), "Cloak: %d", techno->Cloak);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_118: %lf", techno->field_118);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "PitchAngle: %lf", techno->PitchAngle);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "Ammo: %d", techno->Ammo);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "PurchasePrice: %d", techno->PurchasePrice);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "AngleRotatedSideways: %lf", techno->AngleRotatedSideways);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "AngleRotatedForwards: %lf", techno->AngleRotatedForwards);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "RockingSidewaysPerFrame: %lf", techno->RockingSidewaysPerFrame);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "RockingForwardsPerFrame: %lf", techno->RockingForwardsPerFrame);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "EnteredByInfType: %s", InfantryTypeClass::Name_From(techno->EnteredByInfType));
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "Storage: %d,%d,%d,%d", techno->Storage.Get_Amount(TIBERIUM_RIPARIUS), techno->Storage.Get_Amount(TIBERIUM_CRUENTUS), techno->Storage.Get_Amount(TIBERIUM_VINIFERA), techno->Storage.Get_Amount(TIBERIUM_ABOREUS));
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "Door(timer): %f", techno->Door.Timer());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    /*std::snprintf(buffer, sizeof(buffer), "BarrelFacing.Current: %d", (int)techno->BarrelFacing.Current());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "BarrelFacing.Desired: %d", (int)techno->BarrelFacing.Desired());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "BarrelFacing.field_8: %d", (int)techno->BarrelFacing.field_8());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "BarrelFacing.field_14: %d", (int)techno->BarrelFacing.field_14);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);*/

    std::snprintf(buffer, sizeof(buffer), "PrimaryFacing (DirType): %d", (DirType)techno->PrimaryFacing.Current());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "PrimaryFacing (DirType): %d", (DirType)techno->PrimaryFacing.Desired());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "PrimaryFacing (FacingType): %d", (FacingType)techno->PrimaryFacing.Current());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "PrimaryFacing (FacingType): %d", (FacingType)techno->PrimaryFacing.Desired());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "PrimaryFacing.Current: %d", (int)techno->PrimaryFacing.Current());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "PrimaryFacing.Desired: %d", (int)techno->PrimaryFacing.Desired());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "PrimaryFacing.RotationTimer: %d", (int)techno->PrimaryFacing.RotationTimer());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "PrimaryFacing.ROT: %d", (int)techno->PrimaryFacing.ROT);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    /*std::snprintf(buffer, sizeof(buffer), "TurretFacing.Current: %d", (int)techno->TurretFacing.Current());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "TurretFacing.Desired: %d", (int)techno->TurretFacing.Desired());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "TurretFacing.field_8: %d", (int)techno->TurretFacing.field_8());
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "TurretFacing.field_14: %d", (int)techno->TurretFacing.field_14);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);*/

    std::snprintf(buffer, sizeof(buffer), "CurrentBurstIndex: %d", techno->CurrentBurstIndex);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "SoundRandomSeed: %d", techno->SoundRandomSeed);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsALoaner: %d", techno->IsALoaner);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsLocked: %s", techno->IsLocked ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsInRecoilState: %s", techno->IsInRecoilState ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsTethered: %s", techno->IsTethered ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsALemon: %s", techno->IsALemon ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_202: %s", techno->field_202 ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_203: %s", techno->field_203 ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_204: %s", techno->field_204 ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_205: %s", techno->field_205 ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_206: %s", techno->field_206 ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_207: %s", techno->field_207 ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsOnCarryall: %s", techno->IsOnCarryall ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_209: %s", techno->field_209 ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_20A: %s", techno->field_20A ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_20B: %s", techno->field_20B ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "EMPFramesRemaining: %d", techno->EMPFramesRemaining);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_214: %d", techno->field_214);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "LimpetSpeedFactor: %f", techno->LimpetSpeedFactor);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
}


extern "C" __declspec(dllexport) void Print_Current_Foot()
{
    // Format print string.
    char buffer[4096];

    // Get primary surface rect.
    Rect surfrect = PrimarySurface->Get_Rect();
    Rect comprect = CompositeSurface->Get_Rect();

    // Get rect of the font space.
    //Rect fontrect;
    //Font_Ptr(TPF_VCR)->String_Pixel_Rect(buffer, &fontrect);

    // Draw background rect.
    int rect_width = 200; // Needed, otherwise the box jitters about due to Coord value change.
    int rect_height = 100;
    Rect boxrect(comprect.X + comprect.Width - /*fontrect.Width*/rect_width, comprect.Y + 16/*(comprect.Y + comprect.Height) - fontrect.Height    rect_height*/, /*fontrect.Width + 2*/rect_width, /*fontrect.Height*/rect_height);
    //CompositeSurface->Fill_Rect_14(&boxrect, MAKE_RGB565(0, 0, 0));
    RGBClass rgb{0, 0, 0};

    int lines = 8; // Update this when you add a line!
    boxrect.Height = lines * 12 + 4; // 4 == header spacing.

    // Initial draw position.
    Point2D drawpos;
    drawpos.X = comprect.X + comprect.Width - 3;// - /*fontrect.Width*/rect_width;
    //drawpos.Y = comprect.Y + comprect.Height - /*fontrect.Height*/rect_height + 3;
    drawpos.Y = 17;

    // Check we have a object selected.
    if (CurrentObjects.Count() != 1 || CurrentObjects.Fetch_Head()->What_Am_I() == RTTI_BUILDING) {
        boxrect.Height = 12;
        CompositeSurface->Fill_Rect_Trans(boxrect, rgb, 50);

        Fancy_Text_Print("< Select Foot To See Info >",
            CompositeSurface,
            &surfrect,
            &Point2D(drawpos.X, drawpos.Y /*= 10*/),
            TextColor,
            COLOR_TBLACK,
            TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
        return;
    }

    boxrect.Height = comprect.Height;
    CompositeSurface->Fill_Rect_Trans(boxrect, rgb, 50);

    Fancy_Text_Print("Foot",
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y /*= 10*/),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    // Make a space under the title.
    drawpos.Y += 4;

    FootClass *foot = dynamic_cast<FootClass *>(CurrentObjects.Fetch_Head());

    std::snprintf(buffer, sizeof(buffer), "field_220: %d", foot->field_220);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_224: %d,%d", foot->field_224.X, foot->field_224.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_228: %d,%d", foot->field_228.X, foot->field_228.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_230: %lf", foot->field_230);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "CurrentWalkingFrame: %d", foot->CurrentWalkingFrame);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_23C: %d,%d", foot->field_23C.X, foot->field_23C.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_240: %d,%d", foot->field_240.X, foot->field_240.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_244: %d,%d,%d", foot->field_244.X, foot->field_244.Y, foot->field_244.Z);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "Speed: %lf", foot->Speed);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "SpeedBias: %lf", foot->SpeedBias);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    //std::snprintf(buffer, sizeof(buffer), "field_2A0: %s", foot->field_2A0 ? foot->field_2A0 : "(null)");
    //Fancy_Text_Print(buffer,
    //    CompositeSurface,
    //    &surfrect,
    //    &Point2D(drawpos.X, drawpos.Y += 12),
    //    TextColor,
    //    COLOR_TBLACK,
    //    TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "TryTryAgain: %d", foot->TryTryAgain);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "HeadToCoord: %d,%d,%d", foot->HeadToCoord.X, foot->HeadToCoord.Y, foot->HeadToCoord.Z);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "CurrentTube: %d", foot->CurrentTube);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "CurrentTubeDir: %d", foot->CurrentTubeDir);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_33E: %d", foot->field_33E);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsToScatter: %s", foot->IsToScatter ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsScanLimited: %s", foot->IsScanLimited ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsInitiated: %s", foot->IsInitiated ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsNewNavCom: %s", foot->IsNewNavCom ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsPlanningToLook: %s", foot->IsPlanningToLook ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsDeploying: %s", foot->IsDeploying ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsFiring: %s", foot->IsFiring ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsRotating: %s", foot->IsRotating ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsUnloading: %s", foot->IsUnloading ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsNavQueueLoop: %s", foot->IsNavQueueLoop ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "IsScattering: %s", foot->IsScattering ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_34A: %s", foot->field_34A ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_34B: %s", foot->field_34B ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_34C: %s", foot->field_34C ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_34D: %s", foot->field_34D ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_34E: %s", foot->field_34E ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "field_34F: %s", foot->field_34F ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    if (foot->Locomotion) {


        // Fetch locomotor uuid.
        /*WWComPtr<LocomotionClass> loco;
        int hr = loco.func_43C590(foot->Locomotion);
        if (FAILED(hr) && (hr != E_NOINTERFACE)) {
            _com_issue_error(hr);
        }*/

        //_com_ptr_t<LocomotionClass> loco = &foot->Locomotion;
        //WWComPtr<LocomotionClass> loco = &foot->Locomotion;
        //CComPtr<LocomotionClass> loco(foot->Locomotion);
        /*LocomotionClass *locoptr = static_cast<LocomotionClass *>(loco.GetInterfacePtr());
        if (!locoptr) {
            _com_issue_error(E_POINTER);
        }*/

        /*LocomotionClass * l;
        int hr = loco.QueryInterface(__uuidof(DriveLocomotionClass), l);
        if (FAILED(hr) && (hr != E_NOINTERFACE)) {
            _com_issue_error(hr);
        }*/

// Force scope to see comptr destructor.
//{ //////////////////////////////////////////////////////////////////////////////



#if(0)
        if (foot->Locomotion->Is_Moving()) {
            DebugInfo("Loco is moving!\n");




            //_com_ptr_t<IPersist> loco = &foot->Locomotion;
            IPersistPtr loco(foot->Locomotion);

            // Inlined use of ->, might be inlined in ctor.
    //        if (!loco) {
    //            _com_issue_error(E_POINTER);
    //        }

            // This would have been inlined within WWComPtr perhaps? there should not be access to _com_issue_error outside of com_ptr.
            CLSID clsid;
            int hr = loco->GetClassID(&clsid);
            if (FAILED(hr)) {
                _com_issue_error(hr);
            }

            // Doesnt work as we need to cast up to LocomotionClass to access it.
            //loco->Is_Moving();


            if (clsid == __uuidof(DriveLocomotionClass)) {

                // Cast to locomotion class, all hell breaks loose.
                DriveLocomotionClass * l = dynamic_cast<DriveLocomotionClass *>(loco.GetInterfacePtr());

                if (l->Is_Moving()) {
                    DebugInfo("Drive loco is moving!\n");
                }
            }



        }

#endif



//} //////////////////////////////////////////////////////////////////////////////


#if 0

        //
        // Fetch IPersist pointer to allow ILocomotion interface up-casting.
        //
        IPersistPtr loco(foot->Locomotion);

        // This would have been inlined within WWComPtr perhaps? there should not be access to _com_issue_error outside of com_ptr.
        CLSID clsid;
        int hr = loco->GetClassID(&clsid);
        if (FAILED(hr)) {
            _com_issue_error(hr);
        }

        //
        // DriveLocomotionClass
        //
        if (clsid == __uuidof(DriveLocomotionClass)) {
        //if (loco == __uuidof(DriveLocomotionClass)) {
            Fancy_Text_Print("Locomotion: Drive",
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 22),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            DriveLocomotionClass * driveloco = dynamic_cast<DriveLocomotionClass *>(loco.GetInterfacePtr());

            std::snprintf(buffer, sizeof(buffer), "Is_Moving: %s", driveloco->Is_Moving() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "Is_Moving_Now: %s", driveloco->Is_Moving_Now() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_18: %d", driveloco->field_18);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 16),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_1C: %d", driveloco->field_1C);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_20(timer): %f", driveloco->field_20());
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "DestinationCoord: %d,%d,%d", driveloco->DestinationCoord.X, driveloco->DestinationCoord.Y, driveloco->DestinationCoord.Z);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "HeadToCoord: %d,%d,%d", driveloco->HeadToCoord.X, driveloco->HeadToCoord.Y, driveloco->HeadToCoord.Z);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "SpeedAccum: %d", driveloco->SpeedAccum);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_50: %f", driveloco->field_50);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "TrackNumber: %d", driveloco->TrackNumber);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "TrackIndex: %d", driveloco->TrackIndex);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsOnShortTrack: %s", driveloco->IsOnShortTrack ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_61: %s", driveloco->field_61 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsRotating: %s", driveloco->IsRotating ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsDriving: %s", driveloco->IsDriving ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_64: %s", driveloco->field_64 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_65: %s", driveloco->field_65 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "PiggybackLocomotor: %s", driveloco->PiggybackLocomotor ? "(assigned)" : "(null)");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

        //
        // WalkLocomotionClass
        //
        } else if (clsid == __uuidof(WalkLocomotionClass)) {
            Fancy_Text_Print("Locomotion: Walk",
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 22),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            WalkLocomotionClass * walkloco = dynamic_cast<WalkLocomotionClass *>(loco.GetInterfacePtr());

            std::snprintf(buffer, sizeof(buffer), "Is_Moving: %s", walkloco->Is_Moving() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "Is_Moving_Now: %s", walkloco->Is_Moving_Now() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsMoving: %s", walkloco->IsMoving ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_31: %s", walkloco->field_31 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsReallyMoving: %s", walkloco->IsReallyMoving ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "PiggybackLocomotor: %s", walkloco->PiggybackLocomotor ? "(assigned)" : "(null)");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

        //
        // MechLocomotionClass
        //
        } else if (clsid == __uuidof(MechLocomotionClass)) {
            Fancy_Text_Print("Locomotion: Mech",
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 22),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            MechLocomotionClass * mechloco = dynamic_cast<MechLocomotionClass *>(loco.GetInterfacePtr());

            std::snprintf(buffer, sizeof(buffer), "Is_Moving: %s", mechloco->Is_Moving() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "Is_Moving_Now: %s", mechloco->Is_Moving_Now() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "DestinationCoord: %d,%d,%d", mechloco->DestinationCoord.X, mechloco->DestinationCoord.Y, mechloco->DestinationCoord.Z);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 16),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "HeadToCoord: %d,%d,%d", mechloco->HeadToCoord.X, mechloco->HeadToCoord.Y, mechloco->HeadToCoord.Z);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsMoving: %s", mechloco->IsMoving ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

        //
        // HoverLocomotionClass
        //
        } else if (clsid == __uuidof(HoverLocomotionClass)) {
            Fancy_Text_Print("Locomotion: Hover",
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 22),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            HoverLocomotionClass * hoverloco = dynamic_cast<HoverLocomotionClass *>(loco.GetInterfacePtr());

            std::snprintf(buffer, sizeof(buffer), "Is_Moving: %s", hoverloco->Is_Moving() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "Is_Moving_Now: %s", hoverloco->Is_Moving_Now() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "DestinationCoord: %d,%d,%d", hoverloco->DestinationCoord.X, hoverloco->DestinationCoord.Y, hoverloco->DestinationCoord.Z);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 16),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "HeadToCoord: %d,%d,%d", hoverloco->HeadToCoord.X, hoverloco->HeadToCoord.Y, hoverloco->HeadToCoord.Z);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_2C.CurrentFacing: %d", (int)hoverloco->field_2C.Current());
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_2C.DesiredFacing: %d", (int)hoverloco->field_2C.Desired());
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_48: %lf", hoverloco->field_48);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_50: %lf", hoverloco->field_50);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_58: %lf", hoverloco->field_58);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_60: %lf", hoverloco->field_60);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_68: %s", hoverloco->field_68 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_6C: %d", hoverloco->field_6C);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_70: %s", hoverloco->field_70 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

        //
        // JumpjetLocomotionClass
        //
        } else if (clsid == __uuidof(JumpjetLocomotionClass)) {
            Fancy_Text_Print("Locomotion: JumpJet",
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 22),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            JumpjetLocomotionClass * jjloco = dynamic_cast<JumpjetLocomotionClass *>(loco.GetInterfacePtr());

            std::snprintf(buffer, sizeof(buffer), "Is_Moving: %s", jjloco->Is_Moving() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "Is_Moving_Now: %s", jjloco->Is_Moving_Now() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "DestinationCoord: %d,%d,%d", jjloco->DestinationCoord.X, jjloco->DestinationCoord.Y, jjloco->DestinationCoord.Z);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 16),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_20: %d", jjloco->field_20);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_24: %d", jjloco->field_24);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_28.CurrentFacing: %d", (int)jjloco->field_28.Current());
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_28.DesiredFacing: %d", (int)jjloco->field_28.Desired());
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_40: %lf", jjloco->field_40);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_48: %lf", jjloco->field_48);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_50: %d", jjloco->field_50);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_54: %d", jjloco->field_54);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_58: %lf", jjloco->field_58);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_60: %s", jjloco->field_60 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

        //
        // TunnelLocomotionClass
        //
        } else if (clsid == __uuidof(TunnelLocomotionClass)) {
            Fancy_Text_Print("Locomotion: Tunnel",
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 22),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            TunnelLocomotionClass * tunnelloco = dynamic_cast<TunnelLocomotionClass *>(loco.GetInterfacePtr());

            std::snprintf(buffer, sizeof(buffer), "Is_Moving: %s", tunnelloco->Is_Moving() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "Is_Moving_Now: %s", tunnelloco->Is_Moving_Now() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_18: %d,%d,%d", tunnelloco->field_18.X, tunnelloco->field_18.Y, tunnelloco->field_18.Z);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 16),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_14: %d", tunnelloco->field_14);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_24: %d", tunnelloco->field_24);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsMoving: %s", tunnelloco->IsMoving ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

        //
        // LevitateLocomotionClass
        //
        } else if (clsid == __uuidof(LevitateLocomotionClass)) {
            Fancy_Text_Print("Locomotion: Levitate",
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 22),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            LevitateLocomotionClass * levloco = dynamic_cast<LevitateLocomotionClass *>(loco.GetInterfacePtr());

            std::snprintf(buffer, sizeof(buffer), "Is_Moving: %s", levloco->Is_Moving() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "Is_Moving_Now: %s", levloco->Is_Moving_Now() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_14: %d", levloco->field_14);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_18: %lf", levloco->field_18);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_20: %lf", levloco->field_20);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_28: %lf", levloco->field_28);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_30: %lf", levloco->field_30);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_38: %lf", levloco->field_38);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_40: %lf", levloco->field_40);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_48: %lf", levloco->field_48);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_50: %lf", levloco->field_50);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

        //
        // FlyLocomotionClass
        //
        } else if (clsid == __uuidof(FlyLocomotionClass)) {
            Fancy_Text_Print("Locomotion: Fly",
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 22),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            FlyLocomotionClass * flyloco = dynamic_cast<FlyLocomotionClass *>(loco.GetInterfacePtr());

            std::snprintf(buffer, sizeof(buffer), "Is_Moving: %s", flyloco->Is_Moving() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "Is_Moving_Now: %s", flyloco->Is_Moving_Now() ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsMoving: %s", flyloco->IsMoving ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsTakingOff: %s", flyloco->IsTakingOff ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsLanding: %s", flyloco->IsLanding ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_4A: %s", flyloco->field_4A ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_4B: %s", flyloco->field_4B ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_54: %s", flyloco->field_54 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_30: %d", flyloco->field_30);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_38: %f", flyloco->field_38);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_40: %f", flyloco->field_40);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_4C: %d", flyloco->field_4C);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_50: %d", flyloco->field_50);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

        //
        // Unknown.
        //
        } else {
            /*Fancy_Text_Print("Locomotion: Unknown?",
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);*/
        }
#endif
    }
}


static void Print_Current_Top_Level()
{
    // Format print string.
    char buffer[4096];

    // Get primary surface rect.
    Rect surfrect = PrimarySurface->Get_Rect();
    Rect comprect = CompositeSurface->Get_Rect();

    // Get rect of the font space.
    //Rect fontrect;
    //Font_Ptr(TPF_VCR)->String_Pixel_Rect(buffer, &fontrect);

    // Draw background rect.
    int rect_width = 200; // Needed, otherwise the box jitters about due to Coord value change.
    int rect_height = 100;
    Rect boxrect(comprect.X + comprect.Width - /*fontrect.Width*/rect_width, comprect.Y + 16/*(comprect.Y + comprect.Height) - fontrect.Height    rect_height*/, /*fontrect.Width + 2*/rect_width, /*fontrect.Height*/rect_height);
    //CompositeSurface->Fill_Rect_14(&boxrect, MAKE_RGB565(0, 0, 0));
    RGBClass rgb{0, 0, 0};

    int lines = 8; // Update this when you add a line!
    boxrect.Height = lines * 12 + 4; // 4 == header spacing.

    // Initial draw position.
    Point2D drawpos;
    drawpos.X = comprect.X + comprect.Width - 3;// - /*fontrect.Width*/rect_width;
    //drawpos.Y = comprect.Y + comprect.Height - /*fontrect.Height*/rect_height + 3;
    drawpos.Y = 17;

    // Check we have a object selected.
    if (CurrentObjects.Count() != 1) {
        boxrect.Height = 12;
        CompositeSurface->Fill_Rect_Trans(boxrect, rgb, 50);

        Fancy_Text_Print("< Select Object To See Info >",
            CompositeSurface,
            &surfrect,
            &Point2D(drawpos.X, drawpos.Y /*= 10*/),
            TextColor,
            COLOR_TBLACK,
            TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
        return;
    }

//     Fancy_Text_Print(RTTI_Name_From((RTTIType)CurrentObjects.Fetch_Head()->What_Am_I()),
//         CompositeSurface,
//         &surfrect,
//         &Point2D(drawpos.X, drawpos.Y /*= 10*/),
//         TextColor,
//         COLOR_TBLACK,
//         TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    // Make a space under the title.
    drawpos.Y += 4;

    switch (CurrentObjects.Fetch_Head()->What_Am_I()) {
        //
        // Building ------------------------------------------------------------
        //
        case RTTI_BUILDING:
        {
            const BuildingClass *building = dynamic_cast<const BuildingClass *>(CurrentObjects.Fetch_Head());

            std::snprintf(buffer, sizeof(buffer), "WhoLastHurtMe: %d", building->WhoLastHurtMe);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "WhomToRepay: %s", building->WhomToRepay ? static_cast<ObjectClass *>(building->WhomToRepay)->Class_Of()->Name() : "(null)");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "LastStrength: %d", building->LastStrength);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_298: %d", building->field_298);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_29C: %d", building->field_29C);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "LaserFenceFrame: %d", building->LaserFenceFrame);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "FirestormWallFrame: %d", building->FirestormWallFrame);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_2C0: %d", building->field_2C0.Fetch_Stage());
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_2D8: %d,%d,%d,%d", building->field_2D8.X, building->field_2D8.Y, building->field_2D8.Width, building->field_2D8.Height);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_2E8: %d,%d,%d", building->field_2E8.X, building->field_2E8.Y, building->field_2E8.Z);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_2F4: %d,%d", building->field_2F4.X, building->field_2F4.Y);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_301: %s", building->IsReadyToCommence ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_302: %s", building->IsWrenchVisible ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_303: %s", building->IsGoingToBlow ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_304: %s", building->IsSurvivorless ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_308: %s", building->HasOpened ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_309: %s", building->field_309 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_30D: %s", building->field_30D ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_30E: %s", building->field_30E ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_30F: %d", building->field_30F);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "HasCloakRadius: %s", building->HasCloakRadius ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "TranslucencyLevel: %d", building->TranslucencyLevel);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "UpgradeLevel: %d", building->UpgradeLevel);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "GateFrame: %d", building->GateFrame);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
            break;
        }

        //
        // Unit ----------------------------------------------------------------
        //
        case RTTI_UNIT:
        {
            const UnitClass *unit = dynamic_cast<const UnitClass *>(CurrentObjects.Fetch_Head());

            //std::snprintf(buffer, sizeof(buffer), "field_350: %d", unit->field_350);
            //Fancy_Text_Print(buffer,
            //    CompositeSurface,
            //    &surfrect,
            //    &Point2D(drawpos.X, drawpos.Y += 12),
            //    TextColor,
            //    COLOR_TBLACK,
            //    TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            //std::snprintf(buffer, sizeof(buffer), "field_364: %s", unit->field_364 ? unit->field_364->Class_Of()->Name() : "(null)");
            //Fancy_Text_Print(buffer,
            //    CompositeSurface,
            //    &surfrect,
            //    &Point2D(drawpos.X, drawpos.Y += 12),
            //    TextColor,
            //    COLOR_TBLACK,
            //    TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
            //
            //std::snprintf(buffer, sizeof(buffer), "field_364: %s", unit->field_364 ? RTTI_Name_From(unit->field_364->Kind_Of()) : "(null)");
            //Fancy_Text_Print(buffer,
            //    CompositeSurface,
            //    &surfrect,
            //    &Point2D(drawpos.X, drawpos.Y += 12),
            //    TextColor,
            //    COLOR_TBLACK,
            //    TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
            //
            //std::snprintf(buffer, sizeof(buffer), "field_36C: %s", unit->field_36C ? "true" : "false");
            //Fancy_Text_Print(buffer,
            //    CompositeSurface,
            //    &surfrect,
            //    &Point2D(drawpos.X, drawpos.Y += 12),
            //    TextColor,
            //    COLOR_TBLACK,
            //    TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsDumping: %s", unit->IsDumping ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsHarvesting: %s", unit->IsHarvesting ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_36F: %s", unit->field_36F ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            //std::snprintf(buffer, sizeof(buffer), "field_370: %d", unit->field_370);
            //Fancy_Text_Print(buffer,
            //    CompositeSurface,
            //    &surfrect,
            //    &Point2D(drawpos.X, drawpos.Y += 12),
            //    TextColor,
            //    COLOR_TBLACK,
            //    TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            //std::snprintf(buffer, sizeof(buffer), "StartCharge: %d", unit->StartCharge);
            //Fancy_Text_Print(buffer,
            //    CompositeSurface,
            //    &surfrect,
            //    &Point2D(drawpos.X, drawpos.Y += 12),
            //    TextColor,
            //    COLOR_TBLACK,
            //    TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
            //
            //std::snprintf(buffer, sizeof(buffer), "field_378: %d", unit->field_378);
            //Fancy_Text_Print(buffer,
            //    CompositeSurface,
            //    &surfrect,
            //    &Point2D(drawpos.X, drawpos.Y += 12),
            //    TextColor,
            //    COLOR_TBLACK,
            //    TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_37C: %d", unit->field_37C);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            break;
        }

        //
        // Infantry ------------------------------------------------------------
        //
        case RTTI_INFANTRY:
        {
            const InfantryClass *infantry = dynamic_cast<const InfantryClass *>(CurrentObjects.Fetch_Head());

            std::snprintf(buffer, sizeof(buffer), "Doing: %d", infantry->Doing);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "Fear: %d", infantry->Fear);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsBerzerk: %s", infantry->IsBerzerk ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsTechnician: %s", infantry->IsTechnician ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsStoked: %s", infantry->IsStoked ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsProne: %s", infantry->IsProne ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsZoneCheat: %s", infantry->IsZoneCheat ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "WasSelected: %s", infantry->WasSelected ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            break;
        }

        //
        // Aircraft ------------------------------------------------------------
        //
        case RTTI_AIRCRAFT:
        {
            const AircraftClass *aircraft = dynamic_cast<const AircraftClass *>(CurrentObjects.Fetch_Head());

            std::snprintf(buffer, sizeof(buffer), "field_358: %s", aircraft->field_358 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "Passenger: %s", aircraft->Passenger ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_35A: %s", aircraft->field_35A ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_35B: %s", aircraft->field_35B ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "IsLocked: %s", aircraft->IsLocked ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            //std::snprintf(buffer, sizeof(buffer), "field_36C: %d", aircraft->field_36C);
            //Fancy_Text_Print(buffer,
            //    CompositeSurface,
            //    &surfrect,
            //    &Point2D(drawpos.X, drawpos.Y += 12),
            //    TextColor,
            //    COLOR_TBLACK,
            //    TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_370: %s", aircraft->field_370 ? "true" : "false");
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            std::snprintf(buffer, sizeof(buffer), "field_374: %d", aircraft->field_374);
            Fancy_Text_Print(buffer,
                CompositeSurface,
                &surfrect,
                &Point2D(drawpos.X, drawpos.Y += 12),
                TextColor,
                COLOR_TBLACK,
                TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

            break;
        }

    };
}


static void Print_Tactical_Info()
{
    // Format print string.
    char buffer[4096];

    // Get primary surface rect.
    Rect surfrect = PrimarySurface->Get_Rect();
    Rect comprect = CompositeSurface->Get_Rect();

    // Draw background rect.
    int rect_width = 400; // Needed, otherwise the box jitters about due to Coord value change.
    int rect_height = 100;
    Rect boxrect(comprect.X, comprect.Y + 16, rect_width, rect_height);
    //CompositeSurface->Fill_Rect_14(&boxrect, MAKE_RGB565(0, 0, 0));
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
    //std::snprintf(buffer, sizeof(buffer), "field_54: %d", TacticalMap->field_54);
    //Fancy_Text_Print(buffer,
    //    CompositeSurface,
    //    &surfrect,
    //    &Point2D(drawpos.X, drawpos.Y += 12),
    //    TextColor,
    //    COLOR_TBLACK,
    //    TextPrintType(TPF_VCR|TPF_NOSHADOW));

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

    //std::snprintf(buffer, sizeof(buffer), "field_7C: %d,%d", TacticalMap->field_7C.X, TacticalMap->field_7C.Y);
    //Fancy_Text_Print(buffer,
    //    CompositeSurface,
    //    &surfrect,
    //    &Point2D(drawpos.X, drawpos.Y += 12),
    //    TextColor,
    //    COLOR_TBLACK,
    //    TextPrintType(TPF_VCR|TPF_NOSHADOW));
    //
    //std::snprintf(buffer, sizeof(buffer), "field_84: %d,%d", TacticalMap->field_84.X, TacticalMap->field_84.Y);
    //Fancy_Text_Print(buffer,
    //    CompositeSurface,
    //    &surfrect,
    //    &Point2D(drawpos.X, drawpos.Y += 12),
    //    TextColor,
    //    COLOR_TBLACK,
    //    TextPrintType(TPF_VCR|TPF_NOSHADOW));
    //
    //std::snprintf(buffer, sizeof(buffer), "field_8C: %d,%d", TacticalMap->field_8C.X, TacticalMap->field_8C.Y);
    //Fancy_Text_Print(buffer,
    //    CompositeSurface,
    //    &surfrect,
    //    &Point2D(drawpos.X, drawpos.Y += 12),
    //    TextColor,
    //    COLOR_TBLACK,
    //    TextPrintType(TPF_VCR|TPF_NOSHADOW));

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

    std::snprintf(buffer, sizeof(buffer), "field_D34: %d,%d,%d,%d",
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

    std::snprintf(buffer, sizeof(buffer), "field_98: %d", TacticalMap->field_98);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

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

    //std::snprintf(buffer, sizeof(buffer), "ScaleFactor: %lf", TacticalMap->ScaleFactor);
    //Fancy_Text_Print(buffer,
    //    CompositeSurface,
    //    &surfrect,
    //    &Point2D(drawpos.X, drawpos.Y += 12),
    //    TextColor,
    //    COLOR_TBLACK,
    //    TextPrintType(TPF_VCR|TPF_NOSHADOW));

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


static void Print_Display_Info()
{
    // Format print string.
    char buffer[4096];

    // Get primary surface rect.
    Rect surfrect = PrimarySurface->Get_Rect();
    Rect comprect = CompositeSurface->Get_Rect();

    // Draw background rect.
    int rect_width = 200; // Needed, otherwise the box jitters about due to Coord value change.
    int rect_height = 100;
    Rect boxrect(comprect.X, comprect.Y + 16, rect_width, rect_height);
    //CompositeSurface->Fill_Rect_14(&boxrect, MAKE_RGB565(0, 0, 0));
    RGBClass rgb{0, 0, 0};

    int lines = 20; // Update this when you add a line!
    boxrect.Height = lines * 24 + 4; // 4 == header spacing.

    CompositeSurface->Fill_Rect_Trans(boxrect, rgb, 50);

    // Draw position.
    Point2D drawpos;
    drawpos.X = 1;
    drawpos.Y = 17;

    Fancy_Text_Print("Display",
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y /*= 10*/),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    // Make a space under the title.
    drawpos.Y += 4;

    //
    // Display
    //
    std::snprintf(buffer, sizeof(buffer), "IsProximityCheck: %s", Map.IsProximityCheck ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    //std::snprintf(buffer, sizeof(buffer), "field_11A9: %s", Map.field_11A9 ? "true" : "false");
    //Fancy_Text_Print(buffer,
    //    CompositeSurface,
    //    &surfrect,
    //    &Point2D(drawpos.X, drawpos.Y += 12),
    //    TextColor,
    //    COLOR_TBLACK,
    //    TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_11AA: %s", Map.IsFollowingObject ? "true" : "false");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "FollowingObjectPtr: %s", Map.FollowingObjectPtr ? Map.FollowingObjectPtr->Class_Of()->Name() : "(null)");
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    //std::snprintf(buffer, sizeof(buffer), "FollowingObjectPtr: %s", Map.FollowingObjectPtr ? RTTI_Name_From(Map.FollowingObjectPtr->Kind_Of()) : "(null)");
    //Fancy_Text_Print(buffer,
    //    CompositeSurface,
    //    &surfrect,
    //    &Point2D(drawpos.X, drawpos.Y += 12),
    //    TextColor,
    //    COLOR_TBLACK,
    //    TextPrintType(TPF_VCR|TPF_NOSHADOW));

    if (Map.field_11C4) {
        std::snprintf(buffer, sizeof(buffer), "field_11C4: %d, %d, %d", Map.field_11C4->X, Map.field_11C4->Y, Map.field_11C4->Z);
    } else {
        std::snprintf(buffer, sizeof(buffer), "field_11C4: (null)");
    }
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_11C8: %d, %d, %d", Map.field_11C8.X, Map.field_11C8.Y, Map.field_11C8.Z);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));

    std::snprintf(buffer, sizeof(buffer), "field_11D4: %03d, %03d, %03d", Map.field_11D4.R, Map.field_11D4.G, Map.field_11D4.B);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TextPrintType(TPF_VCR|TPF_NOSHADOW));
}



static void Print_Mouse_Info()
{
    // Format print string.
    char buffer[4096];

    // Get primary surface rect.
    Rect surfrect = PrimarySurface->Get_Rect();
    Rect comprect = CompositeSurface->Get_Rect();

    // Draw background rect.
    int rect_width = 200; // Needed, otherwise the box jitters about due to Coord value change.
    int rect_height = 100;
    Rect boxrect(comprect.X + comprect.Width - /*fontrect.Width*/rect_width, (comprect.Y + comprect.Height) - /*fontrect.Height*/rect_height, /*fontrect.Width + 2*/rect_width, /*fontrect.Height*/rect_height);
    //CompositeSurface->Fill_Rect_14(&boxrect, MAKE_RGB565(0, 0, 0));
    RGBClass rgb{0, 0, 0};
    CompositeSurface->Fill_Rect_Trans(boxrect, rgb, 50);

    // Draw position.
    Point2D drawpos;
    drawpos.X = comprect.X + comprect.Width - 3;// - /*fontrect.Width*/rect_width;
    drawpos.Y = comprect.Y + comprect.Height - /*fontrect.Height*/rect_height + 3;

    Fancy_Text_Print("Mouse",
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y /*= 10*/),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    // Make a space under the title.
    drawpos.Y += 4;

    //
    // Mouse
    //
    Point2D mouse = WWMouse->Get_Mouse_XY();
    //CoordStruct screen_coord = TacticalMap->func_60F660(&mouse);
    //CellStruct screen_cell = Coord_Cell(screen_coord);
    //
    //screen_coord.Z = Map.func_51E130(&screen_coord);
    CoordStruct screen_coord = Get_Cell_Under_Mouse();
    CellStruct screen_cell = Get_Coord_Under_Mouse();

    std::snprintf(buffer, sizeof(buffer), "MousePos: %d,%d", mouse.X, mouse.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "Mouse Coord: %d,%d,%d", screen_coord.X, screen_coord.Y, screen_coord.Z);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);

    std::snprintf(buffer, sizeof(buffer), "Mouse Cell: %d,%d", screen_cell.X, screen_cell.Y);
    Fancy_Text_Print(buffer,
        CompositeSurface,
        &surfrect,
        &Point2D(drawpos.X, drawpos.Y += 12),
        TextColor,
        COLOR_TBLACK,
        TPF_RIGHT|TPF_VCR|TPF_NOSHADOW);
}



void Vinifera_Debug_Overlay()
{
    if (!TextColor) {
        TextColor = ColorScheme::As_Pointer("White");
    }

    // Redraw to erase any artifacts.
    Map.Redraw_Tab();
    Map.Redraw_Sidebar();

    //Print_Current_Object();
    //Print_Current_Techno();
    Print_Current_Foot();
    //Print_Current_Top_Level();
    //Print_Tactical_Info();
    //Print_Mouse_Info();
    //Print_Display_Info();

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
 *  This patch intercepts the end of the rendering process for Tactical.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Tactical_Render_Patch)
{
    GET_REGISTER_STATIC(Tactical *, this_ptr, ebp);

    /**
     *  If the developer mode is active, draw the developer overlay.
     */
    if (Vinifera_DeveloperMode) {
        Tactical_Draw_Debug_Overlay();



        Vinifera_Debug_Overlay();
    }

#ifndef RELEASE
    /**
     *  Draw the version number on screen for non-release builds.
     * 
     *  @note: This must be last in the draw order!
     */
    Vinifera_Draw_Version_Text(CompositeSurface);
#endif

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
