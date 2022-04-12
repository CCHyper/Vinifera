/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SDLSURFACE.H
 *
 *  @author        CCHyper
 *
 *  @brief         SDL2 Surface
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


#if 0

#include "always.h"
#include "xsurface.h"
#include "sdl_globals.h"


class SDLSurface : public XSurface
{
    public:
        SDLSurface();
        SDLSurface(int width, int height);
        SDLSurface(SDL_Surface *surface);
        virtual ~SDLSurface();

        virtual bool Copy_From(Rect &toarea, Rect &torect, Surface &fromsurface, Rect &fromarea, Rect &fromrect, bool trans_blit = false, bool a7 = true) override;
        virtual bool Copy_From(Rect &torect, Surface &fromsurface, Rect &fromrect, bool trans_blit = false, bool a5 = true) override;
        virtual bool Copy_From(Surface &fromsurface, bool trans_blit = false, bool a3 = true) override;
        virtual bool Fill_Rect(Rect &rect, unsigned color) override;
        virtual bool Fill_Rect(Rect &area, Rect &rect, unsigned color) override;
        virtual bool Fill(unsigned color) override;
        virtual bool Fill_Rect_Trans(Rect &rect, const RGBClass &color, unsigned opacity) override;
        virtual bool Draw_Ellipse(Point2D center, int radius_x, int radius_y, Rect clip, unsigned color) override;
        virtual bool Put_Pixel(Point2D &point, unsigned color) override;
        virtual unsigned Get_Pixel(Point2D &point) override;
        virtual bool Draw_Line(Point2D &start, Point2D &end, unsigned color) override;
        virtual bool Draw_Line(Rect &area, Point2D &start, Point2D &end, unsigned color) override;
        virtual bool Draw_Line_entry_34(Rect &area, Point2D &start, Point2D &end, unsigned color, int a5, int a6, bool z_only = false) override;
        virtual bool Draw_Line_entry_38(Rect &area, Point2D &start, Point2D &end, int a4, int a5, int a6, bool a7 = false) override;
        virtual bool Draw_Line_entry_3C(Rect &area, Point2D &start, Point2D &end, RGBClass &color, int a5, int a6, bool a7, bool a8, bool a9, bool a10, float a11) override;
        virtual bool entry_40(Rect &area, Point2D &start, Point2D &end, void(*drawer_callback)(Point2D &)) override;
        virtual int Draw_Dashed_Line(Point2D &start, Point2D &end, unsigned color, bool pattern[], int offset) override;
        virtual int entry_48(Point2D &start, Point2D &end, unsigned color, bool pattern[], int offset, bool a6) override;
        virtual bool entry_4C(Point2D &start, Point2D &end, unsigned color, bool a4 = false) override;
        virtual bool Draw_Rect(Rect &rect, unsigned color) override;
        virtual bool Draw_Rect(Rect &area, Rect &rect, unsigned color) override;
        virtual void *Lock(int x = 0, int y = 0) override;
        virtual bool Unlock() override;
        virtual bool Can_Lock(int x = 0, int y = 0) const override;
        virtual bool entry_64(int x = 0, int y = 0) const override;
        virtual bool Is_Locked() const override;
        virtual int Get_Bytes_Per_Pixel() const override;
        virtual int Get_Pitch() const override;
        virtual Rect Get_Rect() const override;
        virtual int Get_Width() const override;
        virtual int Get_Height() const override;
        virtual bool entry_80() const override;
        virtual bool Draw_Line_entry_90(Rect &area, Point2D &start, Point2D &end, RGBClass &a4, RGBClass &a5, float &a6, float &a7);
        virtual bool Can_Blit() const;

        void *Get_Buffer_Ptr(int x = 0, int y = 0) const;

        HDC Get_DC();
        BOOL Release_DC(HDC hdc);
        bool Restore_Check();

        static SDLSurface *Create_Primary(SDLSurface **backbuffer_surface = nullptr);

    public:
        bool IsAllocated;

        /**
         *  Render surface.
         */
        SDL_Surface *VideoSurface;

    private:
        SDLSurface(const SDLSurface & s);
        SDLSurface & operator = (const SDLSurface & s);
};

#endif

