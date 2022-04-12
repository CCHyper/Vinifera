/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VIRTUALSURFACE.H
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
#include "bsurface.h"
#include <ddraw.h>


/**
 *  A buffered clone of DSurface that implements the minimum required, without
 *  using DirectDraw. It has the exact same interface to ensure the game calls
 *  expected functions without crashing.
 */
class VirtualSurface : public BSurface
{
    public:
        VirtualSurface();
        VirtualSurface(int width, int height, bool system_mem = false);
        VirtualSurface(LPDIRECTDRAWSURFACE surface);
        virtual ~VirtualSurface();

        /**
         *  Surface
         */
        virtual bool Copy_From(Rect &toarea, Rect &torect, Surface &fromsurface, Rect &fromarea, Rect &fromrect, bool trans_blit = false, bool a7 = true) override;
        virtual bool Copy_From(Rect &torect, Surface &fromsurface, Rect &fromrect, bool trans_blit = false, bool a5 = true) override;
        virtual bool Copy_From(Surface &fromsurface, bool trans_blit = false, bool a3 = true) override;
        virtual bool Fill_Rect(Rect &rect, unsigned color) override;
        virtual bool Fill_Rect(Rect &area, Rect &rect, unsigned color) override;
        virtual bool Fill_Rect_Trans(Rect &rect, const RGBClass &color, unsigned opacity) override;
        virtual bool Draw_Line_entry_34(Rect &area, Point2D &start, Point2D &end, unsigned color, int a5, int a6, bool z_only = false) override;
        virtual bool Draw_Line_entry_38(Rect &area, Point2D &start, Point2D &end, int a4, int a5, int a6, bool a7 = false) override;
        virtual bool Draw_Line_entry_3C(Rect &area, Point2D &start, Point2D &end, RGBClass &color, int a5, int a6, bool a7, bool a8, bool a9, bool a10, float a11) override;
        virtual int entry_48(Point2D &start, Point2D &end, unsigned color, bool pattern[], int offset, bool a6) override;
        virtual bool entry_4C(Point2D &start, Point2D &end, unsigned color, bool a4 = false) override;
        virtual void *Lock(int x = 0, int y = 0) override;
        virtual bool Unlock() override;
        virtual bool Can_Lock(int x = 0, int y = 0) const override;
        virtual int Get_Bytes_Per_Pixel() const override;
        virtual int Get_Pitch() const override;
        virtual bool entry_80() const override;

        /**
         *  DSurface
         */
        virtual bool Draw_Line_entry_90(Rect &area, Point2D &start, Point2D &end, RGBClass &a4, RGBClass &a5, float &a6, float &a7);
        virtual bool Can_Blit() const;
        
        HDC Get_DC();
        BOOL Release_DC(HDC hdc);
        bool Restore_Check();

    public:
};
