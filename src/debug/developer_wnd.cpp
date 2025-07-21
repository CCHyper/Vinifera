/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_WND.CPP
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

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <tchar.h>
#include <chrono>
#include <thread>

#include <shellscalingapi.h>

#include "developer_wnd.h"
#include "developer_wnd_helper.h"
#include "debughandler.h"
#include "asserthandler.h"
#include "vinifera_globals.h"

/**
 *  Game classes
 */
#include "tibsun_globals.h"

#include "options.h"
#include "scenario.h"
#include "special.h"
#include "rules.h"
#include "unit.h"
#include "unittype.h"
#include "building.h"
#include "buildingtype.h"
#include "infantry.h"
#include "infantrytype.h"
#include "aircraft.h"
#include "aircrafttype.h"




extern void __cdecl Developer_Window_Helper_At_Exit();


ID3D11Device * _D3DDevice = nullptr;
float _DrawScale = 1.0f;





//
// !! IMPORTANT !!
// 
// The creation and setup of the D3D context can not be called from the DllMain, it must be
// done by hooking into one of the games init processes or via static init from a global.


/**
 *  Forward declare message handler from imgui_impl_win32.cpp
 */
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/**
 * 
 *  Utility functions
 * 
 */

/**
 *  Get the DPI scale of the monitor that the requested window is currently on.
 *
 *  @author: 273K @ https://stackoverflow.com/a/70794377
 */
static float Get_Monitor_DPI_Scale(HWND hWnd)
{
    float scMon = 1.0f;
    UINT x, y;

    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &x, &y)) && (x > 0) && (y > 0)) {
        scMon = 1.0f * x / USER_DEFAULT_SCREEN_DPI;           // 1.25
        // scMon = MulDiv(100, x, USER_DEFAULT_SCREEN_DPI);  // 125
    }

    return scMon;
}

static RECT Get_Monitor_Rect(HWND hWnd)
{
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &mi);
    return mi.rcWork;
}

static bool Is_Window_Fullscreen(HWND hWnd)
{
    if (!IsWindow(hWnd)) {
        return false;
    }

    // Get window rectangle in screen coordinates
    RECT windowRect;
    GetWindowRect(hWnd, &windowRect);

    // Get the monitor it's on
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(monitor, &mi)) {
        return false;
    }

    RECT monitorRect = mi.rcMonitor;

    // Check if window covers the entire monitor
    bool sameSize = EqualRect(&windowRect, &monitorRect);

    // Check window styles for decorations
    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    bool hasDecorations = style & (WS_CAPTION | WS_THICKFRAME | WS_BORDER);

    return sameSize && !hasDecorations;
}




static void __cdecl Developer_Window_At_Exit()
{
    Developer_Window_Helper_At_Exit();
}



/**
 * 
 *  Rendering/Logic thread
 * 
 */
static volatile bool ThreadActive = false;
static volatile bool ThreadRunning = false;
static volatile bool ThreadInLoop = false;

DWORD WINAPI DeveloperWindowClass::Window_Thread(LPVOID param)
{
    DEBUG_INFO("DevWnd - Entering thread.\n");

    DeveloperWindowClass * _this = static_cast<DeveloperWindowClass*>(param);

    ThreadRunning = true;

    while (ThreadActive) {

        ThreadInLoop = true;

        /**
         *  All rendering is performed here!
         */
        _this->Window_Render_Loop();

        std::this_thread::yield();

        ThreadInLoop = false;
    }

    ThreadRunning = false;

    DEBUG_INFO("DevWnd - Exiting thread.\n");

    return 0;
}

/**
 * 
 *  Win32 message handler
 *  You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
 *  - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
 *  - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
 *  Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
 * 
 */
LRESULT WINAPI DeveloperWindowClass::Window_Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    // Fetch pointer from GWLP_USERDATA (set during WM_NCCREATE)
    DeveloperWindowClass * _this = reinterpret_cast<DeveloperWindowClass *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if (Message == WM_NCCREATE)
    {
        // First message: store pointer
        CREATESTRUCT * cs = reinterpret_cast<CREATESTRUCT *>(lParam);
        _this = reinterpret_cast<DeveloperWindowClass *>(cs->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)_this);
    }

    // Let ImGui have the first look at messages...
    if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam)) {
        return true;
    }

    switch (Message) {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH)); // Fill with black
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_SIZE:
            if (_this->Get_D3DDevice() != nullptr && wParam != SIZE_MINIMIZED) {
                _this->Cleanup_Render_Target();
                _this->SwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                _this->Create_Render_Target();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) { // Disable ALT application menu
                return 0;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, Message, wParam, lParam);
}


/**
 * 
 *  DeveloperWindowClass implementation
 * 
 */

