#pragma once

#include <al.h>
#include <alc.h>
#include "fmod_audio.h"


/**
 *  Are the required libraries initalised and ready to use?
 */
extern bool FMODInitialised;


/**
 *  Function pointers to the FMOD exports required.
 */
extern LPALSOURCEF alSourcef;
extern LPALSOURCE3F alSource3f;
extern LPALSOURCEI alSourcei;
extern LPALSOURCEQUEUEBUFFERS alSourceQueueBuffers;
extern LPALBUFFERDATA alBufferData;
extern LPALGENBUFFERS alGenBuffers;
extern LPALDELETEBUFFERS alDeleteBuffers;
extern LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers;
extern LPALGETSOURCEI alGetSourcei;
extern LPALSOURCESTOP alSourceStop;
extern LPALSOURCEPLAY alSourcePlay;
extern LPALSOURCEPAUSE alSourcePause;
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
