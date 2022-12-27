/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          EXTENSION_GLOBALS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extension interface global values.
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
#include "extension_globals.h"


DynamicVectorClass<UnitClassExtension *> UnitExtensions;
DynamicVectorClass<AircraftClassExtension *> AircraftExtensions;
DynamicVectorClass<AircraftTypeClassExtension *> AircraftTypeExtensions;
DynamicVectorClass<AnimClassExtension *> AnimExtensions;
DynamicVectorClass<AnimTypeClassExtension *> AnimTypeExtensions;
DynamicVectorClass<BuildingClassExtension *> BuildingExtensions;
DynamicVectorClass<BuildingTypeClassExtension *> BuildingTypeExtensions;
DynamicVectorClass<BulletTypeClassExtension *> BulletTypeExtensions;
DynamicVectorClass<CampaignClassExtension *> CampaignExtensions;
DynamicVectorClass<HouseClassExtension *> HouseExtensions;
DynamicVectorClass<HouseTypeClassExtension *> HouseTypeExtensions;
DynamicVectorClass<InfantryClassExtension *> InfantryExtensions;
DynamicVectorClass<InfantryTypeClassExtension *> InfantryTypeExtensions;
DynamicVectorClass<IsometricTileTypeClassExtension *> IsometricTileTypeExtensions;
DynamicVectorClass<OverlayTypeClassExtension *> OverlayTypeExtensions;
DynamicVectorClass<ParticleTypeClassExtension *> ParticleTypeExtensions;
DynamicVectorClass<ParticleSystemTypeClassExtension *> ParticleSystemTypeExtensions;
DynamicVectorClass<SideClassExtension *> SideExtensions;
DynamicVectorClass<SmudgeTypeClassExtension *> SmudgeTypeExtensions;
DynamicVectorClass<SuperWeaponTypeClassExtension *> SuperWeaponTypeExtensions;
DynamicVectorClass<TerrainClassExtension *> TerrainExtensions;
DynamicVectorClass<TerrainTypeClassExtension *> TerrainTypeExtensions;
DynamicVectorClass<UnitTypeClassExtension *> UnitTypeExtensions;
DynamicVectorClass<VoxelAnimTypeClassExtension *> VoxelAnimTypeExtensions;
DynamicVectorClass<WaveClassExtension *> WaveExtensions;
DynamicVectorClass<TiberiumClassExtension *> TiberiumExtensions;
DynamicVectorClass<WeaponTypeClassExtension *> WeaponTypeExtensions;
DynamicVectorClass<WarheadTypeClassExtension *> WarheadTypeExtensions;
DynamicVectorClass<SuperClassExtension *> SuperExtensions;
DynamicVectorClass<AITriggerTypeClassExtension *> AITriggerTypeExtensions;

TacticalExtension *TacticalMapExtension = nullptr;

RulesClassExtension *RuleExtension = nullptr;
ScenarioClassExtension *ScenExtension = nullptr;
SessionClassExtension *SessionExtension = nullptr;
OptionsClassExtension *OptionsExtension = nullptr;

DynamicVectorClass<ThemeControlExtension *> ThemeControlExtensions;
