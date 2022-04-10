/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_UTIL.CPP
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
#include "audio_util.h"
#include "dsaudio.h"
#include "wwaud.h"
#include "wstring.h"
#include "ccfile.h"
#include "ramfile.h"
#include "vinifera_globals.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  Returns a handle to the requested music filename.
 * 
 *  @author: CCHyper
 */
FileClass *Get_Music_File_Handle(const char *filename)
{
    /**
     *  Search for the file in the additional search paths.
     */
    if (Vinifera_MusicPath_EnvVar[0] != '\0') {
        Wstring fname = Wstring(Vinifera_MusicPath_EnvVar) + Wstring(filename);
        if (RawFileClass(fname.Peek_Buffer()).Is_Available()) {
            DEV_DEBUG_INFO("Audio: Get_Music_File_Handle found file \"%s\" in Music enviroment path.\n", filename);
            return new RawFileClass(fname.Peek_Buffer());
        }
    }
    if (Vinifera_MusicPath[0] != '\0') {
        Wstring fname = Wstring(Vinifera_MusicPath) + Wstring(filename);
        if (RawFileClass(fname.Peek_Buffer()).Is_Available()) {
            DEV_DEBUG_INFO("Audio: Get_Music_File_Handle found file \"%s\" in Music path.\n", filename);
            return new RawFileClass(fname.Peek_Buffer());
        }
    }

    /**
     *  Search for the file in the mix files.
     */
    if (CCFileClass(filename).Is_Available()) {
        //DEV_DEBUG_INFO("Audio: Get_Music_File_Handle found file \"%s\" in a mix file.\n", filename);
        return new CCFileClass(filename);
    }

    return nullptr;
}


/**
 *  Returns a handle to the requested sound effect filename.
 * 
 *  @author: CCHyper
 */
FileClass *Get_Sfx_File_Handle(const char *filename)
{
    /**
     *  Search for the file in the additional search paths.
     */
    if (Vinifera_SoundsPath_EnvVar[0] != '\0') {
        Wstring fname = Wstring(Vinifera_SoundsPath_EnvVar) + Wstring(filename);
        if (RawFileClass(fname.Peek_Buffer()).Is_Available()) {
            DEV_DEBUG_INFO("Audio: Get_Sfx_File_Handle found file \"%s\" in Sounds enviroment path.\n", filename);
            return new RawFileClass(fname.Peek_Buffer());
        }
    }
    if (Vinifera_SoundsPath[0] != '\0') {
        Wstring fname = Wstring(Vinifera_SoundsPath) + Wstring(filename);
        if (RawFileClass(fname.Peek_Buffer()).Is_Available()) {
            DEV_DEBUG_INFO("Audio: Get_Sfx_File_Handle found file \"%s\" in Sounds path.\n", filename);
            return new RawFileClass(fname.Peek_Buffer());
        }
    }

    /**
     *  Search for the file in the mix files.
     */
    if (CCFileClass(filename).Is_Available()) {
        //DEV_DEBUG_INFO("Audio: Get_Sfx_File_Handle found file \"%s\" in a mix file.\n", filename);
        return new CCFileClass(filename);
    }

    return nullptr;
}


/**
 *  Checks if the input music file is available.
 * 
 *  @author: CCHyper
 */
bool Is_Music_File_Available(const char *filename)
{
    /**
     *  Search for the file in the additional search paths.
     */
    if (Vinifera_MusicPath_EnvVar[0] != '\0') {
        Wstring fname = Wstring(Vinifera_MusicPath_EnvVar) + Wstring(filename);
        if (RawFileClass(fname.Peek_Buffer()).Is_Available()) {
            DEV_DEBUG_INFO("Audio: Is_Music_File_Available found file \"%s\" in Sounds enviroment path.\n", filename);
            return true;
        }
    }
    if (Vinifera_MusicPath[0] != '\0') {
        Wstring fname = Wstring(Vinifera_MusicPath) + Wstring(filename);
        if (RawFileClass(fname.Peek_Buffer()).Is_Available()) {
            DEV_DEBUG_INFO("Audio: Is_Music_File_Available found file \"%s\" in Sounds path.\n", filename);
            return true;
        }
    }
    
    /**
     *  Search for the file in the mix files.
     */
    if (CCFileClass(filename).Is_Available()) {
        //DEV_DEBUG_INFO("Audio: Is_Music_File_Available found file \"%s\" in a mix file.\n", filename);
        return true;
    }

    return false;
}


