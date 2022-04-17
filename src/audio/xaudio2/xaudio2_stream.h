/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          XAUDIO2_STREAM.H
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
#include "audio_driver.h"
#include "ttimer.h"
#include "stimer.h"


struct IXAudio2SourceVoice;
struct OggVorbis_File;
class XAudio2SoundResource;


#define STREAMING_BUFFER_SIZE 65536*10
#define MAX_BUFFER_COUNT 3


/**
 *  x
 */
class XAudio2Stream
{
    public:
        XAudio2Stream(XAudio2SoundResource *res = nullptr);
        virtual ~XAudio2Stream();

        virtual bool Start();
        virtual bool Stop(bool play_tails = false);

        virtual bool Play();

        virtual bool Pause(bool play_tails = false);
        virtual bool UnPause();

        virtual void Reset();

        virtual bool Open_Stream();
        virtual bool Update_Stream();

        virtual int Queued_Buffer_Count() const;

        virtual bool Is_Pending() const;
        virtual bool Is_Playing() const;
        virtual bool Is_Paused() const;
        virtual bool Is_Stopped() const;
        virtual bool Is_Finished() const;

        virtual bool Set_Volume(float volume);
        virtual float Get_Volume() const;

        virtual bool Set_Pitch(float pitch);
        virtual float Get_Pitch() const;

        virtual bool Set_Pan(float pan);
        virtual float Get_Pan() const;

        virtual void Fade_In(int seconds, float step = 0.10f);
        virtual void Fade_Out(int seconds, float step = 0.10f);

    public:
        /**
         *  x
         */
        IXAudio2SourceVoice *SourceVoice;

        /**
         *  x
         */
        Wstring FileName;

        /**
         *  x
         */
        FileClass *FileHandle;

        /**
         *  x
         */
        XAudio2SoundResource *SoundResource;
        
        /**
         *  x
         */
        WAVEFORMATEX Format;

        /**
         *  x
         */
        bool IsActive;

        /**
         *  x
         */
        typedef enum AudioStreamStatus
        {
            STREAM_STATUS_NULL,         // No nothing.
            STREAM_STATUS_PENDING,      // Pending load and voice creation.
            STREAM_STATUS_STARTED,      // Voice started.
            STREAM_STATUS_PLAYING,      // Voice playing.
            STREAM_STATUS_PAUSED,       // Voice paused.
            STREAM_STATUS_STOPPED,      // Voice stopped.
            STREAM_STATUS_FINISHED,     // Voice finished.
        } AudioStreamStatus;

        AudioStreamStatus Status;

        /**
         *  0 - 255
         */
        unsigned char Priority;

        /**
         *  x
         */
        float Volume;

        /**
         *  x
         */
        float Pitch;

        /**
         *  x
         */
        float Pan;

        /**
         *  x
         */
        bool IsFadingIn;
        unsigned FadeInSeconds;
        float FadeInStep;

        bool IsFadingOut;
        unsigned FadeOutSeconds;
        float FadeOutStep;

        /**
         *  x
         */
        char Buffers[MAX_BUFFER_COUNT][STREAMING_BUFFER_SIZE];
        int BufferIndex;
};


/**
 *  x
 */
class OggStream final : public XAudio2Stream
{
    public:
        OggStream(XAudio2SoundResource *res);
        virtual ~OggStream();
        
        virtual void Reset() override;

        virtual bool Open_Stream();
        virtual bool Update_Stream() override;

    public:
        /**
         *  x
         */
        OggVorbis_File *OggVorbisFile;
};
