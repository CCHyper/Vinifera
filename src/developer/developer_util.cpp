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
#include "developer_util.h"
#include "dsurface.h"

#include <imgui.h>
#include <d3d11.h>


extern ID3D11Device *g_pd3dDevice;

// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples


/**
 *  Simple helper function to load an surface buffer into a DX11 texture with common settings.
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
    res = g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
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
    res = g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &d3d11surface);
    if (FAILED(res)) {
        return nullptr;
    }

    pTexture->Release();

    surface.Unlock();
    //temp.Unlock();

    return d3d11surface;
}
