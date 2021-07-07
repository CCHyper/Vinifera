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
#include "vinifera_util.h"
#include "language.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


#include "tspp.h"
bool &_fade = Make_Global<bool>(0x006F688C);

DEFINE_IMPLEMENTATION(bool Select_Game(bool), 0x004E1DE0);


DECLARE_PATCH(_Main_Game_Select_Game_Before_Main_Loop_Patch)
{
    static bool retval = false;

    _asm { add esp, 0x10 }

    if (Client::IsActive) {

        retval = Client::Init();
        if (!retval) {
            Vinifera_Do_WWMessageBox("Failed to init client game!\n", Text_String(TXT_OK));
            retval = false;
            goto return_label;
        }

        retval = Client::Start_Game();
        if (!retval) {
            Vinifera_Do_WWMessageBox("Failed to start client game!\n", Text_String(TXT_OK));
            retval = false;
            goto return_label;
        }

    } else {
        retval = Select_Game(_fade);
    }

return_label:
    _asm { mov al, retval }

    JMP_REG(ecx, 0x004629D6);
}


DECLARE_PATCH(_Main_Game_Select_Game_After_Main_Loop_Patch)
{
    static bool retval = false;

    if (Client::IsActive && Client::IsExitOnGameFinish) {
        DEBUG_INFO("Client: Exiting game due to request.\n");
        Client::Shutdown();
        retval = false; // Forces exit of Main_Game().

    } else {
        retval = Select_Game(_fade);
    }

    _asm { mov al, retval }

    JMP_REG(ecx, 0x00462B90);
}

#if 0
#include "textprint.h"
#include "tibsun_globals.h"
#include "dsurface.h"
static void Print_Team_Name(HouseClass *house, ColorSchemeType color, TextPrintType style)
{
    ClientPlayerSettingsType *client_player = nullptr;
    if (Client::IsActive) {
        for (int i = 0; i < Client::GameSettings.Players.Count(); ++i) {
            client_player = Client::GameSettings.Players[i];
            if (client_player->HousePtr == house) {
                break;
            }
        }
    }

    Fancy_Text_Print("%s", SidebarSurface, SidebarSurface->Get_Rect(),
        point, color, COLOR_TBLACK, style, Client::Name_From_Team());
}

DECLARE_PATCH(_Radar_Team)
{
    GET_STACK_STATIC(ColorSchemeType, color, esp, 18h);
    GET_REGISTER_STATIC(TextPrintType, style, esi);

    _asm { mov eax, [esp+40h] }
    _asm { add esp, 20h }
    _asm { add ebx, 9 }

    Print_Team_Name(house, color, style);

    JMP_REG(ecx, );
}
#endif


/**
 *  Main function for patching the hooks.
 */
void Client_Hooks()
{
    Patch_Jump(0x004629D1, &_Main_Game_Select_Game_Before_Main_Loop_Patch);
    Patch_Jump(0x00462B8B, &_Main_Game_Select_Game_After_Main_Loop_Patch);
    //Patch_Jump(0x005B994A, &_Radar_Team);
}
