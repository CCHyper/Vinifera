/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                    *
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_AHANDLE.CPP
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
 *  @note          This file contains heavily modified code from the source code
 *                 released by Electronic Arts for the C&C Remastered Collection
 *                 under the GPL3 license. Source:
 *                 https://github.com/ElectronicArts/CnC_Remastered_Collection
 *
 ******************************************************************************/

#include "audio_ahandle.h"
#include "audio_manager.h"
#include "audio_streaming.h"
#include "ahandle.h"
#include "gametime.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <mutex>


// Callbacks
long (__cdecl *MoveHMIAudioBlock_Callback)(VQAHandle *) = nullptr;
long (__cdecl *VQASync_Callback)(VQAHandle *, void *) = nullptr;


static std::mutex AudioHandleMutex;


static bool IsHandleOpen;
//static float Volume;

static unsigned short SampleRate = 22050;
static unsigned char Channels = 1;
static unsigned char BitsPerSample = 8;

static unsigned long LastBytesPlayed = 0;
static unsigned long LastUpdateTime = 0;
static int PauseAdjust = 0;
static unsigned long PlaybackTime60Hz = 0;
static unsigned long Flags = 0;

static void * CallbackBuffers[2] = { nullptr, nullptr };
static int CurrentBufferIndex = 0;

static void * CallbackBufferPtr = nullptr;

// Our new streaming instance
static AudioStreamingClass * StreamInstance = nullptr;

static std::atomic<bool> AudioHandleThreadExit = {false};
static std::thread AudioHandleThread;

/**
 *  x
 * 
 *  @author: CCHyper
 */
unsigned __stdcall Audio_Handler_Thread(void * context)
{
    auto next_tick = std::chrono::steady_clock::now();

    VQAHandle *vqa = (VQAHandle *)context;
    VQAHandleP *vqap = (VQAHandleP *)context;

    while (!AudioHandleThreadExit.load(std::memory_order_relaxed)) {

        if (!(Flags & AHANDLEF_IS_PAUSED)) {

            // Process next audio block

            if (MoveHMIAudioBlock_Callback) {
                DEBUG_INFO("AudioHandle: MoveHMIAudioBlock_Callback\n");
                MoveHMIAudioBlock_Callback((VQAHandle*)vqa);
            }

            if (VQASync_Callback && CallbackBufferPtr) {
                DEBUG_INFO("AudioHandle: VQASync_Callback\n");
                VQASync_Callback((VQAHandle*)vqa, (char*)CallbackBufferPtr);
            }

            //double blockDurationSec = double(vqa->Config.HMIBufSize) / (SampleRate * Channels * (BitsPerSample / 8));
            //std::this_thread::sleep_for(std::chrono::duration<double>(blockDurationSec));
        }

        next_tick += std::chrono::milliseconds(1000 / 60); // 60 Hz
        std::this_thread::sleep_until(next_tick);
    }

    return 0;
}


unsigned long __cdecl AudioHandleClass::Timer_Callback_Audio_Handler(VQAHandle *vqa)
{
    VQAHandleP* vqap = (VQAHandleP*)vqa;
    VQAConfig* config = &vqap->Config;

    if (Flags & AHANDLEF_IS_PAUSED) {
        return PlaybackTime60Hz;
    }

    unsigned long current_time = Simple_Timer_Callback_Audio_Handler(nullptr) - PauseAdjust;
    unsigned long bytes_played = Get_Total_Bytes_Played(vqa, config);

    if (bytes_played > 0 && bytes_played <= LastBytesPlayed) {
        if (current_time > LastUpdateTime) {
            bytes_played = (current_time - LastUpdateTime);
            LastUpdateTime = current_time;
            PlaybackTime60Hz += bytes_played;
        }
    } else {
        LastBytesPlayed = bytes_played;
        LastUpdateTime = current_time;
        PlaybackTime60Hz = (60 * (bytes_played / (Channels * (BitsPerSample / 8))) / SampleRate);

        if (PlaybackTime60Hz >= config->LatencyAdjustment) {
            PlaybackTime60Hz -= config->LatencyAdjustment;
        } else {
            PlaybackTime60Hz = 0;
        }
    }
    
    DEBUG_INFO("AudioHandle: Timer_Callback_Audio_Handler -> returning %d\n", PlaybackTime60Hz);

    return PlaybackTime60Hz;
}


