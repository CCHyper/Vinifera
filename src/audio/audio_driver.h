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
typedef enum AudioPreloadType
{
    PRELOAD_SFX,
    PRELOAD_MUSIC,
} AudioPreloadType;


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
         *  Music playback.
         */
        virtual bool Play_Music(Wstring filename) = 0;
        virtual bool Pause_Music() = 0;
        virtual bool UnPause_Music() = 0;
        virtual bool Stop_Music() = 0;
        virtual void Fade_In_Music(int seconds = 1, float step = 0.10f) = 0;
        virtual void Fade_Out_Music(int seconds = 1, float step = 0.10f) = 0;
        virtual bool Is_Music_Playing() const = 0;
        virtual bool Is_Music_Paused() const = 0;
        virtual bool Is_Music_Fading_In() const = 0;
        virtual bool Is_Music_Fading_Out() const = 0;
        virtual void Set_Music_Volume(float volume) = 0;
        virtual float Get_Music_Volume() = 0;

        /**
         *  Sound asset preloading.
         */
        virtual bool Request_Preload(Wstring filename, AudioPreloadType type) = 0;
        virtual void Start_Preloader() = 0;

        virtual bool Is_Audio_File_Available(Wstring filename) = 0;

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
