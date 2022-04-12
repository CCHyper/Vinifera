/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VIDEO_HOOKS.CPP
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
#include "video_hooks.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "vinifera_util.h"
#include "virtualsurface.h"
#include "video_driver.h"
#include "ownerdraw_window.h"
#include "options.h"
#include "optionsext.h"
#include "movie.h"
#include "rawfile.h"
#include "filepng.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 * 
 *  x
 * 
 */

static bool Create_Main_Window_Intercept(HINSTANCE hInstance, int command_show, int width, int height)
{
    DWORD dwPid = GetProcessId(GetCurrentProcess());
    if (!dwPid) {
        DEBUG_ERROR("Create_Main_Window() - Failed to get the process id!\n");
        return false;
    }

    bool retval = Video_Driver()->Create_Window(
        Vinifera_Get_Window_Title(dwPid),
        hInstance,
        OptionsExtension->IsWindowed ? OptionsExtension->WindowWidth : Options.ScreenWidth,
        OptionsExtension->IsWindowed ? OptionsExtension->WindowHeight : Options.ScreenHeight,
        OptionsExtension->RefreshRate,
        OptionsExtension->IsWindowed,
        OptionsExtension->IsBorderlessWindow);

    // Create the fake OwnerDraw window.
    OwnerDraw_Create_Window(hInstance, 640, 400);
    
    return retval;
}

static bool Allocate_Surfaces_Intercept(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect)
{
    return Video_Driver()->Allocate_Surfaces(common_rect, composite_rect, tile_rect, sidebar_rect);
}

static XSurface *Create_Primary_Intercept(XSurface **backbuffer_surface)
{
    XSurface *surface = Video_Driver()->Create_Primary(backbuffer_surface);

    // Hack! Window seems to be out of focus at this point...
    // The window needs this initially otherwise we need to alt-tab to gain focus.
    GameInFocus = true;

    return surface;
}

static void Wait_Blit_Intercept()
{
    Video_Driver()->Wait_Blit();
}

static bool Set_Video_Mode_Intercept(HWND hWnd, int width, int height, int bits_per_pixel)
{
    /**
     *  Disregard the input bit depth, all new renderers are created as 32bit.
     */
    return Video_Driver()->Set_Video_Mode(hWnd, width, height, bits_per_pixel);
}

static void Check_Overlapped_Blit_Capability_Intercept()
{
    Video_Driver()->Check_Overlapped_Blit_Capability();
}

static bool Prep_Renderer_Intercept(HWND hWnd)
{
    return Video_Driver()->Prep_Renderer(hWnd);
}










DECLARE_PATCH(_WinMain_Remove_16Bit_Color_Check_Patch)
{
    JMP(0x00601467);
}







static void Video_Flip_Primary()
{
    Video_Driver()->Flip(PrimarySurface);
}

static void Video_Flip_Hidden()
{
    Video_Driver()->Flip(HiddenSurface);
}

static void Video_Set_Cursor_Clip()
{
    Video_Driver()->Set_Cursor_Clip();
}

static void Video_Clear_Cursor_Clip()
{
    Video_Driver()->Clear_Cursor_Clip();
}

static void Video_Frame_Limiter(bool force_blit)
{
    Video_Driver()->Frame_Limiter(force_blit);
}

static void Video_Focus_Loss()
{
    Video_Driver()->Focus_Loss();
}

static void Video_Focus_Restore()
{
    Video_Driver()->Focus_Restore();
}





/**
 *  These functions allow us to replace any calls with the inlined DSurface
 *  constructors and replace them with VirtualSurface's.
 */
static XSurface *_Placement_Surface_Constructor_Patch(XSurface *surface, int dummy, int width, int height, bool sys_mem)
{
    return new (surface) VirtualSurface(width, height, sys_mem);
}

static XSurface * __cdecl Create_Surface(int width, int height)
{
    return new VirtualSurface(width, height);
}

static XSurface * __cdecl Create_Surface(int width, int height, bool sys_mem)
{
    return new VirtualSurface(width, height);
}

static XSurface * __cdecl Placement_Create_Surface(XSurface *surface, int width, int height, bool sys_mem)
{
    return new (surface) VirtualSurface(width, height);
}

static XSurface * __cdecl Placement_Destory_Surface(XSurface *surface)
{
    delete (surface);
}

void __cdecl Write_Surface_Data_To_File(const char *filename, XSurface *surface)
{
#if 1 // #ifndef NDEBUG
    Write_PNG_File(&RawFileClass(filename), *surface, &GamePalette);
#endif
}













