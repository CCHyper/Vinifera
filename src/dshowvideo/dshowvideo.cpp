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
#include "dshowvideo.h"
#include "dshowrenderer.h"
#include "dshowutil.h"
#include "tibsun_globals.h"
#include "vinifera_util.h"
#include "options.h"
#include "ccfile.h"
#include "ini.h"
#include "winutil.h"
#include "wwkeyboard.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <string>
#include <new>


#include "tspp.h"
DEFINE_IMPLEMENTATION(bool VQA_Movie_Message_Loop(), 0x0066B230);



// https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/multimedia/directshow/players/dshowplayer/DShowPlayer.cpp

// https://github.com/eaglezhao/tracnghiemweb/blob/aaae7bb7b9393a8000d395c1d98dcfc389e3c4ed/TestKhoaLuan/DirectShowTVSample/DSBuild/DSBuild.cpp

// https://github.com/svn2github/FS2Open_Trunk/blob/f1d7904e74141b21ecf691a393d730bb8ba0e134/fs2_open-unstable/code/directx/dx8show.cpp



extern HRESULT AddFilterByCLSID(IGraphBuilder *pGraph, REFGUID clsid, IBaseFilter **ppF, LPCWSTR wszName);
extern HRESULT RemoveUnconnectedRenderer(IGraphBuilder *pGraph, IBaseFilter *pRenderer, BOOL *pbRemoved);
extern HRESULT InitializeEVR(IBaseFilter *pEVR, HWND hwnd, IMFVideoDisplayControl ** ppWc); 
extern HRESULT InitWindowlessVMR9(IBaseFilter *pVMR, HWND hwnd, IVMRWindowlessControl9 ** ppWc); 
extern HRESULT InitWindowlessVMR(IBaseFilter *pVMR, HWND hwnd, IVMRWindowlessControl** ppWc); 
extern HRESULT FindConnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);




