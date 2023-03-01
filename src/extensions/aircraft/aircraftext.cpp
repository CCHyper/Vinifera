/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AIRCRAFTEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended AircraftClass class.
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
#include "aircraftext.h"
#include "aircraft.h"
#include "target.h"
#include "house.h"
#include "weapontype.h"
#include "team.h"
#include "iomap.h"
#include "rulesext.h"
#include "voc.h"
#include "wwcrc.h"
#include "extension.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
AircraftClassExtension::AircraftClassExtension(const AircraftClass *this_ptr) :
    FootClassExtension(this_ptr),
    ParadropsRemaining(5)
{
    //if (this_ptr) EXT_DEBUG_TRACE("AircraftClassExtension::AircraftClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    AircraftExtensions.Add(this);
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
AircraftClassExtension::AircraftClassExtension(const NoInitClass &noinit) :
    FootClassExtension(noinit)
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::AircraftClassExtension(NoInitClass) - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Class destructor.
 *  
 *  @author: CCHyper
 */
AircraftClassExtension::~AircraftClassExtension()
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::~AircraftClassExtension - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    AircraftExtensions.Delete(this);
}


/**
 *  Retrieves the class identifier (CLSID) of the object.
 *  
 *  @author: CCHyper
 */
HRESULT AircraftClassExtension::GetClassID(CLSID *lpClassID)
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::GetClassID - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    if (lpClassID == nullptr) {
        return E_POINTER;
    }

    *lpClassID = __uuidof(this);

    return S_OK;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT AircraftClassExtension::Load(IStream *pStm)
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::Load - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = FootClassExtension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) AircraftClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT AircraftClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::Save - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    HRESULT hr = FootClassExtension::Save(pStm, fClearDirty);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}


/**
 *  Return the raw size of class data for save/load purposes.
 *  
 *  @author: CCHyper
 */
int AircraftClassExtension::Size_Of() const
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::Size_Of - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void AircraftClassExtension::Detach(TARGET target, bool all)
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::Detach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void AircraftClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::Compute_CRC - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    crc(ParadropsRemaining);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
int AircraftClassExtension::Mission_Retreat()
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::Mission_Retreat - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    if (!Target_Legal(This()->NavCom)) {

        /**
         *  Head back to towards the map edge. When the edge is
         *  reached, the aircraft should be automatically eliminated.
         */
        SourceType source = This()->House->Get_Starting_Edge();
        Cell cell = Map.Calculated_Cell(source, INVALID_CELL, INVALID_CELL, SPEED_WINGED);
        if (cell) {
            DEBUG_INFO("Aircraft(Mission_Retreat) %s -> Dest Edge cell\n", Name());
            This()->Assign_Destination(&Map[cell]);
        }

    } else {

        if (This()->NavCom == This()->Get_Cell_Ptr()) {
            DEBUG_INFO("Aircraft(Mission_Retreat) %s -> NavCom is current cell, Dest NULL\n", Name());
            This()->Assign_Destination(TARGET_NULL);
        }

    }

    return 3;
}


/**
 *  Drop a passenger by parachute.
 *
 *  @author: CCHyper
 */
