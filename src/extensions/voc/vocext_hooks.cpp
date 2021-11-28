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
#include "wstring.h"
#include "tibsun_functions.h"
#include "ccfile.h"
#include "audio_driver.h"
#include "audio_util.h"

#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Loads the sound effect file.
 * 
 *  @author: CCHyper
 */
static const void *Voc_Load_File(const char *fname)
{
    const void *fileptr = nullptr;

    RawFileClass file(fname);

    if (file.Is_Available()) {
        fileptr = Load_Alloc_Data(file);
    } else {
        fileptr = MFCC::Retrieve(fname);
    }

    return fileptr;
}


/**
 *  Returns the file handle of the best available sound effect.
 * 
 *  @author: CCHyper
 */
static const void *Voc_Get_Available_File(const char *name)
{
    /**
     *  Attempt to play the sound effect in supported formats.
     *  Priority: OGG -> MP3 -> WAV-> AUD
     */
    Wstring fname;
    bool found = false;

    if (!Audio_Driver_Is_Direct_Sound()) {

        fname = Build_Audio_File_Name(name, "xOGG"); // Ogg disabled for now.
        if (Is_Sfx_File_Available(fname.Peek_Buffer())) {
            found = true;
        }

        if (!found) {
            fname = Build_Audio_File_Name(name, "xMP3"); // Mp3 disabled for now.
            if (Is_Sfx_File_Available(fname.Peek_Buffer())) {
                found = true;
            }
        }

        if (!found) {
            fname = Build_Audio_File_Name(name, "xWAV"); // Wav disabled for now.
            if (Is_Sfx_File_Available(fname.Peek_Buffer())) {
                found = true;
            }
        }

    }

    if (!found) {
        fname = Build_Audio_File_Name(name, "AUD");
        if (Is_Sfx_File_Available(fname.Peek_Buffer())) {
            found = true;
        }
    }

    if (found) {
        return Voc_Load_File(fname.Peek_Buffer());
    }

    return nullptr;
}


/**
 *  Patches to load the sound effect file.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_VocClass_Constructor_Load_File_Patch)
{
    GET_REGISTER_STATIC(VocClass *, this_ptr, ebx);

    this_ptr->FilePtr = (void *)Voc_Get_Available_File(this_ptr->Filename);

    _asm { mov eax, this_ptr }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebx }
    _asm { add esp, 0x200 }
    _asm { retn 4 }
}

DECLARE_PATCH(_VocClass_Read_INI_Load_File_Patch)
{
    GET_REGISTER_STATIC(VocClass *, this_ptr, esi);

    this_ptr->FilePtr = (void *)Voc_Get_Available_File(this_ptr->Filename);

    _asm { mov eax, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { add esp, 0x200 }
    _asm { retn 4 }
}

DECLARE_PATCH(_VocClass_Read_INI_Default_Load_File_Patch)
{
    GET_REGISTER_STATIC(VocClass *, this_ptr, esi);

    this_ptr->Priority = 10;
    this_ptr->Volume = 1.0f;

    this_ptr->FilePtr = (void *)Voc_Get_Available_File(this_ptr->Filename);

    _asm { mov eax, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { add esp, 0x200 }
    _asm { retn 4 }
}

DECLARE_PATCH(_VocClass_Init_Inlined_Constructor_Patch)
{
    GET_REGISTER_STATIC(VocClass *, this_ptr, ebp);
    static const void *file;

    file = (void *)Voc_Get_Available_File(this_ptr->Filename);

    _asm { mov eax, file }
    JMP_REG(ecx, 0x0066503D);
}


/**
 *  Main function for patching the hooks.
 */
void VocClassExtension_Hooks()
{
    //Patch_Jump(0x006648E3, &_VocClass_Constructor_Load_File_Patch);
    //Patch_Jump(0x006649CB, &_VocClass_Read_INI_Load_File_Patch);
    //Patch_Jump(0x006649FE, &_VocClass_Read_INI_Default_Load_File_Patch);
    //Patch_Jump(0x0066501D, &_VocClass_Init_Inlined_Constructor_Patch);
}
