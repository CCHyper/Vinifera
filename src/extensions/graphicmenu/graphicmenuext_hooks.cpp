/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FACTORYEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended FactoryClass.
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
#include "factoryext_hooks.h"
#include "tibsun_globals.h"
#include "graphicmenu.h"
#include "theme.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"



/**
 *  
 */
static bool EnteringMenuForFirstTime = false;


DECLARE_PATCH(_NewMenu_Entering_For_First_Time)
{
    EnteringMenuForFirstTime = true;

    _asm { mov eax, 0x004B7780 }
    _asm { mov ecx, ebx }
    _asm { call eax }

    JMP_REG(esi, 0x0058028A);
}


// play theme if not already playing one
DECLARE_PATCH(_GraphicMenu_Process_Loop_Play_Theme_Patch)
{
    GET_REGISTER_STATIC(GraphicMenu *, this_ptr, ebp);
    static ThemeType menu_theme;

    if (/*EnteringMenuForFirstTime || */!Theme.Still_Playing()) {
        menu_theme = Theme.From_Name(this_ptr->Theme);
        Theme.Play_Song(menu_theme);
    }

    JMP_REG(ecx, 0x004B77A2);
}

// kill theme
DECLARE_PATCH(_GraphicMenu_Process_Loop_Quiet_Theme_Patch)
{
    GET_REGISTER_STATIC(GraphicMenu *, this_ptr, ebp);

    if (EnteringMenuForFirstTime) {
        // TODO, only check for exit and TS/FS select
        Theme.Queue_Song(THEME_QUIET);

        //EnteringMenuForFirstTime = false;
    }

    JMP_REG(ecx, 0x004B7A8A);
}


/**
 *  Main function for patching the hooks.
 */
void GraphicMenuExtension_Hooks()
{
    //Patch_Jump(0x004B7789, &_GraphicMenu_Process_Loop_Play_Theme_Patch);
    //Patch_Jump(0x004B7A7E, &_GraphicMenu_Process_Loop_Quiet_Theme_Patch);

    //Patch_Jump(0x00580283, &_NewMenu_Entering_For_First_Time);
}
