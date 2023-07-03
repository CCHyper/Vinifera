/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          D3D_HOOKS.CPP
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
#include "d3d_hooks.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "dsurface.h"
#include "options.h"
#include "optionsext.h"
#include "movie.h"
#include "filepng.h"
#include "fatal.h"
#include "d3d_util.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"

#include <ddraw.h>
#include <d3d11.h>



 /**
  *  x
  */
//static LPDIRECT3D9 Direct3D = nullptr;
//static LPDIRECT3DDEVICE9 Direct3DDevice = nullptr;
//static LPDIRECT3DSURFACE9 Direct3DPrimarySurface = nullptr;
//static LPDIRECT3DSURFACE9 Direct3DBackSurface = nullptr;

/**
 *  x
 */
//static LPDIRECTDRAWSURFACE DirectDrawBackSurface = nullptr;

/**
 *  x
 */
static DSurface *BackSurface = nullptr;

static bool DoubleBackBuffer = false;


extern int & VideoWidth = Make_Global<int>(0x007A1EC0);
extern int & VideoHeight = Make_Global<int>(0x007A1EC4);
extern int & BitsPerPixel = Make_Global<int>(0x007A1EC8);

#define DDRAW_INIT_STRUCT(dxstruct)          ZeroMemory(&dxstruct, sizeof(dxstruct)); dxstruct.dwSize = sizeof(dxstruct);
#define DDRAW_INIT_STRUCT_PTR(dxstruct)      ZeroMemory(dxstruct, sizeof(*dxstruct)); dxstruct->dwSize = sizeof(*dxstruct);

#define DDRAW_ERROR_LOG(str, val)            if (val != DD_OK) { DEBUG_INFO(str, val); }

