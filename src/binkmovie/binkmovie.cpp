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
#include "bink_load_dll.h"
#include "vinifera_util.h"
#include "tibsun_globals.h"
#include "dsurface.h"
#include "bsurface.h"
#include "options.h"
#include "mixfile.h"
#include "ccfile.h"
#include "rgb.h"
#include "vqa.h"
#include "dsaudio.h"
#include "wwkeyboard.h"
#include "debughandler.h"
#include "asserthandler.h"


#include "tspp.h"
DEFINE_IMPLEMENTATION(bool VQA_Movie_Message_Loop(), 0x0066B230);


/**
 *  Master playback volume relative to the options volume.
 */
static float BinkMasterVolume = 0.7f;


BinkMoviePlayer::BinkMoviePlayer(const char *filename) :
    IsBreakoutAllowed(true),
    BinkHandle(0),
    SurfaceFlags(0),
    VideoSurface(nullptr),
    VideoRect(),
    VideoStretchedRect(),
    FileHandle(INVALID_HANDLE_VALUE),
    IsPlaying(true),
    NewFrame(false),
    LastFrameNum(0)
{
    Open(filename);
}


BinkMoviePlayer::BinkMoviePlayer(const char * filename, XSurface * surface) :
    IsBreakoutAllowed(true),
    BinkHandle(0),
    SurfaceFlags(0),
    VideoSurface(surface),
    VideoRect(),
    VideoStretchedRect(),
    FileHandle(INVALID_HANDLE_VALUE),
    IsPlaying(true),
    NewFrame(false),
    LastFrameNum(0)
{
    Open(filename);
}


BinkMoviePlayer::~BinkMoviePlayer()
{
    Close();
}


bool BinkMoviePlayer::Open(const char * filename)
{
    Close();

    LastFrameNum = 0;

    /**
     *  Tell Bink to use DirectSound (must be before BinkOpen)!
     */
    if (Audio.Is_Available()) {
        BinkSoundUseDirectSound((BINKOPENDIRECTSOUND)Audio.Get_Sound_Object());
        BinkSetSoundSystem(BinkOpenDirectSound, (unsigned int)Audio.Get_Sound_Object());
    }
    
    /**
     *  Open a handle to the file if it exists locally.
     */
    if (RawFileClass(filename).Is_Available()) {
        BinkHandle = BinkOpen(filename, 0);

    } else {
        
        /**
         *  So the file was not found locally, try finding it within a mixfile.
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
         *  Adjust playback volume based on the set user volume.
         */
        if (Options.VoiceVolume != BinkMasterVolume) {
            BinkMasterVolume = Options.VoiceVolume;
            BinkSetVolume(BinkHandle, (BinkMasterVolume * 32768.0f));
        }

        RECT rect;
        int x = 0;
        int y = 0;

        if (!VideoSurface) {

            /**
             *  Fullscreen movies draw directly to the primary surface.
             */
            VideoSurface = PrimarySurface;

            GetClientRect(MainWindow, &rect);
            x = (rect.right - rect.left - BinkHandle->Width) / 2;
            y = (rect.bottom - rect.top);

            y = (y - BinkHandle->Height) / 2;

            Set_Position(x, y);

            /**
             *  Shall we stretch the movie?
             */
            if (Options.StretchMovies) {

                /**
                 *  Calculate the stretched rect for this video, maintaining the video ratio.
                 */
                VideoStretchedRect = VideoRect;
                if (Scale_Video_Rect(VideoStretchedRect, HiddenSurface->Width, HiddenSurface->Height, true)) {

                    DEBUG_INFO("BinkMoviePlayer::Open()\n");
                    DEBUG_INFO("  Stretching movie - VideoRect: %d,%d -> VideoStretchedRect: %d,%d\n",
                            VideoRect.Width, VideoRect.Height,
                            VideoStretchedRect.Width, VideoStretchedRect.Height);
                }

            } else {
                DEBUG_INFO("BinkMoviePlayer::Open() - VideoRect: %d,%d\n", VideoRect.Width, VideoRect.Height);
            }

        }

        /**
         *  Store a copy of the surface flags.
         */
        SurfaceFlags = BinkDDSurfaceType(PrimarySurface->Get_DD_Surface());

        return true;
    }

    DEBUG_ERROR("BinkMoviePlayer::Open() - Bink Error: %s\n", BinkGetError());

    return false;
}


