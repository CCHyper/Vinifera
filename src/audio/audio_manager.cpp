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

#include "audio_manager.h"
#include "audio_handle.h"
#include "audio_util.h"
#include "ccfile.h"
#include "tibsun_globals.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <chrono>
#include <thread>


/**
 *  x
 */
AudioManagerClass AudioManager;


/**
 *  MiniAudio options
 */
//#define MA_NO_ENGINE

#define MA_NO_OPUS
//#define MA_NO_VORBIS        // vorbis decoding
//#define MA_NO_WAV
//#define MA_NO_MP3
//#define MA_NO_FLAC
#define MA_NO_WWADPCM       // custom decoder for Westwood ADPCM

// Enable printf() output of debug logs (MA_LOG_LEVEL_DEBUG).
#ifndef NDEBUG
#define MA_DEBUG_OUTPUT
#endif

#ifndef MA_NO_VORBIS
#define STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"
#endif

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>

#ifndef MA_NO_VORBIS
/* stb_vorbis implementation must come after the implementation of miniaudio. */
#undef STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"
#endif


/**
 *  x
 */
static ma_log *_Log;
//static ma_decoder *_Decoder;
//static ma_context *_Context;


/**
 *  x
 * 
 *  @author: CCHyper
 */
#ifndef MA_NO_WWADPCM

#include "soscodec.h"
#include "wwaud.h"

static ma_result Audio_WWADPCM_decoding_backend_init(void *pUserData, ma_read_proc onRead, ma_seek_proc onSeek, ma_tell_proc onTell, void *pReadSeekTellUserData, const ma_decoding_backend_config *pConfig, const ma_allocation_callbacks *pAllocationCallbacks, ma_data_source **ppBackend)
{
    (void)pUserData;

    ma_result result;
    _SOS_COMPRESS_INFO_2 *pSOS;

    pSOS = (_SOS_COMPRESS_INFO_2 *)ma_malloc(sizeof(*pSOS), pAllocationCallbacks);
    if (pSOS == nullptr) {
        return MA_OUT_OF_MEMORY;
    }

    //sosCODEC2InitStream();

    *ppBackend = pSOS;

    return MA_SUCCESS;
}

static ma_result Audio_WWADPCM_decoding_backend_init_file(void *pUserData, const char *pFilePath, const ma_decoding_backend_config *pConfig, const ma_allocation_callbacks *pAllocationCallbacks, ma_data_source **ppBackend)
{
    (void)pUserData;

    ma_result result;
    _SOS_COMPRESS_INFO_2 *pSOS;

    pSOS = (_SOS_COMPRESS_INFO_2 *)ma_malloc(sizeof(*pSOS), pAllocationCallbacks);
    if (pSOS == nullptr) {
        return MA_OUT_OF_MEMORY;
    }

    *ppBackend = pSOS;

    return MA_SUCCESS;
}

static void Audio_WWADPCM_decoding_backend_uninit(void *pUserData, ma_data_source *pBackend, const ma_allocation_callbacks *pAllocationCallbacks)
{
    (void)pUserData;

    _SOS_COMPRESS_INFO_2 *pSOS = (_SOS_COMPRESS_INFO_2*)pBackend;

    ma_free(pSOS, pAllocationCallbacks);
}

static ma_decoding_backend_vtable Audio_DecodingBackend_Westwood_ADPCM =
{
    Audio_WWADPCM_decoding_backend_init,
    Audio_WWADPCM_decoding_backend_init_file,
    nullptr, /* onInitFileW() */    // Not required
    nullptr, /* onInitMemory() */   // Not required
    Audio_WWADPCM_decoding_backend_uninit
};

#endif


/**
 *  x
 */
static ma_decoding_backend_vtable *Audio_DecodingBackendVTables[] =
{
#ifndef MA_NO_WWADPCM
    &Audio_DecodingBackend_Westwood_ADPCM
#else
    nullptr
#endif
};


/**
 *  MiniAudio callback functions that use the C&C engine file io.
 * 
 *  Read:
 *    Function used to read data from memory.
 *    
 *    ptr = Pointer to the buffer that the vorbis files need.
 *    size = How big a byte is.
 *    count = How much we should read.
 *    datasource = This is a pointer to the data we passed into ov_open_callbacks.
 * 
 *  Seek:
 *    Function used to seek to a specific part of the file in memory.
 * 
 *    datasource = This is a pointer to the data we passed into ov_open_callbacks.
 *    offset = The offset from the point we wish to seek to.
 *    origin = Where we want to seek to.
 * 
 *  Close:
 *    Function used to close the file in memory.
 * 
 *    datasource = This is a pointer to the data we passed into ov_open_callbacks.
 * 
 *  Tell:
 *    Function used to tell how much we have read so far.
 * 
 *    datasource = This is a pointer to the data we passed into ov_open_callbacks.
 */

