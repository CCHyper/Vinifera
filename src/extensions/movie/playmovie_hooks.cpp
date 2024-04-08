/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          PLAYMOVIE_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks related to Play_Movie and related functions.
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
#include "playmovie_hooks.h"
#include "tibsun_globals.h"
#include "options.h"
#include "campaign.h"
#include "campaignext.h"
#include "scenario.h"
#include "vqa.h"
#include "movie.h"
#include "playmovie.h"
#include "cd.h"
#include "wstring.h"
#include "binkmovie.h"
#include "binkloaddll.h"
#include "dsurface.h"
#include "iomap.h"
#include "session.h"
#include "vox.h"
#include "dsaudio.h"
#include "wwmouse.h"
#include "wwkeyboard.h"
#include "extension.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"



#include "tspp.h"
DEFINE_IMPLEMENTATION(void Movie_Update(), 0x005646E0);
DEFINE_IMPLEMENTATION(void Movie_Handle_Focus(bool), 0x00563CC0);
void *&Current_Movie_Ptr = Make_Global<void *>(0x00806E1C);
bool &InScenario = Make_Global<bool>(0x007E48FC);


/**
 *  Global instance of the Bink movie player used for sidebar videos.
 */
static BinkMoviePlayer IngameBinkPlayer;


/**
 *  This fixes an issue with some cnc-ddraw setups not redrawing
 *  the sidebar area when a movie is playing.
 * 
 *  @author: CCHyper
 */
static void Redraw_Sidebar_Area()
{
    Rect dest_rect;
    dest_rect.X = PrimarySurface->Width-SidebarSurface->Width;
    dest_rect.Y = 0;
    dest_rect.Width = SidebarSurface->Width;
    dest_rect.Height = SidebarSurface->Height;
        
    Rect dest_clip;
    dest_clip.X = PrimarySurface->Width-SidebarSurface->Width;
    dest_clip.Y = 0;
    dest_clip.Width = SidebarSurface->Width;
    dest_clip.Height = 160;
        
    Rect sidebar_rect;
    sidebar_rect.X = 0;
    sidebar_rect.Y = 0;
    sidebar_rect.Width = SidebarSurface->Width;
    sidebar_rect.Height = SidebarSurface->Height;
        
    Rect sidebar_clip;
    sidebar_clip.X = 0;
    sidebar_clip.Y = 0;
    sidebar_clip.Width = SidebarSurface->Width;
    sidebar_clip.Height = 160;
    
    PrimarySurface->Copy_From(dest_rect, *SidebarSurface, sidebar_rect);
}


/**
 *  Play a Bink movie file.
 * 
 *  @author: CCHyper
 */
