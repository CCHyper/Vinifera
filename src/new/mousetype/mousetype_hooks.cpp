/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MOUSETYPE_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for mouse cursor controls and overrides.
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
#include "mousetype_hooks.h"
#include "mousetype.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


void MouseTypeClass_Hooks()
{
    Patch_Dword(0x0056222C+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, SmallFrame));  // MouseClass::Mouse_Small
    Patch_Dword(0x00562246+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, StartFrame));  // MouseClass::Mouse_Small
    Patch_Dword(0x0056224D+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, X));           // MouseClass::Mouse_Small
    Patch_Dword(0x00562284+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, Y));           // MouseClass::Mouse_Small
    Patch_Dword(0x005622D8+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, StartFrame));  // MouseClass::Get_Mouse_Frame
    Patch_Dword(0x00562323+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, X));           // MouseClass::Get_Mouse_Hotspot
    Patch_Dword(0x0056232A+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, StartFrame));  // MouseClass::Get_Mouse_Hotspot
    Patch_Dword(0x0056239F+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, SmallFrame));  // MouseClass::Override_Mouse_Shape
    Patch_Dword(0x005623EC+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, FrameRate));   // MouseClass::Override_Mouse_Shape
    Patch_Dword(0x0056241C+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, SmallFrame));  // MouseClass::Override_Mouse_Shape
    Patch_Dword(0x0056242E+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, StartFrame));  // MouseClass::Override_Mouse_Shape
    Patch_Dword(0x00562439+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, X));           // MouseClass::Override_Mouse_Shape
    Patch_Dword(0x0056246C+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, Y));           // MouseClass::Override_Mouse_Shape
    Patch_Dword(0x005624E1+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, StartFrame));  // MouseClass::AI
    Patch_Dword(0x005624E8+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, FrameRate));   // MouseClass::AI
    Patch_Dword(0x00562572+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, SmallFrame));  // MouseClass::AI
    Patch_Dword(0x0056258C+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, StartFrame));  // MouseClass::AI
    Patch_Dword(0x0056259F+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, X));           // MouseClass::AI
    Patch_Dword(0x005625D6+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, Y));           // MouseClass::AI
    Patch_Dword(0x00563227+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, StartFrame));  // MouseClass::Get_Mouse_Start_Frame
    Patch_Dword(0x00563247+3, (uintptr_t)(&MouseTypeClass::MouseControl) + offsetof(MouseTypeClass::MouseStruct, FrameCount));  // MouseClass::Get_Mouse_Frame_Count
}
