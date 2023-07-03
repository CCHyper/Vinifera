/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_UTIL.H
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


bool D3D11_ImGui_Initalise(HWND hWnd);
bool D3D11_ImGui_Initalise_D3D();
bool D3D11_ImGui_Create_Context();
bool D3D11_ImGui_Initalise_Fonts();
bool D3D11_ImGui_Initalise_Style();
bool D3D11_ImGui_Initalise_Config();
bool D3D11_ImGui_Initalise_Options();
bool D3D11_ImGui_Shutdown();
bool D3D11_ImGui_New_Frame();
bool D3D11_ImGui_End_Frame();
bool D3D11_ImGui_Main_Draw_Loop();
bool D3D11_ImGui_Render();
bool D3D11_ImGui_Present(bool vsync = false);