static bool Play_Movie_As_Bink(const char *name, ThemeType theme, bool clear_before = true, bool stretch_allowed = true, bool clear_after = true)
{
    WWKeyboard->Clear();
    WWMouse->Hide_Mouse();

    /**
     *  Only play fullscreen movies in campaign/singleplayer!
     */
    if (!Session.Singleplayer_Game()) {
        return false;
    }

    /**
     *  The movie is about to start, do we need to clear the screen?
     */
    if (clear_before) {
        HiddenSurface->Clear();
        GScreenClass::Blit(true, HiddenSurface);
    }

    /**
     *  Create an instance of the Bink movie player.
     */
    BinkMoviePlayer binkplayer(name, stretch_allowed);
    if (!binkplayer.File_Loaded()) {
        DEBUG_ERROR("Failed to load Bink movie file '%s'!", name);
        return false;
    }

    binkplayer.IsClearScreenBefore = clear_before;

    /**
     *  Play the movie!
     */
    binkplayer.Play();

    /**
     *  The movie has finished, do we need to clear the screen?
     */
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
 *  Utility function for checking if a movie exists in any of the known locations.
 * 
 *  @author: CCHyper
 */
bool Vinifera_Is_Movie_Available(const char *name)
{
    Wstring vqa_name = name;
    
    /**
     *  Find the location of the file extension separator.
     */
    char *_movie_name = std::strchr((char *)vqa_name.Peek_Buffer(), '.');
    
    /**
     *  Unexpected filename format passed in?
     */
    if (!_movie_name) {
        return false;
    }

    /**
     *  Insert a null-char where the "." was. This will give us the actual
     *  movie name without the extension, allowing us to rebuild them.
     */
    *_movie_name = '\0';
    Wstring movie_name = _movie_name;

    movie_name.To_Upper();

    movie_name.Concat(".BIK");

    bool bink_available = CCFileClass(movie_name.Peek_Buffer()).Is_Available();

    /**
     *  Do a secondary check to see if the video is available in the sub directory.
     */
    const Wstring movies_dir_filename = BinkMoviePlayer::Get_Search_Path() + movie_name;
    if (RawFileClass(movies_dir_filename).Is_Available()) {
        return true;
    }

    /**
     *  Was the Bink video found in any of the loaded mix files?
     */
    if (bink_available) {
        return true;
    }

    /**
     *  Finally check if the VQA is available.
     */
    return CCFileClass(vqa_name).Is_Available();
}


/**
 *  Intercept to the games Play_Movie which checks if the Bink video
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
     *  Make sure the Bink library was loaded first.
     */
    if (BinkImportsLoaded) {

        char bink_buffer[32-4];
        std::snprintf(bink_buffer, sizeof(bink_buffer), "%s.BIK", upper_filename);

        bool bink_available = CCFileClass(bink_buffer).Is_Available();

        /**
         *  Do a secondary check to see if the video is available in the sub directory.
         */
        const Wstring movies_dir_filename = BinkMoviePlayer::Get_Search_Path() + bink_buffer;
        if (RawFileClass(movies_dir_filename).Is_Available()) {
            bink_available = true;
        }

        /**
         *  If the movie exists as a .BIK, if so, play as Bink!
         */
        if (bink_available) {
            DEBUG_INFO("Play_Movie \"%s\" as Bink!\n", upper_filename);
            if (Play_Movie_As_Bink(bink_buffer, theme, clear_before, stretch_allowed, clear_after)) {
                return;
            }
        }
    }

    /**
     *  Only report this issue in developer mode.
     */
    if (!BinkImportsLoaded) {
        DEV_DEBUG_WARNING("Failed to play movie \"%s\" as Bink!\n", upper_filename);
    }

    char vqa_buffer[32-4];
    std::snprintf(vqa_buffer, sizeof(vqa_buffer), "%s.VQA", upper_filename);

    /**
     *  The movie did not exist as a .BIK or failed to play, attempt to play the .VQA.
     */
    if (CCFileClass(vqa_buffer).Is_Available()) {
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
 *  Play a ingame bink movie.
 * 
 *  @author: CCHyper
 */
static bool Play_Ingame_Movie_As_Bink(const Wstring &filename)
{
    /**
     *  Load Bink dll imports if not already loaded.
     */
    if (!BinkImportsLoaded) {
        if (!Load_Bink_DLL()) {
            DEBUG_WARNING("Bink: Failed to load DLL!\n");
            return false;
        }
    }

    /**
     *  Destroy any existing movie playing.
     */
    IngameBinkPlayer.Reset();

    IngameBinkPlayer.IsIngameMovie = true;
    IngameBinkPlayer.IsCanBeStretched = false;
    IngameBinkPlayer.IsDebugOverlayEnabled = false;

    /**
     *  Make sure the video does not stop when the user presses the ESC key.
     */
    IngameBinkPlayer.IsBreakoutAllowed = false;

    Rect sidebar_rect = SidebarSurface->Get_Rect();
    if (Options.SidebarOn == SIDEBAR_SIDE_RIGHT) {
        sidebar_rect.X += CompositeSurface->Width;
    }

    /**
     *  Set video position. This must be performed before Open() so the Bink
     *  handle is created with this position to adjust it.
     */
    if (!IngameBinkPlayer.Set_Position(sidebar_rect, ((SidebarSurface->Width-140)/2)+1, 27)) {
        IngameBinkPlayer.Reset();
        return false;
    }

    /**
     *  Open a new Bink handle to the radar movie. 
     */
    IngameBinkPlayer.Open(filename);

    /**
     *  Set the drawing surface for the video playback to be relative to.
     */
    IngameBinkPlayer.Set_Surface(SidebarSurface);
    
    /**
     *  
     */
    if (!IngameBinkPlayer.File_Loaded()) {
        IngameBinkPlayer.Reset();
        return false;
    }

    IngameBinkPlayer.IsPlaying = true;

    return true;
}


//
// Intercept to the games Play_Ingame_Movie which checks if the Bink video file is 
// available, falling back to VQA if not.
//
static void _Play_Ingame_Movie_Intercept(VQType vqtype)
{
    if (vqtype == VQ_NONE || vqtype >= Movies.Count()) {
        return;
    }

    // Get pointer to movie name entry
    Wstring movie_name = Movies[vqtype];

    // Invalid filename
    if (movie_name.Is_Empty()) {
        DEBUG_WARNING("Invalid movie filename!\n");
        return;
    }

    movie_name.To_Upper();

    Wstring bink_buffer = movie_name + ".BIK";
    Wstring vqa_buffer = movie_name + ".VQA";

    bool bink_available = CCFileClass(bink_buffer).Is_Available();
    bool vqa_available = CCFileClass(vqa_buffer).Is_Available();

    /**
     *  Do a secondary check to see if the video is available in the sub directory.
     */
    const Wstring movies_dir_filename = BinkMoviePlayer::Get_Search_Path() + bink_buffer;
    if (RawFileClass(movies_dir_filename).Is_Available()) {
        bink_available = true;
    }
    
    // If the movie exists as a .BIK, if so, play as Bink!
    if (bink_available) {
        DEBUG_INFO("Play_Ingame_Movie \"%s\" as Bink!\n", movie_name.Peek_Buffer());
        Play_Ingame_Movie_As_Bink(bink_buffer);
        
    // The movie did not exist as a .BIK, attempt to play the .VQA.
    } else if (vqa_available) {
        DEBUG_INFO("Play_Ingame_Movie \"%s\" as VQA!\n", movie_name.Peek_Buffer());
        Play_Ingame_Movie(vqtype);

    } else {
        DEBUG_INFO("Failed to play ingame movie \"%s\"!\n", movie_name.Peek_Buffer());
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
static void _Movie_Handle_Focus_Intercept(bool state)
{
    if (IngameBinkPlayer.File_Loaded() && IngameBinkPlayer.IsIngameMovie) {
        IngameBinkPlayer.Pause(state);
    } else {
        //DEV_DEBUG_INFO("About to call Movie_Handle_Focus.\n");
        Movie_Handle_Focus(state);
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
static void _MovieClass_Update_Intercept()
{
    /**
     *  ingame
     */
    if (IngameBinkPlayer.File_Loaded() && IngameBinkPlayer.IsIngameMovie) {
        
        /**
         *  Clear the surfaces. This fixes a issue were the game can
         *  be seen if the user moves the window.
         */
        //HiddenSurface->Clear();
        //AlternateSurface->Clear();
        //CompositeSurface->Clear();
        //PrimarySurface->Clear();

        if (!IngameBinkPlayer.Has_Finished()) {

            /**
             *  Request the next frame.
             */
            IngameBinkPlayer.Advance_Frame(false);

            /**
             *  Draw the new frame.
             */
            //IngameBinkPlayer.Draw_Frame();

            // TEST!
            //Rect r(CompositeSurface->Width, 16, 160, 160);
            //PrimarySurface->Fill_Rect(SidebarSurface->Get_Rect(), r, DSurface::RGBA_To_Pixel(255,0,0));

        } else {
            /**
             *  Movie finished, cleanup.
             */
            IngameBinkPlayer.Reset();
            IngameBinkPlayer.Close();
        }
        
    /**
     *  vqa
     */
    } else {
        //DEV_DEBUG_INFO("About to call Movie_Update.\n");
        Movie_Update();
    }
}


static class RadarClassExt final : public RadarClass
{
    public:
        void Play_Movie_Intercept();
        void Play_Bink_Movie();

        void Play_VQA_Movie();
};

DEFINE_IMPLEMENTATION(void RadarClassExt::Play_VQA_Movie(), 0x005BCC40);


/**
 *  
 * 
 *  @author: CCHyper
 */
void RadarClassExt::Play_Movie_Intercept()
{
    if (IngameBinkPlayer.File_Loaded() && IngameBinkPlayer.IsIngameMovie) {
        Play_Bink_Movie();
    } else {
        Play_VQA_Movie();
    }
}


/**
 *  Handle the playback of a Bink movie on the sidebar radar.
 * 
 *  @author: CCHyper
 */
void RadarClassExt::Play_Bink_Movie()
{
    static bool _volume_adjusted = false; // Have we adjusted the game audio volume?
    static int _prev_volume; // Volume level to restore after video finishes.
    
    /**
     *  Debugging "One time" process at the start of the video playback.
     */
    static bool _playing_one_time = false;
    if (!_playing_one_time) {
        if (IngameBinkPlayer.Get_Filename()[0] != '\0') {
            DEBUG_INFO("Bink: Playing in-game movie \"%s\"\n", IngameBinkPlayer.Get_Filename());
        }
        _playing_one_time = true;
    }
    
    /**
     *  Reduce the game volume down so we can hear the radar video.
     */
    if (!_volume_adjusted && !Is_Speaking()) {
       ////////////// _prev_volume = Audio.Adjust_Volume_All(40); // Was 50.
        _volume_adjusted = true;
    }

    /**
     *  Advance the player by 1 frame, but don't draw.
     */
    IngameBinkPlayer.Advance_Frame(false);

    /**
     *  Redraw the sidebar area due to some cnc-ddraw setups not correctly
     *  rendering the sidebar while the video is playing. 
     */
    Rect check_area;
    check_area.X = CompositeSurface->Get_Width();
    check_area.Y = 16;
    check_area.Width = 160;
    check_area.Height = 160;
    if (!UserInputLocked && check_area.Is_Within(WWMouse->Get_Mouse_XY())) {
        WWMouse->Hide_Mouse();
    }
    Redraw_Sidebar_Area();
    if (!UserInputLocked && !check_area.Is_Within(WWMouse->Get_Mouse_XY())) {
        WWMouse->Show_Mouse();
    }

    /**
     *  Redraw the tactical map.
     */
    Flag_To_Redraw(2);
    
    /**
     *  Draw the next frame.
     */
    if (!IngameBinkPlayer.Has_Finished()) {
        IngameBinkPlayer.Draw_Frame();

    } else {

        DEBUG_INFO("Bink: Radar movie finished.\n");
                  
        /**
         *  Restore the original in-game volume if we need to.
         */
        if (_volume_adjusted) {
            DEV_DEBUG_INFO("Bink: Restoring ingame volume.\n");
            Audio.Adjust_Volume_All(_prev_volume);
            _volume_adjusted = false;
        }
        
        DEBUG_INFO("Bink: Restoring radar mode.\n");
        
        field_14B4 = 5;
        Radar_Activate(field_14BC);

        /**
         *  Cleanup the Bink movie player.
         */
        IngameBinkPlayer.Reset();
        IngameBinkPlayer.Close();
        
        /**
         *  Force a final redraw of everything just to be safe.
         */
        DEV_DEBUG_INFO("Bink: Force blit of tactical area.\n");
        WWMouse->Hide_Mouse();
        Redraw_Sidebar_Area();
        WWMouse->Show_Mouse();
        GScreenClass::Blit(true, CompositeSurface);

        /**
         *  Reset the debug flag for the next video.
         */
        _playing_one_time = false;
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
static bool _RadarClass_AI_Intercept()
{
    return (IngameBinkPlayer.File_Loaded() && IngameBinkPlayer.IsIngameMovie)
         || IngameVQ.Count() > 0;
}


/**
 *  This patch fixes a issue were the game be seen behind the video
 *  if the window moves. This was a bug in the vanilla game.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Windows_Procedure_case_WM_PAINT)
{
    GET_REGISTER_STATIC(HWND, hWnd, ebp);
    //GET_REGISTER_STATIC(DSurface *, comp_surface, eax);
    //GET_REGISTER_STATIC(WWMouseClass *, ww_mouse, ecx);

    if (SpecialDialog || !GameInFocus && !Debug_Windowed || !WWMouse || !PrimarySurface || !HiddenSurface || !CompositeSurface) {
        goto validate_rect;
    }

    if (InScenario) {
        GScreenClass::Blit(WWMouse->Is_Captured(), CompositeSurface);
        //Map.Blit_Sidebar(true);

    } else if (Current_Movie_Ptr || IngameVQ.Count() > 0) {
        Movie_Update();

    } else {
        GScreenClass::Blit(WWMouse->Is_Captured(), HiddenSurface);
    }

    if (IngameBinkPlayer.File_Loaded() && IngameBinkPlayer.IsIngameMovie) {
        if (IngameBinkPlayer.IsPlaying) {
            IngameBinkPlayer.Draw_Frame();
        }
    }

validate_rect:
    ValidateRect(hWnd, nullptr);

break_switch:
    JMP(0x00685F9C);
}



#if 0
    if (InScenario) {
        GScreenClass::Blit(WWMouse->Is_Captured(), CompositeSurface);
        //Map.Blit_Sidebar(true);
        JMP_REG(ecx, 0x00685CE0);
        ////_asm { mov eax, CompositeSurface } // Additional dereference needed due to TS++.
        //_asm { mov eax, comp_surface }
        ////_asm { mov ecx, WWMouse } // Additional dereference needed due to TS++.
        //_asm { mov ecx, ww_mouse }
        //JMP_REG(esi, 0x00685CC2);

    } else if (IngameBinkPlayer.File_Loaded() && IngameBinkPlayer.IsIngameMovie) {
        IngameBinkPlayer.Draw_Frame();
        JMP_REG(ecx, 0x00685CFC);

    } else if (Current_Movie_Ptr || IngameVQ.Count() > 0) {
        JMP_REG(ecx, 0x00685CEE);

    } else {
        GScreenClass::Blit(WWMouse->Is_Captured(), HiddenSurface);
    }

    JMP(0x00685D26);
#endif

    
#if 0
    /**
     *  Blit game layer.
     */
blit_game:
    //_asm { mov eax, CompositeSurface } // Additional dereference needed due to TS++.
    _asm { mov eax, comp_surface }
    //_asm { mov ecx, WWMouse } // Additional dereference needed due to TS++.
    _asm { mov ecx, ww_mouse }
    JMP_REG(esi, 0x00685CC2);

update_bink:
    IngameBinkPlayer.Draw_Frame();
        
    /**
     *  Update a movie instance.
     */
movie_update:
    JMP_REG(ecx, 0x00685CEE);
#endif


DECLARE_PATCH(_Windows_Procedure_Draw_Bink_Frame_Patch)
{
    if (SpecialDialog == SDLG_NONE || GameInFocus) {

        if (IngameBinkPlayer.IsIngameMovie) {
            if (!IngameBinkPlayer.Has_Finished()) {
                IngameBinkPlayer.Draw_Frame();
            }
        }

    }

    if (MainWindow) {
        GetMenu(MainWindow);
    }

    _asm { mov ecx, MainWindow }
    _asm { mov ecx, [ecx] }
    JMP(0x00685C4C);
}


/**
 *  
 * 
 *  @author: CCHyper
 */
static bool _Windows_Procedure_Is_Movie_Playing_Patch()
{
    return (IngameBinkPlayer.File_Loaded() && IngameBinkPlayer.IsIngameMovie)
        || Current_Movie_Ptr || IngameVQ.Count() > 0;
}


DECLARE_PATCH(_GScreenClass_Blit_Draw_Bink_Frame_Patch)
{
    if (IngameBinkPlayer.IsIngameMovie) {
        if (!IngameBinkPlayer.Has_Finished()) {
            IngameBinkPlayer.Draw_Frame();
        }
    }

    _asm { pop esi }
    _asm { add esp, 0x5C }
    _asm { ret 4 }
}


/**
 *  Main function for patching the Bink Movie hooks.
 */
void BinkMovie_Hooks()
{
    /**
     *  Patch in the main Play_Movie interceptor.
     */
    Patch_Call(0x004E07AD, &Vinifera_Play_Movie);
    Patch_Call(0x004E07CC, &Vinifera_Play_Movie);
    Patch_Call(0x004E0840, &Vinifera_Play_Movie);
    Patch_Call(0x004E2865, &Vinifera_Play_Movie);
    Patch_Call(0x004E287F, &Vinifera_Play_Movie);
    Patch_Call(0x00563A1C, &Vinifera_Play_Movie);
    Patch_Call(0x0057FEDA, &Vinifera_Play_Movie);
    Patch_Call(0x0057FF3F, &Vinifera_Play_Movie);
    Patch_Call(0x005DB314, &Vinifera_Play_Movie);
    Patch_Call(0x005E35C8, &Vinifera_Play_Movie);

    /**
     *  Patch in the main Play_Ingame_Movie (radar) interceptor.
     */
    Patch_Call(0x0061A90B, &_Play_Ingame_Movie_Intercept);
    Patch_Call(0x0061BF23, &_Play_Ingame_Movie_Intercept);

    /**
     *  These patches handle the update and drawing of the Bink videos.
     */
    Patch_Call(0x005B8F20, &RadarClassExt::Play_Movie_Intercept);

    Patch_Call(0x00685CEE, &_Windows_Procedure_Is_Movie_Playing_Patch);

    Patch_Call(0x00685CF7, &_MovieClass_Update_Intercept);

    Patch_Call(0x005B9144, &_RadarClass_AI_Intercept);
    Patch_Byte(0x005B914B, 0x74); // jle -> jz

    //Patch_Jump(0x00685CB9, &_Windows_Procedure_Should_Blit_Patch);
    //Patch_Jump(0x00685C7A, &_Windows_Procedure_case_WM_PAINT);

    //Patch_Jump(0x004B9A68, &_GScreenClass_Blit_Draw_Bink_Frame_Patch);

    Patch_Call(0x005DB52E, &_Movie_Handle_Focus_Intercept);
    Patch_Jump(0x005DB602, &_Movie_Handle_Focus_Intercept);
    Patch_Call(0x0068598F, &_Movie_Handle_Focus_Intercept);
    Patch_Call(0x00685B97, &_Movie_Handle_Focus_Intercept);
    Patch_Call(0x00685EA0, &_Movie_Handle_Focus_Intercept);

    Patch_Jump(0x00685C35, &_Windows_Procedure_Draw_Bink_Frame_Patch);
}


/**
 *  Scale up the input rect to the desired width and height, while maintaining the aspect ratio.
 * 
 *  @author: CCHyper
 */
static bool Scale_Video_Rect(Rect &rect, int max_width, int max_height, bool maintain_ratio = false)
{
    /**
     *  No need to scale the rect if it is larger than the max width/height
     */
    bool smaller = rect.Width < max_width && rect.Height < max_height;
    if (!smaller) {
        return false;
    }

    /**
     *  This is a workaround for edge case issues with some versions
     *  of cnc-ddraw. This ensures the available draw area is actually
     *  the resolution the user defines, not what the cnc-ddraw forces
     *  the primary surface to.
     */
    int surface_width = std::clamp(HiddenSurface->Width, 0, Options.ScreenWidth);
    int surface_height = std::clamp(HiddenSurface->Height, 0, Options.ScreenHeight);

    if (maintain_ratio) {

        double dSurfaceWidth = surface_width;
        double dSurfaceHeight = surface_height;
        double dSurfaceAspectRatio = dSurfaceWidth / dSurfaceHeight;

        double dVideoWidth = rect.Width;
        double dVideoHeight = rect.Height;
        double dVideoAspectRatio = dVideoWidth / dVideoHeight;
    
        /**
         *  If the aspect ratios are the same then the screen rectangle
         *  will do, otherwise we need to calculate the new rectangle.
         */
        if (dVideoAspectRatio > dSurfaceAspectRatio) {
            int nNewHeight = (int)(surface_width/dVideoWidth*dVideoHeight);
            int nCenteringFactor = (surface_height - nNewHeight) / 2;
            rect.X = 0;
            rect.Y = nCenteringFactor;
            rect.Width = surface_width;
            rect.Height = nNewHeight;

        } else if (dVideoAspectRatio < dSurfaceAspectRatio) {
            int nNewWidth = (int)(surface_height/dVideoHeight*dVideoWidth);
            int nCenteringFactor = (surface_width - nNewWidth) / 2;
            rect.X = nCenteringFactor;
            rect.Y = 0;
            rect.Width = nNewWidth;
            rect.Height = surface_height;

        } else {
            rect.X = 0;
            rect.Y = 0;
            rect.Width = surface_width;
            rect.Height = surface_height;
        }

    } else {
        rect.X = 0;
        rect.Y = 0;
        rect.Width = surface_width;
        rect.Height = surface_height;
    }

    return true;
}


/**
 *  #issue-292
 * 
 *  Videos stretch to the whole screen size and ignore the video aspect ratio.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Play_Movie_Scale_By_Ratio_Patch)
{
    GET_REGISTER_STATIC(MovieClass *, this_ptr, esi);
    static Rect stretched_rect;

    /**
     *  Calculate the stretched rect for this video, maintaining the video ratio.
     */
    stretched_rect = this_ptr->VideoRect;
    if (Scale_Video_Rect(stretched_rect, HiddenSurface->Width, HiddenSurface->Height, true)) {

        /**
         *  Stretched rect calculated, assign it to the movie instance.
         */
        this_ptr->StretchRect = stretched_rect;

        DEBUG_INFO("Stretching movie - VideoRect: %d,%d -> StretchRect: %d,%d\n",
                this_ptr->VideoRect.Width, this_ptr->VideoRect.Height,
                this_ptr->StretchRect.Width, this_ptr->StretchRect.Height);

        /*DEBUG_GAME("Stretching movie %dx%d -> %dx%d\n",
            this_ptr->VideoRect.Width, this_ptr->VideoRect.Height, this_ptr->StretchRect.Width, this_ptr->StretchRect.Height);*/
    }

    JMP(0x00563805);
}


/**
 *  #issue-95
 * 
 *  Patch for handling the campaign intro movies
 *  for "The First Decade" and "Freeware TS" installations.
 * 
 *  @author: CCHyper
 */
static bool Play_Intro_Movie(CampaignType campaign_id)
{
    /**
     *  Catch any cases where we might be starting a non-campaign scenario.
     */
    if (campaign_id == CAMPAIGN_NONE) {
        return false;
    }

    if (Scen->Scenario != 1) {
        return false;
    }

    char movie_filename[32];
    VQType intro_vq = VQ_NONE;

    /**
     *  Fetch the campaign disk id.
     */
    CampaignClass *campaign = Campaigns[campaign_id];
    DiskID cd_num = campaign->WhichCD;

    /**
     *  Check if the current campaign is an original GDI or NOD campaign.
     */
    bool is_original_gdi = (cd_num == DISK_GDI && (Wstring(campaign->IniName) == "GDI1" || Wstring(campaign->IniName) == "GDI1A") && Wstring(campaign->Scenario) == "GDI1A.MAP");
    bool is_original_nod = (cd_num == DISK_NOD && (Wstring(campaign->IniName) == "NOD1" || Wstring(campaign->IniName) == "NOD1A") && Wstring(campaign->Scenario) == "NOD1A.MAP");

    /**
     *  #issue-762
     * 
     *  Fetch the campaign extension (if available) and get the custom intro movie.
     * 
     *  @author: CCHyper
     */
    CampaignClassExtension *campaignext = Extension::Fetch<CampaignClassExtension>(campaign);
    if (campaignext->IntroMovie[0] != '\0') {
        std::snprintf(movie_filename, sizeof(movie_filename), "%s.VQA", campaignext->IntroMovie);
        DEBUG_INFO("About to play \"%s\".\n", movie_filename);
        Play_Movie(movie_filename);

    /**
     *  If this is an original Tiberian Sun campaign, play the respective intro movie.
     */
    } else if (is_original_gdi || is_original_nod) {

        /**
         *  "The First Decade" and "Freeware TS" installations reshuffle
         *  the movie files due to all mix files being local now and a
         *  primitive "no-cd" added;
         *  
         *  MOVIES01.MIX -> INTRO.VQA (GDI) is now INTR0.VQA
         *  MOVIES02.MIX -> INTRO.VQA (NOD) is now INTR1.VQA
         * 
         *  Build the movie filename based on the current campaigns desired CD (see DiskID enum). 
         */
        std::snprintf(movie_filename, sizeof(movie_filename), "INTR%d.VQA", cd_num);

        /**
         *  Now play the movie if it is found, falling back to original behavior otherwise.
         */
        if (CCFileClass(movie_filename).Is_Available()) {
            DEBUG_INFO("About to play \"%s\".\n", movie_filename);
            Play_Movie(movie_filename);

        } else if (CCFileClass("INTRO.VQA").Is_Available()) {
            DEBUG_INFO("About to play \"INTRO.VQA\".\n");
            Play_Movie("INTRO.VQA");

        } else {
            DEBUG_WARNING("Failed to find Intro movie!\n");
            return false;
        }

    } else {
        DEBUG_WARNING("No campaign intro movie defined.\n");
    }

    return true;
}

DECLARE_PATCH(_Start_Scenario_Intro_Movie_Patch)
{
    GET_REGISTER_STATIC(CampaignType, campaign_id, ebx);
    GET_REGISTER_STATIC(char *, name, ebp);

    Play_Intro_Movie(campaign_id);

read_scenario:
    //JMP(0x005DB319);

    /**
     *  The First Decade" and "Freeware TS" EXE's actually have patched code at
     *  the address 0x005DB319, so lets handle the debug log print ourself and
     *  jump back at a safe location.
     */
    DEBUG_GAME("Reading scenario: %s\n", name);
    JMP(0x005DB327);
}


/**
 *  #issue-95
 * 
 *  Patch for handling the campaign intro movies for "The First Decade"
 *  and "Freeware TS" installations when selecting "Intro / Sneak Peak" on
 *  the main menu.
 * 
 *  @author: CCHyper
 */
static void Play_Intro_SneakPeak_Movies()
{
    /**
     *  Backup the current volume.
     */
    //int disk = CD::RequiredCD;

    /**
     *  Find out what movies are available locally.
     */
    bool intro_available = CCFileClass("INTRO.VQA").Is_Available();
    bool intr0_available = CCFileClass("INTR0.VQA").Is_Available();
    bool sizzle_available = CCFileClass("SIZZLE1.VQA").Is_Available();

    bool movie_pair_available = (intro_available && sizzle_available) || (intr0_available && sizzle_available);

    /**
     *  If at least one of the movie pairs were found, we can go ahead and play
     *  them, otherwise set the required disk to GDI and request it if not present.
     */
    if (movie_pair_available || (CD::Set_Required_CD(DISK_GDI), CD().Is_Available(DISK_GDI))) {
        
        /**
         *  Play the intro movie (GDI).
         * 
         *  If the renamed intro is found play that, otherwise falling back to original behavior.
         */
        if (intr0_available) {
            DEBUG_INFO("About to play INTR0.VQA.\n");
            Play_Movie("INTR0.VQA");

            /**
             *  Also attempt to play the NOD intro, just because its a nice improvement.
             */
            DEBUG_INFO("About to play INTR1.VQA.\n");
            Play_Movie("INTR1.VQA");
    
        } else {
        
            DEBUG_INFO("About to play INTRO.VQA.\n");
            Play_Movie("INTRO.VQA");
        }

        /**
         *  Play the sizzle/showreel. This exists loosely on both disks, so we tell
         *  the VQA playback to not use the normal mix file handler.
         */
        VQA_Clear_Option(OPTION_USE_MIX_HANDLER);
        DEBUG_INFO("About to play SIZZLE1.VQA.\n");
        Play_Movie("SIZZLE1.VQA");
        VQA_Set_Option(OPTION_USE_MIX_HANDLER);

    } else {
        DEBUG_WARNING("Failed to find Intro and Sizzle movies!\n");
    }

    /**
     *  Restore the previous volume.
     */
    //CD::Set_Required_CD(disk);
    //CD().Force_Available(disk);
}


DECLARE_PATCH(_Select_Game_Intro_SneakPeak_Movies_Patch)
{
    Play_Intro_SneakPeak_Movies();

    JMP(0x004E288B);
}


/**
 *  Main function for patching the hooks.
 */
void PlayMovieExtension_Hooks()
{
    Patch_Jump(0x005DB2DE, &_Start_Scenario_Intro_Movie_Patch);
    Patch_Jump(0x004E2796, &_Select_Game_Intro_SneakPeak_Movies_Patch);

    /**
     *  #issue-287
     * 
     *  Main menu transition videos incorrectly scale up when "StretchMovies=true".
     *  Changes Change Play_Movie "stretch_allowed" arg to false.
     * 
     *  @author: CCHyper
     */
    Patch_Byte(0x0057FF34+1, 0); // TS_TITLE.VQA
    Patch_Byte(0x0057FECF+1, 0); // FS_TITLE.VQA

    Patch_Jump(0x00563795, &_Play_Movie_Scale_By_Ratio_Patch);

    /**
     *  Patch in new movie systems here.
     */
    BinkMovie_Hooks();
}
