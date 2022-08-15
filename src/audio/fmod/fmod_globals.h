/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FMOD_GLOBALS.H
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
#pragma once

#ifdef USE_FMOD_AUDIO

#include "always.h"
#include "fmod.h"


#if 0
/**
 *
 *  C typedefs for function pointers.
 *
 */


/* FMOD global system functions (optional). */
typedef FMOD_RESULT (F_API *LP_FMOD_Memory_Initialize)(void *poolmem, int poollen, FMOD_MEMORY_ALLOC_CALLBACK useralloc, FMOD_MEMORY_REALLOC_CALLBACK userrealloc, FMOD_MEMORY_FREE_CALLBACK userfree, FMOD_MEMORY_TYPE memtypeflags);
typedef FMOD_RESULT (F_API *LP_FMOD_Memory_GetStats)(int *currentalloced, int *maxalloced, FMOD_BOOL blocking);
typedef FMOD_RESULT (F_API *LP_FMOD_Debug_Initialize)(FMOD_DEBUG_FLAGS flags, FMOD_DEBUG_MODE mode, FMOD_DEBUG_CALLBACK callback, const char *filename);
typedef FMOD_RESULT (F_API *LP_FMOD_File_SetDiskBusy)(int busy);
typedef FMOD_RESULT (F_API *LP_FMOD_File_GetDiskBusy)(int *busy);
typedef FMOD_RESULT (F_API *LP_FMOD_Thread_SetAttributes)(FMOD_THREAD_TYPE type, FMOD_THREAD_AFFINITY affinity, FMOD_THREAD_PRIORITY priority, FMOD_THREAD_STACK_SIZE stacksize);

/* FMOD System factory functions. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_Create)(FMOD_SYSTEM **system, unsigned int headerversion); 
typedef FMOD_RESULT (F_API *LP_FMOD_System_Release)(FMOD_SYSTEM *system); 

/* System API: Setup functions. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetOutput)(FMOD_SYSTEM *system, FMOD_OUTPUTTYPE output);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetOutput)(FMOD_SYSTEM *system, FMOD_OUTPUTTYPE *output);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetNumDrivers)(FMOD_SYSTEM *system, int *numdrivers);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetDriverInfo)(FMOD_SYSTEM *system, int id, char *name, int namelen, FMOD_GUID *guid, int *systemrate, FMOD_SPEAKERMODE *speakermode, int *speakermodechannels);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetDriver)(FMOD_SYSTEM *system, int driver);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetDriver)(FMOD_SYSTEM *system, int *driver);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetSoftwareChannels)(FMOD_SYSTEM *system, int numsoftwarechannels);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetSoftwareChannels)(FMOD_SYSTEM *system, int *numsoftwarechannels);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetSoftwareFormat)(FMOD_SYSTEM *system, int samplerate, FMOD_SPEAKERMODE speakermode, int numrawspeakers);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetSoftwareFormat)(FMOD_SYSTEM *system, int *samplerate, FMOD_SPEAKERMODE *speakermode, int *numrawspeakers);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetDSPBufferSize)(FMOD_SYSTEM *system, unsigned int bufferlength, int numbuffers);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetDSPBufferSize)(FMOD_SYSTEM *system, unsigned int *bufferlength, int *numbuffers);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetFileSystem)(FMOD_SYSTEM *system, FMOD_FILE_OPEN_CALLBACK useropen, FMOD_FILE_CLOSE_CALLBACK userclose, FMOD_FILE_READ_CALLBACK userread, FMOD_FILE_SEEK_CALLBACK userseek, FMOD_FILE_ASYNCREAD_CALLBACK userasyncread, FMOD_FILE_ASYNCCANCEL_CALLBACK userasynccancel, int blockalign);
typedef FMOD_RESULT (F_API *LP_FMOD_System_AttachFileSystem)(FMOD_SYSTEM *system, FMOD_FILE_OPEN_CALLBACK useropen, FMOD_FILE_CLOSE_CALLBACK userclose, FMOD_FILE_READ_CALLBACK userread, FMOD_FILE_SEEK_CALLBACK userseek);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetAdvancedSettings)(FMOD_SYSTEM *system, FMOD_ADVANCEDSETTINGS *settings);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetAdvancedSettings)(FMOD_SYSTEM *system, FMOD_ADVANCEDSETTINGS *settings);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetCallback)(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACK callback, FMOD_SYSTEM_CALLBACK_TYPE callbackmask);

/* System API: Plug-in support. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetPluginPath)(FMOD_SYSTEM *system, const char *path);
typedef FMOD_RESULT (F_API *LP_FMOD_System_LoadPlugin)(FMOD_SYSTEM *system, const char *filename, unsigned int *handle, unsigned int priority);
typedef FMOD_RESULT (F_API *LP_FMOD_System_UnloadPlugin)(FMOD_SYSTEM *system, unsigned int handle);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetNumNestedPlugins)(FMOD_SYSTEM *system, unsigned int handle, int *count);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetNestedPlugin)(FMOD_SYSTEM *system, unsigned int handle, int index, unsigned int *nestedhandle);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetNumPlugins)(FMOD_SYSTEM *system, FMOD_PLUGINTYPE plugintype, int *numplugins);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetPluginHandle)(FMOD_SYSTEM *system, FMOD_PLUGINTYPE plugintype, int index, unsigned int *handle);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetPluginInfo)(FMOD_SYSTEM *system, unsigned int handle, FMOD_PLUGINTYPE *plugintype, char *name, int namelen, unsigned int *version);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetOutputByPlugin)(FMOD_SYSTEM *system, unsigned int handle);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetOutputByPlugin)(FMOD_SYSTEM *system, unsigned int *handle);
typedef FMOD_RESULT (F_API *LP_FMOD_System_CreateDSPByPlugin)(FMOD_SYSTEM *system, unsigned int handle, FMOD_DSP **dsp);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetDSPInfoByPlugin)(FMOD_SYSTEM *system, unsigned int handle, const FMOD_DSP_DESCRIPTION **description);
typedef FMOD_RESULT (F_API *LP_FMOD_System_RegisterCodec)(FMOD_SYSTEM *system, FMOD_CODEC_DESCRIPTION *description, unsigned int *handle, unsigned int priority);
typedef FMOD_RESULT (F_API *LP_FMOD_System_RegisterDSP)(FMOD_SYSTEM *system, const FMOD_DSP_DESCRIPTION *description, unsigned int *handle);
typedef FMOD_RESULT (F_API *LP_FMOD_System_RegisterOutput)(FMOD_SYSTEM *system, const FMOD_OUTPUT_DESCRIPTION *description, unsigned int *handle);

/* System API: Init/Close. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_Init)(FMOD_SYSTEM *system, int maxchannels, FMOD_INITFLAGS flags, void *extradriverdata);
typedef FMOD_RESULT (F_API *LP_FMOD_System_Close)(FMOD_SYSTEM *system);

/* System API: General post-init system functions. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_Update)(FMOD_SYSTEM *system);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetSpeakerPosition)(FMOD_SYSTEM *system, FMOD_SPEAKER speaker, float x, float y, FMOD_BOOL active);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetSpeakerPosition)(FMOD_SYSTEM *system, FMOD_SPEAKER speaker, float *x, float *y, FMOD_BOOL *active);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetStreamBufferSize)(FMOD_SYSTEM *system, unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetStreamBufferSize)(FMOD_SYSTEM *system, unsigned int *filebuffersize, FMOD_TIMEUNIT *filebuffersizetype);
typedef FMOD_RESULT (F_API *LP_FMOD_System_Set3DSettings)(FMOD_SYSTEM *system, float dopplerscale, float distancefactor, float rolloffscale);
typedef FMOD_RESULT (F_API *LP_FMOD_System_Get3DSettings)(FMOD_SYSTEM *system, float *dopplerscale, float *distancefactor, float *rolloffscale);
typedef FMOD_RESULT (F_API *LP_FMOD_System_Set3DNumListeners)(FMOD_SYSTEM *system, int numlisteners);
typedef FMOD_RESULT (F_API *LP_FMOD_System_Get3DNumListeners)(FMOD_SYSTEM *system, int *numlisteners);
typedef FMOD_RESULT (F_API *LP_FMOD_System_Set3DListenerAttributes)(FMOD_SYSTEM *system, int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up);
typedef FMOD_RESULT (F_API *LP_FMOD_System_Get3DListenerAttributes)(FMOD_SYSTEM *system, int listener, FMOD_VECTOR *pos, FMOD_VECTOR *vel, FMOD_VECTOR *forward, FMOD_VECTOR *up);
typedef FMOD_RESULT (F_API *LP_FMOD_System_Set3DRolloffCallback)(FMOD_SYSTEM *system, FMOD_3D_ROLLOFF_CALLBACK callback);
typedef FMOD_RESULT (F_API *LP_FMOD_System_MixerSuspend)(FMOD_SYSTEM *system);
typedef FMOD_RESULT (F_API *LP_FMOD_System_MixerResume)(FMOD_SYSTEM *system);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetDefaultMixMatrix)(FMOD_SYSTEM *system, FMOD_SPEAKERMODE sourcespeakermode, FMOD_SPEAKERMODE targetspeakermode, float *matrix, int matrixhop);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetSpeakerModeChannels)(FMOD_SYSTEM *system, FMOD_SPEAKERMODE mode, int *channels);

/* System API: System information functions. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetVersion)(FMOD_SYSTEM *system, unsigned int *version);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetOutputHandle)(FMOD_SYSTEM *system, void **handle);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetChannelsPlaying)(FMOD_SYSTEM *system, int *channels, int *realchannels);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetCPUUsage)(FMOD_SYSTEM *system, FMOD_CPU_USAGE *usage);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetFileUsage)(FMOD_SYSTEM *system, long long *sampleBytesRead, long long *streamBytesRead, long long *otherBytesRead);

/* System API: Sound/DSP/Channel/FX creation and retrieval. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_CreateSound)(FMOD_SYSTEM *system, const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, FMOD_SOUND **sound);
typedef FMOD_RESULT (F_API *LP_FMOD_System_CreateStream)(FMOD_SYSTEM *system, const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, FMOD_SOUND **sound);
typedef FMOD_RESULT (F_API *LP_FMOD_System_CreateDSP)(FMOD_SYSTEM *system, const FMOD_DSP_DESCRIPTION *description, FMOD_DSP **dsp);
typedef FMOD_RESULT (F_API *LP_FMOD_System_CreateDSPByType)(FMOD_SYSTEM *system, FMOD_DSP_TYPE type, FMOD_DSP **dsp);
typedef FMOD_RESULT (F_API *LP_FMOD_System_CreateChannelGroup)(FMOD_SYSTEM *system, const char *name, FMOD_CHANNELGROUP **channelgroup);
typedef FMOD_RESULT (F_API *LP_FMOD_System_CreateSoundGroup)(FMOD_SYSTEM *system, const char *name, FMOD_SOUNDGROUP **soundgroup);
typedef FMOD_RESULT (F_API *LP_FMOD_System_CreateReverb3D)(FMOD_SYSTEM *system, FMOD_REVERB3D **reverb);
typedef FMOD_RESULT (F_API *LP_FMOD_System_PlaySound)(FMOD_SYSTEM *system, FMOD_SOUND *sound, FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL paused, FMOD_CHANNEL **channel);
typedef FMOD_RESULT (F_API *LP_FMOD_System_PlayDSP)(FMOD_SYSTEM *system, FMOD_DSP *dsp, FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL paused, FMOD_CHANNEL **channel);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetChannel)(FMOD_SYSTEM *system, int channelid, FMOD_CHANNEL **channel);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetDSPInfoByType)(FMOD_SYSTEM *system, FMOD_DSP_TYPE type, const FMOD_DSP_DESCRIPTION **description);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetMasterChannelGroup)(FMOD_SYSTEM *system, FMOD_CHANNELGROUP **channelgroup);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetMasterSoundGroup)(FMOD_SYSTEM *system, FMOD_SOUNDGROUP **soundgroup);

/* System API: Routing to ports. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_AttachChannelGroupToPort)(FMOD_SYSTEM *system, FMOD_PORT_TYPE portType, FMOD_PORT_INDEX portIndex, FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL passThru);
typedef FMOD_RESULT (F_API *LP_FMOD_System_DetachChannelGroupFromPort)(FMOD_SYSTEM *system, FMOD_CHANNELGROUP *channelgroup);

/* System API: Reverb API. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetReverbProperties)(FMOD_SYSTEM *system, int instance, const FMOD_REVERB_PROPERTIES *prop);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetReverbProperties)(FMOD_SYSTEM *system, int instance, FMOD_REVERB_PROPERTIES *prop);

/* System API: System level DSP functionality. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_LockDSP)(FMOD_SYSTEM *system);
typedef FMOD_RESULT (F_API *LP_FMOD_System_UnlockDSP)(FMOD_SYSTEM *system);

/* System API: Recording API. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetRecordNumDrivers)(FMOD_SYSTEM *system, int *numdrivers, int *numconnected);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetRecordDriverInfo)(FMOD_SYSTEM *system, int id, char *name, int namelen, FMOD_GUID *guid, int *systemrate, FMOD_SPEAKERMODE *speakermode, int *speakermodechannels, FMOD_DRIVER_STATE *state);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetRecordPosition)(FMOD_SYSTEM *system, int id, unsigned int *position);
typedef FMOD_RESULT (F_API *LP_FMOD_System_RecordStart)(FMOD_SYSTEM *system, int id, FMOD_SOUND *sound, FMOD_BOOL loop);
typedef FMOD_RESULT (F_API *LP_FMOD_System_RecordStop)(FMOD_SYSTEM *system, int id);
typedef FMOD_RESULT (F_API *LP_FMOD_System_IsRecording)(FMOD_SYSTEM *system, int id, FMOD_BOOL *recording);

/* System API: Geometry API. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_CreateGeometry)(FMOD_SYSTEM *system, int maxpolygons, int maxvertices, FMOD_GEOMETRY **geometry);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetGeometrySettings)(FMOD_SYSTEM *system, float maxworldsize);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetGeometrySettings)(FMOD_SYSTEM *system, float *maxworldsize);
typedef FMOD_RESULT (F_API *LP_FMOD_System_LoadGeometry)(FMOD_SYSTEM *system, const void *data, int datasize, FMOD_GEOMETRY **geometry);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetGeometryOcclusion)(FMOD_SYSTEM *system, const FMOD_VECTOR *listener, const FMOD_VECTOR *source, float *direct, float *reverb);

/* System API: Network functions. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetNetworkProxy)(FMOD_SYSTEM *system, const char *proxy);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetNetworkProxy)(FMOD_SYSTEM *system, char *proxy, int proxylen);
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetNetworkTimeout)(FMOD_SYSTEM *system, int timeout);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetNetworkTimeout)(FMOD_SYSTEM *system, int *timeout);

/* System API: Userdata set/get. */
typedef FMOD_RESULT (F_API *LP_FMOD_System_SetUserData)(FMOD_SYSTEM *system, void *userdata);
typedef FMOD_RESULT (F_API *LP_FMOD_System_GetUserData)(FMOD_SYSTEM *system, void **userdata);

