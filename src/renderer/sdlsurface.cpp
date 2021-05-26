/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SDLSURFACE.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         SDL2 Surface
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
#include "sdlsurface.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "dsurface.h"
#include "options.h"
#include "rgb.h"
#include "sdl_functions.h"
#include "debughandler.h"
#include "asserthandler.h"


SDLSurface::SDLSurface() :
    XSurface(),
    IsAllocated(false),
    VideoSurface(nullptr)
{
}


SDLSurface::SDLSurface(int width, int height) :
    XSurface(width, height),
    IsAllocated(false),
    VideoSurface(nullptr)
{
    VideoSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 16, SDL_PIXELFORMAT_RGB565);
    if (VideoSurface == nullptr) {
        DEBUG_ERROR("VideoSurface could not be created! SDL Error: %s\n", SDL_GetError());
        return;
    }

    SDL_FillRect(VideoSurface, nullptr, 0x000000);

    IsAllocated = true;

    BytesPerPixel = VideoSurface->format->BytesPerPixel;
    Width = VideoSurface->w;
    Height = VideoSurface->h;
}


SDLSurface::SDLSurface(SDL_Surface *surface) :
    XSurface(),
    IsAllocated(false),
    VideoSurface(nullptr)
{
    VideoSurface = surface;
    if (VideoSurface) {
        DEBUG_ERROR("VideoSurface could not be created! SDL Error: %s\n", SDL_GetError());
        return;
    }

    SDL_FillRect(VideoSurface, nullptr, 0x000000);

    BytesPerPixel = VideoSurface->format->BytesPerPixel;
    Width = VideoSurface->w;
    Height = VideoSurface->h;
}


SDLSurface::~SDLSurface()
{
    /**
     *  Deallocate surface.
     */
    SDL_FreeSurface(VideoSurface);
    VideoSurface = nullptr;

    IsAllocated = false;
}


/**
 *  Calculate bit shifts to properly extract channel data.
 */
static void Calculate_Mask_Info(unsigned int mask, unsigned int &left, unsigned int &right, unsigned int bits)
{
    left = 0;
    right = 0;

    unsigned int tmp = mask;

    /**
     *  Figure out how far to shift bits to the left.
     */
    unsigned int l = 0;
    for ( l = 0; l < bits; ++l, tmp >>= 1) { // /= 2
        if (tmp & 1) {   // is odd?
            break;
        }
    }
    left = l;

    /**
     *  Figure out how far to shift bits to the right.
     */
    unsigned int r = 0;
    for ( r = 0; r < 8; ++r, tmp <<= 1) { // *= 2
        if (tmp & 128) {  // is highest bit in the byte is set?
            break;
        }
    }
    right = r;

}


/**
 *  Create the primary drawing surface.
 */
