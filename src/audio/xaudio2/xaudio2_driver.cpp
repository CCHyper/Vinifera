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
#include "audio_util.h"
#include "critsection.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <chrono>
#include <thread>
#include <vector>


IXAudio2 *XAudio2AudioDriver::AudioEngine = nullptr;
IXAudio2MasteringVoice *XAudio2AudioDriver::MasterVoice = nullptr;



// TODO
// consumer and producer threads for handling preloading and playback.
// https://stackoverflow.com/questions/16277840/c-syncing-threads-in-most-elegant-way



struct PreloadRequest
{
    bool operator != (const PreloadRequest & that) const { return Filename != that.Filename && Type != that.Type; }
    bool operator == (const PreloadRequest & that) const { return Filename == that.Filename && Type == that.Type; }

    Wstring Filename;
    AudioSampleType Type;
};

// Files requested be preloaded into the audio engine.
//DynamicVectorClass<PreloadRequest> PreloadRequests;
static std::vector<PreloadRequest> PreloadRequests;

#if 0
template<typename T>
static void vector_pop_front(std::vector<T> & vec)
{
    //ASSERT(!vec.empty());
    vec.erase(vec.begin());
}
#endif

template<typename T>
static void vector_erase(std::vector<T> & vec, T & elem)
{
    //ASSERT(!vec.empty());
    vec.erase(std::remove(vec.begin(), vec.end(), elem), vec.end());
}




static SimpleCriticalSectionClass XAudio2CriticalSection;




typedef struct RequestType
{
    //bool operator == (RequestType &that) const { return Name != that.Name; }
    //bool operator != (RequestType &that) const { return Name != that.Name; }

    RequestType(Wstring name, float volume) :
        Name(name),
        Volume(volume)
    {
    }

    Wstring Name;
    float Volume;
};

inline bool operator == (const RequestType &left, const RequestType &right)
{
    return left.Name != right.Name;
}

inline bool operator != (const RequestType &left, const RequestType &right)
{
    return left.Name != right.Name;
}

static std::vector<RequestType> SpeechRequests;
static std::vector<RequestType> MusicRequests;





typedef struct SoundEffectRequestType : public RequestType
{
    SoundEffectRequestType(Wstring name, float volume, float pan, int loop_count) :
        RequestType(name, volume),
        Pan(pan),
        LoopCount(loop_count)
    {
    }

    float Pan;
    int LoopCount;
};

static std::vector<SoundEffectRequestType> SoundEffectRequests;




static std::thread XAudio2_Preload_Thread;
static volatile bool XAudio2_Preload_Begin = false;

static std::thread XAudio2_Sound_Thread;
static volatile bool XAudio2_Sound_Thread_Active = false;
static volatile bool XAudio2_Sound_Thread_Running = false;




/**
 *  x
 * 
 *  @author: CCHyper
 */
