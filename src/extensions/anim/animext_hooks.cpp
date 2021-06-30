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
#include "animext_hooks.h"
#include "anim.h"
#include "animtype.h"
#include "animtypeext.h"
#include "voc.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-475
 * 
 *  Implements StopSound for AnimTypes.
 * 
 *  @author: CCHyper
 */
static void Sound_Effect_At_Object(VocType sound, ObjectClass *object)
{
	Sound_Effect(sound, object->Center_Coord());
}

DECLARE_PATCH(_AnimClass_Remove_This_StopSound_Patch)
{
	GET_REGISTER_STATIC(AnimClass *, this_ptr, esi);
	static AnimTypeClass *animtype;
	static AnimTypeClassExtension *animtypeext;

	animtype = this_ptr->Class;

	animtypeext = AnimTypeClassExtensions.find(animtype);
	if (animtypeext) {

		/**
		 *  Has the animation finished playing? StopSound should only
		 * be played if the animation was able to complete.
		 */
		if (!this_ptr->IsPlaying) {

			/**
			 *  Play the StopSound if one has been defined.
			 */
			if (animtypeext->StopSound != VOC_NONE) {
				Sound_Effect_At_Object(animtypeext->StopSound, this_ptr);
			}

		}

	}

	/**
	 *  Stolen bytes/code.
	 */
	this_ptr->ObjectClass::entry_E4();

	JMP_REG(ecx, 0x004167D1);
}


/**
 *  Main function for patching the hooks.
 */
void AnimClassExtension_Hooks()
{
	Patch_Jump(0x004167CA, &_AnimClass_Remove_This_StopSound_Patch);
}
