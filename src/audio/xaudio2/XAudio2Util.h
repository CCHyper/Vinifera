#pragma once

#include <always.h>
#include <Windows.h>


struct IXAudio2;


_Use_decl_annotations_
HRESULT FindMediaFileCch( WCHAR* strDestPath, int cchDest, LPCWSTR strFilename );

_Use_decl_annotations_
HRESULT PlayWave(IXAudio2 *pXaudio2, LPCWSTR szFilename );

inline void DX_ThrowIfFailed(HRESULT hr);

_Use_decl_annotations_
bool XAudio2_ComputePan(float pan, unsigned int channels, float *matrix);
