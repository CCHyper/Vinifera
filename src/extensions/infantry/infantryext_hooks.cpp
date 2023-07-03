/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          INFANTRYEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended InfantryClass.
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
#include "infantryext_hooks.h"
#include "infantryext_init.h"
#include "infantry.h"
#include "infantrytype.h"
#include "infantrytypeext.h"
#include "technotype.h"
#include "technotypeext.h"
#include "building.h"
#include "buildingtype.h"
#include "tagtype.h"
#include "house.h"
#include "housetype.h"
#include "target.h"
#include "voc.h"
#include "tibsun_globals.h"
#include "extension.h"
#include "options.h"
#include "rules.h"
#include "wwkeyboard.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"



#include "overlay.h"
#include "buildingext.h"
#include "vox.h"
#include "iomap.h"

class InfantryClassExt : public InfantryClass
{
    public:
        double _Tiberium_Load() const;
        bool _Tiberium_Load_Full();
        int _Mission_Harvest();
        bool Harvesting();
        ActionType _What_Action(Cell &cell, bool check_fog = false, bool disallow_force = false) const;
};

ActionType InfantryClassExt::_What_Action(Cell &cell, bool check_fog, bool disallow_force) const
{
    if (Class->Storage != 0) {
        ActionType action = FootClass::What_Action(cell, check_fog, disallow_force);
        if (action == ACTION_MOVE && Map[cell].Land_Type() == LAND_TIBERIUM)
        {
            return ACTION_HARVEST;
        }
    }

    return InfantryClass::What_Action(cell, check_fog, disallow_force);
}

double InfantryClassExt::_Tiberium_Load() const
{
    if (Techno_Type_Class()->Storage != 0) {
        return Storage.Get_Total_Amount() / Techno_Type_Class()->Storage;
    }

    return 0.0;
}

bool InfantryClassExt::_Tiberium_Load_Full()
{
    return Tiberium_Load() == 1.0;
}

bool InfantryClassExt::Harvesting()
{
    Cell cell = Coord_Cell(Coord);
    CellClass *ptr = &Map[cell];

    /*
    **	Keep waiting if still heading toward a spot to harvest.
    */
    if (Target_Legal(NavCom)) return(true);

    if (!_Tiberium_Load_Full() && ptr->Land_Type() == LAND_TIBERIUM) {

        if (Class->Storage != 0) {
            if (Doing != DO_CRAWL) {
                Do_Action(DO_CRAWL);
            }
            TiberiumType tib = OverlayClass::To_TiberiumType(ptr->Overlay);

            double ratio = Class->Storage - Storage.Get_Total_Amount();
            if (ratio > 1.0) {
                ratio = 1.0;
            }

            int reduce = ptr->Reduce_Tiberium(ratio);
            if (reduce > 0) {
                Storage.Increase_Amount(reduce, tib);
            }
                //return Class->HarvestRate;
                //return 150;
                //return 50;
        }

        /*
        **	Lift some Tiberium from the ground. Try to lift a complete
        **	"level" of Tiberium. A level happens to be 6 steps. If there
        **	is a partial level, then lift that instead. Never lift more
        **	than the harvester can carry.
        */
        //		int reducer = (ptr->OverlayData % 6) + 1;
        //int reducer = 1;
        //OverlayType overlay = ptr->Overlay;
        //reducer = ptr->Reduce_Tiberium(std::min(reducer, Rule->BailCount - Tiberium));
        //Tiberium += reducer;
        //switch (overlay) {
        //case OVERLAY_GOLD1:
        //case OVERLAY_GOLD2:
        //case OVERLAY_GOLD3:
        //case OVERLAY_GOLD4:
        //    Gold += reducer;
        //    break;
        //
        //case OVERLAY_GEMS1:
        //case OVERLAY_GEMS2:
        //case OVERLAY_GEMS3:
        //case OVERLAY_GEMS4:
        //    Gems += reducer;
        //    if (Rule.BailCount > Tiberium) { Gems++; Tiberium++; }
        //    if (Rule.BailCount > Tiberium) { Gems++; Tiberium++; }
        //    if (Rule.BailCount > Tiberium) { Gems++; Tiberium++; }
        //    break;
        //
        //default:
        //    break;
        //}
        //Set_Stage(0);
        //Set_Rate(Rule.OreDumpRate);

    } else {

        /*
        **	If the harvester is stopped on a non Tiberium field and the harvester
        **	isn't loaded with Tiberium, then no further action can be performed
        **	by this logic routine. Bail with a failure and thus cause a branch to
        **	a better suited logic processor.
        */
        //Set_Stage(0);
        //Set_Rate(0);
        return(false);
    }
    return(true);
}
const int Harvester_Load_List[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 0 };

