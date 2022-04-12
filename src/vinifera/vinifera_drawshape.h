/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_DRAWSHAPE.H
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
#pragma once

#include "always.h"
#include "wstring.h"
#include "tibsun_defines.h"


class Rect;
class Point2D;
class XSurface;


bool Vinifera_Is_PNG_File(unsigned char *filedata);

bool Vinifera_Draw_Image(XSurface *surface, Wstring &filename, Point2D *xy, Rect *rect, ShapeFlagsType flags = SHAPE_NORMAL, int framenum = 0);
bool Vinifera_Draw_PNG(XSurface *surface, Wstring &filename, Point2D *xy, Rect *rect, ShapeFlagsType flags = SHAPE_NORMAL);
bool Vinifera_Draw_APNG(XSurface *surface, Wstring &filename, int framenum, Point2D *xy, Rect *rect, ShapeFlagsType flags = SHAPE_NORMAL);
