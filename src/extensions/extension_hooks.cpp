/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          EXTENSION_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for implementing all the extended classes.
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
#include "extension_hooks.h"

#include "abstractext_hooks.h"
#include "technoext_hooks.h"
#include "footext_hooks.h"

#include "objecttypeext_hooks.h"
#include "technotypeext_hooks.h"

#include "unitext_hooks.h"
#include "aircraftext_hooks.h"
#include "aircrafttypeext_hooks.h"
#include "animext_hooks.h"
#include "animtypeext_hooks.h"
#include "buildingext_hooks.h"
#include "buildingtypeext_hooks.h"
#include "bulletext_hooks.h"
#include "bullettypeext_hooks.h"
#include "campaignext_hooks.h"
#include "cellext_hooks.h"
#include "factoryext_hooks.h"
#include "houseext_hooks.h"
#include "housetypeext_hooks.h"
#include "infantryext_hooks.h"
#include "infantrytypeext_hooks.h"
//EXT_RTTI_ISOTILE
#include "isotiletypeext_hooks.h"
//EXT_RTTI_LIGHT
//EXT_RTTI_OVERLAY
#include "overlaytypeext_hooks.h"
//EXT_RTTI_PARTICLE
#include "particletypeext_hooks.h"
#include "particlesysext_hooks.h"
#include "particlesystypeext_hooks.h"
//EXT_RTTI_SCRIPT
//EXT_RTTI_SCRIPTTYPE
#include "sideext_hooks.h"
//EXT_RTTI_SMUDGE
#include "smudgetypeext_hooks.h"
#include "supertypeext_hooks.h"
//EXT_RTTI_TASKFORCE
#include "teamext_hooks.h"
//EXT_RTTI_TEAMTYPE
#include "terrainext_hooks.h"
#include "terraintypeext_hooks.h"
//EXT_RTTI_TRIGGER
//EXT_RTTI_TRIGGERTYPE
#include "unittypeext_hooks.h"
//EXT_RTTI_VOXELANIM
#include "voxelanimtypeext_hooks.h"
#include "waveext_hooks.h"
//EXT_RTTI_TAG
//EXT_RTTI_TAGTYPE
#include "tiberiumext_hooks.h"
#include "tactionext_hooks.h"
//EXT_RTTI_EVENT
#include "weapontypeext_hooks.h"
#include "warheadtypeext_hooks.h"
//EXT_RTTI_WAYPOINT
//EXT_RTTI_TUBE
//EXT_RTTI_LIGHTSOURCE
#include "empulseext_hooks.h"
//EXT_RTTI_TACTICALMAP
#include "superext_hooks.h"
//EXT_RTTI_AITRIGGER
//EXT_RTTI_AITRIGGERTYPE
//EXT_RTTI_NEURON
//EXT_RTTI_FOGGEDOBJECT
//EXT_RTTI_ALPHASHAPE
//EXT_RTTI_VEINHOLEMONSTER







#include "initext_hooks.h"
#include "mainloopext_hooks.h"
#include "newmenuext_hooks.h"

#include "rulesext_hooks.h"
#include "tacticalext_hooks.h"
#include "scenarioext_hooks.h"
#include "displayext_hooks.h"
#include "sidebarext_hooks.h"
#include "tooltipext_hooks.h"
#include "commandext_hooks.h"
#include "optionsext_hooks.h"
#include "msglistext_hooks.h"
#include "sessionext_hooks.h"
#include "cdext_hooks.h"

#include "playmovie_hooks.h"
#include "vqaext_hooks.h"
#include "themeext_hooks.h"








#include "combatext_hooks.h"

#include "empulseext_hooks.h"
#include "waveext_hooks.h"

#include "txtlabelext_hooks.h"

#include "dropshipext_hooks.h"
#include "endgameext_hooks.h"

#include "mapseedext_hooks.h"
#include "multiscoreext_hooks.h"
#include "multimissionext_hooks.h"

