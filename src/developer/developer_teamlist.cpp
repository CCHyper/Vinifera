/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_ASSETBROWSER.CPP
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

#include "developer_window.h"
#include "developer_util.h"
#include "tibsun_globals.h"
#include "team.h"
#include "teamtype.h"


bool DeveloperModeWindowClass::Team_List_Window()
{
    //ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    //ImVec2 window_pos(1350, 0);
    //ImGui::SetNextWindowPos(window_pos);

    //ImVec2 window_size(500, 720);
    //ImGui::SetNextWindowSize(window_size);

    //ImGui::Begin("Team List", nullptr, window_flags);
    ImGui::Begin("Team List");

    if (ImGui::TreeNode("Teams")) {

        for (int index = 0; index < Teams.Count(); ++index) {

            TeamClass *team = Teams[index];
            if (!team) continue;

            // Use SetNextItemOpen() so set the default state of a node to be open. We could
            // also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!
            if (index == 0) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            }

            if (ImGui::TreeNode((void*)(intptr_t)index, team->Class->Name())) {
                ImGui::Text("blah blah");
                ImGui::SameLine();
                if (ImGui::SmallButton("button")) {
                }
                ImGui::TreePop();
            }

        }

        ImGui::TreePop();
    }

    if (ImGui::Button("New")) {
    }

    if (ImGui::Button("Edit")) {
    }

    if (ImGui::Button("Delete")) {
    }

    ImGui::End();

    return true;
}
