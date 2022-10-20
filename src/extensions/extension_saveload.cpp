/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          EXT_SAVELOAD.CPP
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
#include "saveload.h"
#include "wwcrc.h"
#include "vinifera_gitinfo.h"
#include "vinifera_util.h"
#include "tibsun_globals.h"
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

#include "extension.h"


/**
 *  Constant of the current build version number. This number should be
 *  a sum of all the extended class sizes plus the build date.
 */
unsigned ViniferaSaveGameVersion = 10000;


/**
 *  Save file header.
 */
typedef struct ViniferaSaveFileHeaderStruct
{
    // Header marker.
    char Marker[20];

    // Git commit hash.
    char CommitHash[40];

    // Constant header marker to check for.
    static const char * Marker_String() { return "VINIFERA_SAVE_FILE"; }

private:
    char _padding[1024
                  - sizeof(Marker)
                  - sizeof(CommitHash)];
};
static_assert(sizeof(ViniferaSaveFileHeaderStruct), "ViniferaSaveFileHeaderStruct must be 1024 bytes in size!");

static ViniferaSaveFileHeaderStruct ViniferaSaveFileHeader; 


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
     *  Save the new header.
     */
    std::memset(&ViniferaSaveFileHeader, 0, sizeof(ViniferaSaveFileHeader));

    strncpy(ViniferaSaveFileHeader.Marker, ViniferaSaveFileHeaderStruct::Marker_String(), sizeof(ViniferaSaveFileHeader.Marker));
    strncpy(ViniferaSaveFileHeader.CommitHash, Vinifera_Git_Hash(), sizeof(ViniferaSaveFileHeader.CommitHash));

    hr = pStm->Write(&ViniferaSaveFileHeader, sizeof(ViniferaSaveFileHeader), nullptr);
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
     *  Load the new header.
     */
    std::memset(&ViniferaSaveFileHeader, 0, sizeof(ViniferaSaveFileHeader));

    hr = pStm->Read(&ViniferaSaveFileHeader, sizeof(ViniferaSaveFileHeader), nullptr);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}


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
    DEBUG_INFO("Saving Vinifera header\n");
    if (!Vinifera_Save_Header(pStm)) {
        DEBUG_ERROR("\t***** FAILED!\n");
        return false;
    }

    /**
     *  Save class extensions here.
     */
    DEBUG_INFO("Saving class extensions\n");
    if (!Extension::Save(pStm)) {
        DEBUG_ERROR("\t***** FAILED!\n");
        return false;
    }

    /**
     *  Save global data and values here.
     */
    DEBUG_INFO("Saving global data...\n");

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
    DEBUG_INFO("Loading Vinifera header\n");
    if (!Vinifera_Load_Header(pStm)) {
        DEBUG_ERROR("\t***** FAILED!\n");

        ShowCursor(TRUE);
        MessageBoxA(MainWindow, "Failed to load Vinifera save-file header!\n", "Vinifera", MB_OK|MB_ICONEXCLAMATION);
        Vinifera_Generate_Mini_Dump();
        Fatal("Failed to load Vinifera save-file header!\n");

        return false;
    }

    if (std::strncmp(ViniferaSaveFileHeader.Marker, ViniferaSaveFileHeaderStruct::Marker_String(), sizeof(ViniferaSaveFileHeader.Marker)) != 0) {
        DEBUG_WARNING("Invalid header in save file!");

        ShowCursor(TRUE);
        MessageBoxA(MainWindow, "Invalid header in save file!\n", "Vinifera", MB_OK|MB_ICONEXCLAMATION);
        Vinifera_Generate_Mini_Dump();
        Fatal("Invalid header in save file!\n");

        return false;
    }

    if (std::strncmp(ViniferaSaveFileHeader.CommitHash, Vinifera_Git_Hash(), sizeof(ViniferaSaveFileHeader.CommitHash)) != 0) {
        DEBUG_WARNING("Git hash mismatch in save file!");
        //return false;
    }

    /**
     *  Load class extensions here.
     */
    DEBUG_INFO("Loading class extensions\n");
    if (!Extension::Load(pStm)) {
        DEBUG_ERROR("\t***** FAILED!\n");
        return false;
    }

    /**
     *  Load global data and values here.
     */
    DEBUG_INFO("Loading global data...\n");

    return true;
}


/**
 *  Request remapping of all the extension pointers so the swizzle manager
 *  can fix up any reference to extension classes.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Remap_Extension_Pointers()
{
    DEBUG_INFO("Remapping extension pointers\n");
    if (!Extension::Request_Pointer_Remap()) {
        DEBUG_ERROR("\t***** FAILED!\n");
        return false;
    }

    return true;
}