#include "cciniext_hooks.h"
#include "rawfileext_hooks.h"
#include "ccfileext_hooks.h"

#include "theatertype_hooks.h"

#include "fetchres_hooks.h"

#include "skirmishdlg_hooks.h"

#include "filepcx_hooks.h"






#include "iomap.h"

#include "extension.h"
#include "swizzle.h"

#include "hooker.h"
#include "hooker_macros.h"



#include "tracker.h"
static void _Detach_This_From_All_Intercept(TARGET target, bool all)
{
    Extension::Detach_This_From_All(target, all);

    Detach_This_From_All(target, all);
}

#include "tibsun_functions.h"
static void _Clear_Vectors_Intercept()
{
    Extension::Clear_Vectors();

    Clear_Vectors();
}

DECLARE_PATCH(_Print_CRCs_Hook)
{
    GET_REGISTER_STATIC(FILE *, fp, esi);
    GET_REGISTER_OFFSET_STATIC(EventClass *, ev, esp, 0x48); // TODO

    _asm { push esi }
    _asm { mov eax, 0x006B6944 }
    _asm { call eax } //_fclose

    _asm { add esp, 0x8 }

    Extension::Print_CRCs(fp, ev);

    JMP(0x005B8464);
}



void Extension_Hooks()
{
    Patch_Call(0x0053DF7A, &_Clear_Vectors_Intercept); // MapSeedClass::Init_Random
    Patch_Call(0x005DC590, &_Clear_Vectors_Intercept); // Clear_Scenario
    Patch_Call(0x00601BA2, &_Clear_Vectors_Intercept); // Game_Shutdown

    Patch_Call(0x0040DBB3, &_Detach_This_From_All_Intercept); // AircraftClass::~AircraftClass
    Patch_Call(0x0040F123, &_Detach_This_From_All_Intercept); // AircraftClass_Fall_To_Death
    Patch_Call(0x0040FCD3, &_Detach_This_From_All_Intercept); // AircraftTypeClass::~AircraftTypeClass
    Patch_Call(0x00410223, &_Detach_This_From_All_Intercept); // AircraftTypeClass::~AircraftTypeClass
    Patch_Call(0x004142C6, &_Detach_This_From_All_Intercept); // AnimClass::~AnimClass
    Patch_Call(0x00426662, &_Detach_This_From_All_Intercept); // BuildingClass::~BuildingClass
    Patch_Call(0x0043F94D, &_Detach_This_From_All_Intercept); // BuildingTypeClass::~BuildingTypeClass
    Patch_Call(0x0044407D, &_Detach_This_From_All_Intercept); // BuildingTypeClass::~BuildingTypeClass
    Patch_Call(0x004445F3, &_Detach_This_From_All_Intercept); // BulletClass::~BulletClass
    Patch_Call(0x004474D3, &_Detach_This_From_All_Intercept); // BulletClass::~BulletClass
    Patch_Call(0x00447DC3, &_Detach_This_From_All_Intercept); // BulletTypeClass::~BulletTypeClass
    Patch_Call(0x00448723, &_Detach_This_From_All_Intercept); // BulletTypeClass::~BulletTypeClass
    Patch_Call(0x00448AE3, &_Detach_This_From_All_Intercept); // CampaignClass::~CampaignClass
    Patch_Call(0x00448EF3, &_Detach_This_From_All_Intercept); // CampaignClass::~CampaignClass
    Patch_Call(0x00456A26, &_Detach_This_From_All_Intercept); // CellClass::Wall_Update
    Patch_Call(0x00456A58, &_Detach_This_From_All_Intercept); // CellClass::Wall_Update
    Patch_Call(0x00456A7F, &_Detach_This_From_All_Intercept); // CellClass::Wall_Update
    Patch_Call(0x00456AAB, &_Detach_This_From_All_Intercept); // CellClass::Wall_Update
    Patch_Call(0x00456AD2, &_Detach_This_From_All_Intercept); // CellClass::Wall_Update
    Patch_Call(0x004571F9, &_Detach_This_From_All_Intercept); // CellClass::Reduce_Wall
    Patch_Call(0x004927D3, &_Detach_This_From_All_Intercept); // EMPulseClass::~EMPulseClass
    Patch_Call(0x004931E3, &_Detach_This_From_All_Intercept); // EMPulseClass::~EMPulseClass
    Patch_Call(0x00496DB3, &_Detach_This_From_All_Intercept); // FactoryClass::~FactoryClass
    Patch_Call(0x00497AA3, &_Detach_This_From_All_Intercept); // FactoryClass::~FactoryClass
    Patch_Call(0x004BB6DB, &_Detach_This_From_All_Intercept); // HouseClass::~HouseClass
    Patch_Call(0x004CDE93, &_Detach_This_From_All_Intercept); // HouseTypeClass::~HouseTypeClass
    Patch_Call(0x004CE603, &_Detach_This_From_All_Intercept); // HouseTypeClass::~HouseTypeClass
    Patch_Call(0x004D22DC, &_Detach_This_From_All_Intercept); // InfantryClass::~InfantryClass
    Patch_Call(0x004DA3B4, &_Detach_This_From_All_Intercept); // InfantryTypeClass::~InfantryTypeClass
    Patch_Call(0x004DB133, &_Detach_This_From_All_Intercept); // InfantryTypeClass::~InfantryTypeClass
    Patch_Call(0x004F2173, &_Detach_This_From_All_Intercept); // IsometricTileClass::~IsometricTileClass
    Patch_Call(0x004F23E3, &_Detach_This_From_All_Intercept); // IsometricTileClass::~IsometricTileClass
    Patch_Call(0x004F3344, &_Detach_This_From_All_Intercept); // IsometricTileTypeClass::~IsometricTileTypeClass
    Patch_Call(0x005015E3, &_Detach_This_From_All_Intercept); // LightSourceClass::~LightSourceClass
    Patch_Call(0x00501DA3, &_Detach_This_From_All_Intercept); // LightSourceClass::~LightSourceClass
    Patch_Call(0x00585F9E, &_Detach_This_From_All_Intercept); // ObjectClass::Detach_All
    Patch_Call(0x00586DB5, &_Detach_This_From_All_Intercept); // ObjectClass::entry_E4
    Patch_Call(0x0058B563, &_Detach_This_From_All_Intercept); // OverlayClass::~OverlayClass
    Patch_Call(0x0058CB13, &_Detach_This_From_All_Intercept); // OverlayClass::~OverlayClass
    Patch_Call(0x0058D196, &_Detach_This_From_All_Intercept); // OverlayTypeClass::~OverlayTypeClass
    Patch_Call(0x0058DC86, &_Detach_This_From_All_Intercept); // OverlayTypeClass::~OverlayTypeClass
    Patch_Call(0x005A32FA, &_Detach_This_From_All_Intercept); // ParticleClass::~ParticleClass
    Patch_Call(0x005A503A, &_Detach_This_From_All_Intercept); // ParticleClass::~ParticleClass
    Patch_Call(0x005A56D4, &_Detach_This_From_All_Intercept); // ParticleSystemClass::~ParticleSystemClass
    Patch_Call(0x005AE573, &_Detach_This_From_All_Intercept); // ParticleSystemTypeClass::~ParticleSystemTypeClass
    Patch_Call(0x005AEC63, &_Detach_This_From_All_Intercept); // ParticleSystemTypeClass::~ParticleSystemTypeClass
    Patch_Call(0x005AF153, &_Detach_This_From_All_Intercept); // ParticleTypeClass::~ParticleTypeClass
    Patch_Call(0x005AFC33, &_Detach_This_From_All_Intercept); // ParticleTypeClass::~ParticleTypeClass
    Patch_Call(0x005E78C3, &_Detach_This_From_All_Intercept); // ScriptClass::~ScriptClass
    Patch_Call(0x005E7B83, &_Detach_This_From_All_Intercept); // ScriptTypeClass::~ScriptTypeClass
    Patch_Call(0x005E81E3, &_Detach_This_From_All_Intercept); // ScriptClass::~ScriptClass
    Patch_Call(0x005E8293, &_Detach_This_From_All_Intercept); // ScriptTypeClass::~ScriptTypeClass
    Patch_Call(0x005F1AE3, &_Detach_This_From_All_Intercept); // SideClass::~SideClass
    Patch_Call(0x005F1D93, &_Detach_This_From_All_Intercept); // SideClass::~SideClass
    Patch_Call(0x005FAAD3, &_Detach_This_From_All_Intercept); // SmudgeClass::~SmudgeClass
    Patch_Call(0x005FAF03, &_Detach_This_From_All_Intercept); // SmudgeClass::~SmudgeClass
    Patch_Call(0x005FB313, &_Detach_This_From_All_Intercept); // SmudgeTypeClass::~SmudgeTypeClass
    Patch_Call(0x005FC023, &_Detach_This_From_All_Intercept); // SmudgeTypeClass::~SmudgeTypeClass
    Patch_Call(0x00618D03, &_Detach_This_From_All_Intercept); // TActionClass::~TActionClass
    Patch_Call(0x0061DAD3, &_Detach_This_From_All_Intercept); // TActionClass::~TActionClass
    Patch_Call(0x0061E4B6, &_Detach_This_From_All_Intercept); // TagClass::~TagClass
    Patch_Call(0x0061E73B, &_Detach_This_From_All_Intercept); // TagClass::~TagClass
    Patch_Call(0x0061E9AA, &_Detach_This_From_All_Intercept); // TagClass::Spring
    Patch_Call(0x0061F164, &_Detach_This_From_All_Intercept); // TagTypeClass::~TagTypeClass
    Patch_Call(0x00621503, &_Detach_This_From_All_Intercept); // TaskForceClass::~TaskForceClass
    Patch_Call(0x00621E43, &_Detach_This_From_All_Intercept); // TaskForceClass::~TaskForceClass
    Patch_Call(0x006224E3, &_Detach_This_From_All_Intercept); // TeamClass::~TeamClass
    Patch_Call(0x00627EF3, &_Detach_This_From_All_Intercept); // TeamTypeClass::~TeamTypeClass
    Patch_Call(0x00629293, &_Detach_This_From_All_Intercept); // TeamTypeClass::~TeamTypeClass
    Patch_Call(0x0063F188, &_Detach_This_From_All_Intercept); // TerrainClass::~TerrainClass
    Patch_Call(0x00640C38, &_Detach_This_From_All_Intercept); // TerrainClass::~TerrainClass
    Patch_Call(0x00641653, &_Detach_This_From_All_Intercept); // TerrainTypeClass::~TerrainTypeClass
    Patch_Call(0x00641D83, &_Detach_This_From_All_Intercept); // TerrainTypeClass::~TerrainTypeClass
    Patch_Call(0x00642223, &_Detach_This_From_All_Intercept); // TEventClass::~TEventClass
    Patch_Call(0x00642F23, &_Detach_This_From_All_Intercept); // TEventClass::~TEventClass
    Patch_Call(0x00644A45, &_Detach_This_From_All_Intercept); // TiberiumClass::~TiberiumClass
    Patch_Call(0x006491A3, &_Detach_This_From_All_Intercept); // TriggerClass::~TriggerClass
    Patch_Call(0x00649943, &_Detach_This_From_All_Intercept); // TriggerClass::~TriggerClass
    Patch_Call(0x00649E03, &_Detach_This_From_All_Intercept); // TriggerTypeClass::~TriggerTypeClass
    Patch_Call(0x0064AFD3, &_Detach_This_From_All_Intercept); // TubeClass::~TubeClass
    Patch_Call(0x0064B603, &_Detach_This_From_All_Intercept); // TubeClass::~TubeClass
    Patch_Call(0x0064D8A9, &_Detach_This_From_All_Intercept); // UnitClass::~UnitClass
    Patch_Call(0x0065BAD3, &_Detach_This_From_All_Intercept); // UnitTypeClass::~UnitTypeClass
    Patch_Call(0x0065C793, &_Detach_This_From_All_Intercept); // UnitTypeClass::~UnitTypeClass
    Patch_Call(0x0065DF23, &_Detach_This_From_All_Intercept); // VoxelAnimClass::~VoxelAnimClass
    Patch_Call(0x0065F5A3, &_Detach_This_From_All_Intercept); // VoxelAnimTypeClass::~VoxelAnimTypeClass
    Patch_Call(0x00660093, &_Detach_This_From_All_Intercept); // VoxelAnimTypeClass::~VoxelAnimTypeClass
    Patch_Call(0x00661227, &_Detach_This_From_All_Intercept); // VeinholeMonsterClass::~VeinholeMonsterClass
    Patch_Call(0x00661C00, &_Detach_This_From_All_Intercept); // VeinholeMonsterClass::Take_Damage
    Patch_Call(0x0066EF73, &_Detach_This_From_All_Intercept); // WarheadTypeClass::~WarheadTypeClass
    Patch_Call(0x0066FA93, &_Detach_This_From_All_Intercept); // WarheadTypeClass::~WarheadTypeClass
    Patch_Call(0x006702D4, &_Detach_This_From_All_Intercept); // WaveClass::~WaveClass
    Patch_Call(0x00672E73, &_Detach_This_From_All_Intercept); // WaveClass::~WaveClass
    Patch_Call(0x00673563, &_Detach_This_From_All_Intercept); // WaypointPathClass::~WaypointPathClass
    Patch_Call(0x00673AA3, &_Detach_This_From_All_Intercept); // WaypointPathClass::~WaypointPathClass
    Patch_Call(0x00680C54, &_Detach_This_From_All_Intercept); // WeaponTypeClass::~WeaponTypeClass
    Patch_Call(0x006818F4, &_Detach_This_From_All_Intercept); // WeaponTypeClass::~WeaponTypeClass

    Patch_Jump(0x005B845B, &_Print_CRCs_Hook);






    /**
     *  Abstract and stack class extensions here.
     */
    AbstractClassExtension_Hooks();

    /**
     *  All game type class extensions here.
     */
    if (Extension::Is_Supported(RTTI_UNITTYPE) || Extension::Is_Supported(RTTI_INFANTRYTYPE)
     || Extension::Is_Supported(RTTI_BUILDINGTYPE) || Extension::Is_Supported(RTTI_AIRCRAFTTYPE)) {

        ObjectTypeClassExtension_Hooks();
        TechnoTypeClassExtension_Hooks();
    }

    /**
     *  All game class extensions here.
     */
    if (Extension::Is_Supported(RTTI_UNIT) || Extension::Is_Supported(RTTI_INFANTRY)
     || Extension::Is_Supported(RTTI_BUILDING) || Extension::Is_Supported(RTTI_AIRCRAFT)) {

        TechnoClassExtension_Hooks();
    }

    if (Extension::Is_Supported(RTTI_UNIT) || Extension::Is_Supported(RTTI_INFANTRY) || Extension::Is_Supported(RTTI_AIRCRAFT)) {
        FootClassExtension_Hooks();
    }

    if (Extension::Is_Supported(RTTI_UNIT)) { UnitClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_AIRCRAFT)) { AircraftClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_AIRCRAFTTYPE)) { AircraftTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_ANIM)) { AnimClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_ANIMTYPE)) { AnimTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_BUILDING)) { BuildingClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_BUILDINGTYPE)) { BuildingTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_BULLET)) { BulletClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_BULLETTYPE)) { BulletTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_CAMPAIGN)) { CampaignClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_CELL)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FACTORY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_HOUSE)) { HouseClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_HOUSETYPE)) { HouseTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_INFANTRY)) { InfantryClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_INFANTRYTYPE)) { InfantryTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_ISOTILE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ISOTILETYPE)) { }                    // <- Do not save!
    if (Extension::Is_Supported(RTTI_LIGHT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAY)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_OVERLAYTYPE)) { OverlayTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_PARTICLE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_PARTICLETYPE)) { ParticleTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEM)) { ParticleSystemClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_PARTICLESYSTEMTYPE)) { ParticleSystemTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_SCRIPT)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SCRIPTTYPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SIDE)) { SideClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_SMUDGE)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_SMUDGETYPE)) { SmudgeTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_SPECIAL)) { }                        // <- Do not save!
    if (Extension::Is_Supported(RTTI_SUPERWEAPONTYPE)) { SuperWeaponTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_TASKFORCE)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAM)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TEAMTYPE)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TERRAIN)) { TerrainClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_TERRAINTYPE)) { TerrainTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_TRIGGER)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TRIGGERTYPE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_UNITTYPE)) { UnitTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_VOXELANIM)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VOXELANIMTYPE)) { VoxelAnimTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_WAVE)) { WaveClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_TAG)) { }                            // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TAGTYPE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TIBERIUM)) { TiberiumClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_ACTION)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EVENT)) { }                          // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_WEAPONTYPE)) { WeaponTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_WARHEADTYPE)) { WarheadTypeClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_WAYPOINT)) { }                       // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ABSTRACT)) { }                       // <- Do not save!
    if (Extension::Is_Supported(RTTI_TUBE)) { }                           // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_LIGHTSOURCE)) { }                    // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_EMPULSE)) { }                        // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_TACTICALMAP)) { TacticalExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_SUPERWEAPON)) { SuperClassExtension_Hooks(); }
    if (Extension::Is_Supported(RTTI_AITRIGGER)) { }                      // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_AITRIGGERTYPE)) { }                  // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_NEURON)) { }                         // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_FOGGEDOBJECT)) { }                   // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_ALPHASHAPE)) { }                     // <- Not yet implemented
    if (Extension::Is_Supported(RTTI_VEINHOLEMONSTER)) { }                // <- Not yet implemented

    /**
     *  All global class extensions here.
     */
