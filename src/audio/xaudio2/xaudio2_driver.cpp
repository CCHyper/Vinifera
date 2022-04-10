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
#include "xaudio2_driver.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "audio_util.h"
#include "ccfile.h"
#include "ramfile.h"

#define MINIMP3_IMPLEMENTATION // Required for minimp3 lib.
#include <minimp3.h>

#include <vorbisfile.h>
#include "wwaud.h"

#include "dsaudio.h"                        // Required for some enums and types.

#include "debughandler.h"
#include "asserthandler.h"

#include <Windows.h>
#include <mmsystem.h>
#include <xaudio2.h>


#include <wrl\client.h>

#include "XAudio2Util.h"
//#include "WAVFileReader.h"


// Uncomment to enable the volume limiter on the master voice.
//#define MASTERING_LIMITER

using Microsoft::WRL::ComPtr;




#define SAFE_DESTROY_VOICE(voice) if ( voice ) { voice->DestroyVoice(); voice = nullptr; }


//--------------------------------------------------------------------------------------
#define STREAMING_BUFFER_SIZE 65536
#define MAX_BUFFER_COUNT 3

// Use 4k streaming alignment to support Advanced Format (4Kn) drives. See the xwbtool -af switch.
// Otherwise uses 2K streaming alignment to support DVD, HDDs, and Advanced Format (512e) drives.
#define SUPPORT_AF_4KN

#ifdef SUPPORT_AF_4KN
static_assert( (STREAMING_BUFFER_SIZE % 4096) == 0,
    "Streaming size error for non-buffered async I/O for Advanced Format (4Kn)" );
#else
static_assert( (STREAMING_BUFFER_SIZE % 2048) == 0,
    "Streaming size error for non-buffered async I/O for DVD/HDD" );
#endif





// https://github.com/microsoft/DirectXTK/blob/main/Audio/AudioEngine.cpp




static class XAudio2Source : public AudioSource
{
    public:
        XAudio2Source(IXAudio2 *xaudio, int bits_per_sample, int rate, bool channels) :
            AudioSource(),
            SourceVoice(nullptr),
            WaveFormat(),
            AudioBuffer(),
            DSPSettings()
        {
            HRESULT hr;
        
            // NOTE Create the WAVEFORMAT
            ZeroMemory(&WaveFormat, sizeof(WaveFormat));
            WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
            WaveFormat.nChannels = channels;
            WaveFormat.nSamplesPerSec = rate;
            WaveFormat.wBitsPerSample = bits_per_sample;
            WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
            WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;
                    
            // NOTE Create the Souce voice
            hr = xaudio->CreateSourceVoice(&SourceVoice, &WaveFormat);
            if (FAILED(hr)) {
                return;
            }
        
            // NOTE Allocate buffer. The buffer is owned by the program and not the XAudio2
            ZeroMemory(&AudioBuffer, sizeof(AudioBuffer));
            AudioBuffer.AudioBytes = STREAMING_BUFFER_SIZE;
            AudioBuffer.pAudioData = new BYTE(STREAMING_BUFFER_SIZE);
                        
            // NOTE Pass XAUDIO2_BUFFER to the source voice
            hr = SourceVoice->SubmitSourceBuffer(&AudioBuffer);
            if (FAILED(hr)) {
                return;
            }

            ZeroMemory(&DSPSettings, sizeof(DSPSettings));
            DSPSettings.SrcChannelCount = WaveFormat.nChannels;
            //mDSPSettings.DstChannelCount = eng->GetOutputChannels();      // Hmm?
            
        }
        
        virtual ~XAudio2Source()
        {
            SourceVoice->DestroyVoice();
            SourceVoice = nullptr;

            delete Resource;
            Resource = nullptr;
        }
        
