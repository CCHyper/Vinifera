
#include	"ahandle_re.h"
#include	"assert.h"
#include	"tibsun_globals.h"
#include	"gametime.h"
#include	"debughandler.h"

Ahandle _handles[Ahandle::MAX_HANDLES];
bool _restore_primary = false;
WAVEFORMATEX _restore_format;

AHANDLE_CALLBACK_1 _AHandleCallbackFunc1;
AHANDLE_CALLBACK_2 _AHandleCallbackFunc2;

bool restore_primary = false;
WAVEFORMATEX restore_format;

BOOL re_Move_HMI_Audio_Block_To_Direct_Sound_Buffer(VQAHandleP *vqap)
{
	Ahandle  	*audio;

	LPVOID		play_buffer_ptr1;	//Beginning of locked area of buffer
	LPVOID		play_buffer_ptr2;	//Length of locked area in buffer
	DWORD			lock_length1;		//Beginning of second locked area in buffer
	DWORD			lock_length2;		//Length of second locked area in buffer
	unsigned		next_fill_pos;
	HRESULT		return_code;

	audio = &_handles[vqap->field_34];
	VQAConfig *config = &vqap->Config;


	if (audio->SecondaryBufferPtr == NULL) {
		return 0;
	}

	/*************************************************************************
	**
	** Copy the data from the HMI play position into the direct sound buffer
	**
	*/
	next_fill_pos = audio->EndLastAudioChunk;

	/*
	** Lock the buffer to get a pointer to it
	*/
	return_code= audio->SecondaryBufferPtr->Lock ((DWORD)next_fill_pos,
																	(DWORD)config->HMIBufSize,
																	&play_buffer_ptr1,
																	&lock_length1,
																	&play_buffer_ptr2,
																	&lock_length2,
																	0 );

	if (return_code!=DS_OK) return(FALSE);

	int index = audio->AudioBufReadIndex;

	/*
	** Copy the HMI audio buffer to the direct sound buffer
	*/
	if (audio->AudioBufSize[index] > lock_length1) {

		memcpy((char*)play_buffer_ptr1, audio->AudioBuf[index], lock_length1);

		if (audio->AudioBufSize[index] - lock_length1 > lock_length2) {
			memcpy(play_buffer_ptr2, audio->AudioBuf[index], lock_length2);
		} else {
			memcpy(play_buffer_ptr2, audio->AudioBuf[index], audio->AudioBufSize[index] - lock_length1);
		}

	} else {
		memcpy(play_buffer_ptr1, audio->AudioBuf[index], audio->AudioBufSize[index]);
	}


	/*
	** Unlock the direct sound buffer
	*/
	audio->SecondaryBufferPtr->Unlock(play_buffer_ptr1,
												lock_length1,
												play_buffer_ptr2,
												lock_length2);

	/*
	** Update our audio data pointers
	*/
	audio->LastChunkPosition = next_fill_pos;
	audio->EndLastAudioChunk = next_fill_pos + audio->AudioBufSize[index];
	if (audio->EndLastAudioChunk >= audio->SecondaryBufferSize) {
		audio->EndLastAudioChunk = 0;
	}

	return(TRUE);
}

unsigned long Get_Playback_Position(VQAHandle *vqa, Ahandle *audio, VQAConfig *config)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;

	assert(vqap != 0);
	assert(audio != 0);
	assert(config != 0);

	unsigned long s = audio->SecondaryBufferSize;
	assert(s > 0);

	unsigned long dma_diff;
	unsigned long totalbytes;
	DWORD				play_cursor;		//Position that direct sound is reading from
	DWORD				write_cursor;		//Position in buffer that we can write to

	EnterCriticalSection(&audio->CriticalSection);

	long r = vqap->RepeatedBuffers;
	long l = audio->LastChunkPosition;
	long m = audio->ChunksMovedToAudioBuffer;

	totalbytes = config->HMIBufSize * m;

	if (audio->SecondaryBufferPtr &&
		audio->SecondaryBufferPtr->GetCurrentPosition (&play_cursor, &write_cursor) == S_OK) {
		if (l) {
			totalbytes += play_cursor;
		} else {
			if (play_cursor >= (s / 2)) {
				totalbytes += play_cursor - (s / 2);
			} else {
				if (totalbytes > 0) {
					totalbytes += play_cursor + (s / 2);
				}
			}
		}
	}

	LeaveCriticalSection(&audio->CriticalSection);

	dma_diff = totalbytes - config->HMIBufSize * r;
	if (dma_diff > totalbytes) {
		dma_diff = 0;
	}
	return dma_diff;
}

