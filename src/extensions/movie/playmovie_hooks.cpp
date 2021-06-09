/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          PLAYMOVIE_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks related to Play_Movie and related functions.
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
#include "playmovie_hooks.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"


#if 0
DECLARE_PATCH()
{
    _asm { fld OptionsExtension->MovieVolume }
    _asm { mov esi, 0x }
    _asm { jmp esi }
}

DECLARE_PATCH()
{
    _asm { fld OptionsExtension->MovieVolume }
    _asm { mov esi, 0x }
    _asm { jmp esi }
}

DECLARE_PATCH()
{
    _asm { fld OptionsExtension->MovieVolume }
    _asm { mov esi, 0x }
    _asm { jmp esi }
}

/**
 *  Main function for patching the hooks.
 */
void PlayMovieClassExtension_Hooks()
{
    Patch_Jump(0x00563BC2, &);
    Patch_Jump(0x00563A89, &);
    Patch_Jump(0x005636DD, &);
}
#endif
