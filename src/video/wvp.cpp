
#include <functional>
#include "wvp.h"

#include "tibsun_globals.h"
#include "ccfile.h"
#include "wwkeyboard.h"

using bool_lambda = std::function<bool()>;
using void_lambda = std::function<void()>;

const auto EmptyFunction {void_lambda([](){})};

template<typename T> 
void safeRelease(T ** comPtr) 
{
    if (comPtr != nullptr && *comPtr) 
    {
        (*comPtr)->Release();
        (*comPtr) = nullptr;
    }
}

template<typename T> 
void safeDelete(T ** savePointer) 
{
    if(savePointer != nullptr && savePointer) 
    {
        delete (*savePointer);
        (*savePointer) = nullptr;
    }
}

bool async(bool_lambda start, void_lambda onfail = EmptyFunction, void_lambda cleanup = EmptyFunction)
{
    auto successful {start()};

    if(!successful) onfail();
    if(cleanup) cleanup();
   
    return successful;
}

bool async_reverse(void_lambda cleanup, void_lambda onfail, bool_lambda start)
{
    return async(start, onfail, cleanup);
}

bool isPinConnected(IPin * pinPointer, bool * resultPointer)
{
    IPin * tempPinPointer { nullptr };
    auto hr { pinPointer->ConnectedTo(&tempPinPointer) };
    
    if (SUCCEEDED(hr))  
    {
        *resultPointer = true;
    } 
    else if (hr == VFW_E_NOT_CONNECTED) 
    {
        *resultPointer = true;
        hr = S_OK;
    }

    safeRelease(&tempPinPointer);
    return SUCCEEDED(hr);
}

bool isPinDirection(IPin * pinPointer, PIN_DIRECTION dir, BOOL * result)
{
    PIN_DIRECTION pinDirection;
    auto hr { pinPointer->QueryDirection(&pinDirection) };

    if (SUCCEEDED(hr))
    {
        *result = pinDirection == dir;
    }

    return SUCCEEDED(hr);
}

bool findConnectedPin(IBaseFilter * filter, PIN_DIRECTION PinDir, IPin **ppPin)
{
    IEnumPins * enumPins { nullptr };
    IPin * pinPtr { nullptr };

    auto bFound{ FALSE };
    auto hr { filter->EnumPins(&enumPins) };

    *ppPin = nullptr;

    if (FAILED(hr))
    {
        return SUCCEEDED(hr);
    }

    while (S_OK == enumPins->Next(1, &pinPtr, nullptr))
    {
        bool isConnected;
        hr = isPinConnected(pinPtr, &isConnected);

        if (SUCCEEDED(hr)) 
        {
            if (isConnected) 
            {
                hr = isPinDirection(pinPtr, PinDir, &bFound);
            }
        }

        if (FAILED(hr)) 
        {
            pinPtr->Release();
            break;
        }

        if (bFound) 
        {
            *ppPin = pinPtr;
            break;
        }

        pinPtr->Release();
    }

    enumPins->Release();

    if (!bFound) 
    {
        hr = VFW_E_NOT_FOUND;
    }

    return SUCCEEDED(hr);
}

bool removeUnconnectedRenderer(IGraphBuilder * graphBuilder, IBaseFilter * baseFilter, bool * removed)
{
    IPin * pinPointer{ nullptr };
    auto result { findConnectedPin(baseFilter, PINDIR_INPUT, &pinPointer) };

    if (FAILED(result))
    {
        result = SUCCEEDED(graphBuilder->RemoveFilter(baseFilter));
        *removed = true;
    }
    else
    {
        *removed = false;
    }

    safeRelease(&pinPointer);
    return result;
}

