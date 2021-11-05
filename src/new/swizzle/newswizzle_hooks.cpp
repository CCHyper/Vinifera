/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          NEWSWIZZLE_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for implementing the new swizzle manager.
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
#include "tibsun_globals.h"
#include "saveload.h"
#include "swizzle.h"
#include "newswizzle.h"
#include "vinifera_util.h"
#include "debughandler.h"
#include "asserthandler.h"
#include "fatal.h"

#include "hooker.h"
#include "hooker_macros.h"


DEFINE_IMPLEMENTATION(unsigned int String_To_Hex(char *str), 0x004082E0);
DEFINE_IMPLEMENTATION(unsigned int sub_627D30(char *str), 0x00627D30);


/**
 *  The original Tiberian Sun source tree path.
 */
#define TIBSUN_SOURCE_PATH "D:\\Projects\\Sun\\CodeFS\\"


/**
 *  Handy macros for defining the Swizzle patches.
 * 
 *  @author: CCHyper
 */
#define RESET_PATCH(label, func, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        _asm { pushad } \
        VINIFERA_SWIZZLE_RESET(func); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define HERE_I_AM_PATCH(label, id_reg, ptr_reg, func, var, file, line, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(LONG, id, id_reg); \
        GET_REGISTER_STATIC(void **, pointer, ptr_reg); \
        _asm { pushad } \
        VINIFERA_SWIZZLE_HERE_I_AM_DBG(id, pointer, TIBSUN_SOURCE_PATH file, line, func, var); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define FETCH_SWIZZLE_ID_PATCH(label, ptr_reg, id_reg, func, var, file, line, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(uintptr_t, pointer, ptr_reg); \
        GET_REGISTER_STATIC(LONG, id, id_reg); \
        _asm { pushad } \
        VINIFERA_SWIZZLE_FETCH_POINTER_ID_DBG(pointer, &id, TIBSUN_SOURCE_PATH file, line, func, var); \
        _asm { popad } \
        JMP(ret_addr); \
    }

#define SWIZZLE_PATCH(label, ptr_reg, func, var, file, line, ret_addr) \
    DECLARE_PATCH(label) \
    { \
        GET_REGISTER_STATIC(uintptr_t, pointer, ptr_reg); \
        _asm { pushad } \
        VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_DBG(pointer, TIBSUN_SOURCE_PATH file, line, func, var); \
        _asm { popad } \
        JMP(ret_addr); \
    }


/**
 *  The following definitions create intercept patches for various Swizzle calls.
 */
RESET_PATCH(_Load_Game_Swizzle_Reset_Pre_Patch, "Pre Load_Game", 0x005D69C7);
RESET_PATCH(_Load_Game_Swizzle_Reset_Post_Patch, "Post Load_Game", 0x005D6B43);
RESET_PATCH(_Read_Scenario_INI_Swizzle_Reset_Start_Patch, "Start of Read_Scenario_INI", 0x005DD668); // 0x005DD65E
RESET_PATCH(_Read_Scenario_INI_Swizzle_Reset_End_Patch, "End of Read_Scenario_INI", 0x005DDF07); // 0x005DDEFD

HERE_I_AM_PATCH(_AbstractClass_Abstract_Load_Here_I_Am_Patch, edx, esi, STRINGIZE(AbstractClass::Abstract_Load()), "this", "Abstract.cpp", 1, 0x00405D4C);
HERE_I_AM_PATCH(_HouseTypeClass_Load_Here_I_Am_Patch, ecx, edi, STRINGIZE(HouseTypeClass::Load()), "this", "HouseType.cpp", 1, 0x004CE38C);
HERE_I_AM_PATCH(_LocomotionClass_Load_Here_I_Am_Patch, edx, esi, STRINGIZE(LocomotionClass::Load()), "this", "Locomotion.cpp", 1, 0x0050662C);

DECLARE_PATCH(_TagTypeClass_Read_Scenario_INI_Here_I_Am_Patch)
{
    //GET_REGISTER_STATIC(char *, tagtypename, edi);
    GET_REGISTER_STATIC(void **, pointer, esi);
    static unsigned int id;
    _asm { mov ecx, edi }
    _asm { mov eax, 0x004082E0 } // String_To_Uint32
    _asm { call eax }
    _asm { mov id, eax }
    _asm { pushad }
    //id = String_To_Hex(tagtypename);
    VINIFERA_SWIZZLE_HERE_I_AM_DBG(id, pointer, TIBSUN_SOURCE_PATH "TagType.cpp", 1, STRINGIZE(TagTypeClass::Read_Scenario_INI()), "tagtypeptr");
    _asm { popad }
    JMP(0x0061F433);
}

DECLARE_PATCH(_TaskForceClass_Read_Scenario_INI_Here_I_Am_Patch)
{
    //GET_STACK_STATIC(char *, taskforcename, esp, 0x20);
    GET_REGISTER_STATIC(void **, pointer, esi);
    static unsigned int id;
    _asm { lea ecx, [esp+0x24] }
    _asm { mov eax, 0x004082E0 } // String_To_Uint32
    _asm { call eax }
    _asm { mov id, eax }
    _asm { pushad }
    //id = String_To_Hex(taskforcename);
    VINIFERA_SWIZZLE_HERE_I_AM_DBG(id, pointer, TIBSUN_SOURCE_PATH "TaskForce.cpp", 1, STRINGIZE(TaskForceClass::Read_Scenario_INI()), "taskforceptr");
    _asm { popad }
    JMP(0x00621912);
}

HERE_I_AM_PATCH(_VeinholeMonsterClass_Load_Here_I_Am_Patch, edx, ebp, STRINGIZE(VeinholeMonsterClass::Load()), "veinholeptr", "Veinhole.cpp", 1, 0x00663010);

FETCH_SWIZZLE_ID_PATCH(_HouseTypeClass_Save_Fetch_Swizzle_ID_Patch, edi, eax, STRINGIZE(HouseTypeClass::Save()), "this", "HouseType.cpp", 1, 0x004CE3C8);

SWIZZLE_PATCH(_AircraftClass_Load_Swizzle_Patch, ebx, STRINGIZE(AircraftClass::Load()), STRINGIZE(AircraftClass::Class), "Aircraft.cpp", 1, 0x0040ECBE);
SWIZZLE_PATCH(_AITriggerTypeClass_Load_Swizzle_Patch_1, edx, STRINGIZE(AITriggerTypeClass::Load()), STRINGIZE(AITriggerTypeClass::ConditionObject), "AITrigType.cpp", 1, 0x004106E2);
SWIZZLE_PATCH(_AITriggerTypeClass_Load_Swizzle_Patch_2, eax, STRINGIZE(AITriggerTypeClass::Load()), STRINGIZE(AITriggerTypeClass::TeamOne), "AITrigType.cpp", 2, 0x004106F3);
SWIZZLE_PATCH(_AITriggerTypeClass_Load_Swizzle_Patch_3, esi, STRINGIZE(AITriggerTypeClass::Load()), STRINGIZE(AITriggerTypeClass::TeamTwo), "AITrigType.cpp", 3, 0x00410704);
SWIZZLE_PATCH(_AlphaShapeClass_Load_Swizzle_Patch, edx, STRINGIZE(AlphaShapeClass::Load()), STRINGIZE(AlphaShapeClass::), "AlphaShape.cpp", 1, 0x00412A32);
SWIZZLE_PATCH(_AnimClass_Load_Swizzle_Patch_1, edx, STRINGIZE(AnimClass::Load()), STRINGIZE(AnimClass::Class), "Anim.cpp", 1, 0x004164F2);
SWIZZLE_PATCH(_AnimClass_Load_Swizzle_Patch_2, eax, STRINGIZE(AnimClass::Load()), STRINGIZE(AnimClass::xObject), "Anim.cpp", 2, 0x00416500);
SWIZZLE_PATCH(_AnimTypeClass_Load_Swizzle_Patch_1, edx, STRINGIZE(AnimTypeClass::Load()), STRINGIZE(AnimTypeClass::ChainTo), "AnimType.cpp", 1, 0x0041969D);
SWIZZLE_PATCH(_AnimTypeClass_Load_Swizzle_Patch_2, eax, STRINGIZE(AnimTypeClass::Load()), STRINGIZE(AnimTypeClass::ExpireAnim), "AnimType.cpp", 2, 0x004196AE);
SWIZZLE_PATCH(_AnimTypeClass_Load_Swizzle_Patch_3, ecx, STRINGIZE(AnimTypeClass::Load()), STRINGIZE(AnimTypeClass::TrailerAnim), "AnimType.cpp", 3, 0x004196BF);
SWIZZLE_PATCH(_AnimTypeClass_Load_Swizzle_Patch_4, edx, STRINGIZE(AnimTypeClass::Load()), STRINGIZE(AnimTypeClass::BounceAnim), "AnimType.cpp", 4, 0x004196D0);
SWIZZLE_PATCH(_AnimTypeClass_Load_Swizzle_Patch_5, eax, STRINGIZE(AnimTypeClass::Load()), STRINGIZE(AnimTypeClass::Spawns), "AnimType.cpp", 5, 0x004196E1);
SWIZZLE_PATCH(_AnimTypeClass_Load_Swizzle_Patch_6, ecx, STRINGIZE(AnimTypeClass::Load()), STRINGIZE(AnimTypeClass::Warhead), "AnimType.cpp", 6, 0x004196F2);
SWIZZLE_PATCH(_AnimTypeClass_Load_Swizzle_Patch_7, edx, STRINGIZE(AnimTypeClass::Load()), STRINGIZE(AnimTypeClass::TiberiumSpawnType), "AnimType.cpp", 7, 0x00419703);
SWIZZLE_PATCH(_BaseClass_Load_Swizzle_Patch, edx, STRINGIZE(BaseClass::Load()), STRINGIZE(BaseClass::House), "Base.cpp", 1, 0x0041FDA5);
SWIZZLE_PATCH(_BuildingLightClass_Load_Swizzle_Patch_1, edx, STRINGIZE(BuildingLightClass::Load()), STRINGIZE(BuildingLightClass::Source), "BuildingLight.cpp", 1, 0x00422B05);
SWIZZLE_PATCH(_BuildingLightClass_Load_Swizzle_Patch_2, esi, STRINGIZE(BuildingLightClass::Load()), STRINGIZE(BuildingLightClass::Following), "BuildingLight.cpp", 2, 0x00422B16);
SWIZZLE_PATCH(_NeuronClass_Load_Swizzle_Patch_1, edx, STRINGIZE(NeuronClass::Load()), STRINGIZE(NeuronClass::field_14), "Brain.cpp", 1, 0x004254BF);
SWIZZLE_PATCH(_NeuronClass_Load_Swizzle_Patch_2, eax, STRINGIZE(NeuronClass::Load()), STRINGIZE(NeuronClass::field_18), "Brain.cpp", 2, 0x004254CD);
SWIZZLE_PATCH(_NeuronClass_Load_Swizzle_Patch_3, edi, STRINGIZE(NeuronClass::Load()), STRINGIZE(NeuronClass::field_1C), "Brain.cpp", 3, 0x004254DB);
SWIZZLE_PATCH(_BuildingClass_Load_Swizzle_Patch_1, ecx, STRINGIZE(BuildingClass::Load()), STRINGIZE(BuildingClass::Class), "Building.cpp", 1, 0x0043817C);
SWIZZLE_PATCH(_BuildingClass_Load_Swizzle_Patch_2, edx, STRINGIZE(BuildingClass::Load()), STRINGIZE(BuildingClass::Factory), "Building.cpp", 2, 0x0043818D);
SWIZZLE_PATCH(_BuildingClass_Load_Swizzle_Patch_3, eax, STRINGIZE(BuildingClass::Load()), STRINGIZE(BuildingClass::WhomToRepay), "Building.cpp", 3, 0x0043819E);
SWIZZLE_PATCH(_BuildingClass_Load_Swizzle_Patch_4, ecx, STRINGIZE(BuildingClass::Load()), STRINGIZE(BuildingClass::AnimToTrack), "Building.cpp", 4, 0x004381AF);
SWIZZLE_PATCH(_BuildingClass_Load_Swizzle_Patch_5, edx, STRINGIZE(BuildingClass::Load()), STRINGIZE(BuildingClass::BuildingLight), "Building.cpp", 5, 0x004381C0);
SWIZZLE_PATCH(_BuildingClass_Load_Swizzle_Patch_6, edi, STRINGIZE(BuildingClass::Load()), STRINGIZE(BuildingClass::Anims), "Building.cpp", 6, 0x004381D6);
SWIZZLE_PATCH(_BuildingClass_Load_Swizzle_Patch_7, edi, STRINGIZE(BuildingClass::Load()), STRINGIZE(BuildingClass::Upgrades), "Building.cpp", 7, 0x004381F2);

DECLARE_PATCH(_BuildingTypeClass_Load_Swizzle_Patch_1)
{
    _asm { mov [esi+0x518], eax } // this->BuildingClass.ExitList = eax

    GET_REGISTER_STATIC(uintptr_t, pointer, ecx);
    _asm { pushad } \
    VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_DBG(pointer, TIBSUN_SOURCE_PATH "BuildingType.cpp", 1, STRINGIZE(BuildingTypeClass::Load()), STRINGIZE(BuildingTypeClass::ToOverlay));
    _asm { popad } \
    JMP(0x00443497);
}

SWIZZLE_PATCH(_BuildingTypeClass_Load_Swizzle_Patch_2, edx, STRINGIZE(BuildingTypeClass::Load()), STRINGIZE(BuildingTypeClass::FreeUnit), "BuildingType.cpp", 2, 0x004434A8);
SWIZZLE_PATCH(_BulletClass_Load_Swizzle_Patch_1, edx, STRINGIZE(BulletClass::Load()), STRINGIZE(BulletClass::Class), "Bullet.cpp", 1, 0x0044714A);
SWIZZLE_PATCH(_BulletClass_Load_Swizzle_Patch_2, eax, STRINGIZE(BulletClass::Load()), STRINGIZE(BulletClass::Payback), "Bullet.cpp", 2, 0x00447158);
SWIZZLE_PATCH(_BulletClass_Load_Swizzle_Patch_3, ecx, STRINGIZE(BulletClass::Load()), STRINGIZE(BulletClass::Warhead), "Bullet.cpp", 3, 0x00447169);
SWIZZLE_PATCH(_BulletClass_Load_Swizzle_Patch_4, esi, STRINGIZE(BulletClass::Load()), STRINGIZE(BulletClass::TarCom), "Bullet.cpp", 4, 0x0044717A);
SWIZZLE_PATCH(_BulletTypeClass_Load_Swizzle_Patch_1, ecx, STRINGIZE(BulletTypeClass::Load()), STRINGIZE(BulletTypeClass::Trailer), "BulletType.cpp", 1, 0x0044858C);
SWIZZLE_PATCH(_BulletTypeClass_Load_Swizzle_Patch_2, esi, STRINGIZE(BulletTypeClass::Load()), STRINGIZE(BulletTypeClass::AirburstWeapon), "BulletType.cpp", 2, 0x0044859D);
SWIZZLE_PATCH(_CellClass_Load_Swizzle_Patch_1, eax, STRINGIZE(CellClass::Load()), STRINGIZE(CellClass::FoggedObjects), "Cell.cpp", 1, 0x00459903);
SWIZZLE_PATCH(_CellClass_Load_Swizzle_Patch_2, eax, STRINGIZE(CellClass::Load()), STRINGIZE(CellClass::field_1C), "Cell.cpp", 2, 0x0045994E);
SWIZZLE_PATCH(_CellClass_Load_Swizzle_Patch_3, ecx, STRINGIZE(CellClass::Load()), STRINGIZE(CellClass::field_20), "Cell.cpp", 3, 0x0045995C);
SWIZZLE_PATCH(_CellClass_Load_Swizzle_Patch_4, edx, STRINGIZE(CellClass::Load()), STRINGIZE(CellClass::CellTag), "Cell.cpp", 4, 0x0045996A);
SWIZZLE_PATCH(_CellClass_Load_Swizzle_Patch_5, eax, STRINGIZE(CellClass::Load()), STRINGIZE(CellClass::OccupierPtr), "Cell.cpp", 5, 0x00459978);
SWIZZLE_PATCH(_CellClass_Load_Swizzle_Patch_6, esi, STRINGIZE(CellClass::Load()), STRINGIZE(CellClass::AltOccupierPtr), "Cell.cpp", 6, 0x00459986);
SWIZZLE_PATCH(_DropshipLoadoutClass_Load_Swizzle_Patch, edi, STRINGIZE(DropshipLoadoutClass::Load()), STRINGIZE(DropshipLoadoutClass::field_14), "Dropship.cpp", 1, 0x0048425C);
SWIZZLE_PATCH(_FactoryClass_Load_Swizzle_Patch_1, eax, STRINGIZE(FactoryClass::Load()), STRINGIZE(FactoryClass::QueuedObjects), "Factory.cpp", 1, 0x004976BD);
SWIZZLE_PATCH(_FactoryClass_Load_Swizzle_Patch_2, ecx, STRINGIZE(FactoryClass::Load()), STRINGIZE(FactoryClass::House), "Factory.cpp", 2, 0x004976D2);
SWIZZLE_PATCH(_FactoryClass_Load_Swizzle_Patch_3, edi, STRINGIZE(FactoryClass::Load()), STRINGIZE(FactoryClass::Object), "Factory.cpp", 3, 0x004976E0);
SWIZZLE_PATCH(_FoggedObjectClass_Load_Swizzle_Patch_1, ecx, STRINGIZE(FoggedObjectClass::Load()), STRINGIZE(FoggedObjectClass::Owner), "FoggedObj.cpp", 1, 0x0049F487);
SWIZZLE_PATCH(_FoggedObjectClass_Load_Swizzle_Patch_2, ecx, STRINGIZE(FoggedObjectClass::Load()), STRINGIZE(FoggedObjectClass::DrawRecords[]), "FoggedObj.cpp", 2, 0x0049F552);
SWIZZLE_PATCH(_FootClass_Load_Swizzle_Patch_1, eax, STRINGIZE(FootClass::Load()), STRINGIZE(FootClass::Team), "Foot.cpp", 1, 0x004A6104);
SWIZZLE_PATCH(_FootClass_Load_Swizzle_Patch_2, ecx, STRINGIZE(FootClass::Load()), STRINGIZE(FootClass::Member), "Foot.cpp", 2, 0x004A6115);
SWIZZLE_PATCH(_FootClass_Load_Swizzle_Patch_3, edx, STRINGIZE(FootClass::Load()), STRINGIZE(FootClass::NavCom), "Foot.cpp", 3, 0x004A6126);
SWIZZLE_PATCH(_FootClass_Load_Swizzle_Patch_4, eax, STRINGIZE(FootClass::Load()), STRINGIZE(FootClass::SuspendedNavCom), "Foot.cpp", 4, 0x004A6137);
SWIZZLE_PATCH(_FootClass_Load_Swizzle_Patch_5, ecx, STRINGIZE(FootClass::Load()), STRINGIZE(FootClass::field_2A0), "Foot.cpp", 5, 0x004A6148);
SWIZZLE_PATCH(_FootClass_Load_Swizzle_Patch_6, eax, STRINGIZE(FootClass::Load()), STRINGIZE(FootClass::NavList), "Foot.cpp", 6, 0x004A6168);
SWIZZLE_PATCH(_FootClass_Load_Swizzle_Patch_7, edx, STRINGIZE(FootClass::Load()), STRINGIZE(FootClass::PathfindingCells), "Foot.cpp", 7, 0x004A6193);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_1, edx, STRINGIZE(HouseClass::Load()), STRINGIZE(HouseClass::Class), "House.cpp", 1, 0x004C4B97);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_2, eax, STRINGIZE(HouseClass::Load()), STRINGIZE(HouseClass::InfantryFactory), "House.cpp", 2, 0x004C4BA8);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_3, ecx, STRINGIZE(HouseClass::Load()), STRINGIZE(HouseClass::UnitFactory), "House.cpp", 3, 0x004C4BB9);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_4, edx, STRINGIZE(HouseClass::Load()), STRINGIZE(HouseClass::AircraftFactory), "House.cpp", 4, 0x004C4BCA);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_5, eax, STRINGIZE(HouseClass::Load()), STRINGIZE(HouseClass::BuildingFactory), "House.cpp", 5, 0x004C4BDB);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_6, ecx, STRINGIZE(HouseClass::Load()), STRINGIZE(HouseClass::ToCapture), "House.cpp", 6, 0x004C4BEC);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_7, ebp, STRINGIZE(HouseClass::Load()), STRINGIZE(HouseClass::WaypointPaths), "House.cpp", 7, 0x004C4C02);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_8, ecx, STRINGIZE(HouseClass::Load()), STRINGIZE(HouseClass::AngerNodes), "House.cpp", 8, 0x004C4E20);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_9, edx, STRINGIZE(HouseClass::Load()), STRINGIZE(HouseClass::ScoutNodes), "House.cpp", 9, 0x004C4EFE);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_10, ecx, STRINGIZE(HouseClass::Load()), STRINGIZE(HouseClass::SuperWeapon), "House.cpp", 10, 0x004C4FBD);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_11, eax, STRINGIZE(HouseClass::Load()),  STRINGIZE(HouseClass::field_28), "House.cpp", 11, 0x004C4FE2);
SWIZZLE_PATCH(_HouseClass_Load_Swizzle_Patch_12, edx, STRINGIZE(HouseClass::Load()),  STRINGIZE(HouseClass::field_40), "House.cpp", 12, 0x004C5004);
SWIZZLE_PATCH(_InfantryClass_Load_Swizzle_Patch, esi, STRINGIZE(InfantryClass::Load()), STRINGIZE(InfantryClass::Class), "Infantry.cpp", 1, 0x004D94B8);
SWIZZLE_PATCH(_IsometricTileClass_Load_Swizzle_Patch, esi, STRINGIZE(IsometricTileClass::Load()), STRINGIZE(Class), "IsoTile.cpp", 1, 0x004F22A2);
SWIZZLE_PATCH(_IsometricTileTypeClass_Load_Swizzle_Patch, eax, STRINGIZE(IsometricTileTypeClass::Load()), STRINGIZE(IsometricTileTypeClass::NextTileTypeInSet), "IsoTileType.cpp", 1, 0x004F8651);
SWIZZLE_PATCH(_LayerClass_Load_Swizzle_Patch, eax, STRINGIZE(LayerClass::Load()), STRINGIZE(LayerClass::(*this)[]), "Layer.cpp", 1, 0x004FD005);

