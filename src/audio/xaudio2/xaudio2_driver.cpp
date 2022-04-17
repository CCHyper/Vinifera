/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          XAUDIO2_DRIVER.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Installable XAudio2 audio driver.
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
#include "xaudio2_driver.h"
#include "xaudio2_resource.h"
#include "xaudio2_stream.h"
#include "xaudio2_debug.h"
#include "xaudio2_globals.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <chrono>
#include <thread>
#include <vector>


// TODO
// consumer and producer threads for handling preloading and playback.
// https://stackoverflow.com/questions/16277840/c-syncing-threads-in-most-elegant-way


struct PreloadRequest
{
    bool operator != (const PreloadRequest & that) const { return Filename != that.Filename && Type != that.Type; }
    bool operator == (const PreloadRequest & that) const { return Filename == that.Filename && Type == that.Type; }

    Wstring Filename;
    AudioPreloadType Type;
};

// Files requested be preloaded into the audio engine.
//DynamicVectorClass<PreloadRequest> PreloadRequests;
static std::vector<PreloadRequest> PreloadRequests;

template<typename T>
void pop_front(std::vector<T> & vec)
{
    //ASSERT(!vec.empty());
    vec.erase(vec.begin());
}





static Wstring SoundRequestName;
static Wstring MusicRequestName;




#if 0

enum JobRequestType
{
    JOB_PLAY,
    JOB_STOP,
};

typedef struct AudioJobRequest
{
    AudioJobRequest(JobRequestType type, Wstring &filename, float volume, float pitch, int priority, bool loop) :
        Type(type),
        Filename(filename),
        Volume(volume),
        Pitch(pitch),
        Priority(priority),
        IsLoop(loop)
    {
    }

    ~AudioJobRequest()
    {
    }

    bool operator != (const AudioJobRequest & that) const { return Type != that.Type && Filename != that.Filename; }
    bool operator == (const AudioJobRequest & that) const { return Type == that.Type && Filename == that.Filename; }

    JobRequestType Type;

    Wstring Filename;
    float Volume;
    float Pitch;
    int Priority;
    bool IsLoop;

} AudioJobRequest;

static DynamicVectorClass<AudioJobRequest *> JobRequests;

#endif



static std::thread XAudio2_Sound_Thread;
static volatile bool XAudio2_Sound_Thread_Active = false;
static volatile bool XAudio2_Sound_Thread_Running = false;

static void __cdecl XAudio2_Sound_Thread_Function()
{
    XAUDIO2_DEBUG_INFO("XAudio2: Starting sound thread.\n");

    XAudio2_Sound_Thread_Running = true;

    while (XAudio2_Sound_Thread_Active) {

        // Sleep the thread.
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(50));

        reinterpret_cast<XAudio2AudioDriver *>(Audio_Driver())->Process_File_Preload();

        if (SoundRequestName.Is_Not_Empty()) {
            reinterpret_cast<XAudio2AudioDriver *>(Audio_Driver())->Handle_Sound_Request();
            SoundRequestName.Release_Buffer();
        }

        reinterpret_cast<XAudio2AudioDriver *>(Audio_Driver())->Thread_Sound_Callback();

        if (MusicRequestName.Is_Not_Empty()) {
            reinterpret_cast<XAudio2AudioDriver *>(Audio_Driver())->Handle_Music_Request();
            MusicRequestName.Release_Buffer();
        }

        reinterpret_cast<XAudio2AudioDriver *>(Audio_Driver())->Thread_Music_Callback();
    }

    XAudio2_Sound_Thread_Running = false;

    XAUDIO2_DEBUG_INFO("XAudio2: Exiting sound thread.\n");
}




XAudio2AudioDriver::XAudio2AudioDriver() :
    AudioDriver("XAudio2"),
    IsAvailable(false),
    IsEnabled(false),
    MasterVolume(1.0f),
    SoundVolume(1.0f),
    MusicVolume(1.0f),
    Sounds(32),
    Music(nullptr),
    SoundBankIndex(),
    MusicBankIndex()
{
}

