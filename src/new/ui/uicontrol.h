/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          UICONTROL.H
 *
 *  @author        CCHyper
 *
 *  @brief         UI controls and overrides.
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
#include "tpoint.h"
#include "typelist.h"
#include "tibsun_defines.h"


struct IStream;
class CCINIClass;
class NoInitClass;


class UIControlsClass
{
    public:
        UIControlsClass();
        UIControlsClass(const NoInitClass &noinit);
        ~UIControlsClass();

        HRESULT Load(IStream *pStm);
        HRESULT Save(IStream *pStm, BOOL fClearDirty);
        int Size_Of() const;

        bool Read_INI(CCINIClass &ini);

    public:
        /**
         *  Health bar draw positions.
         */
        TPoint2D<int> UnitHealthBarDrawPos;
        TPoint2D<int> InfantryHealthBarDrawPos;

        /**
         *  Should the text label be drawn with an outline?
         */
        bool IsTextLabelOutline;

        /**
         *  Transparency of the text background.
         */
        unsigned TextLabelBackgroundTransparency;

        /**
         *  Should the tactical rubber band box be drawn with a drop shadow?
         */
        bool IsBandBoxDropShadow;

        /**
         *
         */
        bool IsBandBoxThick;

        /**
         *
         */
        RGBStruct BandBoxColor;

        /**
         *
         */
        RGBStruct BandBoxDropShadowColor;

        /**
         *  Transparency of the tactical rubber band.
         */
        unsigned BandBoxTintTransparency;

        /**
         *
         */
        TypeList<RGBStruct> BandBoxTintColors;

        /**
         *
         */
        bool IsMovementLineDashed;

        /**
         *
         */
        bool IsMovementLineDropShadow;

        /**
         *
         */
        bool IsMovementLineThick;

        /**
         *
         */
        RGBStruct MovementLineColor;

        /**
         *
         */
        RGBStruct MovementLineDropShadowColor;

        /**
         *
         */
        bool IsTargetLineDashed;
        bool IsTargetLineDropShadow;
        bool IsTargetLineThick;
        RGBStruct TargetLineColor;
        RGBStruct TargetLineDropShadowColor;

        /**
         *
         */
        bool IsTargetLaserDashed;
        bool IsTargetLaserDropShadow;
        bool IsTargetLaserThick;
        RGBStruct TargetLaserColor;
        RGBStruct TargetLaserDropShadowColor;

        /**
         *
         */
        unsigned TargetLaserTime;

        /**
         *
         */
        bool IsShowNavComQueueLines;

        /**
         *
         */
        bool IsNavComQueueLineDashed;
        bool IsNavComQueueLineDropShadow;
        bool IsNavComQueueLineThick;
        RGBStruct NavComQueueLineColor;
        RGBStruct NavComQueueLineDropShadowColor;
};

extern UIControlsClass *UIControls;
