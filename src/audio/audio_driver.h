/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_DRIVER.H
 *
 *  @author        CCHyper
 *
 *  @brief         Base class for all installable audio device drivers.
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
#include "wstring.h"
#include "dsaudio.h"


/**
 *  
 */
typedef const int AudioHandle;
typedef const void * AudioSample;


/**
 *  
 */
typedef enum AudioSampleType
{
    SAMPLE_NONE,

    SAMPLE_SFX,
    SAMPLE_SPEECH,
    SAMPLE_MUSIC,
} AudioSampleType;

/**
 *  
 */
typedef enum AudioStreamType
{
    STREAM_NONE,

    STREAM_SFX,
    STREAM_SPEECH,
    STREAM_MUSIC,
} AudioStreamType;


/**
 *  
 */
class AudioDriver
{
    public:
        AudioDriver(Wstring driver_name) : DriverName(driver_name), InFocus(false) {}
        virtual ~AudioDriver() {}

        virtual Wstring Get_Name() const { return DriverName; }

        virtual bool Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels = false) = 0;
        virtual void End() = 0;

        virtual bool Is_Available() const = 0;
        virtual bool Is_Enabled() const = 0;

        virtual void Enable() = 0;
        virtual void Disable() = 0;

        virtual bool Start_Engine(bool forced = false) = 0;
        virtual void Stop_Engine() = 0;

        virtual void Focus_Loss() = 0;
        virtual void Focus_Restore() = 0;
        virtual bool In_Focus() const { return InFocus; }

        virtual void Sound_Callback() = 0;

        /**
         *  Sound effect i/o.
         */
        //virtual bool Play_SoundEffect(Wstring filename, float volume = 1.0f, float pan = 0.0f, bool one_shot = true, int loop_count = 1) = 0;
        //virtual bool Stop_SoundEffect(Wstring filename, bool force = true) = 0;
        //virtual bool Is_SoundEffect_Playing(Wstring filename) const = 0;
        virtual void Set_SoundEffect_Volume(float volume) = 0;
        virtual float Get_SoundEffect_Volume() = 0;

        /**
         *  Speech i/o.
         */
        //virtual bool Play_Speech(Wstring filename, float volume = 1.0f) = 0;
        //virtual bool Stop_Speech(Wstring filename, bool force = false) = 0;
        //virtual bool Is_Speech_Playing(Wstring filename) const = 0;
        virtual void Set_Speech_Volume(float volume) = 0;
        virtual float Get_Speech_Volume() = 0;

        /**
         *  Music i/o.
         */
        //virtual bool Play_Music(Wstring filename) = 0;
        virtual bool Pause_Music() = 0;
        virtual bool UnPause_Music() = 0;
        //virtual bool Stop_Music() = 0;
        virtual void Fade_In_Music(int seconds = 1, float step = 0.10f) = 0;
        virtual void Fade_Out_Music(int seconds = 1, float step = 0.10f) = 0;
        //virtual bool Is_Music_Playing() const = 0;
        //virtual bool Is_Music_Finished() const = 0;
        //virtual bool Is_Music_Paused() const = 0;
        //virtual bool Is_Music_Fading_In() const = 0;
        //virtual bool Is_Music_Fading_Out() const = 0;
        virtual void Set_Music_Volume(float volume) = 0;
        virtual float Get_Music_Volume() = 0;

        virtual bool Play(AudioStreamType type, Wstring filename, float volume = 1.0f) = 0;
        virtual bool Play_SoundEffect(Wstring filename, float volume = 1.0f, float pan = 0.0f, int loop_count = 1) = 0;
        virtual bool Stop(AudioStreamType type, Wstring filename, bool force = false, bool all = true) = 0;

        virtual bool Is_Playing(AudioStreamType type, Wstring filename) const = 0;
        virtual void Set_Volume(AudioStreamType type, Wstring filename, float volume) = 0;
        virtual float Get_Volume(AudioStreamType type, Wstring filename) = 0;

        /**
         *  Sound asset preloading.
         */
        virtual bool Request_Preload(Wstring filename, AudioSampleType type) = 0;
        virtual void Start_Preloader(AudioSampleType type) = 0;

        virtual void Clear_Sample_Bank(AudioSampleType type) = 0;
        //virtual void Clear_SoundEffect_Bank() = 0;
        //virtual void Clear_Speech_Bank() = 0;
        //virtual void Clear_Music_Bank() = 0;

        virtual bool Is_Audio_File_Available(Wstring filename) = 0;

        virtual void Debug_Dump_Sound_Banks() {}

    protected:
        /**
         *  x
         */
        Wstring DriverName;

        /**
         *  
         */
        bool InFocus;
};


void __cdecl Install_Audio_Driver(AudioDriver *driver);
void __cdecl Uninstall_Audio_Driver();

AudioDriver * __cdecl Audio_Driver();
