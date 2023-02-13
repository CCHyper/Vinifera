/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_FUNCTIONS.CPP
 *
 *  @authors       CCHyper
 *
 *  @brief         General functions.
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
#include "vinifera_functions.h"
#include "vinifera_globals.h"
#include "vinifera_newdel.h"
#include "tibsun_globals.h"
#include "cncnet4.h"
#include "cncnet4_globals.h"
#include "cncnet5_globals.h"
#include "rulesext.h"
#include "ccfile.h"
#include "ccini.h"
#include "filestraw.h"
#include "readline.h"
#include "cd.h"
#include "ebolt.h"
#include "optionsext.h"
#include "rulesext.h"
#include "sessionext.h"
#include "scenarioext.h"
#include "tacticalext.h"
#include "tclassfactory.h"
#include "testlocomotion.h"
#include "extension.h"
#include "theatertype.h"
#include "uicontrol.h"
#include "luascript.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <string>


static DynamicVectorClass<Wstring> ViniferaSearchPaths;


#if defined(LUA_ENABLED)

static int Lua_Utility_Get_Time(lua_State *state)
{
    return 0;
}

static int Lua_Debug_Message_Info(lua_State *state)
{
    if (lua_gettop(state) < 1) {
        lua_pushboolean(state, 0);
        return luaL_error(state, "%s expects at least 1 argument (string)!", __FUNCDNAME__);
    }

    int ret = lua_tointeger(state, -1);

    /**
     *  x
     */
    if (ret < 0) {
        lua_pushboolean(state, 0);
        return -1;
    }

    /**
     *  x
     */
    const char *message = lua_tostring(state, -1); // grab path string from top of stack

    DEBUG_INFO(message);

    lua_pushboolean(state, int(true));

    return 1;
}

#include "unit.h"
#include "unittype.h"
static int Lua_Unit_Create_One_Of(lua_State *state)
{
    /**
     *  Argument sanity check.
     */
    if (lua_gettop(state) != 3) {
        lua_pushboolean(state, 0);
        return luaL_error(state, "%s expects exactly 3 arguments (UnitType, CellX, CellY)!", __FUNCDNAME__);
    }

    UnitType unit = (UnitType)lua_tointeger(state, 1); // first argument

    if (unit < 0 || unit > UnitTypes.Count()) {
        lua_pushboolean(state, 0);
        return -1;
    }

    int cell_x = lua_tointeger(state, 2); // second argument
    int cell_y = lua_tointeger(state, 3); // third argument

    Cell cell(cell_x, cell_y);

    if (!cell) {
        lua_pushboolean(state, 0);
        return -1;
    }

    const char *name = UnitTypeClass::Name_From(unit);
    DEBUG_INFO("Lua: UnitType::CreateOneOf(%s)\n", name);

    bool result = UnitTypes[unit]->Create_And_Place(Cell(cell_x, cell_y));

    lua_pushboolean(state, int(result));

    return 1;
}

#include "house.h"
#include "housetype.h"
static int Lua_Multi_Flag_To_Die(lua_State *state)
{
    if (lua_gettop(state) != 1) {
        return luaL_error(state, "%s expects exactly 1 argument (HouseType)!", __FUNCDNAME__);
    }

    if (Session.Singleplayer_Game()) {
        lua_pushboolean(state, 0);
        return -1;
    }

    int house_id = lua_tointeger(state, -1);

    if (house_id < 0 || house_id > Houses.Count()) {
        lua_pushboolean(state, 0);
        return -1;
    }

    bool result = Houses[house_id]->Flag_To_Die();

    lua_pushboolean(state, int(result));

    return 1;
}

static int Lua_Player_Flag_To_Die(lua_State *state)
{
    if (!Session.Singleplayer_Game()) {
        lua_pushboolean(state, 0);
        return -1;
    }

    bool result = PlayerPtr->Flag_To_Die();

    lua_pushboolean(state, int(result));

    return 1;
}

static int Lua_TAction_Deactivate_Firestorm_Defense(lua_State *state) { return 0; }
static int Lua_TAction_Activate_Firestorm_Defense(lua_State *state) { return 0; }
static int Lua_TAction_Lightning_Strike_At(lua_State *state) { return 0; }
static int Lua_TAction_Remove_Particle_Anim_At(lua_State *state) { return 0; }
static int Lua_TAction_Particle_Anim_At(lua_State *state) { return 0; }
static int Lua_TAction_Wakeup_Self(lua_State *state) { return 0; }
static int Lua_TAction_Vein_Growth(lua_State *state) { return 0; }
static int Lua_TAction_Tiberium_Growth(lua_State *state) { return 0; }
static int Lua_TAction_Ice_Growth(lua_State *state) { return 0; }
static int Lua_TAction_Wakeup_Sleepers(lua_State *state) { return 0; }
static int Lua_TAction_Wakeup_Harmless(lua_State *state) { return 0; }
static int Lua_TAction_Wakeup_Group(lua_State *state) { return 0; }
static int Lua_TAction_Annouce_Win(lua_State *state) { return 0; }
static int Lua_TAction_Annouce_Lose(lua_State *state) { return 0; }
static int Lua_TAction_Force_End(lua_State *state) { return 0; }
static int Lua_TAction_Apply_100_Damage(lua_State *state) { return 0; }
static int Lua_TAction_Small_Light_Flash_At(lua_State *state) { return 0; }
static int Lua_TAction_Medium_Light_Flash_At(lua_State *state) { return 0; }
static int Lua_TAction_Large_Light_Flash_At(lua_State *state) { return 0; }
static int Lua_TAction_Sell_Building(lua_State *state) { return 0; }
static int Lua_TAction_Go_Bezerk(lua_State *state) { return 0; }
static int Lua_TAction_Turn_Off_Building(lua_State *state) { return 0; }
static int Lua_TAction_Turn_On_Building(lua_State *state) { return 0; }
static int Lua_TAction_Change_House(lua_State *state) { return 0; }
static int Lua_TAction_Set_Team_ID(lua_State *state) { return 0; }
static int Lua_TAction_All_Change_House(lua_State *state) { return 0; }
static int Lua_TAction_Text_Trigger(lua_State *state) { return 0; }
static int Lua_TAction_Make_Ally(lua_State *state) { return 0; }
static int Lua_TAction_Make_Enemy(lua_State *state) { return 0; }
static int Lua_TAction_Preferred_Target(lua_State *state) { return 0; }
static int Lua_TAction_Base_Building(lua_State *state) { return 0; }
static int Lua_TAction_Grow_Shroud(lua_State *state) { return 0; }
static int Lua_TAction_Set_Global(lua_State *state) { return 0; }
static int Lua_TAction_Clear_Global(lua_State *state) { return 0; }
static int Lua_TAction_Reveal_Area(lua_State *state) { return 0; }
static int Lua_TAction_Reduce_Tiberium_At(lua_State *state) { return 0; }
static int Lua_TAction_Reveal_Zone(lua_State *state) { return 0; }
static int Lua_TAction_Reveal_Map(lua_State *state) { return 0; }
static int Lua_TAction_Start_Timer(lua_State *state) { return 0; }
static int Lua_TAction_Stop_Timer(lua_State *state) { return 0; }
static int Lua_TAction_Timer_Extend(lua_State *state) { return 0; }
static int Lua_TAction_Timer_Shorten(lua_State *state) { return 0; }
static int Lua_TAction_Timer_Set(lua_State *state) { return 0; }
static int Lua_TAction_Play_Movie(lua_State *state) { return 0; }
static int Lua_TAction_Play_Ingame_Movie(lua_State *state) { return 0; }
static int Lua_TAction_Play_Sound(lua_State *state) { return 0; }
static int Lua_TAction_Play_Sound_At_Random_Waypoint(lua_State *state) { return 0; }
static int Lua_TAction_Play_Sound_At(lua_State *state) { return 0; }
static int Lua_TAction_Play_Music(lua_State *state) { return 0; }
static int Lua_TAction_Play_Speech(lua_State *state) { return 0; }
static int Lua_TAction_One_Time_Special(lua_State *state) { return 0; }
static int Lua_TAction_Full_Special(lua_State *state) { return 0; }
static int Lua_TAction_Place_Drop_Zone_Flare(lua_State *state) { return 0; }

