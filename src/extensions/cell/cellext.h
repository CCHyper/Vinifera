/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CELLEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended CellClass class.
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

#include "abstractext.h"
#include "cell.h"


class DECLSPEC_UUID(UUID_CELL_EXTENSION)
CellClassExtension final : public AbstractClassExtension
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
        CellClassExtension(const CellClass *this_ptr = nullptr);
        CellClassExtension(const NoInitClass &noinit);
        virtual ~CellClassExtension();

        virtual int Size_Of() const override;
        virtual void Detach(TARGET target, bool all = true) override;
        virtual void Compute_CRC(WWCRCEngine &crc) const override;

        virtual CellClass *This() const override { return reinterpret_cast<CellClass *>(AbstractClassExtension::This()); }
        virtual const CellClass *This_Const() const override { return reinterpret_cast<const CellClass *>(AbstractClassExtension::This_Const()); }
        virtual RTTIType What_Am_I() const override { return RTTI_CELL; }

        virtual const char *Name() const { return This()->Pos.As_String(); }
        virtual const char *Full_Name() const { return This()->Pos.As_String(); }

    public:
};
