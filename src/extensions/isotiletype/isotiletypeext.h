/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ISOTILETYPEEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended IsometricTileTypeClass class.
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

#include "objecttypeext.h"
#include "isotiletype.h"


class DECLSPEC_UUID(UUID_ISOTILE_EXTENSION)
IsometricTileTypeClassExtension final : public ObjectTypeClassExtension
{
    public:
        /**
         *  IPersist
         */
        IFACEMETHOD(GetClassID)(CLSID *pClassID);

        /**
         *  IPersistStream
         */
        IFACEMETHOD(Load)(IStream *pStm);
        IFACEMETHOD(Save)(IStream *pStm, BOOL fClearDirty);

    public:
        IsometricTileTypeClassExtension(const IsometricTileTypeClass *this_ptr = nullptr);
        IsometricTileTypeClassExtension(const NoInitClass &noinit);
        virtual ~IsometricTileTypeClassExtension();

        virtual int Size_Of() const override;
        virtual void Detach(TARGET target, bool all = true) override;
        virtual void Compute_CRC(WWCRCEngine &crc) const override;

        virtual IsometricTileTypeClass *This() const override { return reinterpret_cast<IsometricTileTypeClass *>(ObjectTypeClassExtension::This()); }
        virtual const IsometricTileTypeClass *This_Const() const override { return reinterpret_cast<const IsometricTileTypeClass *>(ObjectTypeClassExtension::This_Const()); }
        virtual ExtensionRTTIType What_Am_I() const override { return EXT_RTTI_ISOTILETYPE; }

        virtual bool Read_INI(CCINIClass &ini) override;

        /**
         *  Initialises theater control file globals.
         */
        static bool Init(CCINIClass &ini);

    public:
        /**
         *  What set is this tile type part of?
         */
        const char *TileSetName;
};