DECLARE_PATCH(_LocomotionClass_Load_Swizzle_Patch)
{
    _asm { mov edi, eax } // result from IStream.Read

    GET_REGISTER_STATIC(uintptr_t, pointer, esi);
    _asm { pushad } \
    VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_DBG(pointer, TIBSUN_SOURCE_PATH "Locomotion.cpp", 1, STRINGIZE(LocomotionClass::Load()), STRINGIZE(LocomotionClass::LinkedTo));
    _asm { popad } \
    JMP(0x00506655);
}

SWIZZLE_PATCH(_SidebarClass_StripClass_Load_Swizzle_Patch, esi, STRINGIZE(SidebarClass::StripClass::Load()), STRINGIZE(SidebarClass::StripClass::Buildables::Factory), "Sidebar.cpp", 1, 0x005627FC);
SWIZZLE_PATCH(_DisplayClass_Load_Swizzle_Patch_1, eax, STRINGIZE(DisplayClass::Load()), STRINGIZE(DisplayClass::PendingObjectPtr), "Display.cpp", 1, 0x00562824);
SWIZZLE_PATCH(_DisplayClass_Load_Swizzle_Patch_2, ecx, STRINGIZE(DisplayClass::Load()), STRINGIZE(DisplayClass::PendingObject), "Display.cpp", 2, 0x00562835);
SWIZZLE_PATCH(_DisplayClass_Load_Swizzle_Patch_3, edx, STRINGIZE(DisplayClass::Load()), STRINGIZE(DisplayClass::FollowingObjectPtr), "Display.cpp", 3, 0x00562846);
SWIZZLE_PATCH(_ObjectClass_Load_Swizzle_Patch_1, ecx, STRINGIZE(ObjectClass::Load()), STRINGIZE(ObjectClass::Next), "Object.cpp", 1, 0x00586712);
SWIZZLE_PATCH(_ObjectClass_Load_Swizzle_Patch_2, esi, STRINGIZE(ObjectClass::Load()), STRINGIZE(ObjectClass::AttachedTag), "Object.cpp", 2, 0x00586720);
SWIZZLE_PATCH(_OverlayClass_Load_Swizzle_Patch, esi, STRINGIZE(OverlayClass::Load()), STRINGIZE(ObjectClass::Class), "Overlay.cpp", 1, 0x0058C512);
SWIZZLE_PATCH(_OverlayTypeClass_Load_Swizzle_Patch, edx, STRINGIZE(OverlayTypeClass::Load()), STRINGIZE(OverlayTypeClass::CellAnim), "OverlayType.cpp", 1, 0x0058D84D);
SWIZZLE_PATCH(_ParticleClass_Load_Swizzle_Patch_1, edx, STRINGIZE(ParticleClass::Load()), STRINGIZE(ParticleClass::Class), "Particle.cpp", 1, 0x005A4E6F);
SWIZZLE_PATCH(_ParticleClass_Load_Swizzle_Patch_2, esi, STRINGIZE(ParticleClass::Load()), STRINGIZE(ParticleClass::ParticleSystem), "Particle.cpp", 2, 0x005A4E80);
SWIZZLE_PATCH(_ParticleSystemClass_Load_Swizzle_Patch_1, ecx, STRINGIZE(ParticleSystemClass::Load()), STRINGIZE(ParticleSystemClass::Class), "ParticleSys.cpp", 1, 0x005A7645);
SWIZZLE_PATCH(_ParticleSystemClass_Load_Swizzle_Patch_2, edx, STRINGIZE(ParticleSystemClass::Load()), STRINGIZE(ParticleSystemClass::Target), "ParticleSys.cpp", 2, 0x005A7656);
SWIZZLE_PATCH(_ParticleSystemClass_Load_Swizzle_Patch_3, eax, STRINGIZE(ParticleSystemClass::Load()), STRINGIZE(ParticleSystemClass::Owner), "ParticleSys.cpp", 3, 0x005A7667);
SWIZZLE_PATCH(_ParticleSystemClass_Load_Swizzle_Patch_4, ecx, STRINGIZE(ParticleSystemClass::Load()), STRINGIZE(ParticleSystemClass::Particles), "ParticleSys.cpp", 4, 0x005A7714);
SWIZZLE_PATCH(_ParticleTypeClass_Load_Swizzle_Patch, ecx, STRINGIZE(ParticleTypeClass::Load()), STRINGIZE(ParticleTypeClass::Warhead), "ParticleType.cpp", 1, 0x005AF9A1);
SWIZZLE_PATCH(_RadioClass_Load_Swizzle_Patch, esi, STRINGIZE(RadioClass::Load()), STRINGIZE(RadioClass::Radio), "Radio.cpp", 1, 0x005BDAC2);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_1, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::NukeWarhead), "Rules.cpp", 1, 0x005D01E5);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_2, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::FlameDamage), "Rules.cpp", 2, 0x005D01F6);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_3, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::FlameDamage2), "Rules.cpp", 3, 0x005D0207);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_4, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::LargeVisceroid), "Rules.cpp", 4, 0x005D0215);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_5, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::SmallVisceroid), "Rules.cpp", 5, 0x005D0223);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_6, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::UnloadingHarvester), "Rules.cpp", 6, 0x005D0231);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_7, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DropPodWeapon), "Rules.cpp", 7, 0x005D0242);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_8, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::EMPulseWarhead), "Rules.cpp", 8, 0x005D0253);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_9, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::C4Warhead), "Rules.cpp", 9, 0x005D0264);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_10, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::IonCannonWarhead), "Rules.cpp", 10, 0x005D0275);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_11, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::FirestormWarhead), "Rules.cpp", 11, 0x005D0286);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_12, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::VeinholeWarhead), "Rules.cpp", 12, 0x005D0297);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_13, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DefaultFirestormExplosionSystem), "Rules.cpp", 13, 0x005D02A8);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_14, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DefaultLargeGreySmokeSystem), "Rules.cpp", 14, 0x005D02B9);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_15, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DefaultSmallGreySmokeSystem), "Rules.cpp", 15, 0x005D02CA);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_16, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DefaultSparkSystem), "Rules.cpp", 16, 0x005D02DB);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_17, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DefaultLargeRedSmokeSystem), "Rules.cpp", 17, 0x005D02EC);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_18, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DefaultSmallRedSmokeSystem), "Rules.cpp", 18, 0x005D02FD);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_19, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DefaultDebrisSmokeSystem), "Rules.cpp", 19, 0x005D030E);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_20, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DefaultFireStreamSystem), "Rules.cpp", 20, 0x005D031F);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_21, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DefaultTestParticleSystem), "Rules.cpp", 21, 0x005D0330);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_22, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DefaultRepairParticleSystem), "Rules.cpp", 22, 0x005D0341);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_23, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::VeinholeTypeClass), "Rules.cpp", 23, 0x005D0352);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_24, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::field_7D4), "Rules.cpp", 24, 0x005D0363);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_25, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Smoke), "Rules.cpp", 25, 0x005D0374);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_26, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::MoveFlash), "Rules.cpp", 26, 0x005D0385);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_27, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BombParachute), "Rules.cpp", 27, 0x005D0396);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_28, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Parachute), "Rules.cpp", 28, 0x005D03A7);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_29, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::SmallFire), "Rules.cpp", 29, 0x005D03B8);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_30, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::LargeFire), "Rules.cpp", 30, 0x005D03C9);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_31, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DropZoneAnim), "Rules.cpp", 31, 0x005D03DA);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_32, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BaseUnit), "Rules.cpp", 32, 0x005D03EB);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_33, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::UnitCrateType), "Rules.cpp", 33, 0x005D03FC);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_34, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Paratrooper), "Rules.cpp", 34, 0x005D040D);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_35, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Disguise), "Rules.cpp", 35, 0x005D041E);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_36, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Technician), "Rules.cpp", 36, 0x005D042F);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_37, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Engineer), "Rules.cpp", 37, 0x005D0440);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_38, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Pilot), "Rules.cpp", 38, 0x005D0451);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_39, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Crew), "Rules.cpp", 39, 0x005D0462);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_40, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::RepairBay), "Rules.cpp", 40, 0x005D0473);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_41, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::GDIGateOne), "Rules.cpp", 41, 0x005D0484);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_42, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::GDIGateTwo), "Rules.cpp", 42, 0x005D0495);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_43, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::NodGateOne), "Rules.cpp", 43, 0x005D04A6);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_44, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::NodGateTwo), "Rules.cpp", 44, 0x005D04B7);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_45, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::WallTower), "Rules.cpp", 45, 0x005D04C8);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_46, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::GDIPowerPlant), "Rules.cpp", 46, 0x005D04D9);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_47, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::GDIPowerTurbine), "Rules.cpp", 47, 0x005D04EA);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_48, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::NodRegularPower), "Rules.cpp", 48, 0x005D04FB);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_49, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::NodAdvancedPower), "Rules.cpp", 49, 0x005D050C);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_50, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::GDIFirestormGenerator), "Rules.cpp", 50, 0x005D051D);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_51, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::GDIHunterSeeker), "Rules.cpp", 51, 0x005D052E);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_52, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::NodHunterSeeker), "Rules.cpp", 52, 0x005D053F); 
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_53, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::NukeProjectile), "Rules.cpp", 53, 0x005D0550);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_54, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::NukeDown), "Rules.cpp", 54, 0x005D0561);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_55, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::EMPulseProjectile), "Rules.cpp", 55, 0x005D0572);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_56, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::TireVoxelDebris), "Rules.cpp", 56, 0x005D0583);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_57, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::ScrapVoxelDebris), "Rules.cpp", 57, 0x005D0594);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_58, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::AtmosphereEntry), "Rules.cpp", 58, 0x005D05A5);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_59, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::InfantryExplode), "Rules.cpp", 59, 0x005D05B6);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_60, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::IonBlast), "Rules.cpp", 60, 0x005D05C7);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_61, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::IonBeam), "Rules.cpp", 61, 0x005D05D8);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_62, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DigAnim), "Rules.cpp", 62, 0x005D05E9);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_63, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::FirestormIdleAnim), "Rules.cpp", 63, 0x005D05FA);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_64, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::FirestormAirAnim), "Rules.cpp", 64, 0x005D060B);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_65, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::FirestormGroundAnim), "Rules.cpp", 65, 0x005D061C);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_66, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::FirestormActiveAnim), "Rules.cpp", 66, 0x005D062D);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_67, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::EMPulseSparkles), "Rules.cpp", 67, 0x005D063E);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_68, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Wake), "Rules.cpp", 68, 0x005D064F);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_69, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::FlamingInfantry), "Rules.cpp", 69, 0x005D0660);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_70, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::VeinAttack), "Rules.cpp", 70, 0x005D0671);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_71, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BarrelExplode), "Rules.cpp", 71, 0x005D067F);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_72, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BarrelParticle), "Rules.cpp", 72, 0x005D068D);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_73, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DropPodPuff), "Rules.cpp", 73, 0x005D069B);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_74, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::IonStormWarhead), "Rules.cpp", 74, 0x005D06AC);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_75, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::WoodCrateImg), "Rules.cpp", 75, 0x005D06BD);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_76, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::CrateImg), "Rules.cpp", 76, 0x005D06CE);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_77, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::WebbedInfantry), "Rules.cpp", 77, 0x005D06DF);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_78, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BarrelDebris), "Rules.cpp", 78, 0x005D06F9);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_79, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::OnFire), "Rules.cpp", 79, 0x005D0721);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_80, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::TreeFire), "Rules.cpp", 80, 0x005D074C);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_81, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::SplashList), "Rules.cpp", 81, 0x005D0777);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_82, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Scorches), "Rules.cpp", 82, 0x005D07A2);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_83, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Scorches1), "Rules.cpp", 83, 0x005D07CD);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_84, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Scorches2), "Rules.cpp", 84, 0x005D07F8);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_85, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Scorches3), "Rules.cpp", 85, 0x005D0823);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_86, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Scorches4), "Rules.cpp", 86, 0x005D084E);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_87, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::Craters), "Rules.cpp", 87, 0x005D0879);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_88, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::HarvesterUnit), "Rules.cpp", 88, 0x005D08A4);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_89, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildConst), "Rules.cpp", 89, 0x005D08CF);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_90, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildPower), "Rules.cpp", 90, 0x005D08FA);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_91, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildRefinery), "Rules.cpp", 91, 0x005D0925);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_92, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildBarracks), "Rules.cpp", 92, 0x005D0950);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_93, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildTech), "Rules.cpp", 93, 0x005D097B);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_94, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildWeapons), "Rules.cpp", 94, 0x005D09A6);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_95, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildDefense), "Rules.cpp", 95, 0x005D09D1);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_96, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildPDefense), "Rules.cpp", 96, 0x005D09FC);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_97, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildAA), "Rules.cpp", 97, 0x005D0A27);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_98, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildHelipad), "Rules.cpp", 98, 0x005D0A52);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_99, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BuildRadar), "Rules.cpp", 99, 0x005D0A7D);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_100, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::ConcreteWalls), "Rules.cpp", 100, 0x005D0AA8);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_101, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::NSGates), "Rules.cpp", 101, 0x005D0AD3);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_102, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::EWGates), "Rules.cpp", 102, 0x005D0AFE);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_103, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::PadAircraft), "Rules.cpp", 103, 0x005D0B29);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_104, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::ExplosiveVoxelDebris), "Rules.cpp", 104, 0x005D0B54);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_105, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DeadBodies), "Rules.cpp", 105, 0x005D0B7F);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_106, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::DropPod), "Rules.cpp", 106, 0x005D0BAA);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_107, edx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::MetallicDebris), "Rules.cpp", 107, 0x005D0BD5);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_108, ecx, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::BridgeExplosions), "Rules.cpp", 108, 0x005D0C00);
SWIZZLE_PATCH(_RulesClass_Load_Swizzle_Patch_109, eax, STRINGIZE(RulesClass::Load()), STRINGIZE(RulesClass::HSBuilding), "Rules.cpp", 109, 0x005D0C25);

