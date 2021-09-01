/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OWNRDRAWEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended OwnerDraw ui class.
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
#include "ownrdrawext_hooks.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <Windows.h>

#include "hooker.h"
#include "hooker_macros.h"


char ButtonFont[256] = { "MS Sans Serif" };
char ListFont[256] = { "MS Sans Serif" };

bool TransitionSound = true;
bool InnerGlow = true;
bool Transition = true;

//https://android.googlesource.com/device/generic/goldfish/+/idea133/camera/Converters.h

// Build RGB565 word from red, green, and blue bytes.
#define RGB565(r, g, b) static_cast<uint16_t>((((static_cast<uint16_t>(b) << 6) | g) << 5) | r)

// Build RGB32 dword from red, green, and blue bytes.
#define RGB32(r, g, b) static_cast<uint32_t>((((static_cast<uint32_t>(b) << 8) | g) << 8) | r)


/**
 *  Main function for patching the hooks.
 */
void OwnerDrawExtension_Hooks()
{
    /**
     *  Removes initialisation of the color globals each time a dialog is created.
     */
    Patch_Jump(0x0059134D, 0x005913AD);

    /**
     *  Removes CreateSolidBrush one-time for 0x00808B78.
     */
    Patch_Jump(0x0058F6EB, 0x0058F70A);

    /**
     *  Set the one time defaults.
     */
    Patch_Dword(0x00809218, 1);
    Patch_Dword(0x00809250, 40);
    Patch_Dword(0x00808B6C, 127);
    Patch_Dword(0x00808B7C, RGB(15,247,0)/*0FF70*/); // TextColor
    Patch_Dword(0x00808B68, /*RGB32(255,0,0)*/0x109010); // some darkish green
    Patch_Dword(0x00809248, /*RGB32(255,0,0)*/0x909090); // grey
    Patch_Dword(0x00809244, RGB(78,182,220)/*0x0DCB64E*/); // OutlineColor
    Patch_Dword(0x00809230, /*RGB32(255,0,0)*/0x615022); // dark teal
    Patch_Dword(0x008093A4, /*RGB32(255,0,0)*/0x221B0B); // very dark teal
    Patch_Dword(0x00808E30, /*RGB32(255,0,0)*/0x443716); // dark brown
    Patch_Dword(0x00808B78, (uintptr_t)CreateSolidBrush(RGB(48,96,48)));
    Patch_Dword(0x007071D0, (uintptr_t)ButtonFont);
    Patch_Dword(0x007071D8, 14); // ButtonFontSize
    Patch_Dword(0x007071D4, (uintptr_t)ListFont);
    Patch_Dword(0x007071DC, 12); // ListFontSize
}
