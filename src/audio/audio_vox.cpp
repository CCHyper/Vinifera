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
#ifdef USE_FMOD_AUDIO

#include "audio_vox.h"
#include "tibsun_globals.h"
#include "options.h"
#include "vox.h"
#include "scenario.h"
#include "ccini.h"
#include "audio_manager.h"
#include "audio_util.h"
#include "asserthandler.h"
#include <thread>


VoxType FMODVoxClass::SpeechBuffer[2];
int FMODVoxClass::SpeechBufferIndex = 0;


/**
 *  x
 */
static DynamicVectorClass<FMODVoxClass *> Voxs;


/**
 *  x
 * 
 *  @author: CCHyper
 */
FMODVoxClass::FMODVoxClass(Wstring filename) :
    Name(filename),
    Sound(),
    Volume(1.0f),
    Pitch(0.0f),
    Effects(AudioManagerClass::FMOD_DSP_NONE)
{
    Voxs.Add(this);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
FMODVoxClass::~FMODVoxClass()
{
    Voxs.Delete(this);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODVoxClass::Read_INI(CCINIClass &ini)
{
    if (!ini.Is_Present(Name.Peek_Buffer())) {
        return;
    }

    char buffer[256];
    const char *name = Name.Peek_Buffer();

    ini.Get_String(name, "Sound", buffer, sizeof(buffer));
    Sound = buffer;

    Volume = ini.Get_Float_Clamp(name, "Volume", 0.0f, 1.0f, Volume);
    Pitch = ini.Get_Float_Clamp(name, "Pitch", -5.0f, 5.0f, Pitch);

    if (ini.Get_String(name, "Effects", buffer, sizeof(buffer)) > 0) {

        char const * ptr = std::strtok(buffer, ",");
        while (ptr) {

            AudioManagerClass::DSPType dsp = AudioManager.DSP_From_Name(ptr);
            if (dsp != AudioManagerClass::FMOD_DSP_NONE) {
                Effects = AudioManagerClass::DSPFlagType(1 << dsp);
            }

            ptr = strtok(nullptr, ",");
        }
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODVoxClass::Scan()
{
    AudioManager.Clear_Speech();
    Clear();

    AudioManager.Set_Speech_Size(VOX_COUNT);

    CCINIClass ini;
    CCFileClass file("SPEECH.INI");

    ini.Load(file, false);

    char buffer[32];

    //int count = ini.Entry_Count("SpeechList");
    //for (int index = 0; index < count; ++index) {
    //
    //    if (ini.Get_String("SpeechList", ini.Get_Entry("SpeechList", index), buffer, sizeof(buffer)) > 0) {
    //
    //    }
    //
    //}

    for (VoxType vox = VOX_FIRST; vox < ARRAY_SIZE(Speech); ++vox) {

        FMODVoxClass *voxptr = new FMODVoxClass(Speech[vox]);
        ASSERT(voxptr != nullptr);

        voxptr->Read_INI(ini);

        Wstring name = voxptr->Name;

        if (voxptr->Sound.Is_Not_Empty()) {
            name = voxptr->Sound;
        }

        if (AudioManager.Submit_Speech(name, vox)) {
            //vocptr->Available = true;

        } else {
            DEBUG_WARNING("FMODVox: Unable to find \"%s\"!\n", name.Peek_Buffer());
        }

    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODVoxClass::Clear()
{
    while (Voxs.Count() > 0) {
        int index = Voxs.Count()-1;
        delete Voxs[index];
        Voxs.Delete(index);
    }
}


/**
 *  EVA speaks to the player.  
 * 
 *  @author: CCHyper
 */
void FMODVoxClass::Speak(VoxType voice, bool force)
{
    ASSERT(voice != VOX_NONE);
    ASSERT(voice < VOX_COUNT);

    if (Debug_Quiet || SpeechVolume <= 0 || !AudioManager.Is_Available()) {
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

    FMODVoxClass *voxptr = Voxs[voice];
    if (!voxptr) {
        return;
    }

    Wstring speech = voxptr->Name;

    // TODO CHECK!
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
void FMODVoxClass::Speak_AI()
{
    if (Debug_Quiet || SpeechVolume <= 0 || !AudioManager.Is_Available()) {
        return;
    }

    if (CurrentVoice == VOX_NONE && SpeakQueue == VOX_NONE) {
        return;
    }

    if (!SpeakTimer.Expired()) {
        return;
    }

    CurrentVoice = VOX_NONE;
    
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
    FMODVoxClass *voxptr = nullptr;
    if (newspeech.Is_Empty()) {
        SpeechBufferIndex = (SpeechBufferIndex + 1) % ARRAY_SIZE(SpeechRecord);

        if (AudioManager.Is_Supported_File_Available(Speech[SpeakQueue], newspeech)) {
            voxptr = Voxs[SpeakQueue];
            SpeechRecord[SpeechBufferIndex] = SpeakQueue;
        }
    
    }
    
    /**
     *  Since the speech file was found, play it.
     */
    if (newspeech.Is_Not_Empty() && voxptr) {

        VoxType vox = SpeakQueue;
        float vol = std::clamp((SpeechVolume * voxptr->Volume), 0.0f, 1.0f);
        float pitch = voxptr->Pitch;

        DEV_DEBUG_INFO("FMODVox: Playing speech \"%s\".\n", voxptr->Name.Peek_Buffer());
        AudioManager.Play_Speech(vox, vol, 1.5f /*pitch*/, voxptr->Effects);

        CurrentVoice = SpeakQueue;
    }

    SpeakQueue = VOX_NONE;
}


/**
 *  Forces the EVA voice to stop talking.
 * 
 *  @author: CCHyper
 */
void FMODVoxClass::Stop_Speaking()
{
    SpeakQueue = VOX_NONE;

    //DEV_DEBUG_INFO("FMODVox: Stopping speech \"%s\".\n", speech->Get_Filename().Peek_Buffer());

    AudioManager.Stop_Speech_Channel();
}


/**
 *  Checks to see if the eva voice is still playing.
 * 
 *  @author: CCHyper
 */
bool FMODVoxClass::Is_Speaking()
{
    Speak_AI();

    if (Debug_Quiet || SpeechVolume <= 0 || !AudioManager.Is_Available()) {
        return false;
    }

    return SpeakQueue != VOX_NONE && AudioManager.Is_Speech_Channel_Playing();
}


/**
 *  Sets the global speech volume to that specified.
 * 
 *  @author: CCHyper
 */
void FMODVoxClass::Set_Speech_Volume(int vol)
{
    SpeechVolume = std::clamp(vol, 0, 255);

    if (Debug_Quiet || !AudioManager.Is_Available()) {
        return;
    }

    AudioManager.Set_Speech_Volume(Audio_iVolume_To_fVolume(vol));
}

#endif
