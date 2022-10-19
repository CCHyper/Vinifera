/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AIRCRAFTEXT_INIT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for initialising the extended AircraftClass.
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
#include "aircraftext.h"
#include "aircrafttypeext.h"
#include "aircraft.h"
#include "aircrafttype.h"
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
DECLARE_PATCH(_AircraftClass_Constructor_Patch)
{
    GET_REGISTER_STATIC(AircraftClass *, this_ptr, esi); // Current "this" pointer.
    static AircraftClassExtension *exttype_ptr;

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
    exttype_ptr = Extension::Make<AircraftClassExtension>(this_ptr);
    if (!exttype_ptr) {
        DEBUG_ERROR("Failed to create AircraftClassExtension instance for 0x%08X!\n", (uintptr_t)this_ptr);
        ShowCursor(TRUE);
        MessageBoxA(MainWindow, "Failed to create AircraftClassExtensions instance!\n", "Vinifera", MB_OK|MB_ICONEXCLAMATION);
        Vinifera_Generate_Mini_Dump();
        Fatal("Failed to create AircraftClassExtensions instance!\n");
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
    _asm { ret 8 }
}


/**
 *  Patch for including the extended class members in the noinit creation process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AircraftClass_NoInit_Constructor_Patch)
{
    GET_REGISTER_STATIC(AircraftClass *, this_ptr, ebx);
    GET_STACK_STATIC(const NoInitClass *, noinit, esp, 0x10);
    static AircraftClassExtension *ext_ptr;

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov dword ptr [ebx], 0x006CADF8 } // this->vftable = const AircraftClass::`vftable';
    JMP(0x0040EB87);
}


/**
 *  Patch for including the extended class members in the destruction process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AircraftClass_Destructor_Patch)
{
    GET_REGISTER_STATIC(AircraftClass *, this_ptr, esi);

    /**
     *  Remove the extended class from the global index.
     */
    Extension::Destroy<AircraftClassExtension>(this_ptr);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { mov edx, ds:0x007E4058 } // Aircraft.vtble
    JMP_REG(eax, 0x0040DBBE);
}


/**
 *  Patch for including the extended class members to the base class detach process.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AircraftClass_Detach_Patch)
{
    GET_REGISTER_STATIC(AircraftClass *, this_ptr, esi);
    GET_STACK_STATIC(TARGET, target, esp, 0x10);
    GET_STACK_STATIC8(bool, all, esp, 0x8);
    static AircraftClassExtension *ext_ptr;

    /**
     *  Fetch the extension instance.
     */
    ext_ptr = Extension::Fetch<AircraftClassExtension>(this_ptr);

    ext_ptr->Detach(target, all);

    /**
     *  Stolen bytes here.
     */
original_code:
    _asm { pop edi }
    _asm { pop esi }
    _asm { ret 8 }
}


/**
 *  Patch for including the extended class members to the base class crc calculation.
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AircraftClass_Compute_CRC_Patch)
{
    GET_REGISTER_STATIC(AircraftClass *, this_ptr, esi);
    GET_STACK_STATIC(WWCRCEngine *, crc, esp, 0xC);
    static AircraftClassExtension *ext_ptr;

    /**
     *  Fetch the extension instance.
     */
    ext_ptr = Extension::Fetch<AircraftClassExtension>(this_ptr);

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
void AircraftClassExtension_Init()
{
    Patch_Jump(0x0040880C, &_AircraftClass_Constructor_Patch);
    Patch_Jump(0x0040EB81, &_AircraftClass_NoInit_Constructor_Patch);
    Patch_Jump(0x0040DBB8, &_AircraftClass_Destructor_Patch);
    //Patch_Jump(0x0040EDC5, &_AircraftClass_Detach_Patch);
    Patch_Jump(0x0040ED91, &_AircraftClass_Compute_CRC_Patch);
}
