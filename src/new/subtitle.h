/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SUBTITLE.H
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
#include "textprint.h"


class WWFontClass;


/**
 *  The available alignment types.
 */
typedef enum SubTitleAlignType
{
    ALIGN_LEFT = 0,
    ALIGN_RIGHT = 1,
    ALIGN_CENTER = 2,
};


class SubTitle
{
    public:
        SubTitle();
        ~SubTitle();

        void Set_Color(ColorType color);
        void Set_Back_Color(ColorType color);
        void Set_Font(WWFontClass *font);
        void Set_Text(const char *text);

    private:
        void Clear();

    public:

        /**
         *  
         */
        int Time;
        int Duration;
        
        /**
         *  
         */
        ColorType Color;
        ColorType BackColor;

        /**
         *  0 = (min) center of screen, 15 = (max) bottom of screen.
         */
        int YAdjust;
        
        /**
         *  
         */
        SubTitleAlignType XAlignment;

        /**
         *  
         */
        char *TextBuffer;
        
        /**
         *  
         */
        TextPrintType TextStyle;
        
        /**
         *  
         */
        WWFontClass *FontPtr;
};
