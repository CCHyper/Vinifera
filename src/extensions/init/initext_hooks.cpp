/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          INITEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains any hooks for the game init process.
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
#include "initext_hooks.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "special.h"
#include "playmovie.h"
#include "theme.h"
#include "ccfile.h"
#include "cd.h"
#include "newmenu.h"
#include "addon.h"
#include "dsaudio.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  #issue-518
 * 
 *  Enable the caching of additional mix files to limit the loading
 *  of large mix files during an active game session.
 */
#define CACHE_EXTRA_MIXFILES 1


/**
 *  #issue-
 * 
 *  Reimplemention of Init_Bootstrap_Mixfiles()
 *  
 *  Registers and caches any mixfiles needed for bootstrapping.
 * 
 *  @author: CCHyper
 */
static bool Vinifera_Init_Bootstrap_Mixfiles()
{
    bool ok;
    MFCC *mix;

    int temp = CD::RequiredCD;
    CD::Set_Required_CD(-2);

    DEBUG_INFO("\n"); // Fixes missing new-line after "Bootstrap..." print.
    //DEBUG_INFO("Init bootstrap mixfiles...\n");

    /**
     *  #issue-514
     * 
     *  Make sure the loading of PATCH.MIX honors the search path interface.
     */
    //if (RawFileClass("PATCH.MIX").Is_Available()) {
    if (CCFileClass("PATCH.MIX").Is_Available()) {
        mix = new MFCC("PATCH.MIX", &FastKey);
        ASSERT(mix);
        if (mix) {
#ifdef CACHE_EXTRA_MIXFILES
            /**
             *  #issue-518 
             * 
             *  Enable the caching of additional mix files to improve performance.
             *  
             *  @author: CCHyper
             */
            mix->Cache();
#endif
            DEBUG_INFO(" PATCH.MIX\n");
        }
    }

    if (CCFileClass("PCACHE.MIX").Is_Available()) {
        mix = new MFCC("PCACHE.MIX", &FastKey);
        ASSERT(mix);
        if (mix) {
            mix->Cache();
            DEBUG_INFO(" PCACHE.MIX\n");
        }
    }

    for (int i = 99; i >= 0; --i) {
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "EXPAND%02d.MIX", i);
        /**
         *  #issue-514
         * 
         *  Make sure the loading of EXPAND##.MIX honors the search path interface.
         */
        //if (RawFileClass(buffer).Is_Available()) {
        if (CCFileClass(buffer).Is_Available()) {
            mix = new MFCC(buffer, &FastKey);
            ASSERT(mix);
            if (!mix) {
                DEBUG_WARNING("Failed to load %s!\n", buffer);
            } else {
#ifdef CACHE_EXTRA_MIXFILES
                ExpansionMixFiles.Add(mix);
                DEBUG_INFO(" %s (Cached)\n", mix->Filename);
                /**
                 *  #issue-518 
                 * 
                 *  Enable the caching of EXPAND mix files to improve performance.
                 *  
                 *  @author: CCHyper
                 */
                mix->Cache();
#else
                ExpansionMixFiles.Add(mix);
                DEBUG_INFO(" %s\n", buffer);
#endif
            }
        }
    }

    for (int i = 99; i >= 0; --i) {
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "ECACHE%02d.MIX", i);
        if (CCFileClass(buffer).Is_Available()) {
            mix = new MFCC(buffer, &FastKey);
            ASSERT(mix);
            if (!mix) {
                DEBUG_WARNING("Failed to load %s!\n", buffer);
            } else {
                mix->Cache();
                ExpansionMixFiles.Add(mix);
                DEBUG_INFO(" %s\n", buffer);
            }
        }
    }

    Addon_Present();

    TibSunMix = new MFCC("TIBSUN.MIX", &FastKey);
    ASSERT(TibSunMix);
    if (!TibSunMix) {
        DEBUG_WARNING("Failed to load TIBSUN.MIX!\n");
        return false;
    }
    DEBUG_INFO(" TIBSUN.MIX\n");

    /*
    **	Bootstrap enough of the system so that the error dialog
    *   box can successfully be displayed.
    */
    CacheMix = new MFCC("CACHE.MIX", &FastKey);
    ASSERT(CacheMix);
    if (!CacheMix) {
        DEBUG_WARNING("Failed to load CACHE.MIX!\n");
        return false;
    }
    if (!CacheMix->Cache()) {
        DEBUG_WARNING("Failed to cache CACHE.MIX!\n");
        return false;
    }
    DEBUG_INFO(" CACHE.MIX\n");

    LocalMix = new MFCC("LOCAL.MIX", &FastKey);
    ASSERT(LocalMix);
    if (!LocalMix) {
        DEBUG_WARNING("Failed to load LOCAL.MIX!\n");
        return false;
    }
    DEBUG_INFO(" LOCAL.MIX\n");

    CD::Set_Required_CD(temp);

    return true;
}


