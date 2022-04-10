#pragma once

#include "always.h"
#include "audio_resource.h"


typedef enum AudioSourceState
{
    STATE_STOPPED,
    STATE_PLAYING,
    STATE_PAUSED,
};


class AudioSource
{
    public:
        AudioSource() :
            Resource(nullptr)
        {
        }

        virtual ~AudioSource()
        {
            delete Resource;
            Resource = nullptr;
        }

        /**
         *  Starts the playback of the sound. If "loop" is set to true, it loops
         *  continuously for the number of iterations set by "loop_count" for either
         *  the entire buffer or using the authored loop points (if any).
         *  If paused, it resumes playback.
         */
        virtual bool Play(bool loop = false, int loop_count = -1) = 0;

        /**
         *  Stops the playback of the voice. If immediate is "true", the sound is
         *  immediately halted. Otherwise the current loop is exited (if looping)
         *  and any 'tails' are played.
         */
        virtual bool Stop(bool immediate = true) = 0;

        /**
         *  Pauses the sound playback.
         */
        virtual bool Pause() = 0;

        /**
         *  Resumes playback if the sound is paused.
         */
        virtual bool Resume() = 0;
        
        /**
         *  
         */
        virtual bool Is_Playing() const = 0;

        /**
         *  
         */
        virtual bool Is_Looping() const = 0;
        
        /**
         *  Sets the playback volume.
         *  Ranges from 0.0 to +1.0.
         */
        virtual void Set_Volume(float volume) = 0;

        virtual float Get_Volume() const = 0;
        
        /**
         *  Sets the pitch-shift factor.
         *  Ranges from -1.0 to +1.0, playback defaults to 0.0 (which is no pitch-shifting).
         */
        virtual void Set_Pitch(float pitch) = 0;

        virtual float Get_Pitch() const = 0;

        /**
         *  Sets the pan setting:
         *  -1.0 is fully left, +1.0 is fully right, and 0.0 is balanced.
         */
        virtual void Set_Pan(float pan) = 0;

        virtual float Get_Pan() const = 0;

        virtual AudioResource *Get_Resource() const { return Resource; }

        virtual void Set_Resource(AudioResource *resource)
        {
            delete Resource;
            Resource = nullptr;

            Resource = resource;
        }

        virtual int Get_Pending_Buffer_Count() const = 0;

    protected:
        /**
         *  Pointer to the audio resource which contains the file data.
         */
        AudioResource *Resource;
        
        /**
         *  
         */
        AudioSourceState State;
};

