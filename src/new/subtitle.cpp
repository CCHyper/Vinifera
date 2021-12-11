/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SUBTITLE.CPP
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
#include "subtitle.h"
#include "dsurface.h"
#include "wwfont.h"


SubTitle::SubTitle() :
	Time(0),
	Duration(1200),
	Color(COLOR_WHITE),
	BackColor(COLOR_BLACK),
	YAdjust(15),
	XAlignment(ALIGN_CENTER),
	TextBuffer(nullptr),
	TextStyle(TPF_METAL12|TPF_NOSHADOW)
{
	FontPtr = Font_Ptr(TextStyle);
}


SubTitle::~SubTitle()
{
	Clear();
}


void SubTitle::Set_Color(ColorType color)
{
	Color = color;
}


void SubTitle::Set_Back_Color(ColorType color)
{
	BackColor = color;
}


void SubTitle::Set_Font(WWFontClass *font)
{
	FontPtr = font;
}


void SubTitle::Set_Text(const char *text)
{
	Clear();

	if (text) {
		TextBuffer = new char [std::strlen(text)+1];
		if (TextBuffer) {
			std::strcpy(TextBuffer, text);
		}
	}
}


void SubTitle::Clear()
{
	delete TextBuffer;
	TextBuffer = nullptr;
}
