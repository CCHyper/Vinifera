/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_WND_HELPER.CPP
 *
 *  @author        CCHyper, and AI used for mass duplication
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

#include "developer_wnd_helper.h"
#include <imgui.h>

#include "tibsun_globals.h"
#include "unittype.h"
#include "rules.h"
#include "scenario.h"


/**
 * 
 * Utility
 * 
 */
namespace ccstd {

template<typename T>
std::string to_string(T value)
{
    char buffer[32];
    if constexpr (std::is_same_v<T, int>)
        snprintf(buffer, sizeof(buffer), "%d", value);
    else if constexpr (std::is_same_v<T, unsigned int>)
        snprintf(buffer, sizeof(buffer), "%u", value);
    else if constexpr (std::is_same_v<T, float>)
        snprintf(buffer, sizeof(buffer), "%.3f", value);
    else if constexpr (std::is_same_v<T, double>)
        snprintf(buffer, sizeof(buffer), "%.3f", value);
    else if constexpr (std::is_same_v<T, size_t>)
        snprintf(buffer, sizeof(buffer), "%zu", value);
    else
        static_assert(sizeof(T) == 0, "Unsupported type in to_string_fallback");

    return std::string(buffer);
}

} // ccstd namespace end


/**
 * 
 * Pickers
 * 
 */