int AircraftClassExtension::Paradrop_Cargo()
{
    FootClass * passenger = This()->Cargo.Detach_Object();
    if (passenger) {

        --This()->Ammo;

        // staggers coords?
        //TODO: have styles for linear drops like RA1, and RA2 staggered drops.

        Coordinate spawn_coord = This()->Center_Coord();
        int spawn_distance = 512; // 128 = leptons.


        //Move_Coord(spawn_coord, DIR_N, spawn_distance);


        // TODO2: New Move_Coord function for moving forward or back?
        double radians = 
            (This()->Ammo & 1) != 0 ?
                            (double)(This()->PrimaryFacing.Current().Get_Raw() - 0x3FFF) :
                            (double)(This()->PrimaryFacing.Current().Get_Raw() + 0x3FFF)
            * -DEG_TO_RAD(360) / (UINT16_MAX - 1);

        spawn_coord.Y -= (WWMath::Sin(radians) * spawn_distance);
        spawn_coord.X += (WWMath::Cos(radians) * spawn_distance);



        CellClass *spawn_cell = &Map[spawn_coord];
        Coordinate closest_free = spawn_cell->Closest_Free_Spot(spawn_coord);

        if (passenger->Can_Enter_Cell(spawn_cell) != MOVE_OK
         || !closest_free
         || !passenger->Paradrop(closest_free)) {

            DEBUG_INFO("Aircraft(Paradrop_Cargo) %s -> Failed, Reattaching Cargo\n", Name());
            This()->Cargo.Attach(passenger);
            passenger->Hidden();
            ++This()->Ammo;

        } else {

            DEBUG_INFO("Aircraft(Paradrop_Cargo) %s -> Success (%s)\n", Name(), passenger->Name());

            /*
            **    Play a sound effect of the parachute opening.
            */
            Sound_Effect(Rule->ChuteSound, This()->Coord);

            if (This()->Team) {

                DEBUG_INFO("Aircraft(Paradrop_Cargo) %s -> Removed from team\n", Name());
                This()->Team->Remove(passenger);

                LastSightCoords = spawn_cell->Center_Coord();

                //DEBUG_INFO("Aircraft(Paradrop_Cargo) %s -> Assigned mission\n", Name());
                //if (passenger->House->IsHuman) {
                //    This()->Assign_Mission(MISSION_GUARD);
                //} else {
                //    This()->Assign_Mission(MISSION_HUNT);
                //}
            }
        }

        ParadropsRemaining = 5;

        This()->Arm = 0;
    }

    return 0;





// YR
#if 0
    passenger = CargoClass::Detach_Object(&this->f.t.cargo);
    if ( !passenger )
    {
        return 0;
    }
    v3 = this->f.t.r.m.o.a.vftable;
    --this->f.t.Ammo;
    v4 = v3->foot_vt.t.r.m.o.a.Center_Coord(this, &coord);
    a1.X = v4->X;
    a1.Y = v4->Y;
    v5 = v4->Z;
    LOBYTE(v4) = this->f.t.Ammo;
    a1.Z = v5;
    if ( (v4 & 1) != 0 )
    {
        LOWORD(v20) = FacingClass::Current(&this->f.t.PrimaryFacing, &v20)->un.Facing - 0x3FFF;
    }
    else
    {
        LOWORD(v20) = FacingClass::Current(&this->f.t.PrimaryFacing, &v20)->un.Facing + 0x3FFF;
    }
    v6 = a1.Z;
    LODWORD(v20) = SLOWORD(v20) - 0x3FFF;
    v7 = DIRMATH_BINARY_ANGLE_MAGIC_TO_RAD_DIV;
    *&coord.X = SLODWORD(v20) * v7;
    v8 = coord.Y;
    v20 = a1.Y;
    LODWORD(v20) = (v20 - FastMath::Sin(*&coord.X) * 128.0);
    v9 = (FastMath::Cos(COERCE_DOUBLE(__PAIR64__(v8, coord.X))) * 128.0 + a1.X);
    a1.Y = LODWORD(v20);
    a1.X = v9;
    a1.Z = v6;
    v10 = MapClass::operator[](&Map.sc.t.sb.p.r.d.m, &a1);
    if ( passenger->f.t.r.m.o.a.vftable->foot_vt.t.r.m.o.Can_Enter_Cell(passenger, v10, FACING_NONE, -1, 0, 1)
      || (v11 = CellClass::Closest_Free_Spot(v10, &coord, &a1, 0, 0, 0), v12 = v11->X, v13 = v11->Y, v14 = v11->Z, v12 == aircraft_defaultcoord.X)
      && v13 == aircraft_defaultcoord.Y
      && v14 == aircraft_defaultcoord.Z
      || (a1.Y = v13, a1.X = v12, !passenger->f.t.r.m.o.a.vftable->foot_vt.t.r.m.o.Paradrop(passenger, &a1)) )
    {
        CargoClass::Attach(&this->f.t.cargo, &passenger->f);
        passenger->f.t.r.m.o.a.vftable->foot_vt.t.r.m.o.Hidden(&passenger->f.t.r.m.o);
        ++this->f.t.Ammo;
        return 0;
    }
    coord = this->f.t.r.m.o.Coord;
    // Play a sound effect of the parachute opening.
    VocClass::Play_Ranged(Rule->ChuteSound, &coord, 0);
    v15 = MapClass::operator[](&Map.sc.t.sb.p.r.d.m, &a1);
    v16 = v15->a.vftable->foot_vt.t.r.m.o.a.Center_Coord(v15, &coord);
    LOWORD(v20) = v16->X / 256;
    WORD1(v20) = v16->Y / 256;
    passenger->f.__LastMapCoords = LODWORD(v20);
    v17 = this->f.Team;
    if ( v17 )
    {
        TeamClass::Remove(v17, &passenger->f, -1, 0);
    }
    v18 = Frame;
    this->ParadropsLeft = 5;
    this->f.t.Arm.Started = v18;
    this->f.t.Arm.Timer = a1.Y;
    result = 0;
    this->f.t.Arm.DelayTime = 0;
#endif
}


