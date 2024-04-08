
#pragma once

#ifndef SUBTITLE_H
#define SUBTITLE_H

#include "always.h"
#include "wstring.h"


class SubTitleClass
{
    public:
        SubTitleClass();
        ~SubTitleClass();

        // Set the time (in milliseconds) at which the subtitle is to be displayed.
        void Set_Display_Time(unsigned long time) { TimeStamp = time; }
    
        // Retrieve the time in ticks (1/60 seconds) this subtitle is to be displayed.
        unsigned long Get_Display_Time(void) const { return TimeStamp; }

        // Set the time duration in ticks (1/60 seconds) for the subtitle to remain displayed.
        void Set_Display_Duration(unsigned long duration) { Duration = duration; }
    
        // Retrieve the duration time in ticks (1/60 seconds) for the subtitle.
        unsigned long Get_Display_Duration(void) const { return Duration; }

        // Set the color the subtitle caption should be displayed in.
        void Set_RGB_Color(unsigned char red, unsigned char green, unsigned char blue);

        // Retrieve the color of the subtitle
        unsigned long Get_RGB_Color(void) const { return RGBColor; }

        // Set the line position the subtitle should be displayed at.
        void Set_Line_Position(int linePos)
        {
            assert((linePos >= 1) && (linePos <= 15));
            LinePosition = linePos;
        }
    
        // Retrieve the line position to display the subtitle at.
        int Get_Line_Position(void) const { return LinePosition; }

        // Caption justifications
        typedef enum
        {
            Left,
            Right,
            Center,
        } AlignmentEnum;

        // Set the alignment of the subtitle caption
        void Set_Alignment(AlignmentEnum align) { Alignment = align; }

        // Retrieve the caption justification
        AlignmentEnum Get_Alignment(void) const { return Alignment; }

        // Set the caption text
        void Set_Caption(Wstring & string);

        // Retrieve the caption text
        const Wstring & Get_Caption(void) const { return Caption; }

    private:
        unsigned long TimeStamp;
        unsigned long Duration;
        unsigned long RGBColor;
        int LinePosition;
        AlignmentEnum Alignment;
        Wstring Caption;
};

#endif
