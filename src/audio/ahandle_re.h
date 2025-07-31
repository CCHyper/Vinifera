#pragma once

#include	"dsaudio.h"
#include	"ahandle.h"
#include	"vqa.h"

unsigned long __cdecl re_Simple_Timer_Callback_Audio_Handler(VQAHandle *vqa);
unsigned long __cdecl re_Timer_Callback_Audio_Handler(VQAHandle *vqa);

long __cdecl re_Lock_Audio_Handler(void);
long __cdecl re_Unlock_Audio_Handler(void);
long __cdecl re_Stream_Audio_Handler(VQAHandle *vqa, long action, void *buffer, long nbytes);

typedef long (__cdecl *AHANDLE_CALLBACK_1)(VQAHandle *vqa);
typedef long (__cdecl *AHANDLE_CALLBACK_2)(VQAHandle *vqa, void *buffer);
