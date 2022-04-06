/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          NULL_DRIVER.H
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
#pragma once

#include "always.h"
#include "audio_driver.h"


/**
 *  
 */
class NullAudioDriver : public AudioDriver
{
    public:
        NullAudioDriver() : AudioDriver("Null") {}
        virtual ~NullAudioDriver() {}

        virtual bool Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels = false) override { return true; }
        virtual void End() override {}

        virtual bool Is_Available() const override { return true; }
        virtual bool Is_Enabled() const override { return true; }

        virtual void Enable() override {}
        virtual void Disable() override {}

        virtual void Stop_Handle_Playing(AudioHandle handle) override {}
        virtual void Stop_Sample_Playing(AudioSample sample) override {}

        virtual AudioHandle Play_Sample(AudioSample sample, int priority = PRIORITY_MAX, int volume = VOLUME_MAX) override { return INVALID_AUDIO_HANDLE; }
        virtual AudioHandle Play_File(Wstring &filename, int priority = PRIORITY_MAX, int volume = VOLUME_MAX) override { return INVALID_AUDIO_HANDLE; }
        virtual AudioHandle Stream_Sample(AudioSample sample, int volume, bool real_time_start = false) override { return INVALID_AUDIO_HANDLE; }
        virtual AudioHandle Stream_File(Wstring &filename, int volume, bool real_time_start = false) override { return INVALID_AUDIO_HANDLE; }

        virtual bool Pause_Handle(AudioHandle handle) override { return true; }
        virtual bool Resume_Handle(AudioHandle handle) override  { return true; }

        virtual void Fade_Sample(AudioHandle handle, int ticks = 60) override {}

        virtual bool Is_Handle_Playing(AudioHandle handle) const override { return false; }
        virtual bool Is_Sample_Playing(AudioSample sample) const override { return false; }
        virtual bool Is_Handle_Looping(AudioHandle handle) const override { return false; }

        virtual AudioHandle Get_Playing_Sample_Handle(AudioSample sample) const override { return INVALID_AUDIO_HANDLE; }

        virtual void Set_Handle_Volume(AudioHandle handle, int volume) override {}
        virtual void Set_Sample_Volume(AudioSample sample, int volume) override {}

        virtual int Set_Sound_Volume(int volume) override { return 0; }
        virtual int Set_Score_Volume(int volume) override { return 0; }

        virtual void Set_Volume_All(int volume) override {}
        virtual int Set_Volume_Percent_All(int vol_percent) override { return 0; }

        virtual AudioHandle Play_Sample_Handle(AudioSample sample, int priority, int volume, AudioHandle handle) override { return INVALID_AUDIO_HANDLE; }

        virtual bool Start_Primary_Sound_Buffer(bool forced = false) override { return true; }
        virtual void Stop_Primary_Sound_Buffer() override {}

        virtual void Sound_Callback() override {}
};
