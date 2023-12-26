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
#include "vinifera_globals.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"



/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or destructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
class AnimTypeClassExt final : public AnimTypeClass
{
    public:
        void _Free_Image();
};


/**
 *  Reimplementation of AnimTypeClass::Free_Image.
 *
 *  @author: CCHyper
 */
void AnimTypeClassExt::_Free_Image()
{
    if (IsDemandLoad && Image) {

        if (IsFreeAfterPlaying) {
            DEV_DEBUG_WARNING("Anim: Freeing loaded image for %s\n", Name());

            /**
             *  The original function would incorrectly try to free memory
             *  that the game does not actually allocate, and as a result of
             *  this, Vinifera's new memory management triggers an assertion
             *  because this is no longer allowed. The original game silently
             *  failed when doing this.
             * 
             *  We now remove this and just correctly nullify the pointer.
             */
             //delete Image;

            Image = nullptr;
        }
    }
}


/**
 *  Write to the debug log when freeing up pre-loaded buildup images.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AnimTypeClass_DTOR_Free_Image_Patch) { GET_REGISTER_STATIC(AnimTypeClass *, this_ptr, esi); this_ptr->Free_Image(); JMP(0x004187F2); }
DECLARE_PATCH(_AnimTypeClass_SDDTOR_Free_Image_Patch) { GET_REGISTER_STATIC(AnimTypeClass *, this_ptr, esi); this_ptr->Free_Image(); JMP(0x00419C22); }


/**
 *  Patches in an assertion check for image data.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AnimTypeClass_Get_Image_Data_Assertion_Patch)
{
    GET_REGISTER_STATIC(AnimTypeClass *, this_ptr, esi);
    GET_REGISTER_STATIC(const ShapeFileStruct *, image, eax);

    if (image == nullptr) {
        DEBUG_WARNING("Anim %s has NULL image data!\n", this_ptr->Name());
    }

    _asm { mov eax, image } // restore eax state.
    _asm { pop esi }
    _asm { add esp, 0x264 }
    _asm { ret }
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

    //Patch_Jump(0x00419B37, &_AnimTypeClass_Get_Image_Data_Assertion_Patch);

    Patch_Jump(0x00419B40, &AnimTypeClassExt::_Free_Image);
    Patch_Jump(0x004187DB, &_AnimTypeClass_DTOR_Free_Image_Patch);
    Patch_Jump(0x00419C0B, &_AnimTypeClass_SDDTOR_Free_Image_Patch);

    /**
     *  Removes any code related to "DemandLoad" if the developer has requested so.
     */
    if (Vinifera_IsAlwaysDemandLoad) {

        // AnimTypeClass::AnimTypeClass()
        //Patch_Byte(0x+1, 0x86); // bl(0) -> al(1)     // #NOTE: Not possible, see AnimTypeClassExtension constructor.

        // OverlayTypeClass::~OverlayTypeClass()
        Patch_Byte_Range(0x0058D164, 0x90, 6);
        Patch_Byte_Range(0x0058D170, 0x90, 2);
        Patch_Byte_Range(0x0058D179, 0x90, 2);
        Patch_Jump(0x0058D17B, 0x0058D18B);

        // OverlayTypeClass::~OverlayTypeClass() (inlined in SDDTOR)
        Patch_Byte_Range(0x0058DC54, 0x90, 6);
        Patch_Byte_Range(0x0058DC60, 0x90, 2);
        Patch_Byte_Range(0x0058DC69, 0x90, 2);
        Patch_Jump(0x0058DC6B, 0x0058DC7B);

        // OverlayTypeClass::Get_Image_Data()
        Patch_Jump(0x0058DB34, 0x0058DB42);

    }
}
