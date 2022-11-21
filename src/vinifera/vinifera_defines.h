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
 *  If defined, this will replace the SwizzleManagerClass with our own new implementation.
 */
#define VINIFERA_USE_NEW_SWIZZLE_MANAGER 1

/**
 *  Enable debug printing of the swizzle remapping process.
 * 
 *  WARNING: This will take cause the save/load process to take up to 10 minutes!
 */
#ifndef NDEBUG
//#define VINIFERA_ENABLE_SWIZZLE_DEBUG_PRINTING 1
#endif


/**
 *  x
 */
typedef enum ExtBAnimType
{
    BANIM_SUPER_ONE = BANIM_COUNT,
    BANIM_SUPER_TWO,
    BANIM_SUPER_THREE,
    BANIM_SUPER_FOUR,

    /**
     *  The new number of available BAnimType's.
     */
    BANIM_NEW_COUNT,

    /**
     *  Use these when directly accessing the extended anim array.
     */
    EXT_BANIM_SUPER_ONE = 0,
    EXT_BANIM_SUPER_TWO,
    EXT_BANIM_SUPER_THREE,
    EXT_BANIM_SUPER_FOUR,

    EXT_BANIM_COUNT,
};
DEFINE_ENUMERATION_OPERATORS(ExtBAnimType);


/**
 *  This is the base CLSID for all COM objects. When defining a new COM CLSID,
 *  you must append the two digit hex number, incrementing from the previous.
 */
#define		VINIFERA_BASE_CLSID		"EBE80B85-EED2-4DEF-92CA-BC0C99AF4A00"

/**
 *  CLSID's for all new locomotors.
 */
#define		CLSID_TEST_LOCOMOTOR	"EBE80B85-EED2-4DEF-92CA-BC0C99AF4A01"