// OWNER DRAW STUFF
static void _OwnerDraw_Set_Window_Pos(HWND dialog, int override_x, int override_y)
{
    //POINT tl;
    //tl.x = 0;
    //tl.y = 0;

    //POINT br;
    //br.x = VideoWidth;
    //br.y = VideoHeight;

    //ClientToScreen(MainWindow, &tl);
    //ClientToScreen(MainWindow, &br);

    RECT window_rect;
    GetClientRect(MainWindow, &window_rect);

    ClientToScreen(MainWindow, (LPPOINT)&window_rect);
    ClientToScreen(MainWindow, (LPPOINT)&window_rect.right);

    RECT dialog_rect;
    //GetWindowRect(dialog, &dialog_rect);

    GetClientRect(dialog, &dialog_rect);

    ClientToScreen(dialog, (LPPOINT)&dialog_rect);
    ClientToScreen(dialog, (LPPOINT)&dialog_rect.right);

    //dialog_rect.right -= dialog_rect.left;
    //dialog_rect.bottom -= dialog_rect.top;
    
    int xpos;
    if (override_x == -1) {
        //xpos = dialog_rect.left - tl.x;
        xpos = dialog_rect.left - window_rect.left;
    } else {
        xpos = override_x;
    }
    dialog_rect.left = xpos;
    
    int ypos;
    if (override_y == -1) {
        //ypos = dialog_rect.top - tl.y;
        ypos = dialog_rect.top - window_rect.top;
    } else {
        ypos = override_y;
    }
    dialog_rect.top = ypos;

    //MoveWindow(dialog, xpos, ypos, dialog_rect.right, dialog_rect.bottom, FALSE);
    MoveWindow(dialog, 0, 0, dialog_rect.right, dialog_rect.bottom, FALSE);
}







//00682F80 -- gets position within main window?
//00592505
//00682F89
//00683E87 -- just once?




// WIN DIALOG STUFF

static bool _Center_Window_Within(HWND dialog, HWND window)
{
    //POINT window_point { 0, 0 };
    //POINT dialog_point { 0, 0 };
    RECT window_rect;
    RECT dialog_rect;

    /**
     *  Get the size of the window we are to center within.
     */
    GetClientRect(window, &window_rect);
    //GetWindowRect(window, &window_rect);

    // TODO: add fullscreen check?
    //if (window == MainWindow) {
    //    window_rect.right = VideoWidth;
    //    window_rect.bottom = VideoHeight;
    //}

    ClientToScreen(window, (LPPOINT)&window_rect);
    ClientToScreen(window, (LPPOINT)&window_rect.right);

    //dialog_rect.left = window_rect.left;
    //dialog_rect.top = window_rect.top;

    //window_rect.right -= window_rect.left;
    //window_rect.bottom -= window_rect.top;

    /**
     *  Get the size of the dialog we are to center.
     */
    GetClientRect(dialog, &dialog_rect);

    ClientToScreen(dialog, (LPPOINT)&dialog_rect);
    ClientToScreen(dialog, (LPPOINT)&dialog_rect.right);

    //dialog_rect.right -= dialog_rect.left;
    //dialog_rect.bottom -= dialog_rect.top;

    //OffsetRect(&dialog_rect, window_point.x, window_point.y);

    /**
     *  
     */
    //int x = (window_rect.right - (dialog_rect.right - dialog_rect.left) + 1) / 2;
    int x = window_rect.left + ((window_rect.right - dialog_rect.right + 1) / 2);
    int y = window_rect.top + ((window_rect.bottom - dialog_rect.bottom + 1) / 2);

    if (x < 0) {
        x = 0;
    }
    if (y < 0) {
        y = 0;
    }

    return SetWindowPos(dialog, nullptr, x, y, -1, -1, SWP_NOSIZE|SWP_NOZORDER);
    //return SetWindowPos(dialog, nullptr, dialog_rect.left, dialog_rect.top, -1, -1, SWP_NOSIZE|SWP_NOZORDER);
}










DECLARE_PATCH(_Sync_Delay_Start)
{
    _asm { mov esp, 0x1C }
    _asm { push ebx }
    _asm { push ebp }
    _asm { push esi }

    Video_Frame_Limiter(true);

    _asm { mov ecx, [0x0047C618] } // FrameTimer.Started
    _asm { or ebp, 0xFFFFFFFF }

    JMP_REG(esi, 0x005094B1);
}


DECLARE_PATCH(_Sync_Delay_Sleep_1)
{
    Video_Frame_Limiter(false);
    JMP(0x00509673);
}

