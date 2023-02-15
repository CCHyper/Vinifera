/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          D3D_HOOKS.CPP
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
#include "d3d_hooks.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "d3dsurface.h"
#include "options.h"
#include "optionsext.h"
#include "movie.h"
#include "filepng.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"
#include "bsurface.h"

#include "hooker.h"
#include "hooker_macros.h"

BSurface *&D3DTileSurface = Make_Global<BSurface *>(0x0074C5CC);
BSurface *&D3DSidebarSurface = Make_Global<BSurface *>(0x0074C5D0);
BSurface *&D3DPrimarySurface = Make_Global<BSurface *>(0x0074C5D8);
BSurface *&D3DHiddenSurface = Make_Global<BSurface *>(0x0074C5DC);
BSurface *&D3DAlternateSurface = Make_Global<BSurface *>(0x0074C5E0);
BSurface *&D3DTempSurface = Make_Global<BSurface *>(0x0074C5E4);
BSurface *&D3DCompositeSurface = Make_Global<BSurface *>(0x0074C5EC);

#pragma comment (lib, "d3d9.lib")

#pragma comment (lib, "d3d9.lib")
 // global declarations
LPDIRECT3D9 Direct3D;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 Direct3DDevice;    // the pointer to the device class
LPDIRECT3DSURFACE9 D3DMainSurface;    // the pointer to the device class
LPDIRECT3DSURFACE9 D3DBackSurface;    // the pointer to the device class

void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame();    // renders a single frame
void cleanD3D();    // closes Direct3D and releases memory

bool surface_ready = false;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int &ScreenWidth = Make_Global<int>(0x007A1EC0);
int &ScreenHeight = Make_Global<int>(0x007A1EC4);

DWORD WINAPI DXWinMain(LPVOID lpParameter)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = ProgramInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "WindowClass";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(
        WS_EX_LEFT | WS_EX_TOPMOST,
        "WindowClass",
        "Direct3D",
        WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CAPTION | WS_THICKFRAME,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr);

    RECT rect;
    SetRect(&rect, 0, 0, ScreenWidth, ScreenHeight);

    AdjustWindowRectEx(&rect,
        GetWindowLong(hWnd, GWL_STYLE),
        GetMenu(hWnd) != nullptr,
        GetWindowLong(hWnd, GWL_EXSTYLE));

    int display_width = GetSystemMetrics(SM_CXSCREEN);
    int display_height = GetSystemMetrics(SM_CYSCREEN);
    /**
     *  #BUGFIX:
     *
     *  Fetch the desktop size, calculate the screen center position the window and move it.
     */
    RECT workarea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);

    int x_pos = (display_width - ScreenWidth) / 2;
    int y_pos = (((display_height - ScreenHeight) / 2) - (display_height - workarea.bottom));

    DEBUG_INFO("Create_Window() - Moving window (%d,%d,%d,%d).\n",
        x_pos, y_pos, (rect.right - rect.left), (rect.bottom - rect.top));

    MoveWindow(hWnd, x_pos, y_pos, (rect.right - rect.left), (rect.bottom - rect.top), TRUE);

    ShowWindow(hWnd, 1);

    MSG msg;

    while (TRUE) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
            break;

        render_frame();
    }

    cleanD3D();
    return 0;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool mD3D_Lock_Surface(LPDIRECT3DSURFACE9 surface, D3DLOCKED_RECT &lock_rect, bool discard, bool readonly)
{
    ASSERT(surface != nullptr);

    lock_rect.Pitch = 0;
    lock_rect.pBits = nullptr;

    DWORD flags = (discard ? D3DLOCK_DISCARD : 0);
    if (readonly) {
        flags |= D3DLOCK_READONLY;
    } else {
        flags |= D3DLOCK_NOSYSLOCK;
    }

    HRESULT res = surface->LockRect(&lock_rect, nullptr, flags);

    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to lock back surface! Error code %x!", res);
        return false;
    }

    if (lock_rect.pBits == nullptr) {
        DEBUG_ERROR("DirectX9 - Failed to get d3d surface bits!\n");
        return false;
    }

    return true;
}