unsigned long __cdecl re_Timer_Callback_Audio_Handler(VQAHandle *vqa)
{
	VQAHandleP    *vqap = (VQAHandleP *)vqa;
	VQAConfig     *config = &vqap->Config;
	Ahandle       *handle = &_handles[vqap->field_34];
	unsigned long d;
	unsigned long t;

	if (handle->Flags & AHANDLEF_IS_PAUSED) {
		DEBUG_INFO("Ahandle: Paused %ld\n", handle->TickCount);
		return(handle->TickCount);
	}

	t = Simple_Timer_Callback_Audio_Handler(NULL) - handle->PauseAdjust;

	d = Get_Playback_Position(vqa, handle, config);

	if (d > 0 && d <= handle->LastPlaybackPosition) {
		if (t > handle->LastTimerTick) {
			d = (t - handle->LastTimerTick);
			handle->LastTimerTick = t;
			handle->TickCount += d;
		}
	} else {
		handle->LastPlaybackPosition = d;
		handle->LastTimerTick = t;
		handle->TickCount = (VQA_TIMETICKS * (d / (vqap->Channels * (vqap->BitsPerSample >> 3))) / vqap->SampleRate);
		
		if (handle->TickCount >= config->LatencyAdjustment) {
			handle->TickCount -= config->LatencyAdjustment;
		} else {
			handle->TickCount = 0;
		}
	}
	return handle->TickCount;
}

long __cdecl re_Stream_Audio_Handler(VQAHandle *vqa, long action, void *buffer, long nbytes)
{
	VQAHandleP *vqap = (VQAHandleP *)vqa;
	VQAConfig *config;

	long error = VQAERR_NONE;

	switch(action) {

		case 1:
			config = &vqap->Config;
			config->TimerCallback = Simple_Timer_Callback_Audio_Handler;
			config->RefreshRate = VQA_TIMETICKS;
			break;

		case 2:
			error = Open_Audio_Handler((VQAHandleP *)vqa, (AhandleInitParams *)buffer, nbytes);
			break;

		case 3:
			error = Close_Audio_Handler((VQAHandleP *)vqa);
			break;

		case 4:
			error = Start_Audio_Handler((VQAHandleP *)vqa);
			break;

		case 5:
			error = Load_Audio_Handler((VQAHandleP *)vqa, buffer, nbytes);
			break;

		case 6:
			error = Pause_Audio_Handler((VQAHandleP *)vqa);
			break;

		case 8:
			error = Play_Audio_Handler((VQAHandleP *)vqa);
			break;

		case 7:
			error = Stop_Audio_Handler((VQAHandleP *)vqa);
			break;
	}

	return(error);
}

