/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VORBIS_LOAD_DLL.H
 *
 *  @author        CCHyper
 *
 *  @brief         Utility functions for performing one-time loading of
 *                 Vorbis library functions from the DLL.
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
#include "vorbis_load_dll.h"
#include "vorbis_globals.h"
#include "wstring.h"
#include "winutil.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <Windows.h>
#include <vorbisfile.h>


#define OGG_DLL_NAME "ogg.dll"
#define VORBIS_DLL_NAME "vorbis.dll"
#define VORBISFILE_DLL_NAME "vorbisfile.dll"


#define LOAD_DLL_IMPORT(dll, type, func) \
    func = (type)GetProcAddress(dll, #func); \
    if (!func) { \
        DEBUG_ERROR("GetProcAddress failed to load %s (error: %d).\n", #func, GetLastError()); \
        /* return false; */ \
    }


/**
 *  Loaded Vorbis library.
 */
static HMODULE VorbisDLL = nullptr;


/**
 *  We successfully loaded all imports?
 */
bool VorbisImportsLoaded = false;


/**
 *  Load the Vorbis DLL and any imports we need.
 * 
 *  @author: CCHyper
 */
bool Load_Vorbis_DLL()
{
    /**
     *  We already performed a successful one-time init, return success.
     */
    if (VorbisImportsLoaded) {
        return true;
    }

    Wstring lib_name;

    lib_name = VORBISFILE_DLL_NAME;
    
    /**
     *  Look for the Vorbis DLL.
     */
    if (GetFileAttributesA(lib_name.Peek_Buffer()) != INVALID_FILE_ATTRIBUTES) {
        VorbisDLL = LoadLibrary(lib_name.Peek_Buffer());
    }

    if (!VorbisDLL) {
        DEBUG_ERROR("Audio: Failed to load Vorbis library!\n");
        VorbisImportsLoaded = false;
        return false;
    }

    DEBUG_INFO("Audio: Vorbis library \"%s\" found.\n", lib_name.Peek_Buffer());

    DEV_DEBUG_INFO("Load_Vorbis_DLL()\n");

    LOAD_DLL_IMPORT(VorbisDLL, LPOVCLEAR, ov_clear);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVFOPEN, ov_fopen);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVOPEN, ov_open);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVOPENCALLBACKS, ov_open_callbacks);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVTEST, ov_test);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVTESTCALLBACKS, ov_test_callbacks);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVTESTOPEN, ov_test_open);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVBITRATE, ov_bitrate);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVBITRATE_INSTANT, ov_bitrate_instant);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVSTREAMS, ov_streams);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVSEEKABLE, ov_seekable);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVSERIALNUMBER, ov_serialnumber);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVRAWTOTAL, ov_raw_total);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVPCMTOTAL, ov_pcm_total);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVTIMETOTAL, ov_time_total);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVRAWSEEK, ov_raw_seek);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVPCMSEEK, ov_pcm_seek);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVPCMSEEK_PAGE, ov_pcm_seek_page);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVTIMESEEK, ov_time_seek);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVTIMESEEKPAGE, ov_time_seek_page);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVRAWSEEKLAP, ov_raw_seek_lap);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVPCMSEEKLAP, ov_pcm_seek_lap);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVPCMSEEKPAGELAP, ov_pcm_seek_page_lap);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVTIMESEEKLAP, ov_time_seek_lap);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVTIMESEEKPAGELAP, ov_time_seek_page_lap);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVRAWTELL, ov_raw_tell);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVPCMTELL, ov_pcm_tell);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVTIMETELL, ov_time_tell);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVINFO, ov_info);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVCOMMENT, ov_comment);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVREADFLOAT, ov_read_float);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVREADFILTER, ov_read_filter);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVREAD, ov_read);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVCROSSLAP, ov_crosslap);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVHALFRATE, ov_halfrate);
    LOAD_DLL_IMPORT(VorbisDLL, LPOVHALFRATE_P, ov_halfrate_p);
    
    /**
     *  All loaded and ready to go.
     */
    VorbisImportsLoaded = true;
    
    return true;
}


/**
 *  Free the Vorbis library and all loaded pointers.
 * 
 *  @author: CCHyper
 */
void Unload_Vorbis_DLL()
{
    DEV_DEBUG_INFO("Unload_Vorbis_DLL()\n");

    FreeLibrary(VorbisDLL);
    
    ov_clear = nullptr;
    ov_read = nullptr;
    ov_pcm_total = nullptr;
    ov_info = nullptr;
    ov_comment = nullptr;
    ov_open_callbacks = nullptr;
    
    VorbisImportsLoaded = false;
}
