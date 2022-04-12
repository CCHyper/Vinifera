/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DDRAW7_SUPPORT.H
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
#pragma once


#if 0

#include "always.h"
#include "ddraw7_util.h"


class XSurface;
class Rect;


extern LPDIRECTDRAW7 DirectDraw7Object;
extern LPDIRECTDRAWSURFACE7 DirectDraw7PrimarySurface;
extern LPDIRECTDRAWSURFACE7 DirectDraw7BackSurface;
extern LPDIRECTDRAWCLIPPER DirectDraw7Clipper;
extern LPDDSURFACEDESC2 VideoSurfaceDescription;


XSurface *DirectDraw7_Create_Primary(XSurface **backbuffer_surface = nullptr);
bool DirectDraw7_Allocate_Surfaces(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect);
void DirectDraw7_Release(HWND hWnd);
void DirectDraw7_Shutdown();
void DirectDraw7_Reset_Video_Mode();
bool DirectDraw7_Set_Video_Mode(HWND hWnd, int width, int height, int bits_per_pixel);
void DirectDraw7_Check_Overlapped_Blit_Capability();
bool DirectDraw7_Prep(HWND hWnd);
void DirectDraw7_Wait_Blit();
void DirectDraw7_Flip(XSurface *surface);
bool DirectDraw7_Can_Blit();
bool DirectDraw7_Can_Flip();
bool DirectDraw7_Is_Surface_Lost();
void DirectDraw7_Focus_Loss();
void DirectDraw7_Focus_Restore();
bool DirectDraw7_Clear_Surface(LPDIRECTDRAWSURFACE7 lpSurface);

#endif
