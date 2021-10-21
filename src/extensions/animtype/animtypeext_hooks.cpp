/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ANIMTYPEEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended AnimTypeClass.
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
#include "animtypeext_hooks.h"
#include "animtypeext_init.h"
#include "animtype.h"
#include "animtypeext.h"
#include "supertype.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  #issue-105
 */
DECLARE_PATCH(_)
{
    GET_REGISTER_STATIC(AnimTypeClass *, this_ptr, esi);
    GET_REGISTER_STATIC(const ShapeFileStruct *, image, eax);
    static AnimTypeClassExtension *animtypeext;

    animtypeext = AnimTypeClassExtensions.find(this_ptr);

    if (!this_ptr->Stages) {
        this_ptr->Stages = image->Get_Frame_Count();

        /**
         *  If this animation is flagged that it has a shadow, half the
         *  shape image frame count to find the end of the actual animation
         *  frames.
         */
        if (animtypeext && animtypeext->IsShadow) {
            this_ptr->Stages = image->Get_Frame_Count() / 2;
        }
    }

    if (!this_ptr->LoopEnd) {
        this_ptr->LoopEnd = this_ptr->Stages;
    }


    // Not sure what to do with Biggest...
    //this_ptr->Biggest = image->Get_Frame_Count() / 2;
}


/**
 *  Main function for patching the hooks.
 */
void AnimTypeClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    AnimTypeClassExtension_Init();

    Patch_Jump(0x00418AEF, &);
}
