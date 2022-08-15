/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          AUDIO_NEWVOC.CPP
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

#include "audio_voc.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "audio_manager.h"
#include "audio_util.h"
#include "tactical.h"
#include "iomap.h"
#include "options.h"
#include "ccini.h"
#include <thread>


/**
 *  x
 * 
 *  @author: CCHyper
 */
FMODVocClass::FMODVocClass(const char *filename) :
    Name(filename),
    Sound(),
    FileName(),
    Priority(10),
    Volume(1.0f),
    Pitch(0.0f),
    Effects(AudioManagerClass::FMOD_DSP_NONE)
{
    Vocs.Add((VocClass *)this);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
FMODVocClass::~FMODVocClass()
{
    Vocs.Delete((VocClass *)this);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODVocClass::Read_INI(CCINIClass &ini)
{
    if (!ini.Is_Present(Name.Peek_Buffer())) {
        return;
    }

    char buffer[256];
    const char *name = Name.Peek_Buffer();

    Priority = ini.Get_Int(name, "Priority", Priority);
    Volume = ini.Get_Float_Clamp(name, "Volume", 0.0f, 1.0f, Volume);

    ini.Get_String(name, "Sound", buffer, sizeof(buffer));
    Sound = buffer;

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
bool FMODVocClass::Can_Play() const
{
    return AudioManager.Is_Available() && !Debug_Quiet && Name.Is_Not_Empty();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODVocClass::Play(float volume, int a2)
{
    if (!AudioManager.Is_Available()) {
        return INVALID_AUDIO_HANDLE;
    }

    if (Options.SoundVolume <= 0.0f) {
        return INVALID_AUDIO_HANDLE;
    }

    float vol = std::clamp((Options.SoundVolume * Volume * volume), 0.0f, 1.0f);
    float pan = 0.0f;
    float pitch = Pitch;
    int priority = Audio_To_FMOD_Priority(Priority);

    VocType id = (VocType)Vocs.ID((VocClass *)this);

    AudioManager.Play_Effect(id, vol, priority, pan, pitch, Effects);
    DEV_DEBUG_INFO("FMODVoc: Playing effect \"%s\".\n", Name.Peek_Buffer());

    /**
     *  New audio interface does not use sample handles, so return ISound instance as int.
     */
    return int(id);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODVocClass::Play(float volume)
{
    if (!AudioManager.Is_Available()) {
        return INVALID_AUDIO_HANDLE;
    }

    if (Options.SoundVolume <= 0.0f) {
        return INVALID_AUDIO_HANDLE;
    }

    float vol = std::clamp((Options.SoundVolume * Volume * volume), 0.0f, 1.0f);
    float pan = 0.0f;
    float pitch = Pitch;
    int priority = Audio_To_FMOD_Priority(Priority);

    VocType id = (VocType)Vocs.ID((VocClass *)this);

    AudioManager.Play_Effect(id, vol, priority, pan, pitch, Effects);
    DEV_DEBUG_INFO("FMODVoc: Playing effect \"%s\".\n", Name.Peek_Buffer());

    /**
     *  New audio interface does not use sample handles, so return ISound instance as int.
     */
    return int(id);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
static void Voc_Calculate_Pan_And_Volume(FMODVocClass &voc, Coordinate &coord, float &pan_result, float &volume_result)
{
    pan_result = 0.0f;
    volume_result = 1.0f;

    Cell cell = Coord_Cell(coord);

    if (!cell) {
        return;
    }

    float pan = 0.0f;
    float volume = 1.0f;

    Point2D pixel;

    if (!TacticalMap->Coord_To_Pixel(coord, pixel)) {

        int xpos = std::abs(pixel.X);
        int ypos = std::abs(pixel.Y);

    }

    pan_result = pan;
    volume_result = volume;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODVocClass::Play(Coordinate &coord)
{
    if (!AudioManager.Is_Available()) {
        return INVALID_AUDIO_HANDLE;
    }

    if (Options.SoundVolume <= 0.0f) {
        return INVALID_AUDIO_HANDLE;
    }

    float vol = 1.0f;
    float pan = 0.0f;
    float pitch = Pitch;
    int priority = Audio_To_FMOD_Priority(Priority);

    Voc_Calculate_Pan_And_Volume(*this, coord, pan, vol);

    vol = std::clamp(vol, 0.0f, 1.0f);
    pan = std::clamp(pan, -1.0f, 1.0f);

    VocType id = (VocType)Vocs.ID((VocClass *)this);

    AudioManager.Play_Effect(id, vol, priority, pan, pitch, Effects);
    DEV_DEBUG_INFO("FMODVoc: Playing effect \"%s\".\n", Name.Peek_Buffer());

    /**
     *  New audio interface does not use sample handles, so return ISound instance as int.
     */
    return int(id);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODVocClass::Play(VocType voc, int a2, float volume)
{
    if (voc == VOC_NONE) {
        return INVALID_AUDIO_HANDLE;
    }

    FMODVocClass *vocptr = (FMODVocClass *)Vocs[voc];
    if (!vocptr) {
        return INVALID_AUDIO_HANDLE;
    }

    return vocptr->Play(volume, a2);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODVocClass::Play(VocType voc, float volume)
{
    if (voc == VOC_NONE) {
        return INVALID_AUDIO_HANDLE;
    }

    FMODVocClass *vocptr = (FMODVocClass *)Vocs[voc];
    if (!vocptr) {
        return INVALID_AUDIO_HANDLE;
    }

    return vocptr->Play(volume);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODVocClass::Play(VocType voc, Coordinate &coord)
{
    if (voc == VOC_NONE) {
        return INVALID_AUDIO_HANDLE;
    }

    FMODVocClass *vocptr = (FMODVocClass *)Vocs[voc];
    if (!vocptr) {
        return INVALID_AUDIO_HANDLE;
    }

    return vocptr->Play(coord);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODVocClass::Process(CCINIClass &ini)
{
#if 1
    if (!ini.Is_Present("SoundList")) {
        return;
    }

    /**
     *  We do this here as VocClass has no global init.
     */
    //static bool _thread_one_time = false;
    //if (!_thread_one_time) {
    //    FMODVoc_Thread_Active = true;
    //    FMODVoc_Thread = std::thread(&FMODVocClass::Scan_Thread_Function);
    //    FMODVoc_Thread.detach(); // The thread is now free, and runs on its own.
    //    _thread_one_time = true;
    //}

    AudioManager.Clear_Effects();
    Clear();

    char buffer[32];

    int count = ini.Entry_Count("SoundList");

    /**
     *  Set the expected size of the audio engines effects vector. We need to
     *  use the ini entry count as the VocTypes are not a hardcoded list.
     */
    AudioManager.Set_Effects_Size(count);

    for (int index = 0; index < count; ++index) {

        if (ini.Get_String("SoundList", ini.Get_Entry("SoundList", index), buffer, sizeof(buffer)) > 0) {

            FMODVocClass *vocptr = new FMODVocClass(buffer);

            VocType voc = (VocType)index;

            vocptr->Read_INI(ini);

            Wstring name = vocptr->Name;

            if (vocptr->Sound.Is_Not_Empty()) {
                name = vocptr->Sound;
            }

            if (AudioManager.Submit_Effect(name, voc)) {
                //vocptr->Available = true;

            } else {
                DEBUG_WARNING("FMODVoc: Unable to find \"%s\"!\n", name.Peek_Buffer());
            }

        }

    }

#if 0//#ifndef NDEBUG
    DEV_DEBUG_WARNING("Voc dump...\n");
    for (int index = 0; index < Vocs.Count(); ++index) {
        DEV_DEBUG_WARNING("  %03d  %s\n", index, ((FMODVocClass *)Vocs[index])->Name.Peek_Buffer());
    }
#endif

    //FMODVoc_Run_Thread_Logic = true;
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODVocClass::Clear()
{
    while (Vocs.Count() > 0) {
        int index = Vocs.Count()-1;
        delete Vocs[index];
        Vocs.Delete(index);
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
VocType FMODVocClass::VocType_From_Voc(VocClass *voc)
{
    for (VocType index = VOC_FIRST; index < Vocs.Count(); ++index) {
        if (Vocs[index] == voc) {
            return index;
        }
    }

    return VOC_NONE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
VocType FMODVocClass::From_Name(const char *name)
{
    ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return VOC_NONE;
    }

    if (name != nullptr) {
        for (VocType index = VOC_FIRST; index < Vocs.Count(); ++index) {
            FMODVocClass *vocptr = (FMODVocClass *)Vocs[index];
            if (!strcasecmp(vocptr->Name.Peek_Buffer(), name)) {
                return index;
            }
        }
    }

    return VOC_NONE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
VocClass *FMODVocClass::Voc_From_Name(const char *name)
{
    ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return nullptr;
    }

    if (name != nullptr) {
        for (VocType index = VOC_FIRST; index < Vocs.Count(); ++index) {
            FMODVocClass *vocptr = (FMODVocClass *)Vocs[index];
            if (!strcasecmp(vocptr->Name.Peek_Buffer(), name)) {
                return (VocClass *)vocptr;
            }
        }
    }

    return nullptr;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
const char *FMODVocClass::INI_Name_From(VocType type)
{
    return (type != VOC_NONE && type < Vocs.Count() ? ((FMODVocClass *)Vocs[type])->Name.Peek_Buffer() : "<none>");
}


#endif // USE_FMOD_AUDIO