/**
 *  x
 *
 *  @author: CCHyper
 */
int AircraftClassExtension::Mission_Paradrop_Approach()
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::Mission_Paradrop_Approach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    LEPTON tarcom_distance = This()->Distance(This()->TarCom);

    if (!Target_Legal(This()->TarCom)) {

        DEBUG_INFO("Aircraft(Paradrop_Approach) %s -> Dest NULL, Mission RETREAT\n", Name());
        This()->Assign_Destination(TARGET_NULL);
        This()->Assign_Mission(MISSION_RETREAT);

    } else if (!Target_Legal(This()->NavCom)) {

        DEBUG_INFO("Aircraft(Paradrop_Approach) %s -> Dest TarCom\n", Name());
        This()->Assign_Destination(This()->TarCom);

    } else if (tarcom_distance <= RuleExtension->ParadropRadius) {

        DEBUG_INFO("Aircraft(Paradrop_Approach) %s -> In range (%d), Mission PARADROP_OVERFLY\n", Name(), tarcom_distance);
        This()->Assign_Mission(MISSION_PARADROP_OVERFLY);
        --ParadropsRemaining;

    }

    return 3;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
int AircraftClassExtension::Mission_Paradrop_Overfly()
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::Mission_Paradrop_Overfly - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    LEPTON tarcom_distance = This()->Distance(This()->TarCom);

    This()->IsLocked = true;

    if (Target_Legal(This()->TarCom) && This()->Cargo.Is_Something_Attached()) {

        if (tarcom_distance > RuleExtension->ParadropRadius) {

            DEBUG_INFO("Aircraft(Paradrop_Overfly) %s -> Out of range (%d)\n", Name(), tarcom_distance);

            This()->IsLocked = false;

            if (ParadropsRemaining > 0) {

                DEBUG_INFO("Aircraft(Paradrop_Overfly) %s -> Remaining %d, Mission PARADROP_APPROACH\n", Name(), ParadropsRemaining);
                This()->Assign_Mission(MISSION_PARADROP_APPROACH);

            } else {

                DEBUG_INFO("Aircraft(Paradrop_Overfly) %s -> Target NULL, Dest NULL, Mission RETREAT\n", Name());
                This()->Assign_Target(TARGET_NULL);
                This()->Assign_Destination(TARGET_NULL);
                This()->Assign_Mission(MISSION_RETREAT);

            }

        } else {

            DEBUG_INFO("Aircraft(Paradrop_Overfly) %s -> In range.\n", Name());

            if (Map.In_Radar(This()->Coord)) {
                DEBUG_INFO("Aircraft(Paradrop_Overfly) %s -> Paradrop Cargo\n", Name());
                Paradrop_Cargo(); //This()->Paradrop_Cargo();
            }

        }

    } else {

        This()->IsLocked = false;

        DEBUG_INFO("Aircraft(Paradrop_Overfly) %s -> Target NULL, Dest NULL, Mission RETREAT\n", Name());
        This()->Assign_Target(TARGET_NULL);
        This()->Assign_Destination(TARGET_NULL);
        This()->Assign_Mission(MISSION_RETREAT);

    }

    return 5;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