long __cdecl re_Open_Audio_Handler(VQAHandleP *vqap, AhandleInitParams *params, long b)
{
	DEBUG_INFO("Opening VQ audio handler\n");
	DEBUG_INFO("Current thread ID is %08x\n", GetCurrentThreadId());

	if (!Audio.Is_Available()) {
		return(VQAERR_AUDIO);
	}

	int index = 0;
	while (index < Ahandle::MAX_HANDLES) {
		if (!_handles[index].Used) {
			break;
		}
		index++;
	}

	if (index != 1 && b == 16) {

		VQAConfig *config = &vqap->Config;

		vqap->field_34 = index;

		Ahandle *handle = &_handles[index];
		memset(handle, 0, sizeof(*handle));
		handle->Used = TRUE;
		handle->Volume = config->Volume;

		InitializeCriticalSection(&handle->CriticalSection);

		if (config->AudioRate != -1) {
			handle->SampleRate = config->AudioRate;
		} else {
			if (config->FrameRate != vqap->FrameRate) {
				//TODO is config->FrameRate unsigned?
				handle->SampleRate = params->SampleRate * (unsigned)config->FrameRate / vqap->FrameRate;
			} else {
				handle->SampleRate = params->SampleRate;
			}
		}

		DSCAPS dscaps;
		memset(&dscaps, 0, sizeof(dscaps));
		dscaps.dwSize = sizeof(DSCAPS);
		
		DEBUG_INFO("Audio.Lock_Mutex\n");
		Audio.Lock_Mutex();

		Audio.SoundObject->GetCaps(&dscaps);

		if (dscaps.dwFlags & DSCAPS_EMULDRIVER) {
			DEBUG_INFO("Ahandle detected emulated sound driver. LatencyAdjustment = %ld ticks\n", config->LatencyAdjustment);
		} else {
			config->LatencyAdjustment = 0;
		}

		DEBUG_INFO("Audio.Get_Primary_Buffer()\n");

		memset(&restore_format, 0, sizeof(restore_format));
		WAVEFORMATEX format;
		memset(&format, 0, sizeof(format));

		DEBUG_INFO("Getting current primary buffer format\n");
		Audio.PrimaryBufferPtr->GetFormat(&restore_format, sizeof(restore_format), NULL);

		restore_primary = false;
		if (params->Channels != restore_format.nChannels || params->SampleRate != restore_format.nSamplesPerSec || params->BitsPerSample != restore_format.wBitsPerSample) {
			DEBUG_INFO("Changing primary buffer format\n");
			Audio.Stop_Primary_Sound_Buffer();
			DEBUG_INFO("Primary buffer stopped\n");
			restore_primary = true;

			format.wFormatTag		= WAVE_FORMAT_PCM;
			format.nSamplesPerSec	= params->SampleRate;
			format.nChannels		= params->Channels;
			format.wBitsPerSample	= (short) params->BitsPerSample;
			format.nBlockAlign		= (unsigned short)( (params->BitsPerSample/8) * params->Channels);
			format.nAvgBytesPerSec	= params->SampleRate * format.nBlockAlign;

			Audio.PrimaryBufferPtr->SetFormat(&format);
			DEBUG_INFO("Primary buffer format changed\n");
			Audio.Start_Primary_Sound_Buffer(false);
		}

		DEBUG_INFO("Audio.Unlock_Mutex\n");
		Audio.Unlock_Mutex();
		handle->Channels = params->Channels;
		handle->BitsPerSample = params->BitsPerSample;
		handle->InitFlags = params->Flags;
		_AHandleCallbackFunc1 = (AHANDLE_CALLBACK_1)params->Callback1;
		_AHandleCallbackFunc2 = (AHANDLE_CALLBACK_2)params->Callback2;

		DEBUG_INFO("Calling timeBeginPeriod\n");
		if (timeBeginPeriod(1000/VQA_TIMETICKS) != TIMERR_NOCANDO) {
			DEBUG_INFO("Creating VQ audio timer thread\n");
			// Set orf 60hz timer
			handle->TimerHandle = timeSetEvent ( 1000/VQA_TIMETICKS , 1 , AudioCallback , (DWORD)vqap , TIME_PERIODIC);
			DEBUG_INFO("VQ audio handler opened OK\n");
			if (handle->TimerHandle != 0) {
				return(VQAERR_NONE);
			}
			return(VQAERR_AUDIO);
		}
	}
	return(VQAERR_AUDIO);
}

