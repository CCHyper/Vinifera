/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_DRAWSHAPE.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the PNG drawers.
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
#include "vinifera_drawshape.h"
#include "ccfile.h"
#include "filepng.h"
#include "lodepng.h"
#include "bsurface.h"
#include "dsurface.h"
#include "drawbuff.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Vinifera_Draw_Image(XSurface *surface, Wstring &filename, Point2D *xy, Rect *rect, ShapeFlagsType flags, int framenum)
{
    ASSERT(surface != nullptr);
    ASSERT(filename.Is_Not_Empty());
    ASSERT(xy != nullptr);
    ASSERT(rect != nullptr);

    bool ok = false;

    if (framenum > 0) {
        ok = Vinifera_Draw_APNG(surface, filename, framenum, xy, rect, flags);
    } else {
        ok = Vinifera_Draw_PNG(surface, filename, xy, rect, flags);
    }

    return ok;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Vinifera_Draw_PNG(XSurface *surface, Wstring &filename, Point2D *xy, Rect *rect, ShapeFlagsType flags)
{
    ASSERT(surface != nullptr);
    ASSERT(filename.Is_Not_Empty());
    ASSERT(xy != nullptr);
    ASSERT(rect != nullptr);

    CCFileClass file(filename.Peek_Buffer());
    BSurface *png_surface = Read_PNG_File(&file);
    if (!png_surface) {
        return false;
    }

    Rect png_rect = png_surface->Get_Rect();
    Rect intersect_rect = Intersect(*rect, png_rect);
    if (!intersect_rect.Is_Valid()) {
        return false;
    }

    Buffer_To_Buffer(surface, intersect_rect, png_surface);

    delete png_surface;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Vinifera_Draw_APNG(XSurface *surface, Wstring &filename, int framenum, Point2D *xy, Rect *rect, ShapeFlagsType flags)
{
    ASSERT(surface != nullptr);
    ASSERT(filename.Is_Not_Empty());
    ASSERT(xy != nullptr);
    ASSERT(rect != nullptr);

    CCFileClass file(filename.Peek_Buffer());
    BSurface *png_surface = Read_APNG_File(&file, framenum);
    if (!png_surface) {
        return false;
    }

    Rect png_rect = png_surface->Get_Rect();
    Rect intersect_rect = Intersect(*rect, png_rect);
    if (!intersect_rect.Is_Valid()) {
        return false;
    }

    Buffer_To_Buffer(surface, intersect_rect, png_surface);

    delete png_surface;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Vinifera_Is_PNG_File(unsigned char *filedata)
{
    return (filedata[1] == 'P' && filedata[2] == 'N' && filedata[3] == 'G');
}
