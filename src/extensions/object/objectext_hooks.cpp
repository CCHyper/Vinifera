/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OBJECTEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended ObjectClass.
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
#include "objectext_hooks.h"
#include "objectext_init.h"
#include "objectext.h"
#include "object.h"
#include "objecttype.h"
#include "objecttypeext.h"
#include "linetrail.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


DECLARE_PATCH(_ObjectClass_Unlimbo_Create_Line_Trail_Patch)
{
    GET_REGISTER_STATIC(ObjectClass *, this_ptr, esi);
    static ObjectClassExtension *objectext;

    objectext = ObjectClassExtensions.find(this_ptr);

    /**
     *  
     */
    if (objectext) {
        objectext->Create_Line_Trails();
    }

    _asm { mov al, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x34 }
    _asm { ret 8 }
}


/**
 *  Main function for patching the hooks.
 */
void ObjectClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    ObjectClassExtension_Init();

    Patch_Jump(0x00585ECB, &_ObjectClass_Unlimbo_Create_Line_Trail_Patch);
}
