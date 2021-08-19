/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DDRAW7_SUPPORT.CPP
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
#include "ddraw7_support.h"
#include "ddraw7_util.h"
#include "d7surface.h"
#include "renderer.h"
#include "tibsun_globals.h"
#include "bsurface.h"
#include "options.h"
#include "rgb.h"
#include "textprint.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"




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

//D7Surface *BackBufferSurface = nullptr;   // not needed?
//BDSurface *BackBufferSurface = nullptr;   // not needed?

PALETTEENTRY PaletteEntries[256];
LPDIRECTDRAWPALETTE	PalettePtr = nullptr;

bool FirstPaletteSet = false;

bool CanVblankSync = true;

unsigned char CurrentPalette[768] = {255};



/**
 *  Blitter utility function to convert 16bit pixel data to 32bit pixel data.
 * 
 *  src     - rgb565 format;
 *  dst     - rgb888 format;
 * 
 *  @author: CCHyper
 */
static void DirectDraw7_Copy_Row_RGB565_To_RGB888(const unsigned short *src, unsigned char *dst, unsigned width)
{
#if 0
    /**
     *  SSE copy only works with alignment of 8 bytes.
     */
    if ((width % 8) == 0) {

        unsigned short *src_rgb565 = src;
        unsigned short *dst_argb = dst;

        // TODO, rewrite to be ABGR

        // pmul method to replicate bits.
        // Math to replicate bits:
        // (v << 8) | (v << 3)
        // v * 256 + v * 8
        // v * (256 + 8)
        // G shift of 5 is incorporated, so shift is 5 + 8 and 5 + 3
        // 20 instructions.
        // Google's libyuv project: https://chromium.googlesource.com/libyuv/libyuv/+/master/source/row_win.cc

        __asm {
            mov       eax, 0x01080108  // generate multiplier to repeat 5 bits
            movd      xmm5, eax
            pshufd    xmm5, xmm5, 0
            mov       eax, 0x20802080  // multiplier shift by 5 and then repeat 6 bits
            movd      xmm6, eax
            pshufd    xmm6, xmm6, 0
            pcmpeqb   xmm3, xmm3       // generate mask 0xf800f800 for Red
            psllw     xmm3, 11
            pcmpeqb   xmm4, xmm4       // generate mask 0x07e007e0 for Green
            psllw     xmm4, 10
            psrlw     xmm4, 5
            pcmpeqb   xmm7, xmm7       // generate mask 0xff00ff00 for Alpha
            psllw     xmm7, 8
            mov       eax, [esp + 4]   // src_rgb565
            mov       edx, [esp + 8]   // dst_argb
            mov       ecx, [esp + 12]  // width
            sub       edx, eax
            sub       edx, eax
    
        convertloop:
            movdqu    xmm0, [eax]   // fetch 8 pixels of bgr565
            movdqa    xmm1, xmm0
            movdqa    xmm2, xmm0
            pand      xmm1, xmm3    // R in upper 5 bits
            psllw     xmm2, 11      // B in upper 5 bits
            pmulhuw   xmm1, xmm5    // * (256 + 8)
            pmulhuw   xmm2, xmm5    // * (256 + 8)
            psllw     xmm1, 8
            por       xmm1, xmm2    // RB
            pand      xmm0, xmm4    // G in middle 6 bits
            pmulhuw   xmm0, xmm6    // << 5 * (256 + 4)
            por       xmm0, xmm7    // AG
            movdqa    xmm2, xmm1
            punpcklbw xmm1, xmm0
            punpckhbw xmm2, xmm0
            movdqu    [eax * 2 + edx], xmm1         // store 4 pixels of ARGB
            movdqu    [eax * 2 + edx + 16], xmm2    // store next 4 pixels of ARGB
            lea       eax, [eax + 16]
            sub       ecx, 8
            jg        convertloop

            ret
        }

    } else {
#endif

        /**
         *  Memory is unaligned.
         */
        for (int i = 0; i < width; ++i) {

            unsigned short src_pixel = *(src++);
            unsigned char r = (src_pixel & 0xF800) >> 11; // Extract the 5 R bits
            unsigned char g = (src_pixel & 0x07E0) >> 5;  // Extract the 6 G bits
            unsigned char b = (src_pixel & 0x001F);       // Extract the 5 B bits
            
            unsigned char *dst_pixel = dst;
            *dst_pixel++ = (b << 3) | (b & 7); // Set the 8 B bit.
            *dst_pixel++ = (g << 2) | (g & 3); // Set the 8 G bit.
            *dst_pixel++ = (r << 3) | (r & 7); // Set the 8 R bit.
            *dst_pixel++ = 255;                // Set the 8 A bit.
        }

#if 0
    }
#endif
}




/**
 *  
 * 
 *  @author: CCHyper, tomsons26
 */
static void Calculate_Mask_Info(unsigned int mask, unsigned int &left, unsigned int &right, unsigned int bits)
{
    left = 0;
    right = 0;

    unsigned int tmp = mask;

    /**
     *  Figure out how far to shift bits to the left.
     */
    unsigned int l = 0;
    for ( l = 0; l < bits; ++l, tmp >>= 1) { // /= 2
        if (tmp & 1) {   // is odd?
            break;
        }
    }
    left = l;

    /**
     *  Figure out how far to shift bits to the right.
     */
    unsigned int r = 0;
    for ( r = 0; r < 8; ++r, tmp <<= 1) { // *= 2
        if (tmp & 128) {  // is highest bit in the byte is set?
            break;
        }
    }
    right = r;
}


/**
 *  Create the new DirectDraw7 primary surface.
 * 
 *  @author: CCHyper
 */
