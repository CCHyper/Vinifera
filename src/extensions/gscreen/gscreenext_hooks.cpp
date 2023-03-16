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
#include "tibsun_globals.h"
#include "rulesext.h"
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
    static long _last_frame = -1;

    /**
     *  #issue-xxx
     * 
     *  Are we still processing the same frame in which we just adjusted?
     *  This gate is required for TibSun as the GScreen input handler is
     *  not tied to the game engine tick, and as a result the screen will
     *  update (shake in this context) at the same rate for the lowest
     *  gamespeed as it would at the highest game speed.
     */
    if (_last_frame == Frame) {
        return;
    }

    /**
     *  Adjust the screen offset based on the desired adjustment.
     * 
     *  #issue-xxx
     * 
     *  Adds support for "ping-pong" style screen shakes (back ported from RA2).
     */
    if (RuleExtension->IsScreenShakePingPong) {

        /**
         *  "Ping-pong" RA2 style.
         */
        if (ScreenX >= 0) {
            if (ScreenX > 0) {
                ScreenX = 1 - ScreenX;
            }
        } else {
            ScreenX = -1 - ScreenX;
        }

        if (ScreenY >= 0) {
            if (ScreenY > 0) {
                ScreenY = 1 - ScreenY;
            }
        } else {
            ScreenY = -1 - ScreenY;
        }

    } else {

        /**
         *  Original TibSun style.
         */
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
     *  Store the frame in which we just shifted.
     */
    _last_frame = Frame;
}


/**
 *  Main function for patching the hooks.
 */
void GScreenClassExtension_Hooks()
{
    Patch_Jump(0x004B9A70, &GScreenClassExt::_AI);
}
