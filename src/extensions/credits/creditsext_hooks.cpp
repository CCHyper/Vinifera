/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CREDITSEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended CreditClass.
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
#include "creditsext_hooks.h"
#include "client_globals.h"
#include "house.h"
#include "houseext.h"
#include "dsurface.h"
#include "textprint.h"
#include "iomap.h"
#include "language.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or deconstructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
class CreditClassFake : public CreditClass
{
    public:
        void _Graphic_Logic(bool forced = false);
};


void CreditClassFake::_Graphic_Logic(bool forced)
{
    /**
     *  In observer mode, we draw the game time rather than the players credits.
     */
    HouseClassExtension *houseext;
    houseext = HouseClassExtensions.find(PlayerPtr);
    if (Client::IsActive && (houseext && houseext->IsObserver)) {

        // Not sure why, but does not always redraw, this forces it.
        if (true /*forced || IsToRedraw*/) {

            TabClass::Draw_Credits_Tab();

            /**
             *  Display the current game time value.
             */

            if (Scen->ElapsedTimer.Is_Active()) {

                long time = Scen->ElapsedTimer;

                /**
                 *  Clamp at 99 hours (should never happen really).
                 */
                if (time/3600 > 99) {
                    time = 359999;
                }
                long hours = time/3600;
                int v0 = 720 * (4 * (time / -3600) - hours) + time;
                long secs = time%60;
                long mins = time/60;
                long msecs = secs/1000; // Not sure?

                //long secs = 720 * (4 * (time / -3600) - time / 3600) + time;
                //long mins = secs / 60;
                //long msecs = secs % 60;

                //long secs = time / 60;
                //long mins = secs / 60;
                //long hours = mins / 60;
                //secs %= 60;
                //mins %= 60;

                //long hours = Scen->ElapsedTimer / TICKS_PER_HOUR;
                //long mins = ((Scen->ElapsedTimer % TICKS_PER_HOUR)/TICKS_PER_MINUTE) % 60;
                //long secs = ((Scen->ElapsedTimer % TICKS_PER_MINUTE)/TICKS_PER_SECOND) % 60;

                Point2D text_pos(SidebarSurface->Get_Width()/2, 1 /*2*/);

                ColorScheme *color = ColorSchemes[0];

                //char const *timeformat = Text_String(TXT_TIME_FORMAT_HOURS); // Text_String(TXT_TIME_FORMAT_NO_HOURS)
                char const *timeformat = "Time: %02d:%02d:%02d";

                Fancy_Text_Print(timeformat,
                    SidebarSurface, &SidebarSurface->Get_Rect(), &text_pos,
                    color, COLOR_TBLACK, TPF_METAL12|TPF_CENTER|TPF_USE_GRAD_PAL, hours, mins, secs);

            }

            IsToRedraw = false;
            IsAudible = false;
        }

    } else {
        
        /**
         *  Call the original Graphic_Logic for normal players.
         */
        CreditClass::Graphic_Logic(forced);

    }
}


/**
 *  Main function for patching the hooks.
 */
void CreditClassExtension_Hooks()
{
    //Patch_Jump(0x00471460, &CreditClassFake::_Graphic_Logic);
}
