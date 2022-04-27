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
#include "tibsun_defines.h"
#include "wstring.h"


/**
 *  A reimplementation of Vox interface for use with the new audio driver interface.
 */
class VoxClass
{
    public:
        VoxClass(Wstring filename);
        ~VoxClass();

        static void Init();

        static void Preload();

        static void Speak(VoxType voice, bool force = false);
        static void Speak_AI();
        static void Stop_Speaking();
        static bool Is_Speaking();
        static void Set_Speech_Volume(int vol);

    private:
        /**
         *  
         */
        Wstring Filename;
        
        /**
         *  
         */
        Wstring Sound;
        
        /**
         *  
         */
        float Volume;

    private:
        /**
         *  
         */
        static Wstring SpeechBuffer[2];

        /**
         *  Current speech index.
         */
        static int SpeechBufferIndex;
};
