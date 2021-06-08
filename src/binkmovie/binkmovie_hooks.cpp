/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BINKMOVIE_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the bink movie player.
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
#include "binkmovie_hooks.h"
#include "binkmovie.h"
#include "bink_load_dll.h"
#include "tibsun_globals.h"
#include "dsurface.h"
#include "iomap.h"
#include "session.h"
#include "playmovie.h"
#include "wwmouse.h"
#include "wwkeyboard.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Play a Bink movie.
 * 
 *  @author: CCHyper
 */
bool Play_Movie_As_Bink(const char *name, ThemeType theme, bool clear_before = true, bool stretch_allowed = true, bool clear_after = true)
{
    WWKeyboard->Clear();
    WWMouse->Hide_Mouse();

    /**
     *  Only play fullscreen movies in campaign/singleplayer!
     */
    if (!Session.Singleplayer_Game()) {
        return false;
    }

    if (clear_before) {
        HiddenSurface->Clear();
        GScreenClass::Blit(true, HiddenSurface);
    }

    /**
     *  
     */
    BinkMoviePlayer binkplayer(name);    
    if (!binkplayer.File_Loaded()) {
        return false;
    }

    /**
     *  Play the movie!
     */
    binkplayer.Play();

    if (clear_after) {
        HiddenSurface->Clear();
        GScreenClass::Blit(true, HiddenSurface);
    }
    
    WWMouse->Show_Mouse();
    WWKeyboard->Clear();

    Map.Flag_To_Redraw(2);
    
    return true;
}


/**
 *  Intercept to the games Play_Movie which checks if the Bink video
 *  file is available, falling back to VQA if not.
 * 
 *  @author: CCHyper
 */
static void _Play_Movie_Intercept(const char *name, ThemeType theme, bool clear_before, bool stretch_allowed, bool clear_after)
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
     *  Make sure the Bink library was loaded first.
     */
    if (BinkImportsLoaded) {

        char bink_buffer[32-4];
        std::snprintf(bink_buffer, sizeof(bink_buffer), "%s.BIK", upper_filename);

        /**
         *  If the movie exists as a .BIK, if so, play as Bink!
         */
        if (CCFileClass().Is_Available(bink_buffer)) {
            DEBUG_INFO("Play_Movie \"%s\" as Bink!\n", upper_filename);
            if (Play_Movie_As_Bink(bink_buffer, theme, clear_before, stretch_allowed, clear_after)) {
                return;
            }
        }
    }

    /**
     *  Only report this issue in developer mode.
     */
    if (!BinkImportsLoaded && Vinifera_DeveloperMode) {
        DEBUG_WARNING("Failed to play movie \"%s\" as Bink!\n", upper_filename);
    }

    char vqa_buffer[32-4];
    std::snprintf(vqa_buffer, sizeof(vqa_buffer), "%s.VQA", upper_filename);

    /**
     *  The movie did not exist as a .BIK or failed to play, attempt to play the .VQA.
     */
    if (CCFileClass().Is_Available(vqa_buffer)) {
        DEBUG_INFO("Play_Movie \"%s\" as VQA!\n", upper_filename);

        /**
         *  Call the games VQA Play_Movie.
         */
        Play_Movie(vqa_buffer, theme, clear_before, stretch_allowed, clear_after);
        
    } else {
        DEBUG_ERROR("Failed to play movie \"%s\"!\n", upper_filename);
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
#if 0
bool Windows_Procedure_Is_Movie_Playing_Patch()
{
    return Current_Movie_Ptr != nullptr || IngameVQ.Count() > 0;
}


void _Movie_Handle_Focus_Intercept(bool state)
{
    if (BinkFullscreenMovie || BinkIngameMovie) {
        BinkMovie_Pause(state);
    } else {
        Movie_Handle_Focus(state);
    }
}
#endif


/**
 *  Main function for patching the hooks.
 */
void BinkMovie_Hooks()
{
    /**
     *  Patch in the main Play_Movie interceptor.
     */
    Patch_Call(0x004E07AD, _Play_Movie_Intercept);
    Patch_Call(0x004E07CC, _Play_Movie_Intercept);
    Patch_Call(0x004E0840, _Play_Movie_Intercept);
    Patch_Call(0x004E2865, _Play_Movie_Intercept);
    Patch_Call(0x004E287F, _Play_Movie_Intercept);
    Patch_Call(0x00563A1C, _Play_Movie_Intercept);
    Patch_Call(0x0057FEDA, _Play_Movie_Intercept);
    Patch_Call(0x0057FF3F, _Play_Movie_Intercept);
    Patch_Call(0x005DB314, _Play_Movie_Intercept);
    Patch_Call(0x005E35C8, _Play_Movie_Intercept);

//    Patch_Call(0x0061A90B, _Play_Ingame_Movie_Intercept);
//    Patch_Call(0x0061BF23, _Play_Ingame_Movie_Intercept);

//    Patch_Call(0x005B8F20, _RadarClass_Play_Movie_Intercept);

//    Patch_Call(0x00685CEE, Windows_Procedure_Is_Movie_Playing_Patch);

//    Patch_Call(0x00685CF7, _MovieClass_Update_Intercept);

//    Patch_Call(0x005B9144, _RadarClass_AI_Intercept);
//    Patch_Byte(0x005B914B, 0x74); // jz

//    Patch_Call(0x005DB52E, _Movie_Handle_Focus_Intercept);
//    Patch_Jump(0x005DB602, _Movie_Handle_Focus_Intercept);
//    Patch_Call(0x0068598F, _Movie_Handle_Focus_Intercept);
//    Patch_Call(0x00685B97, _Movie_Handle_Focus_Intercept);
//    Patch_Call(0x00685EA0, _Movie_Handle_Focus_Intercept);
}