extern const char *const RadioMessages[RADIO_COUNT];

int InfantryClassExt::_Mission_Harvest()
{
#if 0
    if (Class->Storage != 0) {
        CellClass *cptr = Get_Cell_Ptr();
        if (cptr->Land == LAND_TIBERIUM && !_Tiberium_Load_Full()) {
            if (Doing != DO_CRAWL) {
                Do_Action(DO_CRAWL);
            }
            TiberiumType tib = OverlayClass::To_TiberiumType(cptr->Overlay);

            double ratio = Class->Storage - Storage.Get_Total_Amount();
            if (ratio > 1.0) {
                ratio = 1.0;
            }

            int reduce = cptr->Reduce_Tiberium(ratio);
            if (reduce > 0) {
                Storage.Increase_Amount(reduce, tib);
            }
            //return Class->HarvestRate;
            //return 150;
            return 50;
        }
    }

    Do_Action(DO_STAND_READY);
    Assign_Mission(MISSION_GUARD);
    return 1;
#endif

   enum {
       LOOKING,
       HARVESTING,
       FINDHOME,
       HEADINGHOME,
       GOINGTOIDLE,
       UNLOAD,
   };

   /*
   **	A non-harvesting type unit will just sit still if it is given the harvest mission. This
   **	allows combat units to act "brain dead".
   */
   if (!Class->Storage || !Class->Dock[0]) return(TICKS_PER_SECOND * 30);

   /*
   **	If there are no more refineries, then drop into guard mode.
   */
   if (!(House->BQuantity.Count_Of(Class->Dock[0]->Type))) {
       Assign_Mission(MISSION_GUARD);
       return(1);
   }

   switch (Status) {

       /*
       **	Go and find a Tiberium field to harvest.
       */
   case LOOKING:
       //DEBUG_INFO("Infantry Harvester Looking\n");
       /*
       **	When full of tiberium, just skip to finding a free refinery
       **	to unload at.
       */
       if (Tiberium_Load() == 1) {
           Status = FINDHOME;
           return(1);
       }

       /*
       ** Look for ore where we last found some - mine the same patch
       */
       if (Target_Legal(ArchiveTarget)) {
           Assign_Destination(ArchiveTarget);
           ArchiveTarget = 0;
       }
       //IsHarvesting = false;
       if (Goto_Tiberium(Rule->TiberiumLongScan / CELL_LEPTON_W)) {
           //IsHarvesting = true;
           Set_Rate(2);
           Set_Stage(0);
           Status = HARVESTING;
           return(1);
       } else {

           /*
           **	If the harvester isn't on Tiberium and it is not heading toward Tiberium, then
           **	force it to go into guard mode. This will prevent the harvester from repeatedly
           **	searching for Tiberium.
           */
           if (!Target_Legal(NavCom)) {

               /*
               **	If the archive target is legal, then head there since it is presumed
               **	that the archive target points to the last place it harvested at. This might
               **	solve the case where the harvester gets stuck and can't find Tiberium just because
               **	it is greater than 32 squares away.
               */
               if (Target_Legal(ArchiveTarget)) {
                   Assign_Destination(ArchiveTarget);
               } else {
                   Status = GOINGTOIDLE;
                   IsUseless = true;
                   House->IsTiberiumShort = true;
                   return(TICKS_PER_SECOND * 7);
               }
           } else {
               IsUseless = false;
           }
       }
       break;

       /*
       **	Harvest at current location until full or Tiberium exhausted.
       */
   case HARVESTING:
       //DEBUG_INFO("Infantry Harvester Harvesting\n");
       //			if (Fetch_Stage() > ARRAY_SIZE(Class->Harvester_Load_List)) {
       //				Set_Stage(0);
       //			}
       if (Fetch_Rate() == 0) {
           Set_Stage(0);
           Set_Rate(Rule->HarvesterLoadRate);
       }

       if (Fetch_Stage() < ARRAY_SIZE(Harvester_Load_List)) return(1);
       if (!Harvesting()) {
           //IsHarvesting = false;
           if (Tiberium_Load() == 1) {
               Status = FINDHOME;
               ArchiveTarget = ::As_Target(&Map[Coord_Cell(Coord)]);
           } else {
               if (!Goto_Tiberium(Rule->TiberiumShortScan / CELL_LEPTON_W) && !Target_Legal(NavCom)) {
                   //ArchiveTarget = TARGET_NONE;
                   Status = FINDHOME;
               } else {
                   Status = HARVESTING;
                   //IsHarvesting = true;
               }
           }
           return(1);
       }
       return(1);
       //			return(TICKS_PER_SECOND*Rule.OreDumpRate);

               /*
               **	Find and head to refinery.
               */
   case FINDHOME:
       //DEBUG_INFO("Infantry Harvester Finding Home\n");
       if (!Target_Legal(NavCom)) {

           /*
           **	Find nearby refinery and head to it?
           */
           BuildingClass *nearest = nullptr;
           for (int i = 0; i < Class->Dock.Count(); i++) {
               nearest = Find_Docking_Bay(Class->Dock[i]);
               if (nearest) {
                   break;
               }
           }

           /*
           **	Since the refinery said it was ok to load, establish radio
           **	contact with the refinery and then await docking orders.
           */
           //RadioMessageType reply = Transmit_Message(RADIO_HELLO, (RadioClass *)nearest);
           if (nearest != NULL) {
               //DEBUG_INFO("Home replied ROGER\n");
               Status = HEADINGHOME;
               //if (nearest->House == PlayerPtr && (PlayerPtr->Capacity - PlayerPtr->Tiberium) < 300 && PlayerPtr->Capacity > 500 && (PlayerPtr->ActiveBScan & (STRUCTF_REFINERY | STRUCTF_CONST))) {
               //    Speak(VOX_NEED_MO_CAPACITY);
               //}
           } else {
               //DEBUG_INFO("Home replied %s\n", Name_From_RadioMessage(reply));
               ScenarioInit++;
               nearest = nullptr;
               for (int i = 0; i < Class->Dock.Count(); i++) {
                   nearest = Find_Docking_Bay(Class->Dock[i]);
                   if (nearest) {
                       break;
                   }
               }
               ScenarioInit--;
               if (nearest != NULL) {
                   Assign_Destination(&Map[Nearby_Location((TechnoClass *)nearest)]);
               }
           }
       }
       break;

       /*
       **	In communication with refinery so that it will successfully dock and
       **	unload. If, for some reason, radio contact was lost, then hunt for
       **	another refinery to unload at.
       */
   case HEADINGHOME:
       //DEBUG_INFO("Infantry Harvester Heading Home\n");
       //Assign_Mission(MISSION_ENTER);
       Assign_Destination(&Map[Nearby_Location((TechnoClass *)Radio)]);
       Status = UNLOAD;
       return(1);

       case UNLOAD:
        if (Radio && Radio->What_Am_I() == RTTI_BUILDING && Distance(Radio) < 256) {
            //((BuildingClass *)Radio)->entry_278();
            //if (Rule->HarvesterDumpRate * TICKS_PER_MINUTE <= (double)v1->f.t.stage.Stage) {
                TiberiumType slot = (TiberiumType)Storage.First_Used_Slot();
                int amount = Storage.Get_Amount(slot);
                if (slot == TIBERIUM_NONE || (amount = Storage.Decrease_Amount(amount, slot), amount <= 0)) {
                    if (Radio && ((BuildingClass *)Radio)->Class->IsRefinery) {
                        //BuildingClass_Play_Animation(v49, 8, 0, 0);
                    }
                    Status = GOINGTOIDLE;
                    
                } else {
                    House->Tiberium_Harvested(amount, slot);
                    //v1->f.t.stage.Stage = 0;
                }
            //}
                Goto_Tiberium(Rule->TiberiumShortScan / CELL_LEPTON_W);
                Status = LOOKING;
                if (Radio) {
                    Radio->Transmit_Message(RADIO_ALL_DONE);
                    //Radio->Radio_Off();
                }

        } else
        {
            //DEBUG_INFO("NavCom %x, Radio %x\n", NavCom, Radio);
            if (Radio) {
                //Radio->Transmit_Message(RADIO_OVER_OUT);
                //Radio->Radio_Off();
            }
        }

        return(1);

       /*
       **	The harvester has nothing to do. There is no Tiberium nearby and
       **	no where to go.
       */
   case GOINGTOIDLE:
        //DEBUG_INFO("Infantry Harvester Going Idle\n");
       if (IsUseless) {
           //if (House->ActiveBScan & STRUCTF_REPAIR) {
           //    Assign_Mission(MISSION_REPAIR);
           //} else {
           //    Assign_Mission(MISSION_HUNT);
           //}
       }
       Assign_Mission(MISSION_GUARD);
       break;

   }

   return(Get_Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}





/**
 *  #issue-635
 * 
 *  Fixes a bug where EngineerDamage was not used to calculate the engineer damage.
 * 
 *  @author: CCHyper
 */
static int Get_Engineer_Damage(TechnoClass *tech)
{
    float damage = Rule->EngineerDamage;    // Was "Rule->ConditionRed * 0.5f"
    return std::min((tech->Techno_Type_Class()->MaxStrength * damage), (float)(tech->Strength-1));
}


/** 
 *  Is the target buildings health low enough to be captured? 
 * 
 *  @author: CCHyper
 */
static bool Health_Low_Enough_To_Capture(TechnoClass *tech)
{
    /**
     *  #issue-633
     * 
     *  Changed to use Rule->EngineerCaptureLevel.
     * 
     *  @author: CCHyper
     */
    return tech->Health_Ratio() <= Rule->EngineerCaptureLevel;
}


/**
 *  Patch to intercept the engineer capture checks.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Per_Cell_Process_Engineer_Capture_Damage_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, esi);
    GET_REGISTER_STATIC(TechnoClass *, tech, edi);      // From "cellptr->Cell_Building()".
    GET_REGISTER_STATIC(bool, iscapturable, bl);
    static int damage;

    /**
     *  If the target buildings health is low enough, go ahead and capture it.
     */
    if (Health_Low_Enough_To_Capture(tech)) {
        goto capture;
    }

    /**
     *  Health is still not low enough, go ahead and apply some more damage to it.
     */
    damage = Get_Engineer_Damage(tech);
    tech->Take_Damage(damage, 0, Rule->C4Warhead, this_ptr, true);

    /**
     *  Spring the DESTROYED_BY_ANYTHING event and remove this infantry.
     */
spring_and_delete:
    JMP(0x004D378D);

    /**
     *  Processing capturing of the target building.
     */
capture:
    JMP(0x004D36E1);
}