SDLSurface *SDLSurface::Create_Primary(SDLSurface **backbuffer_surface)
{
    DEBUG_INFO("SDLSurface::Create_Primary(enter)\n");

    /**
     *  Create the SDL Surface.
     */
    SDLSurface *primary_surface = new SDLSurface();
    if (!primary_surface) {
        DEBUG_INFO("SDLSurface::Create_Primary() - Failed to create primary surface!\n");
        return nullptr;
    }

    SDL_Surface *sdl_surface = SDL_CreateRGBSurfaceWithFormat(0, Options.ScreenWidth, Options.ScreenHeight, 16, SDL_PIXELFORMAT_RGB565);
    if (!sdl_surface) {
        DEBUG_INFO("SDLSurface::Create_Primary() - Failed to create SDL surface for the primary surface!\n");
        return nullptr;
    }

    primary_surface->VideoSurface = sdl_surface;
    primary_surface->IsAllocated = true;

    if (backbuffer_surface != nullptr) {
        // TODO: SDL handles back buffers?
    }

    /**
     *  Clear the primary surface.
     */
    SDL_FillRect(primary_surface->VideoSurface, nullptr, 0x000000);

    /**
     *  Assign the surface information.
     */
    primary_surface->BytesPerPixel = primary_surface->VideoSurface->format->BytesPerPixel;
    primary_surface->Width = primary_surface->VideoSurface->w;
    primary_surface->Height = primary_surface->VideoSurface->h;

    /**
     *  Calculate all the required DSurface globals.
     */

    Calculate_Mask_Info(primary_surface->VideoSurface->format->Rmask, DSurface::RedLeft, DSurface::RedRight, 16);
    Calculate_Mask_Info(primary_surface->VideoSurface->format->Gmask, DSurface::GreenLeft, DSurface::GreenRight, 16);
    Calculate_Mask_Info(primary_surface->VideoSurface->format->Bmask, DSurface::BlueLeft, DSurface::BlueRight, 16);

    DEBUG_INFO("SDLSurface::Create_Primary() - RedLeft: %d, RedRight: %d.\n", DSurface::RedLeft, DSurface::RedRight);
    DEBUG_INFO("SDLSurface::Create_Primary() - GreenLeft: %d, GreenRight: %d.\n", DSurface::GreenLeft, DSurface::GreenRight);
    DEBUG_INFO("SDLSurface::Create_Primary() - BlueLeft: %d, BlueRight: %d.\n", DSurface::BlueLeft, DSurface::BlueRight);

    DSurface::ColorGrey = DSurface::RGBA_To_Pixel(127, 127, 127);
    DSurface::ColorMidGrey = DSurface::RGBA_To_Pixel(63, 63, 63);
    DSurface::ColorDarkGrey = DSurface::RGBA_To_Pixel(31, 31, 31);

    /**
     *  Detect and set the 16bit pixel format used by the primary surface.
     */
    if (DSurface::RedLeft == 10 && DSurface::RedRight == 3 && DSurface::GreenLeft == 5 && DSurface::GreenRight == 3 && DSurface::BlueLeft == 0 && DSurface::BlueRight == 3) {
        DSurface::RGBPixelFormat = 0; // Uses five bits each for the red, green, and blue components in a pixel.
        DEBUG_INFO("SDLSurface::Create_Primary() - RGBPixelFormat is RGB555.\n");

    } else if (DSurface::RedLeft == 11 && DSurface::RedRight == 3 && DSurface::GreenLeft == 6 && DSurface::GreenRight == 3 && DSurface::BlueLeft == 0 && DSurface::BlueRight == 2) {
        DSurface::RGBPixelFormat = 1; // Uses five bits each for the red and green components, and 6 for the blue component.
        DEBUG_INFO("SDLSurface::Create_Primary() - RGBPixelFormat is RGB556.\n");

    } else if (DSurface::RedLeft == 11 && DSurface::RedRight == 3 && DSurface::GreenLeft == 5 && DSurface::GreenRight == 2 && DSurface::BlueLeft == 0 && DSurface::BlueRight == 3) {
        DSurface::RGBPixelFormat = 2; // Uses five bits for the red and blue components, and six bits for the green component.
        DEBUG_INFO("SDLSurface::Create_Primary() - RGBPixelFormat is RGB565.\n");

    } else if (DSurface::RedLeft == 10 && DSurface::RedRight == 3 && DSurface::GreenLeft == 5 && DSurface::GreenRight == 2 && DSurface::BlueLeft == 0 && DSurface::BlueRight == 3) {
        DSurface::RGBPixelFormat = 3;
        DEBUG_INFO("SDLSurface::Create_Primary() - RGBPixelFormat is RGB655.\n");

    } else {
        DSurface::RGBPixelFormat = -1;
    }

    DEBUG_INFO("SDLSurface::Create_Primary(exit)\n");

    /**
     *  Return pointer to the created primary surface. You must assign this somewhere
     *  as it does not keep track of it within the SDLSurface class.
     */
    return primary_surface;
}


