/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DSOUND_DRIVER.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         
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
#include "dsound_driver.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "dsaudio.h"
#include "debughandler.h"
#include "asserthandler.h"


bool DirectSoundAudioDriver::Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels)
{
    DirectSoundAudio = &Audio;
    ASSERT(DirectSoundAudio != nullptr);

    /**
     *  DirectSound audio engine only supports "22050, 16, mono", so we ignore the requested arguments.
     */
    return DirectSoundAudio->Init(MainWindow, 16, false, 22050);
}

void DirectSoundAudioDriver::End()
{
    ASSERT(DirectSoundAudio != nullptr);

    DirectSoundAudio->End();
}

bool DirectSoundAudioDriver::Is_Available() const
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Is_Available();
}

bool DirectSoundAudioDriver::Is_Enabled() const
{
    ASSERT(DirectSoundAudio != nullptr);

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!\n", __FUNCTION__, DriverName.Peek_Buffer());

    return true;
}

void DirectSoundAudioDriver::Enable()
{
    ASSERT(DirectSoundAudio != nullptr);

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!\n", __FUNCTION__, DriverName.Peek_Buffer());
}

void DirectSoundAudioDriver::Disable()
{
    ASSERT(DirectSoundAudio != nullptr);

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!\n", __FUNCTION__, DriverName.Peek_Buffer());
}

void DirectSoundAudioDriver::Sound_Callback()
{
    ASSERT(DirectSoundAudio != nullptr);

    DirectSoundAudio->Sound_Callback();
}
