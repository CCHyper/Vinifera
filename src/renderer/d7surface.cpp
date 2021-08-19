/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          D7SURFACE.CPP
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
#include "d7surface.h"
#include "ddraw7_support.h"
#include "renderer.h"
#include "tibsun_globals.h"
#include "bsurface.h"
#include "options.h"
#include "rgb.h"
#include "textprint.h"
#include "debughandler.h"
#include "asserthandler.h"




#if 0
/**
 *  
 * 
 *  @author: CCHyper
 */
D7Surface::D7Surface() :
    XSurface(),
    BufferPtr(nullptr),
    IsAllocated(false),
    InVideoRam(false),
    VideoSurfacePtr(nullptr),
    VideoSurfaceDescription(nullptr)
{
    VideoSurfaceDescription = new DDSURFACEDESC2;
    ASSERT(VideoSurfaceDescription != nullptr);
    if (VideoSurfaceDescription != nullptr) {
        DDRAW_INIT_STRUCT_PTR(VideoSurfaceDescription);
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
D7Surface::D7Surface(int width, int height, bool system_mem) :
    XSurface(width, height),
    BufferPtr(nullptr),
    IsAllocated(false),
    InVideoRam(false),
    VideoSurfacePtr(nullptr),
    VideoSurfaceDescription(nullptr)
{
    VideoSurfaceDescription = new DDSURFACEDESC2;
    ASSERT(VideoSurfaceDescription != nullptr);
    if (VideoSurfaceDescription != nullptr) {

        HRESULT ddrval = 0;

        DDRAW_INIT_STRUCT_PTR(VideoSurfaceDescription);

        /**
         *  Tell DirectDraw which members are valid. 
         */
        VideoSurfaceDescription->dwFlags = (DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT);

        VideoSurfaceDescription->dwWidth = width;
        VideoSurfaceDescription->dwHeight = height;

        /**
         *  #BUGFIX:
         *  Made DDSCAPS_VIDEOMEMORY the default dwCaps.
         */
        if (system_mem) {
            DEBUG_INFO("D7Surface::D7Surface() - Creating surface in system memory (DDSCAPS_SYSTEMMEMORY).\n");
            VideoSurfaceDescription->ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
        } else {
            DEBUG_INFO("D7Surface::D7Surface() - Creating surface in video memory (DDSCAPS_VIDEOMEMORY).\n");
            VideoSurfaceDescription->ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
        }

        if (!DirectDraw7Object) {
            DEBUG_ERROR("D7Surface::D7Surface() - DirectDraw7Object is null!\n");
            return;
        }

        ddrval = DirectDraw7Object->CreateSurface(VideoSurfaceDescription, &VideoSurfacePtr, nullptr);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("D7Surface::D7Surface()\n\nCreateSurface failed with error code %08X\n", ddrval);
            return;
        }

        if (!VideoSurfacePtr) {
            DEBUG_ERROR("D7Surface::D7Surface() - VideoSurfacePtr is null!\n");
            return;
        }

        DDRAW_INIT_STRUCT_PTR(VideoSurfaceDescription);

        //DEBUG_INFO("D7Surface::D7Surface() - About to call GetSurfaceDesc\n");

        ddrval = VideoSurfacePtr->GetSurfaceDesc(VideoSurfaceDescription);
        if (FAILED(ddrval)) {
            DDRAW_ERROR_MSGBOX("D7Surface::D7Surface()\n\nGetSurfaceDesc failed with error code %08X\n", ddrval);
            return;
        }

        BytesPerPixel = ((VideoSurfaceDescription->ddpfPixelFormat.dwRGBBitCount + 7) / 8);
        InVideoRam = (VideoSurfaceDescription->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;

        DEBUG_INFO("D7Surface::D7Surface() - InVideoRam = %s\n", InVideoRam ? "true" : "false");
        DEBUG_INFO("D7Surface::D7Surface() - Width = %d, Height = %d, BPP = %d\n", Width, Height, BytesPerPixel);
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
D7Surface::D7Surface(LPDIRECTDRAWSURFACE7 surface) :
    XSurface(),
    BufferPtr(nullptr),
    IsAllocated(false),
    InVideoRam(false),
    VideoSurfacePtr(nullptr),
    VideoSurfaceDescription(nullptr)
{
    if (surface != nullptr) {

        HRESULT ddrval = 0;

        VideoSurfacePtr = surface;
        if (!VideoSurfacePtr) {
            DEBUG_ERROR("D7Surface::D7Surface() - VideoSurfacePtr is null!\n");
            return;
        }

        VideoSurfaceDescription = new DDSURFACEDESC2;
        ASSERT(VideoSurfaceDescription != nullptr);
        if (VideoSurfaceDescription != nullptr) {

            DDRAW_INIT_STRUCT_PTR(VideoSurfaceDescription);

            //DEBUG_INFO("D7Surface::D7Surface() - About to call GetSurfaceDesc\n");

            ddrval = VideoSurfacePtr->GetSurfaceDesc(VideoSurfaceDescription);
            if (FAILED(ddrval)) {
                DDRAW_ERROR_MSGBOX("D7Surface::D7Surface()\n\nGetSurfaceDesc failed with error code %08X\n", ddrval);
                return;
            }

            BytesPerPixel = ((VideoSurfaceDescription->ddpfPixelFormat.dwRGBBitCount + 7) / 8);
            InVideoRam = (VideoSurfaceDescription->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;

            Width = VideoSurfaceDescription->dwWidth;
            Height = VideoSurfaceDescription->dwHeight;

            DEBUG_INFO("D7Surface::D7Surface() - InVideoRam = %s\n", InVideoRam ? "true" : "false");
            DEBUG_INFO("D7Surface::D7Surface() - Width = %d, Height = %d, BPP = %d\n", Width, Height, BytesPerPixel);
        }
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
D7Surface::~D7Surface()
{
    if (IsAllocated) {
        if (VideoSurfacePtr != nullptr) {
            if (DirectDraw7Clipper != nullptr) {
                VideoSurfacePtr->SetClipper(nullptr);
                DirectDraw7Clipper->Release();
                DirectDraw7Clipper = nullptr;
            }
        }
    }
    if (VideoSurfaceDescription != nullptr) {
        delete VideoSurfaceDescription;
        VideoSurfaceDescription = nullptr;
    }
    if (VideoSurfacePtr != nullptr) {
        VideoSurfacePtr->Release();
        VideoSurfacePtr = nullptr;
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Copy_From(Rect &toarea, Rect &torect, Surface &fromsurface, Rect &fromarea, Rect &fromrect, bool trans_blit, bool a7)
{
    if (!toarea.Is_Valid() || !fromarea.Is_Valid()) {
        return false;
    }

    if (!torect.Is_Valid() || !fromrect.Is_Valid()) {
        return false;
    }

    if (!VideoSurfacePtr) {
        DEBUG_WARNING("D7Surface::Fill_Rect() - VideoSurfacePtr is null!\n");
        return false;
    }

    bool use_xsurface = false;

    if (!fromsurface.entry_80() || fromsurface.Is_Locked() || trans_blit) {
        use_xsurface = true;
    }

    if (!use_xsurface && Get_Bytes_Per_Pixel() != fromsurface.Get_Bytes_Per_Pixel()) {
        use_xsurface = true;
    }

    if (IsAllocated && Debug_Windowed) {
        a7 = false;
    }

    if (!use_xsurface && a7) {
        if (!In_Video_Ram() || fromsurface.entry_80() && !static_cast<DSurface &>(fromsurface).In_Video_Ram()) {
            if (fromrect.Width == torect.Width && fromrect.Height == torect.Height) {
                use_xsurface = true;
            }
        }
    }

    if (!Restore_Check()) {
        return false;
    }

    if (use_xsurface) {
        return XSurface::Copy_From(torect, fromsurface, fromrect, trans_blit, true);
    }

    Rect rect4 = Intersect(fromarea, fromsurface.Get_Rect());
    Rect rect2 = Intersect(toarea, Get_Rect());

    //if (!func_7BBE20(torect, rect2, fromrect, rect4)) {
    //    return false;
    //}

    RECT dest_rectangle;
    dest_rectangle.left = rect2.X + torect.X;
    dest_rectangle.top = rect2.Y + torect.Y;
    dest_rectangle.right = torect.Width + rect2.X + torect.X;
    dest_rectangle.bottom = torect.Height + rect2.Y + torect.Y;

    RECT source_rectangle;
    source_rectangle.left = rect4.X + fromrect.X;
    source_rectangle.top = rect4.Y + fromrect.Y;
    source_rectangle.right = fromrect.Width + rect4.X + fromrect.X;
    source_rectangle.bottom = fromrect.Height + rect4.Y + fromrect.Y;

    HRESULT hr = VideoSurfacePtr->Blt(&dest_rectangle, static_cast<D7Surface &>(fromsurface).VideoSurfacePtr, &source_rectangle, DDBLT_WAIT, nullptr);
    return SUCCEEDED(hr);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Copy_From(Rect &torect, Surface &fromsurface, Rect &fromrect, bool trans_blit, bool a5)
{
    Rect toarea = Get_Rect();
    Rect fromarea = fromsurface.Get_Rect();
    return Copy_From(toarea, torect, fromsurface, fromarea, fromrect, trans_blit, a5);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Copy_From(Surface &fromsurface, bool trans_blit, bool a3)
{
    return XSurface::Copy_From(fromsurface, trans_blit, a3);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Fill_Rect(Rect &rect, unsigned color)
{
    Rect my_rect = Get_Rect();
    return D7Surface::Fill_Rect(my_rect, rect, color);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Fill_Rect(Rect &area, Rect &rect, unsigned color)
{
    if (!rect.Is_Valid()) {
        return false;
    }

    if (!VideoSurfacePtr) {
        DEBUG_WARNING("D7Surface::Fill_Rect() - VideoSurfacePtr is null!\n");
        return false;
    }

    if (!DSurface::AllowHardwareBlitFills || Is_Locked() || !Can_Blit()) {
        return XSurface::Fill_Rect(area, rect, color);
    }

    if (!Restore_Check()) {
        return false;
    }

    Rect rect2 = Intersect(area, Get_Rect());

    Rect rect1;
    rect1.X = rect.X + area.X;
    rect1.Y = rect.Y + area.Y;
    rect1.Width = rect.Width;
    rect1.Height = rect.Height;

    Rect rect3 = Intersect(rect1, rect2);

    if (rect3.Width <= 0 || rect3.Height <= 0) {
        return false;
    }

    RECT dest_rectangle;
    dest_rectangle.left = rect3.X;
    dest_rectangle.top = rect3.Y;
    dest_rectangle.right = rect3.Width + rect3.X;
    dest_rectangle.bottom = rect3.Height + rect3.Y;

    DDBLTFX ddBltFx;
    DDRAW_INIT_STRUCT(ddBltFx);
    ddBltFx.dwFillColor = color;

    HRESULT hr = VideoSurfacePtr->Blt(&dest_rectangle, nullptr, nullptr, DDBLT_WAIT|DDBLT_COLORFILL, &ddBltFx);
    return SUCCEEDED(hr);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Fill_Rect_Trans(Rect &rect, const RGBClass &color, unsigned opacity)
{
    int bpp = Get_Bytes_Per_Pixel();
    if (bpp != 2) {
        return false;
    }

    if (!rect.Is_Valid()) {
        return false;
    }

    Rect r1 = Intersect(Get_Rect(), rect);

    if (!r1.Is_Valid()) {
        return false;
    }

    Point2D r1_tl = r1.Top_Left();
    unsigned short *buffptr = (unsigned short *)Lock(r1_tl.X, r1_tl.Y);
    if (buffptr == nullptr) {
        return false;
    }

    opacity = std::min((int)opacity, 100);

    unsigned scale = (opacity * 255) / 100;
    unsigned short delta = (255 - scale) & 0xFFFF;

    unsigned int red_max = (unsigned int)(255 >> DSurface::RedRight) << DSurface::RedLeft;
    unsigned int green_max = (unsigned int)(255 >> DSurface::GreenRight) << DSurface::GreenLeft;
    unsigned int blue_max = (unsigned int)(255 >> DSurface::BlueRight) << DSurface::BlueLeft;

    unsigned short pixel_color = DSurface::RGBA_To_Pixel(color.Red, color.Green, color.Blue);

    unsigned rscaled = scale * (pixel_color & red_max);
    unsigned gscaled = scale * (pixel_color & green_max);
    unsigned bscaled = scale * (pixel_color & blue_max);

    unsigned short rmax = red_max & 0xFFFF;
    unsigned short gmax = green_max & 0xFFFF;
    unsigned short bmax = blue_max & 0xFFFF;

    for (int y = 0; y < r1.Height; ++y) {

        int line = y * (Get_Pitch() / 2);
        unsigned short *pixel = &buffptr[line];


        for (int x = 0; x < r1.Width; ++x) {

            unsigned short current_color = *pixel;
            *pixel++ = (unsigned short)
                       (((current_color & rmax) * (delta + rscaled) >> 8) & rmax)
                     | (((current_color & gmax) * (delta + gscaled) >> 8) & gmax)
                     | (((current_color & bmax) * (delta + bscaled) >> 8) & bmax);

        }
    }

    Unlock();

    return true;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Draw_Line_entry_34(Rect &area, Point2D &start, Point2D &end, unsigned color, int a5, int a6, bool z_only)
{
    return true;        // TODO
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Draw_Line_entry_38(Rect &area, Point2D &start, Point2D &end, int a4, int a5, int a6, bool a7)
{
    return true;        // TODO
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Draw_Line_entry_3C(Rect &area, Point2D &start, Point2D &end, RGBClass &color, int a5, int a6, bool a7, bool a8, bool a9, bool a10, float a11)
{
    return true;        // TODO
}


/**
 *  
 * 
 *  @author: CCHyper
 */
int D7Surface::entry_48(Point2D &start, Point2D &end, unsigned color, bool pattern[], int offset, bool a6)
{
    return 0;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::entry_4C(Point2D &start, Point2D &end, unsigned color, bool a4)
{
    return true;        // TODO
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void *D7Surface::Lock(int x, int y)
{
    if (!Restore_Check()) {
        return nullptr;
    }

    /**
     *  Buffer access sanity check.
     */
    if (x < 0 || y < 0) {
        return nullptr;
    }

    if (!LockLevel) {

        HRESULT ddrval = 0;

        DDSURFACEDESC2 dddesc;
        DDRAW_INIT_STRUCT(dddesc);

        ddrval = VideoSurfacePtr->Lock(nullptr, &dddesc, DDLOCK_WAIT, nullptr);
        if (FAILED(ddrval)) {
            //DEBUG_WARNING("D7Surface::Lock() - Failed to lock primary surface!\n");
            DDRAW_ERROR_MSGBOX("D7Surface::Lock()\n\nLock failed with error code %08X\n", ddrval);
            return nullptr;
        }

        CopyMemory(VideoSurfaceDescription, &dddesc, sizeof(DDSURFACEDESC2));

        /**
         *  Update video description flags.
         */
        BytesPerPixel = ((VideoSurfaceDescription->ddpfPixelFormat.dwRGBBitCount + 7) / 8);
        InVideoRam = (VideoSurfaceDescription->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;

        //DEBUG_INFO("D7Surface::Lock() - InVideoRam = %s\n", InVideoRam ? "true" : "false");
        //DEBUG_INFO("D7Surface::Lock() - Width = %d, Height = %d, BPP = %d\n", Width, Height, BytesPerPixel);

        BufferPtr = VideoSurfaceDescription->lpSurface;
    }

    ++LockLevel;

    /**
     *  Return pointer to the direct draw buffer.
     */
    return Get_Buffer_Ptr(x, y);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Unlock()
{
    HRESULT ddrval = 0;

    Restore_Check();

    if (LockLevel > 0) {

        --LockLevel;

        if (LockLevel == 0) {

            ddrval = VideoSurfacePtr->Unlock(nullptr);
            if (FAILED(ddrval)) {
                //DEBUG_WARNING("D7Surface::Unlock() - Failed to unlock primary surface!\n");
                DDRAW_ERROR_MSGBOX("D7Surface::Unlock()\n\nUnlock failed with error code %08X\n", ddrval);
                return false;
            }

            BufferPtr = nullptr;
        }

        return true;
    }

    return false;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Can_Lock(int x, int y) const
{
    if (LockLevel) {
        return true;
    }

    HRESULT ddrval = 0;

    DDSURFACEDESC2 dddesc;
    DDRAW_INIT_STRUCT(dddesc);

    ddrval = VideoSurfacePtr->Lock(nullptr, &dddesc, 0, nullptr);
    if (FAILED(ddrval)) {
        //DEBUG_WARNING("D7Surface::Can_Lock() - Failed to lock primary surface!\n");
        DDRAW_ERROR_MSGBOX("D7Surface::Can_Lock()\n\nLock failed with error code %08X\n", ddrval);
        return false;
    }

    ddrval = VideoSurfacePtr->Unlock(nullptr);
    if (FAILED(ddrval)) {
        //DEBUG_WARNING("D7Surface::Can_Lock() - Failed to unlock primary surface!\n");
        DDRAW_ERROR_MSGBOX("D7Surface::Can_Lock()\n\nUnlock failed with error code %08X\n", ddrval);
        return false;
    }

    return true;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
int D7Surface::Get_Pitch() const
{
    ASSERT(VideoSurfaceDescription != nullptr);
    return VideoSurfaceDescription->lPitch;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Draw_Line_entry_90(Rect &area, Point2D &start, Point2D &end, RGBClass &a4, RGBClass &a5, float& a6, float& a7)
{
    return true;        // TODO
}


/**
 *  
 * 
 *  @author: CCHyper
 */
HDC D7Surface::Get_DC()
{
    if (!VideoSurfacePtr) {
        DEBUG_WARNING("D7Surface::Get_DC() - VideoSurfacePtr is null!\n");
        return nullptr;
    }

    if (Is_Locked()) {
        return nullptr;
    }

    HDC hdc = nullptr;
    HRESULT ddrval = VideoSurfacePtr->GetDC(&hdc);
    if (FAILED(ddrval)) {
        //DEBUG_INFO("D7Surface::Get_DC() - Failed to create DC!\n");
        DDRAW_ERROR_MSGBOX("D7Surface::Get_DC()\n\nLock failed with error code %08X\n", ddrval);
        return nullptr;
    }
    ++LockLevel;
    return hdc;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Release_DC(HDC hdc)
{
    HRESULT ddrval = VideoSurfacePtr->ReleaseDC(hdc);
    if (FAILED(ddrval)) {
        //DEBUG_INFO("D7Surface::Release_DC() - Failed to create DC!\n");
        DDRAW_ERROR_MSGBOX("D7Surface::Release_DC()\n\nLock failed with error code %08X\n", ddrval);
        return false;
    }
    if (LockLevel > 0) {
        --LockLevel;
    }
    return true;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
DWORD D7Surface::DD_Color_Match(COLORREF dwGDIColor)
{
    HDC hdc = nullptr;
    COLORREF rgb = CLR_INVALID;
    COLORREF color = CLR_INVALID;
    HRESULT ddrval;

    if (dwGDIColor != CLR_INVALID && VideoSurfacePtr->GetDC(&hdc) == DD_OK) {
        rgb = GetPixel(hdc, 0, 0); // Save current pixel value.
        SetPixel(hdc, 0, 0, dwGDIColor); // Set our value.
        VideoSurfacePtr->ReleaseDC(hdc);
    }

    DDSURFACEDESC2 dddesc;
    DDRAW_INIT_STRUCT(dddesc);

    /**
     *  Now lock the surface so we can read back the converted color.
     */
    while (ddrval = VideoSurfacePtr->Lock(nullptr, &dddesc, DDLOCK_SURFACEMEMORYPTR, nullptr) == DDERR_WASSTILLDRAWING);

    if (ddrval == DD_OK) {

        /**
         *  Get DWORD color.
         */
        color = *(COLORREF *)dddesc.lpSurface;

        /*
         *  Skip for 32bit.
         */
        if (dddesc.ddpfPixelFormat.dwRGBBitCount < 32) {
            color &= (1 << dddesc.ddpfPixelFormat.dwRGBBitCount) - 1; // Mask it to bpp.
        }

        VideoSurfacePtr->Unlock(nullptr);
    }

    /**
     *  Now put the color that was there back.
     */
    if (dwGDIColor != CLR_INVALID && VideoSurfacePtr->GetDC(&hdc) == DD_OK) {
        SetPixel(hdc, 0, 0, rgb);
        VideoSurfacePtr->ReleaseDC(hdc);
    }

    return color;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void D7Surface::DD_Set_Color_Key(COLORREF dwGDIColor)
{
    DDCOLORKEY ddcolkey;
    ddcolkey.dwColorSpaceLowValue = 0;
    ddcolkey.dwColorSpaceHighValue = 0;

    COLORREF color = DD_Color_Match(dwGDIColor);
    ddcolkey.dwColorSpaceLowValue = color;
    ddcolkey.dwColorSpaceHighValue = color;

    VideoSurfacePtr->SetColorKey(DDCKEY_SRCBLT, &ddcolkey);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void D7Surface::DD_Get_Surface_Size(Rect &rect)
{
    Restore_Check();

    HRESULT ddrval = 0;

    DDSURFACEDESC2 dddesc;
    DDRAW_INIT_STRUCT(dddesc);
    dddesc.dwFlags = DDSD_WIDTH|DDSD_HEIGHT;

    ddrval = VideoSurfacePtr->GetSurfaceDesc(&dddesc);
    if (FAILED(ddrval)) {
        DDRAW_ERROR_MSGBOX("D7Surface::DD_Get_Surface_Size()\n\nGetSurfaceDesc failed with error code %08X\n", ddrval);
        return;
    }

    rect.X = 0;
    rect.Y = 0;
    rect.Width = dddesc.dwWidth;
    rect.Height = dddesc.dwHeight;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
HRESULT D7Surface::DD_Copy(LPDIRECTDRAWSURFACE dest, LPDIRECTDRAWSURFACE src, Point2D *dest_point, Rect *src_rect, BOOL is_color_keyed)
{
    if (!dest || !src) {
        return DDERR_SURFACELOST;
    }

    HRESULT ddrval;

    /**
     *  Copy the surface.
     */
    for (;;) {
        ddrval = dest->BltFast(dest_point->X, dest_point->Y, src, (LPRECT)src_rect, is_color_keyed ? DDBLTFAST_SRCCOLORKEY : 0); 
        if (ddrval != DDERR_WASSTILLDRAWING) {  

            /**
             *  #NOTE:
             * 
             *  Surfaces should not restore themselves because they don't know how    
             *  to recreate their graphics. If they fail to draw, just return the result     
             *  and let the caller figure it out.
             */  
            return ddrval;
        }   
    }
    return ddrval;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Is_Surface_Lost()
{
    if (!VideoSurfacePtr) {
        //DEBUG_WARNING("D7Surface::Is_Surface_Lost() - VideoSurfacePtr is null!\n");
        return false;
    }

    return VideoSurfacePtr->IsLost() == DDERR_SURFACELOST;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool D7Surface::Restore_Check()
{
    if (!Debug_Windowed && !GameInFocus) {
        return false;
    }

    DWORD ddrval = 0;

    if (VideoSurfacePtr != nullptr) {
        
        if (Is_Surface_Lost()) {

            if (VideoSurfacePtr->Restore() == DD_FALSE || VideoSurfacePtr->IsLost() == DD_FALSE) {
                return false;
            }

            int prev_locklevel = LockLevel;
            if (LockLevel > 0) {
                LockLevel = 0;
                Lock();
                ++LockLevel;
                Unlock();
                LockLevel = prev_locklevel;
            }
        }
    }

    return true;
}
#endif

















#if 0


/**
 *  
 * 
 *  @author: CCHyper
 */
BDSurface::BDSurface() :
    BSurface()
{
}


/**
 *  
 * 
 *  @author: CCHyper
 */
BDSurface::BDSurface(int width, int height, int bpp) :
    BSurface(width, height, 2)
{
}


#if 0
/**
 *  
 * 
 *  @author: CCHyper
 */
BDSurface::BDSurface(int width, int height, bool system_mem) :
    BSurface(width, height, 2)
{
}
#endif


/**
 *  
 * 
 *  @author: CCHyper
 */
BDSurface::BDSurface(LPDIRECTDRAWSURFACE7 surface) :
    BSurface()
{
}


/**
 *  
 * 
 *  @author: CCHyper
 */
BDSurface::~BDSurface()
{
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Copy_From(Rect &toarea, Rect &torect, Surface &fromsurface, Rect &fromarea, Rect &fromrect, bool trans_blit, bool a7)
{
    if (!toarea.Is_Valid() || !fromarea.Is_Valid()) {
        return false;
    }

    if (!torect.Is_Valid() || !fromrect.Is_Valid()) {
        return false;
    }

#if 0
    if (!DirectDraw7BackSurface) {
        DEBUG_WARNING("BDSurface::Fill_Rect() - DirectDraw7BackSurface is null!\n");
        return false;
    }
#endif

    bool use_xsurface = false;

    if (!fromsurface.entry_80() || fromsurface.Is_Locked() || trans_blit) {
        use_xsurface = true;
    }

    if (!use_xsurface && Get_Bytes_Per_Pixel() != fromsurface.Get_Bytes_Per_Pixel()) {
        use_xsurface = true;
    }

    if (Debug_Windowed) {
        a7 = false;
    }

    if (!use_xsurface && a7) {
        if (fromsurface.entry_80() && !static_cast<DSurface &>(fromsurface).In_Video_Ram()) {
            if (fromrect.Width == torect.Width && fromrect.Height == torect.Height) {
                use_xsurface = true;
            }
        }
    }

    if (!Restore_Check()) {
        return false;
    }

    //if (use_xsurface) {
        return XSurface::Copy_From(torect, fromsurface, fromrect, trans_blit, true);
    //}

#if 0
    Rect rect4 = Intersect(fromarea, fromsurface.Get_Rect());
    Rect rect2 = Intersect(toarea, Get_Rect());

    //if (!func_7BBE20(torect, rect2, fromrect, rect4)) {
    //    return false;
    //}

    RECT dest_rectangle;
    dest_rectangle.left = rect2.X + torect.X;
    dest_rectangle.top = rect2.Y + torect.Y;
    dest_rectangle.right = torect.Width + rect2.X + torect.X;
    dest_rectangle.bottom = torect.Height + rect2.Y + torect.Y;

    RECT source_rectangle;
    source_rectangle.left = rect4.X + fromrect.X;
    source_rectangle.top = rect4.Y + fromrect.Y;
    source_rectangle.right = fromrect.Width + rect4.X + fromrect.X;
    source_rectangle.bottom = fromrect.Height + rect4.Y + fromrect.Y;

    HRESULT hr = DirectDraw7BackSurface->Blt(&dest_rectangle, DirectDraw7BackSurface, &source_rectangle, DDBLT_WAIT, nullptr);
    return SUCCEEDED(hr);
#endif
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Copy_From(Rect &torect, Surface &fromsurface, Rect &fromrect, bool trans_blit, bool a5)
{
    Rect toarea = Get_Rect();
    Rect fromarea = fromsurface.Get_Rect();
    return Copy_From(toarea, torect, fromsurface, fromarea, fromrect, trans_blit, a5);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Copy_From(Surface &fromsurface, bool trans_blit, bool a3)
{
    return XSurface::Copy_From(fromsurface, trans_blit, a3);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Fill_Rect(Rect &rect, unsigned color)
{
    Rect my_rect = Get_Rect();
    return BDSurface::Fill_Rect(my_rect, rect, color);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Fill_Rect(Rect &area, Rect &rect, unsigned color)
{
    if (!rect.Is_Valid()) {
        return false;
    }

    if (!DirectDraw7BackSurface) {
        DEBUG_WARNING("BDSurface::Fill_Rect() - DirectDraw7BackSurface is null!\n");
        return false;
    }

    if (!DSurface::AllowHardwareBlitFills || Is_Locked() || !Can_Blit()) {
        return XSurface::Fill_Rect(area, rect, color);
    }
    return false;

#if 0
    if (!Restore_Check()) {
        return false;
    }
#endif

#if 0
    Rect rect2 = Intersect(area, Get_Rect());

    Rect rect1;
    rect1.X = rect.X + area.X;
    rect1.Y = rect.Y + area.Y;
    rect1.Width = rect.Width;
    rect1.Height = rect.Height;

    Rect rect3 = Intersect(rect1, rect2);

    if (rect3.Width <= 0 || rect3.Height <= 0) {
        return false;
    }

    RECT dest_rectangle;
    dest_rectangle.left = rect3.X;
    dest_rectangle.top = rect3.Y;
    dest_rectangle.right = rect3.Width + rect3.X;
    dest_rectangle.bottom = rect3.Height + rect3.Y;

    DDBLTFX ddBltFx;
    DDRAW_INIT_STRUCT(ddBltFx);
    ddBltFx.dwFillColor = color;

    HRESULT hr = DirectDraw7BackSurface->Blt(&dest_rectangle, nullptr, nullptr, DDBLT_WAIT|DDBLT_COLORFILL, &ddBltFx);
    return SUCCEEDED(hr);
#endif
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Fill_Rect_Trans(Rect &rect, const RGBClass &color, unsigned opacity)
{
    int bpp = Get_Bytes_Per_Pixel();
    if (bpp != 2) {
        return false;
    }

    if (!rect.Is_Valid()) {
        return false;
    }

    Rect r1 = Intersect(Get_Rect(), rect);

    if (!r1.Is_Valid()) {
        return false;
    }

    Point2D r1_tl = r1.Top_Left();
    unsigned short *buffptr = (unsigned short *)Lock(r1_tl.X, r1_tl.Y);
    if (buffptr == nullptr) {
        return false;
    }

    opacity = std::min((int)opacity, 100);

    unsigned scale = (opacity * 255) / 100;
    unsigned short delta = (255 - scale) & 0xFFFF;

    unsigned int red_max = (unsigned int)(255 >> DSurface::RedRight) << DSurface::RedLeft;
    unsigned int green_max = (unsigned int)(255 >> DSurface::GreenRight) << DSurface::GreenLeft;
    unsigned int blue_max = (unsigned int)(255 >> DSurface::BlueRight) << DSurface::BlueLeft;

    unsigned short pixel_color = DSurface::RGBA_To_Pixel(color.Red, color.Green, color.Blue);

    unsigned rscaled = scale * (pixel_color & red_max);
    unsigned gscaled = scale * (pixel_color & green_max);
    unsigned bscaled = scale * (pixel_color & blue_max);

    unsigned short rmax = red_max & 0xFFFF;
    unsigned short gmax = green_max & 0xFFFF;
    unsigned short bmax = blue_max & 0xFFFF;

    for (int y = 0; y < r1.Height; ++y) {

        int line = y * (Get_Pitch() / 2);
        unsigned short *pixel = &buffptr[line];


        for (int x = 0; x < r1.Width; ++x) {

            unsigned short current_color = *pixel;
            *pixel++ = (unsigned short)
                       (((current_color & rmax) * (delta + rscaled) >> 8) & rmax)
                     | (((current_color & gmax) * (delta + gscaled) >> 8) & gmax)
                     | (((current_color & bmax) * (delta + bscaled) >> 8) & bmax);

        }
    }

    Unlock();

    return true;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Draw_Line_entry_34(Rect &area, Point2D &start, Point2D &end, unsigned color, int a5, int a6, bool z_only)
{
    return true;        // TODO
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Draw_Line_entry_38(Rect &area, Point2D &start, Point2D &end, int a4, int a5, int a6, bool a7)
{
    return true;        // TODO
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Draw_Line_entry_3C(Rect &area, Point2D &start, Point2D &end, RGBClass &color, int a5, int a6, bool a7, bool a8, bool a9, bool a10, float a11)
{
    return true;        // TODO
}


/**
 *  
 * 
 *  @author: CCHyper
 */
int BDSurface::entry_48(Point2D &start, Point2D &end, unsigned color, bool pattern[], int offset, bool a6)
{
    return 0;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::entry_4C(Point2D &start, Point2D &end, unsigned color, bool a4)
{
    return true;        // TODO
}


#if 0
/**
 *  
 * 
 *  @author: CCHyper
 */
void *BDSurface::Lock(int x, int y)
{
    if (!Restore_Check()) {
        return nullptr;
    }

    /**
     *  Buffer access sanity check.
     */
    if (x < 0 || y < 0) {
        return nullptr;
    }

    if (!LockLevel) {

        HRESULT ddrval = 0;

        DDSURFACEDESC2 dddesc;
        DDRAW_INIT_STRUCT(dddesc);

        ddrval = DirectDraw7BackSurface->Lock(nullptr, &dddesc, DDLOCK_WAIT, nullptr);
        if (FAILED(ddrval)) {
            //DEBUG_WARNING("BDSurface::Lock() - Failed to lock primary surface!\n");
            DDRAW_ERROR_MSGBOX("BDSurface::Lock()\n\nLock failed with error code %08X\n", ddrval);
            return nullptr;
        }

        CopyMemory(VideoSurfaceDescription, &dddesc, sizeof(DDSURFACEDESC2));

        /**
         *  Update video description flags.
         */
        BytesPerPixel = ((VideoSurfaceDescription->ddpfPixelFormat.dwRGBBitCount + 7) / 8);
        InVideoRam = (VideoSurfaceDescription->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;

        //DEBUG_INFO("BDSurface::Lock() - InVideoRam = %s\n", InVideoRam ? "true" : "false");
        //DEBUG_INFO("BDSurface::Lock() - Width = %d, Height = %d, BPP = %d\n", Width, Height, BytesPerPixel);

        BufferPtr = VideoSurfaceDescription->lpSurface;
    }

    ++LockLevel;

    /**
     *  Return pointer to the direct draw buffer.
     */
    return Get_Buffer_Ptr(x, y);
}
#endif


#if 0
/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Unlock()
{
    HRESULT ddrval = 0;

    Restore_Check();

    if (LockLevel > 0) {

        --LockLevel;

        if (LockLevel == 0) {

            ddrval = DirectDraw7BackSurface->Unlock(nullptr);
            if (FAILED(ddrval)) {
                //DEBUG_WARNING("BDSurface::Unlock() - Failed to unlock primary surface!\n");
                DDRAW_ERROR_MSGBOX("BDSurface::Unlock()\n\nUnlock failed with error code %08X\n", ddrval);
                return false;
            }

        }

        return true;
    }

    return false;
}
#endif


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Can_Lock(int x, int y) const
{
    if (LockLevel) {
        return true;
    }

#if 0
    HRESULT ddrval = 0;

    DDSURFACEDESC2 dddesc;
    DDRAW_INIT_STRUCT(dddesc);

    ddrval = DirectDraw7BackSurface->Lock(nullptr, &dddesc, 0, nullptr);
    if (FAILED(ddrval)) {
        //DEBUG_WARNING("BDSurface::Can_Lock() - Failed to lock primary surface!\n");
        DDRAW_ERROR_MSGBOX("BDSurface::Can_Lock()\n\nLock failed with error code %08X\n", ddrval);
        return false;
    }

    ddrval = DirectDraw7BackSurface->Unlock(nullptr);
    if (FAILED(ddrval)) {
        //DEBUG_WARNING("BDSurface::Can_Lock() - Failed to unlock primary surface!\n");
        DDRAW_ERROR_MSGBOX("BDSurface::Can_Lock()\n\nUnlock failed with error code %08X\n", ddrval);
        return false;
    }
#endif

    return true;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
int BDSurface::Get_Pitch() const
{
    ASSERT(VideoSurfaceDescription != nullptr);
    return VideoSurfaceDescription->lPitch;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Draw_Line_entry_90(Rect &area, Point2D &start, Point2D &end, RGBClass &a4, RGBClass &a5, float& a6, float& a7)
{
    return true;        // TODO
}


bool BDSurface::Can_Blit() const
{
    return DirectDraw7_Can_Blit();
}


bool BDSurface::Can_Flip() const
{
    return DirectDraw7_Can_Flip();
}


/**
 *  
 * 
 *  @author: CCHyper
 */
HDC BDSurface::Get_DC()
{
    if (!DirectDraw7BackSurface) {
        DEBUG_WARNING("BDSurface::Get_DC() - DirectDraw7BackSurface is null!\n");
        return nullptr;
    }

    if (Is_Locked()) {
        return nullptr;
    }

    HDC hdc = nullptr;
    HRESULT ddrval = DirectDraw7BackSurface->GetDC(&hdc);
    if (FAILED(ddrval)) {
        //DEBUG_INFO("BDSurface::Get_DC() - Failed to create DC!\n");
        DDRAW_ERROR_MSGBOX("BDSurface::Get_DC()\n\nLock failed with error code %08X\n", ddrval);
        return nullptr;
    }
    ++LockLevel;
    return hdc;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Release_DC(HDC hdc)
{
    HRESULT ddrval = DirectDraw7BackSurface->ReleaseDC(hdc);
    if (FAILED(ddrval)) {
        //DEBUG_INFO("BDSurface::Release_DC() - Failed to create DC!\n");
        DDRAW_ERROR_MSGBOX("BDSurface::Release_DC()\n\nLock failed with error code %08X\n", ddrval);
        return false;
    }
    if (LockLevel > 0) {
        --LockLevel;
    }
    return true;
}


#if 0
/**
 *  
 * 
 *  @author: CCHyper
 */
DWORD BDSurface::DD_Color_Match(COLORREF dwGDIColor)
{
    HDC hdc = nullptr;
    COLORREF rgb = CLR_INVALID;
    COLORREF color = CLR_INVALID;
    HRESULT ddrval;

    if (dwGDIColor != CLR_INVALID && DirectDraw7BackSurface->GetDC(&hdc) == DD_OK) {
        rgb = GetPixel(hdc, 0, 0); // Save current pixel value.
        SetPixel(hdc, 0, 0, dwGDIColor); // Set our value.
        DirectDraw7BackSurface->ReleaseDC(hdc);
    }

    DDSURFACEDESC2 dddesc;
    DDRAW_INIT_STRUCT(dddesc);

    /**
     *  Now lock the surface so we can read back the converted color.
     */
    while (ddrval = DirectDraw7BackSurface->Lock(nullptr, &dddesc, DDLOCK_SURFACEMEMORYPTR, nullptr) == DDERR_WASSTILLDRAWING);

    if (ddrval == DD_OK) {

        /**
         *  Get DWORD color.
         */
        color = *(COLORREF *)dddesc.lpSurface;

        /*
         *  Skip for 32bit.
         */
        if (dddesc.ddpfPixelFormat.dwRGBBitCount < 32) {
            color &= (1 << dddesc.ddpfPixelFormat.dwRGBBitCount) - 1; // Mask it to bpp.
        }

        DirectDraw7BackSurface->Unlock(nullptr);
    }

    /**
     *  Now put the color that was there back.
     */
    if (dwGDIColor != CLR_INVALID && DirectDraw7BackSurface->GetDC(&hdc) == DD_OK) {
        SetPixel(hdc, 0, 0, rgb);
        DirectDraw7BackSurface->ReleaseDC(hdc);
    }

    return color;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void BDSurface::DD_Set_Color_Key(COLORREF dwGDIColor)
{
    DDCOLORKEY ddcolkey;
    ddcolkey.dwColorSpaceLowValue = 0;
    ddcolkey.dwColorSpaceHighValue = 0;

    COLORREF color = DD_Color_Match(dwGDIColor);
    ddcolkey.dwColorSpaceLowValue = color;
    ddcolkey.dwColorSpaceHighValue = color;

    DirectDraw7BackSurface->SetColorKey(DDCKEY_SRCBLT, &ddcolkey);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void BDSurface::DD_Get_Surface_Size(Rect &rect)
{
    Restore_Check();

    HRESULT ddrval = 0;

    DDSURFACEDESC2 dddesc;
    DDRAW_INIT_STRUCT(dddesc);
    dddesc.dwFlags = DDSD_WIDTH|DDSD_HEIGHT;

    ddrval = DirectDraw7BackSurface->GetSurfaceDesc(&dddesc);
    if (FAILED(ddrval)) {
        DDRAW_ERROR_MSGBOX("BDSurface::DD_Get_Surface_Size()\n\nGetSurfaceDesc failed with error code %08X\n", ddrval);
        return;
    }

    rect.X = 0;
    rect.Y = 0;
    rect.Width = dddesc.dwWidth;
    rect.Height = dddesc.dwHeight;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
HRESULT BDSurface::DD_Copy(LPDIRECTDRAWSURFACE dest, LPDIRECTDRAWSURFACE src, Point2D *dest_point, Rect *src_rect, BOOL is_color_keyed)
{
    if (!dest || !src) {
        return DDERR_SURFACELOST;
    }

    HRESULT ddrval;

    /**
     *  Copy the surface.
     */
    for (;;) {
        ddrval = dest->BltFast(dest_point->X, dest_point->Y, src, (LPRECT)src_rect, is_color_keyed ? DDBLTFAST_SRCCOLORKEY : 0); 
        if (ddrval != DDERR_WASSTILLDRAWING) {  

            /**
             *  #NOTE:
             * 
             *  Surfaces should not restore themselves because they don't know how    
             *  to recreate their graphics. If they fail to draw, just return the result     
             *  and let the caller figure it out.
             */  
            return ddrval;
        }   
    }
    return ddrval;
}
#endif


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Is_Surface_Lost()
{
    return DirectDraw7_Is_Surface_Lost();
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool BDSurface::Restore_Check()
{
    if (!Debug_Windowed && !GameInFocus) {
        return false;
    }

    DWORD ddrval = 0;

    if (DirectDraw7BackSurface != nullptr) {
        
        if (Is_Surface_Lost()) {

            if (DirectDraw7BackSurface->Restore() == DD_FALSE || DirectDraw7BackSurface->IsLost() == DD_FALSE) {
                return false;
            }

            int prev_locklevel = LockLevel;
            if (LockLevel > 0) {
                LockLevel = 0;
                Lock();
                ++LockLevel;
                Unlock();
                LockLevel = prev_locklevel;
            }
        }
    }

    return true;
}

#endif
