/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BINK.H
 *
 *  @author        CCHyper
 *
 *  @brief         Header containing bare basic structures and types required
 *                 for interacting with the Bink library.
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


/**
 *  The following will have been larger structs in the real Bink SDK headers, but
 *  are only accessed through pointers so we don't care unless access to the internals
 *  is required.
 */


typedef struct BINK * HBINK;
typedef struct BINKBUFFER * HBINKBUFFER;

struct BINKSND;


#define BINKFRAMERATE         0x00001000L
#define BINKPRELOADALL        0x00002000L
#define BINKSNDTRACK          0x00004000L
#define BINKOLDFRAMEFORMAT    0x00008000L
#define BINKRBINVERT          0x00010000L
#define BINKGRAYSCALE         0x00020000L
#define BINKNOMMX             0x00040000L
#define BINKNOSKIP            0x00080000L
#define BINKALPHA             0x00100000L
#define BINKNOFILLIOBUF       0x00200000L
#define BINKSIMULATE          0x00400000L
#define BINKFILEHANDLE        0x00800000L
#define BINKIOSIZE            0x01000000L
#define BINKIOPROCESSOR       0x02000000L
#define BINKFROMMEMORY        0x04000000L
#define BINKNOTHREADEDIO      0x08000000L

#define BINKSURFACEFAST       0x00000000L
#define BINKSURFACESLOW       0x08000000L
#define BINKSURFACEDIRECT     0x04000000L

#define BINKCOPYALL           0x80000000L
#define BINKCOPY2XH           0x10000000L
#define BINKCOPY2XHI          0x20000000L
#define BINKCOPY2XW           0x30000000L
#define BINKCOPY2XWH          0x40000000L
#define BINKCOPY2XWHI         0x50000000L
#define BINKCOPY1XI           0x60000000L
#define BINKCOPYNOSCALING     0x70000000L

#define BINKSURFACE8P          0
#define BINKSURFACE24          1
#define BINKSURFACE24R         2
#define BINKSURFACE32          3
#define BINKSURFACE32R         4
#define BINKSURFACE32A         5
#define BINKSURFACE32RA        6
#define BINKSURFACE4444        7
#define BINKSURFACE5551        8
#define BINKSURFACE555         9
#define BINKSURFACE565        10
#define BINKSURFACE655        11
#define BINKSURFACE664        12
#define BINKSURFACEYUY2       13
#define BINKSURFACEUYVY       14
#define BINKSURFACEYV12       15
#define BINKSURFACEMASK       15

#define BINKGOTOQUICK          1
#define BINKGOTOQUICKSOUND     2

#define BINKGETKEYPREVIOUS     0
#define BINKGETKEYNEXT         1
#define BINKGETKEYCLOSEST      2
#define BINKGETKEYNOTEQUAL   128

#define BINKBUFFERSTRETCHXINT    0x80000000
#define BINKBUFFERSTRETCHX       0x40000000
#define BINKBUFFERSHRINKXINT     0x20000000
#define BINKBUFFERSHRINKX        0x10000000
#define BINKBUFFERSTRETCHYINT    0x08000000
#define BINKBUFFERSTRETCHY       0x04000000
#define BINKBUFFERSHRINKYINT     0x02000000
#define BINKBUFFERSHRINKY        0x01000000
#define BINKBUFFERSCALES         0xff000000
#define BINKBUFFERRESOLUTION     0x00800000

#define BINKBUFFERAUTO                0
#define BINKBUFFERPRIMARY             1
#define BINKBUFFERDIBSECTION          2
#define BINKBUFFERYV12OVERLAY         3
#define BINKBUFFERYUY2OVERLAY         4
#define BINKBUFFERUYVYOVERLAY         5
#define BINKBUFFERYV12OFFSCREEN       6
#define BINKBUFFERYUY2OFFSCREEN       7
#define BINKBUFFERUYVYOFFSCREEN       8
#define BINKBUFFERRGBOFFSCREENVIDEO   9
#define BINKBUFFERRGBOFFSCREENSYSTEM 10
#define BINKBUFFERLAST               10
#define BINKBUFFERTYPEMASK           31


