/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MISSIONEXT.CPP
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
#include "missionext.h"
#include "mission.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  For adding support for extended MissionClass, we need to also redefine the
 *  mission name array. Do not reorder the original mission list otherwise
 *  this will break the game, add new missions to the end of the list!
 */
const char * MissionClassExtension::Missions[NEW_MISSION_COUNT] = {
    "Sleep",
    "Attack",
    "Move",
    "QMove",
    "Retreat",
    "Guard",
    "Sticky",
    "Enter",
    "Capture",
    "Harvest",
    "Area Guard",
    "Return",
    "Stop",
    "Ambush",
    "Hunt",
    "Unload",
    "Sabotage",
    "Construction",
    "Selling",
    "Repair",
    "Rescue",
    "Missile",
    "Harmless",

    /**
     *  Add new mission names from here on.
     */
};


/**
 *  Fetch mission order from its name.
 * 
 *  @author: CCHyper
 */
MissionType MissionClassExtension::Mission_From_Name(const char *name)
{
    if (name) {
        for (MissionType order = MISSION_FIRST; order < NEW_MISSION_COUNT; ++order) {
            if (stricmp(Missions[order], name) == 0) {
                return order;
            }
        }
    }
    return MISSION_NONE;
}


/**
 *  Converts a mission number into an ASCII string.
 * 
 *  @author: CCHyper
 */
const char *MissionClassExtension::Mission_Name(MissionType mission)
{
    if (mission != MISSION_NONE && mission < NEW_MISSION_COUNT) {
        return Missions[mission];
    }
    return "None";
}


/**
 *  Executes the new missions.
 * 
 *  @author: CCHyper
 */
void MissionClassExtension::Switch(MissionClass *this_ptr)
{
    switch (this_ptr->Mission) {

        /**
         *  Unexpected MissionType.
         */
        default:
            DEV_DEBUG_WARNING("Invalid mission type!\n");
            break;
    };
}