/**
 *  x
 * 
 *  @input:
 *    HousesType - The player index to set the flag to win.
 * 
 *  @output:
 * 
 *  @author: CCHyper
 */
static int Lua_TAction_Win(lua_State *state)
{
    HousesType house = (HousesType)lua_tointeger(state, -1);

    if (house != HOUSE_NONE) {

        HouseClass *hptr = HouseClass::As_Pointer(house);

        if (hptr != nullptr) {

            if (hptr->ID == PlayerPtr->Class->House) {
                PlayerPtr->Flag_To_Win();
            }
            else {
                PlayerPtr->Flag_To_Lose();
            }

            bool success = hptr->Fire_Sale();
            lua_pushboolean(state, int(success));
        }
    }

    return 1;
}

static int Lua_TAction_Lose(lua_State *state) { return 0; }
static int Lua_TAction_Begin_Production(lua_State *state) { return 0; }
static int Lua_TAction_Fire_Sale(lua_State *state) { return 0; }
static int Lua_TAction_Begin_Autocreate(lua_State *state) { return 0; }
static int Lua_TAction_Create_Team(lua_State *state) { return 0; }
static int Lua_TAction_Destroy_Team(lua_State *state) { return 0; }
static int Lua_TAction_Flash_Team(lua_State *state) { return 0; }
static int Lua_TAction_Disable_Speech(lua_State *state) { return 0; }
static int Lua_TAction_Enable_Speech(lua_State *state) { return 0; }
static int Lua_TAction_Talk_Bubble(lua_State *state) { return 0; }
static int Lua_TAction_Reinforcements(lua_State *state) { return 0; }
static int Lua_TAction_Reinforcement_At(lua_State *state) { return 0; }
static int Lua_TAction_All_To_Hunt(lua_State *state) { return 0; }
static int Lua_TAction_Destroy_Object(lua_State *state) { return 0; }
static int Lua_TAction_Change_Zoom_Level(lua_State *state) { return 0; }
static int Lua_TAction_Resize_Player_View(lua_State *state) { return 0; }
static int Lua_TAction_Play_Anim_At(lua_State *state) { return 0; }
static int Lua_TAction_Do_Explosion_At(lua_State *state) { return 0; }
static int Lua_TAction_Meteor_Impact_At(lua_State *state) { return 0; }
static int Lua_TAction_Ion_Storm_Start(lua_State *state) { return 0; }
static int Lua_TAction_Ion_Storm_End(lua_State *state) { return 0; }
static int Lua_TAction_Lock_Input(lua_State *state) { return 0; }
static int Lua_TAction_Unlock_Input(lua_State *state) { return 0; }
static int Lua_TAction_Center_Camera_At(lua_State *state) { return 0; }
static int Lua_TAction_Zoom_In(lua_State *state) { return 0; }
static int Lua_TAction_Zoom_Out(lua_State *state) { return 0; }
static int Lua_TAction_Reshroud_Map(lua_State *state) { return 0; }
static int Lua_TAction_Change_Spotlight_Behavior(lua_State *state) { return 0; }
static int Lua_TAction_Destroy_Trigger(lua_State *state) { return 0; }
static int Lua_TAction_Destroy_Tag(lua_State *state) { return 0; }
static int Lua_TAction_Force_Trigger(lua_State *state) { return 0; }
static int Lua_TAction_Enable_Trigger(lua_State *state) { return 0; }
static int Lua_TAction_Disable_Trigger(lua_State *state) { return 0; }
static int Lua_TAction_Create_Radar_Event(lua_State *state) { return 0; }
static int Lua_TAction_Local_Set(lua_State *state) { return 0; }
static int Lua_TAction_Local_Clear(lua_State *state) { return 0; }
static int Lua_TAction_Meteor_Shower_At(lua_State *state) { return 0; }
static int Lua_TAction_Set_Ambient_Step(lua_State *state) { return 0; }
static int Lua_TAction_Set_Ambient_Rate(lua_State *state) { return 0; }
static int Lua_TAction_Set_Ambient_Light(lua_State *state) { return 0; }
static int Lua_TAction_Set_AI_Triggers_Begin(lua_State *state) { return 0; }
static int Lua_TAction_Set_AI_Triggers_End(lua_State *state) { return 0; }
static int Lua_TAction_Set_Ratio_Of_AI_Trigger_Teams(lua_State *state) { return 0; }
static int Lua_TAction_Set_Ratio_Of_Team_Aircraft(lua_State *state) { return 0; }
static int Lua_TAction_Set_Ratio_Of_Team_Infantry(lua_State *state) { return 0; }
static int Lua_TAction_Set_Ratio_Of_Team_Units(lua_State *state) { return 0; }
static int Lua_TAction_Ion_Cannon_Strike(lua_State *state) { return 0; }
static int Lua_TAction_Nuke_Strike(lua_State *state) { return 0; }
static int Lua_TAction_Chemical_Missile_Strike(lua_State *state) { return 0; }
static int Lua_TAction_Toggle_Train_Cargo(lua_State *state) { return 0; }

