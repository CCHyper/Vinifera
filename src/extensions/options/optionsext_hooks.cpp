/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPTIONSEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended OptionsClass.
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
#include "optionsext_hooks.h"
#include "optionsext_init.h"
#include "options.h"
#include "optionsext.h"
#include "tibsun_globals.h"
#include "audio_voc.h"
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
static int Options_SoundVolume_To_Int() { return Options.SoundVolume * 255; }
DECLARE_PATCH(_OptionsClass_Set_Sound_Volume_Patch)
{
#ifdef USE_MINIAUDIO
    AudioVocClass::Set_Volume(Options_SoundVolume_To_Int());
#endif

    /**
     *  Stolen bytes/code.
     */
    _asm { pop esi }
    _asm { ret 8 }
}


/**
 *  Main function for patching the hooks.
 */
void OptionsClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    OptionsClassExtension_Init();

    Patch_Jump(0x00589B68, &_OptionsClass_Set_Sound_Volume_Patch);
}
