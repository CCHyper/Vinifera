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
#include "ccfile.h"
#include <memory>
#include <xaudio2.h>


struct IXAudio2SourceVoice;
struct OggVorbis_File;
class XAudio2SoundResource;


#define STREAMING_BUFFER_SIZE (65536 * 10)
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

        virtual Wstring Get_Name() const;
        virtual Wstring Get_FileName() const;

        virtual bool Open_Stream();
        virtual bool Update_Stream();

        virtual int Queued_Buffer_Count() const;
        virtual bool Is_Buffer_Empty() const;

        virtual bool Is_Pending() const;
        virtual bool Is_Started() const;
        virtual bool Is_Playing() const;
        virtual bool Is_Paused() const;
        virtual bool Is_Stopped() const;
        virtual bool Is_Finished() const;
        virtual bool Is_Loop_Ended() const;

        virtual bool Set_Volume(float volume);
        virtual float Get_Volume() const;

        virtual bool Set_Pitch(float pitch);
        virtual float Get_Pitch() const;

        virtual bool Set_Pan(float pan);
        virtual float Get_Pan() const;

        virtual void Fade_In(int seconds, float step = 0.10f);
        virtual void Fade_Out(int seconds, float step = 0.10f);

    protected:
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
        std::unique_ptr<CCFileClass> FileHandle;

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

    public:
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
            STREAM_STATUS_LOOP_END,     // Loop iteration finished.

            STREAM_STATUS_ERROR = -1,   // Voice error.
        } AudioStreamStatus;

        AudioStreamStatus Status;

        /**
         *  x
         */
        AudioStreamType Type;

        /**
         *  x
         */
        char Buffers[MAX_BUFFER_COUNT][STREAMING_BUFFER_SIZE];
        int BufferIndex;

        class StreamingVoiceCallback : public IXAudio2VoiceCallback
        {
            public:
                StreamingVoiceCallback(/*const char* event_name*/) :
                    hBufferEndEvent(nullptr),
                    hLoopEndEvent(nullptr)
                {
                    //char buffer[128];

                    //std::snprintf(buffer, sizeof(buffer), "%s_BufferEndEvent", event_name);
                    //hBufferEndEvent = CreateEvent(nullptr, false, false, buffer);
                    hBufferEndEvent = CreateEvent(nullptr, false, false, nullptr);

                    //std::snprintf(buffer, sizeof(buffer), "%s_LoopEndEvent", event_name);
                    //hLoopEndEvent = CreateEvent(nullptr, false, false, buffer);
                    hLoopEndEvent = CreateEvent(nullptr, false, false, nullptr);
                }

                ~StreamingVoiceCallback()
                {
                    CloseHandle(hBufferEndEvent);
                    hBufferEndEvent = nullptr;

                    CloseHandle(hLoopEndEvent);
                    hLoopEndEvent = nullptr;
                }

                // Called during each processing pass for each voice, just before
                // XAudio2 reads data from the voice's buffer queue.
                STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32 BytesRequired) override
                {
                }

                // Called just after the processing pass for the voice ends.
                STDMETHOD_(void, OnVoiceProcessingPassEnd)() override
                {
                }

                // Called when the voice has just finished playing a contiguous audio stream.
                STDMETHOD_(void, OnStreamEnd)()
                {
                    SetEvent(hBufferEndEvent);
                }

                // Called when the voice is about to start processing a new audio buffer.
                STDMETHOD_(void, OnBufferStart)(void * pBufferContext) override
                {
                    XAudio2Stream *context = (XAudio2Stream *)pBufferContext;

                    context->Status = XAudio2Stream::STREAM_STATUS_STARTED;
                }

                // Called when the voice finishes processing a buffer.
                STDMETHOD_(void, OnBufferEnd)(void * pBufferContext) override
                {
                    XAudio2Stream *context = (XAudio2Stream *)pBufferContext;

                    context->Status = XAudio2Stream::STREAM_STATUS_FINISHED;
                }

                // Called when the voice reaches the end position of a loop.
                STDMETHOD_(void, OnLoopEnd)(void * pBufferContext) override
                {
                    XAudio2Stream *context = (XAudio2Stream *)pBufferContext;

                    context->Status = XAudio2Stream::STREAM_STATUS_LOOP_END;

                    SetEvent(hLoopEndEvent);
                }

                // Called when a critical error occurs during voice processing.
                STDMETHOD_(void, OnVoiceError)(void * pBufferContext, HRESULT Error) override
                {
                    XAudio2Stream *context = (XAudio2Stream *)pBufferContext;

                    context->Status = XAudio2Stream::STREAM_STATUS_ERROR;
                }

            public:
                HANDLE hBufferEndEvent;
                HANDLE hLoopEndEvent;
        };
        
        /**
         *  x
         */
        StreamingVoiceCallback StreamingCallback;

    public:
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