bool mD3D_Unlock_Surface(LPDIRECT3DSURFACE9 surface)
{
    ASSERT(surface != nullptr);

    HRESULT res = surface->UnlockRect();

    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to unlock back surface! Error code %x!", res);
        return false;
    }

    return true;
}


XSurface *FlipSurface = nullptr;

bool mD3D_Flip(XSurface *surface)
{
    if (!surface_ready) return false;

    if (!surface) {
        DEBUG_ERROR("DirectX9 - No game surface!\n");
        return false;
    }

    if (!D3DMainSurface) {
        DEBUG_ERROR("DirectX9 - No d3d surface!\n");
        return false;
    }

    D3DLOCKED_RECT lock_rect;

    unsigned short *srcptr = reinterpret_cast<unsigned short *>(surface->Lock());
    if (!srcptr) {
        DEBUG_ERROR("DirectX9 - Failed to lock game surface!\n");
        return false;
    }

    if (!mD3D_Lock_Surface(D3DMainSurface, lock_rect, false, false)) {
        DEBUG_ERROR("DirectX9 - Failed to lock destination surface!\n");
        surface->Unlock();
        return false;
    }

    unsigned short *dstptr = reinterpret_cast<unsigned short *>(lock_rect.pBits);
    std::memcpy(dstptr, srcptr, surface->Get_Width() * surface->Get_Height() * surface->Get_Bytes_Per_Pixel());


    if (!mD3D_Unlock_Surface(D3DMainSurface)) {
        DEBUG_ERROR("DirectX9 - Failed to unlock destination surface!\n");
        surface->Unlock();
        return false;
    }

    if (!surface->Unlock()) {
        DEBUG_ERROR("DirectX9 - Failed to unlock game surface!\n");
        return false;
    }

    RECT src_rect;
    src_rect.left = 0;
    src_rect.top = 0;
    src_rect.right = surface->Get_Width();
    src_rect.bottom = surface->Get_Height();

    RECT dest_rect;
    dest_rect.left = 0;
    dest_rect.top = 0;
    dest_rect.right = surface->Get_Width();
    dest_rect.bottom = surface->Get_Height();


    D3DTEXTUREFILTERTYPE d3dtft = D3DTEXF_NONE;
    Direct3DDevice->StretchRect(D3DMainSurface, &src_rect, D3DBackSurface, &dest_rect, d3dtft);

    // Present the backbuffer contents to the display.
    Direct3DDevice->Present(&src_rect, &dest_rect, nullptr, nullptr);

    return true;
}
DSurface *BackSurface = nullptr;

void render_frame()
{
    // was a pointer to last DSurface that invoked Blt but doesn't work well
    //if (FlipSurface)
    //{
    //    D3D_Flip(FlipSurface);
    //    FlipSurface = nullptr;
    //    //DEBUG_ERROR("Drew Flipped\n");
    //}

    // blit the actual final surface
    if (PrimarySurface) {
        //Direct3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

        mD3D_Flip(PrimarySurface);
        //DEBUG_ERROR("Drew Flipped\n");
    }
}

void cleanD3D()
{
    Direct3DDevice->Release();
    Direct3D->Release();
}

bool D3DInit()
{
    HANDLE hThread = CreateThread(
        NULL,
        0,
        DXWinMain,
        NULL,
        0,
        NULL);

    if (hThread == NULL) {
        return false;
    }

    return true;
}




// Cast DSurface pointer to D3DSurface.
#define D3D_SURFACE_ASSIGN(surface, ptr) surface = reinterpret_cast<DSurface *>(ptr)
#define D3D_SURFACE_CAST(surface) reinterpret_cast<D3DSurface *>(surface)