DECLARE_PATCH(_Load_Misc_Values_Swizzle_Patch_1)
{
    static uintptr_t pointer;
    
    pointer = (uintptr_t)&MasterParticle;

    _asm { pushad } \
    VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_DBG(pointer, TIBSUN_SOURCE_PATH "SaveLoad.cpp", 1, STRINGIZE(Load_Misc_Values()), STRINGIZE(MasterParticle));
    _asm { popad } \
    JMP(0x005D7B1E);
}

DECLARE_PATCH(_Load_Misc_Values_Swizzle_Patch_2)
{
    static uintptr_t pointer;

    pointer = (uintptr_t)&PlayerPtr;

    _asm { pushad } \
    VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_DBG(pointer, TIBSUN_SOURCE_PATH "SaveLoad.cpp", 2, STRINGIZE(Load_Misc_Values()), STRINGIZE(PlayerPtr));
    _asm { popad } \
    JMP(0x005D7B44);
}

SWIZZLE_PATCH(_Load_Misc_Values_Swizzle_Patch_3, ecx, STRINGIZE(Load_Misc_Values()), STRINGIZE(CurrentObjects), "SaveLoad.cpp", 3, 0x005D7C14);
SWIZZLE_PATCH(_Load_Misc_Values_Swizzle_Patch_4, ecx, STRINGIZE(Load_Misc_Values()), STRINGIZE(LogicTriggers), "SaveLoad.cpp", 4, 0x005D7D39);
SWIZZLE_PATCH(_Load_Misc_Values_Swizzle_Patch_5, ecx, STRINGIZE(Load_Misc_Values()), STRINGIZE(MapTriggers), "SaveLoad.cpp", 5, 0x005D7DFB);
SWIZZLE_PATCH(_Scenario_Load_Swizzle_Patch, eax, STRINGIZE(Scenario_Load()), STRINGIZE(ScenarioClass::AllowableUnits), "Scenario.cpp", 1, 0x005DF6D2);
SWIZZLE_PATCH(_ScriptClass_Load_Swizzle_Patch, esi, STRINGIZE(ScriptClass::Load()), STRINGIZE(ScriptClass::Class), "Script.cpp", 1, 0x005E7A8F);
SWIZZLE_PATCH(_SmudgeClass_Load_Swizzle_Patch, esi, STRINGIZE(SmudgeClass::Load()), STRINGIZE(SmudgeClass::Class), "Smudge.cpp", 1, 0x005FAE42);
SWIZZLE_PATCH(_SuperClass_Load_Swizzle_Patch_1, ecx, STRINGIZE(SuperClass::Load()), STRINGIZE(SuperClass::Class), "Super.cpp", 1, 0x0060C7CA);
SWIZZLE_PATCH(_SuperClass_Load_Swizzle_Patch_2, esi, STRINGIZE(SuperClass::Load()), STRINGIZE(SuperClass::House), "Super.cpp", 2, 0x0060C7D8);
SWIZZLE_PATCH(_SuperWeaponTypeClass_Load_Swizzle_Patch_1, edx, STRINGIZE(SuperWeaponTypeClass::Load()), STRINGIZE(SuperWeaponTypeClass::Weapon), "SuperType.cpp", 1, 0x0060D196);
SWIZZLE_PATCH(_SuperWeaponTypeClass_Load_Swizzle_Patch_2, eax, STRINGIZE(SuperWeaponTypeClass::Load()), STRINGIZE(SuperWeaponTypeClass::AuxBuilding), "SuperType.cpp", 2, 0x0060D1A7);
SWIZZLE_PATCH(_Tactical_Load_Swizzle_Patch, ebx, STRINGIZE(Tactical::Load()), STRINGIZE(Tactical::VisibleCells), "Tactical.cpp", 1, 0x00617F6A);
SWIZZLE_PATCH(_TActionClass_Load_Swizzle_Patch_1, edx, STRINGIZE(TActionClass::Load()), STRINGIZE(TActionClass::Next), "TAction.cpp", 1, 0x0061D8DF);
SWIZZLE_PATCH(_TActionClass_Load_Swizzle_Patch_2, eax, STRINGIZE(TActionClass::Load()), STRINGIZE(TActionClass::Team), "TAction.cpp", 2, 0x0061D8ED);
SWIZZLE_PATCH(_TActionClass_Load_Swizzle_Patch_3, ecx, STRINGIZE(TActionClass::Load()), STRINGIZE(TActionClass::Tag), "TAction.cpp", 3, 0x0061D8FB);
SWIZZLE_PATCH(_TActionClass_Load_Swizzle_Patch_4, esi, STRINGIZE(TActionClass::Load()), STRINGIZE(TActionClass::Trigger), "TAction.cpp", 4, 0x0061D909);
SWIZZLE_PATCH(_TagClass_Load_Swizzle_Patch_1, edx, STRINGIZE(TagClass::Load()), STRINGIZE(TagClass::Class), "Tag.cpp", 1, 0x0061EC1F);
SWIZZLE_PATCH(_TagClass_Load_Swizzle_Patch_2, esi, STRINGIZE(TagClass::Load()), STRINGIZE(TagClass::AttachedTrigger), "Tag.cpp", 2, 0x0061EC2D);
SWIZZLE_PATCH(_TagTypeClass_Load_Swizzle_Patch, esi, STRINGIZE(TagTypeClass::Load()), STRINGIZE(TagTypeClass::TriggerType), "TagType.cpp", 1, 0x0061F85F);
SWIZZLE_PATCH(_TaskForceClass_Load_Swizzle_Patch, esi, STRINGIZE(TaskForceClass::Load()), STRINGIZE(TagTypeClass::Members::Class), "TaskForce.cpp", 1, 0x00621D55);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_1, eax, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::Class), "Team.cpp", 1, 0x006252A0);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_2, ecx, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::Script), "Team.cpp", 2, 0x006252AE);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_3, edx, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::House), "Team.cpp", 3, 0x006252BC);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_4, eax, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::Tag), "Team.cpp", 4, 0x006252CA);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_5, ecx, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::Member), "Team.cpp", 5, 0x006252D8);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_6, edx, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::ClosestMember), "Team.cpp", 6, 0x006252E6);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_7, eax, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::Target), "Team.cpp", 7, 0x006252F4);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_8, ecx, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::Zone), "Team.cpp", 8, 0x00625302);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_9, edx, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::MissionTarget), "Team.cpp", 9, 0x00625310);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_10, eax, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::field_20), "Team.cpp", 10, 0x0062531E);
SWIZZLE_PATCH(_TeamClass_Load_Swizzle_Patch_11, esi, STRINGIZE(TeamClass::Load()), STRINGIZE(TeamClass::field_34), "Team.cpp", 11, 0x0062532C);
SWIZZLE_PATCH(_TeamTypeClass_Load_Swizzle_Patch_1, edx, STRINGIZE(TeamTypeClass::Load()), STRINGIZE(TeamTypeClass::House), "TeamType.cpp", 1, 0x00628F12);
SWIZZLE_PATCH(_TeamTypeClass_Load_Swizzle_Patch_2, eax, STRINGIZE(TeamTypeClass::Load()), STRINGIZE(TeamTypeClass::Tag), "TeamType.cpp", 2, 0x00628F23);
SWIZZLE_PATCH(_TeamTypeClass_Load_Swizzle_Patch_3, ecx, STRINGIZE(TeamTypeClass::Load()), STRINGIZE(TeamTypeClass::Script), "TeamType.cpp", 3, 0x00628F34);
SWIZZLE_PATCH(_TeamTypeClass_Load_Swizzle_Patch_4, esi, STRINGIZE(TeamTypeClass::Load()), STRINGIZE(TeamTypeClass::TaskForce), "TeamType.cpp", 4, 0x00628F45);
SWIZZLE_PATCH(_TechnoClass_Load_Swizzle_Patch_1, ecx, STRINGIZE(TechnoClass::Load()), STRINGIZE(TechnoClass::House), "Techno.cpp", 1, 0x00638DED);
SWIZZLE_PATCH(_TechnoClass_Load_Swizzle_Patch_2, edi, STRINGIZE(TechnoClass::Load()), STRINGIZE(TechnoClass::ParticleSystems), "Techno.cpp", 2, 0x00638E03);
SWIZZLE_PATCH(_TechnoClass_Load_Swizzle_Patch_3, edx, STRINGIZE(TechnoClass::Load()), STRINGIZE(TechnoClass::ArchiveTarget), "Techno.cpp", 3, 0x00638E1A);
SWIZZLE_PATCH(_TechnoClass_Load_Swizzle_Patch_4, eax, STRINGIZE(TechnoClass::Load()), STRINGIZE(TechnoClass::field_20C), "Techno.cpp", 4, 0x00638E2B);
SWIZZLE_PATCH(_TechnoClass_Load_Swizzle_Patch_5, ecx, STRINGIZE(TechnoClass::Load()), STRINGIZE(TechnoClass::TarCom), "Techno.cpp", 5, 0x00638E3C);
SWIZZLE_PATCH(_TechnoClass_Load_Swizzle_Patch_6, edx, STRINGIZE(TechnoClass::Load()), STRINGIZE(TechnoClass::SuspendedTarCom), "Techno.cpp", 6, 0x00638E4D);
SWIZZLE_PATCH(_TechnoClass_Load_Swizzle_Patch_7, eax, STRINGIZE(TechnoClass::Load()), STRINGIZE(TechnoClass::Cargo), "Techno.cpp", 7, 0x00638E5E);
SWIZZLE_PATCH(_TechnoClass_Load_Swizzle_Patch_8, esi, STRINGIZE(TechnoClass::Load()), STRINGIZE(TechnoClass::Wave), "Techno.cpp", 8, 0x00638E6F);
SWIZZLE_PATCH(_TechnoTypeClass_Load_Swizzle_Patch_1, eax, STRINGIZE(TechnoTypeClass::Load()), STRINGIZE(TechnoTypeClass::DeploysInto), "TechnoType.cpp", 1, 0x0063DA03);
SWIZZLE_PATCH(_TechnoTypeClass_Load_Swizzle_Patch_2, ecx, STRINGIZE(TechnoTypeClass::Load()), STRINGIZE(TechnoTypeClass::UndeploysInto), "TechnoType.cpp", 2, 0x0063DA14);
SWIZZLE_PATCH(_TechnoTypeClass_Load_Swizzle_Patch_3, edx, STRINGIZE(TechnoTypeClass::Load()), STRINGIZE(TechnoTypeClass::NaturalParticleSystem), "TechnoType.cpp", 3, 0x0063DA25);
SWIZZLE_PATCH(_TechnoTypeClass_Load_Swizzle_Patch_4, edx, STRINGIZE(TechnoTypeClass::Load()), STRINGIZE(TechnoTypeClass::Dock), "TechnoType.cpp", 4, 0x0063DD8D);
SWIZZLE_PATCH(_TechnoTypeClass_Load_Swizzle_Patch_5, ecx, STRINGIZE(TechnoTypeClass::Load()), STRINGIZE(TechnoTypeClass::Explosion), "TechnoType.cpp", 5, 0x0063DDB8);
SWIZZLE_PATCH(_TechnoTypeClass_Load_Swizzle_Patch_6, eax, STRINGIZE(TechnoTypeClass::Load()), STRINGIZE(TechnoTypeClass::DebrisTypes), "TechnoType.cpp", 6, 0x0063DDE3);
SWIZZLE_PATCH(_TechnoTypeClass_Load_Swizzle_Patch_7, esi, STRINGIZE(TechnoTypeClass::Load()), STRINGIZE(TechnoTypeClass::Weapons), "TechnoType.cpp", 7, 0x0063DE04);
SWIZZLE_PATCH(_TechnoTypeClass_Load_Swizzle_Patch_8, edx, STRINGIZE(TechnoTypeClass::Load()), STRINGIZE(TechnoTypeClass::DamageParticleSystems), "TechnoType.cpp", 8, 0x0063DE2A);
SWIZZLE_PATCH(_TerrainClass_Load_Swizzle_Patch, ebp, STRINGIZE(TerrainClass::Load()), STRINGIZE(TEventClass::Class), "Terrain.cpp", 1, 0x006407C2);
SWIZZLE_PATCH(_TEventClass_Load_Swizzle_Patch_1, edx, STRINGIZE(TEventClass::Load()), STRINGIZE(TEventClass::Next), "TEvent.cpp", 1, 0x00642DDF);
SWIZZLE_PATCH(_TEventClass_Load_Swizzle_Patch_2, esi, STRINGIZE(TEventClass::Load()), STRINGIZE(TEventClass::Team), "TEvent.cpp", 2, 0x00642DED);
SWIZZLE_PATCH(_TiberiumClass_Load_Swizzle_Patch_1, edx, STRINGIZE(TiberiumClass::Load()), STRINGIZE(TiberiumClass::Image), "Tiberium.cpp", 1, 0x00645175);
SWIZZLE_PATCH(_TiberiumClass_Load_Swizzle_Patch_2, eax, STRINGIZE(TiberiumClass::Load()), STRINGIZE(TiberiumClass::Debris[]), "Tiberium.cpp", 2, 0x006451E5);
SWIZZLE_PATCH(_TriggerClass_Load_Swizzle_Patch_1, edx, STRINGIZE(TriggerClass::Load()), STRINGIZE(TriggerClass::Class), "Trigger.cpp", 1, 0x006498AF);
SWIZZLE_PATCH(_TriggerClass_Load_Swizzle_Patch_2, esi, STRINGIZE(TriggerClass::Load()), STRINGIZE(TriggerClass::Next), "Trigger.cpp", 2, 0x006498BD);
SWIZZLE_PATCH(_TriggerTypeClass_Load_Swizzle_Patch_1, edx, STRINGIZE(TriggerTypeClass::Load()), STRINGIZE(TriggerTypeClass::Next), "TrigType.cpp", 1, 0x0064AAEF);
SWIZZLE_PATCH(_TriggerTypeClass_Load_Swizzle_Patch_2, eax, STRINGIZE(TriggerTypeClass::Load()), STRINGIZE(TriggerTypeClass::Event), "TrigType.cpp", 2, 0x0064AAFD);
SWIZZLE_PATCH(_TriggerTypeClass_Load_Swizzle_Patch_3, ecx, STRINGIZE(TriggerTypeClass::Load()), STRINGIZE(TriggerTypeClass::Action), "TrigType.cpp", 3, 0x0064AB0B);
SWIZZLE_PATCH(_TriggerTypeClass_Load_Swizzle_Patch_4, esi, STRINGIZE(TriggerTypeClass::Load()), STRINGIZE(TriggerTypeClass::House), "TrigType.cpp", 4, 0x0064AB19);
SWIZZLE_PATCH(_UnitClass_Load_Swizzle_Patch_1, eax, STRINGIZE(UnitClass::Load()), STRINGIZE(UnitClass::Class), "Unit.cpp", 1, 0x00659723);
SWIZZLE_PATCH(_UnitClass_Load_Swizzle_Patch_2, esi, STRINGIZE(UnitClass::Load()), STRINGIZE(UnitClass::FollowingMe), "Unit.cpp", 2, 0x00659734);
SWIZZLE_PATCH(_VoxelAnimClass_Load_Swizzle_Patch_1, edx, STRINGIZE(VoxelAnimClass::Load()), STRINGIZE(VoxelAnimClass::Class), "VoxelAnim.cpp", 1, 0x0065EFD5);
SWIZZLE_PATCH(_VoxelAnimClass_Load_Swizzle_Patch_2, eax, STRINGIZE(VoxelAnimClass::Load()), STRINGIZE(VoxelAnimClass::AttachedParticleSys), "VoxelAnim.cpp", 2, 0x0065EFF7);
SWIZZLE_PATCH(_VoxelAnimClass_Load_Swizzle_Patch_3, esi, STRINGIZE(VoxelAnimClass::Load()), STRINGIZE(VoxelAnimClass::House), "VoxelAnim.cpp", 3, 0x0065EFF7);
SWIZZLE_PATCH(_VoxelAnimTypeClass_Load_Swizzle_Patch_1, edx, STRINGIZE(VoxelAnimTypeClass::Load()), STRINGIZE(VoxelAnimTypeClass::Warhead), "VoxelAnimType.cpp", 1, 0x0065FEB6);
SWIZZLE_PATCH(_VoxelAnimTypeClass_Load_Swizzle_Patch_2, eax, STRINGIZE(VoxelAnimTypeClass::Load()), STRINGIZE(VoxelAnimTypeClass::BounceAnim), "VoxelAnimType.cpp", 2, 0x0065FEC7);
SWIZZLE_PATCH(_VoxelAnimTypeClass_Load_Swizzle_Patch_3, ecx, STRINGIZE(VoxelAnimTypeClass::Load()), STRINGIZE(VoxelAnimTypeClass::ExpireAnim), "VoxelAnimType.cpp", 3, 0x0065FED8);
SWIZZLE_PATCH(_VoxelAnimTypeClass_Load_Swizzle_Patch_4, edx, STRINGIZE(VoxelAnimTypeClass::Load()), STRINGIZE(VoxelAnimTypeClass::TrailerAnim), "VoxelAnimType.cpp", 4, 0x0065FEE9);
SWIZZLE_PATCH(_VoxelAnimTypeClass_Load_Swizzle_Patch_5, eax, STRINGIZE(VoxelAnimTypeClass::Load()), STRINGIZE(VoxelAnimTypeClass::Spawns), "VoxelAnimType.cpp", 5, 0x0065FEFA);
SWIZZLE_PATCH(_VoxelAnimTypeClass_Load_Swizzle_Patch_6, ecx, STRINGIZE(VoxelAnimTypeClass::Load()), STRINGIZE(VoxelAnimTypeClass::AttachedSystem), "VoxelAnimType.cpp", 6, 0x0065FF0B);
SWIZZLE_PATCH(_WarheadTypeClass_Load_Swizzle_Patch_1, edx, STRINGIZE(WarheadTypeClass::Load()), STRINGIZE(WarheadTypeClass::AnimList), "WarheadType.cpp", 1, 0x0066F871);
SWIZZLE_PATCH(_WarheadTypeClass_Load_Swizzle_Patch_2, esi, STRINGIZE(WarheadTypeClass::Load()), STRINGIZE(WarheadTypeClass::Particle), "WarheadType.cpp", 2, 0x0066F88D);