bool addFilterByCLSID(IGraphBuilder *pGraph, REFGUID clsid, IBaseFilter ** baseFilter, LPCWSTR wszName)
{
    *baseFilter = nullptr;
    IBaseFilter * filter {nullptr};
    auto hr { CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&filter)) };

    const auto task = [&]() {
        if (FAILED(hr))
            return false;

        hr = pGraph->AddFilter(filter, wszName);

        if (FAILED(hr))
            return false;

        *baseFilter = filter;
        (*baseFilter)->AddRef();
        return true;
    };

    return async(task, EmptyFunction, [&]() { safeRelease(&filter); });
}

bool removeUnconnectedRenderer(IGraphBuilder * graph, IBaseFilter * renderer)
{
    IPin * pinPointer {nullptr};
    auto hr { findConnectedPin(renderer, PINDIR_INPUT, &pinPointer) };
    pinPointer->Release();
    return SUCCEEDED(!hr ? graph->RemoveFilter(renderer): hr);
}

bool initialiseEvr(IBaseFilter * baseFilter, HWND hwnd, IMFVideoDisplayControl** displayControl)
{
    IMFVideoDisplayControl *display{ nullptr };
    IMFGetService *getService {nullptr };

    auto hr = baseFilter->QueryInterface(IID_PPV_ARGS(&getService));

    const auto cleanup = [&]() {
        safeRelease(&getService);
        safeRelease(&display);
    };

    return async_reverse(cleanup, EmptyFunction, [&]() {
        if (FAILED(hr))
            return false;

        hr = getService->GetService(MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&display));

        if (FAILED(hr))
            return false;

        hr = display->SetVideoWindow(hwnd);

        if (FAILED(hr))
            return false;

        hr = display->SetAspectRatioMode(MFVideoARMode_None);

        if (FAILED(hr))
            return false;

        *displayControl = display;
        (*displayControl)->AddRef();
        return true;
    });
}

Windows_EVR::Windows_EVR() 
    : videoDisplay(nullptr), evr(nullptr)
{
}

Windows_EVR::~Windows_EVR()
{
    safeRelease(&evr);
    safeRelease(&videoDisplay);
}

bool Windows_EVR::hasVideo() const
{
    return videoDisplay != nullptr;
}

bool Windows_EVR::addToGraph(IGraphBuilder * graph, HWND hwnd)
{
    IBaseFilter *evrFilter { nullptr };
    auto hr { addFilterByCLSID(graph, CLSID_EnhancedVideoRenderer, &evrFilter, L"EVR") };
  
    const auto task = [&]() {
        if (FAILED(hr))
            return false;

        initialiseEvr(evrFilter, hwnd, &videoDisplay);

        if (FAILED(hr))
            return false;

        this->evr = evrFilter;
        this->evr->AddRef();

        return true;
    };

    return async(task, [&](){ safeRelease(&evrFilter); });
}

bool Windows_EVR::finaliseGraph(IGraphBuilder * graph)
{
    if (evr == nullptr) 
    {
        return true;
    }

    auto removed { false };
    auto hr { removeUnconnectedRenderer(graph, evr, &removed) };

    if (removed) 
    {
        safeRelease(&evr);
        safeRelease(&videoDisplay);
    }

    return(SUCCEEDED(hr));
}

bool Windows_EVR::updateVideoWindow(HWND hwnd, const LPRECT prc)
{
    if (videoDisplay == nullptr) 
    {
        return true; 
    }

    if (prc) 
    {
        return SUCCEEDED(videoDisplay->SetVideoPosition(nullptr, prc));
    }

    RECT rc;
    GetClientRect(hwnd, &rc);
    return SUCCEEDED(videoDisplay->SetVideoPosition(nullptr, &rc));
}

bool Windows_EVR::repaint()
{
    return SUCCEEDED(videoDisplay ? videoDisplay->RepaintVideo() : S_OK);
}

WindowsVideoPlayer::WindowsVideoPlayer(HWND hwnd)
  : graphBuilder(nullptr),
    mediaControl(nullptr),
    mediaEvents(nullptr),
    mediaSeeking(nullptr),
    videoRenderer(new Windows_EVR()),
    state(PlaybackState::NoVideo),
    windowHandle(hwnd)
{
}

