/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          NEWSIDEBAR_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for NewSidebarClass.
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
#include "newsidebar_hooks.h"
#include "newsidebar.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Main function for patching the hooks.
 */
void NewSidebarClassExtension_Hooks()
{
    //Patch_Jump(0x005F2310, &NewSidebarClass::SBGadgetClass::SBGadgetClass);
    //Patch_Jump(0x005F2330, &NewSidebarClass::SidebarClass);
    //Patch_Jump(0x005F2510, &NewSidebarClass::SidebarClass);
    //Patch_Jump(0x005F25E0, &NewSidebarClass_Set_Cameo_Text_5F25E0);
    Patch_Jump(0x005F2610, &NewSidebarClass::One_Time);
    Patch_Jump(0x005F2660, &NewSidebarClass::Init_Clear);
    Patch_Jump(0x005F2720, &NewSidebarClass::Init_IO);
    Patch_Jump(0x005F2900, &NewSidebarClass::Init_For_House);
    //Patch_Jump(0x005F2B00, &NewSidebarClass_reinitpos_5F2B00);
    Patch_Jump(0x005F2C30, &NewSidebarClass::Which_Column);
    Patch_Jump(0x005F2C50, &NewSidebarClass::Factory_Link);
    Patch_Jump(0x005F2CD0, &NewSidebarClass::Activate_Repair);
    Patch_Jump(0x005F2D40, &NewSidebarClass::Activate_Upgrade);
    Patch_Jump(0x005F2DB0, &NewSidebarClass::Activate_Demolish);
    Patch_Jump(0x005F2E20, &NewSidebarClass::Add);
    Patch_Jump(0x005F2E90, &NewSidebarClass::Scroll);
    Patch_Jump(0x005F30F0, &NewSidebarClass::Scroll_Page);
    Patch_Jump(0x005F3560, &NewSidebarClass::Draw_It);
    //Patch_Jump(0x005F38C0, &NewSidebarClass__Blit_Sidebar_5F38C0);
    Patch_Jump(0x005F3C70, &NewSidebarClass::AI);
    Patch_Jump(0x005F3E20, &NewSidebarClass::Recalc);
    Patch_Jump(0x005F3E60, &NewSidebarClass::Activate);
    //Patch_Jump(0x005F4180, &NewSidebarClass::StripClass::StripClass);
    //Patch_Jump(0x005F4210, &NewSidebarClass::StripClass::One_Time);
    //Patch_Jump(0x005F4230, &NewSidebarClass::StripClass::Get_Special_Cameo);
    //Patch_Jump(0x005F4260, &NewSidebarClass::StripClass::Init_Clear);
    //Patch_Jump(0x005F42A0, &NewSidebarClass::StripClass::Init_IO);
    //Patch_Jump(0x005F4450, &NewSidebarClass::StripClass::Activate);
    //Patch_Jump(0x005F4560, &NewSidebarClass::StripClass::Deactivate);
    //Patch_Jump(0x005F4630, &NewSidebarClass::StripClass::Add);
    //Patch_Jump(0x005F46B0, &NewSidebarClass::StripClass::Scroll);
    //Patch_Jump(0x005F4760, &NewSidebarClass::StripClass::Scroll_Page);
    //Patch_Jump(0x005F48F0, &NewSidebarClass::StripClass::Flag_To_Redraw);
    //Patch_Jump(0x005F4910, &NewSidebarClass::StripClass::AI);
    //Patch_Jump(0x005F4E40, &NewSidebarClass::StripClass::Help_Text);
    //Patch_Jump(0x005F4F10, &NewSidebarClass::StripClass::Draw_It);
    //Patch_Jump(0x005F5610, &NewSidebarClass::StripClass::Recalc);
    //Patch_Jump(0x005F5950, &NewSidebarClass::StripClass::SelectClass::SelectClass);
    //Patch_Jump(0x005F5980, &NewSidebarClass::StripClass::SelectClass::Set_Owner);
    //Patch_Jump(0x005F59A0, &NewSidebarClass::StripClass::SelectClass::Action);
    //Patch_Jump(0x005F5EF0, &NewSidebarClass::SBGadgetClass::Action);
    //Patch_Jump(0x005F5F10, &NewSidebarClass::StripClass::Factory_Link);
    Patch_Jump(0x005F5F70, &NewSidebarClass::Abandon_Production);
    //Patch_Jump(0x005F5FB0, &NewSidebarClass::StripClass::Abandon_Production);
    //Patch_Jump(0x005F6030, &NewSidebarClass_Radar_Mode_Control_5F6030);
    Patch_Jump(0x005F6080, &NewSidebarClass::entry_84);
    Patch_Jump(0x005F6620, &NewSidebarClass::Help_Text);
    //Patch_Jump(0x005F6670, &NewSidebarClass__Max_Visible);
    //Patch_Jump(0x005F66E0, &NewPrint_Cameo_Text);
    Patch_Jump(0x005F68A0, &NewSidebarClass::Load);
    Patch_Jump(0x005F68B0, &NewSidebarClass::Save);
    //Patch_Jump(0x005F68C0, &NewSidebarClass::StripClass::SelectClass::`scalar deleting destructor);
    //Patch_Jump(0x005F68E0, &NewSidebarClass::SBGadgetClass::`scalar deleting destructor);
    //Patch_Jump(0x005F6900, &NewStageClass::StageClass);
    //Patch_Jump(0x005F6920, &NewBuildType__BuildType);
}
