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
#include "dx7_driver.h"
#include "vinifera_globals.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "tibsun_resource.h"
#include "optionsext.h"
#include "resource.h"
#include "virtualsurface.h"
#include "textprint.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <Windows.h>
#include <commctrl.h>

#include "ddraw7_util.h"

#include "directx_window.h"


extern HMODULE DLLInstance;




#include "tspp.h"
DEFINE_IMPLEMENTATION(int Get_Video_Hardware_Capabilities(), 0x00473120);

bool &OverlappedVideoBlits = Make_Global<bool>(0x006F980D);
int &VideoWidth = Make_Global<int>(0x007A1EC0);
int &VideoHeight = Make_Global<int>(0x007A1EC4);
int &BitsPerPixel = Make_Global<int>(0x007A1EC8);




LPDIRECTDRAW7 DirectDraw7Object = nullptr;
LPDIRECTDRAWSURFACE7 DirectDraw7PrimarySurface = nullptr;       // 32bit primary
LPDIRECTDRAWSURFACE7 DirectDraw7BackSurface = nullptr;          // 32bit back
LPDIRECTDRAWCLIPPER DirectDraw7Clipper = nullptr;               // window clipper
LPDDSURFACEDESC2 VideoSurfaceDescription = nullptr;



bool SystemToVideoBlits = false;
bool VideoToSystemBlits = false;
bool SystemToSystemBlits = false;

int PrimaryBytesPerPixel = 0;

bool TripleBuffering = true;

//VirtualSurface *BackBufferSurface = nullptr;   // not needed?

PALETTEENTRY PaletteEntries[256];
LPDIRECTDRAWPALETTE	PalettePtr = nullptr;

bool FirstPaletteSet = false;

bool CanVblankSync = true;

unsigned char CurrentPalette[768] = { 255 };



