/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                    *
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MiniAudio_THEME.CPP
 *
 *  @author        Joe L. Bostic (see notes below)
 *
 *  @contributors  CCHyper, tomsons26
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
#ifdef USE_MINIAUDIO

#include "audio_theme.h"
#include "audio_manager.h"
#include "audio_handle.h"
#include "audio_util.h"
#include "tibsun_inline.h"
#include "tibsun_globals.h"
#include "options.h"
#include "session.h"
#include "house.h"
#include "housetype.h"
#include "ccfile.h"
#include "ccini.h"
#include "addon.h"
#include "side.h"
#include "debughandler.h"


/**
 *  Instance of the new theme engine (extern where needed).
 */
AudioThemeClass AudioTheme;


float AudioThemeClass::FadeOutSeconds = 10.0f;
bool AudioThemeClass::CrossFade = false;
float AudioThemeClass::CrossFadeSeconds = 10.0f;


/**
 *  Default constructor for the theme manager class.
 * 
 *  @author: CCHyper
 */
AudioThemeClass::AudioThemeClass() :
    CurrentScoreHandle(nullptr),
    Score(THEME_NONE),
    Pending(THEME_NONE),
    //Volume(255),
    IsRepeat(false),
    IsShuffle(false),
    Themes()
{
}


/**
 *  Class destructor
 * 
 *  @author: CCHyper
 */
AudioThemeClass::~AudioThemeClass()
{
    Clear();
}


/**
 *  Is this a normal theme that was flaged as available and can be played? 
 * 
 *  @author: CCHyper
 */
bool AudioThemeClass::Is_Playable(ThemeType theme) const
{
    return Is_Regular(theme) && Themes[theme]->Available;
}


/**
 *  Is this a normal theme that can be played?
 * 
 *  @author: CCHyper
 */
bool AudioThemeClass::Is_Regular(ThemeType theme) const
{
    return theme > THEME_NONE && theme < Themes.Count() && Themes[theme]->Normal;
}


/**
 *  Fetches the base filename for the theme specified.
 * 
 *  @author: CCHyper
 */
const char * AudioThemeClass::Base_Name(ThemeType theme) const
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
const char * AudioThemeClass::INI_Base_Name(ThemeType theme) const
{
    if (theme != THEME_NONE && theme < Themes.Count()) {
        return Themes[theme]->Name.Peek_Buffer();
    }
    return "none";
}


/**
 *  Retrieves the full score name.
 */
const char * AudioThemeClass::Full_Name(ThemeType theme) const
{
    if (theme >= THEME_FIRST && Themes.Count()) {
        return Themes[theme]->Fullname.Peek_Buffer();
    }
    return nullptr;
}


/**
 *  Process the theme engine and restart songs.
 */
void AudioThemeClass::AI()
{
    /**
     *  If there is no sound driver, no score file present, or sounds have been
     *  specifically turned off then abort.
     */
    if (!AudioManager.Is_Available() || !ScoresPresent || Debug_Quiet) {
        return;
    }

    if (AudioManager.Get_Group_Volume(AUDIO_GROUP_MUSIC) <= 0.0f) {
        return;
    }

    if (ScenarioInit) {
        return;
    }

    if (Still_Playing()) {
        return;
    }

    if (Pending == THEME_NONE || Pending == THEME_QUIET) {
        return;
    }
    
    DEBUG_INFO("Theme::AI - Time to play next song...\n");
    
    /**
     *  If the pending song needs to be picked, then pick it now.
     */
    if (Pending == THEME_PICK_ANOTHER) {
        Pending = Next_Song(Score);
        DEBUG_INFO("Theme::AI - Next_Song returned \"%s\".\n", Themes[Pending]->Name.Peek_Buffer());
    }
    
    /**
     *  Start the song playing.
     */
    DEBUG_INFO("Theme::AI - About to call Play_Song with \"%s\".\n", Themes[Pending]->Name.Peek_Buffer());
    Play_Song(Pending);

    /**
     *  Now flag it so that a new song will be picked when this one ends.
     */
    Pending = THEME_PICK_ANOTHER;
}


/**
 *  Calculates the next song number to play.
 */
