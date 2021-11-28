/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FMOD_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the FMOD audio engine.
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
#include "fmod_hooks.h"
#include "fmod_util.h"
#include "fmod_audio.h"
#include "fmod_ahandle.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "credits.h"

#include "hooker.h"
#include "hooker_macros.h"



/**
 *  Limits the credits tick sound.
 */
#include "voc.h"
#include "rules.h"
#include "stimer.h"
DECLARE_PATCH(_CreditsClass_Graphic_Logic_Limit_Sfx_Patch)
{
    GET_REGISTER_STATIC(CreditClass *, this_ptr, 0x00471493);
    static CDTimerClass<MSTimerClass> _delay = 35;

    if (_delay.Expired()) {
        if (this_ptr->IsUp) {
            Sound_Effect(Rule->CreditTicks[0], 0, 0.5f);
        } else {
            Sound_Effect(Rule->CreditTicks[1], 0, 0.5f);
        }
        _delay = 35;
        _delay.Start();
    }

    JMP(0x004714CC);
}




/**
 *  Initialise the audio engine.
 */
DECLARE_PATCH(_WinMain_Init_Audio_1_Patch)
{
    FMODAudio.Init(MainWindow, 16, true, 48000);

    /**
     *  We need to retain DirectSound for the VQA player.
     */
    Audio.Init(MainWindow, 16, false, 22050);

    JMP(0x006013C8);
}

DECLARE_PATCH(_WinMain_Init_Audio_2_Patch)
{
    FMODAudio.Init(MainWindow, 16, true, 48000);

    /**
     *  We need to retain DirectSound for the VQA player.
     */
    Audio.Init(MainWindow, 16, false, 22050);

    JMP(0x006016B3);
}


/**
 *  Shutdown the audio engine.
 */
DECLARE_PATCH(_WinMain_End_Audio_Patch)
{
    FMODAudio.End();
    Audio.End();

    JMP(0x00601A1C);
}

DECLARE_PATCH(_Emergency_Exit_End_Audio_Patch)
{
    FMODAudio.End();
    Audio.End();

    JMP(0x0060248A);
}



/**
 *  Main function for patching the hooks.
 */
