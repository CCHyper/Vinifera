/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BINKMOVIE.CPP
 *
 *  @author        CCHyper
 *
 *  @contributors  tomsons26
 *
 *  @brief         Bink video player interface.
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
#include "binkmovie.h"
#include "bink.h"
#include "binkloaddll.h"
#include "vinifera_util.h"
#include "tibsun_globals.h"
#include "convert.h"
#include "dsurface.h"
#include "bsurface.h"
#include "options.h"
#include "mixfile.h"
#include "ccfile.h"
#include "ini.h"
#include "rgb.h"
#include "wstring.h"
#include "vqa.h"
#include "textprint.h"
#include "dsaudio.h"
#include "wwkeyboard.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <string>


#include "tspp.h"
DEFINE_IMPLEMENTATION(bool VQA_Movie_Message_Loop(), 0x0066B230);


/**
 *  Internal flag for clearing the window before first frame draw.
 */
static bool BinkClearedWindow = false;


/**
 *  Additional sub directory to scan for BINK files locally.
 */
char BinkMoviePlayer::BinkSearchDirectory[PATH_MAX] = { '\0' };


/**
 *  Master playback volume relative to the options volume.
 */
static float BinkMasterVolume = 0.7f;


/**
 *  Debug overlay text surface.
 */
//static BSurface *DebugOverlaySurface;


/**
 *  Class default constructor.
 * 
 *  @author: CCHyper
 */
