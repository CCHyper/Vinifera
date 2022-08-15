/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FMOD_DRIVER.H
 *
 *  @author        CCHyper
 *
 *  @brief         Installable FMOD audio driver.
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

#ifdef USE_FMOD_AUDIO

#include "always.h"
#include "wstring.h"
#include "search.h"
#include "tibsun_defines.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <combaseapi.h> // for "interface"
#include <fmod.hpp>


interface ISound2D;


// "channel" is a instance of a sound.
// pitch shift down one note .9438f
// pitch shift up one note 1.059f
// pitch shift down one octave 0.5f
// pitch shift up one octave 2.0f


// for music
// https://qa.fmod.com/t/how-do-i-fade-in-and-fade-out-a-channel-with-stop/11738
// https://qa.fmod.com/t/fade-out-and-pause-issue/17066/3


/**
 *  
 */
class AudioManagerClass
{
    public:
        typedef enum DSPType
        {
            FMOD_DSP_NONE = -1,

#if 0
            FMOD_DSP_OSCILLATOR = 1 << 0,         // This unit generates sine/square/saw/triangle or noise tones.
            FMOD_DSP_LOWPASS = 1 << 1,            // This unit filters sound using a high quality, resonant lowpass filter algorithm but consumes more CPU time.
            FMOD_DSP_ITLOWPASS = 1 << 2,          // This unit filters sound using a resonant lowpass filter algorithm that is used in Impulse Tracker, but with limited cutoff range (0 to 8060hz).
            FMOD_DSP_HIGHPASS = 1 << 3,           // This unit filters sound using a resonant highpass filter algorithm.
            FMOD_DSP_ECHO = 1 << 4,               // This unit produces an echo on the sound and fades out at the desired rate.
            FMOD_DSP_FADER = 1 << 5,              // This unit pans and scales the volume of a unit.
            FMOD_DSP_FLANGE = 1 << 6,             // This unit produces a flange effect on the sound.
            FMOD_DSP_DISTORTION = 1 << 7,         // This unit distorts the sound.
            FMOD_DSP_NORMALIZE = 1 << 8,          // This unit normalizes or amplifies the sound to a certain level.
            FMOD_DSP_LIMITER = 1 << 9,            // This unit limits the sound to a certain level.
            FMOD_DSP_PARAMEQ = 1 << 10,           // This unit attenuates or amplifies a selected frequency range.
            FMOD_DSP_PITCHSHIFT = 1 << 11,        // This unit bends the pitch of a sound without changing the speed of playback.
            FMOD_DSP_CHORUS = 1 << 12,            // This unit produces a chorus effect on the sound.
            FMOD_DSP_ITECHO = 1 << 13,            // This unit produces an echo on the sound and fades out at the desired rate as is used in Impulse Tracker.
            FMOD_DSP_COMPRESSOR = 1 << 14,        // This unit implements dynamic compression (linked multichannel, wideband).
            FMOD_DSP_SFXREVERB = 1 << 15,         // This unit implements SFX reverb.
            FMOD_DSP_LOWPASS_SIMPLE = 1 << 16,    // This unit filters sound using a simple lowpass with no resonance, but has flexible cutoff and is fast.
            FMOD_DSP_DELAY = 1 << 17,             // This unit produces different delays on individual channels of the sound.
            FMOD_DSP_TREMOLO = 1 << 18,           // This unit produces a tremolo / chopper effect on the sound.
            FMOD_DSP_HIGHPASS_SIMPLE = 1 << 19,   // This unit filters sound using a simple highpass with no resonance, but has flexible cutoff and is fast.
            FMOD_DSP_PAN = 1 << 20,               // This unit pans the signal, possibly upmixing or downmixing as well.
            FMOD_DSP_THREE_EQ = 1 << 21,          // This unit is a three-band equalizer.
            FMOD_DSP_FFT = 1 << 22,               // This unit simply analyzes the signal and provides spectrum information back through getParameter.
            FMOD_DSP_LOUDNESS_METER = 1 << 23,    // This unit analyzes the loudness and true peak of the signal.
            FMOD_DSP_ENVELOPEFOLLOWER = 1 << 24,  // This unit tracks the envelope of the input/sidechain signal.
            FMOD_DSP_CONVOLUTIONREVERB = 1 << 25, // This unit implements convolution reverb. 
            FMOD_DSP_OBJECTPAN = 1 << 26,         // This unit sends the signal to a 3d object encoder like Dolby Atmos. Supports a subset of the FMOD_DSP_TYPE_PAN parameters.
            FMOD_DSP_MULTIBAND_EQ = 1 << 27,      // This unit is a flexible five band parametric equalizer.
#endif

            FMOD_DSP_OSCILLATOR,
            FMOD_DSP_LOWPASS,
            FMOD_DSP_ITLOWPASS,
            FMOD_DSP_HIGHPASS,
            FMOD_DSP_ECHO,
            FMOD_DSP_FADER,
            FMOD_DSP_FLANGE,
            FMOD_DSP_DISTORTION,
            FMOD_DSP_NORMALIZE,
            FMOD_DSP_LIMITER,
            FMOD_DSP_PARAMEQ,
            FMOD_DSP_PITCHSHIFT,
            FMOD_DSP_CHORUS,
            FMOD_DSP_ITECHO,
            FMOD_DSP_COMPRESSOR ,
            FMOD_DSP_SFXREVERB,
            FMOD_DSP_LOWPASS_SIMPLE,
            FMOD_DSP_DELAY,
            FMOD_DSP_TREMOLO,
            FMOD_DSP_HIGHPASS_SIMPLE,
            FMOD_DSP_PAN,
            FMOD_DSP_THREE_EQ,
            FMOD_DSP_FFT,
            FMOD_DSP_LOUDNESS_METER,
            FMOD_DSP_ENVELOPEFOLLOWER,
            FMOD_DSP_CONVOLUTIONREVERB,
            FMOD_DSP_OBJECTPAN,
            FMOD_DSP_MULTIBAND_EQ,

            FMOD_DSP_COUNT,
        } DSPType;

