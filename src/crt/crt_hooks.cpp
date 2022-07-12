/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CRT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Setup all the hooks to take control of the basic CRT.
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
#include "crt_hooks.h"
#include "vinifera_globals.h"
#include <fenv.h>
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Timestamp of execution.
 */
int Execute_Day = 0;
int Execute_Month = 0;
int Execute_Year = 0;
int Execute_Hour = 0;
int Execute_Min = 0;
int Execute_Sec = 0;
char Execute_Time_Buffer[256];


extern void * __cdecl crt_alloc(unsigned int size);
extern void * __cdecl crt_calloc(unsigned int count, unsigned int size);
extern void * __cdecl crt_realloc(void *ptr, unsigned int size);
extern void __cdecl crt_free(void *ptr);
extern unsigned int __cdecl crt_msize(void *ptr);

extern void Hook_New_Delete_Malloc_Free();


/**
 *  Reimplementation of strdup() to use our memory allocator.
 */
static char * __cdecl crt_strdup(const char *string)
{
    char *str;
    char *p;
    int len = 0;

    while (string[len]) {
        len++;
    }
    str = (char *)crt_alloc(len + 1);
    p = str;
    while (*string) {
        *p++ = *string++;
    }
    *p = '\0';
    return str;
}


/**
 *  Set the FPU mode to match the game (rounding towards zero [chop mode]).
 */
DECLARE_PATCH(_set_fp_mode)
{
    // Call to "store_fpu_codeword"
    _asm { mov edx, 0x006B2314 };
    _asm { call edx };

    /**
     *  Set the FPU mode to match the game (rounding towards zero [chop mode]).
     */
    _set_controlfp(_RC_CHOP, _MCW_RC);

    /**
     *  And this is required for the std c++ lib.
     */
    fesetround(FE_TOWARDZERO);

    JMP(0x005FFDB0);
}


/**
 *  Main function for patching the hooks.
 */
void CRT_Hooks()
{
    /**
     *  Call the games fpmath to make sure we init 
     */
    Patch_Jump(0x005FFDAB, &_set_fp_mode);

    /**
     *  dynamic init functions call _msize indirectly.
     *  They call __onexit, so we need to patch this
     *  to direct to our memory manager.
     */
    Hook_Function(0x006B80AA, &crt_msize);

    /**
     *  Standard functions.
     */
    Hook_Function(0x006BE766, &crt_strdup);

    /**
     *  C memory functions.
     */
    Hook_Function(0x006B72CC, &crt_alloc);
    Hook_Function(0x006BCA26, &crt_calloc);
    Hook_Function(0x006B7F72, &crt_realloc);
    Hook_Function(0x006B67E4, &crt_free);

    /**
     *  C++ new and delete.
     */
    Hook_Function(0x006B51D7, &crt_alloc);
    Hook_Function(0x006B51CC, &crt_free);

    Hook_New_Delete_Malloc_Free();
}


/**
 *  These wrapper functions are defined at the end of the module as we can't
 *  include the memory manager header at the top of the file as it might cause
 *  issues with existing new and delete operations.
 */
#include "vinifera_memmgr.h"

static void * __cdecl crt_alloc(unsigned int size)
{
    return ViniferaMemory::Alloc(size);
}

static void * __cdecl crt_calloc(unsigned int count, unsigned int size)
{
    return ViniferaMemory::CountAlloc(count, size);
}

static void * __cdecl crt_realloc(void *ptr, unsigned int size)
{
    return ViniferaMemory::Realloc(ptr, size);
}

static void __cdecl crt_free(void *ptr)
{
    ViniferaMemory::Free(ptr);
}

static unsigned int __cdecl crt_msize(void *ptr)
{
    return ViniferaMemory::MemSize(ptr);
}


/**
 *  
 */
void Init_Vinifera_Memory_Manager()
{
    /**
     *  Create a unique filename for the memory log based on the time of execution.
     */
    char filename_buffer[512];
    std::snprintf(filename_buffer, sizeof(filename_buffer), "%s\\MEMORY_%02u-%02u-%04u_%02u-%02u-%02u.LOG",
        Vinifera_DebugDirectory,
        Execute_Day, Execute_Month, Execute_Year, Execute_Hour, Execute_Min, Execute_Sec);

    ViniferaMemoryManager::Set_Log_File(filename_buffer);
}


/**
 *
 *  x
 *
 */

#define PATCH_MALLOC(patch_name, ret_addr, size, file, func, line) \
    DECLARE_PATCH(patch_name) \
    { \
        static uintptr_t _ret_addr = (uintptr_t)ret_addr; \
        _asm { push size } /* __cdecl */ \
        _asm { call crt_alloc } \
        _asm { jmp _ret_addr } \
    }

#define PATCH_FREE(patch_name, ret_addr, ptr, file, func, line) \
    DECLARE_PATCH(patch_name) \
    { \
        static uintptr_t _ret_addr = (uintptr_t)ret_addr; \
        _asm { push ptr } /* __cdecl */ \
        _asm { call crt_free } \
        _asm { jmp _ret_addr } \
    }

#define PATCH_NEW(patch_name, ret_addr, size, file, func, line) \
    DECLARE_PATCH(patch_name) \
    { \
        static uintptr_t _ret_addr = (uintptr_t)ret_addr; \
        _asm { push size } /* __cdecl */ \
        _asm { call crt_alloc } \
        _asm { jmp _ret_addr } \
    }

#define PATCH_DELETE(patch_name, ret_addr, ptr, file, func, line) \
    DECLARE_PATCH(patch_name) \
    { \
        static uintptr_t _ret_addr = (uintptr_t)ret_addr; \
        _asm { push ptr } /* __cdecl */ \
        _asm { call crt_free } \
        _asm { jmp _ret_addr } \
    }


/**
 *
 *  x
 *
 */
PATCH_MALLOC(_MSChoice_CTOR_Malloc_Patch, 0x0056EB1B, 0x400, "mschoice.cpp", "MSChoice::MSChoice", -1);

PATCH_FREE(_CCFileClass_Open_free_Patch, 0x00449AEB, esi, "cdfile.cpp", "CCFileClass::Open", -1);

PATCH_NEW(_ZBuffer_CTOR_new_Patch, 0x006A8E07, 0x20, "zbuffer.cpp", "ZBuffer::ZBuffer", -1);

PATCH_DELETE(_BlitTrans_ushort_SDDTOR_delete_Patch, 0x006A8C77, esi, "blitblit.h", "BlitTrans<unsigned short>::~BlitTrans", -1);


/**
 *  The new memory manager needs source info, so these hooks falsely provide that
 *  as much as we can to help us track memory leaks.
 */
void Hook_New_Delete_Malloc_Free()
{
    // malloc
    Patch_Jump(0x0056EB11, _MSChoice_CTOR_Malloc_Patch);

    // free
    Patch_Jump(0x00449AE6, _CCFileClass_Open_free_Patch);

    // new
    Patch_Jump(0x006A8E02, _ZBuffer_CTOR_new_Patch);

    // delete
    Patch_Jump(0x006A8C71, _BlitTrans_ushort_SDDTOR_delete_Patch);
}
