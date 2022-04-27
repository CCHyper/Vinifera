/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          XAUDIO2_STREAM.CPP
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
#include "xaudio2_stream.h"
#include "xaudio2_driver.h"
#include "xaudio2_resource.h"
#include "xaudio2_globals.h"
#include "xaudio2_debug.h"
#include "vorbis_globals.h"
#include "vorbis_util.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <vorbisfile.h>


/**
 *  x
 * 
 *  @author: CCHyper
 */
XAudio2Stream::XAudio2Stream(XAudio2SoundResource *res) :
    SourceVoice(nullptr),
    FileName(),
    FileHandle(nullptr),
    SoundResource(nullptr),
    StreamingCallback(),
    IsActive(false),
    Status(STREAM_STATUS_NULL),
    Type(STREAM_NONE),
    IsFadingIn(false),
    FadeInSeconds(1),
    FadeInStep(0.10f),
    IsFadingOut(false),
    FadeOutSeconds(1),
    FadeOutStep(0.10f),
    Priority(PRIORITY_MAX),
    Format(),
    Buffers(),
    BufferIndex(0)
{
    if (res) {
        SoundResource = res;
        FileName = res->Get_Name();
        DEV_DEBUG_INFO("XAudio2Stream -  Created stream for \"%s\".\n", FileName.Peek_Buffer());

        // resource set, flag as pending.
        Status = STREAM_STATUS_PENDING;
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
XAudio2Stream::~XAudio2Stream()
{
    Reset();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2Stream::Reset()
{
    DEV_DEBUG_INFO("XAudio2Stream::Reset()\n");

    if (SourceVoice) {
        SourceVoice->DestroyVoice();
        SourceVoice = nullptr;
    }

    if (FileHandle) {
        FileHandle->Close();
        //delete FileHandle;
        //FileHandle = nullptr;
        FileHandle.release();
    }

    IsActive = false;

    IsFadingIn = false;
    FadeInSeconds = 1;
    FadeInStep = 0.10f;
    IsFadingOut = false;
    FadeOutSeconds = 1;
    FadeOutStep = 0.10f;

    Status = STREAM_STATUS_NULL;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
Wstring XAudio2Stream::Get_Name() const
{
    return SoundResource->Get_Name();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
Wstring XAudio2Stream::Get_FileName() const
{
    return SoundResource->Get_Name();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Open_Stream()
{
    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Update_Stream()
{
    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int XAudio2Stream::Queued_Buffer_Count() const
{
    if (!SourceVoice) {
        return false;
    }

    XAUDIO2_VOICE_STATE state;
    SourceVoice->GetState(&state);
    return state.BuffersQueued;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Is_Buffer_Empty() const
{
    return Queued_Buffer_Count() <= 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Start()
{
    if (!SourceVoice) {
        return false;
    }

    // Stop any existing buffers playing.
    if (Is_Playing()) {
        Stop();
    }

    // Fill buffer with initial data.
    if (!Update_Stream()) {
        return false;
    }

    // Start the voice.
    HRESULT hr = SourceVoice->Start();
    if (FAILED(hr)) {
        DEBUG_ERROR("XAudio2Stream::Start - Failed to start \"%s\"!\n", FileName.Peek_Buffer());
        return false;
    }

    IsActive = true;

    Status = STREAM_STATUS_STARTED;

#ifndef NDEBUG
    DEV_DEBUG_INFO("XAudio2Stream::Start - Started \"%s\".\n", FileName.Peek_Buffer());
#endif

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Stop(bool play_tails)
{
    if (!SourceVoice) {
        return false;
    }

    HRESULT hr;

    // stop voice
    hr = SourceVoice->Stop(play_tails ? XAUDIO2_PLAY_TAILS : 0);
    if (FAILED(hr)) {
        DEBUG_ERROR("XAudio2Stream::Stop - Failed to stop \"%s\"!\n", FileName.Peek_Buffer());
        return false;
    }

    // flush buffers
    hr = SourceVoice->FlushSourceBuffers();
    if (FAILED(hr)) {
        DEBUG_ERROR("XAudio2Stream::Stop -  Failed to flush \"%s\" buffers!\n", FileName.Peek_Buffer());
        return false;
    }

    Reset();

    Status = STREAM_STATUS_STOPPED;

#ifndef NDEBUG
    DEV_DEBUG_INFO("XAudio2Stream::Stop - Stopped \"%s\".\n", FileName.Peek_Buffer());
#endif

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Play()
{
    if (!SourceVoice) {
        return false;
    }

    if (Is_Playing()) {
        return true;
    }

    HRESULT hr;

    // flush old data out of buffer.
    hr = SourceVoice->FlushSourceBuffers();
    if (FAILED(hr)) {
        DEBUG_ERROR("XAudio2Stream::Play - Failed to flush \"%s\" buffers!\n", FileName.Peek_Buffer());
        return false;
    }

    // fill buffer with audio data.
    Update_Stream();

    // start voice from beginning of buffer.
    hr = SourceVoice->Start();
    if (FAILED(hr)) {
        DEBUG_ERROR("XAudio2Stream::Play - Failed to start \"%s\"!\n", FileName.Peek_Buffer());
        return false;
    }

    Status = STREAM_STATUS_PLAYING;

#ifndef NDEBUG
    DEV_DEBUG_INFO("XAudio2Stream::Start - Playing \"%s\".\n", FileName.Peek_Buffer());
#endif

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Pause(bool play_tails)
{
    if (!SourceVoice) {
        return false;
    }

    if (!IsActive) {
        return false;
    }

    // already paused, return success.
    if (Status == STREAM_STATUS_PAUSED) {
        return true;
    }

    // stop voice and save position in buffer.
    HRESULT hr = SourceVoice->Stop(play_tails ? XAUDIO2_PLAY_TAILS : 0);
    if (FAILED(hr)) {
        DEBUG_ERROR("XAudio2Stream::Pause - Failed to stop \"%s\"!\n", FileName.Peek_Buffer());
        return false;
    }

    Status = STREAM_STATUS_PAUSED;

#ifndef NDEBUG
    DEV_DEBUG_INFO("XAudio2Stream::Start - Paused \"%s\".\n", FileName.Peek_Buffer());
#endif

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::UnPause()
{
    if (!SourceVoice) {
        return false;
    }

    if (!IsActive) {
        return false;
    }

    // already playing, return success.
    if (Status == STREAM_STATUS_PLAYING) {
        return true;
    }

    // start voice.
    HRESULT hr = SourceVoice->Start();
    if (FAILED(hr)) {
        DEBUG_ERROR("XAudio2Stream::UnPause - Failed to stop \"%s\"!\n", FileName.Peek_Buffer());
        return false;
    }

    Status = STREAM_STATUS_PLAYING;

#ifndef NDEBUG
    DEV_DEBUG_INFO("XAudio2Stream::Start - UnPaused \"%s\".\n", FileName.Peek_Buffer());
#endif

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Is_Pending() const
{
    return Status == STREAM_STATUS_PENDING;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Is_Started() const
{
    return Status == STREAM_STATUS_STARTED;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Is_Playing() const
{
    return Status == STREAM_STATUS_PLAYING;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Is_Paused() const
{
    return Status == STREAM_STATUS_PAUSED;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Is_Stopped() const
{
    return Status == STREAM_STATUS_STOPPED;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Is_Finished() const
{
    return Status == STREAM_STATUS_FINISHED;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Is_Loop_Ended() const
{
    return Status == STREAM_STATUS_LOOP_END;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Set_Volume(float volume)
{
    //ASSERT(volume >= 0.0f && volume <= 2.0f);
        
    if (!SourceVoice) {
        return false;
    }

    HRESULT hr = SourceVoice->SetVolume(volume);
    if (FAILED(hr)) {
        return false;
    }

    Volume = volume;

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
float XAudio2Stream::Get_Volume() const
{
    if (!SourceVoice) {
        return false;
    }

    float volume = 0.0f;
    SourceVoice->GetVolume(&volume);

    return volume;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Set_Pitch(float pitch)
{
    //ASSERT(pitch >= -5.0f && pitch <= 5.0f);
        
    if (!SourceVoice) {
        return false;
    }

    HRESULT hr = SourceVoice->SetFrequencyRatio(pitch, XAUDIO2_MIN_FREQ_RATIO);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
float XAudio2Stream::Get_Pitch() const
{
    if (!SourceVoice) {
        return false;
    }

    float pitch = XAUDIO2_MIN_FREQ_RATIO;
    SourceVoice->GetFrequencyRatio(&pitch);

    return pitch;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool XAudio2Stream::Set_Pan(float pan)
{
    ASSERT(pan <= 1.0f);
    ASSERT(pan >= -1.0f);

    // Get Speaker config.
    DWORD dwChannelMask;
    XAudio2AudioDriver::Get_Master_Voice()->GetChannelMask(&dwChannelMask);
    
    // A pan of -1.0 indicates all left speaker, 
    // 1.0 is all right speaker, 0.0 is default and split between left and right.
    const float left = 0.5f - pan / 2;
    const float right = 0.5f + pan / 2;

    // Create array for matrix output.
    float outputMatrix[8] = { };
   
    switch (dwChannelMask)
    {
        default:
        case SPEAKER_MONO:
            outputMatrix[0] = 1.0;
            break;

        case SPEAKER_STEREO:
        case SPEAKER_2POINT1:
        case SPEAKER_SURROUND:
            outputMatrix[0] = left;
            outputMatrix[1] = right;
            break;

        case SPEAKER_QUAD:
            outputMatrix[0] = outputMatrix[2] = left;
            outputMatrix[1] = outputMatrix[3] = right;
            break;

        case SPEAKER_4POINT1:
            outputMatrix[0] = outputMatrix[3] = left;
            outputMatrix[1] = outputMatrix[4] = right;
            break;

        case SPEAKER_5POINT1:
        case SPEAKER_7POINT1:
        case SPEAKER_5POINT1_SURROUND:
            outputMatrix[0] = outputMatrix[4] = left;
            outputMatrix[1] = outputMatrix[5] = right;
            break;

        case SPEAKER_7POINT1_SURROUND:
            outputMatrix[0] = outputMatrix[4] = outputMatrix[6] = left;
            outputMatrix[1] = outputMatrix[5] = outputMatrix[7] = right;
            break;

    };

    XAUDIO2_VOICE_DETAILS VoiceDetails;
    SourceVoice->GetVoiceDetails(&VoiceDetails);

    XAUDIO2_VOICE_DETAILS MasterVoiceDetails;
    XAudio2AudioDriver::Get_Master_Voice()->GetVoiceDetails(&MasterVoiceDetails);

    HRESULT hr = SourceVoice->SetOutputMatrix(nullptr, VoiceDetails.InputChannels, MasterVoiceDetails.InputChannels, outputMatrix);
    if (FAILED(hr)) {
        return false;
    }

    Pan = pan;

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
float XAudio2Stream::Get_Pan() const
{
    return Pan;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2Stream::Fade_In(int seconds, float step)
{
    IsFadingIn = true;
    FadeInSeconds = seconds;
    FadeInStep = step;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void XAudio2Stream::Fade_Out(int seconds, float step)
{
    IsFadingOut = true;
    FadeOutSeconds = seconds;
    FadeOutStep = step;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
OggStream::OggStream(XAudio2SoundResource *res) :
    XAudio2Stream(res),
    OggVorbisFile(nullptr)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
OggStream::~OggStream()
{
    Reset();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OggStream::Open_Stream()
{
    if (Status != STREAM_STATUS_PENDING) {
        DEV_DEBUG_ERROR("OggStream::Open_Stream - Stream is already open!\n");
        return true;
    }

    if (!SoundResource) {
        DEV_DEBUG_ERROR("OggStream::Open_Stream - Sound resource is null!\n");
        Reset();
        return false;
    }

    HRESULT hr;
    int ov_ret;

    DEV_DEBUG_INFO("OggStream::Open_Stream - Opening streaming pipe for \"%s\".\n", FileName.Peek_Buffer());

    OggVorbisFile = new OggVorbis_File;
    if (!OggVorbisFile) {
        DEV_DEBUG_ERROR("OggStream::Open_Stream - Failed to allocate Ogg Vorbis streaming pipe for \"%s\"!\n", FileName.Peek_Buffer());
        Reset();
        return false;
    }

    DEV_DEBUG_INFO("OggStream::Open_Stream - Opening file handle for \"%s\".\n", FileName.Peek_Buffer());

    FileHandle = SoundResource->Get_Unique_File_Handle();
    if (!FileHandle) {
        DEBUG_ERROR("OggStream::Open_Stream - File handle is null!\n");
        Reset();
        return false;
    }

    FileHandle->Open(FILE_ACCESS_READ);

    // Open the Ogg-Vorbis file pipe.
    ov_ret = ov_open_callbacks(FileHandle.get(), OggVorbisFile, nullptr, -1, cc_ov_callbacks);
    //ov_ret = ov_test_callbacks(FileHandle, OggVorbisFile, nullptr, -1, cc_ov_callbacks);
    if (vorbisFailed(ov_ret)) {
        vorbisError(ov_ret);
        Reset();
        return false;
    }

    // Set the wave format.
    ZeroMemory(&Format, sizeof(WAVEFORMATEX));
    Format.nChannels       = OggVorbisFile->vi->channels;
    Format.wBitsPerSample  = 16;                    // Ogg vorbis is always 16 bit.
    Format.nSamplesPerSec  = OggVorbisFile->vi->rate;
    Format.nAvgBytesPerSec = Format.nSamplesPerSec * Format.nChannels * 2;
    Format.nBlockAlign     = 2 * Format.nChannels;
    Format.wFormatTag      = WAVE_FORMAT_PCM;
    Format.cbSize          = sizeof(WAVEFORMATEX);

    // Create the source voice.
    hr = XAudio2AudioDriver::Get_Audio_Engine()->CreateSourceVoice(&SourceVoice, &Format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &StreamingCallback);
    if (FAILED(hr)) {
        DEBUG_ERROR("OggStream::Open_Stream - Failed to create voice for \"%s\"!\n", FileName.Peek_Buffer());
        Reset();
        return false;
    }

    // start playing.
    Play();

#if 0
    // Start the voice.
    hr = SourceVoice->Start();
    if (FAILED(hr)) {
        DEBUG_ERROR("OggStream::Open_Stream - Failed to start \"%s\"!\n", FileName.Peek_Buffer());
        Reset();
        return false;
    }
#endif

    IsActive = true;

#ifndef NDEBUG
    DEV_DEBUG_INFO("OggStream::Open_Stream - Started \"%s\".\n", FileName.Peek_Buffer());
#endif

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void OggStream::Reset()
{
    XAudio2Stream::Reset();

    int ov_ret = ov_clear(OggVorbisFile);
    if (vorbisFailed(ov_ret)) {
        vorbisError(ov_ret);
    }

    delete OggVorbisFile;
    OggVorbisFile = nullptr;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool OggStream::Update_Stream()
{
    if (!SourceVoice) {
        return false;
    }

    if (!IsActive) {
        return false;
    }

    // Do we have any free Buffers?
    XAUDIO2_VOICE_STATE state;
    SourceVoice->GetState(&state);
    if (state.BuffersQueued >= MAX_BUFFER_COUNT - 1) {
#ifndef NDEBUG
        DEV_DEBUG_WARNING("OggStream::Update_Stream - Stream for \"%s\" still has queued buffers!\n", FileName.Peek_Buffer());
#endif
        return false;
    }

    HRESULT hr;
    int ov_ret;

    long total_bytes_read = 0;
    long bytes_read = 0;
    int current_section = 0;

    int temp = 1;

    ZeroMemory(&Buffers[BufferIndex], sizeof(Buffers[BufferIndex]));

    // Decode Ogg samples into the buffer.
    while (temp && bytes_read < STREAMING_BUFFER_SIZE) {
        temp = ov_read(OggVorbisFile, Buffers[BufferIndex] + bytes_read, STREAMING_BUFFER_SIZE - bytes_read, 0, 2, 1, &current_section);
        if (vorbisFailed(temp)) {
            vorbisError(temp);
            DEBUG_ERROR("OggStream::Update_Stream - Error decoding \"%s\"!\n", FileName.Peek_Buffer());
            Reset();
            return false;
        }
        bytes_read += temp;
        total_bytes_read += temp;
    }

    //ov_ret = ov_clear(OggVorbisFile);
    //if (vorbisFailed(ov_ret)) {
    //    vorbisError(ov_ret);
    //}

#ifndef NDEBUG
    if (total_bytes_read > 0) {
        DEV_DEBUG_INFO("OggStream::Update_Stream - Total of %d bytes read from \"%s\".\n", total_bytes_read, FileName.Peek_Buffer());
    }
#endif

    XAUDIO2_BUFFER buffer;
    ZeroMemory(&buffer, sizeof(XAUDIO2_BUFFER));
    buffer.pContext = this;
    buffer.pAudioData = (BYTE *)&Buffers[BufferIndex];

    // Have we reached the end?
    if (!total_bytes_read) {
        buffer.Flags = XAUDIO2_END_OF_STREAM; // Tell the source voice not to expect any data after this buffer.
        //Status = STREAM_STATUS_FINISHED;
    }

    buffer.AudioBytes = STREAMING_BUFFER_SIZE;

    if (!SourceVoice) {
        DEBUG_ERROR("OggStream::Update_Stream - BAD BUG BAD BUG!\n");
        Reset();
        return false;
    }

    // Submit the audio data to the source voice.
    hr = SourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) {
        DEBUG_ERROR("OggStream::Update_Stream - Failed to submit data to source for \"%s\"!\n", FileName.Peek_Buffer());
        Reset();
        return false;
    }
        
    BufferIndex++;
    BufferIndex %= MAX_BUFFER_COUNT;

    return true;
}
