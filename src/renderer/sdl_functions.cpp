/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SDL_GLOBALS.H
 *
 *  @author        CCHyper
 *
 *  @brief         SDL2 globals.
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

#include "sdl_functions.h"
#include "sdl_globals.h"
#include "sdlsurface.h"
#include "dsurface.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "textprint.h"
#include "debughandler.h"


/**
 *  
 * 
 *  @author: CCHyper
 */
bool SDL_Allocate_Surfaces(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect)
{
    DEBUG_INFO("Allocating new surfaces (SDL)\n");

    if (AlternateSurface) {
        DEBUG_INFO("Deleting AlternateSurface\n");
        delete AlternateSurface;
        AlternateSurface = nullptr;
    }
    if (HiddenSurface) {
        DEBUG_INFO("Deleting HiddenSurface\n");
        delete HiddenSurface;
        HiddenSurface = nullptr;
    }
    if (CompositeSurface) {
        DEBUG_INFO("Deleting CompositeSurface\n");
        delete CompositeSurface;
        CompositeSurface = nullptr;
    }
    if (TileSurface) {
        DEBUG_INFO("Deleting TileSurface\n");
        delete TileSurface;
        TileSurface = nullptr;
    }
    if (SidebarSurface) {
        DEBUG_INFO("Deleting SidebarSurface\n");
        delete SidebarSurface;
        SidebarSurface = nullptr;
    }

    if (common_rect->Width > 0 && common_rect->Height > 0) {
        AlternateSurface = (DSurface *)new SDLSurface(common_rect->Width, common_rect->Height);
        AlternateSurface->Fill(COLOR_TBLACK);
        DEBUG_INFO("AlternateSurface (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (common_rect->Width > 0 && common_rect->Height > 0) {
        HiddenSurface = (DSurface *)new SDLSurface(common_rect->Width, common_rect->Height);
        HiddenSurface->Fill(COLOR_TBLACK);
        DEBUG_INFO("HiddenSurface (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (composite_rect->Width > 0 && composite_rect->Height > 0) {
        CompositeSurface = (DSurface *)new SDLSurface(composite_rect->Width, composite_rect->Height);
        CompositeSurface->Fill(COLOR_TBLACK);
        DEBUG_INFO("CompositeSurface (%dx%d)\n", composite_rect->Width, composite_rect->Height);
    }

    if (tile_rect->Width > 0 && tile_rect->Height > 0) {
        TileSurface = (DSurface *)new SDLSurface(tile_rect->Width, tile_rect->Height);
        TileSurface->Fill(COLOR_TBLACK);
        DEBUG_INFO("TileSurface (%dx%d)\n", tile_rect->Width, tile_rect->Height);
    }

    if (sidebar_rect->Width > 0 && sidebar_rect->Height > 0) {
        SidebarSurface = (DSurface *)new SDLSurface(sidebar_rect->Width, sidebar_rect->Height);
        SidebarSurface->Fill(COLOR_TBLACK);
        DEBUG_INFO("SidebarSurface (%dx%d)\n", sidebar_rect->Width, sidebar_rect->Height);
    }

    return true;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void Set_SDL_Palette(void *rpalette)
{
    SDL_Color colors[256];

    unsigned char* rcolors = (unsigned char *)rpalette;
    for (int i = 0; i < 256; i++) {
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

    SDL_SetPaletteColors(SDLPalette, colors, 0, 256);
}


/**
 *  Update the screen with any rendering performed since the previous call.
 * 
 *  @author: CCHyper, tomsons26
 */
bool SDL_Update_Screen(DSurface *surface, SDL_Rect *src_rect, SDL_Rect *dest_rect)
{
    //DEBUG_INFO("SDL_Update_Screen\n");

    SDL_RenderClear(SDLWindowRenderer);

#if 0
    /**
     *  Blit games surface to SDL's window surface.
     */
    if (surface) {
        SDL_BlitSurface(reinterpret_cast<SDLSurface *&>(surface)->VideoSurface, src_rect, SDLWindowSurface, dest_rect);
    }

    /**
     *  Convert the recently blitted pixel data from 16bit to 32bit.
     */
    void *pixels;
    int pitch;
    SDL_LockTexture(SDLWindowTexture, nullptr, &pixels, &pitch);

    SDL_ConvertPixels(SDLWindowSurface->w, SDLWindowSurface->h, SDLWindowSurface->format->format,
        SDLWindowSurface->pixels, SDLWindowSurface->pitch, SDL_PIXELFORMAT_RGB888, pixels, pitch);

    SDL_UnlockTexture(SDLWindowTexture);

    /**
     *  Update the window texture.
     */
    SDL_UpdateTexture(SDLWindowTexture, nullptr, SDLWindowSurface->pixels, SDLWindowSurface->pitch);

    /**
     *  Copy the texture to the renderer.
     */
    SDL_RenderCopy(SDLWindowRenderer, SDLWindowTexture, nullptr, nullptr);

#else
    /**
     *  Blit games surface to SDL's window surface.
     */
    if (surface) {

#if 0
        Rect rect1{0,0,200,200};
        Rect rect2{200,200,200,200};
        surface->Fill_Rect(rect1, DSurface::RGBA_To_Pixel(0, 255, 0));
        surface->Fill_Rect(rect2, DSurface::RGBA_To_Pixel(255, 0, 0));
#endif

        SDL_Surface *surf = reinterpret_cast<SDLSurface *&>(surface)->VideoSurface;

        /**
         *  Convert the 16bit pixel data from the surface to the SDL window 32bit texture.
         */
        void *pixels;
        int pitch;
        SDL_LockTexture(SDLWindowTexture, nullptr, &pixels, &pitch);
        SDL_ConvertPixels(surf->w, surf->h, surf->format->format, surf->pixels, surf->pitch, SDL_PIXELFORMAT_RGB888, pixels, pitch);
        SDL_UnlockTexture(SDLWindowTexture);

        /**
         *  Update the window texture.
         */
        SDL_UpdateTexture(SDLWindowTexture, nullptr, pixels, pitch);

        /**
         *  Copy the texture to the renderer.
         */
        SDL_RenderCopy(SDLWindowRenderer, SDLWindowTexture, nullptr, nullptr);
    }

#endif

    /**
     *  Update the renderer to the window.
     */
    SDL_RenderPresent(SDLWindowRenderer);

    return true;
}


/**
 *  Clear the initial screen with solid black.
 * 
 *  @author: CCHyper
 */
bool SDL_Clear_Screen()
{
    DEBUG_INFO("SDL_Clear_Screen\n");

    SDL_FillRect(SDLWindowSurface, nullptr, 0x000000);

    SDL_RenderPresent(SDLWindowRenderer);

    return true;
}
