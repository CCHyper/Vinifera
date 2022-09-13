/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          EXT_HOOKS.CPP
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

#include "iomap.h"

/**
 *  Extended classes here.
 */
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

#include "objecttypeext_hooks.h"
#include "technotypeext_hooks.h"
#include "buildingtypeext_hooks.h"
#include "unittypeext_hooks.h"
#include "infantrytypeext_hooks.h"
#include "aircrafttypeext_hooks.h"
#include "warheadtypeext_hooks.h"
#include "weapontypeext_hooks.h"
#include "bullettypeext_hooks.h"
#include "supertypeext_hooks.h"
#include "voxelanimtypeext_hooks.h"
#include "animtypeext_hooks.h"
#include "particletypeext_hooks.h"
#include "particlesystypeext_hooks.h"
#include "isotiletypeext_hooks.h"
#include "overlaytypeext_hooks.h"
#include "smudgetypeext_hooks.h"
#include "terraintypeext_hooks.h"
#include "housetypeext_hooks.h"
#include "sideext_hooks.h"
#include "campaignext_hooks.h"
#include "tiberiumext_hooks.h"
//#include "taskforceext_hooks.h"
//#include "aitrigtypeext_hooks.h"
//#include "scripttypeext_hooks.h"
//#include "tagtypeext_hooks.h"
//#include "triggertypeext_hooks.h"

#include "technoext_hooks.h"
#include "footext_hooks.h"

#include "unitext_hooks.h"
#include "buildingext_hooks.h"
#include "aircraftext_hooks.h"
#include "infantryext_hooks.h"
#include "cellext_hooks.h"
#include "houseext_hooks.h"
#include "teamext_hooks.h"
#include "tactionext_hooks.h"
#include "factoryext_hooks.h"
#include "animext_hooks.h"
#include "bulletext_hooks.h"
#include "terrainext_hooks.h"
#include "superext_hooks.h"
#include "particlesysext_hooks.h"

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

#include "extension.h"
#include "swizzle.h"

#include "hooker.h"
#include "hooker_macros.h"


extern bool Vinifera_ClassExtensionsDisabled;


/**
 *  This patch clears the DWORD at 0x10 (0x10 is "bool IsDirty") to use the space
 *  for storing a pointer to the extension class instance for this AbstractClass.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AbstractClass_Constructor_Extension)
{
    _asm { mov eax, ecx }
    _asm { xor ecx, ecx }

    _asm { mov [eax+0x8], 0xFFFFFFFF } // ID
    _asm { mov [eax+0x0C], ecx } // HeapID

    _asm { mov [eax+0x10], ecx } // IsDirty, now reused as a extension pointer, so we need to clear the DWORD.

    _asm { mov [eax+0x0], 0x006CAA6C } // offset const AbstractClass::`vftable'
    _asm { mov [eax+0x4], 0x006CAA54 } // offset const AbstractClass::`vftable' for IRTTITypeInfo

    _asm { retn }
}


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or deconstructor.
 * 
 *  @note: All functions must not be virtual and must also be prefixed
 *         with "_" to prevent accidental virtualization.
 */
class AbstractClassExt : public AbstractClass
{
    public:
        IFACEMETHOD_(LONG, IsDirty)();
};


/**
 *  This patch forces AbstractClass::IsDirty() to return true.
 * 
 *  @author: CCHyper
 */
LONG STDMETHODCALLTYPE AbstractClassExt::IsDirty()
{
    return TRUE;
}


static void Extension_Abstract_Hooks()
{
    Patch_Jump(0x00405B50, &_AbstractClass_Constructor_Extension);

    Patch_Jump(0x00405E00, &AbstractClassExt::Is_Dirty);

    /**
     *  Removes the branch from AbstractClass::Abstract_Save which clears IsDirty.
     */
    Patch_Byte_Range(0x00405CF8, 0x90, 12);
}


void Extension_Hooks()
{
    if (!Vinifera_ClassExtensionsDisabled) {

        /**
         *  All game class extensions here.
         */
        Extension_Abstract_Hooks();

        ObjectTypeClassExtension_Hooks();
        TechnoTypeClassExtension_Hooks();

        TechnoClassExtension_Hooks();
        FootClassExtension_Hooks();

        /**
         *  All game class extensions here.
         */
//        UnitClassExtension_Hooks();
//        AircraftClassExtension_Hooks();
//        AircraftTypeClassExtension_Hooks();
//        AnimClassExtension_Hooks();
//        AnimTypeClassExtension_Hooks();
        BuildingClassExtension_Hooks();
        BuildingTypeClassExtension_Hooks();
//        BulletClassExtension_Hooks();
//        BulletTypeClassExtension_Hooks();
//        CampaignClassExtension_Hooks();
//        CellClassExtension_Hooks();
//        FactoryClassExtension_Hooks();
//        HouseClassExtension_Hooks();
//        HouseTypeClassExtension_Hooks();
//        InfantryClassExtension_Hooks();
//        InfantryTypeClassExtension_Hooks();
        //EXT_RTTI_ISOTILE
//        IsometricTileTypeClassExtension_Hooks();
        //EXT_RTTI_LIGHT
        //EXT_RTTI_OVERLAY
//        OverlayTypeClassExtension_Hooks();
        //EXT_RTTI_PARTICLE
//        ParticleTypeClassExtension_Hooks();
//        ParticleSystemClassExtension_Hooks();
//        ParticleSystemTypeClassExtension_Hooks();
        //EXT_RTTI_SCRIPT
        //ScriptTypeClassExtension_Hooks();
//        SideClassExtension_Hooks();
        //EXT_RTTI_SMUDGE
//        SmudgeTypeClassExtension_Hooks();
        //EXT_RTTI_SPECIAL
//        SuperWeaponTypeClassExtension_Hooks();
        //TaskForceClassExtension_Hooks();
//        TeamClassExtension_Hooks();
        //EXT_RTTI_TEAMTYPE
//        TerrainClassExtension_Hooks();
//        TerrainTypeClassExtension_Hooks();
        //EXT_RTTI_TRIGGER
        //TriggerTypeClassExtension_Hooks();
//        UnitTypeClassExtension_Hooks();
        //EXT_RTTI_VOXELANIM
//        VoxelAnimTypeClassExtension_Hooks();
//        WaveClassExtension_Hooks();
        //EXT_RTTI_TAG
        //TagTypeClassExtension_Hooks();
//        TiberiumClassExtension_Hooks();
//        TActionClassExtension_Hooks();
        //EXT_RTTI_EVENT
//        WeaponTypeClassExtension_Hooks();
//        WarheadTypeClassExtension_Hooks();
        //EXT_RTTI_WAYPOINT
        //EXT_RTTI_TUBE
        //EXT_RTTI_LIGHTSOURCE
//        EMPulseClassExtension_Hooks();
        //EXT_RTTI_TACTICALMAP
//        SuperClassExtension_Hooks();
        //EXT_RTTI_AITRIGGER
        //AITriggerTypeClassExtension_Hooks();
        //EXT_RTTI_NEURON
        //EXT_RTTI_FOGGEDOBJECT
        //EXT_RTTI_ALPHASHAPE
        //EXT_RTTI_VEINHOLEMONSTER

        /**
         *  All global class extensions here.
         */
//        RulesClassExtension_Hooks();
//        TacticalExtension_Hooks();
//        ScenarioClassExtension_Hooks();
//        DisplayClassExtension_Hooks();
//        SidebarClassExtension_Hooks();
//        OptionsClassExtension_Hooks();
//        SessionClassExtension_Hooks();
//        ThemeClassExtension_Hooks();
    }

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
