/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          PIXELFX.H
 *
 *  @author        CCHyper, tomsons26
 *
 *  @brief         
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
#include "wstring.h"


class CCINIClass;


/**
 *  Pixel effect shown over cells with color ramp.
 * 
 *  #NOTE: This effect MUST use the non-critical random number as it is a
 *  client side effect only and must be alter the network sync'd random number!
 */
class PixelFXClass
{
    private:
        typedef enum PixelFXType
        {
            FX_WATER,
            FX_TIBERIUM1, // Riparius
            FX_TIBERIUM2, // Cruentus
            FX_TIBERIUM3, // Vinifera
            FX_TIBERIUM4, // Aboreus

            FX_COUNT,

            FX_NONE = -1,
        } PixelFXType;

    public:
        PixelFXClass(/*PixelFXType fx_type*/);
        ~PixelFXClass();

        void Init(/*PixelFXType fx_type*/);

        void Update(int time);
        bool Is_To_Update(int time);

        void Set_Offset(Point2D &pos) { Offset = pos; }

        const RGBClass &Get_Color() const { return CurrentColor; }
        const Point2D &Get_Offset() const { return Offset; }
        unsigned Get_BitMask() const { return BitMask; }

        /*static*/ void Get_Type_Colors(/*PixelFXType fx_type,*/ RGBClass &min, RGBClass &max);
        /*static*/ void Set_Type_Colors(/*PixelFXType fx_type,*/ RGBClass &min, RGBClass &max);
        
        void Draw_It();

        bool Read_INI(CCINIClass &ini);

        static void Process(CCINIClass &ini);

        static void Draw_All();
        static void Clear_All();

    private:
        /**
         *  
         */
        Wstring Name;

        /**
         *  What effect type are we?
         */
        //PixelFXType FXType;

        /**
         *  This struct contains the color and animation info for each effect.
         */
        RGBClass Max;
        RGBClass Min;
        unsigned field_18; // rgb max offset?
        unsigned field_1C; // update delay in ms?
        unsigned field_20;
        unsigned field_24;

        /**
         *  The current color to plot.
         */
        RGBClass CurrentColor;

        unsigned BitMask;
        unsigned BitMaskOffset;

        RGBClass MaxRGB;
        RGBClass WorkingRGB;

        Point2D Offset;

        /**
         *  Current time value in milliseconds.
         */
        unsigned CurrentTime;
};
