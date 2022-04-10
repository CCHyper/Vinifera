/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_UTIL.H
 *
 *  @author        CCHyper
 *
 *  @brief         Various audio utility functions.
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
#include "always.h"
#include "dsaudio.h"


class FileClass;
struct SampleTrackerStruct;


FileClass *Get_Music_File_Handle(const char *filename);
FileClass *Get_Sfx_File_Handle(const char *filename);

bool Is_Music_File_Available(const char *filename);
bool Is_Sfx_File_Available(const char *filename);

const char *Build_Audio_File_Name(const char *name, const char *ext);

bool Sample_Is_Ogg(const void *sample);
bool Sample_Is_Mp3(const void *sample);
bool Sample_Is_Aud(const void *sample);
bool Sample_Is_Wav(const void *sample);


/**
 *  Utility functions for handling Westwood AUD files.
 */
void AUD_Read_Reader(const void *sample, int &header_size, unsigned short &rate, int &size, int &uncomp_size, unsigned char &flags, unsigned char &comp);
int AUD_Simple_Copy(void **source, int *ssize, void **alternate, int *altsize, void **dest, int size);
int AUD_Sample_Copy(SCompressType scomp, _SOS_COMPRESS_INFO_2 *sSOSInfo, void **source, int *ssize, void **alternate, int *altsize, void *dest, int size, void *uncomp_buffer);


/**
 *  Utility functions for handling PCM Wav files.
 */
bool WAV_Get_Info(const void *sample, short &channels, int &sample_rate, short &bits_per_sample, int &file_size, int &data_size);
