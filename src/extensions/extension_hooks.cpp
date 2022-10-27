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
//#include "isotileext_hooks.h"
#include "isotiletypeext_hooks.h"
//#include "buildinglightext_hooks.h"
//#include "overlayext_hooks.h"
#include "overlaytypeext_hooks.h"
//#include "particleext_hooks.h"
#include "particletypeext_hooks.h"
#include "particlesysext_hooks.h"
#include "particlesystypeext_hooks.h"
//#include "scriptext_hooks.h"
//#include "scripttypeext_hooks.h"
#include "sideext_hooks.h"
//#include "smudgeext_hooks.h"
#include "smudgetypeext_hooks.h"
#include "supertypeext_hooks.h"
//#include "taskforceext_hooks.h"
#include "teamext_hooks.h"
//#include "teamtypeext_hooks.h"
#include "terrainext_hooks.h"
#include "terraintypeext_hooks.h"
//#include "triggerext_hooks.h"
//#include "triggertypeext_hooks.h"
#include "unittypeext_hooks.h"
//#include "voxelanimext_hooks.h"
#include "voxelanimtypeext_hooks.h"
#include "waveext_hooks.h"
//#include "tagext_hooks.h"
//#include "tagtypeext_hooks.h"
#include "tiberiumext_hooks.h"
#include "tactionext_hooks.h"
//#include "teventext_hooks.h"
#include "weapontypeext_hooks.h"
#include "warheadtypeext_hooks.h"
//#include "waypointeext_hooks.h"
//#include "tubeext_hooks.h"
//#include "lightsourceext_hooks.h"
#include "empulseext_hooks.h"
#include "tacticalext_hooks.h"
#include "superext_hooks.h"
//#include "aitriggerext_hooks.h"
//#include "aitriggertypeext_hooks.h"
//#include "neuronext_hooks.h"
//#include "foggedobjectext_hooks.h"
//#include "alphashapeext_hooks.h"
//#include "veinholemonsterext_hooks.h"

#include "rulesext_hooks.h"
#include "scenarioext_hooks.h"
#include "sessionext_hooks.h"
#include "optionsext_hooks.h"

#include "themeext_hooks.h"

#include "displayext_hooks.h"
#include "sidebarext_hooks.h"

#include "initext_hooks.h"
#include "mainloopext_hooks.h"
#include "newmenuext_hooks.h"
#include "commandext_hooks.h"
#include "cdext_hooks.h"
#include "playmovie_hooks.h"
#include "vqaext_hooks.h"
#include "cciniext_hooks.h"
#include "rawfileext_hooks.h"
#include "ccfileext_hooks.h"

#include "msglistext_hooks.h"
#include "txtlabelext_hooks.h"
#include "tooltipext_hooks.h"

#include "combatext_hooks.h"
#include "dropshipext_hooks.h"
#include "endgameext_hooks.h"
#include "mapseedext_hooks.h"
#include "multiscoreext_hooks.h"
#include "multimissionext_hooks.h"

#include "skirmishdlg_hooks.h"

#include "filepcx_hooks.h"
#include "fetchres_hooks.h"

#include "theatertype_hooks.h"

#include "vinifera_globals.h"
#include "tibsun_functions.h"
#include "iomap.h"
#include "tracker.h"

#include "extension.h"
#include "swizzle.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  This function is for intercepting the calls to Detach_This_From_All to also
 *  process the object through the extension interface.
 * 
 *  @author: CCHyper
 */
static void _Extension_Detach_This_From_All_Intercept(TARGET target, bool all)
{
    Extension::Detach_This_From_All(target, all);

    Detach_This_From_All(target, all);
}


/**
 *  This function is for intercepting the calls to Free_Heaps to also process
 *  the extension interface.
 * 
 *  @author: CCHyper
 */
static void _Extension_Free_Heaps_Intercept()
{
    Extension::Free_Heaps();

    Free_Heaps();
}


