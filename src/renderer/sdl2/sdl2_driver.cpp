/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SDL2_DRIVER.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         
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
#include "sdl2_driver.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "options.h"
#include "wwmouse.h"
#include "debughandler.h"

#if defined(SDL2_RENDERER)

#include <SDL.h>
#include <SDL_syswm.h>


static SDL_Window *SDLWindow = nullptr;
static SDL_Palette *SDLPalette = nullptr;


/**
 *  x
 * 
 *  @author: CCHyper
 */
static LRESULT CALLBACK SDL_WndProcWrapper(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    int low_param = LOWORD(wParam);

    /**
     *  Call the games windows procedure.
     */
    LRESULT res = Main_Window_Procedure(hWnd, Message, wParam, lParam);

    switch (Message) {
        case WM_MOVE:
            WWMouse->Calc_Confining_Rect();
            //SDL_WarpMouseInWindow(Window, WWMouse->Get_Mouse_X(), WWMouse->Get_Mouse_Y());
            break;

        case WM_ACTIVATEAPP:
            SDL_SetWindowGrab(SDLWindow, SDL_TRUE);
            break;

        case WM_ACTIVATE:
            if (low_param == WA_INACTIVE) {
                SDL_SetWindowGrab(SDLWindow, SDL_FALSE);
            }

        default:
            break;
    };

#if 0
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_WINDOWEVENT:
            switch (event.window.event) {

                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    break;

                case SDL_WINDOWEVENT_MOVED:
                    WWMouse->Calc_Confining_Rect();
                    break;

                case SDL_WINDOWEVENT_FOCUS_LOST:
                    break;
            };
    };
#endif

    return res;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