/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
DirectShowVideoPlayer::DirectShowVideoPlayer() :
    m_state(STATE_NO_GRAPH),
    //m_hwndVideo(hwndVideo),
    //m_hwndEvent(nullptr),
    //m_EventMsg(0),
    m_hWnd(nullptr),
    m_fileName(nullptr),
    m_pGraph(nullptr),
    m_pControl(nullptr),
    m_pEvent(nullptr),
    m_pSeek(nullptr),
    m_pAudio(nullptr),
    m_pVideo(nullptr),
    m_pVideoRenderer(nullptr),
    m_pPosition(nullptr),
    m_pFrameStep(nullptr),
    m_videoWidth(640),
    m_videoHeight(400),
    m_seekCaps(0),
    m_breakoutAllowed(true),
    m_scaleAllowed(true),
    m_bMute(false),
    m_dPlaybackRate(1.0),
    m_lVolume(MAX_VOLUME)
{
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
/*DirectShowVideoPlayer::DirectShowVideoPlayer(HWND hWnd) :
    m_state(STATE_NO_GRAPH),
    //m_hwndVideo(hwndVideo),
    //m_hwndEvent(nullptr),
    //m_EventMsg(0),
    m_hWnd(hWnd),
    m_fileName(nullptr),
    m_pGraph(nullptr),
    m_pControl(nullptr),
    m_pEvent(nullptr),
    m_pSeek(nullptr),
    m_pAudio(nullptr),
    m_pVideo(nullptr),
    m_pVideoRenderer(nullptr),
    m_pPosition(nullptr),
    m_pFrameStep(nullptr),
    m_videoWidth(640),
    m_videoHeight(400),
    m_seekCaps(0),
    m_breakoutAllowed(true),
    m_scaleAllowed(true),
    m_bMute(false),
    m_dPlaybackRate(1.0),
    m_lVolume(MAX_VOLUME)
{
}*/


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
DirectShowVideoPlayer::DirectShowVideoPlayer(HWND hWnd, const char *pszFileName) :
    m_state(STATE_NO_GRAPH),
    //m_hwndVideo(hwndVideo),
    //m_hwndEvent(nullptr),
    //m_EventMsg(0),
    m_hWnd(hWnd),
    m_fileName(pszFileName),
    m_pGraph(nullptr),
    m_pControl(nullptr),
    m_pEvent(nullptr),
    m_pSeek(nullptr),
    m_pAudio(nullptr),
    m_pVideo(nullptr),
    m_pVideoRenderer(nullptr),
    m_pPosition(nullptr),
    m_pFrameStep(nullptr),
    m_videoWidth(640),
    m_videoHeight(400),
    m_seekCaps(0),
    m_breakoutAllowed(true),
    m_scaleAllowed(true),
    m_bMute(false),
    m_dPlaybackRate(1.0),
    m_lVolume(MAX_VOLUME)
{
    OpenFile(pszFileName);
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
DirectShowVideoPlayer::~DirectShowVideoPlayer()
{
    TearDownGraph();
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
void DirectShowVideoPlayer::Reset()
{
    TearDownGraph();
}


/**
 *  Open a media file ready for playback.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::OpenFile(const char *pszFileName)
{
    const char *tmp_filename = nullptr;
    IBaseFilter *pSource = nullptr;
    std::string dest_filename;
    HRESULT hr;

    /**
     *  Create a new filter graph (This also closes the old one).
     */
    hr = InitializeGraph();
    if (FAILED(hr)) {
        goto done;
    }

    WCHAR wFileName[MAX_PATH];

    /**
     *  Check to see if the file exists.
     */
    if (!FileAvailable(pszFileName, &dest_filename)) {
        goto done;
    }

    MultiByteToWideChar(CP_ACP, 0, dest_filename.c_str(), -1, wFileName, MAX_PATH);
    
    /**
     *  Add the source filter to the graph.
     */
    hr = m_pGraph->AddSourceFilter(wFileName, nullptr, &pSource);
    if (FAILED(hr)) {
        goto done;
    }

    /**
     *  Store the filename.
     */
    m_fileName = tmp_filename;
    m_fileNameNoExt = pszFileName;

    /**
     *  Try to render the streams.
     */
    hr = RenderStreams(pSource);
    if (FAILED(hr)) {
        goto done;
    }

    /**
     *  Get the seeking capabilities.
     */
    hr = m_pSeek->GetCapabilities(&m_seekCaps);
    if (FAILED(hr)) {
        goto done;
    }

    /**
     *  Set the volume.
     */
    hr = UpdateVolume();
    if (FAILED(hr)) {
        goto done;
    }

    /**
     *  Update our state.
     */
    m_state = STATE_STOPPED;

done:
    if (FAILED(hr)) {
        TearDownGraph();
    }

    SafeRelease(&pSource);

    return hr;
}


/**
 *  Respond to a graph event.
 * 
 *  The owning window should call this method when it receives the window
 *  message that the application specified when it called SetEventWindow.
 * 
 *  pfnOnGraphEvent:
 *      Pointer to the GraphEventCallback callback, implemented by 
 *      the application. This callback is invoked once for each event
 *      in the queue.
 * 
 *  @warning: Do not tear down the graph from inside the callback.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::HandleGraphEvent(GraphEventFN pfnOnGraphEvent)
{
    if (!m_pEvent) {
        return E_UNEXPECTED;
    }

    if (!m_pEvent) {
        return E_UNEXPECTED;
    }

    long evCode = 0;
    LONG_PTR param1 = 0;
    LONG_PTR param2 = 0;

    HRESULT hr = S_OK;
    
    /**
     *  Get the events from the queue.
     */
    while (SUCCEEDED(m_pEvent->GetEvent(&evCode, &param1, &param2, 0))) {

        /**
         *  Invoke the callback.
         */
        pfnOnGraphEvent(m_hWnd, evCode, param1, param2);
        
        /**
         *  Free the event data.
         */
        hr = m_pEvent->FreeEventParams(evCode, param1, param2);
        if (FAILED(hr)) {
            break;
        }
    }

    return hr;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::Play()
{
    if (m_state != STATE_PAUSED && m_state != STATE_STOPPED) {
        return VFW_E_WRONG_STATE;
    }

    ASSERT(m_pGraph); // If state is correct, the graph should exist.

    /**
     *  Run the graph to play the media file.
     */
    HRESULT hr = m_pControl->Run();
    if (SUCCEEDED(hr)) {
        m_state = STATE_RUNNING;
    }

    return hr;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::Play_Callback()
{
    Play();

    WWKeyboard->Clear();

    //if (m_scaleAllowed) {
    //    InvalidateRect(m_hWnd, nullptr, false);
    //}

    PAINTSTRUCT ps;
    HDC hdc;

    hdc = BeginPaint(m_hWnd, &ps);

    bool process = true;

    while (process) {

        /**
         *  Are there any Windows messages to handle?
         */
        if (!VQA_Movie_Message_Loop()) {
            break;
        }

        /**
         *  Handle window focus loss and restore.
         */
        if (!GameInFocus && m_state == STATE_PAUSED) {
            DEBUG_INFO("DirectShow: Focus Restore.\n");
            Play();

        } else if (!GameInFocus && m_state == STATE_RUNNING) {
            DEBUG_INFO("DirectShow: Focus Loss.\n");
            Pause();
        }

        /**
         *  Has the video finished? If so, stop it.
         */
        if (Finished()) {
            DEV_DEBUG_INFO("DirectShow: Video finished.\n");
            m_state = STATE_STOPPED;
            //break;
        }
    
        /**
         *  HAve we lost the video?
         */
        if (!HasVideo()) {
            DEV_DEBUG_INFO("DirectShow: Video lost, ending playback.\n");
            break;
        }
    
        /**
         *  If we have been flagged as finished, we are done.
         */
        if (m_state == STATE_STOPPED) {
            DEV_DEBUG_INFO("DirectShow: Video stopped, ending playback.\n");
            break;
        }

        /**
         *  Are we still playing? Draw the next frame.
         */
        if (m_state == STATE_RUNNING) {

            /**
             *  The player has video, so ask the player to repaint. 
             */
            Repaint(hdc);

        } else {

            /**
             *  We are paused. we don't need to redraw every tick, so add a little wait to take
             *  the stress away from the CPU, because you know, it has a hard life...
             */
            Repaint(hdc);
            Sleep(33); // Sleep for 33 msec.
        }

        /**
         *  Check for any keyboard input.
         */
        if (WWKeyboard->Check()) {

            switch (WWKeyboard->Get()) {

                /**
                 *  Debug only: Space bar toggles pause.
                 */
                case (KN_RLSE_BIT|KN_SPACE):
                    if (Vinifera_DeveloperMode) {
                        if (m_state == STATE_PAUSED) {
                            DEV_DEBUG_INFO("DirectShow: RESUME\n");
                            Play();
                        } else {
                            DEV_DEBUG_INFO("DirectShow: PAUSE\n");
                            Pause();
                        }
                    }
                    break;

                /**
                 *  Debug only: Display the debug overlay.
                 */
                case (KN_RLSE_BIT|KN_D):
                    if (Vinifera_DeveloperMode) {
                    }
                    break;

                /**
                 *  Debug only: Restart playback.
                 */
                case (KN_RLSE_BIT|KN_R):
                    if (Vinifera_DeveloperMode && CanSeek()) {
                        DEV_DEBUG_INFO("DirectShow: Restarting playback.\n");
                        SetPosition(0);
                    }
                    break;

                /**
                 *  Debug only: Mute volume.
                 */
                case (KN_RLSE_BIT|KN_M):
                    if (Vinifera_DeveloperMode) {
                        if (IsMuted()) {
                            DEV_DEBUG_INFO("DirectShow: Mute.\n");
                            Mute(false);
                        } else {
                            DEV_DEBUG_INFO("DirectShow: Unmute.\n");
                            Mute(true);
                        }
                    }
                    break;

                /**
                 *  Debug only: Adjust playback rate.
                 */
                case (KN_RLSE_BIT|KN_MINUS):
                    if (Vinifera_DeveloperMode) {
                        DEV_DEBUG_INFO("DirectShow: Rate down.\n");
                        SetRate(m_dPlaybackRate-0.10f);
                    }
                    break;
                case (KN_RLSE_BIT|KN_EQUAL):
                    if (Vinifera_DeveloperMode) {
                        DEV_DEBUG_INFO("DirectShow: Rate up.\n");
                        SetRate(m_dPlaybackRate+0.10f);
                    }
                    break;

                /**
                 *  Debug only: Adjust volume.
                 */
                case (KN_RLSE_BIT|KN_KEYPAD_MINUS):
                    if (Vinifera_DeveloperMode) {
                        DEV_DEBUG_INFO("DirectShow: Volume down.\n");
                        //Set_Volume(Volume-0.10f);
                    }
                    break;
                case (KN_RLSE_BIT|KN_KEYPAD_PLUS):
                    if (Vinifera_DeveloperMode) {
                        DEV_DEBUG_INFO("DirectShow: Volume up.\n");
                        //Set_Volume(Volume+0.10f);
                    }
                    break;

                /**
                 *  Debug only: Frame step.
                 */
                case (KN_RLSE_BIT|KN_F1):
                {
                    if (Vinifera_DeveloperMode && CanSeek()) {
                        DEV_DEBUG_INFO("DirectShow: Frame step.\n");
                        StepOneFrame();
                    }
                    break;
                }

                /**
                 *  Debug only: Previous and next frame. The payback must be
                 *              paused for these keys to work.
                 */
                case (KN_RLSE_BIT|KN_COMMA):
                {
                    if (Vinifera_DeveloperMode && CanSeek()) {
                        DEV_DEBUG_INFO("DirectShow: Seek back .5 second.\n");
                        LONGLONG current;
                        GetCurrentPosition(&current);
                        SetPosition(current - HALF_SEC);
                    }
                    break;
                }
                case (KN_RLSE_BIT|KN_PERIOD):
                    if (Vinifera_DeveloperMode && CanSeek()) {
                        DEV_DEBUG_INFO("DirectShow: Seek forward .5 second.\n");
                        LONGLONG current;
                        GetCurrentPosition(&current);
                        SetPosition(current + HALF_SEC);
                    }
                    break;

                /**
                 *  Check if the ESC key has been pressed. If so, break out
                 *  and stop all frame updates.
                 */
                case (KN_RLSE_BIT|KN_ESC):
                    if (m_breakoutAllowed) {
                        DEBUG_INFO("DirectShow: Breakout.\n");
                        Stop();
                        UpdateWindow(m_hWnd);
                        //m_state = STATE_STOPPED;
                        //process = false;
                    }
                    break;
            };

        }
    
    }

    /**
     *  
     */
    TearDownGraph();    // #TODO: Causes flicker of black at end?

    //GetWindowRect( hwnd, &rcWnd );
    //ScreenToClient( parWnd, &rcWnd ); // Convert to the parent's co-ordinates
    //
    //SetBkMode(hdc, 0);
    //Rectangle(hdc, thisX, thisY, thisW, thisH); // doesn't work just makes the window a big black rectangle?

    EndPaint(m_hWnd, &ps);

    //if (m_scaleAllowed) {
    //    InvalidateRect(m_hWnd, nullptr, false);
    //}

    /**
     *  Clear the keyboard buffers.
     */
    WWKeyboard->Clear();

    return S_OK;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::Pause()
{
    if (m_state != STATE_RUNNING) {
        return VFW_E_WRONG_STATE;
    }

    ASSERT(m_pGraph); // If state is correct, the graph should exist.

    HRESULT hr = m_pControl->Pause();
    if (SUCCEEDED(hr)) {
        m_state = STATE_PAUSED;
    }

    return hr;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::Stop()
{
    if (m_state != STATE_RUNNING && m_state != STATE_PAUSED) {
        return VFW_E_WRONG_STATE;
    }

    ASSERT(m_pGraph); // If state is correct, the graph should exist.

    HRESULT hr = m_pControl->Stop();
    if (SUCCEEDED(hr)) {
        m_state = STATE_STOPPED;
    }

    return hr;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
BOOL DirectShowVideoPlayer::FileAvailable(const char *pszFileName, std::string *dest)
{
    for (int format = 0; format < FORMAT_COUNT; ++format) {
        
        std::string filename;
        std::string tmp;

        // Reset filename.
        filename = pszFileName;

        switch (format) {
            case FORMAT_MP4:
                filename += ".MP4";
                break;
            case FORMAT_WMV:
                filename += ".WMV";
                break;
            default:
            case FORMAT_AVI:
                filename += ".AVI";
                break;
        };

#ifndef NDEBUG
        /**
         *  Debug only: Load from the environment var path.
         * 
         *  @note: Path must end in "\" otherwise this will fail.
         */
        char var_buff[PATH_MAX];
        DWORD rc = GetEnvironmentVariable("TIBSUN_MOVIES", var_buff, sizeof(var_buff));
        if (rc && rc < sizeof(var_buff)) {
            tmp = var_buff;
            tmp += filename;
            DEV_DEBUG_INFO("DirectShow: Search for \"%s\".\n", tmp.c_str());
            if (CCFileClass(tmp.c_str()).Is_Available()) {
                if (dest) *dest = tmp;
                return true;
            }
        }
#endif
        
        /**
         *  Load from the root directory.
         */
        tmp = Get_Module_Directory();
        tmp += filename;
        DEBUG_INFO("DirectShow: Search for \"%s\" in root directory.\n", tmp.c_str());
        if (CCFileClass(tmp.c_str()).Is_Available()) {
            if (dest) *dest = tmp;
            DEBUG_INFO("DirectShow: Found \"%s\".\n", tmp.c_str());
            return true;
        }

        /**
         *  Load from the sub directory directory.
         */
        tmp = Get_Module_Directory();
        tmp += std::string("MOVIES\\");
        tmp += filename;
        DEBUG_INFO("DirectShow: Search for \"%s\" in MOVIES directory.\n", tmp.c_str());
        if (CCFileClass(tmp.c_str()).Is_Available()) {
            if (dest) *dest = tmp;
            DEBUG_INFO("DirectShow: Found \"%s\".\n", tmp.c_str());
            return true;
        }

        /**
         *  Lastly check in the games mix files.
         */
        DEBUG_INFO("DirectShow: Search for \"%s\" in mix files.\n", filename.c_str());
        if (CCFileClass(filename.c_str()).Is_Available()) {
            if (dest) *dest = filename;
            DEBUG_INFO("DirectShow: Found \"%s\".\n", filename.c_str());
            return true;
        }

    }

    return false;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
BOOL DirectShowVideoPlayer::HasVideo() const 
{
    //if (!m_pVideoRenderer) {
    //    DEBUG_ERROR("DirectShow: m_pVideoRenderer is null!\n");
    //    return false;
    //}
    //if (!m_pVideoRenderer->HasVideo()) {
    //    DEBUG_ERROR("DirectShow: HasVideo returned false!\n");
    //    return false;
    //}
    return m_pVideoRenderer && m_pVideoRenderer->HasVideo(); 
    //return true;
}


/**
 *  Sets the destination rectangle for the video.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
BOOL DirectShowVideoPlayer::UpdateVideoWindow(const LPRECT prc)
{
    bool fixup_default_scale = false;

    /**
     *  Get the size of the current window.
     */
    RECT rect;

    /**
     *  This is a workaround to make sure that we stretch the video to the size set by
     *  a custom DDRAW dll, even if the size is larger than the game resolution.
     */
    if (RawFileClass("DDRAW.DLL").Is_Available()) {
        RawFileClass ddrawfile("DDRAW.INI");
        INIClass ddrawini;

        ddrawini.Load(ddrawfile);

        GetClientRect(m_hWnd, &rect);

        rect.right = ddrawini.Get_Int("ddraw", "width", rect.right);
        rect.bottom = ddrawini.Get_Int("ddraw", "height", rect.bottom);

        if (Options.ScreenWidth != rect.right || Options.ScreenHeight != rect.bottom) {
            fixup_default_scale = true;
            DEV_DEBUG_WARNING("DirectShow: Possible resolution mismatch in ddraw.ini!\n");
        }
    }

    /**
     *  We detected a possible resolution mis-match, attempt to fix it.
     */
    if (fixup_default_scale) {

        /**
         *  #TODO:
         *  For now we have to assume that the user set the resolution
         *  in DDRAW.INI to x2 of the game resolution.
         */
        rect.left = prc->left*2;
        rect.top = prc->top*2;
        rect.right = prc->right*2;
        rect.bottom = prc->bottom*2;

    } else {
        rect.left = prc->left;
        rect.top = prc->top;
        rect.right = prc->right;
        rect.bottom = prc->bottom;
    }

    if (m_pVideoRenderer) {
        //return m_pVideoRenderer->UpdateVideoWindow(m_hwndVideo, &rect);
        return m_pVideoRenderer->UpdateVideoWindow(m_hWnd, &rect) == S_OK;
    }

    return false;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::SetVideoWindow(HWND hWnd)
{
    if (!m_pVideoRenderer /*|| !m_pVideo*/) {
        return S_OK;
    }

    bool fixup_default_scale = false;

    /**
     *  Get the size of the current window.
     */
    RECT rect;
    GetClientRect(hWnd, &rect);

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
            DEV_DEBUG_WARNING("DirectShow: Possible resolution mismatch in ddraw.ini!\n");
        }
    }

    /**
     *  Store the screen surface size.
     */
    int surface_width = rect.right;
    int surface_height = rect.bottom;

    /**
     *  Stretch (while maintaining aspect ratio) the movie to the window size.
     */
    Options.StretchMovies = true;
    if (m_scaleAllowed && Options.StretchMovies) {

        // #TODO: m_pVideo null here for some reason?
        /**
         *  Fetch the video dimensions.
         */
        //if (m_pVideo) {
        //    HRESULT hr = m_pVideo->GetVideoSize(&m_videoWidth, &m_videoHeight);
        //    if (FAILED(hr)) {
        //        return hr;
        //    }
        //}

        /**
         *  Set the full window size, DirectShow will do the scaling for us.
         */
        rect.left = 0;
        rect.top = 0;
        rect.right = surface_width;
        rect.bottom = surface_height;

    /**
     *  We detected a possible resolution mis-match, attempt to fix it.
     */
    } else if (fixup_default_scale) {
    
        /**
         *  #TODO:
         *  For now we have to assume that the user set the resolution
         *  in DDRAW.INI to x2 of the game resolution.
         */
        rect.left = (surface_width-(NORMAL_VIDEO_WIDTH*2))/2;
        rect.top = (surface_height-(NORMAL_VIDEO_HEIGHT*2))/2;
        rect.right = (rect.left+(NORMAL_VIDEO_WIDTH*2));
        rect.bottom = (rect.top+(NORMAL_VIDEO_HEIGHT*2));

    } else {

        /**
         *  Even if the video is larger than 640x400, we still
         *  retain the original expectations of the video playback
         *  and tell DirectShow to scale the video down.
         */
        rect.left = (surface_width-(NORMAL_VIDEO_WIDTH))/2;
        rect.top = (surface_height-(NORMAL_VIDEO_HEIGHT))/2;
        rect.right = (rect.left+(NORMAL_VIDEO_WIDTH));
        rect.bottom = (rect.top+(NORMAL_VIDEO_HEIGHT));
    }

    DEBUG_INFO("DirectShow: %d,%d,%d,%d\n", rect.left, rect.top, rect.right, rect.bottom);

    return m_pVideoRenderer->UpdateVideoWindow(m_hWnd, &rect);
}


/**
 *  Repaints the video. Call this method when the application receives WM_PAINT.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::Repaint(HDC hdc)
{
    if (m_pVideoRenderer) {
        //return m_pVideoRenderer->Repaint(m_hwndVideo, hdc);
        return m_pVideoRenderer->Repaint(m_hWnd, hdc);
    }

    return S_OK;
}


/**
 *  Notifies the video renderer that the display mode changed.
 *  Call this method when the application receives WM_DISPLAYCHANGE.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::DisplayModeChanged()
{
    if (m_pVideoRenderer) {
        return m_pVideoRenderer->DisplayModeChanged();
    }

    return S_OK;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
BOOL DirectShowVideoPlayer::PaintCallback()
{
    /**
     *  Draw the next frame if currently playing.
     */
    if (m_state == STATE_RUNNING) {

        HDC hdc = nullptr;
        HDC hdcMem = nullptr;
        HBITMAP hbmMem = nullptr;
        HANDLE hOld = nullptr;
        PAINTSTRUCT ps;

//        // Get DC for window
//        hdc = BeginPaint(m_hWnd, &ps);
// 
//        // Create an off-screen DC for double-buffering
//        hdcMem = CreateCompatibleDC(hdc);
//        hbmMem = CreateCompatibleBitmap(hdc, 140*2, 110*2);
// 
//        hOld = SelectObject(hdcMem, hbmMem);
// 
//        // Draw into hdcMem here
//        Repaint(hdcMem);
// 
//        // Transfer the off-screen DC to the screen
//        BitBlt(hdc, 0, 0, 140*2, 110*2, hdcMem, 0, 0, SRCCOPY);
// 
//        // Free-up the off-screen DC
//        SelectObject(hdcMem, hOld);
// 
//        DeleteObject(hbmMem);
//        DeleteDC(hdcMem);
// 
//        EndPaint(m_hWnd, &ps);




#if 1
        /**
         *  The player has video, so ask the player to repaint. 
         */
        //if (!hdc) {
        hdc = BeginPaint(m_hWnd, &ps);
        //}

        //DEV_DEBUG_INFO("DirectShow: Paint frame.\n");

        Repaint(hdc);

        EndPaint(m_hWnd, &ps);
#endif




    } else if (Finished() || !HasVideo()) {
        DEV_DEBUG_INFO("DirectShow: Video finished.\n");
        m_state = STATE_STOPPED;
    }


    // Causes performance issues???
    //InvalidateRect(m_hWnd, nullptr, false);

    return true;
}


/**
 *  Returns TRUE if the current file is seekable.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
BOOL DirectShowVideoPlayer::CanSeek() const
{
    const DWORD caps = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanGetDuration;
    return ((m_seekCaps & caps) == caps);
}


/**
 *  Seeks to a new position.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::SetPosition(REFERENCE_TIME pos)
{
    if (m_pControl == NULL || m_pSeek == NULL) {
        return E_UNEXPECTED;
    }

    HRESULT hr = S_OK;

    hr = m_pSeek->SetPositions(&pos, AM_SEEKING_AbsolutePositioning,
        NULL, AM_SEEKING_NoPositioning);

    if (SUCCEEDED(hr)) {

        // If playback is stopped, we need to put the graph into the paused
        // state to update the video renderer with the new frame, and then stop 
        // the graph again. The IMediaControl::StopWhenReady does this.
        if (m_state == STATE_STOPPED) {
            hr = m_pControl->StopWhenReady();
        }
    }

    return hr;
}


/**
 *  Gets the duration of the current file.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::GetDuration(LONGLONG *pDuration)
{
    if (m_pSeek == NULL) {
        return E_UNEXPECTED;
    }

    return m_pSeek->GetDuration(pDuration);
}


/**
 *  Gets the current playback position.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::GetCurrentPosition(LONGLONG *pTimeNow)
{
    if (m_pSeek == NULL) {
        return E_UNEXPECTED;
    }

    return m_pSeek->GetCurrentPosition(pTimeNow);
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
BOOL DirectShowVideoPlayer::Finished()
{
    HRESULT hr = S_OK;

    LONGLONG current = 0;
    LONGLONG duration = 0;

    hr = GetCurrentPosition(&current);
    if (FAILED(hr)) {
        return false;
    }

    hr = GetDuration(&duration);
    if (FAILED(hr)) {
        return false;
    }

    return current >= duration;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
void DirectShowVideoPlayer::GetInfo()
{
}


/**
 *  Mutes or unmutes the audio.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT	DirectShowVideoPlayer::Mute(BOOL bMute)
{
    m_bMute = bMute;
    return UpdateVolume();
}


/**
 *  Sets the media volume.
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT	DirectShowVideoPlayer::SetVolume(float lVolume)
{
    m_lVolume = lVolume;
    return UpdateVolume();
}


/**
 *  Update the volume after a call to Mute() or SetVolume().
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::UpdateVolume()
{
    HRESULT hr = S_OK;

    if (m_bAudioStream && m_pAudio) {

        // If the audio is muted, set the minimum volume. 
        if (m_bMute) {
            hr = m_pAudio->put_Volume(MIN_VOLUME);

        } else {

            // Restore previous volume setting
            hr = m_pAudio->put_Volume(m_lVolume);
        }
    }

    return hr;
}


/**
 *  Create a new filter graph. (Tears down the old graph.) 
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::InitializeGraph()
{
    TearDownGraph();

    // QueryInterface for DirectShow interfaces

    // Create the Filter Graph Manager.
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, nullptr, 
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pGraph));
    if (FAILED(hr)) {
        return hr;
    }

    // Query for graph interfaces.

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pControl));
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pEvent));
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pSeek));
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pAudio));
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pVideo));
    if (FAILED(hr)) {
        return hr;
    }

    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pPosition));
    if (FAILED(hr)) {
        return hr;
    }

    /**
     *  Get the frame step interface, if supported.
     */
    IVideoFrameStep *pFSTest = nullptr;
    hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&pFSTest));
    if (FAILED(hr)) {
        return hr;
    }

    /**
     *  Check if this decoder can step.
     */
    hr = pFSTest->CanStep(false, nullptr);
    if (hr == S_OK) {
        m_pFrameStep = pFSTest;

    } else {
        pFSTest->Release();
    }








    // Set up event notification.
    // Have the graph signal event via window callbacks for performance
    hr = m_pEvent->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPH_EVENT, 0);
    //hr = m_pEvent->SetNotifyWindow((HWND)m_hwndEvent, m_EventMsg, nullptr);
    if (FAILED(hr)) {
        return hr;
    }

    m_state = STATE_STOPPED;

    return hr;
}


