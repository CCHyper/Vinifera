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
#include "mousetype.h"
#include "ccini.h"
#include "asserthandler.h"
#include "debughandler.h"


MouseTypeClass *MouseTypeHandler = nullptr;


/**
 *  These are the ASCII names for the mouse control types.
 */
const char *MouseTypeClass::MouseNames[MOUSE_COUNT] = {
    "Normal",               // MOUSE_NORMAL

    "ScrollN",              // MOUSE_N
    "ScrollNE",             // MOUSE_NE
    "ScrollE",              // MOUSE_E
    "ScrollSE",             // MOUSE_SE
    "ScrollS",              // MOUSE_S
    "ScrollSW",             // MOUSE_SW
    "ScrollW",              // MOUSE_W
    "ScrollNW",             // MOUSE_NW
    "NoScrollN",            // MOUSE_NO_N
    "NoScrollNE",           // MOUSE_NO_NE
    "NoScrollE",            // MOUSE_NO_E
    "NoScrollSE",           // MOUSE_NO_SE
    "NoScrollS",            // MOUSE_NO_S
    "NoScrollSW",           // MOUSE_NO_SW
    "NoScrollW",            // MOUSE_NO_W
    "NoScrollNW",           // MOUSE_NO_NW

    "CanSelect",            // MOUSE_CAN_SELECT
    "CanMove",              // MOUSE_CAN_MOVE
    "NoMove",               // MOUSE_NO_MOVE
    "StayAttack",           // MOUSE_STAY_ATTACK
    "CanAttack",            // MOUSE_CAN_ATTACK
    "AreaGuard",            // MOUSE_AREA_GUARD
    "Tote",                 // MOUSE_TOTE
    "NoTote",               // MOUSE_NO_TOTE
    "Enter",                // MOUSE_ENTER
    "NoEnter",              // MOUSE_NO_ENTER
    "Deploy",               // MOUSE_DEPLOY
    "NoDeploy",             // MOUSE_NO_DEPLOY
    "Undeploy",             // MOUSE_UNDEPLOY
    "Sell",                 // MOUSE_SELL_BACK
    "SellUnit",             // MOUSE_SELL_UNIT
    "NoSell",               // MOUSE_NO_SELL_BACK
    "GRepair",              // MOUSE_GREPAIR
    "Repair",               // MOUSE_REPAIR
    "NoRepair",             // MOUSE_NO_REPAIR
    "Waypoint",             // MOUSE_WAYPOINT
    "PlaceWaypoint",        // MOUSE_PLACE_WAYPOINT
    "NoPlaceWaypoint",      // MOUSE_NO_PLACE_WAYPOINT
    "SelectWaypoint",       // MOUSE_SELECT_WAYPOINT
    "EnterWaypointMode",    // MOUSE_ENTER_WAYPOINT_MODE
    "FollowWaypoint",       // MOUSE_FOLLOW_WAYPOINT
    "ToteWaypoint",         // MOUSE_WAYPOINT_TOTE
    "RepairWaypoint",       // MOUSE_WAYPOINT_REPAIR
    "AttackWaypoint",       // MOUSE_ATTACK_WAYPOINT
    "EnterWaypoint",        // MOUSE_ENTER_WAYPOINT
    "LoopWaypointPath",     // MOUSE_LOOP_WAYPOINT_PATH
    "AirStrike",            // MOUSE_AIR_STRIKE
    "ChemBomb",             // MOUSE_CHEMBOMB
    "Demolitions",          // MOUSE_DEMOLITIONS
    "NuclearBomb",          // MOUSE_NUCLEAR_BOMB
    "TogglePower",          // MOUSE_TOGGLE_POWER
    "NoTogglePower",        // MOUSE_NO_TOGGLE_POWER
    "Heal",                 // MOUSE_HEAL
    "EMPulse",              // MOUSE_EM_PULSE
    "EMPulseRange",         // MOUSE_EM_PULSE_RANGE

    "ScrollCoast",          // MOUSE_SCROLL_COASTING
    "ScrollCoastN",         // MOUSE_SCROLL_COASTING_N
    "ScrollCoastNE",        // MOUSE_SCROLL_COASTING_NE
    "ScrollCoastE",         // MOUSE_SCROLL_COASTING_E
    "ScrollCoastSE",        // MOUSE_SCROLL_COASTING_SE
    "ScrollCoastS",         // MOUSE_SCROLL_COASTING_S
    "ScrollCoastSW",        // MOUSE_SCROLL_COASTING_SW
    "ScrollCoastW",         // MOUSE_SCROLL_COASTING_W
    "ScrollCoastNW",        // MOUSE_SCROLL_COASTING_NW

    "PatrolWaypoint",       // MOUSE_PATROL_WAYPOINT
};


/**
 *  This array of structures is used to control the mouse animation
 *  sequences.
 */
