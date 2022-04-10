/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          XAUDIO2_DRIVER.H
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
#include "audio_source.h"
#include "audio_resource.h"
#include "XAudio2Versions.h"


/**
 *  
 */
class XAudio2AudioDriver : public AudioDriver
{
    public:
        XAudio2AudioDriver();
        virtual ~XAudio2AudioDriver();

        virtual bool Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels = false) override;
        virtual void End() override;

        virtual bool Is_Available() const override;
        virtual bool Is_Enabled() const override;

        virtual void Enable() override;
        virtual void Disable() override;

        virtual bool Resume(bool forced = false) override;
        virtual void Suspend() override;

        virtual void Sound_Callback() override;

        virtual AudioHandle Play_Sound_Effect(Wstring &filename, int priority = PRIORITY_MAX, int volume = VOLUME_MAX) override;
        virtual void Stop_Sound_Effect(AudioHandle handle) override;
        virtual bool Pause_Sound_Effect(AudioHandle handle) override;
        virtual bool Resume_Sound_Effect(AudioHandle handle) override;
        virtual void Fade_Sound_Effect(AudioHandle handle, int ticks = 60) override;
        virtual bool Is_Sound_Effect_Playing(AudioHandle handle) const override;
        virtual void Set_Sound_Effect_Volume(AudioHandle handle, float volume) override;
        virtual void Set_Sound_Effect_Pitch(AudioHandle handle, float pitch) override;
        virtual void Set_Sound_Effect_Pan(AudioHandle handle, float pan) override;
        virtual void Set_All_Sound_Effect_Volume(float volume) override;

        virtual AudioHandle Play_Music(Wstring &filename, int volume, bool real_time_start = false) override;
        virtual void Stop_Music(AudioHandle handle) override;
        virtual bool Pause_Music(AudioHandle handle) override;
        virtual bool Resume_Music(AudioHandle handle) override;
        virtual void Fade_Music(AudioHandle handle, int ticks = 60) override;
        virtual bool Is_Music_Playing(AudioHandle handle) const override;
        virtual void Set_Music_Volume(AudioHandle handle, float volume) override;

        virtual void Set_Master_Volume(float volume) override;

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
         *  
         */
        float SoundEffectVolume;

        /**
         *  
         */
        float MusicVolume;
        
        /**
         *  
         */
        ComPtr<IXAudio2> XAudio2;

        /**
         *  
         */
        IXAudio2MasteringVoice *MasterVoice; 
        
        /**
         *  
         */
        DynamicVectorClass<AudioSource *> SoundEffects;

        /**
         *  
         */
        AudioSource *MusicTrack;
};
