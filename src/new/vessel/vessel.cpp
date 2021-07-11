/*******************************************************************************
/*                     O P E N  S O U R C E  --  T S + +                      **
/*******************************************************************************
 *
 *  @project       TS++
 *
 *  @file          BUILDING.H
 *
 *  @authors       CCHyper, tomsons26
 *
 *  @brief         Unit game object class.
 *
 *  @license       TS++ is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 TS++ is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "vessel.h"
#include "tibsun_globals.h"
#include "tibsun_defines.h"
#include "tibsun_inline.h"
#include "vinifera_globals.h"
#include "vinifera_defines.h"
#include "vesseltype.h"
#include "housetype.h"
#include "house.h"
#include "trigger.h"
#include "triggertype.h"
#include "session.h"
#include "tracker.h"
#include "ccini.h"
#include "team.h"
#include "iomap.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  Retrieves the class identifier (CLSID) of the object.
 * 
 *  @author: CCHyper
 */
IFACEMETHODIMP VesselClass::GetClassID(CLSID *pClassID)
{    
    if (pClassID == nullptr) {
        return E_POINTER;
    }

    *pClassID = __uuidof(this);

    return S_OK;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 * 
 *  @author: CCHyper
 * 
 *  @param      pStm           An IStream pointer to the stream from which the object should be loaded.
 */
IFACEMETHODIMP VesselClass::Load(IStream *pStm)
{
    HRESULT hr = FootClass::Load(pStm);
    if (SUCCEEDED(hr)) {
        // Insert any data to be loaded here.
    }

    return hr;
}


/**
 *  Saves an object to the specified stream.
 * 
 *  @author: CCHyper
 * 
 *  @param      pStm           An IStream pointer to the stream into which the object should be saved.
 * 
 *  @param      fClearDirty    Indicates whether to clear the dirty flag after the save is complete.
 */
IFACEMETHODIMP VesselClass::Save(IStream *pStm, BOOL fClearDirty)
{
    HRESULT hr = FootClass::Save(pStm, fClearDirty);
    if (SUCCEEDED(hr)) {
        // Insert any data to be saved here.
    }

    return hr;
}


/***********************************************************************************************
 * VesselClass::VesselClass -- Constructor for vessel class objects.                           *
 *                                                                                             *
 *    This is the normal constructor for vessel class objects. It will set up a vessel that    *
 *    is valid excepting that it won't be placed on the map.                                   *
 *                                                                                             *
 * INPUT:   classid  -- The type of vessel this will be.                                       *
 *                                                                                             *
 *          house    -- The owner of this vessel.                                              *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/14/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
VesselClass::VesselClass(const VesselTypeClass *classof, HouseClass *house) :
    FootClass(house),
    Class(classof)
{
    Create_ID();

    SecondaryFacing.Set(PrimaryFacing.Current());

    /**
     *  Create an instance of the locomotor and link it to this object.
     */
    if (Class) {
        ILocomotionPtr loco(Class->Locomotor);
        if (Locomotion != loco) {
            Locomotion = loco;
        }
        Locomotion->Link_To_Object(this);
    }

    if (Class) {
        PrimaryFacing.Set_ROT(Class->Rotation);
        SecondaryFacing.Set_ROT(Class->Rotation);

        /**
         *  The ammo member is actually part of the techno class, but must be
         *  initialized manually here because this is where we first have access
         *  to the class pointer.
         */
        Ammo = Class->MaxAmmo;

        /**
         *  The techno class cloakabilty flag is set according to the type
         *  class cloakability flag.
         */
        IsCloakable = Class->IsCloakable;

        Strength = Class->MaxStrength;
    }

    if (House) {
        House->Tracking_Add(this);
    }

    Init();

    Vessels.Add(this);
//    index_80E8C8.Add(Fetch_ID());
}


VesselClass::VesselClass(const NoInitClass &noinit) :
    FootClass(noinit)
{
}


