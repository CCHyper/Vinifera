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
#include "audio_defines.h"
#include "debughandler.h"
#include "asserthandler.h"


struct ma_sound;
typedef ma_sound ma_sound_group;


/**
 *
 */
class AudioHandleClass
{
    // This is required so the thread can access this classes members.
    friend static void __cdecl Audio_Cleanup_Thread_Function();

    public:
        AudioHandleClass(Wstring filename, ma_sound_group *group, AudioGroupType audio_group) :
            Sound(),
            Filename(nullptr),
            ConcurrentLimit(-1),
            IsPaused(false),
            IsFinishedCallback(false),
            Priority(AUDIO_PRIORITY_NORMAL),
            Group(audio_group)
        {
            Load(filename, group);
        }

        virtual ~AudioHandleClass()
        {
            Free();
        }

    private:
        virtual bool Load(Wstring filename, ma_sound_group *group = nullptr);
        virtual bool Free();

    public:
        virtual Wstring Get_Filename() const;

        virtual bool Is_Valid() const;

        virtual bool Start();
        virtual bool Stop(float seconds = 0.0f, bool fade = false, bool reset = true);

        virtual bool Pause();
        virtual bool Resume();
        virtual bool Restart();

        virtual bool Is_Playing() const;
        virtual bool Is_Paused() const;
        virtual bool Is_Fading_Out() const;
        virtual bool Is_Fading_In() const;
        virtual bool Is_Looping() const;
        virtual bool Is_Finished() const;

        virtual bool Fade_In(float seconds);
        virtual bool Fade_Out(float seconds);

        virtual float Get_Volume() const;
        virtual float Get_Pitch() const;
        virtual float Get_Pan() const;
        virtual float Get_Time() const;
        virtual float Get_Length() const;
        virtual AudioPriorityType Get_Priority() const;
        virtual unsigned Get_Sample_Rate() const;
        virtual unsigned Get_Channels() const;

        virtual bool Set_Looping(bool loop);
        virtual bool Set_Volume(float volume);
        virtual bool Set_Pitch(float pitch);
        virtual bool Set_Pan(float pan);
        virtual bool Set_Time(float time_in_seconds);
        virtual bool Set_Priority(AudioPriorityType priority);
        virtual bool Set_Delay(float time_in_seconds);
        virtual bool Set_Limit(int limit);

        virtual bool Seek(float time);

        virtual AudioGroupType Get_Group() const { return Group; }

        virtual bool Set_User_Data(void *user_data);

    private:
        static void Sound_End_Callback(void *pUserData, ma_sound *pSound);

    private:
        /**
         *  x
         */
        ma_sound *Sound;

        /**
         *  x
         */
         //ma_sound_group *Group;
        AudioGroupType Group;

        /**
         *  x
         */
        Wstring Filename;

        /**
         *  x
         */
        AudioPriorityType Priority;

        /**
         *  x
         */
        AudioControlType Control;

        /**
         *  x
         */
        AudioSoundType Type;

        /**
         *  x
         */
        int ConcurrentLimit;

        /**
         *  x
         */
        bool IsPaused;

        /**
         *  Has this sound been marked as finished by the callback?
         */
        bool IsFinishedCallback;

        /**
         *  
         */
        bool IsDelaySet;
};

#endif
