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
    //TechnoClassExtension_Hooks();
    //FootClassExtension_Hooks();

    //ObjectTypeClassExtension_Hooks();
    //TechnoTypeClassExtension_Hooks();

    /**
     *  All game class extensions here.
     */
//    UnitClassExtension_Hooks();
//    AircraftClassExtension_Hooks();
//    AircraftTypeClassExtension_Hooks();
//    AnimClassExtension_Hooks();
//    AnimTypeClassExtension_Hooks();
    BuildingClassExtension_Hooks();
    BuildingTypeClassExtension_Hooks();
//    BulletClassExtension_Hooks();
//    BulletTypeClassExtension_Hooks();
//    CampaignClassExtension_Hooks();
//    CellClassExtension_Hooks();
//    FactoryClassExtension_Hooks();
//    HouseClassExtension_Hooks();
//    HouseTypeClassExtension_Hooks();
//    InfantryClassExtension_Hooks();
//    InfantryTypeClassExtension_Hooks();
    //EXT_RTTI_ISOTILE
//    IsometricTileTypeClassExtension_Hooks();
    //EXT_RTTI_LIGHT
    //EXT_RTTI_OVERLAY
//    OverlayTypeClassExtension_Hooks();
    //EXT_RTTI_PARTICLE
//    ParticleTypeClassExtension_Hooks();
//    ParticleSystemClassExtension_Hooks();
//    ParticleSystemTypeClassExtension_Hooks();
    //EXT_RTTI_SCRIPT
    //ScriptTypeClassExtension_Hooks();
//    SideClassExtension_Hooks();
    //EXT_RTTI_SMUDGE
//    SmudgeTypeClassExtension_Hooks();
    //EXT_RTTI_SPECIAL
//    SuperWeaponTypeClassExtension_Hooks();
    //EXT_RTTI_TASKFORCE
//    TeamClassExtension_Hooks();
    //EXT_RTTI_TEAMTYPE
//    TerrainClassExtension_Hooks();
//    TerrainTypeClassExtension_Hooks();
    //EXT_RTTI_TRIGGER
    //TriggerTypeClassExtension_Hooks();
//    UnitTypeClassExtension_Hooks();
    //EXT_RTTI_VOXELANIM
//    VoxelAnimTypeClassExtension_Hooks();
//    WaveClassExtension_Hooks();
    //EXT_RTTI_TAG
    //TagTypeClassExtension_Hooks();
//    TiberiumClassExtension_Hooks();
//    TActionClassExtension_Hooks();
    //EXT_RTTI_EVENT
//    WeaponTypeClassExtension_Hooks();
//    WarheadTypeClassExtension_Hooks();
    //EXT_RTTI_WAYPOINT
    //EXT_RTTI_TUBE
    //EXT_RTTI_LIGHTSOURCE
//    EMPulseClassExtension_Hooks();
    //EXT_RTTI_TACTICALMAP
//    SuperClassExtension_Hooks();
    //EXT_RTTI_AITRIGGER
    //AITriggerTypeClassExtension_Hooks();
    //EXT_RTTI_NEURON
    //EXT_RTTI_FOGGEDOBJECT
    //EXT_RTTI_ALPHASHAPE
    //EXT_RTTI_VEINHOLEMONSTER

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
