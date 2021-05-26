/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SDL_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         SDL2 Surface
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
#pragma once

#include "sdl_hooks.h"
#include "sdlsurface.h"
#include "sdl_createwindow.h"
#include "sdl_globals.h"
#include "sdl_functions.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "dsurface.h"
#include "options.h"
#include "rawfile.h"
#include "ini.h"
#include "filepcx.h"
#include "textprint.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


DECLARE_PATCH(_WinMain_Create_Main_Window_Patch_1)
{
    static int width = 640;
    static int height = 400;
    GET_STACK_STATIC(int, command_show, ebp, 0x14);
    GET_STACK_STATIC(HINSTANCE, hInstance, ebp, 0x8);

    if (UseSDL2) {
        /**
         *  Create the window using SDL2.
         */
        SDL_Create_Main_Window(hInstance, width, height);

        SDL_Set_Video_Mode(MainWindow, width, height);

    } else {

        /**
         *  Create the window using the Windows API.
         */
        Create_Main_Window(hInstance, command_show, width, height);
    }

    JMP(0x006013B0);
}


DECLARE_PATCH(_WinMain_Create_Main_Window_Patch_2)
{
    static int width = 640;
    static int height = 400;
    width = Options.ScreenWidth;
    height = Options.ScreenHeight;
    GET_STACK_STATIC(int, command_show, ebp, 0x14);
    GET_STACK_STATIC(HINSTANCE, hInstance, ebp, 0x8);

    if (UseSDL2) {
        /**
         *  Create the window using SDL2.
         */
        SDL_Create_Main_Window(hInstance, width, height);

        SDL_Set_Video_Mode(MainWindow, width, height);

    } else {

        /**
         *  Create the window using the Windows API.
         */
        Create_Main_Window(hInstance, command_show, width, height);
    }

    JMP(0x0060169B);
}


DECLARE_PATCH(_WinMain_Prep_Direct_Draw_Patch_1)
{
    if (!UseSDL2) {
        Prep_Direct_Draw();
    }
    JMP(0x006013CD);
}


DECLARE_PATCH(_WinMain_Prep_Direct_Draw_Patch_2)
{
    if (!UseSDL2) {
        Prep_Direct_Draw();
    }
    JMP(0x006016B8);
}


