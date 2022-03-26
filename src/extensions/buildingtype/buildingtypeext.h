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
#include "vinifera_defines.h"


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
        void Init(TheaterType theater);

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
         *	For each stage that a building may be in, its animation is controlled
         *	by this structure. It dictates the starting and length of the animation
         *	frames needed for the specified state. In addition it specifies how long
         *	to delay between changes in animation. With this data it is possible to
         *	control the appearance of all normal buildings. Turrets and SAM sites are
         *	an exception since their animation is not merely cosmetic.
         */
        struct NewAnimControlStruct {
            int Start;
            int Count;
            int Rate;
        } Anims[NEW_BSTATE_COUNT];

        /**
         *  
         */
        const ShapeFileStruct * BuilddownData;
};


extern ExtensionMap<BuildingTypeClass, BuildingTypeClassExtension> BuildingTypeClassExtensions;
