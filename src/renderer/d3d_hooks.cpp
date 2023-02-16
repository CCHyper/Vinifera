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
#include "virtualsurface.h"
#include "options.h"
#include "optionsext.h"
#include "movie.h"
#include "filepng.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"

#include <d3d9.h>
#pragma comment (lib, "d3d9.lib")


LPDIRECT3D9 Direct3D = nullptr;
LPDIRECT3DDEVICE9 Direct3DDevice = nullptr;
LPDIRECT3DSURFACE9 Direct3DPrimarySurface = nullptr;
LPDIRECT3DSURFACE9 Direct3DBackSurface = nullptr;



// Cast DSurface pointer to VirtualSurface.
#define VIRTUAL_SURFACE_ASSIGN(surface, ptr) surface = reinterpret_cast<DSurface *>(ptr)
#define VIRTUAL_SURFACE_CAST(surface) reinterpret_cast<VirtualSurface *>(surface)



void __cdecl D3D_Write_Surface_Data_To_File(const char *filename, XSurface *surface)
{
#if 1 // #ifndef NDEBUG
    Write_PNG_File(&RawFileClass(filename), *surface, &GamePalette);
#endif
}



bool D3D_Create_Primary(XSurface *primary_surface)
{
    HRESULT res;

    // Create primary surface.
    res = Direct3DDevice->CreateOffscreenPlainSurface(
        primary_surface->Get_Width(),
        primary_surface->Get_Height(),
        D3DFMT_R5G6B5,
        D3DPOOL_DEFAULT,
        &Direct3DPrimarySurface,
        nullptr);

    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to create primary surface! Error code %x!", res);
        return false;
    }

    // Create back surface.
#if 0
    res = Direct3DDevice->CreateOffscreenPlainSurface(
        primary_surface->Get_Width(),
        primary_surface->Get_Height(),
        D3DFMT_R5G6B5,
        D3DPOOL_DEFAULT,
        &Direct3DBackSurface,
        nullptr);
    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to create back buffer surface! Error code %x!", res);
        return false;
    }
#else
    res = Direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &Direct3DBackSurface);
    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to get back buffer surface! Error code %x!", res);
        return false;
    }
#endif

    Direct3DDevice->SetRenderTarget(0, Direct3DPrimarySurface);

    return true;
}

bool D3D_Clear_Screen(bool present = false)
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
        DEBUG_ERROR("Direct3D - Failed to lock back surface! Error code %x!", res);
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
        DEBUG_ERROR("Direct3D - Failed to unlock back surface! Error code %x!", res);
        return false;
    }

    return true;
}

