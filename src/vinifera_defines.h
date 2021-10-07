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


typedef enum NewScriptMissionType
{
    /**
     *  This offsets the new TAction enum so they are correctly numbered.
     */
    NEW_SMISSION_PAD = SMISSION_TALK_BUBBLE, // The last ScriptMissionType

    /**
     *  Add new ScriptMissionType's from here, do not reorder these!
     */

    SMISSION_GATHER_AT_ENEMY,                // Gather at Enemy

    /**
     *  The new total ScriptMissionType count.
     */
    NEW_SMISSION_COUNT
} NewScriptMissionType;