/***********************************************************************************************
 * VesselClass::~VesselClass -- Destructor for vessel objects.                                 *
 *                                                                                             *
 *    The destructor will destroy the vessel and ensure that it is properly removed from the   *
 *    game engine.                                                                             *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/14/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
VesselClass::~VesselClass()
{
    if (GameActive && Class) {

        if (House->Can_Build(Class) == -1) {
            House->IsRecalcNeeded = true;
        }

        if (Team) {
            Team->Remove(this);
            Team = nullptr;
        }

        House->Tracking_Remove(this);

        while (Cargo.Is_Something_Attached()) {
            delete Cargo.Detach_Object();
        }

        Limbo();
    }

    Detach_This_From_All(this);

    Vessels.Delete(this);
//    index_80E8C8.Remove(Fetch_ID());


    IsActive = false;
}


void VesselClass::Detach(TARGET target, bool all)
{
    FootClass::Detach(target, all);
}


RTTIType VesselClass::Kind_Of() const
{
    return RTTI_VESSEL;
}


int VesselClass::Size_Of(bool firestorm) const
{
    return sizeof(VesselClass);
}


void VesselClass::Compute_CRC(WWCRCEngine &crc) const
{
    FootClass::Compute_CRC(crc);
}


/***********************************************************************************************
 * VesselClass::AI -- Handles the AI processing for vessel objects.                            *
 *                                                                                             *
 *    This routine is called once for each vessel object during each main game loop. All       *
 *    normal AI processing is handled here. This includes dispatching and maintaining any      *
 *    processing that is specific to vessel objects.                                           *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/20/1996 JLB : Created.                                                                 *
 *   07/16/1996 JLB : Prefers anti-sub weapons if firing on subs.                              *
 *=============================================================================================*/
void VesselClass::AI()
{
    ASSERT(IsActive);
    
    /**
     *  If there is no mission assigned, then this ship should just do its normal thing.
     */
    if (Mission == MISSION_NONE && MissionQueue == MISSION_NONE) {
        Enter_Idle_Mode();
    }

    /**
     *  If the ship finds itself in a hunt order but it has no weapons, then tell it
     *  to sail off the map instead.
     */
    if (Mission == MISSION_HUNT && !Is_Weapon_Equipped()) {
        Assign_Mission(MISSION_RETREAT);
    }

    /**
     *  Act on new orders if the unit is at a good position to do so.
     */
    if (!Locomotor_Ptr()->Is_Moving() && Door.Is_Door_Closed() /*Mission != MISSION_UNLOAD*/) {
        Commence();
    }

//#ifndef CLIPDRAW
//    if (Map.In_View(Coord_Cell(Center_Coord()))) {
//        Mark(MARK_CHANGE);
//    }
//#endif

//#ifdef FIXIT_CARRIER  //  checked - ajw 9/28/98
//// Re-stock the ammo of any on-board helicopters on an aircraft carrier.
//    if (*this == VESSEL_CARRIER && How_Many()) {
//        if (!MoebiusCountDown) {
//            MoebiusCountDown = Rule.ReloadRate * TICKS_PER_MINUTE;
//            ObjectClass *obj = Attached_Object();
//            while (obj) {
//                long bogus;
//                ((AircraftClass *)obj)->Receive_Message(this,RADIO_RELOAD,bogus);
//                obj = (obj->Next);
//            }
//        }
//    }
//#endif

    /**
     *  Process base class AI routine. If as a result of this, the vessel gets
     *  destroyed, then detect this fact and bail early.
     */
    FootClass::AI();
    if (!IsActive) {
        return;
    }

    /**
     *  Handle body and turret rotation.
     */
    Rotation_AI();

    /**
     *  Handle any combat processing required.
     */
    Combat_AI();

    /**
     *  Delete this unit if it finds itself off the edge of the map and it is in
     *  guard or other static mission mode.
     */
    if (Edge_Of_World_AI()) {
        return;
    }

    if (Class->Max_Passengers() > 0) {

//        /**
//         *  Double check that there is a passenger that is trying to load or unload.
//         *  If not, then close the door.
//         */
//        if (!Door.Is_Door_Closed() && Mission != MISSION_UNLOAD && Transmit_Message(RADIO_TRYING_TO_LOAD) != RADIO_ROGER && !(long)DoorShutCountDown) {
//            LST_Close_Door();
//        }
    }

    /**
     *  Don't start a new mission unless the vehicle is in the center of
     *  a cell (not driving) and the door (if any) is closed.
     */
    if (!Locomotor_Ptr()->Is_Moving() && Door.Is_Door_Closed()/*&& Mission != MISSION_UNLOAD*/) {
        Commence();
    }

    /**
     *  Do a step of repair here, if appropriate.
     */
    Repair_AI();
}


