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
 *  
 */
typedef const int AudioHandle;
typedef const void * AudioSample;


/**
 *  
 */
class AudioDriver
{
    public:
        AudioDriver(Wstring driver_name) : DriverName(driver_name), FocusLossFunction(nullptr), StreamLowImpact(false) {}
        virtual ~AudioDriver() {}

        virtual Wstring &Get_Name() { return DriverName; }

        virtual bool Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels = false) = 0;
        virtual void End() = 0;

        virtual bool Is_Available() const = 0;
        virtual bool Is_Enabled() const = 0;

        virtual void Enable() = 0;
        virtual void Disable() = 0;

        virtual void Stop_Handle_Playing(AudioHandle handle) = 0;
        virtual void Stop_Sample_Playing(AudioSample sample) = 0;

        virtual AudioHandle Play_Sample(AudioSample sample, int priority = PRIORITY_MAX, int volume = VOLUME_MAX) = 0;
        virtual AudioHandle Play_File(Wstring &filename, int priority = PRIORITY_MAX, int volume = VOLUME_MAX) = 0;
        virtual AudioHandle Stream_Sample(AudioSample sample, int volume, bool real_time_start = false) = 0;
        virtual AudioHandle Stream_File(Wstring &filename, int volume, bool real_time_start = false) = 0;

        virtual bool Pause_Handle(AudioHandle handle) = 0;
        virtual bool Resume_Handle(AudioHandle handle) = 0;

        virtual void Fade_Sample(AudioHandle handle, int ticks = 60) = 0;

        virtual bool Is_Handle_Playing(AudioHandle handle) const = 0;
        virtual bool Is_Sample_Playing(AudioSample sample) const = 0;
        virtual bool Is_Handle_Looping(AudioHandle handle) const = 0;

        virtual AudioHandle Get_Playing_Sample_Handle(AudioSample sample) const = 0;

        //virtual void Set_Handle_Loop(AudioHandle handle, bool loop) = 0;
        //virtual void Set_Sample_Loop(AudioSample sample, bool loop) = 0;
        //virtual void Stop_Handle_Looping(AudioHandle handle) = 0;
        //virtual void Stop_Sample_Looping(AudioSample sample) = 0;

        //virtual void Set_Score_Handle_Pause(AudioHandle handle) = 0;
        //virtual void Set_Score_Handle_Resume(AudioHandle handle) = 0;

        //virtual void Set_Handle_Pitch(AudioHandle handle, float pitch) = 0;
        //virtual void Set_Sample_Pitch(AudioSample sample, float pitch) = 0;

        //virtual void Set_Handle_Pan(AudioHandle handle, float pan) = 0;
        //virtual void Set_Sample_Pan(AudioSample sample, float pan) = 0;

        virtual void Set_Handle_Volume(AudioHandle handle, int volume) = 0;
        virtual void Set_Sample_Volume(AudioSample sample, int volume) = 0;

        virtual int Set_Sound_Volume(int volume) = 0;
        virtual int Set_Score_Volume(int volume) = 0;

        virtual void Set_Volume_All(int volume) = 0;
        virtual int Set_Volume_Percent_All(int vol_percent) = 0;

        virtual AudioHandle Play_Sample_Handle(AudioSample sample, int priority, int volume, AudioHandle handle) = 0;

        virtual bool Start_Primary_Sound_Buffer(bool forced = false) = 0;
        virtual void Stop_Primary_Sound_Buffer() = 0;

        virtual void Focus_Loss() { if (FocusLossFunction) { FocusLossFunction(); } }

        virtual void Sound_Callback() = 0;

        void Set_Focus_Loss_Function(void (*func)()) { FocusLossFunction = func; }
        void Set_Stream_Low_Impact(bool set) { StreamLowImpact = set; }

    protected:
        Wstring DriverName;
        void (*FocusLossFunction)();
        bool StreamLowImpact;
};


void __cdecl Set_Audio_Driver(AudioDriver *driver);
void __cdecl Remove_Audio_Driver();

AudioDriver * __cdecl Audio_Driver();

bool Audio_Driver_Is_Direct_Sound();
