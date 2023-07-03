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
#include "options.h"
#include "bsurface.h"
#include "drawshape.h"

#include "aircraft.h"
#include "aircrafttype.h"
#include "building.h"
#include "buildingtype.h"
#include "infantry.h"
#include "infantrytype.h"
#include "unit.h"
#include "unittype.h"




static bool ImGui_AssetImage(ObjectTypeClass *objecttype, int shape_number)
{
    if (!objecttype || !objecttype->Get_Image_Data()) {
        return false;
    }

    ShapeFileStruct *shape = objecttype->Get_Image_Data();

    if (shape_number < 0 || shape_number > shape->Get_Frame_Count()) {
        return false;
    }

    ShapeFileFrameStruct *shapeframe = shape->Get_Frame_Data(shape_number);
    Rect shapeframerect = shapeframe->Get_Frame_Dimensions();

    BSurface temp(shapeframerect.Width, shapeframerect.Height, 1, nullptr);

    CC_Draw_Shape(&temp, ObjectDrawer, shape, shape_number, &Point2D(0, 0), &shapeframerect);

    ImVec2 texture_size(shapeframerect.Width, shapeframerect.Height);
    ImTextureID texture = (ImTextureID)D3D11_CreateTextureFromSurface(temp);

    ImGui::Image(texture, texture_size);

    return true;
}


bool DeveloperModeWindowClass::Asset_Browser_Window()
{
    //ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    //ImVec2 window_pos(1350, 0);
    //ImGui::SetNextWindowPos(window_pos);

    //ImVec2 window_size(500, 720);
    //ImGui::SetNextWindowSize(window_size);

    //ImGui::Begin("Asset Browser", nullptr, window_flags);
    ImGui::Begin("Asset Browser");

    void *asset_texture = nullptr;
    int asset_texture_width = -1;
    int asset_texture_height = -1;

    if (ImGui::TreeNode("Aircraft")) {
    }

    if (ImGui::TreeNode("Buildings")) {
        for (int index = 0; index < BuildingTypes.Count(); ++index) {
            BuildingTypeClass *buildingtype = BuildingTypes[index];
            if (!buildingtype) continue;
            if (index == 0) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
            }
            if (ImGui::TreeNode((void*)(intptr_t)index, "%12s [%s]", buildingtype->Name(), buildingtype->Full_Name())) {
                ImGui_AssetImage(buildingtype, 0);
                ImGui::SameLine();
                if (ImGui::SmallButton("Place")) {
                    // todo
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Infantry")) {
    }

    if (ImGui::TreeNode("Overlay")) {
    }

    if (ImGui::TreeNode("Smudges")) {
    }

    if (ImGui::TreeNode("Terrain")) {
    }

    if (ImGui::TreeNode("Units")) {
    }

    if (ImGui::Button("Place")) {
    }

    ImGui::End();

    return true;
}