void __cdecl D3D_Write_Surface_Data_To_File(const char *filename, XSurface *surface)
{
#if 1 // #ifndef NDEBUG
    Write_PNG_File(&RawFileClass(filename), *surface, &GamePalette);
#endif
}

static bool Allocate_Surfaces_Intercept(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect)
{
    DEBUG_INFO("Allocating game surfaces...\n");
    /**
     *  Delete existing surface instances.
     */
    if (D3DAlternateSurface) {
        DEBUG_INFO("Deleting AlternateSurface\n");
        delete D3DAlternateSurface;
        D3DAlternateSurface = nullptr;
    }
    if (D3DHiddenSurface) {
        DEBUG_INFO("Deleting HiddenSurface\n");
        delete D3DHiddenSurface;
        D3DHiddenSurface = nullptr;
    }
    if (D3DCompositeSurface) {
        DEBUG_INFO("Deleting CompositeSurface\n");
        delete D3DCompositeSurface;
        D3DCompositeSurface = nullptr;
    }
    if (D3DTileSurface) {
        DEBUG_INFO("Deleting TileSurface\n");
        delete D3DTileSurface;
        D3DTileSurface = nullptr;
    }
    if (D3DSidebarSurface) {
        DEBUG_INFO("Deleting SidebarSurface\n");
        delete D3DSidebarSurface;
        D3DSidebarSurface = nullptr;
    }

    /**
     *  Create new instances of the surfaces using the Virtual interface.
     */
    if (common_rect->Width > 0 && common_rect->Height > 0) {
        D3DAlternateSurface = new BSurface(common_rect->Width, common_rect->Height, 2);
        D3DAlternateSurface->Clear();
        DEBUG_INFO("AlternateSurface created (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (common_rect->Width > 0 && common_rect->Height > 0) {
        D3DHiddenSurface = new BSurface(common_rect->Width, common_rect->Height, 2);
        D3DHiddenSurface->Clear();
        DEBUG_INFO("HiddenSurface created (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (composite_rect->Width > 0 && composite_rect->Height > 0) {
        D3DCompositeSurface = new BSurface(common_rect->Width, common_rect->Height, 2);
        D3DCompositeSurface->Clear();
        DEBUG_INFO("CompositeSurface created (%dx%d)\n", composite_rect->Width, composite_rect->Height);
    }

    if (tile_rect->Width > 0 && tile_rect->Height > 0) {
        D3DTileSurface = new BSurface(common_rect->Width, common_rect->Height, 2);
        D3DTileSurface->Clear();
        DEBUG_INFO("TileSurface created (%dx%d)\n", tile_rect->Width, tile_rect->Height);
    }

    if (sidebar_rect->Width > 0 && sidebar_rect->Height > 0) {
        D3DSidebarSurface = new BSurface(common_rect->Width, common_rect->Height, 2);
        D3DSidebarSurface->Clear();
        DEBUG_INFO("SidebarSurface created (%dx%d)\n", sidebar_rect->Width, sidebar_rect->Height);
    }
    D3DInit();

    surface_ready = true;

    return true;
}

static XSurface *Create_Primary_Intercept(XSurface **backbuffer_surface)
{
    ScreenWidth = Options.ScreenWidth;
    ScreenHeight = Options.ScreenHeight;


    // Force all new renderers to be created as a window.
    OptionsExtension->IsWindowed = true;
    OptionsExtension->IsBorderlessWindow = false;
    Debug_Windowed = OptionsExtension->IsWindowed;



    /**
     *  Create a virtual surface keep the game happy.
     */
    BSurface *primary_surface = new BSurface(Options.ScreenWidth, Options.ScreenHeight, 2);
    ASSERT(primary_surface != nullptr);

    if (!primary_surface) {
        DEBUG_ERROR("Create_Primary() - Failed to create primary surface!\n");
        Fatal("Create_Primary() - Failed to create primary surface!\n");
        return nullptr;
    }

    DSurface::AllowStretchBlits = true;
    DSurface::AllowHardwareBlitFills = true;

    /**
     *  Tell the game that the virtual surface is RGB 565.
     */
    DSurface::RedLeft = 11;
    DSurface::RedRight = 3;
    DSurface::GreenLeft = 5;
    DSurface::GreenRight = 2;
    DSurface::BlueLeft = 0;
    DSurface::BlueRight = 3;

    DSurface::RGBPixelFormat = 2; // RGB565

    DSurface::ColorGrey = DSurface::RGB_To_Pixel(127, 127, 127);
    DSurface::ColorMidGrey = DSurface::RGB_To_Pixel(63, 63, 63);
    DSurface::ColorDarkGrey = DSurface::RGB_To_Pixel(31, 31, 31);

    // Hack! Window seems to be out of focus at this point...
    // The window needs this initially otherwise we need to alt-tab to gain focus.
    GameInFocus = true;

    return primary_surface;
}

static void Wait_Blit_Intercept()
{
}

static bool Set_Video_Mode_Intercept(HWND hWnd, int width, int height, int bits_per_pixel)
{
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.Windowed = TRUE;

    d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    //d3dpp.BackBufferWidth = width;
    //d3dpp.BackBufferHeight = height;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    //d3dpp.EnableAutoDepthStencil = TRUE;
    //d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    DWORD behaviour_flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    if (false) {
        behaviour_flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

    HRESULT res = Direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, behaviour_flags, &d3dpp, &Direct3DDevice);
    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to create Direct3D device! Error code %x!\n", res);
        return false;
    }

    res = Direct3DDevice->CreateOffscreenPlainSurface(width, height, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &D3DMainSurface, nullptr);

    if (FAILED(res)) {
        DEBUG_ERROR("Failed to create surface with error %x.\n", res);
    }

    res = Direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &D3DBackSurface);
    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to get back buffer surface! Error code %x!", res);
    }
    return true;
}