/* Sound API */
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_Release)(FMOD_SOUND *sound);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetSystemObject)(FMOD_SOUND *sound, FMOD_SYSTEM **system);

/* Sound API: Standard sound manipulation functions. */
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_Lock)(FMOD_SOUND *sound, unsigned int offset, unsigned int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_Unlock)(FMOD_SOUND *sound, void *ptr1, void *ptr2, unsigned int len1, unsigned int len2);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_SetDefaults)(FMOD_SOUND *sound, float frequency, int priority);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetDefaults)(FMOD_SOUND *sound, float *frequency, int *priority);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_Set3DMinMaxDistance)(FMOD_SOUND *sound, float min, float max);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_Get3DMinMaxDistance)(FMOD_SOUND *sound, float *min, float *max);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_Set3DConeSettings)(FMOD_SOUND *sound, float insideconeangle, float outsideconeangle, float outsidevolume);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_Get3DConeSettings)(FMOD_SOUND *sound, float *insideconeangle, float *outsideconeangle, float *outsidevolume);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_Set3DCustomRolloff)(FMOD_SOUND *sound, FMOD_VECTOR *points, int numpoints);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_Get3DCustomRolloff)(FMOD_SOUND *sound, FMOD_VECTOR **points, int *numpoints);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetSubSound)(FMOD_SOUND *sound, int index, FMOD_SOUND **subsound);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetSubSoundParent)(FMOD_SOUND *sound, FMOD_SOUND **parentsound);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetName)(FMOD_SOUND *sound, char *name, int namelen);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetLength)(FMOD_SOUND *sound, unsigned int *length, FMOD_TIMEUNIT lengthtype);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetFormat)(FMOD_SOUND *sound, FMOD_SOUND_TYPE *type, FMOD_SOUND_FORMAT *format, int *channels, int *bits);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetNumSubSounds)(FMOD_SOUND *sound, int *numsubsounds);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetNumTags)(FMOD_SOUND *sound, int *numtags, int *numtagsupdated);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetTag)(FMOD_SOUND *sound, const char *name, int index, FMOD_TAG *tag);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetOpenState)(FMOD_SOUND *sound, FMOD_OPENSTATE *openstate, unsigned int *percentbuffered, FMOD_BOOL *starving, FMOD_BOOL *diskbusy);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_ReadData)(FMOD_SOUND *sound, void *buffer, unsigned int length, unsigned int *read);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_SeekData)(FMOD_SOUND *sound, unsigned int pcm);

typedef FMOD_RESULT (F_API *LP_FMOD_Sound_SetSoundGroup)(FMOD_SOUND *sound, FMOD_SOUNDGROUP *soundgroup);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetSoundGroup)(FMOD_SOUND *sound, FMOD_SOUNDGROUP **soundgroup);

/* Synchronization point API. These points can come from markers embedded in wav files, and can also generate channel callbacks. */

typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetNumSyncPoints)(FMOD_SOUND *sound, int *numsyncpoints);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetSyncPoint)(FMOD_SOUND *sound, int index, FMOD_SYNCPOINT **point);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetSyncPointInfo)(FMOD_SOUND *sound, FMOD_SYNCPOINT *point, char *name, int namelen, unsigned int *offset, FMOD_TIMEUNIT offsettype);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_AddSyncPoint)(FMOD_SOUND *sound, unsigned int offset, FMOD_TIMEUNIT offsettype, const char *name, FMOD_SYNCPOINT **point);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_DeleteSyncPoint)(FMOD_SOUND *sound, FMOD_SYNCPOINT *point);

/* Functions also in Channel class but here they are the 'default' to save having to change it in Channel all the time. */
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_SetMode)(FMOD_SOUND *sound, FMOD_MODE mode);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetMode)(FMOD_SOUND *sound, FMOD_MODE *mode);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_SetLoopCount)(FMOD_SOUND *sound, int loopcount);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetLoopCount)(FMOD_SOUND *sound, int *loopcount);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_SetLoopPoints)(FMOD_SOUND *sound, unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetLoopPoints)(FMOD_SOUND *sound, unsigned int *loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int *loopend, FMOD_TIMEUNIT loopendtype);

/* For MOD/S3M/XM/IT/MID sequenced formats only. */
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetMusicNumChannels)(FMOD_SOUND *sound, int *numchannels);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_SetMusicChannelVolume)(FMOD_SOUND *sound, int channel, float volume);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetMusicChannelVolume)(FMOD_SOUND *sound, int channel, float *volume);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_SetMusicSpeed)(FMOD_SOUND *sound, float speed);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetMusicSpeed)(FMOD_SOUND *sound, float *speed);

/* Userdata set/get. */
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_SetUserData)(FMOD_SOUND *sound, void *userdata);
typedef FMOD_RESULT (F_API *LP_FMOD_Sound_GetUserData)(FMOD_SOUND *sound, void **userdata);

/* 'Channel' API */
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetSystemObject)(FMOD_CHANNEL *channel, FMOD_SYSTEM **system);

/* Channel API: General control functionality for Channels and ChannelGroups. */
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Stop)(FMOD_CHANNEL *channel);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetPaused)(FMOD_CHANNEL *channel, FMOD_BOOL paused);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetPaused)(FMOD_CHANNEL *channel, FMOD_BOOL *paused);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetVolume)(FMOD_CHANNEL *channel, float volume);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetVolume)(FMOD_CHANNEL *channel, float *volume);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetVolumeRamp)(FMOD_CHANNEL *channel, FMOD_BOOL ramp);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetVolumeRamp)(FMOD_CHANNEL *channel, FMOD_BOOL *ramp);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetAudibility)(FMOD_CHANNEL *channel, float *audibility);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetPitch)(FMOD_CHANNEL *channel, float pitch);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetPitch)(FMOD_CHANNEL *channel, float *pitch);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetMute)(FMOD_CHANNEL *channel, FMOD_BOOL mute);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetMute)(FMOD_CHANNEL *channel, FMOD_BOOL *mute);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetReverbProperties)(FMOD_CHANNEL *channel, int instance, float wet);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetReverbProperties)(FMOD_CHANNEL *channel, int instance, float *wet);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetLowPassGain)(FMOD_CHANNEL *channel, float gain);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetLowPassGain)(FMOD_CHANNEL *channel, float *gain);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetMode)(FMOD_CHANNEL *channel, FMOD_MODE mode);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetMode)(FMOD_CHANNEL *channel, FMOD_MODE *mode);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetCallback)(FMOD_CHANNEL *channel, FMOD_CHANNELCONTROL_CALLBACK callback);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_IsPlaying)(FMOD_CHANNEL *channel, FMOD_BOOL *isplaying);