DECLARE_PATCH(_Sync_Delay_Sleep_2)
{
    Video_Frame_Limiter(false);
    JMP(0x00509688);
}

// 005097EA
// 005097F7

DECLARE_PATCH(_Sync_Delay_Sleep_5) // 3
{
    Video_Frame_Limiter(false);
    JMP(0x00509807);
}

















// TODO, check if ClientToScreen is still needed.
DECLARE_PATCH(_MSEngine_Blit_Update_Window_Patch)
{
    // PrimarySurface (ecx) -> Copy_From
    _asm { push eax }
    _asm { mov edx, [ecx] }
    _asm { mov eax, [edx+0x8] }
    _asm { call eax }

    //DEBUG_INFO("MSEngine::Blit() - Copying to PrimarySurface.\n");

    Write_Surface_Data_To_File("MSENGINE.PNG", HiddenSurface);

    Video_Flip_Hidden();

    JMP(0x0057111C);
}

// TODO, check if ClientToScreen is still needed.
DECLARE_PATCH(_MSEngine_Draw_Update_Window_Patch)
{
    // PrimarySurface (ecx) -> Copy_From
    _asm { push edx }
    _asm { mov ecx, [ecx] }
    _asm { mov eax, [ecx+0x8] }
    _asm { call eax }

    //DEBUG_INFO("MSEngine::Draw() - Copying to PrimarySurface.\n");

    Write_Surface_Data_To_File("MSENGINE.PNG", HiddenSurface);

    Video_Flip_Hidden();

    JMP(0x005711F8);
}









DECLARE_PATCH(_SidebarClass_Blit_Sidebar_Update_Window_Patch)
{
    //DEBUG_INFO("SidebarClass::Blit_Sidebar() - Flip.\n");

    Write_Surface_Data_To_File("SIDEBAR.PNG", HiddenSurface);

    Video_Flip_Hidden();

    _asm { ret 4 }
}









// TODO: Needs testing.
DECLARE_PATCH(_ScoreClass_Presentation_Create_Surface_Patch)
{
    static XSurface *surface;

    _asm { mov [esp+0x64], ebx }

    surface = Create_Surface(HiddenSurface->Get_Width(), HiddenSurface->Get_Height(), false);
    _asm { mov eax, surface }
    _asm { mov [ebp+0x8], eax }

    JMP_REG(edx, 0x005E3086);
}


// TODO: Needs testing.
DECLARE_PATCH(_Slide_Show_Create_Surface_Patch)
{
    static XSurface *surface;

    surface = Create_Surface(ScreenRect.Width, ScreenRect.Height, false);
    _asm { mov eax, surface }

    JMP_REG(edx, 0x004915AE);
}


// TODO: Needs testing.
DECLARE_PATCH(_Debug_Motion_Capture_Create_Surface_Patch)
{
    LEA_STACK_STATIC(XSurface *, surface, esp, 0x64);

    Placement_Create_Surface(surface, ScreenRect.Width, ScreenRect.Height, false);
    _asm { mov eax, surface }

    JMP_REG(edx, 0x00508978);
}


// TODO: Needs testing.
DECLARE_PATCH(_PreviewClass_Draw_Map_Create_Surface_Patch)
{
    GET_REGISTER_STATIC(int, width, edi);
    GET_STACK_STATIC(int, height, esp, 0x78);
    static XSurface *surface;

    surface = Create_Surface(width, height, true);
    _asm { mov eax, surface }

    JMP_REG(edx, 0x005AC345);
}


