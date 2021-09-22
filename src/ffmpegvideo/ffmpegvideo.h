/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FFMPEGVIDEO.H
 *
 *  @author        CCHyper
 *
 *  @brief         FFmpeg video player interface.
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


struct AVFormatContext;
struct AVFrame;
struct AVCodec;
struct AVCodecContext;
struct SwsContext;

class BSurface;


/**
 *  Un-stretched video size.
 */
#define NORMAL_VIDEO_WIDTH 720
#define NORMAL_VIDEO_HEIGHT 400


class FFmpegVideoPlayer
{
    public:
        FFmpegVideoPlayer();
        FFmpegVideoPlayer(HWND hWnd, const char *filename);
        ~FFmpegVideoPlayer();

        bool Open(const char *filename);
        void Close();

        void Play() {}

        bool Is_Loaded() { return true; }

    private:
        bool Draw_To_Screen();

    public:
        bool BreakoutAllowed;
        bool CanStretch;

    private:
        HWND hWnd;
        AVFormatContext *FormatContext;
        //AVFrame *Frame;
        //AVFrame *Frame2;
        //AVCodec *Dec;
        //AVCodecContext *DecContext;
        //SwsContext *ImgConvertContext;

        /**
         *  
         */
        BSurface *DrawSurface;
};


bool FFmpeg_Init();
