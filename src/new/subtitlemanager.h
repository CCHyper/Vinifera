/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SUBTITLEMANAGER.H
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
#include "vector.h"
#include "rect.h"


class SubTitle;
class XSurface;
class WWFontClass;


class SubTitleManager
{
	public:
		SubTitleManager();
		~SubTitleManager();

		bool Init(const char *videoname);
		bool Update(unsigned time);
		void Set_Drawer(void (*drawer_func)(SubTitle *));
		void Draw(SubTitle *subtitle);
		bool Subtitles_Loaded() { return SubTitles != nullptr && SubTitles->Count() > 0; }

		static SubTitleManager *Create_Subtitles(const char *videoname);

	private:
		DynamicVectorClass<SubTitle *> *SubTitles;
		int CurrentIndex;
		SubTitle *CurrentSubTitle;
		void (*Drawer)(SubTitle *);
		XSurface *SurfacePtr;
		WWFontClass *FontPtr;
		Rect CurrentRect;
		Rect LastRect;
};
