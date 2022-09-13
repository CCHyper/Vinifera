/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          WARHEADTYPEEXT_INIT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for initialising the extended WarheadTypeClass.
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
#include "warheadtypeext_hooks.h"
#include "warheadtypeext.h"
#include "warheadtype.h"
#include "tibsun_globals.h"
#include "vinifera_util.h"
#include "extension.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Patch for including the extended class members in the creation process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_WarheadTypeClass_Constructor_Patch)
{
    GET_REGISTER_STATIC(WarheadTypeClass *, this_ptr, ebp); // "this" pointer.
    GET_STACK_STATIC(const char *, ini_name, esp, 0x14); // ini name.
    static WarheadTypeClassExtension *exttype_ptr;

    /**
     *  Find existing or create an extended class instance.
     */
    exttype_ptr = Find_Or_Make_Extension<WarheadTypeClassExtension>(this_ptr);
    if (!exttype_ptr) {
        DEBUG_ERROR("Failed to create WarheadTypeClassExtensions instance for \"%s\"!\n", ini_name);
        ShowCursor(TRUE);
        MessageBoxA(MainWindow, "Failed to create WarheadTypeClassExtensions instance!\n", "Vinifera", MB_OK|MB_ICONEXCLAMATION);
        Vinifera_Generate_Mini_Dump();
        Fatal("Failed to create WarheadTypeClassExtensions instance!\n");
        goto original_code; // Keep this for clean code analysis.
    }

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov eax, this_ptr }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { ret 4 }
}


/**
 *  Patch for including the extended class members in the noinit creation process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_WarheadTypeClass_NoInit_Constructor_Patch)
{
    GET_REGISTER_STATIC(WarheadTypeClass *, this_ptr, esi);
    GET_STACK_STATIC(const NoInitClass *, noinit_ptr, esp, 0x4);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov eax, this_ptr }
    _asm { pop esi }
    _asm { ret 4 }
}


/**
 *  Patch for including the extended class members in the destruction process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_WarheadTypeClass_Destructor_Patch)
{
    GET_REGISTER_STATIC(WarheadTypeClass *, this_ptr, esi);

    /**
     *  Remove the extended class from the global index.
     */
    Destroy_Extension<WarheadTypeClassExtension>(this_ptr);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov edx, ds:0x0074C798 } // WarheadTypes.vtble
    JMP_REG(eax, 0x0066EF7E);
}


/**
 *  Patch for including the extended class members in the virtual destruction process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_WarheadTypeClass_Scalar_Destructor_Patch)
{
    GET_REGISTER_STATIC(WarheadTypeClass *, this_ptr, esi);

    /**
     *  Remove the extended class from the global index.
     */
    Destroy_Extension<WarheadTypeClassExtension>(this_ptr);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov edx, ds:0x0074C798 } // WarheadTypes.vtble
    JMP_REG(eax, 0x0066FA9E);
}


/**
 *  Patch for including the extended class members to the base class detach process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_WarheadTypeClass_Detach_Patch)
{
    GET_REGISTER_STATIC(WarheadTypeClass *, this_ptr, ecx);
    GET_STACK_STATIC(TARGET, target, esp, 0x8);
    GET_STACK_STATIC8(bool, all, esp, 0xC);
    static WarheadTypeClassExtension *exttype_ptr;

    /**
     *  Fetch the extension instance.
     */
    exttype_ptr = Fetch_Extension<WarheadTypeClassExtension>(this_ptr);

    /**
     *  Read type class detach.
     */
    exttype_ptr->Detach(target, all);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { push esi }
    _asm { mov ecx, this_ptr } // restore "this".
    _asm { lea esi, [ecx+0x9C] }
    JMP_REG(ecx, 0x0066F9BF);
}


/**
 *  Patch for including the extended class members when computing a unique crc value for this instance.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_WarheadTypeClass_Compute_CRC_Patch)
{
    GET_REGISTER_STATIC(WarheadTypeClass *, this_ptr, esi);
    GET_STACK_STATIC(WWCRCEngine *, crc, esp, 0x18);
    static WarheadTypeClassExtension *exttype_ptr;

    /**
     *  Fetch the extension instance.
     */
    exttype_ptr = Fetch_Extension<WarheadTypeClassExtension>(this_ptr);

    /**
     *  Read type class compute crc.
     */
    exttype_ptr->Compute_CRC(*crc);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebx }
    _asm { mov esp, ebp }
    _asm { pop ebp }
    _asm { ret 4 }
}


/**
 *  Patch for reading the extended class members from the ini instance.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_WarheadTypeClass_Read_INI_Patch)
{
    GET_REGISTER_STATIC(WarheadTypeClass *, this_ptr, esi);
    GET_STACK_STATIC(CCINIClass *, ini, ebp, 0x8); // Can't use EDI as its reused by this point.
    static WarheadTypeClassExtension *exttype_ptr;

    /**
     *  Fetch the extension instance.
     */
    exttype_ptr = Fetch_Extension<WarheadTypeClassExtension>(this_ptr);

    /**
     *  Read type class ini.
     */
    exttype_ptr->Read_INI(*ini);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov al, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebx }
    _asm { mov esp, ebp }
    _asm { pop ebp }
    _asm { ret 4 }
}


/**
 *  Main function for patching the hooks.
 */
void WarheadTypeClassExtension_Init()
{
    Patch_Jump(0x0066EEF4, &_WarheadTypeClass_Constructor_Patch);
    Patch_Jump(0x0066EF4F, &_WarheadTypeClass_NoInit_Constructor_Patch);
    //Patch_Jump(0x0066EF78, &_WarheadTypeClass_Destructor_Patch); // Destructor is actually inlined in scalar destructor!
    Patch_Jump(0x0066FA98, &_WarheadTypeClass_Scalar_Destructor_Patch);
    Patch_Jump(0x0066F9B8, &_WarheadTypeClass_Detach_Patch);
    Patch_Jump(0x0066F6A4, &_WarheadTypeClass_Compute_CRC_Patch);
    Patch_Jump(0x0066F566, &_WarheadTypeClass_Read_INI_Patch);
}
