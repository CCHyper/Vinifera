/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DSHOWVIDEO_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the DirectShow video player.
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
#include "dshowvideo.h"
#include "tibsun_globals.h"
#include "dsurface.h"
#include "iomap.h"
#include "session.h"
#include "playmovie.h"
#include "vox.h"
#include "wstring.h"
#include "dsaudio.h"
#include "wwmouse.h"
#include "wwkeyboard.h"
#include "fatal.h"
#include "winutil.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <string>

#include "hooker.h"
#include "hooker_macros.h"



#include "tspp.h"
DEFINE_IMPLEMENTATION(void Movie_Update(), 0x005646E0);
DEFINE_IMPLEMENTATION(void Movie_Handle_Focus(bool), 0x00563CC0);
void *&Current_Movie_Ptr = Make_Global<void *>(0x00806E1C);
bool &InScenario = Make_Global<bool>(0x007E48FC);


/**
 *  Global instance of the DirectShow video player used for sidebar videos.
 */
/*static*/ DirectShowVideoPlayer VideoPlayer;


/**
 *  This fixes an issue with some cnc-ddraw setups not redrawing
 *  the sidebar area when a video is playing.
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
 *  Play a DirectShow video.
 * 
 *  @author: CCHyper
 */
static bool Play_Movie_DirectShow(const char *name, ThemeType theme, bool clear_before = true, bool stretch_allowed = true, bool clear_after = true)
{
    WWKeyboard->Clear();
    WWMouse->Hide_Mouse();

    /**
     *  Only play fullscreen videos in campaign/single-player!
     */
    if (!Session.Singleplayer_Game()) {
        return false;
    }

    if (clear_before) {
        HiddenSurface->Clear();
        GScreenClass::Blit(true, HiddenSurface);
        InvalidateRect(MainWindow, nullptr, false);
    }

    /**
     *  Open the new video using the DirectShow player.
     */
    VideoPlayer.SetHWND(MainWindow);
    VideoPlayer.OpenFile(name);

    if (!VideoPlayer.HasVideo()) {
        DEBUG_INFO("DirectShow: Failed to create \"%s\" instance!\n", name);
        return false;
    }

    DEBUG_INFO("Play_Movie \"%s\" with DirectShow!\n", name);

    /**
     *  Full-screen videos can be skipped and scaled.
     */
    VideoPlayer.SetBreakout(true);
    VideoPlayer.SetStretch(stretch_allowed);

    /**
     *  Setup the drawing window.
     */
    VideoPlayer.SetVideoWindow(MainWindow);

    /**
     *  Play the movie!
     */
    //VideoPlayer.Play();
    VideoPlayer.Play_Callback();

    if (clear_after) {
        HiddenSurface->Clear();
        GScreenClass::Blit(true, HiddenSurface);
        InvalidateRect(MainWindow, nullptr, false);
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
        return false;
    }

    /**
     *  Insert a null-char where the "." was. This will give us the actual
     *  movie name without the extension, allowing us to rebuild them.
     */
    *movie_name = '\0';

    const char *upper_filename = strupr((char *)filename_buffer);

    /**
     *  Was a DirectShow video found?
     */
    if (DirectShowVideoPlayer::FileAvailable(upper_filename)) {
        return true;
    }

    /**
     *  Finally check if the VQA is available.
     */
    return CCFileClass(name).Is_Available();
}


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
     *  Attempt to play the video using the DirectShow player.
     */
    if (Play_Movie_DirectShow(upper_filename, theme, clear_before, stretch_allowed, clear_after)) {
        return;
    }

    char vqa_buffer[32-4];
    std::snprintf(vqa_buffer, sizeof(vqa_buffer), "%s.VQA", upper_filename);

    /**
     *  The movie did not exist as a DirectShow or failed to play, attempt to play the .VQA.
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
 *  Play a in-game DirectShow video.
 * 
 *  @author: CCHyper
 */
static bool Play_Ingame_Movie_As_DirectShow(const char *filename)
{
    /**
     *  Destroy any existing movie playing.
     */
    //VideoPlayer.Reset();

    /**
     *  Open a new DirectShow handle to the radar video. 
     */
    //VideoPlayer.Open(filename);
    VideoPlayer.OpenFile(filename);

    if (!VideoPlayer.HasVideo()) {
        DEBUG_INFO("DirectShow: Failed to create \"%s\" instance!\n", filename);
        return false;
    }

    Rect sidebar_rect = SidebarSurface->Get_Rect();
    if (Options.SidebarOn == SIDEBAR_SIDE_RIGHT) {
        sidebar_rect.X += CompositeSurface->Width;
    }

    /**
     *  Set video position. This must be performed before Open so the DirectShow
     *  handle is created with this position to adjust it.
     */
    RECT rect;
    rect.left = sidebar_rect.X+15;
    rect.top = sidebar_rect.Y+16+12; // 16 is tab bar height.
    rect.right = rect.left+140;
    rect.bottom = rect.top+110;
//rect.left = 0;
//rect.top = 0;
//rect.right = Options.ScreenWidth;
//rect.bottom = Options.ScreenHeight;
    if (!VideoPlayer.UpdateVideoWindow(&rect)) {
        //VideoPlayer.Reset();
        return false;
    }

    /**
     *  Make sure the video does not stop when the user presses the ESC key.
     */
    VideoPlayer.SetBreakout(false);
    VideoPlayer.SetStretch(false);

    //DEBUG_INFO("Play_Ingame_Movie \"%s\" as DirectShow!\n", filename);

    return true;
}