        virtual bool Play(bool loop = false, int loop_count = XAUDIO2_LOOP_INFINITE) override
        {
            ASSERT(SourceVoice != nullptr);

            HRESULT hr;

            if (!SourceVoice) {
                return false;
            }

            if (!Resource) {
                return false;
            }

            hr = SourceVoice->Stop();
            if (FAILED(hr)) {
                return false;
            }
        
            hr = SourceVoice->FlushSourceBuffers();
            if (FAILED(hr)) {
                return false;
            }
        
            AudioBuffer.PlayBegin = 0;
            AudioBuffer.AudioBytes = Resource->Get_Audio_Bytes();
            AudioBuffer.pAudioData = Resource->Get_Audio_Data();
            AudioBuffer.LoopCount = loop ? loop_count : 0;
            AudioBuffer.Flags = XAUDIO2_END_OF_STREAM;
        
            hr = SourceVoice->SubmitSourceBuffer(&AudioBuffer);
            if (FAILED(hr)) {
                return false;
            }

            hr = SourceVoice->Start();
            if (FAILED(hr)) {
                return false;
            }
        
            hr = SourceVoice->SetVolume(1.0f);
            if (FAILED(hr)) {
                return false;
            }

            return true;
        }

        virtual bool Stop(bool immediate = true) override
        {
            ASSERT(SourceVoice != nullptr);

            HRESULT hr = SourceVoice->Stop();
            if (FAILED(hr)) {
                return false;
            }
            return true;
        }

        virtual bool Pause()
        {
            if (!SourceVoice) {
                return false;
            }

            if (State != STATE_PLAYING) {
                return false;
            }

            HRESULT hr = SourceVoice->Stop();
            if (FAILED(hr)) {
                return false;
            }

            State == STATE_PAUSED;

            return true;
        }

        virtual bool Resume()
        {
            if (!SourceVoice) {
                return false;
            }

            if (State != STATE_PAUSED) {
                return false;
            }

            HRESULT hr = SourceVoice->Start();
            if (FAILED(hr)) {
                return false;
            }

            State == STATE_PLAYING;

            return true;
        }

        virtual bool Is_Playing() const override
        {
            ASSERT(SourceVoice != nullptr);

            XAUDIO2_VOICE_STATE state;
            SourceVoice->GetState(&state);
            return (state.BuffersQueued > 0) != false;
        }

        virtual bool Is_Looping() const
        {
            return State == STATE_PAUSED;
        }
        
        virtual void Set_Volume(float volume) override
        {
            ASSERT(SourceVoice != nullptr);
            ASSERT(volume >= -1.0f && volume <= 1.0f);

            SourceVoice->SetVolume(volume);
        }
        
        virtual float Get_Volume() const override
        {
            ASSERT(SourceVoice != nullptr);

            float volume;
            SourceVoice->GetVolume(&volume);
            return volume;
        }

        virtual void Set_Pitch(float pitch) override
        {
            ASSERT(SourceVoice != nullptr);
            ASSERT(pitch >= -1.0f && pitch <= 1.0f);

            Pitch = pitch;

            FreqRatio = XAudio2SemitonesToFrequencyRatio(pitch * 12.f);

            SourceVoice->SetFrequencyRatio(FreqRatio);
        }

        virtual float Get_Pitch() const override
        {
            return Pitch;
        }

        virtual void Set_Pan(float pan) override
        {
            ASSERT(SourceVoice != nullptr);
            assert(pan >= -1.0f && pan <= 1.0f);

            Pan = pan;

            float matrix[16];
            XAudio2_ComputePan(pan, DSPSettings.SrcChannelCount, matrix);
            SourceVoice->SetOutputMatrix(nullptr, DSPSettings.SrcChannelCount, DSPSettings.DstChannelCount, matrix);
        }

        virtual float Get_Pan() const override
        {
            return Pan;
        }

        virtual int Get_Pending_Buffer_Count() const
        {
            if (!SourceVoice) {
                return 0;
            }

            XAUDIO2_VOICE_STATE xstate;
            SourceVoice->GetState(&xstate, XAUDIO2_VOICE_NOSAMPLESPLAYED);
            return xstate.BuffersQueued;
        }
        
