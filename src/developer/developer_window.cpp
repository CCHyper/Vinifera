/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DEVELOPER_WINDOW.CPP
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

#include "d3d_util.h"
#include "d3d_imgui.h"

#include "developer_window.h"
#include "tibsun_globals.h"
#include "options.h"
#include "scenario.h"
#include "debughandler.h"


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
            if (D3D11Device != nullptr && wParam != SIZE_MINIMIZED)
            {
                D3D11_CleanupRenderTarget();
                D3D11SwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                D3D11_CreateRenderTarget();
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

    DEBUG_INFO("Developer - Entering thread.\n");

    Audio_Debug_Thread_Running = true;

    //if (!_window_created) {
    //    Audio_Debug_Create_Window();
    //    DEBUG_INFO("Developer - Window created.\n");
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

    DEBUG_INFO("Developer - Exiting thread.\n");
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
bool DeveloperModeWindowClass::Create_Window()
{
    SetLastError(0);

    DEBUG_INFO("Developer - Creating window.\n");

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
    wc.lpszClassName = "Developer Mode Window";
    wc.hIconSm = nullptr;

    BOOL rc = RegisterClassEx(&wc);
    if (!rc) {
        DEBUG_ERROR("Developer - Failed to register window class!\n");
        return false;
    }

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "Developer Mode Window",
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
        DEBUG_ERROR("Developer - Failed to create window!\n");
        return false;
    }

    DEBUG_INFO("Developer - Setting window size.\n");

    // Resposition and resize the window based on the monitor scale.
    float scale = Get_Monitor_DPI_Scale(hwnd);

    SetWindowPos(hwnd,
        nullptr,
        GetSystemMetrics(SM_CXSCREEN) - Options.ScreenWidth,
        GetSystemMetrics(SM_CYSCREEN) - Options.ScreenHeight,
        Options.ScreenWidth * scale,
        Options.ScreenHeight * scale,
        SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

    DEBUG_INFO("Developer - Creating Direct3D device.\n");

    // Initialize Direct3D
    if (!D3D11_CreateDeviceD3D(hwnd)) {
        DEBUG_ERROR("Developer - Failed to create Direct3D device!\n");
        D3D11_CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return false;
    }

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    D3D11_ImGui_Initalise_Style();
    D3D11_ImGui_Initalise_Config();
    D3D11_ImGui_Initalise_Options();

#if 0
    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
#endif

    DEBUG_INFO("Developer - Setting up platform and renderer.\n");

    // Setup Platform/Renderer backends
    //D3D11_ImGui_Initalise();

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

    DEBUG_INFO("Developer: Window created.\n");

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DeveloperModeWindowClass::Destroy_Window()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    D3D11_CleanupDeviceD3D();

    //DestroyWindow(ImGuiMainWindow);
    //UnregisterClass(wc.lpszClassName, wc.hInstance);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DeveloperModeWindowClass::Window_Message_Handler()
{
    //if (!ImGuiMainWindow) {
    //    return;
    //}

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
void DeveloperModeWindowClass::Window_Loop()
{
    //if (!ImGuiMainWindow) {
    //    return;
    //}


    // Update and scale the UI.
    //{
    //    ImGuiStyle& style = ImGui::GetStyle();
    //    float scale = Get_Monitor_DPI_Scale(ImGuiMainWindow);
    //    style.ScaleAllSizes(scale);
    //}

    D3D11_ImGui_New_Frame();

#if 0
    if (ImGui::BeginMainMenuBar()) {

        /**
         *  x
         */
        if (ImGui::BeginMenu("Tools")) {

            //if (ImGui::MenuItem("Save", "Ctrl+S", false, MapClass::InMap())) {
            //    //menu_action = "FileSave";
            //}

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Terrain")) {
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::BeginMenu("Debug")) {
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

#endif



    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    static bool show_demo_window = true;
    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }


    Game_View_Window();
    Asset_Browser_Window();
    Trigger_List_Window();
    Team_List_Window();




#if 0




    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    static bool show_demo_window = true;
    //if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    //}

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

#if 0
        //ImGui::Text("Tracker.Count = %d", AudioManagerClass::SoundTrackerArrayType::CollectionCount);
        ImGui::Text("Music.Count = %d", AudioManager.SoundTracker.Raw(AUDIO_GROUP_MUSIC).Count());
        ImGui::Text("MusicAmbient.Count = %d", AudioManager.SoundTracker.Raw(AUDIO_GROUP_MUSIC_AMBIENT).Count());
        ImGui::Text("Speech.Count = %d", AudioManager.SoundTracker.Raw(AUDIO_GROUP_SPEECH).Count());
        ImGui::Text("SoundEffect.Count = %d", AudioManager.SoundTracker.Raw(AUDIO_GROUP_SOUND_EFFECT).Count());
        ImGui::Text("Event.Count = %d", AudioManager.SoundTracker.Raw(AUDIO_GROUP_EVENT).Count());
#endif

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
    //if (show_another_window) {
    //    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    //    ImGui::Text("Hello from another window!");
    //    if (ImGui::Button("Close Me")) {
    //        show_another_window = false;
    //    }
    //    ImGui::End();
    //}
#endif

    //D3D11_ImGui_Render_Frame();

    D3D11_ImGui_End_Frame();

#if 0
    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
#endif
}