void __cdecl XAudio2AudioDriver::Sound_Thread_Function()
{
    XAUDIO2_DEBUG_INFO("XAudio2: Entering sound thread.\n");

    XAudio2_Sound_Thread_Running = true;

    while (XAudio2_Sound_Thread_Active) {

        reinterpret_cast<XAudio2AudioDriver *>(Audio_Driver())->Handle_Requests();

        reinterpret_cast<XAudio2AudioDriver *>(Audio_Driver())->Thread_Callback();

        // Sleep the thread.
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(50));

        //std::this_thread::yield();
    }

    XAudio2_Sound_Thread_Running = false;

    XAUDIO2_DEBUG_INFO("XAudio2: Exiting sound thread.\n");
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void __cdecl XAudio2AudioDriver::Preload_Thread_Function()
{
    XAUDIO2_DEBUG_INFO("XAudio2: Entering preload thread.\n");

    while (true) {

        if (!XAudio2_Sound_Thread_Running) {
            break;
        }

        if (XAudio2_Preload_Begin) {
            reinterpret_cast<XAudio2AudioDriver *>(Audio_Driver())->Process_File_Preload_Requests();
            XAudio2_Preload_Begin = false;
        }

        // Sleep the thread.
        //std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(50));

        std::this_thread::yield();
    }

    XAUDIO2_DEBUG_INFO("XAudio2: Exiting preload thread.\n");
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
XAudio2AudioDriver::XAudio2AudioDriver() :
    AudioDriver("XAudio2"),
    IsAvailable(false),
    IsEnabled(false),
    MasterVolume(1.0f),
    SoundEffectVolume(1.0f),
    SpeechVolume(1.0f),
    MusicVolume(1.0f),
    //SoundEffectStreams(), //SoundEffectStream(nullptr),
    SoundEffectBankIndex(),
    MaxSimultaneousSounds(AudioMaxSimultaneousSounds),
    //SpeechStreams(), //SpeechStream(nullptr),
    SpeechBankIndex(),
    //MusicStreams(), //MusicStream(nullptr),
    MusicBankIndex(),
    //MusicStreamIsFinished(false),
    Streams()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
XAudio2AudioDriver::~XAudio2AudioDriver()
{
    // empty
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels)
{
    HRESULT hr;

    AudioEngine = nullptr;
    MasterVoice = nullptr;

    /**
     *  This only applies to sound effects.
     */
    MaxSimultaneousSounds = num_trackers;

    //hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    //if (FAILED(hr)) {
    //    XAUDIO2_DEBUG_WARNING("CoInitializeEx failed in XAudio2AudioDriver constructor.\n");
    //}

    UINT32 flags = XAUDIO2_STOP_ENGINE_WHEN_IDLE;

#ifdef _DEBUG
    flags |= XAUDIO2_DEBUG_ENGINE;
#endif

    // Create the XAudio2 Engine.
    hr = XAudio2Create(&AudioEngine, flags);
    if (FAILED(hr)) {
        return false;
    }

#ifdef _DEBUG
    XAUDIO2_DEBUG_CONFIGURATION debugConfig = {0};
    debugConfig.BreakMask = XAUDIO2_LOG_ERRORS;
    debugConfig.TraceMask = XAUDIO2_LOG_ERRORS;
    AudioEngine->SetDebugConfiguration(&debugConfig);
#endif

    // Create Master Voice for the Engine.
    hr = AudioEngine->CreateMasteringVoice(&MasterVoice);
    if (FAILED(hr)) {
        SafeRelease(AudioEngine);
        return false;
    }

    // sound maintenance thread.
    XAudio2_Sound_Thread_Active = true;
    XAudio2_Sound_Thread = std::thread(&Sound_Thread_Function);
    XAudio2_Sound_Thread.detach(); // The thread is now free, and runs on its own.

    // sound preloading threads.
    XAudio2_Preload_Thread = std::thread(&Preload_Thread_Function);
    XAudio2_Preload_Thread.detach(); // The thread is now free, and runs on its own.

    IsAvailable = true;
    IsEnabled = true;

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::End()
{
    XAudio2_Sound_Thread_Active = false;

    //delete SoundEffectStream;
    //SoundEffectStream = nullptr;
//    SoundEffectStreams.clear();

    //delete SpeechStream;
    //SpeechStream = nullptr;
//    SpeechStreams.clear();

    //delete MusicStream;
    //MusicStream = nullptr;
//    MusicStreams.clear();

    Streams.clear();

    SoundEffectBankIndex.Clear();
    SpeechBankIndex.Clear();
    MusicBankIndex.Clear();

    PreloadRequests.clear();
    
    if (MasterVoice != nullptr) {
        MasterVoice->DestroyVoice();
        MasterVoice = nullptr;
    }

    SafeRelease(AudioEngine);

    //CoUninitialize();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Is_Available() const
{
    return IsAvailable;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Is_Enabled() const
{
    return IsEnabled;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Enable()
{
    IsEnabled = true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Disable()
{
    IsEnabled = false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Start_Engine(bool forced)
{
    if (AudioEngine) {
        HRESULT hr = AudioEngine->StartEngine();
        return SUCCEEDED(hr);
    }

    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Stop_Engine()
{
    if (AudioEngine) {
        AudioEngine->StopEngine();
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Focus_Loss()
{
    XAUDIO2_DEBUG_INFO("XAudio2::Focus_Loss().\n");

    if (MasterVoice) {

        {

        //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

        // #NOTE: We don't pause the sound effects and eva speech here as that
        //        would create a situation where the gameply and sounds are 
        //        out of sync when focus is regained.

        /**
         *  Pause the music tracks.
         */
        for (auto &it : Streams) {
            if (it->Type == STREAM_MUSIC) {
                it->Pause();
            }
        }
        //if (MusicStream) {
        //    MusicStream->Pause();
        //}

        }

        /**
         *  Store the current master volume before muting.
         */
        MasterVoice->GetVolume(&MasterVolume);

        MasterVoice->SetVolume(0.0f);
    }

    InFocus = false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Focus_Restore()
{
    XAUDIO2_DEBUG_INFO("XAudio2::Focus_Restore().\n");

    if (MasterVoice) {

        {

        //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

        // #NOTE: We don't resume the sound effects and eva speech here as that
        //        would create a situation where the gameply and sounds are 
        //        out of sync when focus is regained.

        /**
         *  Resume the music track.
         */
        for (auto &it : Streams) {
            if (it->Type == STREAM_MUSIC) {
                it->UnPause();
            }
        }
        //if (MusicStream) {
        //    MusicStream->UnPause();
        //}

        }

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


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Sound_Callback()
{
    // XAudio2 API runs in its own maintance thread, so we don't need to implement this.
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Process_File_Preload_Requests()
{
    int count = PreloadRequests.size();

    // Load all the requested files into the index.
    for (int i = 0; i < PreloadRequests.size(); ++i) {
    
        PreloadRequest &req = PreloadRequests.front();
        req.Filename.To_Upper();

        //XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Preloading \"%s\"...\n", req.Filename.Peek_Buffer());
    
        XAudio2SoundResource *resource = new XAudio2CCSoundResource();
        ASSERT(resource != nullptr);
    
        if (!resource->Load(req.Filename)) {
            XAUDIO2_DEBUG_ERROR("XAudio2[Preload]: Preloading of \"%s\" failed!\n", req.Filename.Peek_Buffer());
            PreloadRequests.erase(PreloadRequests.begin()); // remove this request.
            continue;
        }

        unsigned int key = XAudio2_Get_Filename_Hash(resource->Get_Name());

        switch (req.Type) {

            default:
                XAUDIO2_DEBUG_WARNING("XAudio2[Preload]: File \"%s\" is an invalid preload request!\n", req.Filename.Peek_Buffer());
                break;

            case SAMPLE_SFX:
            {
                XAUDIO2_DEBUG_INFO("XAudio2[Preload]: SoundEffect \"%s\".\n", req.Filename.Peek_Buffer());

                if (SoundEffectBankIndex.Is_Present(key)) {
                    XAUDIO2_DEBUG_WARNING("XAudio2[Preload]: File \"%s\" is already present in SoundEffect bank!\n", req.Filename.Peek_Buffer());
                    PreloadRequests.erase(PreloadRequests.begin()); // remove this request.
                    continue;
                }
    
                if (!SoundEffectBankIndex.Add_Index(key, resource)) {
                    XAUDIO2_DEBUG_ERROR("XAudio2[Preload]: Failed to add \"%s\" to SoundEffect bank!\n", req.Filename.Peek_Buffer());
                    PreloadRequests.erase(PreloadRequests.begin()); // remove this request.
                    continue;
                }
            
                break;
            }

            case SAMPLE_SPEECH:
            {
                XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Speech \"%s\".\n", req.Filename.Peek_Buffer());

                if (SpeechBankIndex.Is_Present(key)) {
                    XAUDIO2_DEBUG_WARNING("XAudio2[Preload]: File \"%s\" is already present in Speech bank!\n", req.Filename.Peek_Buffer());
                    PreloadRequests.erase(PreloadRequests.begin()); // remove this request.
                    continue;
                }
    
                if (!SpeechBankIndex.Add_Index(key, resource)) {
                    XAUDIO2_DEBUG_ERROR("XAudio2[Preload]: Failed to add \"%s\" to Speech bank!\n", req.Filename.Peek_Buffer());
                    PreloadRequests.erase(PreloadRequests.begin()); // remove this request.
                    continue;
                }
            
                break;
            }

            case SAMPLE_MUSIC:
            {
                XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Music \"%s\".\n", req.Filename.Peek_Buffer());

                if (MusicBankIndex.Is_Present(key)) {
                    XAUDIO2_DEBUG_WARNING("XAudio2[Preload]: File \"%s\" is already present in Music bank!\n", req.Filename.Peek_Buffer());
                    PreloadRequests.erase(PreloadRequests.begin()); // remove this request.
                    continue;
                }
    
                if (!MusicBankIndex.Add_Index(key, resource)) {
                    XAUDIO2_DEBUG_ERROR("XAudio2[Preload]: Failed to add \"%s\" to Music bank!\n", req.Filename.Peek_Buffer());
                    PreloadRequests.erase(PreloadRequests.begin()); // remove this request.
                    continue;
                }

                break;
            }

        };

        PreloadRequests.erase(PreloadRequests.begin()); // remove this request.
    
        XAUDIO2_DEBUG_INFO("XAudio2[Preload]: File \"%s\" preloaded OK.\n", resource->Get_Name().Peek_Buffer());
    }

    if (count > 0) {
        XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Finished preloading of %d files.\n", count);
    }
}


bool XAudio2AudioDriver::Handle_Requests()
{
    if (SoundEffectRequests.size() > 0) {

        if (!SoundEffectBankIndex.Count()) {
            XAUDIO2_DEBUG_WARNING("XAudio2::Handle_SoundEffect_Request - SoundEffect bank is empty!\n");
            return false;
        }

        for (int i = 0; i < SoundEffectRequests.size(); ++i) {
    
            auto &it = SoundEffectRequests.front();

            if (it.Name.Is_Empty()) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_SoundEffect_Request - Invalid filename!\n");
                continue;
            }

            unsigned int key = XAudio2_Get_Filename_Hash(it.Name);
            if (!SoundEffectBankIndex.Is_Present(key)) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_SoundEffect_Request - Failed to find \"%s\" in SoundEffect bank!\n", it.Name.Peek_Buffer());
                continue;
            }

            XAudio2SoundResource *resource = SoundEffectBankIndex[key];
            if (!resource) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_SoundEffect_Request - Sound resource is null!\n");
                continue;
            }

            std::unique_ptr<XAudio2Stream> stream = XAudio2_Create_Sample_From_Resource(resource);
            if (!stream) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_SoundEffect_Request - Sample is null!\n");
                continue;
            }

            //SoundEffectStreams.push_back(std::move(stream));
            Streams.push_back(std::move(stream));

            SoundEffectRequests.erase(SoundEffectRequests.begin()); // pop front
        }

    }

    if (SpeechRequests.size() > 0) {

        if (!SpeechBankIndex.Count()) {
            XAUDIO2_DEBUG_WARNING("XAudio2::Handle_Speech_Request - Speech bank is empty!\n");
            return false;
        }

        for (int i = 0; i < SpeechRequests.size(); ++i) {
    
            auto &it = SpeechRequests.front();

            if (it.Name.Is_Empty()) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_Speech_Request - Invalid filename!\n");
                continue;
            }

            unsigned int key = XAudio2_Get_Filename_Hash(it.Name);
            if (!SpeechBankIndex.Is_Present(key)) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_Speech_Request - Failed to find \"%s\" in Speech bank!\n", it.Name.Peek_Buffer());
                continue;
            }

            XAudio2SoundResource *resource = SpeechBankIndex[key];
            if (!resource) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_Speech_Request - Sound resource is null!\n");
                continue;
            }

            std::unique_ptr<XAudio2Stream> stream = XAudio2_Create_Sample_From_Resource(resource);
            if (!stream) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_Speech_Request - Sample is null!\n");
                continue;
            }

            //SpeechStreams.push_back(std::move(stream));
            Streams.push_back(std::move(stream));

            SpeechRequests.erase(SpeechRequests.begin()); // pop front
        }

    }

    if (MusicRequests.size() > 0) {

        if (!MusicBankIndex.Count()) {
            XAUDIO2_DEBUG_WARNING("XAudio2::Handle_Music_Request - Music bank is empty!\n");
            return false;
        }

        for (int i = 0; i < MusicRequests.size(); ++i) {
    
            auto &it = MusicRequests.front();

            if (it.Name.Is_Empty()) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_Music_Request - Invalid filename!\n");
                continue;
            }

            unsigned int key = XAudio2_Get_Filename_Hash(it.Name);
            if (!MusicBankIndex.Is_Present(key)) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_Music_Request - Failed to find \"%s\" in Music bank!\n", it.Name.Peek_Buffer());
                continue;
            }

            XAudio2SoundResource *resource = MusicBankIndex[key];
            if (!resource) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_Music_Request - Sound resource is null!\n");
                continue;
            }

            std::unique_ptr<XAudio2Stream> stream = XAudio2_Create_Sample_From_Resource(resource);
            if (!stream) {
                XAUDIO2_DEBUG_WARNING("XAudio2::Handle_Music_Request - Sample is null!\n");
                continue;
            }

            //MusicStreams.push_back(std::move(stream));
            Streams.push_back(std::move(stream));
        
            MusicRequests.erase(MusicRequests.begin()); // pop front
        }

    }

    return true;
}


