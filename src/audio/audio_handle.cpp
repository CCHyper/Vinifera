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

#include "audio_handle.h"
#include "audio_manager.h"
#include "audio_util.h"
#include "ccfile.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <chrono>
#include <thread>

#include <miniaudio/miniaudio.h>


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Load(Wstring filename, ma_sound_group *group)
{
    ma_result result;

    /**
     *  Sound is already loaded, return true.
     */
    if (Sound) {
        AUDIO_DEBUG_WARNING("AudioHandle::Load - Sound already loaded!\n");
        return true;
    }

    if (!CCFileClass(filename.Peek_Buffer()).Is_Available()) {
        AUDIO_DEBUG_ERROR("AudioHandle::Load - Unable to find \"%s\"!\n", filename.Peek_Buffer());
        return false;
    }

    //ma_audio_buffer_config audioConfig = ma_audio_buffer_config_init(format, channels, sizeInFrames, pData, NULL);
    //
    //result = ma_audio_buffer_init(&audioConfig, &AudioBuffer);
    //if (result != MA_SUCCESS) {
    //    AUDIO_DEBUG_ERROR("AudioHandle::Load - ma_audio_buffer_init failed (%s)!\n", ma_result_description(result));
    //    Free();
    //    return nullptr;
    //}

    /**
     *  Create a new sound object.
     */
    Sound = new ma_sound;
    ASSERT(Sound != nullptr);

    ma_sound_flags flags = ma_sound_flags(MA_SOUND_FLAG_NO_SPATIALIZATION);

    result = ma_sound_init_from_file(AudioManager.Engine, filename.Peek_Buffer(), flags, group, nullptr, Sound);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("AudioHandle::Load - ma_sound_init_from_file failed (%s)!\n", ma_result_description(result));
        Free();
        return nullptr;
    }

    /**
     *  x
     */
    ma_sound_set_end_callback(Sound, Sound_End_Callback, this);

    Filename = filename;

    return Sound;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Free()
{
    ma_sound_uninit(Sound);
    Sound = nullptr;
    //delete Sound;

    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
Wstring AudioHandleClass::Get_Filename() const
{
    return  Filename;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Is_Valid() const
{
    if (!Sound) {
        //AUDIO_DEBUG_ERROR("AudioHandle::Is_Valid - Sound is null!\n");  // Not really an error when making query.
        return false;
    }

    return Sound->pDataSource;
}

bool AudioHandleClass::Start()
{
    ma_result result;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Start - Sound is null!\n");
        return false;
    }

    result = ma_sound_start(Sound);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("AudioHandle::Start - ma_sound_start failed (%s)!\n", ma_result_description(result));
        return false;
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Stop(float seconds, bool fade, bool reset)
{
    ma_result result;

    if (!Sound) {
        //AUDIO_DEBUG_ERROR("AudioHandle::Stop - Sound is null!\n"); // Not really an error when stopping.
        return false;
    }

    if (fade && seconds > 0.0f) {

        /**
         *  First we set the sound to fade out.
         */
        ma_sound_set_fade_in_milliseconds(Sound, Get_Volume(), 0.0f, 1000 * seconds);

        /**
         *  Then tell it when to stop (which will be at the end of the fading slope).
         */
        ma_sound_set_stop_time_in_milliseconds(Sound, 1000 * seconds);

    } else if (!fade && seconds > 0.0f) {

        /**
         *  Set the time in which the sound should end.
         */
        ma_sound_set_stop_time_in_milliseconds(Sound, 1000 * seconds);

    } else {

        /**
         *  
         */

        result = ma_sound_stop(Sound);
        if (result != MA_SUCCESS) {
            AUDIO_DEBUG_ERROR("AudioHandle::Stop - ma_sound_stop failed (%s)!\n", ma_result_description(result));
            return false;
        }

        if (reset) {
            result = ma_sound_seek_to_pcm_frame(Sound, 0);
            if (result != MA_SUCCESS) {
                AUDIO_DEBUG_ERROR("AudioHandle::Stop - ma_sound_seek_to_pcm_frame failed (%s)!\n", ma_result_description(result));
            }
        }

    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Pause()
{
    ma_result result;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Pause - Sound is null!\n");
        return false;
    }

    result = ma_sound_stop(Sound);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("AudioHandle::Pause - ma_sound_stop failed (%s)!\n", ma_result_description(result));
        return false;
    }

    IsPaused = true;

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Resume()
{
    ma_result result;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Resume - Sound is null!\n");
        return false;
    }

    if (!IsPaused) {
        AUDIO_DEBUG_ERROR("AudioHandle::Resume - Attmpted to assume a sound that is not paused!\n");
        return false;
    }

    result = ma_sound_start(Sound);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("AudioHandle::Pause - ma_sound_start failed (%s)!\n", ma_result_description(result));
        return false;
    }

    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Restart()
{
    ma_result result;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Restart - Sound is null!\n");
        return false;
    }

    result = ma_sound_seek_to_pcm_frame(Sound, 0);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("AudioHandle::Restart - ma_sound_seek_to_pcm_frame failed (%s)!\n", ma_result_description(result));
        return false;
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Is_Playing() const
{
    ma_bool32 result;

    if (!Sound) {
        //AUDIO_DEBUG_ERROR("AudioHandle::Is_Playing - Sound is null!\n");
        return false; // If sound is null, then this handle is finished with.
    }

    result = ma_sound_is_playing(Sound);

    return result;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Is_Paused() const
{
    return IsPaused;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Is_Fading_Out() const
{
    ma_bool32 result;

    result = ma_sound_is_playing(Sound);

    if (!result) {
        return false;
    }

    return ma_sound_get_current_fade_volume(Sound) != 0.0f;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Is_Fading_In() const
{
    ma_bool32 result;

    result = ma_sound_is_playing(Sound);

    if (!result) {
        return false;
    }

    return ma_sound_get_current_fade_volume(Sound) != 1.0f;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Is_Finished() const
{
    ma_bool32 result;

    if (!Sound) {
        //AUDIO_DEBUG_ERROR("AudioHandle::Is_Finished - Sound is null!\n");
        return true; // If sound is null, then this handle is finished with.
    }

    if (Is_Paused() || Is_Looping()) {
        return false;
    }

    if (IsFinishedCallback) {
        return true;
    }

    result = ma_sound_at_end(Sound);

    return result;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Fade_In(float seconds)
{
    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Fade_In - Sound is null!\n");
        return false;
    }

    ma_sound_set_fade_in_milliseconds(Sound, 0.0f, Get_Volume(), 1000 * seconds);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Fade_Out(float seconds)
{
    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Fade_Out - Sound is null!\n");
        return false;
    }

    ma_sound_set_fade_in_milliseconds(Sound, Get_Volume(), 0.0f, 1000 * seconds);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Is_Looping() const
{
    ma_bool32 result;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Is_Looping - Sound is null!\n");
        return false;
    }

    result = ma_sound_is_looping(Sound);

    return result;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
float AudioHandleClass::Get_Volume() const
{
    ma_float result;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Get_Volume - Sound is null!\n");
        return false;
    }

    result = ma_sound_get_volume(Sound);

    return result;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
float AudioHandleClass::Get_Pitch() const
{
    ma_float result;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Get_Pitch - Sound is null!\n");
        return false;
    }

    result = ma_sound_get_pitch(Sound);

    return result;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
float AudioHandleClass::Get_Pan() const
{
    ma_float result;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Get_Pan - Sound is null!\n");
        return false;
    }

    result = ma_sound_get_pan(Sound);

    return result;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
float AudioHandleClass::Get_Time() const
{
    ma_float time = -1;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Get_Time - Sound is null!\n");
        return false;
    }

    ma_sound_get_cursor_in_seconds(Sound, &time);

    return time;
}


/**
 *  x
 * 
 *  #NOTE: ma_sound_get_length_in_seconds will return zero for Vorbis sounds!
 *
 *  @author: CCHyper
 */
float AudioHandleClass::Get_Length() const
{
    ma_float time = -1;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Get_Length - Sound is null!\n");
        return false;
    }

    ma_sound_get_length_in_seconds(Sound, &time);

    return time;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
AudioPriorityType AudioHandleClass::Get_Priority() const
{
    return Priority;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
unsigned AudioHandleClass::Get_Sample_Rate() const
{
    ma_result result;

    if (!Sound) {
        return 0;
    }

    ma_uint32 sampleRate;

    result = ma_data_source_get_data_format(Sound->pDataSource, nullptr, nullptr, &sampleRate, nullptr, 0);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Time - ma_data_source_get_data_format failed (%s)!\n", ma_result_description(result));
        return 0;
    }

    return sampleRate;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
unsigned AudioHandleClass::Get_Channels() const
{
    ma_result result;

    if (!Sound) {
        return 0;
    }

    ma_uint32 channels;

    result = ma_data_source_get_data_format(Sound->pDataSource, nullptr, &channels, nullptr, nullptr, 0);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Time - ma_data_source_get_data_format failed (%s)!\n", ma_result_description(result));
        return 0;
    }

    return channels;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Set_Looping(bool loop)
{
    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Looping - Sound is null!\n");
        return false;
    }

    ma_sound_set_looping(Sound, loop);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Set_Volume(float volume)
{
    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Volume - Sound is null!\n");
        return false;
    }

    ma_sound_set_volume(Sound, volume);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Set_Pitch(float pitch)
{
    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Pitch - Sound is null!\n");
        return false;
    }

    ma_sound_set_pitch(Sound, pitch);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Set_Pan(float pan)
{
    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Pan - Sound is null!\n");
        return false;
    }

    ma_sound_set_pan(Sound, pan);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Set_Time(float time_in_seconds)
{
    ma_result result;
    ma_uint64 lengthInPCMFrames;
    ma_uint32 sampleRate;

    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Time - Sound is null!\n");
        return false;
    }

    if (Sound->pDataSource == nullptr) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Time - pDataSource is null!\n");
        return false;
    }

    float timePCM = 0;

    result = ma_data_source_get_data_format(Sound->pDataSource, nullptr, nullptr, &sampleRate, nullptr, 0);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Time - ma_data_source_get_data_format failed (%s)!\n", ma_result_description(result));
        return false;
    }

    lengthInPCMFrames = time_in_seconds * sampleRate;

    result = ma_sound_seek_to_pcm_frame(Sound, lengthInPCMFrames);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Time - ma_sound_seek_to_pcm_frame failed (%s)!\n", ma_result_description(result));
        return false;
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Set_Priority(AudioPriorityType priority)
{
    Priority = priority;

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Set_Delay(float delay_in_seconds)
{
    ma_sound_set_start_time_in_milliseconds(Sound, delay_in_seconds * 1000 + ma_engine_get_time_in_milliseconds(AudioManager.Engine));

    IsDelaySet = true;

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Set_Limit(int limit)
{
    ConcurrentLimit = limit;

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioHandleClass::Seek(float time)
{
    ma_result result;

    result = ma_sound_seek_to_pcm_frame(Sound, time * ma_engine_get_sample_rate(ma_sound_get_engine(Sound)));
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_Time - ma_sound_seek_to_pcm_frame failed (%s)!\n", ma_result_description(result));
        return false;
    }

    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioHandleClass::Set_User_Data(void *user_data)
{
    if (!Sound) {
        AUDIO_DEBUG_ERROR("AudioHandle::Set_User_Data - Sound is null!\n");
        return false;
    }

    Sound->pEndCallbackUserData = user_data;

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void AudioHandleClass::Sound_End_Callback(void *pUserData, ma_sound *pSound)
{
    AudioHandleClass *handle = reinterpret_cast<AudioHandleClass *>(pUserData);

    // TODO: Do we need to check for looping flags?
    if (handle->Is_Looping()) {
        return;
    }

#ifndef NDEBUG
    //AUDIO_DEBUG_WARNING("AudioHandle::Callback - %s has finished!\n", handle->Get_Filename().Peek_Buffer());
#endif

    /**
     *  Flag this sound handle as finished and ready to cleanup.
     */
    handle->IsFinishedCallback = true;
}

#endif