XAudio2AudioDriver::~XAudio2AudioDriver()
{
}

bool XAudio2AudioDriver::Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels)
{
    HRESULT hr;

    AudioEngine = nullptr;
    MasterVoice = nullptr;

    //hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    //if (FAILED(hr)) {
    //    XAUDIO2_DEBUG_WARNING("CoInitializeEx failed in XAudio2AudioDriver constructor.\n");
    //}

    UINT32 flags = XAUDIO2_STOP_ENGINE_WHEN_IDLE;

#ifdef _DEBUG
    flags |= XAUDIO2_DEBUG_ENGINE;
#endif

    // Create XAudio2 Engine.
    hr = XAudio2Create(&AudioEngine, flags);
    if (FAILED(hr)) {
        return false;
    }

    // Create Master Voice for Engine.
    hr = AudioEngine->CreateMasteringVoice(&MasterVoice);
    if (FAILED(hr)) {
        SafeRelease(AudioEngine);
        return false;
    }

    // create basic samples.
    for (int i = 0; i < Sounds.Length(); ++i) {
        Sounds[i] = new XAudio2Stream();
    }

    // sound maintenance thread.
    XAudio2_Sound_Thread_Active = true;
    XAudio2_Sound_Thread = std::thread(&XAudio2_Sound_Thread_Function);
    XAudio2_Sound_Thread.detach(); // The thread is now free, and runs on its own.

    IsAvailable = true;
    IsEnabled = true;

    return true;
}

void XAudio2AudioDriver::End()
{
    XAudio2_Sound_Thread_Active = false;

    Sounds.Clear();

    delete Music;
    Music = nullptr;

    SoundBankIndex.Clear();
    MusicBankIndex.Clear();
    PreloadRequests.clear();
    
    if (MasterVoice != nullptr)
        MasterVoice->DestroyVoice();
        MasterVoice = nullptr;

    SafeRelease(AudioEngine);

    //CoUninitialize();
}

bool XAudio2AudioDriver::Is_Available() const
{
    return IsAvailable;
}

bool XAudio2AudioDriver::Is_Enabled() const
{
    return IsEnabled;
}

void XAudio2AudioDriver::Enable()
{
    IsEnabled = true;
}

void XAudio2AudioDriver::Disable()
{
    IsEnabled = false;
}

bool XAudio2AudioDriver::Play_Music(Wstring filename)
{
    MusicRequestName = filename;
    
    return true;
}

bool XAudio2AudioDriver::Pause_Music()
{
    if (!Music) {
        return false;
    }

    return Music->Pause();
}

bool XAudio2AudioDriver::UnPause_Music()
{
    if (!Music) {
        return false;
    }

    return Music->UnPause();
}

bool XAudio2AudioDriver::Stop_Music()
{
    if (!Music) {
        return false;
    }

    //return Music->Stop();
    delete Music;
    Music = nullptr;

    return true;
}

void XAudio2AudioDriver::Fade_In_Music(int seconds, float step)
{
    if (!Music) {
        return;
    }

    Music->Fade_In(seconds, step);
}

void XAudio2AudioDriver::Fade_Out_Music(int seconds, float step)
{
    if (!Music) {
        return;
    }
    
    Music->Fade_Out(seconds, step);
}

bool XAudio2AudioDriver::Is_Music_Playing() const
{
    if (!Music) {
        return false;
    }

    return Music->Is_Playing();
}


bool XAudio2AudioDriver::Is_Music_Paused() const
{
    if (!Music) {
        return false;
    }

    return Music->Is_Paused();
}


bool XAudio2AudioDriver::Is_Music_Fading_In() const
{
    if (!Music) {
        return false;
    }

    return Music->IsFadingIn;
}

bool XAudio2AudioDriver::Is_Music_Fading_Out() const
{
    if (!Music) {
        return false;
    }

    return Music->IsFadingOut;
}

