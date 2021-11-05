/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SAVELOADEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Handles the saving and loading of extended class data.
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
#include "saveloadext.h"
#include "wwcrc.h"
#include "vinifera_gitinfo.h"
#include "vinifera_util.h"
#include "debughandler.h"
#include "asserthandler.h"
#include "fatal.h"
#include <unknwn.h> // for IStream

#include "rulesext.h"
#include "sessionext.h"
#include "scenarioext.h"
#include "tacticalext.h"

#include "objecttypeext.h"
#include "technotypeext.h"
#include "buildingtypeext.h"
#include "unittypeext.h"
#include "infantrytypeext.h"
#include "aircrafttypeext.h"
#include "warheadtypeext.h"
#include "weapontypeext.h"
#include "bullettypeext.h"
#include "supertypeext.h"
#include "voxelanimtypeext.h"
#include "animtypeext.h"
#include "particletypeext.h"
#include "particlesystypeext.h"
#include "isotiletypeext.h"
#include "overlaytypeext.h"
#include "smudgetypeext.h"
#include "terraintypeext.h"
#include "housetypeext.h"
#include "sideext.h"
#include "campaignext.h"
#include "tiberiumext.h"
//#include "taskforceext.h"
//#include "aitrigtypeext.h"
//#include "scripttypeext.h"
//#include "tagtypeext.h"
//#include "triggertypeext.h"

#include "technoext.h"
#include "aircraftext.h"
#include "buildingext.h"
#include "infantryext.h"
#include "unitext.h"
#include "terrainext.h"
#include "superext.h"

#include "waveext.h"


/**
 *  Constant of the current build version number. This number should be
 *  a sum of all the extended class sizes plus the build date.
 */
unsigned ViniferaSaveGameVersion =

            10000

            /**
            *  Global classes.
            */
            + sizeof(RulesClassExtension)
            + sizeof(SessionClassExtension)
            + sizeof(ScenarioClassExtension)
            + sizeof(TacticalMapExtension)

            /**
             *  Extended type classes.
             */
            + sizeof(ObjectTypeClassExtension)
            + sizeof(TechnoTypeClassExtension)
            + sizeof(BuildingTypeClassExtension)
            + sizeof(UnitTypeClassExtension)
            + sizeof(InfantryTypeClassExtension)
            + sizeof(AircraftTypeClassExtension)
            + sizeof(WarheadTypeClassExtension)
            + sizeof(WeaponTypeClassExtension)
            + sizeof(BulletTypeClassExtension)
            + sizeof(SuperWeaponTypeClassExtension)
            + sizeof(VoxelAnimTypeClassExtension)
            + sizeof(AnimTypeClassExtension)
            + sizeof(ParticleTypeClassExtension)
            + sizeof(ParticleSystemTypeClassExtension)
            + sizeof(IsometricTileTypeClassExtension)
            + sizeof(OverlayTypeClassExtension)
            + sizeof(SmudgeTypeClassExtension)
            + sizeof(TerrainTypeClassExtension)
            + sizeof(HouseTypeClassExtension)
            + sizeof(SideClassExtension)
            + sizeof(CampaignClassExtension)
            + sizeof(TiberiumClassExtension)
            //+ sizeof(TaskForceClassExtension)
            //+ sizeof(AITriggerTypeClassExtension)
            //+ sizeof(ScriptTypeClassExtension)
            //+ sizeof(TagTypeClassExtension)
            //+ sizeof(TriggerTypeClassExtension)
            + sizeof(TechnoClassExtension)
            + sizeof(AircraftClassExtension)
            + sizeof(BuildingClassExtension)
            + sizeof(InfantryClassExtension)
            + sizeof(UnitClassExtension)
            + sizeof(TerrainClassExtension)
            + sizeof(WaveClassExtension)
            + sizeof(SuperClassExtension)
;


/**
 *  
 */
#ifndef NDEBUG
#define SAVE_DEBUG_INFO(x, ...) EXT_DEBUG_INFO(x, ##__VA_ARGS__)
#define SAVE_ASSERT_FATAL(x, ...) ASSERT_FATAL(x, ##__VA_ARGS__)
#define SAVE_ASSERT_FATAL_PRINT(x, ...) ASSERT_FATAL_PRINT(x, ##__VA_ARGS__)
#else
#define SAVE_DEBUG_INFO(x, ...) ((void)0)
#define SAVE_ASSERT_FATAL(x, ...) ((void)0)
#define SAVE_ASSERT_FATAL_PRINT(x, ...) ((void)0)
#endif


