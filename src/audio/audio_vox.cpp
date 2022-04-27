/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          AUDIO_NEWVOX.CPP
 *
 *  @author        CCHyper, tomsons26
 *
 *  @brief         
 *
 *  @license       ProjectTSYR is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 ProjectTSYR is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "audio_vox.h"
#include "tibsun_globals.h"
#include "options.h"
#include "vox.h"
#include "scenario.h"
#include "audio_driver.h"
#include "audio_util.h"
#include "asserthandler.h"


Wstring VoxClass::SpeechBuffer[2] = {};
int VoxClass::SpeechBufferIndex = 0;


/**
 *  x
 */
static DynamicVectorClass<VoxClass *> Voxs;


/**
 *  x
 * 
 *  @author: CCHyper
 */
VoxClass::VoxClass(Wstring filename) :
    Filename(filename),
    Sound(),
    Volume(1.0f)
{
    Voxs.Add(this);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
VoxClass::~VoxClass()
{
    Voxs.Delete(this);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void VoxClass::Init()
{
    for (int index = 0; index < ARRAY_SIZE(Speech); ++index) {
        VoxClass *vox = new VoxClass(Speech[index]);
        ASSERT(vox != nullptr);
    }
}


/**
 *  Preload the audio engine with the speech files.
 * 
 *  @author: CCHyper
 */
void VoxClass::Preload()
{
    if (Audio_Driver()->Is_Available()) {

        /**
         *  Clear the existing speech bank (user may have changed side or disk).
         */
        Audio_Driver()->Clear_Sample_Bank(SAMPLE_SPEECH);

        //for (int index = 0; index < ARRAY_SIZE(Speech); ++index) {
        for (int index = 0; index < Voxs.Count(); ++index) {

            //Wstring speechname = Speech[index];
            Wstring speechname = Voxs[index]->Filename;
            speechname.As_Upper();

            /**
             *  Check to see if the speech file exists, request preload of asset if available.
             */
            if (!speechname.Is_Empty()) {
                if (Audio_Driver()->Is_Audio_File_Available(speechname)) {
                    bool available = Audio_Driver()->Request_Preload(speechname, SAMPLE_SPEECH);
                    if (!available) {
                        DEBUG_WARNING("Speech: Failed to preload \"%s\"!\n", speechname.Peek_Buffer());
                    }
                } else {
                    DEV_DEBUG_WARNING("Speech: Unable to find \"%s\"!\n", speechname.Peek_Buffer());
                }
            }
        }

        /**
         *  Flag the preloader thread to begin.
         */
        Audio_Driver()->Start_Preloader(SAMPLE_SPEECH);
    }
}


/**
 *  EVA speaks to the player.  
 * 
 *  @author: CCHyper
 */
void VoxClass::Speak(VoxType voice, bool force)
{
    ASSERT(voice != VOX_NONE);
    ASSERT(voice < VOX_COUNT);

    if (Debug_Quiet || SpeechVolume <= 0 || !Audio_Driver()->Is_Available()) {
        return;
    }

    if (voice == VOX_NONE || voice == SpeakQueue || voice == CurrentVoice) {
        return;
    }

    if (SpeakQueue != VOX_NONE) {
        return;
    }

    if (!IsSpeechAllowed) {
        return;
    }

    //Wstring speech = Speech[voice];
    VoxClass *voxptr = Voxs[voice];
    Wstring speech = voxptr->Filename;

    /**
     *  Only allow EVA speeches to be played by the speech handler. This has been
     *  expanded compared to the original code so up to 99 sides are supported, based
     *  on the currently set SpeechSide of the mission.
     */
    if (!speech.Contains("00-") || !speech.Contains("01-")) {
        return;
    }
    //char buffer[16];
    //std::snprintf(buffer, sizeof(buffer), "%02d-", Scen->SpeechSide);
    //if (!speech.Contains(buffer)) {
    //    return;
    //}

#ifndef NDEBUG
    DEV_DEBUG_INFO("Queued speech \"%s\".\n", speech.Peek_Buffer());
#endif

    SpeakQueue = voice;

    if (force) {
        SpeakTimer = 0;
        Speak_AI();
        return;
    }

    if (SpeakTimer.Expired()) {
        Speak_AI();
        return;
    }

    SpeakTimer = 60;
}


/**
 *  Handles starting the EVA voices.  
 * 
 *  @author: CCHyper
 */
void VoxClass::Speak_AI()
{
    if (Debug_Quiet || SpeechVolume <= 0 || !Audio_Driver()->Is_Available()) {
        return;
    }

    if (CurrentVoice == VOX_NONE && SpeakQueue == VOX_NONE) {
        return;
    }

    Wstring currentspeech = SpeechBuffer[SpeechBufferIndex];

    if (Audio_Driver()->Is_Playing(STREAM_SPEECH, currentspeech)) {
        return;
    }

    if (!SpeakTimer.Expired()) {
        return;
    }

    CurrentVoice = VOX_NONE;

    if (SpeakQueue == VOX_NONE) {
        return;
    }
    
    /**
     *  Try to find a previously loaded copy of the EVA speech in one of the
     *  speech buffers.
     */
    Wstring newspeech;
    for (int index = 0; index < ARRAY_SIZE(SpeechRecord); ++index) {
        if (SpeechRecord[index] == SpeakQueue) {
            break;
        }
    }
    
    /**
     *  If a previous copy could not be located, then load the requested
     *  voice into the oldest buffer available.
     */
    VoxClass *voxptr = nullptr;
    if (newspeech.Is_Empty()) {
        SpeechBufferIndex = (SpeechBufferIndex + 1) % ARRAY_SIZE(SpeechRecord);

        if (Audio_Driver()->Is_Audio_File_Available(Speech[SpeakQueue])) {
            //newspeech = Speech[SpeakQueue];
            voxptr = Voxs[SpeakQueue];
            newspeech = voxptr->Filename;
            SpeechRecord[SpeechBufferIndex] = SpeakQueue;
        }
    
    }
    
    /**
     *  Since the speech file was found, play it.
     */
    if (newspeech.Is_Not_Empty() && voxptr) {
#ifndef NDEBUG
        DEV_DEBUG_INFO("Playing speech \"%s\".\n", newspeech.Peek_Buffer());
#endif

        //Audio_Driver()->Play(STREAM_SPEECH, newspeech, Audio_iVolume_To_fVolume(SpeechVolume));
        Audio_Driver()->Play(STREAM_SPEECH, newspeech, Audio_iVolume_To_fVolume(voxptr->Volume));
        CurrentVoice = SpeakQueue;
    }
    
    SpeakQueue = VOX_NONE;
}


/**
 *  Forces the EVA voice to stop talking.
 * 
 *  @author: CCHyper
 */
void VoxClass::Stop_Speaking()
{
    SpeakQueue = VOX_NONE;

    Wstring speech = SpeechBuffer[SpeechBufferIndex];

#ifndef NDEBUG
    DEV_DEBUG_INFO("Stopping speech \"%s\".\n", speech.Peek_Buffer());
#endif

    Audio_Driver()->Stop(STREAM_SPEECH, speech);
}


/**
 *  Checks to see if the eva voice is still playing.
 * 
 *  @author: CCHyper
 */
bool VoxClass::Is_Speaking()
{
    Speak_AI();

    if (Debug_Quiet || SpeechVolume <= 0 || !Audio_Driver()->Is_Available()) {
        return false;
    }

    Wstring currentspeech = SpeechBuffer[SpeechBufferIndex];

    return SpeakQueue != VOX_NONE && Audio_Driver()->Is_Playing(STREAM_SPEECH, currentspeech);
}


/**
 *  Sets the global speech volume to that specified.
 * 
 *  @author: CCHyper
 */
void VoxClass::Set_Speech_Volume(int vol)
{
    SpeechVolume = std::clamp(vol, 0, 255);

    if (Debug_Quiet || !Audio_Driver()->Is_Available()) {
        return;
    }

    Audio_Driver()->Set_Speech_Volume(Audio_iVolume_To_fVolume(vol));
}