bool SDLSurface::Copy_From(Rect &toarea, Rect &torect, Surface &fromsurface, Rect &fromarea, Rect &fromrect, bool trans_blit, bool a7)
{
    return XSurface::Copy_From(toarea, torect, fromsurface, fromarea, fromrect, trans_blit, a7);

#if 0
    bool use_xsurface = false;

    if (!fromsurface.entry_80() || fromsurface.Is_Locked() || trans_blit) {
        use_xsurface = true;
    }

    if (!use_xsurface && Get_Bytes_Per_Pixel() != fromsurface.Get_Bytes_Per_Pixel()) {
        use_xsurface = true;
    }

    if (Debug_Windowed) {
        a7 = false;
    }

    if (!use_xsurface && a7) {
        if (fromsurface.entry_80()) {
            if (fromrect.Width == torect.Width && fromrect.Height == torect.Height) {
                use_xsurface = true;
            }
        }
    }

    if (use_xsurface) {
        return XSurface::Copy_From(torect, fromsurface, fromrect, trans_blit, a7);
    }

    if (!VideoSurface) {
        DEBUG_WARNING("SDLSurface::Copy_From() - VideoSurface is null!\n");
        return false;
    }

    SDLSurface &from = reinterpret_cast<SDLSurface &>(fromsurface);

    if (!from.VideoSurface) {
        DEBUG_WARNING("SDLSurface::Copy_From() - Source VideoSurface is null!\n");
        return false;
    }

    Rect rect4 = Intersect(fromarea, from.Get_Rect());
    Rect rect2 = Intersect(toarea, Get_Rect());

    if (!func_6A83E0(torect, rect2, fromrect, rect4)) {
        return false;
    }

    SDL_Rect dest_rectangle;
    dest_rectangle.x = torect.X;
    dest_rectangle.y = torect.Y;
    dest_rectangle.w = torect.Width;
    dest_rectangle.h = torect.Height;

    SDL_Rect source_rectangle;
    source_rectangle.x = fromrect.X;
    source_rectangle.y = fromrect.Y;
    source_rectangle.w = fromrect.Width;
    source_rectangle.h = fromrect.Height;

    DEBUG_INFO("SDL Copy_From\n");

    return SDL_BlitSurface(from.VideoSurface, &dest_rectangle, VideoSurface, &source_rectangle) == 0;
#endif
}


bool SDLSurface::Copy_From(Rect &torect, Surface &fromsurface, Rect &fromrect, bool trans_blit, bool a5)
{
    Rect toarea = Get_Rect();
    Rect fromarea = fromsurface.Get_Rect();
    return SDLSurface::Copy_From(toarea, torect, fromsurface, fromarea, fromrect, trans_blit, a5);
}


bool SDLSurface::Copy_From(Surface &fromsurface, bool trans_blit, bool a3)
{
    return XSurface::Copy_From(fromsurface, trans_blit, a3);
}


bool SDLSurface::Fill_Rect(Rect &size, unsigned color)
{
    Rect rect = Get_Rect();
    return SDLSurface::Fill_Rect(rect, size, color);
}


bool SDLSurface::Fill_Rect(Rect &area, Rect &rect, unsigned color)
{
    return XSurface::Fill_Rect(area, rect, color);

#if 0
    if (!rect.Is_Valid()) {
        return false;
    }

    if (Is_Locked()) {
        return XSurface::Fill_Rect(area, rect, color);
    }

    if (!VideoSurface) {
        DEBUG_WARNING("SDLSurface::Fill_Rect() - VideoSurface is null!\n");
        return false;
    }

    Rect rect1;
    rect1.X = rect.X + area.X;
    rect1.Y = rect.Y + area.Y;
    rect1.Width = rect.Width;
    rect1.Height = rect.Height;

    Rect rect2 = Intersect(area, Get_Rect());
    Rect rect3 = Intersect(rect1, rect2);

    if (!rect3.Is_Valid()) {
        return false;
    }

    SDL_Rect dest_rectangle;
    dest_rectangle.x = rect3.X;
    dest_rectangle.y = rect3.Y;
    dest_rectangle.w = rect3.Width;
    dest_rectangle.h = rect3.Height;

    return SDL_FillRect(VideoSurface, &dest_rectangle, color) == 0;
#endif
}


