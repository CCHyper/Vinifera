/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CLIENT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         
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
#include "client_hooks.h"
#include "client_functions.h"
#include "client_globals.h"
#include "vinifera_util.h"
#include "language.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  inits and starts client.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Main_Game_Select_Game_Before_Main_Loop_Patch)
{
    static bool retval;

    _asm { add esp, 0x10 }

    retval = Client::Init();
    if (!retval) {
        Vinifera_Do_WWMessageBox(TXT_CLIENT_FAILED_TO_INIT, Text_String(TXT_OK));
        retval = false;
        goto return_label;
    }
    
    retval = Client::Start_Game();
    if (!retval) {
        Vinifera_Do_WWMessageBox(TXT_CLIENT_FAILED_TO_START, Text_String(TXT_OK));
        retval = false;
        goto return_label;

    }

return_label:
    _asm { mov al, retval }

    JMP_REG(ecx, 0x004629D6);
}


/**
 *  exits after game end.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Main_Game_Select_Game_After_Main_Loop_Patch)
{
    static bool retval;

    DEBUG_INFO("Client: Exiting game.\n");
    //Client::Shutdown();

    /**
     *  Forces Main_Game to exit.
     */
    retval = false;

    _asm { mov al, retval }

    JMP_REG(ecx, 0x00462B90);
}


/**
 *  Main function for patching the hooks.
 */
void Client_Hooks()
{
#if defined(TS_CLIENT)
    Patch_Jump(0x004629D1, &_Main_Game_Select_Game_Before_Main_Loop_Patch);
    Patch_Jump(0x00462B8B, &_Main_Game_Select_Game_After_Main_Loop_Patch);

    /**
     *  inits house trackers for all game modes.
     */
    Patch_Jump(0x004BAC2C, 0x004BAC39);
#endif
}
