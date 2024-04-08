
#include "subtitle.h"


SubTitleClass::SubTitleClass() :
    TimeStamp(0),
    Duration(20 * 60),
    RGBColor(0x00FFFFFF),
    LinePosition(15),
    Alignment(Center),
    Caption()
{
}


SubTitleClass::~SubTitleClass()
{
    Caption.Release_Buffer();
}


// Set the color the subtitle caption should be displayed in.
void SubTitleClass::Set_RGB_Color( unsigned char red, unsigned char green, unsigned char blue)
{
    // Combine components as 8:8:8
    RGBColor = (((unsigned long)red << 16) |
                ((unsigned long)green << 8) |
                 (unsigned long)blue);
}


// Set the caption text
void SubTitleClass::Set_Caption(Wstring & string)
{
    // Release existing caption
    Caption.Release_Buffer();

    // Make a copy of caption
    Caption = string;
}
