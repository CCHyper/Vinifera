/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MISSIONEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended MissionClass class.
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

#include "missionext.h"
#include "tibsun_functions.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
MissionClassExtension::MissionClassExtension(const MissionClass *this_ptr) :
    ObjectClassExtension(this_ptr)
{
    //if (this_ptr) EXT_DEBUG_TRACE("MissionClassExtension::MissionClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
MissionClassExtension::MissionClassExtension(const NoInitClass &noinit) :
    ObjectClassExtension(noinit)
{
    //EXT_DEBUG_TRACE("MissionClassExtension::MissionClassExtension(NoInitClass) - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
MissionClassExtension::~MissionClassExtension()
{
    //EXT_DEBUG_TRACE("MissionClassExtension::~MissionClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT MissionClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("MissionClassExtension::Load - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = ObjectClassExtension::Load(pStm);
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
HRESULT MissionClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("MissionClassExtension::Save - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = ObjectClassExtension::Save(pStm, fClearDirty);
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
void MissionClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("MissionClassExtension::Detach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    ObjectClassExtension::Detach(target, all);
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void MissionClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("MissionClassExtension::Compute_CRC - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    ObjectClassExtension::Compute_CRC(crc);
}


/**
 *  Processes order script.
 *
 *  @author: CCHyper
 */
void MissionClassExtension::AI()
{
    //EXT_DEBUG_TRACE("MissionClassExtension::AI - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    //ASSERT(This()->IsActive);

    This()->ObjectClass::AI();

    if (!This()->IsActive) {
        return;
    }

    /**
     *  This is the script AI equivalent processing.
     */
    if (This()->Timer == 0 && This()->Strength > 0) {
        switch (This()->Mission) {
            default:
                This()->Timer = This()->Mission_Sleep();
                break;

            case MISSION_HARMLESS:
            case MISSION_SLEEP:
                This()->Timer = This()->Mission_Sleep();
                break;

            case MISSION_STICKY:
            case MISSION_GUARD:
                This()->Timer = This()->Mission_Guard();
                break;

            case MISSION_ENTER:
                This()->Timer = This()->Mission_Enter();
                break;

            case MISSION_CONSTRUCTION:
                This()->Timer = This()->Mission_Construction();
                break;

            case MISSION_DECONSTRUCTION:
                This()->Timer = This()->Mission_Deconstruction();
                break;

            case MISSION_CAPTURE:
            case MISSION_SABOTAGE:
                This()->Timer = This()->Mission_Capture();
                break;

            case MISSION_QMOVE:
            case MISSION_MOVE:
                This()->Timer = This()->Mission_Move();
                break;

            case MISSION_ATTACK:
                This()->Timer = This()->Mission_Attack();
                break;

            case MISSION_RETREAT:
                This()->Timer = This()->Mission_Retreat();
                break;

            case MISSION_HARVEST:
                This()->Timer = This()->Mission_Harvest();
                break;

            case MISSION_GUARD_AREA:
                This()->Timer = This()->Mission_Guard_Area();
                break;

            case MISSION_RETURN:
                This()->Timer = This()->Mission_Return();
                break;

            case MISSION_STOP:
                This()->Timer = This()->Mission_Stop();
                break;

            case MISSION_AMBUSH:
                This()->Timer = This()->Mission_Ambush();
                break;

            case MISSION_HUNT:
            case MISSION_RESCUE:
                This()->Timer = This()->Mission_Hunt();
                break;

            case MISSION_UNLOAD:
                This()->Timer = This()->Mission_Unload();
                break;

            case MISSION_REPAIR:
                This()->Timer = This()->Mission_Repair();
                break;

            case MISSION_MISSILE:
                This()->Timer = This()->Mission_Missile();
                break;

            /**
             *
             *  New missions.
             *
             */

            /**
             *  Timed Hunt
             */
            case MISSION_TIMED_HUNT:
                This()->Timer = Mission_Timed_Hunt();
                break;

        }

    }
}


/**
 *  These are the stub routines that handle the mission logic. They do nothing at this
 *  level. Derived classes will override these routine as necessary.
 * 
 *  @return: Returns with the number of game frames to delay before calling this mission
 *           handler again.
 * 
 *  @author: CCHyper
 */
int MissionClassExtension::Mission_Timed_Hunt() { return TICKS_PER_SECOND * 30; }