/**
 *  Patch to update the SDL window surface after clear.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_WinMain_Clear_Primary_Surface_Patch)
{
    PrimarySurface->Fill(COLOR_TBLACK);
    if (UseSDL2) {
        SDL_Update_Screen(PrimarySurface);
    }
    JMP(0x006014CC);
}


DECLARE_PATCH(_WinMain_16Bit_Color_Check_Patch)
{
//    if (!UseSDL2) {
//    }
    JMP(0x00601467);
}


static bool _Set_Video_Mode(HWND hWnd, int w, int h, int bits_per_pixel)
{
    if (UseSDL2) {
        return SDL_Set_Video_Mode(hWnd, w, h);
    } else {
        return Set_Video_Mode(hWnd, w, h, bits_per_pixel);
    }
}


static void _Reset_Video_Mode()
{
    if (UseSDL2) {
        SDL_Reset_Video_Mode();
    } else {
        Reset_Video_Mode();
    }
}


static XSurface *_Create_Primary(DSurface **backbuffer_surface)
{
    if (UseSDL2) {
        return SDLSurface::Create_Primary((SDLSurface **)backbuffer_surface);
    } else {
        return DSurface::Create_Primary((DSurface **)backbuffer_surface);
    }
}


static bool _Allocate_Surfaces(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect, bool alloc_hidden_surf)
{
    if (UseSDL2) {
        return SDL_Allocate_Surfaces(common_rect, composite_rect, tile_rect, sidebar_rect);
    } else {
        return Allocate_Surfaces(common_rect, composite_rect, tile_rect, sidebar_rect, alloc_hidden_surf);
    }
}


static void _Wait_Blit()
{
    if (!UseSDL2) {
        Wait_Blit();
    }
}


static void _Set_DD_Palette(void *rpalette)
{
    if (UseSDL2) {
        Set_SDL_Palette(rpalette);
    } else {
        Set_SDL_Palette(rpalette);
    }
}


// TODO: Rewrite this to not be abusing fastcall!
static HDC _Get_DC(DSurface *this_ptr)
{
    if (UseSDL2) {
        DEBUG_INFO("_Get_DC\n");
        return reinterpret_cast<SDLSurface *>(this_ptr)->Get_DC();
    } else {
        HDC hdc = nullptr;
        HRESULT ddrval = static_cast<DSurface *>(this_ptr)->VideoSurfacePtr->GetDC(&hdc);
        if (FAILED(ddrval)) {
            return nullptr;
        }
        ++static_cast<DSurface *>(this_ptr)->LockLevel;
        return hdc;
    }
}


// TODO: Rewrite this to not be abusing fastcall!
static BOOL _Release_DC(DSurface *this_ptr, int dummy, HDC hdc)
{
    if (UseSDL2) {
        DEBUG_INFO("_Release_DC\n");
        return reinterpret_cast<SDLSurface *>(this_ptr)->Release_DC(hdc);
    } else {
        HRESULT ddrval = static_cast<DSurface *>(this_ptr)->VideoSurfacePtr->ReleaseDC(hdc);
        if (FAILED(ddrval)) {
            return false;
        }
        if (static_cast<DSurface *>(this_ptr)->LockLevel > 0) {
            --static_cast<DSurface *>(this_ptr)->LockLevel;
        }
        return true;
    }
}

#include "tspp.h"
DEFINE_IMPLEMENTATION(bool _DSurface_Restore_Check(DSurface *), 0x0048B510);

static bool _Restore_Check(DSurface *this_ptr)
{
    if (UseSDL2) {
        return reinterpret_cast<SDLSurface *>(this_ptr)->Restore_Check();
    } else {
        return _DSurface_Restore_Check(static_cast<DSurface *>(this_ptr));
    }
}


DECLARE_PATCH(_GScreenClass_Do_Blit_SDL_Update_Window_Patch)
{
    if (UseSDL2) {
        SDL_Update_Screen(PrimarySurface);
    }

    _asm { test bl, bl }
    _asm { pop edi }
    _asm { pop ebp }
    _asm { pop ebx }
    JMP(0x004B9A47);
}


DECLARE_PATCH(_Main_Loop_SDL_Update_Window_Patch)
{
    if (UseSDL2) {
        SDL_Update_Screen(PrimarySurface);
        SDL_Delay(0);
    }

    _asm { setz al }
    _asm { pop edi }
    _asm { add esp, 0x38 }
    _asm { ret }
}


/**
 *  Flip hidden surface onto the primary SDL surface when drawing movie frame.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_MovieClass_Blit_SDL_Update_Window_Patch_1)
{
    // PrimarySurface (ecx) -> Copy_From
    _asm { mov eax, [edx+8] }
    _asm { call eax }

    //DEBUG_INFO("MovieClass::Blit(1) - Copying to PrimarySurface.\n");

    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebx }

    if (UseSDL2) {
        //static DSurface *_movie_surface;
        //_asm { mov eax, 0x00806E1C } // current movie ptr
        //_asm { mov eax, [eax+8] }
        //_asm { mov _movie_surface, eax }

        //static SDL_Rect src_rect;
        //src_rect.x = 0;
        //src_rect.y = 0;
        //src_rect.w = 640;
        //src_rect.h = 400;
        //static SDL_Rect dest_rect;
        //dest_rect.x = (Options.ScreenWidth/2)-640;
        //dest_rect.y = (Options.ScreenHeight/2)-400;
        //dest_rect.w = 640;
        //dest_rect.h = 400;
        SDL_Update_Screen(PrimarySurface/*, &src_rect, &dest_rect*/);
    }
    
    JMP(0x005640D3);
}