    protected:        
        /**
         *  
         */
        IXAudio2SourceVoice *SourceVoice; 

        /**
         *  
         */
        WAVEFORMATEX WaveFormat;

        /**
         *  
         */
        XAUDIO2_BUFFER AudioBuffer;
        
        /**
         *  
         */
        X3DAUDIO_DSP_SETTINGS DSPSettings;

        float Pitch;
        float FreqRatio;
        float Pan;
};


/**
 *  For samples that are "one-shots".
 */
static class XAudio2BasicAudioSource : public XAudio2Source
{
    public:
        XAudio2BasicAudioSource(IXAudio2 *xaudio, int bits_per_sample, int rate, bool channels) :
            XAudio2Source(xaudio, bits_per_sample, rate, channels),
            IsLooping(false)
        {
        }

        virtual ~XAudio2BasicAudioSource()
        {
        }

    private:
        /**
         *  
         */
        bool IsLooping;
};


/**
 *  For longer samples such as voices and music.
 */
static class XAudio2StreamingVoiceContext : public XAudio2Source
{
    public:
        XAudio2StreamingVoiceContext(IXAudio2 *xaudio, int bits_per_sample, int rate, bool channels) :
            XAudio2Source(xaudio, bits_per_sample, rate, channels)
        {
        }

        virtual ~XAudio2StreamingVoiceContext()
        {
        }

    private:
};












XAudio2AudioDriver::XAudio2AudioDriver() :
    AudioDriver("XAudio2"),
    IsInitialised(false),
    IsEnabled(false),
    SoundEffectVolume(1.0f),
    MusicVolume(1.0f),
    XAudio2(nullptr),
    MasterVoice(nullptr),
    SoundEffects(),
    MusicTrack(nullptr)
{
}

XAudio2AudioDriver::~XAudio2AudioDriver()
{
}

bool XAudio2AudioDriver::Init(HWND hWnd, int bits_per_sample, bool stereo, int rate, int num_trackers, bool reverse_channels)
{
    //
    // Initialize XAudio2
    //
    HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        wprintf(L"Failed to init COM: %#X\n", static_cast<unsigned long>(hr));
        return false;
    }

//#ifdef USING_XAUDIO2_7_DIRECTX
//    // Workaround for XAudio 2.7 known issue
//#ifdef _DEBUG
//    HMODULE mXAudioDLL = LoadLibraryExW(L"XAudioD2_7.DLL", nullptr, 0x00000800 /* LOAD_LIBRARY_SEARCH_SYSTEM32 */);
//#else
//    HMODULE mXAudioDLL = LoadLibraryExW(L"XAudio2_7.DLL", nullptr, 0x00000800 /* LOAD_LIBRARY_SEARCH_SYSTEM32 */);
//#endif
//    if (!mXAudioDLL)
//    {
//        wprintf(L"Failed to find XAudio 2.7 DLL");
//        //CoUninitialize();
//        return false;
//    }
//#endif // USING_XAUDIO2_7_DIRECTX

    UINT32 flags = 0;

//#if defined(USING_XAUDIO2_7_DIRECTX) && defined(_DEBUG)
//    flags |= XAUDIO2_DEBUG_ENGINE;
//#endif

    hr = XAudio2Create(XAudio2.GetAddressOf(), flags);
    if( FAILED( hr ) )
    {
        wprintf( L"Failed to init XAudio2 engine: %#X\n", hr );
        //CoUninitialize();
        return false;
    }

#if defined(_DEBUG)
    XAUDIO2_DEBUG_CONFIGURATION debug = {};
    debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
    debug.BreakMask = XAUDIO2_LOG_ERRORS;
    XAudio2->SetDebugConfiguration(&debug, nullptr);
#if defined(USING_XAUDIO2_9)
    //DebugTrace("XAudio 2.9 debugging enabled\n");
