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
#include "tibsun_globals.h"
#include "ccini.h"
#include "wstring.h"
#include "audio_driver.h"
#include "audio_util.h"
#include "debughandler.h"

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
 *  Reimplementation of ThemeClass::Scan().
 * 
 *  @author: CCHyper
 */
void ThemeClassFake::_Scan()
{
    if (Audio_Driver()->Is_Available() && !Debug_Quiet && Themes.Count() > 0) {
        for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {

            ThemeControl *tctrl = Themes[theme];

            /**
             *  Check to see if the theme exists in known formats.
             *  Priority: OGG -> MP3 -> WAV-> AUD
             */
            Wstring fname;

            if (!Audio_Driver_Is_Direct_Sound()) {

                fname = _Theme_File_Name_Ext(theme, "xWAV"); // Wav disabled for now.
                tctrl->Available = Is_Music_File_Available(fname.Peek_Buffer());
                if (tctrl->Available) {
                    DEV_DEBUG_INFO("Theme: Found Wav for \"%s\".\n", tctrl->Name);
                    continue;
                }

            }

            fname = _Theme_File_Name_Ext(theme, "AUD");
            Themes[theme]->Available = Is_Music_File_Available(fname.Peek_Buffer());
        }
    }
}


/**
 *  Reimplementation of ThemeClass::Play_Song().
 * 
 *  @author: CCHyper
 */
int ThemeClassFake::_Play_Song(ThemeType theme)
{
    if (ScoresPresent && Audio_Driver()->Is_Available() && !Debug_Quiet) {

        /**
         *  Stop any theme currently playing in a abrupt manner.
         */
        Stop();

        if (theme != THEME_NONE && theme != THEME_QUIET) {
            if (Volume > 0) {
                Score = theme;

                /**
                 *  Attempt to play the theme in supported formats.
                 *  Priority: OGG -> MP3 -> WAV-> AUD
                 */
                Wstring fname;
                bool found = false;

                if (!Audio_Driver_Is_Direct_Sound()) {

                    if (!found) {
                        fname = _Theme_File_Name_Ext(theme, "xWAV"); // Wav disabled for now.
                        if (Is_Music_File_Available(fname.Peek_Buffer())) {
                            found = true;
                        }
                    }

                }

                if (!found) {
                    fname = _Theme_File_Name_Ext(theme, "AUD");
                    if (Is_Music_File_Available(fname.Peek_Buffer())) {
                        found = true;
                    }
                }

                if (found) {
                    Audio_Driver()->Set_Stream_Low_Impact(true);
                    Current = Audio_Driver()->Play_Music(fname, VOLUME_MAX, true); //Current = Audio_Driver()->Stream_File(fname, VOLUME_MAX, true);
                    Audio_Driver()->Set_Stream_Low_Impact(false);

                    ThemeControl *tctrl = Themes[theme];

                    DEBUG_INFO("Theme::Play_Song(%d:%s) - %s\n", Score, tctrl->Name, (IsRepeat || tctrl->Repeat ? "Repeating" : "Playing"));

                    if (IsRepeat || tctrl->Repeat) {
                        Pending = theme;
                    }
                }

            } else {
                Pending = theme;
            }
        }
    }
    return Current;
}


/**
 *  Reimplementation of ThemeClass::Theme_File_Name() that takes file extension.
 * 
 *  @author: CCHyper
 */
const char *ThemeClassFake::_Theme_File_Name_Ext(ThemeType theme, const char *ext)
{
    static char _buffer[_MAX_FNAME+_MAX_EXT];

    if (theme >= THEME_FIRST && theme < Themes.Count()) {
        std::snprintf(_buffer, sizeof(_buffer), "%s.%s", Themes[theme]->Name, ext);
        return (const char *)&_buffer;
    }

    return "";
}


/**
 *  Main function for patching the hooks.
 */
void ThemeClassExtension_Hooks()
{
    //Patch_Jump(0x00643C70, &ThemeClassFake::_Scan);
    //Patch_Jump(0x00643FE0, &ThemeClassFake::_Play_Song);
}