void CCImGui::ShowUnitTypePicker(UnitTypeClass ** value, const char * label)
{
    // Display current selection as a button
    const char * currentName = (*value)->IniName;
    if (ImGui::Button(currentName, ImVec2(180, 0))) {
        ImGui::OpenPopup(("##pick_" + std::string(label)).c_str());
    }

    // Same line label
    ImGui::SameLine();
    ImGui::TextUnformatted(label);

    // Popup listing all UnitTypes
    if (ImGui::BeginPopup(("##pick_" + std::string(label)).c_str())) {

        if (ImGui::Selectable("<none>", *value == nullptr)) {
            *value = nullptr;
        }

        //  Enumerate global list (replace with your container)
        for (int i = 0; i < UnitTypes.Count(); ++i) {

            UnitTypeClass* ut = UnitTypes[i];
            if (!ut) {
                continue;
            }

            bool selected = (ut == *value);
            if (ImGui::Selectable(ut->IniName, selected)) {
                *value = ut;
            }

            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndPopup();
    }
}

/**
 * 
 * Windows
 * 
 */
void CCImGui::DrawRulesEditor(bool *pOpen)
{
    // Don't continue if flag pointer is NULL
    if (!pOpen) {
        return;
    }

    // Open a window with a close button in the corner
    if (!ImGui::Begin("Rules Editor", pOpen)) {
        ImGui::End(); // required to balance ImGui::Begin()
        return;
    }

    /*  GROUP: simple fields */
    if (ImGui::CollapsingHeader("Basic Flags & Values", ImGuiTreeNodeFlags_DefaultOpen)) {

        /* --- ints --- */
        ImGui::InputInt("AmmoCrateDamage",               &Rule->AmmoCrateDamage);
        ImGui::InputInt("AttackingAircraftSightRange",   &Rule->AttackingAircraftSightRange);
        ImGui::InputInt("WaypointAnimationSpeed",        &Rule->WaypointAnimationSpeed);
        ImGui::InputInt("FlashFrameTime",                &Rule->FlashFrameTime);
        ImGui::InputInt("RadarCombatFlashTime",          &Rule->RadarCombatFlashTime);
        ImGui::InputInt("MaxWaypointPathLength",         &Rule->MaxWaypointPathLength);
        ImGui::InputInt("VeinholeMonsterStrength",       &Rule->VeinholeMonsterStrength);
        ImGui::InputInt("MaxVeinholeGrowth",             &Rule->MaxVeinholeGrowth);
        ImGui::InputInt("VeinholeGrowthRate",            &Rule->VeinholeGrowthRate);
        ImGui::InputInt("VeinholeShrinkRate",            &Rule->VeinholeShrinkRate);
        ImGui::InputInt("VeinDamage",                    &Rule->VeinDamage);
        ImGui::InputInt("MaximumQueuedObjects",          &Rule->MaximumQueuedObjects);
        ImGui::InputInt("AircraftFogReveal",             &Rule->AircraftFogReveal);
        ImGui::InputInt("BridgeVoxelMax",                &Rule->BridgeVoxelMax);
        ImGui::InputInt("CloakingStages",                &Rule->CloakingStages);
        ImGui::InputInt("RevealTriggerRadius",           &Rule->RevealTriggerRadius);
        ImGui::InputInt("SpotlightMovementRadius",       &Rule->SpotlightMovementRadius);
        ImGui::InputInt("SpotlightLocationRadius",       &Rule->SpotlightLocationRadius);
        ImGui::InputInt("SpotlightRadius",               &Rule->SpotlightRadius);

        /* --- bools --- */
        ImGui::Checkbox("IsFreeMCV",                     &Rule->IsFreeMCV);
        ImGui::Checkbox("IsBerzerkAllowed",              &Rule->IsBerzerkAllowed);
        ImGui::Checkbox("IsComputerParanoid",            &Rule->IsComputerParanoid);
        ImGui::Checkbox("IsCurleyShuffle",               &Rule->IsCurleyShuffle);
        ImGui::Checkbox("IsBlendedFog",                  &Rule->IsBlendedFog);
        ImGui::Checkbox("IsCompEasyBonus",               &Rule->IsCompEasyBonus);
        ImGui::Checkbox("IsFineDifficulty",              &Rule->IsFineDifficulty);
        ImGui::Checkbox("IsExplosiveHarvester",          &Rule->IsExplosiveHarvester);
        ImGui::Checkbox("IsHealthBar",                   &Rule->IsHealthBar);
        ImGui::Checkbox("IsAllyReveal",                  &Rule->IsAllyReveal);
        ImGui::Checkbox("IsSeparate",                    &Rule->IsSeparate);
        /* …add remaining bools the same way … */
    }

    /*  GROUP: floats & doubles */
    if (ImGui::CollapsingHeader("Rates / Coefficients"))
    {
        SliderScalarAuto("TunnelSpeed",                      &Rule->TunnelSpeed);
        SliderScalarAuto("TiberiumHeal",                     &Rule->TiberiumHeal);
        SliderScalarAuto("RadarEventColorSpeed",             &Rule->RadarEventColorSpeed);
        SliderScalarAuto("RadarEventSpeed",                  &Rule->RadarEventSpeed);
        SliderScalarAuto("RadarEventRotationSpeed",          &Rule->RadarEventRotationSpeed);
        SliderScalarAuto("AITriggerSuccessWeightDelta",      &Rule->AITriggerSuccessWeightDelta);
        SliderScalarAuto("AITriggerFailureWeightDelta",      &Rule->AITriggerFailureWeightDelta);
        SliderScalarAuto("AITriggerTrackRecordCoefficient",  &Rule->AITriggerTrackRecordCoefficient);
        SliderScalarAuto("JumpjetClimb",                     &Rule->JumpjetClimb);
        SliderScalarAuto("JumpjetAcceleration",              &Rule->JumpjetAcceleration);
        SliderScalarAuto("JumpjetWobblesPerSecond",          &Rule->JumpjetWobblesPerSecond);
        SliderScalarAuto("ZoomInFactor",                     &Rule->ZoomInFactor);
        /* …continue for every float/double… */
    }

    /*  GROUP: pointer pickers */
    if (ImGui::CollapsingHeader("TypeClass Pointers")) {

        ShowUnitTypePicker     (&Rule->LargeVisceroid,      "LargeVisceroid");
        ShowUnitTypePicker     (&Rule->SmallVisceroid,      "SmallVisceroid");
        ShowUnitTypePicker     (&Rule->UnloadingHarvester,  "UnloadingHarvester");
//        ShowAnimTypePicker     (&Rule->DropPodPuff,         "DropPodPuff");
//        ShowAnimTypePicker     (&Rule->BarrelExplode,       "BarrelExplode");
//        ShowParticleSysPicker  (&Rule->BarrelParticle,      "BarrelParticle");
//        ShowAnimTypePicker     (&Rule->Wake,                "Wake");
        /* …repeat pattern for all *TypeClass pointers… */
    }

    /*  GROUP: vector editors */
    if (ImGui::CollapsingHeader("Lists / Vectors")) {

//        ShowPointerVector("HSBuilding",              &Rule->HSBuilding);
//        ShowPointerVector("BarrelDebris",            &Rule->BarrelDebris);
//        ShowPointerVector("DropPod",                 &Rule->DropPod);
//        ShowPointerVector("DeadBodies",              &Rule->DeadBodies);
//        ShowPointerVector("MetallicDebris",          &Rule->MetallicDebris);
//        ShowPointerVector("BridgeExplosions",        &Rule->BridgeExplosions);
//        ShowIntVector    ("PrerequisitePower",       &Rule->PrerequisitePower);
//        ShowIntVector    ("PrerequisiteFactory",     &Rule->PrerequisiteFactory);
//        ShowIntVector    ("PrerequisiteBarracks",    &Rule->PrerequisiteBarracks);
//        ShowIntVector    ("PrerequisiteRadar",       &Rule->PrerequisiteRadar);
//        ShowIntVector    ("PrerequisiteTech",        &Rule->PrerequisiteTech);
        /* …and so on for every TypeList… */
    }

    ImGui::End();
}

void CCImGui::DrawScenarioEditor(bool *pOpen)
{
    // Don't continue if flag pointer is NULL
    if (!pOpen) {
        return;
    }

    // Open a window with a close button in the corner
    if (!ImGui::Begin("Scenario Editor", pOpen)) {
        ImGui::End(); // required to balance ImGui::Begin()
        return;
    }

    // Paths and Strings
    ImGui::InputText("NextScenarioName", Scen->NextScenarioName, sizeof(Scen->NextScenarioName));
    ImGui::InputText("AltNextScenarioName", Scen->AltNextScenarioName, sizeof(Scen->AltNextScenarioName));
    ImGui::InputText("ScenarioName", Scen->ScenarioName, sizeof(Scen->ScenarioName));
    ImGui::InputText("Description", Scen->Description, sizeof(Scen->Description));

    // Briefing text
    static bool editingBriefing = false;

    if (editingBriefing) {
        if (ImGui::Button("Save Briefing")) editingBriefing = false;

        ImGui::InputTextMultiline("##BriefingEdit",
            Scen->BriefingText, sizeof(Scen->BriefingText),
            ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 8));
    } else {
        if (ImGui::Button("Edit Briefing")) editingBriefing = true;

        ImGui::PushTextWrapPos();
        ImGui::TextWrapped("%s", Scen->BriefingText);
        ImGui::PopTextWrapPos();
    }

    // Integers
    ImGui::InputInt("Home", &Scen->Home);
    ImGui::InputInt("AltHome", &Scen->AltHome);

    // UniqueID should not be modified!
    ImGui::BeginDisabled();
        ImGui::InputInt("UniqueID", &Scen->UniqueID);
    ImGui::EndDisabled();

    ImGui::InputInt("Scenario", &Scen->Scenario);
    ImGui::InputInt("CarryOverCap", &Scen->CarryOverCap);
    ImGui::InputInt("Percent", &Scen->Percent);
    ImGui::InputInt("BridgeCount", &Scen->BridgeCount);
    ImGui::InputInt("StartingDropships", &Scen->StartingDropships);
    ImGui::InputInt("AmbientLight", &Scen->AmbientLight);
    ImGui::InputInt("CurrentAmbientLight", &Scen->CurrentAmbientLight);
    ImGui::InputInt("DesiredAmbientLight", &Scen->DesiredAmbientLight);
    ImGui::InputInt("RedTint", &Scen->RedTint);
    ImGui::InputInt("GreenTint", &Scen->GreenTint);
    ImGui::InputInt("BlueTint", &Scen->BlueTint);
    ImGui::InputInt("GroundLight", &Scen->GroundLight);
    ImGui::InputInt("LevelLight", &Scen->LevelLight);
    ImGui::InputInt("IonAmbientLight", &Scen->IonAmbientLight);
    ImGui::InputInt("IonRedTint", &Scen->IonRedTint);
    ImGui::InputInt("IonGreenTint", &Scen->IonGreenTint);
    ImGui::InputInt("IonBlueTint", &Scen->IonBlueTint);
    ImGui::InputInt("IonGroundLight", &Scen->IonGroundLight);
    ImGui::InputInt("IonLevelLight", &Scen->IonLevelLight);
    ImGui::InputInt("InitTime", &Scen->InitTime);
    ImGui::InputInt("Stage", reinterpret_cast<int*>(&Scen->Stage)); // ushort cast

    // Booleans
    ImGui::Checkbox("IsFreeRadar", &Scen->IsFreeRadar);
    ImGui::Checkbox("IsTrainCrate", &Scen->IsTrainCrate);
    ImGui::Checkbox("IsTibGrowth", &Scen->IsTibGrowth);
    ImGui::Checkbox("IsVeinGrowth", &Scen->IsVeinGrowth);
    ImGui::Checkbox("IsIceGrowth", &Scen->IsIceGrowth);
    ImGui::Checkbox("IsBridgeChanged", &Scen->IsBridgeChanged);
    ImGui::Checkbox("IsFlagChanged", &Scen->IsFlagChanged);
    ImGui::Checkbox("IsAmbientChanged", &Scen->IsAmbientChanged);
    ImGui::Checkbox("IsEndOfGame", &Scen->IsEndOfGame);
    ImGui::Checkbox("IsInheritTimer", &Scen->IsInheritTimer);
    ImGui::Checkbox("IsSkipScore", &Scen->IsSkipScore);
    ImGui::Checkbox("IsOneTimeOnly", &Scen->IsOneTimeOnly);
    ImGui::Checkbox("IsNoMapSel", &Scen->IsNoMapSel);
    ImGui::Checkbox("IsTruckCrate", &Scen->IsTruckCrate);
    ImGui::Checkbox("IsMoneyTiberium", &Scen->IsMoneyTiberium);
    ImGui::Checkbox("IsTiberiumDeathToVisceroid", &Scen->IsTiberiumDeathToVisceroid);
    ImGui::Checkbox("IsIgnoreGlobalAITriggers", &Scen->IsIgnoreGlobalAITriggers);
    ImGui::Checkbox("IsGDI", &Scen->IsGDI);
    ImGui::Checkbox("IsMultiplayerOnly", &Scen->IsMultiplayerOnly);
    ImGui::Checkbox("IsRandom", &Scen->IsRandom);
    ImGui::Checkbox("IsCratePickup", &Scen->IsCratePickup);
    ImGui::Checkbox("field_1E38", &Scen->field_1E38);
    ImGui::Checkbox("InputLock", &Scen->InputLock);

    // Floats
    CCImGui::SliderScalarAuto("CarryOverPercent", &Scen->CarryOverPercent, 0.0f, 100.0f);

    // Enums — placeholder combos
    const char* campaign_names[] = { "None", "GDI", "Nod", "Custom" };  // Replace with actual
    ImGui::Combo("Campaign", reinterpret_cast<int*>(&Scen->Campaign), campaign_names, IM_ARRAYSIZE(campaign_names));

    const char* theater_names[] = { "Temperate", "Snow", "Urban", "Desert", "Lunar" };  // Replace
    ImGui::Combo("Theater", reinterpret_cast<int*>(&Scen->Theater), theater_names, IM_ARRAYSIZE(theater_names));

    const char* house_names[] = { "GDI", "Nod", "Neutral", "Civilian" };  // Replace
    ImGui::Combo("PlayerHouse", reinterpret_cast<int*>(&Scen->PlayerHouse), house_names, IM_ARRAYSIZE(house_names));

    const char* speech_side_names[] = { "GDI", "Nod", "Mutant", "Random" };  // Replace
    ImGui::Combo("SpeechSide", reinterpret_cast<int*>(&Scen->SpeechSide), speech_side_names, IM_ARRAYSIZE(speech_side_names));

    // Movie types (VQType enums)
    ShowVQPicker(&Scen->IntroMovie, "IntroMovie");
    ShowVQPicker(&Scen->BriefMovie, "BriefMovie");
    ShowVQPicker(&Scen->WinMovie, "WinMovie");
    ShowVQPicker(&Scen->LoseMovie, "LoseMovie");
    ShowVQPicker(&Scen->ActionMovie, "ActionMovie");
    ShowVQPicker(&Scen->PostScoreMovie, "PostScoreMovie");
    ShowVQPicker(&Scen->PreMapSelectMovie, "PreMapSelectMovie");

    // Lists (TypeList support)
//    ShowPointerVector("AllowableUnits", &Scen->AllowableUnits);
//    ShowIntVector("AllowableUnitMaximums", &Scen->AllowableUnitMaximums);
//    ShowIntVector("AllowableUnitUsage", &Scen->AllowableUnitUsage);

    // Custom structs (placeholders)
    if (ImGui::CollapsingHeader("Special")) {
        DrawSpecialEditor(nullptr);  // Or &open flag
    }

    if (ImGui::CollapsingHeader("Global Flags")) {
        for (int i = 0; i < ARRAYSIZE(Scen->GlobalFlags); ++i) {
            ImGui::Checkbox(("[" + ccstd::to_string(i) + "] " + Scen->GlobalFlags[i].Name).c_str(), (bool*)&Scen->GlobalFlags[i].Value);
        }
    }

    if (ImGui::CollapsingHeader("Local Flags")) {
        for (int i = 0; i < ARRAYSIZE(Scen->LocalFlags); ++i) {
            ImGui::Checkbox(("[" + ccstd::to_string(i) + "] " + Scen->LocalFlags[i].Name).c_str(), (bool*)&Scen->LocalFlags[i].Value);
        }
    }


    ImGui::End();
}

