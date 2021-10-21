/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DRAWSHAPE_EXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the shape drawers.
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
#include "drawshape_hooks.h"
#include "tibsun_defines.h"
#include "xsurface.h"
#include "convert.h"
#include "drawshape.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


static void CC_Draw_PNG(XSurface *surface, ConvertClass *drawer, const unsigned char *pngfile,
    int shapenum, Point2D *xy, Rect *rect, ShapeFlagsType flags, int a8 = 0, int a9 = 0,
    int a10 = 0, int a11 = 1000, ShapeFileStruct *z_shape = nullptr, int z_framenum = 0, int z_xoff = 0, int z_yoff = 0)
{
}


static bool Is_PNG_File(unsigned char *file)
{
    return (file[1] == 'P' && file[2] == 'N' && file[3] == 'G');
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
static void CC_Draw_Shape_Intercept(XSurface *surface, ConvertClass *drawer, const ShapeFileStruct *shapefile,
    int shapenum, Point2D *xy, Rect *rect, ShapeFlagsType flags, int a8 = 0, int a9 = 0,
    int a10 = 0, int a11 = 1000, ShapeFileStruct *z_shape = nullptr, int z_framenum = 0, int z_xoff = 0, int z_yoff = 0)
{

    if (z_framenum > 0) {
        return;
    }

    if ((flags & SHAPE_NORMAL) == 0) {
        return;
    }

    if (Is_PNG_File((unsigned char *)shapefile)) {
        CC_Draw_PNG(surface, drawer, (unsigned char *)shapefile, shapenum, xy, rect, flags, a8, a9, a10, a11, z_shape, z_framenum, z_xoff, z_yoff);
    } else {
        CC_Draw_Shape(surface, drawer, shapefile, shapenum, xy, rect, flags, a8, a9, a10, a11, z_shape, z_framenum, z_xoff, z_yoff);
    }
}




/**
 *  Main function for patching the hooks.
 */
void DrawShapeExtension_Hooks()
{
    Patch_Call(0x004149B4, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00414AB2, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00414BA9, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00414C48, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00428920, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00428A0A, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00428B0D, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00445E8C, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00445EEF, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00454E48, &CC_Draw_Shape_Intercept);
    Patch_Call(0x004555CF, &CC_Draw_Shape_Intercept);
    Patch_Call(0x004557AB, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00455B21, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00484DC2, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00485D05, &CC_Draw_Shape_Intercept);
    Patch_Call(0x004861FF, &CC_Draw_Shape_Intercept);
    Patch_Call(0x004863FC, &CC_Draw_Shape_Intercept);
    Patch_Call(0x004865E9, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0049EB24, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0049EE60, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0049EEDD, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0049EF2A, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0049F0E9, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0049F21C, &CC_Draw_Shape_Intercept);
    Patch_Call(0x004D2EAC, &CC_Draw_Shape_Intercept);
    Patch_Call(0x004D319A, &CC_Draw_Shape_Intercept);
    Patch_Call(0x004EC8C6, &CC_Draw_Shape_Intercept);
    Patch_Call(0x004F5C9F, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0056B091, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0056B38D, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0056B6A4, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0056B9BF, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0056BBCD, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0056BC72, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0056BE3A, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0056BEC4, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00572622, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00572772, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0058C83D, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0058D38C, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005A46E2, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005AB545, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005AB5A5, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005AB5F9, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005AB655, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005AB6B1, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005ADEB1, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005B8E57, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005B8F14, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005B8F97, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005B9639, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005BC8C4, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005BCCD6, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E375C, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E39B7, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E3C77, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E3EC3, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E3F1C, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E448A, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E44E7, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E6C70, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E6D8E, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E7045, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005E7094, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005F1743, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005F367A, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005F371F, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005F3777, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005F37BF, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005F52EE, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005F533E, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005F5527, &CC_Draw_Shape_Intercept);
    Patch_Call(0x005FB5A5, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0060E4E8, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0060E562, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0060E6DC, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0060E758, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0060E91D, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00612510, &CC_Draw_Shape_Intercept);
    Patch_Call(0x006127AC, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00612AB0, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0061718B, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0062BE85, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0062C556, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0062C5D0, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0062C6B7, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0062C947, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0062C9F7, &CC_Draw_Shape_Intercept);
    Patch_Call(0x006352ED, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0063538D, &CC_Draw_Shape_Intercept);
    Patch_Call(0x006354A8, &CC_Draw_Shape_Intercept);
    Patch_Call(0x006376A4, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00637880, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0063796A, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00637A58, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00637B36, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00637BCD, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00637CAC, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0063FBA2, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0063FBEB, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0063FD25, &CC_Draw_Shape_Intercept);
    Patch_Call(0x0063FD6B, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00653282, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00653D6A, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00653E96, &CC_Draw_Shape_Intercept);
    Patch_Call(0x00661B64, &CC_Draw_Shape_Intercept);
}