static bool DirectDraw7_Create_DirectDraw_Surface()
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

    if (!Debug_Windowed) {
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

    if (Debug_Windowed) {

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
        ddsDesc.dwWidth  = Options.ScreenWidth;
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
    DEBUG_INFO("Primary surface info:\n", ddsDesc.ddpfPixelFormat.dwRGBBitCount);
    DEBUG_INFO("  dwRGBBitCount = %d\n", ddsDesc.ddpfPixelFormat.dwRGBBitCount);
    DEBUG_INFO("  dwRBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwRBitMask);
    DEBUG_INFO("  dwGBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwGBitMask);
    DEBUG_INFO("  dwBBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwBBitMask);
    DEBUG_INFO("  dwWidth = %d\n", ddsDesc.dwWidth);
    DEBUG_INFO("  dwHeight = %d\n", ddsDesc.dwHeight);
    
    DirectDraw7PrimarySurface->GetSurfaceDesc(&ddsDesc);
    DEBUG_INFO("Back surface info:\n", ddsDesc.ddpfPixelFormat.dwRGBBitCount);
    DEBUG_INFO("  dwRGBBitCount = %d\n", ddsDesc.ddpfPixelFormat.dwRGBBitCount);
    DEBUG_INFO("  dwRBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwRBitMask);
    DEBUG_INFO("  dwGBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwGBitMask);
    DEBUG_INFO("  dwBBitMask = 0x%08X\n", ddsDesc.ddpfPixelFormat.dwBBitMask);
    DEBUG_INFO("  dwWidth = %d\n", ddsDesc.dwWidth);
    DEBUG_INFO("  dwHeight = %d\n", ddsDesc.dwHeight);

    /**
     *  Clear out both primary and secondary surfaces.
     */
    DirectDraw7_Clear_Surface(DirectDraw7PrimarySurface);
    DirectDraw7_Clear_Surface(DirectDraw7BackSurface);

    return true;
}


/**
 *  Creates the primary 32bit surface.
 * 
 *  @author: CCHyper
 */
XSurface *DirectDraw7_Create_Primary(XSurface **backbuffer_surface)
{
    DEBUG_INFO("DirectDraw7_Create_Primary(enter)\n");

    XSurface *primary_surface = nullptr;

    /**
     *  For the DirectDraw7 renderer, we just need to create a bare minimum
     *  surface to keep the game happy, we handle all the heavy overhead
     *  of copying to the DirectDraw primary surface elsewhere now.
     */

    //D7Surface *dsurface_primary = new D7Surface(Options.ScreenWidth, Options.ScreenHeight, true);
    BSurface *dsurface_primary = new BSurface(Options.ScreenWidth, Options.ScreenHeight, 2);
    ASSERT(dsurface_primary != nullptr);

    if (dsurface_primary == nullptr) {
        DEBUG_ERROR("DirectDraw7_Create_Primary() - Failed to create primary D7Surface!\n");
        Fatal("DirectDraw7_Create_Primary() - Failed to create primary game surface!\n");
        return nullptr;
    }

    DSurface::AllowStretchBlits = true;
    DSurface::AllowHardwareBlitFills = true;

#if 0
    DSurface::RedLeft = 0;
    DSurface::RedRight = 0;
    DSurface::GreenLeft = 0;
    DSurface::GreenRight = 0;
    DSurface::BlueLeft = 0;
    DSurface::BlueRight = 0;

    DDPIXELFORMAT pixel_format = dsurface_primary->VideoSurfaceDescription->ddpfPixelFormat;

    Calculate_Mask_Info(pixel_format.dwRBitMask, DSurface::RedLeft, DSurface::RedRight, (dsurface_primary->BytesPerPixel * 8));
    Calculate_Mask_Info(pixel_format.dwGBitMask, DSurface::GreenLeft, DSurface::GreenRight, (dsurface_primary->BytesPerPixel * 8));
    Calculate_Mask_Info(pixel_format.dwBBitMask, DSurface::BlueLeft, DSurface::BlueRight, (dsurface_primary->BytesPerPixel * 8));
#else
    // BSurface
    DSurface::RedLeft = 11; // Assume RGB 565
    DSurface::RedRight = 3;
    DSurface::GreenLeft = 5;
    DSurface::GreenRight = 2;
    DSurface::BlueLeft = 0;
    DSurface::BlueRight = 3;
#endif

    switch (dsurface_primary->BytesPerPixel) {

        /**
         *  32bit
         */
        case 4:
            DEBUG_INFO("DirectDraw7_Create_Primary() - RGBPixelFormat is RGB888.\n");
            break;

        /**
         *  24bit
         */
        case 3:
            break;

        /**
         *  16bit
         */
        case 2:
            if (DSurface::RedLeft == 10 && DSurface::RedRight == 3 && DSurface::GreenLeft == 5 && DSurface::GreenRight == 3 && DSurface::BlueLeft == 0 && DSurface::BlueRight == 3) {
                DSurface::RGBPixelFormat = RGB555;
                DEBUG_INFO("DirectDraw7_Create_Primary() - RGBPixelFormat is RGB555.\n");

            } else if (DSurface::RedLeft == 11 && DSurface::RedRight == 3 && DSurface::GreenLeft == 6 && DSurface::GreenRight == 3 && DSurface::BlueLeft == 0 && DSurface::BlueRight == 2) {
                DSurface::RGBPixelFormat = RGB556;
                DEBUG_INFO("DirectDraw7_Create_Primary() - RGBPixelFormat is RGB556.\n");

            } else if (DSurface::RedLeft == 11 && DSurface::RedRight == 3 && DSurface::GreenLeft == 5 && DSurface::GreenRight == 2 && DSurface::BlueLeft == 0 && DSurface::BlueRight == 3) {
                DSurface::RGBPixelFormat = RGB565;
                DEBUG_INFO("DirectDraw7_Create_Primary() - RGBPixelFormat is RGB565.\n");

            } else if (DSurface::RedLeft == 10 && DSurface::RedRight == 3 && DSurface::GreenLeft == 5 && DSurface::GreenRight == 2 && DSurface::BlueLeft == 0 && DSurface::BlueRight == 3) {
                DSurface::RGBPixelFormat = RGB655;
                DEBUG_INFO("DirectDraw7_Create_Primary() - RGBPixelFormat is RGB655.\n");

            } else {
                DSurface::RGBPixelFormat = -1;
                DEBUG_WARNING("DirectDraw7_Create_Primary() - RGBPixelFormat is unsupported!.\n");
            }

            break;

        /**
         *  8bit
         */
        case 1:
            DSurface::RGBPixelFormat = -1;
            DEBUG_WARNING("DirectDraw7_Create_Primary() - RGBPixelFormat is unsupported!.\n");
            break;
    };

    DSurface::ColorGrey = DSurface::RGBA_To_Pixel(127, 127, 127);
    DSurface::ColorMidGrey = DSurface::RGBA_To_Pixel(63, 63, 63);
    DSurface::ColorDarkGrey = DSurface::RGBA_To_Pixel(31, 31, 31);

    if (!DirectDraw7_Create_DirectDraw_Surface()) {
        DEBUG_ERROR("DirectDraw7_Create_Primary() - Failed to create primary direct draw surface!\n");
        Fatal("DirectDraw7_Create_Primary() - Failed to create primary direct draw surface!\n");
        return nullptr;
    }

    primary_surface = dsurface_primary;

    DEBUG_INFO("DirectDraw7_Create_Primary(exit)\n");

    return primary_surface;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool DirectDraw7_Allocate_Surfaces(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect)
{
    DEBUG_INFO("Allocate_Surfaces(enter)\n");

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

    if (common_rect->Width > 0 && common_rect->Height > 0) {
        //AlternateSurface = (DSurface *)new D7Surface(common_rect->Width, common_rect->Height);
        AlternateSurface = (DSurface *)new BSurface(common_rect->Width, common_rect->Height, 2);
        //AlternateSurface = (DSurface *)new BDSurface(common_rect->Width, common_rect->Height, 2);
        AlternateSurface->Fill(COLOR_TBLACK);
        DEBUG_INFO("AlternateSurface created (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (common_rect->Width > 0 && common_rect->Height > 0) {
        //HiddenSurface = (DSurface *)new D7Surface(common_rect->Width, common_rect->Height);
        HiddenSurface = (DSurface *)new BSurface(common_rect->Width, common_rect->Height, 2);
        //HiddenSurface = (DSurface *)new BDSurface(common_rect->Width, common_rect->Height, 2);
        HiddenSurface->Fill(COLOR_TBLACK);
        DEBUG_INFO("HiddenSurface created (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (composite_rect->Width > 0 && composite_rect->Height > 0) {
        //CompositeSurface = (DSurface *)new D7Surface(composite_rect->Width, composite_rect->Height);
        CompositeSurface = (DSurface *)new BSurface(composite_rect->Width, composite_rect->Height, 2);
        //CompositeSurface = (DSurface *)new BDSurface(composite_rect->Width, composite_rect->Height, 2);
        CompositeSurface->Fill(COLOR_TBLACK);
        DEBUG_INFO("CompositeSurface created (%dx%d)\n", composite_rect->Width, composite_rect->Height);
    }

    if (tile_rect->Width > 0 && tile_rect->Height > 0) {
        //TileSurface = (DSurface *)new D7Surface(tile_rect->Width, tile_rect->Height);
        TileSurface = (DSurface *)new BSurface(tile_rect->Width, tile_rect->Height, 2);
        //TileSurface = (DSurface *)new BDSurface(tile_rect->Width, tile_rect->Height, 2);
        TileSurface->Fill(COLOR_TBLACK);
        DEBUG_INFO("TileSurface created (%dx%d)\n", tile_rect->Width, tile_rect->Height);
    }

    if (sidebar_rect->Width > 0 && sidebar_rect->Height > 0) {
        //SidebarSurface = (DSurface *)new D7Surface(sidebar_rect->Width, sidebar_rect->Height);
        SidebarSurface = (DSurface *)new BSurface(sidebar_rect->Width, sidebar_rect->Height, 2);
        //SidebarSurface = (DSurface *)new BDSurface(sidebar_rect->Width, sidebar_rect->Height, 2);
        SidebarSurface->Fill(COLOR_TBLACK);
        DEBUG_INFO("SidebarSurface created (%dx%d)\n", sidebar_rect->Width, sidebar_rect->Height);
    }
    
    DEBUG_INFO("Allocate_Surfaces(exit)\n");

    return true;
}


/**
 *  
 *  
 *  @author: CCHyper
 */
void DirectDraw7_Release(HWND hWnd)
{
    //DEBUG_INFO("Release_Direct_Draw(enter)\n");

    if (DirectDraw7Object != nullptr) {

        HRESULT ddrval;

        ddrval = DirectDraw7Object->Release();
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("DirectDraw7_Release()\n\nRelease failed with error code %08X\n", ddrval);
        }

        DirectDraw7Object = nullptr;
    }

    //DEBUG_INFO("Release_Direct_Draw(exit)\n");
}


/**
 *  
 *  
 *  @author: CCHyper
 */
void DirectDraw7_Shutdown()
{
    if (DirectDraw7PrimarySurface) {
        DirectDraw7PrimarySurface->Release();
        DirectDraw7PrimarySurface = nullptr;
    }
    if (DirectDraw7BackSurface) {
        DirectDraw7BackSurface->Release();
        DirectDraw7BackSurface = nullptr;
    }
    if (DirectDraw7Clipper) {
        DirectDraw7Clipper->Release();
        DirectDraw7Clipper = nullptr;
    }
    if (VideoSurfaceDescription) {
        delete VideoSurfaceDescription;
        VideoSurfaceDescription = nullptr;
    }
    if (DirectDraw7Object) {
        DirectDraw7Object->Release();
        DirectDraw7Object = nullptr;
    }
}


/**
 *  
 *  
 *  @author: CCHyper
 */
void DirectDraw7_Reset_Video_Mode()
{
    //DEBUG_INFO("Reset_Video_Mode(enter)\n");

    HRESULT ddrval;

    /**
     *  If a direct draw object has been declared and a video mode has been set
     *  then reset the video mode and release the direct draw object.
     */
    if (DirectDraw7Object) {
        ddrval = DirectDraw7Object->RestoreDisplayMode();
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("Reset_Video_Mode()\n\nRestoreDisplayMode failed with error code %08X\n", ddrval);
        }

        VideoWidth = 0;
        VideoHeight = 0;
        BitsPerPixel = 0;

        ddrval = DirectDraw7Object->Release();
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("Reset_Video_Mode()\n\nRelease failed with error code %08X\n", ddrval);
        }

        DirectDraw7Object = nullptr;
    }

    //DEBUG_INFO("Reset_Video_Mode(exit)\n");
}


/**
 *  
 *  
 *  @author: CCHyper
 */
bool DirectDraw7_Set_Video_Mode(HWND hWnd, int width, int height, int bits_per_pixel)
{
    //DEBUG_INFO("DirectDraw7_Set_Video_Mode(enter)\n");

    if (!DirectDraw7_Prep(hWnd)) {
        DEBUG_ERROR("DirectDraw7_Set_Video_Mode() - Failed to prepare DirectDraw!\n");
        return false;
    }

    HRESULT ddrval;

    /**
     *  Set the required display mode if full screen was requested.
     */
    if (!Debug_Windowed) {
        ddrval = DirectDraw7Object->SetDisplayMode(width, height, bits_per_pixel, 0, 0);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("DirectDraw7_Set_Video_Mode()\n\nSetDisplayMode failed with error code %08X\n", ddrval);
            DirectDraw7Object->Release();
            DirectDraw7Object = nullptr;
            return false;
        }

        DEBUG_INFO("DirectDraw7_Set_Video_Mode() - Display mode set to %d x %d - %d.\n", width, height, bits_per_pixel);
    }

    VideoWidth = width;
    VideoHeight = height;
    BitsPerPixel = bits_per_pixel;

    /**
     *  Create a direct draw palette object.
     */
    if (bits_per_pixel != 8) {;
        ddrval = DirectDraw7Object->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256, &PaletteEntries[0], &PalettePtr, nullptr);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("DirectDraw7_Set_Video_Mode()\n\nCreatePalette failed with error code %08X\n", ddrval);
            DirectDraw7Object->Release();
            DirectDraw7Object = nullptr;
            return false;
        }

        DEBUG_INFO("DirectDraw7_Set_Video_Mode() - Palette created (8bit, 256 colors).\n");

        DirectDraw7_Check_Overlapped_Blit_Capability();
    }

    DDCAPS capabilities;
    DDCAPS emulated_capabilities;
    DDRAW_INIT_STRUCT(capabilities);
    DDRAW_INIT_STRUCT(emulated_capabilities);

    ddrval = DirectDraw7Object->GetCaps(&capabilities, &emulated_capabilities);
    if (FAILED(ddrval)) {
        DDRAW_ERROR_MSGBOX("DirectDraw7_Set_Video_Mode()\n\nGetCaps failed with error code %08X\n", ddrval);
        return false;
    }

    if (capabilities.dwCaps & DDCAPS_CANBLTSYSMEM) {
        SystemToVideoBlits = (capabilities.dwSVBCaps & DDCAPS_BLT) ? true : false;
        VideoToSystemBlits = (capabilities.dwVSBCaps & DDCAPS_BLT) ? true : false;
        SystemToSystemBlits = (capabilities.dwSSBCaps & DDCAPS_BLT) ? true : false;
    } else {
        SystemToVideoBlits = false;
        VideoToSystemBlits = false;
        SystemToSystemBlits = false;
    }

    DEBUG_INFO("DirectDraw7_Set_Video_Mode() - SystemToVideoBlits = %s\n", SystemToVideoBlits ? "true" : "false");
    DEBUG_INFO("DirectDraw7_Set_Video_Mode() - VideoToSystemBlits = %s\n", VideoToSystemBlits ? "true" : "false");
    DEBUG_INFO("DirectDraw7_Set_Video_Mode() - SystemToSystemBlits = %s\n", SystemToSystemBlits ? "true" : "false");

    //DEBUG_INFO("DirectDraw7_Set_Video_Mode(exit)\n");

    return true;
}


/**
 *  
 *  
 *  @author: CCHyper
 */
void DirectDraw7_Check_Overlapped_Blit_Capability()
{
    DEBUG_INFO("DirectDraw7_Check_Overlapped_Blit_Capability(enter)\n");

    //D7Surface test_buffer(64, 64);
    //BDSurface test_buffer(64, 64);
    BSurface test_buffer(64, 64, 2);

    DEBUG_INFO("DirectDraw7_Check_Overlapped_Blit_Capability() - Checking hardware region fill capability...\n");

    /**
     *  Assume we cant until we find out otherwise.
     */
    DSurface::AllowHardwareBlitFills = false;
    OverlappedVideoBlits = false;

    test_buffer.XSurface::Clear();

    Point2D pt1(5, 5);
    if (test_buffer.XSurface::Get_Pixel(pt1) == 0) {
        DSurface::AllowHardwareBlitFills = true;
    } else {
        test_buffer.XSurface::Fill(1);
        if (test_buffer.XSurface::Get_Pixel(pt1) == 1) {
            DSurface::AllowHardwareBlitFills = true;
        }
    }

    DEBUG_INFO("DirectDraw7_Check_Overlapped_Blit_Capability() - Hardware region fill check %s!\n", DSurface::AllowHardwareBlitFills ? "PASSED" : "FAILED");

    DEBUG_INFO("DirectDraw7_Check_Overlapped_Blit_Capability() - Checking overlapped blit capability...\n");

    test_buffer.XSurface::Fill(0);

    /**
     *  Plot a pixel in the top left corner of the buffer.
     */
    Point2D pt2(0, 0);
    test_buffer.XSurface::Put_Pixel(pt2, 255);

    /**
     *  Blit the buffer down by one line. If we end up with a vertical
     *  strip of pixel 255's then overlapped blits dont work.
     */
    Rect dest_rect(0, 1, test_buffer.Get_Width(), test_buffer.Get_Height()-1);
    Rect src_rect(0, 0, test_buffer.Get_Width(), test_buffer.Get_Height()-1);
    //test_buffer.D7Surface::Copy_From(dest_rect, test_buffer, src_rect);
    //test_buffer.BDSurface::Copy_From(dest_rect, test_buffer, src_rect);
    test_buffer.BSurface::Copy_From(dest_rect, test_buffer, src_rect);

    Point2D pt3(0, 5);
    if (test_buffer.XSurface::Get_Pixel(pt3) == 0) {
        OverlappedVideoBlits = true;
    }

    DEBUG_INFO("DirectDraw7_Check_Overlapped_Blit_Capability() - Overlapped blit capability check %s!\n", OverlappedVideoBlits ? "PASSED" : "FAILED");

    DEBUG_INFO("DirectDraw7_Check_Overlapped_Blit_Capability(exit)\n");
}


/**
 *  
 *  
 *  @author: CCHyper
 */
bool DirectDraw7_Prep(HWND hWnd)
{
    //DEBUG_INFO("DirectDraw7_Prep(enter)\n");

    /**
     *  If there is not currently a direct draw object then we need to define one.
     */
    if (DirectDraw7Object == nullptr) {

        DEBUG_INFO("DirectDraw7_Prep() - Creating DirectDraw7Object().\n");

        HRESULT ddrval;

        /**
         *  Create the Direct Draw object. This is used to access anything DirectDraw does.
         */
        ddrval = DirectDrawCreateEx(nullptr, (LPVOID *)&DirectDraw7Object, IID_IDirectDraw7, nullptr);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("DirectDraw7_Prep()\n\nDirectDrawCreateEx failed with error code %08X\n", ddrval);
            DirectDraw7Object = nullptr;
            return false;
        }

        DEBUG_INFO("DirectDraw7_Prep() - DirectDraw7Object objected created sucessfully.\n");

        if (Debug_Windowed) {
            ddrval = DirectDraw7Object->SetCooperativeLevel(nullptr, DDSCL_NORMAL);
        } else {
            ddrval = DirectDraw7Object->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
        }
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("DirectDraw7_Prep()\n\nSetCooperativeLevel failed with error code %08X\n", ddrval);
            DirectDraw7Object->Release();
            DirectDraw7Object = nullptr;
            return false;
        }
    }

    //DEBUG_INFO("DirectDraw7_Prep(exit)\n");

    return true;
}


/**
 *  Waits for the DirectDraw blitter to become idle.
 * 
 *  @author: CCHyper
 */
void DirectDraw7_Wait_Blit()
{
    HRESULT	return_code;

    ASSERT(DirectDraw7PrimarySurface != nullptr);

    do {
        return_code = DirectDraw7PrimarySurface->GetBltStatus(DDGBS_ISBLTDONE);
    } while (return_code != DD_OK && return_code != DDERR_SURFACELOST);
}


/**
 *  Flip the primary surface to the direct draw surface.
 * 
 *  @author: CCHyper, DarthJane
 */
void DirectDraw7_Flip(XSurface *surface)
{
    //DEBUG_INFO("DirectDraw7_Flip(enter)\n");

    ASSERT(surface != nullptr);

    if (!DirectDraw7_Can_Flip()) {
        DEBUG_WARNING("DirectDraw7_Flip() - Not ready to flip yet!\n");
        return;
    }

    HRESULT ddrval = 0;

    DDSURFACEDESC2 ddsDesc;
    DDRAW_INIT_STRUCT(ddsDesc);

    /**
     *  Lock the surfaces before we access them.
     */
    ddrval = DirectDraw7BackSurface->Lock(nullptr, &ddsDesc, DDLOCK_WAIT|DDLOCK_NOSYSLOCK, nullptr);
    if (FAILED(ddrval)) {
        DEBUG_ERROR("DirectDraw7_Flip() - Failed to lock direct draw surface!\n");
        return;
    }

    unsigned short *srcptr = reinterpret_cast<unsigned short *>(surface->Lock());
    if (!srcptr) {
        DEBUG_ERROR("DirectDraw7_Flip() - Failed to lock game surface!\n");
        return;
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
    if (Debug_Windowed) {

        if (!DirectDraw7_Can_Blit()) {
            DEBUG_WARNING("DirectDraw7_Flip() - Unable to blit to primary surface at this time!\n");
            return;
        }

        /**
         *  Find out where on the primary surface our window lives and move
         *  and move it to the correct screen offset.
         */
        RECT rcSrc;  // source blit rectangle
        RECT rcDest; // destination blit rectangle
        POINT p { 0, 0 };

        ClientToScreen(MainWindow, &p);
        GetClientRect(MainWindow, &rcDest);
        OffsetRect(&rcDest, p.x, p.y);
        SetRect(&rcSrc, 0, 0, Options.ScreenWidth, Options.ScreenHeight);

        ddrval = DirectDraw7PrimarySurface->Blt(&rcDest, DirectDraw7BackSurface, &rcSrc, DDBLT_WAIT, nullptr);
        if (FAILED(ddrval)) {
            DEBUG_ERROR("DirectDraw7_Flip() - Blt failed with error code %08X\n", ddrval);
        }

    } else {

        if (!DirectDraw7_Can_Flip()) {
            DEBUG_WARNING("DirectDraw7_Flip() - Unable to flip primary surface at this time!\n");
            return;
        }

        ddrval = DirectDraw7PrimarySurface->Flip(nullptr, DDFLIP_WAIT);
        if (FAILED(ddrval)) {
            DEBUG_ERROR("DirectDraw7_Flip() - Flip failed with error code %08X\n", ddrval);
        }

    }

    //DEBUG_INFO("DirectDraw7_Flip(exit)\n");
}


/**
 *  Is the surface ready to immediately accept blitting?
 * 
 *  @author: CCHyper
 */
bool DirectDraw7_Can_Blit()
{
    return DirectDraw7PrimarySurface && DirectDraw7PrimarySurface->GetBltStatus(DDGBS_CANBLT) == DD_OK;
}


/**
 *  Is the surface ready to immediately flip?
 * 
 *  @author: CCHyper
 */
bool DirectDraw7_Can_Flip()
{
    return DirectDraw7PrimarySurface && DirectDraw7PrimarySurface->GetFlipStatus(DDGFS_CANFLIP) == DD_OK;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool DirectDraw7_Is_Surface_Lost()
{
    return DirectDraw7PrimarySurface && DirectDraw7PrimarySurface->IsLost() == DDERR_SURFACELOST;
}


/**
 *  
 *  
 *  @author: CCHyper
 */
void DirectDraw7_Focus_Loss()
{
}


/**
 *  
 *  
 *  @author: CCHyper
 */
void DirectDraw7_Focus_Restore()
{
    if (!Debug_Windowed) {
#if 0 // TODO
        if (AlternateSurface) {
            AlternateSurface->Restore_Check()
        }
        if (HiddenSurface) {
            HiddenSurface->Restore_Check()
        }
        if (CompositeSurface && CompositeSurface->Restore_Check() && (!InScenario || Frame > 16)) {
            CompositeSurface->Clear();
        }
        if (TileSurface && TileSurface->Restore_Check() && (!InScenario || Frame > 16)) {
            TileSurface->Clear();
        }
        if (SidebarSurface && SidebarSurface->Restore_Check() && (!InScenario || Frame > 16)) {
            SidebarSurface->Clear();
        }
        if (PrimarySurface && PrimarySurface->Restore_Check() && (!InScenario || Frame > 16)) {
            PrimarySurface->Clear();
        }
#endif
        DirectDraw7_Clear_Surface(DirectDraw7BackSurface);
        //DirectDraw7_Clear_Surface(DirectDraw7PrimarySurface);
    }
    //DirectDraw7_Flip(PrimarySurface);
}


/**
 *  
 *  
 *  @author: CCHyper
 */
bool DirectDraw7_Clear_Surface(LPDIRECTDRAWSURFACE7 lpSurface)
{
    if (!lpSurface) {
        return false;
    }

    DDBLTFX ddbltfx;
    DDRAW_INIT_STRUCT(ddbltfx);

    ddbltfx.dwFillColor = 0; 

    HRESULT hr = lpSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx);
    return SUCCEEDED(hr);
}


















































#if 0


USHORT rgb_24_2_565(int r, int g, int b)  
{  
return (USHORT)(((unsigned(r) << 8) & 0xF800) |   
            ((unsigned(g) << 3) & 0x7E0)  |  
            ((unsigned(b) >> 3)));  
}  

 #define RGB565_MASK_RED        0xF800   

    #define RGB565_MASK_GREEN                         0x07E0 
    #define RGB565_MASK_BLUE                         0x001F 
 void rgb565_2_rgb24(BYTE *rgb24, WORD rgb565)  
{   
    //extract RGB 
     rgb24[2] = (rgb565 & RGB565_MASK_RED) >> 11;       // R 
     rgb24[1] = (rgb565 & RGB565_MASK_GREEN) >> 5;      // G
     rgb24[0] = (rgb565 & RGB565_MASK_BLUE);            // B
    //amplify the image 
     //rgb24[2] <<= 3;  
     //rgb24[1] <<= 2;  
     //rgb24[0] <<= 3;  
}   


#endif







    //std::srand(timeGetTime());

    //std::memset(ddbuffptr, 999999, (surface->Width*surface->Height*surface->BytesPerPixel));
    //std::memset(ddbuffptr, std::rand(), (surface->Width*surface->Height*surface->BytesPerPixel));
    //std::memcpy(ddbuffptr, buffptr16, (surface->Width*surface->Height*surface->BytesPerPixel));

    //static BSurface *pic = nullptr;
    //if (!pic) {
    //    pic = Read_PNG_File(&RawFileClass("LOAD400A.PNG"));
    //}

#if 0
    static struct rgb {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };
    rgb *rgbptr = (rgb *)ddbuffptr;
    //unsigned short *p = (unsigned short *)pic->Lock();
    //unsigned short *p = (unsigned short *)surface->Lock();

    //for (int y = 0; y < pic->Height; ++y) {
        //for (int x = 0; x < pic->Width; ++x) {
            //unsigned short *p = (unsigned short *)pic->Lock(x, y);
            //int n = RGB565_To_RGB888(*p);
            //ddbuffptr[x * sizeof(uint32_t) * y] = *p;

#endif



    /**
     *  RGB565 to RGB888
     */
#if 0
    unsigned short *p = (unsigned short *)surface->Lock();
    for (int i = 0; i < (surface->Width * surface->Height); ++i) {
        Copy_Pixel_Row_565_To_888();
        unsigned short value = *(p++);
        unsigned char r = (value & 0xF800) >> 11; // Extract the 5 R bits
        unsigned char g = (value & 0x07E0) >> 5;  // Extract the 6 G bits
        unsigned char b = (value & 0x001F);       // Extract the 5 B bits
        //*ddbuffptr++ = (r * 255) / 31;
        //*ddbuffptr++ = (g * 255) / 63;
        //*ddbuffptr = (b * 255) / 31;
        *ddbuffptr++ = (r << 3) | (r & 7);
        *ddbuffptr++ = (g << 2) | (g & 3);
        *ddbuffptr++ = (b << 3) | (b & 7);
        //ddbuffptr++;
    }
#endif

#if 0
    /**
     *  Copy pixel data from the games 16bit surface to the DirectDraw 32bit surface.
     */
    unsigned char *srcptr = (unsigned char *)surface->Lock();
    unsigned char *dstptr = reinterpret_cast<unsigned char *>(ddsDesc.lpSurface);
    for (int y = 0; y < surface->Height; ++y) {
        Copy_Pixel_Row_565_To_888(srcptr, dstptr, surface->Width);
    }
#endif






















// Old stuff


#if 0

/**
 *  Create the new 32bit Primary, Back and Clipper
 */
bool DirectDraw_Create_New_Primary_And_Back()
{
    //std::atexit(DirectDraw_Shutdown);

    HRESULT ddrval = 0;

    DDSURFACEDESC2 ddsDesc;
    DDRAW_INIT_STRUCT(ddsDesc);
    ddsDesc.dwFlags = DDSD_CAPS;
    ddsDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    // TODO, these cause CreateSurface to fail with invalid caps.
    //ddsDesc.ddsCaps.dwCaps |= DDSCAPS_COMPLEX/*|DDSCAPS_VIDEOMEMORY|DDSCAPS_FRONTBUFFER*/;
    ddsDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

    if (!Debug_Windowed) {
        ddsDesc.ddsCaps.dwCaps |= DDSCAPS_FLIP;

        ddsDesc.dwFlags |= DDSD_BACKBUFFERCOUNT;
        ddsDesc.dwBackBufferCount = (TripleBuffering ? 2 : 1);
    }

    ddrval = DirectDraw7Object->CreateSurface(&ddsDesc, &DirectDraw7PrimarySurface, nullptr);
    if (FAILED(ddrval)) {
        DDRAW_ERROR_MSGBOX("Create_New_Primary_And_Back()\n\CreateSurface failed with error code %08X\n", ddrval);
        return false;
    }

    New_VideoSurfaceDescription = new DDSURFACEDESC2;
    if (!New_VideoSurfaceDescription) {
        return false;
    }
    DDRAW_INIT_STRUCT_PTR(New_VideoSurfaceDescription);

    ddrval = DirectDraw7PrimarySurface->GetSurfaceDesc(New_VideoSurfaceDescription);
    if (FAILED(ddrval)) {
        DDRAW_ERROR_MSGBOX("Create_New_Primary_And_Back()\n\GetSurfaceDesc failed with error code %08X\n", ddrval);
        return false;
    }

    //DDSURFACEDESC ddsDesc;
    DDRAW_INIT_STRUCT(ddsDesc);
    ddsDesc.dwFlags = DDSD_CAPS;
    ddsDesc.dwFlags |= DDSD_WIDTH|DDSD_HEIGHT;
    ddsDesc.dwWidth = Options.ScreenWidth;
    ddsDesc.dwHeight = Options.ScreenHeight;
    ddsDesc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY|DDSCAPS_OFFSCREENPLAIN;
    // TODO, these cause CreateSurface to fail with invalid caps.
    //ddsDesc.ddsCaps.dwCaps = /*DDSCAPS_COMPLEX|DDSCAPS_VIDEOMEMORY|*/DDSCAPS_OFFSCREENPLAIN;
    ddrval = DirectDraw7Object->CreateSurface(&ddsDesc, &DirectDraw7StagingSurface, nullptr);

    //DDSCAPS ddsCaps;
    //ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    //ddrval = DirectDraw7PrimarySurface->GetAttachedSurface(&ddsCaps, &DirectDraw7StagingSurface);

    //if (FAILED(ddrval)) {
    //    DDRAW_ERROR_MSGBOX("Create_New_Primary_And_Back()\n\GetAttachedSurface failed with error code %08X\n", ddrval);
    //    return false;
    //}

    if (!Debug_Windowed) {

        DDSCAPS2 ddsCaps;
        ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

        // TODO use CreateSurface for back buffer instead?
        //ddsDesc.dwSize = sizeof(ddsDesc);
        //ddsDesc.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
        //ddsDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        //ddsDesc.dwWidth = 640;
        //ddsDesc.dwHeight = 400;

        ddrval = DirectDraw7PrimarySurface->GetAttachedSurface(&ddsCaps, &DirectDraw7BackSurface);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("Create_New_Primary_And_Back()\n\GetAttachedSurface failed with error code %08X\n", ddrval);
            return false;
        }
    }

    if (Debug_Windowed) {

        // Create the clipper using the DirectDraw object
        ddrval = DirectDraw7Object->CreateClipper(0, &DirectDraw7Clipper, nullptr);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("Create_New_Primary_And_Back()\n\CreateClipper failed with error code %08X\n", ddrval);
            return false;
        }

        // Assign window's HWND to the clipper
        ddrval = DirectDraw7Clipper->SetHWnd(0, GetActiveWindow());
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("Create_New_Primary_And_Back()\n\SetHWnd failed with error code %08X\n", ddrval);
            return false;
        }

        // Attach the clipper to the primary surface
        ddrval = DirectDraw7PrimarySurface->SetClipper(DirectDraw7Clipper);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("Create_New_Primary_And_Back()\n\SetClipper failed with error code %08X\n", ddrval);
            return false;
        }
    }

    return true;
}

