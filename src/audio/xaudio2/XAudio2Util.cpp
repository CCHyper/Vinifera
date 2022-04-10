#include "XAudio2Util.h"


#include <ppltasks.h>   // For create_task
void DX_ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr)) {
        // Set a breakpoint on this line to catch Win32 API errors.
        //throw Platform::Exception::CreateException(hr);
    }
}



//--------------------------------------------------------------------------------------
// Helper function to try to find the location of a media file
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT FindMediaFileCch( WCHAR* strDestPath, int cchDest, LPCWSTR strFilename )
{
    bool bFound = false;

    if( !strFilename || strFilename[0] == 0 || !strDestPath || cchDest < 10 )
        return E_INVALIDARG;

    // Get the exe name, and exe path
    WCHAR strExePath[MAX_PATH] = {};
    WCHAR strExeName[MAX_PATH] = {};
    WCHAR* strLastSlash = nullptr;
    GetModuleFileNameW( nullptr, strExePath, MAX_PATH );
    strExePath[MAX_PATH - 1] = 0;
    strLastSlash = wcsrchr( strExePath, TEXT( '\\' ) );
    if( strLastSlash )
    {
        wcscpy_s( strExeName, MAX_PATH, &strLastSlash[1] );

        // Chop the exe name from the exe path
        *strLastSlash = 0;

        // Chop the .exe from the exe name
        strLastSlash = wcsrchr( strExeName, TEXT( '.' ) );
        if( strLastSlash )
            *strLastSlash = 0;
    }

    wcscpy_s( strDestPath, cchDest, strFilename );
    if( GetFileAttributesW( strDestPath ) != 0xFFFFFFFF )
        return S_OK;

    // Search all parent directories starting at .\ and using strFilename as the leaf name
    WCHAR strLeafName[MAX_PATH] = {};
    wcscpy_s( strLeafName, MAX_PATH, strFilename );

    WCHAR strFullPath[MAX_PATH] = {};
    WCHAR strFullFileName[MAX_PATH] = {};
    WCHAR strSearch[MAX_PATH] = {};
    WCHAR* strFilePart = nullptr;

    GetFullPathNameW( L".", MAX_PATH, strFullPath, &strFilePart );
    if( !strFilePart )
        return E_FAIL;

    while( strFilePart && *strFilePart != '\0' )
    {
        swprintf_s( strFullFileName, MAX_PATH, L"%s\\%s", strFullPath, strLeafName );
        if( GetFileAttributesW( strFullFileName ) != 0xFFFFFFFF )
        {
            wcscpy_s( strDestPath, cchDest, strFullFileName );
            bFound = true;
            break;
        }

        swprintf_s( strFullFileName, MAX_PATH, L"%s\\%s\\%s", strFullPath, strExeName, strLeafName );
        if( GetFileAttributesW( strFullFileName ) != 0xFFFFFFFF )
        {
            wcscpy_s( strDestPath, cchDest, strFullFileName );
            bFound = true;
            break;
        }

        swprintf_s( strSearch, MAX_PATH, L"%s\\..", strFullPath );
        GetFullPathNameW( strSearch, MAX_PATH, strFullPath, &strFilePart );
    }
    if( bFound )
        return S_OK;

    // On failure, return the file as the path but also return an error code
    wcscpy_s( strDestPath, cchDest, strFilename );

    return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );
}



