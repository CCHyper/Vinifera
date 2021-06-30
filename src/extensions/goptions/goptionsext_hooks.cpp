/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          GOPTIONSEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended GameOptionsClass.
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
#include "goptionsext_hooks.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-
 * 
 *  
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_GameOptionsClass_Dialog_Proc_Handle_Esc_Patch)
{
    GET_REGISTER_STATIC(HWND, hDlg, );
    GET_REGISTER_STATIC(UINT, uMsg, );
    GET_REGISTER_STATIC(WPARAM, wParam, );
    GET_REGISTER_STATIC(LPARAM, lParam, );


    *lParam = true;
}


/**
 *  Main function for patching the hooks.
 */
void FootClassExtension_Hooks()
{
    Patch_Jump(0x004B6956, &_GameOptionsClass_Dialog_Proc_Handle_Esc_Patch);
}