void CCImGui::DrawSpecialEditor(bool *pOpen)
{
}

void CCImGui::DrawGlobalsEditor(bool *pOpen)
{
    // Don't continue if flag pointer is NULL
    if (!pOpen) {
        return;
    }

    // Open a window with a close button in the corner
    if (!ImGui::Begin("Globals Editor", pOpen)) {
        ImGui::End(); // required to balance ImGui::Begin()
        return;
    }

    // --------------------------------------
    if (ImGui::CollapsingHeader("Debug Options", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Windowed", &Debug_Windowed);
        ImGui::Checkbox("ScenarioFilenameSet", &Debug_ScenarioFilenameSet);
        ImGui::InputText("ScenarioFilename", Debug_ScenarioFilename, 128);

        ImGui::Checkbox("MotionCapture", &Debug_MotionCapture);
        ImGui::Checkbox("AllowModeToggle", &Debug_AllowModeToggle);
        ImGui::Checkbox("Quiet", &Debug_Quiet);
        ImGui::Checkbox("Remap", &Debug_Remap);
        ImGui::Checkbox("Icon", &Debug_Icon);
        ImGui::Checkbox("Flag", &Debug_Flag);
        ImGui::Checkbox("Lose", &Debug_Lose);
        ImGui::Checkbox("Win", &Debug_Win);
        ImGui::Checkbox("Map", &Debug_Map);
        ImGui::Checkbox("Passable", &Debug_Passable);
        ImGui::Checkbox("Unshroud", &Debug_Unshroud);
        ImGui::Checkbox("Threat", &Debug_Threat);
        ImGui::Checkbox("Find_Path", &Debug_Find_Path);
        ImGui::Checkbox("Check_Map", &Debug_Check_Map);
        ImGui::Checkbox("Playtest", &Debug_Playtest);
        ImGui::Checkbox("Trap_Check_Heap", &Debug_Trap_Check_Heap);
        ImGui::Checkbox("Modem_Dump", &Debug_Modem_Dump);
        ImGui::Checkbox("Print_Events", &Debug_Print_Events);
    }

    if (ImGui::CollapsingHeader("Cheat Flags", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Cheat", &Debug_Cheat);
        ImGui::Checkbox("Cheat_Pengo", &Cheat_Pengo);
        ImGui::Checkbox("Cheat_TheTeam", &Cheat_TheTeam);
    }

    if (ImGui::CollapsingHeader("Scenario / Player State", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("UserInputLocked", &UserInputLocked);
        ImGui::InputInt("ScenarioInit", &ScenarioInit);
        ImGui::Checkbox("AllowVoice", &AllowVoice);
        ImGui::Checkbox("TacticalViewActive", &TacticalViewActive);
        ImGui::Checkbox("ScenarioStarted", &ScenarioStarted);
        ImGui::Checkbox("GameInFocus", &GameInFocus);
        ImGui::Checkbox("GameActive", &GameActive);
        ImGui::Checkbox("PlayerWins", &PlayerWins);
        ImGui::Checkbox("PlayerLoses", &PlayerLoses);
        ImGui::Checkbox("PlayerRestarts", &PlayerRestarts);
        ImGui::Checkbox("PlayerAborts", &PlayerAborts);
        ImGui::Checkbox("ScoresPresent", &ScoresPresent);
    }

    if (ImGui::CollapsingHeader("Timers", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::TextDisabled("Timers are referenced, not editable directly:");
        ImGui::Text("NetFrameTimer:  %d", NetFrameTimer.Value());
        ImGui::Text("TickCount:      %d", TickCount.Value());
        ImGui::Text("FrameTimer:     %d", FrameTimer.Value());
        ImGui::Text("ActionLineTimer:%d", ActionLineTimer.Value());
    }

    if (ImGui::CollapsingHeader("Frame Control", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputInt("NewMaxAheadFrame1", &NewMaxAheadFrame1);
        ImGui::InputInt("NewMaxAheadFrame2", &NewMaxAheadFrame2);
        ImGui::InputInt("FramesPerSecond", (int*)&FramesPerSecond);
    }

    // --------------------------------------
    if (ImGui::CollapsingHeader("Team State", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputInt("TeamEvent", &TeamEvent);
        ImGui::InputInt("TeamNumber", &TeamNumber);
    }

    // --------------------------------------
    if (ImGui::CollapsingHeader("Pointers / Objects", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("PlayerPtr:        %p", PlayerPtr);
        ImGui::Text("MasterParticle:   %p", MasterParticle);
        ImGui::Text("EndGame:          %p", &EndGame);
        ImGui::Text("SpecialDialog:    %d", (int)SpecialDialog); // Enum as int
        ImGui::Text("Whom (HouseType): %d", (int)Whom); // Enum as int
    }

    // --------------------------------------
    if (ImGui::CollapsingHeader("Seed & Random", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputScalar("Seed", ImGuiDataType_U32, &Seed);
        ImGui::InputScalar("CustomSeed", ImGuiDataType_U32, &CustomSeed);
    }

    // --------------------------------------
    if (ImGui::CollapsingHeader("UI Layout", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("RedrawSidebar", &RedrawSidebar);

        ImGui::Text("SidebarRect");
        ImGui::InputInt4("##Sidebar", &SidebarRect.X);

        ImGui::Text("TacticalRect");
        ImGui::InputInt4("##Tactical", &TacticalRect.X);

        ImGui::Text("ScreenRect");
        ImGui::InputInt4("##Screen", &ScreenRect.X);
    }

    if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::InputInt("BuildLevel", &BuildLevel);
        ImGui::InputInt("RequiredAddon", &RequiredAddon);
        ImGui::InputInt("NewINIFormat", &NewINIFormat);
    }

    ImGui::End();
}

void CCImGui::DrawTypesEditor(bool *pOpen)
{
    if (!ImGui::Begin("Types", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    // Macro to emit one line (label, size, button)
#define TYPE_LINE(label, vec)                             \
    ImGui::Text("%s", label);                             \
    ImGui::SameLine(200);                                 \
    ImGui::Text("%d", vec.Count());                       \
    ImGui::SameLine(260);                                 \
    if (ImGui::Button(("View##" label)))                  \
        ImGui::OpenPopup(label);                          \
    ShowVectorViewer(label, vec);

    // ---- GROUP: Abstract / Object / Techno ----------------
    if (ImGui::CollapsingHeader("Core Types", ImGuiTreeNodeFlags_DefaultOpen))
    {
        TYPE_LINE("AbstractTypes",     AbstractTypes);
        TYPE_LINE("ObjectTypes",       ObjectTypes);
        TYPE_LINE("TechnoTypes",       TechnoTypes);
        TYPE_LINE("BuildingTypes",     BuildingTypes);
        TYPE_LINE("UnitTypes",         UnitTypes);
        TYPE_LINE("InfantryTypes",     InfantryTypes);
        TYPE_LINE("AircraftTypes",     AircraftTypes);
        TYPE_LINE("VoxelAnimTypes",    VoxelAnimTypes);
        TYPE_LINE("WeaponTypes",       WeaponTypes);
        TYPE_LINE("BulletTypes",       BulletTypes);
        TYPE_LINE("WarheadTypes",      WarheadTypes);
        TYPE_LINE("OverlayTypes",      OverlayTypes);
    }

    // ---- GROUP: Game World Objects ------------------------
    if (ImGui::CollapsingHeader("World Objects"))
    {
        TYPE_LINE("Objects", Objects);
        TYPE_LINE("Technos", Technos);
        TYPE_LINE("Foots", Foots);
        TYPE_LINE("Buildings", Buildings);
        TYPE_LINE("Units", Units);
        TYPE_LINE("Infantry", Infantry);
        TYPE_LINE("Aircrafts", Aircrafts);
        TYPE_LINE("Bullets", Bullets);
        TYPE_LINE("VoxelAnims", VoxelAnims);
        TYPE_LINE("ParticleSystems", ParticleSystems);
        TYPE_LINE("Particles", Particles);
        TYPE_LINE("IsoTiles", IsoTiles);
        TYPE_LINE("Terrains", Terrains);
        TYPE_LINE("Smudges", Smudges);
        TYPE_LINE("Overlays", Overlays);
    }

    // ---- GROUP: AI / Teams / Scripts ----------------------
    if (ImGui::CollapsingHeader("AI & Teams"))
    {
        TYPE_LINE("AITriggerTypes", AITriggerTypes);
        TYPE_LINE("TeamTypes", TeamTypes);
        TYPE_LINE("Teams", Teams);
        TYPE_LINE("TaskForces", TaskForces);
        TYPE_LINE("ScriptTypes", ScriptTypes);
        TYPE_LINE("Scripts", Scripts);
        TYPE_LINE("TEvents", TEvents);
        TYPE_LINE("TActions", TActions);
    }

    // ---- GROUP: Houses / Sides ----------------------------
    if (ImGui::CollapsingHeader("Houses & Sides"))
    {
        TYPE_LINE("HouseTypes", HouseTypes);
        TYPE_LINE("Houses", Houses);
        TYPE_LINE("Sides", Sides);
        TYPE_LINE("ColorSchemes", ColorSchemes);
    }

    // ---- GROUP: Audio / Video -----------------------------
    if (ImGui::CollapsingHeader("Audio / Video"))
    {
        TYPE_LINE("Vocs", Vocs);
        TYPE_LINE("IngameVQ", IngameVQ);
    }

    // ---- GROUP: Misc --------------------------------------
    if (ImGui::CollapsingHeader("Misc"))
    {
        TYPE_LINE("Converts", Converts);
        TYPE_LINE("TileDrawers", TileDrawers);
        TYPE_LINE("LightSources", LightSources);
        TYPE_LINE("ParticleSystemTypes", ParticleSystemTypes);
        TYPE_LINE("Spotlights", Spotlights);
        TYPE_LINE("LaserDraws", LaserDraws);
        TYPE_LINE("Empulses", Empulses);
        TYPE_LINE("AlphaShapes", AlphaShapes);
        TYPE_LINE("FoggedObjects", FoggedObjects);
        TYPE_LINE("VeinholeMonsters", VeinholeMonsters);
    }

#undef  TYPE_LINE
    ImGui::End();
}