static void Reset_Video_Mode_Intercept()
{
    if (Direct3DDevice) {
        Direct3DDevice->Release();
        Direct3DDevice = nullptr;
    }
    if (Direct3D) {
        Direct3D->Release();
        Direct3D = nullptr;
    }
}

static void Check_Overlapped_Blit_Capability_Intercept()
{
    // empty
}

static bool Prep_Renderer_Intercept(HWND hWnd)
{
    Direct3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!Direct3D) {
        return false;
    }

    return true;
}







bool D3D_Clear_Screen(bool present)
{
    Direct3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    if (present) {
        Direct3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
    }

    return true;
}



/**
 *  x
 *
 *  @author: CCHyper
 */
bool D3D_Lock_Surface(LPDIRECT3DSURFACE9 surface, D3DLOCKED_RECT& lock_rect, bool discard, bool readonly)
{
    ASSERT(surface != nullptr);

    lock_rect.Pitch = 0;
    lock_rect.pBits = nullptr;

    DWORD flags = (discard ? D3DLOCK_DISCARD : 0);
    if (readonly) {
        flags |= D3DLOCK_READONLY;
    }
    else {
        flags |= D3DLOCK_NOSYSLOCK;
    }

    HRESULT res = surface->LockRect(&lock_rect, nullptr, flags);

    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to lock back surface! Error code %x!", res);
        return false;
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool D3D_Unlock_Surface(LPDIRECT3DSURFACE9 surface)
{
    ASSERT(surface != nullptr);

    HRESULT res = surface->UnlockRect();

    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to unlock back surface! Error code %x!", res);
        return false;
    }

    return true;
}