ThemeType AudioThemeClass::Next_Song(ThemeType theme) const
{
    /**
     *  x
     */
    if (theme > THEME_FIRST && (Themes[theme]->Repeat || IsRepeat)) {
        return theme;
    }

    if (IsShuffle) {

        /**
         *  Shuffle the theme, but never pick the same theme that was just
         *  playing.
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
         *  Sequential score playing.
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
void AudioThemeClass::Queue_Song(ThemeType theme)
{
    /**
     *  If there is no sound driver, no score file present, or sounds have been
     *  specifically turned off then abort.
     */
    if (!AudioManager.Is_Available() || !ScoresPresent || Debug_Quiet) {
        return;
    }

    /**
     *  If the current score volume is set to silent, then there is no need to
     *  play the specified theme.
     */
    if (AudioManager.Get_Group_Volume(AUDIO_GROUP_MUSIC) <= 0.0f) {
        return;
    }

    /**
     *  If the pending theme is available to be set and the specified theme is valid, then
     *  set the queued theme accordingly.
     */
    if (Pending == THEME_NONE || Pending == THEME_PICK_ANOTHER || theme == THEME_NONE || theme == THEME_QUIET) {

        if (theme >= THEME_FIRST) {
            DEBUG_INFO("Theme::Queue_Song - Queued \"%s\".\n", Themes[theme]->Name.Peek_Buffer());
        } else {
            DEBUG_INFO("Theme::Queue_Song - Pending %d, theme %d.\n", Pending, theme);
        }

        Pending = theme;

        if (Still_Playing()) {
            //DEBUG_INFO("Theme::Queue_Song - Fading out \"%s\"...\n", Themes[Score]->Name.Peek_Buffer());
            DEBUG_INFO("Theme::Queue_Song - Fading out current score...\n");
            CurrentScoreHandle->Stop(FadeOutSeconds, true);
        }

    }
}


/**
 *  Starts the specified song play NOW.
 */
bool AudioThemeClass::Play_Song(ThemeType theme)
{
    /**
     *  If there is no sound driver, no score file present, or sounds have been
     *  specifically turned off then abort.
     */
    if (!AudioManager.Is_Available() || !ScoresPresent || Debug_Quiet) {
        return false;
    }

    /**
     *  Stop any theme currently playing in a abrupt manner.
     */
    Stop();

    /**
     *  If the current score volume is set to silent, then there is no need to
     *  play the specified theme.
     */
    if (AudioManager.Get_Group_Volume(AUDIO_GROUP_MUSIC) <= 0.0f) {
        return false;
    }

    /**
     *  x
     */
    if (theme == THEME_NONE || theme == THEME_QUIET || theme >= Themes.Count()) {
        return false;
    }

    ThemeControl *tctrl = Themes[theme];

    /**
     *  #BUGFIX:
     *  Check for availability of the theme before attempting to play it.
     *  This stops the theme handler from spamming attempts to the audio engine.
     */
    if (!tctrl->Available) {
        return false;
    }

    Score = theme;

    /**
     *  x
     */
    AUDIO_DEBUG_INFO("Theme::Play_Song - About to call AudioManager.Play with \"%s\".\n", tctrl->FileName.Peek_Buffer());
    AudioHandleClass *handle = AudioManager.Play(tctrl->FileName,
                                                 AUDIO_GROUP_MUSIC,
                                                 tctrl->Volume,
                                                 1.0f,
                                                 0.0f,
                                                 AUDIO_PRIORITY_HIGH,
                                                 CrossFade ? CrossFadeSeconds : 0.0f);

    if (!handle) {
        AUDIO_DEBUG_ERROR("Theme::Play_Song - Failed to play \"%s\"!\n", Themes[theme]->Name.Peek_Buffer());
        return false;
    }

    /**
     *  x
     */
    if (CurrentScoreHandle) {
        AUDIO_DEBUG_INFO("Theme::Play_Song - Stopping handle for \"%s\"\n", CurrentScoreHandle->Get_Filename().Peek_Buffer());
        CurrentScoreHandle->Stop(CrossFade ? CrossFadeSeconds : 0.0f, CrossFade);
    }

    /**
     *  x
     */
    CurrentScoreHandle = handle;
    
    /**
     *  If this theme is flagged to repeat, set pending.
     */
    if (tctrl->Repeat || IsRepeat) {
        DEBUG_INFO("Theme::Play_Song - Playing \"%s\" (Repeating)\n", Themes[theme]->Name.Peek_Buffer());
        Pending = theme;

    } else {
        DEBUG_INFO("Theme::Play_Song - Playing \"%s\" (Normal)\n", Themes[theme]->Name.Peek_Buffer());
    }

    return true;
}


/**
 *  Calculates the length of the song (in seconds).
 */
int AudioThemeClass::Track_Length(ThemeType theme) const
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
void AudioThemeClass::Fade_Out()
{
    AUDIO_DEBUG_INFO("Theme::Fade_Out - About to call Queue_Song with THEME_QUIET.\n");
    Queue_Song(THEME_QUIET);
}


/**
 *  Stops the current theme from playing.
 * 
 *  @author: CCHyper
 */
void AudioThemeClass::Stop(bool fade)
{
    /**
     *  If there is no sound driver, no score file present, or sounds have been
     *  specifically turned off then abort.
     */
    if (!AudioManager.Is_Available() || !ScoresPresent || Debug_Quiet) {
        return;
    }

    if (Score == THEME_NONE) {
        //AUDIO_DEBUG_WARNING("Theme::Stop - Score is null, nothing to stop.\n");
        return;
    }

    if (!CurrentScoreHandle) {
        //AUDIO_DEBUG_WARNING("Theme::Stop - Handle is null, nothing to stop.\n");
        return;
    }

    if (fade && Still_Playing()) {
        DEBUG_INFO("Theme::Stop - Fading out \"%s\"...\n", Themes[Score]->Name.Peek_Buffer());
        CurrentScoreHandle->Stop(FadeOutSeconds, true);

    } else {
        DEBUG_INFO("Theme::Stop - Forced \"%s\" to stop.\n", Themes[Score]->Name.Peek_Buffer());
        CurrentScoreHandle->Stop();
    }

    Score = THEME_NONE;
    Pending = THEME_NONE;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioThemeClass::Suspend()
{
    /**
     *  If there is no sound driver, no score file present, or sounds have been
     *  specifically turned off then abort.
     */
    if (!AudioManager.Is_Available() || !ScoresPresent || Debug_Quiet) {
        return false;
    }

    if (What_Is_Playing() <= THEME_NONE) {
        return false;
    }

    if (!CurrentScoreHandle) {
        return false;
    }

    DEBUG_INFO("Theme::Suspend - Suspending score \"%s\"\n", Themes[Score]->Name.Peek_Buffer());

    return CurrentScoreHandle->Pause();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioThemeClass::Resume()
{
    /**
     *  If there is no sound driver, no score file present, or sounds have been
     *  specifically turned off then abort.
     */
    if (!AudioManager.Is_Available() || !ScoresPresent || Debug_Quiet) {
        return false;
    }

    if (What_Is_Playing() <= THEME_NONE) {
        return false;
    }

    if (!CurrentScoreHandle) {
        return false;
    }

    DEBUG_INFO("Theme::Resume - Resuming score \"%s\"\n", Themes[Score]->Name.Peek_Buffer());

    return CurrentScoreHandle->Resume();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioThemeClass::Is_Paused() const
{
    if (!CurrentScoreHandle) {
        return false;
    }

    return CurrentScoreHandle->Is_Paused();
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioThemeClass::Clear()
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
void AudioThemeClass::Set_Volume(int volume)
{
    float volf = std::clamp(float(volume/255.0f), 0.0f, 1.0f);
    AudioManager.Set_Group_Volume(AUDIO_GROUP_MUSIC, volf);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int AudioThemeClass::Process(CCINIClass &ini)
{
    static char const * const GENERAL = "General";
    static char const * const THEMES = "Themes";

    AUDIO_DEBUG_INFO("Theme::Process(enter): Themes.Count = %d\n", Themes.Count());

    char buffer[32];

    if (ini.Is_Present(GENERAL)) {
        FadeOutSeconds = ini.Get_Float(GENERAL, "FadeOutSeconds", FadeOutSeconds);
        CrossFade = ini.Get_Bool(GENERAL, "CrossFading", CrossFade);
        CrossFadeSeconds = ini.Get_Float(GENERAL, "CrossFadeSeconds", CrossFadeSeconds);
    }

    int count = ini.Entry_Count(THEMES);
    for (int index = 0; index < count; ++index) {

        if (ini.Get_String(THEMES, ini.Get_Entry(THEMES, index), buffer, sizeof(buffer)-1) > 0) {
            ThemeType theme = From_Name(buffer);

            ThemeControl *ctrl = nullptr;
            if (theme == THEME_NONE) {
                ctrl = new ThemeControl(buffer);
                AUDIO_DEBUG_INFO("Theme::Process: Creating new Theme %s.\n", ctrl->Name.Peek_Buffer());
                Themes.Add(ctrl);

            } else {
                ctrl = Themes[theme];
                AUDIO_DEBUG_INFO("Theme::Process: Found exiting Theme %s.\n", ctrl->Name.Peek_Buffer());
            }
            ctrl->Fill_In(ini);
        }
    }

    AUDIO_DEBUG_INFO("Theme::Process(exit): Themes.Count = %d\n", Themes.Count());

    return count;
}


/**
 *  Determines if music is still playing.
 * 
 *  @author: CCHyper
 */
bool AudioThemeClass::Still_Playing() const
{
    if (!AudioManager.Is_Available() || Debug_Quiet) {
        return false;
    }

    if (!CurrentScoreHandle) {
        return false;
    }

    return CurrentScoreHandle->Is_Playing();
}


/**
 *  Checks to see if the specified theme is legal.
 * 
 *  @author: CCHyper
 */
bool AudioThemeClass::Is_Allowed(ThemeType index) const
{
    //ASSERT(index < Themes.Count()); // Removed as Next_Song goes out of bounds (by design).

    if (index == THEME_QUIET || index == THEME_PICK_ANOTHER) {
        return true;
    }

#if 0
    /**
     *  Is the required theme within the available range?
     * 
     *  #NOTE: Removed as Next_Song goes out of bounds (by design).
     */
    if (index >= Themes.Count()) {
        return false;
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
#else
    /**
     *  Only normal themes (playable during battle) are considered allowed.
     */
    if (!Is_Playable(index)) {
        return false;
    }
#endif

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
            //AUDIO_DEBUG_WARNING("Theme::Is_Allowed - \"%s\" is only available for addon %d.\n", Themes[index]->Name.Peek_Buffer(), addon);
            return false;
        }
    }

    /**
     *  If the theme is not allowed to be played by the player's house, then don't allow
     *  it. If the player's house hasn't yet been determined, then presume this test
     *  passes.
     */
    if (PlayerPtr != nullptr && Themes[index]->Owner != -1 && ((1 << PlayerPtr->Class->Side) & Themes[index]->Owner) == 0) {
        AUDIO_DEBUG_WARNING("Theme::Is_Allowed - Side \"%s\" not allowed to play %s!\n", SideClass::Name_From(PlayerPtr->Class->Side), Themes[index]->Name.Peek_Buffer());
        return false;
    }

    /**
     *  If the scenario doesn't allow this theme yet, then return the failure flag. The
     *  scenario check only makes sense for solo play.
     */
    if (Session.Type == GAME_NORMAL && Scen->Scenario < Themes[index]->Scenario) {
        //AUDIO_DEBUG_WARNING("Theme::Is_Allowed - \"%s\" is not yet available for this scenario (\"%d\").\n", Themes[index]->Name.Peek_Buffer(), Scen->Scenario);
        return false;
    }

    //AUDIO_DEBUG_INFO("Theme::Is_Allowed - \"%s\" is allowed to be played!\n", Themes[index]->Name.Peek_Buffer());

    /**
     *  Since all tests passed, return with the "is allowed" flag.
     */
    return true;
}


/**
 *  Determines theme number from specified name.
 * 
 *  @author: CCHyper
 */
ThemeType AudioThemeClass::From_Name(const char * name) const
{
    if (!name || std::strlen(name) <= 0) {
        return THEME_NONE;
    }

    Wstring theme_name = name;
    theme_name.To_Upper();

    /**
     *  First search for an exact name match with the filename
     *  of the theme. This is guaranteed to be unique.
     */
    for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {
        if (Themes[theme]->Name == theme_name) {
            return theme;
        }
    }

    /**
     *  If the filename scan failed to find a match, then scan for
     *  a substring within the full name of the score. This might
     *  yield a match, but is not guaranteed to be unique.
     */
    for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {
        if (std::strstr(Themes[theme]->Fullname.Peek_Buffer(), theme_name.Peek_Buffer()) != nullptr) {
            return theme;
        }
    }

    return THEME_NONE;
}


/**
 *  Scans all scores for availability.
 *
 *  @author: CCHyper
 */
void AudioThemeClass::Scan()
{
    if (!AudioManager.Is_Available() || Debug_Quiet || !ScoresPresent) {
        return;
    }

    for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {

        ThemeControl *tctrl = Themes[theme];

        tctrl->Available = false;

        Wstring name = tctrl->Name;

        if (tctrl->Sound.Is_Not_Empty()) {
            name = tctrl->Sound;
        }

        /**
         *  x
         */
        AudioManager.Audio_Set_Data(name, tctrl->Available, tctrl->FileType, tctrl->FileName);
    }

#if 0//#ifndef NDEBUG
    AUDIO_DEBUG_WARNING("Theme dump...\n");
    for (int index = 0; index < Themes.Count(); ++index) {
        AUDIO_DEBUG_WARNING("  %03d  %s\n", index, Themes[index]->Name.Peek_Buffer());
    }
#endif
}


/**
 *  Set the theme data for scenario and owner.
 *
 *  @author: CCHyper
 */
void AudioThemeClass::Set_Theme_Data(ThemeType theme, int scenario, SideType owners)
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
bool AudioThemeClass::ThemeControl::Fill_In(CCINIClass &ini)
{
    if (!ini.Is_Present(Name.Peek_Buffer())) {
        return false;
    }

    char buffer[256];
    const char* name = Name.Peek_Buffer();

    Scenario = ini.Get_Int(name, "Scenario", Scenario);
    Duration = ini.Get_Float(name, "Length", Duration);
    Normal = ini.Get_Bool(name, "Normal", Normal);
    Repeat = ini.Get_Bool(name, "Repeat", Repeat);

    /**
     *  #issue-764
     * 
     *  x
     *
     *  @author: CCHyper
     */
    RequiredAddon = (AddonType)ini.Get_Int(name, "RequiredAddon", RequiredAddon);

    /**
     *  #issue-x
     * 
     *  x
     * 
     *  @author: CCHyper
     */
     //Owner = ini.Get_SideType(name, "Side", Owner);
    if (ini.Get_String(name, "Side", buffer, sizeof(buffer)) > 0) {
        const char * token = std::strtok(buffer, ",");
        while (token) {
            SideType side = SideClass::From_Name(token);
            if (side != SIDE_NONE) {
                Owner |= 1 << side;
                AUDIO_DEBUG_INFO("Theme: Setting side %s for %s.\n", token, name);
            }
            token = std::strtok(nullptr, ",");
        }
    }

    Volume = ini.Get_Float_Clamp(name, "Volume", AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX, Volume);

    ini.Get_String(name, "Sound", Sound);

    ini.Get_String(name, "Name", Fullname);
    ini.Get_String(name, "Artist", Artist);

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioThemeClass::Fill_In_All(CCINIClass &ini)
{
    for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); ++theme) {

        ThemeControl *tctrl = Themes[theme];

        if (!ini.Is_Present(tctrl->Name.Peek_Buffer())) {
            continue;
        }

        char buffer[256];
        const char * name = tctrl->Name.Peek_Buffer();

        /**
         *  #issue-x
         *
         *  x
         *
         *  @author: CCHyper
         */
         //Owner = ini.Get_SideType(name, "Side", Owner);
        if (ini.Get_String(name, "Side", buffer, sizeof(buffer)) > 0) {
            const char * token = std::strtok(buffer, ",");
            while (token) {
                SideType side = SideClass::From_Name(token);
                if (side != SIDE_NONE) {
                    if (tctrl->Owner == -1) tctrl->Owner = 0;
                    tctrl->Owner |= 1 << side;
                    AUDIO_DEBUG_INFO("Theme: Setting side %s for %s.\n", token, name);
                }
                token = std::strtok(nullptr, ",");
            }
        }

    }

    return true;
}


#endif // USE_MINIAUDIO
