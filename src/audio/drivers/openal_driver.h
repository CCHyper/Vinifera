/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENAL_DRIVER.H
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
#include "vector.h"
#include "audio_driver.h"
#include "openal_util.h"


struct SampleTrackerStruct;


/**
 *  
 */
class OpenALAudioDriver : public AudioDriver
{
    public:
        OpenALAudioDriver();
        virtual ~OpenALAudioDriver();

        virtual bool Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels = false) override;
        virtual void End() override;

        virtual bool Is_Available() const override;
        virtual bool Is_Enabled() const override;

        virtual void Enable() override;
        virtual void Disable() override;

        virtual void Stop_Handle_Playing(AudioHandle handle) override;
        virtual void Stop_Sample_Playing(AudioSample sample) override;
        virtual bool Is_Handle_Playing(AudioHandle handle) const override;
        virtual bool Is_Sample_Playing(AudioSample sample) const override;

        virtual AudioHandle Play_Sample(AudioSample sample, int priority = PRIORITY_MAX, int volume = VOLUME_MAX) override;
        virtual AudioHandle Play_File(Wstring &filename, int priority = PRIORITY_MAX, int volume = VOLUME_MAX) override;
        virtual AudioHandle Stream_Sample(AudioSample sample, int volume, bool real_time_start = false) override;
        virtual AudioHandle Stream_File(Wstring &filename, int volume, bool real_time_start = false) override;

        virtual void Fade_Sample(AudioHandle handle, int ticks = 60) override;

        virtual AudioHandle Get_Playing_Sample_Handle(AudioSample sample) const override;

        virtual void Set_Handle_Volume(AudioHandle handle, int volume) override;
        virtual void Set_Sample_Volume(AudioSample sample, int volume) override;

        virtual int Set_Sound_Volume(int volume) override;
        virtual int Set_Score_Volume(int volume) override;

        virtual void Set_Volume_All(int volume) override;
        virtual int Set_Volume_Percent_All(int vol_percent) override;
        virtual AudioHandle Play_Sample_Handle(AudioSample sample, int priority, int volume, AudioHandle handle) override;

        virtual bool Start_Primary_Sound_Buffer(bool forced = false) override;
        virtual void Stop_Primary_Sound_Buffer() override;

        virtual void Sound_Callback() override;

    private:
        void Sound_Maintenance_Callback();
        bool File_Callback(short handle, short *odd, void **buffer, int *size);
        void File_Stream_Preload(AudioHandle handle);
        AudioHandle Get_Free_Sample_Handle(int priority);
        AudioHandle Stream_Sample_Callback(void *buffer, int size, bool (*callbackptr)(short, short *, void **, int *), int volume, AudioHandle handle);

    private:

        /**
         *  
         */
        bool IsInitialised;

        /**
         *  
         */
        bool IsEnabled;

        /**
         *  Should the left and right channels be swapped?
         */
        bool IsReverseChannels;

        /**
         *  The primary sound device.
         */
        ALCdevice *Device;
        
        /**
         *  
         */
        ALCcontext *Context;

        /**
         *  
         */
        ALfloat ListenerVolume;
        ALfloat ListenerPosition[3];

        /**
         *  
         */
        ALfloat SoundVolume;

        /**
         *  
         */
        ALfloat ScoreVolume;
        
        /**
         *  
         */
        DynamicVectorClass<SampleTrackerStruct *> SampleTracker;
};
