/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VIRTUALSURFACE.CPP
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
#include "virtualsurface.h"
#include "tibsun_globals.h"
#include "dsurface.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Calculate bit shifts to properly extract channel data.
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


VirtualSurface::VirtualSurface() :
    BSurface()
{
    DEBUG_INFO("VirtualSurface created.\n");
}


VirtualSurface::VirtualSurface(int width, int height, bool system_mem) :
    BSurface(width, height, 2) // Forces 16bit.
{
    DEBUG_INFO("VirtualSurface created: %d x %d x %d.\n", width, height, (Get_Bytes_Per_Pixel() * 8));
}


VirtualSurface::VirtualSurface(LPDIRECTDRAWSURFACE surface) :
    BSurface()
{
    if (surface != nullptr) {

        DDSURFACEDESC ddsdesc;
        ZeroMemory(&ddsdesc, sizeof(DDSURFACEDESC));

        surface->GetSurfaceDesc(&ddsdesc);

        int width = ddsdesc.dwWidth;
        int height = ddsdesc.dwHeight;

        new (this) VirtualSurface(width, height);

        DEBUG_INFO("VirtualSurface created: %d x %d x %d.\n", width, height, (Get_Bytes_Per_Pixel() * 8));
    }
}


VirtualSurface::~VirtualSurface()
{
    DEBUG_INFO("VirtualSurface destroyed.\n");
}


bool VirtualSurface::Copy_From(Rect &toarea, Rect &torect, Surface &fromsurface, Rect &fromarea, Rect &fromrect, bool trans_blit, bool a7)
{
    return BSurface::Copy_From(toarea, torect, fromsurface, fromarea, fromrect, trans_blit, a7);
}


bool VirtualSurface::Copy_From(Rect &torect, Surface &fromsurface, Rect &fromrect, bool trans_blit, bool a5)
{
    return BSurface::Copy_From(torect, fromsurface, fromrect, trans_blit, a5);
}


bool VirtualSurface::Copy_From(Surface &fromsurface, bool trans_blit, bool a3)
{
    return BSurface::Copy_From(fromsurface, trans_blit, a3);
}


bool VirtualSurface::Fill_Rect(Rect &rect, unsigned color)
{
    return BSurface::Fill_Rect(rect, color);
}


bool VirtualSurface::Fill_Rect(Rect &area, Rect &rect, unsigned color)
{
    return BSurface::Fill_Rect(area, rect, color);
}


bool VirtualSurface::Fill_Rect_Trans(Rect &rect, const RGBClass &color, unsigned opacity)
{
    return BSurface::Fill_Rect_Trans(rect, color, opacity);
}


bool VirtualSurface::Draw_Line_entry_34(Rect &area, Point2D &start, Point2D &end, unsigned color, int a5, int a6, bool z_only)
{
    return BSurface::Draw_Line_entry_34(area, start, end, color, a5, a6, z_only);
}


bool VirtualSurface::Draw_Line_entry_38(Rect &area, Point2D &start, Point2D &end, int a4, int a5, int a6, bool a7)
{
    return BSurface::Draw_Line_entry_38(area, start, end, a4, a5, a6, a7);
}


bool VirtualSurface::Draw_Line_entry_3C(Rect &area, Point2D &start, Point2D &end, RGBClass &color, int a5, int a6, bool a7, bool a8, bool a9, bool a10, float a11)
{
    return BSurface::Draw_Line_entry_3C(area, start, end, color, a5, a6, a7, a8, a9, a10, a11);
}


int VirtualSurface::entry_48(Point2D &start, Point2D &end, unsigned color, bool pattern[], int offset, bool a6)
{
    return BSurface::entry_48(start, end, color, pattern, offset, a6);
}


bool VirtualSurface::entry_4C(Point2D &start, Point2D &end, unsigned color, bool a4)
{
    return BSurface::entry_4C(start, end, color, a4);
}


void* VirtualSurface::Lock(int x, int y)
{
    return BSurface::Lock(x, y);
}


bool VirtualSurface::Unlock()
{
    return BSurface::Unlock();
}


bool VirtualSurface::Can_Lock(int x, int y) const
{
    return BSurface::Can_Lock(x, y);
}


int VirtualSurface::Get_Bytes_Per_Pixel() const
{
    return BSurface::Get_Bytes_Per_Pixel();
}


int VirtualSurface::Get_Pitch() const
{
    return BSurface::Get_Pitch();
}


bool VirtualSurface::entry_80() const
{
    return true;
}


bool VirtualSurface::Draw_Line_entry_90(Rect &area, Point2D &start, Point2D &end, RGBClass &a4, RGBClass &a5, float& a6, float& a7)
{
    return true; // TODO, requires implementation.
}


bool VirtualSurface::Can_Blit() const
{
    return true;
}


HDC VirtualSurface::Get_DC()
{
    DEBUG_INFO("VirtualSurface::Release_DC().\n");

    if (Is_Locked()) {
        return nullptr;
    }

    HDC hdc = GetDC(MainWindow);
    if (!hdc) {
        DEBUG_WARNING("VirtualSurface::Get_DC() - Failed to obtain DC!\n");
        return nullptr;
    }

    ++LockLevel;

    return hdc;
}


BOOL VirtualSurface::Release_DC(HDC hdc)
{
    DEBUG_INFO("VirtualSurface::Release_DC().\n");

    HRESULT ddrval = ReleaseDC(MainWindow, hdc);
    if (FAILED(ddrval)) {
        DEBUG_WARNING("VirtualSurface::Get_DC() - Failed to release DC!\n");
        return false;
    }

    if (LockLevel > 0) {
        --LockLevel;
    }

    return true;
}


bool VirtualSurface::Restore_Check()
{
    if (!Debug_Windowed && !GameInFocus) {
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

    return true;
}
