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
 *  @brief         Installable XAudio2 audio driver.
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
#include "vector.h"
#include "search.h"


class XAudio2Stream;
class XAudio2SoundResource;


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

        virtual bool Start_Engine(bool forced = false) override;
        virtual void Stop_Engine() override;

        virtual void Focus_Loss() override;
        virtual void Focus_Restore() override;

        virtual void Sound_Callback() override;

        /**
         *  Music playback.
         */
        virtual bool Play_Music(Wstring filename) override;
        virtual bool Pause_Music() override;
        virtual bool UnPause_Music() override;
        virtual bool Stop_Music() override;
        virtual void Fade_In_Music(int seconds = 1, float step = 0.10f) override;
        virtual void Fade_Out_Music(int seconds = 1, float step = 0.10f) override;
        virtual bool Is_Music_Playing() const override;
        virtual bool Is_Music_Paused() const override;
        virtual bool Is_Music_Fading_In() const override;
        virtual bool Is_Music_Fading_Out() const override;
        virtual void Set_Music_Volume(float volume) override;
        virtual float Get_Music_Volume() override;

        /**
         *  Sound asset preloading.
         */
        virtual bool Request_Preload(Wstring filename, AudioPreloadType type) override;
        virtual void Start_Preloader() override;

        virtual bool Is_Audio_File_Available(Wstring filename) override;

        void Process_Job_Requests();

        void Process_File_Preload();

        bool Handle_Sound_Request();
        void Thread_Sound_Callback();

        bool Handle_Music_Request();
        void Thread_Music_Callback();

    public: //protected:
        /**
         *  
         */
        bool IsAvailable;

        /**
         *  
         */
        bool IsEnabled;
        
        /**
         *  
         */
        float MasterVolume;
        
        /**
         *  
         */
        float SoundVolume;

        /**
         *  
         */
        float MusicVolume;

        VectorClass<XAudio2Stream *> Sounds;
        XAudio2Stream *Music;

        IndexClass<unsigned int, XAudio2SoundResource *> SoundBankIndex;
        IndexClass<unsigned int, XAudio2SoundResource *> MusicBankIndex;
};