static int Lua_TEvent_EnteredBy(lua_State *state) { return 0; }
static int Lua_TEvent_SpiedBy(lua_State *state) { return 0; }
static int Lua_TEvent_ThievedBy(lua_State *state) { return 0; }
static int Lua_TEvent_DiscoveredByPlayer(lua_State *state) { return 0; }
static int Lua_TEvent_HouseDiscovered(lua_State *state) { return 0; }
static int Lua_TEvent_AttackedByAnybody(lua_State *state) { return 0; }
static int Lua_TEvent_DestroyedByAnybody(lua_State *state) { return 0; }
static int Lua_TEvent_AnyEvent(lua_State *state) { return 0; }
static int Lua_TEvent_DestroyedUnitsAll(lua_State *state) { return 0; }
static int Lua_TEvent_DestroyedBuildingsAll(lua_State *state) { return 0; }
static int Lua_TEvent_DestroyedAll(lua_State *state) { return 0; }
static int Lua_TEvent_CreditsExceed(lua_State *state) { return 0; }
static int Lua_TEvent_ElapsedTime(lua_State *state) { return 0; }
static int Lua_TEvent_MissionTimerExpired(lua_State *state) { return 0; }
static int Lua_TEvent_DestroyedBuildings(lua_State *state) { return 0; }
static int Lua_TEvent_DestroyedUnits(lua_State *state) { return 0; }
static int Lua_TEvent_NoFactoriesLeft(lua_State *state) { return 0; }
static int Lua_TEvent_CiviliansEvacuated(lua_State *state) { return 0; }
static int Lua_TEvent_BuildBuildingType(lua_State *state) { return 0; }
static int Lua_TEvent_BuildUnitType(lua_State *state) { return 0; }
static int Lua_TEvent_BuildInfantryType(lua_State *state) { return 0; }
static int Lua_TEvent_BuildAircraftType(lua_State *state) { return 0; }
static int Lua_TEvent_LeavesMap(lua_State *state) { return 0; }
static int Lua_TEvent_ZoneEntryBy(lua_State *state) { return 0; }
static int Lua_TEvent_CrossesHorizontalLine(lua_State *state) { return 0; }
static int Lua_TEvent_CrossesVerticalLine(lua_State *state) { return 0; }
static int Lua_TEvent_GlobalIsSet(lua_State *state) { return 0; }
static int Lua_TEvent_GlobalIsClear(lua_State *state) { return 0; }
static int Lua_TEvent_DestroyedFakesAll(lua_State *state) { return 0; }
static int Lua_TEvent_LowPower(lua_State *state) { return 0; }
static int Lua_TEvent_BridgeDestroyed(lua_State *state) { return 0; }
static int Lua_TEvent_BuildingExists(lua_State *state) { return 0; }
static int Lua_TEvent_SelectedByPlayer(lua_State *state) { return 0; }
static int Lua_TEvent_ComesNearWaypoint(lua_State *state) { return 0; }
static int Lua_TEvent_EnemyInSpotlight(lua_State *state) { return 0; }
static int Lua_TEvent_LocalIsSet(lua_State *state) { return 0; }
static int Lua_TEvent_LocalIsClear(lua_State *state) { return 0; }
static int Lua_TEvent_FirstDamagedCombat(lua_State *state) { return 0; }
static int Lua_TEvent_HalfHealthCombat(lua_State *state) { return 0; }
static int Lua_TEvent_QuarterHealthCombat(lua_State *state) { return 0; }
static int Lua_TEvent_FirstDamagedAnySource(lua_State *state) { return 0; }
static int Lua_TEvent_HalfHealthAnySource(lua_State *state) { return 0; }
static int Lua_TEvent_QuarterHealthAnySource(lua_State *state) { return 0; }
static int Lua_TEvent_AttackedByHouse(lua_State *state) { return 0; }
static int Lua_TEvent_AmbientLightLessThan(lua_State *state) { return 0; }
static int Lua_TEvent_AmbientLightGreaterThan(lua_State *state) { return 0; }
static int Lua_TEvent_ElapsedScenarioTime(lua_State *state) { return 0; }
static int Lua_TEvent_DestroyedByAnything(lua_State *state) { return 0; }
static int Lua_TEvent_PickupCrate(lua_State *state) { return 0; }
static int Lua_TEvent_PickupCrateAny(lua_State *state) { return 0; }
static int Lua_TEvent_RandomDelay(lua_State *state) { return 0; }
static int Lua_TEvent_CreditsBelow(lua_State *state) { return 0; }
static int Lua_TEvent_Paralyzed(lua_State *state) { return 0; }
static int Lua_TEvent_EnemyInSpotlight(lua_State *state) { return 0; }
static int Lua_TEvent_Limped(lua_State *state) { return 0; }

// General utility functions.
bool Lua_Register_Utility_Functions(LuaScriptClass *lua_script)
{
    DEBUG_INFO("Lua: registering utility functions...\n");

    lua_script->Register_Function(Lua_Utility_Get_Time, "GetTime");

    return true;
}

// Debug logging and assitance functions.
bool Lua_Register_Debug_Functions(LuaScriptClass *lua_script)
{
    DEBUG_INFO("Lua: Registering debug functions...\n");

    lua_script->Register_Function(Lua_Debug_Message_Info, "DebugMessageInfo");
    DEBUG_INFO("Lua: function \"DebugMessageInfo\" registered.\n");

    return true;
}

