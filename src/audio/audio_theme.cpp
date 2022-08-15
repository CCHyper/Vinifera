/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                    *
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          FMOD_THEME.CPP
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
#ifdef USE_FMOD_AUDIO

#include "audio_theme.h"
#include "tibsun_inline.h"
#include "tibsun_globals.h"
#include "session.h"
#include "house.h"
#include "housetype.h"
#include "ccfile.h"
#include "ccini.h"
#include "addon.h"
#include "themeext.h"
#include "dsaudio.h"
#include "audio_manager.h"
#include "debughandler.h"
#include <thread>


/**
 *  Instance of the new theme engine (extern where needed).
 */
FMODThemeClass FMODTheme;


//ISoundInstance * FMODThemeClass::SoundHandle = nullptr;
//ISoundInstance * FMODThemeClass::SoundHandleQueue = nullptr;

float FMODThemeClass::FadeOutSeconds = 1.5f;


/**
 *  Default constructor for the theme manager class.
 */
FMODThemeClass::FMODThemeClass() :
    Current(INVALID_AUDIO_HANDLE),
    Score(THEME_NONE),
    Pending(THEME_NONE),
    Volume(255),
    IsRepeat(false),
    IsShuffle(false),
    Themes()
{
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
FMODThemeClass::~FMODThemeClass()
{
    Clear();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool FMODThemeClass::Is_Playable(ThemeType theme) const
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
bool FMODThemeClass::Is_Regular(ThemeType theme) const
{
    return theme != THEME_NONE && Themes[theme]->Normal;
}


/**
 *  Fetches the base filename for the theme specified.
 */
const char * FMODThemeClass::Base_Name(ThemeType theme) const
{
    if (theme != THEME_NONE && theme < Themes.Count()) {
        return Themes[theme]->Name.Peek_Buffer();
    }
    return "No theme";
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
const char * FMODThemeClass::INI_Base_Name(ThemeType theme) const
{
    if (theme != THEME_NONE && theme < Themes.Count()) {
        return Themes[theme]->Name.Peek_Buffer();
    }
    return "none";
}


/**
 *  Retrieves the full score name.
 */
const char * FMODThemeClass::Full_Name(ThemeType theme) const
{
    if (theme >= THEME_FIRST && Themes.Count()) {
        return Themes[theme]->Fullname.Peek_Buffer();
    }
    return nullptr;
}


/**
 *  Process the theme engine and restart songs.
 */
void FMODThemeClass::AI()
{
    /**
     *  #BUGFIX:
     *  Ok, this might end up causing more issues, but we need to skip the
     *  theme update process to every 250ms, otherwise it greatly increases
     *  chance of the new audio processing thread trying to access shared data.
     */
    static CDTimerClass<MSTimerClass> _sleeper = 250;
    if (!_sleeper.Expired()) {
        return;
    }
    _sleeper = 250;

    if (!AudioManager.Is_Available() || Debug_Quiet) {
        return;
    }

    if (!ScoresPresent || Volume <= 0 || ScenarioInit) {
        return;
    }

    if (Still_Playing()) {
        return;
    }

    if (Pending == THEME_NONE || Pending == THEME_QUIET) {
        return;
    }
    
    DEBUG_INFO("FMODTheme::AI - Time to play next song...\n");
    
    /**
     *	If the pending song needs to be picked, then pick it now.
     */
    if (Pending == THEME_PICK_ANOTHER) {
        Pending = Next_Song(Score);
        DEBUG_INFO("FMODTheme::AI(Next_Song = %d)\n", Pending);
    }
    
    /**
     *	Start the song playing and then flag it so that a new song will
     *	be picked when this one ends.
     */
    Play_Song(Pending);
    Pending = THEME_PICK_ANOTHER;
}


/**
 *  Calculates the next song number to play.
 */
ThemeType FMODThemeClass::Next_Song(ThemeType theme) const
{
    if (theme > THEME_FIRST && (Themes[theme]->Repeat || IsRepeat)) {
        return theme;
    }

    if (IsShuffle) {

        /**
         *	Shuffle the theme, but never pick the same theme that was just
         *	playing.
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

        /**
         *	Sequential score playing.
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
void FMODThemeClass::Queue_Song(ThemeType theme)
{
    if (!AudioManager.Is_Available()) {
        return;
    }

    /**
     *	If there is no score file present, then abort.
     */
    if (!ScoresPresent) {
        return;
    }

    /**
     *	If there is no sound driver or sounds have been specifically
     *	turned off, then abort.
     */
    if (Debug_Quiet) {
        return;
    }

    /**
     *	If the current score volume is set to silent, then there is no need to play the
     *	specified theme.
     */
    if (Volume <= 0) {
        return;
    }

    /**
     *	If the pending theme is available to be set and the specified theme is valid, then
     *	set the queued theme accordingly.
     */
    if (Pending == THEME_NONE || Pending == THEME_PICK_ANOTHER || theme == THEME_NONE || theme == THEME_QUIET) {
        Pending = theme;

        DEBUG_INFO("FMODTheme::Queue_Song(%d)\n", theme);

        if (Still_Playing()) {
            DEBUG_INFO("FMODTheme::Queue_Song - Fading.\n");
            AudioManager.Stop_Music_Channel(true, FadeOutSeconds);
        }

    }
}


/**
 *  Starts the specified song play NOW.
 */
bool FMODThemeClass::Play_Song(ThemeType theme)
{
    if (!ScoresPresent || !AudioManager.Is_Available() || Debug_Quiet) {
        return false;
    }

    /**
     *  Stop any theme currently playing in a abrupt manner.
     */
    Stop();

    if (theme == THEME_NONE || theme == THEME_QUIET || theme >= Themes.Count()) {
        return false;
    }

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
             *  Set the volume for this theme.
             */
            float volf = std::clamp(float(Volume/255.0f), 0.0f, 1.0f);
            volf *= tctrl->Volume;


            bool playing = AudioManager.Play_Music(theme, volf, AudioManagerClass::FMOD_DSP_ECHO);
            if (!playing) {
                DEBUG_ERROR("FMODTheme::Play_Song(%d:%s) - Failed to play!\n", Score, tctrl->Name.Peek_Buffer());
                return false;
            }
            
            /**
             *  If this theme is flagged to repeat, set pending.
             */
            if (tctrl->Repeat || IsRepeat) {
                DEBUG_INFO("FMODTheme::Play_Song(%d:%s) - Playing (Repeating)\n", Score, tctrl->Name.Peek_Buffer());
                Pending = theme;

            } else {
                DEBUG_INFO("FMODTheme::Play_Song(%d:%s) - Playing\n", Score, tctrl->Name.Peek_Buffer());
            }

        } else {
            Pending = theme;
        }

    }

    return true;
}


/**
 *  Constructs a filename for the specified theme.
 */
const char * FMODThemeClass::Theme_File_Name(ThemeType theme)
{
    if (theme >= THEME_FIRST && theme < Themes.Count()) {
        return Themes[theme]->FileName.Peek_Buffer();
    }

    return "";
}


/**
 *  Calculates the length of the song (in seconds).
 */
int FMODThemeClass::Track_Length(ThemeType theme) const
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
void FMODThemeClass::Fade_Out()
{
    Queue_Song(THEME_QUIET);
}


/**
 *  Stops the current theme from playing.
 */
void FMODThemeClass::Stop(bool fade)
{
    if (Score == THEME_NONE) {
        return;
    }

    if (!ScoresPresent || !AudioManager.Is_Available() || Debug_Quiet) {
        return;
    }

    Wstring score = Themes[Score]->Name;

    if (fade && Still_Playing()) {
        DEBUG_INFO("FMODTheme::Stop(%d) - Fading\n", Score);
        AudioManager.Stop_Music_Channel(true, FadeOutSeconds);

    } else {
        DEBUG_INFO("FMODTheme::Stop(%d)\n", Score);
        AudioManager.Stop_Music_Channel();
    }

    Score = THEME_NONE;
    Pending = THEME_NONE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODThemeClass::Suspend()
{
    if (Score == THEME_NONE) {
        return;
    }

    if (!ScoresPresent || !AudioManager.Is_Available() || Debug_Quiet) {
        return;
    }

    DEBUG_INFO("FMODTheme::Suspend(%d)\n", Score);

    AudioManager.Pause_Music_Channel();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODThemeClass::Resume()
{
    if (What_Is_Playing() <= THEME_NONE) {
        return;
    }

    if (!ScoresPresent || !AudioManager.Is_Available() || Debug_Quiet) {
        return;
    }

    DEBUG_INFO("FMODTheme::Resume(%d)\n", Score);

    AudioManager.Resume_Music_Channel();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool FMODThemeClass::Is_Paused() const
{
    return false; //return SoundHandle->Is_Paused();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void FMODThemeClass::Clear()
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
void FMODThemeClass::Set_Volume(int volume)
{
    float theme_vol = 1.0f;
    Wstring name;

    Volume = std::min(volume, 255);

    if (Themes.Count()) {
        ThemeType play = What_Is_Playing();
        if (play >= THEME_FIRST && play < Themes.Count()) {
            theme_vol = Themes[play]->Volume;
            name = Themes[play]->Name;
        }
    }

    float volf = std::clamp(float(Volume/255.0f), 0.0f, 1.0f);

    volf *= theme_vol;

    if (name.Is_Not_Empty()) {
        AudioManager.Set_Music_Volume(volf);
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int FMODThemeClass::Process(CCINIClass & ini)
{
    char buffer[32];

    int count = ini.Entry_Count("Themes");
    for (int index = 0; index < count; ++index) {

        if (ini.Get_String("Themes", ini.Get_Entry("Themes", index), buffer, sizeof(buffer)) > 0) {
            ThemeType theme = From_Name(buffer);

            ThemeControl *ctrl = nullptr;
            if (theme == THEME_NONE) {
                ctrl = new ThemeControl;
                ctrl->Name = buffer;
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
bool FMODThemeClass::Still_Playing() const
{
    if (!AudioManager.Is_Available() || Volume <= 0 || Debug_Quiet) {
        return false;
    }

    return AudioManager.Is_Music_Channel_Playing();
}


/**
 *  Checks to see if the specified theme is legal.
 */
bool FMODThemeClass::Is_Allowed(ThemeType index) const
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
ThemeType FMODThemeClass::From_Name(const char * name) const
{
    if (name && std::strlen(name) > 0) {

        Wstring theme_name = name;
        theme_name.To_Upper();

        /**
         *	First search for an exact name match with the filename
         *	of the theme. This is guaranteed to be unique.
         */
        for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {
            if (Themes[theme]->Name == theme_name) {
                return theme;
            }
        }

        /**
         *	If the filename scan failed to find a match, then scan for
         *	a substring within the full name of the score. This might
         *	yield a match, but is not guaranteed to be unique.
         */
        for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {
            if (std::strstr(Themes[theme]->Fullname.Peek_Buffer(), theme_name.Peek_Buffer()) != nullptr) {
                return theme;
            }
        }
    }

    return THEME_NONE;
}


/**
 *  Scans all scores for availability.
 */
void FMODThemeClass::Scan()
{
    if (!AudioManager.Is_Available() || Debug_Quiet || Themes.Count() < 0) {
        return;
    }

    /**
     *  Run the scan thread.
     */
    //FMODTheme_Run_Thread_Logic = true;

    AudioManager.Clear_Music();

    /**
     *  Set the expected size of the audio engines music vector.
     */
    AudioManager.Set_Music_Size(Themes.Count());

#if 1
    for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {

        ThemeControl *tctrl = Themes[theme];

        tctrl->Available = false;

        Wstring name = tctrl->Name;

        if (tctrl->Sound.Is_Not_Empty()) {
            name = tctrl->Sound;
        }

        if (AudioManager.Submit_Music(name, theme)) {
            tctrl->Available = true;

        } else {
            DEBUG_WARNING("FMODTheme: Unable to find \"%s\"!\n", tctrl->Name.Peek_Buffer());
        }

    }
#endif
}


/**
 *  Set the theme data for scenario and owner.
 */
void FMODThemeClass::Set_Theme_Data(ThemeType theme, int scenario, SideType owners)
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
bool FMODThemeClass::ThemeControl::Fill_In(CCINIClass & ini)
{
    if (!ini.Is_Present(Name.Peek_Buffer())) {
        return false;
    }

    char buffer[256];
    const char *name = Name.Peek_Buffer();

    Scenario = ini.Get_Int(name, "Scenario", Scenario);
    Duration = ini.Get_Float(name, "Length", Duration);
    Normal = ini.Get_Bool(name, "Normal", Normal);
    Repeat = ini.Get_Bool(name, "Repeat", Repeat);
    Owner = ini.Get_SideType(name, "Side", Owner);

    ini.Get_String(name, "Name", buffer, sizeof(buffer));
    Fullname = buffer;

    RequiredAddon = (AddonType)ini.Get_Int(name, "RequiredAddon", RequiredAddon);

    /**
     *  New overrides.
     */
    ini.Get_String(name, "Sound", buffer, sizeof(buffer));
    Sound = buffer;

    Volume = ini.Get_Float_Clamp(name, "Volume", 0.0f, 1.0f, Volume);

    return true;
}

#endif // USE_FMOD_AUDIO
