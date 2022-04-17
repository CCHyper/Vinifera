/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VORBIS_GLOBALS.CPP
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
#include "vorbis_globals.h"


bool VorbisInitialised = false;

LPOVCLEAR ov_clear = nullptr;
LPOVFOPEN ov_fopen = nullptr;
LPOVOPEN ov_open = nullptr;
LPOVOPENCALLBACKS ov_open_callbacks = nullptr;
LPOVTEST ov_test = nullptr;
LPOVTESTCALLBACKS ov_test_callbacks = nullptr;
LPOVTESTOPEN ov_test_open = nullptr;
LPOVBITRATE ov_bitrate = nullptr;
LPOVBITRATE_INSTANT ov_bitrate_instant = nullptr;
LPOVSTREAMS ov_streams = nullptr;
LPOVSEEKABLE ov_seekable = nullptr;
LPOVSERIALNUMBER ov_serialnumber = nullptr;
LPOVRAWTOTAL ov_raw_total = nullptr;
LPOVPCMTOTAL ov_pcm_total = nullptr;
LPOVTIMETOTAL ov_time_total = nullptr;
LPOVRAWSEEK ov_raw_seek = nullptr;
LPOVPCMSEEK ov_pcm_seek = nullptr;
LPOVPCMSEEK_PAGE ov_pcm_seek_page = nullptr;
LPOVTIMESEEK ov_time_seek = nullptr;
LPOVTIMESEEKPAGE ov_time_seek_page = nullptr;
LPOVRAWSEEKLAP ov_raw_seek_lap = nullptr;
LPOVPCMSEEKLAP ov_pcm_seek_lap = nullptr;
LPOVPCMSEEKPAGELAP ov_pcm_seek_page_lap = nullptr;
LPOVTIMESEEKLAP ov_time_seek_lap = nullptr;
LPOVTIMESEEKPAGELAP ov_time_seek_page_lap = nullptr;
LPOVRAWTELL ov_raw_tell = nullptr;
LPOVPCMTELL ov_pcm_tell = nullptr;
LPOVTIMETELL ov_time_tell = nullptr;
LPOVINFO ov_info = nullptr;
LPOVCOMMENT ov_comment = nullptr;
LPOVREADFLOAT ov_read_float = nullptr;
LPOVREADFILTER ov_read_filter = nullptr;
LPOVREAD ov_read = nullptr;
LPOVCROSSLAP ov_crosslap = nullptr;
LPOVHALFRATE ov_halfrate = nullptr;
LPOVHALFRATE_P ov_halfrate_p = nullptr;