bool SDLSurface::Fill(unsigned color)
{
    Rect surface_rect = Get_Rect();

    Rect dest_rectangle;
    dest_rectangle.X = surface_rect.X;
    dest_rectangle.Y = surface_rect.Y;
    dest_rectangle.Width = surface_rect.Width;
    dest_rectangle.Height = surface_rect.Height;

    return SDLSurface::Fill_Rect(surface_rect, dest_rectangle, color);
}


bool SDLSurface::Fill_Rect_Trans(Rect &rect, const RGBClass &color, unsigned opacity)
{
    return XSurface::Fill_Rect_Trans(rect, color, opacity);
}


bool SDLSurface::Draw_Ellipse(Point2D center, int radius_x, int radius_y, Rect clip, unsigned color)
{
    return XSurface::Draw_Ellipse(center, radius_x, radius_y, clip, color);
}


bool SDLSurface::Put_Pixel(Point2D &point, unsigned color)
{
    return XSurface::Put_Pixel(point, color);
}


unsigned SDLSurface::Get_Pixel(Point2D &point)
{
    return XSurface::Get_Pixel(point);
}


bool SDLSurface::Draw_Line(Point2D &start, Point2D &end, unsigned color)
{
    return XSurface::Draw_Line(start, end, color);
}


bool SDLSurface::Draw_Line(Rect &area, Point2D &start, Point2D &end, unsigned color)
{
    return XSurface::Draw_Line(area, start, end, color);
}


bool SDLSurface::Draw_Line_entry_34(Rect &area, Point2D &start, Point2D &end, unsigned color, int a5, int a6, bool z_only)
{
    return XSurface::Draw_Line_entry_34(area, start, end, color, a5, a6, z_only);
}


bool SDLSurface::Draw_Line_entry_38(Rect &area, Point2D &start, Point2D &end, int a4, int a5, int a6, bool a7)
{
    return XSurface::Draw_Line_entry_38(area, start, end, a4, a5, a6, a7);
}


bool SDLSurface::Draw_Line_entry_3C(Rect &area, Point2D &start, Point2D &end, RGBClass &color, int a5, int a6, bool a7, bool a8, bool a9, bool a10, float a11)
{
    return XSurface::Draw_Line_entry_3C(area, start, end, color, a5, a6, a7, a8, a9, a10, a11);
}


bool SDLSurface::entry_40(Rect &area, Point2D &start, Point2D &end, void(*drawer_callback)(Point2D &))
{
    return XSurface::entry_40(area, start, end, drawer_callback);
}


int SDLSurface::Draw_Dashed_Line(Point2D &start, Point2D &end, unsigned color, bool pattern[], int offset)
{
    return XSurface::Draw_Dashed_Line(start, end, color, pattern, offset);
}


int SDLSurface::entry_48(Point2D &start, Point2D &end, unsigned color, bool pattern[], int offset, bool a6)
{
    return XSurface::entry_48(start, end, color, pattern, offset, a6);
}


bool SDLSurface::entry_4C(Point2D &start, Point2D &end, unsigned color, bool a4)
{
    return XSurface::entry_4C(start, end, color, a4);
}


bool SDLSurface::Draw_Rect(Rect &rect, unsigned color)
{
    return XSurface::Draw_Rect(rect, color);
}


bool SDLSurface::Draw_Rect(Rect &area, Rect &rect, unsigned color)
{
    return XSurface::Draw_Rect(area, rect, color);
}


void *SDLSurface::Lock(int x, int y)
{
    /**
     *  Buffer access sanity check.
     */
    if (x < 0 || y < 0) {
        return nullptr;
    }

    if (SDL_MUSTLOCK(VideoSurface)) {
        SDL_LockSurface(VideoSurface);
    }

    ++LockLevel;

    /**
     *  Return pointer to the direct draw buffer.
     */
    return Get_Buffer_Ptr(x, y);
}


