/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TACTICALEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended Tactical class.
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
#include "tacticalext.h"
#include "tactical.h"
#include "wwcrc.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "colorscheme.h"
#include "rgb.h"
#include "wwfont.h"
#include "wwcrc.h"
#include "wwmouse.h"
#include "foot.h"
#include "unit.h"
#include "unittype.h"
#include "session.h"
#include "scenario.h"
#include "ebolt.h"
#include "house.h"
#include "housetype.h"
#include "super.h"
#include "superext.h"
#include "supertype.h"
#include "supertypeext.h"
#include "isotiletype.h"
#include "overlaytype.h"
#include "smudgetype.h"
#include "animtype.h"
#include "tag.h"
#include "tagtype.h"
#include "veinholemonster.h"
#include "tiberium.h"
#include "waypoint.h"
#include "rules.h"
#include "rulesext.h"
#include "cell.h"
#include "iomap.h"
#include "swizzle.h"
#include "drivelocomotion.h"
#include "flylocomotion.h"
#include "vinifera_saveload.h"
#include "extension.h"
#include "asserthandler.h"
#include "debughandler.h"
#include "textprint.h"


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
TacticalExtension::TacticalExtension(const Tactical *this_ptr) :
    GlobalExtensionClass(this_ptr),
    IsInfoTextSet(false),
    InfoTextBuffer(),
    InfoTextPosition(BOTTOM_LEFT),
    InfoTextNotifySound(VOC_NONE),
    InfoTextNotifySoundVolume(1.0f),
    InfoTextStyle(TPF_6PT_GRAD|TPF_DROPSHADOW),
    InfoTextTimer(0),
    DebugOverlayState(DEBUG_STATE_ABSTRACT)
{
    //if (this_ptr) EXT_DEBUG_TRACE("TacticalExtension::TacticalExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
TacticalExtension::TacticalExtension(const NoInitClass &noinit) :
    GlobalExtensionClass(noinit),
    InfoTextTimer(noinit)
{
    //EXT_DEBUG_TRACE("TacticalExtension::TacticalExtension(NoInitClass) - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
TacticalExtension::~TacticalExtension()
{
    //EXT_DEBUG_TRACE("TacticalExtension::~TacticalExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT TacticalExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("TacticalExtension::Load - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = GlobalExtensionClass::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) TacticalExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT TacticalExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("TacticalExtension::Save - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = GlobalExtensionClass::Save(pStm, fClearDirty);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}


/**
 *  Return the raw size of class data for save/load purposes.
 *  
 *  @author: CCHyper
 */
int TacticalExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("TacticalExtension::Size_Of - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void TacticalExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("TacticalExtension::Detach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void TacticalExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("TacticalExtension::Compute_CRC - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Set the information text to be displayed.
 * 
 *  @authors: CCHyper
 */
void TacticalExtension::Set_Info_Text(const char *text)
{
    std::memset(TacticalMapExtension->InfoTextBuffer, 0, sizeof(TacticalMapExtension->InfoTextBuffer));
    std::strncpy(TacticalMapExtension->InfoTextBuffer, text, sizeof(TacticalMapExtension->InfoTextBuffer));
    TacticalMapExtension->InfoTextBuffer[std::strlen(text)-1] = '\0';
}


/**
 *  Draws the developer mode overlay.
 * 
 *  @authors: CCHyper
 */
void TacticalExtension::Draw_Debug_Overlay()
{
    RGBClass rgb_black(0,0,0);
    unsigned color_black = DSurface::RGB_To_Pixel(0, 0, 0);
    ColorScheme *text_color = ColorScheme::As_Pointer("White");

    int padding = 2;

    char buffer[256];
    std::snprintf(buffer, sizeof(buffer),
        "[%s] %3d %3d 0x%08X",
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

    /**
     *  Draw the current frame number.
     */
    std::snprintf(buffer, sizeof(buffer), "%d", Frame);
    GradFont6Ptr->String_Pixel_Rect(buffer, &text_rect);

    fill_rect.Width = text_rect.Width+(padding+1);
    fill_rect.Height = 16;
    fill_rect.X = CompositeSurface->Get_Width()-fill_rect.Width;
    fill_rect.Y = 0;
    CompositeSurface->Fill_Rect(fill_rect, color_black);

    text_rect.X = CompositeSurface->Get_Width();
    text_rect.Y = 0;
    text_rect.Width += padding;
    text_rect.Height += 3;

    Fancy_Text_Print(buffer, CompositeSurface, &CompositeSurface->Get_Rect(),
        &Point2D(text_rect.X, text_rect.Y), text_color, COLOR_TBLACK, TextPrintType(TPF_RIGHT|TPF_6PT_GRAD|TPF_NOSHADOW));
}


#ifndef NDEBUG
/**
 *  Draws the current unit facing number.
 * 
 *  @author: CCHyper
 */
bool TacticalExtension::Debug_Draw_Facings()
{
    if (CurrentObjects.Count() != 1) {
        return false;
    }

    ObjectClass *object = CurrentObjects.Fetch_Head();
    if (object->What_Am_I() != RTTI_UNIT) {
        return false;
    }

    UnitClass *unit = reinterpret_cast<UnitClass *>(object);

    Point3D lept = unit->Class_Of()->Lepton_Dimensions();
    Point3D lept_center = Point3D(lept.X/2, lept.Y/2, lept.Z/2);

    Point3D pix = unit->Class_Of()->Pixel_Dimensions();
    Point3D pixel_center = Point3D(pix.X/2, pix.Y/2, pix.Z/2);

    Coordinate coord = unit->Center_Coord();

    Point2D screen = TacticalMap->func_60F150(coord);

    screen.X -= TacticalMap->field_5C.X;
    screen.Y -= TacticalMap->field_5C.Y;

    screen.X += TacticalRect.X;
    screen.Y += TacticalRect.Y;

    TempSurface->Fill_Rect(TacticalRect, Rect(screen.X, screen.Y, 2, 2), DSurface::RGB_To_Pixel(255,0,0));

    TextPrintType style = TPF_CENTER|TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);

    screen.Y -= font->Get_Char_Height()/2;

    char buffer1[32];
    char buffer2[32];

    std::snprintf(buffer1, sizeof(buffer1), "%d", unit->PrimaryFacing.Current().Get_Dir());
    std::snprintf(buffer2, sizeof(buffer2), "%d", unit->PrimaryFacing.Current().Get_Raw());

    Simple_Text_Print(buffer1, TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), style);

    screen.Y += 10;
    Simple_Text_Print(buffer2, TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), style);

    return true;
}


// bool thick = false; // draw thick cell outlines?
static bool Tactical_Debug_Draw_Current_Cell_Hightlight(CellClass *cell, bool thick = false, RGBClass &color_override = RGBClass(-1,-1,-1))
{
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    unsigned color_yellow = DSurface::RGB_To_Pixel(255, 255, 0);

    unsigned override_color = (color_override.Red != -1 && color_override.Green != -1 && color_override.Blue != -1) ?
                              DSurface::RGB_To_Pixel(color_override.Red, color_override.Green, color_override.Blue) :
                              -1;

    static unsigned r;
    static unsigned g;
    static unsigned b;
    static unsigned w;
    static unsigned x;

    /**
     *  Colors taken from CELLSEL.SHP, each entry matches a cell level.
     */
    static unsigned _CellLevelColors[16];
    static RGBClass _CellLevelColorsRGB[16];
    static bool _onetime = false;

    if (!_onetime) {

        _CellLevelColorsRGB[0] = RGBClass(255, 255, 255);  // 0
        _CellLevelColorsRGB[1] = RGBClass(170, 0, 170);    // 1
        _CellLevelColorsRGB[2] = RGBClass(0, 170, 170);    // 2
        _CellLevelColorsRGB[3] = RGBClass(0, 170, 0);      // 3
        _CellLevelColorsRGB[4] = RGBClass(89, 255, 85);    // 4
        _CellLevelColorsRGB[5] = RGBClass(255, 255, 85);   // 5
        _CellLevelColorsRGB[6] = RGBClass(255, 85, 85);    // 6
        _CellLevelColorsRGB[7] = RGBClass(170, 85, 0);     // 7
        _CellLevelColorsRGB[8] = RGBClass(170, 0, 0);      // 8
        _CellLevelColorsRGB[9] = RGBClass(85, 255, 255);   // 9
        _CellLevelColorsRGB[10] = RGBClass(80, 80, 255);   // 10
        _CellLevelColorsRGB[11] = RGBClass(0, 0, 170);     // 11
        _CellLevelColorsRGB[12] = RGBClass(0, 0, 0);       // 12
        _CellLevelColorsRGB[13] = RGBClass(85, 85, 85);    // 13
        _CellLevelColorsRGB[14] = RGBClass(170, 170, 170); // 14
        _CellLevelColorsRGB[15] = RGBClass(255, 255, 255); // 15

        _CellLevelColors[0] = DSurface::RGB_To_Pixel(255, 255, 255);  // 0
        _CellLevelColors[1] = DSurface::RGB_To_Pixel(170, 0, 170);    // 1
        _CellLevelColors[2] = DSurface::RGB_To_Pixel(0, 170, 170);    // 2
        _CellLevelColors[3] = DSurface::RGB_To_Pixel(0, 170, 0);      // 3
        _CellLevelColors[4] = DSurface::RGB_To_Pixel(89, 255, 85);    // 4
        _CellLevelColors[5] = DSurface::RGB_To_Pixel(255, 255, 85);   // 5
        _CellLevelColors[6] = DSurface::RGB_To_Pixel(255, 85, 85);    // 6
        _CellLevelColors[7] = DSurface::RGB_To_Pixel(170, 85, 0);     // 7
        _CellLevelColors[8] = DSurface::RGB_To_Pixel(170, 0, 0);      // 8
        _CellLevelColors[9] = DSurface::RGB_To_Pixel(85, 255, 255);   // 9
        _CellLevelColors[10] = DSurface::RGB_To_Pixel(80, 80, 255);   // 10
        _CellLevelColors[11] = DSurface::RGB_To_Pixel(0, 0, 170);     // 11
        _CellLevelColors[12] = DSurface::RGB_To_Pixel(0, 0, 0);       // 12
        _CellLevelColors[13] = DSurface::RGB_To_Pixel(85, 85, 85);    // 13
        _CellLevelColors[14] = DSurface::RGB_To_Pixel(170, 170, 170); // 14
        _CellLevelColors[15] = DSurface::RGB_To_Pixel(255, 255, 255); // 15

        r = DSurface::RGB_To_Pixel(255, 0, 0);
        g = DSurface::RGB_To_Pixel(0, 255, 0);
        b = DSurface::RGB_To_Pixel(0, 0, 255);
        w = DSurface::RGB_To_Pixel(255, 255, 255);
        x = DSurface::RGB_To_Pixel(255, 255, 0);

        _onetime = true;
    }

    if (!cell) {
        return false;
    }

    Rect cellrect = cell->Get_Rect();

    /**
     *  Get the center point of the cell.
     */
    Point2D cell_center;
    cell_center.X = cellrect.X + cellrect.Width / 2;
    cell_center.Y = cellrect.Y + cellrect.Height / 2;

    /**
     *  Determine if the cell draw rect is within the viewport.
     */
    Rect intersect = Intersect(cellrect, TacticalRect);
    if (!intersect.Is_Valid()) {
        return false;
    }

    /**
     *  Fetch the highlight color based on cell height.
     */
    unsigned color = _CellLevelColors[cell->Level];

    /**
     *  Override the color if set.
     */
    if (override_color != -1) {
        color = override_color;
    }

    /**
     *  x
     */
    float af = 1.0f;
    float bf = 1.0f;

    /**
     *  x
     */
    unsigned nextcolor = _CellLevelColors[cell->Level + 1];
    RGBClass colorRGB = _CellLevelColorsRGB[cell->Level];
    RGBClass nextcolorRGB = _CellLevelColorsRGB[cell->Level + 1];

    /**
     *  Draw the cell selection.
     */
    switch (cell->Ramp) {

        default:
            break;

        /**
         *  No ramp is a flat tile.
         */
        case RAMP_NONE: // top left, top right, bottom right, bottom left
            if (thick) {
                TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D((CELL_PIXEL_W/2), 0), color);
                TempSurface->Draw_Line(cellrect, Point2D((CELL_PIXEL_W/2), 0), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), color);
                TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, (CELL_PIXEL_W/2)), color);
                TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, (CELL_PIXEL_W/2)), Point2D(0, (CELL_PIXEL_H/2)), color);

                TempSurface->Draw_Line(cellrect, Point2D(1, (CELL_PIXEL_H/2)), Point2D((CELL_PIXEL_W/2), 1), color);
                TempSurface->Draw_Line(cellrect, Point2D((CELL_PIXEL_W/2), 1), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)-1), color);
                TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)-1), Point2D(CELL_PIXEL_H-1, (CELL_PIXEL_W/2)), color);
                TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H-1, (CELL_PIXEL_W/2)), Point2D(1, (CELL_PIXEL_H/2)), color);
            } else {
                TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H / 2)), Point2D((CELL_PIXEL_W / 2), 0), color);
                TempSurface->Draw_Line(cellrect, Point2D((CELL_PIXEL_W / 2), 0), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H / 2)), color);
                TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H / 2)), Point2D(CELL_PIXEL_H, (CELL_PIXEL_W / 2)), color);
                TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, (CELL_PIXEL_W / 2)), Point2D(0, (CELL_PIXEL_H / 2)), color);
            }
            break;

        case RAMP_WEST:
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, 0), color);
            TempSurface->Draw_Line_entry_90(cellrect, Point2D(CELL_PIXEL_H, 0), Point2D(CELL_PIXEL_W, 0), colorRGB, nextcolorRGB, af, bf); // grad
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, 0), Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), nextcolor);
            TempSurface->Draw_Line_entry_90(cellrect, Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), Point2D(0, (CELL_PIXEL_H/2)), colorRGB, nextcolorRGB, af, bf); // grad
            break;

        case RAMP_NORTH:
            TempSurface->Draw_Line(cellrect, Point2D(0, 0), Point2D(CELL_PIXEL_H, 0), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, 0), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, 12), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), Point2D(0, 0), w);
            break;

        case RAMP_EAST:
            TempSurface->Draw_Line(cellrect, Point2D(0, 0), Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, CELL_PIXEL_H), Point2D(0, 0), w);
            break;

        case RAMP_SOUTH:
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, 0), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, 0), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, CELL_PIXEL_H), Point2D(0, (CELL_PIXEL_H/2)), w);
            break;

        case RAMP_CORNER_NW:
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, 0), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, 0), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), Point2D(0, (CELL_PIXEL_H/2)), b);
            break;

        case RAMP_CORNER_NE:
            TempSurface->Draw_Line(cellrect, Point2D(0, 0), Point2D(CELL_PIXEL_H, 0), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, 0), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, CELL_PIXEL_H), Point2D(0, 0), w);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, 0), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), x);
            break;

        case RAMP_CORNER_SE:
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, CELL_PIXEL_H), Point2D(0, (CELL_PIXEL_H/2)), w);
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), x);
            break;

        case RAMP_CORNER_SW:
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, 0), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, 0), Point2D(CELL_PIXEL_W, 0), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, 0), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, CELL_PIXEL_H), Point2D(0, (CELL_PIXEL_H/2)), w);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, 0), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), x);
            break;

        case RAMP_MID_NW:
            TempSurface->Draw_Line(cellrect, Point2D(0, 0), Point2D(CELL_PIXEL_W, 0), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, 0), Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), Point2D(0, 0), b);
            break;

        case RAMP_MID_NE:
            TempSurface->Draw_Line(cellrect, Point2D(0, 0), Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), Point2D(0, 0), w);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), x);
            break;

        case RAMP_MID_SE:
            TempSurface->Draw_Line(cellrect, Point2D(0, 0), Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, 0), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, 0), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, CELL_PIXEL_H), Point2D(0, 0), w);
            TempSurface->Draw_Line(cellrect, Point2D(0, 0), Point2D(CELL_PIXEL_W, 0), x);
            break;

        case RAMP_MID_SW:
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, 0), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, 0), Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), Point2D(0, (CELL_PIXEL_H/2)), w);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), x);
            break;

        case RAMP_STEEP_SE:
            /**
             *  PLACEHOLDER:
             *  This tile is normally only 3 pixels graphically.
             */
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, 0), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, 0), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2) - 1), Point2D(CELL_PIXEL_H, (CELL_PIXEL_H - 1)), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, (CELL_PIXEL_H - 1)), Point2D(0, (CELL_PIXEL_H/2) - 1), w);
            break;

        case RAMP_STEEP_SW:
            TempSurface->Draw_Line(cellrect, Point2D(0, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), Point2D(0, -(CELL_PIXEL_H/2)), w);
            break;

        case RAMP_STEEP_NW:
            TempSurface->Draw_Line(cellrect, Point2D(0, 0), Point2D(CELL_PIXEL_H, 0 - CELL_PIXEL_H), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, 0 - CELL_PIXEL_H), Point2D(CELL_PIXEL_W, 0), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, 0), Point2D(CELL_PIXEL_H, CELL_PIXEL_W - CELL_PIXEL_H), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, CELL_PIXEL_W - CELL_PIXEL_H), Point2D(0, 0), w);
            break;

        case RAMP_STEEP_NE:
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, -(CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), Point2D(0, (CELL_PIXEL_H/2)), w);
            break;

        case RAMP_DOUBLE_UP_SW_NE:
            TempSurface->Draw_Line(cellrect, Point2D(0, 0), Point2D(CELL_PIXEL_W, 0), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, 0), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, CELL_PIXEL_H), Point2D(0, 0), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, 0), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), w);
            break;

        case RAMP_DOUBLE_DOWN_SW_NE:
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), Point2D(0, (CELL_PIXEL_H/2)), b);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, (CELL_PIXEL_H/2)), w);
            break;

        case RAMP_DOUBLE_UP_NW_SE:
            TempSurface->Draw_Line(cellrect, Point2D(0, 0), Point2D(CELL_PIXEL_W, 0), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, 0), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, CELL_PIXEL_H), Point2D(0, 0), b);
            break;

        case RAMP_DOUBLE_DOWN_NW_SE:
            TempSurface->Draw_Line(cellrect, Point2D(0, (CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), r);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, -(CELL_PIXEL_H/2)), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), g);
            TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H/2)), Point2D(0, (CELL_PIXEL_H/2)), b);
            break;

    };

    return true;
}


