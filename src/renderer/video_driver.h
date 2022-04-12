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

#include "always.h"
#include "wstring.h"


class Surface;
class XSurface;
class Rect;


class VideoDriver
{
    public:
        VideoDriver(Wstring driver_name);
        virtual ~VideoDriver() {}

        virtual Wstring &Get_Name() { return DriverName; }

        virtual bool Supports_3D() const { return false; }

        virtual bool Create_Window(Wstring window_title, HINSTANCE hInstance, int width, int height, int refresh_rate = 60, bool windowed = false, bool borderless = false);
        virtual void Close_Window(bool force = false) = 0;
        virtual void Destroy_Window(bool force = false) = 0;

        virtual void Show_Window() = 0;
        virtual void Hide_Window() = 0;

        virtual void Minimize_Window() = 0;
        virtual void Maximize_Window() = 0;

        virtual bool Toggle_Fullscreen() = 0;

        virtual void Focus_Loss() = 0;
        virtual void Focus_Restore() = 0;

        virtual void Set_Cursor_Clip() = 0;
        virtual void Clear_Cursor_Clip() = 0;

        virtual bool Update_Screen(XSurface *surface, Rect *src_rect, Rect *dest_rect) = 0;
        virtual bool Clear_Screen(bool present = false) = 0;

        virtual XSurface *Create_Primary(XSurface **backbuffer_surface = nullptr);
        virtual bool Allocate_Surfaces(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect);

        virtual bool Set_Video_Mode(HWND hWnd, int width = 640, int height = 400, int bits_per_pixel = 32) = 0;
        virtual void Reset_Video_Mode() = 0;

        virtual void Release(HWND hWnd) = 0;
        virtual bool Prep_Renderer(HWND hWnd) = 0;

        virtual void Check_Overlapped_Blit_Capability() = 0;
        virtual void Wait_Blit() = 0;

        virtual bool Flip(XSurface *surface) = 0;

        virtual bool Frame_Limiter(bool force_blit);

        virtual void Shutdown() = 0;

    protected:
        virtual bool Internal_Create_Primary(Surface *primary_surface, XSurface **backbuffer_surface = nullptr) = 0;
        virtual bool Internal_Create_Window(HINSTANCE hInstance) = 0;

    public:
        bool Is_Windowed() const { return IsWindowed; }
        bool Is_Borderless_Window() const { return IsBorderlessWindow; }
        int Window_Width() const { return WindowWidth; }
        int Window_Height() const { return WindowHeight; }

    private:
        /**
         *  
         */
        Wstring DriverName;

    protected:
        /**
         *
         */
        Wstring WindowTitle;

        /**
         *
         */
        bool IsWindowed;

        /**
         *  Create the window without a border?
         */
        bool IsBorderlessWindow;

        /**
         *  Create the window at the size of the display as a borderless window?
         */

        /**
         *  
         */
        bool IsClipCursorToWindow;

        /**
         *
         */
        int WindowWidth;
        int WindowHeight;

        /**
         *
         */
        int RefreshRate;

        /**
         *
         */
        int FrameLimit;

        /**
         *
         */
        int MonitorToUse;

        /**
         *
         */
        bool IsUseHardwareRenderer;
};


void __cdecl Set_Video_Driver(VideoDriver *driver);
void __cdecl Remove_Video_Driver();

VideoDriver * __cdecl Video_Driver();

bool Video_Driver_Is_Direct_Draw();