typedef int (__stdcall * BINKSNDOPEN)(struct BINKSND *BnkSnd, unsigned int freq, int bits, int chans, int flags, HBINK bink);
typedef BINKSNDOPEN (__stdcall * BINKSNDSYSOPEN)(unsigned int param);
typedef void (__stdcall * BINKSNDRESET)(struct BINKSND *BnkSnd);
typedef int (__stdcall * BINKSNDREADY)(struct BINKSND *BnkSnd);
typedef int (__stdcall * BINKSNDLOCK)(struct BINKSND *BnkSnd, unsigned char **addr, unsigned int *len);
typedef int (__stdcall * BINKSNDUNLOCK)(struct BINKSND *BnkSnd, unsigned int filled);
typedef void (__stdcall * BINKSNDVOLUME)(struct BINKSND *BnkSnd, int volume);
typedef void (__stdcall * BINKSNDPAN)(struct BINKSND *BnkSnd, int pan);
typedef int (__stdcall * BINKSNDOFF)(struct BINKSND *BnkSnd, int status);
typedef int (__stdcall * BINKSNDPAUSE)(struct BINKSND *BnkSnd, int status);
typedef void (__stdcall * BINKSNDCLOSE)(struct BINKSND *BnkSnd);


typedef struct BINKRECT
{
    int Left;
    int Top;
    int Width;
    int Height;
} BINKRECT;

typedef struct BINKSND
{
    BINKSNDRESET SetParam;
    BINKSNDRESET Reset;
    BINKSNDREADY Ready;
    BINKSNDLOCK Lock;
    BINKSNDUNLOCK Unlock;
    BINKSNDVOLUME Volume;
    BINKSNDPAN Pan;
    BINKSNDPAUSE Pause;
    BINKSNDOFF Off;
    BINKSNDCLOSE Close;
    unsigned int BestSizeIn16;
    unsigned int SoundDroppedOut;
    unsigned int freq;
    int bits;
    int chans;
    unsigned char snddata[128];
} BINKSND;

typedef struct BINKPLANE
{
    int Allocate;
    void *Buffer;
    unsigned int BufferPitch;
} BINKPLANE;


typedef struct BINKFRAMEPLANESET
{
  BINKPLANE YPlane;
  BINKPLANE cRPlane;
  BINKPLANE cBPlane;
  BINKPLANE APlane;
} BINKFRAMEPLANESET;

typedef struct BINKFRAMEBUFFERS
{
    int TotalFrames;
    unsigned int YABufferWidth;
    unsigned int YABufferHeight;
    unsigned int cRcBBufferWidth;
    unsigned int cRcBBufferHeight;
    unsigned int FrameNum;
    BINKFRAMEPLANESET Frames[2];
} BINKFRAMEBUFFERS;

typedef struct BINKBUFFER
{
    unsigned int Width;
    unsigned int Height;
    unsigned int WindowWidth;
    unsigned int WindowHeight;
    unsigned int SurfaceType;
    void *Buffer;
    int BufferPitch;
    int ClientOffsetX;
    int ClientOffsetY;
    unsigned int ScreenWidth;
    unsigned int ScreenHeight;
    unsigned int ScreenDepth;
    unsigned int ExtraWindowWidth;
    unsigned int ExtraWindowHeight;
    unsigned int ScaleFlags;
    unsigned int StretchWidth;
    unsigned int StretchHeight;

    int surface;
    void *ddsurface;
    void *ddclipper;
    int destx;
    int desty;
    int wndx;
    int wndy;
    void *wnd;
    int minimized;
    int ddoverlay;
    int ddoffscreen;
    int lastovershow;

    int issoftcur;
    unsigned int cursorcount;
    void *buffertop;
    unsigned int type;
    int noclipping;

    int loadeddd;
    int loadedwin;

    void *dibh;
    void *dibbuffer;
    int dibpitch;
    void *dibinfo;
    void *dibdc;
    void *diboldbitmap;
} BINKBUFFER;