void FMOD_Audio_Hooks()
{
    Patch_Jump(0x00471493, &_CreditsClass_Graphic_Logic_Limit_Sfx_Patch);


#if 1
    /**
     *  Set the new default sound buffer format.
     */
    Patch_Jump(0x006013B0, &_WinMain_Init_Audio_1_Patch);
    Patch_Jump(0x0060169B, &_WinMain_Init_Audio_2_Patch);

    Patch_Jump(0x00601A12, &_WinMain_End_Audio_Patch);
    Patch_Jump(0x00602480, &_Emergency_Exit_End_Audio_Patch);

    /**
     *  Removes the initialisation of the games audio engine.
     */
//    Patch_Byte(0x00487990, 0xC3); // asm "ret"
//    Patch_Byte(0x004879B0, 0xC3); // asm "ret"

    /**
     *  Function hooks.
     */
    //Patch_Jump(0x004879C0, &Convert_HMI_To_Direct_Sound_Volume);              // Not needed
    
    //Patch_Jump(0x00487A20, &DSAudioHookingClass::FMODAudioClass_Hook_Ctor); // Not needed, global instance handles this.
    //Patch_Jump(0x00487B20, &DSAudioHookingClass::FMODAudioClass_Hook_Dtor); // Not needed, global instance handles this.
    
    //Patch_Jump(0x00487C50, &FMODAudioClass::Init);                    // Not needed, WinMain patches handle these calls.
    
    //Patch_Call(0x00487B31, &FMODAudioClass::End);                     // Not needed
    Patch_Call(0x00601A17, &FMODAudioClass::End);
    Patch_Call(0x00602485, &FMODAudioClass::End);

    // some xrefs not needed
    Patch_Call(0x00554048, &FMODAudioClass::Stop_Sample);     // MapSelect
    Patch_Call(0x005544C4, &FMODAudioClass::Stop_Sample);     // MapSelect
    Patch_Call(0x005545E7, &FMODAudioClass::Stop_Sample);     // MapSelect
    Patch_Call(0x0055466D, &FMODAudioClass::Stop_Sample);     // MapSelect
//    Patch_Call(0x00644043, &FMODAudioClass::Stop_Sample);     // ThemeClass
    Patch_Call(0x00644233, &FMODAudioClass::Stop_Sample);     // ThemeClass
    Patch_Call(0x00644295, &FMODAudioClass::Stop_Sample);     // ThemeClass
    Patch_Call(0x0067E8FB, &FMODAudioClass::Stop_Sample);     // WDT
    Patch_Call(0x0067FCA9, &FMODAudioClass::Stop_Sample);     // WDT
    Patch_Call(0x0067FD89, &FMODAudioClass::Stop_Sample);     // WDT
    Patch_Call(0x0067FE5C, &FMODAudioClass::Stop_Sample);     // WDT
    Patch_Call(0x0067FE96, &FMODAudioClass::Stop_Sample);     // WDT
    Patch_Call(0x00680063, &FMODAudioClass::Stop_Sample);     // WDT
    Patch_Call(0x006800F1, &FMODAudioClass::Stop_Sample);     // WDT
    Patch_Call(0x0068018C, &FMODAudioClass::Stop_Sample);     // WDT
    Patch_Call(0x006801C6, &FMODAudioClass::Stop_Sample);     // WDT
    
    // some xrefs not needed
    Patch_Call(0x00643E12, &FMODAudioClass::Sample_Status);     // ThemeClass
    Patch_Call(0x00643FC3, &FMODAudioClass::Sample_Status);     // ThemeClass
    Patch_Call(0x006441EF, &FMODAudioClass::Sample_Status);     // ThemeClass
    Patch_Call(0x006442E8, &FMODAudioClass::Sample_Status);     // ThemeClass
    Patch_Call(0x0067F892, &FMODAudioClass::Sample_Status);     // WDT
    Patch_Call(0x0067FB46, &FMODAudioClass::Sample_Status);     // WDT
    Patch_Call(0x0067FC3A, &FMODAudioClass::Sample_Status);     // WDT
    Patch_Call(0x0067FC97, &FMODAudioClass::Sample_Status);     // WDT
    Patch_Call(0x0067FD77, &FMODAudioClass::Sample_Status);     // WDT
    Patch_Call(0x0067FE84, &FMODAudioClass::Sample_Status);     // WDT
    Patch_Call(0x0067FF2E, &FMODAudioClass::Sample_Status);     // WDT
    Patch_Call(0x00680051, &FMODAudioClass::Sample_Status);     // WDT
    Patch_Call(0x0068008E, &FMODAudioClass::Sample_Status);     // WDT
    Patch_Call(0x006801B4, &FMODAudioClass::Sample_Status);     // WDT
    Patch_Call(0x00680321, &FMODAudioClass::Sample_Status);     // WDT

    Patch_Call(0x00665999, &FMODAudioClass::Is_Sample_Playing);     // Speak_AI
    Patch_Call(0x00665B5B, &FMODAudioClass::Is_Sample_Playing);     // Is_Speaking

    Patch_Call(0x0056E724, &FMODAudioClass::Stop_Sample_Playing);     // MapSelect
    Patch_Call(0x0056E825, &FMODAudioClass::Stop_Sample_Playing);     // MapStage
    Patch_Call(0x0056F3C0, &FMODAudioClass::Stop_Sample_Playing);     // MSSfxEntry
    Patch_Call(0x00571F88, &FMODAudioClass::Stop_Sample_Playing);     // MSFont
    Patch_Call(0x00574CB0, &FMODAudioClass::Stop_Sample_Playing);     // MSSfx
    Patch_Call(0x005E4B80, &FMODAudioClass::Stop_Sample_Playing);     // ScoreClass
    Patch_Call(0x005E68A6, &FMODAudioClass::Stop_Sample_Playing);     // ScoreFontClass
    Patch_Call(0x005E73C0, &FMODAudioClass::Stop_Sample_Playing);     // ScoreSoundClass
    Patch_Call(0x00665B0C, &FMODAudioClass::Stop_Sample_Playing);     // Stop_Speaking

    //Patch_Jump(0x004887C0, &FMODAudioClass::Get_Free_Sample_Handle);  // Not needed

    Patch_Call(0x0056D05F, &FMODAudioClass::Play_Sample);     // MSWordAnim
    Patch_Call(0x0056D165, &FMODAudioClass::Play_Sample);     // MSWordAnim
    Patch_Call(0x0056F412, &FMODAudioClass::Play_Sample);     // MSSfxEntry
    Patch_Call(0x00572571, &FMODAudioClass::Play_Sample);     // MSFont
    Patch_Call(0x00574CF7, &FMODAudioClass::Play_Sample);     // MSSfx
    Patch_Call(0x00593DF4, &FMODAudioClass::Play_Sample);     // OwnerDraw
    Patch_Call(0x005E35B5, &FMODAudioClass::Play_Sample);     // ScoreClass
    Patch_Call(0x005E3701, &FMODAudioClass::Play_Sample);     // ScoreClass
    Patch_Call(0x005E3E66, &FMODAudioClass::Play_Sample);     // ScoreClass
    Patch_Call(0x005E4432, &FMODAudioClass::Play_Sample);     // ScoreClass
    Patch_Call(0x005E5830, &FMODAudioClass::Play_Sample);     // ScoreClass
    Patch_Call(0x005E6001, &FMODAudioClass::Play_Sample);     // ScoreClass
    Patch_Call(0x005E61C1, &FMODAudioClass::Play_Sample);     // ScoreClass
    Patch_Call(0x005E67E0, &FMODAudioClass::Play_Sample);     // ScoreFontClass
    Patch_Call(0x005E6BBF, &FMODAudioClass::Play_Sample);     // ScoreFontClass
    Patch_Call(0x00664AE5, &FMODAudioClass::Play_Sample);     // VocClass
    Patch_Call(0x00664B89, &FMODAudioClass::Play_Sample);     // VocClass
    Patch_Call(0x00664C44, &FMODAudioClass::Play_Sample);     // VocClass
    Patch_Call(0x00664CF8, &FMODAudioClass::Play_Sample);     // VocClass
    Patch_Call(0x00664E9A, &FMODAudioClass::Play_Sample);     // VocClass
    Patch_Call(0x00665ACE, &FMODAudioClass::Play_Sample);     // Speak_AI
    Patch_Call(0x0067FD12, &FMODAudioClass::Play_Sample);     // WDT
    Patch_Call(0x006800CA, &FMODAudioClass::Play_Sample);     // WDT

    // We need to hook Play_Sample directly due to TS++ inlined functioned.
    Patch_Jump(0x00488960, &FMODAudioClass::Play_Sample);

    //Patch_Jump(0x00488990, &FMODAudioClass::Play_Sample_Handle);      // Not needed

    //Patch_Jump(0x004890F0, &FMODAudioClass::Attempt_Audio_Restore);     // No xrefs

    //Patch_Jump(0x00489140, &FMODAudioClass::Lock_Global_Mutex);         // No xrefs

    //Patch_Jump(0x00489160, &FMODAudioClass::Unlock_Global_Mutex);       // No xrefs

    //Patch_Call(0x0040776A, &FMODAudioClass::Lock_Mutex);        // Not needed
    //Patch_Call(0x00407A8C, &FMODAudioClass::Lock_Mutex);
    //Patch_Call(0x00407C8C, &FMODAudioClass::Lock_Mutex);

    //Patch_Call(0x004078EB, &FMODAudioClass::Unlock_Mutex);      // Not needed
    //Patch_Call(0x00407AE2, &FMODAudioClass::Unlock_Mutex);

    //Patch_Jump(0x00489290, &FMODAudioClass::Sound_Timer_Callback);    // Not needed

    //Patch_Jump(0x004892C0, &FMODAudioClass::maintenance_callback);    // Not needed

    //Patch_Jump(0x004895E0, &FMODAudioClass::Stream_Sample_Vol);       // Not needed

    //Patch_Jump(0x004896C0, &FMODAudioClass::File_Stream_Preload);     // Not needed

    Patch_Call(0x0055405A, &FMODAudioClass::File_Stream_Sample_Vol);     // MapSelect
    Patch_Call(0x005545FD, &FMODAudioClass::File_Stream_Sample_Vol);     // MapSelect
//    Patch_Call(0x006440C1, &FMODAudioClass::File_Stream_Sample_Vol);     // ThemeClass

    Patch_Call(0x00462C8C, &FMODAudioClass::Sound_Callback);     // Call_Back
    Patch_Call(0x00594281, &FMODAudioClass::Sound_Callback);     // OwnerDraw
    Patch_Call(0x00643E6A, &FMODAudioClass::Sound_Callback);     // ThemeClass

    //Patch_Jump(0x00489B20, &FMODAudioClass::File_Callback);           // Not needed

    //Patch_Jump(0x00489D10, &FMODAudioClass::Print_Sound_Error);       // No xrefs

    //Patch_Jump(0x00489D70, &FMODAudioClass::Set_Primary_Buffer_Format); // No xrefs

    //Patch_Jump(0x00489DA0, &FMODAudioClass::Restore_Sound_Buffers);   // Not needed

    Patch_Call(0x005BCE2E, &FMODAudioClass::Set_Volume_All);     // RadarClass

    Patch_Call(0x005BCC67, &FMODAudioClass::Set_Volume_Percent_All);     // RadarClass

    Patch_Call(0x00644431, &FMODAudioClass::Set_Handle_Volume);     // ThemeClass

    Patch_Call(0x00665BB1, &FMODAudioClass::Set_Sample_Volume);     // Set_Speech_Volume

    //Patch_Jump(0x0048A250, &FMODAudioClass::sub_48A250);              // No xrefs

    //Patch_Jump(0x0048A340, &FMODAudioClass::sub_48A340);              // No xrefs

    Patch_Call(0x0055423B, &FMODAudioClass::Fade_Sample);     // MapSelect
    Patch_Call(0x00554430, &FMODAudioClass::Fade_Sample);     // MapSelect
    Patch_Call(0x0055456A, &FMODAudioClass::Fade_Sample);     // MapSelect
    Patch_Call(0x00554660, &FMODAudioClass::Fade_Sample);     // MapSelect
    Patch_Call(0x00643FD6, &FMODAudioClass::Fade_Sample);     // ThemeClass
    Patch_Call(0x00644213, &FMODAudioClass::Fade_Sample);     // ThemeClass

    //Patch_Jump(0x0048A4D0, &FMODAudioClass::Get_Playing_Sample_Handle);      // No xrefs

    //Patch_Call(0x004078D4, &FMODAudioClass::Start_Primary_Sound_Buffer);        // AHandle
    //Patch_Call(0x00407ACB, &FMODAudioClass::Start_Primary_Sound_Buffer);        // AHandle
    //Patch_Call(0x00488FB3, &FMODAudioClass::Start_Primary_Sound_Buffer);        // Not needed
    Patch_Call(0x00685A26, &FMODAudioClass::Start_Primary_Sound_Buffer);     // Focus_Restore

    //Patch_Call(0x00407851, &FMODAudioClass::Stop_Primary_Sound_Buffer);        // AHandle
    //Patch_Call(0x00407AA3, &FMODAudioClass::Stop_Primary_Sound_Buffer);        // AHandle
    Patch_Call(0x00685ED0, &FMODAudioClass::Stop_Primary_Sound_Buffer);

    //Patch_Jump(0x0048A690, &FMODAudioClass::Simple_Copy);             // Not needed

    //Patch_Jump(0x0048A750, &FMODAudioClass::Sample_Copy);             // Not needed

    /**
     *  Global instance access.
     */
    //Patch_Dword(0x00407765+1, (uintptr_t)&FMODAudio);      // AHandle
    //Patch_Dword(0x0040784C+1, (uintptr_t)&FMODAudio);      // AHandle
    //Patch_Dword(0x004078CD+1, (uintptr_t)&FMODAudio);      // AHandle
    //Patch_Dword(0x004078E6+1, (uintptr_t)&FMODAudio);      // AHandle
    //Patch_Dword(0x00407A87+1, (uintptr_t)&FMODAudio);      // AHandle
    //Patch_Dword(0x00407A9E+1, (uintptr_t)&FMODAudio);      // AHandle
    //Patch_Dword(0x00407AC6+1, (uintptr_t)&FMODAudio);      // AHandle
    //Patch_Dword(0x00407ADD+1, (uintptr_t)&FMODAudio);      // AHandle
    //Patch_Dword(0x00407C7D+1, (uintptr_t)&FMODAudio);      // AHandle
    Patch_Dword(0x00462C87+1, (uintptr_t)&FMODAudio);     // Call_Back
    //Patch_Dword(0x00487990+1, (uintptr_t)&FMODAudio);           // DSAudio dyn init, not needed
    //Patch_Dword(0x004879B0+1, (uintptr_t)&FMODAudio);           // DSAudio dyn init, not needed
    //Patch_Dword(0x004892A2+1, (uintptr_t)&FMODAudio);     // Sound_Timer_Callback
    Patch_Dword(0x00554043+1, (uintptr_t)&FMODAudio);     // MapSelect
    Patch_Dword(0x00554055+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x00554236+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x0055442B+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005544BF+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x00554565+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005545E2+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005545F8+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x0055465B+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x00554668+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x0056D05A+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x0056D160+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x0056E71F+1, (uintptr_t)&FMODAudio);     // MapSelect
    Patch_Dword(0x0056E820+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x0056F3BB+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x0056F40D+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x00571F83+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x0057256C+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x00574CAB+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x00574CF2+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x00593DEF+1, (uintptr_t)&FMODAudio);     // OwnerDraw
    Patch_Dword(0x0059427C+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005BCC62+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005BCE28+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005E35B0+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005E36FC+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005E3E61+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005E442D+1, (uintptr_t)&FMODAudio);     // ScoreClass
    Patch_Dword(0x005E4B7B+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005E582B+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005E5FFC+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005E61BC+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005E67DB+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005E68A1+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x005E6BBA+1, (uintptr_t)&FMODAudio);     // ScoreFontClass
    Patch_Dword(0x005E73BB+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x006013BE+1, (uintptr_t)&FMODAudio);     // 
    Patch_Dword(0x006016A9+1, (uintptr_t)&FMODAudio);     // WinMain
    Patch_Dword(0x00601A12+1, (uintptr_t)&FMODAudio);     // WinMain
    Patch_Dword(0x00602480+1, (uintptr_t)&FMODAudio);     // Emergency_Exit
    Patch_Dword(0x00643E0D+1, (uintptr_t)&FMODAudio);     // ThemeClass
    Patch_Dword(0x00643E65+1, (uintptr_t)&FMODAudio);     // ThemeClass
    Patch_Dword(0x00643FBE+1, (uintptr_t)&FMODAudio);     // ThemeClass
    Patch_Dword(0x00643FD1+1, (uintptr_t)&FMODAudio);     // ThemeClass
//    Patch_Dword(0x0064403E+1, (uintptr_t)&FMODAudio);     // ThemeClass
//    Patch_Dword(0x006440BC+1, (uintptr_t)&FMODAudio);     // ThemeClass
    Patch_Dword(0x006441EA+1, (uintptr_t)&FMODAudio);     // ThemeClass
    Patch_Dword(0x0064420E+1, (uintptr_t)&FMODAudio);     // ThemeClass
    Patch_Dword(0x0064422D+1, (uintptr_t)&FMODAudio);     // ThemeClass
    Patch_Dword(0x00644290+1, (uintptr_t)&FMODAudio);     // ThemeClass
    Patch_Dword(0x006442E3+1, (uintptr_t)&FMODAudio);     // ThemeClass
    Patch_Dword(0x0064442C+1, (uintptr_t)&FMODAudio);     // ThemeClass
    Patch_Dword(0x00664ADE+1, (uintptr_t)&FMODAudio);     // VocClass
    Patch_Dword(0x00664B82+1, (uintptr_t)&FMODAudio);     // VocClass
    Patch_Dword(0x00664C3D+1, (uintptr_t)&FMODAudio);     // VocClass
    Patch_Dword(0x00664CF1+1, (uintptr_t)&FMODAudio);     // VocClass
    Patch_Dword(0x00664E93+1, (uintptr_t)&FMODAudio);     // VocClass
    Patch_Dword(0x00665994+1, (uintptr_t)&FMODAudio);     // Speak_AI
    Patch_Dword(0x00665AC2+1, (uintptr_t)&FMODAudio);     // Speak_AI
    Patch_Dword(0x00665B07+1, (uintptr_t)&FMODAudio);     // Stop_Speaking
    Patch_Dword(0x00665B56+1, (uintptr_t)&FMODAudio);     // Is_Speaking
    Patch_Dword(0x00665BAC+1, (uintptr_t)&FMODAudio);     // Set_Speech_Volume
    Patch_Dword(0x0067E8F6+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067F88D+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FB41+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FC35+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FC92+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FCA3+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FD0D+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FD72+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FD83+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FE57+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FE7F+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FE90+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0067FF29+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0068004C+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0068005D+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x00680089+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x006800C5+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x006800EC+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x00680187+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x006801AF+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x006801C0+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x0068031C+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x00685A21+1, (uintptr_t)&FMODAudio);     // WDT
    Patch_Dword(0x00685ECB+1, (uintptr_t)&FMODAudio);     // WDT

    Patch_Dword(0x00686326+2, (uintptr_t)&FMODAudio.AudioFocusLossFunction);  // Create_Main_Window

    Patch_Dword(0x00644080+2, (uintptr_t)&FMODAudio.StreamLowImpact);     // ThemeClass
    Patch_Dword(0x006440C8+2, (uintptr_t)&FMODAudio.StreamLowImpact);     // ThemeClass

    //Patch_Dword(0x00407673+1, (uintptr_t)&FMODAudio.FMODDevice);      // AHandle
    //Patch_Dword(0x0040776F+1, (uintptr_t)&FMODAudio.FMODDevice);      // AHandle
    Patch_Dword(0x00462C6C+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x004AAACB+2, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x004E4615+2, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x00508ACD+1, (uintptr_t)&FMODAudio.FMODDevice);     // Main_Loop
    Patch_Dword(0x0050A758+2, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x0056419F+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x0056E646+2, (uintptr_t)&FMODAudio.FMODDevice);     // MapSelect
    Patch_Dword(0x0056E990+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x0056F296+2, (uintptr_t)&FMODAudio.FMODDevice);     // MSSfxEntry
    Patch_Dword(0x0057146E+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x005721B7+2, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x00574BF8+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x00594261+1, (uintptr_t)&FMODAudio.FMODDevice);     // OwnerDraw
    Patch_Dword(0x005E3552+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x005E369E+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x005E3E03+1, (uintptr_t)&FMODAudio.FMODDevice);     // ScoreClass
    Patch_Dword(0x005E43CF+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x005E57BB+1, (uintptr_t)&FMODAudio.FMODDevice);     // ScoreClass
    Patch_Dword(0x005E5F8B+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x005E615E+1, (uintptr_t)&FMODAudio.FMODDevice);     // ScoreClass
    Patch_Dword(0x005E677E+1, (uintptr_t)&FMODAudio.FMODDevice);     // ScoreFontClass
    Patch_Dword(0x005E68D0+1, (uintptr_t)&FMODAudio.FMODDevice);     // ScoreFontClass
    Patch_Dword(0x005E7308+1, (uintptr_t)&FMODAudio.FMODDevice);     // ScoreSoundClass
    Patch_Dword(0x005FC531+1, (uintptr_t)&FMODAudio.FMODDevice);     // SoundControlsClass
    Patch_Dword(0x00643C87+1, (uintptr_t)&FMODAudio.FMODDevice);     // ThemeClass
    Patch_Dword(0x00643DC0+1, (uintptr_t)&FMODAudio.FMODDevice);     // ThemeClass
    Patch_Dword(0x00643F30+1, (uintptr_t)&FMODAudio.FMODDevice);     // ThemeClass
    Patch_Dword(0x00643F93+1, (uintptr_t)&FMODAudio.FMODDevice);     // ThemeClass
    Patch_Dword(0x00643FF2+1, (uintptr_t)&FMODAudio.FMODDevice);     // ThemeClass
    Patch_Dword(0x006441A0+2, (uintptr_t)&FMODAudio.FMODDevice);     // ThemeClass
    Patch_Dword(0x0064425C+1, (uintptr_t)&FMODAudio.FMODDevice);     // ThemeClass
    Patch_Dword(0x006442B9+1, (uintptr_t)&FMODAudio.FMODDevice);     // ThemeClass
    Patch_Dword(0x00664A7E+1, (uintptr_t)&FMODAudio.FMODDevice);     // VocClass
    Patch_Dword(0x00664B28+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x00664BDD+1, (uintptr_t)&FMODAudio.FMODDevice);     // VocClass
    Patch_Dword(0x00664C97+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x00664E33+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x00665823+1, (uintptr_t)&FMODAudio.FMODDevice);     // Speak
    Patch_Dword(0x00665955+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x00665B2E+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x00665B8C+1, (uintptr_t)&FMODAudio.FMODDevice);     // Set_Speech_Volume
    Patch_Dword(0x0067F77C+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x0067FA0C+1, (uintptr_t)&FMODAudio.FMODDevice);     // 
    Patch_Dword(0x00685A0A+1, (uintptr_t)&FMODAudio.FMODDevice);     // Main_Window_Procedure
    Patch_Dword(0x00685EB9+1, (uintptr_t)&FMODAudio.FMODDevice);     // Main_Window_Procedure

// MIGHT NEED NEW AHANDLE DUE TO INTERFACE CHANGE??
//    Patch_Dword(0x004077F2+1, (uintptr_t)&FMODAudio.PrimaryBufferPtr);      // AHandle
//    Patch_Dword(0x004078A5+1, (uintptr_t)&FMODAudio.PrimaryBufferPtr);      // AHandle
//    Patch_Dword(0x00407AB2+1, (uintptr_t)&FMODAudio.PrimaryBufferPtr);      // AHandle

    Patch_Dword(0x00407683+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00462C75+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x004AAAD5+2, (uintptr_t)&FMODAudio.AudioDone);     // GameSettingsClass_
    Patch_Dword(0x004E461D+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00508AD6+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x0050A762+2, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005641AC+2, (uintptr_t)&FMODAudio.AudioDone);     // MovieClass
    Patch_Dword(0x0056E652+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x0056E99F+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x0056F2A2+2, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00571477+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005721C3+2, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00574C05+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x0059426A+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005E355B+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005E36A7+1, (uintptr_t)&FMODAudio.AudioDone);     // ScoreClass
    Patch_Dword(0x005E3E0C+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005E43D8+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005E57C4+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005E5F98+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005E6167+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005E6787+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005E68E0+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005E7315+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x005FC541+1, (uintptr_t)&FMODAudio.AudioDone);     // SoundControlsClass
    Patch_Dword(0x00643C94+1, (uintptr_t)&FMODAudio.AudioDone);     // ThemeClass
    Patch_Dword(0x00643DD0+2, (uintptr_t)&FMODAudio.AudioDone);     // ThemeClass
    Patch_Dword(0x00643F3D+1, (uintptr_t)&FMODAudio.AudioDone);     // ThemeClass
    Patch_Dword(0x00643F9C+1, (uintptr_t)&FMODAudio.AudioDone);     // ThemeClass
    Patch_Dword(0x00643FFF+2, (uintptr_t)&FMODAudio.AudioDone);     // ThemeClass
    Patch_Dword(0x006441AE+2, (uintptr_t)&FMODAudio.AudioDone);     // ThemeClass
    Patch_Dword(0x00644265+1, (uintptr_t)&FMODAudio.AudioDone);     // ThemeClass
    Patch_Dword(0x006442C2+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00664A87+1, (uintptr_t)&FMODAudio.AudioDone);     // VocClass
    Patch_Dword(0x00664B31+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00664BE6+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00664CA0+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00664E3C+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00665830+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00665962+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00665B37+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00665B95+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x0067F789+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x0067FA19+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00685A16+1, (uintptr_t)&FMODAudio.AudioDone);     // 
    Patch_Dword(0x00685EC2+1, (uintptr_t)&FMODAudio.AudioDone);     // 

#if 0
    // Only these two need hooking.
    Patch_Dword(0x0066B464+6, (uintptr_t)&FMOD_AHandle::func_4072D0);
    Patch_Dword(0x0066B48D+6, (uintptr_t)&FMOD_AHandle::Stream_Handler);

    //Patch_Jump(0x00407650, &FMOD_AHandle::func_407650);     // Not needed
    //Patch_Jump(0x00407980, &FMOD_AHandle::func_407980);     // Not needed
    //Patch_Jump(0x00407B20, &FMOD_AHandle::func_407B20);     // Not needed
    //Patch_Jump(0x00407D30, &FMOD_AHandle::func_407D30);     // Not needed
    //Patch_Jump(0x00407EF0, &FMOD_AHandle::func_407EF0);     // Not needed
    //Patch_Jump(0x00407F40, &FMOD_AHandle::func_407F40);     // Not needed
    //Patch_Jump(0x00407FE0, &FMOD_AHandle::Stop_Audio_Handler);     // Not needed
    //Patch_Jump(0x00408060, &FMOD_AHandle::Sound_Timer_Callback);   // Not needed
    //Patch_Jump(0x00408200, &FMOD_AHandle::func_408200);     // no need to patch, handles timer fine.
    //Patch_Jump(0x00408210, &FMOD_AHandle::func_408210);     // Not needed
    //Patch_Jump(0x00408260, &FMOD_AHandle::func_408260);     // Not needed
    //Patch_Jump(0x004082B0, &FMOD_AHandle::func_4082B0);     // Not needed
    //Patch_Jump(0x004082C0, &FMOD_AHandle::func_4082C0);     // Not needed
#endif

#endif
}