/***********************************************************************************************
 * VesselClass::Class_Of -- Fetches a reference to the vessel's class data.                    *
 *                                                                                             *
 *    This routine will return with a reference to the static class data for this vessel.      *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  Returns with a reference to the class data structure associated with this vessel.  *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/14/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
ObjectTypeClass *const VesselClass::Class_Of() const
{
    ASSERT(IsActive);

    return (ObjectTypeClass *const)Class;
}


/***********************************************************************************************
 * UnitClass::Limbo -- Limbo this unit.                                                        *
 *                                                                                             *
 *    This will cause the unit to go into a limbo state. If it was carrying a flag, then       *
 *    the flag will be dropped where the unit is at.                                           *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  bool; Was this unit limboed?                                                       *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/08/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
bool VesselClass::Limbo()
{
    ASSERT(IsActive);

    if (!FootClass::Limbo()) {
        return false;
    }

    return true;
}


/***********************************************************************************************
 * UnitClass::Unlimbo -- Removes unit from stasis.                                             *
 *                                                                                             *
 *    This routine will place a unit into the game and out of its limbo                        *
 *    state. This occurs whenever a unit is unloaded from a transport.                         *
 *                                                                                             *
 * INPUT:   coord    -- The coordinate to make the unit appear.                                *
 *                                                                                             *
 *          dir      -- The initial facing to impart upon the unit.                            *
 *                                                                                             *
 * OUTPUT:  bool; Was the unit unlimboed successfully?  If the desired                         *
 *                coordinate is illegal, then this might very well return                      *
 *                false.                                                                       *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/22/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool VesselClass::Unlimbo(Coordinate &coord, DirType dir)
{
    ASSERT(IsActive);

    if (!FootClass::Unlimbo(coord, dir)) {
        return false;
    }

    SecondaryFacing = DirStruct(dir);

    /**
     *  Ensure that the owning house knows about the new object.
     */
    //House->UScan |= (1L << Class->Type);
    //House->ActiveUScan |= (1L << Class->Type);

    /**
     *  If it starts off the edge of the map, then it already starts cloaked.
     */
    if (IsCloakable && !IsLocked) {
        Cloak = CLOAKED;
    }

    /**
     *	Units default to no special animation.
     */
    Set_Rate(0);
    Set_Stage(0);

    return true;
}


