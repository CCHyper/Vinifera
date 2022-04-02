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
 *  @brief         Contains the hooks for the new video renderer.
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
#include "ddraw_driver.h"      // Required for driver check function below.
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  
 */
static VideoDriver *VideoRenderer = nullptr;


/**
 *  
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
bool Video_Driver_Is_Direct_Sound()
{
    return dynamic_cast<DirectDrawVideoDriver *>(Video_Driver()) != nullptr;
}
