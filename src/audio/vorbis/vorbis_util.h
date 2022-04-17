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
#pragma once

#include "always.h"
#include <vorbisfile.h>


const char *Get_Vorbis_Error(int code);
bool Is_Vorbis_Available();


#if 0
/**
 *  Structure used by 
 */
struct mmio_file_struct
{
    char *curr_ptr; // Pointer to the current data in memory.
    char *file_ptr; // Pointer to the file in memory.
    int file_size; // Size of the data
};
#endif


/**
 *  Custom file manipulation routines for the C&C file io.
 */
extern ov_callbacks cc_ov_callbacks;


/**
 *  
 */
#define vorbisFailed(error)      (error < 0)

/**
 *  
 */
#define vorbisError(error) \
    { \
        DEBUG_ERROR("Vorbis: %s %d %s!\n", __FUNCTION__, __LINE__, Get_Vorbis_Error(error)); \
    }

/**
 *  
 */
#define vorbisCall(func_call) \
    { \
        int error = func_call ; \
        if (vorbisFailed(error)) { \
            vorbisError(error); \
        } \
    }



unsigned long OGG_APIENTRY ogg_vorbis_decode(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels);

size_t OGG_APIENTRY cc_ogg_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int OGG_APIENTRY cc_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin);
int OGG_APIENTRY cc_ogg_close_func(void *datasource);
long OGG_APIENTRY cc_ogg_tell_func(void *datasource);

#if 0
size_t OGG_APIENTRY mmio_ogg_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int OGG_APIENTRY mmio_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin);
int OGG_APIENTRY mmio_ogg_close_func(void *datasource);
long OGG_APIENTRY mmio_ogg_tell_func(void *datasource);

size_t OGG_APIENTRY std_ogg_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int OGG_APIENTRY std_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin);
int OGG_APIENTRY std_ogg_close_func(void *datasource);
long OGG_APIENTRY std_ogg_tell_func(void *datasource);
#endif