// Compute bytes played (approx) using AudioStreamingClass
unsigned long AudioHandleClass::Get_Total_Bytes_Played(VQAHandle *vqa, VQAConfig *config)
{
    if (!StreamInstance) {
        return 0;
    }

    AudioHandleMutex.lock();
    // Approximate bytes played using total samples played
    uint64_t frames_played = StreamInstance->Get_Frames_Played();
    uint32_t frame_size = Channels * (BitsPerSample / 8);

    AudioHandleMutex.unlock();

    uint32_t total_bytes_played = static_cast<unsigned long>(frames_played * frame_size);

    DEBUG_INFO("AudioHandle: Get_Total_Bytes_Played -> returning %d\n", total_bytes_played);

    return total_bytes_played;
}


long __cdecl AudioHandleClass::Stream_Audio_Handler(VQAHandle *vqa, long action, void *buffer, long nbytes)
{
    VQAHandleP *vqap = (VQAHandleP *)vqa;
    VQAConfig *config = &vqap->Config;
    long error = VQAERR_NONE;

    switch(action) {
        case 1: // Callback
            config->TimerCallback = Simple_Timer_Callback_Audio_Handler;
            config->RefreshRate = 60;
            error = VQAERR_NONE;
            break;
        case 2: // Open
            error = Open_Audio_Handler(vqap, (AhandleInitParams *)buffer, nbytes);
            break;
        case 3: // Close
            error = Close_Audio_Handler(vqap);
            break;
        case 4: // Start
            error = Start_Audio_Handler(vqap);
            break;
        case 5: // Load
            error = Load_Audio_Handler(vqap, buffer, nbytes);
            break;
        case 6: // Pause
            error = Pause_Audio_Handler(vqap);
            break;
        case 7: // Stop
            error = Stop_Audio_Handler(vqap);
            break;
        case 8: // Play
            error = Play_Audio_Handler(vqap);
            break;
        default:
            break;
    }

    return(error);
}


long __cdecl AudioHandleClass::Open_Audio_Handler(VQAHandleP *vqap, AhandleInitParams *params, long b)
{
    DEBUG_INFO("AudioHandle: Opening VQ audio handler\n");

    if (!AudioManager.Is_Available()) {
        return VQAERR_AUDIO;
    }

    VQAConfig * config = &vqap->Config;

    IsHandleOpen = true;
    //Volume = config->Volume;

    std::lock_guard<std::mutex> lock(AudioHandleMutex);

    // TODO break this down!
    SampleRate = (config->AudioRate != -1) ? config->AudioRate
                 : (config->FrameRate != vqap->FrameRate)
                   ? params->SampleRate * (unsigned)config->FrameRate / vqap->FrameRate
                   : params->SampleRate;

    Channels = params->Channels;
    BitsPerSample = params->BitsPerSample;
    Flags = params->Flags;

	MoveHMIAudioBlock_Callback = params->Callback1; // 006A98D0, Move HMI Audio Block -> feeds next chunk to audio
	VQASync_Callback = params->Callback2; // 006A9A20, Sync callback -> informs VQA player that block is processed

    DEBUG_INFO("AudioHandle: Open_Audio_Handler -> %d %d %d\n", SampleRate, Channels, BitsPerSample);

    if (StreamInstance) {
        delete StreamInstance;
        StreamInstance = nullptr;
    }

    StreamInstance = new AudioStreamingClass();
    if (!StreamInstance->Open(Wstring("VQA_AUDIO_STREAM"), SampleRate, Channels, BitsPerSample, true)) {
        DEBUG_ERROR("AudioHandle: Failed to open VQ audio handler!\n");
        return VQAERR_AUDIO;
    }

    //AudioHandleThreadExit.store(false, std::memory_order_relaxed);
    AudioHandleThread = std::thread(&Audio_Handler_Thread, (VQAHandle *)vqap);

    DEBUG_INFO("AudioHandle: VQ audio handler opened\n");

    return VQAERR_NONE;
}


