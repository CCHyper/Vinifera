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
 *  @brief         Contains the hooks for the FMOD audio engine.
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
#include "fmod_globals.h"


bool FMODInitialised = false;

LPALSOURCEF alSourcef = nullptr;
LPALSOURCE3F alSource3f = nullptr;
LPALSOURCEI alSourcei = nullptr;
LPALSOURCEQUEUEBUFFERS alSourceQueueBuffers = nullptr;
LPALBUFFERDATA alBufferData = nullptr;
LPALGENBUFFERS alGenBuffers = nullptr;
LPALDELETEBUFFERS alDeleteBuffers = nullptr;
LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers = nullptr;
LPALGETSOURCEI alGetSourcei = nullptr;
LPALSOURCESTOP alSourceStop = nullptr;
LPALSOURCEPLAY alSourcePlay = nullptr;
LPALSOURCEPAUSE alSourcePause = nullptr;
LPALDELETESOURCES alDeleteSources = nullptr;
LPALGENSOURCES alGenSources = nullptr;
LPALCOPENDEVICE alcOpenDevice = nullptr;
LPALCCLOSEDEVICE alcCloseDevice = nullptr;
LPALCCREATECONTEXT alcCreateContext = nullptr;
LPALCDESTROYCONTEXT alcDestroyContext = nullptr;
LPALCSUSPENDCONTEXT alcSuspendContext = nullptr;
LPALCPROCESSCONTEXT alcProcessContext = nullptr;
LPALCMAKECONTEXTCURRENT alcMakeContextCurrent = nullptr;
LPALCGETCONTEXTSDEVICE alcGetContextsDevice = nullptr;
LPALCGETERROR alcGetError = nullptr;