static ma_result Audio_CCFile_onOpen(ma_vfs *pVFS, const char *pFilePath, ma_uint32 openMode, ma_vfs_file *pFile)
{
    (void)pVFS;

    if (pFile == nullptr) {
        return MA_INVALID_ARGS;
    }

    CCFileClass *hFile = new CCFileClass(pFilePath);
    if (hFile == nullptr) {
        return MA_OUT_OF_MEMORY;
    }

    if (!hFile->Is_Available()) {
        return MA_DOES_NOT_EXIST;
    }

    bool opened = false;

    if ((openMode & MA_OPEN_MODE_READ) != 0) {
        opened = hFile->Open(FILE_ACCESS_READ);

    } else if ((openMode & MA_OPEN_MODE_WRITE) != 0) {
        opened = hFile->Open(FILE_ACCESS_WRITE);
    }

    if (!opened || !hFile->Is_Open()) {
        return MA_DOES_NOT_EXIST;
    }

    if (pFile != nullptr) {
        *pFile = hFile;
    }

    return MA_SUCCESS;
}

static ma_result Audio_CCFile_onOpenW(ma_vfs *pVFS, const wchar_t *pFilePath, ma_uint32 openMode, ma_vfs_file *pFile)
{
    (void)pVFS;

    if (pFile == nullptr) {
        return MA_INVALID_ARGS;
    }

    CCFileClass *hFile = nullptr; // new CCFileClass(pFilePath);
    if (hFile == nullptr) {
        return MA_OUT_OF_MEMORY;
    }

    if (!hFile->Is_Available()) {
        return MA_DOES_NOT_EXIST;
    }

    bool opened = false;

    if ((openMode & MA_OPEN_MODE_READ) != 0) {
        opened = hFile->Open(FILE_ACCESS_READ);

    } else if ((openMode & MA_OPEN_MODE_WRITE) != 0) {
        opened = hFile->Open(FILE_ACCESS_WRITE);
    }

    if (!opened || !hFile->Is_Open()) {
        return MA_DOES_NOT_EXIST;
    }

    if (pFile != nullptr) {
        *pFile = hFile;
    }

    return MA_SUCCESS;
}

static ma_result Audio_CCFile_onClose(ma_vfs *pVFS, ma_vfs_file file)
{
    (void)pVFS;

    if (file == nullptr) {
        return MA_INVALID_ARGS;
    }

    CCFileClass *hFile = reinterpret_cast<CCFileClass *>(file);

    hFile->Close();

    delete hFile;

    return MA_SUCCESS;
}

static ma_result Audio_CCFile_onRead(ma_vfs *pVFS, ma_vfs_file file, void *pDst, size_t sizeInBytes, size_t *pBytesRead)
{
    (void)pVFS;

    if (file == nullptr || pDst == nullptr || sizeInBytes <= 0 || pBytesRead == nullptr) {
        return MA_INVALID_ARGS;
    }

    CCFileClass *hFile = reinterpret_cast<CCFileClass *>(file);

    long totalBytesRead = hFile->Read(pDst, sizeInBytes);
    if (totalBytesRead == 0) {
        *pBytesRead = 0; // #BUGFIX: Will be fixed upstream in miniaudio soon.
        return MA_AT_END;
    }

    if (pBytesRead != nullptr) {
        *pBytesRead = totalBytesRead;
    }

    return MA_SUCCESS;
}

static ma_result Audio_CCFile_onWrite(ma_vfs *pVFS, ma_vfs_file file, const void *pSrc, size_t sizeInBytes, size_t *pBytesWritten)
{
    (void)pVFS;

    if (file == nullptr || pSrc == nullptr || sizeInBytes <= 0 || pBytesWritten == nullptr) {
        return MA_INVALID_ARGS;
    }

    CCFileClass *hFile = reinterpret_cast<CCFileClass *>(file);

    long totalBytesWritten = hFile->Write(pSrc, sizeInBytes);
    if (totalBytesWritten == 0) {
        *pBytesWritten = 0; // #BUGFIX: Will be fixed upstream in miniaudio soon.
        return MA_AT_END;
    }

    if (pBytesWritten != nullptr) {
        *pBytesWritten = totalBytesWritten;
    }

    return MA_SUCCESS;
}

