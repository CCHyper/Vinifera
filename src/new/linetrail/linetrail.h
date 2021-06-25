/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          LINETRAIL.H
 *
 *  @author        CCHyper
 *
 *  @brief         Graphical line trails for Objects.
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
#include "rgb.h"


class LineTrail
{
    public:
        LineTrail();
        ~LineTrail();

        void Set_Color(RGBClass &color);
        void Set_Color_Decrement(int dec);

        static void Draw_All();
        static void Clear_All();

    private:
        void Draw_It();
        void Update();
        bool Is_Something() const;

    private:
        RGBClass Color;
        LineTrail *Next;
        int ColorDecrement;
        int field_C; // CurrentPixel
        int field_10;
        int field_14;
        int field_18;
        int field_1C[32];
};


struct LineTrailType
{
    /**
        *  This flag controls the colour of the LineTrail of this projectile, which is invoked by UseLineTrail. The values are standard RGB. 
        */
    RGBClass Color;

    /**
        *  If this object has UseLineTrail=yes, then this flag defines the decrement (fadeout) of the Line Trail applied every frame.

If the player is using the Low Detail Level, the value specified in this flag is multiplied by 2. 
        */
    int ColorDecrement;
};