long __cdecl re_Close_Audio_Handler(VQAHandleP *vqap)
{
	DEBUG_INFO("Closing VQ audio handler\n");

	Ahandle *handle = &_handles[vqap->field_34];

	if (handle->Used == 1) {

		if (handle->SecondaryBufferPtr != NULL) {
			DEBUG_INFO("Stop_Audio_Handler\n");
			Stop_Audio_Handler(vqap);
		}

		DEBUG_INFO("Calling timeKillEvent\n");
		timeKillEvent(handle->TimerHandle);
		handle->TimerHandle = 0;

		DEBUG_INFO("Calling timeEndPeriod\n");
		timeEndPeriod(1000/VQA_TIMETICKS);

		handle->Used = false;

		if (_restore_primary) {
			DEBUG_INFO("Changing primary buffer format back to original\n");
			_restore_primary = false;

			DEBUG_INFO("Audio.Lock_Mutex()\n");
			Audio.Lock_Mutex();

			DEBUG_INFO("Stopping primary buffer\n");
			Audio.Stop_Primary_Sound_Buffer();

			DEBUG_INFO("Calling SetFormat\n");
			Audio.PrimaryBufferPtr->SetFormat(&_restore_format);

			Audio.Start_Primary_Sound_Buffer(false);

			DEBUG_INFO("Audio.Unlock_Mutex()\n");
			Audio.Unlock_Mutex();
		}
		DEBUG_INFO("Deleting the critical section object\n");
		DeleteCriticalSection(&handle->CriticalSection);
	}

	DEBUG_INFO("VQ audio handler closed OK\n");
	return(VQAERR_NONE);
}

long __cdecl re_Start_Audio_Handler(VQAHandleP *vqap)
{
	/* Dereference commonly used data members for quicker access. */
	VQAConfig *config = &vqap->Config;
	Ahandle *audio = &_handles[vqap->field_34];

	//TODO resolve
	if (audio->Flags & AHANDLEF_IS_PAUSED) {
		return(Play_Audio_Handler(vqap));
	}

	EnterCriticalSection(&audio->CriticalSection);

	assert(audio->SecondaryBufferPtr == NULL);

	/*
	** If we already have a direct sound secondary buffer then get rid of it
	*/
	if (audio->SecondaryBufferPtr != NULL){
		audio->SecondaryBufferPtr->Stop();
		audio->SecondaryBufferPtr->Release();
		audio->SecondaryBufferPtr = NULL;
	}

	/*
	** Make it big enough for 2 blocks of HMI data
	*/
	audio->SecondaryBufferSize = config->HMIBufSize*2;

	/*
	** Define the format for the secondary sound buffer
	*/
	memset (&audio->BufferDesc , 0 , sizeof(DSBUFFERDESC));
	audio->BufferDesc.dwSize				= sizeof(DSBUFFERDESC);
	audio->BufferDesc.dwFlags				= DSBCAPS_CTRLVOLUME|DSBCAPS_GETCURRENTPOSITION2;
	audio->BufferDesc.dwBufferBytes		= audio->SecondaryBufferSize;
	audio->BufferDesc.lpwfxFormat 		= (LPWAVEFORMATEX) &audio->DsBuffFormat;
	memset (&audio->DsBuffFormat , 0 , sizeof(WAVEFORMATEX));
	audio->DsBuffFormat.wFormatTag		= WAVE_FORMAT_PCM;
	audio->DsBuffFormat.nSamplesPerSec	= audio->SampleRate;
	audio->DsBuffFormat.nChannels			= audio->Channels;
	audio->DsBuffFormat.wBitsPerSample	= audio->BitsPerSample;
	audio->DsBuffFormat.nBlockAlign		= (short) ((audio->DsBuffFormat.wBitsPerSample/8) * audio->DsBuffFormat.nChannels);
	audio->DsBuffFormat.nAvgBytesPerSec	= audio->DsBuffFormat.nSamplesPerSec * audio->DsBuffFormat.nBlockAlign;

	/*
	** Create the secondary sound buffer object
	*/
	Audio.Lock_Mutex();
	Audio.SoundObject->CreateSoundBuffer (&audio->BufferDesc , &audio->SecondaryBufferPtr , NULL);
	Audio.Unlock_Mutex();

	if (audio->SecondaryBufferPtr == NULL) {
		LeaveCriticalSection(&audio->CriticalSection);
		return(VQAERR_AUDIO);
	}

	/* Start playback */
	_AHandleCallbackFunc1((VQAHandle *)vqap);
	_AHandleCallbackFunc1((VQAHandle *)vqap);
	audio->AudioBufWriteIndex = 0;
	audio->EndLastAudioChunk = 0;
	audio->ChunksMovedToAudioBuffer = 0;
	audio->SecondaryBufferPtr->SetCurrentPosition (0);

	/*
	** Set the volume
	*/
	audio->SecondaryBufferPtr->SetVolume(Convert_HMI_To_Direct_Sound_Volume(audio->Volume & 255));

	HRESULT return_code = audio->SecondaryBufferPtr->Play(0, 0, DSBPLAY_LOOPING);
	LeaveCriticalSection(&audio->CriticalSection);
	return(return_code == DS_OK ? VQAERR_NONE : VQAERR_AUDIO);
}

