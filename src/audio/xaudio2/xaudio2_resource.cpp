/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          XAUDIO2_DRIVER.CPP
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
#include "xaudio2_resource.h"
#include "xaudio2_stream.h"
#include "vinifera_globals.h"
#include "audio_util.h"
#include "ccfile.h"
#include "crc32.h"
#include "debughandler.h"
#include "asserthandler.h"


static const char *XAudio2FileExtensions[FORMAT_COUNT] = {
    ".OGG", // FORMAT_OGG
};


/**
 *  Is the sample a Ogg file?
 * 
 *  @author: CCHyper
 */
static bool Sample_Is_Ogg(const void *sample)
{
    const char *sample_cast = reinterpret_cast<const char *>(sample);
    return sample_cast[0] == 'O'
        && sample_cast[1] == 'g'
        && sample_cast[2] == 'g'
        && sample_cast[3] == 'S';
}


/**
 *  Returns a handle to the requested music filename.
 * 
 *  @author: CCHyper
 */
static std::unique_ptr<CCFileClass> XAudio2_Get_Audio_File_Handle(Wstring filename)
{
    CCFileClass file;
    Wstring tmp;

    for (int i = 0; i < FORMAT_COUNT; ++i) {

        Wstring fname = filename + XAudio2FileExtensions[i];
        
#ifndef NDEBUG
        /**
         *  Search for the file in the additional search paths.
         */
        if (Vinifera_MusicPath_EnvVar[0] != '\0') {
            tmp = Wstring(Vinifera_MusicPath_EnvVar) + fname;
            file.Set_Name(tmp.Peek_Buffer());
            if (file.Is_Available()) {
                //DEV_DEBUG_INFO("XAudio2: Get_File_Handle found file \"%s\" in Music enviroment path.\n", fname.Peek_Buffer());
                return std::make_unique<CCFileClass>(tmp.Peek_Buffer());
            }
        }
        if (Vinifera_SoundsPath_EnvVar[0] != '\0') {
            tmp = Wstring(Vinifera_SoundsPath_EnvVar) + fname;
            file.Set_Name(tmp.Peek_Buffer());
            if (file.Is_Available()) {
                //DEV_DEBUG_INFO("XAudio2: Get_File_Handle found file \"%s\" in Sounds enviroment path.\n", fname.Peek_Buffer());
                return std::make_unique<CCFileClass>(tmp.Peek_Buffer());
            }
        }
#endif
#if !defined(NDEBUG) || defined(TS_CLIENT)
        if (Vinifera_MusicPath[0] != '\0') {
            tmp = Wstring(Vinifera_MusicPath) + fname;
            file.Set_Name(tmp.Peek_Buffer());
            if (file.Is_Available()) {
                //DEV_DEBUG_INFO("XAudio2: Get_File_Handle found file \"%s\" in Music path.\n", fname.Peek_Buffer());
                return std::make_unique<CCFileClass>(tmp.Peek_Buffer());
            }
        }
        if (Vinifera_SoundsPath[0] != '\0') {
            tmp = Wstring(Vinifera_SoundsPath) + fname;
            file.Set_Name(tmp.Peek_Buffer());
            if (file.Is_Available()) {
                //DEV_DEBUG_INFO("XAudio2: Get_File_Handle found file \"%s\" in Sound path.\n", fname.Peek_Buffer());
                return std::make_unique<CCFileClass>(tmp.Peek_Buffer());
            }
        }
#endif

        /**
         *  Search for the file in the mix files.
         */
        if (CCFileClass(fname.Peek_Buffer()).Is_Available()) {
            //DEV_DEBUG_INFO("XAudio2: Get_File_Handle found file \"%s\" in a mix file.\n", filename);
            return std::make_unique<CCFileClass>(fname.Peek_Buffer());
        }

    }

    return nullptr;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2CCSoundResource::Load(Wstring fname)
{
    if (fname.Is_Empty()) {
        return false;
    }

    // Is the resource already loaded and available?
    if (IsAvailable) {
        return true;
    }
        
    fname.To_Upper();

    std::unique_ptr<CCFileClass> fh = XAudio2_Get_Audio_File_Handle(fname);
    if (!fh) {
        return false;
    }

    if (!fh->Is_Available()) {
        //delete fh;
        return false;
    }

    int file_size = fh->Size();
    if (file_size <= 0) {
        //delete fh;
        return false;
    }

    // Read in some bytes to find out the file format.
    char buffer[16];
    int read = fh->Read(&buffer, sizeof(buffer));
    if (read <= 0) {
        //delete fh;
        return false;
    }

    if (Sample_Is_Ogg(buffer)) {
        Type = FORMAT_OGG;

    } else {
        Type = FORMAT_NONE;
        return false;
    }

    Name = fname;
    Name.To_Upper();

    FullName = fh->File_Name();
    FullName.To_Upper();

    IsAvailable = true;

#ifndef NDEBUG
    //DEV_DEBUG_INFO("XAudio2: Loaded sound resource \"%s\".\n", FilenameExt.Peek_Buffer());
#endif

    fh->Close();
    //delete fh;

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
std::unique_ptr<XAudio2Stream> XAudio2_Create_Sample_From_Resource(XAudio2SoundResource *res)
{
    ASSERT(res != nullptr);

    switch (res->Get_Type()) {
        case FORMAT_OGG:
            return std::make_unique<OggStream>(res);

        default:
            break;
    };

    return nullptr;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2_Is_File_Available(Wstring filename)
{
    CCFileClass file;
    Wstring tmp;

    for (int i = 0; i < FORMAT_COUNT; ++i) {

        Wstring fname = filename + XAudio2FileExtensions[i];

        /**
         *  Search for the file in the additional search paths.
         */
#ifndef NDEBUG
        if (Vinifera_MusicPath_EnvVar[0] != '\0') {
            tmp = Wstring(Vinifera_MusicPath_EnvVar) + fname;
            file.Set_Name(tmp.Peek_Buffer());
            if (file.Is_Available()) {
                return true;
            }
        }
        if (Vinifera_SoundsPath_EnvVar[0] != '\0') {
            tmp = Wstring(Vinifera_SoundsPath_EnvVar) + fname;
            file.Set_Name(tmp.Peek_Buffer());
            if (file.Is_Available()) {
                return true;
            }
        }
#endif
#if !defined(NDEBUG) || defined(TS_CLIENT)
        if (Vinifera_MusicPath[0] != '\0') {
            tmp = Wstring(Vinifera_MusicPath) + fname;
            file.Set_Name(tmp.Peek_Buffer());
            if (file.Is_Available()) {
                return true;
            }
        }
        if (Vinifera_SoundsPath[0] != '\0') {
            tmp = Wstring(Vinifera_SoundsPath) + fname;
            file.Set_Name(tmp.Peek_Buffer());
            if (file.Is_Available()) {
                return true;
            }
        }
#endif

        /**
         *  Search for the file in the mix files.
         */
        file.Set_Name(fname.Peek_Buffer());
        if (file.Is_Available()) {
            return true;
        }

    }

    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
unsigned int XAudio2_Get_Filename_Hash(Wstring filename)
{
    filename.To_Upper();
    return CRC32_String(filename.Peek_Buffer());
}
