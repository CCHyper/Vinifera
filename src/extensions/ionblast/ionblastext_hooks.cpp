/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          INFANTRYEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended InfantryClass.
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
#include "ionblastext_hooks.h"
#include "iomap.h"
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
DECLARE_PATCH(_IonBlastClass_AI_Screen_Shake_Patch)
{
    if (RulesExtension) {

        /**
         *  If this warhead has screen shake values defined, then set the blitter
         *  offset values. GScreenClass::Blit will handle the rest for us.
         */
        if (RulesExtension->IonBlastShakePixelXLo > 0 || RulesExtension->IonBlastShakePixelXHi > 0) {
            Map.ScreenX = Sim_Random_Pick(RulesExtension->IonBlastShakePixelXLo, RulesExtension->IonBlastShakePixelXHi);
        }
        if (RulesExtension->IonBlastShakePixelYLo > 0 || RulesExtension->IonBlastShakePixelYHi > 0) {
            Map.ScreenY = Sim_Random_Pick(RulesExtension->IonBlastShakePixelYLo, RulesExtension->IonBlastShakePixelYHi);
        }

    } else {
        Map.ScreenY = 10;
    }

    JMP(0x004EDD79);
}


/**
 *  Main function for patching the hooks.
 */
void InfantryClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    InfantryClassExtension_Init();

    Patch_Jump(0x004EDD6F, &_IonBlastClass_AI_Screen_Shake_Patch);
}
