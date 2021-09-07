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
#include "tpoint.h"
#include "rgb.h"


/**
 *  This structure defines the control of each line trail [only used for ini loading].
 */
typedef struct LineTrailControlStruct
{
    /**
     *  The colour of the line trail.
     */
    RGBClass Color;

    /**
     *  The decrement (fadeout) of the line trail applied every frame. 
     */
    int ColorDecrement;

    /**
     *  
     */
    TPoint2D<int> Position;

} LineTrailControlStruct;


class LineTrailClass
{
    public:
        enum {
            // Maximum line trails per object.
            MAX_LINE_TRAILS = 16
        };

    public:
        LineTrailClass(RGBClass &color, int dec, TPoint2D<int> &pos);
        ~LineTrailClass();

        //void Set_Color(RGBClass &color);
        //void Set_Color_Decrement(int dec);

        static void Draw_All();
        static void Clear_All();

    private:
        void Draw_It();
        void Update();
        bool Is_Something() const;

    private:

        /**
         *  The colour of the line trail.
         */
        RGBClass Color;

        /**
         *  current coord?
         */
        Coordinate Coord;

        /**
         *  ?
         */
        int Decrement;
};