DeveloperWindowClass::DeveloperWindowClass() :
    D3DDevice(nullptr),
    D3DDeviceContext(nullptr),
    SwapChain(nullptr),
    MainRenderTargetView(nullptr),
    WindowHandle(nullptr),
    IsWindowOpen(false)
{
    ThreadActive = false;
    ThreadRunning = false;
    ThreadInLoop = false;
}

DeveloperWindowClass::~DeveloperWindowClass()
{
}

bool DeveloperWindowClass::Init()
{
    Create_Window();

    ThreadActive = true;

    // We pass "this" so the thread has access to our instance.
    CreateThread(nullptr, 0, Window_Thread, this, 0, nullptr);

    return true;
}

bool DeveloperWindowClass::Shutdown()
{
    ThreadActive = false;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    Cleanup_Device_D3D();

    DestroyWindow(WindowHandle);
    //UnregisterClass(wc.lpszClassName, wc.hInstance);

    return true;
}

bool DeveloperWindowClass::Show(bool and_create)
{
    // We can only show this window when the user is running the game in "windowed mode".
    if (Is_Window_Fullscreen(MainWindow)) {
        return false;
    }

    if (and_create) {
        //Destroy_Window();
        Create_Window();
    }

    ShowWindow(WindowHandle, SW_RESTORE);
    IsWindowOpen = true;

    return true;
}

bool DeveloperWindowClass::Hide()
{
    ShowWindow(WindowHandle, SW_HIDE);
    IsWindowOpen = false;

    return true;
}

void DeveloperWindowClass::New_Frame()
{
    if (!D3DDeviceContext || !SwapChain) {
        return;
    }

    // Start the ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();
}

void DeveloperWindowClass::Render_Frame()
{
    if (!D3DDeviceContext || !SwapChain) {
        return;
    }

    // Rendering
    ImGui::Render();

    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    D3DDeviceContext->OMSetRenderTargets(1, &MainRenderTargetView, nullptr);
    D3DDeviceContext->ClearRenderTargetView(MainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    SwapChain->Present(1, 0); // Present with vsync
    //SwapChain->Present(0, 0); // Present without vsync
}

void DeveloperWindowClass::End_Frame()
{
    // Update and Render additional Platform Windows
    //{
    //    ImGuiIO& io = ImGui::GetIO();
    //    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //        ImGui::UpdatePlatformWindows();
    //        ImGui::RenderPlatformWindowsDefault();
    //    }
    //}

    ImGui::EndFrame();
}

bool DeveloperWindowClass::Create_Render_Target()
{
    if (!D3DDevice) {
        return false;
    }

    ID3D11Texture2D* pBackBuffer;
    SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

    if (pBackBuffer == nullptr) {
        return false;
    }

    D3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &MainRenderTargetView);
    pBackBuffer->Release();

    return true;
}

// This must take hWnd rather than use WindowHandle as we use WindowHandle as a indicator that the window is fully initialized.
bool DeveloperWindowClass::Create_Device_D3D(HWND hWnd)
{
    // If the device is already created, bail.
    if (D3DDevice) {
        return false;
    }

    ASSERT(hWnd && IsWindow(hWnd));

    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;

    RECT rc;
    GetClientRect(hWnd, &rc);
    sd.BufferDesc.Width  = rc.right  - rc.left; //sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = rc.bottom - rc.top; //sd.BufferDesc.Height = 0;

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
#ifndef NDEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &SwapChain, &D3DDevice, &featureLevel, &D3DDeviceContext);
    if (FAILED(hr)) {
        _com_error err(hr);
        DEBUG_ERROR(err.ErrorMessage()); // See the exact DXGI error
        return false;
    }

    Create_Render_Target();

    _D3DDevice = D3DDevice;

    return true;
}

void DeveloperWindowClass::Cleanup_Render_Target()
{
    if (MainRenderTargetView) {
        MainRenderTargetView->Release();
        MainRenderTargetView = nullptr;
    }
}

void DeveloperWindowClass::Cleanup_Device_D3D()
{
    Cleanup_Render_Target();

    if (SwapChain) {
        SwapChain->Release();
        SwapChain = nullptr;
    }
    if (D3DDeviceContext) {
        D3DDeviceContext->Release();
        D3DDeviceContext = nullptr;
    }
    if (D3DDevice) {
        D3DDevice->Release();
        D3DDevice = nullptr;
    }

    // Cleanup.
    _D3DDevice = nullptr;
}

