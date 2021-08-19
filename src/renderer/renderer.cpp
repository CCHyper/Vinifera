/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          RENDERER_SUPPORT.H
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
#include "ddraw7_support.h"
#include "bsurface.h"
#include "options.h"
#include "ccini.h"
#include "wwkeyboard.h"
#include "filepng.h"
#include <chrono>
#include "debughandler.h"
#include "asserthandler.h"


bool Borderless_Window = false;


/**
 *  
 */
bool Renderer::UseDirectDraw7 = false;

/**
 *  
 */
int Renderer::FrameLimit = 120;

/**
 *  
 */
bool Renderer::BorderlessWindow = false;

/**
 *  
 */
bool Renderer::ClipCursorToWindow = true;


/**
 *  
 *  
 *  @author: CCHyper
 */
bool Renderer::Read_INI(CCINIClass &ini)
{
    static char const * const RENDERER = "Renderer";
    static char const * const VIDEO = "Video";


//#ifndef NDEBUG
    UseDirectDraw7 = true;  // TEMP
    FrameLimit = 30;  // TEMP
//#endif



    /**
     *  
     */
    UseDirectDraw7 = ini.Get_Bool(RENDERER, "DirectDraw7", UseDirectDraw7);

    /**
     *  
     */
    FrameLimit = ini.Get_Int(RENDERER, "FrameLimit", FrameLimit);

    return true;
}


/**
 *  
 *  
 *  @author: CCHyper
 */
bool Renderer::Write_INI(CCINIClass &ini)
{
    static char const * const RENDERER = "Renderer";

    return true;
}


/**
 *  
 *  
 *  @author: CCHyper
 */
void __cdecl Renderer::Shutdown()
{
    if (UseDirectDraw7) {
        DirectDraw7_Shutdown();

    /**
     *  Original game function.
     */
    } else {
    }
}


/**
 *  
 *  
 *  @author: CCHyper
 */
void Renderer::Reset_Video_Mode()
{
    //DEBUG_INFO("Reset_Video_Mode(enter)\n");

    if (UseDirectDraw7) {
        DirectDraw7_Reset_Video_Mode();

    /**
     *  Original game function.
     */
    } else {
    }

    //DEBUG_INFO("Reset_Video_Mode(exit)\n");
}


/**
 *  
 *  
 *  @author: CCHyper
 */
bool Renderer::Set_Video_Mode(HWND hWnd, int width, int height, int bits_per_pixel)
{
    //DEBUG_INFO("Set_Video_Mode(enter)\n");

    if (UseDirectDraw7) {
        return DirectDraw7_Set_Video_Mode(hWnd, width, height, bits_per_pixel);

    /**
     *  Original game function.
     */
    } else {
    }

    //DEBUG_INFO("Set_Video_Mode(exit)\n");

    return true;
}


/**
 *  
 *  
 *  @author: CCHyper
 */
void Renderer::Check_Overlapped_Blit_Capability()
{
    //DEBUG_INFO("Check_Overlapped_Blit_Capability(enter)\n");

    if (UseDirectDraw7) {
        DirectDraw7_Check_Overlapped_Blit_Capability();

    /**
     *  Original game function.
     */
    } else {
    }

    //DEBUG_INFO("Check_Overlapped_Blit_Capability(exit)\n");
}


/**
 *  
 *  
 *  @author: CCHyper
 */
bool Renderer::Prep_Renderer(HWND hWnd)
{
    //DEBUG_INFO("Prep_Renderer(enter)\n");

    if (UseDirectDraw7) {
        return DirectDraw7_Prep(hWnd);

    /**
     *  Original game function.
     */
    } else {
    }

    //DEBUG_INFO("Prep_Renderer(exit)\n");

    return true;
}


/**
 *  
 *  
 *  @author: CCHyper
 */
