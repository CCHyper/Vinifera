#include "ingame_overlay.h"
#include "tibsun_globals.h"
#include "vinifera_globals.h"
#include "bsurface.h"
#include "rect.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "imgui.h"
#include "imgui_impl_softraster.h"
#include "imgui_impl_win32.h"
#include "imguial_fonts.h"

#include "mapedit_overlay.h"
#include "developer_overlay.h"


/**
 *  
 */
bool InGameOverlay::IsInitialised = false;

InGameOverlay::OverlayMode InGameOverlay::Mode = InGameOverlay::INGAME_OVERLAY_NONE;


/**
 *  Forward declarations.
 */
// Message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/**
 *  Softraster textures
 */
static texture_color16_t ImGui_SoftrasterTexture;
static texture_alpha8_t ImGui_FontAtlas;


/**
 *  Working game surface.
 */
static BSurface *ImGui_Surface;
static Rect ImGui_Surface_DestRect;


extern HMODULE DLLInstance;
bool BreakThread = false;

#if 1
static DWORD WINAPI ImGui_Render_Thread(LPVOID lpParameter)
{
    while (!BreakThread) {
        //InGameOverlay::Process();
        InGameOverlay::Render(nullptr);
        Sleep(0);
    }
    return TRUE;
}
#endif







bool InGameOverlay::Init()
{
    IMGUI_CHECKVERSION();

    /**
     *  Setup Dear ImGui context
     */
    ImGui::CreateContext();

    /**
     *  
     */
    DEBUG_INFO("ImGui: Setting up IO.\n");
{
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;           // This disables the ini saving for ImGui.
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls.
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
}

    /**
     *  Setup Dear ImGui style
     * 
     *  @author: jane3kb04
     *  https://github.com/ocornut/imgui/issues/707#issuecomment-917151020
     */
    DEBUG_INFO("ImGui: Setting up style.\n");
{
    // #WARNING: We can not use black as the games blitters knock that out!
    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.10f, 0.10f, 0.10f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.10f, 0.10f, 0.10f, 0.24f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.10f, 0.10f, 0.10f, 0.52f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.10f, 0.10f, 0.10f, 0.36f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.10f, 0.10f, 0.10f, 0.52f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.10f, 0.10f, 0.10f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    //colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    //colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.10f, 0.10f, 0.10f, 0.52f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.10f, 0.10f, 0.10f, 0.52f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.10f, 0.10f, 0.10f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
}

    DEBUG_INFO("ImGui: Setting up colors.\n");
{
    ImGuiStyle &style = ImGui::GetStyle();
    style.Alpha                             = 1.0;
    style.WindowPadding                     = ImVec2(8.00f, 8.00f);
    style.FramePadding                      = ImVec2(5.00f, 2.00f);
    style.CellPadding                       = ImVec2(6.00f, 6.00f);
    style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
    style.IndentSpacing                     = 25;
    style.ScrollbarSize                     = 15;
    style.GrabMinSize                       = 10;
    style.WindowBorderSize                  = 1;
    style.ChildBorderSize                   = 1;
    style.PopupBorderSize                   = 1;
    style.FrameBorderSize                   = 1;
    style.TabBorderSize                     = 1;
    style.WindowRounding                    = 7;
    style.ChildRounding                     = 4;
    style.FrameRounding                     = 3;
    style.PopupRounding                     = 4;
    style.ScrollbarRounding                 = 9;
    style.GrabRounding                      = 3;
    style.LogSliderDeadzone                 = 4;
    style.TabRounding                       = 4;
}

    /**
     *  
     */
    DEBUG_INFO("ImGui: Setting up fonts.\n");
{
    /**
     *  Load bitmap font.
     */
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig config;
    io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight|ImFontAtlasFlags_NoMouseCursors;

    uint8_t *pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
    ImGui_FontAtlas.init(width, height, (alpha8_t *)pixels);
    io.Fonts->TexID = &ImGui_FontAtlas;
}

#if 0
    /**
     *  Load FontAwesome
     */
{
    int ttf_size = 0;
    const void *ttf_data = ImGuiAl::Fonts::GetCompressedData(ImGuiAl::Fonts::kFontAwesome5Regular, &ttf_size);
    if (!ttf_data) {
        DEBUG_ERROR("ImGui: Loading FontAwesome failed!.\n");
        return false;
    }

    // merge in icons from Font Awesome.
    static const ImWchar ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true; // #SEE: https://github.com/ocornut/imgui/issues/691

    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromMemoryCompressedTTF(ttf_data, ttf_size, 10.0f, &config, ranges);

    DEBUG_INFO("ImGui: Loaded FontAwesome OK.\n");
}
#endif

    /**
     *  Setup platform backends
     */
    if (!ImGui_ImplWin32_Init(MainWindow)) {
        return false;
    }

    /**
     *  Setup renderer backend.
     */
    if (!ImGui_ImplSoftraster_Init(&ImGui_SoftrasterTexture)) {
        return false;
    }

    /**
     *  
     */
    ImGui_SoftrasterTexture.init(PrimarySurface->Get_Width(), PrimarySurface->Get_Height());

    ImGui_Surface_DestRect = PrimarySurface->Get_Rect();

    ImGui_Surface = new BSurface(PrimarySurface->Get_Width(),
                                 PrimarySurface->Get_Height(),
                                 PrimarySurface->Get_Bytes_Per_Pixel(),
                                 ImGui_SoftrasterTexture.pixels);

    /**
     *  All done and ready to go!
     */
    InGameOverlay::IsInitialised = true;

    //CreateThread(nullptr, 0, ImGui_Render_Thread, DLLInstance, 0, nullptr);

    return true;
}