        typedef int DSPFlagType;

        static DSPType DSP_From_Name(Wstring name);
        //static DSPType DSP_From_Name_String(Wstring name);
        static Wstring DSP_Name_From(DSPType type);
        //static Wstring DSP_Name_String_From(DSPType type);

    public:
        AudioManagerClass();
        virtual ~AudioManagerClass();

        virtual bool Init(HWND hWnd);
        virtual void End();

        virtual bool Is_Available() const;
        virtual bool Is_Enabled() const;

        virtual void Enable();
        virtual void Disable();

        virtual bool Start_Engine(bool forced = false);
        virtual void Stop_Engine();

        virtual void Focus_Loss();
        virtual void Focus_Restore();

        virtual void Sound_Callback();

        /**
         *  Playback interface.
         */
        virtual bool Submit_Effect(Wstring name, VocType index);
        virtual bool Play_Effect(VocType index, float volume, int priority = 0, float pan = 0.0f, float pitch = 1.0f, DSPFlagType dsp_flag = FMOD_DSP_NONE, bool loop = false, bool paused = false);
        virtual bool Stop_Effect_Channel();

        virtual bool Submit_Speech(Wstring name, VoxType index);
        virtual bool Play_Speech(VoxType index, float volume, float pitch, DSPFlagType dsp_flag = FMOD_DSP_NONE, bool paused = false);
        virtual bool Stop_Speech_Channel();
        virtual bool Is_Speech_Channel_Playing() const;

        virtual bool Submit_Music(Wstring name, ThemeType index);
        virtual bool Play_Music(ThemeType index, float volume, DSPFlagType dsp_flag = FMOD_DSP_NONE, bool paused = false);
        virtual bool Stop_Music_Channel(bool fade_out = false, float seconds = 3.0);
        virtual bool Is_Music_Channel_Playing() const;
        virtual bool Pause_Music_Channel() const;
        virtual bool Resume_Music_Channel() const;
        
        virtual bool Stop_Movie_Channel();

        virtual bool Set_Effects_Size(int size);
        virtual bool Set_Speech_Size(int size);
        virtual bool Set_Music_Size(int size);

        virtual void Clear_Effects();
        virtual void Clear_Speech();
        virtual void Clear_Music();

        virtual bool Load_Sound_Bank(Wstring filename);

        //virtual bool Submit_Stream_PCM(const void *data, unsigned int length, int sample_rate, int channels, FMOD::Sound **sound);