void XAudio2AudioDriver::Thread_Callback()
{
    if (!IsAvailable || !IsEnabled) {
        return;
    }

#if 0
    for (int i = 0; i < SpeechStreams.size(); ++i) {
    
        XAudio2Stream *stream = SpeechStreams.front().get();

        // open up pending streams.
        if (stream->Is_Pending()) {
            XAUDIO2_DEBUG_INFO("XAudio2[Speech] - Opening pending stream for \"%s\".\n", stream->Get_Name().Peek_Buffer());
            if (!stream->Open_Stream()) {
                MusicStreams.erase(MusicStreams.begin()); // pop front
            }
            continue;
        }

        if (stream->Is_Paused()) {
            XAUDIO2_DEBUG_INFO("XAudio2[Speech] - \"%s\" is paused...\n", stream->Get_Name().Peek_Buffer());
            continue;
        }

        // Fill buffer with data.
        if (!stream->Update_Stream()) {
            XAUDIO2_DEBUG_ERROR("XAudio2[Speech] - Update stream failed -> Stopping.\n");
            SpeechStreams.erase(SpeechStreams.begin()); // pop front
            continue;
        }

        // If the buffer is depleted, stop the stream (and playing any remaining buffers).
        if (stream->Is_Buffer_Empty()) {
            XAUDIO2_DEBUG_INFO("XAudio2[Speech] - \"%s\" Buffer Empty -> Stopping.\n", stream->Get_Name().Peek_Buffer());
            stream->Stop(true);
            SpeechStreams.erase(SpeechStreams.begin()); // pop front
            continue;
        }

        // If the buffer is finished, now remove it.
        if (stream->Is_Finished()) {
            XAUDIO2_DEBUG_INFO("XAudio2[Speech] - \"%s\" Finished -> Removing.\n", stream->Get_Name().Peek_Buffer());
            SpeechStreams.erase(SpeechStreams.begin()); // pop front
            continue;
        }

    }

    for (int i = 0; i < MusicStreams.size(); ++i) {
    
        XAudio2Stream *stream = MusicStreams.front().get();

        // open up pending streams.
        if (stream->Is_Pending()) {
            XAUDIO2_DEBUG_INFO("XAudio2[Music] - Opening pending stream.\n");
            if (!stream->Open_Stream()) {
                MusicStreams.erase(MusicStreams.begin()); // pop front
            }
            continue;
        }

        if (stream->Is_Paused()) {
            XAUDIO2_DEBUG_INFO("XAudio2[Music] - \"%s\" is paused...\n", stream->Get_Name().Peek_Buffer());
            continue;
        }

        // Fill buffer with more data.
        if (!stream->Update_Stream()) {
            XAUDIO2_DEBUG_ERROR("XAudio2[Music] - Update stream failed -> Stopping.\n");
            MusicStreams.erase(MusicStreams.begin()); // pop front
            continue;
        }

        // If we have finished, stop the stream (playing any remaining buffers).
        if (stream->Is_Finished()) {
            XAUDIO2_DEBUG_INFO("XAudio2[Music] - Finished -> Stopping.\n");
            stream->Stop(true);
            MusicStreams.erase(MusicStreams.begin()); // pop front

            //MusicStreamIsFinished = false;

        // handle fading in.
        } else if (stream->IsFadingIn) {

            float vol = stream->Get_Volume();
            vol += (stream->FadeInStep / float(stream->FadeInSeconds));
            stream->Set_Volume(vol);

            XAUDIO2_DEBUG_INFO("XAudio2[Music] - Fading -> Vol %f.\n", vol);

        
        // handle fading out.
        } else if (stream->IsFadingOut) {

            float vol = stream->Get_Volume();
            vol -= (stream->FadeOutStep / float(stream->FadeOutSeconds));
            stream->Set_Volume(vol);

            XAUDIO2_DEBUG_INFO("XAudio2[Music] - Fading -> Vol %f.\n", vol);

            if (stream->Get_Volume() <= 0.0f) {
                XAUDIO2_DEBUG_INFO("XAudio2[Music] - Fading -> Stop.\n");
                stream->Stop(true);
                MusicStreams.erase(MusicStreams.begin()); // pop front
            }
        }

    }
#endif

    for (int i = 0; i < Streams.size(); ++i) {
    
        XAudio2Stream *stream = Streams.front().get();

        // open up pending streams.
        if (stream->Is_Pending()) {
            XAUDIO2_DEBUG_INFO("XAudio2[%s] - Opening pending stream.\n", stream->Get_Name().Peek_Buffer());
            if (!stream->Open_Stream()) {
                Streams.erase(Streams.begin()); // pop front
            }
            continue;
        }

        if (stream->Is_Paused()) {
            XAUDIO2_DEBUG_INFO("XAudio2[%s] - Is paused...\n", stream->Get_Name().Peek_Buffer(), stream->Get_Name().Peek_Buffer());
            continue;
        }

        // Fill buffer with more data.
        if (!stream->Update_Stream()) {
            XAUDIO2_DEBUG_ERROR("XAudio2[%s] - Update stream failed -> Stopping.\n", stream->Get_Name().Peek_Buffer());
            Streams.erase(Streams.begin()); // pop front
            continue;
        }

        // If we have finished, stop the stream (playing any remaining buffers).
        if (stream->Is_Finished()) {
            XAUDIO2_DEBUG_INFO("XAudio2[%s] - Finished -> Stopping.\n", stream->Get_Name().Peek_Buffer());
            stream->Stop(true);
            Streams.erase(Streams.begin()); // pop front
            continue;
        }

        if (stream->Type == STREAM_MUSIC) {

            // handle fading in.
            if (stream->IsFadingIn) {
                float vol = stream->Get_Volume();
                vol += (stream->FadeInStep / float(stream->FadeInSeconds));
                stream->Set_Volume(vol);
                XAUDIO2_DEBUG_INFO("XAudio2[%s] - Fading -> Vol %f.\n", vol, stream->Get_Name().Peek_Buffer());
                continue;
            }

            // handle fading out.
            if (stream->IsFadingOut) {
                float vol = stream->Get_Volume();
                vol -= (stream->FadeOutStep / float(stream->FadeOutSeconds));
                stream->Set_Volume(vol);
                XAUDIO2_DEBUG_INFO("XAudio2[%s] - Fading -> Vol %f.\n", vol, stream->Get_Name().Peek_Buffer());
                if (stream->Get_Volume() <= 0.0f) {
                    XAUDIO2_DEBUG_INFO("XAudio2[%s] - Fading -> Stop.\n", stream->Get_Name().Peek_Buffer());
                    stream->Stop(true);
                    Streams.erase(Streams.begin()); // pop front
                }
                continue;
            }

        }

    }
}


