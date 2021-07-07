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
#include "tibsun_globals.h"
#include "client_globals.h"
#include "iomap.h"
#include "house.h"
#include "housetype.h"
#include "houseext.h"
#include "session.h"
#include "multiscore.h"
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

    if (Client::IsActive
        && (Client::IsExitOnGameFinish || (Session.Type == GAME_NORMAL && Client::GameSettings.IsOneTimeOnly))) {

        DEBUG_INFO("Client: Exiting game due to request of the client.\n");
        Client::Shutdown();
        retval = false;         // Forces exit of Main_Game().

    } else {
        retval = Select_Game(_fade);
    }

    _asm { mov al, retval }

    JMP_REG(ecx, 0x00462B90);
}


// TODO, move to houseext_hooks!
DECLARE_PATCH(_HouseClass_Init_Trackers_Patch)
{
    if (Session.Type == GAME_INTERNET || Client::IsActive) {
        goto init_trackers;
    }

skip_trackers:
    JMP(0x004BADB0);

init_trackers:
    JMP(0x004BAC39);
}


#if 0
// TODO, move to houseext_hooks!
DECLARE_PATCH(_HouseClass_Tiberium_Harvested_Store_AI_Harvested_Patch)
{
    GET_REGISTER_STATIC(HouseClass *, this_ptr, edi);
    GET_REGISTER_STATIC(int, tib_value, ecx)

    if (Client::IsActive) {
        this_ptr->HarvestedCredits += tib_value;
    }
}
#endif


// TODO, move to houseext_hooks!
// Removes the "Game Over" text on game end for observers.
DECLARE_PATCH(_HouseClass_Flag_To_Lose_Game_Over_Text_Patch)
{
    GET_REGISTER_STATIC(HouseClass *, this_ptr, ebp);

    static HouseClassExtension *houseext;

    houseext = HouseClassExtensions.find(PlayerPtr);
    if (this_ptr == PlayerPtr && (houseext && !houseext->IsObserver)) {
        goto show_game_over_text;
    }

skip_text:
    JMP_REG(ecx, 0x004BFF9B);

show_game_over_text:
    JMP_REG(ecx, 0x004BFF76);
}


// TODO, move to houseext_hooks!
DECLARE_PATCH(_HouseClass_MPlayer_Defeated_You_Have_Lost_Patch)
{
    GET_REGISTER_STATIC(HouseClass *, this_ptr, ebx);
    static HouseClassExtension *houseext;

    /**
     *  Stolen bytes/code.
     */
    Map.Flag_To_Redraw(2);

    houseext = HouseClassExtensions.find(PlayerPtr);
    if (houseext && houseext->IsObserver) {
        goto gscreen_redraw;
    }

show_you_have_lost:
    JMP(0x004BF5F3);

gscreen_redraw:
    _asm { lea esi, [ebx+0x10DE4] } // this->IniName
    JMP_REG(ecx, 0x004BF653);
}


// TODO, move to houseext_hooks!
DECLARE_PATCH(_HouseClass_MPlayer_Defeated_Player_Defeated_Patch)
{
    GET_REGISTER_STATIC(HouseClass *, this_ptr, ebx);
    static HouseClassExtension *houseext;

    if (!this_ptr->Class->IsMultiplayPassive) {

        DEBUG_INFO("MPlayer_Defeated: Frame %d, House %d.\n", Frame, this_ptr->ID);

        houseext = HouseClassExtensions.find(PlayerPtr);
        if (houseext && houseext->IsObserver) {
            goto gscreen_redraw;
        }

    }

show_player_defeated:
    JMP(0x004BF676);

gscreen_redraw:
    JMP_REG(ecx, 0x004BF6D7);
}


// Send stats patch to skip sending for observers.

// dont show observer in radar.

// dont show observer on score screen


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


DECLARE_PATCH(_HouseClass_Flag_To_Win_Ignore_Observer)
{
}


DECLARE_PATCH(_HouseClass_Flag_To_Lose_Ignore_Observer)
{
}


DECLARE_PATCH(_HouseClass_Flag_To_Die_Ignore_Observer)
{
}


DECLARE_PATCH(_HouseClass_AI_Skip_Defeated_Check_For_Observer_Patch)
{
    GET_REGISTER_STATIC(HouseClass *, this_ptr, esi);
    static HouseClassExtension *houseext;

    houseext = HouseClassExtensions.find(this_ptr);
    if (houseext && houseext->IsObserver) {
        //DEV_DEBUG_INFO("HouseClass::AI: Skipped \"%s\" as they are an observer.\n", this_ptr->IniName);
        goto skip_house;
    }

    if (this_ptr->IsDefeated) {
        goto skip_house;
    }

continue_check:
    JMP(0x004BCEB2);

skip_house:
    JMP(0x004BCF6E);
}


/**
 *  Main function for patching the hooks.
 */
void Client_Hooks()
{
    Patch_Jump(0x004629D1, &_Main_Game_Select_Game_Before_Main_Loop_Patch);
    Patch_Jump(0x00462B8B, &_Main_Game_Select_Game_After_Main_Loop_Patch);
    //Patch_Jump(0x005B994A, &_Radar_Team);
    Patch_Jump(0x004BAC2C, &_HouseClass_Init_Trackers_Patch);
    Patch_Jump(0x004BFF69, &_HouseClass_Flag_To_Lose_Game_Over_Text_Patch);
    Patch_Jump(0x004BF5E7, &_HouseClass_MPlayer_Defeated_You_Have_Lost_Patch);
    Patch_Jump(0x004BF669, &_HouseClass_MPlayer_Defeated_Player_Defeated_Patch);
    Patch_Jump(0x004BCEA4, &_HouseClass_AI_Skip_Defeated_Check_For_Observer_Patch);

    //Patch_Jump(0x004BD5B0, 0x004BD5EF); // makes AI houses store harvested.   ??
}
