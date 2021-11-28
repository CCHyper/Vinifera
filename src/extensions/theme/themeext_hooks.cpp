/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          THEMEEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended ThemeClass.
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
#include "themeext_hooks.h"
#include "theme.h"
#include "tibsun_inline.h"
#include "tibsun_globals.h"
#include "vinifera_globals.h"
#include "ccfile.h"
#include "ccini.h"
#include "dsaudio.h"
#include "debughandler.h"
#include <string>

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or deconstructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
static class ThemeClassFake final : public ThemeClass
{
    public:
        void _Scan();
        int _Play_Song(ThemeType theme);
        const char *_Theme_File_Name_Ext(ThemeType theme, const char *ext);
};


/**
 *  Reimplementation of ThemeClass::Play_Song().
 * 
 *  @author: CCHyper
 */
int ThemeClassFake::_Play_Song(ThemeType theme)
{
    if (ScoresPresent && FMODAudio.Is_Available() && !Debug_Quiet) {
        Stop();
        if (theme != THEME_NONE && theme != THEME_QUIET) {
            if (Volume > 0) {
                Score = theme;
                FMODAudio.StreamLowImpact = true;

                /**
                 *  First attempt to play the Ogg file, then attempt to play the AUD.
                 */
                int handle = INVALID_AUDIO_HANDLE;
//                handle = FMODAudio.File_Stream_Sample_Vol(_Theme_File_Name_Ext(theme, ".OGG"), Volume, true);
//                if (handle == INVALID_AUDIO_HANDLE) {
                    handle = FMODAudio.File_Stream_Sample_Vol(_Theme_File_Name_Ext(theme, ".AUD"), Volume, true);
                    //handle = Audio.File_Stream_Sample_Vol(_Theme_File_Name_Ext(theme, ".AUD"), Volume, true);
//                }
                Current = handle;

                FMODAudio.StreamLowImpact = false;
                DEBUG_GAME("Theme::Play_Song(%d) - %s\n", Score, (IsRepeat || Themes[theme]->Repeat ? "Repeating" : "Playing"));
                if (IsRepeat || Themes[theme]->Repeat) {
                    Pending = theme;
                }
            } else {
                Pending = theme;
            }
        }
    }
    return Current;
}


/**
 *  Reimplementation of ThemeClass::Scan().
 * 
 *  @author: CCHyper
 */
void ThemeClassFake::_Scan()
{
    if (FMODAudio.Is_Available() && !Debug_Quiet && Themes.Count() > 0) {
        for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {

            const char *name = _Theme_File_Name_Ext(theme, ".OGG");

            const char *music_envvar_dir_filename = (std::string(Vinifera_MusicPath_EnvVar) + std::string(name)).c_str();
            const char *music_dir_filename = (std::string(Vinifera_MusicPath) + std::string(name)).c_str();

#if 0
            if (RawFileClass(music_envvar_dir_filename).Is_Available()) {
                Themes[theme]->Available = true;

            } else if (RawFileClass(music_dir_filename).Is_Available()) {
                Themes[theme]->Available = true;

            } else if (CCFileClass(name).Is_Available()) {
                Themes[theme]->Available = true;

            } else {
                Themes[theme]->Available = false;
            }

            /**
             *  We found the theme in Ogg format.
             */
            if (Themes[theme]->Available) {
#ifndef NDEBUG
                DEV_DEBUG_INFO("Theme: Found Ogg for \"%s\".\n", Themes[theme]->Name);
#endif
                continue;
            }
#endif

            name = _Theme_File_Name_Ext(theme, ".AUD");

            music_envvar_dir_filename = (std::string(Vinifera_MusicPath_EnvVar) + std::string(name)).c_str();
            music_dir_filename = (std::string(Vinifera_MusicPath) + std::string(name)).c_str();

            if (RawFileClass(music_envvar_dir_filename).Is_Available()) {
                Themes[theme]->Available = true;

            } else if (RawFileClass(music_dir_filename).Is_Available()) {
                Themes[theme]->Available = true;

            } else if (CCFileClass(name).Is_Available()) {
                Themes[theme]->Available = true;

            } else {
                Themes[theme]->Available = false;
            }
        }
    }
}


/**
 *  Reimplementation of ThemeClass::Theme_File_Name() that takes file extension.
 * 
 *  @author: CCHyper
 */
const char *ThemeClassFake::_Theme_File_Name_Ext(ThemeType theme, const char *ext)
{
    static char name[_MAX_FNAME+_MAX_EXT];

    if (theme >= THEME_FIRST && theme < Themes.Count()) {
        _makepath(name, nullptr, nullptr, Themes[theme]->Name, ext);
        return (const char *)(&name[0]);
    }

    return "";
}


/**
 *  Main function for patching the hooks.
 */
void ThemeClassExtension_Hooks()
{
    Patch_Jump(0x00643C70, &ThemeClassFake::_Scan);
    //Patch_Jump(0x00643FE0, &ThemeClassFake::_Play_Song);
}
