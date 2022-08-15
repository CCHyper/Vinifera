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
#include "audio_manager.h"
#include "audio_util.h"
#include "wstring.h"
#include "ccfile.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <combaseapi.h> // for "interface"
#include <fmod.hpp>


/**
 *  x
 */
interface ISound2D
{
    public:
        virtual ~ISound2D() = 0;

        virtual bool Play(FMOD::Sound *sound, FMOD::ChannelGroup *channel_group, FMOD::SoundGroup *sound_group, AudioManagerClass::DSPFlagType dsp_flag, float volume, int priority, float pan, float pitch, int loop_count, bool paused) const = 0;
        virtual bool Stop() const = 0;

        virtual bool Pause() const = 0;
        virtual bool Resume() const = 0;

        virtual bool Mute() const = 0;

        virtual bool Is_Playing() const = 0;
        virtual bool Is_Audible() const = 0;

        virtual Wstring Get_Name() const = 0;
        virtual unsigned int Get_Position() const;
        virtual unsigned int Get_Length() const;
};


/**
 *  x
 */
class Sound2D : public ISound2D
{
    public:
        Sound2D(Wstring name, bool non_blocking = true, bool is_stream = false, bool unique = false) :
            Name(name),
            Sound(nullptr),
            Channel(nullptr),
            IsStream(is_stream)
        {
            Sound = Create_Sound(name, non_blocking, is_stream, unique);
            ASSERT(Sound != nullptr);
        }
        
        /**
         *  x
         */
        virtual ~Sound2D() override
        {
            if (Sound) {
                Sound->release();
                Sound = nullptr;
            }
        }
        
        /**
         *  x
         */
        virtual bool Play(FMOD::Sound *sound, FMOD::ChannelGroup *channel_group, FMOD::SoundGroup *sound_group, AudioManagerClass::DSPFlagType dsp_flag, float volume, int priority, float pan, float pitch, int loop_count, bool paused) const override
        {
            ASSERT(sound != nullptr);
            ASSERT(channel_group != nullptr);

            FMOD_RESULT result;
            FMOD::Channel *channel = nullptr; // FMOD makes channels invalid after playback has finished, so we don't need to release this handle.

            /**
             *  Play the sound handle.
             * 
             *  We start the sounds paused as we need to set some channel attributes, otherwise
             *  the sound will start and there could be inconsitencies in volume, etc, for a
             *  fraction of a second.
             */
            bool paused = true;
            result = AudioManagerClass::Get_Audio_System()->playSound(sound, channel_group, paused, &channel);
            FMOD_ERRCHECK(result);

            /**
             *  
             */
            //FMOD_SOUND_TYPE soundType = FMOD_SOUND_TYPE_UNKNOWN;;
            //FMOD_SOUND_FORMAT formatType = FMOD_SOUND_FORMAT_NONE;
            //int channels = 0;
            //int bits = 0;
            //result = sound->getFormat(&soundType, &formatType, &channels, &bits);
            //FMOD_ERRCHECK(result);

            /**
             *  Set sound attributes.
             */
            result = sound->setDefaults(48000, priority);
            FMOD_ERRCHECK(result);

            result = channel->setVolume(volume);
            FMOD_ERRCHECK(result);

            result = channel->setVolumeRamp(false); // For fixing popping noise at low volume.
            FMOD_ERRCHECK(result);

            result = channel->setPan(pan);
            FMOD_ERRCHECK(result);

            result = channel->setPitch(pitch);
            FMOD_ERRCHECK(result);

            result = channel->setMode(FMOD_LOOP_NORMAL);
            FMOD_ERRCHECK(result);

            if (loop_count > 0) {
                result = channel->setLoopCount(loop_count);
                FMOD_ERRCHECK(result);
            }

            //result = channel->setCallback(FMOD_ChannelCallback);
            //FMOD_ERRCHECK(result);

            result = channel->setUserData((void *)sound); // Set FMOD::Sound instance as user data for the callback.
            FMOD_ERRCHECK(result);

            /**
             *  Apply any flagged DSP effects.
             */
            //if (dsp_flag != FMOD_DSP_NONE) {
            //    Apply_DSP_Effects(sound, channel, dsp_flag);
            //}
    
            /**
             *  If the sound is flagged as not paused (play instantly), play it now we have set all the attributes.
             */
            if (!paused) {
                result = channel->setPaused(false);
                FMOD_ERRCHECK(result);
            }

            char namebuf[256];
            result = sound->getName(namebuf, sizeof(namebuf)-1);
            FMOD_ERRCHECK(result);

            DEBUG_INFO("[FMOD] Sound \"%s\" played succesfully.\n", namebuf);

        #ifndef NDEBUG
            float channel_volume;
            result = channel_group->getVolume(&channel_volume);
            FMOD_ERRCHECK(result);

            char channel_namebuf[256];
            result = channel_group->getName(channel_namebuf, sizeof(channel_namebuf)-1);
            FMOD_ERRCHECK(result);

            DEV_DEBUG_INFO("[FMOD] ChannelGroup name: %s.\n", channel_namebuf);
            DEV_DEBUG_INFO("[FMOD] ChannelGroup volume: %f.\n", channel_volume);
            DEV_DEBUG_INFO("[FMOD] Sound volume: %f.\n", volume);
            DEV_DEBUG_INFO("[FMOD] Sound pan: %f.\n", pan);
            DEV_DEBUG_INFO("[FMOD] Sound pitch: %f.\n", pitch);

            int numdsps = 0;
            result = channel->getNumDSPs(&numdsps);
            FMOD_ERRCHECK(result);

            DEV_DEBUG_INFO("[FMOD] NumDSPs: %d\n", numdsps);
        #endif

            return true;
        }

