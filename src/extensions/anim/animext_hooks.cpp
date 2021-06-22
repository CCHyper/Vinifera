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
#include "houseext_hooks.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "anim.h"
#include "animtype.h"
#include "scenario.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-114
 * 
 *  Animations that use RandomLoopDelay incorrectly use the unsynchronized
 *  random-number generator to generate their loop delay. This causes such
 *  animations to cause sync errors in multiplayer.
 * 
 *  @author: CCHyper (based on research by Rampastring)
 */
DECLARE_PATCH(_AnimClass_AI_RandomLoop_Randomiser_BugFix_Patch)
{
	GET_REGISTER_STATIC(AnimClass *, this_ptr, esi);
	static AnimTypeClass *animtype;

	animtype = reinterpret_cast<AnimTypeClass *>(this_ptr->Class_Of());

	/**
	 *  Generate a random delay using the network synchronized RNG.
	 */
	this_ptr->Delay = Random_Pick(animtype->RandomLoopDelayMin, animtype->RandomLoopDelayMax);

	/**
	 *  Return from the function.
	 */
function_return:
	JMP(0x00415AF2);
}


/**
 *  Main function for patching the hooks.
 */
void AnimClassExtension_Hooks()
{
	Patch_Jump(0x00415ADA, &_AnimClass_AI_RandomLoop_Randomiser_BugFix_Patch);
}