long __cdecl re_Load_Audio_Handler(VQAHandleP *vqap, void *buffer, long nbytes)
{
	Ahandle *handle = &_handles[vqap->field_34];

	if (buffer != NULL && nbytes != 0) {
		EnterCriticalSection(&handle->CriticalSection);
		if (handle->AudioBufInUse[handle->AudioBufReadIndex] == TRUE) {
			LeaveCriticalSection(&handle->CriticalSection);
			return(VQAERR_AUDIO);
		}
		int readindex = handle->AudioBufReadIndex;
		handle->AudioBuf[readindex] = buffer;
		handle->AudioBufSize[readindex] = nbytes;
		handle->AudioBufInUse[readindex] = TRUE;
		re_Move_HMI_Audio_Block_To_Direct_Sound_Buffer(vqap);
		int index = readindex + 1;
		if (index >= Ahandle::MAX_BUFFERS) {
			index = 0;
		}
		handle->AudioBufReadIndex = index;
		LeaveCriticalSection(&handle->CriticalSection);
		return(VQAERR_NONE);
	}
	return(VQAERR_AUDIO);
}

long __cdecl re_Pause_Audio_Handler(VQAHandleP *vqap)
{
	Ahandle *handle = &_handles[vqap->field_34];

	EnterCriticalSection(&handle->CriticalSection);

	if (handle->Used == true && handle->SecondaryBufferPtr != NULL) {
		handle->SecondaryBufferPtr->Stop();
	}
	handle->Flags |= AHANDLEF_IS_PAUSED;

	LeaveCriticalSection(&handle->CriticalSection);

	return(VQAERR_NONE);
}

long __cdecl re_Play_Audio_Handler(VQAHandleP *vqap)
{
	Ahandle *handle = &_handles[vqap->field_34];

	long rc;
	if (!handle->Used || handle->SecondaryBufferPtr == NULL) {
		return(VQAERR_AUDIO);
	}

	EnterCriticalSection(&handle->CriticalSection);

	rc = handle->SecondaryBufferPtr->Play(0, 0, DSBPLAY_LOOPING);
	if (rc == S_OK) {
		handle->PauseAdjust = Simple_Timer_Callback_Audio_Handler(NULL) - handle->LastTimerTick;
		DEBUG_INFO("Ahandle: PauseAdjust %ld\n", handle->PauseAdjust);
		handle->Flags &= ~AHANDLEF_IS_PAUSED;
		rc = VQAERR_NONE;
	} else {
		rc = VQAERR_AUDIO;
	}
	LeaveCriticalSection(&handle->CriticalSection);

	return(rc);
}

long __cdecl re_Stop_Audio_Handler(VQAHandleP *vqap)
{
	Ahandle *handle = &_handles[vqap->field_34];

	if (handle->SecondaryBufferPtr != NULL) {

		EnterCriticalSection(&handle->CriticalSection);
		handle->SecondaryBufferPtr->Stop();
		handle->SecondaryBufferPtr->Release();
		handle->SecondaryBufferPtr = NULL;
		handle->AudioBufReadIndex = 0;
		handle->AudioBufWriteIndex = 0;
		for (int i = 0; i < Ahandle::MAX_BUFFERS; i++) {
			handle->AudioBufInUse[i] = false;
		}

		LeaveCriticalSection(&handle->CriticalSection);
	}
	return(VQAERR_NONE);
}

