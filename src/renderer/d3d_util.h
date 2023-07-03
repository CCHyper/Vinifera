/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_UTIL.H
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
#pragma once

#include "always.h"
#include <d3d11.h>

class XSurface;


extern ID3D11Device* D3D11Device;
extern ID3D11DeviceContext* D3D11DeviceContext;
extern IDXGISwapChain* D3D11SwapChain;
extern ID3D11RenderTargetView* D3D11MainRenderTargetView;



void D3D11_CreateRenderTarget();
void D3D11_CleanupRenderTarget();
bool D3D11_CreateDeviceD3D(HWND hWnd);
void D3D11_CleanupDeviceD3D();

ID3D11ShaderResourceView* D3D11_CreateTextureFromSurface(XSurface& surface);
