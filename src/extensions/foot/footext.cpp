/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FOOTEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended FootClass class.
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
#pragma once

#include "footext.h"
#include "tibsun_inline.h"
#include "house.h"
#include "housetype.h"
#include "session.h"


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
FootClassExtension::FootClassExtension(const FootClass *this_ptr) :
    TechnoClassExtension(this_ptr)
{
    //if (this_ptr) EXT_DEBUG_TRACE("FootClassExtension::FootClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
FootClassExtension::FootClassExtension(const NoInitClass &noinit) :
    TechnoClassExtension(noinit)
{
    //EXT_DEBUG_TRACE("FootClassExtension::FootClassExtension(NoInitClass) - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
FootClassExtension::~FootClassExtension()
{
    //EXT_DEBUG_TRACE("FootClassExtension::~FootClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT FootClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("FootClassExtension::Load - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = TechnoClassExtension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT FootClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("FootClassExtension::Save - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = TechnoClassExtension::Save(pStm, fClearDirty);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void FootClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("FootClassExtension::Detach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    TechnoClassExtension::Detach(target, all);
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void FootClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("FootClassExtension::Compute_CRC - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    TechnoClassExtension::Compute_CRC(crc);
}


/***********************************************************************************************
 * FootClass::Mission_Timed_Hunt -- This is the AI process for multiplayer computer units.     *
 *                                                                                             *
 * For multiplayer games, the computer AI can't just blitz the human players; the humans       *
 * need a little time to set up their base, or whatever.  This state just waits for            *
 * a certain period of time, then goes into hunt mode.                                         *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with the delay before calling this routine again.                          *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/18/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
/**
 *  This is the AI process for multiplayer computer units.
 * 
 *  @author: CCHyper
 */
int FootClassExtension::Mission_Timed_Hunt()
{
    //EXT_DEBUG_TRACE("FootClassExtension::Mission_Timed_Hunt - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    int rndmax;
    bool changed = false; // has the unit changed into Hunt mode?

    if (!This()->House->IsHuman) {

        /**
         *  Jump into HUNT mode if we're supposed to Blitz, and the EndCountDown
         *  has expired, or if our owning house has lost more than 1/4 of its units
         *  (it gets mad at you)
         */
        if ((Session.MPlayerBlitz && This()->House->BlitzTime == 0) ||
            This()->House->CurUnits < ((This()->House->MaxUnit * 4) / 5)) {
            This()->Assign_Mission(MISSION_HUNT);
            changed = true;
        }

        /**
         *  Jump into HUNT mode on a random die roll; the computer units will periodically
         *  "snap out" of their daze, and begin hunting.  Try to time it so that all
         *  units will be hunting within 10 minutes (600 calls to this routine).
         */
        if (Session.Options.Bases) {
            rndmax = 5000;
        } else {
            rndmax = 1000;
        }

        if (Random_Pick(0, rndmax) == 1) {
            This()->Assign_Mission(MISSION_HUNT);
            changed = true;
        }

        /**
         *  If this unit is still just sitting in Timed Hunt mode, call Guard Area
         *  so it doesn't just sit there stupidly.
         */
        if (!changed) {
            This()->Mission_Guard_Area();
        }
    }

    return TICKS_PER_SECOND + Random_Pick(0, 4); // call me back in 1 second.
}
