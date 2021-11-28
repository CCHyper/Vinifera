/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          OPENAL_AUDIO.H
 *
 *  @author        CCHyper
 *
 *  @contributors  tomsons26
 *
 *  @brief         Main header file for the OpenAL audio interface.
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
#include "wstring.h"
#include <al.h>
#include <alc.h>
#include <vorbisfile.h>


class FileClass;
class INIClass;

struct OggVorbis_File;
struct vorbis_info;


struct SampleTrackerStruct;


class OpenALAudioClass
{
    private:


    public:
        OpenALAudioClass();
        ~OpenALAudioClass();

        bool Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers = 16);
        void End();

        bool Is_Available() const { return Device != nullptr && !AudioDone; }

        void Stop_Sample(int handle);
        bool Sample_Status(int handle);
        bool Is_Sample_Playing(const void *sample);
        void Stop_Sample_Playing(const void *sample);

        int Play_Sample(const void *sample, int priority = PRIORITY_MAX, int volume = VOLUME_MAX);
        int File_Play_Sample(const char *filename, int priority = PRIORITY_MAX, int volume = VOLUME_MAX);
        int File_Stream_Sample_Vol(const char *filename, int volume, bool real_time_start = false);

        void Fade_Sample(int handle, int ticks = 60);
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

    private:
        /**
         *  Functions for handling Westwood Aud.
         */
        int Aud_Sample_Copy(SampleTrackerStruct *st, void **source, int *src_size, void **alternate, int *alt_size, void *dest, int dst_size, SCompressType scomp);
        int Aud_Simple_Copy(void **source, int *src_size, void **alternate, int *alt_size, void **dest, int dst_size);

        FileClass *Get_File_Handle(const char *filename);

        static bool File_Callback(short id, short *odd, void **buffer, int *size);

        void Sound_Maintenance_Callback();

    public:
        ALCdevice *Device;
        ALCcontext *Context;
        bool AudioDone;
        int StreamBufferSize;
        unsigned char *UncompBuffer;
        unsigned char *FileStreamBuffer;
        unsigned char *ChunkBuffer;
        bool IsUsingFileStreamBuffer;
        int SoundVolume;
        int ScoreVolume;
        DynamicVectorClass<SampleTrackerStruct *> SampleTracker;
};
