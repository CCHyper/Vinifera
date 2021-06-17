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