/**
 *  Tear down the filter graph and release resources. 
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
void DirectShowVideoPlayer::TearDownGraph()
{
    // Clear open dialog remnants before calling RenderFile()
    UpdateWindow(m_hWnd);

    // Stop sending event messages
    if (m_pEvent) {
        m_pEvent->SetNotifyWindow((OAHWND)nullptr, 0, 0);
    }

    SafeRelease(&m_pGraph);
    SafeRelease(&m_pControl);
    SafeRelease(&m_pEvent);
    SafeRelease(&m_pSeek);
    SafeRelease(&m_pAudio);
    SafeRelease(&m_pVideo);

    SAFE_DELETE(m_pVideoRenderer);

    m_state = STATE_NO_GRAPH;
    m_seekCaps = 0;

    m_bAudioStream = false;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::CreateVideoRenderer()
{
    HRESULT hr = E_FAIL;

    enum { Try_EVR, Try_VMR9, Try_VMR7 };

    for (DWORD i = Try_EVR; i <= Try_VMR7; i++) {

        switch (i) {
            case Try_EVR:
                DEBUG_INFO("DirectShow: Using EVR.\n");
                m_pVideoRenderer = new (std::nothrow) EVR();
                break;

            case Try_VMR9:
                DEBUG_INFO("DirectShow: Using VMR9.\n");
                m_pVideoRenderer = new (std::nothrow) VMR9();
                break;

            case Try_VMR7:
                DEBUG_INFO("DirectShow: Using VMR7.\n");
                m_pVideoRenderer = new (std::nothrow) VMR7();
                break;
        }

        if (m_pVideoRenderer == nullptr) {
            hr = E_OUTOFMEMORY;
            break;
        }

        hr = m_pVideoRenderer->AddToGraph(m_pGraph, m_hWnd);
        if (SUCCEEDED(hr)) {
            DEBUG_INFO("DirectShow: m_pVideoRenderer->AddToGraph passed.\n");
            break;
        }
        
        DEBUG_ERROR("DirectShow: AddToGraph failed!\n");

        SAFE_DELETE(m_pVideoRenderer);

    }

    return hr;
}


/**
 *  Render the streams from a source filter. 
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::RenderStreams(IBaseFilter *pSource)
{
    BOOL bRenderedAnyPin = FALSE;

    IFilterGraph2 *pGraph2 = nullptr;
    IEnumPins *pEnum = nullptr;
    IBaseFilter *pAudioRenderer = nullptr;

    HRESULT hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&pGraph2));
    if (FAILED(hr)) {
        goto done;
    }

    // Add the video renderer to the graph
    hr = CreateVideoRenderer();
    if (FAILED(hr)) {
        goto done;
    }

    // Add the DSound Renderer to the graph.
    hr = AddFilterByCLSID(m_pGraph, CLSID_DSoundRender, 
        &pAudioRenderer, L"Audio Renderer");
    if (FAILED(hr)) {
        goto done;
    }

    // Enumerate the pins on the source filter.
    hr = pSource->EnumPins(&pEnum);
    if (FAILED(hr)) {
        goto done;
    }

    // Loop through all the pins
    IPin *pPin;
    while (S_OK == pEnum->Next(1, &pPin, nullptr)) {

        // Try to render this pin. 
        // It's OK if we fail some pins, if at least one pin renders.
        HRESULT hr2 = pGraph2->RenderEx(pPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, nullptr);

        pPin->Release();
        if (SUCCEEDED(hr2)) {
            bRenderedAnyPin = TRUE;
        }
    }

    // Remove un-used renderers.

    // Try to remove the VMR.
    hr = m_pVideoRenderer->FinalizeGraph(m_pGraph);
    if (FAILED(hr)) {
        goto done;
    }

    // Try to remove the audio renderer.
    BOOL bRemoved = FALSE;
    hr = RemoveUnconnectedRenderer(m_pGraph, pAudioRenderer, &bRemoved);
    if (bRemoved) {
        m_bAudioStream = FALSE;
    } else {
        m_bAudioStream = TRUE;
    }

done:
    SafeRelease(&pEnum);
    //SafeRelease(pVMR);
    SafeRelease(&pAudioRenderer);
    SafeRelease(&m_pAudio);
    SafeRelease(&m_pVideo);
    SafeRelease(&pGraph2);

    // If we succeeded to this point, make sure we rendered at least one 
    // stream.
    if (SUCCEEDED(hr)) {
        if (!bRenderedAnyPin) {
            hr = VFW_E_CANNOT_RENDER;
        }
    }

    return hr;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::StepOneFrame()
{
    HRESULT hr = S_OK;

    // If the Frame Stepping interface exists, use it to step one frame
    if (m_pFrameStep) {

        // The graph must be paused for frame stepping to work
        if (m_state != STATE_PAUSED) {
            Pause();
        }

        // Step the requested number of frames, if supported
        hr = m_pFrameStep->Step(1, NULL);
    }

    return hr;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::StepFrames(int nFramesToStep)
{
    HRESULT hr = S_OK;

    /**
     *  If the Frame Stepping interface exists, use it to step frames.
     */
    if (m_pFrameStep) {

        /**
         *  The renderer may not support frame stepping for more than one
         *  frame at a time, so check for support.  S_OK indicates that the
         *  renderer can step nFramesToStep successfully.
         */
        if ((hr = m_pFrameStep->CanStep(nFramesToStep, NULL)) == S_OK) {

            /**
             *  The graph must be paused for frame stepping to work.
             */
            if (m_state != STATE_PAUSED) {
                Pause();
            }

            /**
             *  Step the requested number of frames, if supported.
             */
            hr = m_pFrameStep->Step(nFramesToStep, NULL);
        }
    }

    return hr;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::ModifyRate(double dRateAdjust)
{
    HRESULT hr = S_OK;
    double dRate;

    /**
     *  If the IMediaPosition interface exists, use it to set rate.
     */
    if ((m_pPosition) && (dRateAdjust != 0)) {
        if ((hr = m_pPosition->get_Rate(&dRate)) == S_OK) {

            /**
             *  Add current rate to adjustment value.
             */
            double dNewRate = dRate + dRateAdjust;
            hr = m_pPosition->put_Rate(dNewRate);

            /**
             *  Save new rate.
             */
            if (SUCCEEDED(hr)) {
                m_dPlaybackRate = dNewRate;
            }
        }
    }

    return hr;
}


/**
 *  
 * 
 *  @author: CCHyper (based on Windows SDK sample code)
 */
HRESULT DirectShowVideoPlayer::SetRate(double dRate)
{
    HRESULT hr = S_OK;

    /**
     *  If the IMediaPosition interface exists, use it to set rate.
     */
    if (m_pPosition) {
        hr = m_pPosition->put_Rate(dRate);

        /**
         *  Save new rate
         */
        if (SUCCEEDED(hr)) {
            m_dPlaybackRate = dRate;
        }
    }

    return hr;
}
