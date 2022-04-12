/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DIRECTDRAW7_DRIVER.H
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

#if defined(DX7_RENDERER)

#include "always.h"
#include "video_driver.h"
#include "ddraw7_util.h"


class DirectX7VideoDriver : public VideoDriver
{
    public:
        DirectX7VideoDriver() : VideoDriver("DirectDraw7") {}
        virtual ~DirectX7VideoDriver() {}

        virtual void Close_Window(bool force = false);
        virtual void Destroy_Window(bool force = false);

        virtual void Show_Window();
        virtual void Hide_Window();

        virtual void Minimize_Window();
        virtual void Maximize_Window();

        virtual bool Toggle_Fullscreen();

        virtual void Focus_Loss();
        virtual void Focus_Restore();

        virtual void Set_Cursor_Clip();
        virtual void Clear_Cursor_Clip();

        virtual bool Update_Screen(XSurface *surface, Rect *src_rect, Rect *dest_rect);
        virtual bool Clear_Screen(bool present = false);

        virtual bool Set_Video_Mode(HWND hWnd, int width = 640, int height = 400, int bits_per_pixel = 32);
        virtual void Reset_Video_Mode();

        virtual void Release(HWND hWnd);
        virtual bool Prep_Renderer(HWND hWnd);

        virtual void Check_Overlapped_Blit_Capability();
        virtual void Wait_Blit();

        virtual bool Flip(XSurface *surface);

        virtual void Shutdown();

    private:
        virtual bool Internal_Create_Primary(Surface *primary_surface, XSurface **backbuffer_surface = nullptr);
        virtual bool Internal_Create_Window(HINSTANCE hInstance);

        bool Clear_Surface(LPDIRECTDRAWSURFACE7 lpSurface);
        bool Can_Blit();
        bool Can_Flip();

    private:
};

#endif