BinkMoviePlayer::BinkMoviePlayer() :
    IsBreakoutAllowed(true),
    BinkHandle(nullptr),
    BinkBuffer(nullptr),
    SurfaceFlags(0),
    FileHandle(INVALID_HANDLE_VALUE),
    DrawSurface(nullptr),
    Volume(BinkMasterVolume),
    IsPlaying(true),
    NewFrame(false),
    PlayFast(false),
    LastFrameNum(0),
    IsIngameMovie(false),
    IsCanBeStretched(true),
    IsClearScreenBefore(true),
    IsDebugOverlayEnabled(false)
{
    std::memset(Filename, 0, sizeof(Filename));
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
BinkMoviePlayer::BinkMoviePlayer(const char *filename, bool can_be_stretched) :
    IsBreakoutAllowed(true),
    BinkHandle(nullptr),
    BinkBuffer(nullptr),
    SurfaceFlags(0),
    FileHandle(INVALID_HANDLE_VALUE),
    DrawSurface(nullptr),
    Volume(BinkMasterVolume),
    IsPlaying(true),
    NewFrame(false),
    PlayFast(false),
    LastFrameNum(0),
    IsIngameMovie(false),
    IsClearScreenBefore(true),
    IsCanBeStretched(can_be_stretched),
    IsDebugOverlayEnabled(false)
{
    std::memset(Filename, 0, sizeof(Filename));

    Open(filename);
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
BinkMoviePlayer::~BinkMoviePlayer()
{
    Close();
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
bool BinkMoviePlayer::Open(const char *filename)
{
    Close();

    LastFrameNum = 0;

    /**
     *  Reset the surface clear flag.
     */
    BinkClearedWindow = false;

    /**
     *  Tell Bink to use DirectSound (must be before BinkOpen)!
     */
    if (Audio.Is_Available()) {
        BinkSoundUseDirectSound((BINKOPENDIRECTSOUND)Audio.Get_Sound_Object());
    } else {
        DEBUG_WARNING("Bink: Audio playback not available!\n");
    }
    
    /**
     *  Open a handle to the file if it exists locally in the sub directory.
     */
    const char *sub_filename = (std::string(BinkSearchDirectory) + std::string(filename)).c_str();
    if (RawFileClass(sub_filename).Is_Available()) {
        BinkHandle = BinkOpen(sub_filename, 0);

    /**
     *  Open a handle to the file if it exists locally.
     */
    } else if (RawFileClass(filename).Is_Available()) {
        BinkHandle = BinkOpen(filename, 0);

    } else {
        
        /**
         *  So the file was not found locally, try finding it within the loaded mixfiles.
         */
        long start = 0;
        MFCC *mixfile = nullptr;
        if (MFCC::Offset(filename, nullptr, &mixfile, &start)) {

            /**
             *  
             */
            FileHandle = CreateFileA(mixfile->Filename, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_SUPPORTS_BLOCK_REFCOUNTING, nullptr);
            if (FileHandle != INVALID_HANDLE_VALUE) {
                
                /**
                 *  Set the file handle pointer to the offset within the mixfile.
                 */
                SetFilePointer(FileHandle, start, nullptr, FILE_BEGIN);

                /**
                 *  Now open the handle. When we pass "BINKFILEHANDLE" into BinkOpen, it tells the
                 *  Bink playback engine that the first param is in fact a Windows file handle and
                 *  not the actual filename.
                 */
                BinkHandle = BinkOpen((char *)FileHandle, BINKFILEHANDLE);
            }
        }
    }

    ASSERT(BinkHandle != nullptr);

    if (BinkHandle) {

        /**
         *  Store the video filename.
         */
        std::strncpy(Filename, filename, sizeof(Filename));

        /**
         *  Adjust master volume if options have been changed, then
         *  set the playback volume.
         */
        BinkMasterVolume = Options.VoiceVolume;
        Set_Volume(BinkMasterVolume);

        Rect bink_rect;
        unsigned flags = BINKBUFFERAUTO;
        bool fixup_default_scale = false;

        /**
         *  Get the size of the current window.
         */
        RECT rect;
        GetClientRect(MainWindow, &rect);

        /**
         *  This is a workaround to make sure that we stretch the video to the size set by
         *  a custom DDRAW dll, even if the size is larger than the game resolution.
         */
        if (RawFileClass("DDRAW.DLL").Is_Available()) {
            RawFileClass ddrawfile("DDRAW.INI");
            INIClass ddrawini;

            ddrawini.Load(ddrawfile);

            rect.right = ddrawini.Get_Int("ddraw", "width", rect.right);
            rect.bottom = ddrawini.Get_Int("ddraw", "height", rect.bottom);

            if (Options.ScreenWidth != rect.right || Options.ScreenHeight != rect.bottom) {
                fixup_default_scale = true;
                DEV_DEBUG_WARNING("Bink: Possible resolution mismatch in ddraw.ini!\n");
            }
        }

        /**
         *  In-game movies need to be handled differently.
         */
        if (IsIngameMovie) {

            int video_width = 140;
            int video_height = 110;

            if (fixup_default_scale) {
            
                /**
                 *  #TODO:
                 *  For now we have to assume that the user set the resolution
                 *  in DDRAW.INI to x2 of the game resolution.
                 */
                bink_rect.Width = video_width*2;
                bink_rect.Height = video_height*2;
                bink_rect.X = XPos*2;
                bink_rect.Y = YPos*2;

                /**
                 *  Tell the Bink system we need to stretch the video buffer.
                 */
                flags |= BINKBUFFERSTRETCHXINT|BINKBUFFERSTRETCHYINT;

            } else {

                /**
                 *  Even if the Bink movie is larger than 140x110, we still
                 *  retain the original expectations of the video playback
                 *  and tell Bink to scale the video down.
                 */
                bink_rect.Width = video_width;
                bink_rect.Height = video_height;
                bink_rect.X = XPos;
                bink_rect.Y = YPos;

                /**
                 *  Tell the Bink system we need to shrink the video.
                 */
                flags |= BINKBUFFERSHRINKXINT|BINKBUFFERSHRINKYINT;
            }

        /**
         *  Video is (should be) full screen.
         */
        } else {

            /**
             *  Stretch (while maintaining aspect ratio) the movie to the window size.
             */
            if (IsCanBeStretched && Options.StretchMovies) {

                Rect scaled;

                int surface_width = rect.right;
                int surface_height = rect.bottom;

                double dSurfaceWidth = surface_width;
                double dSurfaceHeight = surface_height;
                double dSurfaceAspectRatio = dSurfaceWidth/dSurfaceHeight;

                double dVideoWidth = BinkHandle->Width;
                double dVideoHeight = BinkHandle->Height;
                double dVideoAspectRatio = dVideoWidth/dVideoHeight;

                if (dVideoAspectRatio > dSurfaceAspectRatio) {
                    int nNewHeight = (int)(surface_width/dVideoWidth*dVideoHeight);
                    int nCenteringFactor = (surface_height - nNewHeight) / 2;
                    scaled.X = 0;
                    scaled.Y = nCenteringFactor;
                    scaled.Width = surface_width;
                    scaled.Height = nNewHeight;
                
                } else if (dVideoAspectRatio < dSurfaceAspectRatio) {
                    int nNewWidth = (int)(surface_height/dVideoHeight*dVideoWidth);
                    int nCenteringFactor = (surface_width - nNewWidth) / 2;
                    scaled.X = nCenteringFactor;
                    scaled.Y = 0;
                    scaled.Width = nNewWidth;
                    scaled.Height = surface_height;
                
                } else {
                    scaled.X = 0;
                    scaled.Y = 0;
                    scaled.Width = surface_width;
                    scaled.Height = surface_height;
                }

                bink_rect = scaled;

                /**
                 *  Tell the Bink system we need to stretch the video buffer.
                 */
                flags |= BINKBUFFERSTRETCHXINT|BINKBUFFERSTRETCHYINT;

            /**
             *  We detected a possible resolution mis-match, attempt to fix it.
             */
            } else if (fixup_default_scale) {
            
                Rect new_rect;

                /**
                 *  #TODO:
                 *  For now we have to assume that the user set the resolution
                 *  in DDRAW.INI to x2 of the game resolution.
                 */
                new_rect.Width = 640*2;
                new_rect.Height = 400*2;
                new_rect.X = ((rect.right-new_rect.Width)/2);
                new_rect.Y = ((rect.bottom-new_rect.Height)/2);

                bink_rect = new_rect;

                /**
                 *  Tell the Bink system we need to shrink the video.
                 */
                flags |= BINKBUFFERSHRINKXINT|BINKBUFFERSHRINKYINT;

            } else {

                /**
                 *  Even if the Bink movie is larger than 640x400, we still
                 *  retain the original expectations of the video playback
                 *  and tell Bink to scale the video down.
                 */
                bink_rect.Width = 640;
                bink_rect.Height = 400;
                bink_rect.X = (rect.right-bink_rect.Width)/2;
                bink_rect.Y = (rect.bottom-bink_rect.Height)/2;

                /**
                 *  Tell the Bink system we need to shrink the video.
                 */
                flags |= BINKBUFFERSHRINKXINT|BINKBUFFERSHRINKYINT;
            }

        }

        /**
         *  Create a destination buffer to render to.
         */
        BinkBuffer = BinkBufferOpen(MainWindow, BinkHandle->Width, BinkHandle->Height, flags);
        ASSERT(BinkBuffer != nullptr);

        /**
         *  Set the draw size and position.
         */
        BinkBufferSetScale(BinkBuffer, bink_rect.Width, bink_rect.Height);
        BinkBufferSetOffset(BinkBuffer, rect.left+bink_rect.X, rect.top+bink_rect.Y);
        DEBUG_INFO("Bink: %d,%d,%d,%d\n", bink_rect.X, bink_rect.Y, bink_rect.Width, bink_rect.Height);

        /**
         *  Store a copy of the surface flags.
         */
        //SurfaceFlags = BinkDDSurfaceType(PrimarySurface->Get_DD_Surface());
        SurfaceFlags = BinkBuffer->SurfaceType;
        SurfaceFlags |= BINKCOPYALL;

#if 0
        switch (PrimarySurface->Get_RGB_Pixel_Format()) {
            case 0:
                SurfaceFlags |= BINKSURFACE555;
                break;
            /*case 1:
                SurfaceFlags |= BINKSURFACE556;
                break;*/
            case 2:
                SurfaceFlags |= BINKSURFACE565;
                break;
            case 3:
                SurfaceFlags |= BINKSURFACE655;
                break;
            default:
                break;
        };
#endif

        return true;
    }

    DEBUG_ERROR("Bink: Error opening Bink! Bink error: %s\n", BinkGetError());

    return false;
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
void BinkMoviePlayer::Close()
{
    if (BinkHandle) {
        BinkClose(BinkHandle);
        BinkHandle = nullptr;
    }
    if (BinkBuffer) {
        BinkBufferClose(BinkBuffer);
        BinkBuffer = nullptr;
    }
    if (FileHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(FileHandle);
        FileHandle = INVALID_HANDLE_VALUE;
    }

    std::memset(Filename, 0, sizeof(Filename));
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
void BinkMoviePlayer::Reset()
{
    Close();
    
    std::memset(Filename, 0, sizeof(Filename));

    SurfaceFlags = 0;
    DrawSurface = nullptr;
    IsPlaying = false;
    NewFrame = false;
    LastFrameNum = 0;
    IsBreakoutAllowed = true;
    IsIngameMovie = false;
    IsCanBeStretched = true;
    IsDebugOverlayEnabled = false;
}


/**
 *  Set the position to draw the Bink video at, relative to the input rect.
 * 
 *  This should only be used for videos that are smaller than the game window
 *  and are flagged to not be stretched.
 * 
 *  @author: CCHyper
 */
bool BinkMoviePlayer::Set_Position(Rect &rect, unsigned x_pos, unsigned y_pos)
{
    XPos = rect.X+x_pos;
    YPos = rect.Y+y_pos;
    return true;
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
void BinkMoviePlayer::Go_To_Frame(int frame)
{
    if (frame < 0) {
        frame = 0;
    }
    if (frame > BinkHandle->Frames) {
        frame = BinkHandle->Frames;
    }
    BinkGoto(BinkHandle, frame, BINKGOTOQUICK);
    LastFrameNum = 0;
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
void BinkMoviePlayer::Pause(bool pause)
{
    BinkPause(BinkHandle, pause);
    IsPlaying = !pause;
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
bool BinkMoviePlayer::Has_Finished() const
{
    return BinkHandle->FrameNum >= BinkHandle->Frames || BinkHandle->FrameNum < LastFrameNum;
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
void BinkMoviePlayer::Play()
{
    if (!BinkHandle) {
        DEBUG_ERROR("Bink: Handle is null! Bink error: %s\n", BinkGetError());
        return;
    }

    WWKeyboard->Clear();

    IsPlaying = true;
    NewFrame = false;

    bool process = true;

    while (process) {
    
        if (Has_Finished()) {
            break;
        }

        /**
         *  Are there any Windows messages to handle?
         */
        if (!VQA_Movie_Message_Loop()) {
            break;

        } else {

            /**
             *  Draw the next frame if currently playing.
             */
            if (IsPlaying) {

                Advance_Frame();

            } else {

                /**
                 *  We are paused. we don't need to redraw every tick, so add a little wait to take
                 *  the stress away from the CPU, because you know, it has a hard life...
                 */
                Draw_Frame();
                Sleep(33); // Sleep for 33 msec.
            }

            /**
             *  Check for any keyboard input.
             */
            if (!IsIngameMovie && WWKeyboard->Check()) {

                switch (WWKeyboard->Get()) {

                    /**
                     *  Debug only: Space bar toggles pause.
                     */
                    case (KN_RLSE_BIT|KN_SPACE):
                        if (Vinifera_DeveloperMode) {
                            DEBUG_INFO("Bink: %s\n", IsPlaying ? "PAUSE" : "RESUME");
                            Pause(IsPlaying);
                        }
                        break;

                    /**
                     *  Debug only: Display the debug overlay.
                     */
                    case (KN_RLSE_BIT|KN_D):
                        if (Vinifera_DeveloperMode) {
                            IsDebugOverlayEnabled = !IsDebugOverlayEnabled;
                        }
                        break;

                    /**
                     *  Debug only: Restart playback.
                     */
                    case (KN_RLSE_BIT|KN_R):
                        if (Vinifera_DeveloperMode) {
                            DEV_DEBUG_INFO("Bink: Restarting playback of \"%s\".\n", Filename);
                            Go_To_Frame(0);
                        }
                        break;

                    /**
                     *  Debug only: Toggle Play-fast.
                     */
                    case (KN_RLSE_BIT|KN_P):
                        if (Vinifera_DeveloperMode) {
                            DEV_DEBUG_INFO("Bink: Toggle PlayFast.\n", Filename);
                            PlayFast = !PlayFast;
                        }
                        break;

                    /**
                     *  Debug only: Adjust volume.
                     */
                    case (KN_RLSE_BIT|KN_KEYPAD_MINUS):
                        if (Vinifera_DeveloperMode) {
                            DEV_DEBUG_INFO("Bink: Volume down.\n");
                            Set_Volume(Volume-0.10f);
                        }
                        break;
                    case (KN_RLSE_BIT|KN_KEYPAD_PLUS):
                        if (Vinifera_DeveloperMode) {
                            DEV_DEBUG_INFO("Bink: Volume up.\n");
                            Set_Volume(Volume+0.10f);
                        }
                        break;

                    /**
                     *  Debug only: Previous and next frame. The payback must be
                     *              paused for these keys to work.
                     */
                    case (KN_RLSE_BIT|KN_COMMA):
                        if (Vinifera_DeveloperMode) {
                            DEV_DEBUG_INFO("Bink: Current Frame %d, Prev Frame %d\n", BinkHandle->FrameNum, BinkHandle->FrameNum-1);
                            Go_To_Frame(BinkHandle->FrameNum-1);
                            Draw_Frame();
                        }
                        break;
                    case (KN_RLSE_BIT|KN_CTRL_BIT|KN_COMMA):
                        if (Vinifera_DeveloperMode) {
                            DEV_DEBUG_INFO("Bink: Current Frame %d, Prev Frame %d\n", BinkHandle->FrameNum, BinkHandle->FrameNum-10);
                            Go_To_Frame(BinkHandle->FrameNum-10);
                            Draw_Frame();
                        }
                        break;

                    case (KN_RLSE_BIT|KN_PERIOD):
                        if (Vinifera_DeveloperMode) {
                            DEV_DEBUG_INFO("Bink: Current Frame %d, Next Frame %d\n", BinkHandle->FrameNum, BinkHandle->FrameNum+1);
                            Go_To_Frame(BinkHandle->FrameNum+1);
                            Draw_Frame();
                        }
                        break;
                    case (KN_RLSE_BIT|KN_CTRL_BIT|KN_PERIOD):
                        if (Vinifera_DeveloperMode) {
                            DEV_DEBUG_INFO("Bink: Current Frame %d, Next Frame %d\n", BinkHandle->FrameNum, BinkHandle->FrameNum+10);
                            Go_To_Frame(BinkHandle->FrameNum+10);
                            Draw_Frame();
                        }
                        break;

                    /**
                     *  Check if the ESC key has been pressed. If so, break out
                     *  and stop all frame updates.
                     */
                    case (KN_RLSE_BIT|KN_ESC):
                        if (IsBreakoutAllowed) {
                            DEBUG_INFO("Bink: Breakout.\n");
                            process = false;
                        }
                        break;
                };

            }

        }
    
    }

    /**
     *  Clear the keyboard buffers.
     */
    WWKeyboard->Clear();
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
bool BinkMoviePlayer::Next_Frame(bool draw_frame)
{
    /**
     *  Adjust master volume if options have been changed, then
     *  set the playback volume.
     */
    if (Options.VoiceVolume != BinkMasterVolume) {
        BinkMasterVolume = Options.VoiceVolume;
        Set_Volume(BinkMasterVolume);
    }

    Resume_Pause();

    /**
     *  Do we have a new frame to draw?
     */
    if (NewFrame || !BinkWait(BinkHandle)) {

        /**
         *  Check to see if a frame is ready to be drawn.
         */
        while (!BinkWait(BinkHandle) || PlayFast) {

            /**
             *  Start decompressing the next frame.
             */
            BinkDoFrame(BinkHandle);

            NewFrame = false;

            if (draw_frame) {
                Draw_Frame();
            }

            LastFrameNum = BinkHandle->FrameNum;

            /**
             *  Next frame, please.
             */
            BinkNextFrame(BinkHandle);
        }

        return true;
    }

    return false;
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
bool BinkMoviePlayer::Advance_Frame(bool draw_frame)
{
    return Next_Frame(draw_frame);
}


/**
 *  ?
 * 
 *  @author: CCHyper
 */
void BinkMoviePlayer::Draw_Frame()
{
    /**
     *  
     */
    if (DrawSurface) {

        /**
         *  Lock the surface so that we can copy the decompressed frame into it.
         */
        void *buffptr = DrawSurface->Lock();
        if (buffptr) {

            /**
             *  Copy the decompressed frame into the surface buffer (this might be currently on-screen).
             */
            BinkCopyToBuffer(BinkHandle,
                             buffptr,
                             DrawSurface->Get_Pitch(),
                             DrawSurface->Get_Height(),
                             0,
                             0,
                             SurfaceFlags|BINKGRAYSCALE /*|SurfaceFlags*/ /*BinkBuffer->SurfaceType*/);
        }

        /**
         *  Finished, now unlock the BinkBuffer.
         */
        DrawSurface->Unlock();

    /**
     *  
     */
    } else if (BinkBuffer) {

        /**
         *  Perform a one-time clear of the window to remove any artifacts.
         */
        if (!IsIngameMovie && IsClearScreenBefore) {
            if (!BinkClearedWindow) {
                DEBUG_INFO("Bink: One time clear of PrimarySurface.\n");
                PrimarySurface->Clear();
                BinkClearedWindow = true;
            }
        }

        if (BinkBufferLock(BinkBuffer)) {

            BinkCopyToBuffer(BinkHandle,
                             BinkBuffer->Buffer,
                             BinkBuffer->BufferPitch,
                             BinkBuffer->Height,
                             0,
                             0,
                             SurfaceFlags /*BinkBuffer->SurfaceType*/);

            /**
             *  Now unlock the Bink buffer.
             */
            BinkBufferUnlock(BinkBuffer);
        }

        /**
         *  Tell the BinkBuffer to blit the pixels onto the screen (if the
         *  BinkBuffer is using an off-screen blitting style).
         */
        //BinkBufferBlit(BinkBuffer, BinkHandle->FrameRects, BinkGetRects(BinkHandle, SurfaceFlags /*BinkBuffer->SurfaceType*/));
        BinkBufferBlit(BinkBuffer, nullptr, 1);
        
        /**
         *  Draw the debug text if enabled.
         */
        if (!IsIngameMovie) {
            if (Vinifera_DeveloperMode && IsDebugOverlayEnabled) {

                char debug_buff[1024];
                std::snprintf(debug_buff, sizeof(debug_buff),
                    "Filename: %s\n"
                    "Handle:\n"
                    "  Width: %d\n"
                    "  Height: %d\n"
                    "  Frames: %d\n"
                    "  FrameNum: %d\n"
                    "  FrameRate: %d\n"
                    "  Paused: %s\n"
                    "\n"
                    "Buffer:\n"
                    "  WindowWidth: %d\n"
                    "  WindowHeight: %d\n",
                    Filename,
                    BinkHandle->Width,
                    BinkHandle->Height,
                    BinkHandle->Frames,
                    BinkHandle->FrameNum,
                    BinkHandle->FrameRate,
                    IsPlaying ? "FALSE" : "TRUE" ,
                    BinkBuffer->WindowWidth,
                    BinkBuffer->WindowHeight
                );

                RECT textrect;

                /**
                 *  Get the size of the current window.
                 */
                RECT windowrect;
                GetClientRect(MainWindow, &windowrect);

                textrect.left = 10;
                textrect.top = 10;
                textrect.right = windowrect.right;
                textrect.bottom = windowrect.bottom;

                DrawText(GetDC(MainWindow), debug_buff, -1, &textrect, DT_LEFT|DT_TOP);
            }
        }

    }
}


/**
 *  Toggle the playback state of the movie currently playing.
 * 
 *  @author: CCHyper
 */
bool BinkMoviePlayer::Resume_Pause()
{
    bool playing = IsPlaying;

    if (GameInFocus) {
        if (!playing) {
            DEBUG_INFO("Bink: Resume bink movie.\n");
            IsPlaying = true;
            Pause(false);
            Draw_Frame();
        }
    } else if (playing) {
        DEBUG_INFO("Bink: Pause bink movie.\n");
        IsPlaying = false;
        Pause(true);
    }

    return playing;
}


/**
 *  Set the volume of the currently playing movie.
 * 
 *  @author: CCHyper
 */
void BinkMoviePlayer::Set_Volume(float vol)
{
    if (vol < 0.0f) {
        vol = 0.0f;
    }
    if (vol > 1.0f) {
        vol = 1.0f;
    }
    Volume = vol;
    BinkSetVolume(BinkHandle, (Volume * 32768.0f));
}


/**
 *  Set the master playback volume.
 * 
 *  @author: CCHyper
 */
float BinkMoviePlayer::Set_Master_Volume(float vol)
{
    float old = BinkMasterVolume;
    BinkMasterVolume = vol;
    return old;
}


/**
 *  Set the additional search directory when looking for the Bink video file.
 *  
 *  @note: This can either be a sub directory name such as "MOVIES/" or a
 *         absolute path like "C:/MY_MOVIES/".
 * 
 *  @author: CCHyper
 */
void BinkMoviePlayer::Set_Search_Directory(const char *dir_name)
{
    std::strncpy(BinkSearchDirectory, dir_name, sizeof(BinkSearchDirectory));
}
