/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SAVELOADEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for implementing save/load support for
 *                 the extended classes.
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
#include "saveloadext_hooks.h"
#include "saveloadext.h"
#include "iomap.h"
#include "theme.h"
#include "saveload.h"
#include "loadoptions.h"
#include "language.h"
#include "newswizzle.h"
#include "vinifera_globals.h"
#include "vinifera_util.h"
#include "debughandler.h"
#include "asserthandler.h"
#include "fatal.h"

#include "hooker.h"
#include "hooker_macros.h"


struct IStream;


/**
 *  Patch in the Vinifera data to be saved in the stream.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Put_All_Vinifera_Data)
{
    GET_REGISTER_STATIC(IStream *, pStm, esi);

    /**
     *  Call to the Vinifera data stream saver.
     */
    if (!Vinifera_Put_All(pStm)) {
        goto failed;
    }

    /**
     *  Stolen bytes/code.
     */
original_code:
    _asm { mov al, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x8 }
    _asm { ret }

failed:
    _asm { xor al, al }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x8 }
    _asm { ret }
}


/**
 *  Patch in the Vinifera data to be loaded in the stream.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Load_All_Vinifera_Data)
{
    GET_REGISTER_STATIC(IStream *, pStm, esi);

    /**
     *  Call to the Vinifera data stream loader.
     */
    if (!Vinifera_Load_All(pStm)) {
        goto failed;
    }

    /**
     *  Stolen bytes/code.
     */
original_code:

    Map.Flag_To_Redraw(2);

    _asm { mov al, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0xB0 }
    _asm { ret }

failed:
    _asm { xor al, al }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x8 }
    _asm { ret }
}


/**
 *  When writing save game info, write our build version.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Save_Game_Put_Game_Version_Ext)
{
    static int version;
    version = ViniferaSaveGameVersion;

    /**
     *  If we are in developer mode, offset the build number as these save
     *  files should not appear in normal game modes.
     * 
     *  For debug builds, we force an offset so they don't appear in any
     *  other builds or releases.
     */
#ifndef NDEBUG
    version *= 3;
#else
    if (Vinifera_DeveloperMode) {
        version *= 2;
    }
#endif
#if defined(TS_CLIENT)
    ver *= 2;
#endif

    _asm { mov edx, version };

    JMP(0x005D5064);
}


/**
 *  Do not allow save games below our current build version.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_LoadOptionsClass_Read_File_Check_Game_Version_Ext)
{
    GET_REGISTER_STATIC(int, version, eax);
    static int ver;

    /**
     *  If the version in the save file does not match our build
     *  version exactly, then don't add this file to the listing.
     */
    ver = ViniferaSaveGameVersion;
#ifndef NDEBUG
    ver *= 3;
#else
    if (Vinifera_DeveloperMode) {
        ver *= 2;
    }
#endif
#if defined(TS_CLIENT)
    ver *= 2;
#endif
    if (version != ver) {
        JMP(0x00505AAD);
    }

    JMP(0x00505ABB);
}


/**
 *  Change the saved module filename to the DLL name. 
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Save_Game_Change_Module_Filename_Ext)
{
    static const char *DLL_NAME = VINIFERA_DLL;
    _asm { push DLL_NAME }

    JMP(0x005D50E2);
}

       
/**
 *  Removes the code which prefixed older save files with "*".
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_LoadOptionsClass_Read_File_Remove_Older_Prefixing_Ext)
{
    JMP(0x00505AE9);
}


void SaveLoadExt_Hooks()
{
    /**
     *  Hook the new save and load system in.
     */
    Patch_Jump(0x005D68F7, &_Put_All_Vinifera_Data);
    Patch_Jump(0x005D78ED, &_Load_All_Vinifera_Data);

    /**
     *  Write Vinifera save files with the current build number.
     */
    Patch_Jump(0x005D505E, &_Save_Game_Put_Game_Version_Ext);

    /**
     *  Change SUN.EXE to our DLL name.
     */
    Patch_Jump(0x005D50DD, &_Save_Game_Change_Module_Filename_Ext);

    /**
     *  Handle save files in the dialogs.
     */
    Patch_Jump(0x00505A9E, &_LoadOptionsClass_Read_File_Check_Game_Version_Ext);
    Patch_Jump(0x00505ABB, &_LoadOptionsClass_Read_File_Remove_Older_Prefixing_Ext);
}