/*
     Note all 'set' functions alter a final matrix, this is why the only get function is getMixMatrix, to avoid other get functions returning incorrect/obsolete values.
*/

typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetPan)(FMOD_CHANNEL *channel, float pan);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetMixLevelsOutput)(FMOD_CHANNEL *channel, float frontleft, float frontright, float center, float lfe, float surroundleft, float surroundright, float backleft, float backright);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetMixLevelsInput)(FMOD_CHANNEL *channel, float *levels, int numlevels);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetMixMatrix)(FMOD_CHANNEL *channel, float *matrix, int outchannels, int inchannels, int inchannel_hop);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetMixMatrix)(FMOD_CHANNEL *channel, float *matrix, int *outchannels, int *inchannels, int inchannel_hop);

/* Clock based functionality. */
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetDSPClock)(FMOD_CHANNEL *channel, unsigned long long *dspclock, unsigned long long *parentclock);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetDelay)(FMOD_CHANNEL *channel, unsigned long long dspclock_start, unsigned long long dspclock_end, FMOD_BOOL stopchannels);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetDelay)(FMOD_CHANNEL *channel, unsigned long long *dspclock_start, unsigned long long *dspclock_end, FMOD_BOOL *stopchannels);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_AddFadePoint)(FMOD_CHANNEL *channel, unsigned long long dspclock, float volume);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetFadePointRamp)(FMOD_CHANNEL *channel, unsigned long long dspclock, float volume);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_RemoveFadePoints)(FMOD_CHANNEL *channel, unsigned long long dspclock_start, unsigned long long dspclock_end);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetFadePoints)(FMOD_CHANNEL *channel, unsigned int *numpoints, unsigned long long *point_dspclock, float *point_volume);

/* DSP effects. */
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetDSP)(FMOD_CHANNEL *channel, int index, FMOD_DSP **dsp);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_AddDSP)(FMOD_CHANNEL *channel, int index, FMOD_DSP *dsp);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_RemoveDSP)(FMOD_CHANNEL *channel, FMOD_DSP *dsp);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetNumDSPs)(FMOD_CHANNEL *channel, int *numdsps);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetDSPIndex)(FMOD_CHANNEL *channel, FMOD_DSP *dsp, int index);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetDSPIndex)(FMOD_CHANNEL *channel, FMOD_DSP *dsp, int *index);

/* 3D functionality. */
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Set3DAttributes)(FMOD_CHANNEL *channel, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Get3DAttributes)(FMOD_CHANNEL *channel, FMOD_VECTOR *pos, FMOD_VECTOR *vel);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Set3DMinMaxDistance)(FMOD_CHANNEL *channel, float mindistance, float maxdistance);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Get3DMinMaxDistance)(FMOD_CHANNEL *channel, float *mindistance, float *maxdistance);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Set3DConeSettings)(FMOD_CHANNEL *channel, float insideconeangle, float outsideconeangle, float outsidevolume);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Get3DConeSettings)(FMOD_CHANNEL *channel, float *insideconeangle, float *outsideconeangle, float *outsidevolume);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Set3DConeOrientation)(FMOD_CHANNEL *channel, FMOD_VECTOR *orientation);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Get3DConeOrientation)(FMOD_CHANNEL *channel, FMOD_VECTOR *orientation);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Set3DCustomRolloff)(FMOD_CHANNEL *channel, FMOD_VECTOR *points, int numpoints);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Get3DCustomRolloff)(FMOD_CHANNEL *channel, FMOD_VECTOR **points, int *numpoints);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Set3DOcclusion)(FMOD_CHANNEL *channel, float directocclusion, float reverbocclusion);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Get3DOcclusion)(FMOD_CHANNEL *channel, float *directocclusion, float *reverbocclusion);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Set3DSpread)(FMOD_CHANNEL *channel, float angle);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Get3DSpread)(FMOD_CHANNEL *channel, float *angle);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Set3DLevel)(FMOD_CHANNEL *channel, float level);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Get3DLevel)(FMOD_CHANNEL *channel, float *level);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Set3DDopplerLevel)(FMOD_CHANNEL *channel, float level);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Get3DDopplerLevel)(FMOD_CHANNEL *channel, float *level);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Set3DDistanceFilter)(FMOD_CHANNEL *channel, FMOD_BOOL custom, float customLevel, float centerFreq);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_Get3DDistanceFilter)(FMOD_CHANNEL *channel, FMOD_BOOL *custom, float *customLevel, float *centerFreq);

/* Userdata set/get. */
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetUserData)(FMOD_CHANNEL *channel, void *userdata);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetUserData)(FMOD_CHANNEL *channel, void **userdata);

/* Channel specific control functionality. */
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetFrequency)(FMOD_CHANNEL *channel, float frequency);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetFrequency)(FMOD_CHANNEL *channel, float *frequency);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetPriority)(FMOD_CHANNEL *channel, int priority);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetPriority)(FMOD_CHANNEL *channel, int *priority);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetPosition)(FMOD_CHANNEL *channel, unsigned int position, FMOD_TIMEUNIT postype);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetPosition)(FMOD_CHANNEL *channel, unsigned int *position, FMOD_TIMEUNIT postype);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetChannelGroup)(FMOD_CHANNEL *channel, FMOD_CHANNELGROUP *channelgroup);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetChannelGroup)(FMOD_CHANNEL *channel, FMOD_CHANNELGROUP **channelgroup);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetLoopCount)(FMOD_CHANNEL *channel, int loopcount);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetLoopCount)(FMOD_CHANNEL *channel, int *loopcount);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_SetLoopPoints)(FMOD_CHANNEL *channel, unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetLoopPoints)(FMOD_CHANNEL *channel, unsigned int *loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int *loopend, FMOD_TIMEUNIT loopendtype);

/* Information only functions. */
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_IsVirtual)(FMOD_CHANNEL *channel, FMOD_BOOL *isvirtual);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetCurrentSound)(FMOD_CHANNEL *channel, FMOD_SOUND **sound);
typedef FMOD_RESULT (F_API *LP_FMOD_Channel_GetIndex)(FMOD_CHANNEL *channel, int *index);

/* 'ChannelGroup' API */
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetSystemObject)(FMOD_CHANNELGROUP *channelgroup, FMOD_SYSTEM **system);

/* General control functionality for Channels and ChannelGroups. */
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Stop)(FMOD_CHANNELGROUP *channelgroup);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetPaused)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL paused);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetPaused)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL *paused);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetVolume)(FMOD_CHANNELGROUP *channelgroup, float volume);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetVolume)(FMOD_CHANNELGROUP *channelgroup, float *volume);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetVolumeRamp)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL ramp);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetVolumeRamp)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL *ramp);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetAudibility)(FMOD_CHANNELGROUP *channelgroup, float *audibility);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetPitch)(FMOD_CHANNELGROUP *channelgroup, float pitch);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetPitch)(FMOD_CHANNELGROUP *channelgroup, float *pitch);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetMute)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL mute);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetMute)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL *mute);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetReverbProperties)(FMOD_CHANNELGROUP *channelgroup, int instance, float wet);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetReverbProperties)(FMOD_CHANNELGROUP *channelgroup, int instance, float *wet);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetLowPassGain)(FMOD_CHANNELGROUP *channelgroup, float gain);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetLowPassGain)(FMOD_CHANNELGROUP *channelgroup, float *gain);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetMode)(FMOD_CHANNELGROUP *channelgroup, FMOD_MODE mode);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetMode)(FMOD_CHANNELGROUP *channelgroup, FMOD_MODE *mode);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetCallback)(FMOD_CHANNELGROUP *channelgroup, FMOD_CHANNELCONTROL_CALLBACK callback);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_IsPlaying)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL *isplaying);

/* Note all 'set' functions alter a final matrix, this is why the only get function is getMixMatrix, to avoid other get functions returning incorrect/obsolete values. */
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetPan)(FMOD_CHANNELGROUP *channelgroup, float pan);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetMixLevelsOutput)(FMOD_CHANNELGROUP *channelgroup, float frontleft, float frontright, float center, float lfe, float surroundleft, float surroundright, float backleft, float backright);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetMixLevelsInput)(FMOD_CHANNELGROUP *channelgroup, float *levels, int numlevels);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetMixMatrix)(FMOD_CHANNELGROUP *channelgroup, float *matrix, int outchannels, int inchannels, int inchannel_hop);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetMixMatrix)(FMOD_CHANNELGROUP *channelgroup, float *matrix, int *outchannels, int *inchannels, int inchannel_hop);

/* Clock based functionality. */
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetDSPClock)(FMOD_CHANNELGROUP *channelgroup, unsigned long long *dspclock, unsigned long long *parentclock);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetDelay)(FMOD_CHANNELGROUP *channelgroup, unsigned long long dspclock_start, unsigned long long dspclock_end, FMOD_BOOL stopchannels);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetDelay)(FMOD_CHANNELGROUP *channelgroup, unsigned long long *dspclock_start, unsigned long long *dspclock_end, FMOD_BOOL *stopchannels);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_AddFadePoint)(FMOD_CHANNELGROUP *channelgroup, unsigned long long dspclock, float volume);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetFadePointRamp)(FMOD_CHANNELGROUP *channelgroup, unsigned long long dspclock, float volume);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_RemoveFadePoints)(FMOD_CHANNELGROUP *channelgroup, unsigned long long dspclock_start, unsigned long long dspclock_end);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetFadePoints)(FMOD_CHANNELGROUP *channelgroup, unsigned int *numpoints, unsigned long long *point_dspclock, float *point_volume);

/* DSP effects. */
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetDSP)(FMOD_CHANNELGROUP *channelgroup, int index, FMOD_DSP **dsp);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_AddDSP)(FMOD_CHANNELGROUP *channelgroup, int index, FMOD_DSP *dsp);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_RemoveDSP)(FMOD_CHANNELGROUP *channelgroup, FMOD_DSP *dsp);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetNumDSPs)(FMOD_CHANNELGROUP *channelgroup, int *numdsps);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetDSPIndex)(FMOD_CHANNELGROUP *channelgroup, FMOD_DSP *dsp, int index);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetDSPIndex)(FMOD_CHANNELGROUP *channelgroup, FMOD_DSP *dsp, int *index);

