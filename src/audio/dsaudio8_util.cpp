/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          DSAUDIO8_UTIL.CPP
 *
 *  @author        CCHyper
 *
 *  @contributors  tomsons26
 *
 *  @brief         Main header file for the Direct Sound audio interface.
 *
 *  @license       ProjectTSYR is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 ProjectTSYR is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "dsaudio8_util.h"
#include "dsaudio8.h"
#include "dsaudio.h" // TibSun audio engine.


/**
 *  Displays a message box containing the given formatted string.
 */
void DS_Message_Box(HWND hWnd, HRESULT result, char const *message)
{
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "%s\n%s\n\nProceeding with sound disabled.", message, Process_DS_Result(result));
    std::strcat(buffer, "\r\n");
    MessageBox(hWnd, buffer, "DirectSound Error", MB_ICONEXCLAMATION | MB_OK);
}


/**
 *  Returns a pointer to a string describing the given DS error code.
 */
char const *Process_DS_Result(HRESULT result, bool show_msg_box, bool display_ok_msg)
{
    static char buffer[256];
    char const *errorstr = nullptr;

    switch (result) {
        case DSERR_ALLOCATED:
            errorstr = "The call failed because resources (such as a priority level) were already being used by another caller";
            break;

        case DSERR_CONTROLUNAVAIL:
            errorstr = "The control (vol, pan, etc.) requested by the caller is not available";
            break;

        case DSERR_INVALIDPARAM:
            errorstr = "Invalid parameter";
            break;

        case DSERR_INVALIDCALL:
            errorstr = "This call is not valid for the current state of this object";
            break;

        case DSERR_GENERIC:
            errorstr = "An undetermined error occurred inside the DirectSound subsystem";
            break;

        case DSERR_PRIOLEVELNEEDED:
            errorstr = "The caller does not have the priority level required for the function to succeed";
            break;

        case DSERR_OUTOFMEMORY:
            errorstr = "Not enough free memory is available to complete the operation";
            break;

        case DSERR_BADFORMAT:
            errorstr = "The sample rate or the channel format of the specified WAVE format is not supported";
            break;

        case DSERR_UNSUPPORTED:
            errorstr = "The function called is not supported at this time";
            break;

        case DSERR_NODRIVER:
            errorstr = "No sound driver is available for use";
            break;

        case DSERR_ALREADYINITIALIZED:
            errorstr = "This object is already initialized";
            break;

        case DSERR_NOAGGREGATION:
            errorstr = "This object does not support aggregation";
            break;

        case DSERR_BUFFERLOST:
            errorstr = "The buffer memory has been lost, and must be restored";
            break;

        case DSERR_OTHERAPPHASPRIO:
            errorstr = "Another app has a higher priority level, preventing this call from succeeding";
            break;

        case DSERR_UNINITIALIZED:
            errorstr = "This object has not been initialized";
            break;

        case DSERR_NOINTERFACE:
            errorstr = "The requested COM interface is not available";
            break;

        case DSERR_ACCESSDENIED:
            errorstr = "Access is denied";
            break;

        case DSERR_BUFFERTOOSMALL:
            errorstr = "Buffer too small";
            break;

        case DSERR_DS8_REQUIRED:
            errorstr = "Attempt to use DirectSound 8 functionality on an older DirectSound object";
            break;

        case DSERR_SENDLOOP:
            errorstr = "A circular loop of send effects was detected";
            break;

        case DSERR_BADSENDBUFFERGUID:
            errorstr = "The GUID specified in an audiopath file does not match a valid MIXIN buffer";
            break;

        case DSERR_OBJECTNOTFOUND:
            errorstr = "The object requested was not found";
            break;

        case DSERR_FXUNAVAILABLE:
            errorstr = "Requested effects are not available";
            break;

        case DS_OK:
            if (display_ok_msg) {
                errorstr = "Direct Sound request went ok.";
            }
            break;

        default:
            errorstr = "Unrecognized error value.";
            break;
    }

    if (errorstr) {
        if (show_msg_box) {
            DS_Message_Box(nullptr, result, errorstr);
        }

        std::snprintf(buffer, sizeof(buffer), errorstr);
        return buffer;
    }

    return nullptr;
}


/**
 *  Convert linear volume to DirectSound db scale.
 * 
 *  DirectSound controls volume using units of 100th of a decibel,
 *  ranging from -10000 to 0. We use a linear scale of 0 - 255
 *  here, so we need to convert it.
 */
int Convert_HMI_To_Direct_Sound_Volume(int volume)
{
    if (volume <= 0) {
        return DSBVOLUME_MIN;
    }

    if (volume > 255) {
        return DSBVOLUME_MAX;
    }

    float v = float(log10f(volume / (float)(255)) * (1000.0 / 30)) * 100;
    return std::clamp<float>(v, DSBVOLUME_MIN, DSBVOLUME_MAX);
}


int Convert_Direct_Sound_Volume_To_HMI(int volume)
{
    if (volume == DSBVOLUME_MIN) {
        return 0;
    }

    if (volume > DSBVOLUME_MAX) {
        return 255;
    }

    volume = 0;// (int)(powf(10.0f, (float)volume / 2000.0f) * (float)max) + 0.5f;
    return std::clamp<int>(volume, 0, 255);
}