/**
 *  Handy macros for defining the save and load functions for an extended class.
 * 
 *  @author: CCHyper
 */
#define DEFINE_EXTENSION_SAVE(class_name, heap_name) \
    static bool Vinifera_Save_##class_name(IStream *pStm) \
    { \
        if (!pStm) { return false; } \
        LONG count = heap_name.size(); \
        HRESULT hr = pStm->Write(&count, sizeof(count), nullptr); \
        if (FAILED(hr)) { return false; } \
        if (heap_name.size() <= 0) { return true; } \
        for (const auto &i : heap_name.Map) { i.second->Save(pStm, true); } \
        return true; \
    }

#define DEFINE_EXTENSION_LOAD(class_name, heap_name) \
    static bool Vinifera_Load_##class_name(IStream *pStm) \
    { \
        if (!pStm) { return false; } \
        LONG count; \
        HRESULT hr = pStm->Read(&count, sizeof(count), nullptr); \
        if (FAILED(hr)) { return false; } \
        if ((unsigned)count <= 0) { return true; } \
        if ((unsigned)count > heap_name.size()) { return false; } \
        SAVE_ASSERT_FATAL_PRINT((unsigned)count == heap_name.size(), "Count = %d, HeapSize = %d", count, heap_name.size()); \
        for (const auto &i : heap_name.Map) { i.second->Load(pStm); } \
        return true; \
    }

#define DEFINE_GLOBAL_SAVE(class_name, global_name) \
    static bool Vinifera_Save_##class_name(IStream *pStm) \
    { \
        if (!pStm) { return false; } \
        if (!global_name) { return false; } \
        global_name->Save(pStm, true); \
        return true; \
    }

#define DEFINE_GLOBAL_LOAD(class_name, global_name) \
    static bool Vinifera_Load_##class_name(IStream *pStm) \
    { \
        if (!pStm) { return false; } \
        if (!global_name) { return false; } \
        global_name->Load(pStm); \
        return true; \
    }


/**
 *  Handy macros for performing the save and load for an extended class.
 * 
 *  @author: CCHyper
 */