bool DeveloperWindowClass::Create_Window()
{
    SetLastError(0);

    CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

    DEBUG_INFO("DevWnd - Creating window.\n");

    ImGui_ImplWin32_EnableDpiAwareness();

    const char *window_name = "Developer Window";

    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = DeveloperWindowClass::Window_Proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = ProgramInstance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = window_name;
    wc.hIconSm = nullptr;

    BOOL rc = RegisterClassEx(&wc);
    if (!rc) {
        DEBUG_ERROR("DevWnd - Failed to register window class!\n");
        return false;
    }

    int win_width = 1024;
    int win_height = 768;

    DEBUG_INFO("DevWnd - Setting window size.\n");

    HWND hWnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        window_name,
        WS_OVERLAPPEDWINDOW|WS_VISIBLE|WS_POPUP,
        0,
        0,
        win_width,
        win_height,
        nullptr,
        nullptr,
        wc.hInstance,
        this); // We pass "this" so the window proc has access to this class instance.

    if (!hWnd) {
        DEBUG_ERROR("DevWnd - Failed to create window!\n");
        return false;
    }

    // Resposition and resize the window based on the monitor scale.
    _DrawScale = Get_Monitor_DPI_Scale(hWnd);

    float monitor_scale = _DrawScale;
    RECT monitor_rect = Get_Monitor_Rect(hWnd);

    int screenWidth  = monitor_rect.right - monitor_rect.left;
    int screenHeight = monitor_rect.bottom - monitor_rect.top;

    int winX = monitor_rect.left + screenWidth - (win_width * monitor_scale);
    int winY = monitor_rect.top; // Top of the monitor

    SetWindowPos(hWnd,
        nullptr,
        winX,
        winY,
        win_width * monitor_scale,
        win_height * monitor_scale,
        SWP_NOOWNERZORDER | SWP_NOZORDER);

    InvalidateRect(hWnd, nullptr, TRUE); // Mark entire window as dirty
    UpdateWindow(hWnd); // Force immediate WM_PAINT

    DEBUG_INFO("DevWnd - Creating Direct3D device.\n");

    // Initialize Direct3D
    if (!Create_Device_D3D(hWnd)) {
        DEBUG_ERROR("DevWnd - Failed to create Direct3D device!\n");
        Cleanup_Device_D3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return false;
    }

    // Show the window
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    // Update and scale the UI.
    style.ScaleAllSizes(_DrawScale);
    io.FontGlobalScale = _DrawScale; // Scales all text globally

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // The game hides the system cursor, this makes it show only in the ImGui window.
    io.MouseDrawCursor = true;

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    DEBUG_INFO("DevWnd - Setting up platform and renderer.\n");

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(D3DDevice, D3DDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->

    DEBUG_INFO("DevWnd: Window created.\n");

    // Store the window handle.
    WindowHandle = hWnd;

    // 
    std::atexit(Developer_Window_At_Exit);

    return true;
}








/**
 *  Various window and element states
 */
static int SelectedIndex = -1;
static int CurrentTab = 2;      // Start on "Rules" tab where the editor is

#include "filepng.h"
void DeveloperWindowClass::Window_Render_Loop()
{
    if (!WindowHandle) {
        return;
    }
    // If the window is not visible, then there is no need for us to render.
    if (!IsWindowOpen) {
        return;
    }

    New_Frame();

    // Top toolbar (simple horizontal layout)
    ImGui::Begin("Toolbar", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize   |
        ImGuiWindowFlags_NoMove     |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_AlwaysAutoResize
    );
    ImGui::SetWindowPos(ImVec2(0, 0));  // top-left corner

    static bool RulesOpen = false;
    static bool ScenarioOpen = false;
    static bool SpecialOpen = false;
    static bool GlobalsOpen = false;
    static bool TypesOpen = false;

    static bool ShowRulesEditor = false;
    static bool ShowScenarioEditor = false;
    static bool ShowSpecialEditor = false;
    static bool ShowGlobalsEditor = false;
    static bool ShowTypesEditor = false;

    // Add clickable buttons directly
    if (ImGui::Button("Rules")) {
        ShowRulesEditor = !ShowRulesEditor;
    }
    ImGui::SameLine();
    if (ImGui::Button("Scenario")) {
        ShowScenarioEditor = !ShowScenarioEditor;
    }
    ImGui::SameLine();
    if (ImGui::Button("Special")) {
        ShowSpecialEditor = !ShowSpecialEditor;
    }
    ImGui::SameLine();
    if (ImGui::Button("Globals")) {
        ShowGlobalsEditor = !ShowGlobalsEditor;
    }
    ImGui::SameLine();
    if (ImGui::Button("Types")) {
        ShowTypesEditor = !ShowTypesEditor;
    }

    ImGui::End();

    if (ShowRulesEditor) {
        CCImGui::DrawRulesEditor(&RulesOpen);
    }
    if (ShowScenarioEditor) {
        CCImGui::DrawScenarioEditor(&ScenarioOpen);
    }
    if (ShowSpecialEditor) {
        CCImGui::DrawSpecialEditor(&ScenarioOpen);
    }
    if (ShowGlobalsEditor) {
        CCImGui::DrawGlobalsEditor(&ScenarioOpen);
    }
    if (ShowTypesEditor) {
        CCImGui::DrawTypesEditor(&ScenarioOpen);
    }
    if (true) {
    }
    if (true) {
    }
    if (true) {
    }




    Render_Frame();

    End_Frame();
}
