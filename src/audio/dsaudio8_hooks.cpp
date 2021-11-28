#include "dsaudio8_hooks.h"
#include "dsaudio8_util.h"
#include "dsaudio8.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"

#include "hooker.h"
#include "hooker_macros.h"


#if 0
/** 
 *  This class fakes generation of member functions with __thiscall, allowing to you to call
 *  constructors and wrappers to virtual functions without writing these in the class sources
 *  themself. You can also use this to handle hooking of functions that share the same
 *  name but have different input arguments.
 */
class DSAudioHookingClass
{
    public:
        DirectSound8AudioClass *DirectSound8AudioClass_Hook_Ctor() { return new (this) DirectSound8AudioClass(); }
        void DirectSound8AudioClass_Hook_Dtor() { reinterpret_cast<DirectSound8AudioClass *>(this)->DirectSound8AudioClass::~DirectSound8AudioClass(); }
};
#endif



DECLARE_PATCH(_WinMain_Init_Audio_1_Patch)
{
    DirectSound8Audio.Init(MainWindow, 16, true, 48000);

    JMP(0x006013C8);
}

DECLARE_PATCH(_WinMain_Init_Audio_2_Patch)
{
    DirectSound8Audio.Init(MainWindow, 16, true, 48000);

    JMP(0x006016B3);
}


/**
 *  Main function for patching the hooks.
 */
