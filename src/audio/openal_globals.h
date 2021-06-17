#pragma once

#include "openal_audio.h"


/**
 *  Is OpenAL initalised and ready to use?
 */
extern bool OpenALInitialised;


/**
 *  Should we init DirectSound along side OpenAL?
 */
extern bool OpenALInitDirectSound;


/**
 *  Global instance of Open AL.
 */
extern OpenALAudioClass OpenALAudio;


/**
 *  Function pointers to the OpenAL exports required.
 */
extern LPALSOURCEF alSourcef;
extern LPALSOURCEQUEUEBUFFERS alSourceQueueBuffers;
extern LPALBUFFERDATA alBufferData;
extern LPALGENBUFFERS alGenBuffers;
extern LPALDELETEBUFFERS alDeleteBuffers;
extern LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers;
extern LPALGETSOURCEI alGetSourcei;
extern LPALSOURCESTOP alSourceStop;
extern LPALSOURCEPLAY alSourcePlay;
extern LPALDELETESOURCES alDeleteSources;
extern LPALGENSOURCES alGenSources;
extern LPALCOPENDEVICE alcOpenDevice;
extern LPALCCLOSEDEVICE alcCloseDevice;
extern LPALCCREATECONTEXT alcCreateContext;
extern LPALCDESTROYCONTEXT alcDestroyContext;
extern LPALCSUSPENDCONTEXT alcSuspendContext;
extern LPALCPROCESSCONTEXT alcProcessContext;
extern LPALCMAKECONTEXTCURRENT alcMakeContextCurrent;
extern LPALCGETCONTEXTSDEVICE alcGetContextsDevice;
extern LPALCGETERROR alcGetError;