#define DDRAW_ERROR_MSGBOX(str, val, ...) \
            if (val != DD_OK) { \
                DD_Error_Message_Box(val, str, ##__VA_ARGS__); \
            }

/**
 *  Displays a message box containing the given formatted string.
 */
static void DD_Error_Message_Box(HRESULT ddres, char const *fmt, ...)
{
    char buff[1024];

    va_list args;
    va_start(args, fmt);

    std::snprintf(buff, sizeof(buff), fmt, ddres, args);

    va_end(args);

    std::strcat(buff, "\r\n");

    MessageBox(MainWindow, buff, "DirectX Error", MB_ICONEXCLAMATION|MB_OK);
}



#ifndef NDEBUG
void __cdecl D3D_Write_Surface_Data_To_File(const char *filename, DSurface *surface)
{
#if 1 // #ifndef NDEBUG
    Write_PNG_File(&RawFileClass(filename), *surface, &GamePalette);
#endif
}
#endif



static bool Allocate_Surfaces_Intercept(Rect* common_rect, Rect* composite_rect, Rect* tile_rect, Rect* sidebar_rect)
{
    DEBUG_INFO("Allocating game surfaces...\n");
    /**
     *  Delete existing surface instances.
     */
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

    /**
     *  Create new instances of the surfaces using the Virtual interface.
     */
    if (common_rect->Width > 0 && common_rect->Height > 0) {
        AlternateSurface = new DSurface(common_rect->Width, common_rect->Height, 2);
        AlternateSurface->Clear();
        DEBUG_INFO("AlternateSurface created (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (common_rect->Width > 0 && common_rect->Height > 0) {
        HiddenSurface = new DSurface(common_rect->Width, common_rect->Height, 2);
        HiddenSurface->Clear();
        DEBUG_INFO("HiddenSurface created (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (composite_rect->Width > 0 && composite_rect->Height > 0) {
        CompositeSurface = new DSurface(common_rect->Width, common_rect->Height, 2);
        CompositeSurface->Clear();
        DEBUG_INFO("CompositeSurface created (%dx%d)\n", composite_rect->Width, composite_rect->Height);
    }

    if (tile_rect->Width > 0 && tile_rect->Height > 0) {
        TileSurface = new DSurface(common_rect->Width, common_rect->Height, 2);
        TileSurface->Clear();
        DEBUG_INFO("TileSurface created (%dx%d)\n", tile_rect->Width, tile_rect->Height);
    }

    if (sidebar_rect->Width > 0 && sidebar_rect->Height > 0) {
        SidebarSurface = new DSurface(common_rect->Width, common_rect->Height, 2);
        SidebarSurface->Clear();
        DEBUG_INFO("SidebarSurface created (%dx%d)\n", sidebar_rect->Width, sidebar_rect->Height);
    }

    return true;
}


/**
 *  Creates the primary drawing surface for the game.
 * 
 *  @author: CCHyper, tomsons26
 */
static DSurface *Create_Primary_Intercept(DSurface **backbuffer_surface)
{
    HRESULT res;

#if 0
    /**
     *  Create the Direct3D primary surface, and fetch the back buffer surface.
     */
    res = Direct3DDevice->CreateOffscreenPlainSurface(
        OptionsExtension->WindowWidth,
        OptionsExtension->WindowHeight,
        D3DFMT_R5G6B5,
        D3DPOOL_DEFAULT,
        &Direct3DPrimarySurface,
        nullptr);

    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to create primary surface with error %x.\n", res);
    }

    DEBUG_INFO("Create_Primary() - Created primary Direct3D surface.\n");

    res = Direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &Direct3DBackSurface);
    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to get back buffer surface! Error code %x!", res);
    }

    DEBUG_INFO("Create_Primary() - Obtained Direct3D back buffer surface.\n");
#endif

    //unsigned int caps = Get_Video_Hardware_Capabilities();
    //
    //AllowStretchBlits = (caps & VIDEO_ALIGN_BOUNDARY) ? true : false;
    //DEBUG_INFO("Create_Primary() - DSurface::AllowStretchBlits = %s\n", AllowStretchBlits ? "true" : "false");
    //
    //AllowHardwareBlitFills = (caps & VIDEO_COLOR_FILL) ? true : false;
    //DEBUG_INFO("Create_Primary() - DSurface::AllowHardwareBlitFills = %s\n", AllowHardwareBlitFills ? "true" : "false");

    /**
     *  Create the primary game surface.
     */
    DSurface *primary_surface = new DSurface;
    ASSERT(primary_surface != nullptr);

    if (!primary_surface) {
        DEBUG_ERROR("Create_Primary() - Failed to create primary surface!\n");
        Fatal("Create_Primary() - Failed to create primary surface!\n");
        return nullptr;
    }

    //DEBUG_INFO("Create_Primary() - Creating surface desc.\n");

    primary_surface->VideoSurfaceDescription->dwWidth = Options.ScreenWidth;
    primary_surface->VideoSurfaceDescription->dwHeight = Options.ScreenHeight;

    primary_surface->VideoSurfaceDescription->dwFlags = DDSD_CAPS;
    primary_surface->VideoSurfaceDescription->ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY|DDSCAPS_3DDEVICE;
    //primary_surface->VideoSurfaceDescription->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE; // Its not actually "primary" anymore.

    //DEBUG_INFO("Create_Primary() - Backbuffer Surface: Setting back buffer flags.\n");

    primary_surface->VideoSurfaceDescription->ddsCaps.dwCaps |= DDSCAPS_FLIP|DDSCAPS_COMPLEX|DDSCAPS_VIDEOMEMORY|DDSCAPS_3DDEVICE;
    primary_surface->VideoSurfaceDescription->dwFlags |= DDSD_BACKBUFFERCOUNT;

    /**
     *  dwBackBufferCount is the number of back buffers that will be chained to the
     *  primary surface, either for double- or triple-buffering, or for a page flipping
     *  chain. This will be 1 by default.
     */
    primary_surface->VideoSurfaceDescription->dwBackBufferCount = (DoubleBackBuffer ? 2 : 1);

    //DEBUG_INFO("Create_Primary() - About to call CreateSurface.\n");

    /**
     *  Create the primary surface.
     */
    res = DirectDrawObject->CreateSurface(primary_surface->VideoSurfaceDescription, &primary_surface->VideoSurfacePtr, nullptr);
    if (FAILED(res)) {
        DDRAW_ERROR_MSGBOX("Create_Primary()\n\nCreateSurface failed with error code %08X\n", res);
        delete primary_surface;
        return nullptr;
    }

    if (!primary_surface->VideoSurfacePtr) {
        DEBUG_ERROR("Create_Primary() - VideoSurfacePtr is null!\n");
        return nullptr;
    }

    DEBUG_INFO("Create_Primary() - CreateSurface returned DD_OK.\n");

#if 0
    /**
     *  Get the surface directly attached to the primary (the back buffer).
     */
    LPDIRECTDRAWSURFACE lpDDSBack;
    DDSCAPS ddsCaps;
    ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

    DEBUG_INFO("Create_Primary() - About to call GetAttachedSurface.\n");

    res = primary_surface->VideoSurfacePtr->GetAttachedSurface(&ddsCaps, &lpDDSBack);
    if (FAILED(res)) {
        DDRAW_ERROR_MSGBOX("Create_Primary()\n\nGetAttachedSurface failed with error code %08X\n", res);
        delete primary_surface;
        return nullptr;
    }

    DEBUG_INFO("Create_Primary() - Creating back buffer DSurface.\n");

    //DirectDrawBackSurface = lpDDSBack;

    BackSurface = new DSurface(lpDDSBack);
    ASSERT(BackSurface != nullptr);

    DEBUG_INFO("Create_Primary() - Back buffer DSurface created.\n");
#endif

    DDRAW_INIT_STRUCT_PTR(primary_surface->VideoSurfaceDescription);

    DEBUG_INFO("Create_Primary() - About to call GetSurfaceDesc.\n");

    res = primary_surface->VideoSurfacePtr->GetSurfaceDesc(primary_surface->VideoSurfaceDescription);
    if (FAILED(res)) {
        DDRAW_ERROR_MSGBOX("Create_Primary()\n\nGetSurfaceDesc failed with error code %08X\n", res);
        delete primary_surface;
        return nullptr;
    }

    /**
     *  Calculate bytes per pixel.
     */
    //primary_surface->BytesPerPixel = ((primary_surface->VideoSurfaceDescription->ddpfPixelFormat.dwRGBBitCount + 7) / 8);
    primary_surface->BytesPerPixel = 2;
    DEBUG_INFO("Create_Primary() - BytesPerPixel = %d\n", primary_surface->BytesPerPixel);

    primary_surface->InVideoRam = (primary_surface->VideoSurfaceDescription->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;
    DEBUG_INFO("Create_Primary() - InVideoRam = %s\n", primary_surface->InVideoRam ? "true" : "false");

    DEBUG_INFO("Create_Primary() - Setting surface dimensions to %d x %d.\n", primary_surface->VideoSurfaceDescription->dwWidth, primary_surface->VideoSurfaceDescription->dwHeight);
    primary_surface->Width = primary_surface->VideoSurfaceDescription->dwWidth;
    primary_surface->Height = primary_surface->VideoSurfaceDescription->dwHeight;

    DirectDrawPrimarySurface = primary_surface->VideoSurfacePtr;
    ASSERT(DirectDrawPrimarySurface != nullptr);

    primary_surface->IsAllocated = true;

#if 0
    DEBUG_INFO("Create_Primary() - About to call CreateClipper.\n");

    res = DirectDrawObject->CreateClipper(0, &DirectDrawClipper, nullptr);
    if (FAILED(res)) {
        DDRAW_ERROR_MSGBOX("Create_Primary()\n\nCreateClipper failed with error code %08X\n", res);
        delete primary_surface;
        return nullptr;
    }
#endif

#if 0
    DEBUG_INFO("Create_Primary() - About to call SetHWnd.\n");

    res = DirectDrawClipper->SetHWnd(0, GetActiveWindow());
    if (FAILED(res)) {
        DDRAW_ERROR_MSGBOX("Create_Primary()\n\nSetHWnd failed with error code %08X\n", res);
        delete primary_surface;
        return nullptr;
    }
#endif

    DEBUG_INFO("Create_Primary() - About to call SetClipper.\n");

    res = primary_surface->VideoSurfacePtr->SetClipper(DirectDrawClipper);
    if (FAILED(res)) {
        DDRAW_ERROR_MSGBOX("Create_Primary()\n\nSetClipper failed with error code %08X\n", res);
        delete primary_surface;
        return nullptr;
    }

    /**
     *  Fetch the surface descriptions pixel format. We expect the pixel_format to have at least DDPF_RGB (0x40) here.
     */
    DDPIXELFORMAT pixel_format = primary_surface->VideoSurfaceDescription->ddpfPixelFormat;
    if ((pixel_format.dwFlags & DDPF_RGB) == 0) {
        DEBUG_WARNING("Create_Primary() - Video surface desc does not have DDPF_RGB set!\n");
    }

    DEBUG_INFO("Create_Primary() - pixel_format.dwFlags is %d.\n", pixel_format.dwFlags);
    DEBUG_INFO("Create_Primary() - pixel_format.dwSize is %d.\n", pixel_format.dwSize);







    DSurface::AllowStretchBlits = true;
    DSurface::AllowHardwareBlitFills = true;

    /**
     *  Tell the game that the virtual surface is RGB 565.
     */
    DSurface::RedLeft = 11;
    DSurface::RedRight = 3;
    DSurface::GreenLeft = 5;
    DSurface::GreenRight = 2;
    DSurface::BlueLeft = 0;
    DSurface::BlueRight = 3;

    DSurface::RGBPixelFormat = 2; // RGB565

    DSurface::ColorGrey = DSurface::RGB_To_Pixel(127, 127, 127);
    DSurface::ColorMidGrey = DSurface::RGB_To_Pixel(63, 63, 63);
    DSurface::ColorDarkGrey = DSurface::RGB_To_Pixel(31, 31, 31);




    // Hack! Window seems to be out of focus at this point...
    // The window needs this initially otherwise we need to alt-tab to gain focus.
    GameInFocus = true;




    return primary_surface;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
static bool Set_Video_Mode_Intercept(HWND hWnd, int width, int height, int bits_per_pixel)
{
    HRESULT res;

#if 0
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.Windowed = TRUE;

    d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    d3dpp.BackBufferWidth = OptionsExtension->WindowWidth; // width;
    d3dpp.BackBufferHeight = OptionsExtension->WindowHeight; // height;
    d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferCount = D3DFMT_UNKNOWN;

    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    DWORD behaviour_flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    if (false) { // TODO
        behaviour_flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

    res = Direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, behaviour_flags, &d3dpp, &Direct3DDevice);
    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to create Direct3D device! Error code %x!\n", res);
        return false;
    }
#endif

    //DEBUG_INFO("Set_Video_Mode(enter)\n");

    //Prep_Direct_Draw();
    //if (!Prep_Direct_Draw()) {
    //    DEBUG_ERROR("Prep_Direct_Draw() failed in Set_Video_Mode!\n");
    //    return false;
    //}

	///**
	// *  Set the required display mode.
	// */
	//res = DirectDrawObject->SetDisplayMode(width, height, bits_per_pixel);
	//if (FAILED(res)) {
	//	DDRAW_ERROR_MSGBOX("Set_Video_Mode()\n\nSetDisplayMode failed with error code %08X\n", res);
	//	DirectDrawObject->Release();
	//	DirectDrawObject = nullptr;
	//	return false;
	//}

    VideoWidth = Options.ScreenWidth;
    VideoHeight = Options.ScreenHeight;
    BitsPerPixel = bits_per_pixel;

	//DEBUG_INFO("Set_Video_Mode() - Display mode set to %d x %d - %d.\n", width, height, bits_per_pixel);

	/**
	 *  Create a direct draw palette object.
	 */
    //if (bits_per_pixel != 8) {;
	//    res = DirectDrawObject->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256, &PaletteEntries[0], &PalettePtr, nullptr);
	//    if (FAILED(res)) {
	//		//DDRAW_ERROR_MSGBOX("Set_Video_Mode()\n\nCreatePalette failed with error code %08X\n", res);
	//	    DirectDrawObject->Release();
	//	    DirectDrawObject = nullptr;
	//	    return false;
	//    }
    //
	//	DEBUG_INFO("Set_Video_Mode() - Palette created (8bit, 256 colors).\n");
    //
    //    //Check_Overlapped_Blit_Capability();
    //}

#if 0
	DDCAPS capabilities;
	DDCAPS emulated_capabilities;
	DDRAW_INIT_STRUCT(capabilities);
	DDRAW_INIT_STRUCT(emulated_capabilities);

	res = DirectDrawObject->GetCaps(&capabilities, &emulated_capabilities);
	if (FAILED(res)) {
        DDRAW_ERROR_MSGBOX("Set_Video_Mode()\n\nGetCaps failed with error code %08X\n", res);
		return false;
	}

	if (capabilities.dwCaps & DDCAPS_CANBLTSYSMEM) {
		//SystemToVideoBlits = (capabilities.dwSVBCaps & DDCAPS_BLT) ? true : false;
		//VideoToSystemBlits = (capabilities.dwVSBCaps & DDCAPS_BLT) ? true : false;
		//SystemToSystemBlits = (capabilities.dwSSBCaps & DDCAPS_BLT) ? true : false;
	} else {
		//SystemToVideoBlits = false;
		//VideoToSystemBlits = false;
		//SystemToSystemBlits = false;
	}
#endif

    //DEBUG_INFO("Set_Video_Mode() - SystemToVideoBlits = %s\n", SystemToVideoBlits ? "true" : "false");
    //DEBUG_INFO("Set_Video_Mode() - VideoToSystemBlits = %s\n", VideoToSystemBlits ? "true" : "false");
    //DEBUG_INFO("Set_Video_Mode() - SystemToSystemBlits = %s\n", SystemToSystemBlits ? "true" : "false");

    //DEBUG_INFO("Set_Video_Mode(exit)\n");

	return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
static void Reset_Video_Mode_Intercept()
{
    //DEBUG_INFO("Reset_Video_Mode(enter)\n");

    HRESULT res;

    D3D11_CleanupDeviceD3D();

#if 0
    /**
     *  If a direct draw object has been declared and a video mode has been set
     *  then reset the video mode and release the direct draw object.
     */
    if (DirectDrawObject) {
        //res = DirectDrawObject->RestoreDisplayMode();
        //if (FAILED(res)) {
        //    //DDRAW_ERROR_MSGBOX("Reset_Video_Mode()\n\nRestoreDisplayMode failed with error code %08X\n", res);
        //}

        VideoWidth = 0;
        VideoHeight = 0;
        BitsPerPixel = 0;

        res = DirectDrawObject->Release();
        if (FAILED(res)) {
            //DDRAW_ERROR_MSGBOX("Reset_Video_Mode()\n\nRelease failed with error code %08X\n", res);
        }

        DirectDrawObject = nullptr;
    }

    if (Direct3DDevice) {
        Direct3DDevice->Release();
        Direct3DDevice = nullptr;
    }
    if (Direct3D) {
        Direct3D->Release();
        Direct3D = nullptr;
    }
#endif

    //DEBUG_INFO("Reset_Video_Mode(exit)\n");
}


static bool Prep_Direct3D(HWND hWnd)
{
    DEBUG_INFO("Prep_Direct3D() - Creating Direct3D.\n");

    D3D11_CreateDeviceD3D(hWnd);

#if 0
    Direct3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!Direct3D) {
        return false;
    }

    DEBUG_INFO("Prep_Direct3D() - Direct3D objected created sucessfully.\n");

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.Windowed = TRUE;

    d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    d3dpp.BackBufferWidth = OptionsExtension->WindowWidth; // width;
    d3dpp.BackBufferHeight = OptionsExtension->WindowHeight; // height;
    d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferCount = D3DFMT_UNKNOWN;

    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    DWORD behaviour_flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    if (false) { // TODO
        behaviour_flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

    HRESULT res = Direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, behaviour_flags, &d3dpp, &Direct3DDevice);
    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to create Direct3D device! Error code %x!\n", res);
        return false;
    }
#endif

    return true;
}

