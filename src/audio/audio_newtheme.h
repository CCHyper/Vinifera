/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          THEME.H
 *
 *  @author        Joe L. Bostic (see notes below)
 *
 *  @contributors  CCHyper, tomsons26
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
 *  @note          This file contains heavily modified code from the source code
 *                 released by Electronic Arts for the C&C Remastered Collection
 *                 under the GPL3 license. Source:
 *                 https://github.com/ElectronicArts/CnC_Remastered_Collection
 *
 ******************************************************************************/
#pragma once

#include "always.h"
#include "tibsun_defines.h"
#include "vector.h"


class CCINIClass;


/**
 *  A reimplementation of ThemeClass to use the new audio driver interface.
 */
class NewThemeClass
{
    public:
        NewThemeClass();
        ~NewThemeClass();

        ThemeType From_Name(const char * name) const;
        ThemeType Next_Song(ThemeType index) const;
        ThemeType What_Is_Playing() const { return Score; }
        bool Is_Allowed(ThemeType index) const;
        bool Is_Regular(ThemeType theme) const;
        bool Is_Playable(ThemeType theme) const;
        const char * Base_Name(ThemeType index) const;
        const char * INI_Base_Name(ThemeType index) const;
        const char * Full_Name(ThemeType index) const;

        int Max_Themes() const { return Themes.Count(); }

        bool Play_Song(ThemeType index);
        bool Still_Playing() const;
        int Track_Length(ThemeType index) const;
        void AI();
        void Fade_Out();
        void Queue_Song(ThemeType index);
        void Stop(bool fade = false);
        void Suspend(bool fade = false);

        void Pause();
        void UnPause();
        bool Is_Paused() const { return IsPaused; }

        void Clear();
        void Set_Volume(int volume);

        int Process(CCINIClass & ini);

        void Set_Theme_Data(ThemeType theme, int scenario, SideType owners);

        void Set_Shuffle(int on) { IsRepeat = on; }
        void Set_Repeat(int on) { IsShuffle = on; }

        void Scan();

    //private:
        const char * Theme_File_Name(ThemeType theme);

    //private:
        typedef struct ThemeControl
        {
            ThemeControl() :
                Scenario(0),
                Duration(0.0),
                Normal(true),
                Repeat(false),
                Available(false),
                Owner(SIDE_NONE),
                RequiredAddon(ADDON_NONE),
                Volume(1.0f)
            {
                Name[0] = '\0';
                Sound[0] = '\0';
                Fullname[0] = '\0';
            }

            ~ThemeControl() {}

            bool Fill_In(CCINIClass & ini);

            /**
             *  Filename of score.
             */
            char Name[256];

            /**
             *  Text number for full score name.
             */
            char Fullname[64];

            /**
             *  Scenario when it first becomes available.
             */
            int Scenario;

            /**
             *  Duration of theme in seconds.
             */
            float Duration;

            /**
             *  Allowed in normal game play?
             */
            bool Normal;

            /**
             *  Always repeat this score?
             */
            bool Repeat;

            /**
             *  Is the score available?
             */
            bool Available;

            /**
             *  What houses are allowed to play this theme (bit field)?
             */
            SideType Owner;

            /**
             *  The addon required to be active for this theme to be available.
             */
            AddonType RequiredAddon;

            /**
             *  #NEW: User defined filename.
             */
            char Sound[256];
            
            /**
             *  #NEW: Volume control for this theme.
             */
            float Volume;

        } ThemeControl;

        /**
         *  Handle to current score.
         */
        int Current;            // #OBSOLETE, but required here to maintain ABI.

        /**
         *  Score number currently being played.
         */
        ThemeType Score;

        /**
         *  Score to play next.
         */
        ThemeType Pending;

        /**
         *  Volume for scores.
         */
        int Volume;
        
        /**
         *  Score should repeat?
         */
        bool IsRepeat;

        /**
         *  Score list should shuffle?
         */
        bool IsShuffle;

        bool IsPlaying;

        bool IsPaused;
        
        /**
         *  x
         */
        DynamicVectorClass<ThemeControl *> Themes;
};