long __cdecl AudioHandleClass::Close_Audio_Handler(VQAHandleP *vqap)
{
    DEBUG_INFO("AudioHandle: Closing VQ audio handler\n");

    std::lock_guard<std::mutex> lock(AudioHandleMutex);

    if (!IsHandleOpen) {
        return VQAERR_NONE;
    }

    if (StreamInstance) {
        StreamInstance->Stop();
        delete StreamInstance;
        StreamInstance = nullptr;
    }

    IsHandleOpen = false;

    AudioHandleThreadExit.store(true, std::memory_order_relaxed);
    if (AudioHandleThread.joinable()) {
        AudioHandleThread.join();
    }

    return VQAERR_NONE;
}


long __cdecl AudioHandleClass::Start_Audio_Handler(VQAHandleP *vqap)
{
    if (!IsHandleOpen || StreamInstance == nullptr) {
        return VQAERR_AUDIO;
    }

    if ((Flags & AHANDLEF_IS_PAUSED) == 0) {
        DEBUG_ERROR("AudioHandle: Start_Audio_Handler -> Handle is paused!!\n");
        return VQAERR_AUDIO;
    }

    MoveHMIAudioBlock_Callback((VQAHandle *)vqap);

    DEBUG_INFO("AudioHandle: Starting VQ audio handler\n");
    return Play_Audio_Handler(vqap);
}


long __cdecl AudioHandleClass::Load_Audio_Handler(VQAHandleP *vqap, void *buffer, long nbytes)
{
    if (!StreamInstance || !buffer || nbytes <= 0)
        return VQAERR_AUDIO;

    std::lock_guard<std::mutex> lock(AudioHandleMutex);

    CallbackBuffers[CurrentBufferIndex] = buffer;
    CallbackBufferPtr = CallbackBuffers[CurrentBufferIndex];

    // Immediately push chunk into stream
    StreamInstance->Push_Chunk(buffer, nbytes);

    // Rotate index for next load
    CurrentBufferIndex = (CurrentBufferIndex + 1) % 2;

    DEBUG_INFO("AudioHandle: Rotated buffer -> idx = %d ptr = %p\n", CurrentBufferIndex, CallbackBufferPtr);

    DEBUG_INFO("AudioHandle: Pushed bytes %d to stream handler\n", nbytes);

    return VQAERR_NONE;
}


long __cdecl AudioHandleClass::Pause_Audio_Handler(VQAHandleP *vqap)
{
    std::lock_guard<std::mutex> lock(AudioHandleMutex);
    if (StreamInstance) {
        StreamInstance->Pause();
    }
    Flags |= AHANDLEF_IS_PAUSED;
    DEBUG_INFO("AudioHandle: Pausing VQ audio handler\n");
    return VQAERR_NONE;
}


long __cdecl AudioHandleClass::Play_Audio_Handler(VQAHandleP *vqap)
{
    std::lock_guard<std::mutex> lock(AudioHandleMutex);
    if (!StreamInstance) {
        return VQAERR_AUDIO;
    }
    Flags &= ~AHANDLEF_IS_PAUSED;
    DEBUG_INFO("AudioHandle: Playing VQ audio handler\n");
    return StreamInstance->Play() ? VQAERR_NONE : VQAERR_AUDIO;
}


long __cdecl AudioHandleClass::Stop_Audio_Handler(VQAHandleP *vqap)
{
    std::lock_guard<std::mutex> lock(AudioHandleMutex);
    DEBUG_INFO("AudioHandle: Stopping VQ audio handler\n");
    if (StreamInstance) {
        StreamInstance->Stop();
    }
    return VQAERR_NONE;
}


unsigned long __cdecl AudioHandleClass::Simple_Timer_Callback_Audio_Handler(VQAHandle *vqa)
{
    return Get_Game_Time_60Hz();
}


long __cdecl AudioHandleClass::Lock_Audio_Handler(void)
{
    DEBUG_INFO("AudioHandle: Locking handler\n");
    return(1);
}


long __cdecl AudioHandleClass::Unlock_Audio_Handler(void)
{
    DEBUG_INFO("AudioHandle: Unlocking handler\n");
    return(1);
}