void CALLBACK re_AudioCallback ( UINT uTimerID, UINT, DWORD dwUser, DWORD, DWORD )
{
	Ahandle  	*audio;
	DWORD			play_cursor;		//Position that direct sound is reading from
	DWORD			write_cursor;		//Position in buffer that we can write to
	HRESULT		return_code;
	bool			buffer_stopped = false;
	DWORD			status;

	VQAHandle *vqa = (VQAHandle *)dwUser;
	VQAHandleP *vqap = (VQAHandleP *)dwUser;

	audio = &_handles[vqap->field_34];

	if (InterlockedIncrement(&audio->SuspendAudioCallback) != TRUE || (audio->Flags & AHANDLEF_IS_PAUSED)) {
		InterlockedDecrement(&audio->SuspendAudioCallback);
		return;
	}

	EnterCriticalSection(&audio->CriticalSection);

	if (!audio->SecondaryBufferPtr || audio->TimerHandle != uTimerID)  {
		LeaveCriticalSection(&audio->CriticalSection);
		InterlockedDecrement(&audio->SuspendAudioCallback);
		return;
	}

	return_code = audio->SecondaryBufferPtr->GetStatus(&status);
	if (!(status & DSBSTATUS_PLAYING) && !(status & DSBSTATUS_LOOPING)) {
		LeaveCriticalSection(&audio->CriticalSection);
		InterlockedDecrement(&audio->SuspendAudioCallback);
		return;
	}

	/*
	** See if we are nearing the end of the meaningful data in the direct sound buffer
	*/
	return_code = audio->SecondaryBufferPtr->GetCurrentPosition (&play_cursor , &write_cursor);

	bool write_more = false;

	if (return_code == DSERR_BUFFERLOST) {
		audio->SecondaryBufferPtr->Restore();
		audio->SecondaryBufferPtr->Stop();
		buffer_stopped = true;
		audio->SecondaryBufferPtr->SetCurrentPosition (0);
		audio->LastChunkPosition = 0;
		audio->EndLastAudioChunk = 0;
		write_more = true;
	}


	if (play_cursor < audio->EndLastAudioChunk){
		write_more = true;
	}else{
		if ((play_cursor >= audio->SecondaryBufferSize / 2) && audio->EndLastAudioChunk==0){
			write_more = true;
		}
	}

	/*
	** See if we need to fill the buffer
	*/
	if (write_more == true) {
		_AHandleCallbackFunc2((VQAHandle *)vqa, audio->AudioBuf[audio->AudioBufWriteIndex]);
		audio->ChunksMovedToAudioBuffer++;
		int index = audio->AudioBufWriteIndex + 1;
		if (index >= Ahandle::MAX_BUFFERS) {
			index = 0;
		}
		audio->AudioBufInUse[audio->AudioBufWriteIndex] = FALSE;
		_AHandleCallbackFunc1((VQAHandle *)vqa);

		if (audio->AudioBufInUse[index]) {
			audio->AudioBufWriteIndex = index;
		}

		/*
		** Start the buffer playing again if we had to stop it
		*/
		if (buffer_stopped == true) {
			audio->SecondaryBufferPtr->Play(0,0,DSBPLAY_LOOPING);
		}
	}
	LeaveCriticalSection(&audio->CriticalSection);
	InterlockedDecrement(&audio->SuspendAudioCallback);
}