//    RulesClassExtension_Hooks();
//    ScenarioClassExtension_Hooks();
//    DisplayClassExtension_Hooks();
//    SidebarClassExtension_Hooks();
//    OptionsClassExtension_Hooks();
//    SessionClassExtension_Hooks();
//    ThemeClassExtension_Hooks();

    /**
     *  New classes and interfaces.
     */
    TheaterTypeClassExtension_Hooks();

    /**
     *  Various modules and functions.
     */
    GameInit_Hooks();
    MainLoop_Hooks();
    NewMenuExtension_Hooks();
    CommandExtension_Hooks();
    CDExtension_Hooks();
    PlayMovieExtension_Hooks();
    VQAExtension_Hooks();
    CCINIClassExtension_Hooks();
    RawFileClassExtension_Hooks();
    CCFileClassExtension_Hooks();

    MessageListClassExtension_Hooks();
    TextLabelClassExtension_Hooks();
    ToolTipManagerExtension_Hooks();

    CombatExtension_Hooks();
    DropshipExtension_Hooks();
    EndGameExtension_Hooks();
    MapSeedClassExtension_Hooks();
    MultiScoreExtension_Hooks();
    MultiMissionExtension_Hooks();

    /**
     *  Dialogs and associated code.
     */
    SkirmishDialog_Hooks();

    /**
     *  Miscellaneous hooks
     */
    FilePCXExtension_Hooks();
    FetchRes_Hooks();
}