/**
 *  #issue-
 * 
 *  Reimplemention of Init_Secondary_Mixfiles()
 *  
 *  Register and cache secondary mixfiles.
 * 
 *  @author: CCHyper
 */
static bool Vinifera_Init_Secondary_Mixfiles()
{
    char buffer[16];

    DEBUG_INFO("\n"); // Fixes missing new-line after "Init Secondary Mixfiles....." print.
    //DEBUG_INFO("Init secondary mixfiles...\n");

    if (CCFileClass("CONQUER.MIX").Is_Available()) {
        ConquerMix = new MFCC("CONQUER.MIX", &FastKey);
        ASSERT(ConquerMix);
    }
    if (!ConquerMix) {
        DEBUG_WARNING("Failed to load CONQUER.MIX!\n");
        return false;
    }
    DEBUG_INFO(" CONQUER.MIX\n");

    int cd = CD::Get_Volume_Index();

    /**
     *  Make sure we have a grounded volume index (invalid volumes will cause error).
     */
    if (CD::Get_Volume_Index() < 0) {
        cd = 0;
    }

    /**
     *  Mix file indices are 1 based.
     */
    cd += 1;

    std::snprintf(buffer, sizeof(buffer), "MAPS%02d.MIX", cd);
    if (CCFileClass(buffer).Is_Available()) {
        MapsMix = new MFCC(buffer, &FastKey);
        ASSERT(MapsMix);
    }
    if (!MapsMix) {
        DEBUG_WARNING("Failed to load %s!\n", buffer);
        return false;
    }
    DEBUG_INFO(" %s\n", buffer);

    if (CCFileClass("MULTI.MIX").Is_Available()) {
        MultiMix = new MFCC("MULTI.MIX", &FastKey);
        ASSERT(MultiMix);
    }
    if (!MultiMix) {
        DEBUG_WARNING("Failed to load MULTI.MIX!\n");
        return false;
    }
    DEBUG_INFO(" MULTI.MIX\n", buffer);

    if (Addon_407120(ADDON_FIRESTORM)) {
        if (CCFileClass("SOUNDS01.MIX").Is_Available()) {
            FSSoundsMix = new MFCC("SOUNDS01.MIX", &FastKey);
            ASSERT(FSSoundsMix);
        }
        if (!FSSoundsMix) {
            DEBUG_WARNING("Failed to load SOUNDS01.MIX!\n");
            return false;
        }
        DEBUG_INFO(" SOUNDS01.MIX\n", buffer);
    }

    if (CCFileClass("SOUNDS.MIX").Is_Available()) {
        SoundsMix = new MFCC("SOUNDS.MIX", &FastKey);
        ASSERT(SoundsMix);
    }
    if (!SoundsMix) {
        DEBUG_WARNING("Failed to load SOUNDS.MIX!\n");
        return false;
    }
    DEBUG_INFO(" SOUNDS.MIX\n", buffer);

    if (CCFileClass("SCORES01.MIX").Is_Available()) {
        FSScoresMix = new MFCC("SCORES01.MIX", &FastKey);
        ASSERT(FSScoresMix);
    }
    if (!FSScoresMix) {
        DEBUG_WARNING("Failed to load SCORES01.MIX!\n");
        return false;
    }
    DEBUG_INFO(" SCORES01.MIX\n", buffer);

	/*
	**	Register the score mixfile.
	*/
    if (CCFileClass("SCORES.MIX").Is_Available()) {
        ScoreMix = new MFCC("SCORES.MIX", &FastKey);
        ASSERT(ScoreMix);
    }
    if (!ScoreMix) {
        DEBUG_WARNING("Failed to load SCORES.MIX!\n");
        return false;
    }
    DEBUG_INFO(" SCORES.MIX\n", buffer);
	ScoresPresent = true;
	Theme.Scan();

    std::snprintf(buffer, sizeof(buffer), "MOVIES%02d.MIX", cd);
    if (CCFileClass(buffer).Is_Available()) {
        MoviesMix = new MFCC(buffer, &FastKey);
        ASSERT(MoviesMix);
    }
    if (!MoviesMix) {
        DEBUG_WARNING("Failed to load %s!\n", buffer);
        return false;
    }
    DEBUG_INFO(" %s\n", buffer);

    return true;
}


