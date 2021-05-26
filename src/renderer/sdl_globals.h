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

#include "always.h"
#include <SDL.h>
#include <SDL_syswm.h>


/**
 *  Should SDL2 be used to create the game window?
 */
extern bool UseSDL2;

/**
 *  Create the window without a border?
 */
extern bool SDLBorderless;

/**
 *  Create the window at the size of the display as a borderless window?
 */
extern bool SDLBorderlessFullscreen;

extern bool SDLHardwareRenderer;

extern bool SDLClipMouseToWindow;

/**
 *  The window we'll be rendering to.
 */
extern SDL_Window *SDLWindow;

/**
 *  The window renderer.
 */
extern SDL_Renderer *SDLWindowRenderer;

/**
 *  The surface contained by the window.
 */
extern SDL_Surface *SDLWindowSurface;

/**
 *  The texture contained by the window.
 */
extern SDL_Texture *SDLWindowTexture;

/**
 *  256 color palette.
 */
extern SDL_Palette *SDLPalette;
