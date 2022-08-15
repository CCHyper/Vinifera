/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FMOD_DRIVER.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Installable FMOD audio driver.
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
#ifdef USE_FMOD_AUDIO

#include "audio_manager.h"
#include "audio_util.h"
#include "fmod_globals.h"
#include "vinifera_util.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "ccfile.h"
#include "textfile.h"
#include "crc32.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <chrono>
#include <thread>
#include <vector>

#include <fmod_errors.h>


extern int Execute_Day;
extern int Execute_Month;
extern int Execute_Year;
extern int Execute_Hour;
extern int Execute_Min;
extern int Execute_Sec;


/**
 *  
 */
AudioManagerClass AudioManager;


FMOD::System *AudioManagerClass::System = nullptr;

#ifndef NDEBUG
bool AudioManagerClass::IsDebugLoggingEnabled = true;
bool AudioManagerClass::IsMemoryDebugLoggingEnabled = false;
bool AudioManagerClass::IsFileDebugLoggingEnabled = false;
bool AudioManagerClass::IsCodecDebugLoggingEnabled = false;
#else
bool AudioManagerClass::IsDebugLoggingEnabled = false;
bool AudioManagerClass::IsMemoryDebugLoggingEnabled = false;
bool AudioManagerClass::IsFileDebugLoggingEnabled = false;
bool AudioManagerClass::IsCodecDebugLoggingEnabled = false;
#endif


/**
 *  
 */
static TextFileClass FMODDebugLog;


/**
 *  FMOD_FileOpenCallback
 * 
 *  Callback for opening a file.
 */
static FMOD_RESULT F_CALLBACK FMOD_FileOpenCallback(const char *name, unsigned int *filesize, void **handle, void *userdata)
{
    if (!name) {
        return FMOD_ERR_INVALID_PARAM;
    }

    CCFileClass *file = new CCFileClass(name);
    if (!file || !file->Is_Available() || !file->Open()) {
        return FMOD_ERR_FILE_NOTFOUND;
    }

    *handle = file;
    *filesize = file->Size();

    return FMOD_OK;
}


/**
 *  FMOD_FileCloseCallback
 * 
 *  Callback for closing a file.
 */
static FMOD_RESULT F_CALLBACK FMOD_FileCloseCallback(void *handle, void *userdata)
{
    if (!handle) {
        return FMOD_ERR_INVALID_PARAM;
    }

    delete ((CCFileClass *)handle);

    return FMOD_OK;
}


/**
 *  FMOD_FileReadCallback
 * 
 *  Callback for reading from a file.
 */
