/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          NEWINI_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for implementing the new ini class.
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
#include "newini.h"
#include "vinifera_defines.h"

#include "hooker.h"
#include "hooker_macros.h"


void NewINI_Hooks()
{
#ifdef VINIFERA_USE_NEW_INI
    //Patch_Jump(0x, &ViniferaINIClass::ViniferaINIClass~INIClass);
    Patch_Jump(0x004DB540, &ViniferaINIClass::Clear);
    Patch_Jump(0x004DB780, static_cast<bool (ViniferaINIClass:: *)(FileClass &, bool)>(&ViniferaINIClass::Load));
    Patch_Jump(0x004DB7D0, static_cast<bool (ViniferaINIClass:: *)(Straw &, bool)>(&ViniferaINIClass::Load));
    Patch_Jump(0x004DC180, static_cast<int (ViniferaINIClass:: *)(FileClass &, bool) const>(&ViniferaINIClass::Save));
    Patch_Jump(0x004DC1C0, static_cast<int (ViniferaINIClass:: *)(Pipe &, bool) const>(&ViniferaINIClass::Save));
    //Patch_Jump(0x004DC550, &ViniferaINIClass::Find_Section);
    //Patch_Jump(0x004DC690, &ViniferaINIClass::Section_Count);
    //Patch_Jump(0x, &ViniferaINIClass::Entry_Count);
    //Patch_Jump(0x, &ViniferaINIClass::Find_Entry);
    //Patch_Jump(0x, &ViniferaINIClass::Get_Entry);
    //Patch_Jump(0x, &ViniferaINIClass::Put_UUBlock);
    //Patch_Jump(0x, &ViniferaINIClass::Get_UUBlock);
    //Patch_Jump(0x, &ViniferaINIClass::Put_TextBlock);
    //Patch_Jump(0x, &ViniferaINIClass::Get_TextBlock);
    //Patch_Jump(0x, &ViniferaINIClass::Put_Int);
    //Patch_Jump(0x, &ViniferaINIClass::Get_Int);
    //Patch_Jump(0x, &ViniferaINIClass::Get_UUID);
    //Patch_Jump(0x, &ViniferaINIClass::Put_UUID);
    //Patch_Jump(0x, &ViniferaINIClass::Put_Rect);
    //Patch_Jump(0x, &ViniferaINIClass::Get_Rect);
    //Patch_Jump(0x, &ViniferaINIClass::Put_Hex);
    //Patch_Jump(0x, &ViniferaINIClass::Get_Hex);
    //Patch_Jump(0x, &ViniferaINIClass::Get_Double);
    //Patch_Jump(0x, &ViniferaINIClass::Put_Double);
    //Patch_Jump(0x, &ViniferaINIClass::Put_String);
    //Patch_Jump(0x, &ViniferaINIClass::Get_String);
    //Patch_Jump(0x, &ViniferaINIClass::Put_Bool);
    //Patch_Jump(0x, &ViniferaINIClass::Get_Bool);
    //Patch_Jump(0x, &ViniferaINIClass::Put_Point);
    //Patch_Jump(0x, &ViniferaINIClass::Get_Point);
    //Patch_Jump(0x, &ViniferaINIClass::Get_Point);
    //Patch_Jump(0x, &ViniferaINISection::Find_Entry);
    //Patch_Jump(0x, &ViniferaINIClass::Put_PKey);
    //Patch_Jump(0x, &ViniferaINIClass_Extract_Comment);
    //Patch_Jump(0x, &ViniferaINIClass::Strip_Comments);
    //Patch_Jump(0x, &ViniferaINIClass::Contains_Section);
#endif
}

