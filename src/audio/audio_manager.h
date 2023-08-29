/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_MANAGER.H
 *
 *  @author        CCHyper
 *
 *  @brief         Installable MiniAudio audio driver.
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

#ifdef USE_MINIAUDIO

#include "always.h"
#include "wstring.h"
#include "vector.h"
#include "audio_defines.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  Forward declarations.
 */
struct ma_engine;
struct ma_device;
struct ma_sound;
typedef ma_sound ma_sound_group;


/**
 *  x
 */
class AudioManagerClass
{
    friend class AudioHandleClass;
    friend class AudioEventHandleClass;

    friend static void __cdecl Audio_Cleanup_Thread_Function();

    public:
        AudioManagerClass();
        virtual ~AudioManagerClass();

        /**
         *  Audio engine IO.
         */
        virtual bool Init(HWND hWnd);
        virtual void End();

        virtual bool Is_Available() const;
        virtual bool Is_Enabled() const;

        virtual void Enable();
        virtual void Disable();

        virtual bool Start_Engine(bool forced = false);
        virtual bool Stop_Engine();

        virtual void Focus_Loss();
        virtual void Focus_Restore();

        virtual void Sound_Callback() {}

        /**
         *  Sound playback control.
         */
        virtual AudioHandleClass * Play(Wstring filename, AudioGroupType group, float volume = 1.0f, float pitch = 1.0f, float pan = 0.0f, AudioPriorityType priority = AUDIO_PRIORITY_NORMAL, int limit = -1, float fade_in_seconds = 0.0f, float delay_in_seconds = 0.0f, bool start = true);
        virtual bool Stop(AudioHandleClass * handle);

        /**
         *  Volume control.
         */
        virtual bool Set_Master_Volume(float volume) const;

        /**
         *  Audio group control.
         */
        virtual float Get_Group_Volume(AudioGroupType group);
        virtual bool Set_Group_Volume(AudioGroupType group, float volume);
        virtual bool Is_Group_Playing(AudioGroupType group) const;
        virtual bool Start_Group(AudioGroupType group) const;
        virtual bool Stop_Group(AudioGroupType group) const;
        virtual bool Stop_And_Fade_Out_Group(AudioGroupType group) const;

        /**
         *  DirectSound access.
         */
        virtual void * Get_DirectSound_Object() const;
        virtual void * Get_DirectSound_Primary_Buffer() const;
        virtual void * Get_DirectSound_Buffer() const;

        /**
         *  Query functions.
         */
        virtual bool Is_FileType_Supported(AudioFileType type) const;
        virtual bool Is_File_Available(AudioFileType type, Wstring name) const;
        virtual Wstring Build_Filename_From_Type(AudioFileType type, Wstring name);

        /**
         *  Utility functions.
         */
        int AudioPriority_To_Priority(AudioPriorityType priority);
        AudioPriorityType Priority_To_AudioPriority(int priority);
        unsigned int fVolume_To_iVolume(float vol);
        float iVolume_To_fVolume(unsigned int vol);

        bool Audio_Set_Data(Wstring name, bool &available, AudioFileType &filetype, Wstring &filename, bool ignore_error = false);

        //bool Create_Debug_Window(/*HINSTANCE hInstance*/);
        //bool Close_Debug_Window();
        //void Debug_Window_Message_Handler();
        //void Debug_Window_Loop();

    private:
        bool Add_To_Tracker(AudioHandleClass *handle, AudioGroupType group);

    private:
        /**
         *  x
         */
        bool IsAvailable;

        /**
         *  When the window loses focus, the current engine volume is stored here
         *  so it can be restored when focus is restored.
         */
        float FocusRestoreVolume;

        /**
         *  x
         */
        ma_engine *Engine;
        ma_device *Device;

        /**
         *  x
         */
        ma_sound_group *SoundGroups[AUDIO_GROUP_COUNT];

        /**
         *  Tracks all the currently playing sounds.
         */
        typedef DynamicVectorArrayClass<AudioHandleClass *, AUDIO_GROUP_COUNT> SoundTrackerArrayType;
        SoundTrackerArrayType SoundTracker;
};


/**
 *  x
 */
extern AudioManagerClass AudioManager;


#endif
