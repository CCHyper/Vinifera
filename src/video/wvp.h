
#pragma once

#include <Windows.h>
#include <dshow.h>
#include <Evr.h>

#include "wstring.h"

#pragma comment(lib, "strmiids.lib")

enum class PlaybackState
{
    NoVideo,
    Playing,
    Paused,
    Stopped
};

class VideoRenderer 
{
    public:
        virtual ~VideoRenderer() {};
        virtual bool addToGraph(IGraphBuilder * graph, HWND hwnd) = 0;
        virtual bool finaliseGraph(IGraphBuilder * graph) = 0;
        virtual bool updateVideoWindow(HWND hwnd, const LPRECT prc) = 0;
        virtual bool hasVideo() const = 0;
        virtual bool repaint() = 0;
};

class Windows_EVR : public VideoRenderer
{
    public:
        Windows_EVR();
        ~Windows_EVR();

        bool addToGraph(IGraphBuilder * graph, HWND hwnd) override;
        bool finaliseGraph(IGraphBuilder * graph) override;
        bool updateVideoWindow(HWND hwnd, const LPRECT prc) override;
        bool hasVideo() const override;
        bool repaint() override;

    private:
        IMFVideoDisplayControl * videoDisplay;
        IBaseFilter * evr;
};

class WindowsVideoPlayer
{
    public:
        explicit WindowsVideoPlayer(HWND hwnd = nullptr);
        ~WindowsVideoPlayer();
    
        PlaybackState playbackState() const;
    
        bool openVideo(const Wstring & filename);
        bool updateVideoWindow() const;
        bool repaint() const;
        bool resume();
        bool pause();
        bool play();
        bool stop();
    
        bool isPaused() const;
        bool hasFinished() const;
        bool hasVideo() const;

    private:    
        struct RenderStreamsParams {
            IFilterGraph2 * filterGraph2;
            IBaseFilter * audioRenderer;
            IBaseFilter * source;
            IEnumPins * pins;
            HRESULT& hr;
            bool& renderedAnyPin;
        };
    
        bool setupGraph();
        bool createVideoRenderer() const;
    
        HRESULT queryInterface(HRESULT prevResult, const IID& riid, void ** pvObject) const;
    
        bool renderStreams(RenderStreamsParams * params) const;
        bool renderStreams(IBaseFilter * source);
    
        void releaseGraph();

    private:
        IGraphBuilder * graphBuilder;
        IMediaControl * mediaControl;
        IMediaEventEx * mediaEvents;
        IMediaSeeking * mediaSeeking;
        VideoRenderer * videoRenderer;
        PlaybackState state;
        HWND windowHandle;
        Wstring Filename;
};