/**
 *  x
 *
 *  @author: CCHyper
 */
static bool Prep_Direct_Draw_Intercept(HWND hWnd)
{
    //DEBUG_INFO("Prep_Direct_Draw(enter)\n");

	/**
	 *  If there is not currently a direct draw object then we need to define one.
	 */
	if (DirectDrawObject == nullptr) {

		DEBUG_INFO("Prep_Direct_Draw() - Creating DirectDrawObject.\n");

	    HRESULT res;

		/**
         *  Create the Direct Draw object. This is used to access anything DirectDraw does.
		 */
        res = DirectDrawCreateEx(nullptr, (LPVOID *)&DirectDrawObject, IID_IDirectDraw7, nullptr);
		if (FAILED(res)) {
			DDRAW_ERROR_MSGBOX("Prep_Direct_Draw()\n\nSetCooperativeLevel failed with error code %08X\n", res);
		    DirectDrawObject = nullptr;
            return false;
        }

		DEBUG_INFO("Prep_Direct_Draw() - DirectDrawObject objected created sucessfully.\n");

		/**
         *  Set the mode to "normal" as fullscreen as we use borderless windowed instead of fullscreen.
         */
		res = DirectDrawObject->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
		if (FAILED(res)) {
			DDRAW_ERROR_MSGBOX("Prep_Direct_Draw()\n\nSetCooperativeLevel failed with error code %08X\n", res);
		    DirectDrawObject->Release();
		    DirectDrawObject = nullptr;
            return false;
        }
    }

    if (!Prep_Direct3D(hWnd)) {
        return false;
    }

    //DEBUG_INFO("Prep_Direct_Draw(exit)\n");

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool D3D_Clear_Screen(bool present = false)
{
#if 0
    Direct3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    if (present) {
        Direct3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
    }
#endif

    return true;
}


#if 0
/**
 *  x
 *
 *  @author: CCHyper
 */
bool D3D_Lock_Surface(LPDIRECT3DSURFACE9 surface, D3DLOCKED_RECT& lock_rect, bool discard, bool readonly)
{
    ASSERT(surface != nullptr);

    lock_rect.Pitch = 0;
    lock_rect.pBits = nullptr;

    DWORD flags = (discard ? D3DLOCK_DISCARD : 0);
    if (readonly) {
        flags |= D3DLOCK_READONLY;
    }
    else {
        flags |= D3DLOCK_NOSYSLOCK;
    }

    HRESULT res = surface->LockRect(&lock_rect, nullptr, flags);

    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to lock back surface! Error code %x!", res);
        return false;
    }

    return true;
}
#endif