static void SDL_Set_Palette(SDL_Palette *sdl_palette, void *rpalette)
{
    SDL_Color colors[256];

    unsigned char* rcolors = (unsigned char *)rpalette;
    for (int i = 0; i < ARRAY_SIZE(colors); i++) {
        colors[i].r = (unsigned char)rcolors[i * 3] << 2;
        colors[i].g = (unsigned char)rcolors[i * 3 + 1] << 2;
        colors[i].b = (unsigned char)rcolors[i * 3 + 2] << 2;
        colors[i].a = 0xFF;
    }

    /*
    ** First color is transparent. This needs to be set so that hardware cursor has transparent
    ** surroundings when converting from 8-bit to 32-bit.
    */
    colors[0].a = 0;

    SDL_SetPaletteColors(sdl_palette, colors, 0, 256);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
SDL2VideoDriver::SDL2VideoDriver() :
    VideoDriver("SDL2"),
    WindowRenderer(nullptr),
    WindowSurface(nullptr),
    WindowTexture(nullptr),
    Palette(nullptr)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
SDL2VideoDriver::~SDL2VideoDriver()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Close_Window(bool force)
{
}


/**
 *  Destroy the window.
 * 
 *  @author: CCHyper
 */
void SDL2VideoDriver::Destroy_Window(bool force)
{
    SDL_DestroyWindow(SDLWindow);
    SDLWindow = nullptr;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Show_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Hide_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Minimize_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Maximize_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool SDL2VideoDriver::Toggle_Fullscreen()
{
    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Focus_Loss()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Focus_Restore()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Set_Cursor_Clip()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Clear_Cursor_Clip()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool SDL2VideoDriver::Set_Video_Mode(HWND hWnd, int width, int height, int bits_per_pixel)
{
    if (!SDLWindow) {
        DEBUG_ERROR("SDL2 - Window is null!\n");
        return false;
    }

    if (WindowRenderer && WindowSurface && WindowTexture) {
        DEBUG_WARNING("SDL2 - Video mode has already been set!\n");
        return true;
    }

    Reset_Video_Mode();

    Uint32 pixel_format = SDL_GetWindowPixelFormat(SDLWindow);
    if (pixel_format == SDL_PIXELFORMAT_UNKNOWN || SDL_BITSPERPIXEL(pixel_format) < 16) {
        DEBUG_ERROR("SDL2 - Window pixel format unsupported: %s (%d bpp)\n", SDL_GetPixelFormatName(pixel_format), SDL_BITSPERPIXEL(pixel_format));
        return false;
    }

    DEBUG_INFO("SDL2 - Pixel format: %s (%d bpp)\n", SDL_GetPixelFormatName(pixel_format), SDL_BITSPERPIXEL(pixel_format));

    int renderer_index = -1;

    int flags = SDL_RENDERER_TARGETTEXTURE;
    if (IsUseHardwareRenderer) {
        flags |= SDL_RENDERER_ACCELERATED;
    } else {
        flags |= SDL_RENDERER_SOFTWARE;
    }

    /**
     *  Create renderer for window.
     */
    WindowRenderer = SDL_CreateRenderer(SDLWindow, renderer_index, flags);
    if (WindowRenderer == nullptr) {
        DEBUG_ERROR("SDL2 - WindowRenderer could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    DEBUG_INFO("SDL2 - WindowRenderer created.\n");

    SDL_RendererInfo info;
    if (SDL_GetRendererInfo(WindowRenderer, &info) != 0) {
        DEBUG_ERROR("SDL2 - SDL_GetRendererInfo failed to get info! SDL Error: %s\n", SDL_GetError());
        Reset_Video_Mode();
        return false;
    }

    DEBUG_INFO("SDL2 - Initialized SDL2 driver '%s'\n", info.name);
    DEBUG_INFO("  flags:\n");
    if (info.flags & SDL_RENDERER_SOFTWARE) {
        DEBUG_INFO("    SDL_RENDERER_SOFTWARE\n");
    }
    if (info.flags & SDL_RENDERER_ACCELERATED) {
        DEBUG_INFO("    SDL_RENDERER_ACCELERATED\n");
    }
    if (info.flags & SDL_RENDERER_PRESENTVSYNC) {
        DEBUG_INFO("    SDL_RENDERER_PRESENT_VSYNC\n");
    }
    if (info.flags & SDL_RENDERER_TARGETTEXTURE) {
        DEBUG_INFO("    SDL_RENDERER_TARGETTEXTURE\n");
    }

    //DEBUG_INFO("  Max texture size: %dx%d\n", info.max_texture_width, info.max_texture_height);
    DEBUG_INFO("  %d texture formats supported\n", info.num_texture_formats);

    /*
    ** Pick the first pixel format or the user requested one. It better be RGB.
    */
    pixel_format = SDL_PIXELFORMAT_UNKNOWN;
    for (int i = 0; i < info.num_texture_formats; i++) {
        if (pixel_format == SDL_PIXELFORMAT_UNKNOWN && i == 0) {
            pixel_format = info.texture_formats[i];
        }
    }

    for (int i = 0; i < info.num_texture_formats; i++) {
        DEBUG_INFO("    %s%s\n", SDL_GetPixelFormatName(info.texture_formats[i]), (pixel_format == info.texture_formats[i] ? " (selected)" : ""));
    }

    /*
    ** Set requested scaling algorithm.
    */
    if (!SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY, "nearest", SDL_HINT_OVERRIDE)) {
        DEBUG_WARNING("  scaler 'nearest' is unsupported!\n");
    } else {
        DEBUG_INFO("  scaler set to 'nearest'\n");
    }

    /**
     *  Explicitly set input focus to the window.
     */
    SDL_SetWindowInputFocus(SDLWindow);

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void SDL2VideoDriver::Reset_Video_Mode()
{
    /**
     *  Destroy renderer.
     */
    SDL_DestroyRenderer(WindowRenderer);
    WindowRenderer = nullptr;

    /**
     *  Deallocate surface.
     */
    SDL_FreeSurface(WindowSurface);
    WindowSurface = nullptr;

    /**
     *  Deallocate texture.
     */
    SDL_DestroyTexture(WindowTexture);
    WindowTexture = nullptr;

    /**
     *  Deallocate palette.
     */
    SDL_FreePalette(Palette);
    Palette = nullptr;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Release(HWND hWnd)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool SDL2VideoDriver::Prep_Renderer(HWND hWnd)
{
    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Check_Overlapped_Blit_Capability()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Wait_Blit()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
extern void __cdecl Write_Surface_Data_To_File(const char *filename, XSurface *surface);
bool SDL2VideoDriver::Flip(XSurface *surface)
{
    unsigned short *srcptr = reinterpret_cast<unsigned short *>(surface->Lock());
    if (!srcptr) {
        DEBUG_ERROR("SDL2 - Failed to lock game surface!\n");
        return false;
    }

#if 0
    static int _counter = 0;
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "SDL_%04d.png", _counter++);
    Write_Surface_Data_To_File(buffer, surface);
#endif

    SDL_RenderClear(WindowRenderer);

    SDL_Surface *tmp_surface = SDL_CreateRGBSurfaceFrom(
        srcptr,
        surface->Get_Width(),
        surface->Get_Height(),
        8 * surface->Get_Bytes_Per_Pixel(),
        surface->Get_Pitch(),
        0,
        0,
        0,
        0);

    SDL_Rect src_rect;
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = surface->Get_Width();
    src_rect.h = surface->Get_Height();

    SDL_Rect dest_rect;
    dest_rect.x = 0;
    dest_rect.y = 0;
    dest_rect.w = WindowWidth;
    dest_rect.h = WindowHeight;

#if 0

    SDL_UpperBlitScaled(tmp_surface, &src_rect, WindowSurface, &dest_rect);

#else

    /**
     *  Convert the 16bit pixel data from the surface to the SDL window 32bit texture.
     */
    void *pixels;
    int pitch;
    SDL_LockTexture(WindowTexture, nullptr, &pixels, &pitch);
    SDL_ConvertPixels(tmp_surface->w, tmp_surface->h, tmp_surface->format->format, tmp_surface->pixels, tmp_surface->pitch, SDL_PIXELFORMAT_RGB888, pixels, pitch);
    SDL_UnlockTexture(WindowTexture);

    /**
     *  Update the window texture.
     */
    SDL_UpdateTexture(WindowTexture, nullptr, pixels, pitch);

    /**
     *  Copy the texture to the renderer.
     */
    SDL_RenderCopy(WindowRenderer, WindowTexture, &src_rect, &dest_rect);

#endif

    /**
     *  Update the renderer to the window.
     */
    SDL_RenderPresent(WindowRenderer);

    /**
     *  Finished with the surfaces, unlock them.
     */
    SDL_FreeSurface(tmp_surface);
    surface->Unlock();

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void SDL2VideoDriver::Shutdown()
{
    Reset_Video_Mode();
}


/**
 *  Update the screen with any rendering performed since the previous call.
 * 
 *  @author: CCHyper, tomsons26
 */
bool SDL2VideoDriver::Update_Screen(XSurface *surface, Rect *src_rect, Rect *dest_rect)
{
    return true;
}


/**
 *  Clear the initial screen with solid black.
 * 
 *  @author: CCHyper
 */
bool SDL2VideoDriver::Clear_Screen(bool present)
{
    SDL_FillRect(WindowSurface, nullptr, 0x000000);

    if (present) {
        SDL_RenderPresent(WindowRenderer);
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool SDL2VideoDriver::Internal_Create_Primary(Surface *primary_surface, XSurface **backbuffer_surface)
{
    /**
     *  Get window surface.
     */
    WindowSurface = SDL_CreateRGBSurfaceWithFormat(0, WindowWidth, WindowHeight, 32, SDL_PIXELFORMAT_RGB888);
    //WindowSurface = SDL_GetWindowSurface(SDLWindow);
    if (WindowSurface == nullptr) {
        DEBUG_ERROR("SDL2 - WindowSurface could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    DEBUG_INFO("SDL2 - WindowSurface created.\n");

    /**
     *  Create the 32bit window texture.
     */
    WindowTexture = SDL_CreateTexture(WindowRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, WindowWidth, WindowHeight);
    if (WindowTexture == nullptr) {
        DEBUG_ERROR("SDL2 - WindowTexture could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    DEBUG_INFO("SDL2 - WindowTexture created.\n");

    if (!Palette) {
        Palette = SDL_AllocPalette(256);

        SDL_Color colors[256];

        for (int i = 0; i < 256; i++) {
            colors[i].r = 0xFF;
            colors[i].g = 0xFF;
            colors[i].b = 0xFF;
            colors[i].a = 0xFF;
        }

        /**
         *  First color needs to be transparent.
         * 
         *  This needs to be set so that hardware cursor has transparent
         *  surroundings when converting from 8-bit to 32-bit.
         */
        colors[0].a = 0;

        SDL_SetPaletteColors(Palette, colors, 0, 256);

        DEBUG_INFO("SDL2 - 256 color palette created.\n");
    }

    /**
     *  Clear the window screen.
     */
    Clear_Screen();

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool SDL2VideoDriver::Internal_Create_Window(HINSTANCE hInstance)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        DEBUG_ERROR("SDL2 - SDL_Init failed! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    int flags = SDL_WINDOW_SHOWN;

    if (IsClipCursorToWindow) {
        flags |= SDL_WINDOW_INPUT_GRABBED;
        SDL_SetWindowGrab(SDLWindow, SDL_TRUE);
    }

    if (IsBorderlessWindow) {
        DEBUG_INFO("SDL2 - Creating fullscreen desktop window.\n");
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    } else if (false) { // TODO
        DEBUG_INFO("SDL2 - Creating borderless window.\n");
        flags |= SDL_WINDOW_BORDERLESS;
    }

    int width = WindowWidth;
    int height = WindowHeight;

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    /**
     *  Create the window.
     */
    if (IsBorderlessWindow) {
        width = dm.w;
        height = dm.h;
        SDLWindow = SDL_CreateWindow(WindowTitle.Peek_Buffer(), 0, 0, width, height, flags);

    } else {
        //if (dm.w == 3840 && dm.h == 2160) {
        //    width *= 2;
        //    height *= 2;
        //}
        int wnd_xpos = (dm.w-width)/2;
        int wnd_ypos = (dm.h-height)/2;
        wnd_ypos += 38; // Take into account the window title bar.
        SDLWindow = SDL_CreateWindow(WindowTitle.Peek_Buffer(), wnd_xpos, wnd_ypos, width, height, flags);
    }
    if (SDLWindow == nullptr) {
        DEBUG_ERROR("SDL2 - Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    DEBUG_INFO("SDL2 - Window created.\n");

    /**
     *  Do various stuff to make the SDL window intersect with the game correctly.
     */

    //SDL_ShowCursor(SDL_DISABLE);

    SDL_Cursor *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW); 
    SDL_SetCursor(cursor); 

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(SDLWindow, &wmInfo);

    MainWindow = wmInfo.info.win.window;

    /**
     *  Set the games windows proc function to the window.
     */
    SetWindowLong(MainWindow, GWL_WNDPROC, (LONG)SDL_WndProcWrapper);

    return true;
}

#endif
