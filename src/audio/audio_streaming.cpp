/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_INSTANCE.CPP
 *
 *  @author        CCHyper
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

#include "audio_streaming.h"
#include "audio_manager.h"
#include "audio_decoders.h"
#include "audio_util.h"
#include "audio_debug.h"

//#define MINIAUDIO_IMPLEMENTATION      // Not needed here as we just want header info!
#include <miniaudio/miniaudio.h>



static std::mutex AudioStreamingMutex;


AudioStreamingClass::AudioStreamingClass() :
    Sound(nullptr),
    Decoder(),
    DecoderInitialized(false),
    DecoderIsOwnedBySound(false),
    HandleID(INVALID_AUDIO_HANDLE_ID),
    StreamInitialized(false),
    IsStreaming(false),
    IsPCM(false),
    PCMBuffer(nullptr),
    LogicalName(),
    SampleRate(22050),
    Channels(1),
    BitsPerSample(16),
    ChunkBuffer(),
    FramesPushed(0)
{
}


AudioStreamingClass::~AudioStreamingClass()
{
    Close();
}

bool AudioStreamingClass::Open(const Wstring& name, int sample_rate, int channels, int bits_per_sample, bool is_pcm)
{
    Close();

    LogicalName = name;
    SampleRate = sample_rate;
    Channels = channels;
    BitsPerSample = bits_per_sample;
    IsPCM = is_pcm;

    if (IsPCM) {
        return Initialize_PCM_Stream();
    } else {
        // AUD stream will be initialized on first Push_Chunk
        return true;
    }
}

void AudioStreamingClass::Close()
{
    std::lock_guard<std::mutex> lock(AudioStreamingMutex);

    if (Sound) {
        ma_sound_uninit(Sound);
        delete Sound;
        Sound = nullptr;
    }

    if (DecoderInitialized) {
        ma_decoder_uninit(Decoder);
        DecoderInitialized = false;
    }

    if (PCMBuffer) {
        ma_pcm_rb_uninit(PCMBuffer);
        delete PCMBuffer;
        PCMBuffer = nullptr;
    }

    ChunkBuffer.clear();

    StreamInitialized = false;

    HandleID = INVALID_AUDIO_HANDLE_ID;
}

bool AudioStreamingClass::Initialize_PCM_Stream()
{
    ma_result result;

    ma_format format = Audio_GetMAFormatFromBPS(BitsPerSample);

    // Create a ring buffer for streaming PCM
    PCMBuffer = new ma_pcm_rb;
    result = ma_pcm_rb_init(format,
                                   Channels,
                                   4096, // frames per page
                                   nullptr,
                                   nullptr,
                                   PCMBuffer);
    if (result != MA_SUCCESS) {
        delete PCMBuffer;
        PCMBuffer = nullptr;
        return false;
    }

    /**
     *  Create a new sound object.
     */
    if (Sound != nullptr) {
        delete Sound;
        Sound = nullptr;
    }
    Sound = new ma_sound;
    ASSERT(Sound != nullptr);

    // In Miniaudio, the ring buffer itself is a data source
    result = ma_sound_init_from_data_source(AudioManager.Engine, 
                                        &PCMBuffer->ds, 
                                        AUDIO_GROUP_STREAMING, 
                                        AudioManager.SoundGroups[AUDIO_GROUP_STREAMING], 
                                        Sound);
    if (result != MA_SUCCESS) {
        ma_pcm_rb_uninit(PCMBuffer);
        delete PCMBuffer;
        PCMBuffer = nullptr;
        return false;
    }

    ma_sound_set_volume(Sound, 1.0f);
    ma_sound_set_spatialization_enabled(Sound, MA_FALSE);

    StreamInitialized = true;

    return true;
}

