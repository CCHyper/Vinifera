/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          LASERDRAWEXT_INIT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for initialising the extended LaserDrawClass.
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
#include "laserdrawext.h"
#include "laserdraw.h"
#include "techno.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"
#include "vinifera_util.h"


/**
 *  Patch for including the extended class members in the creation process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_LaserDrawClass_Constructor_Patch)
{
    GET_REGISTER_STATIC(LaserDrawClass *, this_ptr, esi); // Current "this" pointer.
    static LaserDrawClassExtension *ext_ptr;

    /**
     *  Find existing or create an extended class instance.
     */
    ext_ptr = LaserDrawClassExtensions.find_or_create(this_ptr);
    if (!ext_ptr) {
        DEBUG_ERROR("Failed to create LaserDrawClassExtension instance for 0x%08X!\n", (uintptr_t)this_ptr);
        ShowCursor(TRUE);
        MessageBoxA(MainWindow, "Failed to create LaserDrawClassExtensions instance!\n", "Vinifera", MB_OK|MB_ICONEXCLAMATION);
        Vinifera_Generate_Mini_Dump();
        Fatal("Failed to create LaserDrawClassExtensions instance!\n");
        goto original_code; // Keep this for clean code analysis.
    }

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov eax, this_ptr }
    _asm { pop edi }
    _asm { pop esi }
    _asm { ret 0x40 }
}


/**
 *  Patch for including the extended class members in the destruction process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_LaserDrawClass_Deconstructor_Patch)
{
    GET_REGISTER_STATIC(LaserDrawClass *, this_ptr, esi);

    /**
     *  Remove the extended class from the global index.
     */
    LaserDrawClassExtensions.remove(this_ptr);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { pop ecx }
    _asm { ret }
}


/**
 *  Main function for patching the hooks.
 */
void LaserDrawClassExtension_Init()
{
    Patch_Jump(0x004FBDB9, &_LaserDrawClass_Constructor_Patch);
    Patch_Jump(0x004FBE0A, &_LaserDrawClass_Deconstructor_Patch);
}
