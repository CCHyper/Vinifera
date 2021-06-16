/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AITRIGGERTYPEEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended AITriggerTypeClass.
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
#include "aitriggertypeext_hooks.h"
#include "aitrigtype.h"
#include "house.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-313
 * 
 *  Patch to fix the AITriggerType checks to consider houses other
 *  than GDI and NOD exist.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AITriggerTypeClass_Process_MultiSide_Patch)
{
    GET_REGISTER_STATIC(AITriggerTypeClass *, this_ptr, esi);
    GET_REGISTER_STATIC(HouseClass *, house, ebp);
    GET_STACK_STATIC(HouseClass *, enemy, esp, 0x18);

    /**
     *  MultiSide of 0 means any side passes.
     */
    if (this_ptr->MultiSide == 0) {
        goto continue_checks;
    }

    /**
     *  Check the processing house side matches the triggers side.
     * 
     *  MultiSide "sides" actually start from "1" (see above) so we need
     *  to adjust it before we compare with the houses ActLike.
     */
    if ((this_ptr->MultiSide-1) != house->ActLike) {
        goto return_false;
    }

#if 0
    /**
     *  Original code.
     */
    if (this_ptr->MultiSide == 1) {
        if (house->ActLike) {
            goto return_false;
        }
    } else if (this_ptr->MultiSide == 2 && house->ActLike != 1) {
        goto return_false;
    }
#endif

    /**
     *  Continue processing the trigger.
     */
continue_checks:
    JMP_REG(edx, 0x00410A1F);

    /**
     *  Return "false" from the function (failed).
     */
return_false:
    JMP_REG(edx, 0x00410A00);
}


/**
 *  Main function for patching the hooks.
 */
void AITriggerTypeClassExtension_Hooks()
{
    Patch_Jump(0x004109EF, &_AITriggerTypeClass_Process_MultiSide_Patch);
}
