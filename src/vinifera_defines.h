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


typedef enum VesselType
{
    VESSEL_NONE = -1,
    VESSEL_FIRST = 0

} VesselType;
DEFINE_ENUMERATION_OPERATORS(VesselType);


typedef enum NewRTTIType
{
    /**
     *  This offsets the new RTTIType enum so they are correctly numbered.
     */
    NEW_RTTI_PAD = RTTI_VEINHOLEMONSTER, // The last RTTIType

    /**
     *  Add new TActionTypes from here, do not reorder these!
     */

    RTTI_VESSEL,
    RTTI_VESSELTYPE,

    /**
     *  The new total TActionType count.
     */
    NEW_RTTI_COUNT

} NewRTTIType;
DEFINE_ENUMERATION_OPERATORS(NewRTTIType);
