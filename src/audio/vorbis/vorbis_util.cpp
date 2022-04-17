/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VORBIS_UTIL.CPP
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
#include "vorbis_util.h"
#include "vorbis_globals.h"
#include "vorbis_load_dll.h"
#include "ccfile.h"
#include "ramfile.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <iostream>
#include <fstream>


ov_callbacks cc_ov_callbacks
{
    &cc_ogg_read_func,
    nullptr, /* &cc_ogg_seek_func, */     // close not required.
    nullptr, /* &cc_ogg_close_func, */     // close not required.
    &cc_ogg_tell_func
};


/**
 *  x
 * 
 *  @author: CCHyper
 * 
 *  @see: https://github.com/xiph/vorbis/blob/master/doc/vorbis-errors.txt
 */
const char *Get_Vorbis_Error(int code)
{
    switch(code) {

        case OV_EREAD:
            return "OV_EREAD: A read from media returned an error.";
        case OV_EFAULT:
            return "OV_EFAULT: Internal logic fault.";
        case OV_EIMPL:
            return "OV_EIMPL: The bitstream makes use of a feature not implemented in this library version.";
        case OV_EINVAL:
            return "OV_EINVAL: Invalid argument value.";
        case OV_ENOTVORBIS:
            return "OV_ENOTVORBIS: Bitstream/page/packet is not Vorbis data.";
        case OV_EBADHEADER:
            return "OV_EBADHEADER: Invalid Vorbis bitstream header.";
        case OV_EVERSION:
            return "OV_EVERSION: Vorbis version mismatch.";
        case OV_ENOTAUDIO:
            return "OV_ENOTAUDIO: Packet data submitted to vorbis_synthesis is not audio data.";
        case OV_EBADPACKET:
            return "OV_EBADPACKET: Invalid packet submitted to vorbis_synthesis.";
        case OV_EBADLINK:
            return "OV_EBADLINK: Invalid stream section supplied, or the requested link is corrupt ";
        case OV_ENOSEEK:
            return "OV_ENOSEEK: Bitstream is not seekable.";
        default:
            return "Unknown error";
    };
}


bool Is_Vorbis_Available()
{
    return VorbisImportsLoaded;
}


unsigned long OGG_APIENTRY ogg_vorbis_decode(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels)
{
    int current_section;
    long lDecodeSize;
    unsigned long ulSamples;
    short *pSamples;

    unsigned long ulBytesDone = 0;

    while (true) {
        lDecodeSize = ov_read(psOggVorbisFile, pDecodeBuffer + ulBytesDone, ulBufferSize - ulBytesDone, 0, 2, 1, &current_section);
        if (lDecodeSize > 0) {
            ulBytesDone += lDecodeSize;
            if (ulBytesDone >= ulBufferSize) {
                break;
            }
        } else {
            break;
        }
    }

    // Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
    // however 6-Channels files need to be re-ordered
    if (ulChannels == 6) {		
        pSamples = (short *)pDecodeBuffer;
        for (ulSamples = 0; ulSamples < (ulBufferSize >> 1); ulSamples += 6) {
            // WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
            // OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
            std::swap(pSamples[ulSamples+1], pSamples[ulSamples+2]);
            std::swap(pSamples[ulSamples+3], pSamples[ulSamples+5]);
            std::swap(pSamples[ulSamples+4], pSamples[ulSamples+5]);
        }
    }

    return ulBytesDone;
}


/**
 *  Ogg Vorbis Callback Functions.
 * 
 * 
 *  Read:
 *    Function used to read data from memory.
 *    
 *    ptr = Pointer to the buffer that the vorbis files need.
 *    size = How big a byte is.
 *    count = How much we should read.
 *    datasource = This is a pointer to the data we passed into ov_open_callbacks.
 * 
 * 
 *  Seek:
 *    Function used to seek to a specific part of the file in memory.
 * 
 *    datasource = This is a pointer to the data we passed into ov_open_callbacks.
 *    offset = The offset from the point we wish to seek to.
 *    origin = Where we want to seek to.
 * 
 * 
 *  Close:
 *    Function used to close the file in memory.
 * 
 *    datasource = This is a pointer to the data we passed into ov_open_callbacks.
 * 
 * 
 *  Tell:
 *    Function used to tell how much we have read so far.
 * 
 *    datasource = This is a pointer to the data we passed into ov_open_callbacks.
 */


/**
 *  Ogg vorbis callback functions that use the C&C engine file io.
 * 
 *  @author: CCHyper
 */
size_t OGG_APIENTRY cc_ogg_read_func(void *ptr, size_t size, size_t count, void *datasource)
{
    ASSERT(datasource != nullptr);

    FileClass *file = static_cast<FileClass *>(datasource);

#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** cc_ogg_read_func(%s)(0x%p,%d,%d,0x%p).\n", file->File_Name(), ptr, size, count, datasource);
#endif

    if (!file->Is_Open()) {
        //DEBUG_WARNING("Vorbis: Opening \"%s\".\n", file->File_Name());
        file->Open(FILE_ACCESS_READ);
    }

    return file->Read(ptr, count);
}

