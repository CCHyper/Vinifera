/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          FMOD_AUDIO.CPP
 *
 *  @author        OmniBlade, CCHyper
 *
 *  @contributors  tomsons26
 *
 *  @brief         Main header file for the FMOD audio interface.
 *
 *  @license       ProjectTSYR is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 ProjectTSYR is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "fmod_audio.h"
#include "fmod_load_dll.h"
#include "fmod_globals.h"
#include "fmod_util.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "ccfile.h"
#include "ini.h"
#include "wwaud.h"
#include "soscodec.h"
#include "dsaudio.h"
#include "endiantype.h"
#include "asserthandler.h"
#include "debughandler.h"
#include <string>


/**
 *  Global instance of the new FMOD audio engine.
 */
FMODAudioClass FMODAudio;


/**
 *  Audio engine settings.
 */
static unsigned Audio_NumSampleTrackers = 16;


/**
 *  
 */
static HWND Audio_hWnd = nullptr;
static int Audio_BitsPerSample = 16;
static bool Audio_IsStereo = false;
static int Audio_Rate = 22050;



/**
 *  x
 * 
 *  @author: CCHyper
 */
FMODAudioClass::FMODAudioClass()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
FMODAudioClass::~FMODAudioClass()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool FMODAudioClass::Init(HWND hWnd, int bits_per_sample, bool stereo, int rate)
{
    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::End()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Stop_Sample(int handle)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool FMODAudioClass::Sample_Status(int handle)
{
    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool FMODAudioClass::Is_Sample_Playing(const void *sample)
{
    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Stop_Sample_Playing(const void *sample)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODAudioClass::Get_Free_Sample_Handle(int priority)
{
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODAudioClass::Play_Sample(const void *sample, int priority, int volume)
{
    return Play_Sample_Handle(sample, priority, volume, Get_Free_Sample_Handle(priority));
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODAudioClass::File_Play_Sample(const char *filename, int priority, int volume)
{
    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODAudioClass::Play_Sample_Handle(const void *sample, int priority, int volume, int handle)
{
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Sound_Maintenance_Callback()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODAudioClass::Stream_Sample_Vol(void *buffer, int size, bool(*callbackptr)(short, short*, void**, int*), int volume, int handle)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::File_Stream_Preload(int handle)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODAudioClass::File_Stream_Sample_Vol(const char *filename, int volume, bool real_time_start)
{
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Sound_Callback()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool FMODAudioClass::File_Callback(short handle, short *odd, void **buffer, int *size)
{
    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Set_Volume_All(int volume)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODAudioClass::Set_Volume_Percent_All(int vol_percent)
{
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Set_Handle_Volume(int handle, int volume)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Set_Sample_Volume(const void *sample, int volume)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Fade_Sample(int handle, int ticks)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODAudioClass::Get_Playing_Sample_Handle(const void *sample)
{
    return INVALID_AUDIO_HANDLE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Set_Handle_Loop(int handle, bool loop)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Set_Sample_Loop(const void *sample, bool loop)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Stop_Handle_Looping(int handle)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Stop_Sample_Looping(const void* sample)
{
}


void FMODAudioClass::Set_Score_Handle_Pause(int handle)
{
}


void FMODAudioClass::Set_Score_Handle_Resume(int handle)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Set_Handle_Pitch(int handle, float pitch)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Set_Sample_Pitch(const void *sample, float pitch)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Set_Handle_Pan(int handle, float pan)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Set_Sample_Pan(const void *sample, float pan)
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODAudioClass::Set_Sound_Vol(int volume)
{
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODAudioClass::Set_Score_Vol(int volume)
{
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
long FMODAudioClass::Sample_Length(const void *sample)
{
    return 0;
};


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool FMODAudioClass::Start_Primary_Sound_Buffer(bool forced)
{
    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODAudioClass::Stop_Primary_Sound_Buffer()
{
}


/**
 *  x
 * 
 *  @author: CCHyper, tomsons26
 */
int FMODAudioClass::Sample_Copy(SampleTrackerType *st, void **source, int *src_size, void **alternate, int *alt_size, void *dest, int dst_size, int scomp)
{
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper, tomsons26
 */
int FMODAudioClass::Simple_Copy(void **source, int *src_size, void **alternate, int *alt_size, void **dest, int dst_size)
{
    return 0;
}


/**
 *  Returns a handle to the requested file.
 * 
 *  @author: CCHyper
 */
FileClass *FMODAudioClass::Get_File_Handle(const char *filename)
{
    return nullptr;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool FMODAudioClass::Load_Settings(INIClass &ini)
{
    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool FMODAudioClass::Save_Settings(INIClass &ini)
{
    return true;
}
