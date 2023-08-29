/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                    *
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_IONSTORM.CPP
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
 *  @note          This file contains heavily modified code from the source code
 *                 released by Electronic Arts for the C&C Remastered Collection
 *                 under the GPL3 license. Source:
 *                 https://github.com/ElectronicArts/CnC_Remastered_Collection
 *
 ******************************************************************************/
#ifdef USE_MINIAUDIO

#include "audio_ambient.h"
#include "audio_manager.h"
#include "audio_util.h"
#include "tibsun_globals.h"


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioAmbientClass::AudioAmbientClass() :
	Handle(nullptr)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
AudioAmbientClass::~AudioAmbientClass()
{
    delete Handle;
    Handle = nullptr;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioAmbientClass::Start()
{
    if (!AudioManager.Is_Available() || Debug_Quiet || !ScoresPresent) {
        return false;
    }

    if (!Handle) {

        /**
         *  x
         */
        AudioManager.Audio_Set_Data("IONSTORM", IsAvailable, FileType, FileName);

        if (!IsAvailable) {
            AUDIO_DEBUG_WARNING("Ambient: Unable to find IONSTORM!\n");
            return false;
        }

        /**
         *  x
         */
        bool start = false;
        float delay = 1.0f; // So it does not start during the static screen effect.

        Handle = AudioManager.Play(FileName, AUDIO_GROUP_MUSIC_AMBIENT, Volume, 1.0f, 0.0f, AUDIO_PRIORITY_CRITICAL, -1, 0.0f, delay, start);
        ASSERT(Handle != nullptr);

        Handle->Set_Looping(true);

        Handle->Pause();

    }

    return Handle->Resume();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioAmbientClass::Stop()
{
    if (!Handle) {
        return false;
    }

    return Handle->Pause();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioAmbientClass::Is_Playing()
{
    if (!Handle) {
        return false;
    }

    return !Handle->Is_Paused() && Handle->Is_Playing();
}


#endif // USE_MINIAUDIO
