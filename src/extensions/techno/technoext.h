/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TECHNOEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended TechnoClass class.
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

#include "extension.h"
#include "container.h"
#include "techno.h"

#include "colorscheme.h"
#include "textprint.h"
#include "vector.h"


class TechnoClassExtension final : public Extension<TechnoClass>
{
    public:
        TechnoClassExtension(TechnoClass *this_ptr);
        TechnoClassExtension(const NoInitClass &noinit);
        ~TechnoClassExtension();

        virtual HRESULT Load(IStream *pStm) override;
        virtual HRESULT Save(IStream *pStm, BOOL fClearDirty) override;
        virtual int Size_Of() const override;

        virtual void Detach(TARGET target, bool all = true) override;
        virtual void Compute_CRC(WWCRCEngine &crc) const override;

    public:
        /**
         *  An instance of the floating string.
         */
        class FloatingStringClass
        {
            public:
                FloatingStringClass(const char *string, ColorSchemeType color, TextPrintType style, Point3D &pos, int frame_time_out, int frame_count, int rate);
                ~FloatingStringClass();

                bool AI();

            public:
                const char *String;         // The text string to display.
                ColorSchemeType Color;      // Text color scheme.
                TextPrintType Style;        // Text print style (font etc).
                Point3D Position;           // Starting location
                int FrameTimeOut;
                int FrameCount;             // The number of frames this string lasts for.
                int Rate;                   // The rate at which this string moves.
                bool IsNormalized;          // Is the rate normalised to the game speed?

            private:
                WWFontClass *Font;          // Pointer to the style font.
                bool IsExpired;             // Has this string expired and ready is to delete?
        };

    public:
        bool Add_Floating_String(const char *string, ColorSchemeType color, TextPrintType style, Point3D &pos, int frame_time_out, int frame_count, int rate);
        bool Clear_Floating_Strings();

    public:
        /**
         *  Vector of all floating strings for this object.
         */
        DynamicVectorClass<FloatingStringClass *> FloatingStrings;
};


extern ExtensionMap<TechnoClass, TechnoClassExtension> TechnoClassExtensions;