bool D3D_Flip(XSurface *surface)
{
    D3D_Clear_Screen();

    D3DLOCKED_RECT lock_rect;

    if (!D3D_Lock_Surface(Direct3DPrimarySurface, lock_rect, false, false)) {
        DEBUG_ERROR("Direct3D - Failed to lock destination surface!\n");
        return false;
    }

    //unsigned short *dstptr = reinterpret_cast<unsigned short *>(VIRTUAL_SURFACE_CAST(PrimarySurface)->Lock());
    //if (!dstptr) {
    //    DEBUG_ERROR("Direct3D - Failed to lock game surface!\n");
    //    return false;
    //}

    unsigned short *srcptr = reinterpret_cast<unsigned short *>(surface->Lock());
    if (!srcptr) {
        DEBUG_ERROR("Direct3D - Failed to lock game surface!\n");
        D3D_Unlock_Surface(Direct3DPrimarySurface);
        return false;
    }

    std::memcpy(lock_rect.pBits, srcptr, surface->Get_Width() * surface->Get_Height() * surface->Get_Bytes_Per_Pixel());
    //std::memcpy(dstptr, srcptr, surface->Get_Width() * surface->Get_Height() * surface->Get_Bytes_Per_Pixel());

    //D3D_Write_Surface_Data_To_File("SRC.PNG", surface);
    //D3D_Write_Surface_Data_To_File("DST.PNG", VIRTUAL_SURFACE_CAST(PrimarySurface));

    if (!surface->Unlock()) {
        DEBUG_ERROR("Direct3D - Failed to unlock game surface!\n");
        VIRTUAL_SURFACE_CAST(PrimarySurface)->Unlock();
        return false;
    }
    //if (!VIRTUAL_SURFACE_CAST(PrimarySurface)->Unlock()) {
    //    DEBUG_ERROR("Direct3D - Failed to unlock destination surface!\n");
    //    return false;
    //}
    if (!D3D_Unlock_Surface(Direct3DPrimarySurface)) {
        DEBUG_ERROR("Direct3D - Failed to unlock destination surface!\n");
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

    //LPDIRECT3DSURFACE9 backsurfaceptr;
    //
    //HRESULT res = Direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backsurfaceptr);
    //if (FAILED(res)) {
    //    DEBUG_ERROR("Direct3D - Failed to get back buffer surface! Error code %x!\n", res);
    //    return false;
    //}

    //D3DTEXF_NONE = 0,      // filtering disabled (valid for mip filter only)
    //D3DTEXF_POINT = 1,     // nearest
    //D3DTEXF_LINEAR = 2,    // linear interpolation

    D3DTEXTUREFILTERTYPE d3dtft = (D3DTEXTUREFILTERTYPE)OptionsExtension->ScalingMode;
    Direct3DDevice->StretchRect(Direct3DPrimarySurface, &src_rect, Direct3DBackSurface, &dest_rect, d3dtft);

    // Present the backbuffer contents to the display.
    Direct3DDevice->Present(&src_rect, &dest_rect, nullptr, nullptr);

    return true;
}

HDC D3D_Primary_Get_DC()
{
    HDC hDC;

    HRESULT res = Direct3DPrimarySurface->GetDC(&hDC);
    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to get DC! Error code %x!\n", res);
        return nullptr;
    }

    return hDC;
}

bool D3D_Primary_Release_DC(HDC hDC)
{
    HRESULT res = Direct3DPrimarySurface->ReleaseDC(hDC);
    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to release DC! Error code %x!\n", res);
        return false;
    }

    return true;
}

void D3D_Flip_Primary()
{
    D3D_Flip(PrimarySurface);
}

void D3D_Flip_Hidden()
{
    D3D_Flip(HiddenSurface);
}

void D3D_Focus_Loss()
{
    Focus_Loss();
}

static void D3D_Focus_Restore()
{
    Focus_Restore();
}