/**
 *  This function is for intercepting the call to Clear_Scenarion in Load_All
 *  to flag that we are performing a load operation, which stops the game from
 *  creating extensions while the Windows API calsl the class factories to create
 *  the instances.
 * 
 *  @author: tomsons26
 */
static void _Extension_On_Load_Clear_Scenario_Intercept()
{
    Clear_Scenario();

    /**
     *  Now the scenario data has been cleaned up, we can now tell the extension
     *  hooks that we will be creating the extension classes via the class factories.
     */
    Vinifera_PerformingLoad = true;
}


/**
 *  This patch calls the Print_CRCs function from extension interface.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Extension_Print_CRCs_Hook)
{
    GET_REGISTER_STATIC(FILE *, fp, esi);
    GET_REGISTER_OFFSET_STATIC(EventClass *, ev, esp, 0x174);

    // Fixup WWCRCEngine stack.
    _asm { add esp, 0x4 }

    /**
     *  Print the extension class CRCs.
     */
    Extension::Print_CRCs(fp, ev);

    _asm { push esi }
    _asm { mov eax, 0x006B6944 }
    _asm { call eax } //_fclose

    _asm { add esp, 0x4 }

    JMP(0x005B8464);
}


/**
 *  Patch in the extension class intercept hooks.
 */