/***********************************************************************************************
 * VesselClass::Read_INI -- Read the vessel data from the INI database.                        *
 *                                                                                             *
 *    This will read and create all vessels specified in the INI database. This routine is     *
 *    called when the scenario starts.                                                         *
 *                                                                                             *
 * INPUT:   ini   -- Reference to the INI database to read the vessel data from.               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Vessels will be created and placed on the map by this function.                 *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/09/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void VesselClass::Read_INI(CCINIClass &ini)
{
#if 0
    VesselClass *vessel;        // Working vessel pointer.
    HousesType inhouse;         // Vessel house.
    VesselType classid;         // Vessel class.
    char buf[128];

    int len = ini.Entry_Count(INI_Name());
    for (int index = 0; index < len; index++) {
        const char *entry = ini.Get_Entry(INI_Name(), index);

        ini.Get_String(INI_Name(), entry, nullptr, buf, sizeof(buf));
        inhouse = HouseTypeClass::From_Name(std::strtok(buf, ","));
        if (inhouse != HOUSE_NONE) {
            classid = VesselTypeClass::From_Name(std::strtok(nullptr, ","));

            if (classid != VESSEL_NONE) {

                if (HouseClass::As_Pointer(inhouse) != nullptr) {
                    vessel = new VesselClass(classid, inhouse);
                    if (vessel != nullptr) {

                        /**
                         *  Read the raw data.
                         */
                        int strength = std::atoi(std::strtok(nullptr, ",\r\n"));
                        Cell cell = std::atoi(std::strtok(nullptr, ",\r\n"));
                        Coordinate coord = Cell_Coord(cell);
                        DirType dir = (DirType)std::atoi(std::strtok(nullptr, ",\r\n"));
                        MissionType mission = MissionClass::Mission_From_Name(std::strtok(nullptr, ",\n\r"));

                        vessel->Trigger = nullptr;
                        TriggerTypeClass * tp = TriggerTypeClass::From_Name(std::strtok(nullptr, ",\r\n"));
                        if (tp != nullptr) {
                            TriggerClass * tt = TriggerClass::Find_Or_Make(tp);
                            if (tt != nullptr) {
                                tt->AttachCount++;
                                vessel->Trigger = tt;
                            }
                        }

                        if (vessel->Unlimbo(coord, dir)) {
                            vessel->Strength = (int)vessel->Class->MaxStrength * fixed(strength, 256);
                            if (vessel->Strength > vessel->Class->MaxStrength-3) vessel->Strength = vessel->Class->MaxStrength;
                            if (Session.Type == GAME_NORMAL || vessel->House->IsHuman) {
                                vessel->Assign_Mission(mission);
                                vessel->Commence();
                            } else {
                                vessel->Enter_Idle_Mode();
                            }

                        } else {

                            /**
                             *  If the vessel could not be unlimbo'ed, then this
                             *  is a catastrophic error condition. Delete the vessel.
                             */
                            delete vessel;
                        }
                    }
                }
            }
        }
    }
#endif
}


/***********************************************************************************************
 * VesselClass::Write_INI -- Write all vessel scenario data to the INI database.               *
 *                                                                                             *
 *    This routine is used to add the vessel data (needed for scenario start) to the INI       *
 *    database specified. If there was any preexisting vessel data in the database, it will    *
 *    be cleared                                                                               *
 *                                                                                             *
 * INPUT:   ini   -- Reference to the ini database to store the vessel data into.              *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/09/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void VesselClass::Write_INI(CCINIClass &ini)
{
    /**
     *  First, clear out all existing vessel data from the ini file.
     */
    ini.Clear(INI_Name());

    /**
     *  Write the vessel data out.
     */
    for (int index = 0; index < Vessels.Count(); ++index) {
        VesselClass *vessel = Vessels[index];
        if (vessel != nullptr && !vessel->IsInLimbo && vessel->IsActive) {
            char vname[10];
            char buf[128];

            std::snprintf(vname, sizeof(vname), "%d", index);
            std::snprintf(buf, sizeof(buf), "%s,%s,%d,%d,%d,%d,%s,%s,%d,%d,%d,%d,%d,%d",
                vessel->House->Class->IniName,
                vessel->Class->IniName,
                vessel->Health_Ratio()*256,
                Coord_Cell(vessel->Coord),
                vessel->PrimaryFacing.Current(),
                MissionClass::Mission_Name(vessel->Mission),
                /*vessel->Trigger ? vessel->Trigger->Class->IniName : */ "None"
                );
            ini.Put_String(INI_Name(), vname, buf);
        }
    }
}