        /**
         *  Volume control.
         */
        virtual float Get_Effect_Volume() const;
        virtual bool Set_Effect_Volume(float volume);

        virtual float Get_Speech_Volume() const;
        virtual bool Set_Speech_Volume(float volume);

        virtual float Get_Music_Volume() const;
        virtual bool Set_Music_Volume(float volume);

        virtual float Get_Movie_Volume() const;
        virtual bool Set_Movie_Volume(float volume);

        virtual float Get_Master_Volume() const;
        virtual bool Set_Master_Volume(float volume);

    private:
        typedef enum FMODPriority
        {
            FMOD_PRIORITY_MUSIC = 0,
            FMOD_PRIORITY_MOVIE = 1,
            FMOD_PRIORITY_SPEECH = 2,
            FMOD_PRIORITY_EFFECT_MIN = 10,
            FMOD_PRIORITY_EFFECT_MAX = 2456,
        } FMODPriority;

        FMOD::Sound *Create_Sound(Wstring name, bool non_blocking = false, bool unique = false);
        bool Play_Sound(FMOD::Sound *sound, FMOD::ChannelGroup *channel_group, FMOD::SoundGroup *sound_group, DSPFlagType dsp_flag, float volume, int priority = 0, float pan = 0.0f, float pitch = 1.0f, int loop_count = -1, bool paused = false);
        bool Apply_DSP_Effects(FMOD::Sound *sound, FMOD::Channel *channel, DSPFlagType dsp_flag);

    private:
        /**
         *  
         */
        bool IsAvailable;

        /**
         *  
         */
        bool IsEnabled;

        /**
         *  
         */
        /*Dynamic*/VectorClass<FMOD::Sound *> Ambient;
        /*Dynamic*/VectorClass<FMOD::Sound *> Effects;
        /*Dynamic*/VectorClass<FMOD::Sound *> Speech;
        /*Dynamic*/VectorClass<FMOD::Sound *> Music;

        /**
         *  
         */
        FMOD::SoundGroup *AmbientSoundGroup;
        FMOD::SoundGroup *EffectSoundGroup;

        /**
         *  
         */
        FMOD::ChannelGroup *AmbientChannelGroup;
        FMOD::ChannelGroup *EffectChannelGroup;
        FMOD::ChannelGroup *SpeechChannelGroup;
        FMOD::ChannelGroup *MusicChannelGroup;
        FMOD::ChannelGroup *MovieChannelGroup;

        /**
         *  
         */
        FMOD::SoundGroup *MasterSoundGroup;
        FMOD::ChannelGroup *MasterChannelGroup;

        /**
         *  
         */
        float MasterVolumeRestore;

        /**
         *  
         */
        FMOD::DSP *DSPEffects[FMOD_DSP_COUNT];

    private:
        /**
         *  
         */
        static FMOD::System *System;
        
        /**
         *  
         */
        static bool IsDebugLoggingEnabled;
        static bool IsMemoryDebugLoggingEnabled;
        static bool IsFileDebugLoggingEnabled;
        static bool IsCodecDebugLoggingEnabled;

    public:
        /**
         *  
         */
        static FMOD::System *Get_Audio_System() { return System; }

        static void *Get_Output_Handle();
        static bool Set_Software_Format(int sample_rate, int bits, int channels);

        /**
         *  Utility interface.
         */
        static bool Is_Supported_File_Available(Wstring name, Wstring &out_name);
        
        /**
         *  
         */
        static void Enable_Debug_Logging(bool enable) { IsDebugLoggingEnabled = enable; }
        static void Enable_Memory_Debug_Logging(bool enable) { IsMemoryDebugLoggingEnabled = enable; }
        static void Enable_File_Debug_Logging(bool enable) { IsFileDebugLoggingEnabled = enable; }
        static void Enable_Codec_Debug_Logging(bool enable) { IsCodecDebugLoggingEnabled = enable; }

    private:
        friend FMOD_RESULT F_CALLBACK FMOD_DebugLogCallback(FMOD_DEBUG_FLAGS flags, const char *file, int line, const char *func, const char *message);
};


/**
 *  
 */
extern AudioManagerClass AudioManager;

#endif
