/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SESSIONEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended SessionClass.
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
#include "sessionext_hooks.h"
#include "sessionext_init.h"
#include "sessionext.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


DECLARE_PATCH(_SessionClass_Loading_Callback_Print_Progress_Patch)
{
    GET_REGISTER_STATIC(SessionClass *, this_ptr, ecx);

    /**
     *  Log the progress update.
     */
    GET_STACK_STATIC(int, progress_value, esp, 0x290);
    DEBUG_INFO("Loading Progress: %d\n", progress_value);

    _asm { mov eax, progress_value }    // Restore EAX register
    JMP_REG(ecx, 0x005EF94E);
}


/**
 *  Main function for patching the hooks.
 */
void SessionClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    SessionClassExtension_Init();

    Patch_Jump(0x005EF947, &_SessionClass_Loading_Callback_Print_Progress_Patch);
}
