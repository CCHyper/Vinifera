/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_IONSTORM.H
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
 *  @note          This file contains heavily modified code from the source code
 *                 released by Electronic Arts for the C&C Remastered Collection
 *                 under the GPL3 license. Source:
 *                 https://github.com/ElectronicArts/CnC_Remastered_Collection
 *
 ******************************************************************************/
#pragma once

#ifdef USE_MINIAUDIO

#include "always.h"
#include "audio_handle.h"


/**
 *  x
 */
class AudioAmbientClass
{
    public:
        AudioAmbientClass();
        ~AudioAmbientClass();

        bool Start();
        bool Stop();
        bool Is_Playing();

    private:
        /**
         *  Handle to ion storm ambient.
         */
        AudioHandleClass *Handle;

        /**
         *  The file type of this sound.
         */
        AudioFileType FileType;

        /**
         *  Full filename of the sound.
         */
        Wstring FileName;

        /**
         *  Is the sound available?
         */
        bool IsAvailable;

        /**
         *  Volume control for this sound.
         */
        float Volume;
};


extern AudioAmbientClass AudioAmbient;


#endif // USE_MINIAUDIO
