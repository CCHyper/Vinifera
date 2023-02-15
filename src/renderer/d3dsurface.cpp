/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          D3DSURFACE.CPP
 *
 *  @author        CCHyper
 *
 *  @contributors  tomsons26
 *
 *  @brief         Direct3D surface class.
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
#include "d3dsurface.h"
#include "tibsun_functions.h"
#include "rgb.h"
#include "debughandler.h"
#include "asserthandler.h"
#include "tibsun_globals.h"


extern LPDIRECT3DDEVICE9 Direct3DDevice;


D3DSurface::D3DSurface(int width, int height, D3DFORMAT format) :
    XSurface(width, height),
    VideoSurfacePtr(nullptr)
{
    // NOTE: "format" is ignored for now.

    BytesPerPixel = 2;
    InVideoRam = true;

    HRESULT res;

    // Create new surface.
    res = Direct3DDevice->CreateOffscreenPlainSurface(
        width,
        height,
        D3DFMT_R5G6B5,
        D3DPOOL_DEFAULT,
        &VideoSurfacePtr,
        nullptr);

    if (FAILED(res)) {
        DEBUG_ERROR("D3DSurface - Failed to create surface! Error code %x!", res);
    }
}

D3DSurface::~D3DSurface()
{
    if (VideoSurfacePtr) {
        VideoSurfacePtr->Release();
        VideoSurfacePtr = nullptr;
    }
}

void *D3DSurface::Lock(int x, int y)
{
#if 0
    /**
     *  Buffer access sanity check.
     */
    if (x < 0 || y < 0) {
        return nullptr;
    }

    D3DLOCKED_RECT lock_rect;

    lock_rect.Pitch = 0;
    lock_rect.pBits = nullptr;

    if (!LockLevel) {

        DWORD flags = 0 | D3DLOCK_NOSYSLOCK;
        HRESULT res = VideoSurfacePtr->LockRect(&lock_rect, nullptr, flags);

        if (FAILED(res)) {
            DEBUG_ERROR("D3DSurface - Failed to lock surface! Error code %x!\n", res);
            return nullptr;
        }
        BufferPtr = lock_rect.pBits;
    }

    

    ++LockLevel;

    return (void*)(x * Get_Bytes_Per_Pixel() + y * Get_Pitch() + (char *)BufferPtr);
#endif
    {

        //if (!DSurface_Restore_(this) || x < 0 || y < 0) {
        //    return 0;
        //}

        if (x < 0 || y < 0) {
            return 0;
        }

        if (!LockLevel)
        {
            D3DLOCKED_RECT lock_rect;

            lock_rect.Pitch = 0;
            lock_rect.pBits = nullptr;

            DWORD flags = 0 | D3DLOCK_NOSYSLOCK;
            HRESULT res = VideoSurfacePtr->LockRect(&lock_rect, nullptr, flags);

            if (FAILED(res)) {
                DEBUG_ERROR("D3DSurface - Failed to lock surface! Error code %x!\n", res);
                return nullptr;
            }
            BufferPtr = lock_rect.pBits;
        }
        ++LockLevel;
        return (void*)(x * Get_Bytes_Per_Pixel() + y * Get_Pitch() + (int)BufferPtr);
    }
}


bool D3DSurface::Unlock()
{
    //DSurface_Restore_(this);
    if (LockLevel <= 0) {
        return false;
    }
    LockLevel--;

    if (LockLevel) {
        return true;
    }
    VideoSurfacePtr->UnlockRect();
    BufferPtr = nullptr;
    return true;
}


bool D3DSurface::Can_Lock(int x, int y) const
{
    if (LockLevel) {
        return true;
    }

    HRESULT res;

    D3DLOCKED_RECT lock_rect;

    lock_rect.Pitch = 0;
    lock_rect.pBits = nullptr;

    res = VideoSurfacePtr->LockRect(&lock_rect, nullptr, D3DLOCK_NOSYSLOCK);
    if (FAILED(res)) {
        DEBUG_ERROR("D3DSurface - Failed to lock back surface! Error code %x!\n", res);
        return false;
    }
        
    res = VideoSurfacePtr->UnlockRect();
    if (FAILED(res)) {
        DEBUG_ERROR("D3DSurface - Failed to unlock back surface! Error code %x!\n", res);
        return false;
    }

    return true;
}

extern LPDIRECT3D9 Direct3D;
extern LPDIRECT3DSURFACE9 D3DBackSurface;

