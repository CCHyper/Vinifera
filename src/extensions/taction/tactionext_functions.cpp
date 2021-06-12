/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TACTIONEXT_FUNCTIONS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the supporting functions for the extended TActionClass.
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
#include "tactionext_functions.h"
#include "vinifera_defines.h"
#include "taction.h"
#include "house.h"
#include "housetype.h"
#include "object.h"
#include "objecttype.h"
#include "trigger.h"
#include "triggertype.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  #issue-158
 * 
 *  Gives credits to the owner of the trigger.
 * 
 *  @author: CCHyper
 */
bool TAction_Give_Credits(TActionClass *taction, HouseClass *house, ObjectClass *object, TriggerClass *trigger, Cell *cell)
{
    if (!taction || !house) {
        return false;
    }

    int amount = taction->Data.Value;

    /**
     *  If positive, grant the cash bonus.
     *  If negative, take money from the house.
     */
    if (amount != 0) {
        if (amount < 0) {
            house->Spend_Money(std::abs(amount));
        } else {
            house->Refund_Money(amount);
        }
    }

    return true;
}


/**
 *  #issue-158
 * 
 *  Give credits to the specified house.
 * 
 *  @author: CCHyper
 */
bool TAction_Give_Credits_To_House(TActionClass *taction, HouseClass *house, ObjectClass *object, TriggerClass *trigger, Cell *cell)
{
    if (!taction || !house) {
        return false;
    }

    int amount = taction->Data.Value;

    /**
     *  We use "Bounds.X" as the second argument, its where the house index is stored.
     */
    HouseClass *hptr = HouseClass::As_Pointer(HousesType(taction->Bounds.X));

    /**
     *  If positive, grant the cash bonus.
     *  If negative, take money from the house.
     */
    if (amount != 0) {
        if (amount < 0) {
            hptr->Spend_Money(std::abs(amount));
        } else {
            hptr->Refund_Money(amount);
        }
    }

    return true;
}