static ma_result Audio_CCFile_onSeek(ma_vfs *pVFS, ma_vfs_file file, ma_int64 offset, ma_seek_origin origin)
{
    (void)pVFS;

    if (file == nullptr) {
        return MA_INVALID_ARGS;
    }

    CCFileClass *hFile = reinterpret_cast<CCFileClass *>(file);

    off_t off = (uint32_t)(offset);
    off_t retoff = 0;

    switch (origin) {
        default:
        case ma_seek_origin_start:
            retoff = hFile->Seek(off, FILE_SEEK_START);
            break;

        case ma_seek_origin_current:
            retoff = hFile->Seek(off, FILE_SEEK_CURRENT);
            break;

        case ma_seek_origin_end:
            retoff = hFile->Seek(off, FILE_SEEK_START);
            break;
    };

    if (retoff < 0) {
        return MA_BAD_SEEK;
    }

    return MA_SUCCESS;
}

static ma_result Audio_CCFile_onTell(ma_vfs *pVFS, ma_vfs_file file, ma_int64 *pCursor)
{
    (void)pVFS;

    if (file == nullptr) {
        return MA_INVALID_ARGS;
    }

    CCFileClass *hFile = reinterpret_cast<CCFileClass *>(file);

    if (pCursor != nullptr) {
        *pCursor = hFile->Tell();
    }

    return MA_SUCCESS;
}

static ma_result Audio_CCFile_onInfo(ma_vfs *pVFS, ma_vfs_file file, ma_file_info *pInfo)
{
    (void)pVFS;

    if (file == nullptr) {
        return MA_INVALID_ARGS;
    }

    CCFileClass *hFile = reinterpret_cast<CCFileClass *>(file);

    if (pInfo != nullptr) {
        pInfo->sizeInBytes = hFile->Size();
    }

    return MA_SUCCESS;
}


/**
 *  x
 */
static ma_vfs_callbacks Audio_CCFileCallbacks = {
    Audio_CCFile_onOpen,
    nullptr, /* onOpenW() */    // Not required, the TibSun engine's file IO does not use wide strings.
    Audio_CCFile_onClose,
    Audio_CCFile_onRead,
    nullptr, /* onWrite() */    // Not required, we don't needs to write audio files.
    Audio_CCFile_onSeek,
    Audio_CCFile_onTell,
    Audio_CCFile_onInfo
};


/**
 *  x
 * 
 *  @author: CCHyper
 */
static void Audio_log_callback(void *pUserData, ma_uint32 level, const char *pMessage)
{
    switch (level)
    {
#ifndef NDEBUG
        case MA_LOG_LEVEL_DEBUG:
            AUDIO_DEBUG_INFO("DEBUG: %s", pMessage);
            break;
#endif
        default:
        case MA_LOG_LEVEL_INFO:
            AUDIO_DEBUG_INFO("%s", pMessage);
            break;
        case MA_LOG_LEVEL_WARNING:
            AUDIO_DEBUG_WARNING("%s", pMessage);
            break;
        case MA_LOG_LEVEL_ERROR:
            AUDIO_DEBUG_ERROR("%s", pMessage);
            break;
    };
}


/**
 *  x
 */
static std::thread Audio_Cleanup_Thread;
static volatile bool Audio_Cleanup_Thread_Active = false;
static volatile bool Audio_Cleanup_Thread_Running = false;
static volatile bool Audio_Cleanup_Thread_InLoop = false;

