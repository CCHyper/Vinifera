/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SETUP_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the main function that sets up all hooks.
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
#include "setup_hooks.h"

/**
 *  Include the hook headers here.
 */
#include "vinifera_newdel.h"
#include "crt_hooks.h"
#include "debug_hooks.h"
#include "vinifera_hooks.h"
#include "ext_hooks.h"
#include "bugfix_hooks.h"
#include "cncnet4_hooks.h"
#include "cncnet5_hooks.h"




//static const char *ThemeSubDirectory = "MUSIC/";
//static const char *ThemeSubDirectory = "C:/Users/CCHyper/Desktop/TS_HD_Music/";
static const char *ThemeSubDirectory = "Z:/Google Drive/External_Share/TibSun_HD/MUSIC/";

#include "theme.h"
#include "ccfile.h"
#include "tibsun_globals.h"
#include "dsaudio.h"
#include "debughandler.h"
#include <string>
#include "hooker.h"
#include "hooker_macros.h"
static class ThemeClassFake : public ThemeClass
{
    public:
        void New_Scan();
        int New_Play_Song(ThemeType index);
        //const char *New_Theme_File_Name(ThemeType theme);
};


void ThemeClassFake::New_Scan()
{
    if (Audio.Is_Available() && !Debug_Quiet && Themes.Count() > 0) {
        for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {

            const char *name = Theme_File_Name(theme);

            const char *music_dir_filename = (std::string(ThemeSubDirectory) + std::string(name)).c_str();
            if (RawFileClass(music_dir_filename).Is_Available()) {
                Themes[theme]->Available = true;

            } else if (CCFileClass(name).Is_Available()) {
                Themes[theme]->Available = true;

            } else {
                Themes[theme]->Available = false;
            }
        }
    }
}

int ThemeClassFake::New_Play_Song(ThemeType index)
{
    if (Audio.Is_Available() && !Debug_Quiet && Themes.Count() > 0) {
        Stop();
        if (index != THEME_NONE && index != THEME_QUIET) {
            if (Volume > 0) {
                Score = index;
                Audio.StreamLowImpact = true;
                Current = Audio.File_Stream_Sample_Vol(Theme_File_Name(index), Volume, true);
                Audio.StreamLowImpact = false;
                DEBUG_INFO("Theme::PlaySong(%d) - %s\n", Score, IsRepeat && Themes[index]->Repeat ? "Repeating" : "Playing");
                if (IsRepeat || Themes[index]->Repeat) {
                    Pending = index;
                }
            } else {
                Pending = index;
            }
        }
    }
    return Current;
}

/*const char *ThemeClassFake::New_Theme_File_Name(ThemeType theme)
{
    static char name[_MAX_FNAME+_MAX_EXT];

    if ((unsigned)theme < (unsigned)Themes.Count()) {
        std::snprintf(name, sizeof(name), "%s.AUD", Themes[theme]->Name);
        return &name[0];
    }

    return "";
}*/


#include "ccfile.h"
#include "tibsun_globals.h"
#include "dsaudio.h"
#include "debughandler.h"
#include <string>
#include "hooker.h"
#include "hooker_macros.h"
FileClass *Get_Streaming_File_Ptr(const char *filename)
{
    const char *music_dir_filename = (std::string(ThemeSubDirectory) + std::string(filename)).c_str();
    if (RawFileClass(music_dir_filename).Is_Available()) {
        DEBUG_INFO("Audio: File \"%s\" loaded from MUSIC directory.\n", filename);
        return new RawFileClass(music_dir_filename);

    } else if (CCFileClass(music_dir_filename).Is_Available()) {
        return new CCFileClass(music_dir_filename);

    } else {
        return nullptr;
    }
}

DECLARE_PATCH(_DSAudio_File_Stream_Sample_Vol_Path_Patch)
{
    GET_REGISTER_STATIC(const char *, filename, ebp);
    static FileClass *file;

    file = Get_Streaming_File_Ptr(filename);

    _asm { mov ebp, file };

    JMP(0x00489906);
}



DECLARE_PATCH(_Init_Game_Theme_INI_Patch)
{
}






static void HD_Audio_Patches()
{
    Patch_Jump(0x00643C70, &ThemeClassFake::New_Scan);
    Patch_Jump(0x00643FE0, &ThemeClassFake::New_Play_Song);

    Patch_Jump(0x004898EA, &_DSAudio_File_Stream_Sample_Vol_Path_Patch);

    Patch_Jump(0x004E09D5, &);
    Patch_Jump(0x004E0A90, &);
}






void Setup_Hooks()
{
    Vinifera_Memory_Hooks();

    CRT_Hooks();
    Debug_Hooks();
    //Vinifera_Hooks();
    Extension_Hooks();
    BugFix_Hooks();

    //CnCNet4_Hooks();
    //CnCNet5_Hooks();

    HD_Audio_Patches();
}