typedef struct BINK
{
    unsigned int Width;
    unsigned int Height;
    unsigned int Frames;
    unsigned int FrameNum;
    unsigned int LastFrameNum;
    unsigned int FrameRate;
    unsigned int FrameRateDiv;
    unsigned int ReadError;
    unsigned int OpenFlags;
    unsigned int BinkType;
    unsigned int Size;
    unsigned int FrameSize;
    unsigned int SndSize;
    unsigned int FrameChangePercent;
    BINKRECT FrameRects[8];
    int NumRects;
    BINKFRAMEBUFFERS *FrameBuffers;
    void *MaskPlane;
    unsigned int MaskPitch;
    unsigned int MaskLength;
    void *AsyncMaskPlane;
    void *InUseMaskPlane;
    void *LastMaskPlane;
    unsigned int LargestFrameSize;
    unsigned int InternalFrames;
    int NumTracks;
    unsigned int Highest1SecRate;
    unsigned int Highest1SecFrame;
    int Paused;
} BINK;


/**
 *  Use this define and don't call BinkOpenDirectSound directly!
 */
#define BinkSoundUseDirectSound(lpDS) BinkSetSoundSystem((BINKSNDSYSOPEN *)BinkOpenDirectSound, (unsigned int)lpDS)

/**
 *  Function typedefs.
 */
typedef int (__stdcall * BINKSETSOUNDSYSTEM)(BINKSNDSYSOPEN *open, unsigned int param);
typedef void * (__stdcall * BINKOPENDIRECTSOUND)(unsigned int param);
typedef char * (__stdcall * BINKGETERROR)(void);
typedef HBINK (__stdcall * BINKOPEN)(const char *name, unsigned int flags);
typedef void (__stdcall * BINKCLOSE)(HBINK bnk);
typedef int (__stdcall * BINKDDSURFACETYPE)(void *lpDDS);
typedef void (__stdcall * BINKGOTO)(HBINK bnk, unsigned int frame, unsigned int flags);  // use 1 for the first frame
typedef int (__stdcall * BINKPAUSE)(HBINK bnk, int pause);
typedef void (__stdcall *BINKNEXTFRAME)(HBINK bnk);
typedef int (__stdcall * BINKCOPYTOBUFFER)(HBINK bnk, void *dest, int destpitch, unsigned int destheight, unsigned int destx, unsigned int desty, unsigned int flags);
typedef int (__stdcall * BINKDOFRAME)(HBINK bnk);
typedef int (__stdcall * BINKWAIT)(HBINK bnk);
typedef int (__stdcall * BINKSETVOLUME)(HBINK bnk, int volume);
typedef void (__stdcall BINKSETPAN)(HBINK bnk, int volume);
typedef HBINKBUFFER (__stdcall * BINKBUFFEROPEN)(void *wnd, unsigned int width, unsigned int height, unsigned int bufferflags);
typedef void (__stdcall * BINKBUFFERCLOSE)(HBINKBUFFER buf);
typedef int (__stdcall * BINKBUFFERLOCK)(HBINKBUFFER buf);
typedef int (__stdcall * BINKBUFFERUNLOCK)(HBINKBUFFER buf);
typedef void (__stdcall * BINKBUFFERBLIT)(HBINKBUFFER buf, BINKRECT *rects, unsigned int numrects);
typedef int  (__stdcall * BINKGETRECTS)(HBINK bnk, unsigned int flags);
typedef int (__stdcall * BINKBUFFERSETSCALE)(HBINKBUFFER buf, unsigned int w, unsigned int h);
typedef int (__stdcall * BINKBUFFERSETOFFSET)(HBINKBUFFER buf, unsigned int destx, unsigned int desty);
typedef void (__stdcall * BINKSETSOUNDTRACK)(unsigned int total_tracks);
