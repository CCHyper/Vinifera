/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VIDEO_DRIVER.CPP
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
#include "video_driver.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "ddraw_driver.h"           // Required for driver check function below.
#include "virtualsurface.h"
#include "options.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <chrono>


/**
 *  
 */
static VideoDriver *VideoRenderer = nullptr;


/**
 *  x
 *
 *  @author: CCHyper
 */
VideoDriver::VideoDriver(Wstring driver_name) :
    DriverName(driver_name),
    WindowTitle("Tiberian Sun"),
    IsWindowed(false),
    IsBorderlessWindow(false),
    IsClipCursorToWindow(true),
    WindowWidth(Options.ScreenWidth),
    WindowHeight(Options.ScreenHeight),
    RefreshRate(30),
    FrameLimit(15),
    MonitorToUse(-1)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool VideoDriver::Create_Window(Wstring window_title, HINSTANCE hInstance, int width, int height, int refresh_rate, bool windowed, bool borderless)
{
    WindowTitle = window_title;

    WindowWidth = width;
    WindowHeight = height;
    RefreshRate = refresh_rate;
    IsWindowed = windowed;
    IsBorderlessWindow = borderless;

    return Internal_Create_Window(hInstance);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
XSurface *VideoDriver::Create_Primary(XSurface **backbuffer_surface)
{
    /**
     *  Create a virtual surface keep the game happy.
     */
    VirtualSurface *virtual_surface = new VirtualSurface(Options.ScreenWidth, Options.ScreenHeight);
    ASSERT(virtual_surface != nullptr);

    if (!virtual_surface) {
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
     *  Call the driver to create its primary surfaces.
     */
    if (!Internal_Create_Primary(virtual_surface, backbuffer_surface)) {
        DEBUG_ERROR("Create_Primary() - Failed to create driver's primary surface!\n");
        Fatal("Create_Primary() - Failed to create driver's primary surface!\n");
        return nullptr;
    }

    return virtual_surface;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool VideoDriver::Allocate_Surfaces(Rect *common_rect, Rect *composite_rect, Rect *tile_rect, Rect *sidebar_rect)
{
    DEBUG_INFO("Creating virtual game surfaces...\n");

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
        AlternateSurface = (DSurface *)new VirtualSurface(common_rect->Width, common_rect->Height);
        AlternateSurface->Clear();
        DEBUG_INFO("AlternateSurface created (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (common_rect->Width > 0 && common_rect->Height > 0) {
        HiddenSurface = (DSurface *)new VirtualSurface(common_rect->Width, common_rect->Height);
        HiddenSurface->Clear();
        DEBUG_INFO("HiddenSurface created (%dx%d)\n", common_rect->Width, common_rect->Height);
    }

    if (composite_rect->Width > 0 && composite_rect->Height > 0) {
        CompositeSurface = (DSurface *)new VirtualSurface(composite_rect->Width, composite_rect->Height);
        CompositeSurface->Clear();
        DEBUG_INFO("CompositeSurface created (%dx%d)\n", composite_rect->Width, composite_rect->Height);
    }

    if (tile_rect->Width > 0 && tile_rect->Height > 0) {
        TileSurface = (DSurface *)new VirtualSurface(tile_rect->Width, tile_rect->Height);
        TileSurface->Clear();
        DEBUG_INFO("TileSurface created (%dx%d)\n", tile_rect->Width, tile_rect->Height);
    }

    if (sidebar_rect->Width > 0 && sidebar_rect->Height > 0) {
        SidebarSurface = (DSurface *)new VirtualSurface(sidebar_rect->Width, sidebar_rect->Height);
        SidebarSurface->Clear();
        DEBUG_INFO("SidebarSurface created (%dx%d)\n", sidebar_rect->Width, sidebar_rect->Height);
    }

    return true;
}


/**
 *  x
 *  
 *  Returns 'true' if we had to sleep.
 * 
 *  @author: CCHyper
 */
bool VideoDriver::Frame_Limiter(bool force_blit)
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


/**
 *  x
 */
void __cdecl Set_Video_Driver(VideoDriver *driver)
{
    if (VideoRenderer) {
        DEBUG_INFO("Video: Removing \"%s\" as the video driver.\n", VideoRenderer->Get_Name().Peek_Buffer());
        Remove_Video_Driver();
    }

    VideoRenderer = driver;
    DEBUG_INFO("Video: Using \"%s\" video driver.\n", VideoRenderer->Get_Name().Peek_Buffer());

    /**
     *  
     */
    std::atexit(Remove_Video_Driver);
}


/**
 *  Removes the current video driver.
 */
void __cdecl Remove_Video_Driver()
{
    delete VideoRenderer;
    VideoRenderer = nullptr;
}


/**
 *  Fetch the video driver instance.
 */
VideoDriver * __cdecl Video_Driver()
{
    ASSERT(VideoRenderer != nullptr);
    return VideoRenderer;
}


/**
 *  Is the current video driver the Direct Draw driver?
 * 
 *  This is used to ensure we are not attempting to operate on a
 *  driver that does support the interface.
 * 
 *  @author: CCHyper
 */
bool Video_Driver_Is_Direct_Draw()
{
    return dynamic_cast<DirectDrawVideoDriver *>(Video_Driver()) != nullptr;
}