// TODO: Needs testing.
DECLARE_PATCH(_PreviewClass_Read_Preview_INI_Create_Surface_Patch)
{
    static int xpos; _asm { mov edi, [eax+0x0] }; _asm { mov xpos, edi }; // return Rect from ini.Get_Rect("Preview")
    static int ypos; _asm { mov edi, [eax+0x4] }; _asm { mov ypos, edi };
    static int width; _asm { mov edi, [eax+0x8] }; _asm { mov width, edi };
    static int height; _asm { mov edi, [eax+0x0C] }; _asm { mov height, edi };
    static XSurface *surface;

    surface = Create_Surface(width, height, true);
    _asm { mov eax, surface }

    JMP_REG(edx, 0x005ACAA0);
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
    static XSurface *surface;

    surface = Create_Surface(width, height, true);
    _asm { mov eax, surface }

    JMP_REG(edx, 0x005AD4E8);
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







// TEMP, this function needs reworking to handle multiplayer case.
DECLARE_PATCH(_Owner_Draw_Loop_Blit_Patch)
{
    Call_Back();

    Video_Flip_Primary();

    Sleep(0);

    JMP(0x005A0B9D);
}








DECLARE_PATCH(_Focus_Loss_Patch)
{
    Video_Focus_Loss();

    JMP(0x00685B67);
}

DECLARE_PATCH(_Focus_Restore_Patch)
{
    Video_Focus_Restore();

    JMP(0x00685B67);
}



#if 0
DECLARE_PATCH(_Focus_Loss_Cursor_Clip_Patch)
{
    Video_Clear_Cursor_Clip();

    _asm { ret }
}
#endif

DECLARE_PATCH(_Focus_Loss_Cursor_Clip_Patch/*_WWMouse_Case*/)
{
    // Stolen bytes/code.
    DEBUG_GAME("Focus_Loss()\n");

    Video_Clear_Cursor_Clip();

    JMP(0x0068598D);
}

DECLARE_PATCH(_Main_Window_Procedure_Focus_Loss_Cursor_Clip_Patch)
{
    // Stolen bytes/code.
    DEBUG_GAME("Focus_Loss()\n");

    Video_Clear_Cursor_Clip();

    JMP(0x00685E9E);
}

DECLARE_PATCH(_Focus_Restore_Cursor_Clip_Patch)
{
    // Stolen bytes/code.
    DEBUG_GAME("Focus_Restore()\n");

    Video_Set_Cursor_Clip();

    JMP(0x00685A0A);
}














DECLARE_PATCH(_OwnerDraw_DialogProc_Update_Window_Patch_Return_0)
{
    Video_Flip_Primary();

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
    Video_Flip_Primary();

    _asm { mov eax, 1 }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x2E8 }
    _asm { ret 0x10 }
}


DECLARE_PATCH(_OwnerDraw_DialogProc_Update_Window_Patch_Return_Var)
{
    Video_Flip_Primary();

    _asm { mov eax, [esp+0x1C] }
    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x2E8 }
    _asm { ret 0x10 }
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

    Video_Flip_Primary();

    JMP(0x005640D3);
}

DECLARE_PATCH(_MovieClass_Blit_Update_Window_Patch_2)
{
    PrimarySurface->Copy_From(
        CurrentMovie->StretchRect, *CurrentMovie->MovieSurface, CurrentMovie->VideoRect);

    Video_Flip_Primary();

    JMP(0x0056478D);
}


/**
 *  Adds a flip call to the end of GScreenClass::Blit.
 */
DECLARE_PATCH(_GScreenClass_Blit_Flip)
{
    Video_Flip_Primary();

    _asm { pop esi }
    _asm { add esp, 0x5C }
    _asm { ret 4 }
}


/**
 *  
 */
DECLARE_PATCH(_GScreenClass_Blit_ClientToScreen_Patch)
{
    LEA_STACK_STATIC(RECT *, rect, esp, 0x50);

    GetClientRect(MainWindow, rect);

    ClientToScreen(MainWindow, (LPPOINT)&rect);
    ClientToScreen(MainWindow, (LPPOINT)&rect->right);





    //_asm { mov [esp+0x50], 0 } // rect.left
    //_asm { mov [esp+0x54], 0 } // rect.top

    JMP(0x004B9744);
}