void InGameOverlay::Shutdown()
{
    ImGui_ImplSoftraster_Shutdown();
    ImGui_ImplWin32_Shutdown();
}


void InGameOverlay::Process()
{
    if (Mode == INGAME_OVERLAY_NONE) {
        return;
    }

    /**
     *  
     */
    switch (Mode) {
        case INGAME_OVERLAY_DEVELOPER:
            if (Vinifera_DeveloperMode && !Debug_Map) {
                Developer_Overlay_Process();
            }
            break;

        case INGAME_OVERLAY_SCENARIO_EDITOR:
            if (Vinifera_DeveloperMode && Debug_Map) {
                MapEdit_Draw_Overlay();
            }
            break;

        default:
            return;
    };
}


#if 0
void InGameOverlay::Render(XSurface *surface)
{
    if (Mode == INGAME_OVERLAY_NONE) {
        return;
    }

    ImGui_ImplWin32_NewFrame();
    ImGui_ImplSoftraster_NewFrame();

    ImGui::NewFrame();

    Process();

    ImGui::Render();

    /**
     *  Render ImGui data to the Softraster texture.
     */
    ImGui_ImplSoftraster_RenderDrawData(ImGui::GetDrawData());

    /**
     *  Finally, copy to the games surface.
     */
    if (ImGui_Surface) {
        surface->Copy_From(ImGui_Surface_DestRect, *ImGui_Surface, ImGui_Surface->Get_Rect(), true);
    }

    ImGui::EndFrame();
}
#endif


void InGameOverlay::Render_To_Surface(XSurface *surface)
{
    /**
     *  Only draw the overlay while in an active game session.
     */
    if (!bool_007E48FC) {
        return;
    }

    if (Mode == INGAME_OVERLAY_NONE) {
        return;
    }

    ImGui_ImplWin32_NewFrame();
    ImGui_ImplSoftraster_NewFrame();

    ImGui::NewFrame();

    Process();

    ImGui::Render();

    /**
     *  Render ImGui data to the Softraster texture.
     */
    ImGui_ImplSoftraster_RenderDrawData(ImGui::GetDrawData());

    /**
     *  Finally, copy to the games surface.
     */
    if (ImGui_Surface) {
        surface->Copy_From(ImGui_Surface_DestRect, *ImGui_Surface, ImGui_Surface->Get_Rect(), true);
    }

    ImGui::EndFrame();
}
