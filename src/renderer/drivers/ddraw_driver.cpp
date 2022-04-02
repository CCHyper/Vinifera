#include "ddraw_driver.h"
#include "vinifera_globals.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
//#include "tibsun_resource.h"
#include "optionsext.h"
#include "resource.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <Windows.h>
#include <commctrl.h>


extern HMODULE DLLInstance;


bool DirectDrawVideoDriver::Create_Window(Wstring window_title, int width, int height, int refresh_rate, bool windowed, bool borderless)
{
#if 0
    //DEV_DEBUG_INFO("Create_Window(enter)\n");

    HWND hWnd = nullptr;
    BOOL rc;
    WNDCLASSEX wc;
    tagRECT rect;
    HICON hIcon = nullptr;
    HICON hSmIcon = nullptr;
    HCURSOR hCursor = nullptr;

    //DEV_DEBUG_INFO("Create_Window() - About to call InitCommonControls()\n");

    InitCommonControls();

    //DEV_DEBUG_INFO("Create_Window() - Preparing window name (with version info).\n");

    DWORD dwPid = GetProcessId(GetCurrentProcess());
    if (!dwPid) {
        DEBUG_ERROR("Create_Window() - Failed to get the process id!\n");
        return;
    }

    //DEV_DEBUG_INFO("Create_Window() - Loading icon and cursor resources.\n");

    /**
     *  Load the Vinifera icon and cursor resources, falling back to the GAME.EXE
     *  resources if not available or failed to load.
     */
    if (Vinifera_IconName[0] != '\0') {
        DEBUG_INFO("Loading custom icon \"%s\"\n", Vinifera_IconName);
        hIcon = (HICON)LoadImage(
            nullptr,
            Vinifera_IconName,
            IMAGE_ICON,
            0,
            0,
            LR_LOADFROMFILE);
        DEBUG_INFO("Loading custom small icon \"%s\"\n", Vinifera_IconName);
        hSmIcon = (HICON)LoadImage(
            nullptr,
            Vinifera_IconName,
            IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON),
            GetSystemMetrics(SM_CXSMICON),
            LR_LOADFROMFILE);
    }
    if (!hIcon) {
        hIcon = LoadIcon((HINSTANCE)DLLInstance, MAKEINTRESOURCE(VINIFERA_MAINICON));
        if (!hIcon) {
            hIcon = LoadIcon((HINSTANCE)hInstance, MAKEINTRESOURCE(IDI_TIBSUN));
        }
    }
    if (Vinifera_CursorName[0] != '\0') {
        DEBUG_INFO("Loading custom cursor \"%s\"\n", Vinifera_CursorName);
        hCursor = LoadCursorFromFile(Vinifera_CursorName);
    }
    if (!hCursor) {
        hCursor = LoadCursor(nullptr, VINIFERA_MAINCURSOR); // IDC_ARROW is a system resource, does not require module.
        if (!hCursor) {
            hCursor = LoadCursor((HINSTANCE)hInstance, MAKEINTRESOURCE(IDI_TIBSUN_ARROW));
        }
    }

    //DEV_DEBUG_INFO("Create_Window() - Setting up window class info.\n");

    /**
     *  Register the window class.
     */
    wc.cbSize         = sizeof(WNDCLASSEX);
    wc.style          = CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc    = Main_Window_Procedure;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = (HINSTANCE)hInstance;
    wc.hIcon          = hIcon;
    wc.hCursor        = hCursor;
    wc.hbrBackground  = CreateSolidBrush(RGB(0,0,0));
    wc.lpszMenuName   = nullptr;
    wc.lpszClassName  = "Vinifera";
    wc.hIconSm        = (hSmIcon ? hSmIcon : hIcon);

    //DEV_DEBUG_INFO("Create_Window() - About to call RegisterClass()\n");

    /**
     *  Register window class.
     */
    rc = RegisterClassEx(&wc);
    if (!rc) {
        DEBUG_INFO("Create_Window() - Failed to register window class!\n");
        return false;
    }

    /**
     *  Get the dimensions of the primary display.
     */
    int display_width = GetSystemMetrics(SM_CXSCREEN);
    int display_height = GetSystemMetrics(SM_CYSCREEN);

    //DEV_DEBUG_INFO("Create_Window() - Desktop size %d x %d\n", display_width, display_height);

    /**
     *  Create our main window.
     */
    if (Debug_Windowed) {

        DEBUG_INFO("Create_Window() - Creating desktop window (%d x %d).\n", width, height);

        bool borderless = false;
        if (OptionsExtension) {
//            borderless = OptionsExtension->BorderlessWindow;
        }
        
        hWnd = CreateWindowEx(
            WS_EX_LEFT/*|WS_EX_TOPMOST*/,   // Removed: Causes focus issues when debugging with MSVC.
            "Vinifera",
            window_title.Peek_Buffer(),
            borderless ? WS_POPUP : WS_SYSMENU|WS_MINIMIZEBOX|WS_CLIPCHILDREN|WS_CAPTION,
            0, 0, 0, 0,
            nullptr,
            nullptr,
            (HINSTANCE)hInstance,
            nullptr);

        SetRect(&rect, 0, 0, width, height);

        AdjustWindowRectEx(&rect,
            GetWindowLong(hWnd, GWL_STYLE),
            GetMenu(hWnd) != nullptr,
            GetWindowLong(hWnd, GWL_EXSTYLE));

        /**
         *  #BUGFIX:
         * 
         *  Fetch the desktop size, calculate the screen center position the window and move it.
         */
        RECT workarea;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);

        int x_pos = (display_width - width) / 2;
        int y_pos = (((display_height - height) / 2) - (display_height - workarea.bottom));
        
        DEBUG_INFO("Create_Window() - Moving window (%d,%d,%d,%d).\n",
            x_pos, y_pos, (rect.right - rect.left), (rect.bottom - rect.top));

        MoveWindow(hWnd, x_pos, y_pos, (rect.right - rect.left), (rect.bottom - rect.top), TRUE);

    } else {

        DEBUG_INFO("Create_Window() - Creating fullscreen window.\n");

        hWnd = CreateWindowEx(
            WS_EX_LEFT|WS_EX_TOPMOST,
            "Vinifera",
            window_title.Peek_Buffer(),
            WS_POPUP|WS_CLIPCHILDREN,
            0, 0,
            display_width,
            display_height,
            nullptr,
            nullptr,
            (HINSTANCE)hInstance,
            nullptr);
    }

    if (!hWnd) {
        DEBUG_INFO("Create_Window() - Failed to create window!\n");
        return false;
    }

    //DEV_DEBUG_INFO("Create_Window() - About to call ShowWindow()\n");

    ShowWindow(hWnd, SW_SHOWNORMAL);
    ShowCommand = TRUE; // nCmdShow

    //DEV_DEBUG_INFO("Create_Window() - About to call UpdateWindow()\n");

    UpdateWindow(hWnd);

    //DEV_DEBUG_INFO("Create_Window() - About to call SetFocus()\n");

    SetFocus(hWnd);

    //DEV_DEBUG_INFO("Create_Window() - About to call RegisterHotKey()\n");

    RegisterHotKey(hWnd, 1, MOD_ALT|MOD_CONTROL|MOD_SHIFT, VK_M);

    //DEV_DEBUG_INFO("Create_Window() - About to call SetCursor()\n");

    SetCursor(hCursor);
#endif
    return true;
}


void DirectDrawVideoDriver::Close_Window(bool force)
{
}


void DirectDrawVideoDriver::Destroy_Window(bool force)
{
}


void DirectDrawVideoDriver::Show_Window()
{
}


void DirectDrawVideoDriver::Hide_Window()
{
}


void DirectDrawVideoDriver::Minimize_Window()
{
}


void DirectDrawVideoDriver::Maximize_Window()
{
}


bool DirectDrawVideoDriver::Toggle_Fullscreen()
{
    return true;
}