WindowsVideoPlayer::~WindowsVideoPlayer()
{
    safeDelete(&videoRenderer);
    releaseGraph();
}

bool WindowsVideoPlayer::openVideo(const Wstring & filename)
{
    if(!CCFileClass(filename.Peek_Buffer()).Is_Available()) 
    {
        return false;
    }

    IBaseFilter* source {nullptr};
    auto hr { setupGraph() };

    WCHAR wFileName[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, filename.Peek_Buffer(), -1, wFileName, MAX_PATH);

    const auto tasks = [&]() {
        hr = SUCCEEDED(graphBuilder->AddSourceFilter(wFileName, nullptr, &source));
        return !hr ? false : renderStreams(source);
    };

    return async(tasks, [&]() { releaseGraph(); }, [&]() { safeRelease(&source); });
}

bool WindowsVideoPlayer::play()
{
    if (state != PlaybackState::Paused && state != PlaybackState::Stopped)
    {
        return false;
    }

    updateVideoWindow();

    auto hr { mediaControl->Run() };

    if (SUCCEEDED(hr))
    {
        state = PlaybackState::Playing;
    }

    if (state != PlaybackState::Playing) {
        return false;
    }

    bool exit = false;
    while (!exit) {

        if (WWKeyboard->Check()) {
            switch (WWKeyboard->Get()) {
                case (KN_RLSE_BIT|KN_SPACE):
                {
                    if (isPaused()) {
                        resume();
                    } else {
                        pause();
                    }
                    break;
                }
                case (KN_RLSE_BIT|KN_ESC):
                    stop();
                    exit = true;
                    break;
            };
        }

        updateVideoWindow();
        repaint();

        if (hasFinished()) {
            exit = true;
        }

    }

    return true;
}

bool WindowsVideoPlayer::resume()
{
    if (state != PlaybackState::Paused)
    {
        return false;
    }

    auto hr { mediaControl->Run() };
    
    if (SUCCEEDED(hr)) 
    {
        state = PlaybackState::Playing;
    }

    return state == PlaybackState::Playing;
}

bool WindowsVideoPlayer::pause()
{
    if (state != PlaybackState::Playing) 
    {
        return false;
    }

    auto hr { mediaControl->Pause() };
    
    if (SUCCEEDED(hr)) 
    {
        state = PlaybackState::Paused;
    }

    return state == PlaybackState::Paused;
}

bool WindowsVideoPlayer::stop()
{
    if (state != PlaybackState::Playing && state != PlaybackState::Paused) 
    {
        return false;
    }

    auto hr { mediaControl->Stop() };

    if (SUCCEEDED(hr))
    {
        state = PlaybackState::Stopped;
        
        auto stopTimes = 1LL;
        auto start = 1LL;

        mediaSeeking->GetPositions(nullptr, &stopTimes);
        mediaSeeking->SetPositions(&start, 0x01 | 0x04, &stopTimes, 0x01 | 0x04);
    }
    
    return state == PlaybackState::Stopped;
}

bool WindowsVideoPlayer::hasVideo() const
{
    return videoRenderer && videoRenderer->hasVideo();
}

bool WindowsVideoPlayer::isPaused() const
{
    return state == PlaybackState::Paused;
}

bool WindowsVideoPlayer::hasFinished() const
{
    auto returnValue {false};

    if(mediaEvents == nullptr) 
    {
        return returnValue;
    }

    auto param1 {0L};
    auto param2 {0L};
    auto evCode {0L};

    while (SUCCEEDED(mediaEvents->GetEvent(&evCode, &param1, &param2, 0)))
    {
        auto hr = mediaEvents->FreeEventParams(evCode, param1, param2);
        
        if (FAILED(hr))
        {
            break;
        }

        if (evCode == EC_COMPLETE) 
        {
            returnValue = true;
            break;
        }
    }

    return returnValue;
}