// 
bool Lua_Register_TibSun_Functions(LuaScriptClass *lua_script)
{
    DEBUG_INFO("Lua: Registering engine interface functions...\n");

    /**
     *  Engine state
     */
    lua_script->Register_Function(nullptr, "GetFrame");

    /**
     *  Technos
     */
    lua_script->Register_Function(Lua_Unit_Create_One_Of, "CreateOneOf");

    /**
     *  Multiplayer houses
     */
    lua_script->Register_Function(Lua_Multi_Flag_To_Die, "FlagToDie", "Multiplayer");

    /**
     *  Player house
     */
    lua_script->Register_Function(Lua_Player_Flag_To_Die, "FlagToDie", "Player");

    /**
     *  Trigger actions.
     */
    lua_script->Register_Function(Lua_TAction_Deactivate_Firestorm_Defense, "DeactivateFirestormDefense");
    lua_script->Register_Function(Lua_TAction_Activate_Firestorm_Defense, "ActivateFirestormDefense");
    lua_script->Register_Function(Lua_TAction_Lightning_Strike_At, "LightningStrikeAt");
    lua_script->Register_Function(Lua_TAction_Remove_Particle_Anim_At, "RemoveParticleAnimAt");
    lua_script->Register_Function(Lua_TAction_Particle_Anim_At, "ParticleAnimAt");
    lua_script->Register_Function(Lua_TAction_Wakeup_Self, "WakeupSelf");
    lua_script->Register_Function(Lua_TAction_Vein_Growth, "VeinGrowth");
    lua_script->Register_Function(Lua_TAction_Tiberium_Growth, "TiberiumGrowth");
    lua_script->Register_Function(Lua_TAction_Ice_Growth, "IceGrowth");
    lua_script->Register_Function(Lua_TAction_Wakeup_Sleepers, "WakeupSleepers");
    lua_script->Register_Function(Lua_TAction_Wakeup_Harmless, "WakeupHarmless");
    lua_script->Register_Function(Lua_TAction_Wakeup_Group, "WakeupGroup");
    lua_script->Register_Function(Lua_TAction_Annouce_Win, "AnnouceWin");
    lua_script->Register_Function(Lua_TAction_Annouce_Lose, "AnnouceLose");
    lua_script->Register_Function(Lua_TAction_Force_End, "ForceEnd");
    lua_script->Register_Function(Lua_TAction_Apply_100_Damage, "Apply100Damage");
    lua_script->Register_Function(Lua_TAction_Small_Light_Flash_At, "SmallLightFlashAt");
    lua_script->Register_Function(Lua_TAction_Medium_Light_Flash_At, "MediumLightFlashAt");
    lua_script->Register_Function(Lua_TAction_Large_Light_Flash_At, "LargeLightFlashAt");
    lua_script->Register_Function(Lua_TAction_Sell_Building, "SellBuilding");
    lua_script->Register_Function(Lua_TAction_Go_Bezerk, "GoBezerk");
    lua_script->Register_Function(Lua_TAction_Turn_Off_Building, "TurnOffBuilding");
    lua_script->Register_Function(Lua_TAction_Turn_On_Building, "TurnOnBuilding");
    lua_script->Register_Function(Lua_TAction_Change_House, "ChangeHouse");
    lua_script->Register_Function(Lua_TAction_Set_Team_ID, "SetTeamID");
    lua_script->Register_Function(Lua_TAction_All_Change_House, "AllChangeHouse");
    lua_script->Register_Function(Lua_TAction_Text_Trigger, "TextTrigger");
    lua_script->Register_Function(Lua_TAction_Make_Ally, "MakeAlly");
    lua_script->Register_Function(Lua_TAction_Make_Enemy, "MakeEnemy");
    lua_script->Register_Function(Lua_TAction_Preferred_Target, "PreferredTarget");
    lua_script->Register_Function(Lua_TAction_Base_Building, "BaseBuilding");
    lua_script->Register_Function(Lua_TAction_Grow_Shroud, "GrowShroud");
    lua_script->Register_Function(Lua_TAction_Set_Global, "SetGlobal");
    lua_script->Register_Function(Lua_TAction_Clear_Global, "ClearGlobal");
    lua_script->Register_Function(Lua_TAction_Reveal_Area, "RevealArea");
    lua_script->Register_Function(Lua_TAction_Reduce_Tiberium_At, "ReduceTiberiumAt");
    lua_script->Register_Function(Lua_TAction_Reveal_Zone, "RevealZone");
    lua_script->Register_Function(Lua_TAction_Reveal_Map, "RevealMap");
    lua_script->Register_Function(Lua_TAction_Start_Timer, "StartTimer");
    lua_script->Register_Function(Lua_TAction_Stop_Timer, "StopTimer");
    lua_script->Register_Function(Lua_TAction_Timer_Extend, "TimerExtend");
    lua_script->Register_Function(Lua_TAction_Timer_Shorten, "TimerShorten");
    lua_script->Register_Function(Lua_TAction_Timer_Set, "TimerSet");
    lua_script->Register_Function(Lua_TAction_Play_Movie, "PlayMovie");
    lua_script->Register_Function(Lua_TAction_Play_Ingame_Movie, "PlayIngameMovie");
    lua_script->Register_Function(Lua_TAction_Play_Sound, "PlaySound");
    lua_script->Register_Function(Lua_TAction_Play_Sound_At_Random_Waypoint, "PlaySoundAtRandomWaypoint");
    lua_script->Register_Function(Lua_TAction_Play_Sound_At, "PlaySoundAt");
    lua_script->Register_Function(Lua_TAction_Play_Music, "PlayMusic");
    lua_script->Register_Function(Lua_TAction_Play_Speech, "PlaySpeech");
    lua_script->Register_Function(Lua_TAction_One_Time_Special, "OneTimeSpecial");
    lua_script->Register_Function(Lua_TAction_Full_Special, "FullSpecial");
    lua_script->Register_Function(Lua_TAction_Place_Drop_Zone_Flare, "PlaceDropZoneFlare");
    lua_script->Register_Function(Lua_TAction_Win, "Win");
    lua_script->Register_Function(Lua_TAction_Lose, "Lose");
    lua_script->Register_Function(Lua_TAction_Begin_Production, "BeginProduction");
    lua_script->Register_Function(Lua_TAction_Fire_Sale, "FireSale");
    lua_script->Register_Function(Lua_TAction_Begin_Autocreate, "BeginAutocreate");
    lua_script->Register_Function(Lua_TAction_Create_Team, "CreateTeam");
    lua_script->Register_Function(Lua_TAction_Destroy_Team, "DestroyTeam");
    lua_script->Register_Function(Lua_TAction_Flash_Team, "FlashTeam");
    lua_script->Register_Function(Lua_TAction_Disable_Speech, "DisableSpeech");
    lua_script->Register_Function(Lua_TAction_Enable_Speech, "EnableSpeech");
    lua_script->Register_Function(Lua_TAction_Talk_Bubble, "TalkBubble");
    lua_script->Register_Function(Lua_TAction_Reinforcements, "Reinforcements");
    lua_script->Register_Function(Lua_TAction_Reinforcement_At, "ReinforcementAt");
    lua_script->Register_Function(Lua_TAction_All_To_Hunt, "AllToHunt");
    lua_script->Register_Function(Lua_TAction_Destroy_Object, "DestroyObject");
    lua_script->Register_Function(Lua_TAction_Change_Zoom_Level, "ChangeZoomLevel");
    lua_script->Register_Function(Lua_TAction_Resize_Player_View, "ResizePlayerView");
    lua_script->Register_Function(Lua_TAction_Play_Anim_At, "PlayAnimAt");
    lua_script->Register_Function(Lua_TAction_Do_Explosion_At, "DoExplosionAt");
    lua_script->Register_Function(Lua_TAction_Meteor_Impact_At, "MeteorImpactAt");
    lua_script->Register_Function(Lua_TAction_Ion_Storm_Start, "IonStormStart");
    lua_script->Register_Function(Lua_TAction_Ion_Storm_End, "IonStormEnd");
    lua_script->Register_Function(Lua_TAction_Lock_Input, "LockInput");
    lua_script->Register_Function(Lua_TAction_Unlock_Input, "UnlockInput");
    lua_script->Register_Function(Lua_TAction_Center_Camera_At, "CenterCameraAt");
    lua_script->Register_Function(Lua_TAction_Zoom_In, "ZoomIn");
    lua_script->Register_Function(Lua_TAction_Zoom_Out, "ZoomOut");
    lua_script->Register_Function(Lua_TAction_Reshroud_Map, "ReshroudMap");
    lua_script->Register_Function(Lua_TAction_Change_Spotlight_Behavior, "ChangeSpotlightBehavior");
    lua_script->Register_Function(Lua_TAction_Destroy_Trigger, "DestroyTrigger");
    lua_script->Register_Function(Lua_TAction_Destroy_Tag, "DestroyTag");
    lua_script->Register_Function(Lua_TAction_Force_Trigger, "ForceTrigger");
    lua_script->Register_Function(Lua_TAction_Enable_Trigger, "EnableTrigger");
    lua_script->Register_Function(Lua_TAction_Disable_Trigger, "DisableTrigger");
    lua_script->Register_Function(Lua_TAction_Create_Radar_Event, "CreateRadarEvent");
    lua_script->Register_Function(Lua_TAction_Local_Set, "LocalSet");
    lua_script->Register_Function(Lua_TAction_Local_Clear, "LocalClear");
    lua_script->Register_Function(Lua_TAction_Meteor_Shower_At, "MeteorShowerAt");
    lua_script->Register_Function(Lua_TAction_Set_Ambient_Step, "SetAmbientStep");
    lua_script->Register_Function(Lua_TAction_Set_Ambient_Rate, "SetAmbientRate");
    lua_script->Register_Function(Lua_TAction_Set_Ambient_Light, "SetAmbientLight");
    lua_script->Register_Function(Lua_TAction_Set_AI_Triggers_Begin, "SetAITriggersBegin");
    lua_script->Register_Function(Lua_TAction_Set_AI_Triggers_End, "SetAITriggersEnd");
    lua_script->Register_Function(Lua_TAction_Set_Ratio_Of_AI_Trigger_Teams, "SetRatioOfAITriggerTeams");
    lua_script->Register_Function(Lua_TAction_Set_Ratio_Of_Team_Aircraft, "SetRatioOfTeamAircraft");
    lua_script->Register_Function(Lua_TAction_Set_Ratio_Of_Team_Infantry, "SetRatioOfTeamInfantry");
    lua_script->Register_Function(Lua_TAction_Set_Ratio_Of_Team_Units, "SetRatioOfTeamUnits");
    lua_script->Register_Function(Lua_TAction_Ion_Cannon_Strike, "IonCannonStrike");
    lua_script->Register_Function(Lua_TAction_Nuke_Strike, "NukeStrike");
    lua_script->Register_Function(Lua_TAction_Chemical_Missile_Strike, "ChemicalMissileStrike");
    lua_script->Register_Function(Lua_TAction_Toggle_Train_Cargo, "ToggleTrainCargo");

    /**
     *  Trigger event callbacks.
     */
    lua_script->Register_Function(Lua_TEvent_EnteredBy, "");
    lua_script->Register_Function(Lua_TEvent_SpiedBy, "");
    lua_script->Register_Function(Lua_TEvent_ThievedBy, "");
    lua_script->Register_Function(Lua_TEvent_DiscoveredByPlayer, "");
    lua_script->Register_Function(Lua_TEvent_HouseDiscovered, "");
    lua_script->Register_Function(Lua_TEvent_AttackedByAnybody, "");
    lua_script->Register_Function(Lua_TEvent_DestroyedByAnybody, "");
    lua_script->Register_Function(Lua_TEvent_AnyEvent, "");
    lua_script->Register_Function(Lua_TEvent_DestroyedUnitsAll, "");
    lua_script->Register_Function(Lua_TEvent_DestroyedBuildingsAll, "");
    lua_script->Register_Function(Lua_TEvent_DestroyedAll, "");
    lua_script->Register_Function(Lua_TEvent_CreditsExceed, "");
    lua_script->Register_Function(Lua_TEvent_ElapsedTime, "");
    lua_script->Register_Function(Lua_TEvent_MissionTimerExpired, "");
    lua_script->Register_Function(Lua_TEvent_DestroyedBuildings, "");
    lua_script->Register_Function(Lua_TEvent_DestroyedUnits, "");
    lua_script->Register_Function(Lua_TEvent_NoFactoriesLeft, "");
    lua_script->Register_Function(Lua_TEvent_CiviliansEvacuated, "");
    lua_script->Register_Function(Lua_TEvent_BuildBuildingType, "");
    lua_script->Register_Function(Lua_TEvent_BuildUnitType, "");
    lua_script->Register_Function(Lua_TEvent_BuildInfantryType, "");
    lua_script->Register_Function(Lua_TEvent_BuildAircraftType, "");
    lua_script->Register_Function(Lua_TEvent_LeavesMap, "");
    lua_script->Register_Function(Lua_TEvent_ZoneEntryBy, "");
    lua_script->Register_Function(Lua_TEvent_CrossesHorizontalLine, "");
    lua_script->Register_Function(Lua_TEvent_CrossesVerticalLine, "");
    lua_script->Register_Function(Lua_TEvent_GlobalIsSet, "");
    lua_script->Register_Function(Lua_TEvent_GlobalIsClear, "");
    lua_script->Register_Function(Lua_TEvent_DestroyedFakesAll, "");
    lua_script->Register_Function(Lua_TEvent_LowPower, "");
    lua_script->Register_Function(Lua_TEvent_BridgeDestroyed, "");
    lua_script->Register_Function(Lua_TEvent_BuildingExists, "");
    lua_script->Register_Function(Lua_TEvent_SelectedByPlayer, "");
    lua_script->Register_Function(Lua_TEvent_ComesNearWaypoint, "");
    lua_script->Register_Function(Lua_TEvent_EnemyInSpotlight, "");
    lua_script->Register_Function(Lua_TEvent_LocalIsSet, "");
    lua_script->Register_Function(Lua_TEvent_LocalIsClear, "");
    lua_script->Register_Function(Lua_TEvent_FirstDamagedCombat, "");
    lua_script->Register_Function(Lua_TEvent_HalfHealthCombat, "");
    lua_script->Register_Function(Lua_TEvent_QuarterHealthCombat, "");
    lua_script->Register_Function(Lua_TEvent_FirstDamagedAnySource, "");
    lua_script->Register_Function(Lua_TEvent_HalfHealthAnySource, "");
    lua_script->Register_Function(Lua_TEvent_QuarterHealthAnySource, "");
    lua_script->Register_Function(Lua_TEvent_AttackedByHouse, "");
    lua_script->Register_Function(Lua_TEvent_AmbientLightLessThan, "");
    lua_script->Register_Function(Lua_TEvent_AmbientLightGreaterThan, "");
    lua_script->Register_Function(Lua_TEvent_ElapsedScenarioTime, "");
    lua_script->Register_Function(Lua_TEvent_DestroyedByAnything, "");
    lua_script->Register_Function(Lua_TEvent_PickupCrate, "");
    lua_script->Register_Function(Lua_TEvent_PickupCrateAny, "");
    lua_script->Register_Function(Lua_TEvent_RandomDelay, "");
    lua_script->Register_Function(Lua_TEvent_CreditsBelow, "");
    lua_script->Register_Function(Lua_TEvent_Paralyzed, "");
    lua_script->Register_Function(Lua_TEvent_EnemyInSpotlight, "");
    lua_script->Register_Function(Lua_TEvent_Limped, "");

    return true;
}

