/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          AUDIO_NEWVOX.CPP
 *
 *  @author        CCHyper, tomsons26
 *
 *  @brief         
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
#include "fmod_ahandle.h"

#ifdef USE_FMOD_AUDIO

#include "audio_manager.h"
#include "vqa.h"
#include "stimer.h"
#include "debughandler.h"


/**
 *  name
 */
static class AHandleTimerClass
{
    public:
        AHandleTimerClass() {}
        ~AHandleTimerClass() {}

        

    private:
};


/**
 *  
 */
static FMODAHandleClass FMODAHandle; 


/**
 *  name
 * 
 *  desc
 */
unsigned int __cdecl FMODAHandleClass::Timer_Handler(VQAHandle *handle)
{
    return 0;
}


/**
 *  name
 * 
 *  desc
 */
VQAErrorType __cdecl FMODAHandleClass::Stream_Handler(VQAHandle *handle, int action, void *buffer, int nbytes)
{
    VQAErrorType error = VQAERR_NONE;

    switch (action) {
        
        case 1: // ?
        {
            DEBUG_INFO("FMODAHandle: Stream_Handler case 1\n");
            handle->Config.TimerHandler = (void *)Get_System_Time();
            handle->Config.RefreshRate = 60;
            break;
        }
        
        case 2: // Open?
        {
            DEBUG_INFO("FMODAHandle: Stream_Handler case 2\n");
            DEBUG_INFO("FMODAHandle: Opening VQ audio handler\n");
            DEBUG_INFO("FMODAHandle: Current thread ID is %08x\n", GetCurrentThreadId());

            if (AudioManager.Is_Available()) {
                
                // TODO

            } else {
                error = VQAERR_AUDIO;
            }

            break;
        }
        
        case 3: // Close?
        {
            DEBUG_INFO("FMODAHandle: Stream_Handler case 3\n");
            break;
        }
        
        case 4: // Open again?
        {
            DEBUG_INFO("FMODAHandle: Stream_Handler case 4\n");

            if (AudioManager.Is_Available()) {
                
                // TODO

            } else {
                error = VQAERR_AUDIO;
            }

            break;
        }
        
        case 5: // Copy?
        {
            DEBUG_INFO("FMODAHandle: Stream_Handler case 5\n");
            break;
        }
        
        case 6: // Stop audio buffer?
        {
            DEBUG_INFO("FMODAHandle: Stream_Handler case 6\n");
            break;
        }
        
        case 7: // Stop and release audio buffer?
        {
            DEBUG_INFO("FMODAHandle: Stream_Handler case 7\n");
            break;
        }
        
        case 8: // Play and pause?
        {
            DEBUG_INFO("FMODAHandle: Stream_Handler case 8\n");
            break;
        }
        
        default:
            break;

    };

    return error;
}


/**
 *  name
 * 
 *  desc
 */
unsigned int __cdecl FMODAHandleClass::Get_System_Time()
{
    static SystemTimerClass _timer;

    DEBUG_INFO("FMODAHandle: Get_System_Time()\n");

    return _timer();
}


/**
 *  name
 * 
 *  desc
 */
BOOL __cdecl FMODAHandleClass::Open_Audio()
{
    DEBUG_INFO("FMODAHandle: Open_Audio()\n");

    return TRUE;
}


/**
 *  name
 * 
 *  desc
 */
BOOL __cdecl FMODAHandleClass::Close_Audio()
{
    DEBUG_INFO("FMODAHandle: Close_Audio()\n");

    return TRUE;
}

#endif
