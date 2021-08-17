#include "dshowutil.h"


HRESULT RemoveUnconnectedRenderer(IGraphBuilder *pGraph, IBaseFilter *pRenderer, BOOL *pbRemoved)
{
    IPin *pPin = nullptr;

    *pbRemoved = FALSE;

    // Look for a connected input pin on the renderer.

    HRESULT hr = FindConnectedPin(pRenderer, PINDIR_INPUT, &pPin);
    SafeRelease(&pPin);

    // If this function succeeds, the renderer is connected, so we don't remove it.
    // If it fails, it means the renderer is not connected to anything, so
    // we remove it.

    if (FAILED(hr)) {
        hr = pGraph->RemoveFilter(pRenderer);
        *pbRemoved = TRUE;
    }

    return hr;
}


HRESULT IsPinConnected(IPin *pPin, BOOL *pResult)
{
    IPin *pTmp = nullptr;
    HRESULT hr = pPin->ConnectedTo(&pTmp);
    if (SUCCEEDED(hr)) {
        *pResult = TRUE;

    } else if (hr == VFW_E_NOT_CONNECTED) {
        // The pin is not connected. This is not an error for our purposes.
        *pResult = FALSE;
        hr = S_OK;
    }

    SafeRelease(&pTmp);
    return hr;
}


HRESULT IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pResult)
{
    PIN_DIRECTION pinDir;
    HRESULT hr = pPin->QueryDirection(&pinDir);
    if (SUCCEEDED(hr)) {
        *pResult = (pinDir == dir);
    }
    return hr;
}


HRESULT FindConnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, 
    IPin **ppPin)
{
    *ppPin = nullptr;

    IEnumPins *pEnum = nullptr;
    IPin *pPin = nullptr;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr)) {
        return hr;
    }

    BOOL bFound = FALSE;
    while (S_OK == pEnum->Next(1, &pPin, nullptr)) {

        BOOL bIsConnected;
        hr = IsPinConnected(pPin, &bIsConnected);
        if (SUCCEEDED(hr)) {
            if (bIsConnected) {
                hr = IsPinDirection(pPin, PinDir, &bFound);
            }
        }

        if (FAILED(hr)) {
            pPin->Release();
            break;
        }
        if (bFound) {
            *ppPin = pPin;
            break;
        }
        pPin->Release();
    }

    pEnum->Release();

    if (!bFound)
    {
        hr = VFW_E_NOT_FOUND;
    }
    return hr;
}


HRESULT AddFilterByCLSID(IGraphBuilder *pGraph, REFGUID clsid, 
    IBaseFilter **ppF, LPCWSTR wszName)
{
    *ppF = 0;

    IBaseFilter *pFilter = nullptr;
    
    HRESULT hr = CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, 
        IID_PPV_ARGS(&pFilter));
    if (FAILED(hr)) {
        goto done;
    }

    hr = pGraph->AddFilter(pFilter, wszName);
    if (FAILED(hr)) {
        goto done;
    }

    *ppF = pFilter;
    (*ppF)->AddRef();

done:
    SafeRelease(&pFilter);
    return hr;
}