#if 0
/**
 *  x
 *
 *  @author: CCHyper
 */
bool D3D_Unlock_Surface(LPDIRECT3DSURFACE9 surface)
{
    ASSERT(surface != nullptr);

    HRESULT res = surface->UnlockRect();

    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to unlock back surface! Error code %x!", res);
        return false;
    }

    return true;
}
#endif

#if 0
/**
 *  x
 *
 *  @author: CCHyper
 */
bool D3D_Flip(DSurface *surface)
{
    if (!surface) {
        DEBUG_ERROR("Direct3D - surface is null!\n");
        return false;
    }

    if (!Direct3DPrimarySurface) {
        DEBUG_ERROR("Direct3D - Direct3DPrimarySurface is null!\n");
        return false;
    }

    Direct3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

    D3DLOCKED_RECT lock_rect;

    if (!D3D_Lock_Surface(Direct3DPrimarySurface, lock_rect, false, false)) {
        DEBUG_ERROR("Direct3D - Failed to lock destination surface!\n");
        return false;
    }

    unsigned short *srcptr = reinterpret_cast<unsigned short *>(surface->Lock());
    if (!srcptr) {
        DEBUG_ERROR("Direct3D - Failed to lock game surface!\n");
        D3D_Unlock_Surface(Direct3DPrimarySurface);
        return false;
    }

    std::memcpy((unsigned short *)lock_rect.pBits, srcptr, surface->Get_Width() * surface->Get_Height() * surface->Get_Bytes_Per_Pixel());

//    D3D_Write_Surface_Data_To_File("SRC.PNG", surface);
//    D3D_Write_Surface_Data_To_File("DST.PNG", PrimarySurface);

    if (!surface->Unlock()) {
        DEBUG_ERROR("Direct3D - Failed to unlock game surface!\n");
        D3D_Unlock_Surface(Direct3DPrimarySurface);
        return false;
    }

    if (!D3D_Unlock_Surface(Direct3DPrimarySurface)) {
        DEBUG_ERROR("Direct3D - Failed to unlock destination surface!\n");
        return false;
    }

    RECT src_rect;
    src_rect.left = 0;
    src_rect.top = 0;
    src_rect.right = surface->Get_Width();
    src_rect.bottom = surface->Get_Height();

    RECT dest_rect;
    dest_rect.left = 0;
    dest_rect.top = 0;
    dest_rect.right = surface->Get_Width();
    dest_rect.bottom = surface->Get_Height();

    LPDIRECT3DSURFACE9 backsurfaceptr;

    HRESULT res = Direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backsurfaceptr);
    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to get back buffer surface! Error code %x!\n", res);
        return false;
    }

    D3DTEXTUREFILTERTYPE d3dtft = D3DTEXF_POINT;
    Direct3DDevice->StretchRect(Direct3DPrimarySurface, &src_rect, backsurfaceptr, &dest_rect, d3dtft);

    // Present the backbuffer contents to the display.
    Direct3DDevice->Present(&src_rect, &dest_rect, nullptr, nullptr);

    return true;
}
#endif


