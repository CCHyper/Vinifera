/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SUBTITLEMANAGER.CPP
 *
 *  @author        CCHyper, tomsons26
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
 ******************************************************************************/
#include "subtitlemanager.h"
#include "subtitleparser.h"
#include "subtitle.h"
#include "tibsun_globals.h"
#include "ccfile.h"
#include "filestraw.h"
#include "bsurface.h"
#include "dsurface.h"
#include "textprint.h"
#include "wwfont.h"
#include "debughandler.h"
#include "asserthandler.h"


SubTitleManager::SubTitleManager() :
    SubTitles(nullptr),
    CurrentIndex(0),
    CurrentSubTitle(nullptr),
    Drawer(nullptr),
    SurfacePtr(nullptr),
    FontPtr(nullptr),
    CurrentRect(),
    LastRect()
{
}


SubTitleManager::~SubTitleManager()
{
    if (SubTitles) { 
        SubTitles->Clear();
    }

    delete SubTitles;
    SubTitles = nullptr;

    delete SurfacePtr;
    SurfacePtr = nullptr;
}


bool SubTitleManager::Init(const char *moviename)
{
    if (!moviename || !std::strlen(moviename)) {
        return false;
    }

    if (FontPtr) {
        FontPtr = nullptr;
    }

    /**
     *  File not found? Abandon ship!
     */
    CCFileClass subsfile("SUBTITLE.TXT");
    if (!subsfile.Is_Available()) {
        return false;
    }

    /**
     *  Parse the subtitle data from the file.
     */
    FileStraw fstraw(subsfile);
    SubTitleParser subsparser(fstraw);

    SubTitles = subsparser.Parse(moviename);

    if (!SubTitles) {
        return false;
    }

    FontPtr = Font_Ptr(TPF_METAL12);
    ASSERT(FontPtr != nullptr);

    return true;
}


bool SubTitleManager::Update(unsigned time)
{
    if (!SubTitles) {
        return false;
    }

    bool drawn = false;

    if (CurrentIndex < SubTitles->Count()) {

        for (int i = CurrentIndex; i < SubTitles->Count();) {

            SubTitle *sub = (*SubTitles)[i];

            if (sub->Time > time) {
                break;
            }

            CurrentSubTitle = sub;
            CurrentIndex = i+1;

            /**
             *  If we have a custom drawer assigned then use this.
             */
            if (Drawer) {
                Drawer(sub);

            } else {
                Draw(sub);
                drawn = true;
            }

            i = CurrentIndex;
        }
    }

    if (CurrentSubTitle) {

        /**
         *  If the requested timestamp is greater than our start time plus duration, then
         *  we need to clear the draw surface to be ready for the next draw request.
         */
        if (time >= (CurrentSubTitle->Time + CurrentSubTitle->Duration)) {

            CurrentSubTitle = nullptr;
            
            /**
             *  A custom drawer will most likely draw to a location other than the video
             *  surface (example, Sidebar movie that outputs the text to the tactical
             *  message list. So we don't need to clear the video overlay surface.
             */
            if (!Drawer) {
                if (SurfacePtr) {

                    SurfacePtr->Clear(CurrentRect);

                    LastRect = CurrentRect;

                    /**
                     *  Reset the current rect values.
                     */
                    CurrentRect.X = 0;
                    CurrentRect.Y = 0;
                    CurrentRect.Width = 0;
                    CurrentRect.Height = 0;

                    drawn = true;
                }
            }
        }
    }

    return drawn;
}


void SubTitleManager::Set_Drawer(void (*drawer_func)(SubTitle *))
{
    Drawer = drawer_func;
}


void SubTitleManager::Draw(SubTitle *subtitle)
{
    if (SurfacePtr) {

        /**
         *  Clear the text area on the surface.
         */
        SurfacePtr->Clear(CurrentRect);

        int width = SurfacePtr->Get_Width();
        int height = SurfacePtr->Get_Height();

        Rect font_rect(0, 0, width, height);
        subtitle->FontPtr->String_Pixel_Rect(subtitle->TextBuffer, &font_rect);

        int draw_x = 0;
        int draw_y = subtitle->FontPtr->Get_Char_Height() * (subtitle->YAdjust - 1);

        /**
         *  Adjust the draw x position based on the subtitle desired alignment.
         */
        switch (subtitle->XAlignment) {
            default:
            case ALIGN_LEFT:
                // We don't need to adjust draw x position.
                break;
            case ALIGN_RIGHT:
                draw_x = width - font_rect.Width;
                break;
            case ALIGN_CENTER:
                draw_x = (width - font_rect.Width) / 2;
                break;
        };

        /**
         *  Draw text to the surface.
         */
        Simple_Text_Print(
            subtitle->TextBuffer,
            SurfacePtr,
            &SurfacePtr->Get_Rect(),
            &Point2D(draw_x, draw_y),
            NormalDrawer,
            subtitle->Color,
            subtitle->BackColor,
            subtitle->TextStyle);

        LastRect = CurrentRect;

        CurrentRect.X = draw_x;
        CurrentRect.Y = draw_y;
        CurrentRect.Width = font_rect.Width;
        CurrentRect.Height = font_rect.Height;
    }
}


SubTitleManager *SubTitleManager::Create_Subtitles(const char *videoname)
{
    if (!videoname || !std::strlen(videoname)) {
        return nullptr;
    }

    SubTitleManager *submgr = new SubTitleManager();
    if (!submgr) {
        return nullptr;
    }
    
    char fname[_MAX_FNAME];
    _splitpath(videoname, nullptr, nullptr, fname, nullptr);

    if (submgr->Init(fname)) {
        submgr->SurfacePtr = new BSurface(640, 400/2, 2);
        if (submgr->SurfacePtr) {
            submgr->SurfacePtr->Clear();
        }
    }

    return submgr;
}