void DirectSound8_Hooks()
{
#if 1
    /**
     *  Set the new default sound buffer format.
     */
    Patch_Jump(0x006013B0, &_WinMain_Init_Audio_1_Patch);
    Patch_Jump(0x0060169B, &_WinMain_Init_Audio_2_Patch);

    /**
     *  Removes the initialisation of the games audio engine.
     */
    Patch_Byte(0x00487990, 0xC3); // asm "ret"
    Patch_Byte(0x004879B0, 0xC3); // asm "ret"

    /**
     *  Function hooks.
     */
    //Patch_Jump(0x004879C0, &Convert_HMI_To_Direct_Sound_Volume);              // Not needed
    
    //Patch_Jump(0x00487A20, &DSAudioHookingClass::DirectSound8AudioClass_Hook_Ctor); // Not needed, global instance handles this.
    //Patch_Jump(0x00487B20, &DSAudioHookingClass::DirectSound8AudioClass_Hook_Dtor); // Not needed, global instance handles this.
    
    //Patch_Jump(0x00487C50, &DirectSound8AudioClass::Init);                    // Not needed, WinMain patches handle these calls.
    
    //Patch_Call(0x00487B31, &DirectSound8AudioClass::End);                     // Not needed
    Patch_Call(0x00601A17, &DirectSound8AudioClass::End);
    Patch_Call(0x00602485, &DirectSound8AudioClass::End);

    // some xrefs not needed
    Patch_Call(0x00554048, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x005544C4, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x005545E7, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x0055466D, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x00644043, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x00644233, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x00644295, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x0067E8FB, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x0067FCA9, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x0067FD89, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x0067FE5C, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x0067FE96, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x00680063, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x006800F1, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x0068018C, &DirectSound8AudioClass::Stop_Sample);
    Patch_Call(0x006801C6, &DirectSound8AudioClass::Stop_Sample);
    
    // some xrefs not needed
    Patch_Call(0x00643E12, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x00643FC3, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x006441EF, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x006442E8, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x0067F892, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x0067FB46, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x0067FC3A, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x0067FC97, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x0067FD77, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x0067FE84, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x0067FF2E, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x00680051, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x0068008E, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x006801B4, &DirectSound8AudioClass::Sample_Status);
    Patch_Call(0x00680321, &DirectSound8AudioClass::Sample_Status);

    Patch_Call(0x00665999, &DirectSound8AudioClass::Is_Sample_Playing);
    Patch_Call(0x00665B5B, &DirectSound8AudioClass::Is_Sample_Playing);

    Patch_Call(0x0056E724, &DirectSound8AudioClass::Stop_Sample_Playing);
    Patch_Call(0x0056E825, &DirectSound8AudioClass::Stop_Sample_Playing);
    Patch_Call(0x0056F3C0, &DirectSound8AudioClass::Stop_Sample_Playing);
    Patch_Call(0x00571F88, &DirectSound8AudioClass::Stop_Sample_Playing);
    Patch_Call(0x00574CB0, &DirectSound8AudioClass::Stop_Sample_Playing);
    Patch_Call(0x005E4B80, &DirectSound8AudioClass::Stop_Sample_Playing);
    Patch_Call(0x005E68A6, &DirectSound8AudioClass::Stop_Sample_Playing);
    Patch_Call(0x005E73C0, &DirectSound8AudioClass::Stop_Sample_Playing);
    Patch_Call(0x00665B0C, &DirectSound8AudioClass::Stop_Sample_Playing);

    //Patch_Jump(0x004887C0, &DirectSound8AudioClass::Get_Free_Sample_Handle);  // Not needed

    Patch_Call(0x0056D05F, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x0056D165, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x0056F412, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x00572571, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x00574CF7, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x00593DF4, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x005E35B5, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x005E3701, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x005E3E66, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x005E4432, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x005E5830, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x005E6001, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x005E61C1, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x005E67E0, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x005E6BBF, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x00664AE5, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x00664B89, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x00664C44, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x00664CF8, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x00664E9A, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x00665ACE, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x0067FD12, &DirectSound8AudioClass::Play_Sample);
    Patch_Call(0x006800CA, &DirectSound8AudioClass::Play_Sample);

    // We need to hook Play_Sample directly due to TS++ inlined functioned.
    Patch_Jump(0x00488960, &DirectSound8AudioClass::Play_Sample);

    //Patch_Jump(0x00488990, &DirectSound8AudioClass::Play_Sample_Handle);      // Not needed

    //Patch_Jump(0x004890F0, &DirectSound8AudioClass::Attempt_Audio_Restore);     // No xrefs

    //Patch_Jump(0x00489140, &DirectSound8AudioClass::Lock_Global_Mutex);         // No xrefs

    //Patch_Jump(0x00489160, &DirectSound8AudioClass::Unlock_Global_Mutex);       // No xrefs

    Patch_Call(0x0040776A, &DirectSound8AudioClass::Lock_Mutex);
    Patch_Call(0x00407A8C, &DirectSound8AudioClass::Lock_Mutex);
    Patch_Call(0x00407C8C, &DirectSound8AudioClass::Lock_Mutex);

    Patch_Call(0x004078EB, &DirectSound8AudioClass::Unlock_Mutex);
    Patch_Call(0x00407AE2, &DirectSound8AudioClass::Unlock_Mutex);

    //Patch_Jump(0x00489290, &DirectSound8AudioClass::Sound_Timer_Callback);    // Not needed

    //Patch_Jump(0x004892C0, &DirectSound8AudioClass::maintenance_callback);    // Not needed

    //Patch_Jump(0x004895E0, &DirectSound8AudioClass::Stream_Sample_Vol);       // Not needed

    //Patch_Jump(0x004896C0, &DirectSound8AudioClass::File_Stream_Preload);     // Not needed

    Patch_Call(0x0055405A, &DirectSound8AudioClass::File_Stream_Sample_Vol);
    Patch_Call(0x005545FD, &DirectSound8AudioClass::File_Stream_Sample_Vol);
    Patch_Call(0x006440C1, &DirectSound8AudioClass::File_Stream_Sample_Vol);

    Patch_Call(0x00462C8C, &DirectSound8AudioClass::Sound_Callback);
    Patch_Call(0x00594281, &DirectSound8AudioClass::Sound_Callback);
    Patch_Call(0x00643E6A, &DirectSound8AudioClass::Sound_Callback);

    //Patch_Jump(0x00489B20, &DirectSound8AudioClass::File_Callback);           // Not needed

    //Patch_Jump(0x00489D10, &DirectSound8AudioClass::Print_Sound_Error);       // No xrefs

    //Patch_Jump(0x00489D70, &DirectSound8AudioClass::Set_Primary_Buffer_Format); // No xrefs

    //Patch_Jump(0x00489DA0, &DirectSound8AudioClass::Restore_Sound_Buffers);   // Not needed

    Patch_Call(0x005BCE2E, &DirectSound8AudioClass::Set_Volume_All);

    Patch_Call(0x005BCC67, &DirectSound8AudioClass::Adjust_Volume_All);

    Patch_Call(0x00644431, &DirectSound8AudioClass::Set_Handle_Volume);

    Patch_Call(0x00665BB1, &DirectSound8AudioClass::Set_Sample_Volume);

    //Patch_Jump(0x0048A250, &DirectSound8AudioClass::sub_48A250);              // No xrefs

    //Patch_Jump(0x0048A340, &DirectSound8AudioClass::sub_48A340);              // No xrefs

    Patch_Call(0x0055423B, &DirectSound8AudioClass::Fade_Sample);
    Patch_Call(0x00554430, &DirectSound8AudioClass::Fade_Sample);
    Patch_Call(0x0055456A, &DirectSound8AudioClass::Fade_Sample);
    Patch_Call(0x00554660, &DirectSound8AudioClass::Fade_Sample);
    Patch_Call(0x00643FD6, &DirectSound8AudioClass::Fade_Sample);
    Patch_Call(0x00644213, &DirectSound8AudioClass::Fade_Sample);

    Patch_Jump(0x0048A4D0, &DirectSound8AudioClass::Get_Playing_Sample_Handle);    // ??????

    Patch_Call(0x004078D4, &DirectSound8AudioClass::Start_Primary_Sound_Buffer);
    Patch_Call(0x00407ACB, &DirectSound8AudioClass::Start_Primary_Sound_Buffer);
    Patch_Call(0x00488FB3, &DirectSound8AudioClass::Start_Primary_Sound_Buffer);
    Patch_Call(0x00685A26, &DirectSound8AudioClass::Start_Primary_Sound_Buffer);

    Patch_Call(0x00407851, &DirectSound8AudioClass::Stop_Primary_Sound_Buffer);
    Patch_Call(0x00407AA3, &DirectSound8AudioClass::Stop_Primary_Sound_Buffer);
    Patch_Call(0x00685ED0, &DirectSound8AudioClass::Stop_Primary_Sound_Buffer);

    //Patch_Jump(0x0048A690, &DirectSound8AudioClass::Simple_Copy);             // Not needed

    //Patch_Jump(0x0048A750, &DirectSound8AudioClass::Sample_Copy);             // Not needed

    /**
     *  Global instance access.
     */
    Patch_Dword(0x00407765+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0040784C+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x004078CD+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x004078E6+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00407A87+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00407A9E+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00407AC6+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00407ADD+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00407C7D+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00462C87+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00487990+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x004879B0+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x004892A2+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00554043+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00554055+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00554236+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0055442B+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005544BF+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00554565+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005545E2+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005545F8+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0055465B+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00554668+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0056D05A+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0056D160+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0056E71F+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0056E820+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0056F3BB+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0056F40D+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00571F83+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0057256C+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00574CAB+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00574CF2+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00593DEF+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0059427C+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005BCC62+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005BCE28+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E35B0+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E36FC+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E3E61+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E442D+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E4B7B+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E582B+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E5FFC+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E61BC+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E67DB+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E68A1+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E6BBA+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x005E73BB+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x006013BE+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x006016A9+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00601A12+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00602480+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00643E0D+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00643E65+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00643FBE+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00643FD1+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0064403E+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x006440BC+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x006441EA+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0064420E+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0064422D+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00644290+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x006442E3+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0064442C+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00664ADE+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00664B82+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00664C3D+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00664CF1+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00664E93+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00665994+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00665AC2+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00665B07+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00665B56+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00665BAC+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067E8F6+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067F88D+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FB41+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FC35+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FC92+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FCA3+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FD0D+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FD72+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FD83+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FE57+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FE7F+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FE90+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0067FF29+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0068004C+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0068005D+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00680089+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x006800C5+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x006800EC+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00680187+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x006801AF+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x006801C0+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x0068031C+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00685A21+1, (uintptr_t)&DirectSound8Audio);
    Patch_Dword(0x00685ECB+1, (uintptr_t)&DirectSound8Audio);

    Patch_Dword(0x00686326+2, (uintptr_t)&DirectSound8Audio.AudioFocusLossFunction);

    Patch_Dword(0x00644080+2, (uintptr_t)&DirectSound8Audio.StreamLowImpact);
    Patch_Dword(0x006440C8+2, (uintptr_t)&DirectSound8Audio.StreamLowImpact);
    
// NOT NEEDED!
// Direction	Type	Address	Text
// Up	r	File_Callback(short,short *,void **,long *):loc_489BBB	imul    edx, DSAudio_StreamBufferSize
// Up	r	File_Callback(short,short *,void **,long *)+C6	mov     eax, DSAudio_StreamBufferSize
// Up	r	File_Callback(short,short *,void **,long *):loc_489C2A	imul    eax, DSAudio_StreamBufferSize
// Up	r	File_Callback(short,short *,void **,long *)+114	mov     edi, DSAudio_StreamBufferSize
// Up	r	File_Callback(short,short *,void **,long *)+125	mov     eax, DSAudio_StreamBufferSize
// Up	r	File_Callback(short,short *,void **,long *):loc_489CA9	imul    edx, DSAudio_StreamBufferSize
// Up	r	File_Callback(short,short *,void **,long *):loc_489CCD	mov     ecx, DSAudio_StreamBufferSize
    
// NOT NEEDED!
// Direction	Type	Address	Text
// Up	o	File_Callback(short,short *,void **,long *)+5C	lea     st, DSAudio_SampleTracker[ecx*8]
// Up	r	File_Callback(short,short *,void **,long *)+63	mov     cl, DSAudio_SampleTracker[ecx*8]
    
// NOT NEEDED!
// Direction	Type	Address	Text
// Up	r	File_Callback(short,short *,void **,long *)+55	mov     eax, st0_FileBuffer[ecx*8]

// NOT NEEDED!
// Direction	Type	Address	Text
// Up	r	Sound_Timer_Callback(uint,uint,ulong,ulong,ulong)	mov     eax, DSAudio_TimerMutex
// Up	r	Sound_Timer_Callback(uint,uint,ulong,ulong,ulong)+1C	mov     eax, DSAudio_TimerMutex

// NOT NEEDED!
// Direction	Type	Address	Text
// Up	r	File_Callback(short,short *,void **,long *)+1C	mov     eax, DSAudio_SecondaryBufferMutexes[edi*4]
// Up	r	File_Callback(short,short *,void **,long *)+1BD	mov     edx, DSAudio_SecondaryBufferMutexes[edi*4]
// Up	r	File_Callback(short,short *,void **,long *):loc_489CF4	mov     eax, DSAudio_SecondaryBufferMutexes[edi*4]
// Up	r	__thiscall DirectSoundAudioClass::Fade_Sample(int,int)+E	mov     eax, DSAudio_SecondaryBufferMutexes[esi*4]

    Patch_Dword(0x00407673+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x0040776F+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00462C6C+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x004AAACB+2, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x004E4615+2, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00508ACD+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x0050A758+2, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x0056419F+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x0056E646+2, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x0056E990+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x0056F296+2, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x0057146E+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005721B7+2, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00574BF8+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00594261+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005E3552+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005E369E+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005E3E03+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005E43CF+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005E57BB+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005E5F8B+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005E615E+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005E677E+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005E68D0+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005E7308+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x005FC531+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00643C87+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00643DC0+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00643F30+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00643F93+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00643FF2+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x006441A0+2, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x0064425C+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x006442B9+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00664A7E+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00664B28+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00664BDD+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00664C97+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00664E33+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00665823+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00665955+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00665B2E+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00665B8C+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x0067F77C+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x0067FA0C+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00685A0A+1, (uintptr_t)&DirectSound8Audio.SoundObject);
    Patch_Dword(0x00685EB9+1, (uintptr_t)&DirectSound8Audio.SoundObject);

// MIGHT NEED NEW AHANDLE DUE TO INTERFACE CHANGE??
    Patch_Dword(0x004077F2+1, (uintptr_t)&DirectSound8Audio.PrimaryBufferPtr);
    Patch_Dword(0x004078A5+1, (uintptr_t)&DirectSound8Audio.PrimaryBufferPtr);
    Patch_Dword(0x00407AB2+1, (uintptr_t)&DirectSound8Audio.PrimaryBufferPtr);

    Patch_Dword(0x00407683+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00462C75+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x004AAAD5+2, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x004E461D+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00508AD6+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x0050A762+2, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005641AC+2, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x0056E652+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x0056E99F+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x0056F2A2+2, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00571477+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005721C3+2, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00574C05+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x0059426A+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005E355B+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005E36A7+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005E3E0C+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005E43D8+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005E57C4+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005E5F98+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005E6167+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005E6787+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005E68E0+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005E7315+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x005FC541+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00643C94+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00643DD0+2, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00643F3D+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00643F9C+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00643FFF+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x006441AE+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00644265+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x006442C2+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00664A87+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00664B31+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00664BE6+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00664CA0+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00664E3C+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00665830+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00665962+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00665B37+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00665B95+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x0067F789+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x0067FA19+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00685A16+1, (uintptr_t)&DirectSound8Audio.AudioDone);
    Patch_Dword(0x00685EC2+1, (uintptr_t)&DirectSound8Audio.AudioDone);
#endif
}
