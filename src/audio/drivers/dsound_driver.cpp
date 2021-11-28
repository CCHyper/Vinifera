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

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());

    return true;
}

void DirectSoundAudioDriver::Enable()
{
    ASSERT(DirectSoundAudio != nullptr);

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());
}

void DirectSoundAudioDriver::Disable()
{
    ASSERT(DirectSoundAudio != nullptr);

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());
}

void DirectSoundAudioDriver::Stop_Handle_Playing(AudioHandle handle)
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Stop_Sample(handle);
}

void DirectSoundAudioDriver::Stop_Sample_Playing(AudioSample sample)
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Stop_Sample_Playing(sample);
}

bool DirectSoundAudioDriver::Is_Handle_Playing(AudioHandle handle) const
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Sample_Status(handle);
}

bool DirectSoundAudioDriver::Is_Sample_Playing(AudioSample sample) const
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Is_Sample_Playing(sample);
}

AudioHandle DirectSoundAudioDriver::Play_Sample(AudioSample sample, int priority, int volume)
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Play_Sample(sample, priority, volume);
}

AudioHandle DirectSoundAudioDriver::Play_File(Wstring &filename, int priority, int volume)
{
    ASSERT(DirectSoundAudio != nullptr);
    
    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());

    return INVALID_AUDIO_HANDLE;
}

AudioHandle DirectSoundAudioDriver::Stream_Sample(AudioSample sample, int volume, bool real_time_start)
{
    ASSERT(DirectSoundAudio != nullptr);

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());

    return INVALID_AUDIO_HANDLE;
}

AudioHandle DirectSoundAudioDriver::Stream_File(Wstring &filename, int volume, bool real_time_start)
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->File_Stream_Sample_Vol(filename.Peek_Buffer(), volume, real_time_start);
}

void DirectSoundAudioDriver::Fade_Sample(AudioHandle handle, int ticks)
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Fade_Sample(handle, ticks);
}

AudioHandle DirectSoundAudioDriver::Get_Playing_Sample_Handle(AudioSample sample) const
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Get_Playing_Sample_Handle(sample);
}

void DirectSoundAudioDriver::Set_Handle_Volume(AudioHandle handle, int volume)
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Set_Handle_Volume(handle, volume);
}

void DirectSoundAudioDriver::Set_Sample_Volume(AudioSample sample, int volume)
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Set_Sample_Volume(sample, volume);
}

int DirectSoundAudioDriver::Set_Sound_Volume(int volume)
{
    ASSERT(DirectSoundAudio != nullptr);

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());

    return 0;
}

int DirectSoundAudioDriver::Set_Score_Volume(int volume)
{
    ASSERT(DirectSoundAudio != nullptr);

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());

    return 0;
}

void DirectSoundAudioDriver::Set_Volume_All(int volume)
{
    ASSERT(DirectSoundAudio != nullptr);

    DirectSoundAudio->Set_Volume_All(volume);
}

int DirectSoundAudioDriver::Set_Volume_Percent_All(int vol_percent)
{
    ASSERT(DirectSoundAudio != nullptr);

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());

    return 0;
}

bool DirectSoundAudioDriver::Start_Primary_Sound_Buffer(bool forced)
{
    ASSERT(DirectSoundAudio != nullptr);

    return DirectSoundAudio->Start_Primary_Sound_Buffer(forced);
}

void DirectSoundAudioDriver::Stop_Primary_Sound_Buffer()
{
    ASSERT(DirectSoundAudio != nullptr);

    DirectSoundAudio->Stop_Primary_Sound_Buffer();
}

void DirectSoundAudioDriver::Sound_Callback()
{
    ASSERT(DirectSoundAudio != nullptr);

    DirectSoundAudio->Sound_Callback();
}


AudioHandle DirectSoundAudioDriver::Play_Sample_Handle(AudioSample sample, int priority, int volume, AudioHandle handle)
{
    ASSERT(DirectSoundAudio != nullptr);

    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());

    return INVALID_AUDIO_HANDLE;
}
