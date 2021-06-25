/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OBJECTEXT_INIT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for initialising the extended ObjectClass.
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
#include "objectext_hooks.h"
#include "object.h"
#include "objectext.h"
#include "tracker.h"
#include "tibsun_globals.h"
#include "vinifera_util.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  Patch for including the extended class members in the creation process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_ObjectClass_Constructor_Patch)
{
    GET_REGISTER_STATIC(ObjectClass *, this_ptr, ebp); // "this" pointer.
    static ObjectClassExtension *ext_ptr;

    //DEV_DEBUG_WARNING("Creating ObjectClassExtension instance.\n");

    /**
     *  Find existing or create an extended class instance.
     */
    ext_ptr = ObjectClassExtensions.find_or_create(this_ptr);
    if (!ext_ptr) {
        DEBUG_ERROR("Failed to create ObjectClassExtension instance!\n");
        ShowCursor(TRUE);
        MessageBoxA(MainWindow, "Failed to create ObjectClassExtension instance!\n", "Vinifera", MB_OK|MB_ICONEXCLAMATION);
        Vinifera_Generate_Mini_Dump();
        Fatal("Failed to create ObjectClassExtension instance!\n");
        goto original_code; // Keep this for clean code analysis.
    }

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov eax, this_ptr }
    _asm { pop esi }
    _asm { pop ebx }
    _asm { ret }
}


/**
 *  Patch for including the extended class members in the noinit creation process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_ObjectClass_NoInit_Constructor_Patch)
{
    GET_REGISTER_STATIC(ObjectClass *, this_ptr, esi);
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
DECLARE_PATCH(_ObjectClass_Destructor_Patch)
{
    GET_REGISTER_STATIC(ObjectClass *, this_ptr, esi);

    /**
     *  Remove the extended class from the global index.
     */
    ObjectClassExtensions.remove(this_ptr);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { pop esi }
    _asm { pop ecx }
    _asm { ret }
}


/**
 *  Patch for including the extended class members to the base class detach from all process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_ObjectClass_Detach_All_Patch)
{
    GET_REGISTER_STATIC(ObjectClass *, this_ptr, esi);
    GET_STACK_STATIC8(bool, all, esp, 0x10);
    static ObjectClassExtension *ext_ptr;

    /**
     *  Find the extension instance.
     */
    ext_ptr = ObjectClassExtensions.find(this_ptr, false);
    if (!ext_ptr) {
        goto original_code;
    }

    /**
     *  Read type class detach.
     */
    ext_ptr->Detach_All(all);

    /**
     *  Stolen bytes here.
     */
    Detach_This_From_All(this_ptr, all);

original_code:
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebx }
    _asm { ret 4 }
}


/**
 *  Main function for patching the hooks.
 */
void ObjectClassExtension_Init()
{
    Patch_Jump(0x00584990, &_ObjectClass_Constructor_Patch);
    Patch_Jump(0x005849BA, &_ObjectClass_NoInit_Constructor_Patch);
    Patch_Jump(0x00584B4D, &_ObjectClass_Destructor_Patch);
    Patch_Jump(0x00585F98, &_ObjectClass_Detach_All_Patch);
}