BOOL Move_HMI_Audio_Block_To_Direct_Sound_Buffer(VQAHandleP *vqap)
{
	Ahandle  	*audio;

	LPVOID		play_buffer_ptr1;	//Beginning of locked area of buffer
	LPVOID		play_buffer_ptr2;	//Length of locked area in buffer
	DWORD			lock_length1;		//Beginning of second locked area in buffer
	DWORD			lock_length2;		//Length of second locked area in buffer
	unsigned		next_fill_pos;
	HRESULT		return_code;

	audio = &_handles[vqap->field_34];
	VQAConfig *config = &vqap->Config;


	if (audio->SecondaryBufferPtr == NULL) {
		return 0;
	}

	/*************************************************************************
	**
	** Copy the data from the HMI play position into the direct sound buffer
	**
	*/
	next_fill_pos = audio->EndLastAudioChunk;

	/*
	** Lock the buffer to get a pointer to it
	*/
	return_code= audio->SecondaryBufferPtr->Lock ((DWORD)next_fill_pos,
																	(DWORD)config->HMIBufSize,
																	&play_buffer_ptr1,
																	&lock_length1,
																	&play_buffer_ptr2,
																	&lock_length2,
																	0 );

	if (return_code!=DS_OK) return(FALSE);

	int index = audio->AudioBufReadIndex;

	/*
	** Copy the HMI audio buffer to the direct sound buffer
	*/
	if (audio->AudioBufSize[index] > lock_length1) {

		memcpy((char*)play_buffer_ptr1, audio->AudioBuf[index], lock_length1);

		if (audio->AudioBufSize[index] - lock_length1 > lock_length2) {
			memcpy(play_buffer_ptr2, audio->AudioBuf[index], lock_length2);
		} else {
			memcpy(play_buffer_ptr2, audio->AudioBuf[index], audio->AudioBufSize[index] - lock_length1);
		}

	} else {
		memcpy(play_buffer_ptr1, audio->AudioBuf[index], audio->AudioBufSize[index]);
	}


	/*
	** Unlock the direct sound buffer
	*/
	audio->SecondaryBufferPtr->Unlock(play_buffer_ptr1,
												lock_length1,
												play_buffer_ptr2,
												lock_length2);

	/*
	** Update our audio data pointers
	*/
	audio->LastChunkPosition = next_fill_pos;
	audio->EndLastAudioChunk = next_fill_pos + audio->AudioBufSize[index];
	if (audio->EndLastAudioChunk >= audio->SecondaryBufferSize) {
		audio->EndLastAudioChunk = 0;
	}

	return(TRUE);
}

unsigned long __cdecl re_Simple_Timer_Callback_Audio_Handler(VQAHandle *vqa)
{
	return Get_Game_Time_60Hz();
}

void re_Pause_All_Audio_Handler(void)
{
	for (int i = 0; i < Ahandle::MAX_HANDLES; i++) {
		Ahandle *handle = &_handles[i];
		if (handle->Used == true && handle->SecondaryBufferPtr != NULL) {

			EnterCriticalSection(&handle->CriticalSection);

			handle->SecondaryBufferPtr->Stop();
			handle->Flags |= AHANDLEF_IS_PAUSED;

			LeaveCriticalSection(&handle->CriticalSection);

		}
	}
}

void re_Resume_All_Audio_Handler(void)
{
	for (int i = 0; i < Ahandle::MAX_HANDLES; i++) {
		Ahandle *handle = &_handles[i];
		if (handle->Used == true && handle->SecondaryBufferPtr != NULL) {

			EnterCriticalSection(&handle->CriticalSection);

			handle->SecondaryBufferPtr->Play(0, 0, DSBPLAY_LOOPING);
			handle->Flags &= ~AHANDLEF_IS_PAUSED;

			LeaveCriticalSection(&handle->CriticalSection);
			

		}
	}
}

long __cdecl re_Lock_Audio_Handler(void)
{
	return(1);
}

long __cdecl re_Unlock_Audio_Handler(void)
{
	return(1);
}


#include "hooker.h"


void Ahandle_Test_Hooks()
{
    Patch_Jump(0x004072D0, &re_Timer_Callback_Audio_Handler);
    Patch_Jump(0x00407450, &re_Stream_Audio_Handler);
    Patch_Jump(0x00407650, &re_Open_Audio_Handler);
    Patch_Jump(0x00407980, &re_Close_Audio_Handler);
    Patch_Jump(0x00407B20, &re_Start_Audio_Handler);
    Patch_Jump(0x00407D30, &re_Load_Audio_Handler);
    Patch_Jump(0x00407EF0, &re_Pause_Audio_Handler);
    Patch_Jump(0x00407F40, &re_Play_Audio_Handler);
    Patch_Jump(0x00407FE0, &re_Stop_Audio_Handler);
    Patch_Jump(0x00408060, &re_AudioCallback);
    Patch_Jump(0x00408200, &re_Simple_Timer_Callback_Audio_Handler);
    Patch_Jump(0x00408210, &re_Pause_All_Audio_Handler);
    Patch_Jump(0x00408260, &re_Resume_All_Audio_Handler);
    Patch_Jump(0x004082B0, &re_Lock_Audio_Handler);
    Patch_Jump(0x004082C0, &re_Unlock_Audio_Handler);
}
