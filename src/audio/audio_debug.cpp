/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_DEBUG.CPP
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
#include "audio_debug.h"
#include "audio_manager.h"
#include "tibsun_globals.h"
#include "options.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <tchar.h>
#include <chrono>
#include <thread>

#include <shellscalingapi.h>



#include "scenario.h"


static HWND ImGuiMainWindow = nullptr;

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);







#if 0
/**
 *  x
 *
 *  @author: CCHyper
 */
static LRESULT CALLBACK Audio_Debug_Window_Procedure(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
        case WM_INITDIALOG:
        {

        }

        case WM_PAINT:
        {
            InvalidateRect(hWnd, nullptr, TRUE);
            PAINTSTRUCT ps;
            HDC hDc = BeginPaint(hWnd, &ps);
            FillRect(hDc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hWnd, &ps);
            return 0;
        }

        /**
         *  Window got keyboard input.
         */
        case WM_CHAR:
        {
            switch (wParam)
            {
                /**
                 *  Set playback priority.
                 */
                case '1':
                case '2':
                case '3':
                case '4':

                /**
                 *  Stop/start all audio groups.
                 */
                case 'S':
                case 's':
                {
                    static bool _flag = false;
                    if (_flag) {
                        AudioManager.Stop_Group(AUDIO_GROUP_MUSIC);
                        AudioManager.Stop_Group(AUDIO_GROUP_MUSIC_AMBIENT);
                        AudioManager.Stop_Group(AUDIO_GROUP_SPEECH);
                        AudioManager.Stop_Group(AUDIO_GROUP_SOUND_EFFECT);
                        AudioManager.Stop_Group(AUDIO_GROUP_EVENT);
                    } else {
                        AudioManager.Start_Group(AUDIO_GROUP_MUSIC);
                        AudioManager.Start_Group(AUDIO_GROUP_MUSIC_AMBIENT);
                        AudioManager.Start_Group(AUDIO_GROUP_SPEECH);
                        AudioManager.Start_Group(AUDIO_GROUP_SOUND_EFFECT);
                        AudioManager.Start_Group(AUDIO_GROUP_EVENT);
                    }
                    _flag = !_flag;
                    break;
                }

                default:
                    break;
            };
        }

        case WM_DESTROY:
            break; // Can not be destroyed.

        case WM_SYSCOMMAND:
            switch (wParam) {
                case SC_CLOSE:
                    return 0;
            }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDOK:
                case IDCANCEL:
                    //EndDialog(hWnd, LOWORD(wParam));
                    return TRUE;
            };

        default:
            break;
    }

    return DefWindowProc(hWnd, Message, wParam, lParam);
}
#endif






// Helper functions