#endif



/**
 *  Load any Vinifera settings that provide overrides.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Load_INI()
{
    CCFileClass file;
    INIClass ini;

    if (CCFileClass("VINIFERA.INI").Is_Available()) {
        file.Set_Name("VINIFERA.INI");

    } else if (CCFileClass("INI\\VINIFERA.INI").Is_Available()) {
        file.Set_Name("INI\\VINIFERA.INI");
    }

    if (!file.Is_Available()) {
        return false;
    }

    ini.Load(file);

    ini.Get_String("General", "ProjectName", Vinifera_ProjectName, sizeof(Vinifera_ProjectName));
    ini.Get_String("General", "IconFile", Vinifera_IconName, sizeof(Vinifera_IconName));
    ini.Get_String("General", "CursorFile", Vinifera_CursorName, sizeof(Vinifera_CursorName));

#if defined(TS_CLIENT)
    {
    /**
     *  TS Client uses a seperate "version" file, so its best we fetch the current
     *  version from there rather than have the user update the INI file each time
     *  they update the project.
     */
    RawFileClass ver_file("version");
    if (!ver_file.Is_Available()) {
        DEBUG_ERROR("Failed to find TS Client version file!\n");
        return false;
    }

    INIClass ver_ini;
    ver_ini.Load(ver_file);

    ver_ini.Get_String("DTA", "Version", Vinifera_ProjectVersion, sizeof(Vinifera_ProjectVersion));
    }
