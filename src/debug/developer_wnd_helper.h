/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_WND_HELPER.H
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

class UnitTypeClass;

class RulesClass;
class ScenarioClass;
class SpecialClass;

enum VocType;
enum VQType;

/**
 * 
 *  ImGui helpers
 *
 */

namespace CCImGui
{

void ShowUnitTypePicker(UnitTypeClass ** value, const char * label);
//void ShowBuildingTypePicker(BuildingTypeClass ** value, const char * label);
//void ShowAnimTypePicker(AnimTypeClass ** value, const char * label);
//void ShowVoxelAnimPicker(VoxelAnimTypeClass ** value, const char * label);
//void ShowParticleSysPicker(ParticleSystemTypeClass ** value, const char * label);
//void ShowOverlayTypePicker(OverlayTypeClass ** value, const char * label);
//void ShowWeaponTypePicker(WeaponTypeClass ** value, const char * label);
//void ShowWarheadTypePicker(WarheadTypeClass ** value, const char * label);
//void ShowBulletTypePicker(BulletTypeClass ** value, const char * label);
//void ShowTerrainTypePicker(TerrainTypeClass ** value, const char * label);
void ShowVocPicker(VocType * value, const char * label);
inline void ShowVQPicker(VQType * value, const char * label) {}

//template<typename T> void ShowPointerVector(const char * label, TypeList<T *> * list);
//template<typename T> void ShowIntVector (const char * label, TypeList<int> * list);

template<typename T>
bool SliderScalarAuto(const char * label, T * v, float min = -1000.f, float max = 1000.f)
{
    if constexpr (std::is_same_v<T, float>) {
        return ImGui::SliderFloat(label, v, min, 1000.f, "%.3f");
    } else {
        // double
        double dMin = min;
        double dMax = max;
        return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &dMin, &dMax, "%.3f");
    }
}

void DrawRulesEditor(bool *pOpen);
void DrawScenarioEditor(bool *pOpen);
void DrawSpecialEditor(bool *pOpen);
void DrawGlobalsEditor(bool *pOpen);
void DrawTypesEditor(bool *pOpen);

} // CCImGui namespace end
