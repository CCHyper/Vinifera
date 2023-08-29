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
#include "objectext.h"
#include "object.h"
#include "audio_voc.h"
#include "extension.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  x
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_ObjectClass_Limbo_AmbientSound_Patch)
{
    GET_REGISTER_STATIC(ObjectClass *, this_ptr, esi);
    static ObjectClassExtension *this_ext;

    this_ext = Extension::Fetch<ObjectClassExtension>(this_ptr);

    AudioVocClass::Stop_Audio_Event(this_ext->AmbientSound, this_ptr->Coord);

    //JMP();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_ObjectClass_AI_AmbientSound_Patch)
{
    GET_REGISTER_STATIC(ObjectClass *, this_ptr, esi);
    static ObjectClassExtension *this_ext;

    this_ext = Extension::Fetch<ObjectClassExtension>(this_ptr);

    /**
     *  x
     */
    if (!this_ptr->IsInLimbo) {
        AudioVocClass::Update_Audio_Event(this_ext->AmbientSound, this_ptr->Coord);
    }

    //JMP();
}


/**
 *  Main function for patching the hooks.
 */
void ObjectClassExtension_Hooks()
{
    //Patch_Jump(0x00584C18, &_ObjectClass_AI_AmbientSound_Patch);
}
