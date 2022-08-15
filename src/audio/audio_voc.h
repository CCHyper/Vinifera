/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          AUDIO_NEWVOC.H
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

#ifdef USE_FMOD_AUDIO

#include "always.h"
#include "tibsun_defines.h"
#include "audio_manager.h"
#include "voc.h"
#include "wstring.h"


class CCINIClass;


class FMODVocClass
{
    public:
        FMODVocClass(const char *filename);
        ~FMODVocClass();

        void Read_INI(CCINIClass &ini);
        bool Can_Play() const;

        int Play(float volume, int a2);
        int Play(float volume);
        int Play(Coordinate &coord);

        static int Play(VocType voc, int a2, float volume = 1.0f);
        static int Play(VocType voc, float volume = 1.0f);
        static int Play(VocType voc, Coordinate &coord);

        static void Process(CCINIClass &ini);
        static void Clear();

        static VocType VocType_From_Voc(VocClass *voc);
        static VocType From_Name(const char *name);
        static VocClass *Voc_From_Name(const char *name);
        static const char *INI_Name_From(VocType type);

        static const VocClass *As_Pointer(VocType type)
        {
            return type != VOC_NONE && type < Vocs.Count() ? Vocs[type] : nullptr;
        }

    private:
        /**
         *  x
         */
        Wstring Name;

        /**
         *  #NEW: x
         */
        Wstring Sound;

        /**
         *  #NEW: x
         */
        Wstring FileName;
        
        /**
         *  x
         */
        int Priority;
        
        /**
         *  x
         */
        float Volume;
        
        /**
         *  #NEW: x
         */
        float Pitch;
        
        /**
         *  #NEW: x
         */
        AudioManagerClass::DSPFlagType Effects;
};


#endif // USE_FMOD_AUDIO