DECLARE_PATCH(_WaveClass_Load_Swizzle_Patch_1)
{
    _asm { mov [edi+0x0EC], ecx } // this->WaveClass.CurrentSonicPoints = ecx

    GET_REGISTER_STATIC(uintptr_t, pointer, edx);
    _asm { pushad } \
    VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_DBG(pointer, TIBSUN_SOURCE_PATH "Wave.cpp", 1, STRINGIZE(WaveClass::Load()), STRINGIZE(WaveClass::field_F0));
    _asm { popad } \
    JMP(0x00670BCB);
}

SWIZZLE_PATCH(_WaveClass_Load_Swizzle_Patch_2, eax, STRINGIZE(WaveClass::Load()), STRINGIZE(WaveClass::Source), "Wave.cpp", 2, 0x00670BDC);
SWIZZLE_PATCH(_WaveClass_Load_Swizzle_Patch_3, eax, STRINGIZE(WaveClass::Load()), STRINGIZE(WaveClass::Cells), "Wave.cpp", 3, 0x00670C8B);
SWIZZLE_PATCH(_WeaponTypeClass_Load_Swizzle_Patch_1, ecx, STRINGIZE(WeaponTypeClass::Load()), STRINGIZE(WeaponTypeClass::WarheadPtr), "WeaponType.cpp", 1, 0x006816DC);
SWIZZLE_PATCH(_WeaponTypeClass_Load_Swizzle_Patch_2, edx, STRINGIZE(WeaponTypeClass::Load()), STRINGIZE(WeaponTypeClass::Bullet), "WeaponType.cpp", 2, 0x006816EA);
SWIZZLE_PATCH(_WeaponTypeClass_Load_Swizzle_Patch_3, eax, STRINGIZE(WeaponTypeClass::Load()), STRINGIZE(WeaponTypeClass::AttachedParticleSystem), "WeaponType.cpp", 3, 0x006816FB);
SWIZZLE_PATCH(_WeaponTypeClass_Load_Swizzle_Patch_4, edx, STRINGIZE(WeaponTypeClass::Load()), STRINGIZE(WeaponTypeClass::Anim), "WeaponType.cpp", 4, 0x0068171C);


