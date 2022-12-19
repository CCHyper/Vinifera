/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MOUSETYPE.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Mouse cursor controls and overrides.
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
 *                 If not, see <http://www.gnu.org/licenses/ }.
 *
 ******************************************************************************/
#include "actiontype.h"
#include "ccini.h"
#include "vinifera_globals.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  These are the ASCII names for the action types.
 */
Wstring ActionTypeClass::ActionName[ACTION_COUNT] = {
    "None",                 // ACTION_NONE
    "Move",                 // ACTION_MOVE
    "NoMove",               // ACTION_NOMOVE
    "Enter",                // ACTION_ENTER
    "Self",                 // ACTION_SELF
    "Attack",               // ACTION_ATTACK
    "Harvest",              // ACTION_HARVEST
    "Select",               // ACTION_SELECT
    "ToggleSelect",         // ACTION_TOGGLE_SELECT
    "Capture",              // ACTION_CAPTURE
    "Repair",               // ACTION_REPAIR
    "Sell",                 // ACTION_SELL
    "SellUnit",             // ACTION_SELL_UNIT
    "NoSell",               // ACTION_NO_SELL
    "NoRepair",             // ACTION_NO_REPAIR
    "Sabotage",             // ACTION_SABOTAGE
    "Tote",                 // ACTION_PARA_BOMB         // Would have been "ParaBomb" early in game development, then "DontUse1". Re-purposed for the carryall and renamed to "Tote".
    "ParaInfantry",         // ACTION_PARA_INFANTRY     // Was "DontUse2"
    "ParaSaboteur",         // ACTION_PARA_SABOTEUR     // Was "DontUse3"
    "Nuke",                 // ACTION_NUKE_BOMB
    "AirStrike",            // ACTION_AIR_STRIKE        // Was "DontUse4"
    "Chronosphere",         // ACTION_CHRONOSPHERE      // Was "DontUse5"
    "Chronosphere2",        // ACTION_CHRONO2           // Was "DontUse6"
    "IronCurtain",          // ACTION_IRON_CURTAIN      // Was "DontUse7"
    "SpyMission",           // ACTION_SPY_MISSION       // Was "DontUse8"
    "GuardArea",            // ACTION_GUARD_AREA
    "Heal",                 // ACTION_HEAL
    "Damage",               // ACTION_DAMAGE
    "GRepair",              // ACTION_GREPAIR
    "NoDeploy",             // ACTION_NO_DEPLOY
    "NoEnter",              // ACTION_NO_ENTER
    "NoGRepair",            // ACTION_NO_GREPAIR
    "TogglePower",          // ACTION_TOGGLE_POWER
    "NoTogglePower",        // ACTION_NO_TOGGLE_POWER
    "EnterTunnel",          // ACTION_ENTER_TUNNEL
    "NoEnterTunnel",        // ACTION_NO_ENTER_TUNNEL
    "EMPulse",              // ACTION_EMPULSE
    "IonCannon",            // ACTION_ION_CANNON
    "EMPulseRange",         // ACTION_EMPULSE_RANGE
    "ChemBomb",             // ACTION_CHEM_BOMB
    "PlaceWaypoint",        // ACTION_PLACE_WAYPOINT
    "NoPlaceWaypoint",      // ACTION_NO_PLACE_WAYPOINT
    "EnterWaypointMode",    // ACTION_ENTER_WAYPOINT_MODE
    "FollowWaypoint",       // ACTION_FOLLOW_WAYPOINT
    "SelectWaypoint",       // ACTION_SELECT_WAYPOINT
    "LoopWaypointPath",     // ACTION_LOOP_WAYPOINT_PATH
    "DragWaypoint",         // ACTION_DRAG_WAYPOINT
    "AttackWaypoint",       // ACTION_ATTACK_WAYPOINT
    "EnterWaypoint",        // ACTION_ENTER_WAYPOINT
    "PatrolWaypoint",       // ACTION_PATROL_WAYPOINT
    "DropPod",              // ACTION_DROP_POD
    "RallyToPoint",         // ACTION_RALLY_TO_POINT
    "AttackSupport"         // ACTION_ATTACK_SUPPORT
};


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
ActionTypeClass::ActionTypeClass(Wstring name) :
    Name(name)
{
    ActionTypes.Add(this);
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
ActionTypeClass::ActionTypeClass(const NoInitClass &noinit)
{
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
ActionTypeClass::~ActionTypeClass()
{
    ActionTypes.Delete(this);
}


/**
 *  Performs one time initialization of the action type class.
 *
 *  @warning: Do not change this function, otherwise it will break support
 *            with the original game!
 *
 *  @author: CCHyper
 */
void ActionTypeClass::One_Time()
{
    /**
     *  Iterate all of the default action types first, creating a class instance for each.
     */
    for (ActionType action = ACTION_FIRST; action < ACTION_COUNT; ++action) {

        Wstring name = ActionName[action];

        if (name == "DontUse1" || name == "Tote") {
            name = "ParaBomb";
        }
        if (name == "DontUse2") {
            name = "ParaInfantry";
        }
        if (name == "DontUse3") {
            name = "ParaSaboteur";
        }
        if (name == "DontUse4") {
            name = "AirStrike";
        }
        if (name == "DontUse5") {
            name = "Chronosphere";
        }
        if (name == "DontUse6") {
            name = "Chronosphere2";
        }
        if (name == "DontUse7") {
            name = "IronCurtain";
        }
        if (name == "DontUse8") {
            name = "SpyMission";
        }

        ActionTypeClass *ptr = new ActionTypeClass(name);
        ASSERT(ptr != nullptr);
    }
}


/**
 *  Reads action object data from an INI file.
 *
 *  @author: CCHyper
 */
bool ActionTypeClass::Read_INI(CCINIClass &ini)
{
    static Wstring const ACTION = "ActionTypes";

    if (!ini.Is_Present(ACTION)) {
        return false;
    }

    int counter = ini.Entry_Count(ACTION);
    for (int index = 0; index < counter; ++index) {
        Wstring entry = ini.Get_Entry(ACTION, index);

        /**
         *  Get a action entry.
         */
        Wstring buf;
        if (ini.Get_String(ACTION, entry, buf)) {

            /**
             *  Find or create a action of the name specified.
             */
            const ActionTypeClass *actiontype = ActionTypeClass::Find_Or_Make(entry);
            if (actiontype) {
                DEV_DEBUG_INFO("Action: Found ActionType \"%s\".\n", buf.Peek_Buffer());
            } else {
                DEV_DEBUG_WARNING("Action: Error processing ActionType \"%s\"!\n", buf.Peek_Buffer());
            }

            char *tok = nullptr;

            tok = std::strtok(buf.Peek_Buffer(), ",");
            ASSERT_FATAL_PRINT(tok != nullptr, "Invalid IsEnabled for ActionType \"%s\"!", buf.Peek_Buffer());
            bool is_enabled = std::strtol(tok, nullptr, 10);

        }

    }

    return counter > 0;
}


/**
 *  Retrieves the TheaterType for action name.
 *
 *  @author: CCHyper
 */
ActionType ActionTypeClass::From_Name(Wstring &name)
{
    ASSERT(name != nullptr);

    if (name == "<none>" || name == "none") {
        return ACTION_NONE;
    }

    if (name != nullptr) {
        for (ActionType index = ACTION_FIRST; index < ActionTypes.Count(); ++index) {
            if (ActionTypes[index]->Name == name) {
                return index;
            }
        }
    }

    return ACTION_NONE;
}


/**
 *  Returns name for given action type.
 *
 *  @author: CCHyper
 */
Wstring ActionTypeClass::Name_From(ActionType type)
{
    return (type != ACTION_NONE && type < ActionTypes.Count() ? ActionTypes[type]->Name : "<none>");
}


/**
 *  Find or create a action of the type specified.
 *
 *  @author: CCHyper
 */
const ActionTypeClass *ActionTypeClass::Find_Or_Make(Wstring &name)
{
    ASSERT(name != nullptr);

    if (name == "<none>" || name == "none") {
        return nullptr;
    }

    for (ActionType index = ACTION_FIRST; index < ActionTypes.Count(); ++index) {
        if (ActionTypes[index]->Name == name) {
            return ActionTypes[index];
        }
    }

    ActionTypeClass *ptr = new ActionTypeClass(name);
    ASSERT(ptr != nullptr);
    return ptr;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
Wstring ActionTypeClass::Internal_Name(Wstring name)
{
    /**
     *  x
     */
    if (name == "DontUse1" || name == "Tote") {
        name = "ParaBomb";
    }
    if (name == "DontUse2") {
        name = "ParaInfantry";
    }
    if (name == "DontUse3") {
        name = "ParaSaboteur";
    }
    if (name == "DontUse4") {
        name = "AirStrike";
    }
    if (name == "DontUse5") {
        name = "Chronosphere";
    }
    if (name == "DontUse6") {
        name = "Chronosphere2";
    }
    if (name == "DontUse7") {
        name = "IronCurtain";
    }
    if (name == "DontUse8") {
        name = "SpyMission";
    }

    return name;
}