void XAudio2AudioDriver::Set_Music_Volume(float volume)
{
    //ASSERT(volume >= 0.0f);
    //ASSERT(volume <= 1.0f);

    if (!Music) {
        return;
    }

    //volume = std::clamp(volume, -1.0f, 1.0f);
    Music->Set_Volume(volume);
}

float XAudio2AudioDriver::Get_Music_Volume()
{
    if (!Music) {
        return false;
    }

    return Music->Get_Volume();
}

bool XAudio2AudioDriver::Start_Engine(bool forced)
{
    if (AudioEngine) {
        HRESULT hr = AudioEngine->StartEngine();
        return SUCCEEDED(hr);
    }

    return false;
}

void XAudio2AudioDriver::Stop_Engine()
{
    if (AudioEngine) {
        AudioEngine->StopEngine();
    }
}

void XAudio2AudioDriver::Focus_Loss()
{
    XAUDIO2_DEBUG_INFO("XAudio2::Focus_Loss().\n");

    if (MasterVoice) {

        /**
         *  Pause the music track.
         */
        if (Music) {
            Music->Pause();
        }

        // #NOTE: We don't pause the sound effects here as that would
        //        create a situation where the gameply and sounds are 
        //        out of sync when focus is regained.

        /**
         *  Store the current master volume before muting.
         */
        MasterVoice->GetVolume(&MasterVolume);

        MasterVoice->SetVolume(0.0f);
    }

    InFocus = false;
}

void XAudio2AudioDriver::Focus_Restore()
{
    XAUDIO2_DEBUG_INFO("XAudio2::Focus_Restore().\n");

    if (MasterVoice) {

        /**
         *  Resume the music track.
         */
        if (Music) {
            Music->UnPause();
        }

        // #NOTE: We don't resume the sound effects here as that would
        //        create a situation where the gameply and sounds are 
        //        out of sync when focus is regained.

        /**
         *  
         */
        //float current_volume = 1.0f;
        //MasterVoice->GetVolume(&current_volume);
        //if (current_volume != MasterVolume) {
            MasterVoice->SetVolume(MasterVolume);
        //}
    }

    InFocus = true;
}

void XAudio2AudioDriver::Sound_Callback()
{
    // XAudio2 driver runs in its own maintance thread.
}

bool XAudio2AudioDriver::Handle_Sound_Request()
{
    return true;
}

void XAudio2AudioDriver::Thread_Sound_Callback()
{
    if (!IsAvailable || !IsEnabled) {
        return;
    }
}

bool XAudio2AudioDriver::Handle_Music_Request()
{
    if (MusicRequestName.Is_Empty()) {
        XAUDIO2_DEBUG_WARNING("XAudio2::Play_File - Invalid filename!\n");
        return false;
    }

    if (!MusicBankIndex.Count()) {
        XAUDIO2_DEBUG_WARNING("XAudio2::Play_File - Music bank is empty!\n");
        return false;
    }

    unsigned int key = XAudio2_Get_Filename_Hash(MusicRequestName);
    if (!MusicBankIndex.Is_Present(key)) {
        XAUDIO2_DEBUG_WARNING("XAudio2::Play_File - Failed to find \"%s\" in Music bank!\n", MusicRequestName.Peek_Buffer());
        return false;
    }

    XAudio2SoundResource *resource = MusicBankIndex[key];
    if (!resource) {
        XAUDIO2_DEBUG_WARNING("XAudio2::Play_File - Sound resource is null!\n");
        return false;
    }

    XAudio2Stream *stream = XAudio2_Create_Sample_From_Resource(resource);
    if (!stream) {
        XAUDIO2_DEBUG_WARNING("XAudio2::Play_File - Sample is null!\n");
        return false;
    }

    // delete previous, replace it with our new one.
    delete Music;
    Music = stream;

    return true;
}