static FMOD_RESULT F_CALLBACK FMOD_FileReadCallback(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
{
    if (!handle) {
        return FMOD_ERR_INVALID_PARAM;
    }

    CCFileClass *file = (CCFileClass *)handle;
    *bytesread = file->Read(buffer, sizebytes);

    if (*bytesread != sizebytes) {
        return FMOD_ERR_FILE_EOF;
    }

    return FMOD_OK;
}


/**
 *  FMOD_FileSeekCallback
 * 
 *  Callback for seeking within a file.
 */
static FMOD_RESULT F_CALLBACK FMOD_FileSeekCallback(void *handle, unsigned int pos, void *userdata)
{
    ASSERT(handle);

    CCFileClass *file = (CCFileClass *)handle;
    file->Seek(pos, FILE_SEEK_START);

    return FMOD_OK;
}


/**
 *  FMOD_PCMReadCallback
 * 
 *  desc
 */
// https://stackoverflow.com/questions/4125981/c-fmod-ex-playing-a-pcm-array-buffer-in-real-time
static FMOD_RESULT F_CALLBACK FMOD_PCMReadCallback(FMOD_SOUND *sound, void *data, unsigned int datalen)
{
    // Read from your buffer here...
    return FMOD_OK;
}


/**
 *  FMOD_PCMSetPosCallback
 * 
 *  desc
 */
static FMOD_RESULT F_CALLBACK FMOD_PCMSetPosCallback(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    // Seek to a location in your data, may not be required for what you want to do.
    return FMOD_OK;
}


/**
 *  FMOD_ChannelCallback
 * 
 *  desc
 */
FMOD_RESULT F_CALLBACK FMOD_ChannelCallback(FMOD_CHANNELCONTROL *channel_control, FMOD_CHANNELCONTROL_TYPE control_type, FMOD_CHANNELCONTROL_CALLBACK_TYPE callback_type, void *command_data_1, void *command_data_2)
{
    FMOD_RESULT result;
    FMOD::Channel *channel = nullptr;
    FMOD::ChannelGroup *channel_group = nullptr;
    FMOD::Sound *sound = nullptr;

    switch (callback_type) {

        /**
         *  Called when a sound ends.
         */
        case FMOD_CHANNELCONTROL_CALLBACK_END:

            switch (control_type) {
                case FMOD_CHANNELCONTROL_CHANNEL:
                {
                    channel = (FMOD::Channel *)channel_control;
                    
                    result = channel->getUserData((void **)&sound);
                    FMOD_ERRCHECK(result);
                    
                    char namebuf[256];
                    result = sound->getName(namebuf, sizeof(namebuf)-1);
                    FMOD_ERRCHECK(result);

                    DEBUG_INFO("[FMOD] END -> Sound \"%s\".\n", namebuf);

                    // Channel specific functions here...

                    break;
                }

                case FMOD_CHANNELCONTROL_CHANNELGROUP:
                {
                    channel_group = (FMOD::ChannelGroup *)channel_control;
                    FMOD_ERRCHECK(result);

                    // ChannelGroup specific functions here...

                    break;
                }

                default:
                    break;
            };

            break;
            
        /**
         *  Called when a voice is swapped out or swapped in.
         */
        case FMOD_CHANNELCONTROL_CALLBACK_VIRTUALVOICE:
            break;
            
        /**
         *  Called when a syncpoint is encountered. Can be from wav file markers.
         */
        case FMOD_CHANNELCONTROL_CALLBACK_SYNCPOINT:
            break;
            
        /**
         *  Called when the channel has its geometry occlusion value calculated. Can be used to clamp or change the value.
         */
        case FMOD_CHANNELCONTROL_CALLBACK_OCCLUSION:
            break;

        default:
            break;
    };

    return FMOD_OK;

#if 0
    // We only care about when the sound ends
    if (callback_type != FMOD_CHANNELCONTROL_CALLBACK_END) {
        return FMOD_OK;
    }
    
    if (control_type == FMOD_CHANNELCONTROL_CHANNEL) {
        FMOD::Channel *channel = (FMOD::Channel *)channel_control;
        // Channel specific functions here...

#if 0
        AudioEngine * engine;
        result = channel->getUserData((void**)(&engine));
        errorCheck();
        
        FMOD::Sound *sound;
        channel->getCurrentSound(&sound);
        if (sound == engine->brakeSound) {
            PhysicsEntity* source =  engine->currentlyPlaying[channel];
            engine->vehicleLoops[source].brake = false;
        }

        auto iter = engine->currentlyPlaying.find(channel);
        engine->currentlyPlaying.erase(iter);
#endif

    if (control_type == FMOD_CHANNELCONTROL_CHANNELGROUP) {
        FMOD::ChannelGroup *group = (FMOD::ChannelGroup *)channel_control;
        // ChannelGroup specific functions here... (but we don't use channelgroups)
    }

    // ChannelControl generic functions here...

    return FMOD_OK;
#endif
}


/**
 *  FMOD_ErrorCallback
 * 
 *  
 */
static FMOD_RESULT F_CALLBACK FMOD_ErrorCallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACK_TYPE type, void *commanddata1, void *commanddata2, void *userdata)
{
    if (type == FMOD_SYSTEM_CALLBACK_MEMORYALLOCATIONFAILED) {
        __debugbreak();
    }

    if (type == FMOD_SYSTEM_CALLBACK_ERROR) {
        FMOD_ERRORCALLBACK_INFO *info = (FMOD_ERRORCALLBACK_INFO *)commanddata1;
        DEBUG_ERROR("[FMOD] (%d) from %s(%s) with instance %p (type %d)\n", info->result, info->functionname, info->functionparams, info->instance, info->instancetype);

        if (info->result != FMOD_ERR_DMA) {
            __debugbreak();
        }
    }

    return FMOD_OK;
}

/**
 *  FMOD_DebugLogCallback
 * 
 *  
 */
static FMOD_RESULT F_CALLBACK FMOD_DebugLogCallback(FMOD_DEBUG_FLAGS flags, const char *file, int line, const char *func, const char *message)
{
    /**
     *  Strip path from "file".
     */
    file = (std::strrchr(file, '\\') ? std::strrchr(file, '\\') + 1 : file);

    /**
     *  Remove the '\n' from the end of the message.
     */
    auto msg = std::string(message);
    msg.pop_back();

    if (AudioManagerClass::IsMemoryDebugLoggingEnabled && (flags & FMOD_DEBUG_TYPE_MEMORY)) {
        DEBUG_INFO("FMOD_MEM [%s : %u : %s] - %s\n", file, line, func, msg.c_str());

    } else if (AudioManagerClass::IsFileDebugLoggingEnabled && (flags & FMOD_DEBUG_TYPE_FILE)) {
        DEBUG_INFO("FMOD_FILE [%s : %u : %s] - %s\n", file, line, func, msg.c_str());

    } else if (AudioManagerClass::IsCodecDebugLoggingEnabled && (flags & FMOD_DEBUG_TYPE_CODEC)) {
        DEBUG_INFO("FMOD_CODEC [%s : %u : %s] - %s\n", file, line, func, msg.c_str());

    } else if ((flags & FMOD_DEBUG_LEVEL_ERROR)) {
       DEBUG_ERROR("FMOD_ERROR [%s : %u : %s] - %s\n", file, line, func, msg.c_str());

    } else if ((flags & FMOD_DEBUG_LEVEL_WARNING)) {
        DEBUG_WARNING("FMOD_WARNING [%s : %u : %s] - %s\n", file, line, func, msg.c_str());

    } else if ((flags & FMOD_DEBUG_LEVEL_LOG)) {
        DEBUG_INFO("FMOD_LOG [%s : %u : %s] - %s\n", file, line, func, msg.c_str());
    }

    return FMOD_OK;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioManagerClass::AudioManagerClass() :
    IsAvailable(false),
    IsEnabled(false),
    Ambient(),
    Effects(),
    Speech(),
    Music(),
    EffectSoundGroup(nullptr),
    AmbientSoundGroup(nullptr),
    AmbientChannelGroup(nullptr),
    EffectChannelGroup(nullptr),
    SpeechChannelGroup(nullptr),
    MusicChannelGroup(nullptr),
    MovieChannelGroup(nullptr),
    MasterSoundGroup(nullptr),
    MasterChannelGroup(nullptr),
    MasterVolumeRestore(1.0f),
    DSPEffects()
{
    for (int i = 0; i < FMOD_DSP_COUNT; ++i) {
        DSPEffects[i] = nullptr;
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioManagerClass::~AudioManagerClass()
{
    // empty
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioManagerClass::Init(HWND hWnd)
{
    if (System) {
        DEBUG_WARNING("[FMOD] Driver already initialised!\n");
        return true;
    }

    DEBUG_INFO("[FMOD] Init...\n");

    //CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    FMOD_RESULT result;
    unsigned int version;
    void *extradriverdata = nullptr;

    /**
     *  Initialize Debugging.
     */
    if (IsDebugLoggingEnabled) {

        FMOD_DEBUG_FLAGS dbg_flags = FMOD_DEBUG_LEVEL_NONE
                                   | FMOD_DEBUG_LEVEL_ERROR
                                   | FMOD_DEBUG_LEVEL_WARNING
                                   | FMOD_DEBUG_LEVEL_LOG;

#ifndef NDEBUG
        if (IsMemoryDebugLoggingEnabled) {
            dbg_flags |= FMOD_DEBUG_TYPE_MEMORY;
        }
        if (IsFileDebugLoggingEnabled) {
            dbg_flags |= FMOD_DEBUG_TYPE_FILE;
        }
        if (IsCodecDebugLoggingEnabled) {
            dbg_flags |= FMOD_DEBUG_TYPE_CODEC;
        }
#endif

        FMOD::Debug_Initialize(dbg_flags, FMOD_DEBUG_MODE_CALLBACK, FMOD_DebugLogCallback);
    }

    /**
     *  Create the main system object.
     */
    result = FMOD::System_Create(&System); 
    if (result != FMOD_OK) {
        DEBUG_ERROR("[FMOD] Error - (%d) %s\n", result, FMOD_ErrorString(result));
        return false;
    }
    
    /**
     *  Check link version.
     */
    result = System->getVersion(&version);
    FMOD_ERRCHECK(result);
    if (version < FMOD_VERSION) {
        DEBUG_ERROR("[FMOD] Error - You are using an old version of FMOD (%08x). This program requires %08x!\n", version, FMOD_VERSION);
        return false;
    }

    /**
     *  Sets custom system and file callback.
     */
    result = System->setCallback(FMOD_ErrorCallback, FMOD_SYSTEM_CALLBACK_MEMORYALLOCATIONFAILED);
    FMOD_ERRCHECK(result);

    result = System->setFileSystem(FMOD_FileOpenCallback, FMOD_FileCloseCallback, FMOD_FileReadCallback, FMOD_FileSeekCallback, nullptr, nullptr, -1);
    FMOD_ERRCHECK(result);


    /**
     *  
     */
    //int maxsoftwarechannels = 32 + 4 + 1 + 2 + 1; // 32 sfx, 4 ui, 1 speech, 2 music, 1 movie.
    //int maxsoftwarechannels = 128;
    //result = System->setSoftwareChannels(maxsoftwarechannels);
    //FMOD_ERRCHECK(result);

    /**
     *  Set the playback format.
     */
    //result = System->setSoftwareFormat(48000, FMOD_SPEAKERMODE_STEREO, 0);
    //FMOD_ERRCHECK(result);
    //
    //DEBUG_INFO("[FMOD] SampleRate: %d, SpeakerMode: %s\n", 48000, "Stereo");

#if 0

    result = System->setStreamBufferSize(32768, FMOD_TIMEUNIT_RAWBYTES);
    FMOD_ERRCHECK(result);

    /**
     *  
     */
    int numdrivers;
    result = System->getNumDrivers(&numdrivers);
    FMOD_ERRCHECK(result);
    if (numdrivers == 0) {
        DEBUG_ERROR("[FMOD] Error - No drivers found!\n");
        result = System->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
        FMOD_ERRCHECK(result);
    }

    DEBUG_INFO("[FMOD] NumberOfDrivers: %d\n", numdrivers);

    for (int i = 0; i < numdrivers; ++i) {
        static char name[256];
        result = System->getDriverInfo(i, name, sizeof(name), nullptr, nullptr, nullptr, nullptr);
        FMOD_ERRCHECK(result);
        DEBUG_INFO("[FMOD] Driver[%d] %s\n", i, name);
    }

    result = System->setDriver(0);
    FMOD_ERRCHECK(result);
    //DEBUG_INFO("[FMOD] Driver set to : %s\n", drivername);

    /**
     *  Log FMOD info.
     */
    int r_numbuffers, r_samplerate, r_channels;
    unsigned int r_bufferlength;
    int latency = 100;
    char r_name[512];
    result = System->getDriverInfo(0, r_name, sizeof(r_name)-1, nullptr, &r_samplerate, nullptr, &r_channels);
    FMOD_ERRCHECK(result);

    result = System->getDSPBufferSize(&r_bufferlength, &r_numbuffers);
    FMOD_ERRCHECK(result);

    if (r_samplerate != 0) {
        latency = (int)(1000.0f * r_bufferlength * r_numbuffers / r_samplerate);
    }

    DEBUG_INFO("[FMOD] NumBuffers: %d, SampleRate: %d, Channels: %d, BufferLength: %d, Latency: %d\n", r_numbuffers, r_samplerate, r_channels, r_bufferlength, latency);
#endif

    /**
     *  Initialize FMOD.
     * 
     *  We pass hWnd into the "extradriverdata" parameter to make sure FMOD
     *  is focused on only our window.
     */
    FMOD_INITFLAGS flags = FMOD_INIT_NORMAL
                          |FMOD_INIT_THREAD_UNSAFE
                          |FMOD_INIT_VOL0_BECOMES_VIRTUAL;
#ifndef NDEBUG
    flags |= /*FMOD_INIT_PROFILE_ENABLE
            |FMOD_INIT_PROFILE_METER_ALL
            |*/FMOD_INIT_MEMORY_TRACKING;
#endif
    result = System->init(2048, flags, hWnd);
    if (result != FMOD_OK) {
        DEBUG_ERROR("[FMOD] Error - (%d) %s\n", result, FMOD_ErrorString(result));
        return false;
    }

    // fmod qa tests
    System->setOutput(FMOD_OUTPUTTYPE_WINSONIC);
    //System->setOutput(FMOD_OUTPUTTYPE_ASIO);
    
    char namebuf[256];

    /**
     *  Create sound groups.
     */
    result = System->createSoundGroup("Effects", &EffectSoundGroup);
    FMOD_ERRCHECK(result);

    EffectSoundGroup->getName(namebuf, sizeof(namebuf)-1);
    DEBUG_INFO("[FMOD] Created sound group \"%s\".\n", namebuf);

    result = System->getMasterSoundGroup(&MasterSoundGroup);
    FMOD_ERRCHECK(result);

    MasterSoundGroup->getName(namebuf, sizeof(namebuf)-1);
    DEBUG_INFO("[FMOD] Master sound group name \"%s\".\n", namebuf);

    /**
     *  setMaxAudible -> Limits the number of concurrent playbacks of sounds in
     *                   this sound group to the specified value.
     * 
     *  setMaxAudibleBehavior -> The way the sound playback behaves when too many
     *                           sounds are playing in a soundgroup.
     * 
     *  setMuteFadeSpeed -> When more sounds are playing in the SoundGroup than
     *                      we set by setMaxAudible, the least important sounds
     *                      will fade to silence.
     */
    EffectSoundGroup->setMaxAudible(16);
    EffectSoundGroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_STEALLOWEST);
    EffectSoundGroup->setMuteFadeSpeed(1.0f);

    DEBUG_INFO("[FMOD] EffectSoundGroup - MaxAudible:%d.\n", 32);
    DEBUG_INFO("[FMOD] EffectSoundGroup - MaxAudibleBehavior:%s.\n", "BEHAVIOR_STEALLOWEST");
    DEBUG_INFO("[FMOD] EffectSoundGroup - MuteFadeSpeed:%f.\n", 1.0f);

    /**
     *  Create channel groups.
     */
    result = System->createChannelGroup("Effects", &EffectChannelGroup);
    FMOD_ERRCHECK(result);

    EffectChannelGroup->getName(namebuf, sizeof(namebuf)-1);
    DEBUG_INFO("[FMOD] Created channel group \"%s\".\n", namebuf);

    result = System->createChannelGroup("Speech", &SpeechChannelGroup);
    FMOD_ERRCHECK(result);

    SpeechChannelGroup->getName(namebuf, sizeof(namebuf)-1);
    DEBUG_INFO("[FMOD] Created channel group \"%s\".\n", namebuf);

    result = System->createChannelGroup("Music", &MusicChannelGroup);
    FMOD_ERRCHECK(result);

    MusicChannelGroup->getName(namebuf, sizeof(namebuf)-1);
    DEBUG_INFO("[FMOD] Created channel group \"%s\".\n", namebuf);

    result = System->createChannelGroup("Movie", &MovieChannelGroup);
    FMOD_ERRCHECK(result);

    MovieChannelGroup->getName(namebuf, sizeof(namebuf)-1);
    DEBUG_INFO("[FMOD] Created channel group \"%s\".\n", namebuf);

    result = System->getMasterChannelGroup(&MasterChannelGroup);
    FMOD_ERRCHECK(result);

    MasterChannelGroup->getName(namebuf, sizeof(namebuf)-1);
    DEBUG_INFO("[FMOD] Master channel group name \"%s\".\n", namebuf);

    /**
     *  Instead of being independent, set the groups to be children of the master group.
     */
    result = MasterChannelGroup->addGroup(EffectChannelGroup);
    FMOD_ERRCHECK(result);

    result = MasterChannelGroup->addGroup(SpeechChannelGroup);
    FMOD_ERRCHECK(result);

    result = MasterChannelGroup->addGroup(MusicChannelGroup);
    FMOD_ERRCHECK(result);

    result = MasterChannelGroup->addGroup(MovieChannelGroup);
    FMOD_ERRCHECK(result);

    /**
     *  Create the DSP effects.
     */
    result = System->createDSPByType(FMOD_DSP_TYPE_OSCILLATOR, &DSPEffects[FMOD_DSP_OSCILLATOR]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &DSPEffects[FMOD_DSP_LOWPASS]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_ITLOWPASS, &DSPEffects[FMOD_DSP_ITLOWPASS]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &DSPEffects[FMOD_DSP_HIGHPASS]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_ECHO, &DSPEffects[FMOD_DSP_ECHO]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_FADER, &DSPEffects[FMOD_DSP_FADER]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_FLANGE, &DSPEffects[FMOD_DSP_FLANGE]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &DSPEffects[FMOD_DSP_DISTORTION]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_NORMALIZE, &DSPEffects[FMOD_DSP_NORMALIZE]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_LIMITER, &DSPEffects[FMOD_DSP_LIMITER]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_PARAMEQ, &DSPEffects[FMOD_DSP_PARAMEQ]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &DSPEffects[FMOD_DSP_PITCHSHIFT]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_CHORUS, &DSPEffects[FMOD_DSP_CHORUS]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_ITECHO, &DSPEffects[FMOD_DSP_ITECHO]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_COMPRESSOR, &DSPEffects[FMOD_DSP_COMPRESSOR]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &DSPEffects[FMOD_DSP_SFXREVERB]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_LOWPASS_SIMPLE, &DSPEffects[FMOD_DSP_LOWPASS_SIMPLE]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_DELAY, &DSPEffects[FMOD_DSP_DELAY]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &DSPEffects[FMOD_DSP_TREMOLO]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_HIGHPASS_SIMPLE, &DSPEffects[FMOD_DSP_HIGHPASS_SIMPLE]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_PAN, &DSPEffects[FMOD_DSP_PAN]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_THREE_EQ, &DSPEffects[FMOD_DSP_THREE_EQ]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_FFT, &DSPEffects[FMOD_DSP_FFT]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_LOUDNESS_METER, &DSPEffects[FMOD_DSP_LOUDNESS_METER]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_ENVELOPEFOLLOWER, &DSPEffects[FMOD_DSP_ENVELOPEFOLLOWER]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_CONVOLUTIONREVERB, &DSPEffects[FMOD_DSP_CONVOLUTIONREVERB]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_OBJECTPAN, &DSPEffects[FMOD_DSP_OBJECTPAN]);
    FMOD_ERRCHECK(result);

    result = System->createDSPByType(FMOD_DSP_TYPE_MULTIBAND_EQ, &DSPEffects[FMOD_DSP_MULTIBAND_EQ]);
    FMOD_ERRCHECK(result);

    IsAvailable = true;
    IsEnabled = true;
    
    DEBUG_INFO("[FMOD] Init done!\n");

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::End()
{
    FMOD_RESULT result;

    Effects.Clear();
    Speech.Clear();
    Music.Clear();

    for (int i = 0; i < FMOD_DSP_COUNT; ++i) {
        DSPEffects[i]->release();
        DSPEffects[i] = nullptr;
    }

    result = EffectSoundGroup->release();
    FMOD_ERRCHECK(result);
    EffectSoundGroup = nullptr;

    result = EffectChannelGroup->release();
    FMOD_ERRCHECK(result);
    EffectChannelGroup = nullptr;

    result = SpeechChannelGroup->release();
    FMOD_ERRCHECK(result);
    SpeechChannelGroup = nullptr;

    result = MusicChannelGroup->release();
    FMOD_ERRCHECK(result);
    MusicChannelGroup = nullptr;

    result = MovieChannelGroup->release();
    FMOD_ERRCHECK(result);
    MovieChannelGroup = nullptr;

    result = MasterChannelGroup->release();
    FMOD_ERRCHECK(result);
    MasterChannelGroup = nullptr;

    result = System->release();
    FMOD_ERRCHECK(result);
    System = nullptr;

    IsAvailable = false;
    IsEnabled = false;

    CoUninitialize();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioManagerClass::Is_Available() const
{
    return IsAvailable;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioManagerClass::Is_Enabled() const
{
    return IsEnabled;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::Enable()
{
    IsEnabled = true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::Disable()
{
    IsEnabled = false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioManagerClass::Start_Engine(bool forced)
{
    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::Stop_Engine()
{
    // empty
}


/**
 *  Handle application focus loss.
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::Focus_Loss()
{
    DEBUG_INFO("FMOD::Focus_Loss().\n");

    /**
     *  Backup the current volume for when we restore focus.
     */
    MasterChannelGroup->getVolume(&MasterVolumeRestore);

    MasterChannelGroup->setVolume(0.0f);
    System->mixerSuspend();
}


/**
 *  Handle application focus restore.
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::Focus_Restore()
{
    DEBUG_INFO("FMOD::Focus_Restore().\n");

    MasterChannelGroup->setVolume(MasterVolumeRestore);
    System->mixerResume();
}


/**
 *  Updates the FMOD system. This should be called once per 'game' tick (in Main_Loop).
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::Sound_Callback()
{
    System->update();

#if 0
    /**
     *  This kludge is required as ddraw.dll takes complete ownership of the game
     *  window and thus breaks the focus detection code for the game.
     */
    static bool _one_time = false;
    static bool _ddraw_found = false;
    static bool _in_focus = false;

    if (!_one_time) {
        if (RawFileClass("DDRAW.DLL").Is_Available()) {
            _ddraw_found = true;
        }
        _one_time = true;
    }

    if (_ddraw_found) {
        if (GetFocus() == MainWindow && !_in_focus) {
            Focus_Restore();
            _in_focus = true;

        } else if (GetFocus() != MainWindow && _in_focus) {
            Focus_Loss();
            _in_focus = false;
        }
    }
#endif
}


bool AudioManagerClass::Submit_Effect(Wstring name, VocType index)
{
    name.To_Upper();

    FMOD::Sound *sound = Create_Sound(name, true);
    if (!sound) {
        return false;
    }

    //bool added = Effects.Add(sound);
    //if (!added) {
    //    return false;
    //}
    Effects[index] = sound;

    /**
     *  Quick check to make sure the vector index matches the enumeration.
     */
    ASSERT_PRINT(index == Effects.ID(sound), "index is %d, while Effects.ID is %d", index, Effects.ID(sound));

    DEBUG_INFO("[FMOD] Effect \"%s\" was submitted.\n", name.Peek_Buffer());

    return true;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Play_Effect(VocType index, float volume, int priority, float pan, float pitch, AudioManagerClass::DSPFlagType dsp_flag, bool loop, bool paused)
{
    //ASSERT_PRINT(index < Effects.Count(), "index is %d, while Effects.Count is %d", index, Effects.Count());
    ASSERT_PRINT(index < Effects.Length(), "index is %d, while Effects.Length is %d", index, Effects.Length());

    FMOD::Sound *sound = Effects[index];
    if (!sound) {
        return false;
    }

    /**
     *  Clamp the priority within the allowed range.
     */
    priority = std::clamp<int>(priority, FMOD_PRIORITY_EFFECT_MIN, FMOD_PRIORITY_EFFECT_MAX);

    return Play_Sound(sound, EffectChannelGroup, EffectSoundGroup, dsp_flag, volume, priority, pan, pitch, loop, paused);
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Submit_Speech(Wstring name, VoxType index)
{
    name.To_Upper();

    FMOD::Sound *sound = Create_Sound(name, true);
    if (!sound) {
        return false;
    }

    //bool added = Speech.Add(sound);
    //if (!added) {
    //    return false;
    //}
    Speech[index] = sound;

    /**
     *  Quick check to make sure the vector index matches the enumeration.
     */
    ASSERT_PRINT(index == Speech.ID(sound), "index is %d, while Speech.ID is %d", index, Speech.ID(sound));

    DEBUG_INFO("[FMOD] Speech \"%s\" was submitted.\n", name.Peek_Buffer());

    return true;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Play_Speech(VoxType index, float volume, float pitch, AudioManagerClass::DSPFlagType dsp_flag, bool paused)
{
    //ASSERT_PRINT(index < Speech.Count(), "index is %d, while Speech.Count is %d", index, Speech.Count());
    ASSERT_PRINT(index < Speech.Length(), "index is %d, while Speech.Length is %d", index, Speech.Length());

    FMOD::Sound *sound = Speech[index];
    if (!sound) {
        return false;
    }

    return Play_Sound(sound, SpeechChannelGroup, nullptr, dsp_flag, volume, FMOD_PRIORITY_SPEECH, 0.0f, 1.0f, false, paused);
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Submit_Music(Wstring name, ThemeType index)
{
    name.To_Upper();

    FMOD::Sound *sound = Create_Sound(name, true);
    if (!sound) {
        return false;
    }

    //bool added = Music.Add(sound);
    //if (!added) {
    //    return false;
    //}
    Music[index] = sound;

    /**
     *  Quick check to make sure the vector index matches the enumeration.
     */
    ASSERT_PRINT(index == Music.ID(sound), "index is %d, while Music.ID is %d", index, Music.ID(sound));

    DEBUG_INFO("[FMOD] Music \"%s\" was submitted.\n", name.Peek_Buffer());

    return true;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Play_Music(ThemeType index, float volume, AudioManagerClass::DSPFlagType dsp_flag, bool paused)
{
    //ASSERT_PRINT(index < Music.Count(), "index is %d, while Music.Count is %d", index, Music.Count());
    ASSERT_PRINT(index < Music.Length(), "index is %d, while Music.Length is %d", index, Music.Length());

    FMOD::Sound *sound = Music[index];
    if (!sound) {
        return false;
    }

    return Play_Sound(sound, MusicChannelGroup, nullptr, dsp_flag, volume, FMOD_PRIORITY_MUSIC, 0.0f, 1.0f, false, paused);
}


#if 0
/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Submit_Stream_PCM(const void *data, unsigned int length, int sample_rate, int channels, FMOD::Sound **sound)
{
    FMOD_CREATESOUNDEXINFO exinfo;
    std::memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));

    exinfo.cbsize            = sizeof(FMOD_CREATESOUNDEXINFO);                    // required.
    exinfo.decodebuffersize  = sample_rate;                                       // Chunk size of stream update in samples. This will be the amount of data passed to the user callback.
    exinfo.length            = sample_rate * channels * sizeof(signed short) * 5; // Length of PCM data in bytes of whole song (for Sound::getLength).
    exinfo.numchannels       = channels;                                          // Number of channels in the sound.
    exinfo.defaultfrequency  = sample_rate;                                       // Default playback rate of sound.
    exinfo.format            = FMOD_SOUND_FORMAT_PCM16;                           // Data format of sound.
    exinfo.pcmreadcallback   = FMOD_PCMReadCallback;                              // User callback for reading.
    exinfo.pcmsetposcallback = FMOD_PCMSetPosCallback;                            // User callback for seeking.

    FMOD_RESULT result = System->createStream(nullptr, FMOD_OPENUSER, &exinfo, sound);
    FMOD_ERRCHECK(result);

    return true;
}
#endif


/**
 *  AudioManagerClass::Stop_Effect_Channel
 * 
 *  Stops all sound effects from playing.
 */
bool AudioManagerClass::Stop_Effect_Channel()
{
    FMOD_RESULT result;

    result = EffectSoundGroup->stop();
    FMOD_ERRCHECK(result);

    result = EffectChannelGroup->stop();
    FMOD_ERRCHECK(result);

    return true;
}


/**
 *  AudioManagerClass::Stop_Speech_Channel
 * 
 *  Stops all speech from playing.
 */
bool AudioManagerClass::Stop_Speech_Channel()
{
    FMOD_RESULT result = SpeechChannelGroup->stop();
    FMOD_ERRCHECK(result);

    return true;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Is_Speech_Channel_Playing() const
{
    bool playing = false;
    FMOD_RESULT result = SpeechChannelGroup->isPlaying(&playing);

    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
        FMOD_ERRCHECK(result);
    }

    return playing;
}


/**
 *  AudioManagerClass::Stop_Music_Channel
 * 
 *  Stops all music from playing.
 */
bool AudioManagerClass::Stop_Music_Channel(bool fade_out, float seconds)
{
    FMOD_RESULT result;

    if (fade_out) {

        unsigned long long dspclock;
        int rate;
        float channelvolume;

        /**
         *  Get mixer rate.
         */
        result = System->getSoftwareFormat(&rate, 0, 0);
        FMOD_ERRCHECK(result);

        /**
         *  Get current channel volume.
         */
        result = MusicChannelGroup->getVolume(&channelvolume);

        /**
         *  Get the reference clock, which is the parent channel group.
         */
        result = MusicChannelGroup->getDSPClock(0, &dspclock);
        FMOD_ERRCHECK(result);
        
        /**
         *  Add a fade point at 'now' with full volume.
         */
        result = MusicChannelGroup->addFadePoint(dspclock, channelvolume);
        FMOD_ERRCHECK(result);
        
        /**
         *  Add a fade point 5 seconds later at 0 volume.
         */
        result = MusicChannelGroup->addFadePoint(dspclock + (rate * seconds), 0.0f);
        FMOD_ERRCHECK(result);
        
        /**
         *  Add a delayed stop command at 5 seconds ('stopchannels = true')
         */
        result = MusicChannelGroup->setDelay(0, dspclock + (rate * seconds), true);
        FMOD_ERRCHECK(result);

    } else {

        result = MusicChannelGroup->stop();
        FMOD_ERRCHECK(result);

    }

    return true;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Is_Music_Channel_Playing() const
{
    bool playing = false;
    FMOD_RESULT result = MusicChannelGroup->isPlaying(&playing);

    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
        FMOD_ERRCHECK(result);
    }

    return playing;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Pause_Music_Channel() const
{
    FMOD_RESULT result = MusicChannelGroup->setPaused(true);

    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
        FMOD_ERRCHECK(result);
    }

    return true;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Resume_Music_Channel() const
{
    FMOD_RESULT result = MusicChannelGroup->setPaused(false);

    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) {
        FMOD_ERRCHECK(result);
    }

    return true;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Stop_Movie_Channel()
{
    FMOD_RESULT result = MovieChannelGroup->stop();
    FMOD_ERRCHECK(result);

    return true;
}


/**
 *  name
 * 
 *  Sets the expected size of the Effects vector.
 */
bool AudioManagerClass::Set_Effects_Size(int size)
{
    return Effects.Resize(size);
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Set_Speech_Size(int size)
{
    return Speech.Resize(size);
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Set_Music_Size(int size)
{
    return Music.Resize(size);
}


/**
 *  name
 * 
 *  desc
 */
void AudioManagerClass::Clear_Effects()
{
#if 0
    while (Effects.Count() > 0) {
        int index = Effects.Count()-1;
        Effects[index]->release();
        delete Effects[index];
        Effects.Delete(index);
    }
#endif
}


/**
 *  name
 * 
 *  desc
 */
void AudioManagerClass::Clear_Speech()
{
#if 0
    while (Speech.Count() > 0) {
        int index = Speech.Count()-1;
        Speech[index]->release();
        delete Speech[index];
        Speech.Delete(index);
    }
#endif
}


/**
 *  name
 * 
 *  desc
 */
void AudioManagerClass::Clear_Music()
{
#if 0
    while (Music.Count() > 0) {
        int index = Music.Count()-1;
        Music[index]->release();
        delete Music[index];
        Music.Delete(index);
    }
#endif
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Load_Sound_Bank(Wstring filename)
{
    return false;
}


/**
 *  name
 * 
 *  desc
 */
float AudioManagerClass::Get_Effect_Volume() const
{
    float volume = 1.0f;

    FMOD_RESULT result = EffectChannelGroup->getVolume(&volume);
    FMOD_ERRCHECK(result);

    return volume;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Set_Effect_Volume(float volume)
{
    FMOD_RESULT result = EffectChannelGroup->setVolume(volume);
    FMOD_ERRCHECK(result);

    return true;
}


/**
 *  name
 * 
 *  desc
 */
float AudioManagerClass::Get_Speech_Volume() const
{
    float volume = 1.0f;

    FMOD_RESULT result = SpeechChannelGroup->getVolume(&volume);
    FMOD_ERRCHECK(result);

    return volume;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Set_Speech_Volume(float volume)
{
    FMOD_RESULT result = SpeechChannelGroup->setVolume(volume);
    FMOD_ERRCHECK(result);

    return true;
}


/**
 *  name
 * 
 *  desc
 */
float AudioManagerClass::Get_Music_Volume() const
{
    float volume = 1.0f;

    FMOD_RESULT result = MusicChannelGroup->getVolume(&volume);
    FMOD_ERRCHECK(result);

    return volume;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Set_Music_Volume(float volume)
{
    FMOD_RESULT result = MusicChannelGroup->setVolume(volume);
    FMOD_ERRCHECK(result);

    return true;
}


/**
 *  name
 * 
 *  desc
 */
float AudioManagerClass::Get_Movie_Volume() const
{
    float volume = 1.0f;

    FMOD_RESULT result = MovieChannelGroup->getVolume(&volume);
    FMOD_ERRCHECK(result);

    return volume;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Set_Movie_Volume(float volume)
{
    FMOD_RESULT result = MovieChannelGroup->setVolume(volume);
    FMOD_ERRCHECK(result);

    return true;
}


/**
 *  name
 * 
 *  desc
 */
float AudioManagerClass::Get_Master_Volume() const
{
    float volume = 1.0f;

    FMOD_RESULT result = MasterChannelGroup->getVolume(&volume);
    FMOD_ERRCHECK(result);

    return volume;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Set_Master_Volume(float volume)
{
    FMOD_RESULT result = MasterChannelGroup->setVolume(volume);
    FMOD_ERRCHECK(result);

    return true;
}


/**
 *  name
 * 
 *  desc
 */
static Wstring Remove_File_Extension(Wstring fname)
{
    size_t lastindex = std::string(fname.Peek_Buffer()).find_last_of(".");
    if (lastindex != std::string::npos) {
        fname = Wstring( std::string(fname.Peek_Buffer()).substr(0, lastindex).c_str() );
    }

    return fname;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Is_Supported_File_Available(Wstring name, Wstring &out_name)
{
    CCFileClass file;

    name = Remove_File_Extension(name);

    /**
     *  
     */
    for (int type = 0; type < FMOD_SOUND_TYPE_MAX; ++type) {

         if (type == FMOD_SOUND_TYPE_OGGVORBIS) {
            file.Set_Name( Wstring(name + ".OGG").Peek_Buffer() );
            if (file.Is_Available()) {
            #ifndef NDEBUG
                //DEV_DEBUG_INFO("[FMOD] \"%s\" found.\n", file.File_Name());
            #endif
                out_name = Wstring(name + ".OGG");
                return true;
            }

        } else if (type == FMOD_SOUND_TYPE_MPEG) {
            file.Set_Name( Wstring(name + ".MP3").Peek_Buffer() );
            if (file.Is_Available()) {
            #ifndef NDEBUG
                //DEV_DEBUG_INFO("[FMOD] \"%s\" found.\n", file.File_Name());
            #endif
                out_name = Wstring(name + ".MP3");
                return true;
            }

        } else if (type == FMOD_SOUND_TYPE_WAV) {
            file.Set_Name( Wstring(name + ".WAV").Peek_Buffer() );
            if (file.Is_Available()) {
            #ifndef NDEBUG
                //DEV_DEBUG_INFO("[FMOD] \"%s\" found.\n", file.File_Name());
            #endif
                out_name = Wstring(name + ".WAV");
                return true;
            }

        } else if (type == FMOD_SOUND_TYPE_XMA) {
            file.Set_Name( Wstring(name + ".XMA").Peek_Buffer() );
            if (file.Is_Available()) {
            #ifndef NDEBUG
                //DEV_DEBUG_INFO("[FMOD] \"%s\" found.\n", file.File_Name());
            #endif
                out_name = Wstring(name + ".XMA");
                return true;
            }

        } else if (type == FMOD_SOUND_TYPE_FLAC) {
            file.Set_Name( Wstring(name + ".FLAC").Peek_Buffer() );
            if (file.Is_Available()) {
            #ifndef NDEBUG
                //DEV_DEBUG_INFO("[FMOD] \"%s\" found.\n", file.File_Name());
            #endif
                out_name = Wstring(name + ".FLAC");
                return true;
            }
        }

    }

#ifndef NDEBUG
    DEBUG_WARNING("[FMOD] Failed to find \"%s\"!\n", name.Peek_Buffer());
#endif

    return false;
}


/**
 *  name
 * 
 *  desc
 */
FMOD::Sound *AudioManagerClass::Create_Sound(Wstring name, bool non_blocking, bool unique)
{
    FMOD_RESULT result;
    FMOD::Sound *sound = nullptr;

    Wstring filename;
    if (!Is_Supported_File_Available(name, filename)) {
        DEBUG_WARNING("[FMOD] Sound \"%s\" was not found in a supported format!\n", name.Peek_Buffer());
        return nullptr;
    }

    int size = CCFileClass(filename.Peek_Buffer()).Size();

    DEV_DEBUG_INFO("[FMOD] Sound \"%s\" was found as \"%s\" (Size: %d).\n", name.Peek_Buffer(), filename.Peek_Buffer(), size);
    
    FMOD_CREATESOUNDEXINFO exinfo;
    FMOD_MODE mode = FMOD_DEFAULT
                    |FMOD_2D
                    |FMOD_LOOP_NORMAL
                    |FMOD_IGNORETAGS;

    // For opening sounds and getting streamed subsounds (seeking) asyncronously.
    if (non_blocking) {
        mode |= FMOD_NONBLOCKING;
    }

    // Unique sound, can only be played one at a time.
    if (unique) {
        mode |= FMOD_UNIQUE;
    }

    int max_buffer_size = 1048576; // 1mb

    /**
     *  Files larger than 1mb will be streamed.
     */
    if (size > max_buffer_size) {
        result = System->createStream(filename.Peek_Buffer(), mode, nullptr, &sound);
        FMOD_ERRCHECK(result);
    } else {
        result = System->createSound(filename.Peek_Buffer(), mode, nullptr, &sound);
        FMOD_ERRCHECK(result);
    }

    if (!sound) {
        DEBUG_ERROR("[FMOD] Sound object was null in Create_Sound for \"%s\"!\n", filename.Peek_Buffer());
        return nullptr;
    }

    return sound;
}


/**
 *  name
 * 
 *  desc
 */
bool AudioManagerClass::Play_Sound(FMOD::Sound *sound, FMOD::ChannelGroup *channel_group, FMOD::SoundGroup *sound_group, AudioManagerClass::DSPFlagType dsp_flag, float volume, int priority, float pan, float pitch, int loop_count, bool paused)
{
    ASSERT(sound != nullptr);
    ASSERT(channel_group != nullptr);

    FMOD_RESULT result;
    FMOD::Channel *channel = nullptr; // FMOD makes channels invalid after playback has finished, so we don't need to release this handle.

    /**
     *  Play the sound handle.
     * 
     *  We start the sounds paused as we need to set some channel attributes, otherwise
     *  the sound will start and there could be inconsitencies in volume, etc, for a
     *  fraction of a second.
     */
    result = System->playSound(sound, channel_group, true, &channel);
    FMOD_ERRCHECK(result);

    /**
     *  
     */
    //FMOD_SOUND_TYPE soundType = FMOD_SOUND_TYPE_UNKNOWN;;
    //FMOD_SOUND_FORMAT formatType = FMOD_SOUND_FORMAT_NONE;
    //int channels = 0;
    //int bits = 0;
    //result = sound->getFormat(&soundType, &formatType, &channels, &bits);
    //FMOD_ERRCHECK(result);

    /**
     *  Set sound attributes.
     */
    result = sound->setDefaults(48000, priority);
    FMOD_ERRCHECK(result);

    result = channel->setVolume(volume);
    FMOD_ERRCHECK(result);

    result = channel->setVolumeRamp(false); // For fixing popping noise at low volume.
    FMOD_ERRCHECK(result);

    result = channel->setPan(pan);
    FMOD_ERRCHECK(result);

    result = channel->setPitch(pitch);
    FMOD_ERRCHECK(result);

    result = channel->setMode(FMOD_LOOP_NORMAL);
    FMOD_ERRCHECK(result);

    if (loop_count > 0) {
        result = channel->setLoopCount(loop_count);
        FMOD_ERRCHECK(result);
    }

    result = channel->setCallback(FMOD_ChannelCallback);
    FMOD_ERRCHECK(result);

    result = channel->setUserData((void *)sound); // Set FMOD::Sound instance as user data for the callback.
    FMOD_ERRCHECK(result);

    /**
     *  Apply any flagged DSP effects.
     */
    if (dsp_flag != FMOD_DSP_NONE) {
        Apply_DSP_Effects(sound, channel, dsp_flag);
    }
    
    /**
     *  If the sound is flagged as not paused (play instantly), play it now we have set all the attributes.
     */
    if (!paused) {
        result = channel->setPaused(false);
        FMOD_ERRCHECK(result);
    }

    char namebuf[256];
    result = sound->getName(namebuf, sizeof(namebuf)-1);
    FMOD_ERRCHECK(result);

    DEBUG_INFO("[FMOD] Sound \"%s\" played succesfully.\n", namebuf);

#ifndef NDEBUG
    float channel_volume;
    result = channel_group->getVolume(&channel_volume);
    FMOD_ERRCHECK(result);

    char channel_namebuf[256];
    result = channel_group->getName(channel_namebuf, sizeof(channel_namebuf)-1);
    FMOD_ERRCHECK(result);

    DEV_DEBUG_INFO("[FMOD] ChannelGroup name: %s.\n", channel_namebuf);
    DEV_DEBUG_INFO("[FMOD] ChannelGroup volume: %f.\n", channel_volume);
    DEV_DEBUG_INFO("[FMOD] Sound volume: %f.\n", volume);
    DEV_DEBUG_INFO("[FMOD] Sound pan: %f.\n", pan);
    DEV_DEBUG_INFO("[FMOD] Sound pitch: %f.\n", pitch);

    int numdsps = 0;
    result = channel->getNumDSPs(&numdsps);
    FMOD_ERRCHECK(result);

    DEV_DEBUG_INFO("[FMOD] NumDSPs: %d\n", numdsps);
#endif

    return true;
}


/**
 *  name
 * 
 *  Applies the DSP effects to the specified channel.
 */
bool AudioManagerClass::Apply_DSP_Effects(FMOD::Sound *sound, FMOD::Channel *channel, AudioManagerClass::DSPFlagType dsp_flag)
{
    FMOD_RESULT result;

    char namebuf[256];
    result = sound->getName(namebuf, sizeof(namebuf)-1);
    FMOD_ERRCHECK(result);

    for (int index = 0; index < FMOD_DSP_COUNT; ++index) {

        DSPType dsp = DSPType(index);
        int bit_dsp = (1 << index);

        if ((dsp_flag & bit_dsp) == 1) {
            if (DSPEffects[index] != nullptr) {

                result = channel->addDSP(FMOD_CHANNELCONTROL_DSP_TAIL, DSPEffects[index]);
                FMOD_ERRCHECK(result);

                DEV_DEBUG_INFO("[FMOD] Applying DSP effect \"%s\" to \"%s\".\n", DSP_Name_From(dsp).Peek_Buffer(), namebuf);

            } else {
                DEV_DEBUG_WARNING("[FMOD] Failed to apply DSP effect \"%s\" to \"%s\".\n", DSP_Name_From(dsp).Peek_Buffer(), namebuf);
                //return false;
            }
        }
    }

    return true;
}


/**
 *  Returns a handle to FMOD's internal DirectSound instance. You must cast
 *  this to LPDIRECTSOUND where required.
 */
void *AudioManagerClass::Get_Output_Handle()
{
    void *output = nullptr;
    System->getOutputHandle(&output);
    return output;
}


/**
 *  Set the playback format for the FMOD sound system.
 */
bool AudioManagerClass::Set_Software_Format(int sample_rate, int bits, int channels)
{
    FMOD_RESULT result;
    result = System->setSoftwareFormat(sample_rate, FMOD_SPEAKERMODE_STEREO, 0);
    FMOD_ERRCHECK(result);

    DEBUG_INFO("[FMOD] SampleRate: %d, SpeakerMode: %s\n", sample_rate, "Stereo");

    return true;
}


static const char *_fmod_dsp_types[AudioManagerClass::FMOD_DSP_COUNT] = {
    "OSCILLATOR",
    "LOWPASS",
    "ITLOWPASS",
    "HIGHPASS",
    "ECHO",
    "FADER",
    "FLANGE",
    "DISTORTION",
    "NORMALIZE",
    "LIMITER",
    "PARAMEQ",
    "PITCHSHIFT",
    "CHORUS",
    "ITECHO",
    "COMPRESSOR",
    "SFXREVERB",
    "LOWPASS_SIMPLE",
    "DELAY",
    "TREMOLO",
    "HIGHPASS_SIMPLE",
    "PAN",
    "THREE_EQ",
    "FFT",
    "LOUDNESS_METER",
    "ENVELOPEFOLLOWER",
    "CONVOLUTIONREVERB",
    "OBJECTPAN",
    "MULTIBAND_EQ"
};


/**
 *  name
 * 
 *  desc
 */
AudioManagerClass::DSPType AudioManagerClass::DSP_From_Name(Wstring name)
{
    if (name.Is_Empty()) {
        return FMOD_DSP_NONE;
    }

    for (int index = 0; index < FMOD_DSP_COUNT; ++index) {
        if (Wstring(_fmod_dsp_types[index]) == name) {
            return AudioManagerClass::DSPType(index);
        }
    }

    return FMOD_DSP_NONE;
}


#if 0
/**
 *  name
 * 
 *  desc
 */
AudioManagerClass::DSPType AudioManagerClass::DSP_From_Name_String(Wstring name)
{
    if (name.Is_Empty()) {
        return FMOD_DSP_NONE;
    }

    AudioManagerClass::DSPType dsp = FMOD_DSP_NONE;

    // TODO

    return dsp;
}
#endif


/**
 *  name
 * 
 *  desc
 * 
 *  #WARNING: Only use this if the type is a single DSP!
 */
Wstring AudioManagerClass::DSP_Name_From(AudioManagerClass::DSPType type)
{
    if (type == FMOD_DSP_NONE || type >= FMOD_DSP_COUNT) {
        return Wstring();
    }

    return _fmod_dsp_types[type];
}


#if 0
/**
 *  name
 * 
 *  desc
 */
Wstring AudioManagerClass::DSP_Name_String_From(AudioManagerClass::DSPType type)
{
    Wstring name;

    for (int i = 0; i < FMOD_DSP_COUNT; ++i) {
        if (type & (1 << i)) {
            if (!name.Is_Empty()) {
                name += ",";
            }
            name += _fmod_dsp_types[i];
        }
    }

    return name;
}
#endif

#endif