bool XAudio2AudioDriver::Play(AudioStreamType type, Wstring filename, float volume)
{
    bool play = false;

    switch (type) {
        case STREAM_SFX:
            ASSERT_FATAL_PRINT(true, "Please use Play_SoundEffect() for sound effects!");
            play = false;
            break;
        case STREAM_SPEECH:
            XAUDIO2_DEBUG_INFO("XAudio2[Speech]: File \"%s\" has been requested for playing.\n", filename.Peek_Buffer());
            SpeechRequests.emplace_back(filename, volume);
            play = true;
            break;
        case STREAM_MUSIC:
            XAUDIO2_DEBUG_INFO("XAudio2[Music]: File \"%s\" has been requested for playing.\n", filename.Peek_Buffer());
            MusicRequests.emplace_back(filename, volume);
            play = true;
            break;
        default:
            break;
    };

    return play;
}

// pan == left or right channel, one_shot == one time sound, otherwise loop_count is used.
bool XAudio2AudioDriver::Play_SoundEffect(Wstring filename, float volume, float pan, int loop_count)
{
    bool play = false;

    //switch (type) {
    //    case STREAM_SFX:
            XAUDIO2_DEBUG_INFO("XAudio2[SoundEffect]: File \"%s\" has been requested for playing.\n", filename.Peek_Buffer());
            SoundEffectRequests.emplace_back(filename, volume, pan, loop_count);
            play = true;
    //        break;
    //    case STREAM_SPEECH:
    //    case STREAM_MUSIC:
    //        ASSERT_FATAL_PRINT(true, "Please use Play() for non-sound effects!");
    //        play = false;
    //        break;
    //    default:
    //        break;
    //};

    return play;
}