bool D3D_Flip(XSurface *surface)
{
    Direct3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

    D3DLOCKED_RECT lock_rect;

    if (!D3D_Lock_Surface(D3D_SURFACE_CAST(PrimarySurface)->Get_Video_Surface_Ptr(), lock_rect, false, false)) {
        DEBUG_ERROR("DirectX9 - Failed to lock destination surface!\n");
        return false;
    }

    //unsigned short *dstptr = reinterpret_cast<unsigned short *>(D3D_SURFACE_CAST(PrimarySurface)->Lock());
    //if (!dstptr) {
    //    DEBUG_ERROR("DirectX9 - Failed to lock game surface!\n");
    //    return false;
    //}

    unsigned short *srcptr = reinterpret_cast<unsigned short *>(surface->Lock());
    if (!srcptr) {
        DEBUG_ERROR("DirectX9 - Failed to lock game surface!\n");
        D3D_SURFACE_CAST(PrimarySurface)->Unlock();
        return false;
    }

    std::memcpy(lock_rect.pBits, srcptr, surface->Get_Width() * surface->Get_Height() * surface->Get_Bytes_Per_Pixel());
    //std::memcpy(dstptr, srcptr, surface->Get_Width() * surface->Get_Height() * surface->Get_Bytes_Per_Pixel());

    D3D_Write_Surface_Data_To_File("SRC.PNG", surface);
    D3D_Write_Surface_Data_To_File("DST.PNG", D3D_SURFACE_CAST(PrimarySurface));

    if (!surface->Unlock()) {
        DEBUG_ERROR("DirectX9 - Failed to unlock game surface!\n");
        D3D_SURFACE_CAST(PrimarySurface)->Unlock();
        return false;
    }
    //if (!D3D_SURFACE_CAST(PrimarySurface)->Unlock()) {
    //    DEBUG_ERROR("DirectX9 - Failed to unlock destination surface!\n");
    //    return false;
    //}
    if (!D3D_Unlock_Surface(D3D_SURFACE_CAST(PrimarySurface)->Get_Video_Surface_Ptr())) {
        DEBUG_ERROR("DirectX9 - Failed to unlock destination surface!\n");
        return false;
    }

    RECT src_rect;
    src_rect.left = 0;
    src_rect.top = 0;
    src_rect.right = surface->Get_Width();
    src_rect.bottom = surface->Get_Height();

    RECT dest_rect;
    dest_rect.left = 0;
    dest_rect.top = 0;
    dest_rect.right = surface->Get_Width();
    dest_rect.bottom = surface->Get_Height();

    LPDIRECT3DSURFACE9 backsurfaceptr;

    HRESULT res = Direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backsurfaceptr);
    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to get back buffer surface! Error code %x!\n", res);
        return false;
    }

    D3DTEXTUREFILTERTYPE d3dtft = D3DTEXF_NONE;
    Direct3DDevice->StretchRect(D3D_SURFACE_CAST(PrimarySurface)->Get_Video_Surface_Ptr(), &src_rect, backsurfaceptr, &dest_rect, d3dtft);

    // Present the backbuffer contents to the display.
    Direct3DDevice->Present(&src_rect, &dest_rect, nullptr, nullptr);

    return true;
}

HDC D3D_Primary_Get_DC()
{
    return D3D_SURFACE_CAST(PrimarySurface)->Get_DC();
}

bool D3D_Primary_Release_DC(HDC hDC)
{
    return D3D_SURFACE_CAST(PrimarySurface)->Release_DC(hDC);
}

void D3D_Flip_Primary()
{
    //D3D_Flip(PrimarySurface);
}

void D3D_Flip_Hidden()
{
    //D3D_Flip(HiddenSurface);
}

void D3D_Focus_Loss()
{
    Focus_Loss();
}

static void D3D_Focus_Restore()
{
    Focus_Restore();
}





/**
 *  These functions allow us to replace any calls with the inlined DSurface
 *  constructors and replace them with VirtualSurface's.
 */
static XSurface *D3D_Placement_Surface_Constructor_Patch(XSurface *surface, int dummy, int width, int height, bool sys_mem)
{
    return new (surface) D3DSurface(width, height);
}