bool TacticalExtension::Debug_Draw_Current_Cell()
{
    if (Map.IsRubberBand) {
        return false;
    }

    CellClass *cell = nullptr;

    if (Debug_Map) {

        // TODO

    } else {
        Cell mouse_cell = Map.Click_Cell_Calc(Point2D(WWMouse->Get_Mouse_X(), WWMouse->Get_Mouse_Y()));
        cell = &Map[mouse_cell];
    }

    if (!cell) {
        return false;
    }

    return Tactical_Debug_Draw_Current_Cell_Hightlight(cell, true);
}


bool TacticalExtension::Debug_Draw_Occupiers()
{
    unsigned color_white = DSurface::RGB_To_Pixel(255, 255, 255);

    ColorScheme *text_color = ColorScheme::As_Pointer("White");

    // TODO

    return true;
}


bool TacticalExtension::Debug_Draw_CellTags()
{
    unsigned color_dark_blue = DSurface::RGB_To_Pixel(0, 0, 128);
    unsigned color_dark_red = DSurface::RGB_To_Pixel(128, 0, 0);
    unsigned color_white = DSurface::RGB_To_Pixel(255, 255, 255);

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

            } else {

                /**
                *  This is a workaround for objects on the high bridges, which use the alternative occupier ptr.
                */
                ObjectClass *occupier = cell->OccupierPtr;
                if (cell->AltOccupierPtr) {
                    occupier = cell->AltOccupierPtr;
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

    return true;
}


bool TacticalExtension::Debug_Draw_Waypoints()
{
    unsigned color_dark_green = DSurface::RGB_To_Pixel(0, 128, 0);
    unsigned color_white = DSurface::RGB_To_Pixel(255, 255, 255);
    unsigned color_black = DSurface::RGB_To_Pixel(0, 0, 0);

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

    return true;
}


/**
*  Draws the current object mission state.
* 
*  @author: CCHyper
*/
bool TacticalExtension::Debug_Draw_Missions()
{
    if (CurrentObjects.Count() != 1) {
        return false;
    }

    ObjectClass *object = CurrentObjects.Fetch_Head();
    if (!object->Is_Techno()) {
        return false;
    }

    TechnoClass *techno = reinterpret_cast<TechnoClass *>(object);

    Point3D lept = techno->Class_Of()->Lepton_Dimensions();
    Point3D lept_center = Point3D(lept.X/2, lept.Y/2, lept.Z/2);

    Point3D pix = techno->Class_Of()->Pixel_Dimensions();
    Point3D pixel_center = Point3D(pix.X/2, pix.Y/2, pix.Z/2);

    Coordinate coord = techno->Center_Coord();

    Point2D screen = TacticalMap->func_60F150(coord);

    screen.X -= TacticalMap->field_5C.X;
    screen.Y -= TacticalMap->field_5C.Y;

    screen.X += TacticalRect.X;
    screen.Y += TacticalRect.Y;

    TextPrintType style = TPF_CENTER|TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);

    int row_height = font->Get_Char_Height();;

    char buffer1[32];
    char buffer2[32];
    char buffer3[32];

    std::snprintf(buffer1, sizeof(buffer1), "%s", MissionClass::Mission_Name(techno->Mission));
    std::snprintf(buffer2, sizeof(buffer2), "%s", MissionClass::Mission_Name(techno->SuspendedMission));
    std::snprintf(buffer3, sizeof(buffer3), "%s", MissionClass::Mission_Name(techno->MissionQueue));

    int org_screen_x = screen.X;

    if (techno->Mission != MISSION_NONE) {
        screen.X = org_screen_x - (font->String_Pixel_Width(buffer1)/2);
        Simple_Text_Print(buffer1, TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style);
    }

    if (techno->SuspendedMission != MISSION_NONE) {
        screen.Y += row_height;
        screen.X = org_screen_x - (font->String_Pixel_Width(buffer2)/2);
        Simple_Text_Print(buffer2, TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style);
    }

    if (techno->MissionQueue != MISSION_NONE) {
        screen.Y += row_height;
        screen.X = org_screen_x - (font->String_Pixel_Width(buffer3)/2);
        Simple_Text_Print(buffer3, TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style);
    }

    return true;
}