#define HOTSPOT_LEFT 0
#define HOTSPOT_CENTER 12345
#define HOTSPOT_RIGHT 54321

#define HOTSPOT_TOP 0
#define HOTSPOT_MIDDLE 12345
#define HOTSPOT_BOTTOM 54321

MouseTypeClass::MouseStruct MouseTypeClass::MouseControl[MOUSE_COUNT] = {
    { 0,        1,      0,       1,     HOTSPOT_LEFT,       HOTSPOT_TOP },          // MOUSE_NORMAL,

    { 2,        1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_TOP },          // MOUSE_N,
    { 3,        1,      0,      -1,     HOTSPOT_RIGHT,      HOTSPOT_TOP },          // MOUSE_NE,
    { 4,        1,      0,      -1,     HOTSPOT_RIGHT,      HOTSPOT_MIDDLE },       // MOUSE_E,
    { 5,        1,      0,      -1,     HOTSPOT_RIGHT,      HOTSPOT_BOTTOM },       // MOUSE_SE,
    { 6,        1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_BOTTOM },       // MOUSE_S,
    { 7,        1,      0,      -1,     HOTSPOT_LEFT,       HOTSPOT_BOTTOM },       // MOUSE_SW,
    { 8,        1,      0,      -1,     HOTSPOT_LEFT,       HOTSPOT_MIDDLE },       // MOUSE_W,
    { 9,        1,      0,      -1,     HOTSPOT_LEFT,       HOTSPOT_TOP },          // MOUSE_NW,
    { 10,       1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_TOP },          // MOUSE_NO_N,
    { 11,       1,      0,      -1,     HOTSPOT_RIGHT,      HOTSPOT_TOP },          // MOUSE_NO_NE,
    { 12,       1,      0,      -1,     HOTSPOT_RIGHT,      HOTSPOT_MIDDLE },       // MOUSE_NO_E,
    { 13,       1,      0,      -1,     HOTSPOT_RIGHT,      HOTSPOT_BOTTOM },       // MOUSE_NO_SE,
    { 14,       1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_BOTTOM },       // MOUSE_NO_S,
    { 15,       1,      0,      -1,     HOTSPOT_LEFT,       HOTSPOT_BOTTOM },       // MOUSE_NO_SW,
    { 16,       1,      0,      -1,     HOTSPOT_LEFT,       HOTSPOT_MIDDLE },       // MOUSE_NO_W,
    { 17,       1,      0,      -1,     HOTSPOT_LEFT,       HOTSPOT_TOP },          // MOUSE_NO_NW,

    { 18,      13,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_CAN_SELECT,
    { 31,      10,      4,      42,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_CAN_MOVE,
    { 41,       1,      0,      52,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_NO_MOVE,
    { 53,       5,      4,      63,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_STAY_ATTACK,
    { 58,       5,      4,      63,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_CAN_ATTACK,
    { 68,       5,      4,      73,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_AREA_GUARD,
    { 78,      10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_TOTE,
    { 88,       1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_NO_TOTE,
    { 89,      10,      4,     100,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_ENTER,
    { 99,       1,      0,      63,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_NO_ENTER,
    { 110,      9,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_DEPLOY,
    { 119,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_NO_DEPLOY,
    { 120,      9,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_UNDEPLOY,
    { 129,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SELL_BACK,
    { 139,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SELL_UNIT,
    { 149,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_NO_SELL_BACK,
    { 150,     20,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_GREPAIR,          // Engineer entering friendly building to heal it.
    { 170,     20,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_REPAIR,
    { 190,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_NO_REPAIR,
    { 191,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_WAYPOINT,
    { 201,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_PLACE_WAYPOINT,
    { 211,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_NO_PLACE_WAYPOINT,
    { 212,      7,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SELECT_WAYPOINT,
    { 219,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_ENTER_WAYPOINT_MODE,
    { 229,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_FOLLOW_WAYPOINT,
    { 239,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_WAYPOINT_TOTE,
    { 249,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_WAYPOINT_REPAIR,
    { 259,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_ATTACK_WAYPOINT,
    { 269,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_ENTER_WAYPOINT,
    { 356,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_LOOP_WAYPOINT_PATH,
    { 279,     20,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_AIR_STRIKE,
    { 299,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_CHEMBOMB,
    { 309,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_DEMOLITIONS,
    { 319,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_NUCLEAR_BOMB,
    { 329,     16,      2,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_TOGGLE_POWER,
    { 345,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_NO_TOGGLE_POWER,
    { 346,     10,      4,      42,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_HEAL,
    { 357,     20,      3,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_EM_PULSE,
    { 377,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_EM_PULSE_RANGE,

    { 378,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SCROLL_COASTING,
    { 379,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SCROLL_COASTING_N,
    { 380,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SCROLL_COASTING_NE,
    { 381,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SCROLL_COASTING_E,
    { 382,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SCROLL_COASTING_SE,
    { 383,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SCROLL_COASTING_S,
    { 384,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SCROLL_COASTING_SW,
    { 385,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SCROLL_COASTING_W,
    { 386,      1,      0,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE },       // MOUSE_SCROLL_COASTING_NW,

    { 387,     10,      4,      -1,     HOTSPOT_CENTER,     HOTSPOT_MIDDLE }        // MOUSE_PATROL_WAYPOINT,
};


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
MouseTypeClass::MouseTypeClass()
{
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
MouseTypeClass::MouseTypeClass(const NoInitClass &noinit)
{
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
MouseTypeClass::~MouseTypeClass()
{
}


/**
 *  Process the mouse controls from INI.
 *  
 *  @author: CCHyper
 */
bool MouseTypeClass::Read_INI(CCINIClass &ini)
{
    static char const *const MOUSE = "MouseTypes";

    if (!ini.Is_Present(MOUSE)) {
        return false;
    }

    char buffer[1024];
    char *tok = nullptr;
    int value = 0;

    for (MouseType mouse = MOUSE_NORMAL; mouse < MOUSE_COUNT; ++mouse) {

        /**
         *  x
         */
        if (!ini.Get_String(MOUSE, MouseNames[mouse], buffer, sizeof(buffer))) {
            DEBUG_ERROR("Mouse: Unable to find definition for %s!\n", MouseNames[mouse]);
            return false;
        }

        MouseStruct &mousectrl = MouseControl[mouse];

        tok = std::strtok(buffer, ",");
        mousectrl.StartFrame = std::strtol(tok, nullptr, 10);

        tok = std::strtok(nullptr, ",");
        mousectrl.FrameCount = std::strtol(tok, nullptr, 10);

        tok = std::strtok(nullptr, ",");
        mousectrl.FrameRate = std::strtol(tok, nullptr, 10);

        tok = std::strtok(nullptr, ",");
        mousectrl.SmallFrame = std::strtol(tok, nullptr, 10);

        tok = std::strtok(nullptr, ",");
        if (!strcmpi(tok, "left")) {
            value = HOTSPOT_LEFT;
        } else if (!strcmpi(tok, "center")) {
            value = HOTSPOT_CENTER;
        } else if (!strcmpi(tok, "right")) {
            value = HOTSPOT_RIGHT;
        } else {
            value = std::strtol(tok, nullptr, 10);
        }
        mousectrl.X = value;

        tok = std::strtok(nullptr, ",");
        if (!strcmpi(tok, "top")) {
            value = HOTSPOT_TOP;
        } else if (!strcmpi(tok, "middle")) {
            value = HOTSPOT_MIDDLE;
        } else if (!strcmpi(tok, "bottom")) {
            value = HOTSPOT_BOTTOM;
        } else {
            value = std::strtol(tok, nullptr, 10);
        }
        mousectrl.Y = value;

    }

    return true;
}


#ifndef NDEBUG
/**
 *  Writes out the default mouse control values.
 *
 *  @author: CCHyper
 */
bool MouseTypeClass::Write_Default_INI(CCINIClass &ini)
{
    static char const *const MOUSE = "MouseTypes";

    char buffer[1024];

    for (MouseType mouse = MOUSE_NORMAL; mouse < MOUSE_COUNT; ++mouse) {

        MouseStruct &mousectrl = MouseControl[mouse];

        const char *hotspot_x = nullptr;
        const char *hotspot_y = nullptr;

        switch (mousectrl.X) {
            case HOTSPOT_LEFT:
                hotspot_x = "left";
                break;
            case HOTSPOT_CENTER:
                hotspot_x = "center";
                break;
            case HOTSPOT_RIGHT:
                hotspot_x = "right";
                break;
            default:
                DEBUG_ERROR("Mouse: Invalid hotspot X for %s!\n", MouseNames[mouse]);
                return false;
        };

        switch (mousectrl.Y) {
            case HOTSPOT_TOP:
                hotspot_y = "top";
                break;
            case HOTSPOT_MIDDLE:
                hotspot_y = "middle";
                break;
            case HOTSPOT_BOTTOM:
                hotspot_y = "bottom";
                break;
            default:
                DEBUG_ERROR("Mouse: Invalid hotspot Y for %s!\n", MouseNames[mouse]);
                return false;
        };

        std::snprintf(buffer, sizeof(buffer), "%d,%d,%d,%d,%s,%s",
                                    mousectrl.StartFrame,
                                    mousectrl.FrameCount,
                                    mousectrl.FrameRate,
                                    mousectrl.SmallFrame,
                                    hotspot_x,
                                    hotspot_y);

        ini.Put_String(MOUSE, MouseNames[mouse], buffer);
    }

    return true;
}
#endif
