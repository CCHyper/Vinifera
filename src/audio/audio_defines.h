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


/**
 *  x
 */
constexpr int AUDIO_FSHIFT_MIN = -100;
constexpr int AUDIO_FSHIFT_MAX = 100;
constexpr int AUDIO_VSHIFT_MIN = 0;
constexpr int AUDIO_VSHIFT_MAX = 100;

constexpr float AUDIO_VOLUME_MIN = 0.0f;
constexpr float AUDIO_VOLUME_MAX = 1.0f;


/**
 *  x
 */
typedef enum AudioGroupType
{
    AUDIO_GROUP_MUSIC,            // Background music.
    AUDIO_GROUP_MUSIC_AMBIENT,    // Ambient music/sound (ie, Ion storm).
    AUDIO_GROUP_SPEECH,           // EVA speech and map selection voiceover.
    AUDIO_GROUP_SOUND_EFFECT,     // In-game sound effects.
    AUDIO_GROUP_EVENT,            // Audio events and ambient sounds.

    AUDIO_GROUP_COUNT,
    AUDIO_GROUP_NONE = -1
} AudioGroupType;


/**
 *  x
 */
typedef enum AudioFileType
{
    AUDIO_TYPE_AUD,
    AUDIO_TYPE_OGG,
    AUDIO_TYPE_MP3,
    AUDIO_TYPE_WAV,
    AUDIO_TYPE_FLAC,

    AUDIO_TYPE_COUNT,
    AUDIO_TYPE_NONE = -1
} AudioFileType;


/**
 *  x
 */
typedef enum AudioPriorityType
{
    AUDIO_PRIORITY_LOWEST = 0,
    AUDIO_PRIORITY_LOW = 1,
    AUDIO_PRIORITY_NORMAL = 2,
    AUDIO_PRIORITY_HIGH = 3,
    AUDIO_PRIORITY_CRITICAL = 4,
} AudioPriorityType;


/**
 *  x
 */
typedef enum AudioControlType
{
    /**
     *  No special controls applied.
     */
    AUDIO_CONTROL_NORMAL = 0,

    /**
     *  Continually play sound event. LoopLimit attribute can be used to
     *  specify how many times to loop the event.
     */
    //AUDIO_CONTROL_LOOP = 1 << 0,

    /**
     *  Randomly pick sound from sound list to play.
     */
    AUDIO_CONTROL_RANDOM = 1 << 1,

    /**
     *  TODO: x
     */
    //AUDIO_CONTROL_SEQUENTIAL = 1 << 2,

    /**
     *  Use all sounds in sound list.
     */
    //AUDIO_CONTROL_ALL = 1 << 3,

    /**
     *  Normally if the DELAY attribute is specified the delay happens
     *  when the sound finishes playing. PREDELAY forces the delay to 
     *  happen at the start of the sound. Random predelays help avoid
     *  'phasing' when multiple instances of the same audio event happen
     *  at the same time.
     */
    AUDIO_CONTROL_PREDELAY = 1 << 4,

    /**
     *  TODO: x
     * 
     *  Reserved for eva speech!
     */
     AUDIO_CONTROL_QUEUE = 1 << 5,

     /**
      *  TODO: x
      * 
      *  Reserved for eva speech!
      */
     AUDIO_CONTROL_QUEUED_INTERRUPT = 1 << 6,

    /**
     *  This new instances of this type sound event have priority over
     *  already playing instances.
     */
    AUDIO_CONTROL_INTERRUPT = 1 << 7,

    /**
     *  Specifies that the first sound in the sound list gets played,
     *  regardless, at the start of the audio event (see ATTACK attribute
     *  also). This sound is called the attack sound.
     */
    //AUDIO_CONTROL_ATTACK = 1 << 8,

    /**
     *  Specifies that the last sound in the sound list gets played,
     *  regardless, at the end of the audio event. (see DECAY attribute
     *  also). This sound is called the decay sound.
     */
    //AUDIO_CONTROL_DECAY = 1 << 9,

    /**
     *  x
     */
    AUDIO_CONTROL_AMBIENT = 1 << 10,

} AudioControlType;


typedef enum AudioSoundType
{
    /**
     *  The default type.
     */
    AUDIO_SOUND_NORMAL = 1 << 0,

    /**
     *  x
     */
    //AUDIO_SOUND_VIOLENT = 1 << 1,

    /**
     *  x
     */
    //AUDIO_SOUND_MOVEMENT = 1 << 2,

    /**
     *  x
     */
    //AUDIO_SOUND_QUIET = 1 << 3,

    /**
     *  x
     */
    //AUDIO_SOUND_LOUD = 1 << 4,

    /**
     *  Positional audio event is always audible regardless of where in
     *  the world it is. Instead of fading to silence when out of range
     *  like normal events, global events do not fade below MINVOLUME.
     */
    AUDIO_SOUND_GLOBAL = 1 << 5,

    /**
     *  Audio event fades out only when it moves off the edge of the screen.
     */
    AUDIO_SOUND_SCREEN = 1 << 6,

    /**
     *  Only audible are its point of origin in the game world.
     */
    AUDIO_SOUND_LOCAL = 1 << 7,

    /**
     *  Only audible by the player who triggerd this sound event.
     */
    //AUDIO_SOUND_PLAYER = 1 << 8,

    /**
     *  TODO: x
     */
    //AUDIO_SOUND_ALLIES = 1 << 9,

    /**
     *  TODO: x
     */
    //AUDIO_SOUND_ENEMIES = 1 << 10,

    /**
     *  TODO: x
     */
    //AUDIO_SOUND_EVERYONE = 1 << 11,

    /**
     *  Not audible if sounds with greater volume than us are playing.
     */
    //AUDIO_SOUND_GUN_SHY = 1 << 12,

    /**
     *  Not audible if other sounds are playing.
     */
    //AUDIO_SOUND_NOISE_SHY = 1 << 13,

    /**
     *  Not audible when not covered by shroud.
     */
    AUDIO_SOUND_UNSHROUDED = 1 << 14,

    /**
     *  Not audible when shrouded.
     * 
     *  If this flag is present, the sound will not play if its
     *  position is inside the shroud or fog of war. 
     */
    AUDIO_SOUND_SHROUDED = 1 << 15,

    /**
     *  TODO: Ambient background sound, not affected by INTERRUPT.
     */
    //AUDIO_SOUND_AMBIENT = 1 << 16,

    /**
     *  
     * 
     *  Reserved for use by eva speech!
     */
    AUDIO_SOUND_VOICE = 1 << 18,

    /**
     *
     *
     *  Reserved for use ui sound effects!
     */
     AUDIO_SOUND_UI = 1 << 19,

} AudioSoundType;

#endif