/**
 *  Draws various properties of the cell under the mouse position.
 *
 *  @author: CCHyper
 */
bool TacticalExtension::Debug_Draw_Mouse_Cell_Members()
{
    Cell cell = Map.Click_Cell_Calc(Point2D(WWMouse->Get_Mouse_X(), WWMouse->Get_Mouse_Y()));
    CellClass *cellptr = &Map[cell];
    if (!cellptr) {
        return false;
    }

    Point2D drawpos(TacticalRect.Width-2, 2);

    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    TextPrintType style = TPF_RIGHT|TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);

    int rowheight = font->Get_Char_Height();

    Fancy_Text_Print("Pos: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Pos.As_String());
    drawpos.Y += rowheight;

    Fancy_Text_Print("FoggedObjects: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->FoggedObjects ? cellptr->FoggedObjects->Count() : 0);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_1C: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_1C ? cellptr->field_1C->Pos.As_String() : "<none>"); // on bridge start sections?
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_20: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_20);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Drawer: 0x%08X", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Drawer);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Tile: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, IsometricTileTypeClass::Name_From(cellptr->Tile));
    drawpos.Y += rowheight;

    Fancy_Text_Print("CellTag: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->CellTag ? cellptr->CellTag->Name() : "<none>");
    drawpos.Y += rowheight;

    Fancy_Text_Print("Overlay: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, OverlayTypeClass::Name_From(cellptr->Overlay));
    drawpos.Y += rowheight;

    Fancy_Text_Print("Smudge: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, SmudgeTypeClass::Name_From(cellptr->Smudge));
    drawpos.Y += rowheight;

    Fancy_Text_Print("Passability: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Passability);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Owner: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, HouseTypeClass::Name_From(cellptr->Owner));
    drawpos.Y += rowheight;

    Fancy_Text_Print("InfType: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, HouseTypeClass::Name_From(cellptr->InfType));
    drawpos.Y += rowheight;

    Fancy_Text_Print("AltInfType: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, HouseTypeClass::Name_From(cellptr->AltInfType));
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_48: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_48); // shadow/shroud flag? Changes as shroud gets mapped
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_4C: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_4C); // -- always "-1"
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_50: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_50); // -- always "-1"
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_54: %d,%d,%d,%d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_54.X, cellptr->field_54.Y, cellptr->field_54.Width, cellptr->field_54.Height); // set on cells that are the center of a bridge.
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_64: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_64); // -- no hits
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_68: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_68); // -- no hits     cloaked house flags?
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_6C: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_6C); // foundation clear flags of adj cells (see 0045CA20)
    drawpos.Y += rowheight;

    Fancy_Text_Print("OccupierPtr: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->OccupierPtr ? cellptr->OccupierPtr->Name() : "<none>");
    drawpos.Y += rowheight;

    Fancy_Text_Print("AltOccupierPtr: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->AltOccupierPtr ? cellptr->AltOccupierPtr->Name() : "<none>");
    drawpos.Y += rowheight;

    Fancy_Text_Print("Land: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, Name_From_Land(cellptr->Land));
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_7C: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_7C); // intensity?
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_80: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_80); // ambient or saturation?
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_82: %d,%d,%d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_82.R, cellptr->field_82.G, cellptr->field_82.B); // tint lowest?
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_88: %d,%d,%d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_88.R, cellptr->field_88.G, cellptr->field_88.B); // tint highest?
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_8E: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_8E); // -- always "-1"
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_90: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_90);
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsIceGrowthAllowed: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsIceGrowthAllowed ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("SubTile: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->SubTile);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Level: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Level);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Ramp: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Ramp);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_95: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_95);
    drawpos.Y += rowheight;

    Fancy_Text_Print("OverlayData: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->OverlayData);
    drawpos.Y += rowheight;

    Fancy_Text_Print("SmudgeData: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->SmudgeData);
    drawpos.Y += rowheight;

    Fancy_Text_Print("ShadowIndex: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_98);
    drawpos.Y += rowheight;

    Fancy_Text_Print("FogIndex: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_99);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_9A: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->field_9A);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Flag: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Flag.Composite);
    drawpos.Y += rowheight;

    Fancy_Text_Print("AltFlag: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->AltFlag.Composite);
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsPlot: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsPlot ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsCursorHere: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsCursorHere ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsMapped: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsMapped ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsVisible: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsVisible ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsFogMapped: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsFogMapped ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsFogVisible: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsFogVisible ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsWaypoint: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsWaypoint ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsRadarCursor: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsRadarCursor ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsFlagged: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsFlagged ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsToShroud: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsToShroud ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsToFog: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsToFog ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit2_8: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit2_8 ? "true" : "false"); // is the starting point of a bridge connection? (from start to end?)
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit2_16: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit2_16 ? "true" : "false"); // is covered by bridge overlay?
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit2_32: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit2_32 ? "true" : "false"); // is covered by bridge overlay? also
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit2_64: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit2_64 ? "true" : "false"); // unrepaired/repairable bridge connection.
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit2_128: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit2_128 ? "true" : "false"); // draws on cells that are the length of the bridge body. (low down?)
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit3_1: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit3_1 ? "true" : "false"); // draws on cells that are the length of the bridge body. (high up?)
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit3_2: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit3_2 ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit3_4: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit3_4 ? "true" : "false"); // Is ice edge? Flagged when smoothing required.
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit3_8: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit3_8 ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsOvershadowed: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsOvershadowed ? "true" : "false"); // Casts shadow? or Overcasted with shadow?
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsAnimAttached: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsAnimAttached ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit3_64: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit3_64 ? "true" : "false"); // Toggled by A* pathfinding code.
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit3_128: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit3_128 ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsHorizontalLine: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsHorizontalLine ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsVerticalLine: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->IsVerticalLine ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("Bit4_4: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->Bit4_4 ? "true" : "false");
    drawpos.Y += rowheight;

    return true;
}


bool TacticalExtension::Debug_Draw_Invalid_Tiles()
{
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    unsigned color_white = DSurface::RGB_To_Pixel(255, 255, 255);
    unsigned color_green = DSurface::RGB_To_Pixel(0, 255, 0);
    unsigned rect_color = DSurface::RGB_To_Pixel(128, 0, 0); // Red
    bool draw = true;

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
            cell_center.X = cellrect.X + cellrect.Width / 2;
            cell_center.Y = cellrect.Y + cellrect.Height / 2;

            /**
             *  Is this an invalid tile (fixed up by the map load).
             */
            if (cell->Tile == ISOTILE_NONE) {

                Wstring text = "INVALID";

                /**
                 *  Fetch the text occupy rect.
                 */
                Rect text_rect;
                EditorFont->String_Pixel_Rect(text.Peek_Buffer(), &text_rect);

                /**
                 *  Move into position.
                 */
                text_rect.X = cell_center.X;
                text_rect.Y = (cell_center.Y - text_rect.Height) - 33;
                text_rect.Width += 4;
                text_rect.Height += 2;

                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print(text.Peek_Buffer(),
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT | TPF_FULLSHADOW));
            }

        }
    }

    return true;
}


/**
 *  Draws various properties of the cell isotile under the mouse position.
 *
 *  @author: CCHyper
 */
