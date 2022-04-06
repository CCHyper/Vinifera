/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENAL_UTIL.CPP
 *
 *  @author        OmniBlade
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
#include "openal_util.h"
#include "openal_globals.h"
#include "dsaudio.h"
#include "wwaud.h"


/**
 *  x
 * 
 *  @author: OmniBlade
 */
ALenum Get_OpenAL_Format(int bits, int channels)
{
    switch (bits) {
        case 16:
            if (channels > 1) {
                return AL_FORMAT_STEREO16;
            } else {
                return AL_FORMAT_MONO16;
            }

        case 8:
            if (channels > 1) {
                return AL_FORMAT_STEREO8;
            } else {
                return AL_FORMAT_MONO8;
            }

        default:
            return -1;
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
const char *Get_OpenAL_Error(ALenum error)
{
    switch (error) {
        //case AL_NO_ERROR:
        //    return "AL_NO_ERROR: No error occurred.";
        case AL_INVALID_NAME:
            return "AL_INVALID_NAME: Invalid device name.";
        case AL_INVALID_ENUM:
            return "AL_INVALID_ENUM: Invalid Enum.";
        case AL_INVALID_VALUE:
            return "AL_INVALID_VALUE: Invalid value.";
        case AL_INVALID_OPERATION:
            return "AL_INVALID_OPERATION: Invalid operation.";
        case AL_OUT_OF_MEMORY:
            return "AL_OUT_OF_MEMORY: Out of memory.";
        default:
            return "Unknown error.";
    };
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
const char *Get_OpenAL_Context_Error(ALenum error)
{
    switch (error) {
        //case ALC_NO_ERROR:
        //    return "ALC_NO_ERROR: No error occurred.";
        case ALC_INVALID_DEVICE:
            return "ALC_INVALID_DEVICE: Invalid or no device selected.";
        case ALC_INVALID_CONTEXT:
            return "ALC_INVALID_CONTEXT: Invalid or no context selected";
        case ALC_INVALID_ENUM:
            return "ALC_INVALID_ENUM: Invalid enum value";
        case ALC_INVALID_VALUE:
            return "ALC_INVALID_VALUE: Invalid parameter value";
        case ALC_OUT_OF_MEMORY:
            return "ALC_OUT_OF_MEMORY: OpenAL ran out of memory";
        default:
            return "Unknown error.";
    };
}


/**
 *  Converts volume (0 - 255) to OpenAL volume (0.0 - 1.0)
 * 
 *  @author: CCHyper
 */
float Vol255_To_OpenAL_Volume(int volume)
{
    return ((float)volume) / 255.0f;
}


/**
 *  Converts OpenAL volume (0.0 - 1.0) to volume (0 - 255)
 * 
 *  @author: CCHyper
 */
int OpenAL_Volume_To_Vol255(float volume)
{
    return (std::clamp(volume, 0.0f, 1.0f) * 255);
}
