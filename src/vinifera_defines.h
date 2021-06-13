/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_GLOBALS.H
 *
 *  @authors       CCHyper
 *
 *  @brief         Vinifera defines and constants.
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

#include "always.h"
#include "tibsun_defines.h"


typedef enum NewTActionType
{
    /**
     *  This offsets the new TAction enum so they are correctly numbered.
     */
    NEW_TACTION_PAD = TACTION_TALK_BUBBLE, // The last TActionType

    /**
     *  Add new TActionTypes from here, do not reorder these!
     */

    TACTION_CREDITS,                 // Gives credits to the owner of the trigger.
    TACTION_CREDITS_HOUSE,           // Gives credits to the specified house.
    TACTION_RESHROUD_MAP_AT,         // RA2 - Reshroud map at waypoint.
    TACTION_SET_TECH_LEVEL,          // RA2 - Set tech level of Techno.
    TACTION_CREATE_CRATE,            // RA2 - Create goodie crate.
    TACTION_PAUSE_GAME_FOR,          // RA2 - Pause game for specified seconds.
    TACTION_JUMP_WAYPOINT,           // RA2 - Center (Jump) camera at waypoint.
    TACTION_PLAY_MOVIE_PAUSE,        // YR - Play an ingame movie (pause game).
    TACTION_CLEAR_SMUDGES,           // YR - Clear all smudges.
    TACTION_DESTROY_ALL_OF,          // YR - Destroy all of.
    TACTION_DESTROY_BUILDINGS,       // YR - Destroy all Buildings of.
    TACTION_DESTROY_UNITS,           // YR - Destroy all Units of.
    TACTION_CREATE_BUILDING,         // YR - Create Building At.
    TACTION_SET_SPECIAL_CHARGE,      // YR - Set superweapon charge.
    TACTION_FLASH_BUILDINGS,         // YR - Flash buildings of specified type.
    TACTION_SET_SPECIAL_RECHARGE,    // YR - Set superweapon recharge time.
    TACTION_RESET_SPECIAL_RECHARGE,  // YR - Reset superweapon recharge time.
    TACTION_SPECIAL_RESET,           // YR - Reset superweapon.
    TACTION_PREFERRED_TARGET_SET,    // YR - Set preferred target.
    TACTION_PREFERRED_TARGET_CLEAR,  // YR - Clear preferred target.
    TACTION_CENTER_BASE_SET,         // YR - Center Base Cell Set.
    TACTION_CENTER_BASE_CLEAR,       // YR - Center Base Cell Clear.
    TACTION_BLACKOUT_RADAR,          // YR - Blackout Radar.
    TACTION_RETINT_RED,              // YR - Retint Red.
    TACTION_RETINT_GREEN,            // YR - Retint Green.
    TACTION_RETINT_BLUE,             // YR - Retint Blue.
    TACTION_JUMP_CAMERA_HOME,        // YR - Jump camera home.

    /**
     *  The new total TActionType count.
     */
    NEW_TACTION_COUNT
};