#else
    ini.Get_String("General", "ProjectVersion", "No version number set", Vinifera_ProjectVersion, sizeof(Vinifera_ProjectVersion));
#endif

    Vinifera_ProjectName[sizeof(Vinifera_ProjectName)-1] = '\0';
    Vinifera_ProjectVersion[sizeof(Vinifera_ProjectVersion)-1] = '\0';
    Vinifera_IconName[sizeof(Vinifera_IconName)-1] = '\0';
    Vinifera_CursorName[sizeof(Vinifera_CursorName)-1] = '\0';

    char buffer[1024];
    if (ini.Get_String("General", "SearchPaths", buffer, sizeof(buffer)) > 0) {
        char *path = std::strtok(buffer, ",");
        while (path) {
            if (!ViniferaSearchPaths.Is_Present(path)) {
                ViniferaSearchPaths.Add(path);
            }
            path = std::strtok(nullptr, ",");
        }
#if defined(TS_CLIENT)
    } else {
        DEBUG_ERROR("Failed to find SearchPaths in VINIFERA.INI!\n");
        MessageBox(MainWindow, "Failed to find SearchPaths in VINIFERA.INI, please reinstall Vinifera.", "Vinifera", MB_ICONEXCLAMATION|MB_OK);
        return false;
#endif
    }

    return true;
}


/**
 *  Loads the exception database.
 * 
 *  @author: CCHyper
 */
static bool Vinifera_Load_Exception_Database(const char *filename)
{
    RawFileClass file(filename);
    if (!file.Is_Available()) {
        return false;
    }

    FileStraw fstraw(file);

    bool eof = false;
    bool found_first_line = false;
    char line_buffer[11 + 2 + 2 + 1024]; // address, bool, bool, desc
    ExceptionInfoDatabaseStruct einfo;

    while (true) {

        char *tok = nullptr;
           
        /**
         *  Read the line into the buffer.
         */
        int count = Read_Line(fstraw, line_buffer, sizeof(line_buffer), eof);

        /**
         *  Handle end of file and invalid line cases.
         */
        if (eof) {
            break;
        }
        if (!count) {
            continue;
        }
        if (count >= sizeof(line_buffer)) {
            break;
        }

        int index = 0; // cursor position.

        /**
         *  Step over any indenting.
         */
        while (std::isspace(line_buffer[index])) {
            ++index;
        }

        /**
         *  Handle commented out lines.
         */
        if (line_buffer[index] == ';') {
            continue;
        }

        /**
         *  Process the database line.
         */

        tok = std::strtok(&line_buffer[index], ",");
        ASSERT(tok != nullptr);
        if (tok[0] != '0' || tok[1] != 'x') {
            DEBUG_WARNING("Invalid address format in exception database!\n");
            return false;
        }
        einfo.Address = std::strtoul(tok+2, nullptr, 16);

        tok = std::strtok(nullptr, ",");
        ASSERT(tok != nullptr);
        einfo.CanContinue = std::strtoul(tok, nullptr, 10) ? true : false;

        tok = std::strtok(nullptr, ",");
        ASSERT(tok != nullptr);
        einfo.Ignore = std::strtoul(tok, nullptr, 10) ? true : false;
        
        tok = std::strtok(nullptr, ",");
        ASSERT(tok != nullptr);
        std::strncpy(einfo.Description, tok, std::strlen(tok));

        ExceptionInfoDatabase.Add(einfo);
    }

    if (!ExceptionInfoDatabase.Count()) {
        DEBUG_WARNING("Invalid format in exception database!\n");
        return false;
    }

#ifndef NDEBUG
    DEV_DEBUG_INFO("Exception database dump...\n");
    for (int i = 0; i < ExceptionInfoDatabase.Count(); ++i) {
        ExceptionInfoDatabaseStruct &e = ExceptionInfoDatabase[i];
        DEV_DEBUG_INFO("  0x%08X %s %s \"%.32s...\"\n",
                       e.Address, e.CanContinue ? "true " : "false",
                       e.Ignore ? "true " : "false",
                       e.Description);
    }
#endif

    return true;
}