#else
    // To see the trace output, you need to view ETW logs for this application:
    //    Go to Control Panel, Administrative Tools, Event Viewer.
    //    View->Show Analytic and Debug Logs.
    //    Applications and Services Logs / Microsoft / Windows / XAudio2. 
    //    Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK 
    //DebugTrace("XAudio 2.8 debugging enabled\n");
#endif
#endif

    //
    // Create a mastering voice for device
    //
    hr = XAudio2->CreateMasteringVoice(&MasterVoice);
    if (FAILED(hr)) {
        wprintf( L"Failed creating mastering voice: %#X\n", hr );
        XAudio2.Reset();
        return false;
    }

    DWORD dwChannelMask;
    hr = MasterVoice->GetChannelMask(&dwChannelMask);
    if (FAILED(hr)) {
        SAFE_DESTROY_VOICE(MasterVoice);
        XAudio2.Reset();
        return false;
    }

    XAUDIO2_VOICE_DETAILS details;
    MasterVoice->GetVoiceDetails(&details);

    uint32_t masterChannelMask = dwChannelMask;
    uint32_t masterChannels = details.InputChannels;
    uint32_t masterRate = details.InputSampleRate;

    //DebugTrace("INFO: mastering voice has %u channels, %u sample rate, %08X channel mask\n",
    //    masterChannels, masterRate, masterChannelMask);

    if (MasterVolume != 1.0f) {
        hr = MasterVoice->SetVolume(MasterVolume);
        if (FAILED(hr)) {
            SAFE_DESTROY_VOICE(MasterVoice);
            XAudio2.Reset();
            return false;
        }
    }



#if 0
    //
    // Setup mastering volume limiter (optional)
    //
    //if (mEngineFlags & AudioEngine_UseMasteringLimiter)
    //{
        FXMASTERINGLIMITER_PARAMETERS params = {};
        params.Release = FXMASTERINGLIMITER_DEFAULT_RELEASE;
        params.Loudness = FXMASTERINGLIMITER_DEFAULT_LOUDNESS;

        hr = CreateFX(__uuidof(FXMasteringLimiter), mVolumeLimiter.ReleaseAndGetAddressOf(), &params, sizeof(params));
        if (FAILED(hr)) {
            SAFE_DESTROY_VOICE(MasterVolume);
            XAudio2.Reset();
            return hr;
        }

        XAUDIO2_EFFECT_DESCRIPTOR desc = {};
        desc.InitialState = TRUE;
        desc.OutputChannels = masterChannels;
        desc.pEffect = mVolumeLimiter.Get();

        XAUDIO2_EFFECT_CHAIN chain = { 1, &desc };
        hr = MasterVolume->SetEffectChain(&chain);
        if (FAILED(hr)) {
            SAFE_DESTROY_VOICE(MasterVolume);
            mVolumeLimiter.Reset();
            XAudio2.Reset();
            return hr;
        }

        //DebugTrace("INFO: Mastering volume limiter enabled\n");
    //}
#endif













    // Create sound effect voices
    for (int i = 0; i < num_trackers; ++i) {
        XAudio2BasicAudioSource *soundeffect = new XAudio2BasicAudioSource(XAudio2.Get(), bits_per_sample, rate, stereo ? 2 : 1);
        SoundEffects.Add(soundeffect);
    }

    ASSERT(SoundEffects.Count() > 0);

    // Create music streaming voice.
    MusicTrack = new XAudio2StreamingVoiceContext(XAudio2.Get(), bits_per_sample, rate, stereo ? 2 : 1);

    ASSERT(MusicTrack != nullptr);

    

}

void XAudio2AudioDriver::End()
{
    if (MasterVoice) {
        MasterVoice->DestroyVoice();
    }
    if (XAudio2) {
        XAudio2->Release();
    }
}

