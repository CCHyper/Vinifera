/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OWNERDRAW_WINDOW.CPP
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
#include "ownerdraw_window.h"


/**
 *  Creates a hidden window for OwnerDraw.
 * 
 *  Credit goes to cfehunter for the solution!
 */


HWND OwnerDrawWindow = nullptr;


static LRESULT CALLBACK OwnerDraw_Main_Window_Procedure(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, Message, wParam, lParam);
}

bool OwnerDraw_Create_Window(HINSTANCE hInstance, int width, int height)
{
    BOOL rc;
    WNDCLASSEX wc;
    tagRECT rect;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = OwnerDraw_Main_Window_Procedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = (HINSTANCE)hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "Hidden OwnerDraw Window";
    wc.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

    rc = RegisterClassEx(&wc);
    if (!rc) {
        return false;
    }

    HWND hWnd = CreateWindowEx(
        WS_EX_LEFT,
        "Hidden OwnerDraw Window",
        nullptr,
        WS_OVERLAPPED,
        0, 0, 0, 0,
        HWND_MESSAGE,
        nullptr,
        (HINSTANCE)hInstance,
        nullptr);

    if (!hWnd) {
        return false;
    }

    SetRect(&rect, 0, 0, width, height);

    AdjustWindowRectEx(&rect,
        GetWindowLong(hWnd, GWL_STYLE),
        GetMenu(hWnd) != nullptr,
        GetWindowLong(hWnd, GWL_EXSTYLE));

    OwnerDrawWindow = hWnd;

    ShowWindow(hWnd, SW_SHOWNORMAL);

    UpdateWindow(hWnd);

    return true;
}

HDC OwnerDraw_Get_DC()
{
    return GetDC(OwnerDrawWindow);
}

void OwnerDraw_Release_DC(HDC hDC)
{
    ReleaseDC(OwnerDrawWindow, hDC);
}

#if 0
HBITMAP OwnerDraw_Get_Bitmap(int width, int height)
{
    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(BITMAPINFO));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 16;

    HDC hDC = GetDC(OwnerDrawWindow);

    char *buffer;
    HBITMAP bitmap = CreateDIBSection(hDC, &bi, DIB_RGB_COLORS, &buffer, NULL, 0);

    // copy pixels 
    SelectObject(hDC, bitmap);

    BitBlt();
}
#endif

//
// TODO, do I need to create the window with a DSurface?
//