bool TacticalExtension::Debug_Draw_Mouse_Cell_IsoTile_Members()
{
    Cell cell = Map.Click_Cell_Calc(Point2D(WWMouse->Get_Mouse_X(), WWMouse->Get_Mouse_Y()));
    CellClass *cellptr = &Map[cell];
    if (!cellptr) {
        return false;
    }

    if (cellptr->Tile == ISOTILE_NONE) {
        return false;
    }

    IsometricTileTypeClass *isotile = IsoTileTypes[cellptr->Tile];
    if (!isotile) {
        return false;
    }

    Point2D drawpos(2, 2);

    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    TextPrintType style = TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);

    int rowheight = font->Get_Char_Height();

    Fancy_Text_Print("Type: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, IsometricTileTypeClass::Name_From(isotile->Type));
    drawpos.Y += rowheight;

    Fancy_Text_Print("MarbleMadness: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, IsometricTileTypeClass::Name_From(isotile->MarbleMadness));
    drawpos.Y += rowheight;

    Fancy_Text_Print("NonMarbleMadness: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, IsometricTileTypeClass::Name_From(isotile->NonMarbleMadness));
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_130: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->field_130);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_134.Count: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->field_134.Count());
    drawpos.Y += rowheight;

    Fancy_Text_Print("NextTileTypeInSet: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->NextTileTypeInSet ? isotile->NextTileTypeInSet->Name() : "<none>");
    drawpos.Y += rowheight;

    if (Scen->Theater == THEATER_TEMPERATE && isotile->ToSnowTheater != -1) {
        Fancy_Text_Print("ToSnowTheater: %04d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->ToSnowTheater);
        drawpos.Y += rowheight;
    }

    if (Scen->Theater == THEATER_SNOW && isotile->ToTemperateTheater != -1) {
        Fancy_Text_Print("ToTemperateTheater: %04d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->ToTemperateTheater);
        drawpos.Y += rowheight;
    }

    Fancy_Text_Print("Anim: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, AnimTypeClass::Name_From(isotile->Anim));
    drawpos.Y += rowheight;

    Fancy_Text_Print("XOffset: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->XOffset);
    drawpos.Y += rowheight;

    Fancy_Text_Print("YOffset: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->YOffset);
    drawpos.Y += rowheight;

    Fancy_Text_Print("AttachesTo: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->AttachesTo);
    drawpos.Y += rowheight;

    Fancy_Text_Print("ZAdjust: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->ZAdjust);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_16C: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->field_16C);
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsMorphable: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->IsMorphable ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsShadowCaster: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->IsShadowCaster ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsAllowToPlace: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->IsAllowToPlace ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsRequiredForRMG: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->IsRequiredForRMG ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("TileBlockHeight: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->TileBlockHeight);
    drawpos.Y += rowheight;

    Fancy_Text_Print("TileBlockWidth: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->TileBlockWidth);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_17C: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->field_17C);
    drawpos.Y += rowheight;

    Fancy_Text_Print("TilesInSequence: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->TilesInSequence);
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsFileLoaded: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->IsFileLoaded ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("Filename: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->Filename);
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsAllowBurrowing: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->IsAllowBurrowing ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsAllowTiberium: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->IsAllowTiberium ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_198: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->field_198);
    drawpos.Y += rowheight;

    // image data
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image.Width: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->Get_Tile_Data()->Get_Width());
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image.Height: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->Get_Tile_Data()->Get_Height());
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image.Width: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->Get_Tile_Data()->Get_Image_Width());
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image.Height: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, isotile->Get_Tile_Data()->Get_Image_Height());
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image[%d].IsHasExtraData: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->SubTile, isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->IsHasExtraData);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image[%d].IsHasZData: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->SubTile, isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->IsHasZData);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image[%d].IsRandomised: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->SubTile, isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->IsRandomised);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image[%d].Height: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->SubTile, isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->Height);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image[%d].TileType: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->SubTile, isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->TileType);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image[%d].RampType: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->SubTile, isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->RampType);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image[%d].LowColor: %d,%d,%d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->SubTile,
        isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->LowColor.R,
        isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->LowColor.G,
        isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->LowColor.B);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Image[%d].HighColor: %d,%d,%d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, cellptr->SubTile,
                                                        isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->HighColor.R,
                                                        isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->HighColor.G,
                                                        isotile->Get_Tile_Data()->Get_Tile_Image_Data(cellptr->SubTile)->HighColor.B);
    drawpos.Y += rowheight;

    return true;
}


/**
 *  Draws various properties of the cell isotile under the mouse position.
 *
 *  @author: CCHyper
 */
bool TacticalExtension::Debug_Draw_Tactical_Members()
{
    Point2D drawpos(2, 2);

    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    TextPrintType style = TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);

    int rowheight = font->Get_Char_Height();

    Fancy_Text_Print("ScreenText: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->ScreenText);
    drawpos.Y += rowheight;

    Fancy_Text_Print("LastAIFrame: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->LastAIFrame);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_58: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_58 ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_59: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_59 ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_5C: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_5C.X, This()->field_5C.Y);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_64: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_64.X, This()->field_64.Y);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_6C: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_6C);
    drawpos.Y += rowheight;

    Fancy_Text_Print("ZoomFactor: %f", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->ZoomFactor);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_78: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_78);
    drawpos.Y += rowheight;

    Fancy_Text_Print("MoveFrom: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->MoveFrom.X, This()->MoveFrom.Y);
    drawpos.Y += rowheight;

    Fancy_Text_Print("MoveTo: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->MoveTo.X, This()->MoveTo.Y);
    drawpos.Y += rowheight;

    Fancy_Text_Print("MoveRate: %f", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->MoveRate);
    drawpos.Y += rowheight;

    Fancy_Text_Print("MoveFactor: %f", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->MoveFactor);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_94: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_94);
    drawpos.Y += rowheight;

    if (This()->field_94 > 0) {
        Fancy_Text_Print("field_98[0]: Pos-> %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_98[0]->Pos.X, This()->field_98[0]->Pos.Y);
        drawpos.Y += rowheight;
    }

    Fancy_Text_Print("field_D18: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D18.X, This()->field_D18.Y);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_D20: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D20.X, This()->field_D20.Y);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_D28: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D28.X, This()->field_D28.Y);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_D30: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D30 ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("IsToRedraw: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->IsToRedraw ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_D32: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D32 ? "true" : "false");
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_D34: %d, %d, %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D34.X, This()->field_D34.Y, This()->field_D34.Width, This()->field_D34.Height);
    drawpos.Y += rowheight;

    Fancy_Text_Print("Band: %d, %d, %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->Band.X, This()->Band.Y, This()->Band.Width, This()->Band.Height);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_D54: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D54);
    drawpos.Y += rowheight;

    Fancy_Text_Print("WaypointAnimTimer: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->WaypointAnimTimer());
    drawpos.Y += rowheight;

    drawpos.Y += rowheight;

    Fancy_Text_Print("field_D64: %.5f, %.5f, %.5f, %.5f", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D64[0].X, This()->field_D64[0].Y, This()->field_D64[0].Z, This()->field_D64[0].W);
    drawpos.Y += rowheight;
    Fancy_Text_Print("           %.5f, %.5f, %.5f, %.5f", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D64[1].X, This()->field_D64[1].Y, This()->field_D64[1].Z, This()->field_D64[1].W);
    drawpos.Y += rowheight;
    Fancy_Text_Print("           %.5f, %.5f, %.5f, %.5f", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D64[2].X, This()->field_D64[2].Y, This()->field_D64[2].Z, This()->field_D64[2].W);
    drawpos.Y += rowheight;

    Fancy_Text_Print("field_D94: %.5f, %.5f, %.5f, %.5f", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D94[0].X, This()->field_D94[0].Y, This()->field_D94[0].Z, This()->field_D94[0].W);
    drawpos.Y += rowheight;
    Fancy_Text_Print("           %.5f, %.5f, %.5f, %.5f", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D94[1].X, This()->field_D94[1].Y, This()->field_D94[1].Z, This()->field_D94[1].W);
    drawpos.Y += rowheight;
    Fancy_Text_Print("           %.5f, %.5f, %.5f, %.5f", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, This()->field_D94[2].X, This()->field_D94[2].Y, This()->field_D94[2].Z, This()->field_D94[2].W);
    drawpos.Y += rowheight;

    Point2D mouse = WWMouse->Get_Mouse_XY();
    Cell mouse_click_cell = Map.Click_Cell_Calc(mouse);
    Coordinate mouse_click_coord = Cell_Coord(mouse_click_cell);
    
    Coordinate mouse_coord = This()->Pixel_To_Coord(mouse);
    Cell mouse_cell = Coord_Cell(mouse_coord);

    drawpos.Y += rowheight;
    drawpos.Y += rowheight;

    {
    Fancy_Text_Print("MousePos: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, mouse.X, mouse.Y);
    drawpos.Y += rowheight;
    }

    drawpos.Y += rowheight;

    {
    Point2D ret = This()->func_60F150(mouse_coord); // Coord to Pixel (without validation)?
    Fancy_Text_Print("func_60F150: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, ret.X, ret.Y);
    drawpos.Y += rowheight;
    }

    {
    Point2D ret = This()->func_60F270(mouse_cell); // Cell to pixel (without validation)?
    Fancy_Text_Print("func_60F270: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, ret.X, ret.Y);
    drawpos.Y += rowheight;
    }

    //{
    //Point2D ret = This()->func_60F350(mouse);
    //Fancy_Text_Print("func_60F350: %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, ret.X, ret.Y);
    //drawpos.Y += rowheight;
    //}

    {
    Point2D pixel;
    bool ret = This()->Coord_To_Pixel(mouse_coord, pixel);
    Fancy_Text_Print("Coord_To_Pixel: %d, %d - %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, pixel.X, pixel.Y, ret);
    drawpos.Y += rowheight;
    }

    {
    Coordinate ret = This()->Pixel_To_Coord(mouse);
    Fancy_Text_Print("Pixel_To_Coord: %d, %d - %d, %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, mouse.X, mouse.Y, ret.X, ret.Y, ret.Z);
    drawpos.Y += rowheight;
    }

    {
    Point2D pixel;
    Coordinate ret = This()->func_60F740(pixel);
    Fancy_Text_Print("func_60F740: %d, %d - %d, %d, %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, pixel.X, pixel.Y, ret.X, ret.Y, ret.Z);
    drawpos.Y += rowheight;
    }

    return true;
}


/**
 *  Draws various cell properties.
 *
 *  @author: CCHyper
 */
bool TacticalExtension::Debug_Draw_All_Cell_Info()
{
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    unsigned color_white = DSurface::RGB_To_Pixel(255, 255, 255);
    unsigned rect_color = DSurface::RGB_To_Pixel(128, 0, 0); // Dark Red
    bool draw = true;

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
            cell_center.X = cellrect.X + cellrect.Width / 2;
            cell_center.Y = cellrect.Y + cellrect.Height / 2;

            const char* string = "XX";

            /**
             *  Fetch the text occupy rect.
             */
            Rect text_rect;
            EditorFont->String_Pixel_Rect(string, &text_rect);

            /**
             *  Move into position.
             */
            text_rect.X = cell_center.X;
            text_rect.Y = (cell_center.Y - text_rect.Height) - 33;
            text_rect.Width += 4;
            text_rect.Height += 2;

            /**
             *  Passability
             */
#if 0
            switch (cell->Passability) {
                case PASSABLE_OK:
                    rect_color = DSurface::RGB_To_Pixel(0, 255, 128); // Green
                    draw = false;
                    break;
                case PASSABLE_CRUSH:
                    rect_color = DSurface::RGB_To_Pixel(0, 0, 128); // Dark Blue
                    draw = false;
                    break;
                case PASSABLE_WALL:
                    rect_color = DSurface::RGB_To_Pixel(128, 128, 0); // Yellow
                    draw = false;
                    break;
                case PASSABLE_WATER:
                    rect_color = DSurface::RGB_To_Pixel(128, 0, 0); // Dark Red
                    draw = false;
                    break;
                case PASSABLE_FREE_SPOTS:
                    rect_color = DSurface::RGB_To_Pixel(0, 128, 128); // Dirty something
                    draw = false;
                    break;
                case PASSABLE_NO:
                    rect_color = DSurface::RGB_To_Pixel(64, 0, 0); // Very Dark Red
                    draw = false;
                    break;
                case PASSABLE_OUTSIDE:
                    rect_color = DSurface::RGB_To_Pixel(0, 128, 0); // Dark Green
                    draw = false;
                    break;
            };

            if (draw) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->Passability);
            }
#endif
            /**
             *  field_18
             */
#if 0
            if (cell->field_18) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_18->Count());
            }
#endif
            /**
             *  field_1C
             */
#if 0
            if (cell->field_1C) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_1C);
            }
#endif
            /**
             *  field_20
             */
#if 1
            if (cell->field_20) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_20);
            }
#endif
            /**
             *  field_24
             */
#if 0
            if (cell->field_24) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%s",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), "X");
            }
#endif
            /**
             *  field_44
             */
#if 0
            if (cell->field_44 != HOUSE_NONE) {

                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_44);
            }