bool XAudio2AudioDriver::Stop(AudioStreamType type, Wstring filename, bool force, bool all)
{
    bool stopped = false;

#if 0
    switch (type) {
        case STREAM_SFX:
            for (auto &it : SoundEffectStreams) {
                if (it->Get_Name() == filename) {
                    stopped |= it->Stop();
                    if (!all) {
                        break;
                    }
                }
            }
            break;
        case STREAM_SPEECH:
            for (auto &it : SpeechStreams) {
                if (it->Get_Name() == filename && it->Is_Playing()) {
                    stopped |= it->Stop();
                    break;
                }
            }
            break;
        case STREAM_MUSIC:
            for (auto &it : MusicStreams) {
                if (it->Get_Name() == filename && it->Is_Playing()) {
                    stopped |= it->Stop();
                    break;
                }
            }
            break;
        default:
            break;
    };
#endif

    for (auto &it : Streams) {
        if (it->Get_Name() == filename) {
            if (it->Type == type && it->Is_Playing()) {
                stopped |= it->Stop();
                if (!all) {
                    break;
                }
            }
        }
    }

    return stopped;
}

bool XAudio2AudioDriver::Is_Playing(AudioStreamType type, Wstring filename) const
{
    bool is_playing = false;

#if 0
    switch (type) {
        case STREAM_SFX:
            for (auto &it : SoundEffectStreams) {
                if (it->Get_Name() == filename && it->Is_Playing()) {
                    is_playing = true;
                    break;
                }
            }
            break;
        case STREAM_SPEECH:
            for (auto &it : SpeechStreams) {
                if (it->Get_Name() == filename && it->Is_Playing()) {
                    is_playing = true;
                    break;
                }
            }
            break;
        case STREAM_MUSIC:
            for (auto &it : MusicStreams) {
                if (it->Get_Name() == filename && it->Is_Playing()) {
                    is_playing = true;
                    break;
                }
            }
            break;
        default:
            break;
    };
#endif

    for (auto &it : Streams) {
        if (it->Get_Name() == filename) {
            if (it->Type == type && it->Is_Playing()) {
                is_playing = true;
                break;
            }
        }
    }

    return is_playing;
}

