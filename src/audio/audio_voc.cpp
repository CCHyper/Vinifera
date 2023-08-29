/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_NEWVOC.CPP
 *
 *  @author        CCHyper, tomsons26
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
#ifdef USE_MINIAUDIO

#include "audio_voc.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "audio_manager.h"
#include "audio_util.h"
#include "audio_event.h"
#include "tactical.h"
#include "iomap.h"
#include "options.h"
#include "ccini.h"
#include <thread>


/**
 *  These are the defaults for all sounds loaded from the ini database.
 */
static int DefaultLimit = 5;
static int DefaultRange = 10;
static AudioSoundType DefaultType = AUDIO_SOUND_SCREEN;
static AudioControlType DefaultControl = AUDIO_CONTROL_NORMAL;
static int DefaultPriority = AudioManager.Priority_To_AudioPriority(10);
static float DefaultVolume = AUDIO_VOLUME_MAX;
static float DefaultMinVolume = AUDIO_VOLUME_MIN;
static float DefaultMaxVolume = AUDIO_VOLUME_MAX;


/**
 *  x
 */
AudioVocClass *VocEmblem = nullptr;


/**
 *  x
 * 
 *  @author: tomsons26, CCHyper
 */
static void Voc_Calculate_Pan_And_Volume(AudioVocClass &voc, Coordinate &coord, float &pan_result, float &volume_result)
{
    if (!voc.Available && !coord) {
        return;
    }

    float tact_center_x = (float)TacticalRect.Width / 2.0f;
    float tact_center_y = (float)TacticalRect.Height / 2.0f;

    float tact_center_x_sq = tact_center_x + tact_center_x;

    float range = CELL_PIXEL_W * voc.Range;

    float volume = AUDIO_VOLUME_MIN;
    float pan = 0.0f;

    /**
     *  Adjust the volume of the sound.
     */
    Point2D pixel;
    //if (!TacticalMap->Coord_To_Pixel(coord, pixel)) {
    TacticalMap->Coord_To_Pixel(coord, pixel);

        float x_delta = (float)pixel.X - tact_center_x;
        float y_delta = (float)pixel.Y - tact_center_y;

        float abs_dist_x = (float)std::abs((int)x_delta);
        float abs_dist_y = (float)std::abs((int)y_delta);

        // ...if not played locally.
        if ((voc.Type & AUDIO_SOUND_LOCAL) == 0) {
            abs_dist_x = abs_dist_x - tact_center_x;
            abs_dist_y = abs_dist_y - tact_center_y;
            if (abs_dist_x < 0.0f) {
                abs_dist_x = 0.0f;
            }
            if (abs_dist_y < 0.0f) {
                abs_dist_y = 0.0f;
            }
        }

        float abs_dist_y_sq = abs_dist_y + abs_dist_y;

        if (abs_dist_x < range && abs_dist_y_sq < range && range > 0.0f) {
            float subval = abs_dist_x > abs_dist_y_sq ? abs_dist_x : abs_dist_y_sq;
            volume = (range - subval) / range;
        }

        /**
         *  ...if played globally.
         */
        if ((voc.Type & AUDIO_SOUND_GLOBAL) != 0) {
            volume = std::max(volume, voc.MinVolume);
        }

    //}

    if (volume < 0.05) {
        volume = AUDIO_VOLUME_MIN;
    }

    /**
     *  Calculate the stereo panning depending on the direction of the source.
     * 
     *  -1.0 is the left speaker, 1.0 is the right speaker, 0.0 is split between left and right speakers.
     */
    if (x_delta < -tact_center_x_sq) {
        x_delta = -tact_center_x_sq;

    } else if (x_delta > tact_center_x_sq) {
        x_delta = tact_center_x_sq;
    }

    constexpr float pan_scale = 16384.0f;

    pan = (x_delta * (pan_scale / 2.0f) / tact_center_x_sq + (pan_scale / 2.0f));

    // normalize range to -1.0 to +1.0 range.
    pan = (pan / (pan_scale / 2.0f)) - 1.0f;

    /**
     *  Finally, clamp the results within the expected ranges.
     */
    volume_result = std::clamp(volume, AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX);
    pan_result = std::clamp(pan, -1.0f, 1.0f);

    //AUDIO_DEBUG_INFO("Voc::Play - V %f P %f\n", volume_result, pan_result);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
static bool Voc_Play(AudioVocClass &voc, Coordinate &coord = Coordinate(0,0,0), int variation = 0, float volume = 1.0f)
{
    /**
     *  x
     */
    if (!AudioManager.Is_Available() || Debug_Quiet) {
        return false;
    }

    /**
     *  x
     */
    if (AudioManager.Get_Group_Volume(AUDIO_GROUP_SOUND_EFFECT) <= AUDIO_VOLUME_MIN) {
        return false;
    }

    /**
     *  x
     */
    if (voc.FileType == AUDIO_TYPE_NONE) {
        AUDIO_DEBUG_ERROR("Voc::Play - Voc has invalid file type!\n");
        return false;
    }

#if 0
    /**
     *  Was the voc file found during initialisation?
     */
    if (!vocptr->Available) {
        AUDIO_DEBUG_ERROR("Voc::Play - Voc file is unavailable!\n");
        return false;
    }
#endif

    Wstring filename;

    /**
     *  If the RANDOM flag has been set, pick a random sound from the list.
     */
    if (voc.Sounds.Count()> 0 && (voc.Control & AUDIO_CONTROL_RANDOM) == 0) {
        Wstring sound = voc.Sounds[Sim_Random_Pick(0, voc.Sounds.Count()-1)];
        filename = AudioManager.Build_Filename_From_Type(voc.FileType, sound);
    
    } else {
        filename = AudioManager.Build_Filename_From_Type(voc.FileType, voc.Name);
    }

    /**
     *  x
     */
    if (filename.Is_Empty()) {
        AUDIO_DEBUG_ERROR("Voc::Play - Voc %s has a null filename!\n", voc.Name.Peek_Buffer());
        return false;
    }

    /**
     *  x
     */
    VocType id = (VocType)Vocs.ID((VocClass*)&voc);

    AudioPriorityType priority = AudioManager.Priority_To_AudioPriority(voc.Priority);
    int type = voc.Type;
    int control = voc.Control;

    /**
     *  If we were given a coord, check to see if this sound is subject to certain rules.
     */
    if (coord) {

        Cell cell = Coord_Cell(coord);
        if (!cell) {
            return false;
        }

        /**
         *  Can this sound only be played if the cell is revealed?
         */
        if ((voc.Type & AUDIO_SOUND_SHROUDED) != 0) {
            if (Map[cell].IsVisible|| Map[cell].IsFogVisible) {
                return false;
            }

        /**
         *  Can this sound only be played if the cell is unrevealed?
         */
        } else if ((voc.Type & AUDIO_SOUND_UNSHROUDED) != 0) {
            if (!Map[cell].IsVisible && !Map[cell].IsFogVisible) {
                return false;
            }
        }

    }

    float vol = volume;
    float pan = 0.0f;

    /**
     *  x
     */
    float vshift = AUDIO_VOLUME_MIN;
    if (voc.VolumeShift.X != 0 || voc.VolumeShift.Y != 0) {
        vshift = Sim_Random_Pick_Float((float(voc.VolumeShift.X) / float(AUDIO_VSHIFT_MAX)), (float(voc.VolumeShift.Y) / float(AUDIO_VSHIFT_MAX)));
    }

    /**
     *  Voice and UI sounds are not subject to volume and panning adjustments.
     */
    if ((type & AUDIO_SOUND_VOICE) == 0 && (type & AUDIO_SOUND_UI) == 0) {

        /**
         *  Adjust the volume and panning of the sound depending on its
         *  location to the tactical screen.
         */
        if (coord) {
            Voc_Calculate_Pan_And_Volume(voc, coord, pan, vol);
        }

    }

    vol = std::min(std::clamp((voc.Volume * vol + vshift), AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX), voc.MaxVolume);

    /**
     *  If the volume drops below this level, just skip it. Otherwise the sounds
     *  will bleed into adjacent areas a little too far.
     */
    if (vol < 0.05f) {
        return false;
    }

    /**
     *  x
     */
    float fshift = 0.0f;
    if (voc.FrequencyShift.X != 0 || voc.FrequencyShift.Y != 0) {
        fshift = Sim_Random_Pick_Float((float(voc.FrequencyShift.X) / float(AUDIO_FSHIFT_MAX)), (float(voc.FrequencyShift.Y) / float(AUDIO_FSHIFT_MAX)));
    }

    float pitch = 1.0f + fshift;

    /**
     *  x
     */
    //AUDIO_DEBUG_INFO("Voc::Play - About to call AudioManager.Play with \"%s\".\n", filename.Peek_Buffer());
    AudioHandleClass *handle = AudioManager.Play(filename, AUDIO_GROUP_SOUND_EFFECT, vol, pitch, pan, priority, voc.Limit);
    if (!handle) {
        DEBUG_ERROR("Voc::Play - Failed to play \"%s\"!\n", voc.Name.Peek_Buffer());
        return false;
    }

    //AUDIO_DEBUG_INFO("Voc::Play - Playing effect \"%s\".\n", voc.Name.Peek_Buffer());

    /**
     *  Store the handle to the sound we are playing.
     */
    voc.Handle = handle;

    return true;
}



/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioVocClass::AudioVocClass(const char *name) :
    Handle(nullptr),
    Name(name),
    Sounds(),
    FileType(AUDIO_TYPE_AUD),
    FileName(),
    Available(false),
    Priority(DefaultPriority),
    Limit(DefaultLimit),
    Range(DefaultRange),
    Type(DefaultType),
    Volume(DefaultVolume),
    MinVolume(DefaultMinVolume),
    MaxVolume(DefaultMaxVolume),
    VolumeShift(AUDIO_VSHIFT_MIN,AUDIO_VSHIFT_MIN),
    FrequencyShift(AUDIO_VSHIFT_MIN,AUDIO_VSHIFT_MIN),
    Control(AUDIO_CONTROL_NORMAL)
{
    Name.To_Upper();

    Vocs.Add((VocClass *)this);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioVocClass::~AudioVocClass()
{
    Vocs.Delete((VocClass *)this);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioVocClass::Read_INI(CCINIClass &ini)
{
    if (!ini.Is_Present(Name.Peek_Buffer())) {
        return;
    }

    char buffer[256];
    const char *name = Name.Peek_Buffer();

    Sounds = ini.Get_Strings(name, "Sounds", Sounds);

    Priority = ini.Get_Int(name, "Priority", Priority);
    Volume = ini.Get_Float_Clamp(name, "Volume", AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX, Volume);

    MinVolume = ini.Get_Float_Clamp(name, "MinVolume", AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX, MinVolume);
    //MaxVolume = ini.Get_Float_Clamp(name, "MaxVolume", AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX, MaxVolume); // Not to be loaded from the ini database.

    VolumeShift = ini.Get_Point(name, "VShift", VolumeShift);
    VolumeShift.X = std::clamp(VolumeShift.X, AUDIO_VSHIFT_MIN, AUDIO_VSHIFT_MAX);
    VolumeShift.Y = std::clamp(VolumeShift.Y, AUDIO_VSHIFT_MIN, AUDIO_VSHIFT_MAX);

    FrequencyShift = ini.Get_Point(name, "FShift", FrequencyShift);
    FrequencyShift.X = std::clamp(FrequencyShift.X, AUDIO_FSHIFT_MIN, AUDIO_FSHIFT_MAX);
    FrequencyShift.Y = std::clamp(FrequencyShift.Y, AUDIO_FSHIFT_MIN, AUDIO_FSHIFT_MAX);

    if (ini.Get_String(name, "Type", buffer, sizeof(buffer) - 1) > 0) {

        static struct _typestruct
        {
            Wstring name;
            AudioSoundType type;

        } _sound_types[] = {
            "NORMAL", AUDIO_SOUND_NORMAL,
            //"VIOLENT", ,
            //"MOVEMENT", ,
            //"QUIET", ,
            //"LOUD", ,
            "GLOBAL", AUDIO_SOUND_GLOBAL,
            //"SCREEN", ,
            "LOCAL", AUDIO_SOUND_LOCAL,
            //"PLAYER", ,
            //"ALLIES", ,
            //"ENEMIES", ,
            //"EVERYONE", ,
            //"GUN_SHY", ,
            //"NOISE_SHY", ,
            "UNSHROUDED", AUDIO_SOUND_UNSHROUDED,
            "SHROUDED", AUDIO_SOUND_SHROUDED,
            //"AMBIENT", ,
            //"VOICE", ,
            //"UI", ,
        };

        int flags = 0;
        Wstring tmp;
        const char * type = strtok(buffer, ",");

        while (type) {

            tmp = type;
            tmp.To_Upper();

            for (int i = 0; i < ARRAY_SIZE(_sound_types); ++i) {
                if (_sound_types[i].name == tmp) {
                    flags |= _sound_types[i].type;
                }
            }

            type = strtok(nullptr, ",");
        }

        Type = AudioSoundType(flags);
    }

    if (ini.Get_String(name, "Control", buffer, sizeof(buffer)-1) > 0) {

        static struct _controlstruct
        {
            Wstring name;
            AudioControlType control;

        } _control_types[] = {
            "NORMAL", AUDIO_CONTROL_NORMAL,
            //"LOOP", AUDIO_CONTROL_LOOP,
            "RANDOM", AUDIO_CONTROL_RANDOM,
            //"SEQUENTIAL", AUDIO_CONTROL_SEQUENTIAL,
            //"ALL", AUDIO_CONTROL_ALL,
            "PREDELAY", AUDIO_CONTROL_PREDELAY,
            "QUEUE", AUDIO_CONTROL_QUEUE,
            //"QUEUED_INTERUPT", AUDIO_CONTROL_QUEUED_INTERUPT,
            //"INTERUPT", AUDIO_CONTROL_INTERUPT,
            //"ATTACK", AUDIO_CONTROL_ATTACK,
            //"DECAY", AUDIO_CONTROL_DECAY,
            "AMBIENT", AUDIO_CONTROL_AMBIENT,
        };

        int flags = 0;
        Wstring tmp;
        const char * type = strtok(buffer, ",");

        while (type) {

            Wstring tmp = type;
            tmp.To_Upper();

            for (int i = 0; i < ARRAY_SIZE(_control_types); ++i) {
                if (_control_types[i].name == tmp) {
                    flags |= _control_types[i].control;
                }
            }

            type = strtok(nullptr, ",");
        }

        Control = AudioControlType(flags);
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioVocClass::Can_Play() const
{
    return AudioManager.Is_Available() && !Debug_Quiet && Name.Is_Not_Empty();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int AudioVocClass::Play(float volume, int variation)
{
    Voc_Play(*this, Coordinate(0,0,0), variation, volume);

    /**
     *  New audio interface does not use sample handles, so just return INVALID_AUDIO_HANDLE.
     */
    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int AudioVocClass::Play(float volume)
{
    Voc_Play(*this, Coordinate(0,0,0), -1, volume);

    /**
     *  New audio interface does not use sample handles, so just return INVALID_AUDIO_HANDLE.
     */
    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int AudioVocClass::Play(Coordinate &coord)
{
    Voc_Play(*this, coord);

    /**
     *  New audio interface does not use sample handles, so just return INVALID_AUDIO_HANDLE.
     */
    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int AudioVocClass::Play(VocType voc, int variation, float volume)
{
    Voc_Play((AudioVocClass &)*Vocs[voc], Coordinate(0,0,0), variation, volume);

    /**
     *  New audio interface does not use sample handles, so just return INVALID_AUDIO_HANDLE.
     */
    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int AudioVocClass::Play(VocType voc, float volume)
{
    Voc_Play((AudioVocClass &)*Vocs[voc], Coordinate(0,0,0), -1, volume);

    /**
     *  New audio interface does not use sample handles, so just return INVALID_AUDIO_HANDLE.
     */
    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int AudioVocClass::Play(VocType voc, Coordinate &coord)
{
    Voc_Play((AudioVocClass &)*Vocs[voc], coord);

    /**
     *  New audio interface does not use sample handles, so just return INVALID_AUDIO_HANDLE.
     */
    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void AudioVocClass::Scan()
{
    for (int index = 0; index < Vocs.Count(); ++index) {

        AudioVocClass *vocptr = (AudioVocClass *)Vocs[index];

        /**
         *  Check if the audio file is available. As Voc's can have multiple sounds
         *  defined which can be picked at random (if the RANDOM flag is set), we also
         *  flag the audio engine to ignore any errors at this point.
         */
        AudioManager.Audio_Set_Data(vocptr->Name, vocptr->Available, vocptr->FileType, vocptr->FileName, true);
    }

#if 0//#ifndef NDEBUG
    AUDIO_DEBUG_WARNING("Vox dump...\n");
    for (int index = 0; index < Voxs.Count(); ++index) {
        AUDIO_DEBUG_WARNING("  %03d  %s\n", index, ((AudioVoxClass*)Voxs[index])->Name.Peek_Buffer());
    }
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioVocClass::Process(CCINIClass &ini)
{
    static char const * const DEFAULTS = "Defaults";
    static char const * const SOUNDLIST = "SoundList";

    AUDIO_DEBUG_INFO("Voc::Process(enter): Vocs.Count = %d\n", Vocs.Count());

    //Clear();

    char buffer[32];

    if (ini.Is_Present(DEFAULTS)) {
        DefaultLimit = ini.Get_Int(DEFAULTS, "Limit", DefaultLimit);
        DefaultRange = ini.Get_Int(DEFAULTS, "Range", DefaultRange);
        DefaultType; // Not currently supported.
        DefaultControl; // Not currently supported.
        DefaultPriority = ini.Get_Int(DEFAULTS, "Priority", DefaultPriority);
        DefaultVolume = ini.Get_Float_Clamp(DEFAULTS, "Volume", AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX, DefaultVolume);
        DefaultMinVolume = ini.Get_Float_Clamp(DEFAULTS, "MinVolume", AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX, DefaultMinVolume);
        //DefaultMaxVolume = ini.Get_Float_Clamp(DEFAULTS, "MaxVolume", AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX, DefaultMaxVolume); // Not to be loaded from the ini database.
    }

    if (ini.Is_Present(SOUNDLIST)) {

        int count = ini.Entry_Count(SOUNDLIST);

        for (int index = 0; index < count; ++index) {

            if (ini.Get_String(SOUNDLIST, ini.Get_Entry(SOUNDLIST, index), buffer, sizeof(buffer)-1) > 0) {

                VocType voc = From_Name(buffer);

                AudioVocClass *vocptr = nullptr;
                if (voc == VOC_NONE) {
                    vocptr = new AudioVocClass(buffer);
                    AUDIO_DEBUG_INFO("Voc::Process: Creating new Voc %s.\n", vocptr->Name.Peek_Buffer());
                    Vocs.Add((VocClass *)vocptr);

                } else {
                    vocptr = (AudioVocClass *)Vocs[voc];
                    AUDIO_DEBUG_INFO("Voc::Process: Found exiting Voc %s.\n", vocptr->Name.Peek_Buffer());
                }
                vocptr->Read_INI(ini);

            }

        }

    }

    AUDIO_DEBUG_INFO("Voc: Loading isolated audio files...\n");

    /**
     *  Various sounds are loaded on the fly (dialog animation sound, etc), so we manually
     *  allocate these so they are a part of the new audio engine setup.
     */
    if (!VocEmblem) {
        VocEmblem = new AudioVocClass("EMBLEM");
        ASSERT(VocEmblem != nullptr);
    }
    if (VocEmblem) {
        VocEmblem->Read_INI(ini);
        AudioManager.Audio_Set_Data(VocEmblem->Name, VocEmblem->Available, VocEmblem->FileType, VocEmblem->FileName);
        VocEmblem->Type = AUDIO_SOUND_UI;
        VocEmblem->Control = AUDIO_CONTROL_INTERRUPT|AUDIO_CONTROL_AMBIENT;
    }

#if 0//#ifndef NDEBUG
    AUDIO_DEBUG_WARNING("Voc dump...\n");
    for (int index = 0; index < Vocs.Count(); ++index) {
        AUDIO_DEBUG_WARNING("  %03d  %s\n", index, ((AudioVocClass *)Vocs[index])->Name.Peek_Buffer());
    }
#endif

    AUDIO_DEBUG_INFO("Voc::Process(exit): Vocs.Count = %d\n", Vocs.Count());
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioVocClass::Clear()
{
    while (Vocs.Count() > 0) {
        int index = Vocs.Count()-1;
        delete Vocs[index];
        Vocs.Delete(index);
    }

    if (VocEmblem) {
        delete VocEmblem;
        VocEmblem = nullptr;
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
VocType AudioVocClass::VocType_From_Voc(AudioVocClass *voc)
{
    for (VocType index = VOC_FIRST; index < Vocs.Count(); ++index) {
        if ((AudioVocClass *)Vocs[index] == voc) {
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
VocType AudioVocClass::From_Name(const char *name)
{
    ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return VOC_NONE;
    }

    if (name != nullptr) {
        for (VocType index = VOC_FIRST; index < Vocs.Count(); ++index) {
            AudioVocClass *vocptr = (AudioVocClass *)Vocs[index];
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
AudioVocClass *AudioVocClass::Voc_From_Name(const char *name)
{
    ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return nullptr;
    }

    if (name != nullptr) {
        for (VocType index = VOC_FIRST; index < Vocs.Count(); ++index) {
            AudioVocClass *vocptr = (AudioVocClass *)Vocs[index];
            if (vocptr->Name == name) {
                return vocptr;
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
const char *AudioVocClass::INI_Name_From(VocType type)
{
    return (type != VOC_NONE && type < Vocs.Count() ? ((AudioVocClass *)Vocs[type])->Name.Peek_Buffer() : "<none>");
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioVocClass::Update_Audio_Event(AudioEventHandleClass &event, Coordinate &coord)
{
    /**
     *  x
     */
    if (!AudioManager.Is_Available() || Debug_Quiet) {
        return false;
    }

    /**
     *  x
     */
    if (AudioManager.Get_Group_Volume(AUDIO_GROUP_SOUND_EFFECT) <= AUDIO_VOLUME_MIN) {
        return false;
    }

    AudioVocClass &voc = event.Get_Voc();

    if (!coord) {
        AUDIO_DEBUG_ERROR("Voc::Update_Audio_Event - Invalid coord when updating \"%s\"!\n", voc.Name.Peek_Buffer());
        return false;
    }

    Cell cell = Coord_Cell(coord);
    if (!cell) {
        AUDIO_DEBUG_ERROR("Voc::Update_Audio_Event - Invalid cell when updating \"%s\"!\n", voc.Name.Peek_Buffer());
        return false;
    }

    float vol = AUDIO_VOLUME_MAX;
    float pitch = 1.0f;
    float pan = 0.0f;

    /**
     *  x
     */
    if (!event.Is_Playing()) {

        Wstring filename;

        /**
         *  x
         */
        //if (voc.Sounds.Count() && (voc.Control & AUDIO_CONTROL_RANDOM) == 0) {
        //    Wstring sound = voc.Sounds[Sim_Random_Pick(0, voc.Sounds.Count()-1)];
        //    filename = AudioManager.Build_Filename_From_Type(voc.FileType, sound);
        //
        //} else {
            filename = AudioManager.Build_Filename_From_Type(voc.FileType, voc.Name);
        //}

        event.Init(filename);

        /**
         *  x
         */
        float vshift = AUDIO_VOLUME_MIN;
        if (voc.VolumeShift.X != 0 || voc.VolumeShift.Y != 0) {
            vshift = Sim_Random_Pick_Float((float(voc.VolumeShift.X) / float(AUDIO_VSHIFT_MAX)), (float(voc.VolumeShift.Y) / float(AUDIO_VSHIFT_MAX)));
        }

        /**
         *  x
         */
        float fshift = 0.0f;
        if (voc.FrequencyShift.X != 0 || voc.FrequencyShift.Y != 0) {
            fshift = Sim_Random_Pick_Float((float(voc.FrequencyShift.X) / float(AUDIO_FSHIFT_MAX)), (float(voc.FrequencyShift.Y) / float(AUDIO_FSHIFT_MAX)));
        }

        vol = std::min(std::clamp((voc.Volume * vshift), AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX), voc.MaxVolume);
        pitch = 1.0f + fshift;

        event.Set_Volume(vol);
        event.Set_Pitch(pitch);

        if (!event.Start()) {
            DEBUG_ERROR("Voc::Update_Audio_Event - Failed to start \"%s\"!\n", voc.Name.Peek_Buffer());
            return false;
        }

    }

    /**
     *  Adjust the volume and panning of the sound depending on its
     *  location to the tactical screen.
     */
    Voc_Calculate_Pan_And_Volume(voc, coord, pan, vol);

    /**
     *  If the volume drops below this level, just mute it. Otherwise the sounds
     *  will bleed into adjacent areas a little too far.
     */
    if (vol < 0.05f) {
        event.Mute();
        return true;
    }

    /**
     *  Can this sound only be heard if the cell is revealed?
     */
    if ((voc.Type & AUDIO_SOUND_SHROUDED) != 0) {
        if (Map[cell].IsVisible|| Map[cell].IsFogVisible) {
            event.Mute();
            return true;
        }

    /**
     *  Can this sound only be heard if the cell is unrevealed?
     */
    } else if ((voc.Type & AUDIO_SOUND_UNSHROUDED) != 0) {
        if (!Map[cell].IsVisible && !Map[cell].IsFogVisible) {
            event.Mute();
            return true;
        }
    }

    /**
     *  Update the audio event.
     */
    event.Set_Volume(vol);
    event.Set_Pan(pan);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioVocClass::Stop_Audio_Event(AudioEventHandleClass &event, Coordinate &coord)
{
    event.Stop();

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void AudioVocClass::Set_Volume(int volume)
{
    float volf = std::clamp(float(volume/255.0f), AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX);
    AudioManager.Set_Group_Volume(AUDIO_GROUP_SOUND_EFFECT, volf);
    AudioManager.Set_Group_Volume(AUDIO_GROUP_EVENT, volf);
}


#endif // USE_MINIAUDIO
