/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          GSCREENEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended GScreenClass.
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
#include "gscreenext_hooks.h"
#include "gscreen.h"
#include "extension.h"
#include "fatal.h"
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
class GScreenClassExt final : public GScreenClass
{
    public:
        void _AI(KeyNumType &input, Point2D &xy);
};


/**
 *  Reimplementation of GScreenClass::AI.
 * 
 *  @author: CCHyper
 */
void GScreenClassExt::_AI(KeyNumType &input, Point2D &xy)
{
    if (ScreenX >= 0) {
        if (ScreenX > 0) {
            ScreenX = ScreenX - 1;
        }
    } else {
        ScreenX = ScreenX + 1;
    }

    if (ScreenY >= 0) {
        if (ScreenY > 0) {
            ScreenY = ScreenY - 1;
        }
    } else {
        ScreenY = ScreenY + 1;
    }
}


/**
 *  Main function for patching the hooks.
 */
void GScreenClassExtension_Hooks()
{
    Patch_Jump(0x004B9A70, &GScreenClassExt::_AI);
}
