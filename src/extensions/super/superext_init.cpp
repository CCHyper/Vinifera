/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SUPEREXT_INIT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for initialising the extended SuperClass.
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
#include "superext.h"
#include "super.h"
#include "tibsun_globals.h"
#include "vinifera_util.h"
#include "vinifera_globals.h"
#include "extension.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Patch for including the extended class members in the creation process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_SuperClass_Default_Constructor_Patch)
{
    GET_REGISTER_STATIC(SuperClass *, this_ptr, esi); // Current "this" pointer.
    static SuperClassExtension *exttype_ptr;

    /**
     *  If we are performing a load operation, the Windows API will invoke the
     *  constructors for us as part of the operation, so we can skip our hook here.
     */
    if (Vinifera_PerformingLoad) {
        goto original_code;
    }

    /**
     *  Find existing or create an extended class instance.
     */
    exttype_ptr = Find_Or_Make_Extension<SuperClassExtension>(this_ptr);
    if (!exttype_ptr) {
        DEBUG_ERROR("Failed to create SuperClassExtension instance for 0x%08X!\n", (uintptr_t)this_ptr);
        ShowCursor(TRUE);
        MessageBoxA(MainWindow, "Failed to create SuperClassExtensions instance!\n", "Vinifera", MB_OK|MB_ICONEXCLAMATION);
        Vinifera_Generate_Mini_Dump();
        Fatal("Failed to create SuperClassExtensions instance!\n");
        goto original_code; // Keep this for clean code analysis.
    }

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov eax, this_ptr }
    _asm { pop esi }
    _asm { ret }
}


/**
 *  Patch for including the extended class members in the creation process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_SuperClass_Constructor_Patch)
{
    GET_REGISTER_STATIC(SuperClass *, this_ptr, esi); // Current "this" pointer.
    static SuperClassExtension *exttype_ptr;

    /**
     *  Find existing or create an extended class instance.
     */
    exttype_ptr = Find_Or_Make_Extension<SuperClassExtension>(this_ptr);
    if (!exttype_ptr) {
        DEBUG_ERROR("Failed to create SuperClassExtension instance for 0x%08X!\n", (uintptr_t)this_ptr);
        ShowCursor(TRUE);
        MessageBoxA(MainWindow, "Failed to create SuperClassExtensions instance!\n", "Vinifera", MB_OK|MB_ICONEXCLAMATION);
        Vinifera_Generate_Mini_Dump();
        Fatal("Failed to create SuperClassExtensions instance!\n");
        goto original_code; // Keep this for clean code analysis.
    }

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov eax, this_ptr }
    _asm { pop esi }
    _asm { pop ebx }
    _asm { ret 8 }
}


/**
 *  Patch for including the extended class members in the destruction process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_SuperClass_Destructor_Patch)
{
    GET_REGISTER_STATIC(SuperClass *, this_ptr, esi);

    /**
     *  Remove the extended class from the global index.
     */
    Destroy_Extension<SuperClassExtension>(this_ptr);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov edx, ds:0x0080F588 } // Neuron vector.vtble
    JMP_REG(eax, 0x0060B520);
}


/**
 *  Patch for including the extended class members in the virtual destruction process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_SuperClass_Scalar_Destructor_Patch)
{
    GET_REGISTER_STATIC(SuperClass *, this_ptr, esi);

    /**
     *  Remove the extended class from the global index.
     */
    Destroy_Extension<SuperClassExtension>(this_ptr);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov edx, ds:0x0080F588 } // Neuron vector.vtble
    JMP_REG(eax, 0x0060CC30);
}


/**
 *  Patch for including the extended class members to the base class detach process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_SuperClass_Detach_Patch)
{
    GET_REGISTER_STATIC(SuperClass *, this_ptr, ecx);
    GET_STACK_STATIC(TARGET, target, esp, 0x10);
    GET_STACK_STATIC8(bool, all, esp, 0x8);
    static SuperClassExtension *ext_ptr;

    /**
     *  Fetch the extension instance.
     */
    ext_ptr = Fetch_Extension<SuperClassExtension>(this_ptr);

    ext_ptr->Detach(target, all);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { ret 8 }
}


/**
 *  Patch for including the extended class members to the base class crc calculation.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_SuperClass_Compute_CRC_Patch)
{
    GET_REGISTER_STATIC(SuperClass *, this_ptr, esi);
    GET_STACK_STATIC(WWCRCEngine *, crc, esp, 0xC);
    static SuperClassExtension *ext_ptr;

    /**
     *  Fetch the extension instance.
     */
    ext_ptr = Fetch_Extension<SuperClassExtension>(this_ptr);

    ext_ptr->Compute_CRC(*crc);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { pop edi }
    _asm { pop esi }
    _asm { ret 4 }
}


/**
 *  Main function for patching the hooks.
 */
void SuperClassExtension_Init()
{
    Patch_Jump(0x0060B352, &_SuperClass_Default_Constructor_Patch);
    Patch_Jump(0x0060B4AB, &_SuperClass_Constructor_Patch);
    Patch_Jump(0x0060B51A, &_SuperClass_Destructor_Patch);
    Patch_Jump(0x0060CC2A, &_SuperClass_Scalar_Destructor_Patch);
    Patch_Jump(0x0060C81C, &_SuperClass_Detach_Patch);
    Patch_Jump(0x0060C870, &_SuperClass_Compute_CRC_Patch);
}
