/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DDRAW_RENDERER.H
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
#include "video_driver.h"


class DirectDrawVideoDriver : public VideoDriver
{
    public:
        DirectDrawVideoDriver() : VideoDriver("DirectDraw") {}
        virtual ~DirectDrawVideoDriver() {}

        virtual bool Create_Window(Wstring window_title, int width, int height, int refresh_rate = 60, bool windowed = false, bool borderless = false) override;
        virtual void Close_Window(bool force = false) override;
        virtual void Destroy_Window(bool force = false) override;
        virtual void Show_Window() override;
        virtual void Hide_Window() override;
        virtual void Minimize_Window() override;
        virtual void Maximize_Window() override;
        virtual bool Toggle_Fullscreen() override;

    private:

    private:
};
