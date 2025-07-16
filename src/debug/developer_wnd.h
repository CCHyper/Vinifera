/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_WND.H
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

#include <d3d11.h>

class DeveloperWindowClass
{
    public:
        DeveloperWindowClass();
        ~DeveloperWindowClass();

        bool Init();
        bool Shutdown();

        bool Show(bool and_create = false);
        bool Hide();

        ID3D11Device * Get_D3DDevice() const { return D3DDevice; }

    private:
        //
        // ImGui wrapper functions
        //
        void New_Frame();
        void Render_Frame();
        void End_Frame();

        //
        // D3D
        //
        bool Create_Render_Target();
        bool Create_Device_D3D(HWND hWnd);
        void Cleanup_Render_Target();
        void Cleanup_Device_D3D();

        bool Create_Window();
        void Window_Render_Loop();

    private:
        static LRESULT WINAPI Window_Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
        static DWORD WINAPI Window_Thread(LPVOID);

    private:
        ID3D11Device * D3DDevice;
        ID3D11DeviceContext * D3DDeviceContext;
        IDXGISwapChain * SwapChain;
        ID3D11RenderTargetView * MainRenderTargetView;
        volatile HWND WindowHandle;
        volatile bool IsWindowOpen;
};