void Renderer::Release(HWND hWnd)
{
    //DEBUG_INFO("Release_Direct_Draw(enter)\n");

    if (UseDirectDraw7) {
        DirectDraw7_Release(hWnd);

    /**
     *  Original game function.
     */
    } else {
    }

    //DEBUG_INFO("Release_Direct_Draw(exit)\n");
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool Renderer::Allocate_Surfaces(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect)
{
    //DEBUG_INFO("Allocate_Surfaces(enter)\n");

    if (UseDirectDraw7) {
        DirectDraw7_Allocate_Surfaces(common_rect, composite_rect, tile_rect, sidebar_rect);

    /**
     *  Original game function.
     */
    } else {
    }
    
    //DEBUG_INFO("Allocate_Surfaces(exit)\n");

    return true;
}


/**
 *  
 *  
 *  @author: CCHyper
 */
XSurface *Renderer::Create_Primary(XSurface **backbuffer_surface)
{
    //DEBUG_INFO("Create_Primary(enter)\n");

    if (UseDirectDraw7) {
        return DirectDraw7_Create_Primary(backbuffer_surface);

    /**
     *  Original game function.
     */
    } else {
        return DSurface::Create_Primary((DSurface **)backbuffer_surface);
    }
}


/**
 *  Waits for the blitter to become idle.
 * 
 *  @author: CCHyper
 */
void Renderer::Wait_Blit()
{
    //DEBUG_INFO("Wait_Blit(enter)\n");

    if (UseDirectDraw7) {
        DirectDraw7_Wait_Blit();

    /**
     *  Original game function.
     */
    } else {
    }

    //DEBUG_INFO("Wait_Blit(exit)\n");
}


/**
 *  
 *  
 *  @author: CCHyper
 */
bool Renderer::Flip(XSurface *surface)
{
    //DEBUG_INFO("Flip(enter)\n");

#if 0 // #ifndef NDEBUG
    if (GameInFocus && WWKeyboard->Down(KN_N)) {
        RawFileClass("PRIMARY.BIN").Write(PrimarySurface->Lock(), (PrimarySurface->Width*PrimarySurface->Height*PrimarySurface->BytesPerPixel));
        RawFileClass("COMPOSITE.BIN").Write(CompositeSurface->Lock(), (CompositeSurface->Width*CompositeSurface->Height*CompositeSurface->BytesPerPixel));
        RawFileClass("HIDDEN.BIN").Write(HiddenSurface->Lock(), (HiddenSurface->Width*HiddenSurface->Height*HiddenSurface->BytesPerPixel));
        RawFileClass("ALTERNATE.BIN").Write(AlternateSurface->Lock(), (AlternateSurface->Width*AlternateSurface->Height*AlternateSurface->BytesPerPixel));
        RawFileClass("TEMP.BIN").Write(TempSurface->Lock(), (TempSurface->Width*TempSurface->Height*TempSurface->BytesPerPixel));
        TempSurface->Unlock();
        AlternateSurface->Unlock();
        HiddenSurface->Unlock();
        CompositeSurface->Unlock();
        PrimarySurface->Unlock();
    }
#endif
#ifndef NDEBUG
    if (GameInFocus && WWKeyboard->Down(KN_N)) {
        DEBUG_WARNING("Writing snapshots of game surfaces...\n");
        Write_PNG_File(&RawFileClass("PRIMARY.PNG"), *PrimarySurface, &GamePalette);
        Write_PNG_File(&RawFileClass("COMPOSITE.PNG"), *CompositeSurface, &GamePalette);
        Write_PNG_File(&RawFileClass("HIDDEN.PNG"), *HiddenSurface, &GamePalette);
        Write_PNG_File(&RawFileClass("ALTERNATE.PNG"), *AlternateSurface, &GamePalette);
        Write_PNG_File(&RawFileClass("TEMP.PNG"), *TempSurface, &GamePalette);
        DEBUG_WARNING("DONE!\n");
    }
#endif

    if (UseDirectDraw7) {
        DirectDraw7_Flip(surface);

    /**
     *  Original game function.
     */
    } else {
    }

    //DEBUG_INFO("Flip(exit)\n");

    return true;
}


void Renderer::Focus_Loss()
{
    //DEBUG_INFO("Focus_Loss(enter)\n");

    Renderer::Clear_Cursor_Clip();

    //DEBUG_INFO("Focus_Loss(exit)\n");
}


void Renderer::Focus_Restore()
{
    //DEBUG_INFO("Focus_Restore(enter)\n");

    Renderer::Set_Cursor_Clip();

    if (UseDirectDraw7) {
        DirectDraw7_Focus_Restore();

    /**
     *  Original game function.
     */
    } else {
    }

    //DEBUG_INFO("Focus_Restore(exit)\n");
}


/**
 *  
 *  
 *  @author: CCHyper
 */
bool Renderer::Clear_Screen()
{
    //DEBUG_INFO("Clear_Screen(enter)\n");

    if (UseDirectDraw7) {
        DirectDraw7_Clear_Surface(DirectDraw7BackSurface);
        DirectDraw7_Clear_Surface(DirectDraw7PrimarySurface);

    /**
     *  Original game function.
     */
    } else {
    }

    //DEBUG_INFO("Clear_Screen(exit)\n");

    return true;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void Renderer::Set_Cursor_Clip()
{
    if (!ClipCursorToWindow) {
        return;
    }

    DEBUG_INFO("Set_Cursor_Clip()\n");

    /**
     *  
     */
    if (Debug_Windowed) {

        RECT rect;
        GetClientRect(MainWindow, &rect);

        POINT ul;
        ul.x = rect.left;
        ul.y = rect.top;

        POINT lr;
        lr.x = rect.right;
        lr.y = rect.bottom;

        MapWindowPoints(MainWindow, nullptr, &ul, 1);
        MapWindowPoints(MainWindow, nullptr, &lr, 1);

        rect.left = ul.x;
        rect.top = ul.y;

        rect.right = lr.x;
        rect.bottom = lr.y;

        ClipCursor(&rect);

    } else {

        RECT rect;
        GetClientRect(MainWindow, &rect);

        //rect.left = 0;
        //rect.top = 0;
        //rect.right = Options.ScreenWidth;
        //rect.bottom = Options.ScreenHeight;

        ClipCursor(&rect);

    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void Renderer::Clear_Cursor_Clip()
{
    if (!ClipCursorToWindow) {
        return;
    }

    DEBUG_INFO("Clear_Cursor_Clip()\n");

    /**
     *  
     */
    ClipCursor(nullptr);
}


/**
 *  
 *  
 *  Returns 'true' if we had to sleep.
 * 
 *  @author: CCHyper
 */
bool Renderer::Frame_Limiter(bool force_blit)
{
    static auto frame_start = std::chrono::steady_clock::now();
    static auto render_avg = 0;

    if (!FrameLimit) {
        return false;
    }

    auto render_start = std::chrono::steady_clock::now();

    int64_t _ms_per_tick = 1000 / FrameLimit;
    auto render_remaining = _ms_per_tick - std::chrono::duration_cast<std::chrono::milliseconds>(frame_start - render_start).count();

    if (!force_blit && render_remaining > render_avg) {
#ifndef NDEBUG
        DEBUG_INFO("Sleeping for '%d' milliseconds\n", unsigned(render_remaining));
#endif
        Sleep(unsigned(render_remaining));
        return true;
    }

    /**
     *  Flip the games PrimarySurface to the screen.
     */
    Flip(PrimarySurface);

    auto render_end = std::chrono::steady_clock::now();
    auto render_time = std::chrono::duration_cast<std::chrono::milliseconds>(render_end - render_start).count();

    /**
     *  Keep up some average so we have an idea if we need to skip a frame or not.
     */
    render_avg = (render_avg + render_time) / 2;

    return false;
}