/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Close_Window(bool force)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Destroy_Window(bool force)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Show_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Hide_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Minimize_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Maximize_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX7VideoDriver::Toggle_Fullscreen()
{
    /**
     *  Is borderless window?
     */
    if (IsBorderlessWindow && (GetWindowLongPtr(MainWindow, GWL_STYLE) & WS_POPUP)) {
        SetWindowLongPtr(MainWindow, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
        SetWindowPos(MainWindow, nullptr, 0, 0, WindowWidth, WindowHeight, SWP_FRAMECHANGED);

    /**
     *  Fullscreen
     */
    } else {
        int w = GetSystemMetrics(SM_CXSCREEN);
        int h = GetSystemMetrics(SM_CYSCREEN);
        SetWindowLongPtr(MainWindow, GWL_STYLE, WS_VISIBLE | WS_POPUP);
        SetWindowPos(MainWindow, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Focus_Loss()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Focus_Restore()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Set_Cursor_Clip()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Clear_Cursor_Clip()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX7VideoDriver::Update_Screen(XSurface *surface, Rect *src_rect, Rect *dest_rect)
{
    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX7VideoDriver::Clear_Screen(bool present)
{
    return true;
}

bool DirectX7VideoDriver::Set_Video_Mode(HWND hWnd, int width, int height, int bits_per_pixel)
{
    //DEBUG_INFO("Set_Video_Mode(enter)\n");

    if (!Prep_Renderer(hWnd)) {
        DEBUG_ERROR("DirectDraw7 - Failed to prepare DirectDraw!\n");
        return false;
    }

    HRESULT ddrval;

    /**
     *  Set the required display mode if full screen was requested.
     */
    if (!IsWindowed) {
        ddrval = DirectDraw7Object->SetDisplayMode(width, height, bits_per_pixel, 0, 0);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("Set_Video_Mode()\n\nSetDisplayMode failed with error code %08X\n", ddrval);
            DirectDraw7Object->Release();
            DirectDraw7Object = nullptr;
            return false;
        }

        DEBUG_INFO("DirectDraw7 - Display mode set to %d x %d - %d.\n", width, height, bits_per_pixel);
    }

    VideoWidth = width;
    VideoHeight = height;
    BitsPerPixel = bits_per_pixel;

    /**
     *  Create a direct draw palette object.
     */
    if (bits_per_pixel != 8) {
        ;
        ddrval = DirectDraw7Object->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, &PaletteEntries[0], &PalettePtr, nullptr);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("Set_Video_Mode()\n\nCreatePalette failed with error code %08X\n", ddrval);
            DirectDraw7Object->Release();
            DirectDraw7Object = nullptr;
            return false;
        }

        DEBUG_INFO("DirectDraw7 - Palette created (8bit, 256 colors).\n");

        Check_Overlapped_Blit_Capability();
    }

    DDCAPS capabilities;
    DDCAPS emulated_capabilities;
    DDRAW_INIT_STRUCT(capabilities);
    DDRAW_INIT_STRUCT(emulated_capabilities);

    ddrval = DirectDraw7Object->GetCaps(&capabilities, &emulated_capabilities);
    if (FAILED(ddrval)) {
        DDRAW_ERROR_MSGBOX("Set_Video_Mode()\n\nGetCaps failed with error code %08X\n", ddrval);
        return false;
    }

    SystemToVideoBlits = false;
    VideoToSystemBlits = false;
    SystemToSystemBlits = false;

    if (capabilities.dwCaps & DDCAPS_CANBLTSYSMEM) {
        SystemToVideoBlits = (capabilities.dwSVBCaps & DDCAPS_BLT) ? true : false;
        VideoToSystemBlits = (capabilities.dwVSBCaps & DDCAPS_BLT) ? true : false;
        SystemToSystemBlits = (capabilities.dwSSBCaps & DDCAPS_BLT) ? true : false;
    }

    DEBUG_INFO("DirectDraw7 - SystemToVideoBlits = %s\n", SystemToVideoBlits ? "true" : "false");
    DEBUG_INFO("DirectDraw7 - VideoToSystemBlits = %s\n", VideoToSystemBlits ? "true" : "false");
    DEBUG_INFO("DirectDraw7 - SystemToSystemBlits = %s\n", SystemToSystemBlits ? "true" : "false");

    //DEBUG_INFO("Set_Video_Mode(exit)\n");

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Reset_Video_Mode()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Release(HWND hWnd)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX7VideoDriver::Prep_Renderer(HWND hWnd)
{
    //DEBUG_INFO("Prep_Renderer(enter)\n");

    /**
     *  If there is not currently a direct draw object then we need to define one.
     */
    if (DirectDraw7Object == nullptr) {

        DEBUG_INFO("DirectDraw7 - Creating DirectDraw7Object.\n");

        HRESULT ddrval;

        /**
         *  Create the Direct Draw object. This is used to access anything DirectDraw does.
         */
        ddrval = DirectDrawCreateEx(nullptr, (LPVOID*)&DirectDraw7Object, IID_IDirectDraw7, nullptr);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("Prep_Renderer()\n\nDirectDrawCreateEx failed with error code %08X\n", ddrval);
            DirectDraw7Object = nullptr;
            return false;
        }

        DEBUG_INFO("DirectDraw7 - DirectDraw7Object objected created sucessfully.\n");

        if (IsWindowed) {
            ddrval = DirectDraw7Object->SetCooperativeLevel(nullptr, DDSCL_NORMAL);
        }
        else {
            ddrval = DirectDraw7Object->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
        }
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("Prep_Renderer()\n\nSetCooperativeLevel failed with error code %08X\n", ddrval);
            DirectDraw7Object->Release();
            DirectDraw7Object = nullptr;
            return false;
        }
    }

    //DEBUG_INFO("Prep_Renderer(exit)\n");

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Check_Overlapped_Blit_Capability()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Wait_Blit()
{
    HRESULT	return_code;

    ASSERT(DirectDraw7PrimarySurface != nullptr);

    do {
        return_code = DirectDraw7PrimarySurface->GetBltStatus(DDGBS_ISBLTDONE);
    } while (return_code != DD_OK && return_code != DDERR_SURFACELOST);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX7VideoDriver::Flip(XSurface *surface)
{
    ASSERT(surface != nullptr);

    if (!Can_Flip()) {
        DEBUG_WARNING("DirectDraw7 - Not ready to flip yet!\n");
        return false;
    }

    HRESULT ddrval = 0;

    DDSURFACEDESC2 ddsDesc;
    DDRAW_INIT_STRUCT(ddsDesc);

    /**
     *  Lock the surfaces before we access them.
     */
    ddrval = DirectDraw7BackSurface->Lock(nullptr, &ddsDesc, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, nullptr);
    if (FAILED(ddrval)) {
        DEBUG_ERROR("DirectDraw7 - Failed to lock direct draw surface!\n");
        return false;
    }

    unsigned short *srcptr = reinterpret_cast<unsigned short *>(surface->Lock());
    if (!srcptr) {
        DEBUG_ERROR("DirectDraw7 - Failed to lock game surface!\n");
        return false;
    }

    /**
     *  Copy pixel data from the games 16bit surface to the DirectDraw 32bit surface.
     */
#if 0
    unsigned char *dstptr = reinterpret_cast<unsigned char *>(ddsDesc.lpSurface);
    for (int i = 0; i < (surface->Get_Width() * surface->Get_Height()); ++i) {
        unsigned short s = *(srcptr++);
        unsigned char r = (s & 0xF800) >> 11; // Extract the 5 R bits
        unsigned char g = (s & 0x07E0) >> 5;  // Extract the 6 G bits
        unsigned char b = (s & 0x001F);       // Extract the 5 B bits
        *dstptr++ = (b << 3) | (b & 7);       // Set the 8 B bit.
        *dstptr++ = (g << 2) | (g & 3);       // Set the 8 G bit.
        *dstptr++ = (r << 3) | (r & 7);       // Set the 8 R bit.
        *dstptr++ = 255;                      // Set the 8 A bit.
    }
#else
    unsigned long *dstptr = reinterpret_cast<unsigned long *>(ddsDesc.lpSurface);
    for (int i = 0; i < (surface->Width * surface->Height); ++i) {
        unsigned short src_pixel = *(srcptr++);
        int r = (src_pixel >> 11) & 0x1F;
        r = ((r << 3) | (r & 7)) << 16;

        int g = (src_pixel >> 5) & 0x3F;
        g = ((g << 2) | (g & 3)) << 8;

        int b = src_pixel & 0x1F;
        b = (b << 3) | (b & 7);

        *dstptr++ = (r | g | b);
    }
#endif

    // TODO: DOES NOT WORK
#if 0
    for (int y = 0; y < surface->Get_Height(); ++y) {
        DirectDraw7_Copy_Row_RGB565_To_RGB888(srcptr, dstptr, surface->Get_Width());
    }
#endif

    /**
     *  Finished with the surfaces, unlock them.
     */
    surface->Unlock();
    DirectDraw7BackSurface->Unlock(nullptr);

    /**
     *  Now flip the back surface to the primary.
     *
     *  For windowed mode; Just blit directory to the surface.
     *  For full scree mode; Flip the back surface to the primary, DirectDraw will do the rest.
     */
    if (IsWindowed) {

        if (!Can_Blit()) {
            DEBUG_WARNING("DirectDraw7 - Unable to blit to primary surface at this time!\n");
            return false;
        }

        /**
         *  Find out where on the primary surface our window lives and move
         *  and move it to the correct screen offset.
         */
        RECT rcSrc;  // source blit rectangle
        RECT rcDest; // destination blit rectangle
        POINT p{ 0, 0 };

        ClientToScreen(MainWindow, &p);
        GetClientRect(MainWindow, &rcDest);
        OffsetRect(&rcDest, p.x, p.y);
        SetRect(&rcSrc, 0, 0, Options.ScreenWidth, Options.ScreenHeight);

        ddrval = DirectDraw7PrimarySurface->Blt(&rcDest, DirectDraw7BackSurface, &rcSrc, DDBLT_WAIT, nullptr);
        if (FAILED(ddrval)) {
            DEBUG_ERROR("DirectDraw7 - Blt failed with error code %08X\n", ddrval);
        }

    } else {

        if (!Can_Flip()) {
            DEBUG_WARNING("DirectDraw7 - Unable to flip primary surface at this time!\n");
            return false;
        }

        ddrval = DirectDraw7PrimarySurface->Flip(nullptr, DDFLIP_WAIT);
        if (FAILED(ddrval)) {
            DEBUG_ERROR("DirectDraw7 - Flip failed with error code %08X\n", ddrval);
        }

    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX7VideoDriver::Shutdown()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX7VideoDriver::Internal_Create_Primary(Surface *primary_surface, XSurface **backbuffer_surface)
{
    //DEBUG_INFO("DirectDraw7_Create_DirectDraw_Surface(enter)\n");

    if (!DirectDraw7Object) {
        DEBUG_ERROR("DirectDraw7 - DirectDraw7Object is null!\n");
        MessageBox(MainWindow, "DirectDraw7Object is null!", "Tiberian Sun", MB_OK);
        Fatal("DirectDraw7 - DirectDraw7Object is null!\n");
        return false;
    }

    unsigned int caps = Get_Video_Hardware_Capabilities();

    DSurface::AllowStretchBlits = (caps & VIDEO_ALIGN_BOUNDARY) ? true : false;
    DEBUG_INFO("DirectDraw7 - DSurface::AllowStretchBlits = %s\n", DSurface::AllowStretchBlits ? "true" : "false");

    DSurface::AllowHardwareBlitFills = (caps & VIDEO_COLOR_FILL) ? true : false;
    DEBUG_INFO("DirectDraw7 - DSurface::AllowHardwareBlitFills = %s\n", DSurface::AllowHardwareBlitFills ? "true" : "false");

    HRESULT ddrval = 0;

    VideoSurfaceDescription = new DDSURFACEDESC2;
    if (!VideoSurfaceDescription) {
        DDRAW_ERROR_MSGBOX("DirectDraw7\nFailed to create surface description!\n", ddrval);
        Fatal("DirectDraw7 - Failed to create surface description!\n");
        return false;
    }

    DDRAW_INIT_STRUCT_PTR(VideoSurfaceDescription);

    VideoSurfaceDescription->dwFlags = DDSD_CAPS;
    VideoSurfaceDescription->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if (!IsWindowed) {
        VideoSurfaceDescription->dwFlags |= DDSD_BACKBUFFERCOUNT;
        VideoSurfaceDescription->ddsCaps.dwCaps |= DDSCAPS_FLIP;
        VideoSurfaceDescription->ddsCaps.dwCaps |= DDSCAPS_COMPLEX;
        VideoSurfaceDescription->dwBackBufferCount = 1;
    }

    //VideoSurfaceDescription->dwFlags |= DDSD_WIDTH | DDSD_HEIGHT;
    VideoSurfaceDescription->dwWidth = Options.ScreenWidth;
    VideoSurfaceDescription->dwHeight = Options.ScreenHeight;

    ddrval = DirectDraw7Object->CreateSurface(VideoSurfaceDescription, &DirectDraw7PrimarySurface, nullptr);
    if (FAILED(ddrval)) {
        DDRAW_ERROR_MSGBOX("DirectDraw7\nCreateSurface failed with error code %08X\n", ddrval);
        Fatal("DirectDraw7 - CreateSurface failed with error code %08X\n", ddrval);
        return false;
    }

    DDSURFACEDESC2 ddsc;
    DDRAW_INIT_STRUCT(ddsc);

    ddrval = DirectDraw7PrimarySurface->GetSurfaceDesc(&ddsc);
    if (FAILED(ddrval)) {
        DDRAW_ERROR_MSGBOX("DirectDraw7\nGetSurfaceDesc failed with error code %08X\n", ddrval);
        Fatal("DirectDraw7 - GetSurfaceDesc failed with error code %08X\n", ddrval);
        return false;
    }

    DEBUG_INFO("DirectDraw7 - Bytes per pixel to %d\n", ((ddsc.ddpfPixelFormat.dwRGBBitCount + 7) / 8));
    DEBUG_INFO("DirectDraw7 - In video memory = %s\n", ((ddsc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0) ? "true" : "false");
    DEBUG_INFO("DirectDraw7 - Surface dimensions to %d x %d.\n", ddsc.dwWidth, ddsc.dwHeight);

    if (IsWindowed) {

        ddrval = DirectDraw7Object->CreateClipper(0, &DirectDraw7Clipper, nullptr);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("DirectDraw7\nCreateClipper failed with error code %08X\n", ddrval);
            Fatal("DirectDraw7 - CreateClipper failed with error code %08X\n", ddrval);
            return false;
        }

        ddrval = DirectDraw7Clipper->SetHWnd(0, MainWindow);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("DirectDraw7\nSetHWnd failed with error code %08X\n", ddrval);
            Fatal("DirectDraw7 - SetHWnd failed with error code %08X\n", ddrval);
            return false;
        }

        ddrval = DirectDraw7PrimarySurface->SetClipper(DirectDraw7Clipper);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("DirectDraw7\nSetClipper failed with error code %08X\n", ddrval);
            Fatal("DirectDraw7 - SetClipper failed with error code %08X\n", ddrval);
            return false;
        }

        DDSURFACEDESC2 ddsDesc;
        DDRAW_INIT_STRUCT(ddsDesc);

        ddsDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        ddsDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        ddsDesc.dwWidth = Options.ScreenWidth;
        ddsDesc.dwHeight = Options.ScreenHeight;

        ddrval = DirectDraw7Object->CreateSurface(&ddsDesc, &DirectDraw7BackSurface, nullptr);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("DirectDraw7\nCreateSurface failed with error code %08X\n", ddrval);
            Fatal("DirectDraw7 - CreateSurface failed with error code %08X\n", ddrval);
            return false;
        }

    } else {

        DDSCAPS2 ddsCaps;
        ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

        ddrval = DirectDraw7PrimarySurface->GetAttachedSurface(&ddsCaps, &DirectDraw7BackSurface);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("DirectDraw7\nGetAttachedSurface failed with error code %08X\n", ddrval);
            Fatal("DirectDraw7 - GetAttachedSurface failed with error code %08X\n", ddrval);
            return false;
        }

    }

    /**
     *  Print surface info.
     */
    DDSURFACEDESC2 ddsDesc;
    DDRAW_INIT_STRUCT(ddsDesc);

    DirectDraw7PrimarySurface->GetSurfaceDesc(&ddsDesc);
    DEBUG_INFO("DirectDraw7 - Primary surface info:\n", ddsDesc.ddpfPixelFormat.dwRGBBitCount);
    DEBUG_INFO("  dwRGBBitCount = %d\n", ddsDesc.ddpfPixelFormat.dwRGBBitCount);
    DEBUG_INFO("  dwRBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwRBitMask);
    DEBUG_INFO("  dwGBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwGBitMask);
    DEBUG_INFO("  dwBBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwBBitMask);
    DEBUG_INFO("  dwWidth = %d\n", ddsDesc.dwWidth);
    DEBUG_INFO("  dwHeight = %d\n", ddsDesc.dwHeight);

    DirectDraw7BackSurface->GetSurfaceDesc(&ddsDesc);
    DEBUG_INFO("DirectDraw7 - Back surface info:\n", ddsDesc.ddpfPixelFormat.dwRGBBitCount);
    DEBUG_INFO("  dwRGBBitCount = %d\n", ddsDesc.ddpfPixelFormat.dwRGBBitCount);
    DEBUG_INFO("  dwRBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwRBitMask);
    DEBUG_INFO("  dwGBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwGBitMask);
    DEBUG_INFO("  dwBBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwBBitMask);
    DEBUG_INFO("  dwWidth = %d\n", ddsDesc.dwWidth);
    DEBUG_INFO("  dwHeight = %d\n", ddsDesc.dwHeight);

    /**
     *  Clear out both primary and secondary surfaces.
     */
    Clear_Surface(DirectDraw7PrimarySurface);
    Clear_Surface(DirectDraw7BackSurface);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX7VideoDriver::Internal_Create_Window(HINSTANCE hInstance)
{
#if 0
    HWND hWnd = nullptr;
    BOOL rc;
    WNDCLASSEX wc;
    tagRECT rect;
    HICON hIcon = nullptr;
    HICON hSmIcon = nullptr;
    HCURSOR hCursor = nullptr;

    //DEV_DEBUG_INFO("Create_Window() - About to call InitCommonControls()\n");

    InitCommonControls();

    //DEV_DEBUG_INFO("Create_Window() - Preparing window name (with version info).\n");

    DWORD dwPid = GetProcessId(GetCurrentProcess());
    if (!dwPid) {
        DEBUG_ERROR("Create_Window() - Failed to get the process id!\n");
        return false;
    }

    //DEV_DEBUG_INFO("Create_Window() - Loading icon and cursor resources.\n");

    /**
     *  Load the Vinifera icon and cursor resources, falling back to the GAME.EXE
     *  resources if not available or failed to load.
     */
    if (Vinifera_IconName[0] != '\0') {
        DEBUG_INFO("Loading custom icon \"%s\"\n", Vinifera_IconName);
        hIcon = (HICON)LoadImage(
            nullptr,
            Vinifera_IconName,
            IMAGE_ICON,
            0,
            0,
            LR_LOADFROMFILE);
        DEBUG_INFO("Loading custom small icon \"%s\"\n", Vinifera_IconName);
        hSmIcon = (HICON)LoadImage(
            nullptr,
            Vinifera_IconName,
            IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON),
            GetSystemMetrics(SM_CXSMICON),
            LR_LOADFROMFILE);
    }
    if (!hIcon) {
        hIcon = LoadIcon((HINSTANCE)DLLInstance, MAKEINTRESOURCE(VINIFERA_MAINICON));
        if (!hIcon) {
            hIcon = LoadIcon((HINSTANCE)hInstance, MAKEINTRESOURCE(IDI_TIBSUN));
        }
    }
    if (Vinifera_CursorName[0] != '\0') {
        DEBUG_INFO("Loading custom cursor \"%s\"\n", Vinifera_CursorName);
        hCursor = LoadCursorFromFile(Vinifera_CursorName);
    }
    if (!hCursor) {
        hCursor = LoadCursor(nullptr, VINIFERA_MAINCURSOR); // IDC_ARROW is a system resource, does not require module.
        if (!hCursor) {
            hCursor = LoadCursor((HINSTANCE)hInstance, MAKEINTRESOURCE(IDC_TIBSUN_ARROW));
        }
    }

    //DEV_DEBUG_INFO("Create_Window() - Setting up window class info.\n");

    /**
     *  Register the window class.
     */
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Main_Window_Procedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = (HINSTANCE)hInstance;
    wc.hIcon = hIcon;
    wc.hCursor = hCursor;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "Vinifera";
    wc.hIconSm = (hSmIcon ? hSmIcon : hIcon);

    //DEV_DEBUG_INFO("Create_Window() - About to call RegisterClass()\n");

    /**
     *  Register window class.
     */
    rc = RegisterClassEx(&wc);
    if (!rc) {
        DEBUG_INFO("Create_Window() - Failed to register window class!\n");
        return false;
    }

    /**
     *  Get the dimensions of the primary display.
     */
    int display_width = GetSystemMetrics(SM_CXSCREEN);
    int display_height = GetSystemMetrics(SM_CYSCREEN);

    //DEV_DEBUG_INFO("Create_Window() - Desktop size %d x %d\n", display_width, display_height);

    /**
     *  Create our main window.
     */
    if (IsWindowed) {

        DEBUG_INFO("Create_Window() - Creating desktop window (%d x %d).\n", WindowWidth, WindowHeight);

        hWnd = CreateWindowEx(
            WS_EX_LEFT/*|WS_EX_TOPMOST*/,   // Removed: Causes focus issues when debugging with MSVC.
            "Vinifera",
            WindowTitle.Peek_Buffer(),
            IsBorderlessWindow ? WS_POPUP : WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CAPTION,
            0, 0, 0, 0,
            nullptr,
            nullptr,
            (HINSTANCE)hInstance,
            nullptr);

        SetRect(&rect, 0, 0, WindowWidth, WindowHeight);

        AdjustWindowRectEx(&rect,
            GetWindowLong(hWnd, GWL_STYLE),
            GetMenu(hWnd) != nullptr,
            GetWindowLong(hWnd, GWL_EXSTYLE));

        /**
         *  #BUGFIX:
         *
         *  Fetch the desktop size, calculate the screen center position the window and move it.
         */
        RECT workarea;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);

        int x_pos = (display_width - WindowWidth) / 2;
        int y_pos = (((display_height - WindowHeight) / 2) - (display_height - workarea.bottom));

        DEBUG_INFO("Create_Window() - Moving window (%d,%d,%d,%d).\n",
            x_pos, y_pos, (rect.right - rect.left), (rect.bottom - rect.top));

        MoveWindow(hWnd, x_pos, y_pos, (rect.right - rect.left), (rect.bottom - rect.top), TRUE);

    }
    else {

        DEBUG_INFO("Create_Window() - Creating fullscreen window.\n");

        hWnd = CreateWindowEx(
            WS_EX_LEFT | WS_EX_TOPMOST,
            "Vinifera",
            WindowTitle.Peek_Buffer(),
            WS_POPUP | WS_CLIPCHILDREN,
            0, 0,
            display_width,
            display_height,
            nullptr,
            nullptr,
            (HINSTANCE)hInstance,
            nullptr);
    }

    if (!hWnd) {
        DEBUG_INFO("Create_Window() - Failed to create window!\n");
        return false;
    }

    MainWindow = hWnd;

    //DEV_DEBUG_INFO("Create_Window() - About to call ShowWindow()\n");

    ShowWindow(hWnd, SW_SHOWNORMAL);
    ShowCommand = TRUE; // nCmdShow

    //DEV_DEBUG_INFO("Create_Window() - About to call UpdateWindow()\n");

    UpdateWindow(hWnd);

    //DEV_DEBUG_INFO("Create_Window() - About to call SetFocus()\n");

    SetFocus(hWnd);

    //DEV_DEBUG_INFO("Create_Window() - About to call RegisterHotKey()\n");

    RegisterHotKey(hWnd, 1, MOD_ALT | MOD_CONTROL | MOD_SHIFT, VK_M);

    //DEV_DEBUG_INFO("Create_Window() - About to call SetCursor()\n");

    SetCursor(hCursor);
#endif

    return DirectX_Create_Window(this, hInstance);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX7VideoDriver::Clear_Surface(LPDIRECTDRAWSURFACE7 lpSurface)
{
    if (!lpSurface) {
        return false;
    }

    DEBUG_INFO("DirectDraw7 - Clearing surface.\n");

    DDBLTFX ddbltfx;
    DDRAW_INIT_STRUCT(ddbltfx);

    ddbltfx.dwFillColor = 0;

    HRESULT hr = lpSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
    return SUCCEEDED(hr);
}


/**
 *  Is the surface ready to immediately accept blitting?
 *
 *  @author: CCHyper
 */
bool DirectX7VideoDriver::Can_Blit()
{
    return DirectDraw7PrimarySurface && DirectDraw7PrimarySurface->GetBltStatus(DDGBS_CANBLT) == DD_OK;
}


/**
 *  Is the surface ready to immediately flip?
 *
 *  @author: CCHyper
 */
bool DirectX7VideoDriver::Can_Flip()
{
    return DirectDraw7PrimarySurface && DirectDraw7PrimarySurface->GetFlipStatus(DDGFS_CANFLIP) == DD_OK;
}
