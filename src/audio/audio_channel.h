/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_CHANNEL.H
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
#include "audio_util.h"
#include "wstring.h"
#include <fmod.hpp>


class AudioChannelClass
{
    public:
        AudioChannelClass(FMOD::Channel *channel) :
            Channel(channel)
        {
        }

        ~AudioChannelClass()
        {
            Channel = nullptr;
        }

        /**
         *  x
         */
        bool Is_Playing() const
        {
            if (!Channel) {
                return false;
            }

            bool is_playing = false;
            FMOD_ERRCHECK(Channel->isPlaying(&is_playing));

            return is_playing;
        }
        
        /**
         *  x
         */
        Wstring Get_Current_Sound_Name() const
        {
            Wstring name = "Unknown";

            if (!Channel) {
                return name;
            }

            FMOD::Sound *sound;
            if (Channel->getCurrentSound(&sound) != FMOD_OK) {
                return name;
            }

            char namebuff[256];
            if (sound->getName(namebuff, sizeof(namebuff)-1) != FMOD_OK) {
                return name;
            }

            name = *namebuff;

            return name;
        }

    public: //private:
        FMOD::Channel *Channel;
};
