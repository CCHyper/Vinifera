/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          FMOD_AUDIO.H
 *
 *  @author        CCHyper
 *
 *  @contributors  tomsons26
 *
 *  @brief         Main header file for the FMOD audio interface.
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

#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>

#include <fmod_studio.hpp>
#include <fmod.hpp>


class FileClass;
class INIClass;


struct FMOD_Implementation
{
    FMOD_Implementation();
    ~FMOD_Implementation();

    void Update();

    FMOD::Studio::System *mpStudioSystem;
    FMOD::System *mpSystem;

    int mnNextChannelId;

    typedef map<string, FMOD::Sound *> SoundMap;
    typedef map<int, FMOD::Channel *> ChannelMap;
    typedef map<string, FMOD::Studio::EventInstance *> EventMap;
    typedef map<string, FMOD::Studio::Bank *> BankMap;

    BankMap mBanks;
    EventMap mEvents;
    SoundMap mSounds;
    ChannelMap mChannels;
};




class FMODAudioClass
{
    public:
        /**
         *  Define the different type of sound compression available.
         */
        typedef enum SCompressType
        {
            SCOMP_NONE = 0,     // No compression -- raw data.

            //SCOMP_WESTWOOD = 1, // Special sliding window delta compression.
            SCOMP_SOS = 99      // SOS frame compression.
        } SCompressType;

        /**
         *  Various values used by the audio engine.
         */
        enum AudioEnums
        {
            PRIORITY_MIN = 0,
            PRIORITY_MAX = 255,

            VOLUME_MIN = 0,
            VOLUME_MAX = 255,
            VOLUME_SCALE = 256,

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
             *  Numbers of FMOD buffers.
             */
            NUM_BUFFERS = 2,

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
            MAX_SFX = 63+1, // #BUGFIX: Sound effects get noticeably cut off in busy battles in the
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
             *  Closes the file handle.
             * 
             *  @author: CCHyper
             */
            void Close_File()
            {
                if (FileHandle) {
                    FileHandle->Close();
                    delete FileHandle;
                    FileHandle = nullptr;
                }
            }

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
             *  This flag indicates that there is more source data
             *  to copy to the play buffer.
             */
            bool IsMoreSource;

            /**
             *  This flag indicates that the entire sample fitted inside the
             *  direct sound secondary buffer.
             */
            bool IsOneShot;

            /**
             *  
             */
            bool IsLooping;

            /**
             *  
             */
            bool IsPaused;

            /**
             *  This flag indicates whether this sample needs servicing.
             *  Servicing entails filling one of the empty low buffers.
             */
            bool Service;

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

            /**
             *  A point in space that is the source of this sound.
             */
            ALuint Source;

            /**
             *  The format of the audio stream.
             */  
            ALenum Format;

            /**
             *  The Frequency of the audio stream.
             */            
            unsigned Frequency;

            /**
             *  A set of buffers.
             */
            ALuint AudioBuffers[NUM_BUFFERS];

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
             *  Pointer to the sound data that has not yet been copied
             *  to the playback buffers.
             */
            void *SourcePtr;

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
    
            /**
             *  This is the current volume of the sample as it is being played.
             */
            int Volume;

            /**
             *  Amount to reduce volume per tick when fading out.
             */
            int Reducer;

            /**
             *  
             */
            float Pitch;

            /**
             *  This is the compression that the sound data is using.
             */
            SCompressType Compression;

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
             *  Ogg Vorbis
             */
            bool IsOggVorbis;

            /**
             *  
             */
            OggVorbis_File *OggVorbisFile;

            /**
             *  
             */
            vorbis_info *OggVorbisInfo;

            /**
             *  
             */
            ov_callbacks OggVorbisCallbacks;

        } SampleTrackerType;

    public:
        FMODAudioClass();
        ~FMODAudioClass();

        bool Init(HWND hWnd, int bits_per_sample, bool stereo, int rate);
        void End();

        bool Is_Available() const { return FMODDevice != nullptr && !AudioDone; }

        void Stop_Sample(int handle);
        bool Sample_Status(int handle);
        bool Is_Sample_Playing(const void *sample);
        void Stop_Sample_Playing(const void *sample);

        int Play_Sample(const void *sample, int priority = PRIORITY_MAX, int volume = VOLUME_MAX);
        int File_Play_Sample(const char *filename, int priority = PRIORITY_MAX, int volume = VOLUME_MAX);
        int File_Stream_Sample_Vol(const char *filename, int volume, bool real_time_start = false);

        void Fade_Sample(int handle, int ticks);
        int Get_Playing_Sample_Handle(const void *sample);

        void Set_Handle_Loop(int handle, bool loop);
        void Set_Sample_Loop(const void *sample, bool loop);
        void Stop_Handle_Looping(int handle);
        void Stop_Sample_Looping(const void *sample);

        void Set_Score_Handle_Pause(int handle);
        void Set_Score_Handle_Resume(int handle);

        void Set_Handle_Pitch(int handle, float pitch);
        void Set_Sample_Pitch(const void *sample, float pitch);

        void Set_Handle_Pan(int handle, float pan);
        void Set_Sample_Pan(const void *sample, float pan);

        void Set_Handle_Volume(int handle, int volume);
        void Set_Sample_Volume(const void *sample, int volume);

        int Set_Sound_Vol(int volume);
        int Set_Score_Vol(int volume);

        void Set_Volume_All(int volume);
        int Set_Volume_Percent_All(int vol_percent);

        long Sample_Length(const void *sample);

        bool Start_Primary_Sound_Buffer(bool forced = false);
        void Stop_Primary_Sound_Buffer();

        void Sound_Callback();

        int Get_Free_Sample_Handle(int priority);
        int Play_Sample_Handle(const void *sample, int priority, int volume, int handle);
        int Stream_Sample_Vol(void *buffer, int size, bool (*callbackptr)(short, short *, void **, int *), int volume, int handle);

        void File_Stream_Preload(int handle);

        bool Load_Settings(INIClass &ini);
        bool Save_Settings(INIClass &ini);

    private:
        int Sample_Copy(SampleTrackerType *st, void **source, int *src_size, void **alternate, int *alt_size, void *dest, int dst_size, int scomp);
        int Simple_Copy(void **source, int *src_size, void **alternate, int *alt_size, void **dest, int dst_size);

        void Sound_Maintenance_Callback();

        static bool File_Callback(short id, short *odd, void **buffer, int *size);

        FileClass *Get_File_Handle(const char *filename);
        void Close_File();

    public:
        void (*AudioFocusLossFunction)();

        bool AudioDone;
        
        bool field_380;

        bool StreamLowImpact;

        unsigned int MagicNumber;

        void *UncompBuffer;
        void *FileStreamBuffer;
        int StreamBufferSize;

        int SoundVolume;
        int ScoreVolume;

        /**
         *  The primary sound device.
         */
        ALCdevice *FMODDevice;

        ALCcontext *FMODContext;

        DynamicVectorClass<SampleTrackerType *> SampleTracker;
};