/**
 *  Parses the command line parameters.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Parse_Command_Line(int argc, char *argv[])
{
    if (argc > 1) {
        DEBUG_INFO("Parsing command line arguments...\n");
    }

    bool menu_skip = false;

    /**
     *  Iterate over all command line params.
     */
    for (int index = 1; index < argc; index++) {

        char arg_string[512];

        char *src = argv[index];
        char *dest = arg_string; 
        for (int i= 0; i < std::strlen(argv[index]); ++i) {
            if (*src == '\"') {
                src++;
            } else {
                *dest++ = *src++;
            }
        }
        *dest++ = '\0';

        char *string = arg_string; // Pointer to current argument.
        strupr(string);

        /**
         *  Add all new command line params here.
         */

        /**
         *  Mod developer mode.
         */
        if (stricmp(string, "-DEVELOPER") == 0) {
            DEBUG_INFO("  - Developer mode enabled.\n");
            Vinifera_DeveloperMode = true;
            continue;
        }

        /**
         *  Skip the startup videos.
         */
        if (stricmp(string, "-NO_STARTUP_VIDEO") == 0) {
            DEBUG_INFO("  - Skipping startup videos.\n");
            Vinifera_SkipStartupMovies = true;
            continue;
        }

        /**
         *  Skip directly to Tiberian Sun menu.
         */
        if (stricmp(string, "-SKIP_TO_TS_MENU") == 0) {
            DEBUG_INFO("  - Skipping to Tiberian Sun menu.\n");
            Vinifera_SkipToTSMenu = true;
            menu_skip = true;
            continue;
        }

        /**
         *  Skip directly to Firestorm menu.
         */
        if (stricmp(string, "-SKIP_TO_FS_MENU") == 0) {
            DEBUG_INFO("  - Skipping to Firestorm menu.\n");
            Vinifera_SkipToFSMenu = true;
            menu_skip = true;
            continue;
        }

        /**
         *  Skip directly to a specific game mode dialog.
         */
        if (stricmp(string, "-SKIP_TO_LAN") == 0) {
            DEBUG_INFO("  - Skipping to LAN dialog.\n");
            Vinifera_SkipToLAN = true;
            menu_skip = true;
            continue;
        }

        if (stricmp(string, "-SKIP_TO_CAMPAIGN") == 0) {
            DEBUG_INFO("  - Skipping to campaign dialog.\n");
            Vinifera_SkipToCampaign = true;
            menu_skip = true;
            continue;
        }

        if (stricmp(string, "-SKIP_TO_SKIRMISH") == 0) {
            DEBUG_INFO("  - Skipping to skirmish dialog.\n");
            Vinifera_SkipToSkirmish = true;
            menu_skip = true;
            continue;
        }

        if (stricmp(string, "-SKIP_TO_INTERNET") == 0) {
            DEBUG_INFO("  - Skipping to internet dialog.\n");
            Vinifera_SkipToInternet = true;
            menu_skip = true;
            continue;
        }

        /**
         *  Exit the game after the dialog we skipped to has been canceled?
         */
        if (stricmp(string, "-EXIT_AFTER_SKIP") == 0) {
            DEBUG_INFO("  - Skipping to Firestorm menu.\n");
            Vinifera_ExitAfterSkip = true;
            menu_skip = true;
            continue;
        }

        /**
         *  #issue-513
         * 
         *  Re-implements the file search path override logic of "-CD" from Red Alert.
         */
        if (std::strstr(string, "-CD")) {
            DEBUG_INFO("  - \"-CD\" argument detected.\n");

            if (std::isspace(string[3]) || !std::isgraph(string[3])) {
                DEBUG_ERROR("Invalid search path defined!");
                MessageBox(MainWindow, "Invalid search path defined with -CD command line argument!", "Vinifera", MB_ICONEXCLAMATION|MB_OK);
                return false;
            }

            CCFileClass::Set_Search_Drives(&string[3]);
            if (CCFileClass::Is_There_Search_Drives()) {
                DEBUG_INFO("  - Search path set to \"%s\".\n", &string[3]);

                /**
                 *  Flag the cd search system to search for files locally.
                 */
                CD::IsFilesLocal = true;
            }
            continue;
        }

        /**
         *  Should assertions only be printed to the debug log?
         */
        if (stricmp(string, "-SILENT_ASSERTS") == 0) {
            DEBUG_INFO("  - Assertions are silent.\n");
            SilentAsserts = true;
            continue;
        }

        /**
         *  Ignore all assertions?
         */
        if (stricmp(string, "-IGNORE_ASSERTS") == 0) {
            DEBUG_INFO("  - Ignore all assertions.\n");
            IgnoreAllAsserts = true;
            continue;
        }

        /**
         *  Are file io errors fatal?
         */
        if (stricmp(string, "-FILE_ERROR_FATAL") == 0) {
            DEBUG_INFO("  - File read/write errors are fatal.\n");
            Vinifera_FatalFileErrors = true;
            continue;
        }

        /**
         *  Trigger an assertion on file io errors?
         */
        if (stricmp(string, "-FILE_ERROR_ASSERT") == 0) {
            DEBUG_INFO("  - Assertions on file read/write error.\n");
            Vinifera_AssertFileErrors = true;
            continue;
        }

        /**
         *  Specify the random number seed (for debugging).
         */
        if (std::strstr(string, "-SEED")) {
            CustomSeed = (unsigned short)(std::atoi(string + std::strlen("SEED")));
            continue;
        }

    }

    if (argc > 1) {
        DEBUG_INFO("Finished parsing command line arguments.\n");
    }

    /**
     *  Firestorm has priority over Tiberian Sun.
     */
    if (Vinifera_SkipToTSMenu && Vinifera_SkipToFSMenu) {
        Vinifera_SkipToTSMenu = false;
    }

    /**
     *  If any of the menu skip commands have been set then
     *  we also need to skip the startup movies.
     */
    if (menu_skip) {
        Vinifera_SkipStartupMovies = true;
    }

    return true;
}


/**
 *  This function will get called on application startup, allowing you to
 *  perform any action that would effect the game initialisation process.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Startup()
{
    DWORD rc;

    ViniferaSearchPaths.Clear();

    /**
     *  If -CD has been defined, set the root directory as highest priority.
     */
    if (CD::IsFilesLocal) {
        ViniferaSearchPaths.Add(".");
    }
    
#ifndef NDEBUG
    /**
     *  Debug paths for CD contents (folders must contain .DSK files of the same name).
     */
    ViniferaSearchPaths.Add("TS1");
    ViniferaSearchPaths.Add("TS2");
    ViniferaSearchPaths.Add("TS3");
#endif

    /**
     *  #issue-514:
     * 
     *  Adds various search paths for loading files locally for the TS-Client builds only.
     * 
     *  #NOTE: REMOVED: Additional paths must now be set via SearchPaths in VINIFERA.INI!
     * 
     *  @author: CCHyper
     */
#if 0 // #if defined(TS_CLIENT)

    // Only required for the TS Client builds as most projects will
    // put VINIFERA.INI in this directory.
    ViniferaSearchPaths.Add("INI");

#if defined(LUA_ENABLED)
    ViniferaSearchPaths.Add("LUA");
#endif

    // Required for startup mix files to be found.
    ViniferaSearchPaths.Add("MIX");
#endif

#if !defined(TS_CLIENT)
    // Required for startup movies to be found.
    ViniferaSearchPaths.Add("MOVIES");
#endif

    // REMOVED: Paths are now set via SearchPaths in VINIFERA.INI
//#if defined(TS_CLIENT)
//    ViniferaSearchPaths.Add("MUSIC");
//    ViniferaSearchPaths.Add("SOUNDS");
//    ViniferaSearchPaths.Add("MAPS");
//    ViniferaSearchPaths.Add("MAPS\\MULTIPLAYER");
//    ViniferaSearchPaths.Add("MAPS\\MISSION");
//#endif

    /**
     *  Load Vinifera settings and overrides.
     */
    if (Vinifera_Load_INI()) {
        DEBUG_INFO("\n");
        DEBUG_INFO("Project information:\n");
        DEBUG_INFO("  Title: %s\n", Vinifera_ProjectName);
        DEBUG_INFO("  Version: %s\n", Vinifera_ProjectVersion);
        DEBUG_INFO("\n");
    } else {
        DEBUG_WARNING("Failed to load VINIFERA.INI!\n");
#if defined(TS_CLIENT)
        MessageBoxA(nullptr, "Failed to load VINIFERA.INI!", "Vinifera", MB_ICONERROR|MB_OK);
        return false;
#endif
    }

    /**
     *  Current path (perhaps set set with -CD) should go next.
     */
    if (CCFileClass::RawPath[0] != '\0' && std::strlen(CCFileClass::RawPath) > 1) {
        ViniferaSearchPaths.Add(CCFileClass::RawPath);
    }

    if (ViniferaSearchPaths.Count() > 0) {
        char *new_path = new char[_MAX_PATH * ViniferaSearchPaths.Count()+1];
        new_path[0] = '\0';

        /**
         *  Build the search path string.
         */
        for (int i = 0; i < ViniferaSearchPaths.Count(); ++i) {
            if (i != 0) std::strcat(new_path, ";");
            std::strcat(new_path, ViniferaSearchPaths[i].Peek_Buffer());
        }

        /**
         *  Clear the current path ready to be set.
         */
        CCFileClass::Clear_Search_Drives();
        CCFileClass::Reset_Raw_Path();

        /**
         *  Set the new search drive path.
         */
        CCFileClass::Set_Search_Drives(new_path);

        delete[] new_path;

        DEBUG_INFO("SearchPath: %s\n", CCFileClass::RawPath);
    }

    /**
     *  We are finished with the vector, clear it.
     */
    ViniferaSearchPaths.Clear();

    /**
     *  Check for the existence of the exception database.
     */
    RawFileClass dbfile(VINIFERA_TARGET_EDB);
    if (!dbfile.Is_Available()) {
        DEBUG_ERROR("Failed to find the exception database!\n");
        MessageBox(MainWindow, "Failed to find the exception database, please reinstall Vinifera.", "Vinifera", MB_OK);
        return false;
    }
    if (!dbfile.Size()) {
        DEBUG_ERROR("Invalid or corrupt exception database!\n");
        MessageBox(MainWindow, "Invalid or corrupt exception database, please reinstall Vinifera.", "Vinifera", MB_OK);
        return false;
    }
    if (!Vinifera_Load_Exception_Database(dbfile.File_Name())) {
        DEBUG_ERROR("Failed to load the exception database!\n");
        MessageBox(MainWindow, "Failed to load the exception database, please reinstall Vinifera.", "Vinifera", MB_OK);
        return false;
    }

