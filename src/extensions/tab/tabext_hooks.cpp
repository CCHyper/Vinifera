/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TABEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended TabClass.
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
#include "tabext_hooks.h"
#include "tab.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


DECLARE_PATCH(_zzz)
{
	_asm { mov ecx, TempSurface }
	_asm { mov ecx, [ecx] }
	_asm { mov ecx, [ecx] }

	_asm { push 1 }

	JMP_REG(eax, 0x0060E552);
}


/**
 *  Main function for patching the hooks.
 */
void TabClassExtension_Hooks()
{
	Patch_Byte(0x0060E9CF, 168);
	//Patch_Jump(0x0060E54C, &_zzz);
}
