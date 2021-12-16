/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          RENDERER.H
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
#include "renderer.h"
#include "tibsun_globals.h"
#include "options.h"
#include "debughandler.h"
#include "asserthandler.h"




#include "bsurface.h"




/**
 *  For ImGui.
 */
#include "imgui.h"
#include "imgui_impl_softraster.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
//#include "imgui_impl_gdi.h"
#include "imgui_impl_gdi_2.h"
#include "imgui_impl_softraster.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>


/**
 *  Forward declarations.
 */
// Message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


//static LPDIRECT3D9 ImGui_pD3D = nullptr;
static LPDIRECT3D9EX ImGui_pD3D = nullptr;
//static LPDIRECT3DDEVICE9 ImGui_pd3dDevice = nullptr;
static LPDIRECT3DDEVICE9EX ImGui_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS ImGui_d3dpp = {};

static ImVec4 ImGui_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

bool /*Renderer_ImGui::*/ImGui_Available = false;
bool Renderer_ImGui::Show = false;
bool Renderer_ImGui::BreakThread = false;

static texture_color16_t ImGui_Screen;
static texture_alpha8_t ImGui_fontAtlas;

static BSurface *ImGui_Surface;
static Rect ImGui_Surface_DestRect;


static bool ImGui_CreateDeviceD3D(HWND hWnd)
{
    //if ((ImGui_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr) {
    Direct3DCreate9Ex(D3D_SDK_VERSION, &ImGui_pD3D);

    // Create the D3DDevice
    ZeroMemory(&ImGui_d3dpp, sizeof(ImGui_d3dpp));
    ImGui_d3dpp.BackBufferWidth = Options.ScreenWidth;
    ImGui_d3dpp.BackBufferHeight = Options.ScreenHeight;
    ImGui_d3dpp.BackBufferCount = 1;
    //ImGui_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    ImGui_d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
    ImGui_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    ImGui_d3dpp.Windowed = TRUE;
    //ImGui_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    //ImGui_d3dpp.SwapEffect = D3DSWAPEFFECT_OVERLAY;
    ImGui_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
    ImGui_d3dpp.EnableAutoDepthStencil = TRUE;
    ImGui_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    ImGui_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //ImGui_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    ImGui_d3dpp.hDeviceWindow = hWnd;
    //ImGui_d3dpp.Flags = D3DPRESENTFLAG_VIDEO;
    //ImGui_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    //if (ImGui_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &ImGui_d3dpp, &ImGui_pd3dDevice) < 0) {
    if (ImGui_pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &ImGui_d3dpp, nullptr, &ImGui_pd3dDevice) < 0) {
        return false;
    }

    return true;
}


static void ImGui_CleanupDeviceD3D()
{
    if (ImGui_pd3dDevice) {
        ImGui_pd3dDevice->Release();
        ImGui_pd3dDevice = nullptr;
    }
    if (ImGui_pD3D) {
        ImGui_pD3D->Release();
        ImGui_pD3D = nullptr;
    }
}


static void ImGui_ResetDeviceD3D()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    HRESULT hr = ImGui_pd3dDevice->Reset(&ImGui_d3dpp);
    if (hr == D3DERR_INVALIDCALL) {
        IM_ASSERT(0);
    }

    ImGui_ImplDX9_CreateDeviceObjects();
}


extern HMODULE DLLInstance;


#if 0
static DWORD WINAPI ImGui_Render_Thread(LPVOID lpParameter)
{
    while (!Renderer_ImGui::BreakThread) {
        Renderer_ImGui::Render_Loop();
        Sleep(1);
    }
    return TRUE;
}
#endif


bool Renderer_ImGui::Init()
{
    IMGUI_CHECKVERSION();

    #if 0
    /**
     *  Initialize ImGui Direct3D.
     */
    // Initialize Direct3D
    if (!ImGui_CreateDeviceD3D(MainWindow)) {
        ImGui_CleanupDeviceD3D();
        ImGui_Available = false;
        return false;
    }
    #endif

    /**
     *  Setup Dear ImGui context
     */
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    /**
     *  Setup Dear ImGui style
     */
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    ImGuiStyle &style = ImGui::GetStyle();
    style.AntiAliasedLines = false;
    style.AntiAliasedFill = false;
    //style.WindowRounding = 0.0f;

    /**
     *  Setup Platform/Renderer backends
     */
    ImGui_ImplWin32_Init(MainWindow);
    //ImGui_ImplDX9_Init(ImGui_pd3dDevice);
    //ImGui_ImplGDI_2_Init();
    //ImGui_ImplGDI_Init();



    ImGui_ImplSoftraster_Init(&ImGui_Screen);
    ImGui_Screen.init(CompositeSurface->Get_Width(), CompositeSurface->Get_Height());

    ImGui_Surface = new BSurface(CompositeSurface->Get_Width(),
                             CompositeSurface->Get_Height(), 2, ImGui_Screen.pixels);

    ImGui_Surface_DestRect = Rect(0, 16, CompositeSurface->Get_Width(), CompositeSurface->Get_Height());




    io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight | ImFontAtlasFlags_NoMouseCursors;

    uint8_t *pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
    ImGui_fontAtlas.init(width, height, (alpha8_t *)pixels);
    io.Fonts->TexID = &ImGui_fontAtlas;



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
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    ImGui_Available = true;

    //CreateThread(nullptr, 0, ImGui_Render_Thread, DLLInstance, 0, nullptr);

    return true;
}