static void __cdecl Audio_Cleanup_Thread_Function()
{
    AUDIO_DEBUG_INFO("Audio::Cleanup - Entering thread.\n");

    Audio_Cleanup_Thread_Running = true;

    while (Audio_Cleanup_Thread_Active) {

        Audio_Cleanup_Thread_InLoop = true;

        for (int group = 0; group < AudioManagerClass::SoundTrackerArrayType::CollectionCount; ++group) {

            DynamicVectorClass<AudioHandleClass *> &group_list = AudioManager.SoundTracker.Raw(group);

#ifndef NDEBUG
            if (group_list.Count() > 0) {
                //AUDIO_DEBUG_WARNING("Audio::Cleanup - Processing group list %d with %d entries!\n", group, group_list.Count());
            }
#endif

            /**
             *  x
             */
#if 0
            int index = group_list.Count();
            while (index > 0) {

                AudioHandleClass *handle = group_list[index];

                if (!handle) {
                    AUDIO_DEBUG_WARNING("Audio::Cleanup - Null audio handle detected!\n");
                    continue;
                }

                if (!handle->IsAtEnd) {
                    continue;
                }

                Wstring name = handle->Get_Filename();

                handle->Free();

                group_list.Delete(index);

                --index = group_list.Count() - 1;

                AUDIO_DEBUG_WARNING("Audio::Cleanup - Removed %s!\n", name.Peek_Buffer());

            }

#else

            int count = group_list.Count();
            for (int index = 0; index < group_list.Count(); ++index) {

                AudioHandleClass* handle = group_list[index];
                if (!handle) {
                    AUDIO_DEBUG_WARNING("Audio::Cleanup - Null audio handle detected!\n");
                    continue;
                }

#ifndef NDEBUG
                //AUDIO_DEBUG_WARNING("Audio::Cleanup - %d %d IsPlaying %d, IsPaused %d, IsFinished %d!\n",
                //    handle->Group, index, handle->Is_Playing(), handle->Is_Paused(), handle->Is_Finished());
#endif

                // Should this sound handle be removed from the tracker?
                bool remove = false;
                bool fadeout = false;

#if 0
                /**
                 *  x
                 */
                if (handle->ConcurrentLimit > 0) {
                    int playing_count = 0;
                    for (int j = 0; j < group_list.Count(); ++j) {
                        if (group_list[j]->Filename == handle->Filename) ++playing_count;
                    }
                    if (playing_count >= handle->ConcurrentLimit) {
                        AUDIO_DEBUG_WARNING("Audio::Cleanup - Concurrent limit of %d reached, removing \"%s\"\n.",
                            handle->ConcurrentLimit, handle->Filename.Peek_Buffer());
                        remove = true;
                    }

                 /**
                  *  Has the sound finished? Remove it.
                  */
                } else
#endif
                if (handle->Is_Finished()) {
                    remove = true;

                    /**
                     *  This is to catch any handles that are lingering about and force
                     *  their removal. Why this happens, I have no idea...
                     */
                } else
                if (!handle->Is_Playing() && !handle->Is_Paused() && !handle->Is_Finished() && !handle->IsDelaySet) {
#ifndef NDEBUG
                    AUDIO_DEBUG_WARNING("Audio::Cleanup - Found lingering handle at index %d, forcing removal of \"%s\"...\n",
                        index, handle->Filename.Peek_Buffer());
#endif
                    remove = true;

                    /**
                     *  x
                     */
                } else
                if (!handle->Is_Playing() && !handle->Is_Paused()) {
                    remove = true;
                }

                if (fadeout) {

                    handle->Stop(0.25f, true);
                
                } else if (remove) {

#ifndef NDEBUG
                    //AUDIO_DEBUG_WARNING("Audio::Cleanup - Removing \"%s\".\n", handle->Filename.Peek_Buffer());
#endif

                    handle->Stop();
                    handle->Free();

                    bool removed = group_list.Delete(index);
                    ASSERT_FATAL(removed);

                    //delete handle;

#ifndef NDEBUG
                    //AUDIO_DEBUG_WARNING("Audio::Cleanup - Removed successfully!\n");
#endif
                }

            }

#endif

        }

        // Sleep the thread.
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(500));

        //std::this_thread::yield();

        Audio_Cleanup_Thread_InLoop = false;
    }

    Audio_Cleanup_Thread_Running = false;

    AUDIO_DEBUG_INFO("Audio::Cleanup - Exiting thread.\n");
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioManagerClass::AudioManagerClass() :
    IsAvailable(false),
    FocusRestoreVolume(AUDIO_VOLUME_MAX),
    Engine(nullptr),
    Device(nullptr),
    SoundGroups(),
    SoundTracker()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioManagerClass::~AudioManagerClass()
{
    End();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioManagerClass::Init(HWND hWnd)
{
    ma_result result;

    if (IsAvailable) {
        AUDIO_DEBUG_WARNING("Audio: System already initialized!\n");
        return true;
    }

    AUDIO_DEBUG_INFO("Audio: Init...\n");

#if 0
    /**
     *  x
     */
    if (Vinifera_DeveloperMode) {
        AUDIO_DEBUG_INFO("Audio: About to create debug window.\n");
        if (!Create_Debug_Window()) {
            AUDIO_DEBUG_ERROR("Audio: Failed to create debug window!\n");
            return false;
        }
    }
#endif

#if 0
    /**
     *  
     */
    _log = new ma_log;
    ASSERT(_log != nullptr);

    result = ma_log_init(nullptr, _log);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio: Failed to initialize engine (%s)!\n", ma_result_description(result));
        return false;
    }

    ma_log_register_callback(_log, ma_log_callback_init(Audio_log_callback, nullptr));
#endif

    /**
     *  
     */
    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.pResourceManagerVFS = &Audio_CCFileCallbacks; // Assign the custom virtual file system.
    //engineConfig.pLog = _log; // Assign the custom logging handler.
    engineConfig.noAutoStart = MA_TRUE;

    // x
    engineConfig.sampleRate = 48000;
    engineConfig.channels = 2;
    engineConfig.monoExpansionMode = ma_mono_expansion_mode_stereo_only;

    Engine = new ma_engine;
    ASSERT(Engine != nullptr);

    result = ma_engine_init(&engineConfig, Engine);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio: Failed to initialize engine (%s)!\n", ma_result_description(result));
        return false;
    }

    AUDIO_DEBUG_INFO("Audio: Engine initialized.\n");

