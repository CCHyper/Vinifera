/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_GAMEVIEW.CPP
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
#include <imgui.h>
#include <d3d11.h>
#include "d3d_util.h"
#include "developer_window.h"
#include "tibsun_globals.h"
#include "options.h"
#include "bsurface.h"
#include "dsurface.h"





bool DeveloperModeWindowClass::Game_View_Window()
{
    static bool _game_view_in_focus = false;

    ID3D11ShaderResourceView *d3d_texture = D3D11_CreateTextureFromSurface(*PrimarySurface);
    if (!d3d_texture) {
        return false;
    }

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse;

    ImVec2 game_view_pos(0, 0);
    ImGui::SetNextWindowPos(game_view_pos);

    ImGui::Begin("Game View", nullptr, window_flags);

    //ImVec2 game_view_size(Options.ScreenWidth/2, Options.ScreenHeight/2);
    ImVec2 game_view_size(Options.ScreenWidth, Options.ScreenHeight);

    if (ImGui::ImageButton("gameview", (ImTextureID)d3d_texture, game_view_size)) {
        _game_view_in_focus = true;
    }

    if (_game_view_in_focus) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Press [ESC] to unfocus the game view.");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Click in the game view to capture mouse input.");
    }

    ImGui::End();

    // Cleanup memory.
    //d3d_texture->Release();

    return _game_view_in_focus;
}
