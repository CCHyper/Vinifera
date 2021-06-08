/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BINK_LOAD_DLL.H
 *
 *  @author        CCHyper
 *
 *  @brief         Utility functions for performing one-time loading of
 *                 Bink library functions from the DLL.
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
#include "bink_load_dll.h"
#include "debughandler.h"


/**
 *  Loaded Bink DLL library.
 */
static HMODULE BinkDLL = nullptr;


/**
 *  We successfully loaded all imports?
 */
bool BinkImportsLoaded = false;


/**
 *  Pointers to Bink DLL exports.
 */
BINKCLOSE BinkClose = nullptr;
BINKDDSURFACETYPE BinkDDSurfaceType = nullptr;
BINKSETVOLUME BinkSetVolume = nullptr;
BINKGETERROR BinkGetError = nullptr;
BINKOPEN BinkOpen = nullptr;
BINKSETSOUNDSYSTEM BinkSetSoundSystem = nullptr;
BINKOPENDIRECTSOUND BinkOpenDirectSound = nullptr;
BINKGOTO BinkGoto = nullptr;
BINKPAUSE BinkPause = nullptr;
BINKNEXTFRAME BinkNextFrame = nullptr;
BINKCOPYTOBUFFER BinkCopyToBuffer = nullptr;
BINKDOFRAME BinkDoFrame = nullptr;
BINKWAIT BinkWait = nullptr;


/**
 *  Load the Bink DLL and any imports we need.
 * 
 *  @author: CCHyper
 */
bool Load_Bink_DLL()
{
    /**
     *  We already performed a successful one-time init, return true.
     */
    if (BinkImportsLoaded) {
        return true;
    }
    
    DEBUG_INFO("Load_Bink_DLL()\n");

    /**
     *  Look for Bink DLL.
     */
    if (BinkDLL == nullptr && GetFileAttributesA("binkw32.dll") != INVALID_FILE_ATTRIBUTES) {
        BinkDLL = LoadLibraryA("binkw32.dll");
    }
    
    if (BinkDLL == nullptr) {
        DEBUG_ERROR("LoadLibraryA() failed with %d.\n", GetLastError());
        BinkImportsLoaded = false;
        return false;
    }

    BinkClose = (BINKCLOSE)GetProcAddress(BinkDLL, "_BinkClose@4");
    if (!BinkClose) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkClose", GetLastError());
        return false;
    }

    BinkDDSurfaceType = (BINKDDSURFACETYPE)GetProcAddress(BinkDLL, "_BinkDDSurfaceType@4");
    if (!BinkDDSurfaceType) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkDDSurfaceType", GetLastError());
        return false;
    }

    BinkSetVolume = (BINKSETVOLUME)GetProcAddress(BinkDLL, "_BinkSetVolume@8");
    if (!BinkSetVolume) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkSetVolume", GetLastError());
        return false;
    }

    BinkGetError = (BINKGETERROR)GetProcAddress(BinkDLL, "_BinkGetError@0");
    if (!BinkGetError) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkGetError", GetLastError());
        return false;
    }

    BinkOpen = (BINKOPEN)GetProcAddress(BinkDLL, "_BinkOpen@8");
    if (!BinkOpen) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "_BinkOpen@8", GetLastError());
        return false;
    }

    BinkSetSoundSystem = (BINKSETSOUNDSYSTEM)GetProcAddress(BinkDLL, "_BinkSetSoundSystem@8");
    if (!BinkSetSoundSystem) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkSetSoundSystem", GetLastError());
        return false;
    }

    BinkOpenDirectSound = (BINKOPENDIRECTSOUND)GetProcAddress(BinkDLL, "_BinkOpenDirectSound@4");
    if (!BinkOpenDirectSound) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkOpenDirectSound", GetLastError());
        return false;
    }

    BinkGoto = (BINKGOTO)GetProcAddress(BinkDLL, "_BinkGoto@12");
    if (!BinkGoto) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkGoto", GetLastError());
        return false;
    }

    BinkPause = (BINKPAUSE)GetProcAddress(BinkDLL, "_BinkPause@8");
    if (!BinkPause) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkPause", GetLastError());
        return false;
    }

    BinkNextFrame = (BINKNEXTFRAME)GetProcAddress(BinkDLL, "_BinkNextFrame@4");
    if (!BinkNextFrame) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkNextFrame", GetLastError());
        return false;
    }

    BinkCopyToBuffer = (BINKCOPYTOBUFFER)GetProcAddress(BinkDLL, "_BinkCopyToBuffer@28");
    if (!BinkCopyToBuffer) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkCopyToBuffer", GetLastError());
        return false;
    }

    BinkDoFrame = (BINKDOFRAME)GetProcAddress(BinkDLL, "_BinkDoFrame@4");
    if (!BinkDoFrame) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkDoFrame", GetLastError());
        return false;
    }

    BinkWait = (BINKWAIT)GetProcAddress(BinkDLL, "_BinkWait@4");
    if (!BinkWait) {
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", "BinkWait", GetLastError());
        return false;
    }

    /**
     *  All loaded and ready to go.
     */
    BinkImportsLoaded = true;

    return true;
}


/**
 *  Free the Bink library and all loaded pointers.
 * 
 *  @author: CCHyper
 */
void Unload_Bink_DLL()
{
    FreeLibrary(BinkDLL);
    
    BinkClose = nullptr;
    BinkDDSurfaceType = nullptr;
    BinkSetVolume = nullptr;
    BinkGetError = nullptr;
    BinkOpen = nullptr;
    BinkSetSoundSystem = nullptr;
    BinkOpenDirectSound = nullptr;
    BinkGoto = nullptr;
    BinkPause = nullptr;
    BinkNextFrame = nullptr;
    BinkCopyToBuffer = nullptr;
    BinkDoFrame = nullptr;
    BinkWait = nullptr;
    
    BinkImportsLoaded = false;
}
