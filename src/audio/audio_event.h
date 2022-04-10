/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_EVENT.H
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
#pragma once

#include "always.h"
#include "wstring.h"
#include "audio_driver.h"
#include "tibsun_defines.h"


/**
 *  Used in conjunction with the new audio driver interface.
 * 
 *  This struct tracks the sample and the playing handle for a specific
 *  sound effect or steam. Common uses for this would be to track a looping sample.
 */
typedef struct AudioEvent
{
    AudioEvent() :
        Voc(VOC_NONE),
        Filename("<unknown>"),
        //Sample(nullptr),
        Handle(INVALID_AUDIO_HANDLE)
    {
    }

    AudioEvent(VocType voc, Wstring &filename, /*AudioSample sample,*/ AudioHandle handle) :
        Voc(voc),
        Filename(filename),
        //Sample(sample),
        Handle(handle)
    {
    }

    ~AudioEvent()
    {
    }

    void Set_Looping()
    {
        //Audio_Driver()->Set_Handle_Loop(Handle)
    }

    bool Is_Playing() const
    {
        return IsPlaying;
    }

    bool Is_Looping() const
    {
        return IsLooping;
    }

    bool Stop()
    {
        Audio_Driver()->Stop_Handle_Playing(Handle);

        return true;
    }

    bool Pause()
    {
        return Audio_Driver()->Pause_Handle(Handle);
    }

    bool Resume()
    {
        return Audio_Driver()->Resume_Handle(Handle);
    }

    void Update_Status()
    {
        IsPlaying = (Handle != INVALID_AUDIO_HANDLE && Audio_Driver()->Is_Handle_Playing(Handle));
        IsLooping = Audio_Driver()->Is_Handle_Looping(Handle);
    }

    /**
     *  The C&C engine sound type.
     */
    VocType Voc;

    /**
     *  The filename of the sample.
     */
    Wstring Filename;

    /**
     *  Pointer to the sample file in memory.
     */
    //AudioSample Sample;

    /**
     *  Playing handle in the audio engine.
     */
    AudioHandle Handle;

    /**
     *  Is this sample currently playing?
     */
    bool IsPlaying;

    /**
     *  
     */
    bool IsLooping;

} AudioEvent;