static XSurface * __cdecl D3D_Create_Surface(int width, int height)
{
    return new D3DSurface(width, height);
}

static XSurface * __cdecl D3D_Create_Surface(int width, int height, bool sys_mem)
{
    return new D3DSurface(width, height);
}

static XSurface * __cdecl D3D_Placement_Create_Surface(XSurface *surface, int width, int height, bool sys_mem)
{
    return new (surface) D3DSurface(width, height);
}

static XSurface * __cdecl D3D_Placement_Destory_Surface(XSurface *surface)
{
    delete (surface);
}







DECLARE_PATCH(_Focus_Loss_Patch)
{
    D3D_Focus_Loss();

    JMP(0x00685B67);
}

DECLARE_PATCH(_Focus_Restore_Patch)
{
    D3D_Focus_Restore();

    JMP(0x00685B67);
}

/**
 *  Flip primary surface to screen when drawing movie frame.
 *
 *  @author: CCHyper
 */
DECLARE_PATCH(_MovieClass_Blit_Update_Window_Patch_1)
{
    PrimarySurface->Copy_From(
        CurrentMovie->StretchRect, *CurrentMovie->MovieSurface, CurrentMovie->VideoRect);

    D3D_Flip_Primary();

    JMP(0x005640D3);
}

DECLARE_PATCH(_MovieClass_Blit_Update_Window_Patch_2)
{
    PrimarySurface->Copy_From(
        CurrentMovie->StretchRect, *CurrentMovie->MovieSurface, CurrentMovie->VideoRect);

    D3D_Flip_Primary();

    JMP(0x0056478D);
}

/**
 *  Adds a flip call to the end of GScreenClass::Blit.
 */
DECLARE_PATCH(_GScreenClass_Blit_Flip)
{
    D3D_Flip_Primary();

    _asm { pop esi }
    _asm { add esp, 0x5C }
    _asm { ret 4 }
}

/**
 *
 */
DECLARE_PATCH(_GScreenClass_Blit_ClientToScreen_Patch)
{
    LEA_STACK_STATIC(RECT*, rect, esp, 0x50);

    GetClientRect(MainWindow, rect);

    ClientToScreen(MainWindow, (LPPOINT)&rect);
    ClientToScreen(MainWindow, (LPPOINT)&rect->right);





    //_asm { mov [esp+0x50], 0 } // rect.left
    //_asm { mov [esp+0x54], 0 } // rect.top

    JMP(0x004B9744);
}
static BSurface *__cdecl Create_Surface(int width, int height, bool sys_mem)
{
    return new BSurface(width, height, 2);
}

// TODO: Needs testing.
DECLARE_PATCH(_6_Create_Surface_Patch) // RadarClass compute something?
{
    GET_STACK_STATIC(XSurface *, rsurf, esi, 0x1228);
    static XSurface *surface;

    surface = Create_Surface(rsurf->Width, rsurf->Height, true);
    _asm { mov eax, surface }

    JMP_REG(edx, 0x005B9CE8);
}




/**
 *  Flip primary surface to screen when drawing movie frame.
 *
 *  @author: CCHyper
 */








DECLARE_PATCH(_OwnerDraw_DialogProc_Update_Window_Patch_Return_0)
{
    mD3D_Flip(PrimarySurface);

    _asm { xor eax, eax }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x2E8 }
    _asm { ret 0x10 }
}


DECLARE_PATCH(_OwnerDraw_DialogProc_Update_Window_Patch_Return_1)
{
    mD3D_Flip(PrimarySurface);

    _asm { mov eax, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x2E8 }
    _asm { ret 0x10 }
}

static XSurface *_Placement_Surface_Constructor_Patch(XSurface *surface, int dummy, int width, int height, bool sys_mem)
{
    return new (surface) BSurface(width, height, 2);
}