#define SAVE_EXTENSION_CLASS(class_name, heap_name) \
    { \
        DEBUG_INFO("Saving %s...", #class_name); \
        if (!Vinifera_Save_##class_name(pStm)) { \
            DEBUG_ERROR(" ***** FAILED! *****\n"); \
            return false; \
        } \
        DEBUG_INFO(" OK! (Count: %d)\n", heap_name.size()); \
    }

#define LOAD_EXTENSION_CLASS(class_name, heap_name) \
    { \
        DEBUG_INFO("Loading %s...", #class_name); \
        if (!Vinifera_Load_##class_name(pStm)) { \
            DEBUG_ERROR(" ***** FAILED! *****\n"); \
            return false; \
        } \
        DEBUG_INFO(" OK! (Count: %d)\n", heap_name.size()); \
    }

#define SAVE_GLOBAL_CLASS(class_name, heap_name) \
    { \
        DEBUG_INFO("Saving %s...", #class_name); \
        if (!Vinifera_Save_##class_name(pStm)) { \
            DEBUG_ERROR(" ***** FAILED! *****\n"); \
            return false; \
        } \
        DEBUG_INFO(" OK!\n"); \
    }

#define LOAD_GLOBAL_CLASS(class_name, heap_name) \
    { \
        DEBUG_INFO("Loading %s...", #class_name); \
        if (!Vinifera_Load_##class_name(pStm)) { \
            DEBUG_ERROR(" ***** FAILED! *****\n"); \
            return false; \
        } \
        DEBUG_INFO(" OK!\n"); \
    }


static char DataCommitHash[64];
static char DataHeaderString[64];

#define VINIFERA_SAVE_HEADER_NAME "VINIFERA_SAVE_DATA  "


/**
 *  Saves the header marker for validating data on load.
 * 
 *  @author: CCHyper
 */
static bool Vinifera_Save_Header(IStream *pStm)
{
    if (!pStm) {
        return false;
    }

    HRESULT hr;

    /**
     *  Save the header string.
     */
    hr = pStm->Write(VINIFERA_SAVE_HEADER_NAME, sizeof(DataHeaderString), nullptr);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}


/**
 *  Loads the save data header marker.
 * 
 *  @author: CCHyper
 */
static bool Vinifera_Load_Header(IStream *pStm)
{
    if (!pStm) {
        return false;
    }

    HRESULT hr;

    /**
     *  Load the header string.
     */
    hr = pStm->Read(DataHeaderString, sizeof(DataHeaderString), nullptr);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}


/**
 *  Saves the commit hash for checking on load.
 * 
 *  @author: CCHyper
 */
static bool Vinifera_Save_Version_Info(IStream *pStm)
{
    if (!pStm) {
        return false;
    }

    HRESULT hr;

    /**
     *  Save the commit hash.
     */
    hr = pStm->Write(Vinifera_Git_Hash(), sizeof(DataCommitHash), nullptr);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}


/**
 *  Load the commit hash for version checks.
 * 
 *  @author: CCHyper
 */
static bool Vinifera_Load_Version_Info(IStream *pStm)
{
    if (!pStm) {
        return false;
    }

    HRESULT hr;

    /**
     *  Load the commit hash.
     */
    hr = pStm->Read(DataCommitHash, sizeof(DataCommitHash), nullptr);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}


/**
 * 
 *  Define the save and load functions for the new classes.
 * 
 */

DEFINE_GLOBAL_SAVE(RulesClassExtension, RulesExtension);
DEFINE_GLOBAL_SAVE(SessionClassExtension, SessionExtension);
DEFINE_GLOBAL_SAVE(ScenarioClassExtension, ScenarioExtension);
DEFINE_GLOBAL_SAVE(TacticalMapExtension, TacticalExtension);

DEFINE_GLOBAL_LOAD(RulesClassExtension, RulesExtension);
DEFINE_GLOBAL_LOAD(SessionClassExtension, SessionExtension);
DEFINE_GLOBAL_LOAD(ScenarioClassExtension, ScenarioExtension);
DEFINE_GLOBAL_LOAD(TacticalMapExtension, TacticalExtension);

DEFINE_EXTENSION_SAVE(ObjectTypeClassExtension, ObjectTypeClassExtensions);
DEFINE_EXTENSION_SAVE(TechnoTypeClassExtension, TechnoTypeClassExtensions);
DEFINE_EXTENSION_SAVE(BuildingTypeClassExtension, BuildingTypeClassExtensions);
DEFINE_EXTENSION_SAVE(UnitTypeClassExtension, UnitTypeClassExtensions);
DEFINE_EXTENSION_SAVE(InfantryTypeClassExtension, InfantryTypeClassExtensions);
DEFINE_EXTENSION_SAVE(AircraftTypeClassExtension, AircraftTypeClassExtensions);
DEFINE_EXTENSION_SAVE(WarheadTypeClassExtension, WarheadTypeClassExtensions);
DEFINE_EXTENSION_SAVE(WeaponTypeClassExtension, WeaponTypeClassExtensions);
DEFINE_EXTENSION_SAVE(BulletTypeClassExtension, BulletTypeClassExtensions);
DEFINE_EXTENSION_SAVE(SuperWeaponTypeClassExtension, SuperWeaponTypeClassExtensions);
DEFINE_EXTENSION_SAVE(VoxelAnimTypeClassExtension, VoxelAnimTypeClassExtensions);
DEFINE_EXTENSION_SAVE(AnimTypeClassExtension, AnimTypeClassExtensions);
DEFINE_EXTENSION_SAVE(ParticleTypeClassExtension, ParticleTypeClassExtensions);
DEFINE_EXTENSION_SAVE(ParticleSystemTypeClassExtension, ParticleSystemTypeClassExtensions);
DEFINE_EXTENSION_SAVE(IsometricTileTypeClassExtension, IsometricTileTypeClassExtensions);
DEFINE_EXTENSION_SAVE(OverlayTypeClassExtension, OverlayTypeClassExtensions);
DEFINE_EXTENSION_SAVE(SmudgeTypeClassExtension, SmudgeTypeClassExtensions);
DEFINE_EXTENSION_SAVE(TerrainTypeClassExtension, TerrainTypeClassExtensions);
DEFINE_EXTENSION_SAVE(HouseTypeClassExtension, HouseTypeClassExtensions);
DEFINE_EXTENSION_SAVE(SideClassExtension, SideClassExtensions);
DEFINE_EXTENSION_SAVE(CampaignClassExtension, CampaignClassExtensions);
DEFINE_EXTENSION_SAVE(TiberiumClassExtension, TiberiumClassExtensions);
//DEFINE_EXTENSION_SAVE(TaskForceClassExtension, TaskForceClassExtensions);
//DEFINE_EXTENSION_SAVE(AITriggerTypeClassExtension, AITriggerTypeClassExtensions);
//DEFINE_EXTENSION_SAVE(ScriptTypeClassExtension, ScriptTypeClassExtensions);
//DEFINE_EXTENSION_SAVE(TagTypeClassExtension, TagTypeClassExtensions);
//DEFINE_EXTENSION_SAVE(TriggerTypeClassExtension, TriggerTypeClassExtensions);
DEFINE_EXTENSION_SAVE(TechnoClassExtension, TechnoClassExtensions);
DEFINE_EXTENSION_SAVE(AircraftClassExtension, AircraftClassExtensions);
DEFINE_EXTENSION_SAVE(BuildingClassExtension, BuildingClassExtensions);
DEFINE_EXTENSION_SAVE(InfantryClassExtension, InfantryClassExtensions);
DEFINE_EXTENSION_SAVE(UnitClassExtension, UnitClassExtensions);
DEFINE_EXTENSION_SAVE(TerrainClassExtension, TerrainClassExtensions);
DEFINE_EXTENSION_SAVE(SuperClassExtension, SuperClassExtensions);
DEFINE_EXTENSION_SAVE(WaveClassExtension, WaveClassExtensions);

DEFINE_EXTENSION_LOAD(ObjectTypeClassExtension, ObjectTypeClassExtensions);
DEFINE_EXTENSION_LOAD(TechnoTypeClassExtension, TechnoTypeClassExtensions);
DEFINE_EXTENSION_LOAD(BuildingTypeClassExtension, BuildingTypeClassExtensions);
DEFINE_EXTENSION_LOAD(UnitTypeClassExtension, UnitTypeClassExtensions);
DEFINE_EXTENSION_LOAD(InfantryTypeClassExtension, InfantryTypeClassExtensions);
DEFINE_EXTENSION_LOAD(AircraftTypeClassExtension, AircraftTypeClassExtensions);
DEFINE_EXTENSION_LOAD(WarheadTypeClassExtension, WarheadTypeClassExtensions);
DEFINE_EXTENSION_LOAD(WeaponTypeClassExtension, WeaponTypeClassExtensions);
DEFINE_EXTENSION_LOAD(BulletTypeClassExtension, BulletTypeClassExtensions);
DEFINE_EXTENSION_LOAD(SuperWeaponTypeClassExtension, SuperWeaponTypeClassExtensions);
DEFINE_EXTENSION_LOAD(VoxelAnimTypeClassExtension, VoxelAnimTypeClassExtensions);
DEFINE_EXTENSION_LOAD(AnimTypeClassExtension, AnimTypeClassExtensions);
DEFINE_EXTENSION_LOAD(ParticleTypeClassExtension, ParticleTypeClassExtensions);
DEFINE_EXTENSION_LOAD(ParticleSystemTypeClassExtension, ParticleSystemTypeClassExtensions);
DEFINE_EXTENSION_LOAD(IsometricTileTypeClassExtension, IsometricTileTypeClassExtensions);
DEFINE_EXTENSION_LOAD(OverlayTypeClassExtension, OverlayTypeClassExtensions);
DEFINE_EXTENSION_LOAD(SmudgeTypeClassExtension, SmudgeTypeClassExtensions);
DEFINE_EXTENSION_LOAD(TerrainTypeClassExtension, TerrainTypeClassExtensions);
DEFINE_EXTENSION_LOAD(HouseTypeClassExtension, HouseTypeClassExtensions);
DEFINE_EXTENSION_LOAD(SideClassExtension, SideClassExtensions);
DEFINE_EXTENSION_LOAD(CampaignClassExtension, CampaignClassExtensions);
DEFINE_EXTENSION_LOAD(TiberiumClassExtension, TiberiumClassExtensions);
//DEFINE_EXTENSION_LOAD(TaskForceClassExtension, TaskForceClassExtensions);
//DEFINE_EXTENSION_LOAD(AITriggerTypeClassExtension, AITriggerTypeClassExtensions);
//DEFINE_EXTENSION_LOAD(ScriptTypeClassExtension, ScriptTypeClassExtensions);
//DEFINE_EXTENSION_LOAD(TagTypeClassExtension, TagTypeClassExtensions);
//DEFINE_EXTENSION_LOAD(TriggerTypeClassExtension, TriggerTypeClassExtensions);
DEFINE_EXTENSION_LOAD(TechnoClassExtension, TechnoClassExtensions);
DEFINE_EXTENSION_LOAD(AircraftClassExtension, AircraftClassExtensions);
DEFINE_EXTENSION_LOAD(BuildingClassExtension, BuildingClassExtensions);
DEFINE_EXTENSION_LOAD(InfantryClassExtension, InfantryClassExtensions);
DEFINE_EXTENSION_LOAD(UnitClassExtension, UnitClassExtensions);
DEFINE_EXTENSION_LOAD(TerrainClassExtension, TerrainClassExtensions);
DEFINE_EXTENSION_LOAD(SuperClassExtension, SuperClassExtensions);
DEFINE_EXTENSION_LOAD(WaveClassExtension, WaveClassExtensions);


/**
 *  Save all Vinifera data to the file stream.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Put_All(IStream *pStm)
{
    /**
     *  Save the Vinifera data marker which can be used to verify
     *  the state of the data to follow on load.
     */
    DEBUG_INFO("Saving Vinifera header marker...");
    if (!Vinifera_Save_Header(pStm)) {
        DEBUG_ERROR(" ***** FAILED! *****\n");
        return false;
    }
    DEBUG_INFO(" OK!\n");

    /**
     *  Save the build version information for load checks.
     */
    DEBUG_INFO("Saving Vinifera version information...");
    if (!Vinifera_Save_Version_Info(pStm)) {
        DEBUG_ERROR(" ***** FAILED! *****\n");
        return false;
    }
    DEBUG_INFO(" OK!\n");

    DEBUG_INFO("Saving with Git commit hash: %s.\n", Vinifera_Git_Hash());

    /**
     *  Save class extensions here.
     */
    //DEBUG_INFO("Saving extended class data...\n");

    SAVE_GLOBAL_CLASS(RulesClassExtension, RulesExtension);
    SAVE_GLOBAL_CLASS(SessionClassExtension, SessionExtension);
    SAVE_GLOBAL_CLASS(ScenarioClassExtension, ScenarioExtension);
    SAVE_GLOBAL_CLASS(TacticalMapExtension, TacticalExtension);

    SAVE_EXTENSION_CLASS(ObjectTypeClassExtension, ObjectTypeClassExtensions);
    SAVE_EXTENSION_CLASS(TechnoTypeClassExtension, TechnoTypeClassExtensions);
    SAVE_EXTENSION_CLASS(BuildingTypeClassExtension, BuildingTypeClassExtensions);
    SAVE_EXTENSION_CLASS(UnitTypeClassExtension, UnitTypeClassExtensions);
    SAVE_EXTENSION_CLASS(InfantryTypeClassExtension, InfantryTypeClassExtensions);
    SAVE_EXTENSION_CLASS(AircraftTypeClassExtension, AircraftTypeClassExtensions);
    SAVE_EXTENSION_CLASS(WarheadTypeClassExtension, WarheadTypeClassExtensions);
    SAVE_EXTENSION_CLASS(WeaponTypeClassExtension, WeaponTypeClassExtensions);
    SAVE_EXTENSION_CLASS(BulletTypeClassExtension, BulletTypeClassExtensions);
    SAVE_EXTENSION_CLASS(SuperWeaponTypeClassExtension, SuperWeaponTypeClassExtensions);
    SAVE_EXTENSION_CLASS(VoxelAnimTypeClassExtension, VoxelAnimTypeClassExtensions);
    SAVE_EXTENSION_CLASS(AnimTypeClassExtension, AnimTypeClassExtensions);
    SAVE_EXTENSION_CLASS(ParticleTypeClassExtension, ParticleTypeClassExtensions);
    SAVE_EXTENSION_CLASS(ParticleSystemTypeClassExtension, ParticleSystemTypeClassExtensions);
    SAVE_EXTENSION_CLASS(OverlayTypeClassExtension, OverlayTypeClassExtensions);
    SAVE_EXTENSION_CLASS(SmudgeTypeClassExtension, SmudgeTypeClassExtensions);
    SAVE_EXTENSION_CLASS(TerrainTypeClassExtension, TerrainTypeClassExtensions);
    SAVE_EXTENSION_CLASS(HouseTypeClassExtension, HouseTypeClassExtensions);
    SAVE_EXTENSION_CLASS(SideClassExtension, SideClassExtensions);
    SAVE_EXTENSION_CLASS(CampaignClassExtension, CampaignClassExtensions);
    SAVE_EXTENSION_CLASS(TiberiumClassExtension, TiberiumClassExtensions);
    //SAVE_EXTENSION_CLASS(TaskForceClassExtension, TaskForceClassExtensions);
    //SAVE_EXTENSION_CLASS(AITriggerTypeClassExtension, AITriggerTypeClassExtensions);
    //SAVE_EXTENSION_CLASS(ScriptTypeClassExtension, ScriptTypeClassExtensions);
    //SAVE_EXTENSION_CLASS(TagTypeClassExtension, TagTypeClassExtensions);
    //SAVE_EXTENSION_CLASS(TriggerTypeClassExtension, TriggerTypeClassExtensions);
    SAVE_EXTENSION_CLASS(TechnoClassExtension, TechnoClassExtensions);
    SAVE_EXTENSION_CLASS(AircraftClassExtension, AircraftClassExtensions);
    SAVE_EXTENSION_CLASS(BuildingClassExtension, BuildingClassExtensions);
    SAVE_EXTENSION_CLASS(InfantryClassExtension, InfantryClassExtensions);
    SAVE_EXTENSION_CLASS(UnitClassExtension, UnitClassExtensions);
    SAVE_EXTENSION_CLASS(TerrainClassExtension, TerrainClassExtensions);
    SAVE_EXTENSION_CLASS(SuperClassExtension, SuperClassExtensions);
    SAVE_EXTENSION_CLASS(WaveClassExtension, WaveClassExtensions);

    /**
     *  Save global data and values here.
     */
    //DEBUG_INFO("Saving global data...\n");

    return true;
}


/**
 *  Load all Vinifera data from the file stream.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Load_All(IStream *pStm)
{
    /**
     *  Load the Vinifera data marker which can be used to verify
     *  the state of the data to follow.
     */
    DEBUG_INFO("Loading Vinifera header marker...");
    if (!Vinifera_Load_Header(pStm)) {
        DEBUG_ERROR(" ***** FAILED! *****\n");
#if 0
        ShowCursor(TRUE);
        MessageBoxA(MainWindow, "Failed to load Vinifera save-file header!\n", "Vinifera", MB_OK|MB_ICONEXCLAMATION);
        Vinifera_Generate_Mini_Dump();
        Fatal("Failed to load Vinifera save-file header!\n");
#endif
        return false;
    }
    DEBUG_INFO(" OK!\n");
    
    DEBUG_INFO("Checking version header...");
    if (std::strncmp(VINIFERA_SAVE_HEADER_NAME, DataHeaderString, sizeof(DataHeaderString)) != 0) {
        DEBUG_WARNING(" ***** MISMATCH! *****\n");
        DEBUG_WARNING("Loaded: %s\n", DataHeaderString);
        DEBUG_WARNING("Expected: %s\n", VINIFERA_SAVE_HEADER_NAME);
#if 0
        ShowCursor(TRUE);
        MessageBoxA(MainWindow, "Invalid header in save file!\n", "Vinifera", MB_OK|MB_ICONEXCLAMATION);
        Vinifera_Generate_Mini_Dump();
        Fatal("Invalid header in save file!\n");
#endif
        return false;
    }
    DEBUG_INFO(" OK!\n");

    /**
     *  Load the build version information.
     */
    DEBUG_INFO("Loading Vinifera version information...");
    if (!Vinifera_Load_Version_Info(pStm)) {
        DEBUG_ERROR(" ***** FAILED! *****\n");
        return false;
    }
    DEBUG_INFO(" OK!\n");
    
    DEBUG_INFO("Checking Git commit hash...");
    if (std::strncmp(Vinifera_Git_Hash(), DataCommitHash, sizeof(DataCommitHash)) != 0) {
        DEBUG_WARNING(" ***** MISMATCH! *****\n");
        DEBUG_WARNING("Loaded: %s\n", DataCommitHash);
        DEBUG_WARNING("Expected: %s\n", Vinifera_Git_Hash());
        //return false;
    } else {
        DEBUG_INFO(" OK!\n");
        DEBUG_INFO("Loaded commit hash: %s.\n", DataCommitHash);
    }

    /**
     *  Load class extensions here.
     */
    //DEBUG_INFO("Loading extended class data...\n");

    LOAD_GLOBAL_CLASS(RulesClassExtension, RulesExtension);
    LOAD_GLOBAL_CLASS(SessionClassExtension, SessionExtension);
    LOAD_GLOBAL_CLASS(ScenarioClassExtension, ScenarioExtension);
    LOAD_GLOBAL_CLASS(TacticalMapExtension, TacticalExtension);

    LOAD_EXTENSION_CLASS(ObjectTypeClassExtension, ObjectTypeClassExtensions);
    LOAD_EXTENSION_CLASS(TechnoTypeClassExtension, TechnoTypeClassExtensions);
    LOAD_EXTENSION_CLASS(BuildingTypeClassExtension, BuildingTypeClassExtensions);
    LOAD_EXTENSION_CLASS(UnitTypeClassExtension, UnitTypeClassExtensions);
    LOAD_EXTENSION_CLASS(InfantryTypeClassExtension, InfantryTypeClassExtensions);
    LOAD_EXTENSION_CLASS(AircraftTypeClassExtension, AircraftTypeClassExtensions);
    LOAD_EXTENSION_CLASS(WarheadTypeClassExtension, WarheadTypeClassExtensions);
    LOAD_EXTENSION_CLASS(WeaponTypeClassExtension, WeaponTypeClassExtensions);
    LOAD_EXTENSION_CLASS(BulletTypeClassExtension, BulletTypeClassExtensions);
    LOAD_EXTENSION_CLASS(SuperWeaponTypeClassExtension, SuperWeaponTypeClassExtensions);
    LOAD_EXTENSION_CLASS(VoxelAnimTypeClassExtension, VoxelAnimTypeClassExtensions);
    LOAD_EXTENSION_CLASS(AnimTypeClassExtension, AnimTypeClassExtensions);
    LOAD_EXTENSION_CLASS(ParticleTypeClassExtension, ParticleTypeClassExtensions);
    LOAD_EXTENSION_CLASS(ParticleSystemTypeClassExtension, ParticleSystemTypeClassExtensions);
    LOAD_EXTENSION_CLASS(OverlayTypeClassExtension, OverlayTypeClassExtensions);
    LOAD_EXTENSION_CLASS(SmudgeTypeClassExtension, SmudgeTypeClassExtensions);
    LOAD_EXTENSION_CLASS(TerrainTypeClassExtension, TerrainTypeClassExtensions);
    LOAD_EXTENSION_CLASS(HouseTypeClassExtension, HouseTypeClassExtensions);
    LOAD_EXTENSION_CLASS(SideClassExtension, SideClassExtensions);
    LOAD_EXTENSION_CLASS(CampaignClassExtension, CampaignClassExtensions);
    LOAD_EXTENSION_CLASS(TiberiumClassExtension, TiberiumClassExtensions);
    //LOAD_EXTENSION_CLASS(TaskForceClassExtension, TaskForceClassExtensions);
    //LOAD_EXTENSION_CLASS(AITriggerTypeClassExtension, AITriggerTypeClassExtensions);
    //LOAD_EXTENSION_CLASS(ScriptTypeClassExtension, ScriptTypeClassExtensions);
    //LOAD_EXTENSION_CLASS(TagTypeClassExtension, TagTypeClassExtensions);
    //LOAD_EXTENSION_CLASS(TriggerTypeClassExtension, TriggerTypeClassExtensions);
    LOAD_EXTENSION_CLASS(TechnoClassExtension, TechnoClassExtensions);
    LOAD_EXTENSION_CLASS(AircraftClassExtension, AircraftClassExtensions);
    LOAD_EXTENSION_CLASS(BuildingClassExtension, BuildingClassExtensions);
    LOAD_EXTENSION_CLASS(InfantryClassExtension, InfantryClassExtensions);
    LOAD_EXTENSION_CLASS(UnitClassExtension, UnitClassExtensions);
    LOAD_EXTENSION_CLASS(TerrainClassExtension, TerrainClassExtensions);
    LOAD_EXTENSION_CLASS(SuperClassExtension, SuperClassExtensions);
    LOAD_EXTENSION_CLASS(WaveClassExtension, WaveClassExtensions);

    /**
     *  Load global data and values here.
     */
    //DEBUG_INFO("Loading global data...\n");

    return true;
}
