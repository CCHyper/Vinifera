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
#include "object.h"
#include "objecttype.h"
#include "unit.h"
#include "unittype.h"
#include "dsurface.h"
#include "textprint.h"
#include "wwfont.h"
#include "scenario.h"
#include "session.h"
#include "colorscheme.h"
#include "voc.h"
#include "fatal.h"
#include "vinifera_globals.h"
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


#ifndef NDEBUG
/**
 *  Draws the current unit facing number.
 * 
 *  @author: CCHyper
 */
bool Tactical_Debug_Draw_Facings()
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

    TempSurface->Fill_Rect(TacticalRect, Rect(screen.X, screen.Y, 2, 2), DSurface::RGBA_To_Pixel(255,0,0));

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
#endif


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




#include "iomap.h"
#include "bsurface.h"
#include "dsurface.h"
#include "hsv.h"
#include "rgb.h"


static void Adjust_Surface_Saturation(BSurface *surface, float level)
{
    if (!surface) {
        return;
    }

    /**
     *  Clamp the saturation level within expected range.
     */
    level = std::clamp(level, 0.0f, 1.0f);

    unsigned char *bufptr = (unsigned char *)surface->Lock();
    if (bufptr) {

        unsigned char r;
        unsigned char g;
        unsigned char b;

        for (int y = 0; y < surface->Get_Height(); ++y) {

            /**
             *  Make sure we skip the tab bar.
             */
            if (y >= TacticalRect.Y) {

                for (int x = 0; x < surface->Get_Width(); ++x) {

                    unsigned short *pixel = ((unsigned short *)bufptr + x);
                    if (*pixel != 0) {

                        /**
                         *  Get the existing pixel color values.
                         */
                        r = ((*pixel >> DSurface::RedLeft) << DSurface::RedRight);
                        g = ((*pixel >> DSurface::GreenLeft) << DSurface::GreenRight);
                        b = ((*pixel >> DSurface::BlueLeft) << DSurface::BlueRight);
                        //DSurface::Pixel_To_RGBA(*pixel, &r, &g, &b);

                        /**
                         *  Convert to HSV so we can easily adjust the saturation.
                         */
                        HSVClass hsv = RGBClass(r, g, b);
                        hsv.Saturation = std::clamp((int)(level * hsv.Saturation), 0, 255);

                        /**
                         *  Now convert back to RGB.
                         */
                        RGBClass rgb = hsv;

                        /**
                         *  Put the adjusted pixel back.
                         */
                        *pixel = DSurface::RGBA_To_Pixel(rgb.Red, rgb.Green, rgb.Blue);
                    }
                
                }
            }

            bufptr += surface->Get_Pitch();
        }

        surface->Unlock();
    }
}


/**
 *  
 */
static void Do_Chrono_Effect_AI()
{
    static bool _is_fading_in = false;
    static bool _is_fading_out = false;

    //static CDTimerClass<FrameTimerClass> _fading_in_timer;
    //static CDTimerClass<FrameTimerClass> _fading_out_timer;
    static CDTimerClass<MSTimerClass> _fading_in_timer;
    static CDTimerClass<MSTimerClass> _fading_out_timer;
    
    //static const int FADE_IN_RATE = (1*TICKS_PER_SECOND)/2;
    //static const int FADE_OUT_RATE = (1*TICKS_PER_SECOND);
    static const int FADE_IN_RATE = 1000/3;
    static const int FADE_OUT_RATE = 1000/3;

    static BSurface *_working_surface = nullptr;

    static float l = 1.0f;

    /**
     *  
     */
    if (!_working_surface) {
        _working_surface = new BSurface(CompositeSurface->Get_Width(),
            CompositeSurface->Get_Height(), CompositeSurface->Get_Bytes_Per_Pixel());
    }

    //if (PendingChronoFlash) {
    if (PendingScreenFlash) {
        _is_fading_in = true;
        _is_fading_out = false;
        _fading_in_timer = FADE_IN_RATE;
        //PendingChronoFlash = false;
        PendingScreenFlash = false;
    }

    /**
     *  Fading from grey to color.
     */
    if (_is_fading_out) {
        if (_fading_out_timer.Expired()) {
            _is_fading_out = false;
        }

        /**
         *  Update working surface.
         */
        _working_surface->Copy_From(*CompositeSurface);

        /**
         *  Adjust working surface saturation.
         */
        float level = (_fading_out_timer) * 1.0f / FADE_OUT_RATE;
        Adjust_Surface_Saturation(_working_surface, level);
        
        /**
         *  Copy back to the composite surface.
         */
        CompositeSurface->Copy_From(*_working_surface);
    }

    /**
     *  Fading from color to grey.
     */
    if (_is_fading_in) {
        if (_fading_in_timer.Expired()) {
            _is_fading_in = false;
        }
        
        /**
         *  Update working surface.
         */
        _working_surface->Copy_From(*CompositeSurface);

        /**
         *  Adjust working surface saturation.
         */
        float level = (FADE_IN_RATE-_fading_in_timer) * 1.0f / FADE_IN_RATE;
        Adjust_Surface_Saturation(_working_surface, level);
        
        /**
         *  Copy back to the composite surface.
         */
        CompositeSurface->Copy_From(*_working_surface);

        /**
         *  Flag and prepare the fade back timer.
         */
        if (!_is_fading_in) {
            _is_fading_out = true;
            _fading_out_timer = FADE_OUT_RATE;
        }
    }
}


/**
 *  
 */
static void Do_Flash_Effect_AI()
{
    static bool _is_fading_in = false;
    static bool _is_fading_out = false;

    //static CDTimerClass<FrameTimerClass> _fading_in_timer;
    //static CDTimerClass<FrameTimerClass> _fading_out_timer;
    static CDTimerClass<MSTimerClass> _fading_in_timer;
    static CDTimerClass<MSTimerClass> _fading_out_timer;

    static RGBClass flash_color(255,255,255);
    
    static const int FADE_INTENSITY = 90;
    //static const int FADE_IN_RATE = (1*TICKS_PER_SECOND)/2;
    //static const int FADE_OUT_RATE = (1*TICKS_PER_SECOND);
    static const int FADE_IN_RATE = 1000/4;
    static const int FADE_OUT_RATE = 1000/3;

    if (PendingScreenFlash) {
        _is_fading_in = true;
        _is_fading_out = false;
        _fading_in_timer = FADE_IN_RATE;
        PendingScreenFlash = false;
    }

    /**
     *  Fading from white to game.
     */
    if (_is_fading_out) {
        if (_fading_out_timer.Expired()) {
            _is_fading_out = false;
        }
        unsigned adjust = (_fading_out_timer) * FADE_INTENSITY / FADE_OUT_RATE;
        CompositeSurface->Fill_Rect_Trans(TacticalRect, flash_color, adjust);
    }

    /**
     *  Fading from game to white.
     */
    if (_is_fading_in) {
        if (_fading_in_timer.Expired()) {
            _is_fading_in = false;
        }
        unsigned adjust = (FADE_IN_RATE-_fading_in_timer) * FADE_INTENSITY / FADE_IN_RATE;
        CompositeSurface->Fill_Rect_Trans(TacticalRect, flash_color, adjust);

        /**
         *  Flag and prepare the fade back timer.
         */
        if (!_is_fading_in) {
            _is_fading_out = true;
            _fading_out_timer = FADE_OUT_RATE;
        }
    }
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

        if (Vinifera_Developer_FrameStep) {
            Tactical_Draw_FrameStep_Overlay();
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

    //Do_Flash_Effect_AI();
    Do_Chrono_Effect_AI();

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
