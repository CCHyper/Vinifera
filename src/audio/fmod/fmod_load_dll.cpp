/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FMOD_LOAD_DLL.H
 *
 *  @author        CCHyper
 *
 *  @brief         Utility functions for performing one-time loading of
 *                 FMOD library functions from the DLL.
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
#include "fmod_load_dll.h"
#include "fmod_globals.h"
#include "winutil.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <Windows.h>


/**
 *  Loaded FMOD DLL library.
 */
static HMODULE FMODDLL = nullptr;


/**
 *  We successfully loaded all imports?
 */
bool FMODImportsLoaded = false;


/**
 *  Load the FMOD DLL and any imports we need.
 * 
 *  @author: CCHyper
 */
bool Load_FMOD_DLL()
{
    /**
     *  We already performed a successful one-time init, return success.
     */
    if (FMODImportsLoaded) {
        return true;
    }
    
    const char *lib_name = nullptr;

    /**
     *  Look for the FMOD dll in the system directory.
     */
    FMODDLL = LoadLibrary("FMOD32.dll");
    lib_name = "FMOD32.dll";

#if 0
    /**
     *  If LoadLibrary failed, try loading the FMOD-Soft DLL from the game directory.
     */
    if (FMODDLL == nullptr && GetFileAttributesA("soft_oal.dll") != INVALID_FILE_ATTRIBUTES) {
        FMODDLL = LoadLibrary("soft_oal.dll");
        lib_name = "soft_oal.dll";
    }
#endif
    
    if (FMODDLL == nullptr) {
        DEBUG_INFO("Failed to load FMOD library!\n");
        FMODImportsLoaded = false;
        return false;
    }

    DEBUG_INFO("FMOD library found: \"%s\"\n", lib_name);
    DEV_DEBUG_INFO("Load_FMOD_DLL()\n");
    
    alSourcef = (LPALSOURCEF)GetProcAddress(FMODDLL, "alSourcef");
    if (!alSourcef) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourcef", GetLastError());
        return false;
    }

    alSource3f = (LPALSOURCE3F)GetProcAddress(FMODDLL, "alSource3f");
    if (!alSource3f) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSource3f", GetLastError());
        return false;
    }

    alSourcei = (LPALSOURCEI)GetProcAddress(FMODDLL, "alSourcei");
    if (!alSourcei) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourcei", GetLastError());
        return false;
    }
    
    alSourceQueueBuffers = (LPALSOURCEQUEUEBUFFERS)GetProcAddress(FMODDLL, "alSourceQueueBuffers");
    if (!alSourceQueueBuffers) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourceQueueBuffers", GetLastError());
        return false;
    }
    
    alBufferData = (LPALBUFFERDATA)GetProcAddress(FMODDLL, "alBufferData");
    if (!alBufferData) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alBufferData", GetLastError());
        return false;
    }
    
    alGenBuffers = (LPALGENBUFFERS)GetProcAddress(FMODDLL, "alGenBuffers");
    if (!alGenBuffers) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alGenBuffers", GetLastError());
        return false;
    }
    
    alDeleteBuffers = (LPALDELETEBUFFERS)GetProcAddress(FMODDLL, "alDeleteBuffers");
    if (!alDeleteBuffers) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alDeleteBuffers", GetLastError());
        return false;
    }
    
    alSourceUnqueueBuffers = (LPALSOURCEUNQUEUEBUFFERS)GetProcAddress(FMODDLL, "alSourceUnqueueBuffers");
    if (!alSourceUnqueueBuffers) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourceUnqueueBuffers", GetLastError());
        return false;
    }
    
    alGetSourcei = (LPALGETSOURCEI)GetProcAddress(FMODDLL, "alGetSourcei");
    if (!alGetSourcei) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alGetSourcei", GetLastError());
        return false;
    }
    
    alSourceStop = (LPALSOURCESTOP)GetProcAddress(FMODDLL, "alSourceStop");
    if (!alSourceStop) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourceStop", GetLastError());
        return false;
    }
    
    alSourcePlay = (LPALSOURCEPLAY)GetProcAddress(FMODDLL, "alSourcePlay");
    if (!alSourcePlay) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourcePlay", GetLastError());
        return false;
    }
    
    alDeleteSources = (LPALDELETESOURCES)GetProcAddress(FMODDLL, "alDeleteSources");
    if (!alDeleteSources) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alDeleteSources", GetLastError());
        return false;
    }
    
    alGenSources = (LPALGENSOURCES)GetProcAddress(FMODDLL, "alGenSources");
    if (!alGenSources) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alGenSources", GetLastError());
        return false;
    }
    
    alcOpenDevice = (LPALCOPENDEVICE)GetProcAddress(FMODDLL, "alcOpenDevice");
    if (!alcOpenDevice) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcOpenDevice", GetLastError());
        return false;
    }
    
    alcCloseDevice = (LPALCCLOSEDEVICE)GetProcAddress(FMODDLL, "alcCloseDevice");
    if (!alcCloseDevice) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcCloseDevice", GetLastError());
        return false;
    }
    
    alcCreateContext = (LPALCCREATECONTEXT)GetProcAddress(FMODDLL, "alcCreateContext");
    if (!alcCreateContext) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcCreateContext", GetLastError());
        return false;
    }
    
    alcDestroyContext = (LPALCDESTROYCONTEXT)GetProcAddress(FMODDLL, "alcDestroyContext");
    if (!alcDestroyContext) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcDestroyContext", GetLastError());
        return false;
    }
    
    alcSuspendContext = (LPALCSUSPENDCONTEXT)GetProcAddress(FMODDLL, "alcSuspendContext");
    if (!alcSuspendContext) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcSuspendContext", GetLastError());
        return false;
    }
    
    alcProcessContext = (LPALCPROCESSCONTEXT)GetProcAddress(FMODDLL, "alcProcessContext");
    if (!alcProcessContext) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcProcessContext", GetLastError());
        return false;
    }
    
    alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)GetProcAddress(FMODDLL, "alcMakeContextCurrent");
    if (!alcMakeContextCurrent) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcMakeContextCurrent", GetLastError());
        return false;
    }
    
    alcGetContextsDevice = (LPALCGETCONTEXTSDEVICE)GetProcAddress(FMODDLL, "alcGetContextsDevice");
    if (!alcGetContextsDevice) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcGetContextsDevice", GetLastError());
        return false;
    }

    alcGetError = (LPALCGETERROR)GetProcAddress(FMODDLL, "alcGetError");
    if (!alcGetError) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcGetError", GetLastError());
        return false;
    }
    
    /**
     *  All loaded and ready to go.
     */
    FMODImportsLoaded = true;
    
    return true;
}


/**
 *  Free the FMOD library and all loaded pointers.
 * 
 *  @author: CCHyper
 */
void Unload_FMOD_DLL()
{
    DEV_DEBUG_INFO("Unload_FMOD_DLL()\n");

    FreeLibrary(FMODDLL);
    
    alSourcef = nullptr;
    alSourcei = nullptr;
    alSourceQueueBuffers = nullptr;
    alBufferData = nullptr;
    alGenBuffers = nullptr;
    alDeleteBuffers = nullptr;
    alSourceUnqueueBuffers = nullptr;
    alGetSourcei = nullptr;
    alSourceStop = nullptr;
    alSourcePlay = nullptr;
    alDeleteSources = nullptr;
    alGenSources = nullptr;
    alcOpenDevice = nullptr;
    alcCloseDevice = nullptr;
    alcCreateContext = nullptr;
    alcDestroyContext = nullptr;
    alcSuspendContext = nullptr;
    alcProcessContext = nullptr;
    alcMakeContextCurrent = nullptr;
    alcGetContextsDevice = nullptr;
    
    FMODImportsLoaded = false;
}