#if 0
    /**
     *  
     */
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig->playback.format = ma_format_f32;
    deviceConfig->pUserData = nullptr;

    device = new ma_device;
    ASSERT(device != nullptr);

    result = ma_device_init(nullptr, &deviceConfig, device);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio: Failed to initialize device (%s)!\n", ma_result_description(result));
        return false;
    }

    AUDIO_DEBUG_INFO("Audio: Device initialized.\n");
#endif

#if 0
    /**
     *  Initialise a context so we can enumerate our playback devices.
     */
    context = new ma_context;
    ASSERT(context != nullptr);

    result = ma_context_init(nullptr, 0, nullptr, context);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio: Failed to initialize context (%s)!\n", ma_result_description(result));
        return false;
    }

    AUDIO_DEBUG_INFO("Audio: Context initialized.\n");
#endif

#if 0
    /**
     *  x
     */
    ma_decoder_config decoderConfig = ma_decoder_config_init_default();
    decoderConfig.pCustomBackendUserData = nullptr;
    decoderConfig.ppCustomBackendVTables = Audio_DecodingBackendVTables;
    decoderConfig.customBackendCount = ARRAY_SIZE(Audio_DecodingBackendVTables);

    result = ma_decoder_init(&decoderConfig);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio: Failed to initialize decoder (%s)!\n", ma_result_description(result));
        return false;
    }

    AUDIO_DEBUG_INFO("Audio: Custom decoders initialized.\n");
#ifndef MA_NO_WWADPCM
    AUDIO_DEBUG_INFO("        - Westwood ADPCM.\n");