void Renderer_ImGui::Shutdown()
{
    BreakThread = true;
    
    ImGui_ImplSoftraster_Shutdown();
    //ImGui_ImplGDI_Shutdown();
    //ImGui_ImplGDI_2_Shutdown();
    //ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}


bool Renderer_ImGui::Is_Available()
{
    return ImGui_Available;
}


void Renderer_ImGui::Render_Loop(XSurface *dest_surface)
{
    if (!ImGui_Available) {
        return;
    }

    if (!Show) {
        return;
    }

    /**
     *  Only draw the overlay while in an active game session.
     */
    if (!bool_007E48FC) {
        return;
    }

    Start();
    Process();
    Render(dest_surface);
    End();
}


/**
 *  Start new ImGui frame.
 */
void Renderer_ImGui::Start()
{
    ImGui_ImplWin32_NewFrame();
    //ImGui_ImplDX9_NewFrame();
    //ImGui_ImplGDI_NewFrame();
    //ImGui_ImplGDI_2_NewFrame();
    ImGui_ImplSoftraster_NewFrame();

    ImGui::NewFrame();
}


void Renderer_ImGui::Process()
{
#ifndef NDEBUG
    static bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);    // For testing.
#endif

    static bool show_another_window = true;

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
#ifndef NDEBUG
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
#endif
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&ImGui_clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me")) {
            show_another_window = false;
        }
        ImGui::End();
    }
}


void Renderer_ImGui::Render(XSurface *dest_surface)
{
#if 0
    ImGui_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    ImGui_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    ImGui_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(ImGui_clear_color.x*ImGui_clear_color.w*255.0f),
                                          (int)(ImGui_clear_color.y*ImGui_clear_color.w*255.0f),
                                          (int)(ImGui_clear_color.z*ImGui_clear_color.w*255.0f),
                                          (int)(ImGui_clear_color.w*255.0f));

    ImGui_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

    if (ImGui_pd3dDevice->BeginScene() < 0) {
        return;
    }



    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    #endif

    ImGui::Render();




    //ImGui_ImplGDI_SetBackgroundColor(&ImGui_clear_color);
    //ImGui_ImplGDI_RenderDrawData(ImGui::GetDrawData());

    //ImGui_ImplGDI_2_SetBackgroundColor(&ImGui_clear_color);
    //ImGui_ImplGDI_2_RenderDrawData(ImGui::GetDrawData());


    ImGui_ImplSoftraster_RenderDrawData(ImGui::GetDrawData());


    /**
     *  Finally, copy to the games surface.
     */
    if (ImGui_Surface) {
        dest_surface->Copy_From(ImGui_Surface_DestRect, *ImGui_Surface, ImGui_Surface->Get_Rect(), true);
    }



    #if 0

    
// Grab the backbuffer from the Direct3D device
LPDIRECT3DSURFACE9 back_buffer = NULL;
ImGui_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);

// Get the buffer's description and make an offscreen surface in system memory.
// We need to do this because the backbuffer is in video memory and can't be locked
// unless the device was created with a special flag (D3DPRESENTFLAG_LOCKABLE_BACKBUFFER).
// Unfortunately, a video-memory buffer CAN be locked with LockRect.  The effect is
// that it crashes your app when you try to read or write to it.
D3DLOCKED_RECT d3dlr;
D3DSURFACE_DESC desc;
LPDIRECT3DSURFACE9 offscreen_surface = NULL;
back_buffer->GetDesc(&desc);
ImGui_pd3dDevice->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format,
                                    D3DPOOL_SYSTEMMEM, &offscreen_surface, NULL);

// Copy from video memory to system memory
ImGui_pd3dDevice->GetRenderTargetData(back_buffer, offscreen_surface);
  
// Lock the surface using some flags for optimization
offscreen_surface->LockRect(&d3dlr, NULL, D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_READONLY);


    unsigned long *srcptr = reinterpret_cast<unsigned long *>(d3dlr.pBits);
    unsigned short *dstptr = reinterpret_cast<unsigned short *>(PrimarySurface->Lock());

    for (int i = 0; i < (PrimarySurface->Width * PrimarySurface->Height)/2; ++i) {
            unsigned short src_pixel = *(srcptr++);
            int r = (src_pixel >> 11) & 0x1F;
            r = ((r << 3) | (r & 7)) << 16;

            int g = (src_pixel >> 5) & 0x3F;
            g = ((g << 2) | (g & 3)) << 8;

            int b = src_pixel & 0x1F;
            b = (b << 3) | (b & 7);

            *dstptr++ = (r | g | b);
    }

    PrimarySurface->Unlock();

// Release all of our references
offscreen_surface->UnlockRect();
offscreen_surface->Release();
back_buffer->Release();


#endif



#if 0
    ImGui_pd3dDevice->EndScene();

    ///**
    // *  Handle loss of D3D9 device.
    // */
    HRESULT result = ImGui_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
    if (result == D3DERR_DEVICELOST && ImGui_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
        ImGui_ResetDeviceD3D();
    }
#endif








}


/**
 *  End the ImGui frame.
 */
void Renderer_ImGui::End()
{
    ImGui::EndFrame();
}


/**
 *  ImGui Win32 message handler
 */
LRESULT WINAPI Renderer_ImGui::Window_Procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /**
     *  Only process overlay messages while in an active game session.
     */
    if (!bool_007E48FC) {
        return false;
    }

    return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}