bool WindowsVideoPlayer::updateVideoWindow() const
{
    RECT rc;
    GetClientRect(windowHandle, &rc);
    return SUCCEEDED(videoRenderer ? videoRenderer->updateVideoWindow(windowHandle, &rc) : S_OK);
}

bool WindowsVideoPlayer::repaint() const
{
    return SUCCEEDED(videoRenderer ? videoRenderer->repaint() : S_OK);
}

HRESULT WindowsVideoPlayer::queryInterface(HRESULT prevResult, const IID& riid, void ** pvObject) const
{
    return SUCCEEDED(prevResult) ? graphBuilder->QueryInterface(riid, pvObject) : E_FAIL;
}

bool WindowsVideoPlayer::setupGraph()
{
    releaseGraph();

    auto hr { CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&graphBuilder)) };

    if (FAILED(hr))
    {
        return SUCCEEDED(hr);
    }

    hr = queryInterface(hr, IID_PPV_ARGS(&mediaControl));
    hr = queryInterface(hr, IID_PPV_ARGS(&mediaEvents));
    hr = queryInterface(hr, IID_PPV_ARGS(&mediaSeeking));
    
    if (SUCCEEDED(hr))
    {
        state = PlaybackState::Stopped;
    }

    return SUCCEEDED(hr);
}

void WindowsVideoPlayer::releaseGraph()
{
    state = PlaybackState::NoVideo;

    safeRelease(&graphBuilder);
    safeRelease(&mediaControl);
    safeRelease(&mediaSeeking);
    safeRelease(&mediaEvents);
}

PlaybackState WindowsVideoPlayer::playbackState() const 
{
    return state;
}

bool WindowsVideoPlayer::createVideoRenderer() const
{
    auto hr { E_FAIL };

    if (videoRenderer == nullptr) 
    {
        hr = E_OUTOFMEMORY;
        return false;
    }

    return SUCCEEDED(videoRenderer->addToGraph(graphBuilder, windowHandle));
}

bool WindowsVideoPlayer::renderStreams(RenderStreamsParams * params) const
{
    if (FAILED(params->hr))
    {
        return false;
    }

    params->hr = createVideoRenderer();

    if (FAILED(params->hr))
    {
        return false;
    }

    params->hr = addFilterByCLSID(graphBuilder, CLSID_DSoundRender, &params->audioRenderer, L"Audio Renderer");

    if (FAILED(params->hr))
    {
        return false;
    }

    params->hr = params->source->EnumPins(&params->pins);   

    if (FAILED(params->hr))
    {
        return false;
    }

    IPin * pin { nullptr };

    while (S_OK == params->pins->Next(1, &pin, nullptr))
    {
        auto rendered = params->filterGraph2->RenderEx(pin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, nullptr);
       
        if (SUCCEEDED(rendered))
        {
            params->renderedAnyPin = TRUE;
        }

        pin->Release();
    }

    params->hr = videoRenderer->finaliseGraph(graphBuilder);

    if (FAILED(params->hr)) 
    {
        return false;
    }

    bool removed;
    params->hr = removeUnconnectedRenderer(graphBuilder, params->audioRenderer, &removed);
    return SUCCEEDED(params->hr);
}

bool WindowsVideoPlayer::renderStreams(IBaseFilter * source)
{
    IFilterGraph2 * filterGraph2 { nullptr };
    IBaseFilter * audioRenderer { nullptr };
    IEnumPins * enumPins { nullptr };
  
    auto renderedAnyPin { false };
    auto hr { graphBuilder->QueryInterface(IID_PPV_ARGS(&filterGraph2)) };

    auto cleanup = [&]() {
        safeRelease(&enumPins);
        safeRelease(&audioRenderer);
        safeRelease(&filterGraph2);
    };

    auto task = [&]() {
        RenderStreamsParams params = {
            filterGraph2, audioRenderer, source, 
            enumPins, hr, renderedAnyPin
        };

        return renderStreams(&params);
    };

    return async(task, cleanup);
}