#if 0
/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or deconstructor.
 * 
 *  @note: All functions must not be virtual and must also be prefixed
 *         with "_" to prevent accidental virtualization.
 */
static class FakeSwizzleManagerClass final : public ViniferaSwizzleManagerClass
{
    public:
        COM_DECLSPEC_NOTHROW LONG STDAPICALLTYPE _Reset()
        {
            //DEBUG_INFO("SwizzleManager::Reset\n");
            return ViniferaSwizzleManagerClass::Reset();
        }

        COM_DECLSPEC_NOTHROW LONG STDAPICALLTYPE _Swizzle(void **pointer)
        {
            //DEBUG_INFO("SwizzleManager::Swizzle - pointer 0x%08X, 0x%08X\n", pointer, *pointer);
            return ViniferaSwizzleManagerClass::Swizzle(pointer);
        }

        COM_DECLSPEC_NOTHROW LONG STDAPICALLTYPE _Fetch_Swizzle_ID(void *pointer, LONG *id)
        {
            //DEBUG_INFO("SwizzleManager::Fetch_Swizzle_ID - pointer 0x%08X id 0x%08X\n", pointer, id);
            return ViniferaSwizzleManagerClass::Fetch_Swizzle_ID(pointer, id);
        }

        COM_DECLSPEC_NOTHROW LONG STDAPICALLTYPE _Here_I_Am(LONG id, void *pointer)
        {
            //DEBUG_INFO("SwizzleManager::Here_I_Am - id 0x%08X pointer 0x%08X\n", id, pointer);
            return ViniferaSwizzleManagerClass::Here_I_Am(id, pointer);
        }
};
#endif