#endif










// create primary stuff


#if 0
    //DEBUG_INFO("D7Surface_Create_Primary(enter)\n");

    if (!DirectDraw7Object) {
        DEBUG_ERROR("D7Surface::Create_Primary() - DirectDraw7Object is null!\n");
        MessageBox(MainWindow, "DirectDraw7Object is null!", "Tiberian Sun", MB_OK);
        Fatal("D7Surface::Create_Primary() - DirectDraw7Object is null!\n");
        return nullptr;
    }

#if 1
    if (!DirectDraw_Create_New_Primary_And_Back()) {
        MessageBox(MainWindow, "Failed to create new DirectDraw surfaces!", "Tiberian Sun", MB_OK);
        Fatal("D7Surface::Create_Primary() - Failed to create new DirectDraw surfaces!\n");
        return nullptr;
    }
#endif

    unsigned int caps = Get_Video_Hardware_Capabilities();

    DSurface::AllowStretchBlits = (caps & VIDEO_ALIGN_BOUNDARY) ? true : false;
    DEBUG_INFO("DirectDraw7_Create_Primary() - DSurface::AllowStretchBlits = %s\n", DSurface::AllowStretchBlits ? "true" : "false");

    DSurface::AllowHardwareBlitFills = (caps & VIDEO_COLOR_FILL) ? true : false;
    DEBUG_INFO("DirectDraw7_Create_Primary() - DSurface::AllowHardwareBlitFills = %s\n", DSurface::AllowHardwareBlitFills ? "true" : "false");

    D7Surface *primary_surface = new D7Surface;
    ASSERT(primary_surface != nullptr);

    if (primary_surface == nullptr) {
        return nullptr;
    }

    HRESULT ddrval = 0;

    primary_surface->VideoSurfaceDescription->dwFlags = DDSD_CAPS;
    primary_surface->VideoSurfaceDescription->dwFlags |= DDSD_WIDTH|DDSD_HEIGHT;
    primary_surface->VideoSurfaceDescription->dwWidth = Options.ScreenWidth;
    primary_surface->VideoSurfaceDescription->dwHeight = Options.ScreenHeight;

    //primary_surface->VideoSurfaceDescription->ddsCaps.dwCaps = /*DDSCAPS_COMPLEX|DDSCAPS_VIDEOMEMORY|*/DDSCAPS_OFFSCREENPLAIN;
    primary_surface->VideoSurfaceDescription->ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY|DDSCAPS_OFFSCREENPLAIN;
    //primary_surface->VideoSurfaceDescription->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    //if (backbuffer_surface != nullptr) {
    //    primary_surface->VideoSurfaceDescription->ddsCaps.dwCaps |= DDSCAPS_FLIP|DDSCAPS_COMPLEX;
    //    primary_surface->VideoSurfaceDescription->dwFlags |= DDSD_BACKBUFFERCOUNT;
    //
    //    primary_surface->VideoSurfaceDescription->dwBackBufferCount = (TripleBuffering ? 2 : 1);
    //}

    //ddrval = DirectDraw7Object->CreateSurface(primary_surface->VideoSurfaceDescription, &primary_surface->VideoSurfacePtr, nullptr);
    ddrval = DirectDraw7Object->CreateSurface((LPDDSURFACEDESC2)primary_surface->VideoSurfaceDescription,
                                            (LPDIRECTDRAWSURFACE7 *)&primary_surface->VideoSurfacePtr, nullptr);
    if (FAILED(ddrval)) {
        DDRAW_ERROR_MSGBOX("Create_New_Primary_And_Back()\n\CreateSurface failed with error code %08X\n", ddrval);
        delete primary_surface;
        return nullptr;
    }

    //if (backbuffer_surface != nullptr) {
    //
    //    LPDIRECTDRAWSURFACE lpDDSBack;
    //    DDSCAPS ddsCaps;
    //    ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
    //
    //    ddrval = primary_surface->VideoSurfacePtr->GetAttachedSurface(&ddsCaps, &lpDDSBack);
    //    if (FAILED(ddrval)) {
    //        delete primary_surface;
    //        return nullptr;
    //    }
    //
    //    DirectDraw7BackSurface = lpDDSBack;
    //
    //    *backbuffer_surface = new D7Surface(lpDDSBack);
    //    ASSERT(*backbuffer_surface != nullptr);
    //}

    DDRAW_INIT_STRUCT_PTR(primary_surface->VideoSurfaceDescription);

    ddrval = primary_surface->VideoSurfacePtr->GetSurfaceDesc(primary_surface->VideoSurfaceDescription);
    //ddrval = primary_surface->VideoSurfacePtr->GetSurfaceDesc(primary_surface->VideoSurfaceDescription);
    if (FAILED(ddrval)) {
        DDRAW_ERROR_MSGBOX("Create_New_Primary_And_Back()\n\GetSurfaceDesc failed with error code %08X\n", ddrval);
        delete primary_surface;
        return nullptr;
    }

    primary_surface->BytesPerPixel = ((primary_surface->VideoSurfaceDescription->ddpfPixelFormat.dwRGBBitCount + 7) / 8);
    DEBUG_INFO("DirectDraw7_Create_Primary() - Setting BytesPerPixel to %d\n", ((primary_surface->VideoSurfaceDescription->ddpfPixelFormat.dwRGBBitCount + 7) / 8));

    primary_surface->InVideoRam = (primary_surface->VideoSurfaceDescription->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;
    DEBUG_INFO("DirectDraw7_Create_Primary() - In video memory = %s\n", primary_surface->InVideoRam ? "true" : "false");

    DEBUG_INFO("DirectDraw7_Create_Primary() - Setting surface dimensions to %d x %d.\n", primary_surface->VideoSurfaceDescription->dwWidth, primary_surface->VideoSurfaceDescription->dwHeight);
    primary_surface->Width = primary_surface->VideoSurfaceDescription->dwWidth;
    primary_surface->Height = primary_surface->VideoSurfaceDescription->dwHeight;

    DirectDraw7PrimarySurface = primary_surface->VideoSurfacePtr;
    ASSERT(DirectDraw7PrimarySurface != nullptr);

    primary_surface->IsAllocated = true;

#if 0
    if (Debug_Windowed) {
        ddrval = DirectDraw7Object->CreateClipper(0, &DirectDrawClipper, nullptr);
        if (FAILED(ddrval)) {
            delete primary_surface;
            return nullptr;
        }

        ddrval = DirectDrawClipper->SetHWnd(0, GetActiveWindow());
        if (FAILED(ddrval)) {
            delete primary_surface;
            return nullptr;
        }

        ddrval = primary_surface->VideoSurfacePtr->SetClipper(DirectDrawClipper);
        if (FAILED(ddrval)) {
            delete primary_surface;
            return nullptr;
        }
    }
#endif

    DDPIXELFORMAT pixel_format = primary_surface->VideoSurfaceDescription->ddpfPixelFormat;

    DSurface::RedLeft = 0;
    DSurface::RedRight = 0;
    DSurface::GreenLeft = 0;
    DSurface::GreenRight = 0;
    DSurface::BlueLeft = 0;
    DSurface::BlueRight = 0;

    Calculate_Mask_Info(pixel_format.dwRBitMask, DSurface::RedLeft, DSurface::RedRight, (primary_surface->BytesPerPixel * 8));
    Calculate_Mask_Info(pixel_format.dwGBitMask, DSurface::GreenLeft, DSurface::GreenRight, (primary_surface->BytesPerPixel * 8));
    Calculate_Mask_Info(pixel_format.dwBBitMask, DSurface::BlueLeft, DSurface::BlueRight, (primary_surface->BytesPerPixel * 8));

    switch (primary_surface->BytesPerPixel) {

        /**
         *  32bit
         */
        case 4:
            DEBUG_WARNING("D7Surface::Create_Primary() - RGBPixelFormat is RGB888 ?????.\n");
            break;

        /**
         *  24bit
         */
        case 3:
            break;

        /**
         *  16bit
         */
        case 2:
            if (DSurface::RedLeft == 10 && DSurface::RedRight == 3 && DSurface::GreenLeft == 5 && DSurface::GreenRight == 3 && DSurface::BlueLeft == 0 && D7Surface::BlueRight == 3) {
                DSurface::RGBPixelFormat = RGB555;
                DEBUG_INFO("DirectDraw7_Create_Primary() - RGBPixelFormat is RGB555.\n");

            } else if (DSurface::RedLeft == 11 && DSurface::RedRight == 3 && DSurface::GreenLeft == 6 && DSurface::GreenRight == 3 && DSurface::BlueLeft == 0 && D7Surface::BlueRight == 2) {
                DSurface::RGBPixelFormat = RGB556;
                DEBUG_INFO("DirectDraw7_Create_Primary() - RGBPixelFormat is RGB556.\n");

            } else if (DSurface::RedLeft == 11 && DSurface::RedRight == 3 && DSurface::GreenLeft == 5 && DSurface::GreenRight == 2 && DSurface::BlueLeft == 0 && D7Surface::BlueRight == 3) {
                DSurface::RGBPixelFormat = RGB565;
                DEBUG_INFO("DirectDraw7_Create_Primary() - RGBPixelFormat is RGB565.\n");

            } else if (DSurface::RedLeft == 10 && DSurface::RedRight == 3 && DSurface::GreenLeft == 5 && DSurface::GreenRight == 2 && DSurface::BlueLeft == 0 && D7Surface::BlueRight == 3) {
                DSurface::RGBPixelFormat = RGB655;
                DEBUG_INFO("DirectDraw7_Create_Primary() - RGBPixelFormat is RGB655.\n");

            } else {
                DSurface::RGBPixelFormat = RGBINVALID;
                DEBUG_WARNING("D7Surface::Create_Primary() - RGBPixelFormat is unsupported!.\n");
            }

            break;

        /**
         *  8bit
         */
        case 1:
            break;
    };

    DSurface::ColorGrey = DSurface::RGBA_To_Pixel(127, 127, 127);
    DSurface::ColorMidGrey = DSurface::RGBA_To_Pixel(63, 63, 63);
    DSurface::ColorDarkGrey = DSurface::RGBA_To_Pixel(31, 31, 31);

    PrimaryBytesPerPixel = primary_surface->BytesPerPixel;
    return primary_surface;
#endif

