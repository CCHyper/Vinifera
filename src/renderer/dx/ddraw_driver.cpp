/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DDRAW_DRIVER.CPP
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
#include "ddraw_driver.h"
#include "tibsun_functions.h"
#include "dsurface.h"

void DirectDrawVideoDriver::Close_Window(bool force)
{
}

void DirectDrawVideoDriver::Destroy_Window(bool force)
{
}

void DirectDrawVideoDriver::Show_Window()
{
}

void DirectDrawVideoDriver::Hide_Window()
{
}

void DirectDrawVideoDriver::Minimize_Window()
{
}

void DirectDrawVideoDriver::Maximize_Window()
{
}

bool DirectDrawVideoDriver::Toggle_Fullscreen()
{
    return false;
}

void DirectDrawVideoDriver::Focus_Loss()
{
    ::Focus_Loss();
}

void DirectDrawVideoDriver::Focus_Restore()
{
    ::Focus_Restore();
}

void DirectDrawVideoDriver::Set_Cursor_Clip()
{
}

void DirectDrawVideoDriver::Clear_Cursor_Clip()
{
}

bool DirectDrawVideoDriver::Update_Screen(XSurface *surface, Rect *src_rect, Rect *dest_rect)
{
    return false;
}

bool DirectDrawVideoDriver::Clear_Screen(bool present)
{
    return false;
}

bool DirectDrawVideoDriver::Set_Video_Mode(HWND hWnd, int width, int height, int bits_per_pixel)
{
    return ::Set_Video_Mode(hWnd, width, height, bits_per_pixel);
}

void DirectDrawVideoDriver::Reset_Video_Mode()
{
    ::Reset_Video_Mode();
}

void DirectDrawVideoDriver::Release(HWND hWnd)
{
}

bool DirectDrawVideoDriver::Prep_Renderer(HWND hWnd)
{
    Prep_Direct_Draw();
    return true;
}

void DirectDrawVideoDriver::Check_Overlapped_Blit_Capability()
{
    //::Check_Overlapped_Blit_Capability();
}

void DirectDrawVideoDriver::Wait_Blit()
{
    ::Wait_Blit();
}

bool DirectDrawVideoDriver::Flip(XSurface *surface)
{
    return true;
}

bool DirectDrawVideoDriver::Frame_Limiter(bool force_blit)
{
    return false;
}

void DirectDrawVideoDriver::Shutdown()
{
}

bool DirectDrawVideoDriver::Internal_Create_Primary(Surface *primary_surface, XSurface **backbuffer_surface)
{
    primary_surface = DSurface::Create_Primary((DSurface **)backbuffer_surface);
    return true;
}

bool DirectDrawVideoDriver::Internal_Create_Window(HINSTANCE hInstance)
{
    return Create_Main_Window(hInstance, 0, 640, 400);
}