/**
 *  Flip hidden surface onto the primary SDL surface when drawing movie frame.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_MovieClass_Blit_SDL_Update_Window_Patch_2)
{
    // PrimarySurface (ecx) -> Copy_From
    _asm { mov eax, [edx+8] }
    _asm { call eax }

    //DEBUG_INFO("MovieClass::Blit(2) - Copying to PrimarySurface.\n");

    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebx }

    if (UseSDL2) {
        //static DSurface *_movie_surface;
        //_asm { mov eax, 0x00806E1C } // current movie ptr
        //_asm { mov eax, [eax+8] }
        //_asm { mov _movie_surface, eax }

        //static SDL_Rect src_rect;
        //src_rect.x = 0;
        //src_rect.y = 0;
        //src_rect.w = 640;
        //src_rect.h = 400;
        //static SDL_Rect dest_rect;
        //dest_rect.x = (Options.ScreenWidth/2)-640;
        //dest_rect.y = (Options.ScreenHeight/2)-400;
        //dest_rect.w = 640;
        //dest_rect.h = 400;
        SDL_Update_Screen(PrimarySurface/*, &src_rect, &dest_rect*/);
    }
    
    JMP(0x0056478D);
}


DECLARE_PATCH(_MSEngine_Blit_SDL_Update_Window_Patch)
{
    // PrimarySurface (ecx) -> Copy_From
    _asm { push eax }
    _asm { mov edx, [ecx] }
    _asm { mov eax, [edx+0x8] }
    _asm { call eax }

    DEBUG_INFO("MSEngine::Blit() - Copying to PrimarySurface.\n");

    if (UseSDL2) {

        //static RawFileClass file("SURFACE.PCX");
        //Write_PCX_File(&file, *HiddenSurface, nullptr);

        SDL_Update_Screen(PrimarySurface);
    }

    JMP(0x0057111C);
}

DECLARE_PATCH(_MSEngine_Draw_SDL_Update_Window_Patch)
{
    // PrimarySurface (ecx) -> Copy_From
    _asm { push edx }
    _asm { mov ecx, [ecx] }
    _asm { mov eax, [ecx+0x8] }
    _asm { call eax }

    DEBUG_INFO("MSEngine::Draw() - Copying to PrimarySurface.\n");

    if (UseSDL2) {

        //static RawFileClass file("SURFACE.PCX");
        //Write_PCX_File(&file, *HiddenSurface, nullptr);

        SDL_Update_Screen(PrimarySurface);
    }

    JMP(0x005711F8);
}


DECLARE_PATCH(_SidebarClass_Blit_Sidebar_SDL_Update_Window_Patch)
{
    if (UseSDL2) {

        //static RawFileClass file("SURFACE.PCX");
        //Write_PCX_File(&file, *HiddenSurface, nullptr);

        SDL_Update_Screen(PrimarySurface);
    }

    _asm { ret 4 }
}


DECLARE_PATCH(_OwnerDraw_DialogProc_SDL_Update_Window_Patch_Return_0)
{
    if (UseSDL2) {
        SDL_Update_Screen(PrimarySurface);
    }

    _asm { xor eax, eax }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x2E8 }
    _asm { ret 0x10 }
}


DECLARE_PATCH(_OwnerDraw_DialogProc_SDL_Update_Window_Patch_Return_1)
{
    if (UseSDL2) {
        SDL_Update_Screen(PrimarySurface);
    }

    _asm { mov eax, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x2E8 }
    _asm { ret 0x10 }
}


DECLARE_PATCH(_OwnerDraw_DialogProc_SDL_Update_Window_Patch_Return_Var)
{
    if (UseSDL2) {
        SDL_Update_Screen(PrimarySurface);
    }

    _asm { mov eax, [esp+0x1C] }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x2E8 }
    _asm { ret 0x10 }
}


static DSurface * __cdecl Create_DSurface(int width, int height)
{
    if (UseSDL2) {
        return (DSurface *)new SDLSurface(width, height);
    } else {
        return new DSurface(width, height);
    }
}


static DSurface * __cdecl Create_DSurface(int width, int height, bool a3)
{
    if (UseSDL2) {
        return (DSurface *)new SDLSurface(width, height);
    } else {
        return new DSurface(width, height, a3);
    }
}


static DSurface * __cdecl Placement_Create_DSurface(DSurface *surface, int width, int height, bool a3)
{
    if (UseSDL2) {
        return (DSurface *)new (surface) SDLSurface(width, height);
    } else {
        return new (surface) DSurface(width, height, a3);
    }
}


