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

#if 0

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
void Set_SDL_Palette(void *rpalette)
{
    SDL_Color colors[256];

    unsigned char *rcolors = (unsigned char *)rpalette;
    for (int i = 0; i < 256; i++) {
        colors[i].r = (unsigned char)rcolors[i * 3] << 2;
        colors[i].g = (unsigned char)rcolors[i * 3 + 1] << 2;
        colors[i].b = (unsigned char)rcolors[i * 3 + 2] << 2;
        colors[i].a = 255;
    }

    /**
     *  First color is transparent. This needs to be set so that hardware cursor has
     *  transparent surroundings when converting from 8-bit to 32-bit.
     */
    colors[0].a = 0;

    SDL_SetPaletteColors(SDLPalette, colors, 0, ARRAY_SIZE(colors));
}

#endif
