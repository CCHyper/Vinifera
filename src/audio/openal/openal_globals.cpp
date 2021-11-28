/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TACTICALEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the OpenAL audio engine.
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
#include "openal_globals.h"


bool OpenALInitialised = false;

LPALENABLE alEnable = nullptr;
LPALDISABLE alDisable = nullptr;
LPALISENABLED alIsEnabled = nullptr;
LPALGETSTRING alGetString = nullptr;
LPALGETBOOLEANV alGetBooleanv = nullptr;
LPALGETINTEGERV alGetIntegerv = nullptr;
LPALGETFLOATV alGetFloatv = nullptr;
LPALGETDOUBLEV alGetDoublev = nullptr;
LPALGETBOOLEAN alGetBoolean = nullptr;
LPALGETINTEGER alGetInteger = nullptr;
LPALGETFLOAT alGetFloat = nullptr;
LPALGETDOUBLE alGetDouble = nullptr;
LPALGETERROR alGetError = nullptr;
LPALISEXTENSIONPRESENT alIsExtensionPresent = nullptr;
LPALGETPROCADDRESS alGetProcAddress = nullptr;
LPALGETENUMVALUE alGetEnumValue = nullptr;
LPALLISTENERF alListenerf = nullptr;
LPALLISTENER3F alListener3f = nullptr;
LPALLISTENERFV alListenerfv = nullptr;
LPALLISTENERI alListeneri = nullptr;
LPALLISTENER3I alListener3i = nullptr;
LPALLISTENERIV alListeneriv = nullptr;
LPALGETLISTENERF alGetListenerf = nullptr;
LPALGETLISTENER3F alGetListener3f = nullptr;
LPALGETLISTENERFV alGetListenerfv = nullptr;
LPALGETLISTENERI alGetListeneri = nullptr;
LPALGETLISTENER3I alGetListener3i = nullptr;
LPALGETLISTENERIV alGetListeneriv = nullptr;
LPALGENSOURCES alGenSources = nullptr;
LPALDELETESOURCES alDeleteSources = nullptr;
LPALISSOURCE alIsSource = nullptr;
LPALSOURCEF alSourcef = nullptr;
LPALSOURCE3F alSource3f = nullptr;
LPALSOURCEFV alSourcefv = nullptr;
LPALSOURCEI alSourcei = nullptr;
LPALSOURCE3I alSource3i = nullptr;
LPALSOURCEIV alSourceiv = nullptr;
LPALGETSOURCEF alGetSourcef = nullptr;
LPALGETSOURCE3F alGetSource3f = nullptr;
LPALGETSOURCEFV alGetSourcefv = nullptr;
LPALGETSOURCEI alGetSourcei = nullptr;
LPALGETSOURCE3I alGetSource3i = nullptr;
LPALGETSOURCEIV alGetSourceiv = nullptr;
LPALSOURCEPLAYV alSourcePlayv = nullptr;
LPALSOURCESTOPV alSourceStopv = nullptr;
LPALSOURCEREWINDV alSourceRewindv = nullptr;
LPALSOURCEPAUSEV alSourcePausev = nullptr;
LPALSOURCEPLAY alSourcePlay = nullptr;
LPALSOURCESTOP alSourceStop = nullptr;
LPALSOURCEREWIND alSourceRewind = nullptr;
LPALSOURCEPAUSE alSourcePause = nullptr;
LPALSOURCEQUEUEBUFFERS alSourceQueueBuffers = nullptr;
LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers = nullptr;
LPALGENBUFFERS alGenBuffers = nullptr;
LPALDELETEBUFFERS alDeleteBuffers = nullptr;
LPALISBUFFER alIsBuffer = nullptr;
LPALBUFFERDATA alBufferData = nullptr;
LPALBUFFERF alBufferf = nullptr;
LPALBUFFER3F alBuffer3f = nullptr;
LPALBUFFERFV alBufferfv = nullptr;
LPALBUFFERI alBufferi = nullptr;
LPALBUFFER3I alBuffer3i = nullptr;
LPALBUFFERIV alBufferiv = nullptr;
LPALGETBUFFERF alGetBufferf = nullptr;
LPALGETBUFFER3F alGetBuffer3f = nullptr;
LPALGETBUFFERFV alGetBufferfv = nullptr;
LPALGETBUFFERI alGetBufferi = nullptr;
LPALGETBUFFER3I alGetBuffer3i = nullptr;
LPALGETBUFFERIV alGetBufferiv = nullptr;
LPALDOPPLERFACTOR alDopplerFactor = nullptr;
LPALDOPPLERVELOCITY alDopplerVelocity = nullptr;
LPALSPEEDOFSOUND alSpeedOfSound = nullptr;
LPALDISTANCEMODEL alDistanceModel = nullptr;

LPALCCREATECONTEXT alcCreateContext = nullptr;
LPALCMAKECONTEXTCURRENT alcMakeContextCurrent = nullptr;
LPALCPROCESSCONTEXT alcProcessContext = nullptr;
LPALCSUSPENDCONTEXT alcSuspendContext = nullptr;
LPALCDESTROYCONTEXT alcDestroyContext = nullptr;
LPALCGETCURRENTCONTEXT alcGetCurrentContext = nullptr;
LPALCGETCONTEXTSDEVICE alcGetContextsDevice = nullptr;
LPALCOPENDEVICE alcOpenDevice = nullptr;
LPALCCLOSEDEVICE alcCloseDevice = nullptr;
LPALCGETERROR alcGetError = nullptr;
LPALCISEXTENSIONPRESENT alcIsExtensionPresent = nullptr;
LPALCGETPROCADDRESS alcGetProcAddress = nullptr;
LPALCGETENUMVALUE alcGetEnumValue = nullptr;
LPALCGETSTRING alcGetString = nullptr;
LPALCGETINTEGERV alcGetIntegerv = nullptr;
LPALCCAPTUREOPENDEVICE alcCaptureOpenDevice = nullptr;
LPALCCAPTURECLOSEDEVICE alcCaptureCloseDevice = nullptr;
LPALCCAPTURESTART alcCaptureStart = nullptr;
LPALCCAPTURESTOP alcCaptureStop = nullptr;
LPALCCAPTURESAMPLES alcCaptureSamples = nullptr;