/**
 *  #issue-264
 * 
 *  Implements EnterTransportSound for infantry when they enter a transport.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Per_Cell_Process_Transport_Attach_Sound_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, esi);
    GET_REGISTER_STATIC(TechnoClass *, techno, edi);        // Radio contact
    static TechnoTypeClassExtension *radio_technotypeext;

    /**
     *  Stolen bytes/code.
     */
    techno->Cargo.Attach(this_ptr);

    /**
     *  If this transport we are entering has a passenger entering sound, play it now.
     */
    radio_technotypeext = Extension::Fetch<TechnoTypeClassExtension>(techno->Techno_Type_Class());
    if (radio_technotypeext->EnterTransportSound != VOC_NONE) {
        Sound_Effect(radio_technotypeext->EnterTransportSound, techno->Coord);
    }

    JMP(0x004D3A87);
}


/**
 *  #issue-226
 * 
 *  Implements IsMechanic for infantry when searching for targets.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Firing_AI_Mechanic_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, ebp);
    GET_REGISTER_STATIC(ObjectClass *, targ, esi);      // TarCom as ObjectClass.
    static InfantryTypeClassExtension *infantrytypeext;

    infantrytypeext = Extension::Fetch<InfantryTypeClassExtension>(this_ptr->Class);

    /**
     *  Is this infantry a "dual healer" (can it heal both infantry and units)?
     */
    if (infantrytypeext->IsOmniHealer) {

        /**
         *  Is the target being queried a unit, aircraft or infantry? If so, make
         *  sure this infantry is a mechanic before allowing it to heal the unit.
         */
        if (targ->What_Am_I() == RTTI_UNIT || (targ->What_Am_I() == RTTI_AIRCRAFT && !targ->In_Air()) || targ->What_Am_I() == RTTI_INFANTRY) {
            goto health_ratio_check;
        }

    /**
     *  Is this infantry a mechanic?
     */
    } else if (infantrytypeext->IsMechanic) {

        /**
         *  Is the target being queried a unit or aircraft? If so, make sure this
         *  infantry is a mechanic before allowing it to heal the unit.
         */
        if (targ->What_Am_I() == RTTI_UNIT || (targ->What_Am_I() == RTTI_AIRCRAFT && !targ->In_Air())) {
            goto health_ratio_check;
        }

    /**
     *  Original code.
     */
    } else if (targ->What_Am_I() == RTTI_INFANTRY) {
        goto health_ratio_check;
    }