void XAudio2AudioDriver::Set_Volume(AudioStreamType type, Wstring filename, float volume)
{
#if 0
    switch (type) {
        case STREAM_SFX:
            for (auto &it : SoundEffectStreams) {
                if (it->Get_Name() == filename) {
                    it->Set_Volume(volume);
                    //break;    // TODO; break here or update all?
                }
            }
            break;
        case STREAM_SPEECH:
            for (auto &it : SpeechStreams) {
                if (it->Get_Name() == filename) {
                    it->Set_Volume(volume);
                    //break;    // TODO; break here or update all?
                }
            }
            break;
        case STREAM_MUSIC:
            for (auto &it : MusicStreams) {
                if (it->Get_Name() == filename) {
                    it->Set_Volume(volume);
                    //break;    // TODO; break here or update all?
                }
            }
            break;
        default:
            break;
    };
#endif

    for (auto &it : Streams) {
        if (it->Get_Name() == filename) {
            if (it->Type == type) {
                it->Set_Volume(volume);
                //break;    // TODO; break here or update all?
            }
        }
    }
}

float XAudio2AudioDriver::Get_Volume(AudioStreamType type, Wstring filename)
{
#if 0
    switch (type) {
        case STREAM_SFX:
            for (auto &it : SoundEffectStreams) {
                if (it->Get_Name() == filename) {
                    return it->Get_Volume();
                }
            }
            break;
        case STREAM_SPEECH:
            for (auto &it : SpeechStreams) {
                if (it->Get_Name() == filename) {
                    return it->Get_Volume();
                }
            }
            break;
        case STREAM_MUSIC:
            for (auto &it : MusicStreams) {
                if (it->Get_Name() == filename) {
                    return it->Get_Volume();
                }
            }
            break;
        default:
            break;
    };
#endif

    for (auto &it : Streams) {
        if (it->Get_Name() == filename) {
            if (it->Type == type) {
                return it->Get_Volume();
            }
        }
    }

    return 0.0f;
}