int AircraftClassExtension::Mission_Spyplane_Approach()
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::Mission_Spyplane_Approach - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    LEPTON tarcom_distance = This()->Distance(This()->TarCom);
    TARGET tarcom = This()->TarCom;


    // Temp hack!
    RuleExtension->SpyPlaneCameraSound = Rule->SellSound;


    if (Target_Legal(tarcom)) {

        if (Target_Legal(This()->NavCom)) {

            ASSERT(This()->Get_Weapon()->Weapon != nullptr);

            if (tarcom_distance <= This()->Get_Weapon()->Weapon->Range) {
                DEBUG_INFO("Aircraft(Spyplane_Approach) %s -> In range, Look, Play sound\n", Name());
                TechnoClassExtension::Look(); // new YR function
                //This()->OnUnlimbo_sub_70AF50_deactivate(0,0,0,0,This()->Get_Weapon()->Weapon->Attack); // new YR function?
                //Map.visibility_567DA0(&This()->Coord, 0, (LastSightRange + 3), 0); // new YR function?
                Sound_Effect(RuleExtension->SpyPlaneCameraSound, This()->Coord);
            }

        } else {

            DEBUG_INFO("Aircraft(Spyplane_Approach) %s -> Dest Tarcom\n", Name());
            This()->Assign_Destination(tarcom);

        }

    } else {

        DEBUG_INFO("Aircraft(Spyplane_Approach) %s -> Dest NULL, Mission RETREAT\n", Name());
        This()->Assign_Destination(nullptr);
        This()->Assign_Mission(MISSION_RETREAT);

    }

    if (tarcom_distance <= 768) { // TODO macro calculation.

        DEBUG_INFO("Aircraft(Spyplane_Approach) %s -> In range.\n", Name());

        DEBUG_INFO("Aircraft(Spyplane_Approach) %s -> Mission SPYPLANE_OVERFLY.\n", Name());
        This()->Assign_Mission(MISSION_SPYPLANE_OVERFLY);

        This()->IsLocked = true;

        SourceType source = This()->House->Get_Starting_Edge();
        Cell cell = Map.Calculated_Cell(source, INVALID_CELL, INVALID_CELL, SPEED_WINGED, true);
        if (cell) {
            DEBUG_INFO("Aircraft(Spyplane_Approach) %s -> Dest edge cell.\n", Name());
            This()->Assign_Destination(&Map[cell]);
        }

    }

    return RuleExtension->SpyPlaneCameraFrames;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
int AircraftClassExtension::Mission_Spyplane_Overfly()
{
    //EXT_DEBUG_TRACE("AircraftClassExtension::Mission_Spyplane_Overfly - Name: %s (0x%08X)\n", Name(), (uintptr_t)(This()));

    LEPTON tarcom_distance = This()->Distance(This()->TarCom);

    ASSERT(This()->Get_Weapon()->Weapon != nullptr);

    if (tarcom_distance <= This()->Get_Weapon()->Weapon->Range) {
        DEBUG_INFO("Aircraft(Spyplane_Overfly) %s -> In range, Look\n", Name());
        TechnoClassExtension::Look(); // new YR function
        //This()->OnUnlimbo_sub_70AF50_deactivate(0,0,0,0,This()->Get_Weapon()->Weapon->Attack); // new YR function?
        //Map.visibility_567DA0(&This()->Coord, 0, (LastSightRange + 3), 0); // new YR function?
    }

    if (!Target_Legal(This()->NavCom)) {
        SourceType source = This()->House->Get_Starting_Edge();
        Cell cell = Map.Calculated_Cell(source, INVALID_CELL, INVALID_CELL, SPEED_WINGED, true);
        if (cell) {
            DEBUG_INFO("Aircraft(Spyplane_Overfly) %s -> Dest Edge cell\n", Name());
            This()->Assign_Destination(&Map[cell]);
        }
    }

    return 3;
}