static void Extension_Intercept_Hooks()
{
    Patch_Call(0x0053DF7A, &_Extension_Free_Heaps_Intercept); // MapSeedClass::Init_Random
    Patch_Call(0x005DC590, &_Extension_Free_Heaps_Intercept); // Clear_Scenario
    Patch_Call(0x00601BA2, &_Extension_Free_Heaps_Intercept); // Game_Shutdown

    Patch_Call(0x0040DBB3, &_Extension_Detach_This_From_All_Intercept); // AircraftClass::~AircraftClass
    Patch_Call(0x0040F123, &_Extension_Detach_This_From_All_Intercept); // AircraftClass_Fall_To_Death
    Patch_Call(0x0040FCD3, &_Extension_Detach_This_From_All_Intercept); // AircraftTypeClass::~AircraftTypeClass
    Patch_Call(0x00410223, &_Extension_Detach_This_From_All_Intercept); // AircraftTypeClass::~AircraftTypeClass
    Patch_Call(0x004142C6, &_Extension_Detach_This_From_All_Intercept); // AnimClass::~AnimClass
    Patch_Call(0x00426662, &_Extension_Detach_This_From_All_Intercept); // BuildingClass::~BuildingClass
    Patch_Call(0x0043F94D, &_Extension_Detach_This_From_All_Intercept); // BuildingTypeClass::~BuildingTypeClass
    Patch_Call(0x0044407D, &_Extension_Detach_This_From_All_Intercept); // BuildingTypeClass::~BuildingTypeClass
    Patch_Call(0x004445F3, &_Extension_Detach_This_From_All_Intercept); // BulletClass::~BulletClass
    Patch_Call(0x004474D3, &_Extension_Detach_This_From_All_Intercept); // BulletClass::~BulletClass
    Patch_Call(0x00447DC3, &_Extension_Detach_This_From_All_Intercept); // BulletTypeClass::~BulletTypeClass
    Patch_Call(0x00448723, &_Extension_Detach_This_From_All_Intercept); // BulletTypeClass::~BulletTypeClass
    Patch_Call(0x00448AE3, &_Extension_Detach_This_From_All_Intercept); // CampaignClass::~CampaignClass
    Patch_Call(0x00448EF3, &_Extension_Detach_This_From_All_Intercept); // CampaignClass::~CampaignClass
    Patch_Call(0x00456A26, &_Extension_Detach_This_From_All_Intercept); // CellClass::Wall_Update
    Patch_Call(0x00456A58, &_Extension_Detach_This_From_All_Intercept); // CellClass::Wall_Update
    Patch_Call(0x00456A7F, &_Extension_Detach_This_From_All_Intercept); // CellClass::Wall_Update
    Patch_Call(0x00456AAB, &_Extension_Detach_This_From_All_Intercept); // CellClass::Wall_Update
    Patch_Call(0x00456AD2, &_Extension_Detach_This_From_All_Intercept); // CellClass::Wall_Update
    Patch_Call(0x004571F9, &_Extension_Detach_This_From_All_Intercept); // CellClass::Reduce_Wall
    Patch_Call(0x004927D3, &_Extension_Detach_This_From_All_Intercept); // EMPulseClass::~EMPulseClass
    Patch_Call(0x004931E3, &_Extension_Detach_This_From_All_Intercept); // EMPulseClass::~EMPulseClass
    Patch_Call(0x00496DB3, &_Extension_Detach_This_From_All_Intercept); // FactoryClass::~FactoryClass
    Patch_Call(0x00497AA3, &_Extension_Detach_This_From_All_Intercept); // FactoryClass::~FactoryClass
    Patch_Call(0x004BB6DB, &_Extension_Detach_This_From_All_Intercept); // HouseClass::~HouseClass
    Patch_Call(0x004CDE93, &_Extension_Detach_This_From_All_Intercept); // HouseTypeClass::~HouseTypeClass
    Patch_Call(0x004CE603, &_Extension_Detach_This_From_All_Intercept); // HouseTypeClass::~HouseTypeClass
    Patch_Call(0x004D22DC, &_Extension_Detach_This_From_All_Intercept); // InfantryClass::~InfantryClass
    Patch_Call(0x004DA3B4, &_Extension_Detach_This_From_All_Intercept); // InfantryTypeClass::~InfantryTypeClass
    Patch_Call(0x004DB133, &_Extension_Detach_This_From_All_Intercept); // InfantryTypeClass::~InfantryTypeClass
    Patch_Call(0x004F2173, &_Extension_Detach_This_From_All_Intercept); // IsometricTileClass::~IsometricTileClass
    Patch_Call(0x004F23E3, &_Extension_Detach_This_From_All_Intercept); // IsometricTileClass::~IsometricTileClass
    Patch_Call(0x004F3344, &_Extension_Detach_This_From_All_Intercept); // IsometricTileTypeClass::~IsometricTileTypeClass
    Patch_Call(0x005015E3, &_Extension_Detach_This_From_All_Intercept); // LightSourceClass::~LightSourceClass
    Patch_Call(0x00501DA3, &_Extension_Detach_This_From_All_Intercept); // LightSourceClass::~LightSourceClass
    Patch_Call(0x00585F9E, &_Extension_Detach_This_From_All_Intercept); // ObjectClass::Detach_All
    Patch_Call(0x00586DB5, &_Extension_Detach_This_From_All_Intercept); // ObjectClass::entry_E4
    Patch_Call(0x0058B563, &_Extension_Detach_This_From_All_Intercept); // OverlayClass::~OverlayClass
    Patch_Call(0x0058CB13, &_Extension_Detach_This_From_All_Intercept); // OverlayClass::~OverlayClass
    Patch_Call(0x0058D196, &_Extension_Detach_This_From_All_Intercept); // OverlayTypeClass::~OverlayTypeClass
    Patch_Call(0x0058DC86, &_Extension_Detach_This_From_All_Intercept); // OverlayTypeClass::~OverlayTypeClass
    Patch_Call(0x005A32FA, &_Extension_Detach_This_From_All_Intercept); // ParticleClass::~ParticleClass
    Patch_Call(0x005A503A, &_Extension_Detach_This_From_All_Intercept); // ParticleClass::~ParticleClass
    Patch_Call(0x005A56D4, &_Extension_Detach_This_From_All_Intercept); // ParticleSystemClass::~ParticleSystemClass
    Patch_Call(0x005AE573, &_Extension_Detach_This_From_All_Intercept); // ParticleSystemTypeClass::~ParticleSystemTypeClass
    Patch_Call(0x005AEC63, &_Extension_Detach_This_From_All_Intercept); // ParticleSystemTypeClass::~ParticleSystemTypeClass
    Patch_Call(0x005AF153, &_Extension_Detach_This_From_All_Intercept); // ParticleTypeClass::~ParticleTypeClass
    Patch_Call(0x005AFC33, &_Extension_Detach_This_From_All_Intercept); // ParticleTypeClass::~ParticleTypeClass
    Patch_Call(0x005E78C3, &_Extension_Detach_This_From_All_Intercept); // ScriptClass::~ScriptClass
    Patch_Call(0x005E7B83, &_Extension_Detach_This_From_All_Intercept); // ScriptTypeClass::~ScriptTypeClass
    Patch_Call(0x005E81E3, &_Extension_Detach_This_From_All_Intercept); // ScriptClass::~ScriptClass
    Patch_Call(0x005E8293, &_Extension_Detach_This_From_All_Intercept); // ScriptTypeClass::~ScriptTypeClass
    Patch_Call(0x005F1AE3, &_Extension_Detach_This_From_All_Intercept); // SideClass::~SideClass
    Patch_Call(0x005F1D93, &_Extension_Detach_This_From_All_Intercept); // SideClass::~SideClass
    Patch_Call(0x005FAAD3, &_Extension_Detach_This_From_All_Intercept); // SmudgeClass::~SmudgeClass
    Patch_Call(0x005FAF03, &_Extension_Detach_This_From_All_Intercept); // SmudgeClass::~SmudgeClass
    Patch_Call(0x005FB313, &_Extension_Detach_This_From_All_Intercept); // SmudgeTypeClass::~SmudgeTypeClass
    Patch_Call(0x005FC023, &_Extension_Detach_This_From_All_Intercept); // SmudgeTypeClass::~SmudgeTypeClass
    Patch_Call(0x00618D03, &_Extension_Detach_This_From_All_Intercept); // TActionClass::~TActionClass
    Patch_Call(0x0061DAD3, &_Extension_Detach_This_From_All_Intercept); // TActionClass::~TActionClass
    Patch_Call(0x0061E4B6, &_Extension_Detach_This_From_All_Intercept); // TagClass::~TagClass
    Patch_Call(0x0061E73B, &_Extension_Detach_This_From_All_Intercept); // TagClass::~TagClass
    Patch_Call(0x0061E9AA, &_Extension_Detach_This_From_All_Intercept); // TagClass::Spring
    Patch_Call(0x0061F164, &_Extension_Detach_This_From_All_Intercept); // TagTypeClass::~TagTypeClass
    Patch_Call(0x00621503, &_Extension_Detach_This_From_All_Intercept); // TaskForceClass::~TaskForceClass
    Patch_Call(0x00621E43, &_Extension_Detach_This_From_All_Intercept); // TaskForceClass::~TaskForceClass
    Patch_Call(0x006224E3, &_Extension_Detach_This_From_All_Intercept); // TeamClass::~TeamClass
    Patch_Call(0x00627EF3, &_Extension_Detach_This_From_All_Intercept); // TeamTypeClass::~TeamTypeClass
    Patch_Call(0x00629293, &_Extension_Detach_This_From_All_Intercept); // TeamTypeClass::~TeamTypeClass
    Patch_Call(0x0063F188, &_Extension_Detach_This_From_All_Intercept); // TerrainClass::~TerrainClass
    Patch_Call(0x00640C38, &_Extension_Detach_This_From_All_Intercept); // TerrainClass::~TerrainClass
    Patch_Call(0x00641653, &_Extension_Detach_This_From_All_Intercept); // TerrainTypeClass::~TerrainTypeClass
    Patch_Call(0x00641D83, &_Extension_Detach_This_From_All_Intercept); // TerrainTypeClass::~TerrainTypeClass
    Patch_Call(0x00642223, &_Extension_Detach_This_From_All_Intercept); // TEventClass::~TEventClass
    Patch_Call(0x00642F23, &_Extension_Detach_This_From_All_Intercept); // TEventClass::~TEventClass
    Patch_Call(0x00644A45, &_Extension_Detach_This_From_All_Intercept); // TiberiumClass::~TiberiumClass
    Patch_Call(0x006491A3, &_Extension_Detach_This_From_All_Intercept); // TriggerClass::~TriggerClass
    Patch_Call(0x00649943, &_Extension_Detach_This_From_All_Intercept); // TriggerClass::~TriggerClass
    Patch_Call(0x00649E03, &_Extension_Detach_This_From_All_Intercept); // TriggerTypeClass::~TriggerTypeClass
    Patch_Call(0x0064AFD3, &_Extension_Detach_This_From_All_Intercept); // TubeClass::~TubeClass
    Patch_Call(0x0064B603, &_Extension_Detach_This_From_All_Intercept); // TubeClass::~TubeClass
    Patch_Call(0x0064D8A9, &_Extension_Detach_This_From_All_Intercept); // UnitClass::~UnitClass
    Patch_Call(0x0065BAD3, &_Extension_Detach_This_From_All_Intercept); // UnitTypeClass::~UnitTypeClass
    Patch_Call(0x0065C793, &_Extension_Detach_This_From_All_Intercept); // UnitTypeClass::~UnitTypeClass
    Patch_Call(0x0065DF23, &_Extension_Detach_This_From_All_Intercept); // VoxelAnimClass::~VoxelAnimClass
    Patch_Call(0x0065F5A3, &_Extension_Detach_This_From_All_Intercept); // VoxelAnimTypeClass::~VoxelAnimTypeClass
    Patch_Call(0x00660093, &_Extension_Detach_This_From_All_Intercept); // VoxelAnimTypeClass::~VoxelAnimTypeClass
    Patch_Call(0x00661227, &_Extension_Detach_This_From_All_Intercept); // VeinholeMonsterClass::~VeinholeMonsterClass
    Patch_Call(0x00661C00, &_Extension_Detach_This_From_All_Intercept); // VeinholeMonsterClass::Take_Damage
    Patch_Call(0x0066EF73, &_Extension_Detach_This_From_All_Intercept); // WarheadTypeClass::~WarheadTypeClass
    Patch_Call(0x0066FA93, &_Extension_Detach_This_From_All_Intercept); // WarheadTypeClass::~WarheadTypeClass
    Patch_Call(0x006702D4, &_Extension_Detach_This_From_All_Intercept); // WaveClass::~WaveClass
    Patch_Call(0x00672E73, &_Extension_Detach_This_From_All_Intercept); // WaveClass::~WaveClass
    Patch_Call(0x00673563, &_Extension_Detach_This_From_All_Intercept); // WaypointPathClass::~WaypointPathClass
    Patch_Call(0x00673AA3, &_Extension_Detach_This_From_All_Intercept); // WaypointPathClass::~WaypointPathClass
    Patch_Call(0x00680C54, &_Extension_Detach_This_From_All_Intercept); // WeaponTypeClass::~WeaponTypeClass
    Patch_Call(0x006818F4, &_Extension_Detach_This_From_All_Intercept); // WeaponTypeClass::~WeaponTypeClass

    Patch_Call(0x005D6BEC, &_Extension_On_Load_Clear_Scenario_Intercept); // Load_All

    Patch_Jump(0x005B845B, &_Extension_Print_CRCs_Hook);
}


