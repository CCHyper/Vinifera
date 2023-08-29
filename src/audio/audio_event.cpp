/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_MANAGER.CPP
 *
 *  @author        CCHyper
 * 
 *  @contributions mackron (miniaudio developer)
 *
 *  @brief         Installable MiniAudio audio driver.
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
#ifdef USE_MINIAUDIO

#include "audio_event.h"
#include "audio_handle.h"
#include "audio_manager.h"
#include "audio_voc.h"
#include "audio_util.h"


/**
 *  x
 *
 *  @author: CCHyper
 */
AudioEventHandleClass::AudioEventHandleClass(AudioVocClass &voc) :
    Voc(&voc),
    Handle(nullptr)
{
    Init(voc.FileName);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
AudioEventHandleClass::~AudioEventHandleClass()
{
    delete Handle;
    Handle = nullptr;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Init(Wstring filename)
{
    Handle = new AudioHandleClass(filename, AudioManager.SoundGroups[AUDIO_GROUP_EVENT], AUDIO_GROUP_EVENT);
    ASSERT(Handle != nullptr);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Start()
{
    if (!Handle) {
        AUDIO_DEBUG_ERROR("AudioEvent::Start - Handle is null!\n");
        return false;
    }

    return Handle->Start();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Stop(float fade_out_seconds, bool fade)
{
    if (!Handle) {
        //AUDIO_DEBUG_ERROR("AudioEvent::Stop - Handle is null!\n"); // Not really an error when stopping.
        return false;
    }

    return Handle->Stop(fade_out_seconds, fade);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Pause()
{
    if (!Handle) {
        AUDIO_DEBUG_ERROR("AudioEvent::Pause - Handle is null!\n");
        return false;
    }

    return Handle->Pause();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Resume()
{
    if (!Handle) {
        AUDIO_DEBUG_ERROR("AudioEvent::Resume - Handle is null!\n");
        return false;
    }

    return Handle->Resume();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Is_Playing() const
{
    if (!Handle) {
        AUDIO_DEBUG_ERROR("AudioEvent::Is_Playing - Handle is null!\n");
        return false;
    }

    return Handle->Is_Playing();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Set_Looping(bool loop)
{
    if (!Handle) {
        AUDIO_DEBUG_ERROR("AudioEvent::Set_Looping - Handle is null!\n");
        return false;
    }

    return Handle->Set_Looping(loop);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Set_Volume(float volume)
{
    if (!Handle) {
        AUDIO_DEBUG_ERROR("AudioEvent::Set_Volume - Handle is null!\n");
        return false;
    }

    return Handle->Set_Volume(volume);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Set_Pitch(float pitch)
{
    if (!Handle) {
        AUDIO_DEBUG_ERROR("AudioEvent::Set_Pitch - Handle is null!\n");
        return false;
    }

    return Handle->Set_Pitch(pitch);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Set_Pan(float pan)
{
    if (!Handle) {
        AUDIO_DEBUG_ERROR("AudioEvent::Set_Pan - Handle is null!\n");
        return false;
    }

    return Handle->Set_Pan(pan);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioEventHandleClass::Mute()
{
    return Set_Volume(0.0f);
}

#endif
