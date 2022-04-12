/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DDRAW7_HOOKS.CPP
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
#include "d7surface_hooks.h"
#include "d7surface.h"
#include "dsurface.h"
#include "rawfile.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"



#if 0


static int DDraw7_BitsPerPixel = 16;


/**
 *  These patches make sure Set_Video_Mode is called with 32 instead of 16 for the bit depth.
 */
DECLARE_PATCH(_WinMain_16Bit_One)
{
    _asm { mov eax, DDraw7_BitsPerPixel }
    JMP_REG(ecx, 0x0060160B);
}

DECLARE_PATCH(_WinMain_16Bit_Two)
{
    _asm { mov edx, DDraw7_BitsPerPixel }
    JMP_REG(ecx, 0x006015D9);
}

DECLARE_PATCH(_WinMain_16Bit_Three)
{
    _asm { mov edx, DDraw7_BitsPerPixel }
    JMP_REG(eax, 0x00601703);
}


void DDraw7_Hooks()
{
    /**
     *  Removes community ddraw.dll replacements as they conflict with the system.
     */
    if (RawFileClass("DDRAW.DLL").Is_Available()) {
        MessageBox(nullptr, "Renaming DDRAW.DLL to DDRAW.BAK due to conflict with the DirectDraw7 renderer!", "Vinifera", MB_OK|MB_ICONASTERISK);
        std::rename("DDRAW.DLL", "DDRAW.BAK");
    }

    /**
     *  Fetch the primary monitor bit depth.
     */
    HDC dc = GetDC(nullptr);
    DDraw7_BitsPerPixel = GetDeviceCaps(dc, BITSPIXEL);
    ReleaseDC(nullptr, dc);

    /**
     *  Param for Set_Video_Mode, Changes 16 to 32.
     */
    Patch_Byte(0x0050AD2F+1, DDraw7_BitsPerPixel);
    Patch_Jump(0x006015FC, &_WinMain_16Bit_One);
    Patch_Jump(0x006015C9, &_WinMain_16Bit_Two);
    Patch_Jump(0x006016F3, &_WinMain_16Bit_Three);

    /**
     *  Removes fatal primary surface check for 16bit.
     */
    Patch_Jump(0x00601428, 0x00601467);
}


#endif
