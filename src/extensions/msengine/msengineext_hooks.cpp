/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MSENGINEEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended MS classes.
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
#include "msengineext_hooks.h"
#include "msengine.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "ccfile.h"
#include "debughandler.h"
#include <string>

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Loads the sound effect file from the mix files.
 * 
 *  @author: CCHyper
 */
static const void *Load_File(const char *filename)
{
    char buffer[_MAX_FNAME+_MAX_EXT];
    const void *fileptr = nullptr;

    char *fnamecast = (char *)filename;
    fnamecast[std::strlen(filename)-4] = '\0';

#if 0
    /**
     *  Search for the Ogg version first.
     */
    _makepath(buffer, nullptr, nullptr, filename, ".OGG");
    fileptr = MFCC::Retrieve(buffer);
    if (fileptr) {
#ifndef NDEBUG
        DEV_DEBUG_INFO("MSSfx: Found Ogg for \"%s\".\n", filename);
#endif
        return fileptr;
    }
#endif

    _makepath(buffer, nullptr, nullptr, filename, ".AUD");

#ifdef NDEBUG
    fileptr = MFCC::Retrieve(buffer);
#else
    RawFileClass file(buffer);

    if (file.Is_Available()) {
        fileptr = Load_Alloc_Data(file);
    } else {
        fileptr = MFCC::Retrieve(buffer);
    }
    if (fileptr) {
#ifndef NDEBUG
        DEV_DEBUG_INFO("MSSfx: Found AUD for \"%s\".\n", filename);
#endif
        return fileptr;
    }
#endif

    return nullptr;
}


DECLARE_PATCH(_MSSfxEntry_Constructor_Load_File_Patch)
{
    GET_REGISTER_STATIC(const char *, fname, edi);
    static const void *fileptr;

    _asm { mov [esi], eax }

    fileptr = Load_File(fname);
    _asm { mov eax, fileptr }

    _asm { mov edi, fname }

    JMP_REG(ecx, 0x0056F32F);
}


/**
 *  Main function for patching the hooks.
 */
void MSEngineExtension_Hooks()
{
    Patch_Jump(0x0056F326, &_MSSfxEntry_Constructor_Load_File_Patch);
}
