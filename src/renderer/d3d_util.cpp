/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_ASSETBROWSER.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         
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
#include "d3d_util.h"
#include "dsurface.h"
#include "debughandler.h"
#include "d3d_imgui.h"
#include <imgui.h>
#include <d3d11.h>


ID3D11Device* D3D11Device = nullptr;
ID3D11DeviceContext* D3D11DeviceContext = nullptr;
IDXGISwapChain* D3D11SwapChain = nullptr;
ID3D11RenderTargetView* D3D11MainRenderTargetView = nullptr;


/**
 *  x
 *
 *  @author: CCHyper
 */
void D3D11_CreateRenderTarget()
{
    if (!D3D11Device) {
        return;
    }

    ID3D11Texture2D* pBackBuffer;
    D3D11SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    D3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &D3D11MainRenderTargetView);
    pBackBuffer->Release();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void D3D11_CleanupRenderTarget()
{
    if (D3D11MainRenderTargetView) {
        D3D11MainRenderTargetView->Release();
        D3D11MainRenderTargetView = nullptr;
    }
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool D3D11_CreateDeviceD3D(HWND hWnd)
{
    if (D3D11Device) {
        DEBUG_ERROR("D3DDevice already created!\n");
        return false;
    }

    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &D3D11SwapChain, &D3D11Device, &featureLevel, &D3D11DeviceContext) != S_OK) {
        return false;
    }

    D3D11_CreateRenderTarget();

    D3D11_ImGui_Initalise_D3D();

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void D3D11_CleanupDeviceD3D()
{
    D3D11_CleanupRenderTarget();

    if (D3D11SwapChain) {
        D3D11SwapChain->Release();
        D3D11SwapChain = nullptr;
    }
    if (D3D11DeviceContext) {
        D3D11DeviceContext->Release();
        D3D11DeviceContext = nullptr;
    }
    if (D3D11Device) {
        D3D11Device->Release();
        D3D11Device = nullptr;
    }
}


/**
 *  Simple helper function to load an surface buffer into a DX11 texture with common settings.
 * 
 *  https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
 */
ID3D11ShaderResourceView * D3D11_CreateTextureFromSurface(XSurface &surface)
{
    HRESULT res;
    ID3D11ShaderResourceView *d3d11surface = nullptr;
    ID3D11Texture2D *pTexture = nullptr;

    unsigned char *buffptr = (unsigned char *)surface.Lock();
    int buffwidth = surface.Get_Width();
    int buffheight = surface.Get_Height();

    //BSurface temp(surface.Get_Width(), surface.Get_Height(), surface.Get_Bytes_Per_Pixel(), surface.Lock());
    //unsigned char *buffptr = (unsigned char *)temp.Lock();

    // Create 2d texture of the surface buffer.
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    //desc.Width = temp.Get_Width(); //desc.Width = buffwidth;
    //desc.Height = temp.Get_Height(); //desc.Height = buffheight;
    desc.Width = desc.Width = buffwidth;
    desc.Height = desc.Height = buffheight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B5G6R5_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA subResource;
    ZeroMemory(&subResource, sizeof(subResource));
    subResource.pSysMem = buffptr;
    subResource.SysMemPitch = desc.Width * 2;
    subResource.SysMemSlicePitch = 0;
    res = D3D11Device->CreateTexture2D(&desc, &subResource, &pTexture);
    if (FAILED(res)) {
        return nullptr;
    }

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_B5G6R5_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    res = D3D11Device->CreateShaderResourceView(pTexture, &srvDesc, &d3d11surface);
    if (FAILED(res)) {
        return nullptr;
    }

    pTexture->Release();

    surface.Unlock();
    //temp.Unlock();

    return d3d11surface;
}
