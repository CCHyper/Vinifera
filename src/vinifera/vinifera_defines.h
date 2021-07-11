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
 *  This is the base CLSID for all COM objects. When defining a new COM CLSID,
 *  you must append the two digit hex number, incrementing from the previous.
 */
#define		VINIFERA_BASE_CLSID		"EBE80B85-EED2-4DEF-92CA-BC0C99AF4A00"

/**
 *  CLSID's for all new locomotors.
 */
#define		CLSID_TEST_LOCOMOTOR	"EBE80B85-EED2-4DEF-92CA-BC0C99AF4A01"

/**
 *  CLSID's for all classes.
 */
#define		CLSID_VESSEL			"EBE80B85-EED2-4DEF-92CA-BC0C99AF4A50"
#define		CLSID_VESSEL_TYPE		"EBE80B85-EED2-4DEF-92CA-BC0C99AF4A51"


/**
 *  The naval vessels are enumerated below.
 */
typedef enum VesselType
{
    VESSEL_NONE = -1,
    VESSEL_FIRST = 0

} VesselType;
DEFINE_ENUMERATION_OPERATORS(VesselType);


/**
 *  This is the extended enumeration for RTTIType.
 */
typedef enum ExtRTTIType
{
    /**
     *  This offsets the new RTTIType enum so they are correctly numbered.
     */
    EXT_RTTI_PAD = RTTI_VEINHOLEMONSTER, // The last RTTIType

    /**
     *  Add new RTTITypes from here, do not reorder these!
     */

    EXT_RTTI_VESSEL,
    EXT_RTTI_VESSELTYPE,

    /**
     *  The new total RTTIType count.
     */
    EXT_RTTI_COUNT

} NewRTTIType;
DEFINE_ENUMERATION_OPERATORS(NewRTTIType);

/**
 *  These defines ensure that comparison checks and argument usage pass compiler checks.
 */
#define RTTI_VESSEL         RTTIType(EXT_RTTI_VESSEL)
#define RTTI_VESSELTYPE     RTTIType(EXT_RTTI_VESSELTYPE)
