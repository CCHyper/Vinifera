/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DSHOWVIDEO.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         DirectShow video player interface.
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
#include "ffmpegvideo.h"
#include "tibsun_globals.h"
#include "wwkeyboard.h"
#include "options.h"
#include "bsurface.h"
#include "dsurface.h"
#include "iomap.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <string>
#include <new>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}



// Another video player
// https://github.com/pockethook/player

// https://github.com/rambodrahmani/ffmpeg-video-player

// https://github.com/fosterseth/sdl2_video_player





// USES DIRECT X!
// https://github.com/Yacov-lu/ffmpeg-DXVA-decode/tree/master/ffmpeg_DXVA_decoder/ffmpeg_DXVA_decoder

// DIRECT SHOW
// https://github.com/dpeng/ffmpegplay




// http://dranger.com/ffmpeg/tutorial01.html
// https://github.com/FFmpeg/FFmpeg/blob/master/fftools/ffplay.c


/**
 *  Default constructor.
 * 
 *  @author: CCHyper
 */
FFmpegVideoPlayer::FFmpegVideoPlayer() :
    hWnd(nullptr),
    FormatContext(nullptr)
{
}


/**
 *  Constructor that takes window handle and filename.
 * 
 *  @author: CCHyper
 */
FFmpegVideoPlayer::FFmpegVideoPlayer(HWND hWnd, const char *filename) :
    hWnd(nullptr),
    FormatContext(nullptr)
{
    Open(filename);
}


/**
 *  
 *  Class destructor
 * 
 *  @author: CCHyper
 */
FFmpegVideoPlayer::~FFmpegVideoPlayer()
{
    Close();
}


void FFmpegVideoPlayer::Close()
{
    avformat_close_input(&FormatContext);

    delete DrawSurface;
    DrawSurface = nullptr;
}


/**
 *  Open a media file ready for playback.
 * 
 *  @author: CCHyper
 */
