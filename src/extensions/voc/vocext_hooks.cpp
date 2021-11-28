/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VOCEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended VocClass.
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
#include "vocext_hooks.h"
#include "voc.h"
#include "tibsun_functions.h"
#include "ccfile.h"

#include "debughandler.h"
//#include <string>

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or deconstructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
static class VocClassFake final : public VocClass
{
    public:
        const void *_Load_File(const char *name);
};


/**
 *  Loads the sound effect file from the mix files.
 * 
 *  @author: CCHyper
 */
const void *VocClassFake::_Load_File(const char *name)
{
    char buffer[_MAX_FNAME+_MAX_EXT];
    const void *fileptr = nullptr;

#if 0
    /**
     *  Search for the Ogg version first.
     */
    _makepath(buffer, nullptr, nullptr, name, ".OGG");
    fileptr = MFCC::Retrieve(buffer);
    if (fileptr) {
#ifndef NDEBUG
        DEV_DEBUG_INFO("Voc: Found Ogg for \"%s\".\n", name);
#endif
        return nullptr;
    }
#endif








    _makepath(buffer, nullptr, nullptr, name, ".AUD");

#ifdef NDEBUG
    fileptr = MFCC::Retrieve(filename);
#else
    RawFileClass file(buffer);

    if (file.Is_Available()) {
        fileptr = Load_Alloc_Data(file);
    } else {
        fileptr = MFCC::Retrieve(buffer);
    }
    if (fileptr) {
//#ifndef NDEBUG
//        DEV_DEBUG_INFO("Voc: Found AUD for \"%s\".\n", name);
//#endif
        return fileptr;
    }
#endif

    return nullptr;
}


/**
 *  Patches to load the sound effect file.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_VocClass_Constructor_Load_File_Patch)
{
    GET_REGISTER_STATIC(VocClassFake *, this_ptr, ebx);

    this_ptr->FilePtr = (void *)this_ptr->_Load_File(this_ptr->Filename);

    _asm { mov eax, this_ptr }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebx }
    _asm { add esp, 0x200 }
    _asm { retn 4 }
}

DECLARE_PATCH(_VocClass_Read_INI_Load_File_Patch)
{
    GET_REGISTER_STATIC(VocClassFake *, this_ptr, esi);

    this_ptr->FilePtr = (void *)this_ptr->_Load_File(this_ptr->Filename);

    _asm { mov eax, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { add esp, 0x200 }
    _asm { retn 4 }
}

DECLARE_PATCH(_VocClass_Read_INI_Default_Load_File_Patch)
{
    GET_REGISTER_STATIC(VocClassFake *, this_ptr, esi);

    this_ptr->Priority = 10;
    this_ptr->Volume = 1.0f;

    this_ptr->FilePtr = (void *)this_ptr->_Load_File(this_ptr->Filename);

    _asm { mov eax, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { add esp, 0x200 }
    _asm { retn 4 }
}

DECLARE_PATCH(_VocClass_Init_Inlined_Constructor_Patch)
{
    GET_REGISTER_STATIC(VocClassFake *, this_ptr, ebp);
    static const void *file;

    file = (void *)this_ptr->_Load_File(this_ptr->Filename);

    _asm { mov eax, file }
    JMP_REG(ecx, 0x0066503D);
}


/**
 *  Main function for patching the hooks.
 */
void VocClassExtension_Hooks()
{
    Patch_Jump(0x006648E3, &_VocClass_Constructor_Load_File_Patch);
    Patch_Jump(0x006649CB, &_VocClass_Read_INI_Load_File_Patch);
    Patch_Jump(0x006649FE, &_VocClass_Read_INI_Default_Load_File_Patch);
    Patch_Jump(0x0066501D, &_VocClass_Init_Inlined_Constructor_Patch);
}