void Extension_Hooks()
{
    Extension_Intercept_Hooks();

    /**
     *  Abstract and stack class extensions here.
     */
    AbstractClassExtension_Hooks();

    /**
     *  All game type class extensions here.
     */
    ObjectTypeClassExtension_Hooks();
    TechnoTypeClassExtension_Hooks();

    /**
     *  All game class extensions here.
     */
    TechnoClassExtension_Hooks();
    FootClassExtension_Hooks();

    UnitClassExtension_Hooks();
    AircraftClassExtension_Hooks();
    AircraftTypeClassExtension_Hooks();
    AnimClassExtension_Hooks();
    AnimTypeClassExtension_Hooks();
    BuildingClassExtension_Hooks();
    BuildingTypeClassExtension_Hooks();
    BulletClassExtension_Hooks();
    BulletTypeClassExtension_Hooks();
    CampaignClassExtension_Hooks();
    CellClassExtension_Hooks();
    FactoryClassExtension_Hooks();
    HouseClassExtension_Hooks();
    HouseTypeClassExtension_Hooks();
    InfantryClassExtension_Hooks();
    InfantryTypeClassExtension_Hooks();
    //IsometricTileClassExtension_Hooks();                  // <- Not yet implemented
    IsometricTileTypeClassExtension_Hooks();
    //BuildingLightExtension_Hooks();                       // <- Not yet implemented
    //OverlayClassExtension_Hooks();                        // <- Not yet implemented
    OverlayTypeClassExtension_Hooks();
    //ParticleClassExtension_Hooks();                       // <- Not yet implemented
    ParticleTypeClassExtension_Hooks();
    ParticleSystemClassExtension_Hooks();
    ParticleSystemTypeClassExtension_Hooks();
    //ScriptClassExtension_Hooks();                         // <- Not yet implemented
    //ScriptTypeClassExtension_Hooks();                     // <- Not yet implemented
    SideClassExtension_Hooks();
    //SmudgeClassExtension_Hooks();                         // <- Not yet implemented
    SmudgeTypeClassExtension_Hooks();
    SuperWeaponTypeClassExtension_Hooks();
    //TaskForceClassExtension_Hooks();                      // <- Not yet implemented
    TeamClassExtension_Hooks();
    //TeamTypeClassExtension_Hooks();                       // <- Not yet implemented
    TerrainClassExtension_Hooks();
    TerrainTypeClassExtension_Hooks();
    //TriggerTypeExtension_Hooks();                         // <- Not yet implemented
    //TriggerTypeClassExtension_Hooks();                    // <- Not yet implemented
    UnitTypeClassExtension_Hooks();
    //VoxelAnimClassExtension_Hooks();                      // <- Not yet implemented
    VoxelAnimTypeClassExtension_Hooks();
    WaveClassExtension_Hooks();
    //TagClassExtension_Hooks();                            // <- Not yet implemented
    //TagTypeClassExtension_Hooks();                        // <- Not yet implemented
    TiberiumClassExtension_Hooks();
    //TActionClassExtension_Hooks();                        // <- Not yet implemented
    //TEventClassExtension_Hooks();                         // <- Not yet implemented
    WeaponTypeClassExtension_Hooks();
    WarheadTypeClassExtension_Hooks();
    //WaypointClassExtension_Hooks();                       // <- Not yet implemented
    //TubeClassExtension_Hooks();                           // <- Not yet implemented
    //LightSourceClassExtension_Hooks();                    // <- Not yet implemented
    EMPulseClassExtension_Hooks();
    TacticalExtension_Hooks();
    SuperClassExtension_Hooks();
    //AITriggerClassExtension_Hooks();                      // <- Not yet implemented
    //AITriggerTypeClassExtension_Hooks();                  // <- Not yet implemented
    //NeuronClassExtension_Hooks();                         // <- Not yet implemented
    //FoggedObjectClassExtension_Hooks();                   // <- Not yet implemented
    //AlphaShapeClassExtension_Hooks();                     // <- Not yet implemented
    //VeinholeMonsterClassExtension_Hooks();                // <- Not yet implemented

    /**
     *  All global class extensions here.
     */
    RulesClassExtension_Hooks();
    ScenarioClassExtension_Hooks();
    SessionClassExtension_Hooks();
    OptionsClassExtension_Hooks();

    ThemeClassExtension_Hooks();

    DisplayClassExtension_Hooks();
    SidebarClassExtension_Hooks();

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

    /**
     *  New classes and interfaces.
     */
    TheaterTypeClassExtension_Hooks();
}
