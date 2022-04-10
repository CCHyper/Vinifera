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
 *  @brief         Contains the hooks for the new audio engine.
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
 *  The handle value for the original audio engine is a internal index
 *  of the tracker that is playing the sound. For all new drivers, this
 *  should be a unique value, such as a crc hash of the filename.
 */
typedef const uint32_t AudioHandle;

typedef const void * AudioSample;


/**
 *  
 */
class AudioDriver
{
    public:
        AudioDriver(Wstring driver_name) :
            DriverName(driver_name)
        {
        }
        virtual ~AudioDriver() {}

        virtual const Wstring &Get_Name() final { return DriverName; }

        virtual bool Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels = false) = 0;
        virtual void End() = 0;

        virtual bool Is_Available() const = 0;
        virtual bool Is_Enabled() const = 0;

        virtual void Enable() = 0;
        virtual void Disable() = 0;

        virtual bool Resume(bool forced = false) = 0;
        virtual void Suspend() = 0;

        virtual void Focus_Loss() { if (FocusLossFunction) { FocusLossFunction(); } }

        virtual void Sound_Callback() = 0;

        /**
         *  Sound Effects
         */
        virtual AudioHandle Play_Sound_Effect(Wstring &filename, int priority = PRIORITY_MAX, int volume = VOLUME_MAX) = 0;
        virtual void Stop_Sound_Effect(AudioHandle handle) = 0;
        virtual bool Pause_Sound_Effect(AudioHandle handle) = 0;
        virtual bool Resume_Sound_Effect(AudioHandle handle) = 0;
        virtual void Fade_Sound_Effect(AudioHandle handle, int ticks = 60) = 0;
        virtual bool Is_Sound_Effect_Playing(AudioHandle handle) const = 0;
        virtual void Set_Sound_Effect_Volume(AudioHandle handle, float volume) = 0;
        virtual void Set_Sound_Effect_Pitch(AudioHandle handle, float pitch) = 0;
        virtual void Set_Sound_Effect_Pan(AudioHandle handle, float pan) = 0;
        virtual void Set_All_Sound_Effect_Volume(float volume) = 0;

        /**
         *  Music
         */
        virtual AudioHandle Play_Music(Wstring &filename, int volume, bool real_time_start = false) = 0;
        virtual void Stop_Music(AudioHandle handle) = 0;
        virtual bool Pause_Music(AudioHandle handle) = 0;
        virtual bool Resume_Music(AudioHandle handle) = 0;
        virtual void Fade_Music(AudioHandle handle, int ticks = 60) = 0;
        virtual bool Is_Music_Playing(AudioHandle handle) const = 0;
        virtual void Set_Music_Volume(AudioHandle handle, float volume) = 0;

        virtual void Set_Master_Volume(float volume) { MasterVolume = volume; }
        virtual float Get_Master_Volume() const { return MasterVolume; }

        void Set_Focus_Loss_Function(void (*func)()) { FocusLossFunction = func; }
        void Set_Stream_Low_Impact(bool set) { /*StreamLowImpact = set;*/ }

    protected:
    
        /**
         *  The name of this driver interface.
         */
        Wstring DriverName;
        
        /**
         *  The function to call when the game window loses focus.
         */
        static void (*FocusLossFunction)();
        
        /**
         *  
         */
        //static bool StreamLowImpact;

        /**
         *  
         */
        static float MasterVolume;

        /**
         *  Should the left and right channels be swapped?
         */
        static bool IsReverseChannels;

        /**
         *  
         */
        static int MaxSampleTrackers;
};


void __cdecl Set_Audio_Driver(AudioDriver *driver);
void __cdecl Remove_Audio_Driver();

AudioDriver * __cdecl Audio_Driver();

bool Audio_Driver_Is_Direct_Sound();


extern int Audio_MaxSampleTrackers;
extern bool Audio_IsReverseChannels;
extern char Audio_DriverName[32];
