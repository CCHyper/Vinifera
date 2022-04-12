/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          RENDERER_SUPPORT.H
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


class CCINIClass;
class Rect;
class XSurface;


/**
 *  
 */
namespace Renderer
{

extern bool UseDirectDraw7;

extern int FrameLimit;
extern bool BorderlessWindow;
extern bool ClipCursorToWindow;

bool Read_INI(CCINIClass &ini);
bool Write_INI(CCINIClass &ini);

void __cdecl Shutdown();
void Reset_Video_Mode();
bool Set_Video_Mode(HWND hWnd, int width, int height, int bits_per_pixel);
void Check_Overlapped_Blit_Capability();
bool Prep_Renderer(HWND hWnd);
void Release(HWND hWnd);
void Wait_Blit();
bool Allocate_Surfaces(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect);
XSurface *Create_Primary(XSurface **backbuffer_surface = nullptr);
bool Flip(XSurface *surface);
void Sync_Delay();
void Focus_Loss();
void Focus_Restore();
bool Clear_Screen();
void Set_Cursor_Clip();
void Clear_Cursor_Clip();
bool Frame_Limiter(bool force_blit);

};

#endif
