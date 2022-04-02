/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          RENDERER_SUPPORT.H
 *
 *  @author        CCHyper
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
#include "wstring.h"


class VideoDriver
{
    public:
        VideoDriver() {}
        virtual ~VideoDriver() {}

        virtual bool Create_Window(Wstring window_title, int width, int height, bool windowed = false, bool borderless = false) = 0;
        virtual void Close_Window(bool force = false) = 0;
        virtual void Destroy_Window(bool force = false) = 0;
        virtual void Show_Window() = 0;
        virtual void Hide_Window() = 0;
        virtual void Minimize_Window()  = 0;
        virtual void Maximize_Window()  = 0;
        virtual bool Toggle_Fullscreen() = 0;
};


void __cdecl Set_Video_Driver(VideoDriver *driver);
void __cdecl Remove_Video_Driver();

VideoDriver * __cdecl Video_Driver();

bool Video_Driver_Is_Direct_Draw();
