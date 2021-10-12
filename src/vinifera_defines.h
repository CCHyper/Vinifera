/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_DEFINES.H
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


/**
 *  Enum for players who spawn at multiplayer starting waypoints.
 */
typedef enum HousesPlayerType
{
    HOUSE_PLAYER_OFFSET = 1000,

    HOUSE_PLAYER_A = HOUSE_PLAYER_OFFSET,    // <Player @ A>
    HOUSE_PLAYER_B,                          // <Player @ B>
    HOUSE_PLAYER_C,                          // <Player @ C>
    HOUSE_PLAYER_D,                          // <Player @ D>
    HOUSE_PLAYER_E,                          // <Player @ E>
    HOUSE_PLAYER_F,                          // <Player @ F>
    HOUSE_PLAYER_G,                          // <Player @ G>
    HOUSE_PLAYER_H,                          // <Player @ H>

    HOUSE_PLAYER_COUNT,                      // For use in loops.
    HOUSE_PLAYER_NUM = 8,                    // Real count

    HOUSE_PLAYER_FIRST = HOUSE_PLAYER_A,
    HOUSE_PLAYER_START = 0,
    HOUSE_PLAYER_NONE = -1
} HousesPlayerType;


/**
 *  Defines and constants.
 */
#define TXT_PLAYER_AT_A "<Player @ A>"
#define TXT_PLAYER_AT_B "<Player @ B>"
#define TXT_PLAYER_AT_C "<Player @ C>"
#define TXT_PLAYER_AT_D "<Player @ D>"
#define TXT_PLAYER_AT_E "<Player @ E>"
#define TXT_PLAYER_AT_F "<Player @ F>"
#define TXT_PLAYER_AT_G "<Player @ G>"
#define TXT_PLAYER_AT_H "<Player @ H>"
