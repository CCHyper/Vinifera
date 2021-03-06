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
#include "ext_hooks.h"
#include "saveload_hooks.h"
#include "iomap.h"

/**
 *  Extended classes here.
 */
#include "tacticalext_hooks.h"
#include "commandext_hooks.h"

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

#include "hooker.h"
#include "hooker_macros.h"


void Extension_Hooks()
{
    /**
     *  Hook the new save and load system in.
     */
    SaveLoad_Hooks();

    /**
     *  All class extensions here.
     */
    TacticalExtension_Hooks();
    CommandExtension_Hooks();

    /**
     *  All type class extensions here.
     */
    ObjectTypeClassExtension_Hooks();
    TechnoTypeClassExtension_Hooks();
    BuildingTypeClassExtension_Hooks();
    UnitTypeClassExtension_Hooks();
    InfantryTypeClassExtension_Hooks();
    AircraftTypeClassExtension_Hooks();
    WarheadTypeClassExtension_Hooks();
    WeaponTypeClassExtension_Hooks();
    BulletTypeClassExtension_Hooks();
    SuperWeaponTypeClassExtension_Hooks();
    VoxelAnimTypeClassExtension_Hooks();
    AnimTypeClassExtension_Hooks();
    ParticleTypeClassExtension_Hooks();
    ParticleSystemTypeClassExtension_Hooks();
    IsometricTileTypeClassExtension_Hooks();
    OverlayTypeClassExtension_Hooks();
    SmudgeTypeClassExtension_Hooks();
    TerrainTypeClassExtension_Hooks();
    HouseTypeClassExtension_Hooks();
    SideClassExtension_Hooks();
    CampaignClassExtension_Hooks();
    TiberiumClassExtension_Hooks();
    //TaskForceClassExtension_Hooks();
    //AITriggerTypeClassExtension_Hooks();
    //ScriptTypeClassExtension_Hooks();
    //TagTypeClassExtension_Hooks();
    //TriggerTypeClassExtension_Hooks();
}