/* 3D functionality. */
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Set3DAttributes)(FMOD_CHANNELGROUP *channelgroup, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Get3DAttributes)(FMOD_CHANNELGROUP *channelgroup, FMOD_VECTOR *pos, FMOD_VECTOR *vel);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Set3DMinMaxDistance)(FMOD_CHANNELGROUP *channelgroup, float mindistance, float maxdistance);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Get3DMinMaxDistance)(FMOD_CHANNELGROUP *channelgroup, float *mindistance, float *maxdistance);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Set3DConeSettings)(FMOD_CHANNELGROUP *channelgroup, float insideconeangle, float outsideconeangle, float outsidevolume);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Get3DConeSettings)(FMOD_CHANNELGROUP *channelgroup, float *insideconeangle, float *outsideconeangle, float *outsidevolume);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Set3DConeOrientation)(FMOD_CHANNELGROUP *channelgroup, FMOD_VECTOR *orientation);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Get3DConeOrientation)(FMOD_CHANNELGROUP *channelgroup, FMOD_VECTOR *orientation);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Set3DCustomRolloff)(FMOD_CHANNELGROUP *channelgroup, FMOD_VECTOR *points, int numpoints);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Get3DCustomRolloff)(FMOD_CHANNELGROUP *channelgroup, FMOD_VECTOR **points, int *numpoints);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Set3DOcclusion)(FMOD_CHANNELGROUP *channelgroup, float directocclusion, float reverbocclusion);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Get3DOcclusion)(FMOD_CHANNELGROUP *channelgroup, float *directocclusion, float *reverbocclusion);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Set3DSpread)(FMOD_CHANNELGROUP *channelgroup, float angle);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Get3DSpread)(FMOD_CHANNELGROUP *channelgroup, float *angle);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Set3DLevel)(FMOD_CHANNELGROUP *channelgroup, float level);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Get3DLevel)(FMOD_CHANNELGROUP *channelgroup, float *level);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Set3DDopplerLevel)(FMOD_CHANNELGROUP *channelgroup, float level);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Get3DDopplerLevel)(FMOD_CHANNELGROUP *channelgroup, float *level);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Set3DDistanceFilter)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL custom, float customLevel, float centerFreq);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Get3DDistanceFilter)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL *custom, float *customLevel, float *centerFreq);

/* Userdata set/get. */
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_SetUserData)(FMOD_CHANNELGROUP *channelgroup, void *userdata);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetUserData)(FMOD_CHANNELGROUP *channelgroup, void **userdata);

typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_Release)(FMOD_CHANNELGROUP *channelgroup);

/* Nested channel groups. */
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_AddGroup)(FMOD_CHANNELGROUP *channelgroup, FMOD_CHANNELGROUP *group, FMOD_BOOL propagatedspclock, FMOD_DSPCONNECTION **connection);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetNumGroups)(FMOD_CHANNELGROUP *channelgroup, int *numgroups);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetGroup)(FMOD_CHANNELGROUP *channelgroup, int index, FMOD_CHANNELGROUP **group);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetParentGroup)(FMOD_CHANNELGROUP *channelgroup, FMOD_CHANNELGROUP **group);

/* Information only functions. */
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetName)(FMOD_CHANNELGROUP *channelgroup, char *name, int namelen);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetNumChannels)(FMOD_CHANNELGROUP *channelgroup, int *numchannels);
typedef FMOD_RESULT (F_API *LP_FMOD_ChannelGroup_GetChannel)(FMOD_CHANNELGROUP *channelgroup, int index, FMOD_CHANNEL **channel);

/* 'SoundGroup' API */
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_Release)(FMOD_SOUNDGROUP *soundgroup);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_GetSystemObject)(FMOD_SOUNDGROUP *soundgroup, FMOD_SYSTEM **system);

/* SoundGroup control functions. */
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_SetMaxAudible)(FMOD_SOUNDGROUP *soundgroup, int maxaudible);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_GetMaxAudible)(FMOD_SOUNDGROUP *soundgroup, int *maxaudible);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_SetMaxAudibleBehavior)(FMOD_SOUNDGROUP *soundgroup, FMOD_SOUNDGROUP_BEHAVIOR behavior);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_GetMaxAudibleBehavior)(FMOD_SOUNDGROUP *soundgroup, FMOD_SOUNDGROUP_BEHAVIOR *behavior);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_SetMuteFadeSpeed)(FMOD_SOUNDGROUP *soundgroup, float speed);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_GetMuteFadeSpeed)(FMOD_SOUNDGROUP *soundgroup, float *speed);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_SetVolume)(FMOD_SOUNDGROUP *soundgroup, float volume);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_GetVolume)(FMOD_SOUNDGROUP *soundgroup, float *volume);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_Stop)(FMOD_SOUNDGROUP *soundgroup);

/* Information only functions. */
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_GetName)(FMOD_SOUNDGROUP *soundgroup, char *name, int namelen);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_GetNumSounds)(FMOD_SOUNDGROUP *soundgroup, int *numsounds);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_GetSound)(FMOD_SOUNDGROUP *soundgroup, int index, FMOD_SOUND **sound);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_GetNumPlaying)(FMOD_SOUNDGROUP *soundgroup, int *numplaying);

/* Userdata set/get. */
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_SetUserData)(FMOD_SOUNDGROUP *soundgroup, void *userdata);
typedef FMOD_RESULT (F_API *LP_FMOD_SoundGroup_GetUserData)(FMOD_SOUNDGROUP *soundgroup, void **userdata);

/* 'DSP' API */
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_Release)(FMOD_DSP *dsp);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetSystemObject)(FMOD_DSP *dsp, FMOD_SYSTEM **system);

/* Connection / disconnection / input and output enumeration. */
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_AddInput)(FMOD_DSP *dsp, FMOD_DSP *input, FMOD_DSPCONNECTION **connection, FMOD_DSPCONNECTION_TYPE type);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_DisconnectFrom)(FMOD_DSP *dsp, FMOD_DSP *target, FMOD_DSPCONNECTION *connection);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_DisconnectAll)(FMOD_DSP *dsp, FMOD_BOOL inputs, FMOD_BOOL outputs);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetNumInputs)(FMOD_DSP *dsp, int *numinputs);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetNumOutputs)(FMOD_DSP *dsp, int *numoutputs);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetInput)(FMOD_DSP *dsp, int index, FMOD_DSP **input, FMOD_DSPCONNECTION **inputconnection);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetOutput)(FMOD_DSP *dsp, int index, FMOD_DSP **output, FMOD_DSPCONNECTION **outputconnection);

/* DSP unit control. */
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_SetActive)(FMOD_DSP *dsp, FMOD_BOOL active);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetActive)(FMOD_DSP *dsp, FMOD_BOOL *active);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_SetBypass)(FMOD_DSP *dsp, FMOD_BOOL bypass);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetBypass)(FMOD_DSP *dsp, FMOD_BOOL *bypass);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_SetWetDryMix)(FMOD_DSP *dsp, float prewet, float postwet, float dry);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetWetDryMix)(FMOD_DSP *dsp, float *prewet, float *postwet, float *dry);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_SetChannelFormat)(FMOD_DSP *dsp, FMOD_CHANNELMASK channelmask, int numchannels, FMOD_SPEAKERMODE source_speakermode);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetChannelFormat)(FMOD_DSP *dsp, FMOD_CHANNELMASK *channelmask, int *numchannels, FMOD_SPEAKERMODE *source_speakermode);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetOutputChannelFormat)(FMOD_DSP *dsp, FMOD_CHANNELMASK inmask, int inchannels, FMOD_SPEAKERMODE inspeakermode, FMOD_CHANNELMASK *outmask, int *outchannels, FMOD_SPEAKERMODE *outspeakermode);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_Reset)(FMOD_DSP *dsp);

/* DSP parameter control. */
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_SetParameterFloat)(FMOD_DSP *dsp, int index, float value);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_SetParameterInt)(FMOD_DSP *dsp, int index, int value);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_SetParameterBool)(FMOD_DSP *dsp, int index, FMOD_BOOL value);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_SetParameterData)(FMOD_DSP *dsp, int index, void *data, unsigned int length);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetParameterFloat)(FMOD_DSP *dsp, int index, float *value, char *valuestr, int valuestrlen);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetParameterInt)(FMOD_DSP *dsp, int index, int *value, char *valuestr, int valuestrlen);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetParameterBool)(FMOD_DSP *dsp, int index, FMOD_BOOL *value, char *valuestr, int valuestrlen);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetParameterData)(FMOD_DSP *dsp, int index, void **data, unsigned int *length, char *valuestr, int valuestrlen);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetNumParameters)(FMOD_DSP *dsp, int *numparams);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetParameterInfo)(FMOD_DSP *dsp, int index, FMOD_DSP_PARAMETER_DESC **desc);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetDataParameterIndex)(FMOD_DSP *dsp, int datatype, int *index);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_ShowConfigDialog)(FMOD_DSP *dsp, void *hwnd, FMOD_BOOL show);

/* DSP attributes. */
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetInfo)(FMOD_DSP *dsp, char *name, unsigned int *version, int *channels, int *configwidth, int *configheight);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetType)(FMOD_DSP *dsp, FMOD_DSP_TYPE *type);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetIdle)(FMOD_DSP *dsp, FMOD_BOOL *idle);

/* Userdata set/get. */
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_SetUserData)(FMOD_DSP *dsp, void *userdata);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetUserData)(FMOD_DSP *dsp, void **userdata);

/* Metering. */
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_SetMeteringEnabled)(FMOD_DSP *dsp, FMOD_BOOL inputEnabled, FMOD_BOOL outputEnabled);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetMeteringEnabled)(FMOD_DSP *dsp, FMOD_BOOL *inputEnabled, FMOD_BOOL *outputEnabled);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetMeteringInfo)(FMOD_DSP *dsp, FMOD_DSP_METERING_INFO *inputInfo, FMOD_DSP_METERING_INFO *outputInfo);
typedef FMOD_RESULT (F_API *LP_FMOD_DSP_GetCPUUsage)(FMOD_DSP *dsp, unsigned int *exclusive, unsigned int *inclusive);

