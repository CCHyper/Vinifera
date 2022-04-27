/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       ProjectTSYR (Common Library)
 *
 *  @file          AUDIO_NEWVOC.CPP
 *
 *  @author        CCHyper, tomsons26
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
 ******************************************************************************/
#include "audio_voc.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "audio_driver.h"
#include "audio_util.h"
#include "tactical.h"
#include "iomap.h"
#include "options.h"
#include "voc.h"


/**
 *  Preload the audio engine with the sound effect files.
 * 
 *  @author: CCHyper
 */
void NewVocClass::Preload()
{
    if (Audio_Driver()->Is_Available()) {

        /**
         *  Clear the existing speech bank (user may have changed side or disk).
         */
        Audio_Driver()->Clear_Sample_Bank(SAMPLE_SFX);

        for (int index = 0; index < Vocs.Count(); ++index) {

            VocClass *voc = Vocs[index];
            if (!voc) {
                continue;
            }

            Wstring sfxname = voc->Filename;
            sfxname.As_Upper();

            /**
             *  Check to see if the speech file exists, request preload of asset if available.
             */
            if (!sfxname.Is_Empty()) {
                if (Audio_Driver()->Is_Audio_File_Available(sfxname)) {
                    bool available = Audio_Driver()->Request_Preload(sfxname, SAMPLE_SFX);
                    if (!available) {
                        DEBUG_WARNING("SoundEffect: Failed to preload \"%s\"!\n", sfxname.Peek_Buffer());
                    }
                } else {
                    DEV_DEBUG_WARNING("SoundEffect: Unable to find \"%s\"!\n", sfxname.Peek_Buffer());
                }
            }

        }

        /**
         *  Flag the preloader thread to begin.
         */
        Audio_Driver()->Start_Preloader(SAMPLE_SFX);
    }
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int NewVocClass::Play(VocType voc, int a2, float volume)
{
    if (voc == VOC_NONE) {
        return INVALID_AUDIO_HANDLE;
    }

    if (Options.SoundVolume <= 0.0f) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!Audio_Driver()->Is_Available()) {
        return INVALID_AUDIO_HANDLE;
    }

    VocClass *vocptr = Vocs[voc];

    if (!vocptr->FilePtr) {
        return INVALID_AUDIO_HANDLE;
    }

    Wstring filename = vocptr->Filename;

    float playvol = std::clamp((Options.SoundVolume * vocptr->Volume * volume), 0.0f, 1.0f);

    Audio_Driver()->Play_SoundEffect(filename, playvol);

    /**
     *  New audio interface does not use sample handles, so return 0.
     */
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int NewVocClass::Play(VocType voc, float volume)
{
    if (voc == VOC_NONE) {
        return INVALID_AUDIO_HANDLE;
    }

    if (Options.SoundVolume <= 0.0f) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!Audio_Driver()->Is_Available()) {
        return INVALID_AUDIO_HANDLE;
    }

    VocClass *vocptr = Vocs[voc];

    if (!vocptr->FilePtr) {
        return INVALID_AUDIO_HANDLE;
    }

    Wstring filename = vocptr->Filename;

    float vol = std::clamp((Options.SoundVolume * vocptr->Volume * volume), 0.0f, 1.0f);

    Audio_Driver()->Play_SoundEffect(filename, vol);

    /**
     *  New audio interface does not use sample handles, so return 0.
     */
    return 0;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
static void Voc_Calculate_Pan_And_Volume(VocClass &voc, Coordinate &coord, float &pan_result, float &volume_result)
{
    pan_result = 0.0f;
    volume_result = 1.0f;

    Cell cell = Coord_Cell(coord);

    if (!cell) {
        return;
    }

    float pan = 0.0f;
    float volume = 1.0f;

    Point2D pixel;

    if (!TacticalMap->Coord_To_Pixel(coord, pixel)) {

        int xpos = std::abs(pixel.X);
        int ypos = std::abs(pixel.Y);

    }

    pan_result = pan;
    volume_result = volume;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
int NewVocClass::Play(VocType voc, Coordinate &coord)
{
    if (voc == VOC_NONE) {
        return INVALID_AUDIO_HANDLE;
    }

    if (Options.SoundVolume <= 0.0f) {
        return INVALID_AUDIO_HANDLE;
    }

    if (!Audio_Driver()->Is_Available()) {
        return INVALID_AUDIO_HANDLE;
    }

    VocClass *vocptr = Vocs[voc];

    if (!vocptr->FilePtr) {
        return INVALID_AUDIO_HANDLE;
    }

    float vol = 1.0f;
    float pan = 0.0f;

    Voc_Calculate_Pan_And_Volume(*vocptr, coord, pan, vol);

    Wstring filename = vocptr->Filename;

    vol = std::clamp(vol, 0.0f, 1.0f);
    pan = std::clamp(pan, -1.0f, 1.0f);

    Audio_Driver()->Play_SoundEffect(filename, vol, pan);

    /**
     *  New audio interface does not use sample handles, so return 0.
     */
    return 0;
}
