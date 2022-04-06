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
#include "wstring.h"
#include "winutil.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <Windows.h>
#include <typeinfo>


#define OPENALSOFT_DLL_NAME "openal-soft.dll"


#define LOAD_DLL_IMPORT(dll, type, func) \
    func = (type)GetProcAddress(dll, #func); \
    if (!func) { \
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", #func, GetLastError()); \
        /* return false; */ \
    }
    


/**
 *  Loaded OpenAL DLL library.
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

    Wstring lib_name;

    lib_name = OPENALSOFT_DLL_NAME;

    /**
     *  Look for the OpenAL-Soft DLL.
     */
    if (GetFileAttributesA(lib_name.Peek_Buffer()) != INVALID_FILE_ATTRIBUTES) {
        OpenALDLL = LoadLibrary(lib_name.Peek_Buffer());
    }
    
    if (!OpenALDLL) {
        DEBUG_ERROR("Audio: Failed to load OpenAL library!\n");
        OpenALImportsLoaded = false;
        return false;
    }

    DEBUG_INFO("Audio: OpenAL library \"%s\" found.\n", lib_name.Peek_Buffer());

    DEV_DEBUG_INFO("Load_OpenAL_DLL()\n");

    LOAD_DLL_IMPORT(OpenALDLL, LPALENABLE, alEnable);
    LOAD_DLL_IMPORT(OpenALDLL, LPALDISABLE, alDisable);
    LOAD_DLL_IMPORT(OpenALDLL, LPALISENABLED, alIsEnabled);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETSTRING, alGetString);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETBOOLEANV, alGetBooleanv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETINTEGERV, alGetIntegerv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETFLOATV, alGetFloatv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETDOUBLEV, alGetDoublev);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETBOOLEAN, alGetBoolean);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETINTEGER, alGetInteger);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETFLOAT, alGetFloat);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETDOUBLE, alGetDouble);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETERROR, alGetError);
    LOAD_DLL_IMPORT(OpenALDLL, LPALISEXTENSIONPRESENT, alIsExtensionPresent);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETPROCADDRESS, alGetProcAddress);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETENUMVALUE, alGetEnumValue);
    LOAD_DLL_IMPORT(OpenALDLL, LPALLISTENERF, alListenerf);
    LOAD_DLL_IMPORT(OpenALDLL, LPALLISTENER3F, alListener3f);
    LOAD_DLL_IMPORT(OpenALDLL, LPALLISTENERFV, alListenerfv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALLISTENERI, alListeneri);
    LOAD_DLL_IMPORT(OpenALDLL, LPALLISTENER3I, alListener3i);
    LOAD_DLL_IMPORT(OpenALDLL, LPALLISTENERIV, alListeneriv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETLISTENERF, alGetListenerf);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETLISTENER3F, alGetListener3f);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETLISTENERFV, alGetListenerfv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETLISTENERI, alGetListeneri);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETLISTENER3I, alGetListener3i);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETLISTENERIV, alGetListeneriv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGENSOURCES, alGenSources);
    LOAD_DLL_IMPORT(OpenALDLL, LPALDELETESOURCES, alDeleteSources);
    LOAD_DLL_IMPORT(OpenALDLL, LPALISSOURCE, alIsSource);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEF, alSourcef);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCE3F, alSource3f);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEFV, alSourcefv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEI, alSourcei);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCE3I, alSource3i);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEIV, alSourceiv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETSOURCEF, alGetSourcef);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETSOURCE3F, alGetSource3f);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETSOURCEFV, alGetSourcefv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETSOURCEI, alGetSourcei);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETSOURCE3I, alGetSource3i);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETSOURCEIV, alGetSourceiv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEPLAYV, alSourcePlayv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCESTOPV, alSourceStopv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEREWINDV, alSourceRewindv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEPAUSEV, alSourcePausev);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEPLAY, alSourcePlay);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCESTOP, alSourceStop);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEREWIND, alSourceRewind);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEPAUSE, alSourcePause);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEQUEUEBUFFERS, alSourceQueueBuffers);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSOURCEUNQUEUEBUFFERS, alSourceUnqueueBuffers);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGENBUFFERS, alGenBuffers);
    LOAD_DLL_IMPORT(OpenALDLL, LPALDELETEBUFFERS, alDeleteBuffers);
    LOAD_DLL_IMPORT(OpenALDLL, LPALISBUFFER, alIsBuffer);
    LOAD_DLL_IMPORT(OpenALDLL, LPALBUFFERDATA, alBufferData);
    LOAD_DLL_IMPORT(OpenALDLL, LPALBUFFERF, alBufferf);
    LOAD_DLL_IMPORT(OpenALDLL, LPALBUFFER3F, alBuffer3f);
    LOAD_DLL_IMPORT(OpenALDLL, LPALBUFFERFV, alBufferfv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALBUFFERI, alBufferi);
    LOAD_DLL_IMPORT(OpenALDLL, LPALBUFFER3I, alBuffer3i);
    LOAD_DLL_IMPORT(OpenALDLL, LPALBUFFERIV, alBufferiv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETBUFFERF, alGetBufferf);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETBUFFER3F, alGetBuffer3f);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETBUFFERFV, alGetBufferfv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETBUFFERI, alGetBufferi);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETBUFFER3I, alGetBuffer3i);
    LOAD_DLL_IMPORT(OpenALDLL, LPALGETBUFFERIV, alGetBufferiv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALDOPPLERFACTOR, alDopplerFactor);
    LOAD_DLL_IMPORT(OpenALDLL, LPALDOPPLERVELOCITY, alDopplerVelocity);
    LOAD_DLL_IMPORT(OpenALDLL, LPALSPEEDOFSOUND, alSpeedOfSound);
    LOAD_DLL_IMPORT(OpenALDLL, LPALDISTANCEMODEL, alDistanceModel);

    LOAD_DLL_IMPORT(OpenALDLL, LPALCCREATECONTEXT, alcCreateContext);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCMAKECONTEXTCURRENT, alcMakeContextCurrent);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCPROCESSCONTEXT, alcProcessContext);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCSUSPENDCONTEXT, alcSuspendContext);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCDESTROYCONTEXT, alcDestroyContext);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCGETCURRENTCONTEXT, alcGetCurrentContext);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCGETCONTEXTSDEVICE, alcGetContextsDevice);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCOPENDEVICE, alcOpenDevice);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCCLOSEDEVICE, alcCloseDevice);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCGETERROR, alcGetError);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCISEXTENSIONPRESENT, alcIsExtensionPresent);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCGETPROCADDRESS, alcGetProcAddress);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCGETENUMVALUE, alcGetEnumValue);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCGETSTRING, alcGetString);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCGETINTEGERV, alcGetIntegerv);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCCAPTUREOPENDEVICE, alcCaptureOpenDevice);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCCAPTURECLOSEDEVICE, alcCaptureCloseDevice);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCCAPTURESTART, alcCaptureStart);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCCAPTURESTOP, alcCaptureStop);
    LOAD_DLL_IMPORT(OpenALDLL, LPALCCAPTURESAMPLES, alcCaptureSamples);

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
    DEV_DEBUG_INFO("Unload_OpenAL_DLL()\n");

    FreeLibrary(OpenALDLL);
    
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
    alListenerf = nullptr;
    
    OpenALImportsLoaded = false;
}