void BinkMoviePlayer::Close()
{
    if (BinkHandle) {
        BinkClose(BinkHandle);
    }
    if (FileHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(FileHandle);
        FileHandle = INVALID_HANDLE_VALUE;
    }
}


void BinkMoviePlayer::Set_Position(unsigned x_pos, unsigned y_pos)
{
    Rect surface_rect = VideoSurface->Get_Rect();
    Rect bink_rect(x_pos, y_pos, BinkHandle->Width, BinkHandle->Height);

    VideoRect = Intersect(surface_rect, bink_rect);
}


void BinkMoviePlayer::Go_To_Frame(int frame)
{
    BinkGoto(BinkHandle, frame, BINKGOTOQUICK);
    LastFrameNum = 0;
}


void BinkMoviePlayer::Pause(bool pause)
{
    BinkPause(BinkHandle, (pause & 0xFF));
}


bool BinkMoviePlayer::Has_Finished() const
{
    return BinkHandle->FrameNum >= BinkHandle->Frames || BinkHandle->FrameNum < LastFrameNum;
}


void BinkMoviePlayer::Play()
{
    if (!BinkHandle) {
        DEBUG_ERROR("BinkMoviePlayer::Play() - Bink handle is null! Bink Error: %s\n", BinkGetError());
        return;
    }

    WWKeyboard->Clear();

    IsPlaying = true;
    NewFrame = false;

    for (;;) {
    
        if (Has_Finished()) {
            break;
        }

        /**
         *  Are there any messages to handle?
         */
        if (!VQA_Movie_Message_Loop()) {
            break;

        } else {

            /**
             *  If paused, we don't need to redraw every tick, so add a little wait to take
             *  the stress away from the CPU, because you know, it has a hard life...
             */
            if (!IsPlaying) {
                Sleep(33); // Sleep for 33 msec.
            }

            /**
             *  Draw the next frame.
             */
            if (VideoSurface == PrimarySurface) {

                RECT rect;
                GetClientRect(MainWindow, &rect);
                ClientToScreen(MainWindow, (LPPOINT)&rect);

                Next_Frame(VideoSurface, rect.left + VideoRect.X, rect.top + VideoRect.Y);

            } else if (Next_Frame(VideoSurface, VideoRect.X, VideoRect.Y)) {

                RECT rect;
                GetClientRect(MainWindow, &rect);
                ClientToScreen(MainWindow, (LPPOINT)&rect);

                Rect dest_rect = VideoRect;
                dest_rect.X += rect.left;
                dest_rect.Y += rect.top;
                PrimarySurface->Copy_From(dest_rect, *VideoSurface, VideoRect);
            }

            /**
             *  Check if the Esc key has been pressed. If so, break out and stop all
             *  frame updates.
             */
            if (IsBreakoutAllowed) {
                if (WWKeyboard->Check() && WWKeyboard->Get() == (KN_RLSE_BIT|KN_ESC)) {
                    DEBUG_INFO("BinkMoviePlayer::Callback() - Breakout.\n");
                    break;
                }
            }

        }
    
    }

    WWKeyboard->Clear();
}


bool BinkMoviePlayer::Next_Frame(XSurface * surface, unsigned x_pos, unsigned y_pos)
{
    if (Options.VoiceVolume != BinkMasterVolume) {
        BinkMasterVolume = Options.VoiceVolume;
        BinkSetVolume(BinkHandle, (BinkMasterVolume * 32768.0f));
    }

    Resume_Pause();

    int result = BinkWait(BinkHandle);

    /**
     *  Do we have a new frame to draw?
     */
    if (NewFrame || !result) {

        /**
         *  Check to see if a frame is ready to be drawn.
         */
        while (!BinkWait(BinkHandle)) {

            /**
             *  Start decompressing the next frame.
             */
            BinkDoFrame(BinkHandle);

            NewFrame = false;

            Render_Frame(surface, x_pos, y_pos, true);

            LastFrameNum = BinkHandle->FrameNum;

            /**
             *  Next frame, please.
             */
            BinkNextFrame(BinkHandle);
        }

        return true;
    }

    return result;
}