bool D3DSurface::Copy_From(Rect &toarea, Rect &torect, Surface &fromsurface, Rect &fromarea, Rect &fromrect, bool trans_blit, bool a7)
{
    if (!toarea.Is_Valid() || !fromarea.Is_Valid()) {
        return false;
    }

    if (!torect.Is_Valid() || !fromrect.Is_Valid()) {
        return false;
    }

    if (!VideoSurfacePtr) {
        DEBUG_WARNING("DSurface::Fill_Rect() - VideoSurfacePtr is null!\n");
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

    //if (!Restore_Check()) {
    //    return false;
    //}

    if (use_xsurface) {
        return XSurface::Copy_From(torect, fromsurface, fromrect, trans_blit, true);
    }

    Rect rect4 = Intersect(fromarea, fromsurface.Get_Rect());
    Rect rect2 = Intersect(toarea, Get_Rect());

    if (!func_6A83E0(torect, rect2, fromrect, rect4)) {
        return false;
    }

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

    XSurface::Copy_From(torect, fromsurface, fromrect, trans_blit, true);

    //if (fromsurface.entry_80()) {
    //    Direct3DDevice->StretchRect(VideoSurfacePtr, &source_rectangle, static_cast<D3DSurface &>(fromsurface).VideoSurfacePtr, &dest_rectangle, D3DTEXF_POINT);
    //    Direct3DDevice->Present(NULL, NULL, NULL, NULL);
    //}

    return true;
}


bool D3DSurface::Copy_From(Rect &torect, Surface &fromsurface, Rect &fromrect, bool trans_blit, bool a5)
{
    Rect toarea = Get_Rect();
    Rect fromarea = fromsurface.Get_Rect();
    return Copy_From(toarea, torect, fromsurface, fromarea, fromrect, trans_blit, a5);
}


bool D3DSurface::Copy_From(Surface &src, bool trans_blit, bool a3)
{
    return XSurface::Copy_From(src, trans_blit, a3);
}


bool D3DSurface::Fill_Rect(Rect &size, unsigned color)
{
    Rect rect = Get_Rect();
    return D3DSurface::Fill_Rect(rect, size, color);
}


bool D3DSurface::Fill_Rect(Rect &area, Rect &rect, unsigned color)
{
    if (!rect.Is_Valid()) {
        return false;
    }

    if (!VideoSurfacePtr) {
        DEBUG_WARNING("D3DSurface::Fill_Rect() - VideoSurfacePtr is null!\n");
        return false;
    }

#if 0
    if (!AllowHardwareBlitFills || Is_Locked() || !Can_Blit()) {
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
#endif
    return true;
}


/**
 *  x
 */
bool D3DSurface::Fill_Rect_Trans(Rect &rect, const RGBClass &rgb, unsigned opacity)
{
#if 0
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

    unsigned int red_max = (unsigned int)(255 >> RedRight) << RedLeft;
    unsigned int green_max = (unsigned int)(255 >> GreenRight) << GreenLeft;
    unsigned int blue_max = (unsigned int)(255 >> BlueRight) << BlueLeft;

    unsigned short color = RGBA_To_Pixel(rgb.Red_Component(), rgb.Green_Component(), rgb.Blue_Component());

    unsigned rscaled = scale * (color & red_max);
    unsigned gscaled = scale * (color & green_max);
    unsigned bscaled = scale * (color & blue_max);

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
#endif
    return true;
}

bool D3DSurface::Draw_Line_entry_34(Rect &area, Point2D &start, Point2D &end, unsigned color, int a5, int a6, bool z_only)
{
    return false;
}

bool D3DSurface::Draw_Line_entry_38(Rect &area, Point2D &start, Point2D &end, int a4, int a5, int a6, bool a7)
{
    return false;
}

bool D3DSurface::Draw_Line_entry_3C(Rect &area, Point2D &start, Point2D &end, RGBClass &color, int a5, int a6, bool a7, bool a8, bool a9, bool a10, float a11)
{
    return false;
}

bool D3DSurface::Draw_Ellipse(Point2D point, int radius_x, int radius_y, Rect clip, unsigned color)
{
    return false;
}

bool D3DSurface::Put_Pixel(Point2D &point, unsigned color)
{
    return false;
}


int D3DSurface::Get_Pitch() const
{
    //D3DSURFACE_DESC d3ddsc;
    //VideoSurfacePtr->GetDesc(&d3ddsc);
    //return d3ddsc.;

    return 2;
}

bool D3DSurface::Draw_Line_entry_90(Rect &area, Point2D &start, Point2D &end, RGBClass &a4, RGBClass &a5, float &a6, float &a7)
{
    return false;
}


/**
 *  Creates a GDI-compatible handle of a device context for the direct draw surface.
 * 
 *  @warning    Make sure to call Release_DC when finished!
 */

PAINTSTRUCT ps;

HDC D3DSurface::Get_DC()
{
    //if (!VideoSurfacePtr) {
    //    DEBUG_WARNING("D3DSurface::Get_DC() - VideoSurfacePtr is null!\n");
    //    return nullptr;
    //}
    //
    //if (Is_Locked()) {
    //    return nullptr;
    //}
    //
    //HDC hdc = nullptr;
    //HRESULT ddrval = VideoSurfacePtr->GetDC(&hdc);
    //if (FAILED(ddrval)) {
    //    //DEBUG_INFO("D3DSurface::Get_DC() - Failed to create DC!\n");
    //    //DDRAW_ERROR_MSGBOX("D3DSurface::Get_DC()\n\nLock failed with error code %08X\n", ddrval);
    //    return nullptr;
    //}
    //++LockLevel;
    HDC hdc = BeginPaint(MainWindow, &ps);
    return hdc;
}


bool D3DSurface::Release_DC(HDC hdc)
{
   //HRESULT ddrval = VideoSurfacePtr->ReleaseDC(hdc);
   //if (FAILED(ddrval)) {
   //    //DEBUG_INFO("D3DSurface::Release_DC() - Failed to create DC!\n");
   //    //DDRAW_ERROR_MSGBOX("D3DSurface::Release_DC()\n\nLock failed with error code %08X\n", ddrval);
   //    return false;
   //}
   //if (LockLevel > 0) {
   //    --LockLevel;
   //}
    EndPaint(MainWindow, &ps);
    return true;
}
