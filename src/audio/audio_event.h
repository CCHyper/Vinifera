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


class AudioVocClass;
class AudioHandleClass;

struct ma_sound;
typedef ma_sound ma_sound_group;


/**
 *  
 */
class AudioEventHandleClass
{
    public:
        AudioEventHandleClass() : Voc(nullptr), Handle(nullptr) {}
        AudioEventHandleClass(AudioVocClass &voc);
        virtual ~AudioEventHandleClass();

        virtual bool Init(Wstring filename);

        virtual AudioVocClass &Get_Voc() const { return *Voc; }

        virtual bool Start();
        virtual bool Stop(float fade_out_seconds = 0.0f, bool fade = false);

        virtual bool Pause();
        virtual bool Resume();

        virtual bool Is_Playing() const;

        virtual bool Set_Looping(bool loop);
        virtual bool Set_Volume(float volume);
        virtual bool Set_Pitch(float pitch);
        virtual bool Set_Pan(float pan);

        virtual bool Mute();

    private:
        /**
         *  x
         */
        AudioVocClass *Voc;

        /**
         *  x
         */
        AudioHandleClass *Handle;
};

#endif
