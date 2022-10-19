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






void Extension_Hooks()
{
    /**
     *  Abstract and stack class extensions here.
     */
    AbstractClassExtension_Hooks();

//    if (Is_Extension_Support_Enabled(RTTI_UNIT)
//     || Is_Extension_Support_Enabled(RTTI_INFANTRY)
//     || Is_Extension_Support_Enabled(RTTI_BUILDING)
//     || Is_Extension_Support_Enabled(RTTI_AIRCRAFT)) {
//
//        TechnoClassExtension_Hooks();
//        FootClassExtension_Hooks();
//    }
//
//    if (Is_Extension_Support_Enabled(RTTI_UNITTYPE)
//     || Is_Extension_Support_Enabled(RTTI_INFANTRYTYPE)
//     || Is_Extension_Support_Enabled(RTTI_BUILDINGTYPE)
//     || Is_Extension_Support_Enabled(RTTI_AIRCRAFTTYPE)) {
//
//        ObjectTypeClassExtension_Hooks();
//        TechnoTypeClassExtension_Hooks();
//    }

    /**
     *  All game class extensions here.
     */
    if (Is_Extension_Support_Enabled(RTTI_UNIT)) { UnitClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFT)) { AircraftClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_AIRCRAFTTYPE)) { AircraftTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_ANIM)) { AnimClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_ANIMTYPE)) { AnimTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_BUILDING)) { BuildingClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_BUILDINGTYPE)) { BuildingTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_BULLET)) { BulletClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_BULLETTYPE)) { BulletTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_CAMPAIGN)) { CampaignClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_CELL)) { CellClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_FACTORY)) { FactoryClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_HOUSE)) { HouseClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_HOUSETYPE)) { HouseTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRY)) { InfantryClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_INFANTRYTYPE)) { InfantryTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_ISOTILE)) { EXT_RTTI_ISOTILE; }
    if (Is_Extension_Support_Enabled(RTTI_ISOTILETYPE)) { IsometricTileTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_LIGHT)) { EXT_RTTI_LIGHT; }
    if (Is_Extension_Support_Enabled(RTTI_OVERLAY)) { EXT_RTTI_OVERLAY; }
    if (Is_Extension_Support_Enabled(RTTI_OVERLAYTYPE)) { OverlayTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLE)) { EXT_RTTI_PARTICLE; }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLETYPE)) { ParticleTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLESYSTEM)) { ParticleSystemClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_PARTICLESYSTEMTYPE)) { ParticleSystemTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_SCRIPT)) { EXT_RTTI_SCRIPT; }
    if (Is_Extension_Support_Enabled(RTTI_SCRIPTTYPE)) { EXT_RTTI_SCRIPTTYPE; }
    if (Is_Extension_Support_Enabled(RTTI_SIDE)) { SideClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_SMUDGE)) { EXT_RTTI_SMUDGE; }
    if (Is_Extension_Support_Enabled(RTTI_SMUDGETYPE)) { SmudgeTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPONTYPE)) { SuperWeaponTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_TASKFORCE)) { EXT_RTTI_TASKFORCE; }
    if (Is_Extension_Support_Enabled(RTTI_TEAM)) { TeamClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_TEAMTYPE)) { EXT_RTTI_TEAMTYPE; }
    if (Is_Extension_Support_Enabled(RTTI_TERRAIN)) { TerrainClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_TERRAINTYPE)) { TerrainTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_TRIGGER)) { EXT_RTTI_TRIGGER; }
    if (Is_Extension_Support_Enabled(RTTI_TRIGGERTYPE)) { EXT_RTTI_TRIGGERTYPE; }
    if (Is_Extension_Support_Enabled(RTTI_UNITTYPE)) { UnitTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_VOXELANIM)) { EXT_RTTI_VOXELANIM; }
    if (Is_Extension_Support_Enabled(RTTI_VOXELANIMTYPE)) { VoxelAnimTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_WAVE)) { WaveClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_TAG)) { EXT_RTTI_TAG; }
    if (Is_Extension_Support_Enabled(RTTI_TAGTYPE)) { EXT_RTTI_TAGTYPE; }
    if (Is_Extension_Support_Enabled(RTTI_TIBERIUM)) { TiberiumClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_ACTION)) { TActionClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_EVENT)) { EXT_RTTI_EVENT; }
    if (Is_Extension_Support_Enabled(RTTI_WEAPONTYPE)) { WeaponTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_WARHEADTYPE)) { WarheadTypeClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_WAYPOINT)) { EXT_RTTI_WAYPOINT; }
    if (Is_Extension_Support_Enabled(RTTI_TUBE)) { EXT_RTTI_TUBE; }
    if (Is_Extension_Support_Enabled(RTTI_LIGHTSOURCE)) { EXT_RTTI_LIGHTSOURCE; }
    if (Is_Extension_Support_Enabled(RTTI_EMPULSE)) { EMPulseClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_TACTICALMAP)) { EXT_RTTI_TACTICALMAP; }
    if (Is_Extension_Support_Enabled(RTTI_SUPERWEAPON)) { SuperClassExtension_Hooks(); }
    if (Is_Extension_Support_Enabled(RTTI_AITRIGGER)) { EXT_RTTI_AITRIGGER; }
    if (Is_Extension_Support_Enabled(RTTI_AITRIGGERTYPE)) { EXT_RTTI_AITRIGGERTYPE; }
    if (Is_Extension_Support_Enabled(RTTI_NEURON)) { EXT_RTTI_NEURON; }
    if (Is_Extension_Support_Enabled(RTTI_FOGGEDOBJECT)) { EXT_RTTI_FOGGEDOBJECT; }
    if (Is_Extension_Support_Enabled(RTTI_ALPHASHAPE)) { EXT_RTTI_ALPHASHAPE; }
    if (Is_Extension_Support_Enabled(RTTI_VEINHOLEMONSTER)) { EXT_RTTI_VEINHOLEMONSTER; }

    /**
     *  All global class extensions here.
     */
//    RulesClassExtension_Hooks();
//    TacticalExtension_Hooks();
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

    //CombatExtension_Hooks();
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
