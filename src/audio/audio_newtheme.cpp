/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          THEME.CPP
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
#include "audio_newtheme.h"
#include "tibsun_inline.h"
#include "tibsun_globals.h"
#include "session.h"
#include "house.h"
#include "housetype.h"
#include "ccfile.h"
#include "ccini.h"
#include "addon.h"
#include "themeext.h"
#include "audio_driver.h"
#include "debughandler.h"


/**
 *  Instance of the new theme engine (extern where needed).
 */
NewThemeClass NewTheme;


/**
 *  Default constructor for the theme manager class.
 */
NewThemeClass::NewThemeClass() :
    Score(THEME_NONE),
    Pending(THEME_NONE),
    Volume(255),
    IsRepeat(false),
    IsShuffle(false),
    IsPlaying(false),
    Themes()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
NewThemeClass::~NewThemeClass()
{
    Clear();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool NewThemeClass::Is_Playable(ThemeType theme) const
{
    if (theme != THEME_QUIET && theme != THEME_PICK_ANOTHER && theme < Themes.Count()) {
        return Themes[theme]->Normal;
    }
    return false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool NewThemeClass::Is_Regular(ThemeType theme) const
{
#if defined(REDALERT2_BUILD) || defined(YURISREVENGE_BUILD)
    if (theme == THEME_QUIET || theme == THEME_PICK_ANOTHER || theme >= Themes.Count()) {
        return false;
    }
#endif
    return theme != THEME_NONE && Themes[theme]->Normal;
}


/**
 *  Fetches the base filename for the theme specified.
 */
const char * NewThemeClass::Base_Name(ThemeType theme) const
{
    if (theme != THEME_NONE && theme < Themes.Count()) {
        return Themes[theme]->Name;
    }
    return "No theme";
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
const char * NewThemeClass::INI_Base_Name(ThemeType theme) const
{
    if (theme != THEME_NONE && theme < Themes.Count()) {
        return Themes[theme]->Name;
    }
    return "none";
}


/**
 *  Retrieves the full score name.
 */
const char * NewThemeClass::Full_Name(ThemeType theme) const
{
    if (theme >= THEME_FIRST && Themes.Count()) {
        return Themes[theme]->Fullname;
    }
    return nullptr;
}


/**
 *  Process the theme engine and restart songs.
 */
void NewThemeClass::AI()
{
    if (!Audio_Driver()->Is_Available() || Debug_Quiet) {
        return;
    }

#if 0
    /**
     *  #BUGFIX:
     *  Ok, this might end up causing more issues, but we need to skip the
     *  theme update process to every 1 second, otherwise it greatly increases
     *  chance of the new audio processing thread trying to 
     */
    static CDTimerClass<MSTimerClass> _sleeper = 1000;
    if (!_sleeper.Expired()) {
        return;
    }
    _sleeper = 1000;
#endif

    if (!ScoresPresent || Volume <= 0 || ScenarioInit) {
        return;
    }

    if (Pending == THEME_NONE || Pending == THEME_QUIET) {
        return;
    }

    if (IsPlaying || IsPaused) {
        return;
    }
    
    DEBUG_INFO("Theme::AI - Time to play next song...\n");
    
    /*
    **	If the pending song needs to be picked, then pick it now.
    */
    if (Pending == THEME_PICK_ANOTHER) {
        Pending = Next_Song(Score);
        DEBUG_INFO("Theme::AI(Next_Song = %d)\n", Pending);
    }
    
    /*
    **	Start the song playing and then flag it so that a new song will
    **	be picked when this one ends.
    */
    Play_Song(Pending);
    Pending = THEME_PICK_ANOTHER;
}


/**
 *  Calculates the next song number to play.
 */
ThemeType NewThemeClass::Next_Song(ThemeType theme) const
{
    if (theme > THEME_FIRST && (Themes[theme]->Repeat || IsRepeat)) {
        return theme;
    }

    if (IsShuffle) {

        /*
        **	Shuffle the theme, but never pick the same theme that was just
        **	playing.
        */
        int tries = 0;
        bool maxed = false;
        ThemeType newtheme;
        while (tries++ <= 1000) {
            newtheme = Sim_Random_Pick(THEME_FIRST, (ThemeType)Themes.Count()-1);
            theme = newtheme;
            maxed = tries == 1000;
            if (newtheme != theme || Is_Allowed(newtheme)) {
                break;
            }
        }
        if (maxed) {
            theme = THEME_FIRST;
        }

    } else {

        /*
        **	Sequential score playing.
        */
        for (int i = Themes.Count()+1; i > 0; --i) {
            if (++theme > Themes.Count()) {
                theme = THEME_FIRST;
            }
            if (Is_Allowed(theme)) {
                return theme;
            }
        }

        theme = THEME_FIRST;
    }

    return theme;
}


/**
 *  Queues the song to the play queue.
 */
void NewThemeClass::Queue_Song(ThemeType theme)
{
    if (!Audio_Driver()->Is_Available()) {
        return;
    }

    /*
    **	If there is no score file present, then abort.
    */
    if (!ScoresPresent) {
        return;
    }

    /*
    **	If there is no sound driver or sounds have been specifically
    **	turned off, then abort.
    */
    if (Debug_Quiet) {
        return;
    }

    /*
    **	If the current score volume is set to silent, then there is no need to play the
    **	specified theme.
    */
    if (Volume <= 0) {
        return;
    }

    /*
    **	If the pending theme is available to be set and the specified theme is valid, then
    **	set the queued theme accordingly.
    */
    if (Pending == THEME_NONE || Pending == THEME_PICK_ANOTHER || theme == THEME_NONE || theme == THEME_QUIET) {
        Pending = theme;

        DEBUG_INFO("Theme::Queue_Song(%d)\n", theme);

        if (Still_Playing()) {
            DEBUG_INFO("Theme::Queue_Song - Fading.\n");
            Audio_Driver()->Fade_Out_Music();
        }

    }
}


/**
 *  Starts the specified song play NOW.
 */
bool NewThemeClass::Play_Song(ThemeType theme)
{
    if (!ScoresPresent || !Audio_Driver()->Is_Available() || Debug_Quiet) {
    }

    /**
     *  Stop any theme currently playing in a abrupt manner.
     */
    Stop();

    if (theme != THEME_NONE && theme != THEME_QUIET) {

        ThemeControl *tctrl = Themes[theme];

        /**
         *  #BUGFIX:
         *  Check for availability of the theme before attempting to play it.
         *  This stops the theme handler from spamming attempts to the audio engine.
         */
        if (tctrl->Available) {

            if (Volume > 0) {

                Score = theme;

                /**
                 *  Attempt to play the theme.
                 */
                Wstring fname = tctrl->Name;

                bool ok = Audio_Driver()->Play_Music(fname);
                if (!ok) {
                    return false;
                }

                IsPlaying = true;

                /**
                 *  Set the volume for this theme.
                 */
                float volf = std::clamp(float(Volume/255.0f), 0.0f, 1.0f);
                volf *= tctrl->Volume;
                Audio_Driver()->Set_Music_Volume(volf);
                
                /**
                 *  If this theme is flagged to repeat, set pending.
                 */
                if (tctrl->Repeat || IsRepeat) {
                    DEBUG_INFO("Theme::Play_Song(%d:%s) - Playing (Repeating)\n", Score, tctrl->Name);
                    Pending = theme;

                } else {
                    DEBUG_INFO("Theme::Play_Song(%d:%s) - Playing\n", Score, tctrl->Name);
                }

            } else {
                Pending = theme;
            }

        }

    }

    return true;
}


/**
 *  Constructs a filename for the specified theme.
 */
const char * NewThemeClass::Theme_File_Name(ThemeType theme)
{
    static char name[_MAX_FNAME+_MAX_EXT];

    if (theme >= THEME_FIRST && theme < Themes.Count()) {
        _makepath(name, nullptr, nullptr, Themes[theme]->Name, ".OGG");
        return (const char *)(&name[0]);
    }

    return "";
}


/**
 *  Calculates the length of the song (in seconds).
 */
int NewThemeClass::Track_Length(ThemeType theme) const
{
    if (theme < Themes.Count()) {
        return Themes[theme]->Duration * TIMER_SECOND;
    }
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void NewThemeClass::Fade_Out()
{
    Queue_Song(THEME_QUIET);
}


/**
 *  Stops the current theme from playing.
 */
void NewThemeClass::Stop(bool fade)
{
    if (Score == THEME_NONE) {
        return;
    }

    if (!ScoresPresent || !Audio_Driver()->Is_Available() || Debug_Quiet) {
        return;
    }

    if (fade && Still_Playing()) {
        DEBUG_INFO("Theme::Stop(%d) - Fading\n", Score);
        Audio_Driver()->Fade_Out_Music();
    } else {
        DEBUG_INFO("Theme::Stop(%d)\n", Score);
        Audio_Driver()->Stop_Music();
    }

    Score = THEME_NONE;
    Pending = THEME_NONE;

    IsPlaying = false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void NewThemeClass::Suspend(bool fade)
{
    if (Score == THEME_NONE) {
        return;
    }

    if (!ScoresPresent || !Audio_Driver()->Is_Available() || Debug_Quiet) {
        return;
    }

    DEBUG_INFO("Theme::Suspend(%d)\n", Score);

    Audio_Driver()->Stop_Music();

    Pending = Score;
    Score = THEME_NONE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void NewThemeClass::Pause()
{
    if (What_Is_Playing() <= THEME_NONE) {
        return;
    }

    if (!ScoresPresent || !Audio_Driver()->Is_Available() || Debug_Quiet) {
        return;
    }

    Audio_Driver()->Pause_Music();

    IsPaused = true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void NewThemeClass::UnPause()
{
    if (What_Is_Playing() <= THEME_NONE) {
        return;
    }

    if (!ScoresPresent || !Audio_Driver()->Is_Available() || Debug_Quiet) {
        return;
    }

    Audio_Driver()->UnPause_Music();

    IsPaused = false;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void NewThemeClass::Clear()
{
    //Themes.Delete_All();

    while (Themes.Count() > 0) {
        int index = Themes.Count()-1;
        delete Themes[index];
        Themes.Delete(index);
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void NewThemeClass::Set_Volume(int volume)
{
    float theme_vol = 1.0f;

    Volume = std::min(volume, 255);

    if (Themes.Count()) {
        ThemeType play = What_Is_Playing();
        if (play >= THEME_FIRST && play < Themes.Count()) {
            theme_vol = Themes[play]->Volume;
        }
    }

    float volf = std::clamp(float(Volume/255.0f), 0.0f, 1.0f);

    volf *= theme_vol;

    Audio_Driver()->Set_Music_Volume(volf);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int NewThemeClass::Process(CCINIClass & ini)
{
    char buffer[32];

    int count = ini.Entry_Count("Themes");
    for (int index = 0; index < count; ++index) {

        if (ini.Get_String("Themes", ini.Get_Entry("Themes", index), buffer, sizeof(buffer)) > 0) {
            ThemeType theme = From_Name(buffer);

            ThemeControl *ctrl = nullptr;
            if (theme == THEME_NONE) {
                ctrl = new ThemeControl;
                std::strcpy(ctrl->Name, buffer);
                Themes.Add(ctrl);
            } else {
                ctrl = Themes[theme];
            }
            ctrl->Fill_In(ini);
        }
    }

    return count;
}


/**
 *  Determines if music is still playing.
 */
bool NewThemeClass::Still_Playing() const
{
    if (IsPlaying) {
        return true;
    }

    if (Audio_Driver()->Is_Available() && Volume > 0 && !Debug_Quiet) {
        return Audio_Driver()->Is_Music_Playing();
    }

    return false;
}


/**
 *  Checks to see if the specified theme is legal.
 */
bool NewThemeClass::Is_Allowed(ThemeType index) const
{
    if (index == THEME_QUIET || index == THEME_PICK_ANOTHER) {
        return true;
    }
    
    if (index >= Themes.Count()) {
        return true;
    }

    /**
     *  If the theme is not present, then it certainly isn't allowed.
     */
    if (!Themes[index]->Available) {
        return false;
    }

    /**
     *  Only normal themes (playable during battle) are considered allowed.
     */
    if (!Themes[index]->Normal) {
        return false;
    }

    /**
     *  #issue-764
     * 
     *  If this theme requires an addon, make sure that addon is active.
     * 
     *  @author: CCHyper
     */
    AddonType addon = Themes[index]->RequiredAddon;
    if (addon != ADDON_NONE) {
        if (!Addon_Enabled(addon)) {
            return false;
        }
    }

    /**
     *  If the theme is not allowed to be played by the player's house, then don't allow
     *  it. If the player's house hasn't yet been determined, then presume this test
     *  passes.
     */
    SideType owner = Themes[index]->Owner;
    if (PlayerPtr != nullptr && owner != SIDE_NONE && PlayerPtr->Class->Side != owner) {
        return false;
    }

    /**
     *  If the scenario doesn't allow this theme yet, then return the failure flag. The
     *  scenario check only makes sense for solo play.
     */
    if (Session.Type == GAME_NORMAL && Scen->Scenario < Themes[index]->Scenario) {
        return false;
    }

    /**
     *  Since all tests passed, return with the "is allowed" flag.
     */
    return true;
}


/**
 *  Determines theme number from specified name.
 */
ThemeType NewThemeClass::From_Name(const char * name) const
{
    if (name && std::strlen(name) > 0) {

        /*
        **	First search for an exact name match with the filename
        **	of the theme. This is guaranteed to be unique.
        */
        for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {
            if (stricmp(Themes[theme]->Name, name) == 0) {
                return theme;
            }
        }

        /*
        **	If the filename scan failed to find a match, then scan for
        **	a substring within the full name of the score. This might
        **	yield a match, but is not guaranteed to be unique.
        */
        for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {
            if (std::strstr(Themes[theme]->Fullname, name) != nullptr) {
                return theme;
            }
        }
    }

    return THEME_NONE;
}


/**
 *  Scans all scores for availability.
 */
void NewThemeClass::Scan()
{
    if (Audio_Driver()->Is_Available() && !Debug_Quiet && Themes.Count() > 0) {

        for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {

            ThemeControl *tctrl = Themes[theme];

            bool available = false;

            /**
             *  
             */
            if (std::strlen(tctrl->Sound) > 0) {
                Wstring soundname = tctrl->Sound;
                available = Audio_Driver()->Is_Audio_File_Available(soundname);
                if (available) {
                    tctrl->Available = Audio_Driver()->Request_Preload(soundname, PRELOAD_MUSIC);
                }
            }
            
            /**
             *  Custom override was not available, use the base ini name.
             */
            if (!available) {

                /**
                 *  Check to see if the theme exists, request preload of asset if available.
                 */
                Wstring name = tctrl->Name;
                available = Audio_Driver()->Is_Audio_File_Available(name);
                if (available) {
                    tctrl->Available = Audio_Driver()->Request_Preload(name, PRELOAD_MUSIC);
                }
            }

            if (!available) {
                DEV_DEBUG_WARNING("Theme: Unable to find \"%s\"!\n", tctrl->Name);
            }

        }
        
        /**
         *  Flag the preloader to begin.
         */
        Audio_Driver()->Start_Preloader();

    }
}


/**
 *  Set the theme data for scenario and owner.
 */
void NewThemeClass::Set_Theme_Data(ThemeType theme, int scenario, SideType owners)
{
    if (theme != THEME_NONE) {
        Themes[theme]->Normal = true;
        Themes[theme]->Scenario = scenario;
        Themes[theme]->Owner = owners;
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool NewThemeClass::ThemeControl::Fill_In(CCINIClass & ini)
{
    if (!ini.Is_Present(Name)) {
        return false;
    }

    Scenario = ini.Get_Int(Name, "Scenario", Scenario);
    Duration = ini.Get_Float(Name, "Length", Duration);
    Normal = ini.Get_Bool(Name, "Normal", Normal);
    Repeat = ini.Get_Bool(Name, "Repeat", Repeat);
    Owner = ini.Get_SideType(Name, "Side", Owner);
    ini.Get_String(Name, "Name", Fullname, sizeof(Fullname));

    RequiredAddon = (AddonType)ini.Get_Int(Name, "RequiredAddon", RequiredAddon);

    /**
     *  New overrides.
     */
    ini.Get_String(Name, "Sound", Sound, sizeof(Sound));

    Volume = ini.Get_Float(Name, "Volume", Volume);
    Volume = std::clamp(Volume, 0.0f, 1.0f);

    return true;
}