bool SDLSurface::Unlock()
{
    if (LockLevel > 0) {
        --LockLevel;
        if (LockLevel == 0) {
            SDL_UnlockSurface(VideoSurface);
        }
        return true;
    }
    return false;
}


bool SDLSurface::Can_Lock(int x, int y) const
{
    return XSurface::Can_Lock(x, y);
}


bool SDLSurface::entry_64(int x, int y) const
{
    return XSurface::entry_64(x, y);
}


bool SDLSurface::Is_Locked() const
{
    return XSurface::Is_Locked();
}


int SDLSurface::Get_Bytes_Per_Pixel() const
{
    return 4; // Game surfaces are all 16bit colour.

    //Uint32 format;
    //SDL_QueryTexture(VideoTexture, &format, nullptr, nullptr, nullptr);
    //switch (format) {
    //    case SDL_PIXELFORMAT_RGB332: // 8bit colour
    //        return 1;
    //    case SDL_PIXELFORMAT_RGB555: // 15bit/16bit colour
    //        return 2;
    //    case SDL_PIXELFORMAT_ABGR8888: // 32-bit colour
    //        return 4;
    //};
    //return 0;
}


int SDLSurface::Get_Pitch() const
{
    return VideoSurface->pitch;
}


Rect SDLSurface::Get_Rect() const
{
    return XSurface::Get_Rect();
}


int SDLSurface::Get_Width() const
{
    return XSurface::Get_Width();
}


int SDLSurface::Get_Height() const
{
    return XSurface::Get_Height();
}


bool SDLSurface::entry_80() const
{
    return XSurface::entry_80();
}


bool SDLSurface::Draw_Line_entry_90(Rect &area, Point2D &start, Point2D &end, RGBClass &a4, RGBClass &a5, float &a6, float &a7)
{
    unsigned color = DSurface::RGBA_To_Pixel(a4.Red, a4.Green, a4.Blue);
    return XSurface::Draw_Line(area, start, end, color);
}


bool SDLSurface::Can_Blit() const
{
    return true;
}


void *SDLSurface::Get_Buffer_Ptr(int x, int y) const
{
    return (unsigned char *)(VideoSurface->pixels) + (x * Get_Bytes_Per_Pixel()) + (y * Get_Pitch());
}


HDC SDLSurface::Get_DC()
{
    if (!VideoSurface) {
        DEBUG_WARNING("SDLSurface::Get_DC() - VideoSurface is null!\n");
        return nullptr;
    }

    if (Is_Locked()) {
        return nullptr;
    }

    SDL_SysWMinfo info;
    SDL_GetWindowWMInfo(SDLWindow, &info);
    HDC hdc = info.info.win.hdc;
    //HDC hdc = GetDC(MainWindow);
    if (!hdc) {
        DEBUG_WARNING("SDLSurface::Get_DC() - Failed to obtain DC!\n");
        return nullptr;
    }
    ++LockLevel;
    return hdc;
}


BOOL SDLSurface::Release_DC(HDC hdc)
{
    //HRESULT ddrval = ReleaseDC(MainWindow, hdc);
    //if (FAILED(ddrval)) {
    //    DEBUG_WARNING("SDLSurface::Get_DC() - Failed to release DC!\n");
    //    return false;
    //}
    if (LockLevel > 0) {
        --LockLevel;
    }
    return true;
}


bool SDLSurface::Restore_Check()
{
    if (!Debug_Windowed && !GameInFocus) {
        return false;
    }

    if (VideoSurface != nullptr) {
        int prev_locklevel = LockLevel;
        if (LockLevel > 0) {
            LockLevel = 0;
            Lock();
            ++LockLevel;
            Unlock();
            LockLevel = prev_locklevel;
        }
    }
    return true;
}