/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          XAUDIO2_DRIVER.H
 *
 *  @author        CCHyper
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
#pragma once

#include "always.h"
#include "ccfile.h"
#include "wstring.h"


class XAudio2Stream;
class CCFileClass;


enum XAudio2FileFormatType
{
    FORMAT_OGG,

    FORMAT_COUNT,

    FORMAT_NONE = -1,
};


/**
 *  x
 */
class XAudio2SoundResource
{
    public:
        XAudio2SoundResource() :
            Type(FORMAT_NONE),
            Name(),
            FullName(),
            IsAvailable(false)
        {
        }

        virtual ~XAudio2SoundResource()
        {
        }

        bool operator != (const XAudio2SoundResource & that) const { return Type != that.Type && Name != that.Name; }
        bool operator == (const XAudio2SoundResource & that) const { return Type == that.Type && Name == that.Name; }

        virtual XAudio2FileFormatType Get_Type() const { return Type; }
        virtual Wstring Get_Name() const { return Name; }
        virtual Wstring Get_FullName() const { return FullName; }

        virtual bool Load(Wstring fname) = 0;
        virtual bool Is_Available() const { return IsAvailable; }
        virtual std::unique_ptr<CCFileClass> Get_Unique_File_Handle() const = 0;

    protected:
        /**
         *  x
         */
        XAudio2FileFormatType Type;

        /**
         *  x
         */
        Wstring Name;
        Wstring FullName;

        /**
         *  Is the resource loaded and ready for use?
         */
        bool IsAvailable;
};


/**
 *  x
 */
class XAudio2CCSoundResource final : public XAudio2SoundResource
{
    public:
        XAudio2CCSoundResource() : XAudio2SoundResource() {}
        virtual ~XAudio2CCSoundResource() {}

        virtual bool Load(Wstring fname) override;
        virtual bool Is_Available() const override { return IsAvailable; }
        virtual std::unique_ptr<CCFileClass> Get_Unique_File_Handle() const override { return std::make_unique<CCFileClass>(FullName.Peek_Buffer()); }
};


std::unique_ptr<XAudio2Stream> XAudio2_Create_Sample_From_Resource(XAudio2SoundResource *res);
bool XAudio2_Is_File_Available(Wstring filename);
unsigned int XAudio2_Get_Filename_Hash(Wstring filename);
