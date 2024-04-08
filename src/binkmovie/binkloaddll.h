/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BINK_LOAD_DLL.H
 *
 *  @author        CCHyper
 *
 *  @brief         Utility functions for performing one-time loading of
 *                 Bink library functions from the DLL.
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
#include "bink.h"


extern bool BinkImportsLoaded;


bool Load_Bink_DLL();
void Unload_Bink_DLL();


extern BINKCLOSE BinkClose;
extern BINKDDSURFACETYPE BinkDDSurfaceType;
extern BINKSETVOLUME BinkSetVolume;
extern BINKGETERROR BinkGetError;
extern BINKOPEN BinkOpen;
extern BINKSETSOUNDSYSTEM BinkSetSoundSystem;
extern BINKOPENDIRECTSOUND BinkOpenDirectSound;
extern BINKGOTO BinkGoto;
extern BINKPAUSE BinkPause;
extern BINKNEXTFRAME BinkNextFrame;
extern BINKCOPYTOBUFFER BinkCopyToBuffer;
extern BINKDOFRAME BinkDoFrame;
extern BINKWAIT BinkWait;
extern BINKBUFFEROPEN BinkBufferOpen;
extern BINKBUFFERCLOSE BinkBufferClose;
extern BINKBUFFERLOCK BinkBufferLock;
extern BINKBUFFERUNLOCK BinkBufferUnlock;
extern BINKBUFFERBLIT BinkBufferBlit;
extern BINKGETRECTS BinkGetRects;
extern BINKBUFFERSETSCALE BinkBufferSetScale;
extern BINKBUFFERSETOFFSET BinkBufferSetOffset;
extern BINKSETSOUNDTRACK BinkSetSoundTrack;
