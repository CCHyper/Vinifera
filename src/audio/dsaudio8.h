/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          DSAUDIO8.H
 *
 *  @author        CCHyper
 *
 *  @contributors  tomsons26
 *
 *  @brief         Main header file for the Direct Sound audio interface.
 *
 *  @license       ProjectTSYR is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 ProjectTSYR is distributed in the hope that it will be
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
#include "dsaudio.h"
#include "soscodec.h"
#include "vector.h"
#include "wwfile.h"


class FileClass;


class DirectSound8AudioClass
{
    public:
        /**
         *  Define the different type of sound compression available.
         */
        typedef enum SCompressType
        {
            SCOMP_NONE = 0,     // No compression -- raw data.

            SCOMP_WESTWOOD = 1, // Special sliding window delta compression.
            SCOMP_SOS = 99      // SOS frame compression.
        } SCompressType;

        /**
         *  
         */
        enum AudioEnums
        {
            PRIORITY_MIN = 0,
            PRIORITY_MAX = 255,
            VOLUME_MIN = 0,
            VOLUME_MAX = 255,

            TARGET_RESOLUTION = 10,      // 10-millisecond target resolution
            TIMER_DELAY = 25,
            TIMER_RESOLUTION = 1,
            MUTEX_TIMEOUT = 10000,       // 10-milliseconds

            INVALID_AUDIO_HANDLE = -1,

            BUFFER_CHUNK_SIZE = 8192,

            /**
             *  Size of temp HMI low memory staging buffer.
             */
            SECONDARY_BUFFER_SIZE = (1024*32), // 32kb

            STREAM_BUFFER_COUNT = 16,

            /**
             *  Max chunk size in auds.
             */
            MAX_AUD_CHUNK_SIZE = 512,
            AUD_MARGIN = 50,

            /**
             *  Uncompress buffer chunk size. 4 is the max size data would grow
             *  assuming we are decompressing an IMA based codec (IMA compresses to about / 4 the size).
             */
            UNCOMP_CHUNK_SIZE = (MAX_AUD_CHUNK_SIZE * 4) + AUD_MARGIN,

            /**
             *  This is the safety overrun margin for the sonarc compressed
             *  data frames. This value should be equal the maximum 'order' times
             *  the maximum number of bytes per sample.  It should be evenly divisible
             *  by 16 to aid paragraph alignment.
             */
            SONARC_MARGIN = 32,

            /**
             *  Maximum number of sound effects that may run at once. One tracker is
             *  always used for the streaming music.
             */
            MAX_SFX = 31+1, // #BUGFIX: Sound effects get noticeably cut off in busy battles in the
                            //          original game as the value was just 5, so we increase this value.
        };

    private:
        /**
         *  Define the sample control structure which helps us to handle feeding
         *  data to the sound interrupt.
         */
        typedef struct tSampleTrackerType
        {
            tSampleTrackerType(int index);
            ~tSampleTrackerType();

            /**
             *  This flags whether this sample structure is active or not.
             */
            bool Active;

            /**
             *  This flags whether the sample is loading or has been started.
             */
            bool Loading;

            /**
             *  If this sample is really to be considered a score rather than
             *  a sound effect, then special rules apply. These largely fall into
             *  the area of volume control.
             */
            bool IsScore;   

            /**
             *  This is the original sample pointer. It is used to control the sample based on
             *  pointer rather than handle. The handle method is necessary when more than one
             *  sample could be playing simultaneously. The pointer method is necessary when
             *  the dealing with a sample that may have stopped behind the programmer's back and
             *  this occurrence is not otherwise determinable. It is also used in
             *  conjunction with original size to unlock a sample which has been DPMI
             *  locked.
             */
            const void *Original;
            //int OriginalSize;

            WAVEFORMATEX BufferFormat;
            DSBUFFERDESC BufferDesc;

            /**
             *  These are pointers to the double buffers.
             */
            LPDIRECTSOUNDBUFFER PlayBuffer;

            /**
             *  Variable to keep track of the playback rate of this buffer.
             */
            int PlaybackRate;

            /**
             *  Variable to keep track of the sample type (8 or 16 bit) of this buffer.
             */
            int BitSize;

            /**
             *  Variable to keep track of the stereo ability of this buffer.
             */
            bool Stereo;

            /**
             *  The number of bytes in the buffer that has been filled but is not
             *  yet playing.  This value is normally the size of the buffer,
             *  except for the case of the last bit of the sample.
             */
            int DataLength;

            /**
             *  Pointer into the play buffer for writing the next
             *  chunk of sample to.
             */
            void *DestPtr;

            /**
             *  This flag indicates that there is more source data
             *  to copy to the play buffer.
             */
            bool MoreSource;

            bool field_25;

            /**
             *  This flag indicates that the entire sample fitted inside the
             *  direct sound secondary buffer.
             */
            bool OneShot;

            bool field_27;

            /**
             *  Pointer to the sound data that has not yet been copied
             *  to the playback buffers.
             */
            void *Source;

            /**
             *  This is the number of bytes remaining in the source data as
             *  pointed to by the "Source" element.
             */
            int Remainder;

            /**
             *  Samples maintain a priority which is used to determine
             *  which sounds live or die when the maximum number of
             *  sounds are being played.
             */
            int Priority;

            int Handle;

            int DSVolume;
    
            /**
             *  This is the current volume of the sample as it is being played.
             */
            int Volume;

            /**
             *  Amount to reduce volume per tick.
             */
            int Reducer;

            /**
             *  This is the compression that the sound data is using.
             */
            SCompressType Compression;

            /**
             *  This flag indicates whether this sample needs servicing.
             *  Servicing entails filling one of the empty low buffers.
             */
            bool Service;

            /**
             *  Streaming control handlers.
             */
            bool (*Callback)(short, short *, void **, int *);

            // Pointer to continued sample data.
            void *QueueBuffer;

            // Size of queue buffer attached.
            int QueueSize;

            // Block number tracker (0..StreamBufferCount-1).
            short Odd;

            // Number of buffers already filled ahead.
            int FilePending;

            // Number of bytes in last filled buffer.
            int FilePendingSize;

            /**
             *  The file variables are used when streaming directly off of the
             *  hard drive.
             */

            // Streaming file handle (ERROR = not in use).
            FileClass *FileHandle;

            // Temporary streaming buffer (allowed to be freed).
            void *FileBuffer;

            /**
             *  The following structure is used if the sample if compressed using
             *  the sos 16 bit compression codec.
             */
            _tagCOMPRESS_INFO2 sSOSInfo;

            /**
             *  
             */
            HANDLE BufferMutex;

        } SampleTrackerType;

