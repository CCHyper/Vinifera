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

#include "sdl_globals.h"


bool UseSDL2 = true;
bool SDLBorderless = false;
bool SDLBorderlessFullscreen = true;
bool SDLHardwareRenderer = false;
bool SDLClipMouseToWindow = true;

SDL_Window *SDLWindow = nullptr;
SDL_Renderer *SDLWindowRenderer = nullptr;
SDL_Surface *SDLWindowSurface = nullptr;
SDL_Texture *SDLWindowTexture = nullptr;
SDL_Palette *SDLPalette = nullptr;