void Video_Hooks()
{
    // Force all new renderers to be created as a window.
    Debug_Windowed = true;



    /**
     *  Patch in the renderer frame limiter.
     */
    Patch_Jump(0x00509670, &_Sync_Delay_Sleep_1);
    Patch_Jump(0x00509685, &_Sync_Delay_Sleep_2);


    Patch_Jump(0x00509804, &_Sync_Delay_Sleep_5);

    /**
     *  Patch in the renderer flipper.
     */
    Patch_Jump(0x004B9A68, &_GScreenClass_Blit_Flip);
    Patch_Jump(0x004B9701, &_GScreenClass_Blit_ClientToScreen_Patch);




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

    Patch_Call(0x00472950, &_Placement_Surface_Constructor_Patch); // Check_Overlapped_Blit_Capability
    Patch_Call(0x00508973, &_Placement_Surface_Constructor_Patch); // Debug_Motion_Capture
    Patch_Call(0x005AD02C, &_Placement_Surface_Constructor_Patch); // creates palleted map preview?




    /**
     *  Handle focus loss and restore.
     */
    //Patch_Jump(0x00685A6D, &_Focus_Restore_Patch);

    // Bit tricky, need to rewrite the function end to move it to a
    // location we can jump from.
    Patch_Byte(0x006859CC+1, 0xA4); // jz 0x00685972
    Patch_Jump(0x00685972, &_Focus_Loss_Cursor_Clip_Patch);

    Patch_Jump(0x00685E91, &_Main_Window_Procedure_Focus_Loss_Cursor_Clip_Patch);
    Patch_Jump(0x00685980, &_Focus_Loss_Cursor_Clip_Patch/*_WWMouse_Case*/);

    Patch_Jump(0x00685A00, &_Focus_Restore_Cursor_Clip_Patch);
    Patch_Byte_Range(0x00685A0F, 0x90, 3); // Removes "add esp, 4".



    /**
     *  Some renderers handle the window differently, so some patches do not apply too all.
     */
    //if (VideoDriver::UseDirectDraw7) {

        // possible locations of PrimarySurface->Copy_From
        //004919FA slide show
        //004EAC27 screenshot command           -- not needed, copy from primary
        //0050894D debug motion capture         -- not needed, copy from primary
        //00553902 mapsel
        //00553C35 mapsel

        Patch_Jump(0x00564061, &_MovieClass_Blit_Update_Window_Patch_1);            // DONE
        Patch_Jump(0x0056471A, &_MovieClass_Blit_Update_Window_Patch_2);            // DONE
        Patch_Jump(0x00571116, &_MSEngine_Blit_Update_Window_Patch);
        Patch_Jump(0x005711F5, &_MSEngine_Draw_Update_Window_Patch);

        Patch_Jump(0x005A0B98, &_Owner_Draw_Loop_Blit_Patch);
        //Patch_Jump(0x005A0B98, &_Owner_Draw_Flip_Loop_1); // Need confirming!
        //Patch_Jump(0x004B9A68, &_Owner_Draw_Flip_Loop_2); // Need confirming!

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
        Patch_Jump(0x00592356, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_1);
        Patch_Jump(0x0059264F, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_0);
        Patch_Jump(0x005926D8, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_1);
        Patch_Jump(0x00592802, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_1);
        Patch_Jump(0x005944EF, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_1);
        Patch_Jump(0x005944FE, &_OwnerDraw_DialogProc_Update_Window_Patch_Return_Var);

        //0059F651 owner draw       -- hit in dialog open

        //005E4CE8 score
        //005E6465 score

        //005F3A0C sidebar
        //005F3AEA sidebar
        //005F3B70 sidebar
        //005F3C4A sidebar
        Patch_Jump(0x005F3C61, &_SidebarClass_Blit_Sidebar_Update_Window_Patch);

        //0067CA16 wdt





        Patch_Jump(0x00685600, &_Center_Window_Within);  // -- Center raw dialogs correctly (rules select etc).




        Patch_Jump(0x005A0BA0, &_OwnerDraw_Set_Window_Pos);



        // GetWindowRect > GetClientRect
        //Patch_Dword(0x00572929+2, 0x006CA388);
        //Patch_Dword(0x0058FF8D+2, 0x006CA388);
        //Patch_Dword(0x00592505+2, 0x006CA388);
        //Patch_Dword(0x00593024+2, 0x006CA388);
        //Patch_Dword(0x0059596B+2, 0x006CA388);
        //Patch_Dword(0x00596CE6+2, 0x006CA388);
        //Patch_Dword(0x005A0BF4+2, 0x006CA388);
        //Patch_Dword(0x00682F89+2, 0x006CA388);
        //Patch_Dword(0x00683DFC+2, 0x006CA388);
        //Patch_Dword(0x00684943+2, 0x006CA388);
        //Patch_Dword(0x00684A85+2, 0x006CA388);
        //Patch_Dword(0x00684B21+2, 0x006CA388);
        //Patch_Dword(0x00684C8E+2, 0x006CA388);
        //Patch_Dword(0x00685009+2, 0x006CA388);
        //Patch_Dword(0x00685264+2, 0x006CA388);




    //}


    // removes Debug_Windowed check and calls Set_Video_Mode.
    Patch_Jump(0x006016B8, 0x006015FC);



    /**
     *  Patch in the renderer interface interception functions.
     */
    Patch_Call(0x006013AB, &Create_Main_Window_Intercept);
    Patch_Call(0x00601696, &Create_Main_Window_Intercept);
    Patch_Call(0x0050AD34, &Set_Video_Mode_Intercept);
    Patch_Call(0x006015E6, &Set_Video_Mode_Intercept);
    Patch_Call(0x0060161C, &Set_Video_Mode_Intercept);
    Patch_Call(0x00601716, &Set_Video_Mode_Intercept);
    Patch_Call(0x00601790, &Set_Video_Mode_Intercept);
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
}
