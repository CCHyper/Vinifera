/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SDL_CREATEWINDOW.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Creates the main SDL2 window.
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
#pragma once

#include "sdl_createwindow.h"
#include "sdl_globals.h"
#include "sdl_functions.h"
#include "sdlsurface.h"
#include "dsurface.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "textprint.h"
#include "options.h"
#include "wwmouse.h"
#include "debughandler.h"


/**
 *  
 * 
 *  @author: CCHyper
 */
static LRESULT CALLBACK WndProcWrapper(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    int low_param = LOWORD(wParam);

    /**
     *  Call the games windows procedure.
     */
    LRESULT res = Main_Window_Procedure(hWnd, Message, wParam, lParam);

    switch (Message) {
        case WM_MOVE:
            WWMouse->Calc_Confining_Rect();
            //SDL_WarpMouseInWindow(SDLWindow, WWMouse->Get_Mouse_X(), WWMouse->Get_Mouse_Y());
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
 *  
 * 
 *  @author: CCHyper
 */
bool SDL_Create_Main_Window(HINSTANCE hInstance, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        DEBUG_ERROR("SDL_Init failed! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    int flags = SDL_WINDOW_SHOWN;

    if (SDLClipMouseToWindow) {
        flags |= SDL_WINDOW_INPUT_GRABBED;
        SDL_SetWindowGrab(SDLWindow, SDL_TRUE);
    }

    if (SDLBorderlessFullscreen) {
        DEBUG_INFO("Creating fullscreen desktop window.\n");
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    } else if (SDLBorderless) {
        DEBUG_INFO("Creating borderless window.\n");
        flags |= SDL_WINDOW_BORDERLESS;
    }

    const char *window_title = "Tiberian Sun";
    char buffer[64];    
    std::snprintf(buffer, sizeof(buffer), "%s (SDL)", window_title);

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    /**
     *  Create the window.
     */
    if (SDLBorderlessFullscreen) {
        width = dm.w;
        height = dm.h;
        SDLWindow = SDL_CreateWindow(buffer, 0, 0, width, height, flags);
    } else {
        //if (dm.w == 3840 && dm.h == 2160) {
        //    width *= 2;
        //    height *= 2;
        //}
        int wnd_xpos = (dm.w-width)/2;
        int wnd_ypos = (dm.h-height)/2;
        wnd_ypos += 38; // Take into account the window title bar.
        SDLWindow = SDL_CreateWindow(buffer, wnd_xpos, wnd_ypos, width, height, flags);
    }
    if (SDLWindow == nullptr) {
        DEBUG_ERROR("SDLWindow could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    DEBUG_INFO("SDLWindow created.\n");

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
    SetWindowLong(MainWindow, GWL_WNDPROC, (LONG)WndProcWrapper);

    return true;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void SDL_Destroy_Main_Window()
{
    /**
     *  Destroy window.
     */
    SDL_DestroyWindow(SDLWindow);
    SDLWindow = nullptr;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool SDL_Set_Video_Mode(HWND hWnd, int width, int height)
{
    if (!SDLWindow) {
        DEBUG_ERROR("SDLWindow is null!\n");
        return false;
    }

    if (SDLWindowRenderer || SDLWindowSurface || SDLWindowTexture) {
        DEBUG_WARNING("Video mode has already been set!\n");
        return true;
    }

    Uint32 pixel_format = SDL_GetWindowPixelFormat(SDLWindow);
    if (pixel_format == SDL_PIXELFORMAT_UNKNOWN || SDL_BITSPERPIXEL(pixel_format) < 16) {
        DEBUG_ERROR("SDL2 window pixel format unsupported: %s (%d bpp)\n", SDL_GetPixelFormatName(pixel_format), SDL_BITSPERPIXEL(pixel_format));
        return false;
    }

    DEBUG_INFO("Pixel format: %s (%d bpp)\n", SDL_GetPixelFormatName(pixel_format), SDL_BITSPERPIXEL(pixel_format));

    int renderer_index = -1;

    int flags = SDL_RENDERER_TARGETTEXTURE;
    if (SDLHardwareRenderer) {
        flags |= SDL_RENDERER_ACCELERATED;
    } else {
        flags |= SDL_RENDERER_SOFTWARE;
    }

    /**
     *  Create renderer for window.
     */
    SDLWindowRenderer = SDL_CreateRenderer(SDLWindow, renderer_index, flags);
    if (SDLWindowRenderer == nullptr) {
        DEBUG_ERROR("SDLWindowRenderer could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    DEBUG_INFO("SDLWindowRenderer created.\n");

    /**
     *  Get window surface.
     */
    SDLWindowSurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGB888);
    //SDLWindowSurface = SDL_GetWindowSurface(SDLWindow);
    if (SDLWindowSurface == nullptr) {
        DEBUG_ERROR("SDLWindowSurface could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    DEBUG_INFO("SDLWindowSurface created.\n");

    /**
     *  Create window texture.
     */
    SDLWindowTexture = SDL_CreateTexture(SDLWindowRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (SDLWindowTexture == nullptr) {
        DEBUG_ERROR("SDLWindowTexture could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    DEBUG_INFO("SDLWindowTexture created.\n");

    SDL_RendererInfo info;
    if (SDL_GetRendererInfo(SDLWindowRenderer, &info) != 0) {
        DEBUG_ERROR("SDL_GetRendererInfo failed to get info! SDL Error: %s\n", SDL_GetError());
        SDL_Reset_Video_Mode();
        return false;
    }

    /**
     *  Clear the window screen to black.
     */
    SDL_Clear_Screen();
    SDL_Update_Screen(nullptr);

    DEBUG_INFO("Initialized SDL2 driver '%s'\n", info.name);
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

    if (!SDLPalette) {
        SDLPalette = SDL_AllocPalette(256);

        SDL_Color colors[256];

        for (int i = 0; i < 256; i++) {
            colors[i].r = 0xFF;
            colors[i].g = 0xFF;
            colors[i].b = 0xFF;
            colors[i].a = 0xFF;
        }

        /*
        ** First color needs to be transparent.
        */
        colors[0].a = 0;

        SDL_SetPaletteColors(SDLPalette, colors, 0, 256);

        DEBUG_INFO("  256 color palette created.\n");
    }

    /**
     *  Explicitly set input focus to the window.
     */
    SDL_SetWindowInputFocus(SDLWindow);
    GameInFocus = true; // The SDL window needs this initially otherwise we need to alt-tab to gain focus.

    return true;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void SDL_Reset_Video_Mode()
{
    /**
     *  Destroy renderer.
     */
    SDL_DestroyRenderer(SDLWindowRenderer);
    SDLWindowRenderer = nullptr;

    /**
     *  Deallocate surface.
     */
    SDL_FreeSurface(SDLWindowSurface);
    SDLWindowSurface = nullptr;

    /**
     *  Deallocate texture.
     */
    SDL_DestroyTexture(SDLWindowTexture);
    SDLWindowTexture = nullptr;

    /**
     *  Deallocate palette.
     */
    SDL_FreePalette(SDLPalette);
    SDLPalette = nullptr;
}
