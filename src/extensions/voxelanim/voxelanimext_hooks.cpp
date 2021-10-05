/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VOXELANIMEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended VoxelAnimClass.
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
#include "voxelanimext_hooks.h"
#include "voxelanim.h"
#include "voxelanimtype.h"
#include "voxelanimtypeext.h"
#include "tiberium.h"
#include "overlay.h"
#include "cell.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-
 * 
 *  
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_VoxelAnimClass_AI_Tiberium_To_Spawn_Patch)
{
    GET_REGISTER_STATIC(VoxelAnimClass *, this_ptr, ebp);
    GET_REGISTER_STATIC(CellClass *, cell, esi);
    static VoxelAnimTypeClassExtension *voxelanimtypeext;
    static TiberiumClass *tiberium;

    #define OVERLAY_TIB2_01 OverlayType(127)

    voxelanimtypeext = VoxelAnimTypeClassExtensions.find(this_ptr->Class);
    if (voxelanimtypeext && voxelanimtypeext->TiberiumSpawnType != TIBERIUM_NONE) {

        tiberium = (TiberiumClass *)TiberiumClass::As_Pointer(voxelanimtypeext->TiberiumSpawnType);

    } else {

        static TiberiumType tib;
        tib = OverlayClass::To_TiberiumType(OVERLAY_TIB2_01);
        tiberium = Tiberiums[tib];
    }

    _asm { push 0x50 } // for new OverlayClass
    _asm { mov edi, tiberium } // TiberiumClass instance.
    JMP_REG(ecx, 0x0065E9B1);
}


/**
 *  Main function for patching the hooks.
 */
void VoxelAnimClassExtension_Hooks()
{
    Patch_Jump(0x0065E999, &_VoxelAnimClass_AI_Tiberium_To_Spawn_Patch);
}
