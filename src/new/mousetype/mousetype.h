/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MOUSETYPE.H
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
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

#include "always.h"
#include "iomap.h"


class CCINIClass;
class NoInitClass;


class MouseTypeClass
{
    public:
        MouseTypeClass();
        MouseTypeClass(const NoInitClass &noinit);
        ~MouseTypeClass();

        bool Read_INI(CCINIClass &ini);

#ifndef NDEBUG
        bool Write_Default_INI(CCINIClass &ini);
#endif

    public:
         /**
          *  This type is used to control the frames and rates of the mouse
          *  pointer. Some mouse pointers are actually looping animations.
          */
        typedef struct MouseStruct
        {
            int StartFrame;    // Starting frame number.
            int FrameCount;    // Number of animation frames.
            int FrameRate;     // Frame delay between changing frames.
            int SmallFrame;    // Start frame number for small version (if any).
            int X, Y;          // Hotspot X and Y offset.
        } MouseStruct;

        /**
         *  The control frames and rates for the various mouse pointers are stored
         *  in this static array.
         */
        static MouseStruct MouseControl[MOUSE_COUNT];
        static const char *MouseTypeClass::MouseNames[MOUSE_COUNT];
};

extern MouseTypeClass *MouseTypeHandler;