#endif
#endif

    /**
     *  x
     */
    for (int group = 0; group < AUDIO_GROUP_COUNT; ++group) {

        SoundGroups[group] = new ma_sound_group;
        ASSERT(SoundGroups[group] != nullptr);

        result = ma_sound_group_init(Engine, MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, SoundGroups[group]);
        if (result != MA_SUCCESS) {
            AUDIO_DEBUG_ERROR("Audio: Failed to initialize sound group %d (%s)!\n", group, ma_result_description(result));
            return false;
        }

        result = ma_sound_group_start(SoundGroups[group]);
        if (result != MA_SUCCESS) {
            AUDIO_DEBUG_ERROR("Audio: Failed to start sound group %d (%s)!\n", group, ma_result_description(result));
            return false;
        }

        AUDIO_DEBUG_INFO("Audio: Sound groups initialized.\n");
    }

    /**
     *  x
     */
    Set_Master_Volume(AUDIO_VOLUME_MAX);

    /**
     *  x
     */
    Audio_Cleanup_Thread_Active = true;
    Audio_Cleanup_Thread = std::thread(&Audio_Cleanup_Thread_Function);
    Audio_Cleanup_Thread.detach(); // The thread is now free, and runs on its own.

    Start_Engine(true);

    AUDIO_DEBUG_INFO("Audio: Init done!\n");

    //IsEnabled = true;
    IsAvailable = true;

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::End()
{
    /*
     *  Uninitialize the device first to ensure the data callback is stopped
     *  and doesn't try to access any data.
     */
    ma_device_uninit(Device);
    delete Device;
    Device = nullptr;

    /**
     *  Uninitialize the sound groups.
     */
    for (int i = 0; i < AUDIO_GROUP_COUNT; ++i) {
        ma_sound_group_uninit(SoundGroups[i]);
        delete SoundGroups[i];
        SoundGroups[i] = nullptr;
    }

    /**
     *  Now we can uninitialize the engine.
     */
    ma_engine_uninit(Engine);
    delete Engine;
    Engine = nullptr;

    /**
     *  The context can only be uninitialized after the devices.
     */
    //ma_context_uninit(context);
    //delete context;

    /**
     *  x
     */
    //ma_log_uninit(_log);
    //delete _log;

    /**
     *  
     */
    //Sounds.Clear();
    SoundTracker.Clear_All();

    IsAvailable = false;
    //IsEnabled = false;

    Audio_Cleanup_Thread_Active = false;
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
    return true; // IsEnabled;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::Enable()
{
    //IsEnabled = true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::Disable()
{
    //IsEnabled = false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioManagerClass::Start_Engine(bool forced)
{
    AUDIO_DEBUG_INFO("Audio::Start_Engine().\n");

    ma_result result;

    result = ma_engine_start(Engine);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio: Failed to start engine (%s)!\n", ma_result_description(result));
        return false;
    }

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioManagerClass::Stop_Engine()
{
    AUDIO_DEBUG_INFO("Audio::Stop_Engine().\n");

    ma_result result;

    result = ma_engine_stop(Engine);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio: Failed to stop engine (%s)!\n", ma_result_description(result));
        return false;
    }

    return true;
}


/**
 *  Handle application focus loss.
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::Focus_Loss()
{
    AUDIO_DEBUG_INFO("Audio::Focus_Loss().\n");

    ma_result result;

    result = ma_engine_set_volume(Engine, 0.0f);
    if (result != MA_SUCCESS) {
        //return false;
    }
}


/**
 *  Handle application focus restore.
 * 
 *  @author: CCHyper
 */
void AudioManagerClass::Focus_Restore()
{
    AUDIO_DEBUG_INFO("Audio::Focus_Restore().\n");

    ma_result result;

    result = ma_engine_set_volume(Engine, FocusRestoreVolume);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio::Focus_Restore - ma_engine_set_volume failed (%s)!\n", ma_result_description(result));
    }
}


/**
 *  x
 *
 *  @author: CCHyper
 */
AudioHandleClass * AudioManagerClass::Play(Wstring filename, AudioGroupType group, float volume, float pitch, float pan, AudioPriorityType priority, int limit, float fade_in_seconds, float delay_in_seconds, bool start)
{
    ma_result result;

    AudioHandleClass *handle = new AudioHandleClass(filename, SoundGroups[group], group);
    ASSERT(handle != nullptr);

    if (!handle->Is_Valid()) {
        AUDIO_DEBUG_ERROR("Audio::Play - Invalid handle returned!\n");
        return false;
    }

    if (volume <= 0.0f) {
        AUDIO_DEBUG_WARNING("Audio::Play - Volume is zero, skipping playback!\n");
        return false;
    }

    AUDIO_DEBUG_INFO("Audio::Play - %s : SampleRate %d, Channels %d, Volume %f, Pitch %f, Pan %f.\n",
                        filename.Peek_Buffer(), handle->Get_Sample_Rate(), handle->Get_Channels(), volume, pitch, pan);

    handle->Set_Volume(volume);
    handle->Set_Pitch(pitch);
    handle->Set_Pan(pan);

    if (fade_in_seconds > 0.0f) {
        handle->Fade_In(fade_in_seconds);
    }

    if (delay_in_seconds > 0.0f) {
        handle->Set_Delay(delay_in_seconds);
    }

    handle->Set_Limit(limit);

    if (start) {
        if (!handle->Start()) {
            AUDIO_DEBUG_ERROR("Audio::Play - Failed to start!\n");
            return false;
        }
    }

    Add_To_Tracker(handle, group);

    return handle;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Stop(AudioHandleClass *handle)
{
    if (!handle) {
        AUDIO_DEBUG_ERROR("Audio::Stop - Null handle!\n");
        return false;
    }

    if (!handle->Stop()) {
        AUDIO_DEBUG_ERROR("Audio::Stop - Failed to stop handle!\n");
        return false;
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Set_Master_Volume(float volume) const
{
    ma_result result;

    result = ma_engine_set_volume(Engine, AUDIO_VOLUME_MAX);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio: Failed to set engine volume (%s)!\n", ma_result_description(result));
        return false;
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Set_Group_Volume(AudioGroupType group, float volume)
{
    ma_sound_group_set_volume(SoundGroups[group], volume);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
float AudioManagerClass::Get_Group_Volume(AudioGroupType group)
{
    float volume = ma_sound_group_get_volume(SoundGroups[group]);

    return volume;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Is_Group_Playing(AudioGroupType group) const
{
    ma_bool32 result;

    result = ma_sound_group_is_playing(SoundGroups[group]);

    return result;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Start_Group(AudioGroupType group) const
{
    ma_result result;

    result = ma_sound_group_start(SoundGroups[group]);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio::Start_Group - ma_sound_group_start failed (%s)!\n", ma_result_description(result));
        return false;
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Stop_Group(AudioGroupType group) const
{
    ma_result result;

    result = ma_sound_group_stop(SoundGroups[group]);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_ERROR("Audio::Stop_Group - ma_sound_group_stop failed (%s)!\n", ma_result_description(result));
        return false;
    }

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Stop_And_Fade_Out_Group(AudioGroupType group) const
{
    return true; // TODO
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void * AudioManagerClass::Get_DirectSound_Object() const
{
    return Device->dsound.pPlayback;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void * AudioManagerClass::Get_DirectSound_Primary_Buffer() const
{
    return Device->dsound.pPlaybackPrimaryBuffer;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void * AudioManagerClass::Get_DirectSound_Buffer() const
{
    return Device->dsound.pPlaybackBuffer;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Is_FileType_Supported(AudioFileType type) const
{
    switch (type) {
#ifndef MA_NO_OPUS
        case AUDIO_TYPE_OPUS: return true;
#endif
#ifndef MA_NO_VORBIS
        case AUDIO_TYPE_OGG: return true;
#endif
#ifndef MA_NO_FLAC
        case AUDIO_TYPE_FLAC: return true;
#endif
#ifndef MA_NO_MP3
        case AUDIO_TYPE_MP3: return true;
#endif
#ifndef MA_NO_WAV
        case AUDIO_TYPE_WAV: return true;
#endif
#ifndef MA_NO_WWADPCM
        case AUDIO_TYPE_AUD: return true;
#endif
        default: break;
    };

    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Is_File_Available(AudioFileType type, Wstring name) const
{
    switch (type) {
#ifndef MA_NO_OPUS
        case AUDIO_TYPE_OPUS:
            name += ".OPUS";
            break;
#endif
#ifndef MA_NO_VORBIS
        case AUDIO_TYPE_OGG:
            name += ".OGG";
            break;
#endif
#ifndef MA_NO_FLAC
        case AUDIO_TYPE_FLAC:
            name += ".FLAC";
            break;
#endif
#ifndef MA_NO_MP3
        case AUDIO_TYPE_MP3:
            name += ".MP3";
            break;
#endif
#ifndef MA_NO_WAV
        case AUDIO_TYPE_WAV:
            name += ".WAV";
            break;
#endif
#ifndef MA_NO_WWADPCM
        case AUDIO_TYPE_AUD:
            name += ".AUD";
            break;
#endif
        default: break;
    };

    /**
     *  Search for the file in the mix files.
     */
    if (CCFileClass(name.Peek_Buffer()).Is_Available()) {
        return true;
    }

    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
Wstring AudioManagerClass::Build_Filename_From_Type(AudioFileType type, Wstring name)
{
    switch (type) {
#ifndef MA_NO_OPUS
        case AUDIO_TYPE_OPUS: return name + ".OPUS";
#endif
#ifndef MA_NO_VORBIS
        case AUDIO_TYPE_OGG: return name + ".OGG";
#endif
#ifndef MA_NO_FLAC
        case AUDIO_TYPE_FLAC: return name + ".FLAC";
#endif
#ifndef MA_NO_MP3
        case AUDIO_TYPE_MP3: return name + ".MP3";
#endif
#ifndef MA_NO_WAV
        case AUDIO_TYPE_WAV: return name + ".WAV";
#endif
#ifndef MA_NO_WWADPCM
        case AUDIO_TYPE_AUD: return name + ".AUD";
#endif
        default: break;
    };

    return nullptr;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
int AudioManagerClass::AudioPriority_To_Priority(AudioPriorityType priority)
{
    int adj_priority = priority / 5;
    int priority_step = 255 / 5;

    int retval = 255 / 2;

    if (priority == AUDIO_PRIORITY_LOWEST) {
        return priority_step;
    }
    if (priority == AUDIO_PRIORITY_LOW) {
        return priority_step*2;
    }
    if (priority == AUDIO_PRIORITY_NORMAL) {
        return priority_step*3;
    }
    if (priority == AUDIO_PRIORITY_HIGH) {
        return priority_step*4;
    }
    if (priority == AUDIO_PRIORITY_CRITICAL) {
        return priority_step*5;
    }

    return retval;
}


/**
 *  Utility function for converting from game priority to the new priority type.
 *
 *  @author: CCHyper
 */
AudioPriorityType AudioManagerClass::Priority_To_AudioPriority(int priority)
{
    int adj_priority = priority / 5;
    int priority_step = 255 / 5;

    AudioPriorityType retval = AUDIO_PRIORITY_NORMAL;

    if (adj_priority <= priority_step) {
        return AUDIO_PRIORITY_LOWEST;
    }
    if (adj_priority <= priority_step*2) {
        return AUDIO_PRIORITY_LOW;
    }
    if (adj_priority <= priority_step*3) {
        return AUDIO_PRIORITY_NORMAL;
    }
    if (adj_priority <= priority_step*4) {
        return AUDIO_PRIORITY_HIGH;
    }
    if (adj_priority <= priority_step*5) {
        return AUDIO_PRIORITY_CRITICAL;
    }

    return retval;
}


/**
 *  Utility functions for converting the integer audio volume to and from float.
 *
 *  @author: CCHyper
 */
unsigned int AudioManagerClass::fVolume_To_iVolume(float vol)
{
    vol = std::clamp(vol, 0.0f, 1.0f);
    return (vol * 255);
}

float AudioManagerClass::iVolume_To_fVolume(unsigned int vol)
{
    return float(vol) / 255.0f;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Audio_Set_Data(Wstring name, bool &available, AudioFileType &filetype, Wstring &filename, bool ignore_error)
{
    /**
     *  Check to see if the audio file exists in known formats. If found, store
     *  its type and filename so it can be passed into the audio engine at play.
     * 
     *  Priority: FLAC -> WAV -> OGG -> MP3 -> AUD
     */

#ifndef MA_NO_FLAC
    if (Is_File_Available(AUDIO_TYPE_FLAC, name)) {
        available = true;
        filetype = AUDIO_TYPE_FLAC;
        filename = Build_Filename_From_Type(AUDIO_TYPE_FLAC, name);
        AUDIO_DEBUG_INFO("Audio - Found \"%s\".\n", filename.Peek_Buffer());
        return true;
    }
#endif

#ifndef MA_NO_WAV
    if (Is_File_Available(AUDIO_TYPE_WAV, name)) {
        available = true;
        filetype = AUDIO_TYPE_WAV;
        filename = Build_Filename_From_Type(AUDIO_TYPE_WAV, name);
        AUDIO_DEBUG_INFO("Audio - Found \"%s\".\n", filename.Peek_Buffer());
        return true;
    }
#endif

#ifndef MA_NO_VORBIS
    if (Is_File_Available(AUDIO_TYPE_OGG, name)) {
        available = true;
        filetype = AUDIO_TYPE_OGG;
        filename = Build_Filename_From_Type(AUDIO_TYPE_OGG, name);
        AUDIO_DEBUG_INFO("Audio - Found \"%s\".\n", filename.Peek_Buffer());
        return true;
    }
#endif

#ifndef MA_NO_MP3
    if (Is_File_Available(AUDIO_TYPE_MP3, name)) {
        available = true;
        filetype = AUDIO_TYPE_MP3;
        filename = Build_Filename_From_Type(AUDIO_TYPE_MP3, name);
        AUDIO_DEBUG_INFO("Audio - Found \"%s\".\n", filename.Peek_Buffer());
        return true;
    }
#endif

#ifndef MA_NO_WWADPCM
    if (Is_File_Available(AUDIO_TYPE_AUD, name)) {
        available = true;
        filetype = AUDIO_TYPE_AUD;
        filename = Build_Filename_From_Type(AUDIO_TYPE_AUD, name);
        AUDIO_DEBUG_INFO("Audio - Found \"%s\".\n", filename.Peek_Buffer());
        return true;
    }
#endif

    if (ignore_error) {
        AUDIO_DEBUG_WARNING("Audio - Unable to find \"%s\" in a supported format!\n", name.Peek_Buffer());
    }
    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioManagerClass::Add_To_Tracker(AudioHandleClass *handle, AudioGroupType group)
{
    // Wait for the thread to finish its cleanup before adding to the list.
    //while (Audio_Cleanup_Thread_InLoop) { AUDIO_DEBUG_WARNING("Audio: Waiting on thread...\n"); }

    DynamicVectorClass<AudioHandleClass *> &group_list = SoundTracker.Raw(group);

    return group_list.Add(handle);
}


#endif