// maintain music streaming.
void XAudio2AudioDriver::Thread_Music_Callback()
{
    if (!IsAvailable || !IsEnabled) {
        return;
    }

    if (!Music) {
        return;
    }

    // open up pending streams.
    if (Music->Is_Pending()) {
        Music->Open_Stream();
    }

    if (Music->Is_Paused() || !Music->Is_Playing()) {
        return;
    }

    // Fill buffer with more data.
    Music->Update_Stream();

    // If we have finished, stop the stream (playing any remaining buffers).
    if (Music->Is_Finished()) {
        Music->Stop(true);

    // handle fading in.
    } else if (Music->IsFadingIn) {

        float vol = Music->Get_Volume();
        vol += (Music->FadeInStep / float(Music->FadeInSeconds));
        Music->Set_Volume(vol);

        XAUDIO2_DEBUG_INFO("XAudio2[Music] - Fading -> Vol %f.\n", vol);

        
    // handle fading out.
    } else if (Music->IsFadingOut) {

        float vol = Music->Get_Volume();
        vol -= (Music->FadeOutStep / float(Music->FadeOutSeconds));
        Music->Set_Volume(vol);

        XAUDIO2_DEBUG_INFO("XAudio2[Music] - Fading -> Vol %f.\n", vol);

        if (Music->Get_Volume() <= 0.0f) {
            XAUDIO2_DEBUG_INFO("XAudio2[Music] - Fading -> Stop.\n");
            Music->Stop(true);
        }
    }
}

void XAudio2AudioDriver::Process_Job_Requests()
{
#if 0
    for (int i = 0; i < JobRequests.Count(); ++i) {

        AudioJobRequest *req = JobRequests[i];
        if (!req) {
            continue;
        }

        unsigned int key = XAudio2_Get_Filename_Hash(req->Filename);
        if (!SoundBankIndex.Is_Present(key)) {
            XAUDIO2_DEBUG_WARNING("XAudio2[Job] - Failed to find \"%s\" in sound bank!\n", req->Filename.Peek_Buffer());
            continue;
        }

        XAudio2SoundResource *resource = SoundBankIndex[key];
        if (!resource) {
            XAUDIO2_DEBUG_WARNING("XAudio2[Job] - Sound resource is null!\n");
            continue;
        }

        switch (req->Type)
        {
            case JOB_STOP:
            {
                break;
            }

            case JOB_PLAY:
            {
                int playing_handle = Get_Free_Sample_Handle();
                if (playing_handle == INVALID_AUDIO_HANDLE) {
                    continue;
                }

                XAudio2Stream *stream = XAudio2_Create_Sample_From_Resource(resource);
                if (!stream) {
                    XAUDIO2_DEBUG_WARNING("XAudio2[Job] - Sample is null!\n");
                    continue;
                }

                // delete previous, replace it with our new one.
                delete ActiveStreams[playing_handle];
                ActiveStreams[playing_handle] = stream;

                if (!stream->Load(resource)) {
                    XAUDIO2_DEBUG_WARNING("XAudio2[Job] - Failed to load!\n");
                    continue;
                }

                if (!stream->Start()) {
                    XAUDIO2_DEBUG_WARNING("XAudio2[Job] - Failed to start!\n");
                    continue;
                }

                break;
            }
        };

    }
#endif
}