assign_NULL_target:
    JMP(0x004D8824);

    /**
     *  Check the targets health ratio.
     */
health_ratio_check:
    JMP(0x004D87F5);
}


/**
 *  #issue-226
 * 
 *  Implements IsMechanic and IsDualHealer for infantry when deciding what action to perform.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_What_Action_Mechanic_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, edi);
    GET_REGISTER_STATIC(/*const */ObjectClass *, object, esi);  // target
    static InfantryTypeClassExtension *infantrytypeext;

    infantrytypeext = Extension::Fetch<InfantryTypeClassExtension>(this_ptr->Class);

    /**
     *  Is this infantry a "dual healer" (can it heal both infantry and units)?
     */
    if (infantrytypeext->IsOmniHealer) {

        /**
         *  If the mouse is over ourself, show the guard area cursor.
         */
        if (object == this_ptr) {
            goto guard_area;
        }

        /**
         *  Is the target being queried a unit, aircraft or infantry? If so, make
         *  sure this infantry is a mechanic before allowing it to heal the unit.
         */
        if (object->What_Am_I() == RTTI_UNIT || object->What_Am_I() == RTTI_AIRCRAFT || object->What_Am_I() == RTTI_INFANTRY) {

            /**
             *  If we are force-moving into an Transport, don't try to heal it!
             */
            if (object->Techno_Type_Class()->MaxPassengers > 0) {
                if (WWKeyboard->Down(Options.KeyForceMove1) || WWKeyboard->Down(Options.KeyForceMove2)) {
                    goto next_check;
                }
            }

            /**
             *  Before return ACTION_HEAL, check the targets health.
             */
            goto health_ratio_check;
        }

    /**
     *  Is this infantry a mechanic?
     */
    } else if (infantrytypeext->IsMechanic) {

        /**
         *  If the mouse is over ourself, show the guard area cursor.
         */
        if (object == this_ptr) {
            goto guard_area;
        }

        /**
         *  Is the target being queried a unit or aircraft? If so, make sure this
         *  infantry is a mechanic before allowing it to heal the unit.
         */
        if (object->What_Am_I() == RTTI_UNIT || object->What_Am_I() == RTTI_AIRCRAFT) {

            /**
             *  If we are force-moving into an Transport, don't try to heal it!
             */
            if (object->Techno_Type_Class()->MaxPassengers > 0) {
                if (WWKeyboard->Down(Options.KeyForceMove1) || WWKeyboard->Down(Options.KeyForceMove2)) {
                    goto next_check;
                }
            }

            /**
             *  Before return ACTION_HEAL, check the targets health.
             */
            goto health_ratio_check;
        }

    /**
     *  Original code.
     */
    } else if (object->What_Am_I() == RTTI_INFANTRY) {

        /**
         *  If the mouse is over ourself, show the guard area cursor.
         */
        if (object == this_ptr) {
            goto guard_area;
        }

        /**
         *  Before return ACTION_HEAL, check the targets health.
         */
        goto health_ratio_check;
    }

