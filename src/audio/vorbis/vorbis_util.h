/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VORBIS_UTIL.H
 *
 *  @author        OpenAL SDK, CCHyper
 *
 *  @brief         Various Vorbis utility functions.
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
#include <vorbisfile.h>


const char *Get_Vorbis_Error(int code);
bool Is_Vorbis_Available();


/**
 *  Structure used by 
 */
struct mmio_file_struct
{
    char *curr_ptr; // Pointer to the current data in memory.
    char *file_ptr; // Pointer to the file in memory.
    int file_size; // Size of the data
};


/**
 *  
 */
#define vorbisCall(func_call) \
    { \
        int error = func_call ; \
        if (error < 0) { \
            DEBUG_ERROR("Vorbis: File: %s, Function: %s, Line: %d\n", __FILE__ __FUNCTION__, __LINE__); \
            DEBUG_ERROR("Vorbis: %s!\n", Get_Vorbis_Error(error)); \
        } \
    }


unsigned long OGG_APIENTRY ogg_vorbis_decode(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels);

size_t OGG_APIENTRY raw_ogg_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int OGG_APIENTRY raw_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin);
int OGG_APIENTRY raw_ogg_close_func(void *datasource);
long OGG_APIENTRY raw_ogg_tell_func(void *datasource);

size_t OGG_APIENTRY mix_ogg_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int OGG_APIENTRY mix_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin);
int OGG_APIENTRY mix_ogg_close_func(void *datasource);
long OGG_APIENTRY mix_ogg_tell_func(void *datasource);

size_t OGG_APIENTRY ram_ogg_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int OGG_APIENTRY ram_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin);
int OGG_APIENTRY ram_ogg_close_func(void *datasource);
long OGG_APIENTRY ram_ogg_tell_func(void *datasource);

size_t OGG_APIENTRY mmio_ogg_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int OGG_APIENTRY mmio_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin);
int OGG_APIENTRY mmio_ogg_close_func(void *datasource);
long OGG_APIENTRY mmio_ogg_tell_func(void *datasource);

size_t OGG_APIENTRY std_ogg_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int OGG_APIENTRY std_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin);
int OGG_APIENTRY std_ogg_close_func(void *datasource);
long OGG_APIENTRY std_ogg_tell_func(void *datasource);
