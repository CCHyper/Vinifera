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


class XSurface;


/**
 *  Bink video player class that uses the games drawer.
 */
class BinkMoviePlayer
{
    public:
        BinkMoviePlayer(const char *filename);
        BinkMoviePlayer(const char *filename, XSurface *surface);
        ~BinkMoviePlayer();

        void Set_Position(unsigned x_pos, unsigned y_pos);
        void Go_To_Frame(int frame);
        void Pause(bool pause);
        bool Has_Finished() const;
        void Play();
        bool Advance_Frame();
        void Draw_Frame();
        bool Resume_Pause();

        bool Open(const char *filename);
        void Close();

        bool Next_Frame(XSurface *surface, unsigned x_pos, unsigned y_pos);
        void Render_Frame(XSurface *surface, unsigned x_pos, unsigned y_pos);
        void Render_Frame(XSurface *surface, unsigned x_pos, unsigned y_pos, bool full = false);

        HBINK Get_Bink_Handle() const { return BinkHandle; }
        bool File_Loaded() const { return BinkHandle != nullptr; }

        static float Set_Master_Volume(float vol);

    public:
        bool IsBreakoutAllowed;
        HBINK BinkHandle;
        int SurfaceFlags;
        XSurface *VideoSurface;
        Rect VideoRect;
        Rect VideoStretchedRect;
        HANDLE FileHandle;
        bool IsPlaying;
        bool NewFrame;
        unsigned LastFrameNum;
};
