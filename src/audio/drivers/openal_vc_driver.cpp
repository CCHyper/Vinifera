/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENAL_DRIVER.CPP
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
#include "openal_vc_driver.h"
#include "openal_audio.h"
#include "tibsun_globals.h"
#include "debughandler.h"
#include "asserthandler.h"


static OpenALAudioClass OpenALAudio;


OpenAL_VC_AudioDriver::OpenAL_VC_AudioDriver() :
    AudioDriver("OpenAL_VC"),
    Instance(&OpenALAudio)
{
}

OpenAL_VC_AudioDriver::~OpenAL_VC_AudioDriver()
{
}

bool OpenAL_VC_AudioDriver::Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels)
{
    return Instance->Init(MainWindow, 16, false, 22050);
}

void OpenAL_VC_AudioDriver::End()
{
    Instance->End();
}

bool OpenAL_VC_AudioDriver::Is_Available() const
{
    return Instance->Is_Available();
}

bool OpenAL_VC_AudioDriver::Is_Enabled() const
{
    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());

    return true;
}

void OpenAL_VC_AudioDriver::Enable()
{
    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());
}

void OpenAL_VC_AudioDriver::Disable()
{
    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());
}

void OpenAL_VC_AudioDriver::Stop_Handle_Playing(AudioHandle handle)
{
    Instance->Stop_Sample(handle);
}

bool OpenAL_VC_AudioDriver::Is_Handle_Playing(AudioHandle handle) const
{
    return Instance->Sample_Status(handle);
}

bool OpenAL_VC_AudioDriver::Is_Sample_Playing(AudioSample sample) const
{
    return Instance->Is_Sample_Playing(sample);
}

void OpenAL_VC_AudioDriver::Stop_Sample_Playing(AudioSample sample)
{
    Instance->Stop_Sample_Playing(sample);
}

AudioHandle OpenAL_VC_AudioDriver::Play_Sample(AudioSample sample, int priority, int volume)
{
    return Instance->Play_Sample(sample, priority, volume);
}

AudioHandle OpenAL_VC_AudioDriver::Play_File(Wstring &filename, int priority, int volume)
{
    return Instance->File_Play_Sample(filename.Peek_Buffer(), priority, volume);
}

AudioHandle OpenAL_VC_AudioDriver::Stream_Sample(AudioSample sample, int volume, bool real_time_start)
{
    DEBUG_WARNING("\"%s\" not implemented for \"%s\"!", __FUNCTION__, DriverName.Peek_Buffer());

    return INVALID_AUDIO_HANDLE;
}

AudioHandle OpenAL_VC_AudioDriver::Stream_File(Wstring &filename, int volume, bool real_time_start)
{
    return Instance->File_Stream_Sample_Vol(filename.Peek_Buffer(), volume, real_time_start);
}

void OpenAL_VC_AudioDriver::Fade_Sample(AudioHandle handle, int ticks)
{
    Instance->Fade_Sample(handle, ticks);
}

AudioHandle OpenAL_VC_AudioDriver::Get_Playing_Sample_Handle(AudioSample sample) const
{
    return Instance->Get_Playing_Sample_Handle(sample);
}

void OpenAL_VC_AudioDriver::Set_Handle_Volume(AudioHandle handle, int volume)
{
    Instance->Set_Handle_Volume(handle, volume);
}

void OpenAL_VC_AudioDriver::Set_Sample_Volume(AudioSample sample, int volume)
{
    Instance->Set_Sample_Volume(sample, volume);
}

int OpenAL_VC_AudioDriver::Set_Sound_Volume(int volume)
{
    return Instance->Set_Sound_Vol(volume);
}

int OpenAL_VC_AudioDriver::Set_Score_Volume(int volume)
{
    return Instance->Set_Score_Vol(volume);
}

void OpenAL_VC_AudioDriver::Set_Volume_All(int volume)
{
    Instance->Set_Volume_All(volume);
}

int OpenAL_VC_AudioDriver::Set_Volume_Percent_All(int vol_percent)
{
    return Instance->Set_Volume_Percent_All(vol_percent);
}

bool OpenAL_VC_AudioDriver::Start_Primary_Sound_Buffer(bool forced)
{
    return Instance->Start_Primary_Sound_Buffer(forced);
}

void OpenAL_VC_AudioDriver::Stop_Primary_Sound_Buffer()
{
    Instance->Stop_Primary_Sound_Buffer();
}

void OpenAL_VC_AudioDriver::Sound_Callback()
{
    Instance->Sound_Callback();
}

AudioHandle OpenAL_VC_AudioDriver::Play_Sample_Handle(AudioSample sample, int priority, int volume, AudioHandle handle)
{
    return Instance->Play_Sample_Handle(sample, priority, volume, handle);
}