next_check:
    JMP(0x004D71B0);

    /**
     *  Show the guard area mouse cursor over us.
     */
guard_area:
    JMP(0x004D71A1);

    /**
     *  Check the targets health ratio.
     */
health_ratio_check:
    JMP(0x004D7178);
}


/**
 *  #issue-226
 * 
 *  Allow all foot objects to be valid targets when this infantry deals negative damage.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Can_Fire_Target_Check_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, esi);
    GET_STACK_STATIC(TARGET, target, esp, 0x10);
    GET_STACK_STATIC(int, which, esp, 0x14);
    static FootClass *targ;

    targ = Target_As_Foot(target);
    if (targ == nullptr) {
        goto return_FIRE_ILLEGAL;
    }

health_ratio_check:
    JMP_REG(ecx, 0x004D5ACF);

return_FIRE_ILLEGAL:
    JMP(0x004D5AE8);
}


/**
 *  #issue-80
 * 
 *  Fixes the bug where the Jumpjet uses the wrong DoType when idle on the
 *  ground. This was because the original code did not check if the infantry
 *  was actually in the air or not and always assumed it was on the ground, as
 *  a result it was always setting DO_STAND_READY.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Doing_AI_JumpJet_Idle_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, esi);
    static const InfantryTypeClass *infantrytype;

    infantrytype = reinterpret_cast<const InfantryTypeClass *>(this_ptr->Class_Of());

    /**
     *  Stolen code.
     * 
     *  If infantry is prone, set DO_PRONE.
     */
    if (this_ptr->IsProne) {
        JMP(0x004D8B12);
    }

    if (infantrytype->IsJumpJet) {

        /**
         *  This is a Jumpjet infantry, make sure its in the air before
         *  assigning the hover graphic sequence.
         */
        if (this_ptr->In_Air()) {
            this_ptr->Do_Action(DO_HOVER, true);

        } else {
            this_ptr->Do_Action(DO_STAND_READY, true);
        }

    /**
     *  Handle normal infantry.
     */
    } else {
        this_ptr->Do_Action(DO_STAND_READY, true);
    }

    JMP(0x004D8CA1);
}


