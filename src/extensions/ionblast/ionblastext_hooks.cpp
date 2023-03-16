/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          IONBLASTEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended IonBlastClass.
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
#include "ionblast.h"
#include "iomap.h"
#include "rulesext.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-xxx
 * 
 *  x
 *
 *  @author: CCHyper
 */
static void IonBlast_Shake_The_Screen(IonBlastClass *this_ptr)
{
    /**
     *  Slight bug fix to make sure screen shakes are not stacked.
     */
    if (Map.ScreenX == 0 && Map.ScreenY == 0) {

        /**
         *  If ion cannon has screen shake values defined, then set the blitter
         *  offset values. GScreenClass::Blit will handle the rest for us.
         */
        if (RuleExtension->IonCannonShakePixelXLo > 0 || RuleExtension->IonCannonShakePixelXHi > 0) {
            Map.ScreenX = Sim_Random_Pick(RuleExtension->IonCannonShakePixelXLo, RuleExtension->IonCannonShakePixelXHi);
        }
        if (RuleExtension->IonCannonShakePixelYLo > 0 || RuleExtension->IonCannonShakePixelYHi > 0) {
            Map.ScreenY = Sim_Random_Pick(RuleExtension->IonCannonShakePixelYLo, RuleExtension->IonCannonShakePixelYHi);
        }

    }
}

DECLARE_PATCH(_IonBlastClass_AI_ShakeScreen_Patch)
{
    GET_REGISTER_STATIC(IonBlastClass *, this_ptr, edi);

    IonBlast_Shake_The_Screen(this_ptr);

    JMP(0x004EDD79);
}


/**
 *  Main function for patching the hooks.
 */
void IonBlastClassExtension_Hooks()
{
    JMP(0x004EDD6F, &_IonBlastClass_AI_ShakeScreen_Patch);
}
