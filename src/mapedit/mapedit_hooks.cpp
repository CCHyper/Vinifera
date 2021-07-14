/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ANIMEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended AnimClass.
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
#include "mapedit_hooks.h"
#include "mapedit.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "scenarioini.h"
#include "scenario.h"
#include "iomap.h"
#include "dsurface.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


DECLARE_PATCH(_CellClass_Draw_Tile_Marble_Madness_Patch)
{
    if (MapEditClass::IsMarbleMadnessEnabled) {
    } else {
    }
}


DECLARE_PATCH(_Select_Game_Editor_Start_Scenario_Patch)
{
    if (!Debug_ScenarioFilenameSet) {
        JMP_REG(ecx, 0x004E2D25);
    }

    static bool ret;
    ret = Start_Scenario(Debug_ScenarioFilename, false, CAMPAIGN_NONE);
    _asm { mov al, ret }
    JMP_REG(ecx, 0x004E2D84);
}

DECLARE_PATCH(_Change_Video_Mode_Check_Editor_Patch)
{
    LEA_STACK_STATIC(Rect *, viewrect, esp, 0x28);

    if (Debug_Map) {
        viewrect->X = 0;
        viewrect->Y = 0;
        viewrect->Width = Options.ScreenWidth;
        viewrect->Height = Options.ScreenHeight;
        Map.Set_View_Dimensions(*viewrect);
        Map.Init_IO();
        Map.Activate(0);
        Map.entry_84();
    } else {
        Map.Set_View_Dimensions(*viewrect);
        Map.Init_IO();
        Map.Activate(1);
        Map.entry_84();
    }

    JMP_REG(ecx, 0x0050B161);
}


//DECLARE_PATCH(_MouseClass_Init_IO_Debug_Map_Patch)
//{
//    if (Debug_Map) {
//        MapEditClass::Init_IO();
//    } else {
//        Map.SidebarClass::Init_IO();
//    }
//    _asm { ret }
//}

//DECLARE_PATCH(_SidebarClass_entry_84_Debug_Map_Patch)
//{
//    GET_REGISTER_STATIC(SidebarClass *, this_ptr, ecx);
//    if (Debug_Map) {
//        _asm { add esp, 0x3C }
//        _asm { ret }
//    }
//
//    static SidebarSide sidebarside;
//    sidebarside = Options.SidebarOn;
//    _asm { mov al, sidebarside }
//    _asm { mov ecx, this_ptr }
//    JMP_REG(edx, 0x005F6088);
//}


static void Alloc_Initial_Surfaces()
{
    static int _sidebar_width = 168;

    int tactical_w = Debug_Map ? ScreenRect.Width : (ScreenRect.Width - _sidebar_width);

    Rect hidden(0, 0, tactical_w, ScreenRect.Height);
    Rect comp(0, 0, tactical_w, ScreenRect.Height);
    Rect tile(0, 0, tactical_w, ScreenRect.Height);
    Rect sidebar(0, 0, _sidebar_width, ScreenRect.Height);

    Allocate_Surfaces(&ScreenRect, &comp, &tile, &sidebar);
}

DECLARE_PATCH(_WinMain_Alloc_Surfaces_Patch)
{
    Alloc_Initial_Surfaces();

    JMP_REG(edx, 0x00601548);
}


DECLARE_PATCH(_ObjectClass_Render_Debug_Unshroud_Check_Patch)
{
    if (!MainWindow || !Debug_Unshroud) {
        JMP(0x00585862);
    }

    JMP(0x0058584C)
}

DECLARE_PATCH(_Cell_Draw_Shroud_Fog_Patch)
{
    if (Debug_Unshroud) {
        _asm { ret 0xC }
    }

    __asm _emit 0xA0; // "mov al, _onetime"
    __asm _emit 0xCD;
    __asm _emit 0x0C;
    __asm _emit 0x76;
    __asm _emit 0x00;
    JMP_REG(ecx, 0x00454E65);
}

DECLARE_PATCH(_GScreenClass_Render_Draw_Buttons_Debug_Map_Patch)
{
    if (GScreenClass::Buttons) {
        GScreenClass::Buttons->Draw_All(Debug_Map);
    }
    //if (GScreenClass::Buttons && Debug_Map) {
    //    GScreenClass::Buttons->Draw_All(true);
    //}
    JMP_REG(ecx, 0x004B9667);
}


/**
 *  Main function for patching the hooks.
 */
void MapEditor_Hooks()
{
    Patch_Jump(0x004E2D1C, &_Select_Game_Editor_Start_Scenario_Patch);
    Patch_Jump(0x0050B132, &_Change_Video_Mode_Check_Editor_Patch);
    Patch_Jump(0x006014CC, &_WinMain_Alloc_Surfaces_Patch);

    //Patch_Jump(0x00402B00, &_MouseClass_Init_IO_Debug_Map_Patch);
    //Patch_Jump(0x005F6083, &_SidebarClass_entry_84_Debug_Map_Patch);

    //Patch_Jump(0x00585830, &_ObjectClass_Render_Debug_Unshroud_Check_Patch);
    Patch_Jump(0x00454E60, &_Cell_Draw_Shroud_Fog_Patch);

    Patch_Jump(0x004B9656, &_GScreenClass_Render_Draw_Buttons_Debug_Map_Patch);


    /**
     *  Hook in MapEditClass as the top level layer.
     */
    Change_Virtual_Address(0x006CA768, Get_Func_Address(&MapEditClass::_One_Time));
    Patch_Call(0x004E4838, &MapEditClass::_One_Time);
    Change_Virtual_Address(0x006CA774, Get_Func_Address(&MapEditClass::_Init_IO));
    Change_Virtual_Address(0x006CA77C, Get_Func_Address(&MapEditClass::_AI));
    Change_Virtual_Address(0x006CA790, Get_Func_Address(&MapEditClass::_Draw_It));
    Change_Virtual_Address(0x006CA7B4, Get_Func_Address(&MapEditClass::_Detach));
    Change_Virtual_Address(0x006CA7F0, Get_Func_Address(&MapEditClass::_Scroll_Map));
}