void NewSwizzle_Hooks()
{
#ifdef VINIFERA_USE_NEW_SWIZZLE_MANAGER

    /**
     *  Removes dynamic inits for SwizzleManager global.
     */
    Patch_Byte(0x0060D8A0, 0xC3); // retn
    Patch_Byte(0x0060D920, 0xC3); // retn

#if 0
    /**
     *  Replace the implementation of SwizzleManagerClass with our own implementation.
     */
    Patch_Jump(0x0060DA60, &FakeSwizzleManagerClass::_Reset);
    Patch_Jump(0x0060DA70, &FakeSwizzleManagerClass::_Swizzle);
    Patch_Jump(0x0060DCC0, &FakeSwizzleManagerClass::_Fetch_Swizzle_ID);
    Patch_Jump(0x0060DAF0, &FakeSwizzleManagerClass::_Here_I_Am);
#endif

#if 0
    /**
     *  These patches fetch the virtual table from the dummy instance and
     *  replaces any references to the games SwizzleManagerClass with our
     *  implementation.
     */
    uintptr_t *vtable = (uintptr_t *)((uintptr_t *)&ViniferaSwizzleManager)[0];
    Patch_Dword(0x0060D8F8+6, (uintptr_t)vtable);
    Patch_Dword(0x0060D926+6, (uintptr_t)vtable);
    Patch_Dword(0x0060D9F2+2, (uintptr_t)vtable);
    Patch_Dword(0x0060DA04+2, (uintptr_t)vtable);
#endif

#endif

    /**
     *  Replace ALL calls to Swizzler functions in the original game with calls to our debug wrappers.
     */
    Patch_Jump(0x005D69BD, &_Load_Game_Swizzle_Reset_Pre_Patch);
    Patch_Jump(0x005D6B39, &_Load_Game_Swizzle_Reset_Post_Patch);
    Patch_Jump(0x005DD65E, &_Read_Scenario_INI_Swizzle_Reset_Start_Patch);
    Patch_Jump(0x005DDEFD, &_Read_Scenario_INI_Swizzle_Reset_End_Patch);

    Patch_Jump(0x00405D40, &_AbstractClass_Abstract_Load_Here_I_Am_Patch);
    Patch_Jump(0x004CE380, &_HouseTypeClass_Load_Here_I_Am_Patch);
    Patch_Jump(0x00506620, &_LocomotionClass_Load_Here_I_Am_Patch);
    Patch_Jump(0x0061F420, &_TagTypeClass_Read_Scenario_INI_Here_I_Am_Patch);
    Patch_Jump(0x006218FD, &_TaskForceClass_Read_Scenario_INI_Here_I_Am_Patch);
    Patch_Jump(0x00663004, &_VeinholeMonsterClass_Load_Here_I_Am_Patch);

    Patch_Jump(0x004CE3BC, &_HouseTypeClass_Save_Fetch_Swizzle_ID_Patch);

    Patch_Jump(0x0040ECB3, &_AircraftClass_Load_Swizzle_Patch);
    Patch_Jump(0x004106D7, &_AITriggerTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x004106E8, &_AITriggerTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x004106F9, &_AITriggerTypeClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x00412A27, &_AlphaShapeClass_Load_Swizzle_Patch);
    Patch_Jump(0x004164E7, &_AnimClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x004164F5, &_AnimClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x00419692, &_AnimTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x004196A3, &_AnimTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x004196B4, &_AnimTypeClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x004196C5, &_AnimTypeClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x004196D6, &_AnimTypeClass_Load_Swizzle_Patch_5);
    Patch_Jump(0x004196E7, &_AnimTypeClass_Load_Swizzle_Patch_6);
    Patch_Jump(0x004196F8, &_AnimTypeClass_Load_Swizzle_Patch_7);
    Patch_Jump(0x0041FD9A, &_BaseClass_Load_Swizzle_Patch);
    Patch_Jump(0x00422AFA, &_BuildingLightClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x00422B0B, &_BuildingLightClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x004254B4, &_NeuronClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x004254C2, &_NeuronClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x004254D0, &_NeuronClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x00438171, &_BuildingClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x00438182, &_BuildingClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x00438193, &_BuildingClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x004381A4, &_BuildingClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x004381B5, &_BuildingClass_Load_Swizzle_Patch_5);
    Patch_Jump(0x004381CB, &_BuildingClass_Load_Swizzle_Patch_6);
    Patch_Jump(0x004381E7, &_BuildingClass_Load_Swizzle_Patch_7);
    Patch_Jump(0x00443486, &_BuildingTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0044349D, &_BuildingTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0044713F, &_BulletClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0044714D, &_BulletClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0044715E, &_BulletClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x0044716F, &_BulletClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x00448581, &_BulletTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x00448592, &_BulletTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x004598F8, &_CellClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x00459943, &_CellClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x00459951, &_CellClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x0045995F, &_CellClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x0045996D, &_CellClass_Load_Swizzle_Patch_5);
    Patch_Jump(0x0045997B, &_CellClass_Load_Swizzle_Patch_6);
    Patch_Jump(0x00484251, &_DropshipLoadoutClass_Load_Swizzle_Patch);
    Patch_Jump(0x004976B2, &_FactoryClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x004976C7, &_FactoryClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x004976D5, &_FactoryClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x0049F47C, &_FoggedObjectClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0049F547, &_FoggedObjectClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x004A60F9, &_FootClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x004A610A, &_FootClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x004A611B, &_FootClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x004A612C, &_FootClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x004A613D, &_FootClass_Load_Swizzle_Patch_5);
    Patch_Jump(0x004A615D, &_FootClass_Load_Swizzle_Patch_6);
    Patch_Jump(0x004A6188, &_FootClass_Load_Swizzle_Patch_7);
    Patch_Jump(0x004C4B8C, &_HouseClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x004C4B9D, &_HouseClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x004C4BAE, &_HouseClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x004C4BBF, &_HouseClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x004C4BD0, &_HouseClass_Load_Swizzle_Patch_5);
    Patch_Jump(0x004C4BE1, &_HouseClass_Load_Swizzle_Patch_6);
    Patch_Jump(0x004C4BF7, &_HouseClass_Load_Swizzle_Patch_7);
    Patch_Jump(0x004C4E15, &_HouseClass_Load_Swizzle_Patch_8);
    Patch_Jump(0x004C4EF3, &_HouseClass_Load_Swizzle_Patch_9);
    Patch_Jump(0x004C4FB2, &_HouseClass_Load_Swizzle_Patch_10);
    Patch_Jump(0x004C4FD7, &_HouseClass_Load_Swizzle_Patch_11);
    Patch_Jump(0x004C4FF9, &_HouseClass_Load_Swizzle_Patch_12);
    Patch_Jump(0x004D94AD, &_InfantryClass_Load_Swizzle_Patch);
    Patch_Jump(0x004F2297, &_IsometricTileClass_Load_Swizzle_Patch);
    Patch_Jump(0x004F8646, &_IsometricTileTypeClass_Load_Swizzle_Patch);
    Patch_Jump(0x004FCFFA, &_LayerClass_Load_Swizzle_Patch);
    Patch_Jump(0x00506648, &_LocomotionClass_Load_Swizzle_Patch);
    Patch_Jump(0x005627F1, &_SidebarClass_StripClass_Load_Swizzle_Patch);
    Patch_Jump(0x00562819, &_DisplayClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0056282A, &_DisplayClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0056283B, &_DisplayClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x00586707, &_ObjectClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x00586715, &_ObjectClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0058C507, &_OverlayClass_Load_Swizzle_Patch);
    Patch_Jump(0x0058D842, &_OverlayTypeClass_Load_Swizzle_Patch);
    Patch_Jump(0x005A4E64, &_ParticleClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x005A4E75, &_ParticleClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x005A763A, &_ParticleSystemClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x005A764B, &_ParticleSystemClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x005A765C, &_ParticleSystemClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x005A7709, &_ParticleSystemClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x005AF996, &_ParticleTypeClass_Load_Swizzle_Patch);
    Patch_Jump(0x005BDAB7, &_RadioClass_Load_Swizzle_Patch);
    Patch_Jump(0x005D01DA, &_RulesClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x005D01EB, &_RulesClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x005D01FC, &_RulesClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x005D020A, &_RulesClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x005D0218, &_RulesClass_Load_Swizzle_Patch_5);
    Patch_Jump(0x005D0226, &_RulesClass_Load_Swizzle_Patch_6);
    Patch_Jump(0x005D0237, &_RulesClass_Load_Swizzle_Patch_7);
    Patch_Jump(0x005D0248, &_RulesClass_Load_Swizzle_Patch_8);
    Patch_Jump(0x005D0259, &_RulesClass_Load_Swizzle_Patch_9);
    Patch_Jump(0x005D026A, &_RulesClass_Load_Swizzle_Patch_10);
    Patch_Jump(0x005D027B, &_RulesClass_Load_Swizzle_Patch_11);
    Patch_Jump(0x005D028C, &_RulesClass_Load_Swizzle_Patch_12);
    Patch_Jump(0x005D029D, &_RulesClass_Load_Swizzle_Patch_13);
    Patch_Jump(0x005D02AE, &_RulesClass_Load_Swizzle_Patch_14);
    Patch_Jump(0x005D02BF, &_RulesClass_Load_Swizzle_Patch_15);
    Patch_Jump(0x005D02D0, &_RulesClass_Load_Swizzle_Patch_16);
    Patch_Jump(0x005D02E1, &_RulesClass_Load_Swizzle_Patch_17);
    Patch_Jump(0x005D02F2, &_RulesClass_Load_Swizzle_Patch_18);
    Patch_Jump(0x005D0303, &_RulesClass_Load_Swizzle_Patch_19);
    Patch_Jump(0x005D0314, &_RulesClass_Load_Swizzle_Patch_20);
    Patch_Jump(0x005D0325, &_RulesClass_Load_Swizzle_Patch_21);
    Patch_Jump(0x005D0336, &_RulesClass_Load_Swizzle_Patch_22);
    Patch_Jump(0x005D0347, &_RulesClass_Load_Swizzle_Patch_23);
    Patch_Jump(0x005D0358, &_RulesClass_Load_Swizzle_Patch_24);
    Patch_Jump(0x005D0369, &_RulesClass_Load_Swizzle_Patch_25);
    Patch_Jump(0x005D037A, &_RulesClass_Load_Swizzle_Patch_26);
    Patch_Jump(0x005D038B, &_RulesClass_Load_Swizzle_Patch_27);
    Patch_Jump(0x005D039C, &_RulesClass_Load_Swizzle_Patch_28);
    Patch_Jump(0x005D03AD, &_RulesClass_Load_Swizzle_Patch_29);
    Patch_Jump(0x005D03BE, &_RulesClass_Load_Swizzle_Patch_30);
    Patch_Jump(0x005D03CF, &_RulesClass_Load_Swizzle_Patch_31);
    Patch_Jump(0x005D03E0, &_RulesClass_Load_Swizzle_Patch_32);
    Patch_Jump(0x005D03F1, &_RulesClass_Load_Swizzle_Patch_33);
    Patch_Jump(0x005D0402, &_RulesClass_Load_Swizzle_Patch_34);
    Patch_Jump(0x005D0413, &_RulesClass_Load_Swizzle_Patch_35);
    Patch_Jump(0x005D0424, &_RulesClass_Load_Swizzle_Patch_36);
    Patch_Jump(0x005D0435, &_RulesClass_Load_Swizzle_Patch_37);
    Patch_Jump(0x005D0446, &_RulesClass_Load_Swizzle_Patch_38);
    Patch_Jump(0x005D0457, &_RulesClass_Load_Swizzle_Patch_39);
    Patch_Jump(0x005D0468, &_RulesClass_Load_Swizzle_Patch_40);
    Patch_Jump(0x005D0479, &_RulesClass_Load_Swizzle_Patch_41);
    Patch_Jump(0x005D048A, &_RulesClass_Load_Swizzle_Patch_42);
    Patch_Jump(0x005D049B, &_RulesClass_Load_Swizzle_Patch_43);
    Patch_Jump(0x005D04AC, &_RulesClass_Load_Swizzle_Patch_44);
    Patch_Jump(0x005D04BD, &_RulesClass_Load_Swizzle_Patch_45);
    Patch_Jump(0x005D04CE, &_RulesClass_Load_Swizzle_Patch_46);
    Patch_Jump(0x005D04DF, &_RulesClass_Load_Swizzle_Patch_47);
    Patch_Jump(0x005D04F0, &_RulesClass_Load_Swizzle_Patch_48);
    Patch_Jump(0x005D0501, &_RulesClass_Load_Swizzle_Patch_49);
    Patch_Jump(0x005D0512, &_RulesClass_Load_Swizzle_Patch_50);
    Patch_Jump(0x005D0523, &_RulesClass_Load_Swizzle_Patch_51);
    Patch_Jump(0x005D0534, &_RulesClass_Load_Swizzle_Patch_52);
    Patch_Jump(0x005D0545, &_RulesClass_Load_Swizzle_Patch_53);
    Patch_Jump(0x005D0556, &_RulesClass_Load_Swizzle_Patch_54);
    Patch_Jump(0x005D0567, &_RulesClass_Load_Swizzle_Patch_55);
    Patch_Jump(0x005D0578, &_RulesClass_Load_Swizzle_Patch_56);
    Patch_Jump(0x005D0589, &_RulesClass_Load_Swizzle_Patch_57);
    Patch_Jump(0x005D059A, &_RulesClass_Load_Swizzle_Patch_58);
    Patch_Jump(0x005D05AB, &_RulesClass_Load_Swizzle_Patch_59);
    Patch_Jump(0x005D05BC, &_RulesClass_Load_Swizzle_Patch_60);
    Patch_Jump(0x005D05CD, &_RulesClass_Load_Swizzle_Patch_61);
    Patch_Jump(0x005D05DE, &_RulesClass_Load_Swizzle_Patch_62);
    Patch_Jump(0x005D05EF, &_RulesClass_Load_Swizzle_Patch_63);
    Patch_Jump(0x005D0600, &_RulesClass_Load_Swizzle_Patch_64);
    Patch_Jump(0x005D0611, &_RulesClass_Load_Swizzle_Patch_65);
    Patch_Jump(0x005D0622, &_RulesClass_Load_Swizzle_Patch_66);
    Patch_Jump(0x005D0633, &_RulesClass_Load_Swizzle_Patch_67);
    Patch_Jump(0x005D0644, &_RulesClass_Load_Swizzle_Patch_68);
    Patch_Jump(0x005D0655, &_RulesClass_Load_Swizzle_Patch_69);
    Patch_Jump(0x005D0666, &_RulesClass_Load_Swizzle_Patch_70);
    Patch_Jump(0x005D0674, &_RulesClass_Load_Swizzle_Patch_71);
    Patch_Jump(0x005D0682, &_RulesClass_Load_Swizzle_Patch_72);
    Patch_Jump(0x005D0690, &_RulesClass_Load_Swizzle_Patch_73);
    Patch_Jump(0x005D06A1, &_RulesClass_Load_Swizzle_Patch_74);
    Patch_Jump(0x005D06B2, &_RulesClass_Load_Swizzle_Patch_75);
    Patch_Jump(0x005D06C3, &_RulesClass_Load_Swizzle_Patch_76);
    Patch_Jump(0x005D06D4, &_RulesClass_Load_Swizzle_Patch_77);
    Patch_Jump(0x005D06EE, &_RulesClass_Load_Swizzle_Patch_78);
    Patch_Jump(0x005D0716, &_RulesClass_Load_Swizzle_Patch_79);
    Patch_Jump(0x005D0741, &_RulesClass_Load_Swizzle_Patch_80);
    Patch_Jump(0x005D076C, &_RulesClass_Load_Swizzle_Patch_81);
    Patch_Jump(0x005D0797, &_RulesClass_Load_Swizzle_Patch_82);
    Patch_Jump(0x005D07C2, &_RulesClass_Load_Swizzle_Patch_83);
    Patch_Jump(0x005D07ED, &_RulesClass_Load_Swizzle_Patch_84);
    Patch_Jump(0x005D0818, &_RulesClass_Load_Swizzle_Patch_85);
    Patch_Jump(0x005D0843, &_RulesClass_Load_Swizzle_Patch_86);
    Patch_Jump(0x005D086E, &_RulesClass_Load_Swizzle_Patch_87);
    Patch_Jump(0x005D0899, &_RulesClass_Load_Swizzle_Patch_88);
    Patch_Jump(0x005D08C4, &_RulesClass_Load_Swizzle_Patch_89);
    Patch_Jump(0x005D08EF, &_RulesClass_Load_Swizzle_Patch_90);
    Patch_Jump(0x005D091A, &_RulesClass_Load_Swizzle_Patch_91);
    Patch_Jump(0x005D0945, &_RulesClass_Load_Swizzle_Patch_92);
    Patch_Jump(0x005D0970, &_RulesClass_Load_Swizzle_Patch_93);
    Patch_Jump(0x005D099B, &_RulesClass_Load_Swizzle_Patch_94);
    Patch_Jump(0x005D09C6, &_RulesClass_Load_Swizzle_Patch_95);
    Patch_Jump(0x005D09F1, &_RulesClass_Load_Swizzle_Patch_96);
    Patch_Jump(0x005D0A1C, &_RulesClass_Load_Swizzle_Patch_97);
    Patch_Jump(0x005D0A47, &_RulesClass_Load_Swizzle_Patch_98);
    Patch_Jump(0x005D0A72, &_RulesClass_Load_Swizzle_Patch_99);
    Patch_Jump(0x005D0A9D, &_RulesClass_Load_Swizzle_Patch_100);
    Patch_Jump(0x005D0AC8, &_RulesClass_Load_Swizzle_Patch_101);
    Patch_Jump(0x005D0AF3, &_RulesClass_Load_Swizzle_Patch_102);
    Patch_Jump(0x005D0B1E, &_RulesClass_Load_Swizzle_Patch_103);
    Patch_Jump(0x005D0B49, &_RulesClass_Load_Swizzle_Patch_104);
    Patch_Jump(0x005D0B74, &_RulesClass_Load_Swizzle_Patch_105);
    Patch_Jump(0x005D0B9F, &_RulesClass_Load_Swizzle_Patch_106);
    Patch_Jump(0x005D0BCA, &_RulesClass_Load_Swizzle_Patch_107);
    Patch_Jump(0x005D0BF5, &_RulesClass_Load_Swizzle_Patch_108);
    Patch_Jump(0x005D0C1A, &_RulesClass_Load_Swizzle_Patch_109);
    Patch_Jump(0x005D7B0F, &_Load_Misc_Values_Swizzle_Patch_1);
    Patch_Jump(0x005D7B35, &_Load_Misc_Values_Swizzle_Patch_2);
    Patch_Jump(0x005D7C09, &_Load_Misc_Values_Swizzle_Patch_3);
    Patch_Jump(0x005D7D2E, &_Load_Misc_Values_Swizzle_Patch_4);
    Patch_Jump(0x005D7DF0, &_Load_Misc_Values_Swizzle_Patch_5);
    Patch_Jump(0x005DF6C7, &_Scenario_Load_Swizzle_Patch);
    Patch_Jump(0x005E7A84, &_ScriptClass_Load_Swizzle_Patch);
    Patch_Jump(0x005FAE37, &_SmudgeClass_Load_Swizzle_Patch);
    Patch_Jump(0x0060C7BF, &_SuperClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0060C7CD, &_SuperClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0060D18B, &_SuperWeaponTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0060D19C, &_SuperWeaponTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x00617F5F, &_Tactical_Load_Swizzle_Patch);
    Patch_Jump(0x0061D8D4, &_TActionClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0061D8E2, &_TActionClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0061D8F0, &_TActionClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x0061D8FE, &_TActionClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x0061EC14, &_TagClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0061EC22, &_TagClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0061F854, &_TagTypeClass_Load_Swizzle_Patch);
    Patch_Jump(0x00621D4A, &_TaskForceClass_Load_Swizzle_Patch);
    Patch_Jump(0x00625295, &_TeamClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x006252A3, &_TeamClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x006252B1, &_TeamClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x006252BF, &_TeamClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x006252CD, &_TeamClass_Load_Swizzle_Patch_5);
    Patch_Jump(0x006252DB, &_TeamClass_Load_Swizzle_Patch_6);
    Patch_Jump(0x006252E9, &_TeamClass_Load_Swizzle_Patch_7);
    Patch_Jump(0x006252F7, &_TeamClass_Load_Swizzle_Patch_8);
    Patch_Jump(0x00625305, &_TeamClass_Load_Swizzle_Patch_9);
    Patch_Jump(0x00625313, &_TeamClass_Load_Swizzle_Patch_10);
    Patch_Jump(0x00625321, &_TeamClass_Load_Swizzle_Patch_11);
    Patch_Jump(0x00628F07, &_TeamTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x00628F18, &_TeamTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x00628F29, &_TeamTypeClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x00628F3A, &_TeamTypeClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x00638DE2, &_TechnoClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x00638DF8, &_TechnoClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x00638E0F, &_TechnoClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x00638E20, &_TechnoClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x00638E31, &_TechnoClass_Load_Swizzle_Patch_5);
    Patch_Jump(0x00638E42, &_TechnoClass_Load_Swizzle_Patch_6);
    Patch_Jump(0x00638E53, &_TechnoClass_Load_Swizzle_Patch_7);
    Patch_Jump(0x00638E64, &_TechnoClass_Load_Swizzle_Patch_8);
    Patch_Jump(0x0063D9F8, &_TechnoTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0063DA09, &_TechnoTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0063DA1A, &_TechnoTypeClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x0063DD82, &_TechnoTypeClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x0063DDAD, &_TechnoTypeClass_Load_Swizzle_Patch_5);
    Patch_Jump(0x0063DDD8, &_TechnoTypeClass_Load_Swizzle_Patch_6);
    Patch_Jump(0x0063DDF9, &_TechnoTypeClass_Load_Swizzle_Patch_7);
    Patch_Jump(0x0063DE1F, &_TechnoTypeClass_Load_Swizzle_Patch_8);
    Patch_Jump(0x006407B7, &_TerrainClass_Load_Swizzle_Patch);
    Patch_Jump(0x00642DD4, &_TEventClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x00642DE2, &_TEventClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0064516A, &_TiberiumClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x006451DA, &_TiberiumClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x006498A4, &_TriggerClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x006498B2, &_TriggerClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0064AAE4, &_TriggerTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0064AAF2, &_TriggerTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0064AB00, &_TriggerTypeClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x0064AB0E, &_TriggerTypeClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x00659718, &_UnitClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x00659729, &_UnitClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0065EFCA, &_VoxelAnimClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0065EFDB, &_VoxelAnimClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0065EFEC, &_VoxelAnimClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x0065FEAB, &_VoxelAnimTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0065FEBC, &_VoxelAnimTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x0065FECD, &_VoxelAnimTypeClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x0065FEDE, &_VoxelAnimTypeClass_Load_Swizzle_Patch_4);
    Patch_Jump(0x0065FEEF, &_VoxelAnimTypeClass_Load_Swizzle_Patch_5);
    Patch_Jump(0x0065FF00, &_VoxelAnimTypeClass_Load_Swizzle_Patch_6);
    Patch_Jump(0x0066F866, &_WarheadTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x0066F882, &_WarheadTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x00670BBA, &_WaveClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x00670BD1, &_WaveClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x00670C80, &_WaveClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x006816D1, &_WeaponTypeClass_Load_Swizzle_Patch_1);
    Patch_Jump(0x006816DF, &_WeaponTypeClass_Load_Swizzle_Patch_2);
    Patch_Jump(0x006816F0, &_WeaponTypeClass_Load_Swizzle_Patch_3);
    Patch_Jump(0x00681711, &_WeaponTypeClass_Load_Swizzle_Patch_4);

}