static bool Allocate_Surfaces_Intercept(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect)
{
    DEBUG_INFO("Allocating game surfaces...\n");

    /**
     *  Delete existing surface instances.
     */
    if (AlternateSurface) {
        DEBUG_INFO("Deleting AlternateSurface\n");
        delete AlternateSurface;
        AlternateSurface = nullptr;
    }
    if (HiddenSurface) {
        DEBUG_INFO("Deleting HiddenSurface\n");
        delete HiddenSurface;
        HiddenSurface = nullptr;
    }
    if (CompositeSurface) {
        DEBUG_INFO("Deleting CompositeSurface\n");
        delete CompositeSurface;
        CompositeSurface = nullptr;
    }
    if (TileSurface) {
        DEBUG_INFO("Deleting TileSurface\n");
        delete TileSurface;
        TileSurface = nullptr;
    }
    if (SidebarSurface) {
        DEBUG_INFO("Deleting SidebarSurface\n");
        delete SidebarSurface;
        SidebarSurface = nullptr;
    }

    /**
     *  Create new instances of the surfaces using the Virtual interface.
     */
    if (common_rect->Width > 0 && common_rect->Height > 0) {
        VIRTUAL_SURFACE_ASSIGN(AlternateSurface, new VirtualSurface(common_rect->Width, common_rect->Height));
        VIRTUAL_SURFACE_CAST(AlternateSurface)->Clear();
        DEBUG_INFO("AlternateSurface created (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (common_rect->Width > 0 && common_rect->Height > 0) {
        VIRTUAL_SURFACE_ASSIGN(HiddenSurface, new VirtualSurface(common_rect->Width, common_rect->Height));
        VIRTUAL_SURFACE_CAST(HiddenSurface)->Clear();
        DEBUG_INFO("HiddenSurface created (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (composite_rect->Width > 0 && composite_rect->Height > 0) {
        VIRTUAL_SURFACE_ASSIGN(CompositeSurface, new VirtualSurface(composite_rect->Width, composite_rect->Height));
        VIRTUAL_SURFACE_CAST(CompositeSurface)->Clear();
        DEBUG_INFO("CompositeSurface created (%dx%d)\n", composite_rect->Width, composite_rect->Height);
    }

    if (tile_rect->Width > 0 && tile_rect->Height > 0) {
        VIRTUAL_SURFACE_ASSIGN(TileSurface, new VirtualSurface(tile_rect->Width, tile_rect->Height));
        VIRTUAL_SURFACE_CAST(TileSurface)->Clear();
        DEBUG_INFO("TileSurface created (%dx%d)\n", tile_rect->Width, tile_rect->Height);
    }

    if (sidebar_rect->Width > 0 && sidebar_rect->Height > 0) {
        VIRTUAL_SURFACE_ASSIGN(SidebarSurface, new VirtualSurface(sidebar_rect->Width, sidebar_rect->Height));
        VIRTUAL_SURFACE_CAST(SidebarSurface)->Clear();
        DEBUG_INFO("SidebarSurface created (%dx%d)\n", sidebar_rect->Width, sidebar_rect->Height);
    }

    return true;
}

static XSurface *Create_Primary_Intercept(XSurface **backbuffer_surface)
{
    // Force all new renderers to be created as a window.
    OptionsExtension->IsWindowed = true;
    OptionsExtension->IsBorderlessWindow = false;
    Debug_Windowed = OptionsExtension->IsWindowed;



    /**
     *  Create a virtual surface keep the game happy.
     */
    VirtualSurface *primary_surface = new VirtualSurface(Options.ScreenWidth, Options.ScreenHeight);
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

    /**
     *  Create the primary Direct3D surface.
     */
    if (!D3D_Create_Primary(primary_surface)) {
        return false;
    }

    Direct3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

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

    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferCount = D3DFMT_UNKNOWN;

    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    DWORD behaviour_flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    if (false) {
        behaviour_flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

    HRESULT res = Direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, behaviour_flags, &d3dpp, &Direct3DDevice);
    if (FAILED(res)) {
        DEBUG_ERROR("Direct3D - Failed to create Direct3D device! Error code %x!\n", res);
        return false;
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










/**
 *  These functions allow us to replace any calls with the inlined DSurface
 *  constructors and replace them with VirtualSurface's.
 */
static XSurface *D3D_Placement_Surface_Constructor_Patch(XSurface *surface, int dummy, int width, int height, bool sys_mem)
{
    return new (surface) VirtualSurface(width, height);
}

static XSurface * __cdecl D3D_Create_Surface(int width, int height)
{
    return new VirtualSurface(width, height);
}

static XSurface * __cdecl D3D_Create_Surface(int width, int height, bool sys_mem)
{
    return new VirtualSurface(width, height);
}

static XSurface * __cdecl D3D_Placement_Create_Surface(XSurface *surface, int width, int height, bool sys_mem)
{
    return new (surface) VirtualSurface(width, height);
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





void D3D_Hooks()
{
    // Removes Debug_Windowed check and calls Set_Video_Mode.
    Patch_Jump(0x006016B8, 0x006015FC);

    // Remove the 16bit display depth check.
    Patch_Jump(0x0060142F, 0x00601467);



    Patch_Jump(0x00564061, &_MovieClass_Blit_Update_Window_Patch_1);            // DONE
    Patch_Jump(0x0056471A, &_MovieClass_Blit_Update_Window_Patch_2);            // DONE

    Patch_Jump(0x004B9A68, &_GScreenClass_Blit_Flip);
    Patch_Jump(0x004B9701, &_GScreenClass_Blit_ClientToScreen_Patch);


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

    Patch_Jump(0x0048B2E0, &VirtualSurface::Get_DC);
    Patch_Jump(0x0048B320, &VirtualSurface::Release_DC);
}