#endif
            /**
             *  field_48
             */
#if 0
            if (cell->field_48) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_48);
            }
#endif
            /**
             *  field_4C
             */
#if 0
            if (cell->field_4C) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_4C);
            }
#endif
            /**
             *  field_50
             */
#if 0
            if (cell->field_50) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_50);
            }
#endif
            /**
             *  field_54
             */
#if 0
            if (cell->field_54.Is_Valid()) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%s",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), "X");
            }
#endif
            /**
             *  field_64
             */
#if 0
            if (cell->field_64) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_64);
            }
#endif
            /**
             *  field_68
             */
#if 0
            if (cell->field_68) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_68);
            }
#endif
            /**
             *  field_6C
             */
#if 0
            if (cell->field_6C) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_6C);
            }
#endif
            /**
             *  field_74
             */
#if 0
            if (cell->field_74) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_74);
            }
#endif
            /**
             *  field_7C
             */
#if 0
            if (cell->field_7C != 65536) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_7C);
            }
#endif
            /**
             *  field_80
             */
#if 0
            if (cell->field_80) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_80);
            }
#endif
            /**
             *  field_82
             */
#if 0
#endif
             /**
              *  field_88
              */
#if 0
#endif
              /**
               *  field_8E
               */
#if 1
            if (cell->field_8E != -1) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_8E);
            }
#endif
            /**
             *  field_90
             */
#if 0
            if (cell->field_90 != 255) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_90);
            }
#endif
            /**
             *  field_94
             */
#if 1
            if (cell->Ramp) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->Ramp);
            }
#endif
            /**
             *  field_95
             */
#if 0
            if (cell->field_95) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_95);
            }
#endif
            /**
             *  field_98
             */
#if 0
            if (cell->field_98 != 255) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_98);
            }
#endif
            /**
             *  field_99
             */
#if 0
            if (cell->field_99 != 254) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_99);
            }
#endif
            /**
             *  field_9A
             */
#if 0
            if (cell->field_9A) {
                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print("%d",
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW), cell->field_9A);
            }
#endif
        }
    }

    return true;
}


/**
 *  Draws the bridge markers.
 *
 *  @authors: CCHyper
 */
bool TacticalExtension::Debug_Draw_Bridge_Info()
{
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    unsigned color_white = DSurface::RGB_To_Pixel(255, 255, 255);
    unsigned color_green = DSurface::RGB_To_Pixel(0, 255, 0);
    unsigned rect_color = DSurface::RGB_To_Pixel(128, 0, 0); // Red
    bool draw = true;

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
            cell_center.X = cellrect.X + cellrect.Width / 2;
            cell_center.Y = cellrect.Y + cellrect.Height / 2;

            /**
            *  field_1C
            */
            if (cell->field_1C) {

                Rect nextcellrect = cell->field_1C->Get_Rect();

                /**
                *  Determine if the cell draw rect is within the viewport.
                */
                Rect nextintersect = Intersect(nextcellrect, TacticalRect);
                if (!nextintersect.Is_Valid()) {
                    continue;
                }

                /**
                 *  Get the center point of the cell.
                 */
                Point2D next_cell_center;
                //next_cell_center.X = nextcellrect.X + CELL_PIXEL_W/2;
                //next_cell_center.Y = nextcellrect.Y + CELL_PIXEL_H/2;
                next_cell_center.X = nextcellrect.X + nextcellrect.Width / 2;
                next_cell_center.Y = nextcellrect.Y + nextcellrect.Height / 2;


                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(next_cell_center.X, next_cell_center.Y), color_green);

#if 0
                TempSurface->Draw_Line(cellrect, Point2D(0, CELL_PIXEL_H / 2), Point2D(CELL_PIXEL_H, 0), color_green);
                TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, 0), Point2D(CELL_PIXEL_W, (CELL_PIXEL_H / 2)), color_green);
                TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_W, (CELL_PIXEL_H / 2)), Point2D(CELL_PIXEL_H, CELL_PIXEL_H), color_green);
                TempSurface->Draw_Line(cellrect, Point2D(CELL_PIXEL_H, CELL_PIXEL_H), Point2D(0, (CELL_PIXEL_H / 2)), color_green);
#endif

#if 0
                Wstring text = cell->field_1C->Pos.As_String();

                /**
                 *  Fetch the text occupy rect.
                 */
                Rect text_rect;
                EditorFont->String_Pixel_Rect(text.Peek_Buffer(), &text_rect);

                /**
                 *  Move into position.
                 */
                text_rect.X = cell_center.X;
                text_rect.Y = (cell_center.Y - text_rect.Height) - 33;
                text_rect.Width += 4;
                text_rect.Height += 2;

                /**
                 *  Draw the arrow.
                 */
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X - 3, cell_center.Y - 3), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X + 4, cell_center.Y - 4), color_white);
                TempSurface->Draw_Line(Point2D(cell_center.X, cell_center.Y), Point2D(cell_center.X, text_rect.Y), color_white);

                /**
                 *  Draw the text tooltip.
                 */
                TempSurface->Fill_Rect(text_rect, rect_color);
                TempSurface->Draw_Rect(text_rect, color_white);
                Fancy_Text_Print(text.Peek_Buffer(),
                    TempSurface, &TempSurface->Get_Rect(), &Point2D(text_rect.X + 1, text_rect.Y + 1),
                    text_color, COLOR_TBLACK, TextPrintType(TPF_EFNT|TPF_FULLSHADOW));
#endif
            }

        }
    }

    return true;
}


#if 0
#include "astarpath.h"
bool TacticalExtension::Draw_Debug_AStarPathFinder()
{
    ASSERT(ThisPtr != nullptr);

    TextPrintType style = TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);




    // use these this block to get the screen position of the selected unit.
#if 0
    if (CurrentObjects.Count() != 1) {
        return false;
    }

    ObjectClass *object = CurrentObjects.Fetch_Head();
    if (object->What_Am_I() != RTTI_UNIT) {
        return false;
    }

    FootClass *foot = reinterpret_cast<FootClass *>(object);

    Point3D lept = foot->Class_Of()->Lepton_Dimensions();
    Point3D lept_center = Point3D(lept.X / 2, lept.Y / 2, lept.Z / 2);

    Point3D pix = foot->Class_Of()->Pixel_Dimensions();
    Point3D pixel_center = Point3D(pix.X / 2, pix.Y / 2, pix.Z / 2);

    Coordinate coord = foot->Center_Coord();

    Point2D screen = TacticalMap->func_60F150(coord);

    screen.X -= TacticalMap->field_5C.X;
    screen.Y -= TacticalMap->field_5C.Y;

    screen.X += TacticalRect.X;
    screen.Y += TacticalRect.Y;
#endif





    // top left of tactical display.
    Point2D screen = TacticalRect.Top_Left();

    Fancy_Text_Print(
        "bool_0: %s",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.bool_0 ? "true" : "false");

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "bool_1: %s",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.bool_1 ? "true" : "false");

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "bool_2: %s",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.bool_2 ? "true" : "false");

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "boolchar_3: %s",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.boolchar_3 ? "true" : "false");

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "float_4: %f",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.float_4);

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "initedcount: %d",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.initedcount);

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "Speed: %d",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.Speed);

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "StartCellLevel: %d",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.StartCellLevel);

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "EndCellLevel: %d",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.EndCellLevel);

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "__PathsNeedProcessing: %d",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.__PathsNeedProcessing);

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "dword_6C: %d",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.dword_6C);

    screen.Y += font->Get_Char_Height() + 1;
    Fancy_Text_Print(
        "__OriginCell: %d,%d",
        TempSurface,
        &TacticalRect,
        &screen,
        ColorScheme::As_Pointer("White"),
        COLOR_BLACK,
        style,
        AStarPathFinder.__OriginCell.X, AStarPathFinder.__OriginCell.Y);

    return true;
}
#endif


/**
 *  Draws the overlay for frame step mode.
 * 
 *  @authors: CCHyper
 */
