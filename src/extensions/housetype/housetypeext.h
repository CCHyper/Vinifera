/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          HOUSETYPEEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended HouseTypeClass class.
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

#include "abstracttypeext.h"
#include "housetype.h"


class DECLSPEC_UUID(UUID_HOUSETYPE_EXTENSION)
HouseTypeClassExtension final : public AbstractTypeClassExtension
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
        HouseTypeClassExtension(const HouseTypeClass *this_ptr = nullptr);
        HouseTypeClassExtension(const NoInitClass &noinit);
        virtual ~HouseTypeClassExtension();

        virtual int Size_Of() const override;
        virtual void Detach(TARGET target, bool all = true) override;
        virtual void Compute_CRC(WWCRCEngine &crc) const override;
        
        virtual HouseTypeClass *This() const override { return reinterpret_cast<HouseTypeClass *>(AbstractTypeClassExtension::This()); }
        virtual const HouseTypeClass *This_Const() const override { return reinterpret_cast<const HouseTypeClass *>(AbstractTypeClassExtension::This_Const()); }
        virtual RTTIType What_Am_I() const override { return RTTI_HOUSETYPE; }

        virtual bool Read_INI(CCINIClass &ini) override;

    public:
        /**
         *  x
         */
        float BuildTimeAircraftMultiplier;
        float BuildTimeUnitsMultiplier;
        float BuildTimeInfantryMultiplier;
        float BuildTimeBuildingsMultiplier;
        float BuildTimeDefensesMultiplier;

        /**
         *  x
         */
        float ArmorAircraftMultiplier;
        float ArmorUnitsMultiplier;
        float ArmorInfantryMultiplier;
        float ArmorBuildingsMultiplier;
        float ArmorDefensesMultiplier;

        /**
         *  x
         */
        float CostAircraftMultiplier;
        float CostUnitsMultiplier;
        float CostInfantryMultiplier;
        float CostBuildingsMultiplier;
        float CostDefensesMultiplier;

        /**
         *  x
         */
        float SpeedAircraftMultiplier;
        float SpeedUnitsMultiplier;
        float SpeedInfantryMultiplier;
};