int OGG_APIENTRY cc_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin)
{
    ASSERT(datasource != nullptr);

    FileClass *file = static_cast<FileClass *>(datasource);

#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** cc_ogg_seek_func(%s)(0x%p,%d,%d).\n", file->File_Name(), datasource, offset, origin);
#endif

    if (!file->Is_Open()) {
        //DEBUG_WARNING("Vorbis: Opening \"%s\".\n", file->File_Name());
        file->Open(FILE_ACCESS_READ);
    }

    return file->Seek(offset, (FileSeekType)origin);
}

int OGG_APIENTRY cc_ogg_close_func(void *datasource)
{
#if 0 // The handle must not be closed as the audio driver closes it, so this is just a null function.
    ASSERT(datasource != nullptr);

    FileClass *file = static_cast<FileClass *>(datasource);

#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** cc_ogg_close_func(%s)(0x%p).\n", file->File_Name(), datasource);
#endif

    file->Close();
#endif

    return 0;
}

long OGG_APIENTRY cc_ogg_tell_func(void *datasource)
{
    ASSERT(datasource != nullptr);
    
    FileClass *file = static_cast<FileClass *>(datasource);

#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** cc_ogg_tell_func(%s)(0x%p).\n", file->File_Name(), datasource);
#endif

    if (!file->Is_Open()) {
        //DEBUG_WARNING("Vorbis: Opening \"%s\".\n", file->File_Name());
        file->Open(FILE_ACCESS_READ);
    }

    return file->Seek(0, FILE_SEEK_CURRENT);
}


#if 0
/**
 *  Ogg vorbis callback functions for use with preloaded files.
 * 
 *  @author: CCHyper (Based off code from Doom 3)
 */
size_t OGG_APIENTRY mmio_ogg_read_func(void *ptr, size_t size, size_t count, void *datasource)
{
#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** mmio_ogg_read_func(0x%p,%d,%d,0x%p).\n", ptr, size, count, datasource);
#endif

    mmio_file_struct *of = reinterpret_cast<mmio_file_struct *>(datasource);
    int len = size * count;
    if (of->curr_ptr + len > of->file_ptr + of->file_size) {
        len = of->file_ptr + of->file_size - of->curr_ptr;
    }
    std::memcpy(ptr, of->curr_ptr, len);
    of->curr_ptr += len;
    return len;
}


int OGG_APIENTRY mmio_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin)
{
#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** mmio_ogg_seek_func(0x%p,%d,%d).\n", datasource, offset, origin);
#endif

    mmio_file_struct *of = reinterpret_cast<mmio_file_struct *>(datasource);

    switch (origin) {
        case SEEK_CUR:
            of->curr_ptr += offset;
            break;
        case SEEK_END:
            of->curr_ptr = of->file_ptr + of->file_size - offset;
            break;
        case SEEK_SET:
            of->curr_ptr = of->file_ptr + offset;
            break;
        default:
            return -1;
    }
    if (of->curr_ptr < of->file_ptr) {
        of->curr_ptr = of->file_ptr;
        return -1;
    }
    if (of->curr_ptr > of->file_ptr + of->file_size) {
        of->curr_ptr = of->file_ptr + of->file_size;
        return -1;
    }
    return 0;
}


int OGG_APIENTRY mmio_ogg_close_func(void *datasource)
{
#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** mmio_ogg_close_func(0x%p).\n", datasource);
#endif

    return 0;
}


long OGG_APIENTRY mmio_ogg_tell_func(void *datasource)
{
#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** mmio_ogg_tell_func(0x%p).\n", datasource);
#endif

    mmio_file_struct *of = reinterpret_cast<mmio_file_struct *>(datasource);
    return (of->curr_ptr - of->file_ptr);
}


/**
 *  Ogg vorbis callback functions that use the std stream.
 * 
 *  @author: CCHyper (Based off code from StackOverflow)
 */
size_t OGG_APIENTRY std_ogg_read_func(void *ptr, size_t size, size_t count, void *datasource)
{
#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** std_ogg_read_func(0x%p,%d,%d,0x%p).\n", ptr, size, count, datasource);
#endif

    std::ifstream &stream = *static_cast<std::ifstream *>(datasource);
    stream.read(static_cast<char *>(ptr), count);
    const std::streamsize bytesRead = stream.gcount();
    stream.clear(); // In case we read past EOF
    return static_cast<size_t>(bytesRead);
}

int OGG_APIENTRY std_ogg_seek_func(void *datasource, ogg_int64_t offset, int origin)
{
#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** std_ogg_seek_func(0x%p,%d,%d).\n", datasource, offset, origin);
#endif

    static const std::ios_base::seek_dir _seek_directions[] = {
        std::ios_base::beg, std::ios_base::cur, std::ios_base::end
    };

    std::ifstream &stream = *static_cast<std::ifstream *>(datasource);
    stream.seekg(offset, _seek_directions[origin]);
    stream.clear(); // In case we seeked to EOF
    return 0;
}

int OGG_APIENTRY std_ogg_close_func(void *datasource)
{
#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** std_ogg_close_func(0x%p).\n", datasource);
#endif

    return 0;
}

long OGG_APIENTRY std_ogg_tell_func(void *datasource)
{
#ifndef NDEBUG
    //DEV_DEBUG_INFO("Vorbis: ***** std_ogg_tell_func(0x%p).\n", datasource);
#endif

    std::ifstream &stream = *static_cast<std::ifstream *>(datasource);
    const auto position = stream.tellg();
    ASSERT(position >= 0);
    return static_cast<long>(position);
}
#endif
