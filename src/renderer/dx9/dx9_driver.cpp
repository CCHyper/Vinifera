/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          DIRECTX9_DRIVER.H
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

#if defined(DX9_RENDERER)

#include "dx9_driver.h"
#include "vinifera_globals.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "tibsun_resource.h"
#include "optionsext.h"
#include "resource.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <Windows.h>
#include <commctrl.h>

#include "directx_window.h"

//#include <d3d9math.h>
//#include <d3dx9.h>
//#include <dxerr9.h>

#pragma comment (lib, "d3d9.lib")
//#pragma comment (lib, "d3dx9.lib")
//#pragma comment (lib, "dxerr9.lib")


LPDIRECT3D9 Direct3D = nullptr;
LPDIRECT3DDEVICE9 Direct3DDevice = nullptr;
LPDIRECT3DSURFACE9 Direct3DPrimarySurface = nullptr;
LPDIRECT3DSURFACE9 Direct3DBackSurface = nullptr;
LPDIRECT3DVERTEXBUFFER9 Direct3DVertexBuffer = nullptr;


// Our custom FVF, which describes our custom vertex structure.
#ifdef SHOW_HOW_TO_USE_TCI
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#else
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#endif

// A structure for our custom vertex type. We added texture coordinates.
struct CUSTOMVERTEX
{
    //D3DXVECTOR3 position; // The coordinates the vertex is located at.
    D3DCOLOR color;       // The colour/alpha value of the vertex in standard 32-bit AARRGGBB format.
#ifndef SHOW_HOW_TO_USE_TCI
    FLOAT tu, tv;         // The texture coordinates of the vertex.
#endif
};



/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Close_Window(bool force)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Destroy_Window(bool force)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Show_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Hide_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Minimize_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Maximize_Window()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX9VideoDriver::Toggle_Fullscreen()
{
    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Focus_Loss()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Focus_Restore()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Set_Cursor_Clip()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Clear_Cursor_Clip()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX9VideoDriver::Update_Screen(XSurface *surface, Rect *src_rect, Rect *dest_rect)
{
    return false;
}


/**
 *  Clears the backbuffer to black.
 *
 *  @author: CCHyper
 */
bool DirectX9VideoDriver::Clear_Screen(bool present)
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
bool DirectX9VideoDriver::Set_Video_Mode(HWND hWnd, int width, int height, int bits_per_pixel)
{
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = IsWindowed;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferCount = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    HRESULT res = Direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &Direct3DDevice);
    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to create Direct3D device! Error code %x!", res);
        return false;
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Reset_Video_Mode()
{
    if (Direct3DVertexBuffer) {
        Direct3DVertexBuffer->Release();
        Direct3DVertexBuffer = nullptr;
    }
    if (Direct3DBackSurface) {
        Direct3DBackSurface->Release();
        Direct3DBackSurface = nullptr;
    }
    if (Direct3DPrimarySurface) {
        Direct3DPrimarySurface->Release();
        Direct3DPrimarySurface = nullptr;
    }
    if (Direct3DDevice) {
        Direct3DDevice->Release();
        Direct3DDevice = nullptr;
    }
    if (Direct3D) {
        Direct3D->Release();
        Direct3D = nullptr;
    }
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Release(HWND hWnd)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX9VideoDriver::Prep_Renderer(HWND hWnd)
{
    Direct3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!Direct3D) {
        return false;
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Check_Overlapped_Blit_Capability()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Wait_Blit()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
#include "bsurface.h"
extern void __cdecl Write_Surface_Data_To_File(const char *filename, XSurface *surface);
bool DirectX9VideoDriver::Flip(XSurface *surface)
{
    Direct3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);







#if 1
    D3DLOCKED_RECT lock_rect;

    if (!Lock_D3D_Surface(Direct3DPrimarySurface, lock_rect, false)) {
        DEBUG_ERROR("DirectX9 - Failed to lock destination surface!\n");
        return false;
    }

    unsigned short *srcptr = reinterpret_cast<unsigned short *>(surface->Lock());
    if (!srcptr) {
        DEBUG_ERROR("DirectX9 - Failed to lock game surface!\n");
        return false;
    }

#if 0
    static int _counter = 0;
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "DX9_%04d.png", _counter++);
    Write_Surface_Data_To_File(buffer, surface);
#endif

#if 0
    static int _counter1 = 0;
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "DX9_1_%04d.png", _counter1++);
    BSurface *s1 = new BSurface(surface->Width, surface->Height, 4, lock_rect.pBits);
    Write_Surface_Data_To_File(buffer, s1);
    delete s1;
#endif

#if 1
    unsigned long *dstptr = reinterpret_cast<unsigned long *>(lock_rect.pBits);
    for (int i = 0; i < (surface->Width * surface->Height); ++i) {
        unsigned short src_pixel = *(srcptr++);

        // Red
        int r = (src_pixel >> 11) & 0x1F;
        r = ((r << 3) | (r & 7)) << 16;

        // Green
        int g = (src_pixel >> 5) & 0x3F;
        g = ((g << 2) | (g & 3)) << 8;

        // Blue
        int b = src_pixel & 0x1F;
        b = (b << 3) | (b & 7);

        *dstptr++ = (r | g | b);
    }
#else
    unsigned char *dstptr = reinterpret_cast<unsigned char *>(lock_rect.pBits);
    for (int i = 0; i < (surface->Width * surface->Height); ++i) {
        unsigned short s = *(srcptr++);
        unsigned char r = (s & 0xF800) >> 11; // Extract the 5 R bits
        unsigned char g = (s & 0x07E0) >> 5;  // Extract the 6 G bits
        unsigned char b = (s & 0x001F);       // Extract the 5 B bits
        *dstptr++ = 255;     // Set the 8 A bit.
        *dstptr++ = b;       // Set the 8 B bit.
        *dstptr++ = g;       // Set the 8 G bit.
        *dstptr++ = r;       // Set the 8 R bit.
    }
#endif

#if 0
    static int _counter2 = 0;
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "DX9_2_%04d.png", _counter2++);
    BSurface *s2 = new BSurface(surface->Width, surface->Height, 4, lock_rect.pBits);
    Write_Surface_Data_To_File(buffer, s2);
    delete s2;
