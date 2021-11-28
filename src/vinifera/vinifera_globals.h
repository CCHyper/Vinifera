/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_GLOBALS.H
 *
 *  @authors       CCHyper
 *
 *  @brief         Vinifera global values.
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
#pragma once

#include "always.h"
#include "vector.h"
#include "ccfile.h"
#include "fmod_audio.h"


class EBoltClass;


extern bool Vinifera_DeveloperMode;

extern char Vinifera_DebugDirectory[PATH_MAX];
extern char Vinifera_ScreenshotDirectory[PATH_MAX];

extern char Vinifera_ProjectName[64];
extern char Vinifera_ProjectVersion[64];
extern char Vinifera_IconName[64];
extern char Vinifera_CursorName[64];

extern char Vinifera_MusicPath_EnvVar[PATH_MAX];
extern char Vinifera_MusicPath[PATH_MAX];


/**
 *  Defines and constants.
 */
#define TEXT_S_S					"%s: %s"


/**
 *  Developer mode globals.
 */
extern bool Vinifera_Developer_InstantBuild;
extern bool Vinifera_Developer_AIInstantBuild;
extern bool Vinifera_Developer_InstantSuperRecharge;
extern bool Vinifera_Developer_AIInstantSuperRecharge;
extern bool Vinifera_Developer_BuildCheat;
extern bool Vinifera_Developer_Unshroud;
extern bool Vinifera_Developer_ShowCursorPosition;
extern bool Vinifera_Developer_FrameStep;
extern int Vinifera_Developer_FrameStepCount;
extern bool Vinifera_Developer_AIControl;


/**
 *  Various globals.
 */
extern bool Vinifera_SkipWWLogoMovie;
extern bool Vinifera_SkipStartupMovies;

extern DynamicVectorClass<MFCC *> ViniferaMapsMixes;
extern DynamicVectorClass<MFCC *> ViniferaMoviesMixes;

extern MFCC *GenericMix;
extern MFCC *IsoGenericMix;


/**
 *  Global vectors and heaps.
 */
extern DynamicVectorClass<EBoltClass *> EBolts;


/**
 *  Skip to menus.
 */
extern bool Vinifera_SkipToTSMenu;
extern bool Vinifera_SkipToFSMenu;
extern bool Vinifera_SkipToLAN;
extern bool Vinifera_SkipToSkirmish;
extern bool Vinifera_SkipToCampaign;
extern bool Vinifera_SkipToInternet;
extern bool Vinifera_ExitAfterSkip;


/**
 *  New audio engine instance.
 */
//#ifdef NEW_AUDIO_ENGINE
//#define WWAudio Audio
//#else
extern FMODAudioClass FMODAudio; // Actually resides in fmod_audio.cpp
//#define WWAudio FMODAudio
//#endif