bool FFmpegVideoPlayer::Open(const char *filename)
{
    int retcode;        // Error return code.

    DEBUG_INFO("FFmpeg: Opening \"%s\"", filename);

    if (FormatContext) {
        DEBUG_ERROR("FFmpeg: Existing context already allocated!\n");
        return false;
    }

    FormatContext = avformat_alloc_context();
    if (!FormatContext) {
        DEBUG_ERROR("FFmpeg: Failed to allocate format context!\n");
        return false;
    }

    char video_filename[PATH_MAX];
    std::snprintf(video_filename, sizeof(video_filename), "%s.MP4", filename);

    const void *video_file_ptr = MFCC::Retrieve(video_filename);
    if (video_file_ptr) {

        // TODO: Open stream from memory pointer.

        DEBUG_ERROR("FFmpeg: Failed to open input file from mix!\n");
        return false;

    } else {

        std::snprintf(video_filename, sizeof(video_filename), "MOVIES/%s.MP4", filename);

        if (avformat_open_input(&FormatContext, video_filename, nullptr, nullptr) < 0) {
            DEBUG_ERROR("FFmpeg: Failed to open input file!\n");
            return false;
        }

    }

    DEBUG_INFO("FFmpeg: Format:%s, Duration:%lld us", FormatContext->iformat->long_name, FormatContext->duration);

    if (avformat_find_stream_info(FormatContext, nullptr) < 0) {
        DEBUG_ERROR("FFmpeg: Failed get stream info!\n");
        return false;
    }

#ifndef NDEBUG
    /**
     *  Dump information about file onto standard error.
     */
    av_dump_format(FormatContext, 0, video_filename, 0);
#endif

    AVCodec *codec = nullptr;
    AVCodecParameters *codec_parameters =  nullptr;
    int video_stream_index = -1;
    int audio_stream_index = -1;

    for (int i = 0; i < FormatContext->nb_streams; ++i) {

        AVCodecParameters *local_codec_parameters = FormatContext->streams[i]->codecpar;
        DEBUG_INFO("FFmpeg: AVStream->time_base before open coded %d/%d\n", FormatContext->streams[i]->time_base.num, FormatContext->streams[i]->time_base.den);
        DEBUG_INFO("FFmpeg: AVStream->r_frame_rate before open coded %d/%d\n", FormatContext->streams[i]->r_frame_rate.num, FormatContext->streams[i]->r_frame_rate.den);
        DEBUG_INFO("FFmpeg: AVStream->start_time %" PRId64 "\n", FormatContext->streams[i]->start_time);
        DEBUG_INFO("FFmpeg: AVStream->duration %" PRId64 "\n", FormatContext->streams[i]->duration);

        DEBUG_INFO("FFmpeg: Finding the proper decoder (codec)...\n");

        /**
         *  Find the decoder for the video stream.
         */
        AVCodec *pLocalCodec = avcodec_find_decoder(local_codec_parameters->codec_id);
        if (pLocalCodec == nullptr) {
            DEBUG_ERROR("FFmpeg: Unsupported codec!\n");
            continue;
        }

        if (local_codec_parameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            DEBUG_INFO("FFmpeg: Video Codec: resolution %d x %d\n", local_codec_parameters->width, local_codec_parameters->height);
            if (video_stream_index == -1) {
                video_stream_index = i;
                codec = pLocalCodec;
                codec_parameters = local_codec_parameters;
            }
        }

        if (local_codec_parameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            DEBUG_INFO("FFmpeg: Audio Codec: resolution %d x %d\n", local_codec_parameters->width, local_codec_parameters->height);
            if (audio_stream_index == -1) {
                audio_stream_index = i;
                //codec = pLocalCodec;
                //codec_parameters = local_codec_parameters;
            }
        }

        DEBUG_INFO("FFmpeg: Codec:%s ID:%d BitRate:%lld\n", pLocalCodec->name, pLocalCodec->id, local_codec_parameters->bit_rate);
    }

    if (video_stream_index == -1) {
        DEBUG_ERROR("FFmpeg: \"%s\" does not contain a video stream!\n", filename);
        return false;
    }

    if (audio_stream_index == -1) {
        DEBUG_WARNING("FFmpeg: \"%s\" does not contain a audio stream!\n", filename);
        //return false; // No audio stream is not an error for us.
    }

    /**
     *  Copy the context.
     */
    AVCodecContext *codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        DEBUG_ERROR("FFmpeg: Failed to allocate codec context!\n");
        return false;
    }
    if (avcodec_parameters_to_context(codec_context, codec_parameters) < 0) {
        DEBUG_ERROR("FFmpeg: Failed to copy codec params to codec context!\n");
        return false;
    }

    /**
     *  Open codec.
     */
    if (avcodec_open2(codec_context, codec, nullptr) < 0) {
        DEBUG_ERROR("FFmpeg: Failed to open video decoder!\n");
        return false;
    }

    /**
     *  
     */
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        DEBUG_ERROR("FFmpeg: Failed to allocate frame!\n");
        return false;
    }

    /**
     *  
     */
    AVFrame *frameRGB565 = av_frame_alloc();
    if (!frameRGB565) {
        DEBUG_ERROR("FFmpeg: Failed to allocate RGB565 frame!\n");
        return false;
    }

    /**
     *  Determine required buffer size and allocate buffer.
     */
    int byte_buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGB565BE, codec_context->width, codec_context->height, 16);
    unsigned char *byte_buffer = (unsigned char *)av_malloc(byte_buffer_size * sizeof(unsigned char));
    if (!byte_buffer) {
        DEBUG_ERROR("FFmpeg: Unable to allocate output buffer!\n");
        return false;
    }

    /**
     *  Associate the frame with our newly allocated buffer.
     */
    avpicture_fill((AVPicture *)frameRGB565, byte_buffer, AV_PIX_FMT_RGB565BE, codec_context->width, codec_context->height);

    /**
     *  Initialize SWS context for software scaling.
     */
    SwsContext *sws_ctx = sws_getContext(codec_context->width,
        codec_context->height,
        codec_context->pix_fmt,
        codec_context->width,
        codec_context->height,
        AV_PIX_FMT_RGB565BE,
        SWS_BICUBIC,
        nullptr,
        nullptr,
        nullptr
        );

    /**
     *  Allocate the frame packet.
     */
    AVPacket *packet = av_packet_alloc();
    if (!packet) {
        DEBUG_ERROR("FFmpeg: Failed to allocate packet!\n");
        return false;
    }

    int frame_finished = false;
    int response = 0;
    int how_many_packets_to_process = 8;

    while (av_read_frame(FormatContext, packet) >= 0) {

        av_init_packet(packet);

        if (packet->stream_index == video_stream_index) {
            DEBUG_INFO("FFmpeg: [Video] AVPacket->pts %" PRId64 "\n", packet->pts);

            /**
             *  Decode video frame.
             */
            retcode = avcodec_decode_video2(codec_context, frame, &frame_finished, packet);
            if (retcode < 0) {
                DEBUG_ERROR("FFmpeg: [Video] Error decoding frame!\n");
                return false;
            }

            /**
             *  Did we get a video frame?
             */
            if (frame_finished) {

                /**
                 *  Convert the image from its native format to RGB65.
                 */
                sws_scale(sws_ctx, (const uint8_t * const *)frame->data,
                            frame->linesize, 0, codec_context->height,
                            frameRGB565->data, frameRGB565->linesize);

                /**
                 *  
                 */
                int num_bytes_written = av_image_copy_to_buffer(byte_buffer, byte_buffer_size,
                                                       (const uint8_t * const *)frame->data,
                                                       (const int *)frame->linesize, codec_context->pix_fmt,
                                                       frame->width, frame->height, 1);
                if (num_bytes_written < 0) {
                    DEBUG_ERROR("FFmpeg: [Video] Unable to copy image to buffer!\n");
                    return false;
                }



                /**
                 *  Create a one-time buffered draw surface for copying to the screen.
                 */
                if (!DrawSurface) {
                    DrawSurface = new BSurface(PrimarySurface->Get_Width(),
                                               PrimarySurface->Get_Height(),
                                               PrimarySurface->Get_Bytes_Per_Pixel());
                }

                if (!DrawSurface) {
                    DEBUG_ERROR("FFmpeg: Draw surface is null!\n");
                    return false;
                }

                void *buffptr = DrawSurface->Lock();
                if (buffptr) {

                    // TODO: Copy ffmpeg image to game surface here.

                    DrawSurface->Unlock();
                }

                GScreenClass::Blit(false, DrawSurface);


                //response = decode_packet(packet, codec_context, frame);
                //if (response < 0) {
                //    break;
                //}
                //if (--how_many_packets_to_process <= 0) {
                //    break;
                //}


                /**
                 *  Draw the video frame to the game screen.
                 */
                Draw_To_Screen();

            }

        } else if (packet->stream_index == audio_stream_index) {
            DEBUG_INFO("FFmpeg: [Audio] AVPacket->pts %" PRId64 "\n", packet->pts);

            /**
             *  Decode audio frame.
             */
            retcode = avcodec_decode_audio4(codec_context, frame, &frame_finished, packet);
            if (retcode < 0) {
                DEBUG_ERROR("FFmpeg: [Audio] Error decoding frame!\n");
                return false;
            }
        }

        Sleep(33); // Sleep for 33 msec.

        /**
         *  Check for any keyboard input.
         */
        if (WWKeyboard->Check()) {

            switch (WWKeyboard->Get()) {

                /**
                 *  Check if the ESC key has been pressed. If so, break out
                 *  and stop all updates.
                 */
                case (KN_RLSE_BIT|KN_ESC):
                    if (BreakoutAllowed) {
                        DEBUG_INFO("FFmpeg: Breakout.\n");
                    }
                    break;

            };

        }

        /**
         *  Free the packet.
         */
        av_packet_unref(packet);
    }
      
    DEBUG_INFO("FFmpeg: Releasing all resources\n");

    avformat_close_input(&FormatContext);

    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codec_context);

    return true;
}


bool FFmpeg_Init()
{
    /**
     *  This registers all available file formats and codecs with the library
     *  so they will be used automatically when a file with the corresponding
     *  format/codec is opened.
     */
    av_register_all();

    return true;
}