/**
 *  Checks if the input sound effect file is available.
 * 
 *  @author: CCHyper
 */
bool Is_Sfx_File_Available(const char *filename)
{
    /**
     *  Search for the file in the additional search paths.
     */
    if (Vinifera_SoundsPath_EnvVar[0] != '\0') {
        Wstring fname = Wstring(Vinifera_SoundsPath_EnvVar) + Wstring(filename);
        if (RawFileClass(fname.Peek_Buffer()).Is_Available()) {
            DEV_DEBUG_INFO("Audio: Is_Sfx_File_Available found file \"%s\" in Sounds enviroment path.\n", filename);
            return true;
        }
    }
    if (Vinifera_SoundsPath[0] != '\0') {
        Wstring fname = Wstring(Vinifera_SoundsPath) + Wstring(filename);
        if (RawFileClass(fname.Peek_Buffer()).Is_Available()) {
            DEV_DEBUG_INFO("Audio: Is_Sfx_File_Available found file \"%s\" in Sounds path.\n", filename);
            return true;
        }
    }
    
    /**
     *  Search for the file in the mix files.
     */
    if (CCFileClass(filename).Is_Available()) {
        //DEV_DEBUG_INFO("Audio: Is_Sfx_File_Available found file \"%s\" in a mix file.\n", filename);
        return true;
    }

    return false;
}


/**
 *  Build a file name from the input name and extension.
 * 
 *  @author: CCHyper
 */
const char *Build_Audio_File_Name(const char *name, const char *ext)
{
    static char _buffer[_MAX_FNAME+_MAX_EXT];

    std::snprintf(_buffer, sizeof(_buffer), "%s.%s", name, ext);
    return (const char *)&_buffer;
}


/**
 *  Is the sample a Ogg file?
 * 
 *  @author: CCHyper
 */
bool Sample_Is_Ogg(const void *sample)
{
    const char *sample_cast = reinterpret_cast<const char *>(sample);
    return sample_cast[0] == 'O'
        && sample_cast[1] == 'g'
        && sample_cast[2] == 'g'
        && sample_cast[3] == 'S';
}


/**
 *  Is the sample a MP3 file?
 * 
 *  @author: CCHyper
 */
bool Sample_Is_Mp3(const void *sample)
{
    const char *sample_cast = reinterpret_cast<const char *>(sample);
    return (sample_cast[0] == 'I' && sample_cast[1] == 'D' && sample_cast[2] == '3') // ID tagged Mp3.
        || (sample_cast[0] == 0xFF && sample_cast[1] == 0xFB); // Mp3 header.
}


/**
 *  Is the sample a Westwood AUD file?
 * 
 *  @author: CCHyper
 */
bool Sample_Is_Aud(const void *sample)
{
    const AUDHeaderType *sample_cast = reinterpret_cast<const AUDHeaderType *>(sample);
    return sample_cast->Compression == SCOMP_SOS; // This is a real bad check, but its all we can do...
}


/**
 *  Is the sample a WAVE file?
 * 
 *  @author: CCHyper
 */