    public:
        DirectSound8AudioClass();
        ~DirectSound8AudioClass();

        bool Init(HWND hWnd, int bits_per_sample, bool stereo, int rate);
        void End();

        bool Is_Available() const { return SoundObject != nullptr && !AudioDone; }

        void Stop_Sample(int handle);
        bool Sample_Status(int handle);
        bool Is_Sample_Playing(const void *sample);
        void Stop_Sample_Playing(const void *sample);
        int Play_Sample(const void *sample, int priority = PRIORITY_MAX, int volume = VOLUME_MAX);
        int File_Stream_Sample_Vol(char const *filename, int volume, bool real_time_start = false);
        void Fade_Sample(int handle, int ticks);
        int Get_Playing_Sample_Handle(const void *sample);

        bool Lock_Global_Mutex();
        bool Unlock_Global_Mutex();
        bool Lock_Timer_Mutex();
        bool Unlock_Timer_Mutex();
        bool Lock_Secondary_Mutex(int handle);
        bool Unlock_Secondary_Mutex(int handle);
        bool Lock_Mutex();
        bool Unlock_Mutex();

        void Gain_All(int a1);
        int Mute_All();

        void Set_Handle_Volume(int handle, int volume);
        void Set_Sample_Volume(const void *sample, int volume);

        int Set_Sound_Vol(int volume);
        int Set_Score_Vol(int volume);

        void sub_48A250(int handle, int volume);
        void sub_48A340(int handle, int volume);

        int Set_Volume_All(int vol);
        int Adjust_Volume_All(int vol_percent);

        bool Start_Primary_Sound_Buffer(bool forced = false);
        void Stop_Primary_Sound_Buffer();

        void Sound_Callback();

        int Get_Free_Sample_Handle(int priority);
        int Play_Sample_Handle(const void *sample, int priority, int volume, int handle);
        int Stream_Sample_Vol(void *buffer, int size, bool (*callbackptr)(short, short *, void **, int *), int volume, int handle);

        void File_Stream_Preload(int handle);

        int Sample_Copy(SampleTrackerType *st, void **source, int *src_size, void **alternate, int *alt_size, void *dest, int dst_size, int scomp);
        int Simple_Copy(void **source, int *src_size, void **alternate, int *alt_size, void **dest, int dst_size);

        bool Attempt_Audio_Restore(LPDIRECTSOUNDBUFFER buffer);
        int Attempt_To_Play_Buffer(int id);
        bool Set_Primary_Buffer_Format();
        void Restore_Sound_Buffers();

        LPDIRECTSOUND Get_Sound_Object() const { return SoundObject; }
        LPDIRECTSOUNDBUFFER Get_Primary_Buffer() const { return PrimaryBufferPtr; }

    private:
        void Sound_Maintenance_Callback();

        static bool File_Callback(short id, short *odd, void **buffer, int *size);
        static void CALLBACK Sound_Timer_Callback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

    public:
        void (*AudioFocusLossFunction)();

        bool AudioDone;

        bool StreamLowImpact;
        unsigned int MagicNumber;
        void *UncompBuffer;
        void *FileStreamBuffer;
        int StreamBufferSize;

        int SoundVolume;
        int ScoreVolume;

        bool field_380;

        HANDLE TimerMutex;
        HANDLE GlobalAudioMutex;

        UINT AudioTimerHandle;
        UINT AudioTimerResolution;

        /**
         *  The primary sound device.
         */
        LPDIRECTSOUND SoundObject;

        /**
         *  Primary sound buffer. This handles mixing of secondary buffers.
         */
        LPDIRECTSOUNDBUFFER PrimaryBufferPtr;

        LPWAVEFORMATEX PrimaryBufferFormat;
        LPDSBUFFERDESC PrimaryBufferDesc;

        DynamicVectorClass<SampleTrackerType *> SampleTracker;
};
