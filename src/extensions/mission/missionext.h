/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MISSIONEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended MissionClassExtension class.
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

#include "objectext.h"
#include "mission.h"


class MissionClassExtension : public ObjectClassExtension
{
    public:
        /**
         *  IPersistStream
         */
        IFACEMETHOD(Load)(IStream *pStm);
        IFACEMETHOD(Save)(IStream *pStm, BOOL fClearDirty);

    public:
        MissionClassExtension(const MissionClass *this_ptr);
        MissionClassExtension(const NoInitClass &noinit);
        virtual ~MissionClassExtension();

        virtual void Detach(TARGET target, bool all = true) override;
        virtual void Compute_CRC(WWCRCEngine &crc) const override;

        virtual MissionClass *This() const override { return reinterpret_cast<MissionClass *>(ObjectClassExtension::This()); }
        virtual const MissionClass *This_Const() const override { return reinterpret_cast<const MissionClass *>(ObjectClassExtension::This_Const()); }

        void AI();

    protected:
        virtual int Mission_Paradrop_Approach();
        virtual int Mission_Paradrop_Overfly();
        virtual int Mission_Spyplane_Approach();
        virtual int Mission_Spyplane_Overfly();

    public:
};