#if 0
/**
 *  Creates a GDI-compatible handle of a device context for the direct draw surface.
 * 
 *  @warning    Make sure to call Release_DC when finished!
 */
HDC D3D_Surface_Get_DC(LPDIRECT3DSURFACE9 surface)
{
    if (!surface) {
        DEBUG_WARNING("D3DSurface::Get_DC() - surface is null!\n");
        return nullptr;
    }

    HDC hdc = nullptr;
    HRESULT res = surface->GetDC(&hdc);
    if (FAILED(res)) {
        return nullptr;
    }

    return hdc;
}
#endif


#if 0
/**
 *  x
 *
 *  @author: CCHyper
 */
bool D3D_Surface_Release_DC(LPDIRECT3DSURFACE9 surface, HDC hdc)
{
    HRESULT res = surface->ReleaseDC(hdc);
    if (FAILED(res)) {
        return false;
    }

    return true;
}
#endif


void D3D_Hooks()
{
    // Removes Debug_Windowed check and calls Set_Video_Mode.
    Patch_Jump(0x006016B8, 0x006015FC);

    // Remove the 16bit display depth check.
    Patch_Jump(0x0060142F, 0x00601467);

    /**
     *  Patch in the D3D intercept functions.
     */
    Patch_Call(0x0050AD34, &Set_Video_Mode_Intercept);
    Patch_Call(0x006015E6, &Set_Video_Mode_Intercept);
    Patch_Call(0x0060161C, &Set_Video_Mode_Intercept);
    Patch_Call(0x00601716, &Set_Video_Mode_Intercept);
    Patch_Call(0x00601790, &Set_Video_Mode_Intercept);
    Patch_Call(0x005FF7B4, &Reset_Video_Mode_Intercept);
    Patch_Call(0x006013C8, &Prep_Direct_Draw_Intercept);
    Patch_Call(0x006016B3, &Prep_Direct_Draw_Intercept);
    Patch_Call(0x0050AD5A, &Create_Primary_Intercept);
    Patch_Call(0x0050AF41, &Create_Primary_Intercept);
    Patch_Call(0x0060141E, &Create_Primary_Intercept);
    Patch_Call(0x0050B05D, &Allocate_Surfaces_Intercept);
    Patch_Call(0x00601543, &Allocate_Surfaces_Intercept);
    Patch_Byte(0x005636C0+1, 0x84);

    //TEMP
    Debug_Windowed = true;
}