//--------------------------------------------------------------------------------------
// Name: PlayWave
// Desc: Plays a wave and blocks until the wave finishes playing
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT PlayWave(IXAudio2 *pXaudio2, LPCWSTR szFilename )
{
    //
    // Locate the wave file
    //
    WCHAR strFilePath[MAX_PATH];
    HRESULT hr = FindMediaFileCch( strFilePath, MAX_PATH, szFilename );
    if( FAILED( hr ) )
    {
        wprintf( L"Failed to find media file: %s\n", szFilename );
        return hr;
    }

    //
    // Read in the wave file
    //
    std::unique_ptr<uint8_t[]> waveFile;
    DirectX::WAVData waveData;
    if ( FAILED( hr = DirectX::LoadWAVAudioFromFileEx( strFilePath, waveFile, waveData ) ) )
    {
        wprintf( L"Failed reading WAV file: %#X (%s)\n", hr, strFilePath );
        return hr;
    }

    //
    // Play the wave using a XAudio2SourceVoice
    //

    // Create the source voice
    IXAudio2SourceVoice* pSourceVoice;
    if( FAILED( hr = pXaudio2->CreateSourceVoice( &pSourceVoice, waveData.wfx ) ) )
    {
        wprintf( L"Error %#X creating source voice\n", hr );
        return hr;
    }

    // Submit the wave sample data using an XAUDIO2_BUFFER structure
    XAUDIO2_BUFFER buffer = {0};
    buffer.pAudioData = waveData.startAudio;
    buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
    buffer.AudioBytes = waveData.audioBytes;

    if ( waveData.loopLength > 0 )
    {
        buffer.LoopBegin = waveData.loopStart;
        buffer.LoopLength = waveData.loopLength;
        buffer.LoopCount = 1; // We'll just assume we play the loop twice
    }

#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/)
    if ( waveData.seek )
    {
        XAUDIO2_BUFFER_WMA xwmaBuffer = {0};
        xwmaBuffer.pDecodedPacketCumulativeBytes = waveData.seek;
        xwmaBuffer.PacketCount = waveData.seekCount;
        if( FAILED( hr = pSourceVoice->SubmitSourceBuffer( &buffer, &xwmaBuffer ) ) )
        {
            wprintf( L"Error %#X submitting source buffer (xWMA)\n", hr );
            pSourceVoice->DestroyVoice();
            return hr;
        }
    }
#else
    if ( waveData.seek )
    {
        wprintf( L"This platform does not support xWMA or XMA2\n" );
        pSourceVoice->DestroyVoice();
        return hr;
    }
#endif
    else if( FAILED( hr = pSourceVoice->SubmitSourceBuffer( &buffer ) ) )
    {
        wprintf( L"Error %#X submitting source buffer\n", hr );
        pSourceVoice->DestroyVoice();
        return hr;
    }

    hr = pSourceVoice->Start( 0 );

    // Let the sound play
    BOOL isRunning = TRUE;
    while( SUCCEEDED( hr ) && isRunning )
    {
        XAUDIO2_VOICE_STATE state;
        pSourceVoice->GetState( &state );
        isRunning = ( state.BuffersQueued > 0 ) != 0;

        // Wait till the escape key is pressed
        if( GetAsyncKeyState( VK_ESCAPE ) )
            break;

        Sleep( 10 );
    }

    // Wait till the escape key is released
    while( GetAsyncKeyState( VK_ESCAPE ) )
        Sleep( 10 );

    pSourceVoice->DestroyVoice();

    return hr;
}

_Use_decl_annotations_
bool XAudio2_ComputePan(float pan, unsigned int channels, float* matrix) noexcept
{
    memset(matrix, 0, sizeof(float) * 16);

    if (channels == 1)
    {
        // Mono panning
        float left = 1.f - pan;
        left = std::min<float>(1.f, left);
        left = std::max<float>(0.f, left);

        float right = pan + 1.f;
        right = std::min<float>(1.f, right);
        right = std::max<float>(0.f, right);

        matrix[0] = left;
        matrix[1] = right;
    }
    else if (channels == 2)
    {
        // Stereo panning
        if (-1.f <= pan && pan <= 0.f)
        {
            matrix[0] = .5f * pan + 1.f;    // .5 when pan is -1, 1 when pan is 0
            matrix[1] = .5f * -pan;         // .5 when pan is -1, 0 when pan is 0
            matrix[2] = 0.f;                //  0 when pan is -1, 0 when pan is 0
            matrix[3] = pan + 1.f;          //  0 when pan is -1, 1 when pan is 0
        }
        else
        {
            matrix[0] = -pan + 1.f;         //  1 when pan is 0,   0 when pan is 1
            matrix[1] = 0.f;                //  0 when pan is 0,   0 when pan is 1
            matrix[2] = .5f * pan;          //  0 when pan is 0, .5f when pan is 1
            matrix[3] = .5f * -pan + 1.f;   //  1 when pan is 0. .5f when pan is 1
        }
    }
    else
    {
        if (pan != 0.f)
        {
            DebugTrace("WARNING: Only supports panning on mono or stereo source data, ignored\n");
        }
        return false;
    }

    return true;
}
