/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DSHOWVIDEO.H
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
#pragma once

#include "always.h"
#include <windows.h>
#include <dshow.h>
#include <d3d9.h>
#include <control.h>
#include <string>


class BaseVideoRenderer;


enum PlaybackState
{
    STATE_NO_GRAPH,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED,
};

enum FormatType
{
    FORMAT_MP4,
    FORMAT_WMV,
    FORMAT_AVI,
    FORMAT_COUNT
};


#define NORMAL_VIDEO_WIDTH 640
#define NORMAL_VIDEO_HEIGHT 400

const long MIN_VOLUME = -10000;
const long MAX_VOLUME = 0;

const LONG ONE_MSEC = 10000;   // The number of 100-ns in 1 msec.
const LONG HALF_SEC = (ONE_MSEC*1000)/2;
const LONG ONE_SEC = (ONE_MSEC*1000);

const UINT WM_GRAPH_EVENT = WM_APP + 1;


typedef void (CALLBACK *GraphEventFN)(HWND hWnd, long eventCode, LONG_PTR param1, LONG_PTR param2);


/**
 *  
 */
class DirectShowVideoPlayer
{
    public:
        DirectShowVideoPlayer();
        //DirectShowVideoPlayer(HWND hWnd);
        DirectShowVideoPlayer(HWND hWnd, const char *pszFileName);
        ~DirectShowVideoPlayer();

        HRESULT OpenFile(const char *pszFileName);
    
        // Streaming
        HRESULT Play();
        HRESULT Play_Callback();
        HRESULT Pause();
        HRESULT Stop();

        void Reset();

        static BOOL FileAvailable(const char *pszFileName, std::string *dest = nullptr);

        // VMR functionality
        BOOL HasVideo() const;
        BOOL UpdateVideoWindow(const LPRECT prc);
        HRESULT SetVideoWindow(HWND hWnd);
        HRESULT Repaint(HDC hdc);
        HRESULT DisplayModeChanged();

        // Callback to paint a frame.
        BOOL PaintCallback();

        // Events
        HRESULT HandleGraphEvent(GraphEventFN pfnOnGraphEvent);

        // Seeking
        BOOL CanSeek() const;
        HRESULT SetPosition(REFERENCE_TIME pos);
        HRESULT GetDuration(LONGLONG *pDuration);
        HRESULT GetCurrentPosition(LONGLONG *pTimeNow);

        BOOL Finished();

        void GetInfo();

        // Audio
        HRESULT Mute(BOOL bMute);
        BOOL IsMuted() const { return m_bMute; }
        //HRESULT SetVolume(long lVolume);
        HRESULT SetVolume(float lVolume);
        //long GetVolume() const { return m_lVolume; }
        float GetVolume() const { return (-m_lVolume) * 10000; }

        // Frame stepping
        HRESULT StepOneFrame();
        HRESULT StepFrames(int nFramesToStep);

        // Playback
        HRESULT ModifyRate(double dRateAdjust);
        HRESULT SetRate(double dRate);

        void SetBreakout(bool breakout) { m_breakoutAllowed = breakout; }
        void SetStretch(bool stretch) { m_scaleAllowed = stretch; }

        void SetHWND(HWND hWnd) { m_hWnd = hWnd; }

        PlaybackState State() const { return m_state; }
        FormatType Format() const { return m_format; }

        const char *Filename() const { return m_fileName; }
        const char *FilenameNoExt() const { return m_fileNameNoExt; }

    protected:
        HRESULT InitializeGraph();
        void TearDownGraph();
        HRESULT CreateVideoRenderer();
        HRESULT RenderStreams(IBaseFilter *pSource);
        HRESULT UpdateVolume();

    private:
        PlaybackState m_state;
        FormatType m_format;

        //HWND m_hwndVideo; // Video clipping window
        //HWND m_hwndEvent; // Window to receive events
        //UINT m_EventMsg; // Windows message for graph events

        HWND m_hWnd; // Video window. This window also receives graph events.

        DWORD m_seekCaps; // Caps bits for IMediaSeeking

        // Audio
        BOOL m_bAudioStream; // Is there an audio stream?
        long m_lVolume; // Current volume (unless muted)
        BOOL m_bMute; // Volume muted?

        // Playback
        double m_dPlaybackRate;

        BOOL m_breakoutAllowed;
        BOOL m_scaleAllowed;

        const char *m_fileName;
        const char *m_fileNameNoExt;

        LONG m_videoWidth;
        LONG m_videoHeight;

        IGraphBuilder *m_pGraph;
        IBasicVideo *m_pVideo;
        IMediaControl *m_pControl;
        IMediaEventEx *m_pEvent;
        IMediaSeeking *m_pSeek;
        IMediaPosition *m_pPosition;
        IBasicAudio *m_pAudio;
        IVideoFrameStep *m_pFrameStep;
        BaseVideoRenderer *m_pVideoRenderer;
};
