/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FMOD_UTIL.CPP
 *
 *  @author        OmniBlade
 *
 *  @brief         Various FMOD utility functions.
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
#include "fmod_util.h"



/**
 *  x
 * 
 *  @author: OmniBlade
 */
ALenum Get_FMOD_Format(int bits, int channels)
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
 *  @author: OmniBlade
 */
const char *Get_FMOD_Error(ALenum error)
{
    switch (error) {
        case AL_NO_ERROR:
            return "No error occurred.";

        case AL_INVALID_NAME:
            return "Invalid device name.";

        case AL_INVALID_ENUM:
            return "Invalid Enum.";

        case AL_INVALID_VALUE:
            return "Invalid value.";

        case AL_INVALID_OPERATION:
            return "Invalid operation.";

        case AL_OUT_OF_MEMORY:
            return "Out of memory.";

        default:
            break;
    }

    return "Unknown error.";
}
