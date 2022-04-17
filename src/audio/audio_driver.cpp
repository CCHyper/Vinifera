/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_DRIVER.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the new audio engine.
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
#include "audio_driver.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  
 */
static AudioDriver *AudioEngine = nullptr;


/**
 *  x
 * 
 *  @author: CCHyper
 */
void __cdecl Install_Audio_Driver(AudioDriver *driver)
{
    if (AudioEngine) {
        DEBUG_INFO("Audio: Removing \"%s\" as the audio driver.\n", AudioEngine->Get_Name().Peek_Buffer());
        Uninstall_Audio_Driver();
    }

    AudioEngine = driver;
    DEBUG_INFO("Audio: Using \"%s\" audio driver.\n", AudioEngine->Get_Name().Peek_Buffer());

    /**
     *  
     */
    std::atexit(Uninstall_Audio_Driver);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
/**
 *  Removes the current audio driver.
 */
void __cdecl Uninstall_Audio_Driver()
{
    delete AudioEngine;
    AudioEngine = nullptr;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
/**
 *  Fetch the audio driver instance.
 */
AudioDriver * __cdecl Audio_Driver()
{
    ASSERT(AudioEngine != nullptr);
    return AudioEngine;
}
