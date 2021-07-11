/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          HOUSEEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended HouseClass class.
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
#include "typelist.h"
#include "counter.h"
#include "vinifera_defines.h"


class HouseClass;
class CCINIClass;
class VesselTypeClass;
class UnitTrackerClass;
class FactoryClass;


class HouseClassExtension final : public Extension<HouseClass>
{
    public:
        HouseClassExtension(HouseClass *this_ptr);
        HouseClassExtension(const NoInitClass &noinit);
        ~HouseClassExtension();

        virtual HRESULT Load(IStream *pStm) override;
        virtual HRESULT Save(IStream *pStm, BOOL fClearDirty) override;
        virtual int Size_Of() const override;

        virtual void Detach(TARGET target, bool all = true) override;
        virtual void Compute_CRC(WWCRCEngine &crc) const override;

        int AI_Vessel();

        VesselTypeClass *Get_First_Ownable(TypeList<VesselTypeClass *> &list) const;

    public:
        /**
         *  This is a record of the last vessel that was built.
         */
        VesselType JustBuiltVessel;

        /**
         *  This is the running count of the number of vessels owned by this house
         *  and is used to keep track of ownership limits.
         */
        unsigned CurVessels;

        /**
         *  The total number of units built by this house.
         */
        UnitTrackerClass *VesselTotals;

        /**
         *  Total number of vessels destroyed by this house.
         */
        UnitTrackerClass *DestroyedVessels;

        /**
         *  Records the number of vessel factories active. This value is
         *  used to regulate the speed of production.
         */
        int VesselFactories;

        /**
         *  For human controlled houses, only one type of vessel can be produced
         *  at any one instant. This factory object controls this production.
         */
        FactoryClass *VesselFactory;

        /**
         *  Tracks number of each vessel type owned by this house. Even if the
         *  vessel is in construction, it will be reflected in this total.
         */
        TCounterClass<VesselType> VQuantity;
        TCounterClass<VesselType> ActiveVQuantity;
        TCounterClass<VesselType> FactoryVQuantity;

        /**
         *  This elaborates the suggested vessel to construct. When the specified
         *  object is constructed, then this value will be reset to nill state.
         *  The expert system decides what should be produced, and then records the
         *  recommendation in this variable.
         */
        VesselType BuildVessel;

        /**
         *  
         */
        int RatioTeamVessels;
};


extern ExtensionMap<HouseClass, HouseClassExtension> HouseClassExtensions;