bool BinkMoviePlayer::Advance_Frame()
{
    return Next_Frame(VideoSurface, VideoRect.X, VideoRect.Y);
}


void BinkMoviePlayer::Draw_Frame()
{
    if (VideoSurface == PrimarySurface) {
        //DEBUG_INFO("BinkMoviePlayer::Draw_Frame() - On PrimarySurface.\n");
        RECT rect;
        GetClientRect(MainWindow, &rect);
        ClientToScreen(MainWindow, (LPPOINT)&rect);
        Render_Frame(VideoSurface, rect.left + VideoRect.X, rect.top + VideoRect.Y, true);
    } else {
        //DEBUG_INFO("BinkMoviePlayer::Draw_Frame() - On custom surface.\n");
        Render_Frame(VideoSurface, VideoRect.X, VideoRect.Y, true);
    }
}


void BinkMoviePlayer::Render_Frame(XSurface * surface, unsigned x_pos, unsigned y_pos)
{
    ASSERT(surface != nullptr);

    if (!surface) {
        DEBUG_WARNING("BinkMoviePlayer::Render_Frame() - Surface is null!\n");
        return;
    }

    Render_Frame(surface, x_pos, y_pos, true);
}


void BinkMoviePlayer::Render_Frame(XSurface * surface, unsigned x_pos, unsigned y_pos, bool full)
{
    ASSERT(surface != nullptr);

    if (!surface) {
        DEBUG_WARNING("BinkMoviePlayer::Render_Frame() - Surface is null!\n");
        return;
    }

    if (Options.StretchMovies) {

        /**
         *  Lock the surface so that we can copy the decompressed frame into it.
         */
        void *buffptr = HiddenSurface->Lock();
        if (buffptr) {

            /**
             *  Clear the surfaces.
             */
            HiddenSurface->Clear();

            /**
             *  Copy the decompressed frame into the surface buffer (this might be currently on-screen).
             */
            BinkCopyToBuffer(BinkHandle, buffptr, HiddenSurface->Get_Pitch(), HiddenSurface->Get_Height(), x_pos, y_pos, (full ? SurfaceFlags|BINKCOPYALL : SurfaceFlags));

            /**
             *  Now copy to the destination surface.
             */
            surface->Copy_From(VideoStretchedRect, *HiddenSurface, VideoRect);
            
        }

        /**
         *  Finished, now unlock the BinkBuffer.
         */
        surface->Unlock();

    } else {
        
        /**
         *  Lock the surface so that we can copy the decompressed frame into it.
         */
        void *buffptr = surface->Lock();
        if (buffptr) {

            /**
             *  Copy the decompressed frame into the surface buffer (this might be currently on-screen).
             */
            BinkCopyToBuffer(BinkHandle, buffptr, surface->Get_Pitch(), surface->Get_Height(), x_pos, y_pos, (full ? SurfaceFlags|BINKCOPYALL : SurfaceFlags));        
        }

        /**
         *  Finished, now unlock the BinkBuffer.
         */
        surface->Unlock();
    }
}


bool BinkMoviePlayer::Resume_Pause()
{
    bool playing = IsPlaying;

    if (GameInFocus) {
        if (!playing) {
            DEBUG_INFO("BinkMoviePlayer::Resume_Pause() - Resume bink movie.\n");
            IsPlaying = true;
            Pause(false);
            Draw_Frame();
        }

    } else if (playing) {
        DEBUG_INFO("BinkMoviePlayer::Resume_Pause() - Pause bink movie.\n");
        IsPlaying = false;
        Pause(true);
    }

    return playing;
}


float BinkMoviePlayer::Set_Master_Volume(float vol)
{
    float old = BinkMasterVolume;
    BinkMasterVolume = vol;
    return old;
}
