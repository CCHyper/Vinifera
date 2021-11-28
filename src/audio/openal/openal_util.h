/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENAL_UTIL.H
 *
 *  @author        CCHyper
 *
 *  @brief         Various OpenAL utility functions.
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
#include "always.h"
#include <al.h>
#include <alc.h>
#include "debughandler.h"


ALenum Get_OpenAL_Format(int bits, int channels);
const char *Get_OpenAL_Error(ALenum error);
const char *Get_OpenAL_Context_Error(ALenum error);


/**
 *  
 */
#define alCall(func_call) \
    { \
        alGetError(); /* Clear existing error code. */ \
        func_call ; \
        ALCenum alErrorCode; \
        if ((alErrorCode = alGetError()) != AL_NO_ERROR) { \
            DEBUG_ERROR("OpenAL: File: %s, Function: %s, Line: %d\n", __FILE__ __FUNCTION__, __LINE__); \
            DEBUG_ERROR("OpenAL: %s!\n", Get_OpenAL_Error(alErrorCode)); \
        } \
    }


/**
 *  
 */
#define alcCall(device, func_call) \
    { \
        alcGetError(device); /* Clear existing error code. */ \
        func_call ; \
        ALCenum alErrorCode; \
        if ((alErrorCode = alcGetError(device)) != AL_NO_ERROR) { \
            DEBUG_ERROR("OpenAL: File: %s, Function: %s, Line: %d\n", __FILE__ __FUNCTION__, __LINE__); \
            DEBUG_ERROR("OpenAL: %s!\n", Get_OpenAL_Error(alErrorCode)); \
        } \
    }


float Vol255_To_OpenAL_Volume(int volume);
int OpenAL_Volume_To_Vol255(float volume);
