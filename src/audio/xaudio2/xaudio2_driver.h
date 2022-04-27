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
#include "search.h"
#include <vector>


class XAudio2Stream;
class XAudio2SoundResource;






#include <xaudio2.h>
#include "debughandler.h"


// https://github.com/F5J2Jf/m/blob/d7f0c8347629879cbff14dec8b080b1cccf31668/frameworks/cocos2d-x/cocos/audio/winrt/Audio.h#L119




/**
 *  
 */
class XAudio2AudioDriver : public AudioDriver
{
    public:
        static IXAudio2 *Get_Audio_Engine() { return AudioEngine; }
        static IXAudio2MasteringVoice *Get_Master_Voice() { return MasterVoice; }

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


        // TODO querying the thread crashes!
        
        /**
         *  Sound effect i/o.
         */
        //virtual bool Play_SoundEffect(Wstring filename, float volume = 1.0f, float pan = 0.0f, bool one_shot = true, int loop_count = 1) override;
        //virtual bool Stop_SoundEffect(Wstring filename, bool force = true) override;
        //virtual bool Is_SoundEffect_Playing(Wstring filename) const override;
        virtual void Set_SoundEffect_Volume(float volume) override;
        virtual float Get_SoundEffect_Volume() override;

        /**
         *  Speech i/o.
         */
        //virtual bool Play_Speech(Wstring filename, float volume = 1.0f) override;
        //virtual bool Stop_Speech(Wstring filename, bool force = false) override;
        //virtual bool Is_Speech_Playing(Wstring filename) const override;
        virtual void Set_Speech_Volume(float volume) override;
        virtual float Get_Speech_Volume() override;

        /**
         *  Music i/o.
         */
        //virtual bool Play_Music(Wstring filename) override;
        virtual bool Pause_Music() override;
        virtual bool UnPause_Music() override;
       // virtual bool Stop_Music() override;
        virtual void Fade_In_Music(int seconds = 1, float step = 0.10f) override;
        virtual void Fade_Out_Music(int seconds = 1, float step = 0.10f) override;
        //virtual bool Is_Music_Playing() const override;
        //virtual bool Is_Music_Finished() const override;
        //virtual bool Is_Music_Paused() const override;
        //virtual bool Is_Music_Fading_In() const override;
        //virtual bool Is_Music_Fading_Out() const override;
        virtual void Set_Music_Volume(float volume) override;
        virtual float Get_Music_Volume() override;

        virtual bool Play(AudioStreamType type, Wstring filename, float volume = 1.0f) override;
        virtual bool Play_SoundEffect(Wstring filename, float volume = 1.0f, float pan = 0.0f, int loop_count = 1) override;
        virtual bool Stop(AudioStreamType type, Wstring filename, bool force = false, bool all = true) override;

        virtual bool Is_Playing(AudioStreamType type, Wstring filename) const override;
        virtual void Set_Volume(AudioStreamType type, Wstring filename, float volume) override;
        virtual float Get_Volume(AudioStreamType type, Wstring filename) override;

        /**
         *  Sound asset preloading.
         */
        virtual bool Request_Preload(Wstring filename, AudioSampleType type) override;
        virtual void Start_Preloader(AudioSampleType type) override;

        virtual void Clear_Sample_Bank(AudioSampleType type) override;
        //virtual void Clear_SoundEffect_Bank() override;
        //virtual void Clear_Speech_Bank() override;
        //virtual void Clear_Music_Bank() override;

        virtual bool Is_Audio_File_Available(Wstring filename) override;

        virtual void Debug_Dump_Sound_Banks() override;

    private:
        static void __cdecl Sound_Thread_Function();
        static void __cdecl Preload_Thread_Function();

        void Process_File_Preload_Requests();

        bool Handle_Requests();
        void Thread_Callback();

    private:
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
        float SoundEffectVolume;

        /**
         *  
         */
        float SpeechVolume;

        /**
         *  
         */
        float MusicVolume;

        /**
         *  
         */
        // /*std::atomic<XAudio2Stream *>*/ XAudio2Stream *SoundEffectStream;
        //std::vector<std::unique_ptr<XAudio2Stream>> SoundEffectStreams;

        /**
         *  
         */
        IndexClass<unsigned int, XAudio2SoundResource *> SoundEffectBankIndex;
        
        /**
         *  
         */
        int MaxSimultaneousSounds;

        /**
         *  
         */
        // /*std::atomic<XAudio2Stream *>*/ XAudio2Stream *SpeechStream;
        //std::vector<std::unique_ptr<XAudio2Stream>> SpeechStreams;

        /**
         *  
         */
        IndexClass<unsigned int, XAudio2SoundResource *> SpeechBankIndex;

        /**
         *  
         */
        // /*std::atomic<XAudio2Stream *>*/ XAudio2Stream *MusicStream;
        //std::vector<std::unique_ptr<XAudio2Stream>> MusicStreams;

        /**
         *  
         */
        IndexClass<unsigned int, XAudio2SoundResource *> MusicBankIndex;

        /**
         *  
         */
        //bool MusicStreamIsFinished;

        /**
         *  
         */
        std::vector<std::unique_ptr<XAudio2Stream>> Streams;

    private:
        /**
         *  
         */
        static IXAudio2 *AudioEngine;

        /**
         *  
         */
        static IXAudio2MasteringVoice *MasterVoice;
};