static DSurface * __cdecl Placement_Destory_DSurface(DSurface *surface)
{
    if (UseSDL2) {
        delete (surface);
    } else {
        delete (surface);
    }
}


DECLARE_PATCH(_Check_Overlapped_Blit_Capability_Create_Surface_Patch)
{
    LEA_STACK_STATIC(DSurface *, surface, esp, 0x34);

    Placement_Create_DSurface(surface, 64, 64, false);

    JMP(0x00472955);
}


// TODO: Needs testing.
DECLARE_PATCH(_ScoreClass_Presentation_Create_Surface_Patch)
{
    static DSurface *surface;

    _asm { mov [esp+0x64], ebx }

    surface = Create_DSurface(HiddenSurface->Get_Width(), HiddenSurface->Get_Height(), false);
    _asm { mov eax, surface }
    _asm { mov [ebp+0x8], eax }

    _asm { mov edx, 0x005E3086 }// EAX is used as return from constructor.
    _asm { jmp edx }
}


// TODO: Needs testing.
DECLARE_PATCH(_Slide_Show_Create_Surface_Patch)
{
    static DSurface *surface;

    surface = Create_DSurface(ScreenRect.Width, ScreenRect.Height, false);
    _asm { mov eax, surface }

    _asm { mov edx, 0x004915AE }// EAX is used as return from constructor.
    _asm { jmp edx }
}


// TODO: Needs testing.
DECLARE_PATCH(_Debug_Motion_Capture_Create_Surface_Patch)
{
    LEA_STACK_STATIC(DSurface *, surface, esp, 0x64);

    Placement_Create_DSurface(surface, ScreenRect.Width, ScreenRect.Height, false);
    _asm { mov eax, surface }

    _asm { mov edx, 0x00508978 }// EAX is used as return from constructor.
    _asm { jmp edx }
}


// TODO: Needs testing.
DECLARE_PATCH(_PreviewClass_Draw_Map_Create_Surface_Patch)
{
    GET_REGISTER_STATIC(int, width, edi);
    GET_STACK_STATIC(int, height, esp, 0x78);
    static DSurface *surface;

    surface = Create_DSurface(width, height, true);
    _asm { mov eax, surface }

    _asm { mov edx, 0x005AC345 }// EAX is used as return from constructor.
    _asm { jmp edx }
}


// TODO: Needs testing.
DECLARE_PATCH(_PreviewClass_Read_Preview_INI_Create_Surface_Patch)
{
    static int xpos; _asm { mov edi, [eax+0x0] }; _asm { mov xpos, edi }; // return Rect from ini.Get_Rect("Preview")
    static int ypos; _asm { mov edi, [eax+0x4] }; _asm { mov ypos, edi };
    static int width; _asm { mov edi, [eax+0x8] }; _asm { mov width, edi };
    static int height; _asm { mov edi, [eax+0x0C] }; _asm { mov height, edi };
    static DSurface *surface;

    surface = Create_DSurface(width, height, true);
    _asm { mov eax, surface }

    _asm { mov edx, 0x005ACAA0 }// EAX is used as return from constructor.
    _asm { jmp edx }
}


DECLARE_PATCH(_PreviewClass_Read_PCX_Preview_Create_Surface_Patch)
{
}


//0x005AD00B


// TODO: Needs testing.
DECLARE_PATCH(_PreviewClass_Create_Preview_Surface_Create_Surface_Patch)
{
    GET_STACK_STATIC(int, width, esp, 0x3C);
    GET_REGISTER_STATIC(int, height, ebx);

    static DSurface *surface = Create_DSurface(width, height, true);
    _asm { mov eax, surface }

    _asm { mov edx, 0x005AD4E8 }// EAX is used as return from constructor.
    _asm { jmp edx }
}


// TODO: Needs testing.
DECLARE_PATCH(_6_Create_Surface_Patch) // RadarClass compute something?
{
    GET_STACK_STATIC(DSurface *, rsurf, esi, 0x1228);

    static DSurface *surface = Create_DSurface(rsurf->Width, rsurf->Height, true);
    _asm { mov eax, surface }

    _asm { mov edx, 0x005B9CE8 }// EAX is used as return from constructor.
    _asm { jmp edx }
}


