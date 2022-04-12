/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DDRAW7_UTIL.H
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

#undef DIRECTDRAW_VERSION
#define DIRECTDRAW_VERSION 0x0700

#include <Windows.h>
#include <ddraw.h>


class CCINIClass;
class Rect;
class XSurface;


char const *Get_DD_Error_Description(HRESULT ddres);
void DD_Error_Message_Box(HRESULT ddres, char const *fmt, ...);


/**
 *  Some handy macros.
 */
#define DDRAW_INIT_STRUCT(dxstruct)          ZeroMemory(&dxstruct, sizeof(dxstruct)); dxstruct.dwSize = sizeof(dxstruct);
#define DDRAW_INIT_STRUCT_PTR(dxstruct)      ZeroMemory(dxstruct, sizeof(*dxstruct)); dxstruct->dwSize = sizeof(*dxstruct);
#define DDRAW_ALLOC_STRUCT_PTR(dxstruct)     //#define ALLOC_STRUCT( StructName ) ((StructName *)malloc( sizeof( StructName )))

#define DDRAW_ERROR_LOG(str, val)            if (val != DD_OK) { DEBUG_INFO(str, val); }

#define DDRAW_ERROR_MSGBOX(str, val, ...) \
            if (val != DD_OK) { \
                DD_Error_Message_Box(val, str, ##__VA_ARGS__); \
            }

#define COLOR_CLIP(x) (x < 0 ? 0 : (x > 255 ? 255 : x))

#define TO_RGB555(r, g, b) ((((r) & 0xF8) << 7) | (((g) & 0xF8) << 2) | (((b) >> 3) & 0x1F))
#define TO_RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) >> 3) & 0x1F))
#define TO_RGB655(r, g, b) ((((r) & 0xFC) << 8) | (((g) & 0xF8) << 2) | (((b) >> 3) & 0x1F))
#define TO_RGB556(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xF8) << 3) | (((b) >> 2) & 0x2F))

#define GET_R(rgb) (((rgb) >> 16) & 0xFF)
#define GET_G(rgb) (((rgb) >> 8) & 0xFF)
#define GET_B(rgb) ((rgb) & 0xFF)

// pack 3 [0,255] ints into one 32 bit one.
#define PACK_RGB(r, g, b) (((r) << 16) | ((g) << 8) | (b))


/**
 *  16bit pixel rgb format modes.
 */
typedef enum DirectDraw7_RGBMode
{
    RGBINVALID = -1,
    RGB555 = 0, // Uses five bits each for the red, green, and blue components in a pixel.
    RGB556 = 1, // Uses five bits each for the red and green components, and 6 for the blue component.
    RGB565 = 2, // Uses five bits for the red and blue components, and six bits for the green component.
    RGB655 = 3,
} DirectDraw7_RGBMode;

/**
 *  Flags returned by Get_Video_Hardware_Capabilities
 */
typedef enum DirectDraw7_CapabilityFlags
{
    VIDEO_NONE = 0,
    VIDEO_BLITTER = 1 << 0,             // Hardware blits supported?
    VIDEO_BLITTER_ASYNC = 1 << 1,       // Hardware blits asyncronous?
    VIDEO_SYNC_PALETTE = 1 << 2,        // Can palette changes be synced to vertical refresh?
    VIDEO_BANK_SWITCHED = 1 << 3,       // Is the video cards memory bank switched?
    VIDEO_COLOR_FILL = 1 << 4,          // Can the blitter do filled rectangles?
    VIDEO_NO_HARDWARE_ASSIST = 1 << 5,  // Is there no hardware assistance available at all?
    VIDEO_ALIGN_BOUNDARY = 1 << 6
} DirectDraw7_CapabilityFlags;
DEFINE_ENUM_FLAG_OPERATORS(DirectDraw7_CapabilityFlags);