/* 'DSPConnection' API */
typedef FMOD_RESULT (F_API *LP_FMOD_DSPConnection_GetInput)(FMOD_DSPCONNECTION *dspconnection, FMOD_DSP **input);
typedef FMOD_RESULT (F_API *LP_FMOD_DSPConnection_GetOutput)(FMOD_DSPCONNECTION *dspconnection, FMOD_DSP **output);
typedef FMOD_RESULT (F_API *LP_FMOD_DSPConnection_SetMix)(FMOD_DSPCONNECTION *dspconnection, float volume);
typedef FMOD_RESULT (F_API *LP_FMOD_DSPConnection_GetMix)(FMOD_DSPCONNECTION *dspconnection, float *volume);
typedef FMOD_RESULT (F_API *LP_FMOD_DSPConnection_SetMixMatrix)(FMOD_DSPCONNECTION *dspconnection, float *matrix, int outchannels, int inchannels, int inchannel_hop);
typedef FMOD_RESULT (F_API *LP_FMOD_DSPConnection_GetMixMatrix)(FMOD_DSPCONNECTION *dspconnection, float *matrix, int *outchannels, int *inchannels, int inchannel_hop);
typedef FMOD_RESULT (F_API *LP_FMOD_DSPConnection_GetType)(FMOD_DSPCONNECTION *dspconnection, FMOD_DSPCONNECTION_TYPE *type);

/* Userdata set/get. */
typedef FMOD_RESULT (F_API *LP_FMOD_DSPConnection_SetUserData)(FMOD_DSPCONNECTION *dspconnection, void *userdata);
typedef FMOD_RESULT (F_API *LP_FMOD_DSPConnection_GetUserData)(FMOD_DSPCONNECTION *dspconnection, void **userdata);

/* 'Geometry' API */
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_Release)(FMOD_GEOMETRY *geometry);

/* Polygon manipulation. */
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_AddPolygon)(FMOD_GEOMETRY *geometry, float directocclusion, float reverbocclusion, FMOD_BOOL doublesided, int numvertices, const FMOD_VECTOR *vertices, int *polygonindex);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_GetNumPolygons)(FMOD_GEOMETRY *geometry, int *numpolygons);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_GetMaxPolygons)(FMOD_GEOMETRY *geometry, int *maxpolygons, int *maxvertices);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_GetPolygonNumVertices)(FMOD_GEOMETRY *geometry, int index, int *numvertices);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_SetPolygonVertex)(FMOD_GEOMETRY *geometry, int index, int vertexindex, const FMOD_VECTOR *vertex);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_GetPolygonVertex)(FMOD_GEOMETRY *geometry, int index, int vertexindex, FMOD_VECTOR *vertex);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_SetPolygonAttributes)(FMOD_GEOMETRY *geometry, int index, float directocclusion, float reverbocclusion, FMOD_BOOL doublesided);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_GetPolygonAttributes)(FMOD_GEOMETRY *geometry, int index, float *directocclusion, float *reverbocclusion, FMOD_BOOL *doublesided);

/* Object manipulation. */
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_SetActive)(FMOD_GEOMETRY *geometry, FMOD_BOOL active);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_GetActive)(FMOD_GEOMETRY *geometry, FMOD_BOOL *active);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_SetRotation)(FMOD_GEOMETRY *geometry, const FMOD_VECTOR *forward, const FMOD_VECTOR *up);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_GetRotation)(FMOD_GEOMETRY *geometry, FMOD_VECTOR *forward, FMOD_VECTOR *up);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_SetPosition)(FMOD_GEOMETRY *geometry, const FMOD_VECTOR *position);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_GetPosition)(FMOD_GEOMETRY *geometry, FMOD_VECTOR *position);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_SetScale)(FMOD_GEOMETRY *geometry, const FMOD_VECTOR *scale);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_GetScale)(FMOD_GEOMETRY *geometry, FMOD_VECTOR *scale);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_Save)(FMOD_GEOMETRY *geometry, void *data, int *datasize);

/* Userdata set/get. */
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_SetUserData)(FMOD_GEOMETRY *geometry, void *userdata);
typedef FMOD_RESULT (F_API *LP_FMOD_Geometry_GetUserData)(FMOD_GEOMETRY *geometry, void **userdata);

/* 'Reverb3D' API */
typedef FMOD_RESULT (F_API *LP_FMOD_Reverb3D_Release)(FMOD_REVERB3D *reverb3d);

/* Reverb manipulation. */
typedef FMOD_RESULT (F_API *LP_FMOD_Reverb3D_Set3DAttributes)(FMOD_REVERB3D *reverb3d, const FMOD_VECTOR *position, float mindistance, float maxdistance);
typedef FMOD_RESULT (F_API *LP_FMOD_Reverb3D_Get3DAttributes)(FMOD_REVERB3D *reverb3d, FMOD_VECTOR *position, float *mindistance, float *maxdistance);
typedef FMOD_RESULT (F_API *LP_FMOD_Reverb3D_SetProperties)(FMOD_REVERB3D *reverb3d, const FMOD_REVERB_PROPERTIES *properties);
typedef FMOD_RESULT (F_API *LP_FMOD_Reverb3D_GetProperties)(FMOD_REVERB3D *reverb3d, FMOD_REVERB_PROPERTIES *properties);
typedef FMOD_RESULT (F_API *LP_FMOD_Reverb3D_SetActive)(FMOD_REVERB3D *reverb3d, FMOD_BOOL active);
typedef FMOD_RESULT (F_API *LP_FMOD_Reverb3D_GetActive)(FMOD_REVERB3D *reverb3d, FMOD_BOOL *active);

/* Userdata set/get. */
typedef FMOD_RESULT (F_API *LP_FMOD_Reverb3D_SetUserData)(FMOD_REVERB3D *reverb3d, void *userdata);
typedef FMOD_RESULT (F_API *LP_FMOD_Reverb3D_GetUserData)(FMOD_REVERB3D *reverb3d, void **userdata);


/**
 *
 *  Function pointers to the FMOD exports required.
 *
 */
#define DEC_FUNC_PTR(name) extern LP_# name;
#define DEF_FUNC_PTR(name) LP_# name = nullptr;