bool TacticalExtension::Debug_Draw_Veinhole_Monster_Info()
{
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    unsigned color_white = DSurface::RGB_To_Pixel(255, 255, 255);

    TextPrintType style = TPF_RIGHT|TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);

    int rowheight = font->Get_Char_Height();

    for (int index = 0; index < VeinholeMonsters.Count(); ++index) {
        VeinholeMonsterClass *vm = VeinholeMonsters[index];
        if (!vm) {
            continue;
        }

        CellClass *cell = &Map[vm->MonsterCell];
        if (!cell) {
            continue;
        }

        Rect cellrect = cell->Get_Rect();

        /**
         *  Determine if the cell draw rect is within the viewport.
         */
        Rect intersect = Intersect(cellrect, TacticalRect);
        if (!intersect.Is_Valid()) {
            continue;
        }

        /**
         *  Get the center point of the cell.
         */
        Point2D cell_center;
        cell_center.X = cellrect.X + cellrect.Width / 2;
        cell_center.Y = cellrect.Y + cellrect.Height / 2;

        Point2D drawpos;
        drawpos.X = cell_center.X;
        drawpos.X = cell_center.Y;

        Fancy_Text_Print("field_4C: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_4C);
        drawpos.Y += rowheight;

        Fancy_Text_Print("field_58: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_58());
        drawpos.Y += rowheight;

        Fancy_Text_Print("VeinholeRateTimer: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_64);
        drawpos.Y += rowheight;

        Fancy_Text_Print("field_68: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_68);
        drawpos.Y += rowheight;

        Fancy_Text_Print("field_6C: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_6C);
        drawpos.Y += rowheight;

        Fancy_Text_Print("field_70: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_70);
        drawpos.Y += rowheight;

        Fancy_Text_Print("field_74: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_74());
        drawpos.Y += rowheight;

        Fancy_Text_Print("field_80: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_80);
        drawpos.Y += rowheight;

        Fancy_Text_Print("field_84: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_84);
        drawpos.Y += rowheight;

        Fancy_Text_Print("field_88: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_88());
        drawpos.Y += rowheight;

        Fancy_Text_Print("CenterCell: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->MonsterCell.As_String());
        drawpos.Y += rowheight;

        Fancy_Text_Print("ShapeFrame: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->ShapeFrame);
        drawpos.Y += rowheight;

        Fancy_Text_Print("IsToShrink: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->IsToShrink ? "true" : "false");
        drawpos.Y += rowheight;

        Fancy_Text_Print("IsToPuffGas: %s", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->IsToPuffGas ? "true" : "false");
        drawpos.Y += rowheight;

        Fancy_Text_Print("VeinCellCount: %d", TempSurface, &TacticalRect, &drawpos, text_color, COLOR_TBLACK, style, vm->field_A0);
        drawpos.Y += rowheight;

    }

    return true;
}



bool TacticalExtension::Debug_Draw_Tiberium_Nodes()
{
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    unsigned color_white = DSurface::RGB_To_Pixel(255, 255, 255);
    unsigned color_black = DSurface::RGB_To_Pixel(0, 0, 0);
    RGBClass rgb_orange = RGBClass(255, 64, 0);
    RGBClass rgb_white = RGBClass(255, 255, 255);

    TextPrintType style = TPF_RIGHT|TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);

    int rowheight = font->Get_Char_Height();

    for (int index = 0; index < Tiberiums.Count(); ++index) {
        TiberiumClass *tib = Tiberiums[index];
        if (!tib) {
            continue;
        }

        // Growth
        for (int i = 0; i < tib->GrowthCellQueue->Get_Count(); ++i) {
            PriorityQueueClassNode<Cell, float> *node = (*tib->GrowthCellQueue)[i];

            if (!node) {
                continue;
            }

            CellClass *nodecell = &Map[node->Get_Element()];
            if (!nodecell) {
                continue;
            }

            Tactical_Debug_Draw_Current_Cell_Hightlight(nodecell, true, rgb_white);
        }

        // Spread
        for (int i = 0; i < tib->SpreadCellQueue->Get_Count(); ++i) {
            PriorityQueueClassNode<Cell, float> *node = (*tib->SpreadCellQueue)[i];

            if (!node) {
                continue;
            }

            CellClass *nodecell = &Map[node->Get_Element()];
            if (!nodecell) {
                continue;
            }

            Tactical_Debug_Draw_Current_Cell_Hightlight(nodecell, true, rgb_orange);
        }

    }


    return true;
}


bool TacticalExtension::Debug_Draw_Veinhole_Monster_Nodes()
{
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    unsigned color_white = DSurface::RGB_To_Pixel(255, 255, 255);
    unsigned color_black = DSurface::RGB_To_Pixel(0, 0, 0);
    bool thick = true; // draw thick cell outlines?

    TextPrintType style = TPF_RIGHT|TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);

    int rowheight = font->Get_Char_Height();

    for (int index = 0; index < VeinholeMonsters.Count(); ++index) {
        VeinholeMonsterClass *vm = VeinholeMonsters[index];
        if (!vm) {
            continue;
        }

        CellClass *vmcell = &Map[vm->MonsterCell];
        if (!vmcell) {
            continue;
        }

        Rect vmcellrect = vmcell->Get_Rect();

        /**
         *  Determine if the cell draw rect is within the viewport.
         */
        Rect intersect = Intersect(vmcellrect, TacticalRect);
        if (!intersect.Is_Valid()) {
            continue;
        }

        for (int i = 0; i < vm->VeinCellQueue->Get_Count(); ++i) {
            PriorityQueueClassNode<Cell, float> *node = (*vm->VeinCellQueue)[i];

            if (!node) {
                continue;
            }

            CellClass *nodecell = &Map[node->Get_Element()];
            if (!nodecell) {
                continue;
            }

            Tactical_Debug_Draw_Current_Cell_Hightlight(nodecell);

        }

    }


    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool TacticalExtension::Debug_Draw_Cell_Debug_Break()
{
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    unsigned color_white = DSurface::RGB_To_Pixel(255, 255, 255);
    unsigned rect_color = DSurface::RGB_To_Pixel(128, 0, 0); // Dark Red
    bool draw = true;

    /**
     *  Reset the cell iterator.
     */
    Map.Iterator_Reset();

    /**
     *  Iterate over all the map cells.
     */
    for (CellClass *cell = Map.Iterator_Next_Cell(); cell != nullptr; cell = Map.Iterator_Next_Cell()) {

        if (cell) {
            if (cell->Bit3_1) {
                draw = true; // breakpoint me
            }
            if (cell->Bit3_2) {
                draw = true; // breakpoint me
            }
            if (cell->Bit3_4) {
                draw = true; // breakpoint me
            }
            if (cell->Bit3_8) {
                draw = true; // breakpoint me
            }
            if (cell->Bit3_64) {
                draw = true; // breakpoint me
            }
            if (cell->Bit3_128) {
                draw = true; // breakpoint me
            }
            if (cell->Bit4_4) {
                draw = true; // breakpoint me
            }
        }

    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool TacticalExtension::Debug_Draw_Voxel_Caches()
{
    if (CurrentObjects.Count() != 1) {
        return false;
    }

    ObjectClass *object = CurrentObjects.Fetch_Head();
    if (!object->Is_Techno()) {
        return false;
    }

    TechnoClass *techno = reinterpret_cast<TechnoClass *>(object);

    TechnoTypeClass *techtype = techno->Techno_Type_Class();
    if (!techtype) {
        return false;
    }

    if (techno->What_Am_I() != RTTI_UNIT && techno->What_Am_I() != RTTI_BUILDING && techno->What_Am_I() != RTTI_AIRCRAFT) {
        return false;
    }

    Point3D lept = techno->Class_Of()->Lepton_Dimensions();
    Point3D lept_center = Point3D(lept.X/2, lept.Y/2, lept.Z/2);

    Point3D pix = techno->Class_Of()->Pixel_Dimensions();
    Point3D pixel_center = Point3D(pix.X/2, pix.Y/2, pix.Z/2);

    Coordinate coord = techno->Center_Coord();

    Point2D screen = TacticalMap->func_60F150(coord);

    screen.X -= TacticalMap->field_5C.X;
    screen.Y -= TacticalMap->field_5C.Y;

    screen.X += TacticalRect.X;
    screen.Y += TacticalRect.Y;

    TextPrintType style = TPF_CENTER|TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);

    int row_height = font->Get_Char_Height();
    int org_screen_x = screen.X;

    //if (techno->What_Am_I() == RTTI_UNIT || techno->What_Am_I() == RTTI_AIRCRAFT) {
    //    FootClass *foot = reinterpret_cast<FootClass *>(techno);
    //
    //    int draw_key = -1;
    //    int shadow_key = -1;
    //
    //    foot->Locomotor_Ptr()->Draw_Matrix(&draw_key);
    //    foot->Locomotor_Ptr()->Shadow_Matrix(&shadow_key);
    //
    //    Fancy_Text_Print("Draw_Key: %d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, draw_key);
    //
    //    screen.Y += row_height;
    //    Fancy_Text_Print("Shadow_Key: %d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, shadow_key);
    //
    //    screen.Y += row_height;
    //}

    //Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, techtype->field_D4.Count());
    //
    //screen.Y += row_height;
    //Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, techtype->field_E8.Count());
    //
    //screen.Y += row_height;
    //Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, techtype->field_FC.Count());
    //
    //screen.Y += row_height;
    //Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, techtype->field_110.Count());






    screen.Y += row_height;





    //if (techtype->field_D4.Count()) {



        struct VoxelKeyStruct
        {
            int Get_Index_ID() const
            {
                return *reinterpret_cast<const int *>(this);
            }

            bool Is_Valid_Key() const
            {
                return *reinterpret_cast<const int *>(this) != -1;
            }

            unsigned RotatingCW:1;  // 1
            unsigned RotatingCCW:1; // 2
            unsigned Pitched:1;     // 4
            unsigned field_8:1;     // 8
            unsigned field_16:1;    // 16

            unsigned Facing:5;      // 32
            unsigned Ramp:3;        // 32

            unsigned field_64:8;    // 64
            unsigned field_128:8;   // 128
        };



        struct VoxelCacheData
        {
            short X;
            short Y;
            short Width;
            short Height;
            void * DataPtr;
        };

        struct StaticBufferClass
        {
            int Size;
            int BufferPtr;
            VoxelCacheData * CurrentBufferPtr;
        };

        struct NodeElement
        {
            VoxelKeyStruct ID;
            VoxelCacheData *Data;
        };




        //StaticBufferClass *field_D4_Value = (StaticBufferClass *)techtype->field_D4[0];
        //VoxelCacheData *field_D4_Value = (VoxelCacheData *)techtype->field_D4[0];





        NodeElement *main_node = (NodeElement *)techtype->field_D4.Get_Archive();

        if (main_node != nullptr) {

            VoxelCacheData *vc = main_node->Data;
            VoxelKeyStruct key = main_node->ID;

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->X);

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Y);

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Width);

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Height);

            screen.Y += row_height;
            Fancy_Text_Print("0x%0", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->DataPtr);

            screen.Y += row_height;

            screen.Y += row_height;
            Fancy_Text_Print("RotatingCW: 0x%X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.RotatingCW);

            screen.Y += row_height;
            Fancy_Text_Print("RotatingCCW: 0x%X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.RotatingCCW);

            screen.Y += row_height;
            Fancy_Text_Print("Pitched: 0x%X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.Pitched);

            screen.Y += row_height;
            Fancy_Text_Print("field_8: 0x%X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.field_8);

            screen.Y += row_height;
            Fancy_Text_Print("field_16: 0x%X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.field_16);

            screen.Y += row_height;
            Fancy_Text_Print("Facing: 0x%X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.Facing);

            screen.Y += row_height;
            Fancy_Text_Print("Ramp: 0x%X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.Ramp);

            screen.Y += row_height;
            Fancy_Text_Print("field_64: 0x%X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.field_64);

            screen.Y += row_height;
            Fancy_Text_Print("field_128: 0x%X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.field_128);

        }




        #if 0

        NodeElement *aux_node = (NodeElement *)techtype->field_E8.Get_Archive();

        if (aux_node != nullptr) {

            screen.Y += row_height;

            VoxelCacheData *vc = aux_node->Data;
            VoxelKeyStruct key = aux_node->ID;

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->X);

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Y);

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Width);

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Height);

            screen.Y += row_height;
            Fancy_Text_Print("0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->DataPtr);

            screen.Y += row_height;
            Fancy_Text_Print("Facing: 0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.Facing);

            screen.Y += row_height;
            Fancy_Text_Print("Turret: 0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.Turret);

            screen.Y += row_height;
            Fancy_Text_Print("Frame: 0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.Frame);

        }




        NodeElement *shadow_node = (NodeElement *)techtype->field_FC.Get_Archive();

        if (shadow_node != nullptr) {

            screen.Y += row_height;

            VoxelCacheData *vc = shadow_node->Data;
            VoxelKeyStruct key = shadow_node->ID;

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->X);

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Y);

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Width);

            screen.Y += row_height;
            Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Height);

            screen.Y += row_height;
            Fancy_Text_Print("0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->DataPtr);

            screen.Y += row_height;
            Fancy_Text_Print("Facing: 0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.Facing);

            screen.Y += row_height;
            Fancy_Text_Print("Turret: 0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.Turret);

            screen.Y += row_height;
            Fancy_Text_Print("Frame: 0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, key.Frame);

        }


        #endif


            //screen.Y += row_height;
            //Fancy_Text_Print("Key: 0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, field_D4_Value->Size);

            //screen.Y += row_height;
            //Fancy_Text_Print("0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, field_D4_Value->Size);
            //
            //screen.Y += row_height;
            //Fancy_Text_Print("0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, field_D4_Value->BufferPtr);
            //
            //screen.Y += row_height;
            //Fancy_Text_Print("0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, field_D4_Value->CurrentBufferPtr);
            //
            //screen.Y += row_height;
            //Fancy_Text_Print("%d : %d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, field_D4_Value->CurrentBufferPtr->width, field_D4_Value->CurrentBufferPtr->height);
            //
            //screen.Y += row_height;
            //Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->X);
            //
            //screen.Y += row_height;
            //Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Y);
            //
            //screen.Y += row_height;
            //Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Width);
            //
            //screen.Y += row_height;
            //Fancy_Text_Print("%d", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->Height);
            //
            //screen.Y += row_height;
            //Fancy_Text_Print("0x%08X", TempSurface, &TacticalRect, &screen, ColorScheme::As_Pointer("White"), COLOR_BLACK, style, vc->DataPtr);

    //}




    return true;
}


#endif


/**
 *  Draws the overlay for frame step mode.
 * 
 *  @authors: CCHyper
 */
void TacticalExtension::Draw_FrameStep_Overlay()
{
    RGBClass rgb_black(0,0,0);
    unsigned color_black = DSurface::RGB_To_Pixel(0, 0, 0);
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


/**
 *  Draw the overlay information text if set.
 * 
 *  @author: CCHyper
 */
void TacticalExtension::Draw_Information_Text()
{
    if (!IsInfoTextSet) {
        return;
    }

    RGBClass rgb_black(0,0,0);
    unsigned color_black = DSurface::RGB_To_Pixel(0, 0, 0);
    ColorScheme *text_color = ColorScheme::As_Pointer("White");

    int padding = 2;

    const char *text = InfoTextBuffer;

    /**
     * Fetch the text occupy area.
     */
    Rect text_rect;
    GradFont6Ptr->String_Pixel_Rect(text, &text_rect);

    Rect fill_rect;

    TextPrintType style = InfoTextStyle;
    int pos_x = 0;
    int pos_y = 0;

    switch (InfoTextPosition) {

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
 *  For drawing any new post-effects/systems.
 * 
 *  @authors: CCHyper
 */
void TacticalExtension::Render_Post()
{
    //EXT_DEBUG_TRACE("TacticalExtension::Render_Post - 0x%08X\n", (uintptr_t)(This()));

    /**
     *  Draw any new post effects here.
     */
    //DEV_DEBUG_INFO("Before EBoltClass::Draw_All\n");
    EBoltClass::Draw_All();
    //DEV_DEBUG_INFO("After EBoltClass::Draw_All\n");

    /**
     *  Draw any overlay text.
     */
    Draw_Super_Timers();
}


/**
 *  Prints a single super weapon timer to the tactical screen.
 * 
 *  @authors: CCHyper
 */
void TacticalExtension::Super_Draw_Timer(int row_index, ColorScheme *color, int time, const char *name, unsigned long *flash_time, bool *flash_state)
{
    static WWFontClass *_font = nullptr;

    TextPrintType style = TPF_8POINT|TPF_RIGHT|TPF_NOSHADOW|TPF_METAL12|TPF_SOLIDBLACK_BG;

    if (!_font) {
        _font = Font_Ptr(style);
    }

    char fullbuff[128];
    char namebuff[128];
    char timerbuff[128];
    int text_width = -1;
    int flash_delay = 500; // was 1000
    bool to_flash = false;
    unsigned color_black = DSurface::RGB_To_Pixel(0, 0, 0);
    RGBClass rgb_black(0, 0, 0);
    ColorScheme *white_color = ColorScheme::As_Pointer("White", 1);
    int background_tint = 50;

    long hours = (time / 60 / 60);
    long seconds = (time % 60);
    long minutes = (time / 60 % 60);

    if (hours) {
        std::snprintf(fullbuff, sizeof(fullbuff), "%s %d:%02d:%02d", name, hours, minutes, seconds);
        std::snprintf(namebuff, sizeof(namebuff), "%s", name);
        std::snprintf(timerbuff, sizeof(timerbuff), "%d:%02d:%02d", hours, minutes, seconds);
    } else {
        std::snprintf(fullbuff, sizeof(fullbuff), "%s %02d:%02d", name, minutes, seconds);
        std::snprintf(namebuff, sizeof(namebuff), "%s", name);
        std::snprintf(timerbuff, sizeof(timerbuff), "%02d:%02d", minutes, seconds);
    }

    /**
     *  Is it time to flash
     */
    if (!time) {
        if (flash_time && flash_state) {
            if (timeGetTime() >= *flash_time) {
                *flash_time = timeGetTime() + flash_delay;
                *flash_state = !*flash_state;
            }
            to_flash = *flash_state;
        }
    }

    Rect name_rect;
    _font->String_Pixel_Rect(namebuff, &name_rect);

    Rect timer_rect;
    _font->String_Pixel_Rect(timerbuff, &timer_rect);

    int font_width = _font->Get_Font_Width();
    int font_height = _font->Get_Font_Height();

    int y_pos = TacticalRect.Height - (row_index * (font_height + 2)) + 3;

    Point2D timer_point;
    timer_point.X = TacticalRect.Width - 4;
    timer_point.Y = y_pos;

    int x_offset = hours ? 56 : 38; // timer_rect.Width

    Point2D name_point;
    name_point.X = TacticalRect.Width - x_offset - 3;
    name_point.Y = y_pos;

    Rect fill_rect;
    fill_rect.X = TacticalRect.Width - (x_offset + name_rect.Width) - 4;
    fill_rect.Y = y_pos-1;
    fill_rect.Width = x_offset + name_rect.Width + 2;
    fill_rect.Height = timer_rect.Height + 2;

    //CompositeSurface->Fill_Rect(CompositeSurface->Get_Rect(), fill_rect, color_black);
    CompositeSurface->Fill_Rect_Trans(fill_rect, rgb_black, background_tint);

    Fancy_Text_Print(timerbuff, CompositeSurface, &CompositeSurface->Get_Rect(), 
        &timer_point, to_flash ? white_color : color, COLOR_TBLACK, style);

    Fancy_Text_Print(namebuff, CompositeSurface, &CompositeSurface->Get_Rect(), 
        &name_point, color, COLOR_TBLACK, style);
}


/**
 *  Draws super weapon timers to the tactical screen.
 * 
 *  @authors: CCHyper
 */
void TacticalExtension::Draw_Super_Timers()
{
    //EXT_DEBUG_TRACE("TacticalExtension::Draw_Super_Timers - 0x%08X\n", (uintptr_t)(This()));

    /**
     *  Super weapon timers are for multiplayer only.
     */
#if 0
    if (Session.Type == GAME_NORMAL) {
        return;
    }
#endif

    /**
     *  Does the game rules state that the super weapon timers should be shown?
     */
    if (!RuleExtension->IsShowSuperWeaponTimers) {
        return;
    }

    /**
     *  Has the user toggled the visibility of the super weapon timers?
     */
    if (!Vinifera_ShowSuperWeaponTimers) {
        return;
    }

    /**
     *  Non-release builds print the version information to the tactical view
     *  so we need to adjust the timers to print above this text.
     */
#ifdef RELEASE
    int row_index = 0;
#else
    int row_index = 3;
#endif

    /**
     *  Iterate over all active super weapons and print their recharge timers.
     */
    for (int i = 0; i < Supers.Count(); ++i) {

        SuperClass *super = Supers[i];
        SuperClassExtension *superext = Extension::Fetch<SuperClassExtension>(super);
        SuperWeaponTypeClassExtension *supertypeext = Extension::Fetch<SuperWeaponTypeClassExtension>(super->Class);

        /**
         *  Should we show the recharge timer for this super?
         */
        if (!supertypeext->IsShowTimer) {
            continue;
        }

        if (super->House->Class->IsMultiplayPassive) {
            continue;
        }

        /**
         *  Skip supers that are disabled.
         */
        if (!super->IsPresent) {
            continue;
        }

        if (super->Control.Value() != super->Class->RechargeTime) {

            Super_Draw_Timer(
                row_index++,
                ColorSchemes[super->House->RemapColor],
                super->Control.Value() / TICKS_PER_SECOND,
                super->Class->FullName,
                &superext->FlashTimeEnd,
                &superext->TimerFlashState
            );
        }

    }
}


/**
 *  x
 */
static void Tactical_Debug_Draw_Abstract(ObjectClass *object)
{
    TextPrintType style = TPF_6PT_GRAD|TPF_NOSHADOW;
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    Point2D drawpos = TacticalRect.Top_Left();

    drawpos.X += 2;
    drawpos.Y += 2;

    Fancy_Text_Print("this: 0x%08X",
        CompositeSurface,
        &CompositeSurface->Get_Rect(),
        &Point2D(drawpos.X, drawpos.Y),
        text_color,
        COLOR_BLACK,
        style,
        uintptr_t(object));

    Fancy_Text_Print("ID: %d",
        CompositeSurface,
        &CompositeSurface->Get_Rect(),
        &Point2D(drawpos.X, drawpos.Y += 14),
        text_color,
        COLOR_BLACK,
        style,
        object->ID);

    Fancy_Text_Print("HeapID: %d",
        CompositeSurface,
        &CompositeSurface->Get_Rect(),
        &Point2D(drawpos.X, drawpos.Y += 14),
        text_color,
        COLOR_BLACK,
        style,
        object->Get_Heap_ID());

    Fancy_Text_Print("Dirty: %s",
        CompositeSurface,
        &CompositeSurface->Get_Rect(),
        &Point2D(drawpos.X, drawpos.Y += 14),
        text_color,
        COLOR_BLACK,
        style,
        object->Dirty ? "true" : "false");

    Fancy_Text_Print("WhatAmI: %s",
        CompositeSurface,
        &CompositeSurface->Get_Rect(),
        &Point2D(drawpos.X, drawpos.Y += 14),
        text_color,
        COLOR_BLACK,
        style,
        Name_From_RTTI(RTTIType(object->What_Am_I())));
}

/**
 *  x
 */
static void Tactical_Debug_Draw_Object(ObjectClass *object)
{
    if (!object->Is_Techno()) {
        return;
    }

    TextPrintType style = TPF_6PT_GRAD|TPF_NOSHADOW;
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    Point2D drawpos = TacticalRect.Top_Left();

    drawpos.X += 2;
    drawpos.Y += 2;
}

/**
 *  x
 */
static void Tactical_Debug_Draw_Techno(ObjectClass *object)
{
    if (!object->Is_Techno()) {
        return;
    }

    TextPrintType style = TPF_6PT_GRAD|TPF_NOSHADOW;
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    Point2D drawpos = TacticalRect.Top_Left();

    drawpos.X += 2;
    drawpos.Y += 2;
}

/**
 *  x
 */
static void Tactical_Debug_Draw_Mission(ObjectClass *object)
{
    if (!object->Is_Techno()) {
        return;
    }

    TextPrintType style = TPF_6PT_GRAD|TPF_NOSHADOW;
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    Point2D drawpos = TacticalRect.Top_Left();

    drawpos.X += 2;
    drawpos.Y += 2;
}

/**
 *  x
 */
static void Tactical_Debug_Draw_Radio(ObjectClass *object)
{
    if (!object->Is_Techno()) {
        return;
    }

    RadioClass *radio = reinterpret_cast<RadioClass *>(object);

    TextPrintType style = TPF_6PT_GRAD|TPF_NOSHADOW;
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    Point2D drawpos = TacticalRect.Top_Left();

    drawpos.X += 2;
    drawpos.Y += 2;

    Fancy_Text_Print("In_Radio_Contact: %s",
        CompositeSurface,
        &CompositeSurface->Get_Rect(),
        &Point2D(drawpos.X, drawpos.Y),
        text_color,
        COLOR_BLACK,
        style,
        radio->In_Radio_Contact() ? "true" : "false");

    Fancy_Text_Print("Contact_With_Whom: %s",
        CompositeSurface,
        &CompositeSurface->Get_Rect(),
        &Point2D(drawpos.X, drawpos.Y += 14),
        text_color,
        COLOR_BLACK,
        style,
        radio->Contact_With_Whom() ? radio->Contact_With_Whom()->Name() : "<nothing>");

    for (int i = 0; i < ARRAYSIZE(RadioClass::Old); ++i) {
        Fancy_Text_Print("Old[%d]: \"%s\"",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            i,
            Name_From_RadioMessage(radio->Old[i]));
    }
}

/**
 *  x
 */
static void Tactical_Debug_Draw_Foot(ObjectClass *object)
{
    if (!object->Is_Techno()) {
        return;
    }

    TextPrintType style = TPF_6PT_GRAD|TPF_NOSHADOW;
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    Point2D drawpos = TacticalRect.Top_Left();

    drawpos.X += 2;
    drawpos.Y += 2;
}

/**
 *  x
 */
static void Tactical_Debug_Draw_Final(ObjectClass *object)
{
    if (!object->Is_Techno()) {
        return;
    }

    TextPrintType style = TPF_6PT_GRAD|TPF_NOSHADOW;
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    Point2D drawpos = TacticalRect.Top_Left();

    drawpos.X += 2;
    drawpos.Y += 2;
}

/**
 *  x
 */
static void Tactical_Debug_Draw_Locomotion(ObjectClass *object)
{
    if (!object->Is_Foot()) {
        return;
    }

    FootClass *foot = reinterpret_cast<FootClass *>(object);

    //
    // Fetch IPersist pointer to allow ILocomotion interface up-casting.
    //
    IPersistPtr loco(foot->Locomotor_Ptr());

    // This would have been inlined within WWComPtr perhaps? there should not be access to _com_issue_error outside of com_ptr.
    CLSID clsid;
    int hr = loco->GetClassID(&clsid);
    if (FAILED(hr)) {
        _com_issue_error(hr);
        return;
    }

    TextPrintType style = TPF_6PT_GRAD|TPF_NOSHADOW;
    ColorScheme *text_color = ColorScheme::As_Pointer("White");
    Point2D drawpos = TacticalRect.Top_Left();

    drawpos.X += 2;
    drawpos.Y += 2;

    //
    // DriveLocomotionClass
    //
    if (clsid == __uuidof(DriveLocomotionClass)) {
        Fancy_Text_Print("Locomotion: Drive",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y),
            text_color,
            COLOR_BLACK,
            style);

        DriveLocomotionClass *driveloco = dynamic_cast<DriveLocomotionClass *>(loco.GetInterfacePtr());

        Fancy_Text_Print("Is_Moving: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->Is_Moving() ? "true" : "false");

        Fancy_Text_Print("Is_Moving_Now: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->Is_Moving_Now() ? "true" : "false");

        Fancy_Text_Print("CurrentRamp: %d",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 16),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->CurrentRamp);

        Fancy_Text_Print("PreviousRamp: %d",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->PreviousRamp);

        Fancy_Text_Print("field_20(timer): %f",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->RampTransitionTimer());

        Fancy_Text_Print("DestinationCoord: %d,%d,%d",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->DestinationCoord.X, driveloco->DestinationCoord.Y, driveloco->DestinationCoord.Z);

        Fancy_Text_Print("HeadToCoord: %d,%d,%d",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->HeadToCoord.X, driveloco->HeadToCoord.Y, driveloco->HeadToCoord.Z);

        Fancy_Text_Print("SpeedAccum: %d",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->SpeedAccum);

        Fancy_Text_Print("field_50: %f",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->field_50);

        Fancy_Text_Print("TrackNumber: %d",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->TrackNumber);

        Fancy_Text_Print("TrackIndex: %d",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->TrackIndex);

        Fancy_Text_Print("IsOnShortTrack: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->IsOnShortTrack ? "true" : "false");

        Fancy_Text_Print("IsTurretLockedDown: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->IsTurretLockedDown ? "true" : "false");

        Fancy_Text_Print("IsRotating: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->IsRotating ? "true" : "false");

        Fancy_Text_Print("IsDriving: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->IsDriving ? "true" : "false");

        Fancy_Text_Print("IsRocking: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->IsRocking ? "true" : "false");

        Fancy_Text_Print("IsUnlocked: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->IsUnlocked ? "true" : "false");

        Fancy_Text_Print("PiggybackLocomotor: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            driveloco->Piggybacker ? "(assigned)" : "(null)");

    //
    // FlyLocomotionClass
    //
    } else if (clsid == __uuidof(FlyLocomotionClass)) {
        Fancy_Text_Print("Locomotion: Fly",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 22),
            text_color,
            COLOR_BLACK,
            style);

        FlyLocomotionClass *flyloco = dynamic_cast<FlyLocomotionClass *>(loco.GetInterfacePtr());

        Fancy_Text_Print("Is_Moving: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->Is_Moving() ? "true" : "false");

        Fancy_Text_Print("Is_Moving_Now: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->Is_Moving_Now() ? "true" : "false");

        Fancy_Text_Print("IsMoving: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->IsMoving ? "true" : "false");

        Fancy_Text_Print("IsTakingOff: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->IsTakingOff ? "true" : "false");

        Fancy_Text_Print("IsLanding: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->IsLanding ? "true" : "false");

        Fancy_Text_Print("WasLanding: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->WasLanding ? "true" : "false");

        Fancy_Text_Print("field_4B: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->field_4B ? "true" : "false");

        Fancy_Text_Print("IsElevating: %s",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->IsElevating ? "true" : "false");

        Fancy_Text_Print("FlightLevel: %d",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->FlightLevel);

        Fancy_Text_Print("TargetSpeed: %f",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->TargetSpeed);

        Fancy_Text_Print("CurrentSpeed: %f",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->CurrentSpeed);

        Fancy_Text_Print("field_4C: %d",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->field_4C);

        Fancy_Text_Print("field_50: %d",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style,
            flyloco->field_50);

    //
    // Unknown.
    //
    } else {
        Fancy_Text_Print("Locomotion: Unknown!",
            CompositeSurface,
            &CompositeSurface->Get_Rect(),
            &Point2D(drawpos.X, drawpos.Y += 14),
            text_color,
            COLOR_BLACK,
            style);
    }
}


/**
 *  Draws information and state about the currently selected unit.
 *
 *  @author: CCHyper
 */
bool TacticalExtension::Debug_Draw_Selected_Object_State()
{
    if (CurrentObjects.Count() != 1) {
        return false;
    }

    ObjectClass *object = CurrentObjects.Fetch_Head();

    Point3D lept = object->Class_Of()->Lepton_Dimensions();
    Point3D lept_center = Point3D(lept.X/2, lept.Y/2, lept.Z/2);

    Point3D pix = object->Class_Of()->Pixel_Dimensions();
    Point3D pixel_center = Point3D(pix.X/2, pix.Y/2, pix.Z/2);

    Coordinate coord = object->Center_Coord();

    switch (DebugOverlayState) {
        case DEBUG_STATE_ABSTRACT:
            Tactical_Debug_Draw_Abstract(object);
            break;
        case DEBUG_STATE_OBJECT:
            Tactical_Debug_Draw_Object(object);
            break;
        case DEBUG_STATE_TECHNO:
            Tactical_Debug_Draw_Techno(object);
            break;
        case DEBUG_STATE_MISSION:
            Tactical_Debug_Draw_Mission(object);
            break;
        case DEBUG_STATE_RADIO:
            Tactical_Debug_Draw_Radio(object);
            break;
        case DEBUG_STATE_FOOT:
            Tactical_Debug_Draw_Foot(object);
            break;
        case DEBUG_STATE_FINAL:
            Tactical_Debug_Draw_Final(object);
            break;
        case DEBUG_STATE_LOCOMOTION:
            Tactical_Debug_Draw_Locomotion(object);
            break;
        case DEBUG_STATE_FACING:
            //Tactical_Debug_Draw_Facings(object);
            break;
        default:
            break;
    }

    return true;
}