static void ImGui_CreateRenderTarget()
{
    if (!g_pd3dDevice) {
        return;
    }

    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

static void ImGui_CleanupRenderTarget()
{
    if (g_mainRenderTargetView) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

static bool ImGui_CreateDeviceD3D(HWND hWnd)
{
    if (g_pd3dDevice) {
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
    if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK) {
        return false;
    }

    ImGui_CreateRenderTarget();

    return true;
}

static void ImGui_CleanupDeviceD3D()
{
    ImGui_CleanupRenderTarget();

    if (g_pSwapChain) {
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }
    if (g_pd3dDeviceContext) {
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = nullptr;
    }
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
}


// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
static LRESULT WINAPI Audio_Debug_Main_Window_Procedure(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam)) {
        return true;
    }

    switch (Message)
    {
        case WM_SIZE:
            if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
            {
                ImGui_CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                ImGui_CreateRenderTarget();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, Message, wParam, lParam);
}

static void ImGui_New_Frame()
{
    if (!g_pd3dDeviceContext || !g_pSwapChain) {
        return;
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

static void ImGui_Render_Frame()
{
    if (!g_pd3dDeviceContext || !g_pSwapChain) {
        return;
    }

    // Rendering
    ImGui::Render();

    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    g_pSwapChain->Present(1, 0); // Present with vsync
    //g_pSwapChain->Present(0, 0); // Present without vsync
}

static void ImGui_End_Frame()
{
    ImGui::EndFrame();
}
















/**
 *  x
 */
static std::thread Audio_Debug_Thread;
static volatile bool Audio_Debug_Thread_Active = false;
static volatile bool Audio_Debug_Thread_Running = false;
static volatile bool Audio_Debug_Thread_InLoop = false;

/**
 *  x
 *
 *  @author: CCHyper
 */
#if 0
static void __cdecl Audio_Debug_Thread_Function()
{
    static bool _window_created = false;

    DEBUG_INFO("Audio::Debug - Entering thread.\n");

    Audio_Debug_Thread_Running = true;

    //if (!_window_created) {
    //    Audio_Debug_Create_Window();
    //    DEBUG_INFO("Audio::Debug - Window created.\n");
    //    _window_created = true;
    //}

    while (Audio_Debug_Thread_Active) {

        Audio_Debug_Thread_InLoop = true;

        // body
        Audio_Debug_Function();

        // Sleep the thread.
        //std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(1));

        std::this_thread::yield();

        Audio_Debug_Thread_InLoop = false;
    }

    Audio_Debug_Thread_Running = false;

    DEBUG_INFO("Audio::Debug - Exiting thread.\n");
}
#endif


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


/**
 *  Creates the debug ImGui window.
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Create_Debug_Window()
{
    return true;

    SetLastError(0);

    DEBUG_INFO("Audio::Debug - Creating window.\n");

    ImGui_ImplWin32_EnableDpiAwareness();

    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = Audio_Debug_Main_Window_Procedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = ProgramInstance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "Audio Debug Window";
    wc.hIconSm = nullptr;

    BOOL rc = RegisterClassEx(&wc);
    if (!rc) {
        DEBUG_ERROR("Audio::Debug - Failed to register window class!\n");
        return false;
    }

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "Audio Debug Window",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        Options.ScreenWidth,
        Options.ScreenHeight,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr);

    if (!hwnd) {
        DEBUG_ERROR("Audio::Debug - Failed to create window!\n");
        return false;
    }

    DEBUG_INFO("Audio::Debug - Setting window size.\n");

    // Resposition and resize the window based on the monitor scale.
    float scale = Get_Monitor_DPI_Scale(hwnd);

    SetWindowPos(hwnd,
        nullptr,
        GetSystemMetrics(SM_CXSCREEN) - Options.ScreenWidth,
        GetSystemMetrics(SM_CYSCREEN) - Options.ScreenHeight,
        Options.ScreenWidth * scale,
        Options.ScreenHeight * scale,
        SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

    DEBUG_INFO("Audio::Debug - Creating Direct3D device.\n");

    // Initialize Direct3D
    if (!ImGui_CreateDeviceD3D(hwnd)) {
        DEBUG_ERROR("Audio::Debug - Failed to create Direct3D device!\n");
        ImGui_CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return false;
    }

    // Store the window handle.
    ImGuiMainWindow = hwnd;

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    DEBUG_INFO("Audio::Debug - Setting up platform and renderer.\n");

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(ImGuiMainWindow);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

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

    DEBUG_INFO("Audio::Debug: Window created.\n");

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Close_Debug_Window()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    ImGui_CleanupDeviceD3D();

    DestroyWindow(ImGuiMainWindow);
    //UnregisterClass(wc.lpszClassName, wc.hInstance);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void AudioManagerClass::Debug_Window_Message_Handler()
{
    if (!ImGuiMainWindow) {
        return;
    }

    MSG msg;

    // Poll and handle messages (inputs, window resize, etc.)
    // See the WndProc() function below for our to dispatch events to the Win32 backend.
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
            break;
        }
    }
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void AudioManagerClass::Debug_Window_Loop()
{
    if (!ImGuiMainWindow) {
        return;
    }

    ImGui_New_Frame();

    // Update and scale the UI.
    //{
    //    ImGuiStyle& style = ImGui::GetStyle();
    //    float scale = Get_Monitor_DPI_Scale(ImGuiMainWindow);
    //    style.ScaleAllSizes(scale);
    //}

    // Our state
    static bool show_demo_window = true;
    static bool show_another_window = false;

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        //ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //ImGui::Checkbox("Another Window", &show_another_window);
        //
        //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        //
        //if (ImGui::Button("Button")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
        //    counter++;
        //}
        //
        //ImGui::SameLine();

        //ImGui::Text("Tracker.Count = %d", AudioManagerClass::SoundTrackerArrayType::CollectionCount);
        ImGui::Text("Music.Count = %d", AudioManager.SoundTracker.Raw(AUDIO_GROUP_MUSIC).Count());
        ImGui::Text("MusicAmbient.Count = %d", AudioManager.SoundTracker.Raw(AUDIO_GROUP_MUSIC_AMBIENT).Count());
        ImGui::Text("Speech.Count = %d", AudioManager.SoundTracker.Raw(AUDIO_GROUP_SPEECH).Count());
        ImGui::Text("SoundEffect.Count = %d", AudioManager.SoundTracker.Raw(AUDIO_GROUP_SOUND_EFFECT).Count());
        ImGui::Text("Event.Count = %d", AudioManager.SoundTracker.Raw(AUDIO_GROUP_EVENT).Count());

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    {
        ImGui::Begin("Hello, world! 2");
        ImGui::Text("UnitTypes.Count = %d", UnitTypes.Count());
        ImGui::Text("Units.Count = %d", Units.Count());
        ImGui::End();
    }

    // Scenario
    {
        if (Scen) {
            ImGui::Begin("Scenario");

            bool IsShadowGrow = Scen->SpecialFlags.IsShadowGrow;
            bool IsSpeedBuild = Scen->SpecialFlags.IsSpeedBuild;
            bool IsFromInstall = Scen->SpecialFlags.IsFromInstall;
            bool IsCaptureTheFlag = Scen->SpecialFlags.IsCaptureTheFlag;
            bool IsInert = Scen->SpecialFlags.IsInert;
            bool IsTGrowth = Scen->SpecialFlags.IsTGrowth;
            bool IsTSpread = Scen->SpecialFlags.IsTSpread;
            bool IsMCVDeploy = Scen->SpecialFlags.IsMCVDeploy;
            bool InitialVeteran = Scen->SpecialFlags.InitialVeteran;
            bool FixedAlliance = Scen->SpecialFlags.FixedAlliance;
            bool HarvesterImmune = Scen->SpecialFlags.HarvesterImmune;
            bool FogOfWar = Scen->SpecialFlags.FogOfWar;
            bool Bit2_16 = Scen->SpecialFlags.Bit2_16;
            bool TiberiumExplosive = Scen->SpecialFlags.TiberiumExplosive;
            bool DestroyableBridges = Scen->SpecialFlags.DestroyableBridges;
            bool Meteorites = Scen->SpecialFlags.Meteorites;
            bool IonStorms = Scen->SpecialFlags.IonStorms;
            bool IsVisceroids = Scen->SpecialFlags.IsVisceroids;

            ImGui::Checkbox("IsShadowGrow", &IsShadowGrow);
            ImGui::Checkbox("IsSpeedBuild", &IsSpeedBuild);
            ImGui::Checkbox("IsFromInstall", &IsFromInstall);
            ImGui::Checkbox("IsCaptureTheFlag", &IsCaptureTheFlag);
            ImGui::Checkbox("IsInert", &IsInert);
            ImGui::Checkbox("IsTGrowth", &IsTGrowth);
            ImGui::Checkbox("IsTSpread", &IsTSpread);
            ImGui::Checkbox("IsMCVDeploy", &IsMCVDeploy);
            ImGui::Checkbox("InitialVeteran", &InitialVeteran);
            ImGui::Checkbox("FixedAlliance", &FixedAlliance);
            ImGui::Checkbox("HarvesterImmune", &HarvesterImmune);
            ImGui::Checkbox("FogOfWar", &FogOfWar);
            ImGui::Checkbox("Bit2_16", &Bit2_16);
            ImGui::Checkbox("TiberiumExplosive", &TiberiumExplosive);
            ImGui::Checkbox("DestroyableBridges", &DestroyableBridges);
            ImGui::Checkbox("Meteorites", &Meteorites);
            ImGui::Checkbox("IonStorms", &IonStorms);
            ImGui::Checkbox("IsVisceroids", &IsVisceroids);

            Scen->SpecialFlags.IsShadowGrow = IsShadowGrow;
            Scen->SpecialFlags.IsSpeedBuild = IsSpeedBuild;
            Scen->SpecialFlags.IsFromInstall = IsFromInstall;
            Scen->SpecialFlags.IsCaptureTheFlag = IsCaptureTheFlag;
            Scen->SpecialFlags.IsInert = IsInert;
            Scen->SpecialFlags.IsTGrowth = IsTGrowth;
            Scen->SpecialFlags.IsTSpread = IsTSpread;
            Scen->SpecialFlags.IsMCVDeploy = IsMCVDeploy;
            Scen->SpecialFlags.InitialVeteran = InitialVeteran;
            Scen->SpecialFlags.FixedAlliance = FixedAlliance;
            Scen->SpecialFlags.HarvesterImmune = HarvesterImmune;
            Scen->SpecialFlags.FogOfWar = FogOfWar;
            Scen->SpecialFlags.Bit2_16 = Bit2_16;
            Scen->SpecialFlags.TiberiumExplosive = TiberiumExplosive;
            Scen->SpecialFlags.DestroyableBridges = DestroyableBridges;
            Scen->SpecialFlags.Meteorites = Meteorites;
            Scen->SpecialFlags.IonStorms = IonStorms;
            Scen->SpecialFlags.IsVisceroids = IsVisceroids;

            ImGui::SliderInt("AmbientOriginal", &Scen->AmbientOriginal, 0, 10000, "%d");
            ImGui::SliderInt("AmbientCurrent", &Scen->AmbientCurrent, 0, 10000, "%d");
            ImGui::SliderInt("AmbientTarget", &Scen->AmbientTarget, 0, 10000, "%d");

            ImGui::SliderInt("Red", &Scen->Red, 0, 10000, "%d");
            ImGui::SliderInt("Green", &Scen->Green, 0, 10000, "%d");
            ImGui::SliderInt("Blue", &Scen->Blue, 0, 10000, "%d");
            ImGui::SliderInt("Ground", &Scen->Ground, 0, 10000, "%d");
            ImGui::SliderInt("Level", &Scen->Level, 0, 10000, "%d");
            ImGui::Checkbox("IsFreeRadar", &Scen->IsFreeRadar);
            ImGui::Checkbox("IsTrainCrate", &Scen->IsTrainCrate);
            ImGui::Checkbox("IsTiberiumGrowth", &Scen->IsTiberiumGrowth);
            ImGui::Checkbox("IsVeinGrowth", &Scen->IsVeinGrowth);
            ImGui::Checkbox("IsIceGrowth", &Scen->IsIceGrowth);
            ImGui::Checkbox("IsBridgeChanged", &Scen->IsBridgeChanged);
            ImGui::Checkbox("IsFlagChanged", &Scen->IsFlagChanged);
            ImGui::Checkbox("IsAmbientChanged", &Scen->IsAmbientChanged);
            ImGui::Checkbox("IsEndOfGame", &Scen->IsEndOfGame);
            ImGui::Checkbox("IsInheritTimer", &Scen->IsInheritTimer);
            ImGui::Checkbox("IsSkipScore", &Scen->IsSkipScore);
            ImGui::Checkbox("IsOneTimeOnly", &Scen->IsOneTimeOnly);
            ImGui::Checkbox("IsNoMapSel", &Scen->IsNoMapSel);
            ImGui::Checkbox("IsTruckCrate", &Scen->IsTruckCrate);
            ImGui::Checkbox("IsMoneyTiberium", &Scen->IsMoneyTiberium);
            ImGui::Checkbox("IsTiberiumDeathToVisceroid", &Scen->IsTiberiumDeathToVisceroid);
            ImGui::Checkbox("IsIgnoreGlobalAITriggers", &Scen->IsIgnoreGlobalAITriggers);
            ImGui::Checkbox("IsGDI", &Scen->IsGDI);
            ImGui::Checkbox("IsMultiplayerOnly", &Scen->IsMultiplayerOnly);
            ImGui::Checkbox("IsRandom", &Scen->IsRandom);
            ImGui::Checkbox("CratePickedUp", &Scen->CratePickedUp);

            ImGui::End();
        }
    }

    // 3. Show another simple window.
    if (show_another_window) {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me")) {
            show_another_window = false;
        }
        ImGui::End();
    }

    ImGui_Render_Frame();

    ImGui_End_Frame();

    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}
