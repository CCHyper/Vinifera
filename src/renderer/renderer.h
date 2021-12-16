/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          RENDERER.H
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


class XSurface;


/**
 *  
 */
namespace Renderer
{

};


/**
 *  
 */
namespace Renderer_ImGui
{

extern bool Show;
extern bool BreakThread;

bool Is_Available();

bool Init();
void Shutdown();

void Render_Loop(XSurface *dest_surface);

void Start();
void Process();
void Render(XSurface *dest_surface);
void End();

LRESULT WINAPI Window_Procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};