/**
 *  #issue-513
 * 
 *  Patch to add check for CD::IsFilesLocal to make sure -CD really
 *  was set by the user.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Init_CDROM_Access_Local_Files_Patch)
{
    _asm { add esp, 4 }

    if (CCFileClass::Is_There_Search_Drives() && CD::IsFilesLocal) {
        goto files_local;
    }

init_cdrom:
    JMP(0x004E0471);

files_local:
    JMP(0x004E06F5);
}


/**
 *  #issue-518 
 * 
 *  Enable the caching of various game mix files to improve performance.
 * 
 *  @note: Original code returned false if caching failed.
 *  
 *  @author: CCHyper
 */
DECLARE_PATCH(_Init_Bulk_Data_Cache_Mix_Files_Patch)
{
    /**
     *  Stolen bytes/code.
     */
    _asm { sub esp, 0x29C }
    _asm { push edi }

#ifdef CACHE_EXTRA_MIXFILES

    DEBUG_INFO("Caching additional mix files...\n");

    //if (TibSunMix) DEBUG_INFO("  %s\n", TibSunMix->Filename); TibSunMix->Cache();

    if (ConquerMix) DEBUG_INFO("  %s\n", ConquerMix->Filename); ConquerMix->Cache();
    if (LocalMix) DEBUG_INFO("  %s\n", LocalMix->Filename); LocalMix->Cache();

    if (MapsMix) DEBUG_INFO("  %s\n", MapsMix->Filename); MapsMix->Cache();
    if (MultiMix) DEBUG_INFO("  %s\n", MultiMix->Filename); MultiMix->Cache();

    if (Audio.Is_Available()) {
        if (SoundsMix) DEBUG_INFO("  %s\n", SoundsMix->Filename); SoundsMix->Cache();
        if (FSSoundsMix) DEBUG_INFO("  %s\n", FSSoundsMix->Filename); FSSoundsMix->Cache();
        if (ScoreMix) DEBUG_INFO("  %s\n", ScoreMix->Filename); ScoreMix->Cache();
        if (FSScoresMix) DEBUG_INFO("  %s\n", FSScoresMix->Filename); FSScoresMix->Cache();
    }
    
    //if (MoviesMix) DEBUG_INFO("  %s\n", MoviesMix->Filename); MoviesMix->Cache();

    DEBUG_INFO("Mix file caching complete!\n");

#else

    /**
     *  Original code.
     */

    //if (!ConquerMix) {
    //    goto return_false;
    //}
    if (ConquerMix) {
        ConquerMix->Cache();
    }

    if (Audio.Is_Available()) {
        if (SoundsMix) {
            SoundsMix->Cache();
        }
        if (FSSoundsMix) {
            FSSoundsMix->Cache();
        }
    }

#endif

continue_function:
    _asm { xor edi, edi }   // Set EDI to expected state.
    JMP(0x004E465F);

return_false:
    JMP(0x004E49B7);
}


static bool CCFile_Is_Available(const char *filename)
{
    return CCFileClass(filename).Is_Available();
}


/**
 *  #issue-478
 * 
 *  
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Init_Game_Skip_Startup_Movies_Patch)
{
    if (Vinifera_SkipStartupMovies) {
        DEBUG_INFO("Skipping startup movies.\n");
        goto skip_loading_screen;
    }

    if (Special.IsFromInstall) {
        DEBUG_GAME("Playing first time intro sequence.\n");
        Play_Movie("EVA.VQA", THEME_NONE, true, true, true);
    }

    if (!Vinifera_SkipWWLogoMovie) {
        DEBUG_GAME("Playing startup movies.\n");
        Play_Movie("WWLOGO.VQA", THEME_NONE, true, true, true);
    } else {
        DEBUG_INFO("Skipping startup movie.\n");
    }

    if (!NewMenuClass::Get()) {
        if (CCFile_Is_Available("FS_TITLE.VQA")) {
            Play_Movie("FS_TITLE.VQA", THEME_NONE, true, false, true);
        } else {
            Play_Movie("STARTUP.VQA", THEME_NONE, true, false, true);
        }
    }

loading_screen:
    _asm { or ebx, 0xFFFFFFFF }
    JMP(0x004E0848);

skip_loading_screen:
    JMP(0x004E084D);
}


/**
 *  Main function for patching the hooks.
 */
void GameInit_Hooks()
{
    Patch_Jump(0x004E0786, &_Init_Game_Skip_Startup_Movies_Patch);
    Patch_Jump(0x004E3D20, &Vinifera_Init_Bootstrap_Mixfiles);
    Patch_Jump(0x004E4120, &Vinifera_Init_Secondary_Mixfiles);
    Patch_Jump(0x004E0461, &_Init_CDROM_Access_Local_Files_Patch);
    Patch_Jump(0x004E45F0, &_Init_Bulk_Data_Cache_Mix_Files_Patch);
}