static DSurface *_Placement_DSurface_CTOR_Patch(DSurface *surface, int dummy, int width, int height, bool a3)
{
    if (UseSDL2) {
        return (DSurface *)new (surface) SDLSurface(width, height);
    } else {
        return new (surface) DSurface(width, height, a3);
    }
}


void SDL_Hooks()
{
    {
        RawFileClass file("SUN.INI");
        INIClass ini(file);

        UseSDL2 = ini.Get_Bool("Video", "UseSDL2", UseSDL2);
        SDLBorderless = true; // ini.Get_Bool("Video", "SDLBorderless", SDLBorderless);
        SDLBorderlessFullscreen = false; //ini.Get_Bool("Video", "SDLFullscreen", SDLBorderlessFullscreen);
        SDLClipMouseToWindow = true; //ini.Get_Bool("Video", "SDLClipMouse", SDLClipMouseToWindow);
    }

    Patch_Jump(0x0060139A, &_WinMain_Create_Main_Window_Patch_1);
    Patch_Jump(0x00601688, &_WinMain_Create_Main_Window_Patch_2);

    Patch_Jump(0x006013C8, &_WinMain_Prep_Direct_Draw_Patch_1);
    Patch_Jump(0x006016B3, &_WinMain_Prep_Direct_Draw_Patch_2);

    Patch_Jump(0x006014C0, &_WinMain_Clear_Primary_Surface_Patch);

    Patch_Jump(0x00601428, &_WinMain_16Bit_Color_Check_Patch);

    Patch_Call(0x0050AD34, &_Set_Video_Mode);
    Patch_Call(0x006015E6, &_Set_Video_Mode);
    Patch_Call(0x0060161C, &_Set_Video_Mode);
    Patch_Call(0x00601716, &_Set_Video_Mode);
    Patch_Call(0x00601790, &_Set_Video_Mode);

    Patch_Call(0x005FF7B4, &_Reset_Video_Mode);

    /**
     *  Replacement DSurface function calls and patches.
     */
    Patch_Call(0x0060141E, &_Create_Primary);
    Patch_Call(0x0050AF41, &_Create_Primary);
    Patch_Call(0x0050AD5A, &_Create_Primary);
    Patch_Call(0x0059C506, &_Get_DC);
    Patch_Call(0x0059E063, &_Get_DC);
    Patch_Call(0x0059F227, &_Get_DC);
    Patch_Call(0x0059C5B8, &_Release_DC);
    Patch_Call(0x0059E0C7, &_Release_DC);
    Patch_Call(0x0059F30E, &_Release_DC);
    Patch_Call(0x00685A85, &_Restore_Check);
    Patch_Call(0x00685A94, &_Restore_Check);
    Patch_Call(0x00685AA8, &_Restore_Check);
    Patch_Call(0x00685AD9, &_Restore_Check);
    Patch_Call(0x00685B0A, &_Restore_Check);
    Patch_Call(0x00685B3B, &_Restore_Check);

    /**
     *  Patches to allocate the game surfaces using SDLSurface.
     */
    Patch_Call(0x0050B05D, &_Allocate_Surfaces);
    Patch_Call(0x005D6D03, &_Allocate_Surfaces);
    Patch_Call(0x00601543, &_Allocate_Surfaces);

    /**
     *  Global DirectDraw functions.
     */
    Patch_Call(0x004EC1C7, &_Wait_Blit);
    Patch_Call(0x0050953F, &_Wait_Blit);
    Patch_Call(0x00509781, &_Wait_Blit);
    Patch_Call(0x0050AC87, &_Wait_Blit);
    Patch_Call(0x00571253, &_Wait_Blit);
    Patch_Call(0x0047285D, &_Set_DD_Palette);
    Patch_Call(0x004728AF, &_Set_DD_Palette);
    Patch_Call(0x0066B7B6, &_Set_DD_Palette);
    Patch_Call(0x0066BAF3, &_Set_DD_Palette);
    Patch_Call(0x0066BD3E, &_Set_DD_Palette);

    /**
     *  Constructor calls.
     */
    Patch_Jump(0x00491586, &_Slide_Show_Create_Surface_Patch);

    Patch_Jump(0x005AC324, &_PreviewClass_Draw_Map_Create_Surface_Patch);
    Patch_Jump(0x005ACA68, &_PreviewClass_Read_Preview_INI_Create_Surface_Patch);
    Patch_Jump(0x005ACD42, &_PreviewClass_Read_PCX_Preview_Create_Surface_Patch);
    //Patch_Jump(0x005AD00B, &);
    Patch_Jump(0x005AD4C7, &_PreviewClass_Create_Preview_Surface_Create_Surface_Patch);
    Patch_Jump(0x005B9CAF, &_6_Create_Surface_Patch); // radar class compute?
    Patch_Jump(0x005E304D, &_ScoreClass_Presentation_Create_Surface_Patch);

    Patch_Call(0x00472950, &_Placement_DSurface_CTOR_Patch); // Check_Overlapped_Blit_Capability
    Patch_Call(0x00508973, &_Placement_DSurface_CTOR_Patch); // Debug_Motion_Capture
    Patch_Call(0x005AD02C, &_Placement_DSurface_CTOR_Patch); // creates palleted map preview?

    /**
     *  Patches to force update to the SDL surfaces.
     */
    // possible locations of PrimarySurface->Copy_From
    //004919FA slide show
    //004EAC27 screenshot command           -- not needed, copy from primary
    //0050894D debug motion capture         -- not needed, copy from primary
    //00553902 mapsel
    //00553C35 mapsel
    Patch_Jump(0x005640CD, &_MovieClass_Blit_SDL_Update_Window_Patch_1);
    Patch_Jump(0x00564787, &_MovieClass_Blit_SDL_Update_Window_Patch_2);
    Patch_Jump(0x00571116, &_MSEngine_Blit_SDL_Update_Window_Patch);
    Patch_Jump(0x005711F5, &_MSEngine_Draw_SDL_Update_Window_Patch);
    //0058FFE3 owner draw
    //00591F3C owner draw
    //005920A8 owner draw       -- maybe blit?
    //00592273 owner draw
    //00592306 owner draw
    //00592F7A owner draw
    //0059310C owner draw
    //005932AB owner draw
    //00593372 owner draw
    //0059353E owner draw
    //0059371D owner draw
    //005938ED owner draw

    //00593F8A owner draw       -- hit in dialog open, hit while open
    //00594046 owner draw       -- hit in dialog open
    //005940FE owner draw       -- hit in dialog open
    //005941C0 owner draw       -- hit in dialog open
    //00594379 owner draw       -- hit in dialog open
    //0059449C owner draw
    Patch_Jump(0x00592356, &_OwnerDraw_DialogProc_SDL_Update_Window_Patch_Return_1);
    Patch_Jump(0x0059264F, &_OwnerDraw_DialogProc_SDL_Update_Window_Patch_Return_0);
    Patch_Jump(0x005926D8, &_OwnerDraw_DialogProc_SDL_Update_Window_Patch_Return_1);
    Patch_Jump(0x00592802, &_OwnerDraw_DialogProc_SDL_Update_Window_Patch_Return_1);
    Patch_Jump(0x005944EF, &_OwnerDraw_DialogProc_SDL_Update_Window_Patch_Return_1);
    Patch_Jump(0x005944FE, &_OwnerDraw_DialogProc_SDL_Update_Window_Patch_Return_Var);

    //0059F651 owner draw       -- hit in dialog open

    //005E4CE8 score
    //005E6465 score

    //005F3A0C sidebar
    //005F3AEA sidebar
    //005F3B70 sidebar
    //005F3C4A sidebar
    //Patch_Jump(0x005F3C61, &_SidebarClass_Blit_Sidebar_SDL_Update_Window_Patch);

    //0067CA16 wdt

    /**
     *  Patches to update after main loop tick or request.
     */
    //Patch_Jump(0x0050939E, &_Main_Loop_SDL_Update_Window_Patch);
    Patch_Jump(0x004B9A42, &_GScreenClass_Do_Blit_SDL_Update_Window_Patch);
}
