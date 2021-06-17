/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENAL_AUDIO.H
 *
 *  @authors       OmniBlade, CCHyper, tomsons26
 *
 *  @brief         OpenAL audio interface.
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
#include "vector.h"
#include "wwfile.h"
#include "soscodec.h"
#include "dsaudio.h"
#include <al.h>
#include <alc.h>


class FileClass;


/**
 *  OpenAL audio interface.
 */
class OpenALAudioClass
{
    public:
        enum {
            PRIORITY_MIN = 0,
            PRIORITY_MAX = 255,
            VOLUME_MIN = 0,
            VOLUME_MAX = 255,

            INVALID_AUDIO_HANDLE = -1,

            BUFFER_CHUNK_SIZE = 8192,
            UNCOMP_BUFFER_SIZE = 2098,

            STREAM_BUFFER_COUNT = 16,
            OPENAL_BUFFER_COUNT = 2,

            DEFAULT_TRACKER_COUNT = 8
        };

    private:
        typedef struct SampleTrackerType
        {
            SampleTrackerType();
            ~SampleTrackerType();
            
            /**
             *  This flags whether this sample structure is active or not.
             */
            bool IsActive;
            
            /**
             *  This flags whether the sample is loading or has been started.
             */
            bool IsLoading;
            
            /**
             *  If this sample is really to be considered a score rather than
             *  a sound effect, then special rules apply. These largely fall into
             *  the area of volume control.
             */
            bool IsScore;

            /**
             *  If this sample is a speech sample, then special rules apply.
             *  These largely fall into the area of priority control.
             */
            bool IsSpeech;

            /**
             *  This flag indicates whether this sample needs servicing.
             *  Servicing entails filling one of the empty low buffers.
             */
            bool IsNeedingService;

            /**
             *  This flag indicates that there is more source data to copy to the play buffer.
             */
            bool MoreSource;

            /**
             *  This flag indicates that the entire sample fitted inside the buffer.
             */
            bool IsOneShot;
            
            /**
             *  This is the original sample pointer. It is used to control the sample based on
             *  pointer rather than handle. The handle method is necessary when more than one
             *  sample could be playing simultaneously. The pointer method is necessary when
             *  the dealing with a sample that may have stopped behind the programmer's back and
             *  this occurance is not otherwise determinable.
             */
            const void *Original;
            int OriginalSize;

            /**
             *  Pointer to the sound data that has not yet been copied to the playback buffers.
             */
            void *Source;

            /**
             *  This is the number of bytes remaining in the source data as to the playback buffers.
             */
            int Remainder;

            /**
             *  Samples maintain a priority which is used to determine which sounds
             *  live or die when the maximum number of sounds are being played.
             */
            int Priority;
            
            /**
             *  This is the current volume of the sample as it is being played.
             */
            int Volume;
            
            /**
             *  This is the amount to reduce volume per tick.
             */
            int Reducer;
            
            /**
             *  Streaming control handlers.
             */
            bool (*Callback_Func)(short handle, short *odd, void **buffer, int *size);
            int FilePending;        // Number of buffers already filled ahead.
            int FilePendingSize;    // Number of bytes in last filled buffer.
            short Odd;              // Block number tracker (0..StreamBufferCount-1).
            void *QueueBuffer;      // Pointer to continued sample data.
            int QueueSize;          // Size of queue buffer attached.

            /**
             *  The file variables are used when streaming directly off of the hard drive.
             */
            FileClass *FileHandle;  // null == not in use.
            void *FileBuffer;

            /**
             *  This is the compression that the sound data is using.
             */
            SCompressType Compression;

            /**
             *  The following structure is used if the sample if compressed
             *  using the sos 16 bit compression codec.
             */
            _tagCOMPRESS_INFO2 sSOSInfo;

            /**
             *  A point in space that is the source of this sound.
             */
            ALuint OpenALSource;

            /**
             *  The format of the audio stream.
             */
            ALenum OpenALFormat;

            /**
             *  The frequency of the audio stream.
             */
            int OpenALFrequency;

            /**
             *  A set of buffers.
             */
            ALuint OpenALAudioBuffers[OPENAL_BUFFER_COUNT];

        } SampleTrackerType;

    public:
        OpenALAudioClass();
        ~OpenALAudioClass();

        bool Init(HWND hWnd, int tracker_count = DEFAULT_TRACKER_COUNT);
        void End();

        bool Is_Available() const { return OpenALContext != nullptr && !AudioDone; }

        bool Init_Trackers(int new_tracker_count);

        int Play_Sample(const void *sample, int priority = PRIORITY_MAX, int volume = VOLUME_MAX);
        int Play_Sample_Handle(const void *sample, int priority, int volume, int handle);

        int File_Stream_Sample_Vol(char const *filename, int volume, bool real_time_start = false);
        int Stream_Sample_Vol(void *buffer, int size, bool (*callback)(short, short *, void **, int *), int volume, int handle);

        void Set_Handle_Volume(int handle, int volume);
        void Set_Sample_Volume(const void *sample, int volume);
        void Set_Volume_All(int volume);
        void Adjust_Volume_All(int vol_percent);

        int Set_Sound_Vol(int volume);
        int Set_Score_Vol(int volume);

        int Get_Playing_Sample_Handle(const void *sample);
        int Get_Sample_Handle(const void *sample);

        void Fade_Sample(int handle, int ticks);
        void Stop_Sample(int handle);
        void Free_Sample(const void *sample);

        bool Sample_Status(int index);
        bool Is_Sample_Playing(const void *sample);
        void Stop_Sample_Playing(const void *sample);

        void Restore_Sound_Buffers();
        bool Set_Primary_Buffer_Format();

        bool Start_Primary_Sound_Buffer(bool forced = false);
        void Stop_Primary_Sound_Buffer();

        void Sound_Callback();

    private:
        int Simple_Copy(void **source, int *ssize, void **alternate, int *altsize, void **dest, int size);
        int Sample_Copy(SampleTrackerType *st, void **source, int *ssize, void **alternate, int *altsize, void *dest, int size, SCompressType scomp, void *trailer, short *trailersize);
        
        void File_Stream_Preload(int handle);

        //void *Load_Sample(const char *filename);
        //long Load_Sample_Into_Buffer(const char *filename, void *buffer, long size);
        //long Sample_Read(FileClass *file, void *buffer, long size);
        //long Sample_Length(const void *sample);

        int Get_Free_Sample_Handle(int priority);

        int Attempt_To_Play_Buffer(int handle);

        void Maintenance_Callback();
        bool File_Callback(short handle, short *odd, void **buffer, int *size);

    private:
        bool AudioDone;
        ALCdevice *OpenALDevice;
        ALCcontext *OpenALContext;
        unsigned MagicNumber;
        unsigned char *UncompBuffer;
        unsigned char *FileStreamBuffer;
        unsigned char ChunkBuffer[BUFFER_CHUNK_SIZE];
        int StreamBufferSize;
        int StreamBufferCount;
        int SoundVolume;
        int ScoreVolume;
        bool field_380;
        DynamicVectorClass<SampleTrackerType *> SampleTracker;
};