#if !defined(TS_CLIENT)
    /**
     *  Initialise the CnCNet4 system.
     */
    if (!CnCNet4::Init()) {
        CnCNet4::IsEnabled = false;
        DEBUG_WARNING("Failed to initialise CnCNet4, continuing without CnCNet4 support!\n");
    }

    /**
     *  Disable CnCNet4 if CnCNet5 is active, they can not co-exist.
     */
    if (CnCNet4::IsEnabled && CnCNet5::IsActive) {
        CnCNet4::Shutdown();
        CnCNet4::IsEnabled = false;
    }
#else
    /**
     *  Client builds can only use CnCNet5.
     */
    CnCNet4::IsEnabled = false;
    //CnCNet5::IsActive = true; // Enable when new Client system is implemented.
#endif

    return true;
}


/**
 *  This function will get called on application shutdown, allowing you to
 *  perform any memory cleanup or shutdown of new systems.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Shutdown()
{
    /**
     *  Cleanup mixfiles.
     */
    delete GenericMix;
    GenericMix = nullptr;

    delete IsoGenericMix;
    IsoGenericMix = nullptr;

    ViniferaMapsMixes.Clear();
    ViniferaMoviesMixes.Clear();

    /**
     *  Cleanup global heaps/vectors.
     */
    EBoltClass::Clear_All();
    TheaterTypes.Clear();

    /**
     *  Cleanup global extension instances.
     */
    delete OptionsExtension;
    OptionsExtension = nullptr;

    delete UIControls;
    UIControls = nullptr;

    /**
     *  Cleanup additional extension instances.
     */
    ThemeControlExtensions.Clear();

#if defined(LUA_ENABLED)

#endif

    DEV_DEBUG_INFO("Shutdown - New Count: %d, Delete Count: %d\n", Vinifera_New_Count, Vinifera_Delete_Count);

    return true;
}


/**
 *  This function will get called "before" the games "Init_Game" function,
 *  allowing you to perform any action that would effect the game initialisation process.
 * 
 *  @author: CCHyper
 */
int Vinifera_Pre_Init_Game(int argc, char *argv[])
{
    /**
     *  Read the UI controls and overrides.
     */
    UIControls = new UIControlsClass;

    CCFileClass ui_file("UI.INI");
    CCINIClass ui_ini;

    if (ui_file.Is_Available()) {

        ui_ini.Load(ui_file, false);

        if (!UIControls->Read_INI(ui_ini)) {
            DEV_DEBUG_ERROR("Failed to read UI.INI!\n");
            //return EXIT_FAILURE;
        }

    } else {
        DEV_DEBUG_WARNING("UI.INI not found!\n");
    }

#if defined(TS_CLIENT)
    /**
     *  The TS Client allows player to jump right into a game, so no need to
     *  show the startup movies for these builds.
     */
    Vinifera_SkipStartupMovies = true;
#endif

    return EXIT_SUCCESS;
}


/**
 *  This function will get called "after" the games "Init_Game" function,
 *  allowing you to perform any action that would effect the game initialisation process.
 * 
 *  @author: CCHyper
 */
int Vinifera_Post_Init_Game(int argc, char *argv[])
{
    TheaterTypeClass::One_Time();

    CCFileClass theater_file("THEATERS.INI");
    CCINIClass theater_ini;

    if (theater_file.Is_Available()) {

        theater_ini.Load(theater_file, false);

        if (!TheaterTypeClass::Read_Theaters_INI(theater_ini)) {
            DEV_DEBUG_ERROR("Failed to read THEATERS.INI!\n");
            //return EXIT_FAILURE;
        }

    } else {
        DEV_DEBUG_WARNING("THEATERS.INI not found!\n");
    }

#if defined(LUA_ENABLED)
    /**
     *  Load the common lua script.
     */
    if (CCFileClass("utility.lua").Is_Available()) {

        /**
         *  We found the lua script, now initalise a instance of the system.
         */
        LuaScriptClass *LuaCommonScript = new LuaScriptClass();
        //ASSERT(LuaCommonScript != nullptr);

        DEBUG_INFO("About to call LuaCommonScript->Initalize(utility.lua)...\n");

        if (!LuaCommonScript->Initalize("utility.lua", Lua_Register_Utility_Functions)) {
            return EXIT_FAILURE;
        }

    }

    /**
     *  Load the debug lua script.
     */
    if (CCFileClass("debug.lua").Is_Available()) {

        /**
         *  We found the lua script, now initalise a instance of the system.
         */
        LuaScriptClass *LuaDebugScript = new LuaScriptClass();
        //ASSERT(LuaDebugScript != nullptr);

        DEBUG_INFO("About to call LuaDebugScript->Initalize(debug.lua)...\n");

        if (!LuaDebugScript->Initalize("debug.lua", Lua_Register_Debug_Functions)) {
            return EXIT_FAILURE;
        }

    }

    /**
     *  Load the common lua script.
     */
    if (CCFileClass("tibsun.lua").Is_Available()) {

        /**
         *  We found the lua script, now initalise a instance of the system.
         */
        LuaScriptClass *LuaCommonScript = new LuaScriptClass();
        //ASSERT(LuaCommonScript != nullptr);

        DEBUG_INFO("About to call LuaCommonScript->Initalize(tibsun.lua)...\n");

        if (!LuaCommonScript->Initalize("tibsun.lua", Lua_Register_TibSun_Functions)) {
            return EXIT_FAILURE;
        }

    }
#endif

    return EXIT_SUCCESS;
}


/**
 *  This function registers any com objects required by the DLL.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Register_Com_Objects()
{
    DEBUG_INFO("Registering new com objects...\n");

    //DEBUG_INFO("  TestLocomotionClass\n");
    REGISTER_CLASS(TestLocomotionClass);
    
    //DEBUG_INFO("  Extension classes\n");
    Extension::Register_Class_Factories();

    DEBUG_INFO("  ...OK!\n");

    return true;
}