//
// Intercept to the games Play_Ingame_Movie which checks if the DirectShow video file is 
// available, falling back to VQA if not.
//
static void _Play_Ingame_Movie_Intercept(VQType vqtype)
{
    if (vqtype == VQ_NONE || vqtype >= Movies.Count()) {
        return;
    }
    
    // Get pointer to movie name entry
    const char *movie_name = Movies[vqtype];
    
    static char filename_buffer[32];
    std::strncpy(filename_buffer, movie_name, 32);
    
    // Invalid filename
    if (filename_buffer[0] == '\0') {
        DEBUG_INFO("Invalid movie filename \"%s\"!\n", filename_buffer);
        return;
    }

    char *upper_filename = strupr((char *)filename_buffer);
    
    /**
     *  Attempt to play the video using the DirectShow player.
     */
    if (Play_Ingame_Movie_As_DirectShow(upper_filename)) {
        return;
    }

    char vqa_buffer[32-4];
    std::sprintf(vqa_buffer, "%s.VQA", upper_filename);

    // The movie did not exist as a .BIK, attempt to play the .VQA.
    if (CCFileClass(vqa_buffer).Is_Available()) {
        DEBUG_INFO("Play_Ingame_Movie \"%s\" as VQA!\n", upper_filename);
        Play_Ingame_Movie(VQType(vqtype));
        
    } else {
        DEBUG_INFO("Failed to play ingame movie \"%s\"!\n", upper_filename);
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
static void _Movie_Handle_Focus_Intercept(bool state)
{
    if (InScenario && VideoPlayer.HasVideo() && !VideoPlayer.Finished()) {
        if (state) {
            VideoPlayer.Pause();
        } else {
            VideoPlayer.Play();
        }
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
    if (VideoPlayer.HasVideo() && !VideoPlayer.Finished()) {
        
        /**
         *  Clear the surfaces. This fixes a issue were the game can
         *  be seen if the user moves the window.
         */
        //HiddenSurface->Clear();
        //AlternateSurface->Clear();
        //CompositeSurface->Clear();
        //PrimarySurface->Clear();

        //if (!VideoPlayer.Finished()) {

            /**
             *  Request the next frame.
             */
            //VideoPlayer.Advance_Frame(false);
            VideoPlayer.PaintCallback();

            /**
             *  Draw the new frame.
             */
            //VideoPlayer.Draw_Frame();

            // TEST!
            //Rect r(CompositeSurface->Width, 16, 160, 160);
            //PrimarySurface->Fill_Rect(SidebarSurface->Get_Rect(), r, DSurface::RGBA_To_Pixel(255,0,0));

        //} else {
            /**
             *  Movie finished, cleanup.
             */
            //VideoPlayer.Reset();
            //VideoPlayer.Close();
        //}
        
    /**
     *  vqa
     */
    } else {
        //DEV_DEBUG_INFO("About to call Movie_Update.\n");
        Movie_Update();
    }
}


static class RadarClassFake final : public RadarClass
{
    public:
        void Play_Movie_Intercept();
        void Play_DirectShow_Movie();

        void Play_VQA_Movie();
};

DEFINE_IMPLEMENTATION(void RadarClassFake::Play_VQA_Movie(), 0x005BCC40);


/**
 *  
 * 
 *  @author: CCHyper
 */
void RadarClassFake::Play_Movie_Intercept()
{
    if (InScenario && VideoPlayer.HasVideo() && !VideoPlayer.Finished()) {
        Play_DirectShow_Movie();
    } else {
        Play_VQA_Movie();
    }
}


/**
 *  Handle the playback of a DirectShow video on the sidebar radar.
 * 
 *  @author: CCHyper
 */
void RadarClassFake::Play_DirectShow_Movie()
{
    static bool _volume_adjusted = false; // Have we adjusted the game audio volume?
    static int _prev_volume; // Volume level to restore after video finishes.
    
    /**
     *  "One time" process at the start of the video playback.
     */
    static bool _playing_one_time = false;
    if (!_playing_one_time) {
        DEBUG_INFO("Play_Ingame_Movie \"%s\" as DirectShow!\n", VideoPlayer.FilenameNoExt());

        /**
         *  Play!
         */
        VideoPlayer.Play();

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
//    VideoPlayer.Advance_Frame(false);
//    VideoPlayer.Callback();

#if 0
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
#endif
    
    /**
     *  Draw the next frame.
     */
    if (VideoPlayer.HasVideo() && !VideoPlayer.Finished()) {

        /**
         *  Redraw the tactical map.
         */
        //Flag_To_Redraw(1);
        //GScreenClass::Blit(true, CompositeSurface);

        VideoPlayer.PaintCallback();

    } else {

        DEBUG_INFO("DirectShow: Radar movie finished.\n");
                  
        /**
         *  Restore the original in-game volume if we need to.
         */
        if (_volume_adjusted) {
            DEV_DEBUG_INFO("DirectShow: Restoring ingame volume.\n");
            Audio.Adjust_Volume_All(_prev_volume);
            _volume_adjusted = false;
        }
        
        DEBUG_INFO("DirectShow: Restoring radar mode.\n");
        
        field_14B4 = 5;
        Radar_Activate(field_14BC);

        /**
         *  Cleanup the DirectShow movie player.
         */
//        VideoPlayer.Reset();
//        VideoPlayer.Close();
        
        /**
         *  Force a final redraw of everything just to be safe.
         */
        DEV_DEBUG_INFO("DirectShow: Force blit of tactical area.\n");
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
    return (InScenario && VideoPlayer.HasVideo() && !VideoPlayer.Finished())
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

    //if (InScenario) {
    //    if (VideoPlayer.HasVideo() && !VideoPlayer.Finished()) {
    //        VideoPlayer.PaintCallback();
    //    }
    //}

    DEBUG_INFO("WM_PAINT\n");

validate_rect:
    //InvalidateRect(hWnd, nullptr, false);
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

    } else if (VideoPlayer.File_Loaded() && VideoPlayer.IsIngameMovie) {
        VideoPlayer.Draw_Frame();
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
    VideoPlayer.Draw_Frame();
        
    /**
     *  Update a video instance.
     */
movie_update:
    JMP_REG(ecx, 0x00685CEE);
#endif


DECLARE_PATCH(_Windows_Procedure_Draw_DirectShow_Frame_Patch)
{
    if (InScenario && (SpecialDialog == SDLG_NONE || GameInFocus)) {
        if (VideoPlayer.HasVideo() && !VideoPlayer.Finished()) {
            VideoPlayer.PaintCallback();
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
    return (InScenario && VideoPlayer.HasVideo() && !VideoPlayer.Finished())
        || Current_Movie_Ptr || IngameVQ.Count() > 0;
}


DECLARE_PATCH(_GScreenClass_Blit_Draw_DirectShow_Frame_Patch)
{
    if (InScenario && (SpecialDialog == SDLG_NONE || GameInFocus)) {
        if (VideoPlayer.HasVideo() && !VideoPlayer.Finished()) {
            VideoPlayer.PaintCallback();
        }
    }

    DEBUG_INFO("GScreenClass_Blit\n");

    _asm { pop esi }
    _asm { add esp, 0x5C }
    _asm { ret 4 }
}


/**
 *  Main function for patching the hooks.
 */
void DirectShowVideo_Hooks()
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
     *  These patches handle the update and drawing of the DirectShow videos.
     */
    Patch_Call(0x005B8F20, &RadarClassFake::Play_Movie_Intercept);

    Patch_Call(0x00685CEE, &_Windows_Procedure_Is_Movie_Playing_Patch);

    //Patch_Call(0x00685CF7, &_MovieClass_Update_Intercept);

    Patch_Call(0x005B9144, &_RadarClass_AI_Intercept);
    Patch_Byte(0x005B914B, 0x74); // jle -> jz

    //Patch_Jump(0x00685CB9, &_Windows_Procedure_Should_Blit_Patch);
    Patch_Jump(0x00685C7A, &_Windows_Procedure_case_WM_PAINT);

    Patch_Jump(0x004B9A68, &_GScreenClass_Blit_Draw_DirectShow_Frame_Patch);

    //Patch_Call(0x005DB52E, &_Movie_Handle_Focus_Intercept);
    //Patch_Jump(0x005DB602, &_Movie_Handle_Focus_Intercept);
    //Patch_Call(0x0068598F, &_Movie_Handle_Focus_Intercept);
    //Patch_Call(0x00685B97, &_Movie_Handle_Focus_Intercept);
    //Patch_Call(0x00685EA0, &_Movie_Handle_Focus_Intercept);

    //Patch_Jump(0x00685C35, &_Windows_Procedure_Draw_DirectShow_Frame_Patch);
}