DECLARE_PATCH(_OwnerDraw_DialogProc_Update_Window_Patch_Return_Var)
{
    mD3D_Flip(PrimarySurface);

    _asm { mov eax, [esp + 0x1C] }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x2E8 }
    _asm { ret 0x10 }
}







void D3D_Hooks()
{
    // Removes Debug_Windowed check and calls Set_Video_Mode.
    Patch_Jump(0x006016B8, 0x006015FC);

    // Remove the 16bit display depth check.
    Patch_Jump(0x0060142F, 0x00601467);

    Patch_Jump(0x005B9CAF, &_6_Create_Surface_Patch); // radar class compute?

    Patch_Call(0x00472950, &_Placement_Surface_Constructor_Patch); // Check_Overlapped_Blit_Capability
    Patch_Call(0x00508973, &_Placement_Surface_Constructor_Patch); // Debug_Motion_Capture
    Patch_Call(0x005AD02C, &_Placement_Surface_Constructor_Patch); // creates palleted map preview?


    Patch_Jump(0x00564061, &_MovieClass_Blit_Update_Window_Patch_1);            // DONE
    Patch_Jump(0x0056471A, &_MovieClass_Blit_Update_Window_Patch_2);            // DONE
    
    Patch_Jump(0x004B9A68, &_GScreenClass_Blit_Flip);
    Patch_Jump(0x004B9701, &_GScreenClass_Blit_ClientToScreen_Patch);

    //Patch_Jump(0x00571116, &_MSEngine_Blit_Update_Window_Patch);
    //Patch_Jump(0x005711F5, &_MSEngine_Draw_Update_Window_Patch);

    //Patch_Jump(0x005A0B98, &_Owner_Draw_Loop_Blit_Patch);

    Patch_Jump(0x00592356, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_1);
    Patch_Jump(0x0059264F, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_0);
    Patch_Jump(0x005926D8, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_1);
    Patch_Jump(0x00592802, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_1);
    Patch_Jump(0x005944EF, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_1);
    Patch_Jump(0x005944FE, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_Var);

    //Patch_Jump(0x005F3C61, &_SidebarClass_Blit_Sidebar_Update_Window_Patch);
    /**
     *  Patch in the D3D intercept functions.
     */
    Patch_Call(0x0050AD34, &Set_Video_Mode_Intercept);
    Patch_Call(0x006015E6, &Set_Video_Mode_Intercept);
    Patch_Call(0x0060161C, &Set_Video_Mode_Intercept);
    Patch_Call(0x00601716, &Set_Video_Mode_Intercept);
    Patch_Call(0x00601790, &Set_Video_Mode_Intercept);
    Patch_Call(0x005FF7B4, &Reset_Video_Mode_Intercept);
    Patch_Call(0x00472FCD, &Check_Overlapped_Blit_Capability_Intercept);
    Patch_Call(0x006013C8, &Prep_Renderer_Intercept);
    Patch_Call(0x006016B3, &Prep_Renderer_Intercept);
    Patch_Call(0x004EC1C7, &Wait_Blit_Intercept);
    Patch_Call(0x0050953F, &Wait_Blit_Intercept);
    Patch_Call(0x00509781, &Wait_Blit_Intercept);
    Patch_Call(0x0050AC87, &Wait_Blit_Intercept);
    Patch_Call(0x00571253, &Wait_Blit_Intercept);
    Patch_Call(0x0050B05D, &Allocate_Surfaces_Intercept);
    Patch_Call(0x00601543, &Allocate_Surfaces_Intercept);
    Patch_Call(0x0050AD5A, &Create_Primary_Intercept);
    Patch_Call(0x0050AF41, &Create_Primary_Intercept);
    Patch_Call(0x0060141E, &Create_Primary_Intercept);

    Patch_Jump(0x0048B2E0, &D3DSurface::Get_DC);
    Patch_Jump(0x0048B320, &D3DSurface::Release_DC);
}
