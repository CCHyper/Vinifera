/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          AUDIO_NEWVOX.H
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
#pragma once

#include "always.h"


#ifdef USE_FMOD_AUDIO

#include "vqa.h"


class FMODAHandleClass
{
    public:
        FMODAHandleClass() {}
        ~FMODAHandleClass() {}

        static unsigned int __cdecl Timer_Handler(VQAHandle *handle);
        static VQAErrorType __cdecl Stream_Handler(VQAHandle *handle, int action, void *buffer, int nbytes);

        static unsigned int __cdecl Get_System_Time();

        static BOOL __cdecl Open_Audio();
        static BOOL __cdecl Close_Audio();

    private:
        //ISoundInstance *Sound;
        unsigned int Time;
};

#endif