/**
 *  #issue-80
 * 
 *  Fixes the bug where the Jumpjet uses the wrong DoType when on the
 *  ground and in between firing rounds. This was because the original code
 *  did not check if the infantry was actually in the air and assumed it always
 *  is, thus setting DO_STAND_READY.
 * 
 *  @warning: This patch is within a branch that has already checked if the
 *            infantry is firing!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_AI_JumpJet_Idle_Between_Firing_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, esi);
    static const InfantryTypeClass *infantrytype;

    infantrytype = reinterpret_cast<const InfantryTypeClass *>(this_ptr->Class_Of());

    if (infantrytype->IsJumpJet) {

        /**
         *  This is a Jumpjet infantry, make sure its in the air before
         *  assigning the hover graphic sequence.
         */
        if (this_ptr->In_Air()) {
            this_ptr->Do_Action(DO_HOVER);

        } else {
            this_ptr->Do_Action(DO_STAND_READY);
        }

    /**
     *  Handle normal infantry.
     */
    } else {
        this_ptr->Do_Action(DO_STAND_READY);
    }

    /**
     *  Stolen code.
     * 
     *  Clear the firing flag.
     */
    this_ptr->IsFiring = false;

    JMP(0x004D50E0);
}


/**
 *  #issue-80
 * 
 *  Fixes the bug where the Jumpjet uses the wrong DoType when not moving
 *  but actually in the air.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Movement_AI_JumpJet_Not_Moving_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, ebp);
    static const InfantryTypeClass *infantrytype;

    infantrytype = reinterpret_cast<const InfantryTypeClass *>(this_ptr->Class_Of());

    if (infantrytype->IsJumpJet) {

        /**
         *  This is a Jumpjet infantry, make sure its in the air before
         *  assigning the hover graphic sequence.
         */
        if (this_ptr->In_Air()) {
            this_ptr->Do_Action(DO_HOVER);

        } else {
            this_ptr->Do_Action(DO_STAND_READY);
        }

    /**
     *  Handle normal infantry.
     */
    } else {
        this_ptr->Do_Action(DO_STAND_READY);
    }

    JMP(0x004D9087);
}