#endif

    /**
     *  Finished with the surfaces, unlock them.
     */
    if (!surface->Unlock()) {
        DEBUG_ERROR("DirectX9 - Failed to unlock game surface!\n");
        return false;
    }
    if (!Unlock_D3D_Surface(Direct3DPrimarySurface)) {
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
    dest_rect.right = WindowWidth;
    dest_rect.bottom = WindowHeight;

    D3DTEXTUREFILTERTYPE d3dtft = D3DTEXF_NONE;
    Direct3DDevice->StretchRect(Direct3DPrimarySurface, &src_rect, Direct3DBackSurface, &dest_rect, d3dtft);

    // Present the backbuffer contents to the display.
    Direct3DDevice->Present(&src_rect, &dest_rect, nullptr, nullptr);

#elif 0

    // Vertex Test

    // A structure for our custom vertex type
    struct CUSTOMVERTEX
    {
        FLOAT x, y, z, rhw; // The transformed position for the vertex
        DWORD color;        // The vertex color
    };

    // Our custom FVF, which describes our custom vertex structure
    #define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

    // Initialize three vertices for rendering a triangle
    CUSTOMVERTEX vertices[] =
    {
        { 150.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
        { 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
        {  50.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
    };

    // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if (FAILED(Direct3DDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &Direct3DVertexBuffer, nullptr))) {
        return false;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
    VOID *pVertices;
    if (FAILED(Direct3DVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertices, 0))) {
        return false;
    }
    std::memcpy(pVertices, vertices, sizeof(vertices));
    Direct3DVertexBuffer->Unlock();



    // Clear the backbuffer to a blue color.
    Direct3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    // Begin the scene
    if (SUCCEEDED(Direct3DDevice->BeginScene())) {

        // Draw the triangles in the vertex buffer. This is broken into a few
        // steps. We are passing the vertices down a "stream", so first we need
        // to specify the source of that stream, which is our vertex buffer. Then
        // we need to let D3D know what vertex shader to use. Full, custom vertex
        // shaders are an advanced topic, but in most cases the vertex shader is
        // just the FVF, so that D3D knows what type of vertices we are dealing
        // with. Finally, we call DrawPrimitive() which does the actual rendering
        // of our geometry (in this case, just one triangle).
        Direct3DDevice->SetStreamSource(0, Direct3DVertexBuffer, 0, sizeof(CUSTOMVERTEX));
        Direct3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
        Direct3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

        // End the scene
        Direct3DDevice->EndScene();

    }

    // Present the backbuffer contents to the display
    Direct3DDevice->Present(nullptr, nullptr, nullptr, nullptr);

#endif

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX9VideoDriver::Frame_Limiter(bool force_blit)
{
    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void DirectX9VideoDriver::Shutdown()
{
    Reset_Video_Mode();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX9VideoDriver::Internal_Create_Primary(Surface *primary_surface, XSurface **backbuffer_surface)
{
    HRESULT res;

    // Create primary surface.
    res = Direct3DDevice->CreateOffscreenPlainSurface(
        primary_surface->Get_Width(),
        primary_surface->Get_Height(),
        D3DFMT_X8R8G8B8,
        D3DPOOL_DEFAULT,
        &Direct3DPrimarySurface,
        nullptr);

    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to create primary surface! Error code %x!", res);
        return false;
    }

    // Create back surface.
#if 0
    res = Direct3DDevice->CreateOffscreenPlainSurface(
        primary_surface->Get_Width(),
        primary_surface->Get_Height(),
        D3DFMT_X8R8G8B8,
        D3DPOOL_DEFAULT,
        &Direct3DBackSurface,
        nullptr);
    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to create back buffer surface! Error code %x!", res);
        return false;
    }
#else
    res = Direct3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &Direct3DBackSurface);
    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to get back buffer surface! Error code %x!", res);
        return false;
    }
#endif

    Direct3DDevice->SetRenderTarget(0, Direct3DPrimarySurface);

    // Set vertex shader.
    Direct3DDevice->SetVertexShader(nullptr);
    Direct3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

    // Create vertex buffer.
    Direct3DDevice->CreateVertexBuffer(sizeof(CUSTOMVERTEX) * 4, 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &Direct3DVertexBuffer, nullptr);
    Direct3DDevice->SetStreamSource(0, Direct3DVertexBuffer, 0, sizeof(CUSTOMVERTEX));

    Direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);               // Turn off culling.
    Direct3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);                      // Turn off D3D lighting.
    Direct3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);                        // Turn on the zbuffer.
    Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);               // Turn on alpha blending.
    Direct3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    Direct3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    Direct3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    CUSTOMVERTEX* pVertices;
    if (FAILED(Direct3DVertexBuffer->Lock(0, 0, (void**)&pVertices, 0))) {
        return false;
    }