/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Request_Preload(Wstring filename, AudioSampleType type)
{
    XAUDIO2_DEBUG_INFO("XAudio2[Preload]: File \"%s\" has been requested for preloading.\n", filename.Peek_Buffer());

    PreloadRequests.push_back(PreloadRequest{filename, type});

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Start_Preloader(AudioSampleType type)
{
    if (!PreloadRequests.size()) {
        XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Preload start requested but list is empty!\n");
        return;
    }
    
    XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Preload of %d files requested.\n", PreloadRequests.size());

    XAudio2_Preload_Begin = true;
}


void XAudio2AudioDriver::Clear_Sample_Bank(AudioSampleType type)
{
    switch (type) {
        case SAMPLE_SFX:
            XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Clearing SoundEffect (%d) bank.\n", SoundEffectBankIndex.Count());
            SoundEffectBankIndex.Clear();
            break;
        case SAMPLE_SPEECH:
            XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Clearing Speech (%d) bank.\n", SoundEffectBankIndex.Count());
            SpeechBankIndex.Clear();
            break;
        case SAMPLE_MUSIC:
            XAUDIO2_DEBUG_INFO("XAudio2[Preload]: Clearing Music (%d) bank.\n", SoundEffectBankIndex.Count());
            MusicBankIndex.Clear();
            break;
        default:
            break;
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Is_Audio_File_Available(Wstring filename)
{
    return XAudio2_Is_File_Available(filename);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Debug_Dump_Sound_Banks()
{
#ifndef NDEBUG
#endif

    XAUDIO2_DEBUG_INFO("SoundEffect - Count: %d\n", SoundEffectBankIndex.Count());

    for (int i = 0; i < SoundEffectBankIndex.Count(); ++i) {
        XAudio2SoundResource *entry = SoundEffectBankIndex.Fetch_By_Position(i);
        XAUDIO2_DEBUG_INFO("  %04d: %s %s\n", i, entry->Get_Name().Peek_Buffer(), entry->Get_FullName().Peek_Buffer());
    }

    XAUDIO2_DEBUG_INFO("Speech - Count: %d\n", SpeechBankIndex.Count());

    for (int i = 0; i < SpeechBankIndex.Count(); ++i) {
        XAudio2SoundResource *entry = SpeechBankIndex.Fetch_By_Position(i);
        XAUDIO2_DEBUG_INFO("  %04d: %s %s\n", i, entry->Get_Name().Peek_Buffer(), entry->Get_FullName().Peek_Buffer());
    }

    XAUDIO2_DEBUG_INFO("Music - Count: %d\n", MusicBankIndex.Count());

    for (int i = 0; i < MusicBankIndex.Count(); ++i) {
        XAudio2SoundResource *entry = MusicBankIndex.Fetch_By_Position(i);
        XAUDIO2_DEBUG_INFO("  %04d: %s %s\n", i, entry->Get_Name().Peek_Buffer(), entry->Get_FullName().Peek_Buffer());
    }
}


/**
 *  
 *  Sound effect stream interface 
 *  
 */
 
#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Play_SoundEffect(Wstring filename, float volume, float pan, bool one_shot, int loop_count)
{
    // one_shot == one time sound, otherwise loop_count is used.

    XAUDIO2_DEBUG_INFO("XAudio2[SoundEffect]: File \"%s\" has been requested for playing.\n", filename.Peek_Buffer());

    SoundEffectRequests.emplace_back(filename, volume, pan, one_shot, loop_count);

    return false;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Stop_SoundEffect(Wstring filename, bool force)
{
    bool stopped = false;

    for (auto &it : SpeechStreams) {
        if (it->Get_Name() == filename && it->Is_Playing()) {
            stopped = true;
            break;
        }
    }

    return stopped;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Is_SoundEffect_Playing(Wstring filename) const
{
    bool is_playing = false;

    for (auto &it : SpeechStreams) {
        if (it->Get_Name() == filename && it->Is_Playing()) {
            is_playing = true;
            break;
        }
    }

    return is_playing;
}
#endif


#if 1
/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Set_SoundEffect_Volume(float volume)
{
    //ASSERT(volume >= 0.0f);
    //ASSERT(volume <= 1.0f);

    SoundEffectVolume = volume;

#if 0
    if (!SoundEffectStream) {
        return;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    //volume = std::clamp(volume, -1.0f, 1.0f);
    SoundEffectStream->Set_Volume(volume);
#endif
}
#endif


#if 1
/**
 *  x
 * 
 *  @author: CCHyper
 */
float XAudio2AudioDriver::Get_SoundEffect_Volume()
{
#if 0
    if (!SoundEffectStream) {
        return 0.0f;
    }
#endif

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    //return SoundEffectStream->Get_Volume();
    return SoundEffectVolume;
}
#endif


/**
 *  
 *  Speech stream interface 
 *  
 */


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Play_Speech(Wstring filename, float volume)
{
    XAUDIO2_DEBUG_INFO("XAudio2[Speech]: File \"%s\" has been requested for playing.\n", filename.Peek_Buffer());

    SpeechRequests.emplace_back(filename, volume);

    return true;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Stop_Speech(Wstring filename, bool force)
{
    bool stopped = false;

    for (auto &it : SpeechStreams) {
        if (it->Get_Name() == filename && it->Is_Playing()) {
            stopped = true;
            break;
        }
    }

    return stopped;
}
#endif


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Is_Speech_Playing(Wstring filename) const
{
    bool is_playing = false;

    for (auto &it : SpeechStreams) {
        if (it->Get_Name() == filename && it->Is_Playing()) {
            is_playing = true;
            break;
        }
    }

    return is_playing;
}
#endif


#if 1
/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Set_Speech_Volume(float volume)
{
    //ASSERT(volume >= 0.0f);
    //ASSERT(volume <= 1.0f);

    SpeechVolume = volume;

#if 0
    if (!SpeechStream) {
        return;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    //volume = std::clamp(volume, -1.0f, 1.0f);
    SpeechStream->Set_Volume(volume);
#endif
}
#endif


#if 1
/**
 *  x
 * 
 *  @author: CCHyper
 */
float XAudio2AudioDriver::Get_Speech_Volume()
{
#if 0
    if (!SpeechStream) {
        return 0.0f;
    }
#endif

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    //return SpeechStream->Get_Volume();
    return SpeechVolume;
}
#endif


/**
 *  
 *  Music stream interface 
 *  
 */

#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Play_Music(Wstring filename)
{
    XAUDIO2_DEBUG_INFO("XAudio2[Music]: File \"%s\" has been requested for playing.\n", filename.Peek_Buffer());

    MusicRequests.emplace_back(filename, 1.0f);

    return true;
}
#endif


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Pause_Music()
{
#if 0
    if (!MusicStream) {
        return false;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    return MusicStream->Pause();
#endif

    bool retval = false;

    for (auto &it : Streams) {
        if (it->Type == STREAM_MUSIC) {
            retval |= it->Pause();
        }
    }

    return retval;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::UnPause_Music()
{
#if 0
    if (!MusicStream) {
        return false;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    return MusicStream->UnPause();
#endif

    bool retval = false;

    for (auto &it : Streams) {
        if (it->Type == STREAM_MUSIC) {
            retval |= it->UnPause();
        }
    }

    return retval;
}


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Stop_Music()
{
#if 0
    if (!MusicStream) {
        return false;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    //return MusicStream->Stop();
    delete MusicStream;
    MusicStream = nullptr;

    return true;
#endif

    bool retval = false;

    for (auto &it : MusicStreams) {
        retval |= it->Stop();
        vector_erase(MusicStreams, it);
    }

    return retval;
}
#endif


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Fade_In_Music(int seconds, float step)
{
#if 0
    if (!MusicStream) {
        return;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    MusicStream->Fade_In(seconds, step);
#endif

    for (auto &it : Streams) {
        if (it->Type == STREAM_MUSIC) {
            it->Fade_In(seconds, step);
        }
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Fade_Out_Music(int seconds, float step)
{
#if 0
    if (!MusicStream) {
        return;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);
    
    MusicStream->Fade_Out(seconds, step);
#endif

    for (auto &it : Streams) {
        if (it->Type == STREAM_MUSIC) {
            it->Fade_Out(seconds, step);
        }
    }
}


#if 0
/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Is_Music_Playing() const
{
#if 0
    if (!MusicStream) {
        return false;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    return MusicStream->Is_Playing();
#endif

    bool retval = false;

    for (auto &it : MusicStreams) {
        retval |= it->Is_Playing();
    }

    return retval;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Is_Music_Finished() const
{
#if 0
    if (!MusicStream) {
        return false;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    return MusicStreamIsFinished;
    //return MusicStream->Is_Finished();
#endif

    bool retval = false;

    for (auto &it : MusicStreams) {
        retval |= it->Pause();
    }

    return retval;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Is_Music_Paused() const
{
#if 0
    if (!MusicStream) {
        return false;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    return MusicStream->Is_Paused();
#endif

    bool retval = false;

    for (auto &it : MusicStreams) {
        retval |= it->Is_Paused();
    }

    return retval;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Is_Music_Fading_In() const
{
#if 0
    if (!MusicStream) {
        return false;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    return MusicStream->IsFadingIn;
#endif

    bool retval = false;

    for (auto &it : MusicStreams) {
        retval |= it->IsFadingIn;
    }

    return retval;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2AudioDriver::Is_Music_Fading_Out() const
{
#if 0
    if (!MusicStream) {
        return false;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    return MusicStream->IsFadingOut;
#endif

    bool retval = false;

    for (auto &it : MusicStreams) {
        retval |= it->IsFadingOut;
    }

    return retval;
}
#endif


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2AudioDriver::Set_Music_Volume(float volume)
{
#if 0
    //ASSERT(volume >= 0.0f);
    //ASSERT(volume <= 1.0f);

    MusicVolume = volume;

    if (!MusicStream) {
        return;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    //volume = std::clamp(volume, -1.0f, 1.0f);
    MusicStream->Set_Volume(volume);
#endif

    for (auto &it : Streams) {
        if (it->Type == STREAM_MUSIC) {
            it->Set_Volume(volume);
        }
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
float XAudio2AudioDriver::Get_Music_Volume()
{
#if 0
    if (!MusicStream) {
        return 0.0f;
    }

    //ScopedCriticalSectionClass cs(&XAudio2CriticalSection);

    return MusicStream->Get_Volume();
#endif

    return 1.0f; // TODO
}