void XAudio2AudioDriver::Process_File_Preload()
{
    // Load all the requested files into the index.
    for (int i = 0; i < PreloadRequests.size(); ++i) {
    
        PreloadRequest &req = PreloadRequests.front();
        req.Filename.To_Upper();

        //XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Preloading \"%s\"...\n", req.Filename.Peek_Buffer());
    
        XAudio2SoundResource *resource = new XAudio2CCSoundResource();
        ASSERT(resource != nullptr);
    
        if (!resource->Load(req.Filename)) {
            XAUDIO2_DEBUG_ERROR("XAudio2[Preload]: Preloading of \"%s\" failed!\n", req.Filename.Peek_Buffer());
            pop_front(PreloadRequests); // Remove this request.
            continue;
        }

        unsigned int key = XAudio2_Get_Filename_Hash(resource->Get_Name());

        switch (req.Type) {

            default:
                XAUDIO2_DEBUG_WARNING("XAudio2[Preload]: File \"%s\" is invalid preload request!\n", req.Filename.Peek_Buffer());
                break;

            case PRELOAD_MUSIC:
            {    
                if (MusicBankIndex.Is_Present(key)) {
                    XAUDIO2_DEBUG_WARNING("XAudio2[Preload]: File \"%s\" is already present in music bank!\n", req.Filename.Peek_Buffer());
                    pop_front(PreloadRequests); // Remove this request.
                    continue;
                }
    
                if (!MusicBankIndex.Add_Index(key, resource)) {
                    XAUDIO2_DEBUG_ERROR("XAudio2[Preload]: Failed to add \"%s\" to music bank!\n", req.Filename.Peek_Buffer());
                    pop_front(PreloadRequests); // Remove this request.
                    continue;
                }

                break;
            }
            
            case PRELOAD_SFX:
            {    
                if (SoundBankIndex.Is_Present(key)) {
                    XAUDIO2_DEBUG_WARNING("XAudio2[Preload]: File \"%s\" is already present in sound bank!\n", req.Filename.Peek_Buffer());
                    pop_front(PreloadRequests); // Remove this request.
                    continue;
                }
    
                if (!SoundBankIndex.Add_Index(key, resource)) {
                    XAUDIO2_DEBUG_ERROR("XAudio2[Preload]: Failed to add \"%s\" to sound bank!\n", req.Filename.Peek_Buffer());
                    pop_front(PreloadRequests); // Remove this request.
                    continue;
                }

                break;
            }

        };

        // pop the element from the 
        pop_front(PreloadRequests);
    
        XAUDIO2_DEBUG_INFO("XAudio2[Preload]: File \"%s\" preloaded OK.\n", resource->Get_Name().Peek_Buffer());
    }

}

bool XAudio2AudioDriver::Request_Preload(Wstring filename, AudioPreloadType type)
{
    XAUDIO2_DEBUG_INFO("XAudio2: File \"%s\" has been requested for preloading.\n", filename.Peek_Buffer());

    PreloadRequests.push_back(PreloadRequest{filename, type});

    return true;
}

void XAudio2AudioDriver::Start_Preloader()
{
#if 0
    if (!PreloadRequests.size()) {
        return;
    }

    XAUDIO2_DEBUG_INFO("XAudio2: Preload of %d files requested.\n", PreloadRequests.Count());

#if 0
    // Load all the requested files into the index.
    for (int i = 0; i < PreloadRequests.Count(); ++i) {

        Wstring reqname = PreloadRequests[i];
        reqname.To_Upper();

        XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Preloading \"%s\"...\n", reqname.Peek_Buffer());

        XAudio2SoundResource *resource = new XAudio2SoundResource();
        ASSERT(resource != nullptr);

        if (!resource->Load(reqname)) {
            XAUDIO2_DEBUG_ERROR("XAudio2[Preload]: Preloading of \"%s\" failed!\n", reqname.Peek_Buffer());
            PreloadRequests.Delete(i); // Remove this request.
            continue;
        }

        unsigned int key = XAudio2_Get_Filename_Hash(resource->Filename);

        if (SoundBankIndex.Is_Present(key)) {
            XAUDIO2_DEBUG_WARNING("XAudio2[Preload]: File \"%s\" is already present in sound bank!\n", reqname.Peek_Buffer());
            PreloadRequests.Delete(i); // Remove this request.
            continue;
        }

        if (!SoundBankIndex.Add_Index(key, resource)) {
            XAUDIO2_DEBUG_ERROR("XAudio2[Preload]: Failed to add \"%s\" to sound bank!\n", reqname.Peek_Buffer());
            PreloadRequests.Delete(i); // Remove this request.
            continue;
        }

        XAUDIO2_DEBUG_INFO("XAudio2[Preload]: File \"%s\" preloaded OK.\n", resource->FilenameExt.Peek_Buffer());
    }

    PreloadRequests.Clear();
#endif

#endif

}

bool XAudio2AudioDriver::Is_Audio_File_Available(Wstring filename)
{
    return XAudio2_Is_File_Available(filename);
}