        /**
         *  x
         */
        virtual bool Stop() const override;
        
        /**
         *  x
         */
        virtual bool Pause() const override;

        /**
         *  x
         */
        virtual bool Resume() const override;
        
        /**
         *  x
         */
        virtual bool Mute() const override;

        /**
         *  x
         */
        virtual bool Is_Playing() const override
        {
            FMOD_RESULT result;
            bool is_playing = false;

            if (!Sound) {
                return false;
            }

            result = Channel->isPlaying(&is_playing);
            FMOD_ERRCHECK(result);

            return is_playing;
        }
        
        /**
         *  x
         */
        virtual bool Is_Audible() const override
        {
            FMOD_RESULT result;
            float volume = 0.0f;

            if (!Channel) {
                return false;
            }

            result = Channel->getVolume(&volume);
            FMOD_ERRCHECK(result);

            return volume > 0.0f;
        }

        /**
         *  x
         */
        virtual Wstring Get_Name() const override
        {
            Wstring name = "Unknown";

            if (!Sound) {
                return name;
            }

            char namebuff[256];
            if (Sound->getName(namebuff, sizeof(namebuff)-1) != FMOD_OK) {
                return nullptr;
            }

            name = *namebuff;

            return name;
        }
        
        /**
         *  x
         */
        virtual unsigned int Get_Position() const
        {
            FMOD_RESULT result;
            unsigned int position_ms = 0;

            if (!Channel) {
                return 0;
            }

            result = Channel->getPosition(&position_ms, FMOD_TIMEUNIT_MS);
            FMOD_ERRCHECK(result);

            return position_ms;
        }
        
        /**
         *  x
         */
        virtual unsigned int Get_Length() const override
        {
            FMOD_RESULT result;
            unsigned int length_ms = 0;

            if (!Sound) {
                return 0;
            }

            result = Sound->getLength(&length_ms, FMOD_TIMEUNIT_MS);
            FMOD_ERRCHECK(result);

            return length_ms;
        }

    private:
        /**
         *  name
         * 
         *  desc
         */
        FMOD::Sound *Create_Sound(Wstring name, bool non_blocking, bool is_stream, bool unique)
        {
            ASSERT(system != nullptr);

            FMOD_RESULT result;
            FMOD::Sound *sound = nullptr;

            Wstring filename;
            if (!AudioManagerClass::Is_Supported_File_Available(name, filename)) {
                DEBUG_WARNING("[Sound2D] Sound \"%s\" was not found in a supported format!\n", name.Peek_Buffer());
                return nullptr;
            }

            int size = CCFileClass(filename.Peek_Buffer()).Size();

            DEV_DEBUG_INFO("[Sound2D] Sound \"%s\" was found as \"%s\" (Size: %d).\n", name.Peek_Buffer(), filename.Peek_Buffer(), size);
    
            FMOD_CREATESOUNDEXINFO exinfo;
            FMOD_MODE mode = FMOD_DEFAULT
                            |FMOD_2D
                            |FMOD_LOOP_NORMAL
                            |FMOD_IGNORETAGS;

            // For opening sounds and getting streamed subsounds (seeking) asyncronously.
            if (non_blocking) {
                mode |= FMOD_NONBLOCKING;
            }

            // Unique sound, can only be played one at a time.
            if (unique) {
                mode |= FMOD_UNIQUE;
            }

            int max_buffer_size = 1048576; // 1mb

            /**
             *  Files larger than 1mb will be streamed.
             */
            if (size > max_buffer_size) {
                result = AudioManagerClass::Get_Audio_System()->createStream(filename.Peek_Buffer(), mode, nullptr, &sound);
                FMOD_ERRCHECK(result);
            } else {
                result = AudioManagerClass::Get_Audio_System()->createSound(filename.Peek_Buffer(), mode, nullptr, &sound);
                FMOD_ERRCHECK(result);
            }

            if (!sound) {
                DEBUG_ERROR("[Sound2D] Sound object was null in Create_Sound for \"%s\"!\n", filename.Peek_Buffer());
                return nullptr;
            }

            return sound;
        }

        /**
         *  name
         * 
         *  Applies the DSP effects to the specified channel.
         */
        bool Apply_DSP_Effects(FMOD::Sound *sound, FMOD::Channel *channel, AudioManagerClass::DSPFlagType dsp_flag)
        {
            #if 0
            FMOD_RESULT result;

            char namebuf[256];
            result = sound->getName(namebuf, sizeof(namebuf)-1);
            FMOD_ERRCHECK(result);

            for (int index = 0; index < FMOD_DSP_COUNT; ++index) {

                DSPType dsp = DSPType(index);
                int bit_dsp = (1 << index);

                if ((dsp_flag & bit_dsp) == 1) {
                    if (DSPEffects[index] != nullptr) {

                        result = channel->addDSP(FMOD_CHANNELCONTROL_DSP_TAIL, DSPEffects[index]);
                        FMOD_ERRCHECK(result);

                        DEV_DEBUG_INFO("[FMOD] Applying DSP effect \"%s\" to \"%s\".\n", DSP_Name_From(dsp).Peek_Buffer(), namebuf);

                    } else {
                        DEV_DEBUG_WARNING("[FMOD] Failed to apply DSP effect \"%s\" to \"%s\".\n", DSP_Name_From(dsp).Peek_Buffer(), namebuf);
                        //return false;
                    }
                }
            }
            #endif

            return true;
        }

    public: //private:
        /**
         *  x
         */
        Wstring Name;

        /**
         *  x
         */
        FMOD::Sound *Sound;

        /**
         *  x
         */
        FMOD::Channel *Channel;

        /**
         *  x
         */
        bool IsStream;
};