#if 0
bool AudioStreamingClass::Initialize_AUD_Decoder(const void* initialData, size_t size)
{
    ma_result result;

    if (DecoderInitialized) {
        return true;
    }

    ma_sound_flags flags = ma_sound_flags(MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_STREAM);

    /**
     *  Create a new decoder object for handling any WS AUD files. This must
     *  be done within this branch to ensure we don't alloc the decoder object for
     *  non custom decoder files. ma_sound_init_from_data_source takes ownership.
     */
    Decoder = new ma_decoder;
    ASSERT(Decoder != nullptr);

    // Create a decoder config and register the custom backends.
    ma_decoder_config decoderConfig = ma_decoder_config_init_default();
    decoderConfig.pCustomBackendUserData = nullptr;
    decoderConfig.ppCustomBackendVTables = (ma_decoding_backend_vtable **)ma_custom_backend_vtable;
    decoderConfig.customBackendCount = sizeof(ma_custom_backend_vtable) / sizeof(ma_custom_backend_vtable[0]);

    //error in ma_decoder_init_from_vtable__internal?

    // Initialize the decoder using from a memory buffer with the AUD decoder backend.
    result = ma_decoder_init_memory(initialData, size, &decoderConfig, Decoder);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_MSG(LEVEL_ERROR, TYPE_INSTANCE, "AudioInstance::Load - ma_decoder_init_vfs failed (%s)!\n", ma_result_description(result));
        return false;
    }

    ASSERT_FATAL(Decoder != nullptr);

    DecoderInitialized = true; // Handy flag to ensure it was initialized correctly.

    // Attach the decoder to the sound system as a streaming data source.
    result = ma_sound_init_from_data_source(AudioManager.Engine, Decoder, flags, AudioManager.SoundGroups[AUDIO_GROUP_STREAMING], Sound);
    if (result != MA_SUCCESS) {
        AUDIO_DEBUG_MSG(LEVEL_ERROR, TYPE_INSTANCE, "AudioInstance::Load - ma_sound_init_from_data_source failed (%s)!\n", ma_result_description(result));
        return false;
    }

    DecoderIsOwnedBySound = true;

    return true;
}
#endif

bool AudioStreamingClass::Push_Chunk(const void* data, size_t size)
{
    std::lock_guard<std::mutex> lock(AudioStreamingMutex);

    if (IsPCM) {

        if (!PCMBuffer) {
            return false;
        }

        size_t frames = size / (Channels * (BitsPerSample / 8));
        size_t framesPushed = 0;
        const ma_uint8* src = (const ma_uint8*)data;

        FramesPushed += frames;

        while (framesPushed < frames) {

            ma_uint32 framesToWrite = (ma_uint32)(frames - framesPushed);
            void* dst;
            ma_uint32 framesWritable;

            ma_pcm_rb_acquire_write(PCMBuffer, &framesWritable, &dst);
            if (framesWritable == 0) {
                break; // buffer full
            }

            if (framesWritable > framesToWrite) framesWritable = (ma_uint32)framesToWrite;

            memcpy(dst, src + framesPushed * Channels * (BitsPerSample/8),
                   framesWritable * Channels * (BitsPerSample/8));

            ma_pcm_rb_commit_write(PCMBuffer, framesWritable);
            framesPushed += framesWritable;
        }

    } else {

        // AUD stream: Initialize decoder on first chunk
        //if (!DecoderInitialized) {
        //    return Initialize_AUD_Decoder(data, size);
        //}

        // For full streaming AUD: a custom streaming data source would be needed.
        return true;

    }

    return true;
}

bool AudioStreamingClass::Play()
{
    std::lock_guard<std::mutex> lock(AudioStreamingMutex);
    if (!Sound) {
        return false;
    }
    return ma_sound_start(Sound) == MA_SUCCESS;
}

bool AudioStreamingClass::Pause()
{
    std::lock_guard<std::mutex> lock(AudioStreamingMutex);
    if (!Sound) {
        return false;
    }
    return ma_sound_stop(Sound) == MA_SUCCESS;
}

bool AudioStreamingClass::Stop()
{
    std::lock_guard<std::mutex> lock(AudioStreamingMutex);
    if (!Sound) {
        return false;
    }
    return ma_sound_stop(Sound) == MA_SUCCESS;
}

bool AudioStreamingClass::Is_Playing() const
{
    if (!Sound) {
        return false;
    }
    return ma_sound_is_playing(Sound);
}

uint64_t AudioStreamingClass::Get_Frames_Played() const
{
    if (IsPCM && PCMBuffer) {
        ma_uint32 framesInBuffer = ma_pcm_rb_available_read(PCMBuffer);
        return FramesPushed - framesInBuffer;
    }

    // For AUD or non-buffered streams, just return FramesPushed
    return FramesPushed;
}