bool Sample_Is_Wav(const void *sample)
{
    const char *sample_cast = reinterpret_cast<const char *>(sample);
    return sample_cast[0] == 'W'
        && sample_cast[1] == 'A'
        && sample_cast[2] == 'V'
        && sample_cast[3] == 'E';
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AUD_Read_Reader(const void *sample, int &header_size, unsigned short &rate, int &size, int &uncomp_size, unsigned char &flags, unsigned char &comp)
{
    AUDHeaderType hdr;
    CopyMemory(&hdr, sample, sizeof(AUDHeaderType));

    header_size = sizeof(AUDHeaderType);

    rate = hdr.Rate;
    size = hdr.Size;
    uncomp_size = hdr.UncompSize;
    flags = hdr.Flags;
    comp = hdr.Compression;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int AUD_Simple_Copy(void **source, int *ssize, void **alternate, int *altsize, void **dest, int size)
{
    int out = 0;

    if (*ssize == 0) {
        *source = *alternate;
        *ssize = *altsize;
        *alternate = nullptr;
        *altsize = 0;
    }

    if (*source == nullptr || *ssize == 0) {
        return out;
    }

    int s = size;

    if (*ssize < size) {
        s = *ssize;
    }

    CopyMemory(*dest, *source, s);
    *source = static_cast<unsigned char *>(*source) + s;
    *ssize -= s;
    *dest = static_cast<unsigned char *>(*dest) + s;
    out = s;

    if ((size - s) == 0) {
        return out;
    }

    *source = *alternate;
    *ssize = *altsize;
    *alternate = nullptr;
    *altsize = 0;

    out = AUD_Simple_Copy(source, ssize, alternate, altsize, dest, (size - s)) + s;

    return out;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int AUD_Sample_Copy(SCompressType scomp, _SOS_COMPRESS_INFO_2 *sSOSInfo, void **source, int *ssize, void **alternate, int *altsize, void *dest, int size, void *uncomp_buffer)
{
    //ASSERT(st != nullptr);

    //if (st->TrackerType != TRACKER_AUD) {
    //    DEV_DEBUG_WARNING("Invalid sample tracker in AUD_Sample_Copy()!\n");
    //    return 0;
    //}

    /**
     *  There is no compression or it doesn't match any of the supported compressions so we just copy the data over.
     */
    if (scomp == SCOMP_NONE || (scomp != SCOMP_WESTWOOD && scomp != SCOMP_SOS)) {
        return AUD_Simple_Copy(source, ssize, alternate, altsize, &dest, size);
    }

    int datasize = 0;

    while (size > 0) {

        unsigned short comp_size;
        unsigned short decomp_size;
        unsigned magicnum;

        void *csz_ptr = &comp_size;
        void *dcsz_ptr = &decomp_size;
        void *mnum_ptr = &magicnum;

        /**
         *  Verify and seek over the chunk header.
         */
        if (AUD_Simple_Copy(source, ssize, alternate, altsize, &csz_ptr, sizeof(comp_size)) < sizeof(comp_size)) {
            break;
        }

        if (AUD_Simple_Copy(source, ssize, alternate, altsize, &dcsz_ptr, sizeof(decomp_size)) < sizeof(decomp_size) || decomp_size > size) {
            break;
        }

        if (AUD_Simple_Copy(source, ssize, alternate, altsize, &mnum_ptr, sizeof(magicnum)) < sizeof(magicnum) || magicnum != AUD_CHUNK_MAGIC_ID) {
            break;
        }

        if (comp_size == decomp_size) {

            // File size matches size to decompress, so there's nothing to do other than copy the buffer over.
            if (AUD_Simple_Copy(source, ssize, alternate, altsize, &dest, comp_size) < decomp_size) {
                return datasize;
            }

        } else {

            // Else we need to decompress it.
            if (AUD_Simple_Copy(source, ssize, alternate, altsize, &uncomp_buffer, comp_size) < comp_size) {
                return datasize;
            }

            switch (scomp) {
                default:
                    break;
                case SCOMP_WESTWOOD:
                    //Westwood_Unzap(uncomp_buffer, dest, decomp_size);
                    break;
                case SCOMP_SOS:
                    ASSERT(sSOSInfo != nullptr);
                    sSOSInfo->lpSource = (unsigned char *)uncomp_buffer;
                    sSOSInfo->lpDest = (unsigned char *)dest;
                    sosCODEC2DecompressData(sSOSInfo, decomp_size);
                    break;
            };

            dest = reinterpret_cast<char *>(dest) + decomp_size;
        }

        datasize += decomp_size;
        size -= decomp_size;
    }

    return datasize;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool WAV_Get_Info(const void *sample, short &channels, int &sample_rate, short &bits_per_sample, int &file_size, int &data_size)
{
#if 0
    char buffer[4];

    unsigned char *sample_ptr = (unsigned char *)sample;

    // the RIFF
    //if (!file.read(buffer, sizeof(buffer))) {
    //    DEBUG_ERROR("ERROR: could not read RIFF.\n");
    //    return false;
    //}
    if (std::strncmp((char *)sample_ptr, "RIFF", 4) != 0) {
        DEBUG_ERROR("ERROR: file is not a valid WAVE file (header doesn't begin with RIFF).\n");
        return false;
    }

    sample_ptr += 4;

    // the size of the file
    if (!file.read(buffer, sizeof(buffer))) {
        DEBUG_ERROR("ERROR: could not read size of file.\n");
        return false;
    }

    sample_ptr += 4;

    // the WAVE
    //if (!file.read(buffer, sizeof(buffer))) {
    //    DEBUG_ERROR("ERROR: could not read WAVE.\n");
    //    return false;
    //}
    if (std::strncmp((char *)sample_ptr, "WAVE", 4) != 0) {
        DEBUG_ERROR("ERROR: file is not a valid WAVE file (header doesn't contain WAVE).\n");
        return false;
    }

    // "fmt/0"
    //if (!file.read(buffer, sizeof(buffer))) {
    //    DEBUG_ERROR("ERROR: could not read fmt/0.\n");
    //    return false;
    //}

    // "fmt/0"
    sample_ptr += 4;

    // this is always 16, the size of the fmt data chunk
    //if (!file.read(buffer, sizeof(buffer))) {
    //    DEBUG_ERROR("ERROR: could not read the 16.\n");
    //    return false;
    //}

    sample_ptr += 4;

    // PCM should be 1?
    //if (!file.read(buffer, 2)) {
    //    DEBUG_ERROR("ERROR: could not read PCM.\n");
    //    return false;
    //}

    sample_ptr += 2;

    // the number of channels
    if (!file.read(buffer, 2)) {
        DEBUG_ERROR("ERROR: could not read number of channels.\n");
        return false;
    }
    channels = convert_to_int(buffer, 2);

    // sample rate
    if (!file.read(buffer, sizeof(buffer))) {
        DEBUG_ERROR("ERROR: could not read sample rate.\n");
        return false;
    }
    sampleRate = convert_to_int(buffer, 4);

    // (sampleRate * bitsPerSample * channels) / 8
    if (!file.read(buffer, sizeof(buffer))) {
        DEBUG_ERROR("ERROR: could not read (sampleRate * bitsPerSample * channels) / 8.\n");
        return false;
    }

    // ?? dafaq
    if (!file.read(buffer, 2)) {
        DEBUG_ERROR("ERROR: could not read dafaq.\n");
        return false;
    }

    // bitsPerSample
    if (!file.read(buffer, 2)) {
        DEBUG_ERROR("ERROR: could not read bits per sample.\n");
        return false;
    }
    bitsPerSample = convert_to_int(buffer, 2);

    // data chunk header "data"
    //if (!file.read(buffer, sizeof(buffer))) {
    //    DEBUG_ERROR("ERROR: could not read data chunk header.\n");
    //    return false;
    //}
    if (std::strncmp(buffer, "data", sizeof(buffer)) != 0) {
        DEBUG_ERROR("ERROR: file is not a valid WAVE file (doesn't have 'data' tag).\n");
        return false;
    }

    // size of data
    if(!file.read(buffer, sizeof(buffer))) {
        DEBUG_ERROR("ERROR: could not read data size.\n");
        return false;
    }
    size = convert_to_int(buffer, sizeof(buffer));

    /* cannot be at the end of file */
    //if (file.eof()) {
    //    DEBUG_ERROR("ERROR: reached EOF on the file.\n");
    //    return false;
    //}
    //if (file.fail()) {
    //    DEBUG_ERROR("ERROR: fail state set on the file.\n");
    //    return false;
    //}
#endif

    return true;
}
