/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENAL_LOAD_DLL.H
 *
 *  @author        CCHyper
 *
 *  @brief         Utility functions for performing one-time loading of
 *                 OpenAL library functions from the DLL.
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
#include "openal_load_dll.h"
#include "openal_globals.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <Windows.h>


/**
 *  Loaded Bink DLL library.
 */
static HMODULE OpenALDLL = nullptr;


/**
 *  We successfully loaded all imports?
 */
bool OpenALImportsLoaded = false;


/**
 *  Load the OpenAL DLL and any imports we need.
 * 
 *  @author: CCHyper
 */
bool Load_OpenAL_DLL()
{
    /**
     *  We already performed a successful one-time init, return success.
     */
    if (OpenALImportsLoaded) {
        return true;
    }
    
    DEBUG_INFO("Load_OpenAL_DLL()\n");
    
    /**
     *  Look for the OpenAL DLL.
     */
    if (OpenALDLL == nullptr && GetFileAttributesA("soft_oal.dll") != INVALID_FILE_ATTRIBUTES) {
        OpenALDLL = LoadLibrary("soft_oal.dll");
    }
    if (OpenALDLL == nullptr && GetFileAttributesA("openal.dll") != INVALID_FILE_ATTRIBUTES) {
        OpenALDLL = LoadLibrary("openal.dll");
    }
    
    if (OpenALDLL == nullptr) {
        DEBUG_INFO("LoadLibraryA() failed with %d.\n", GetLastError());
        OpenALImportsLoaded = false;
        return false;
    }
    
    alSourcef = (LPALSOURCEF)GetProcAddress(OpenALDLL, "alSourcef");
    if (!alSourcef) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourcef", GetLastError());
        return false;
    }
    
    alSourceQueueBuffers = (LPALSOURCEQUEUEBUFFERS)GetProcAddress(OpenALDLL, "alSourceQueueBuffers");
    if (!alSourceQueueBuffers) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourceQueueBuffers", GetLastError());
        return false;
    }
    
    alBufferData = (LPALBUFFERDATA)GetProcAddress(OpenALDLL, "alBufferData");
    if (!alBufferData) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alBufferData", GetLastError());
        return false;
    }
    
    alGenBuffers = (LPALGENBUFFERS)GetProcAddress(OpenALDLL, "alGenBuffers");
    if (!alGenBuffers) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alGenBuffers", GetLastError());
        return false;
    }
    
    alDeleteBuffers = (LPALDELETEBUFFERS)GetProcAddress(OpenALDLL, "alDeleteBuffers");
    if (!alDeleteBuffers) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alDeleteBuffers", GetLastError());
        return false;
    }
    
    alSourceUnqueueBuffers = (LPALSOURCEUNQUEUEBUFFERS)GetProcAddress(OpenALDLL, "alSourceUnqueueBuffers");
    if (!alSourceUnqueueBuffers) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourceUnqueueBuffers", GetLastError());
        return false;
    }
    
    alGetSourcei = (LPALGETSOURCEI)GetProcAddress(OpenALDLL, "alGetSourcei");
    if (!alGetSourcei) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alGetSourcei", GetLastError());
        return false;
    }
    
    alSourceStop = (LPALSOURCESTOP)GetProcAddress(OpenALDLL, "alSourceStop");
    if (!alSourceStop) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourceStop", GetLastError());
        return false;
    }
    
    alSourcePlay = (LPALSOURCEPLAY)GetProcAddress(OpenALDLL, "alSourcePlay");
    if (!alSourcePlay) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alSourcePlay", GetLastError());
        return false;
    }
    
    alDeleteSources = (LPALDELETESOURCES)GetProcAddress(OpenALDLL, "alDeleteSources");
    if (!alDeleteSources) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alDeleteSources", GetLastError());
        return false;
    }
    
    alGenSources = (LPALGENSOURCES)GetProcAddress(OpenALDLL, "alGenSources");
    if (!alGenSources) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alGenSources", GetLastError());
        return false;
    }
    
    alcOpenDevice = (LPALCOPENDEVICE)GetProcAddress(OpenALDLL, "alcOpenDevice");
    if (!alcOpenDevice) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcOpenDevice", GetLastError());
        return false;
    }
    
    alcCloseDevice = (LPALCCLOSEDEVICE)GetProcAddress(OpenALDLL, "alcCloseDevice");
    if (!alcCloseDevice) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcCloseDevice", GetLastError());
        return false;
    }
    
    alcCreateContext = (LPALCCREATECONTEXT)GetProcAddress(OpenALDLL, "alcCreateContext");
    if (!alcCreateContext) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcCreateContext", GetLastError());
        return false;
    }
    
    alcDestroyContext = (LPALCDESTROYCONTEXT)GetProcAddress(OpenALDLL, "alcDestroyContext");
    if (!alcDestroyContext) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcDestroyContext", GetLastError());
        return false;
    }
    
    alcSuspendContext = (LPALCSUSPENDCONTEXT)GetProcAddress(OpenALDLL, "alcSuspendContext");
    if (!alcSuspendContext) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcSuspendContext", GetLastError());
        return false;
    }
    
    alcProcessContext = (LPALCPROCESSCONTEXT)GetProcAddress(OpenALDLL, "alcProcessContext");
    if (!alcProcessContext) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcProcessContext", GetLastError());
        return false;
    }
    
    alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)GetProcAddress(OpenALDLL, "alcMakeContextCurrent");
    if (!alcMakeContextCurrent) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcMakeContextCurrent", GetLastError());
        return false;
    }
    
    alcGetContextsDevice = (LPALCGETCONTEXTSDEVICE)GetProcAddress(OpenALDLL, "alcGetContextsDevice");
    if (!alcGetContextsDevice) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcGetContextsDevice", GetLastError());
        return false;
    }

    alcGetError = (LPALCGETERROR)GetProcAddress(OpenALDLL, "alcGetError");
    if (!alcGetError) {
        DEBUG_INFO("GetProcAddress failed to load %s (error: %d).\n", "alcGetError", GetLastError());
        return false;
    }
    
    /**
     *  All loaded and ready to go.
     */
    OpenALImportsLoaded = true;
    
    return true;
}


/**
 *  Free the OpenAL library and all loaded pointers.
 * 
 *  @author: CCHyper
 */
void Unload_OpenAL_DLL()
{
    DEBUG_INFO("Unload_OpenAL_DLL()\n");

    FreeLibrary(OpenALDLL);
    
    alSourcef = nullptr;
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
    
    OpenALImportsLoaded = false;
}
