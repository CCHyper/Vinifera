/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_WINDOW.H
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


class DeveloperModeWindowClass
{
    public:
        //DeveloperModeWindowClass();
        //~DeveloperModeWindowClass();

        static bool Create_Window();
        static bool Destroy_Window();

        // Temp!
        //static void Callback()
        //{
        //    Window_Message_Handler();
        //    Window_Loop();
        //}

        static bool Game_View_Window();
        static bool Asset_Browser_Window();
        static bool Trigger_List_Window();
        static bool Team_List_Window();

    private:
        static void Window_Message_Handler();
        static void Window_Loop();

    private:
};
