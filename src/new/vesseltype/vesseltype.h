/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VESSELTYPE.H
 *
 *  @authors       CCHyper
 *
 *  @brief         Class for vessel types.
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

#include "technotype.h"
#include "tpoint.h"
#include "tibsun_defines.h"
#include "vinifera_defines.h"


/**
 *  This specifies the constant attribute data associated with naval vessels.
 */
class DECLSPEC_UUID(CLSID_VESSEL_TYPE)
VesselTypeClass : public TechnoTypeClass
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
        VesselTypeClass(const char *ini_name = nullptr);
        VesselTypeClass(const NoInitClass &noinit);
        virtual ~VesselTypeClass();

        /**
         *  AbstractClass
         */
        virtual RTTIType Kind_Of() const override;
        virtual int Size_Of(bool firestorm = false) const override;
        virtual int entry_34() const override;
        virtual void Compute_CRC(WWCRCEngine &crc) const override;
        virtual int Get_Heap_ID() const override;

        /**
         *  AbstractTypeClass
         */
        virtual bool Read_INI(CCINIClass &ini) override;

        /**
         *  ObjectTypeClass
         */
        virtual Coordinate Coord_Fixup(Coordinate *coord) const override;
        virtual Point3D Pixel_Dimensions() const override;
        virtual Point3D Lepton_Dimensions() const override;
        virtual bool Create_And_Place(Cell &cell, HouseClass *house = nullptr) const override;
        virtual ObjectClass *const Create_One_Of(HouseClass *house = nullptr) const override;

        /**
         *  TechnoTypeClass
         */
        virtual int Repair_Step() const override;

        const TPoint3D<int> Turret_Adjust(DirType dir, TPoint3D<int> &pos) const;

        static const VesselTypeClass &As_Reference(VesselType type);
        static const VesselTypeClass *As_Pointer(VesselType type);
        static const VesselTypeClass &As_Reference(const char *name);
        static const VesselTypeClass *As_Pointer(const char *name);
        static VesselType From_Name(const char *name);
        static const char *Name_From(VesselType type);
        static const VesselTypeClass *Find_Or_Make(const char *name);

        static void One_Time();

    public:
        /**
         *  This value represents the unit class. It can serve as a unique
         *  identification number for this unit class.
         */
        VesselType Type;
};
