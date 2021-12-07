/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ROCKETLOCOMOTION.H
 *
 *  @authors       CCHyper, tomsons26
 *
 *  @brief         Rocket locomotion.
 *
 *  @license       Vinifera is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 Vinifera is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY     without even the implied
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
#include "locomotion.h"
#include "ttimer.h"
#include "vinifera_defines.h"


class AircraftTypeClass;


typedef struct RocketControlStruct
{
    int PauseFrames;
    int TiltFrames;
    float PitchInitial;
    float PitchFinal;
    float TurnRate;
    float RaiseRate;
    float Acceleration;
    int Altitude;
    int Damage;
    int EliteDamage;
    int BodyLength;
    bool IsLazyCurve;
    AircraftTypeClass *Type;
} RocketControlStruct;


class DECLSPEC_UUID(CLSID_ROCKET_LOCOMOTOR)
RocketLocomotionClass : public LocomotionClass
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

        /**
         *  ILocomotion
         */
        IFACEMETHOD_(bool, Is_Moving)();
        IFACEMETHOD_(Coordinate, Destination)();
        IFACEMETHOD_(Matrix3D, Draw_Matrix)(int *key);
        IFACEMETHOD_(bool, Process)();
        IFACEMETHOD_(void, Move_To)(Coordinate to);
        IFACEMETHOD_(void, Stop_Moving)();
        IFACEMETHOD_(LayerType, In_Which_Layer)();
        IFACEMETHOD_(bool, Is_Moving_Now)();

    public:
        virtual int Size_Of(bool firestorm = false) const override { return sizeof(*this); }

    public:
        RocketLocomotionClass();
        ~RocketLocomotionClass();

    private:
        Coordinate func_00661FE0(int a1); // YR 00661FE0
        bool func_006620F0(RocketStruct &a1); // YR 006620F0
        float Get_Angle(); // YR 00662240
        void func_00663030(); // YR 00663030

    protected:
        Coordinate DestinationCoord;
        CDRateTimerClass<FrameTimerClass> field_24;
        CDRateTimerClass<FrameTimerClass> field_34;
        int field_44;
        int field_48;
        int field_4C;
        bool field_50;
        bool field_51;
        float field_54;
        int field_58;
}    