bool XAudio2AudioDriver::Is_Available() const
{
    return IsInitialised && IsEnabled;
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

bool XAudio2AudioDriver::Resume(bool forced)
{
    if (XAudio2) {
        HRESULT hr = XAudio2->StartEngine();
        return SUCCEEDED(hr);
    }
    return false;
}

void XAudio2AudioDriver::Suspend()
{
    if (XAudio2) {
        XAudio2->StopEngine();
    }
}

void XAudio2AudioDriver::Sound_Callback()
{



    for (int i = 0; i < SoundEffects.Count(); ++i) {

    }


    if (MusicTrack) {

        while (MusicTrack->Is_Playing()) {
        }

    }


}

AudioHandle XAudio2AudioDriver::Play_Sound_Effect(Wstring &filename, int priority, int volume)
{
    return INVALID_AUDIO_HANDLE;
}

void XAudio2AudioDriver::Stop_Sound_Effect(AudioHandle handle)
{
    for (int i = 0; i < SoundEffects.Count(); ++i) {
        AudioSource *source = SoundEffects[i];
        if (source->Get_Resource()->Get_Handle() == handle) {
            source->Stop();
            break;
        }
    }
}

bool XAudio2AudioDriver::Pause_Sound_Effect(AudioHandle handle)
{
    for (int i = 0; i < SoundEffects.Count(); ++i) {
        AudioSource *source = SoundEffects[i];
        if (source->Get_Resource()->Get_Handle() == handle) {
            return source->Stop();
        }
    }
    return false;
}

bool XAudio2AudioDriver::Resume_Sound_Effect(AudioHandle handle)
{
    for (int i = 0; i < SoundEffects.Count(); ++i) {
        AudioSource *source = SoundEffects[i];
        if (source->Get_Resource()->Get_Handle() == handle) {
            //return source->Play();
        }
    }
    return false;
}

void XAudio2AudioDriver::Fade_Sound_Effect(AudioHandle handle, int ticks)
{
}

bool XAudio2AudioDriver::Is_Sound_Effect_Playing(AudioHandle handle) const
{
    for (int i = 0; i < SoundEffects.Count(); ++i) {
        AudioSource *source = SoundEffects[i];
        if (source->Get_Resource()->Get_Handle() == handle) {
            return source->Is_Playing();
        }
    }
    return false;
}

void XAudio2AudioDriver::Set_Sound_Effect_Volume(AudioHandle handle, float volume)
{
    for (int i = 0; i < SoundEffects.Count(); ++i) {
        AudioSource *source = SoundEffects[i];
        if (source->Get_Resource()->Get_Handle() == handle) {
            source->Set_Volume(volume);
            break;
        }
    }
}

void XAudio2AudioDriver::Set_Sound_Effect_Pitch(AudioHandle handle, float pitch)
{
    for (int i = 0; i < SoundEffects.Count(); ++i) {
        AudioSource *source = SoundEffects[i];
        if (source->Get_Resource()->Get_Handle() == handle) {
            source->Set_Pitch(pitch);
            break;
        }
    }
}

void XAudio2AudioDriver::Set_Sound_Effect_Pan(AudioHandle handle, float pan)
{
    for (int i = 0; i < SoundEffects.Count(); ++i) {
        AudioSource *source = SoundEffects[i];
        if (source->Get_Resource()->Get_Handle() == handle) {
            source->Set_Pan(pan);
            break;
        }
    }
}

void XAudio2AudioDriver::Set_All_Sound_Effect_Volume(float volume)
{
    for (int i = 0; i < SoundEffects.Count(); ++i) {
        AudioSource *source = SoundEffects[i];
        source->Set_Volume(volume);
    }
}

AudioHandle XAudio2AudioDriver::Play_Music(Wstring &filename, int volume, bool real_time_start)
{
    AudioSource *source = nullptr;

    // find free slot.
    for (int i = 0; i < SoundEffects.Count(); ++i) {
        source = SoundEffects[i];
        if (!source->Is_Playing()) {
            break;
        }
    }

    if (!source) {
        return INVALID_AUDIO_HANDLE;
    }

    source->Stop();

    AudioResource *resource = new WaveAudioResource(filename);
    if (!resource) {
        return INVALID_AUDIO_HANDLE;
    }

    source->Set_Resource(resource);

    source->Play(false);








    //
    // Play a PCM wave file
    //
    wprintf( L"Playing mono WAV PCM file..." );
    if( FAILED( hr = PlayWave( XAudio2.Get(), L"Music.wav" ) ) )
    {
        wprintf( L"Failed creating source voice: %#X\n", hr );
        XAudio2.Reset();
        //CoUninitialize();
        return 0;
    }

    //
    // Play an ADPCM wave file
    //
    wprintf( L"\nPlaying mono WAV ADPCM file..." );
    if( FAILED( hr = PlayWave( XAudio2.Get(), L"MusicMono_adpcm.wav" ) ) )
    {
        wprintf( L"Failed creating source voice: %#X\n", hr );
        XAudio2.Reset();
        //CoUninitialize();
        return 0;
    }

    //
    // Play a 5.1 PCM wave extensible file
    //
    wprintf( L"\nPlaying 5.1 WAV PCM file..." );
    if( FAILED( hr = PlayWave( XAudio2.Get(), L"MusicSurround.wav" ) ) )
    {
        wprintf( L"Failed creating source voice: %#X\n", hr );
        XAudio2.Reset();
        //CoUninitialize();
        return 0;
    }

#if defined(USING_XAUDIO2_7_DIRECTX) || defined(USING_XAUDIO2_9)

    //
    // Play a mono xWMA wave file
    //
    wprintf( L"\nPlaying mono xWMA file..." );
    if( FAILED( hr = PlayWave( XAudio2.Get(), L"MusicMono_xwma.wav" ) ) )
    {
        wprintf( L"Failed creating source voice: %#X\n", hr );
        XAudio2.Reset();
        //CoUninitialize();
        return 0;
    }

    //
    // Play a 5.1 xWMA wave file
    //
    wprintf( L"\nPlaying 5.1 xWMA file..." );
    if( FAILED( hr = PlayWave( XAudio2.Get(), L"MusicSurround_xwma.wav" ) ) )
    {
        wprintf( L"Failed creating source voice: %#X\n", hr );
        XAudio2.Reset();
        //CoUninitialize();
        return 0;
    }

#endif

    wprintf( L"\nFinished playing\n" );




    return INVALID_AUDIO_HANDLE;
}

void XAudio2AudioDriver::Stop_Music(AudioHandle handle)
{
    if (MusicTrack) {
        MusicTrack->Stop();
    }
}

bool XAudio2AudioDriver::Pause_Music(AudioHandle handle)
{
    if (MusicTrack) {
        return MusicTrack->Stop();
    }
    return false;
}

bool XAudio2AudioDriver::Resume_Music(AudioHandle handle)
{
    if (MusicTrack) {
        return MusicTrack->Play(false);
    }
    return false;
}

void XAudio2AudioDriver::Fade_Music(AudioHandle handle, int ticks)
{
    // TODO
}

bool XAudio2AudioDriver::Is_Music_Playing(AudioHandle handle) const
{
    if (MusicTrack) {
        return MusicTrack->Is_Playing();
    }
    return false;
}

void XAudio2AudioDriver::Set_Music_Volume(AudioHandle handle, float volume)
{
    if (MusicTrack) {
        MusicTrack->Set_Volume(volume);
    }
}

void XAudio2AudioDriver::Set_Master_Volume(float volume)
{
    ASSERT(volume >= -XAUDIO2_MAX_VOLUME_LEVEL && volume <= XAUDIO2_MAX_VOLUME_LEVEL);

    AudioDriver::Set_Master_Volume(volume);

    if (MasterVoice) {
        volume = std::clamp(volume, 0.0f, 1.0f);
        MasterVoice->SetVolume(volume);
    }
}
