/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_PLAYMOVIE.CPP
 *
 *  @authors       CCHyper
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
#include "vinifera_playmovie.h"
#include "tibsun_globals.h"
#include "playmovie.h"
#include "ccfile.h"
#include "wvp.h"
#include "debughandler.h"
#include "asserthandler.h"


extern bool Play_Windows_Movie(const char *name, ThemeType theme, bool clear_before, bool stretch_allowed, bool clear_after);


/**
 *  Intercept to the games Play_Movie which checks if the DirectShow video
 *  file is available, falling back to VQA if not.
 * 
 *  @author: CCHyper
 */
void Vinifera_Play_Movie(const char *name, ThemeType theme, bool clear_before, bool stretch_allowed, bool clear_after)
{
    static char filename_buffer[32];
    std::strncpy(filename_buffer, name, sizeof(filename_buffer));
    
    /**
     *  Find the location of the file extension separator.
     */
    char *movie_name = std::strchr((char *)filename_buffer, '.');
    
    /**
     *  Unexpected filename format passed in?
     */
    if (!movie_name) {
        DEBUG_ERROR("Invalid movie filename \"%s\"!\n", filename_buffer);
        return;
    }

    /**
     *  Insert a null-char where the "." was. This will give us the actual
     *  movie name without the extension, allowing us to rebuild them.
     */
    *movie_name = '\0';

    const char *upper_filename = strupr((char *)filename_buffer);

    /**
     *  Attempt to play the video using the new video handler.
     */
    if (Play_Windows_Movie(upper_filename, theme, clear_before, stretch_allowed, clear_after)) {
        return;
    } else {
        DEV_DEBUG_WARNING("Failed to play \"%s\" using DirectShow!\n", upper_filename);
    }

    char vqa_buffer[32-4];
    std::snprintf(vqa_buffer, sizeof(vqa_buffer), "%s.VQA", upper_filename);

    /**
     *  The movie did not exist using a new video player or failed to play, attempt to play the .VQA.
     */
    if (CCFileClass(vqa_buffer).Is_Available()) {
        DEBUG_INFO("Play_Movie \"%s\" as VQA!\n", upper_filename);

        /**
         *  Call the games VQA Play_Movie.
         */
        Play_Movie(vqa_buffer, theme, clear_before, stretch_allowed, clear_after);
        
    } else {
        DEBUG_ERROR("Failed to play movie \"%s\" as VQA!\n", upper_filename);
    }
}
