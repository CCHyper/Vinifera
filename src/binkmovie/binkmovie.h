/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BINKMOVIE.H
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
#pragma once

#include "always.h"
#include "bink.h"
#include "rect.h"


class Surface;


/**
 *  Bink video player class that uses the games drawer.
 */
class BinkMoviePlayer
{
    public:
        BinkMoviePlayer();
        BinkMoviePlayer(const char *filename, bool can_be_stretched = true);
        //BinkMoviePlayer(const char *filename, Surface *surface, bool can_be_stretched = true);
        ~BinkMoviePlayer();

        bool Set_Position(Rect &rect, unsigned x_pos, unsigned y_pos);
        void Go_To_Frame(int frame);
        void Pause(bool pause);
        bool Has_Finished() const;
        void Play();
        bool Resume_Pause();
        void Set_Volume(float vol);

        bool Open(const char *filename);
        void Close();
        void Reset();
        
        bool Advance_Frame(bool draw_frame = true);
        bool Next_Frame(bool draw_frame = true);

        void Draw_Frame();

        void Set_Surface(Surface *surface) { DrawSurface = surface; }

        HBINK Get_Bink_Handle() const { return BinkHandle; }
        bool File_Loaded() const { return BinkHandle != nullptr; }
        const char *Get_Filename() const { return Filename; }

        static float Set_Master_Volume(float vol);
        static void Set_Search_Directory(const char *dir_name);
        static const char *Get_Search_Directory() { return BinkSearchDirectory; }

    public:
        /**
         *  Is the user allowed to skip this movie?
         */
        bool IsBreakoutAllowed;

        /**
         *  Is this a in-game (radar) movie?
         */
        bool IsIngameMovie;

        /**
         *  Can this movie be stretch to the full window size?
         */
        bool IsCanBeStretched;

        /**
         *  Clear screen buffer before drawing? 
         */
        bool IsClearScreenBefore;

        /**
         *  Display the current movie info for debugging purposes?
         */
        bool IsDebugOverlayEnabled;

        /**
         *  Is the movie currently playing?
         */
        bool IsPlaying;

    private:
        HBINK BinkHandle;
        HBINKBUFFER BinkBuffer;
        char Filename[256];
        int SurfaceFlags;
        int XPos;
        int YPos;
        HANDLE FileHandle;
        Surface *DrawSurface;
        float Volume;
        bool NewFrame;
        bool PlayFast;
        unsigned LastFrameNum;

    private:
        static char BinkSearchDirectory[PATH_MAX];
};