#if 0
    for (int index = 0; index < 50; ++index)
    {
        FLOAT theta = (2 * D3DX_PI * index) / (50 - 1);

        pVertices[2 * index + 0].position = D3DXVECTOR3(std::sinf(theta), -1.0f, std::cosf(theta));
        pVertices[2 * index + 0].color = 0xFFFFFFFF;
#ifndef SHOW_HOW_TO_USE_TCI
        pVertices[2 * index + 0].tu = ((FLOAT)index) / (50 - 1);
        pVertices[2 * index + 0].tv = 1.0f;
#endif

        pVertices[2 * index + 1].position = D3DXVECTOR3(std::sinf(theta), 1.0f, std::cosf(theta));
        pVertices[2 * index + 1].color = 0xFF808080;
#ifndef SHOW_HOW_TO_USE_TCI
        pVertices[2 * index + 1].tu = ((FLOAT)index) / (50 - 1);
        pVertices[2 * index + 1].tv = 0.0f;
#endif
    }

    if (FAILED(Direct3DVertexBuffer->Unlock())) {
        return false;
    }
#endif

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX9VideoDriver::Internal_Create_Window(HINSTANCE hInstance)
{
    return DirectX_Create_Window(this, hInstance);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX9VideoDriver::Lock_D3D_Surface(LPDIRECT3DSURFACE9 surface, D3DLOCKED_RECT &lock_rect, bool discard, bool readonly)
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

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool DirectX9VideoDriver::Unlock_D3D_Surface(LPDIRECT3DSURFACE9 surface)
{
    ASSERT(surface != nullptr);

    HRESULT res = surface->UnlockRect();

    if (FAILED(res)) {
        DEBUG_ERROR("DirectX9 - Failed to unlock back surface! Error code %x!", res);
        return false;
    }

    return true;
}


#endif
