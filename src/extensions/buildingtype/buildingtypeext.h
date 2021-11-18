/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BUILDINGTYPEEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended BuildingTypeClass class.
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


class BuildingTypeClass;
class CCINIClass;


class BuildingTypeClassExtension final : public Extension<BuildingTypeClass>
{
    public:
        BuildingTypeClassExtension(BuildingTypeClass *this_ptr);
        BuildingTypeClassExtension(const NoInitClass &noinit);
        ~BuildingTypeClassExtension();

        virtual HRESULT Load(IStream *pStm) override;
        virtual HRESULT Save(IStream *pStm, BOOL fClearDirty) override;
        virtual int Size_Of() const override;

        virtual void Detach(TARGET target, bool all = true) override;
        virtual void Compute_CRC(WWCRCEngine &crc) const override;

        bool Read_INI(CCINIClass &ini);

    protected:
        bool Read_Foundation(CCINIClass &ini, const char *section);

    public:
        /**
         *  This is the sound effect to play when the animation of the gate is rising.
         */
        VocType GateUpSound;

        /**
         *  This is the sound effect to play when the animation of the gate is lowering.
         */
        VocType GateDownSound;

        /**
         *  Credits bonus when captured from a house with "IsMultiplayPassive" set.
         */
        unsigned ProduceCashStartup;
        
        /**
         *  Amount every give to/take from the house every delay.
         */
        int ProduceCashAmount;
        
        /**
         *  Frame delay between amounts.
         */
        unsigned ProduceCashDelay;
        
        /**
         *  The total budget for this building.
         */
        unsigned ProduceCashBudget;
        
        /**
         *  Is the capture bonus a "one one" special (further captures will not get the bonus)?
         */
        bool IsStartupCashOneTime;
        
        /**
         *  Reset the available budget when captured?
         */
        bool IsResetBudgetOnCapture;

        /**
         *  Is this building eligible for proximity checks by players who are its owner's allies?
         */
        bool IsEligibleForAllyBuilding;

        /**
         *  This is the custom size of the building. These are used to
         *  calculate the "footprint" of the building.
         */
        //TPoint2D<int> CustomSize;

        /**
         *  This is a pointer to a list of offsets (from the upper left corner) that
         *  are used to indicate the building's "footprint". This footprint is used
         *  to determine building placement legality and terrain possibility.
         */
        //Cell *CustomOccupyList;

        /**
         *  When determine which cell to head toward when exiting a building, use the
         *  list elaborated by this variable. There are directions of exit that are
         *  more suitable than others. This list is here to inform the system which
         *  directions those are.
         */
        //Cell *CustomExitList;

        /**
         *  The cell override in which war factory units "exit" from.
         */
        Cell FactoryExitCell;
};


extern ExtensionMap<BuildingTypeClass, BuildingTypeClassExtension> BuildingTypeClassExtensions;