/**
 *  #issue-81
 * 
 *  Fixes the bug where the Jumpjet uses the wrong DoType when firing on
 *  the ground. This was because the original code did not check if the infantry
 *  was actually in the air and assumed it always is, thus setting DO_FIREFLY.
 * 
 *  @warning: This patch is within a branch that has already checked if the
 *            infantry is controlled by the Jumpjet locomotor!
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_InfantryClass_Firing_AI_JumpJet_In_Air_Patch)
{
    GET_REGISTER_STATIC(InfantryClass *, this_ptr, ebp);

    /**
     *  Make sure its in the air before assigning the hover firing graphic sequence.
     */
    if (this_ptr->In_Air()) {
        this_ptr->Do_Action(DO_FIREFLY);
    } else {
        this_ptr->Do_Action(DO_FIRE_WEAPON);
    }

    JMP(0x004D8933);
}


/**
 *  Main function for patching the hooks.
 */
void InfantryClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    InfantryClassExtension_Init();

    Patch_Jump(0x004D88FA, &_InfantryClass_Firing_AI_JumpJet_In_Air_Patch);
    Patch_Jump(0x004D8C83, &_InfantryClass_Doing_AI_JumpJet_Idle_Patch);
    Patch_Jump(0x004D50C9, &_InfantryClass_AI_JumpJet_Idle_Between_Firing_Patch);
    Patch_Jump(0x004D9076, &_InfantryClass_Movement_AI_JumpJet_Not_Moving_Patch);
    Patch_Jump(0x004D5AB4, &_InfantryClass_Can_Fire_Target_Check_Patch);
    Patch_Jump(0x004D7168, &_InfantryClass_What_Action_Mechanic_Patch);
    Patch_Jump(0x004D87E9, &_InfantryClass_Firing_AI_Mechanic_Patch);
    Patch_Jump(0x004D3A7B, &_InfantryClass_Per_Cell_Process_Transport_Attach_Sound_Patch);
    Patch_Jump(0x004D35F9, &_InfantryClass_Per_Cell_Process_Engineer_Capture_Damage_Patch);
    Patch_Jump(0x00637230, &InfantryClassExt::_Tiberium_Load);
    Change_Virtual_Address(0x006D218C, Get_Func_Address(&InfantryClassExt::_What_Action));
    Change_Virtual_Address(0x006D22E8, Get_Func_Address(&InfantryClassExt::_Mission_Harvest));

    /**
     *  ACTION_DAMAGE no longer a case in DisplayClass::Left_Mouse_Up to show the
     *  correct mouse cursor for the multi-engineer damage (MOUSE.SHP also does not
     *  contain any artwork for this), so with the multi-engineer fixes above it shows
     *  the default arrow cursor. We fix this by making it use ACTION_CAPTURE still
     *  to make sure the mouse shows the correct visual cursor.
     */
    Patch_Byte(0x004D7124+1, ACTION_CAPTURE);
}