DEC_FUNC_PTR(FMOD_Memory_Initialize);
DEC_FUNC_PTR(FMOD_Memory_GetStats);
DEC_FUNC_PTR(FMOD_Debug_Initialize);
DEC_FUNC_PTR(FMOD_File_SetDiskBusy);
DEC_FUNC_PTR(FMOD_File_GetDiskBusy);
DEC_FUNC_PTR(FMOD_Thread_SetAttributes);
DEC_FUNC_PTR(FMOD_System_Create);
DEC_FUNC_PTR(FMOD_System_Release);
DEC_FUNC_PTR(FMOD_System_SetOutput);
DEC_FUNC_PTR(FMOD_System_GetOutput);
DEC_FUNC_PTR(FMOD_System_GetNumDrivers);
DEC_FUNC_PTR(FMOD_System_GetDriverInfo);
DEC_FUNC_PTR(FMOD_System_SetDriver);
DEC_FUNC_PTR(FMOD_System_GetDriver);
DEC_FUNC_PTR(FMOD_System_SetSoftwareChannels);
DEC_FUNC_PTR(FMOD_System_GetSoftwareChannels);
DEC_FUNC_PTR(FMOD_System_SetSoftwareFormat);
DEC_FUNC_PTR(FMOD_System_GetSoftwareFormat);
DEC_FUNC_PTR(FMOD_System_SetDSPBufferSize);
DEC_FUNC_PTR(FMOD_System_GetDSPBufferSize);
DEC_FUNC_PTR(FMOD_System_SetFileSystem);
DEC_FUNC_PTR(FMOD_System_AttachFileSystem);
DEC_FUNC_PTR(FMOD_System_SetAdvancedSettings);
DEC_FUNC_PTR(FMOD_System_GetAdvancedSettings);
DEC_FUNC_PTR(FMOD_System_SetCallback);
DEC_FUNC_PTR(FMOD_System_SetPluginPath);
DEC_FUNC_PTR(FMOD_System_LoadPlugin);
DEC_FUNC_PTR(FMOD_System_UnloadPlugin);
DEC_FUNC_PTR(FMOD_System_GetNumNestedPlugins);
DEC_FUNC_PTR(FMOD_System_GetNestedPlugin);
DEC_FUNC_PTR(FMOD_System_GetNumPlugins);
DEC_FUNC_PTR(FMOD_System_GetPluginHandle);
DEC_FUNC_PTR(FMOD_System_GetPluginInfo);
DEC_FUNC_PTR(FMOD_System_SetOutputByPlugin);
DEC_FUNC_PTR(FMOD_System_GetOutputByPlugin);
DEC_FUNC_PTR(FMOD_System_CreateDSPByPlugin);
DEC_FUNC_PTR(FMOD_System_GetDSPInfoByPlugin);
DEC_FUNC_PTR(FMOD_System_RegisterCodec);
DEC_FUNC_PTR(FMOD_System_RegisterDSP);
DEC_FUNC_PTR(FMOD_System_RegisterOutput);
DEC_FUNC_PTR(FMOD_System_Init);
DEC_FUNC_PTR(FMOD_System_Close);
DEC_FUNC_PTR(FMOD_System_Update);
DEC_FUNC_PTR(FMOD_System_SetSpeakerPosition);
DEC_FUNC_PTR(FMOD_System_GetSpeakerPosition);
DEC_FUNC_PTR(FMOD_System_SetStreamBufferSize);
DEC_FUNC_PTR(FMOD_System_GetStreamBufferSize);
DEC_FUNC_PTR(FMOD_System_Set3DSettings);
DEC_FUNC_PTR(FMOD_System_Get3DSettings);
DEC_FUNC_PTR(FMOD_System_Set3DNumListeners);
DEC_FUNC_PTR(FMOD_System_Get3DNumListeners);
DEC_FUNC_PTR(FMOD_System_Set3DListenerAttributes);
DEC_FUNC_PTR(FMOD_System_Get3DListenerAttributes);
DEC_FUNC_PTR(FMOD_System_Set3DRolloffCallback);
DEC_FUNC_PTR(FMOD_System_MixerSuspend);
DEC_FUNC_PTR(FMOD_System_MixerResume);
DEC_FUNC_PTR(FMOD_System_GetDefaultMixMatrix);
DEC_FUNC_PTR(FMOD_System_GetSpeakerModeChannels);
DEC_FUNC_PTR(FMOD_System_GetVersion);
DEC_FUNC_PTR(FMOD_System_GetOutputHandle);
DEC_FUNC_PTR(FMOD_System_GetChannelsPlaying);
DEC_FUNC_PTR(FMOD_System_GetCPUUsage);
DEC_FUNC_PTR(FMOD_System_GetFileUsage);
DEC_FUNC_PTR(FMOD_System_CreateSound);
DEC_FUNC_PTR(FMOD_System_CreateStream);
DEC_FUNC_PTR(FMOD_System_CreateDSP);
DEC_FUNC_PTR(FMOD_System_CreateDSPByType);
DEC_FUNC_PTR(FMOD_System_CreateChannelGroup);
DEC_FUNC_PTR(FMOD_System_CreateSoundGroup);
DEC_FUNC_PTR(FMOD_System_CreateReverb3D);
DEC_FUNC_PTR(FMOD_System_PlaySound);
DEC_FUNC_PTR(FMOD_System_PlayDSP);
DEC_FUNC_PTR(FMOD_System_GetChannel);
DEC_FUNC_PTR(FMOD_System_GetDSPInfoByType);
DEC_FUNC_PTR(FMOD_System_GetMasterChannelGroup);
DEC_FUNC_PTR(FMOD_System_GetMasterSoundGroup);
DEC_FUNC_PTR(FMOD_System_AttachChannelGroupToPort);
DEC_FUNC_PTR(FMOD_System_DetachChannelGroupFromPort);
DEC_FUNC_PTR(FMOD_System_SetReverbProperties);
DEC_FUNC_PTR(FMOD_System_GetReverbProperties);
DEC_FUNC_PTR(FMOD_System_LockDSP);
DEC_FUNC_PTR(FMOD_System_UnlockDSP);
DEC_FUNC_PTR(FMOD_System_GetRecordNumDrivers);
DEC_FUNC_PTR(FMOD_System_GetRecordDriverInfo);
DEC_FUNC_PTR(FMOD_System_GetRecordPosition);
DEC_FUNC_PTR(FMOD_System_RecordStart);
DEC_FUNC_PTR(FMOD_System_RecordStop);
DEC_FUNC_PTR(FMOD_System_IsRecording);
DEC_FUNC_PTR(FMOD_System_CreateGeometry);
DEC_FUNC_PTR(FMOD_System_SetGeometrySettings);
DEC_FUNC_PTR(FMOD_System_GetGeometrySettings);
DEC_FUNC_PTR(FMOD_System_LoadGeometry);
DEC_FUNC_PTR(FMOD_System_GetGeometryOcclusion);
DEC_FUNC_PTR(FMOD_System_SetNetworkProxy);
DEC_FUNC_PTR(FMOD_System_GetNetworkProxy);
DEC_FUNC_PTR(FMOD_System_SetNetworkTimeout);
DEC_FUNC_PTR(FMOD_System_GetNetworkTimeout);
DEC_FUNC_PTR(FMOD_System_SetUserData);
DEC_FUNC_PTR(FMOD_System_GetUserData);
DEC_FUNC_PTR(FMOD_Sound_Release);
DEC_FUNC_PTR(FMOD_Sound_GetSystemObject);
DEC_FUNC_PTR(FMOD_Sound_Lock);
DEC_FUNC_PTR(FMOD_Sound_Unlock);
DEC_FUNC_PTR(FMOD_Sound_SetDefaults);
DEC_FUNC_PTR(FMOD_Sound_GetDefaults);
DEC_FUNC_PTR(FMOD_Sound_Set3DMinMaxDistance);
DEC_FUNC_PTR(FMOD_Sound_Get3DMinMaxDistance);
DEC_FUNC_PTR(FMOD_Sound_Set3DConeSettings);
DEC_FUNC_PTR(FMOD_Sound_Get3DConeSettings);
DEC_FUNC_PTR(FMOD_Sound_Set3DCustomRolloff);
DEC_FUNC_PTR(FMOD_Sound_Get3DCustomRolloff);
DEC_FUNC_PTR(FMOD_Sound_GetSubSound);
DEC_FUNC_PTR(FMOD_Sound_GetSubSoundParent);
DEC_FUNC_PTR(FMOD_Sound_GetName);
DEC_FUNC_PTR(FMOD_Sound_GetLength);
DEC_FUNC_PTR(FMOD_Sound_GetFormat);
DEC_FUNC_PTR(FMOD_Sound_GetNumSubSounds);
DEC_FUNC_PTR(FMOD_Sound_GetNumTags);
DEC_FUNC_PTR(FMOD_Sound_GetTag);
DEC_FUNC_PTR(FMOD_Sound_GetOpenState);
DEC_FUNC_PTR(FMOD_Sound_ReadData);
DEC_FUNC_PTR(FMOD_Sound_SeekData);
DEC_FUNC_PTR(FMOD_Sound_SetSoundGroup);
DEC_FUNC_PTR(FMOD_Sound_GetSoundGroup);
DEC_FUNC_PTR(FMOD_Sound_GetNumSyncPoints);
DEC_FUNC_PTR(FMOD_Sound_GetSyncPoint);
DEC_FUNC_PTR(FMOD_Sound_GetSyncPointInfo);
DEC_FUNC_PTR(FMOD_Sound_AddSyncPoint);
DEC_FUNC_PTR(FMOD_Sound_DeleteSyncPoint);
DEC_FUNC_PTR(FMOD_Sound_SetMode);
DEC_FUNC_PTR(FMOD_Sound_GetMode);
DEC_FUNC_PTR(FMOD_Sound_SetLoopCount);
DEC_FUNC_PTR(FMOD_Sound_GetLoopCount);
DEC_FUNC_PTR(FMOD_Sound_SetLoopPoints);
DEC_FUNC_PTR(FMOD_Sound_GetLoopPoints);
DEC_FUNC_PTR(FMOD_Sound_GetMusicNumChannels);
DEC_FUNC_PTR(FMOD_Sound_SetMusicChannelVolume);
DEC_FUNC_PTR(FMOD_Sound_GetMusicChannelVolume);
DEC_FUNC_PTR(FMOD_Sound_SetMusicSpeed);
DEC_FUNC_PTR(FMOD_Sound_GetMusicSpeed);
DEC_FUNC_PTR(FMOD_Sound_SetUserData);
DEC_FUNC_PTR(FMOD_Sound_GetUserData);
DEC_FUNC_PTR(FMOD_Channel_GetSystemObject);
DEC_FUNC_PTR(FMOD_Channel_Stop);
DEC_FUNC_PTR(FMOD_Channel_SetPaused);
DEC_FUNC_PTR(FMOD_Channel_GetPaused);
DEC_FUNC_PTR(FMOD_Channel_SetVolume);
DEC_FUNC_PTR(FMOD_Channel_GetVolume);
DEC_FUNC_PTR(FMOD_Channel_SetVolumeRamp);
DEC_FUNC_PTR(FMOD_Channel_GetVolumeRamp);
DEC_FUNC_PTR(FMOD_Channel_GetAudibility);
DEC_FUNC_PTR(FMOD_Channel_SetPitch);
DEC_FUNC_PTR(FMOD_Channel_GetPitch);
DEC_FUNC_PTR(FMOD_Channel_SetMute);
DEC_FUNC_PTR(FMOD_Channel_GetMute);
DEC_FUNC_PTR(FMOD_Channel_SetReverbProperties);
DEC_FUNC_PTR(FMOD_Channel_GetReverbProperties);
DEC_FUNC_PTR(FMOD_Channel_SetLowPassGain);
DEC_FUNC_PTR(FMOD_Channel_GetLowPassGain);
DEC_FUNC_PTR(FMOD_Channel_SetMode);
DEC_FUNC_PTR(FMOD_Channel_GetMode);
DEC_FUNC_PTR(FMOD_Channel_SetCallback);
DEC_FUNC_PTR(FMOD_Channel_IsPlaying);
DEC_FUNC_PTR(FMOD_Channel_SetPan);
DEC_FUNC_PTR(FMOD_Channel_SetMixLevelsOutput);
DEC_FUNC_PTR(FMOD_Channel_SetMixLevelsInput);
DEC_FUNC_PTR(FMOD_Channel_SetMixMatrix);
DEC_FUNC_PTR(FMOD_Channel_GetMixMatrix);
DEC_FUNC_PTR(FMOD_Channel_GetDSPClock);
DEC_FUNC_PTR(FMOD_Channel_SetDelay);
DEC_FUNC_PTR(FMOD_Channel_GetDelay);
DEC_FUNC_PTR(FMOD_Channel_AddFadePoint);
DEC_FUNC_PTR(FMOD_Channel_SetFadePointRamp);
DEC_FUNC_PTR(FMOD_Channel_RemoveFadePoints);
DEC_FUNC_PTR(FMOD_Channel_GetFadePoints);
DEC_FUNC_PTR(FMOD_Channel_GetDSP);
DEC_FUNC_PTR(FMOD_Channel_AddDSP);
DEC_FUNC_PTR(FMOD_Channel_RemoveDSP);
DEC_FUNC_PTR(FMOD_Channel_GetNumDSPs);
DEC_FUNC_PTR(FMOD_Channel_SetDSPIndex);
DEC_FUNC_PTR(FMOD_Channel_GetDSPIndex);
DEC_FUNC_PTR(FMOD_Channel_Set3DAttributes);
DEC_FUNC_PTR(FMOD_Channel_Get3DAttributes);
DEC_FUNC_PTR(FMOD_Channel_Set3DMinMaxDistance);
DEC_FUNC_PTR(FMOD_Channel_Get3DMinMaxDistance);
DEC_FUNC_PTR(FMOD_Channel_Set3DConeSettings);
DEC_FUNC_PTR(FMOD_Channel_Get3DConeSettings);
DEC_FUNC_PTR(FMOD_Channel_Set3DConeOrientation);
DEC_FUNC_PTR(FMOD_Channel_Get3DConeOrientation);
DEC_FUNC_PTR(FMOD_Channel_Set3DCustomRolloff);
DEC_FUNC_PTR(FMOD_Channel_Get3DCustomRolloff);
DEC_FUNC_PTR(FMOD_Channel_Set3DOcclusion);
DEC_FUNC_PTR(FMOD_Channel_Get3DOcclusion);
DEC_FUNC_PTR(FMOD_Channel_Set3DSpread);
DEC_FUNC_PTR(FMOD_Channel_Get3DSpread);
DEC_FUNC_PTR(FMOD_Channel_Set3DLevel);
DEC_FUNC_PTR(FMOD_Channel_Get3DLevel);
DEC_FUNC_PTR(FMOD_Channel_Set3DDopplerLevel);
DEC_FUNC_PTR(FMOD_Channel_Get3DDopplerLevel);
DEC_FUNC_PTR(FMOD_Channel_Set3DDistanceFilter);
DEC_FUNC_PTR(FMOD_Channel_Get3DDistanceFilter);
DEC_FUNC_PTR(FMOD_Channel_SetUserData);
DEC_FUNC_PTR(FMOD_Channel_GetUserData);
DEC_FUNC_PTR(FMOD_Channel_SetFrequency);
DEC_FUNC_PTR(FMOD_Channel_GetFrequency);
DEC_FUNC_PTR(FMOD_Channel_SetPriority);
DEC_FUNC_PTR(FMOD_Channel_GetPriority);
DEC_FUNC_PTR(FMOD_Channel_SetPosition);
DEC_FUNC_PTR(FMOD_Channel_GetPosition);
DEC_FUNC_PTR(FMOD_Channel_SetChannelGroup);
DEC_FUNC_PTR(FMOD_Channel_GetChannelGroup);
DEC_FUNC_PTR(FMOD_Channel_SetLoopCount);
DEC_FUNC_PTR(FMOD_Channel_GetLoopCount);
DEC_FUNC_PTR(FMOD_Channel_SetLoopPoints);
DEC_FUNC_PTR(FMOD_Channel_GetLoopPoints);
DEC_FUNC_PTR(FMOD_Channel_IsVirtual);
DEC_FUNC_PTR(FMOD_Channel_GetCurrentSound);
DEC_FUNC_PTR(FMOD_Channel_GetIndex);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetSystemObject);
DEC_FUNC_PTR(FMOD_ChannelGroup_Stop);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetPaused);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetPaused);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetVolume);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetVolume);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetVolumeRamp);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetVolumeRamp);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetAudibility);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetPitch);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetPitch);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetMute);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetMute);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetReverbProperties);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetReverbProperties);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetLowPassGain);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetLowPassGain);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetMode);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetMode);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetCallback);
DEC_FUNC_PTR(FMOD_ChannelGroup_IsPlaying);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetPan);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetMixLevelsOutput);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetMixLevelsInput);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetMixMatrix);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetMixMatrix);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetDSPClock);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetDelay);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetDelay);
DEC_FUNC_PTR(FMOD_ChannelGroup_AddFadePoint);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetFadePointRamp);
DEC_FUNC_PTR(FMOD_ChannelGroup_RemoveFadePoints);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetFadePoints);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetDSP);
DEC_FUNC_PTR(FMOD_ChannelGroup_AddDSP);
DEC_FUNC_PTR(FMOD_ChannelGroup_RemoveDSP);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetNumDSPs);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetDSPIndex)(FMOD_CHANNELGROUP *channelgroup, FMOD_DSP *dsp, int index);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetDSPIndex)(FMOD_CHANNELGROUP *channelgroup, FMOD_DSP *dsp, int *index);
DEC_FUNC_PTR(FMOD_ChannelGroup_Set3DAttributes)(FMOD_CHANNELGROUP *channelgroup, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel);
DEC_FUNC_PTR(FMOD_ChannelGroup_Get3DAttributes)(FMOD_CHANNELGROUP *channelgroup, FMOD_VECTOR *pos, FMOD_VECTOR *vel);
DEC_FUNC_PTR(FMOD_ChannelGroup_Set3DMinMaxDistance)(FMOD_CHANNELGROUP *channelgroup, float mindistance, float maxdistance);
DEC_FUNC_PTR(FMOD_ChannelGroup_Get3DMinMaxDistance)(FMOD_CHANNELGROUP *channelgroup, float *mindistance, float *maxdistance);
DEC_FUNC_PTR(FMOD_ChannelGroup_Set3DConeSettings)(FMOD_CHANNELGROUP *channelgroup, float insideconeangle, float outsideconeangle, float outsidevolume);
DEC_FUNC_PTR(FMOD_ChannelGroup_Get3DConeSettings)(FMOD_CHANNELGROUP *channelgroup, float *insideconeangle, float *outsideconeangle, float *outsidevolume);
DEC_FUNC_PTR(FMOD_ChannelGroup_Set3DConeOrientation)(FMOD_CHANNELGROUP *channelgroup, FMOD_VECTOR *orientation);
DEC_FUNC_PTR(FMOD_ChannelGroup_Get3DConeOrientation)(FMOD_CHANNELGROUP *channelgroup, FMOD_VECTOR *orientation);
DEC_FUNC_PTR(FMOD_ChannelGroup_Set3DCustomRolloff)(FMOD_CHANNELGROUP *channelgroup, FMOD_VECTOR *points, int numpoints);
DEC_FUNC_PTR(FMOD_ChannelGroup_Get3DCustomRolloff)(FMOD_CHANNELGROUP *channelgroup, FMOD_VECTOR **points, int *numpoints);
DEC_FUNC_PTR(FMOD_ChannelGroup_Set3DOcclusion)(FMOD_CHANNELGROUP *channelgroup, float directocclusion, float reverbocclusion);
DEC_FUNC_PTR(FMOD_ChannelGroup_Get3DOcclusion)(FMOD_CHANNELGROUP *channelgroup, float *directocclusion, float *reverbocclusion);
DEC_FUNC_PTR(FMOD_ChannelGroup_Set3DSpread)(FMOD_CHANNELGROUP *channelgroup, float angle);
DEC_FUNC_PTR(FMOD_ChannelGroup_Get3DSpread)(FMOD_CHANNELGROUP *channelgroup, float *angle);
DEC_FUNC_PTR(FMOD_ChannelGroup_Set3DLevel)(FMOD_CHANNELGROUP *channelgroup, float level);
DEC_FUNC_PTR(FMOD_ChannelGroup_Get3DLevel)(FMOD_CHANNELGROUP *channelgroup, float *level);
DEC_FUNC_PTR(FMOD_ChannelGroup_Set3DDopplerLevel)(FMOD_CHANNELGROUP *channelgroup, float level);
DEC_FUNC_PTR(FMOD_ChannelGroup_Get3DDopplerLevel)(FMOD_CHANNELGROUP *channelgroup, float *level);
DEC_FUNC_PTR(FMOD_ChannelGroup_Set3DDistanceFilter)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL custom, float customLevel, float centerFreq);
DEC_FUNC_PTR(FMOD_ChannelGroup_Get3DDistanceFilter)(FMOD_CHANNELGROUP *channelgroup, FMOD_BOOL *custom, float *customLevel, float *centerFreq);
DEC_FUNC_PTR(FMOD_ChannelGroup_SetUserData)(FMOD_CHANNELGROUP *channelgroup, void *userdata);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetUserData)(FMOD_CHANNELGROUP *channelgroup, void **userdata);
DEC_FUNC_PTR(FMOD_ChannelGroup_Release)(FMOD_CHANNELGROUP *channelgroup);
DEC_FUNC_PTR(FMOD_ChannelGroup_AddGroup)(FMOD_CHANNELGROUP *channelgroup, FMOD_CHANNELGROUP *group, FMOD_BOOL propagatedspclock, FMOD_DSPCONNECTION **connection);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetNumGroups)(FMOD_CHANNELGROUP *channelgroup, int *numgroups);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetGroup)(FMOD_CHANNELGROUP *channelgroup, int index, FMOD_CHANNELGROUP **group);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetParentGroup)(FMOD_CHANNELGROUP *channelgroup, FMOD_CHANNELGROUP **group);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetName)(FMOD_CHANNELGROUP *channelgroup, char *name, int namelen);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetNumChannels)(FMOD_CHANNELGROUP *channelgroup, int *numchannels);
DEC_FUNC_PTR(FMOD_ChannelGroup_GetChannel)(FMOD_CHANNELGROUP *channelgroup, int index, FMOD_CHANNEL **channel);
DEC_FUNC_PTR(FMOD_SoundGroup_Release)(FMOD_SOUNDGROUP *soundgroup);
DEC_FUNC_PTR(FMOD_SoundGroup_GetSystemObject)(FMOD_SOUNDGROUP *soundgroup, FMOD_SYSTEM **system);
DEC_FUNC_PTR(FMOD_SoundGroup_SetMaxAudible)(FMOD_SOUNDGROUP *soundgroup, int maxaudible);
DEC_FUNC_PTR(FMOD_SoundGroup_GetMaxAudible)(FMOD_SOUNDGROUP *soundgroup, int *maxaudible);
DEC_FUNC_PTR(FMOD_SoundGroup_SetMaxAudibleBehavior)(FMOD_SOUNDGROUP *soundgroup, FMOD_SOUNDGROUP_BEHAVIOR behavior);
DEC_FUNC_PTR(FMOD_SoundGroup_GetMaxAudibleBehavior)(FMOD_SOUNDGROUP *soundgroup, FMOD_SOUNDGROUP_BEHAVIOR *behavior);
DEC_FUNC_PTR(FMOD_SoundGroup_SetMuteFadeSpeed)(FMOD_SOUNDGROUP *soundgroup, float speed);
DEC_FUNC_PTR(FMOD_SoundGroup_GetMuteFadeSpeed)(FMOD_SOUNDGROUP *soundgroup, float *speed);
DEC_FUNC_PTR(FMOD_SoundGroup_SetVolume)(FMOD_SOUNDGROUP *soundgroup, float volume);
DEC_FUNC_PTR(FMOD_SoundGroup_GetVolume)(FMOD_SOUNDGROUP *soundgroup, float *volume);
DEC_FUNC_PTR(FMOD_SoundGroup_Stop)(FMOD_SOUNDGROUP *soundgroup);
DEC_FUNC_PTR(FMOD_SoundGroup_GetName)(FMOD_SOUNDGROUP *soundgroup, char *name, int namelen);
DEC_FUNC_PTR(FMOD_SoundGroup_GetNumSounds)(FMOD_SOUNDGROUP *soundgroup, int *numsounds);
DEC_FUNC_PTR(FMOD_SoundGroup_GetSound)(FMOD_SOUNDGROUP *soundgroup, int index, FMOD_SOUND **sound);
DEC_FUNC_PTR(FMOD_SoundGroup_GetNumPlaying)(FMOD_SOUNDGROUP *soundgroup, int *numplaying);
DEC_FUNC_PTR(FMOD_SoundGroup_SetUserData)(FMOD_SOUNDGROUP *soundgroup, void *userdata);
DEC_FUNC_PTR(FMOD_SoundGroup_GetUserData)(FMOD_SOUNDGROUP *soundgroup, void **userdata);
DEC_FUNC_PTR(FMOD_DSP_Release)(FMOD_DSP *dsp);
DEC_FUNC_PTR(FMOD_DSP_GetSystemObject)(FMOD_DSP *dsp, FMOD_SYSTEM **system);
DEC_FUNC_PTR(FMOD_DSP_AddInput)(FMOD_DSP *dsp, FMOD_DSP *input, FMOD_DSPCONNECTION **connection, FMOD_DSPCONNECTION_TYPE type);
DEC_FUNC_PTR(FMOD_DSP_DisconnectFrom)(FMOD_DSP *dsp, FMOD_DSP *target, FMOD_DSPCONNECTION *connection);
DEC_FUNC_PTR(FMOD_DSP_DisconnectAll)(FMOD_DSP *dsp, FMOD_BOOL inputs, FMOD_BOOL outputs);
DEC_FUNC_PTR(FMOD_DSP_GetNumInputs)(FMOD_DSP *dsp, int *numinputs);
DEC_FUNC_PTR(FMOD_DSP_GetNumOutputs)(FMOD_DSP *dsp, int *numoutputs);
DEC_FUNC_PTR(FMOD_DSP_GetInput)(FMOD_DSP *dsp, int index, FMOD_DSP **input, FMOD_DSPCONNECTION **inputconnection);
DEC_FUNC_PTR(FMOD_DSP_GetOutput)(FMOD_DSP *dsp, int index, FMOD_DSP **output, FMOD_DSPCONNECTION **outputconnection);
DEC_FUNC_PTR(FMOD_DSP_SetActive)(FMOD_DSP *dsp, FMOD_BOOL active);
DEC_FUNC_PTR(FMOD_DSP_GetActive)(FMOD_DSP *dsp, FMOD_BOOL *active);
DEC_FUNC_PTR(FMOD_DSP_SetBypass)(FMOD_DSP *dsp, FMOD_BOOL bypass);
DEC_FUNC_PTR(FMOD_DSP_GetBypass)(FMOD_DSP *dsp, FMOD_BOOL *bypass);
DEC_FUNC_PTR(FMOD_DSP_SetWetDryMix)(FMOD_DSP *dsp, float prewet, float postwet, float dry);
DEC_FUNC_PTR(FMOD_DSP_GetWetDryMix)(FMOD_DSP *dsp, float *prewet, float *postwet, float *dry);
DEC_FUNC_PTR(FMOD_DSP_SetChannelFormat)(FMOD_DSP *dsp, FMOD_CHANNELMASK channelmask, int numchannels, FMOD_SPEAKERMODE source_speakermode);
DEC_FUNC_PTR(FMOD_DSP_GetChannelFormat)(FMOD_DSP *dsp, FMOD_CHANNELMASK *channelmask, int *numchannels, FMOD_SPEAKERMODE *source_speakermode);
DEC_FUNC_PTR(FMOD_DSP_GetOutputChannelFormat)(FMOD_DSP *dsp, FMOD_CHANNELMASK inmask, int inchannels, FMOD_SPEAKERMODE inspeakermode, FMOD_CHANNELMASK *outmask, int *outchannels, FMOD_SPEAKERMODE *outspeakermode);
DEC_FUNC_PTR(FMOD_DSP_Reset)(FMOD_DSP *dsp);
DEC_FUNC_PTR(FMOD_DSP_SetParameterFloat)(FMOD_DSP *dsp, int index, float value);
DEC_FUNC_PTR(FMOD_DSP_SetParameterInt)(FMOD_DSP *dsp, int index, int value);
DEC_FUNC_PTR(FMOD_DSP_SetParameterBool)(FMOD_DSP *dsp, int index, FMOD_BOOL value);
DEC_FUNC_PTR(FMOD_DSP_SetParameterData)(FMOD_DSP *dsp, int index, void *data, unsigned int length);
DEC_FUNC_PTR(FMOD_DSP_GetParameterFloat)(FMOD_DSP *dsp, int index, float *value, char *valuestr, int valuestrlen);
DEC_FUNC_PTR(FMOD_DSP_GetParameterInt)(FMOD_DSP *dsp, int index, int *value, char *valuestr, int valuestrlen);
DEC_FUNC_PTR(FMOD_DSP_GetParameterBool)(FMOD_DSP *dsp, int index, FMOD_BOOL *value, char *valuestr, int valuestrlen);
DEC_FUNC_PTR(FMOD_DSP_GetParameterData)(FMOD_DSP *dsp, int index, void **data, unsigned int *length, char *valuestr, int valuestrlen);
DEC_FUNC_PTR(FMOD_DSP_GetNumParameters)(FMOD_DSP *dsp, int *numparams);
DEC_FUNC_PTR(FMOD_DSP_GetParameterInfo)(FMOD_DSP *dsp, int index, FMOD_DSP_PARAMETER_DESC **desc);
DEC_FUNC_PTR(FMOD_DSP_GetDataParameterIndex)(FMOD_DSP *dsp, int datatype, int *index);
DEC_FUNC_PTR(FMOD_DSP_ShowConfigDialog)(FMOD_DSP *dsp, void *hwnd, FMOD_BOOL show);
DEC_FUNC_PTR(FMOD_DSP_GetInfo)(FMOD_DSP *dsp, char *name, unsigned int *version, int *channels, int *configwidth, int *configheight);
DEC_FUNC_PTR(FMOD_DSP_GetType)(FMOD_DSP *dsp, FMOD_DSP_TYPE *type);
DEC_FUNC_PTR(FMOD_DSP_GetIdle)(FMOD_DSP *dsp, FMOD_BOOL *idle);
DEC_FUNC_PTR(FMOD_DSP_SetUserData)(FMOD_DSP *dsp, void *userdata);
DEC_FUNC_PTR(FMOD_DSP_GetUserData)(FMOD_DSP *dsp, void **userdata);
DEC_FUNC_PTR(FMOD_DSP_SetMeteringEnabled)(FMOD_DSP *dsp, FMOD_BOOL inputEnabled, FMOD_BOOL outputEnabled);
DEC_FUNC_PTR(FMOD_DSP_GetMeteringEnabled)(FMOD_DSP *dsp, FMOD_BOOL *inputEnabled, FMOD_BOOL *outputEnabled);
DEC_FUNC_PTR(FMOD_DSP_GetMeteringInfo)(FMOD_DSP *dsp, FMOD_DSP_METERING_INFO *inputInfo, FMOD_DSP_METERING_INFO *outputInfo);
DEC_FUNC_PTR(FMOD_DSP_GetCPUUsage)(FMOD_DSP *dsp, unsigned int *exclusive, unsigned int *inclusive);
DEC_FUNC_PTR(FMOD_DSPConnection_GetInput)(FMOD_DSPCONNECTION *dspconnection, FMOD_DSP **input);
DEC_FUNC_PTR(FMOD_DSPConnection_GetOutput)(FMOD_DSPCONNECTION *dspconnection, FMOD_DSP **output);
DEC_FUNC_PTR(FMOD_DSPConnection_SetMix)(FMOD_DSPCONNECTION *dspconnection, float volume);
DEC_FUNC_PTR(FMOD_DSPConnection_GetMix)(FMOD_DSPCONNECTION *dspconnection, float *volume);
DEC_FUNC_PTR(FMOD_DSPConnection_SetMixMatrix)(FMOD_DSPCONNECTION *dspconnection, float *matrix, int outchannels, int inchannels, int inchannel_hop);
DEC_FUNC_PTR(FMOD_DSPConnection_GetMixMatrix)(FMOD_DSPCONNECTION *dspconnection, float *matrix, int *outchannels, int *inchannels, int inchannel_hop);
DEC_FUNC_PTR(FMOD_DSPConnection_GetType)(FMOD_DSPCONNECTION *dspconnection, FMOD_DSPCONNECTION_TYPE *type);
DEC_FUNC_PTR(FMOD_DSPConnection_SetUserData)(FMOD_DSPCONNECTION *dspconnection, void *userdata);
DEC_FUNC_PTR(FMOD_DSPConnection_GetUserData)(FMOD_DSPCONNECTION *dspconnection, void **userdata);
DEC_FUNC_PTR(FMOD_Geometry_Release)(FMOD_GEOMETRY *geometry);
DEC_FUNC_PTR(FMOD_Geometry_AddPolygon)(FMOD_GEOMETRY *geometry, float directocclusion, float reverbocclusion, FMOD_BOOL doublesided, int numvertices, const FMOD_VECTOR *vertices, int *polygonindex);
DEC_FUNC_PTR(FMOD_Geometry_GetNumPolygons)(FMOD_GEOMETRY *geometry, int *numpolygons);
DEC_FUNC_PTR(FMOD_Geometry_GetMaxPolygons)(FMOD_GEOMETRY *geometry, int *maxpolygons, int *maxvertices);
DEC_FUNC_PTR(FMOD_Geometry_GetPolygonNumVertices)(FMOD_GEOMETRY *geometry, int index, int *numvertices);
DEC_FUNC_PTR(FMOD_Geometry_SetPolygonVertex)(FMOD_GEOMETRY *geometry, int index, int vertexindex, const FMOD_VECTOR *vertex);
DEC_FUNC_PTR(FMOD_Geometry_GetPolygonVertex)(FMOD_GEOMETRY *geometry, int index, int vertexindex, FMOD_VECTOR *vertex);
DEC_FUNC_PTR(FMOD_Geometry_SetPolygonAttributes);
DEC_FUNC_PTR(FMOD_Geometry_GetPolygonAttributes);
DEC_FUNC_PTR(FMOD_Geometry_SetActive);
DEC_FUNC_PTR(FMOD_Geometry_GetActive);
DEC_FUNC_PTR(FMOD_Geometry_SetRotation);
DEC_FUNC_PTR(FMOD_Geometry_GetRotation);
DEC_FUNC_PTR(FMOD_Geometry_SetPosition);
DEC_FUNC_PTR(FMOD_Geometry_GetPosition);
DEC_FUNC_PTR(FMOD_Geometry_SetScale);
DEC_FUNC_PTR(FMOD_Geometry_GetScale);
DEC_FUNC_PTR(FMOD_Geometry_Save);
DEC_FUNC_PTR(FMOD_Geometry_SetUserData);
DEC_FUNC_PTR(FMOD_Geometry_GetUserData);
DEC_FUNC_PTR(FMOD_Reverb3D_Release);
DEC_FUNC_PTR(FMOD_Reverb3D_Set3DAttributes);
DEC_FUNC_PTR(FMOD_Reverb3D_Get3DAttributes);
DEC_FUNC_PTR(FMOD_Reverb3D_SetProperties);
DEC_FUNC_PTR(FMOD_Reverb3D_GetProperties);
DEC_FUNC_PTR(FMOD_Reverb3D_SetActive);
DEC_FUNC_PTR(FMOD_Reverb3D_GetActive);
DEC_FUNC_PTR(FMOD_Reverb3D_SetUserData);
DEC_FUNC_PTR(FMOD_Reverb3D_GetUserData);
#endif

#endif
