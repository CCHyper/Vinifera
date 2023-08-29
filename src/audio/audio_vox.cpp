/*******************************************************************************
/*                  O P E N  S O U R C E -- V I N I F E R A                   **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          AUDIO_NEWVOX.CPP
 *
 *  @author        CCHyper, tomsons26
 *
 *  @brief         
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
#ifdef USE_MINIAUDIO

#include "audio_vox.h"
#include "tibsun_globals.h"
#include "options.h"
#include "vox.h"
#include "scenario.h"
#include "house.h"
#include "housetype.h"
#include "side.h"
#include "ccini.h"
#include "audio_handle.h"
#include "audio_util.h"
#include "asserthandler.h"
#include <algorithm>
#include <thread>


/**
 *  x
 */
static DynamicVectorClass<AudioVoxClass *> Voxs;


/**
 *  x
 */
bool AudioVoxClass::IsSpeechAllowed = true;


/**
 *  The sound handle to the current speech being played. This is
 *  used to query if a speech is currently playing.
 */
static AudioHandleClass *CurrentSpeechHandle = nullptr;


/**
 *  These are the defaults for all speeches loaded from the ini database.
 */
static AudioSoundType DefaultType = AUDIO_SOUND_VOICE;
static AudioControlType DefaultControl = AUDIO_CONTROL_QUEUE;
static int DefaultPriority = AudioManager.AudioPriority_To_Priority(AUDIO_PRIORITY_NORMAL);
static float DefaultDelay = 0.2f; // We add a fake delay so the speech is not played on top of the building placement sound effect etc.
static int DefaultFrequencyShift = 1.0f;
static float DefaultVolume = AUDIO_VOLUME_MAX;
static float DefaultMinVolume = AUDIO_VOLUME_MIN;
static float DefaultMaxVolume = AUDIO_VOLUME_MAX;


#if 0
/**
 *  x
 *
 *  @author: CCHyper
 */
typedef struct SpeechRemapStruct
{
    Wstring Name;
    //int Side;           // -1 = any side can play, >= 0 side
    bool RemapFilename;         // Remap the filename prefix to the side index (SideType).
} SpeechRemapStruct;

static SpeechRemapStruct & Get_Vox_Remap_Info(VoxType vox)
{
    /**
     *
     *
     *  Thank you to Crimsonum for the description list.
     */
    static SpeechRemapStruct SpeechRemapArray[VOX_COUNT] = {
        SpeechRemapStruct { Speech[VOX_ACCOMPLISHED], true },               // EVA/CABAL - Mission accomplished
        SpeechRemapStruct { Speech[VOX_FAIL], true },                       // EVA/CABAL - Your mission has failed
        SpeechRemapStruct { Speech[VOX_NO_FACTORY], true },                 // EVA/CABAL - Unable to comply, building in progress
        SpeechRemapStruct { Speech[VOX_CONSTRUCTION], true },               // EVA/CABAL - Construction complete
        SpeechRemapStruct { Speech[VOX_UNIT_READY], true },                 // EVA/CABAL - Unit ready
        SpeechRemapStruct { Speech[VOX_NEW_CONSTRUCT], true },              // EVA/CABAL - New construction options
        SpeechRemapStruct { Speech[VOX_DEPLOY], true },                     // EVA/CABAL - Cannot deploy here
        SpeechRemapStruct { Speech[VOX_STRUCTURE_DESTROYED], true },        // EVA/CABAL - Structure destroyed <side2> GDI structure destroyed
        SpeechRemapStruct { Speech[VOX_NO_CASH], true },                    // EVA/CABAL - Insufficient funds
        SpeechRemapStruct { Speech[VOX_CONTROL_EXIT], true },               // EVA/CABAL - Battle control offline
        SpeechRemapStruct { Speech[VOX_REINFORCEMENTS], true },             // EVA/CABAL - Reinforcements have arrived
        SpeechRemapStruct { Speech[VOX_CANCELED], true },                   // EVA/CABAL - Canceled
        SpeechRemapStruct { Speech[VOX_BUILDING], true },                   // EVA/CABAL - Building
        SpeechRemapStruct { Speech[VOX_LOW_POWER], true },                  // EVA/CABAL - Low power
        SpeechRemapStruct { Speech[VOX_BASE_UNDER_ATTACK], true },          // EVA/CABAL - Base under attack
        SpeechRemapStruct { Speech[VOX_PRIMARY_SELECTED], true },           // EVA/CABAL - Primary building set
        SpeechRemapStruct { Speech[VOX_UNIT_LOST], true },                  // EVA/CABAL - Unit lost
        SpeechRemapStruct { Speech[VOX_SELECT_TARGET], true },              // EVA/CABAL - Select target
        SpeechRemapStruct { Speech[VOX_NEED_MO_CAPACITY], true },           // EVA/CABAL - Silos needed
        SpeechRemapStruct { Speech[VOX_SUSPENDED], true },                  // EVA/CABAL - On hold
        SpeechRemapStruct { Speech[VOX_REPAIRING], true },                  // EVA/CABAL - Repairing
        SpeechRemapStruct { Speech[VOX_TRAINING], true },                   // EVA/CABAL - Training
        SpeechRemapStruct { Speech[VOX_UPGRADE_ARMOR], true },              // EVA/CABAL - Unit armor upgraded
        SpeechRemapStruct { Speech[VOX_UPGRADE_FIREPOWER], true },          // EVA/CABAL - Unit firepower upgraded
        SpeechRemapStruct { Speech[VOX_UPGRADE_SPEED], true },              // EVA/CABAL - Unit speed upgraded
        SpeechRemapStruct { Speech[VOX_UNIT_REPAIRED], true },              // EVA/CABAL - Unit repaired
        SpeechRemapStruct { Speech[VOX_STRUCTURE_SOLD], true },             // EVA/CABAL - Structure sold
        SpeechRemapStruct { Speech[VOX_HARVESTER_UNDER_ATTACK], true },     // EVA/CABAL - Harvester under attack
        SpeechRemapStruct { Speech[VOX_CLOAKED_DETECTED], true },           // EVA/CABAL - Cloaked unit detected
        SpeechRemapStruct { Speech[VOX_SUBTERRANEAN_DETECTED], true },      // EVA/CABAL - Subterranean unit detected
        SpeechRemapStruct { Speech[VOX_TIME_20], true },                    // EVA/CABAL - 20 minutes remaining
        SpeechRemapStruct { Speech[VOX_TIME_10], true },                    // EVA/CABAL - 10 minutes remaining
        SpeechRemapStruct { Speech[VOX_TIME_5], true },                     // EVA/CABAL - 5 minutes remaining
        SpeechRemapStruct { Speech[VOX_TIME_4], true },                     // EVA/CABAL - 4 minutes remaining
        SpeechRemapStruct { Speech[VOX_TIME_3], true },                     // EVA/CABAL - 3 minutes remaining
        SpeechRemapStruct { Speech[VOX_TIME_2], true },                     // EVA/CABAL - 2 minutes remaining
        SpeechRemapStruct { Speech[VOX_TIME_1], true },                     // EVA/CABAL - 1 minute remaining
        SpeechRemapStruct { Speech[VOX_UNIT_SOLD], true },                  // EVA/CABAL - Unit sold
        SpeechRemapStruct { Speech[VOX_BUILDING_CAPTURED], true },          // EVA/CABAL - Building captured
        SpeechRemapStruct { Speech[VOX_CONTROL_ESTABLISHED], true },        // EVA/CABAL - Establishing battlefield control, standby
        SpeechRemapStruct { Speech[VOX_00_I176], true },                    // EVA/CABAL - Ion storm approaching
        SpeechRemapStruct { Speech[VOX_00_I178], true },                    // EVA/CABAL - Meteor storm approaching
        SpeechRemapStruct { Speech[VOX_00_I198], true },                    // EVA/CABAL - New terrain discovered
        SpeechRemapStruct { Speech[VOX_00_I150], true },                    // EVA/CABAL - Missile launch detected
        SpeechRemapStruct { Speech[VOX_00_I152], true },                    // EVA/CABAL - Chemical missile ready
        SpeechRemapStruct { Speech[VOX_00_I154], true },                    // EVA/CABAL - Cluster missile ready
        SpeechRemapStruct { Speech[VOX_00_I156], true },                    // EVA/CABAL - Ion cannon ready
        SpeechRemapStruct { Speech[VOX_00_I158], true },                    // EVA/CABAL - EMPulse cannon ready
        SpeechRemapStruct { Speech[VOX_00_I162], true },                    // EVA/CABAL - Firestorm defense ready
        SpeechRemapStruct { Speech[VOX_00_I170], true },                    // EVA/CABAL - Firestorm defense offline
        SpeechRemapStruct { Speech[VOX_00_I100], true },                    // EVA/CABAL - Primary objective achieved
        SpeechRemapStruct { Speech[VOX_00_I102], true },                    // EVA/CABAL - Secondary objective achieved
        SpeechRemapStruct { Speech[VOX_00_I104], true },                    // EVA/CABAL - Tertiary objective achieved
        SpeechRemapStruct { Speech[VOX_00_I106], true },                    // EVA/CABAL - Quaternary objective achieved
        SpeechRemapStruct { Speech[VOX_00_I194], true },                    // EVA/CABAL - Critical unit lost
        SpeechRemapStruct { Speech[VOX_00_I196], true },                    // EVA/CABAL - Critical structure lost
        SpeechRemapStruct { Speech[VOX_00_I208], true },                    // EVA/CABAL - Mutant supplies found
        SpeechRemapStruct { Speech[VOX_00_I210], true },                    // EVA/CABAL - Commandos en route
        SpeechRemapStruct { Speech[VOX_BUILDING_INFILTRATED], true },       // EVA/CABAL - Building infiltrated
        SpeechRemapStruct { Speech[VOX_TIME_START], true },                 // EVA/CABAL - Timer started
        SpeechRemapStruct { Speech[VOX_TIME_STOP], true },                  // EVA/CABAL - Timer stopped
        SpeechRemapStruct { Speech[VOX_00_I118], true },                    // EVA/CABAL - Bridge repaired
        SpeechRemapStruct { Speech[VOX_00_I180], true },                    // EVA/CABAL - Base defenses offline
        SpeechRemapStruct { Speech[VOX_BUILDING_OFFLINE], true },           // EVA/CABAL - Building offline
        SpeechRemapStruct { Speech[VOX_BUILDING_ONLINE], true },            // EVA/CABAL - Building online
        SpeechRemapStruct { Speech[VOX_00_I252], true },                    // EVA/CABAL - Player has resigned
        SpeechRemapStruct { Speech[VOX_00_I268], true },                    // EVA/CABAL - Player was defeated
        SpeechRemapStruct { Speech[VOX_00_I284], true },                    // EVA/CABAL - You are victorious
        SpeechRemapStruct { Speech[VOX_00_I286], true },                    // EVA/CABAL - You have lost
        SpeechRemapStruct { Speech[VOX_00_I288], true },                    // EVA/CABAL - You have resigned
        SpeechRemapStruct { Speech[VOX_00_I290], true },                    // EVA/CABAL - Mutant commandos available
        SpeechRemapStruct { Speech[VOX_ALLIANCE_FORMED], true },            // EVA/CABAL - Alliance formed
        SpeechRemapStruct { Speech[VOX_ALLIANCE_BROKEN], true },            // EVA/CABAL - Alliance broken
        SpeechRemapStruct { Speech[VOX_ALLY_ATTACK], true },                // EVA/CABAL - Our ally is under attack
        SpeechRemapStruct { Speech[VOX_00_I310], true },                    // EVA/CABAL - Build more powerplants to restore full power
        SpeechRemapStruct { Speech[VOX_00_I312], true },                    // EVA/CABAL - Build barracks to train additional troops
        SpeechRemapStruct { Speech[VOX_00_I314], true },                    // EVA/CABAL - Build hand of Nod to train additional troops
        SpeechRemapStruct { Speech[VOX_00_I316], true },                    // EVA/CABAL - Build a tiberium refinery to harvest tiberium
        SpeechRemapStruct { Speech[VOX_00_I318], true },                    // EVA/CABAL - Build tiberium silos to store excess tiberium
        SpeechRemapStruct { Speech[VOX_GDI_TAUNT_1], true },                // EVA - Inferior tactics detected
        SpeechRemapStruct { Speech[VOX_GDI_TAUNT_2], true },                // EVA - Retreat is your only logical option
        SpeechRemapStruct { Speech[VOX_GDI_TAUNT_3], true },                // EVA - Surrender is your only logical option
        SpeechRemapStruct { Speech[VOX_GDI_TAUNT_4], true },                // EVA - Your defeat is at hand
        SpeechRemapStruct { Speech[VOX_GDI_TAUNT_5], true },                // EVA - You have been marked for termination
        SpeechRemapStruct { Speech[VOX_GDI_TAUNT_6], true },                // EVA - Time to erase human factor from this equation
        SpeechRemapStruct { Speech[VOX_GDI_TAUNT_7], true },                // EVA - Your probability of success is insignificant and dropping
        SpeechRemapStruct { Speech[VOX_GDI_TAUNT_8], true },                // EVA - Surrender is acceptable and preferable against the odds you are facing
        SpeechRemapStruct { Speech[VOX_GDI_TAUNT_9], true },                // EVA - Data received indicates that you  are a threat, prepare for sterilization
        SpeechRemapStruct { Speech[VOX_GDI_TAUNT_10], true },               // EVA - Defeat of enemy predicted in T minus 3 2 1
        SpeechRemapStruct { Speech[VOX_NOD_TAUNT_1], true },                // CABAL - Inferior tactics detected
        SpeechRemapStruct { Speech[VOX_NOD_TAUNT_2], true },                // CABAL - Observe superior tactics while you still have human eyes
        SpeechRemapStruct { Speech[VOX_NOD_TAUNT_3], true },                // CABAL - Your defeat is at hand
        SpeechRemapStruct { Speech[VOX_NOD_TAUNT_4], true },                // CABAL - You have been marked for termination
        SpeechRemapStruct { Speech[VOX_NOD_TAUNT_5], true },                // CABAL - Time to erase human factor from this equation
        SpeechRemapStruct { Speech[VOX_NOD_TAUNT_6], true },                // CABAL - Prepare for decimation, for you are not worthy of assimilation
        SpeechRemapStruct { Speech[VOX_NOD_TAUNT_7], true },                // CABAL - You make this easy fleshbag
        SpeechRemapStruct { Speech[VOX_NOD_TAUNT_8], true },                // CABAL - We tire of your insignificant defiance
        SpeechRemapStruct { Speech[VOX_NOD_TAUNT_9], true },                // CABAL - The sacrifice of the many is what the pleasure for the few
        SpeechRemapStruct { Speech[VOX_NOD_TAUNT_10], true },               // CABAL - Terminator protocol initiated, proceed it with final sweep 
        SpeechRemapStruct { Speech[VOX_33_N000], false },                   // GDI - Where the hell are those reinforcements
        SpeechRemapStruct { Speech[VOX_33_N002], false },                   // GDI - Its Nod sir, they came out of nowhere, we have to get back to the base and rebuild the barracks and refinery before they hit us again
        SpeechRemapStruct { Speech[VOX_33_N004], false },                   // GDI - Sir, looks like they tore this place apart already, Nod must be ready to ship the artifact out, we don't have much time 
        SpeechRemapStruct { Speech[VOX_33_N008], false },                   // GDI - Here they come
        SpeechRemapStruct { Speech[VOX_33_N010], false },                   // GDI - Is there any tech left at the Nod base
        SpeechRemapStruct { Speech[VOX_33_N012], false },                   // GDI - These critters don't look too friendly
        SpeechRemapStruct { Speech[VOX_33_N014], false },                   // GDI - The cavalry has arrived
        SpeechRemapStruct { Speech[VOX_33_N020], false },                   // GDI - I believe there is an old GDI base near, could be worth looking into
        SpeechRemapStruct { Speech[VOX_33_N022], false },                   // GDI - Here comes Kane's welcoming committee
        SpeechRemapStruct { Speech[VOX_33_N024], false },                   // GDI - What's the ETA on that MCV, this UFO gives me heebie jeebies
        SpeechRemapStruct { Speech[VOX_35_N000], false },                   // GDI - This is lieutenant Stark, we have touched down on our waiting Tratos for immediate evac, get him over here, ASAP
        SpeechRemapStruct { Speech[VOX_35_N002], false },                   // GDI - Nod forces are converging on our position sir, with all due respect, move your ass
        SpeechRemapStruct { Speech[VOX_35_N004], false },                   // GDI - Coordinates transmitted and confirmed, ETA for airstrike is 5 minutes
        SpeechRemapStruct { Speech[VOX_35_N008], false },                   // GDI - Pickup service
        SpeechRemapStruct { Speech[VOX_35_N010], false },                   // GDI - Negative, I can make it
        SpeechRemapStruct { Speech[VOX_35_N012], false },                   // GDI - I said, I can make it
        SpeechRemapStruct { Speech[VOX_35_N014], false },                   // GDI - I can UAAAH!
        SpeechRemapStruct { Speech[VOX_40_N000_1], false },                 // BOTH - We have been touched by the spirit hand of Kane, and are ready to serve the technology of peace, peace through power
        SpeechRemapStruct { Speech[VOX_00_N000], false },                   // EVA - To begin harvesting tiberium, build a tiberium refinery
        SpeechRemapStruct { Speech[VOX_00_N002], false },                   // EVA - Harvester will automatically begin harvesting any nearby tiberium
        SpeechRemapStruct { Speech[VOX_00_N004], false },                   // EVA - In order to build more troops, build a barracks
        SpeechRemapStruct { Speech[VOX_00_N006], false },                   // EVA - Destroy all Nod forces in the area
        SpeechRemapStruct { Speech[VOX_00_N018], false },                   // EVA - Civilian killed
        SpeechRemapStruct { Speech[VOX_00_N020], false },                   // EVA - Nod sam sites destroyed
        SpeechRemapStruct { Speech[VOX_00_N022], false },                   // EVA - GDI transports en route
        SpeechRemapStruct { Speech[VOX_00_N024], false },                   // EVA - Civilians evacuated, objective complete
        SpeechRemapStruct { Speech[VOX_00_N032], false },                   // EVA - Site secure, objective complete
        SpeechRemapStruct { Speech[VOX_00_N034], false },                   // EVA - Technology center captured, objective complete
        SpeechRemapStruct { Speech[VOX_00_N040], false },                   // EVA - Base destroyed, objective complete
        SpeechRemapStruct { Speech[VOX_00_N042], false },                   // EVA - It would be advantageous to destroy any bridges leading from this region
        SpeechRemapStruct { Speech[VOX_00_N044], false },                   // EVA - Bridges destroyed, objective complete
        SpeechRemapStruct { Speech[VOX_00_N052], false },                   // EVA - UFO under attack
        SpeechRemapStruct { Speech[VOX_00_N054], false },                   // EVA - UFO destroyed, mission failed
        SpeechRemapStruct { Speech[VOX_00_N055], false },                   // EVA - Array destroyed, objective complete
        SpeechRemapStruct { Speech[VOX_00_N056], false },                   // EVA - Incoming transmission
        SpeechRemapStruct { Speech[VOX_00_N058], false },                   // EVA - Tratos evacuated, primary objective complete
        SpeechRemapStruct { Speech[VOX_00_N059], false },                   // EVA - Transport destroyed, mission failed
        SpeechRemapStruct { Speech[VOX_01_N320], false },                   // CABAL - Intruders detected
        SpeechRemapStruct { Speech[VOX_01_N322], false },                   // CABAL - Probable objective is rescue of mutant prisoners
        SpeechRemapStruct { Speech[VOX_01_N324], false },                   // CABAL - Kill all prisoners
        SpeechRemapStruct { Speech[VOX_01_N326], false },                   // CABAL - All forces converge
        SpeechRemapStruct { Speech[VOX_00_N068], false },                   // EVA - Ion storm approaching, ETA 5 minutes
        SpeechRemapStruct { Speech[VOX_00_N070], false },                   // EVA - Ion storm abating
        SpeechRemapStruct { Speech[VOX_00_N072], false },                   // EVA - Air power will be ineffective during ion storms
        SpeechRemapStruct { Speech[VOX_00_N074], false },                   // EVA - Dam destroyed, mission complete 
        SpeechRemapStruct { Speech[VOX_00_N075], false },                   // EVA - Dam sighted, target confirmed
        SpeechRemapStruct { Speech[VOX_01_N900], false },                   // CABAL - Warning, regulators offline
        SpeechRemapStruct { Speech[VOX_01_N901], false },                   // CABAL - Dam integrity failing
        SpeechRemapStruct { Speech[VOX_00_N084], false },                   // EVA - Enemy sam sites detected
        SpeechRemapStruct { Speech[VOX_00_N086], false },                   // EVA - All sam sites must be destroyed before dropships could be deployed
        SpeechRemapStruct { Speech[VOX_00_N088], false },                   // EVA - Sam sites destroyed, dropships inbound
        SpeechRemapStruct { Speech[VOX_00_N090], false },                   // <missing>
        SpeechRemapStruct { Speech[VOX_00_N092], false },                   // <missing>
        SpeechRemapStruct { Speech[VOX_00_N094], false },                   // <missing>
        SpeechRemapStruct { Speech[VOX_00_N096], false },                   // EVA - Command center destroyed, mission complete
        SpeechRemapStruct { Speech[VOX_00_N098], false },                   // EVA - Warning. inbound tactical nuclear missile detected
        SpeechRemapStruct { Speech[VOX_01_N328], false },                   // CABAL - Main power offline, activating emergency generators
        SpeechRemapStruct { Speech[VOX_01_N330], false },                   // CABAL - Main power restored
        SpeechRemapStruct { Speech[VOX_00_N112], false },                   // EVA - Perimeter deactivated
        SpeechRemapStruct { Speech[VOX_00_N114], false },                   // EVA - Reinforcements inbound
        SpeechRemapStruct { Speech[VOX_00_N128], false },                   // EVA - Bridge repaired
        SpeechRemapStruct { Speech[VOX_00_N130], false },                   // EVA - Train returning to Nod base
        SpeechRemapStruct { Speech[VOX_00_N132], false },                   // EVA - Reinforcement en route
        SpeechRemapStruct { Speech[VOX_00_N134], false },                   // EVA - Capture Nod tech center to recover the crystals
        SpeechRemapStruct { Speech[VOX_00_N136], false },                   // EVA - Train disabled, carryall inbound
        SpeechRemapStruct { Speech[VOX_00_N138], false },                   // EVA - Crystals destroyed, mission failed
        SpeechRemapStruct { Speech[VOX_00_N140], false },                   // EVA - Warning, rogue ion storm forming directly over this sector, pilot, return to base
        SpeechRemapStruct { Speech[VOX_00_N142], false },                   // EVA - Return to base, pilot
        SpeechRemapStruct { Speech[VOX_00_N156], false },                   // EVA - Force detected, enemy units en route
        SpeechRemapStruct { Speech[VOX_00_N158], false },                   // EVA - Mutant lost, mission failed
        SpeechRemapStruct { Speech[VOX_00_N160], false },                   // EVA - Airstrike ready
        SpeechRemapStruct { Speech[VOX_00_N162], false },                   // EVA - Transports inbound
        SpeechRemapStruct { Speech[VOX_00_N166], false },                   // EVA - Supply base destroyed, mission complete
        SpeechRemapStruct { Speech[VOX_00_N168], false },                   // EVA - Mutants en route to power grid
        SpeechRemapStruct { Speech[VOX_00_N180], false },                   // EVA - C4 planted, mission complete
        SpeechRemapStruct { Speech[VOX_00_N182], false },                   // EVA - Ghost stalker terminated, mission failed
        SpeechRemapStruct { Speech[VOX_00_N188], false },                   // EVA - Missile complex destroyed, mission complete
        SpeechRemapStruct { Speech[VOX_00_N190], false },                   // EVA - Tiberium missile inbound
        SpeechRemapStruct { Speech[VOX_00_N192], false },                   // EVA - Ion storm will disable fighters
        SpeechRemapStruct { Speech[VOX_00_N206], false },                   // EVA - Mutants detected, enemy forces en route
        SpeechRemapStruct { Speech[VOX_00_N208], false },                   // EVA - Fighter production facility located and position recorded, dropship en route
        SpeechRemapStruct { Speech[VOX_00_N210], false },                   // EVA - Fighter production facility destroyed, mission complete
        SpeechRemapStruct { Speech[VOX_00_N224], false },                   // <missing>
        SpeechRemapStruct { Speech[VOX_00_N226], false },                   // <missing>
        SpeechRemapStruct { Speech[VOX_00_N228], false },                   // <missing>
        SpeechRemapStruct { Speech[VOX_00_N236], false },                   // EVA - Kodiak under attack
        SpeechRemapStruct { Speech[VOX_00_N238], false },                   // EVA - Storm abating, commence attack on Nod forces
        SpeechRemapStruct { Speech[VOX_00_N240], false },                   // EVA - Kodiak destroyed, mission failed
        SpeechRemapStruct { Speech[VOX_00_N239], false },                   // EVA - Kodiak in critical condition
        SpeechRemapStruct { Speech[VOX_00_N241], false },                   // EVA - Eye of the storm has been entered, equipment usage now at its maximum efficiency
        SpeechRemapStruct { Speech[VOX_00_N243], false },                   // EVA - Tiberium lifeform detected
        SpeechRemapStruct { Speech[VOX_00_N245], false },                   // EVA - Re-entering ion storm, caution is advised
        SpeechRemapStruct { Speech[VOX_00_N247], false },                   // EVA - Clear the zone for MCV dropship deployment
        SpeechRemapStruct { Speech[VOX_00_N248], false },                   // EVA - Philadelphia in range, ICBM launch detected, mission failed
        SpeechRemapStruct { Speech[VOX_00_N249], false },                   // EVA - Tiberium lifeform detected
        SpeechRemapStruct { Speech[VOX_00_N250], false },                   // EVA - Tiberium missile launched, mission failed
        SpeechRemapStruct { Speech[VOX_00_N251], false },                   // EVA - ICBMs destroyed, Philadelphia is out of danger, proceed with tiberium missile destruction
        SpeechRemapStruct { Speech[VOX_00_N252], false },                   // EVA - Nod has deployed several ICBMs which they will use to destroy the Philadelphia once it is in orbit over this sector
        SpeechRemapStruct { Speech[VOX_00_N254], false },                   // EVA - To stop them, you must destroy the other 3 launchers before the Philadelphia is in range
        SpeechRemapStruct { Speech[VOX_00_N255], false },                   // EVA - Civilian city is under attack
        SpeechRemapStruct { Speech[VOX_38_N000], false },                   // Nod - Move it, move it, get back to the base
        SpeechRemapStruct { Speech[VOX_38_N002], false },                   // Nod - Ahh sir, tiberium is lethal to unprotected infantry
        SpeechRemapStruct { Speech[VOX_38_N006], false },                   // Nod - Laser turret, run for it
        SpeechRemapStruct { Speech[VOX_40_N000], false },                   // BOTH - We have been touched by the spirit hand of Kane, and are ready to serve the technology of peace, peace through power
        SpeechRemapStruct { Speech[VOX_41_N000], false },                   // Nod - Stand and identify yourself in the name of Kane
        SpeechRemapStruct { Speech[VOX_41_N002], false },                   // Nod - Sound the alarm, Slav's forces are here
        SpeechRemapStruct { Speech[VOX_41_N004], false },                   // Nod - Base command, this is post TK421, we have got
        SpeechRemapStruct { Speech[VOX_41_N006], false },                   // Nod - We just got word from general Hassan, he is moving up the Cairo base, he wants it, what, hey what you doing
        SpeechRemapStruct { Speech[VOX_43_N000], false },                   // Nod - Beep, I have the codes
        SpeechRemapStruct { Speech[VOX_01_N000], false },                   // CABAL - Harvest the tiberium to the north
        SpeechRemapStruct { Speech[VOX_01_N002], false },                   // CABAL - Destroy all Hassan's elite guard
        SpeechRemapStruct { Speech[VOX_01_N004], false },                   // CABAL - To get production online, build a tiberium refinery
        SpeechRemapStruct { Speech[VOX_01_N006], false },                   // CABAL - Establishing battle controls, please standby
        SpeechRemapStruct { Speech[VOX_01_N008], false },                   // CABAL - Battle controls established
        SpeechRemapStruct { Speech[VOX_01_N010], false },                   // CABAL - Power levels are low, construct more power plants
        SpeechRemapStruct { Speech[VOX_01_N005], false },                   // CABAL - Base perimeter has been breached
        SpeechRemapStruct { Speech[VOX_01_N007], false },                   // CABAL - To build or train, left click on the icons located in the sidebar
        SpeechRemapStruct { Speech[VOX_01_N009], false },                   // CABAL - Tiberium is hazardous to unprotected infantry, caution is advised
        SpeechRemapStruct { Speech[VOX_01_N011], false },                   // CABAL - To repair a structure, left click on the wrench icon in the sidebar and then left click on the structure
        SpeechRemapStruct { Speech[VOX_01_N020], false },                   // CABAL - Capture the TV station to the east
        SpeechRemapStruct { Speech[VOX_01_N022], false },                   // CABAL - To repair a bridge, send an engineer into the bridge repair hut located at the base of the bridge
        SpeechRemapStruct { Speech[VOX_01_N024], false },                   // CABAL - Destroy the remainder of Hassan's guard
        SpeechRemapStruct { Speech[VOX_01_N026], false },                   // CABAL - To capture a building, select an engineer, place the cursor over the intended target and left click 
        SpeechRemapStruct { Speech[VOX_01_N027], false },                   // CABAL - To deploy a vehicle, select it, place the cursor over vehicle and left click it
        SpeechRemapStruct { Speech[VOX_01_N032], false },                   // CABAL - Move to an open area and build your base
        SpeechRemapStruct { Speech[VOX_01_N034], false },                   // CABAL - After you locate Hassan's pyramid, use your tick tanks to destroy it
        SpeechRemapStruct { Speech[VOX_01_N036], false },                   // CABAL - When Hassan flees like a dog, capture him
        SpeechRemapStruct { Speech[VOX_01_N038], false },                   // CABAL - Capturing the local radar towers, will allow me to extend our propaganda and expand our divination efforts
        SpeechRemapStruct { Speech[VOX_01_N040], false },                   // CABAL - MCV has arrived to the south east
        SpeechRemapStruct { Speech[VOX_01_N054], false },                   // CABAL - It seems that certain structures in this region attract lightning, creating a kind of safety zone
        SpeechRemapStruct { Speech[VOX_01_N055], false },                   // CABAL - Use them to your advantage
        SpeechRemapStruct { Speech[VOX_01_N064], false },                   // CABAL - GDI base operational
        SpeechRemapStruct { Speech[VOX_01_N066], false },                   // CABAL - Tacitus has been acquired
        SpeechRemapStruct { Speech[VOX_01_N068], false },                   // CABAL - Tiberium lifeform detected
        SpeechRemapStruct { Speech[VOX_01_N070], false },                   // CABAL - Mutants vermin detected
        SpeechRemapStruct { Speech[VOX_01_N072], false },                   // CABAL - GDI dropship detected
        SpeechRemapStruct { Speech[VOX_01_N074], false },                   // CABAL - Bullet train departing
        SpeechRemapStruct { Speech[VOX_01_N076], false },                   // CABAL - Prevent that train from leaving and retrieve the tacitus
        SpeechRemapStruct { Speech[VOX_01_N086], false },                   // CABAL - Protect your engineers, they are your only hope of capturing the GDI base
        SpeechRemapStruct { Speech[VOX_01_N088], false },                   // CABAL - Congratulation on your success
        SpeechRemapStruct { Speech[VOX_01_N090], false },                   // CABAL - I will send an APC now to rendezvous with you at the GDI base
        SpeechRemapStruct { Speech[VOX_01_N100], false },                   // CABAL - Your forces have been detected
        SpeechRemapStruct { Speech[VOX_01_N102], false },                   // CABAL - Prevent GDI's evacuation at all cost
        SpeechRemapStruct { Speech[VOX_01_N114], false },                   // CABAL - Incoming transport detected
        SpeechRemapStruct { Speech[VOX_01_N116], false },                   // CABAL - Mutant lifeform detected onboard
        SpeechRemapStruct { Speech[VOX_01_N128], false },                   // CABAL - Mutants located
        SpeechRemapStruct { Speech[VOX_01_N130], false },                   // CABAL - Tunnel secured, NCV en route
        SpeechRemapStruct { Speech[VOX_01_N132], false },                   // CABAL - Research facility located
        SpeechRemapStruct { Speech[VOX_01_N134], false },                   // CABAL - Research facility destroyed, objective complete
        SpeechRemapStruct { Speech[VOX_01_N136], false },                   // CABAL - Biotoxin convoy approaching
        SpeechRemapStruct { Speech[VOX_01_N144], false },                   // CABAL - Biotoxin tankers located
        SpeechRemapStruct { Speech[VOX_01_N156], false },                   // CABAL - GDI bullet train arriving at outpost
        SpeechRemapStruct { Speech[VOX_01_N158], false },                   // CABAL - GDI bullet train arriving at GDI main prison facility
        SpeechRemapStruct { Speech[VOX_01_N160], false },                   // CABAL - Transport has arrived
        SpeechRemapStruct { Speech[VOX_01_N162], false },                   // CABAL - Transport lost
        SpeechRemapStruct { Speech[VOX_01_N164], false },                   // CABAL - Transport has been detected
        SpeechRemapStruct { Speech[VOX_01_N174], false },                   // CABAL - The creature is the tiberium substance we seek, use it
        SpeechRemapStruct { Speech[VOX_01_N176], false },                   // CABAL - Tiberium missile ready
        SpeechRemapStruct { Speech[VOX_01_N178], false },                   // CABAL - Do not allow the construction yard to be destroyed
        SpeechRemapStruct { Speech[VOX_01_N180], false },                   // CABAL - You must build a tiberium waste facility to store the substance
        SpeechRemapStruct { Speech[VOX_01_N192], false },                   // CABAL - Convoy truck lost
        SpeechRemapStruct { Speech[VOX_01_N194], false },                   // CABAL - Tiberium missile ready
        SpeechRemapStruct { Speech[VOX_01_N196], false },                   // CABAL - Convoy inbound
        SpeechRemapStruct { Speech[VOX_01_N208], false },                   // CABAL - Stealth is key
        SpeechRemapStruct { Speech[VOX_01_N210], false },                   // CABAL - Spy lost, mission failed
        SpeechRemapStruct { Speech[VOX_01_N212], false },                   // CABAL - COM center infiltrated, location detected
        SpeechRemapStruct { Speech[VOX_01_N228], false },                   // CABAL - Convoy sighted
        SpeechRemapStruct { Speech[VOX_01_N230], false },                   // CABAL - McNeil killed, mission failed
        SpeechRemapStruct { Speech[VOX_01_N232], false },                   // CABAL - McNeil captured, mission complete
        SpeechRemapStruct { Speech[VOX_01_N233], false },                   // CABAL - Production facility destroyed
        SpeechRemapStruct { Speech[VOX_01_N227], false },                   // CABAL - If he detects the trap, capture him before he can flee the sector
        SpeechRemapStruct { Speech[VOX_01_N229], false },                   // CABAL - Transport ETA 30 minutes, do not let McNeil escape
        SpeechRemapStruct { Speech[VOX_01_N231], false },                   // CABAL - All toxin soldiers killed, mission failed
        SpeechRemapStruct { Speech[VOX_01_N234], false },                   // CABAL - McNeil escaped, mission failed
        SpeechRemapStruct { Speech[VOX_01_N256], false },                   // CABAL - Orbit 1 complete
        SpeechRemapStruct { Speech[VOX_01_N258], false },                   // CABAL - Orbit 2 complete
        SpeechRemapStruct { Speech[VOX_01_N260], false },                   // CABAL - Orbit 3 complete, mission failed
        SpeechRemapStruct { Speech[VOX_01_N262], false },                   // CABAL - Perimeter deactivated, commence attack
        SpeechRemapStruct { Speech[VOX_01_N264], false },                   // CABAL - ICBM launcher lost, mission failed
        SpeechRemapStruct { Speech[VOX_01_N266], false },                   // CABAL - ICBM launcher under attack
        SpeechRemapStruct { Speech[VOX_01_N268], false },                   // CABAL - Ion cannon firing, resending control codes
        SpeechRemapStruct { Speech[VOX_01_N270], false },                   // CABAL - The ion cannon is ours
        SpeechRemapStruct { Speech[VOX_01_N284], false },                   // CABAL - Spy killed, mission failed
        SpeechRemapStruct { Speech[VOX_01_N286], false },                   // CABAL - Proceed to evac location, transport en route
        SpeechRemapStruct { Speech[VOX_10_N032], false },                   // GDI - A medical facility is located to the east
        SpeechRemapStruct { Speech[VOX_10_N034], false },                   // GDI - A medical facility is located to the west
        SpeechRemapStruct { Speech[VOX_10_N036], false },                   // GDI - We must get Tratos out of that facility, but try to be subtle, if we are detected he is as good as dead
        SpeechRemapStruct { Speech[VOX_10_N038], false },                   // GDI - So much for subtlety
        SpeechRemapStruct { Speech[VOX_10_N040], false },                   // GDI - Tratos, no
        SpeechRemapStruct { Speech[VOX_10_N042], false },                   // GDI - We must get Tratos to the transport
        SpeechRemapStruct { Speech[VOX_10_N044], false },                   // GDI - Our work here is done, we are out of here
        SpeechRemapStruct { Speech[VOX_10_N046], false },                   // GDI - They have seen us, fallback
        SpeechRemapStruct { Speech[VOX_44_N000], false },                   // GDI - Sir, we are here and waiting for your arrival
        SpeechRemapStruct { Speech[VOX_36_N000], false },                   // GDI - That train goes to the power grid we told you of, get us onboard
        SpeechRemapStruct { Speech[VOX_36_N002], false },                   // GDI - What an opportunity, thanks, will enjoy this
        SpeechRemapStruct { Speech[VOX_36_N004], false },                   // GDI - Somethings wrong, we should have been there by now
        SpeechRemapStruct { Speech[VOX_36_N008], false },                   // GDI - We have been betrayed, GDI bots have kidnapped Tratos
        SpeechRemapStruct { Speech[VOX_37_N000], false },                   // GDI - Thanks for your help
        SpeechRemapStruct { Speech[VOX_37_N002], false },                   // GDI - They may be on to us
        SpeechRemapStruct { Speech[VOX_38_N004], false },                   // Nod - We need more men, train them from the hand of Nod
        SpeechRemapStruct { Speech[VOX_38_N008], false },                   // Nod - The traitors are coming, blow the bridge
        SpeechRemapStruct { Speech[VOX_38_N010], false },                   // Nod - Sir, Hassan is escaping, we are moving to intercept him now
        SpeechRemapStruct { Speech[VOX_38_N012], false },                   // Nod - Hassan has been captured, we are heading in now sir
        SpeechRemapStruct { Speech[VOX_38_N014], false },                   // Nod - We got an old stockpile here sir, all very salvageable
        SpeechRemapStruct { Speech[VOX_38_N016], false },                   // Nod - They are pulling back, reinforcements, the mutants have turned on us
        SpeechRemapStruct { Speech[VOX_38_N018], false },                   // Nod - They are everywhere
        SpeechRemapStruct { Speech[VOX_38_N020], false },                   // Nod - Lets get him to the convoy point
        SpeechRemapStruct { Speech[VOX_38_N022], false },                   // Nod - Take point soldier
        SpeechRemapStruct { Speech[VOX_38_N024], false },                   // Nod - Glad to see you free, sir
        SpeechRemapStruct { Speech[VOX_38_N026], false },                   // Nod - The lieutenant is being held at an outpost to the east
        SpeechRemapStruct { Speech[VOX_38_N028], false },                   // Nod - Woaho sir, careful, the ice won't support the weight of multiple vehicles crossing at once 
        SpeechRemapStruct { Speech[VOX_39_N000], false },                   // Nod - I am free, now it is time for some payback, for the GDI loving traitor Hassan, I have hidden MCV and some more forces, that I will pledge to your fight 
        SpeechRemapStruct { Speech[VOX_47_N000], false },                   // Nod - Don't shoot, don't shoot, I can download GDI's evac locations if you let me live, please let me live, praise Kane
        SpeechRemapStruct { Speech[VOX_00_I020], true },                    // EVA/CABAL - Incoming transmission

        // Firestorm additions.
        SpeechRemapStruct { Speech[VOX_00_I500], true },          // EVA/CABAL - Objective complete
        SpeechRemapStruct { Speech[VOX_00_I502], true },          // EVA/CABAL - Final objective complete
        SpeechRemapStruct { Speech[VOX_00_I504], true },          // EVA/CABAL - Unable to comply, mobile warfactory deployed <CABAL> Fist of Nod deployed
        SpeechRemapStruct { Speech[VOX_00_N400], false },         // EVA - Kodiak located
        SpeechRemapStruct { Speech[VOX_00_N402], false },         // EVA - Tacitus acquired, proceed to beacon for extraction
        SpeechRemapStruct { Speech[VOX_00_N404], false },         // EVA - Tacitus lost
        SpeechRemapStruct { Speech[VOX_00_N406], false },         // EVA - Tacitus found
        SpeechRemapStruct { Speech[VOX_00_N408], false },         // EVA - No one has captured the tacitus, recover it at all cost
        SpeechRemapStruct { Speech[VOX_00_N410], false },         // EVA - Find and evacuate any civilians in the area
        SpeechRemapStruct { Speech[VOX_00_N412], false },         // EVA - Maintain all factories until reinforcements arrive
        SpeechRemapStruct { Speech[VOX_00_N414], false },         // EVA - Escort the civilians to the orca transport for immediate airlift
        SpeechRemapStruct { Speech[VOX_00_N416], false },         // EVA - Human leader neutralized
        SpeechRemapStruct { Speech[VOX_00_N418], false },         // EVA - Mutant leader neutralized
        SpeechRemapStruct { Speech[VOX_00_N420], false },         // EVA - Food center under attack
        SpeechRemapStruct { Speech[VOX_00_N422], false },         // EVA - Food center destroyed
        SpeechRemapStruct { Speech[VOX_00_N424], false },         // EVA - Water purifier is under attack
        SpeechRemapStruct { Speech[VOX_00_N426], false },         // EVA - Water purifier is destroyed
        SpeechRemapStruct { Speech[VOX_00_N428], false },         // EVA - The riots were incited by 2 members of each side's opposing factions
        SpeechRemapStruct { Speech[VOX_00_N430], false },         // EVA - Neutralize these leaders to end hostilities
        SpeechRemapStruct { Speech[VOX_00_N432], false },         // EVA - Riot leader neutralized
        SpeechRemapStruct { Speech[VOX_00_N434], false },         // EVA - First riot leader neutralized
        SpeechRemapStruct { Speech[VOX_00_N436], false },         // EVA - Second riot leader neutralized
        SpeechRemapStruct { Speech[VOX_00_N438], false },         // EVA - Third riot leader neutralized
        SpeechRemapStruct { Speech[VOX_00_N440], false },         // EVA - Fourth riot leader neutralized
        SpeechRemapStruct { Speech[VOX_00_N442], false },         // EVA - All riot leaders have been neutralized
        SpeechRemapStruct { Speech[VOX_00_N444], false },         // EVA - Enemy reinforcements have arrived
        SpeechRemapStruct { Speech[VOX_00_N446], false },         // EVA - Destroy the 2 bridges in the area
        SpeechRemapStruct { Speech[VOX_00_N448], false },         // EVA - Penetrate the base and disable CABAL's defenses
        SpeechRemapStruct { Speech[VOX_00_N450], false },         // EVA - Use an engineer to capture CABAL's core
        SpeechRemapStruct { Speech[VOX_00_N452], false },         // EVA - Outpost located
        SpeechRemapStruct { Speech[VOX_00_N454], false },         // EVA - Destroy all CABAL forces
        SpeechRemapStruct { Speech[VOX_00_N456], false },         // EVA - Return to outpost and evacuate Dr. Boudreau
        SpeechRemapStruct { Speech[VOX_00_N458], false },         // EVA - GDI base is located due east of your position
        SpeechRemapStruct { Speech[VOX_00_N460], false },         // EVA - Proceed with caution
        SpeechRemapStruct { Speech[VOX_00_N462], false },         // EVA - There are several civilian villages in the area, inform them of CABAL's intent
        SpeechRemapStruct { Speech[VOX_00_N464], false },         // EVA - Village successfully warned
        SpeechRemapStruct { Speech[VOX_00_N466], false },         // EVA - All villages successfully warned
        SpeechRemapStruct { Speech[VOX_00_N468], false },         // EVA - CABAL has begun operations, we have lost contact with the GDI base in this sector
        SpeechRemapStruct { Speech[VOX_00_N470], false },         // EVA - We have lost contact with the GDI base in this region, it is theorized CABAL is to blame
        SpeechRemapStruct { Speech[VOX_00_N472], false },         // EVA - GDI base in this sector is currently under siege
        SpeechRemapStruct { Speech[VOX_00_N474], false },         // EVA - 3 small civilian enclaves in this area must be warned
        SpeechRemapStruct { Speech[VOX_00_N478], false },         // EVA - 2 more small civilian outposts must be warned
        SpeechRemapStruct { Speech[VOX_00_N479], false },         // EVA - 1 final village must be warned
        SpeechRemapStruct { Speech[VOX_00_N480], false },         // EVA - The city of Trondheim must be warned
        SpeechRemapStruct { Speech[VOX_00_N482], false },         // EVA - Destroy CABAL's base and forces
        SpeechRemapStruct { Speech[VOX_00_N484], false },         // EVA - Infected cyborg introduced into factory
        SpeechRemapStruct { Speech[VOX_00_N486], false },         // EVA - Deliver the infected cyborg into the communication network
        SpeechRemapStruct { Speech[VOX_00_N488], false },         // EVA - Destroy the cyborg production plant
        SpeechRemapStruct { Speech[VOX_00_N490], false },         // EVA - Deliver the infected cyborg into CABAL's base undetected and get it to the communications network
        SpeechRemapStruct { Speech[VOX_00_N492], false },         // EVA - There is another communications array in CABAL's main base
        SpeechRemapStruct { Speech[VOX_00_N494], false },         // EVA - Deliver a second modified cyborg to the new array
        SpeechRemapStruct { Speech[VOX_00_N496], false },         // EVA - Multiple missile launches detected
        SpeechRemapStruct { Speech[VOX_00_N498], false },         // EVA - Destroy all of CABAL's harvesters, refineries and silos
        SpeechRemapStruct { Speech[VOX_00_N500], false },         // EVA - Control station captured
        SpeechRemapStruct { Speech[VOX_00_N502], false },         // EVA - There are 3 control stations in this sector regulating CABAL's defenses
        SpeechRemapStruct { Speech[VOX_00_N504], false },         // EVA - Capture these stations and retrieve the codes to disable his defenses
        SpeechRemapStruct { Speech[VOX_00_N506], false },         // EVA - Enemy bridges may allow for unit reinforcement, there destruction would be beneficial in completing this mission
        SpeechRemapStruct { Speech[VOX_00_N508], false },         // EVA - Locating the technicians may help in disabling the laser fencing
        SpeechRemapStruct { Speech[VOX_00_N510], false },         // EVA - Probable location within civilian outpost to the north
        SpeechRemapStruct { Speech[VOX_01_N400], false },         // CABAL - CABAL online
        SpeechRemapStruct { Speech[VOX_01_N402], false },         // CABAL - Perhaps you should attempt to kill their leader
        SpeechRemapStruct { Speech[VOX_01_N404], false },         // CABAL - Ghost stalker killed, probability of success now at 10 percent
        SpeechRemapStruct { Speech[VOX_01_N406], false },         // CABAL - Juggernaut destroyed, probability of success drastically reduced
        SpeechRemapStruct { Speech[VOX_01_N408], false },         // CABAL - Find the temple and recover the tacitus
        SpeechRemapStruct { Speech[VOX_01_N410], false },         // CABAL - Cyborg replication error, reset system
        SpeechRemapStruct { Speech[VOX_01_N412], false },         // CABAL - System reset, normal parameter restored
        SpeechRemapStruct { Speech[VOX_01_N414], false },         // CABAL - GDI patrol, near
        SpeechRemapStruct { Speech[VOX_01_N416], false },         // CABAL - Tiberium lifeform detected
        SpeechRemapStruct { Speech[VOX_01_N418], false },         // CABAL - GDI has detected you
        SpeechRemapStruct { Speech[VOX_01_N420], false },         // CABAL - Remain hidden from the GDI forces
        SpeechRemapStruct { Speech[VOX_01_N422], false },         // CABAL - Destroy all civilian structures without being detected
        SpeechRemapStruct { Speech[VOX_01_N424], false },         // CABAL - Eliminate all GDI and civilian life without being detected
        SpeechRemapStruct { Speech[VOX_01_N426], false },         // CABAL - Use the toxin soldiers to capture civilians
        SpeechRemapStruct { Speech[VOX_01_N428], false },         // CABAL - Bait the tiberium lifeforms by luring them with the drugged civilians
        SpeechRemapStruct { Speech[VOX_01_N430], false },         // CABAL - Leave the tiberium lifeforms to the GDI and civilian occupied area
        SpeechRemapStruct { Speech[VOX_01_N432], false },         // CABAL - Once the lifeform has devoured the civilians, it will proceed to the settlements
        SpeechRemapStruct { Speech[VOX_01_N434], false },         // CABAL - Use this location as a staging point for your operation
        SpeechRemapStruct { Speech[VOX_01_N436], false },         // CABAL - GDI cannot reach you here
        SpeechRemapStruct { Speech[VOX_01_N438], false },         // CABAL - Outpost located
        SpeechRemapStruct { Speech[VOX_01_N440], false },         // CABAL - Tacitus found
        SpeechRemapStruct { Speech[VOX_01_N442], false },         // CABAL - The tacitus is safely in our hands
        SpeechRemapStruct { Speech[VOX_01_N444], false },         // CABAL - Exterminate the mutant vermin
        SpeechRemapStruct { Speech[VOX_01_N446], false },         // CABAL - Locate the mutant outpost
        SpeechRemapStruct { Speech[VOX_01_N448], false },         // CABAL - Find the truck containing the tacitus and return it to the beacon
        SpeechRemapStruct { Speech[VOX_01_N450], false },         // CABAL - Eliminate all mutant forces
        SpeechRemapStruct { Speech[VOX_01_N452], false },         // CABAL - Ha, run humans
        SpeechRemapStruct { Speech[VOX_99_N454], false },         // CABAL - You dare to attack me, how pathetic
        SpeechRemapStruct { Speech[VOX_99_N456], false },         // CABAL - Cybernetic intelligence will always be superior
        SpeechRemapStruct { Speech[VOX_99_N458], false },         // CABAL - By the way, missile launch detected
        SpeechRemapStruct { Speech[VOX_99_N460], false },         // CABAL - Ha ha ha ha ha ha ha ha ha ha ha ha
        SpeechRemapStruct { Speech[VOX_01_N462], false },         // CABAL - Activating defense protocol firestorm now
        SpeechRemapStruct { Speech[VOX_99_N464], false },         // CABAL - Activating defense protocol defender now
        SpeechRemapStruct { Speech[VOX_99_N466], false },         // CABAL - Miscalculation in enemy capability, compensation initiated
        SpeechRemapStruct { Speech[VOX_01_N468], false },         // CABAL - You amuse me
        SpeechRemapStruct { Speech[VOX_01_N470], false },         // CABAL - SAM sites destroyed, airpower incoming
        SpeechRemapStruct { Speech[VOX_01_N472], false },         // CABAL - You have been detected, Tratos is escaping by air transport
        SpeechRemapStruct { Speech[VOX_01_N474], false },         // CABAL - You have failed, Tratos has escaped
        SpeechRemapStruct { Speech[VOX_01_N476], false },         // CABAL - The arrays have been destroyed, sensors are now down
        SpeechRemapStruct { Speech[VOX_01_N478], false },         // CABAL - Capturing 6 powerplants will shut down the firestorm generator
        SpeechRemapStruct { Speech[VOX_19_N100], false },         // GDI - Sir, I have done the best I can, but the damage is too extensive, looks like the kodiak is a loss
        SpeechRemapStruct { Speech[VOX_19_N102], false },         // Nod - I have got the evac unit, lets get out of here
        SpeechRemapStruct { Speech[VOX_38_N100], false },         // GDI - Something crashed near here, lets put on the report
        SpeechRemapStruct { Speech[VOX_38_N102], false },         // GDI - Sir, we found something, I think it is the tacitus
        SpeechRemapStruct { Speech[VOX_38_N104], false },         // GDI - We'll fall back to the old airfield and wait out the storm
        SpeechRemapStruct { Speech[VOX_38_N106], false },         // Nod - We've recovered a piece of the CABAL's core
        SpeechRemapStruct { Speech[VOX_38_N108], false },         // Nod - I think we have been spotted
        SpeechRemapStruct { Speech[VOX_38_N110], false },         // Nod - GDI forces are nearby
        SpeechRemapStruct { Speech[VOX_38_N112], false },         // Nod - We have recovered piece of the core
        SpeechRemapStruct { Speech[VOX_38_N114], false },         // Nod - Third piece recovered, lets get out of here
        SpeechRemapStruct { Speech[VOX_38_N116], false },         // Nod - Sir, recon reports of heavy concentration of tiberium poisoning in the area
        SpeechRemapStruct { Speech[VOX_38_N118], false },         // Nod - We have to find that airfield or we are finished
        SpeechRemapStruct { Speech[VOX_38_N120], false },         // Nod - Message transmitted, montauk is en route
        SpeechRemapStruct { Speech[VOX_38_N122], false },         // Nod - This is the montauk, we are standing by for your evac
        SpeechRemapStruct { Speech[VOX_38_N124], false },         // Nod - We need to create a distraction to clear the GDI base of air support
        SpeechRemapStruct { Speech[VOX_38_N126], false },         // Nod - I bet if we attacked the civilians, GDI will come to the rescue
        SpeechRemapStruct { Speech[VOX_38_N128], false },         // Nod - GDI planes are away from the base, now is our chance to get our engineer into the radar, but we better hurry
        SpeechRemapStruct { Speech[VOX_38_N130], false },         // Nod - Sir, enemy forces just routed, the core is undefended
        SpeechRemapStruct { Speech[VOX_38_N132], false },         // Nod - Sir, the core isn't here, we have been tricked
        SpeechRemapStruct { Speech[VOX_38_N134], false },         // Nod - Looks like this was just a remotely operated substation
        SpeechRemapStruct { Speech[VOX_38_N136], false },         // Nod - Sir, reinforcements can be brought in, if we repair the bridge in this area
        SpeechRemapStruct { Speech[VOX_38_N138], false },         // GDI - Sir, GDI has requested that we help the civilians first
        SpeechRemapStruct { Speech[VOX_38_N140], false },         // GDI - Once they are safe, we can concentrate on CABAL's harvesters
        SpeechRemapStruct { Speech[VOX_38_N142], false },         // GDI - Sir, GDI is sending us additional funding to aiding the civilians
        SpeechRemapStruct { Speech[VOX_38_N144], false },         // GDI - Forget the civilians, they are dead, concentrate on those harvesters
        SpeechRemapStruct { Speech[VOX_38_N146], false },         // GDI - This should be easy enough
        SpeechRemapStruct { Speech[VOX_38_N148], false },         // GDI - Lets get those harvesters
        SpeechRemapStruct { Speech[VOX_38_N150], false },         // GDI - GDI is uploading the final pieces of the code now, standby
        SpeechRemapStruct { Speech[VOX_38_N152], false },         // GDI - Code received, firestorm wall coming down in 3 2 1
        SpeechRemapStruct { Speech[VOX_38_N154], false },         // GDI - Firestorm is offline
        SpeechRemapStruct { Speech[VOX_38_N156], false },         // GDI - Code fragment retrieved
        SpeechRemapStruct { Speech[VOX_52_N000], false },         // GDI - Welcome stranger, surely a higher power has guided your footsteps to this land
        SpeechRemapStruct { Speech[VOX_52_N002], false },         // GDI - Can I offer you a cool beverage
        SpeechRemapStruct { Speech[VOX_52_N004], false },         // GDI - What do you think you are doing
        SpeechRemapStruct { Speech[VOX_54_N000], false },         // GDI - No tacitus piece in here
        SpeechRemapStruct { Speech[VOX_54_N002], false },         // GDI - Nothing in this we need, you sure we are in the right place
        SpeechRemapStruct { Speech[VOX_54_N004], false },         // GDI - Got it, lets get out of here
        SpeechRemapStruct { Speech[VOX_54_N006], false },         // GDI - Nothing in there
        SpeechRemapStruct { Speech[VOX_54_N008], false },         // GDI - The hieroglyphics on the temple read
        SpeechRemapStruct { Speech[VOX_54_N010], false },         // GDI - Temple of time
        SpeechRemapStruct { Speech[VOX_54_N012], false },         // GDI - Temple of thunder
        SpeechRemapStruct { Speech[VOX_54_N014], false },         // GDI - Temple of tacitus
        SpeechRemapStruct { Speech[VOX_54_N016], false },         // GDI - Command, this is Valdez, I have got the tacitus
        SpeechRemapStruct { Speech[VOX_54_N018], false },         // GDI - The blue tiberium is highly explosive, may be
        SpeechRemapStruct { Speech[VOX_54_N020], false },         // GDI - May be we can blast a path through there
        SpeechRemapStruct { Speech[VOX_55_N000], false },         // GDI - Help
        SpeechRemapStruct { Speech[VOX_55_N002], false },         // GDI - Save us
        SpeechRemapStruct { Speech[VOX_55_N004], false },         // GDI - It is coming right at us
        SpeechRemapStruct { Speech[VOX_55_N006], false },         // GDI - Get us out, please
        SpeechRemapStruct { Speech[VOX_55_N008], false },         // GDI - Its after us
        SpeechRemapStruct { Speech[VOX_55_N010], false },         // GDI - Look out
        SpeechRemapStruct { Speech[VOX_55_N012], false },         // GDI - What is GDI doing, nothing
        SpeechRemapStruct { Speech[VOX_55_N014], false },         // GDI - Why are we in this God forsaken place
        SpeechRemapStruct { Speech[VOX_55_N016], false },         // GDI - Are we gonna let those shiners take our food and water?
        SpeechRemapStruct { Speech[VOX_55_N018], false },         // GDI - Can't we all just get along
        SpeechRemapStruct { Speech[VOX_55_N020], false },         // GDI - Riot troops, run
        SpeechRemapStruct { Speech[VOX_55_N022], false },         // GDI - Thanks for the warning, here is a reward for your help
        SpeechRemapStruct { Speech[VOX_55_N024], false },         // GDI - Here commander, please take these 2 disruptors to help in your battle
        SpeechRemapStruct { Speech[VOX_55_N026], false },         // GDI - Yes, yes, I will see to it that everyone is evacuated, please take this MCV and may luck be with you
        SpeechRemapStruct { Speech[VOX_55_N028], false },         // Nod - I will never turn, uh
        SpeechRemapStruct { Speech[VOX_55_N030], false },         // Nod - Peace through power
        SpeechRemapStruct { Speech[VOX_55_N032], false },         // Nod - For the technology of peace
        SpeechRemapStruct { Speech[VOX_55_N034], false },         // Nod - We got one
        SpeechRemapStruct { Speech[VOX_55_N036], false },         // Nod - Fish is on the hook
        SpeechRemapStruct { Speech[VOX_55_N038], false },         // Nod - Here creature creature
        SpeechRemapStruct { Speech[VOX_55_N040], false },         // GDI - Mayday, mayday, we are currently under siege, can anyone help us?
        SpeechRemapStruct { Speech[VOX_55_N042], false },         // GDI - Thanks for the help
        SpeechRemapStruct { Speech[VOX_55_N044], false },         // Nod - We can shut that fencing down for you, just get us into one of the fences power arrays
        SpeechRemapStruct { Speech[VOX_56_N000], false },         // GDI - Lets show GDI that we are not going to take this
        SpeechRemapStruct { Speech[VOX_56_N002], false },         // GDI - Kill are the sinners
        SpeechRemapStruct { Speech[VOX_56_N004], false },         // GDI - Down with GDI
        SpeechRemapStruct { Speech[VOX_56_N006], false },         // GDI - Women and children to the shelter, thanks for the warning, here take these units
        SpeechRemapStruct { Speech[VOX_56_N008], false },         // GDI - Understood, we will evacuate the town, take this for your troubles
        SpeechRemapStruct { Speech[VOX_56_N010], false },         // GDI - Help us
        SpeechRemapStruct { Speech[VOX_56_N012], false },         // GDI - Save us
        SpeechRemapStruct { Speech[VOX_56_N014], false },         // GDI - They are coming
        SpeechRemapStruct { Speech[VOX_56_N016], false },         // GDI - Get us out
        SpeechRemapStruct { Speech[VOX_56_N018], false },         // GDI - Don't leave us
        SpeechRemapStruct { Speech[VOX_56_N020], false },         // GDI - Wait
        SpeechRemapStruct { Speech[VOX_57_N100], false },         // GDI - You picked the wrong place to surface, Nod scum
        SpeechRemapStruct { Speech[VOX_57_N102], false },         // GDI - GDI promised us peace and now Tratos is dead
        SpeechRemapStruct { Speech[VOX_57_N104], false },         // GDI - GDI lied to us
        SpeechRemapStruct { Speech[VOX_57_N106], false },         // GDI - Destroy the plants
        SpeechRemapStruct { Speech[VOX_57_N108], false },         // GDI - Show them this is our land
        SpeechRemapStruct { Speech[VOX_57_N110], false },         // GDI - Make them remember us
        SpeechRemapStruct { Speech[VOX_57_N112], false },         // GDI - They poisoned our food and water
        SpeechRemapStruct { Speech[VOX_57_N114], false },         // GDI - They are trying to kill us
        SpeechRemapStruct { Speech[VOX_57_N116], false },         // GDI - Understood Blunt, take this harvester for your troubles
        SpeechRemapStruct { Speech[VOX_57_N118], false },         // Nod - Release the hounds
        SpeechRemapStruct { Speech[VOX_57_N120], false },         // Nod - Halt and prepare for vehicle inspection
        SpeechRemapStruct { Speech[VOX_57_N122], false },         // Nod - OK, looks good, head on in
        SpeechRemapStruct { Speech[VOX_57_N124], false },         // Nod - Its bugged, destroy it now
        SpeechRemapStruct { Speech[VOX_58_N100], false },         // GDI - Your orders were clear commander, no civilian or mutant causalities
        SpeechRemapStruct { Speech[VOX_58_N102], false },         // GDI - The riot should end quickly but stay vigilant
        SpeechRemapStruct { Speech[VOX_58_N104], false },         // GDI - Copy that Valdez, transport is dusting off now, extraction in T minus 2 minutes at your initial drop off point
        SpeechRemapStruct { Speech[VOX_58_N106], false },         // GDI - We are providing you with the latest development from R&D, it is called juggernaut, it should greatly help your cause
        SpeechRemapStruct { Speech[VOX_58_N108], false },         // GDI - Get your people to the evac point
        SpeechRemapStruct { Speech[VOX_58_N110], false },         // GDI - Get your people to the transport
        SpeechRemapStruct { Speech[VOX_58_N112], false },         // GDI - Valdez, we recommend that you try and take out their leader
        SpeechRemapStruct { Speech[VOX_58_N114], false },         // GDI - Protect Dr. Boudreau with your life commander
        SpeechRemapStruct { Speech[VOX_58_N116], false },         // GDI - Our base is under attack, CABAL is moving faster than expected
        SpeechRemapStruct { Speech[VOX_58_N118], false },         // GDI - What in the world
        SpeechRemapStruct { Speech[VOX_58_N120], false },         // GDI - I should go warn the civilians in this area
        SpeechRemapStruct { Speech[VOX_58_N122], false },         // GDI - CABAL is one sick bastard
        SpeechRemapStruct { Speech[VOX_58_N124], false },         // GDI - O oh, CABAL is taking prisoners, this can't be good
        SpeechRemapStruct { Speech[VOX_58_N126], false },         // GDI - Arm yourselves, CABAL is conscripting humans into cyborg army
        SpeechRemapStruct { Speech[VOX_58_N128], false },         // GDI - Be warned, CABAL intends to turn you all into cyborgs
        SpeechRemapStruct { Speech[VOX_58_N130], false },         // GDI - Attention mutants, CABAL is currently harvesting biological components for his cyborgs, arm yourselves
        SpeechRemapStruct { Speech[VOX_58_N132], false },         // GDI - You must evacuate the city immediately, CABAL is coming
        SpeechRemapStruct { Speech[VOX_58_N134], false },         // GDI - Civilians to arms, defend your town
        SpeechRemapStruct { Speech[VOX_58_N136], false },         // GDI - This must be the base
        SpeechRemapStruct { Speech[VOX_58_N138], false },         // GDI - What the
        SpeechRemapStruct { Speech[VOX_58_N140], false },         // GDI - Those poor bastards, CABAL has started harvesting people for cyborgs, I must get back to warn citizens of Trondheim
        SpeechRemapStruct { Speech[VOX_58_N142], false },         // GDI - People of Trondheim, you must evacuate the city immediately, CABAL is actively capturing civilians to them into cyborgs
        SpeechRemapStruct { Speech[VOX_59_N100], false },         // GDI - Commander, we can ill afford to lose more civilians to CABAL
        SpeechRemapStruct { Speech[VOX_59_N102], false },         // GDI - Get in there and protect those civilians
        SpeechRemapStruct { Speech[VOX_59_N104], false },         // GDI - Well done commander, reinforcements en route
        SpeechRemapStruct { Speech[VOX_61_N000], false },         // GDI - Mutant abomination, dare you defile sacred ground
        SpeechRemapStruct { Speech[VOX_61_N002], false },         // GDI - Kill the mutant
        SpeechRemapStruct { Speech[VOX_61_N004], false },         // GDI - Stop, thief
        SpeechRemapStruct { Speech[VOX_61_N006], false },         // GDI - Kill the heretics
        SpeechRemapStruct { Speech[VOX_61_N008], false },         // GDI - Do not let them escape
        SpeechRemapStruct { Speech[VOX_61_N010], false },         // GDI - What, they killed the leader
        SpeechRemapStruct { Speech[VOX_61_N012], false },         // GDI - We join you in the here after
        SpeechRemapStruct { Speech[VOX_22_N100], false },         // GDI - Terminate all biological lifeforms
        SpeechRemapStruct { Speech[VOX_22_N102], false },         // GDI - The will of CABAL must be enforced
        SpeechRemapStruct { Speech[VOX_22_N104], false },         // BOTH - Err error
        SpeechRemapStruct { Speech[VOX_22_N106], false },         // BOTH - System failure
        SpeechRemapStruct { Speech[VOX_22_N108], false },         // BOTH - Mal malfunction
        SpeechRemapStruct { Speech[VOX_22_N110], false },         // GDI - Hit him
        SpeechRemapStruct { Speech[VOX_22_N112], false },         // GDI - Intruder alert
        SpeechRemapStruct { Speech[VOX_22_N114], false },         // GDI - You are not one of us
        SpeechRemapStruct { Speech[VOX_22_N116], false },         // GDI - Fire in the hole
        SpeechRemapStruct { Speech[VOX_22_N118], false },         // GDI - Proceeding to target
        SpeechRemapStruct { Speech[VOX_22_N120], false },         // GDI - Sequence engaged
        SpeechRemapStruct { Speech[VOX_71_N000], false },         // GDI - Open fire, open fire
        SpeechRemapStruct { Speech[VOX_71_N100], false },         // GDI - What, the civilians are shooting at us
        SpeechRemapStruct { Speech[VOX_71_N102], false },         // GDI - So are the shiners, retreat to the factory
        SpeechRemapStruct { Speech[VOX_71_N104], false },         // GDI - Command station is been captured, defenses offline
        SpeechRemapStruct { Speech[VOX_71_N106], false },         // GDI - CABAL has betrayed us, we need reinforcements now
        SpeechRemapStruct { Speech[VOX_71_N108], false },         // Nod - What was that
        SpeechRemapStruct { Speech[VOX_71_N110], false },         // Nod - Lets check that out
        SpeechRemapStruct { Speech[VOX_71_N112], false },         // Nod - Did you hear something
        SpeechRemapStruct { Speech[VOX_71_N114], false },         // Nod - Yeah, she ain't going anywhere
        SpeechRemapStruct { Speech[VOX_71_N116], false },         // Nod - I'll get back to base and get help
        SpeechRemapStruct { Speech[VOX_71_N118], false },         // Nod - Shoot it, naee
        SpeechRemapStruct { Speech[VOX_71_N120], false },         // Nod - Lets get these civilians out of here now
        SpeechRemapStruct { Speech[VOX_71_N122], false },         // Nod - We are under attack
        SpeechRemapStruct { Speech[VOX_71_N124], false },         // Nod - What the hell are those things
        SpeechRemapStruct { Speech[VOX_71_N126], false },         // Nod - Where are these things coming from
        SpeechRemapStruct { Speech[VOX_71_N128], false },         // GDI - Nod is uploading the final pieces of the code now, standby
        SpeechRemapStruct { Speech[VOX_71_N130], false },         // GDI - Code received, firestorm wall coming down in 3 2 1
        SpeechRemapStruct { Speech[VOX_71_N132], false },         // GDI - Firestorm is offline
        SpeechRemapStruct { Speech[VOX_71_N134], false },         // GDI - Code fragment retrieved
        SpeechRemapStruct { Speech[VOX_71_N136], false },         // Nod - Sir, these laser posts are stronger than normal
        SpeechRemapStruct { Speech[VOX_DROP_PODS_READY], true },  // EVA - Droppods available
        SpeechRemapStruct { Speech[VOX_62_N000], false },         // GDI - Welcome traveler, have you come to rejoice in the glory of our leader?
        SpeechRemapStruct { Speech[VOX_62_N002], false },         // GDI - Join us
        SpeechRemapStruct { Speech[VOX_62_N004], false },         // GDI - Join me
        SpeechRemapStruct { Speech[VOX_62_N006], false },         // GDI - Existence is futile
        SpeechRemapStruct { Speech[VOX_62_N008], false },         // GDI - I am coming to join you
        SpeechRemapStruct { Speech[VOX_62_N010], false },         // GDI - I am a messenger
        SpeechRemapStruct { Speech[VOX_62_N012], false }          // GDI - Come to me
    };

    return SpeechRemapArray[vox];
}
#endif


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioVoxClass::AudioVoxClass(Wstring name) :
    FileType(AUDIO_TYPE_AUD),
    FileName(),
    Available(false),
    Name(name),
    Sound(nullptr),
    Priority(DefaultPriority),
    Volume(DefaultVolume),
    MinVolume(DefaultMinVolume),
    MaxVolume(DefaultMaxVolume),
    Delay(DefaultDelay),
    FrequencyShift(DefaultFrequencyShift),
    Type(DefaultType),
    Control(DefaultControl)
{
    Name.To_Upper();

    Voxs.Add(this);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
AudioVoxClass::~AudioVoxClass()
{
    Voxs.Delete(this);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioVoxClass::Read_INI(CCINIClass &ini)
{
    char buffer[256];
    const char *name = Name.Peek_Buffer();

    if (!ini.Is_Present(name)) {
        return;
    }

    ini.Get_String(name, "Sound", Sound);
    Priority = ini.Get_Int(name, "Priority", DefaultPriority);
    DefaultDelay = ini.Get_Int(name, "Delay", DefaultDelay);
    Volume = ini.Get_Float_Clamp(name, "Volume", 0.0f, 1.0f, DefaultVolume);
    //Delay = ini.Get_Float_Clamp(name, "Delay", 0.0f, 5.0f, DefaultDelay); // Not to be read from the ini database.
    FrequencyShift = ini.Get_Float_Clamp(name, "FShift", -5.0f, 5.0f, FrequencyShift);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void AudioVoxClass::One_Time()
{
    for (VoxType vox = VOX_FIRST; vox < VOX_COUNT; ++vox) {
        AudioVoxClass *voxptr = new AudioVoxClass(Speech[vox]);
        ASSERT(voxptr != nullptr);
    }

#ifndef NDEBUG
    // To help find duplicate loading errors.
    ASSERT_FATAL_PRINT(Voxs.Count() == VOX_COUNT, "Voxs.Count() == %d, VOX_COUNT == %d.", Voxs.Count(), VOX_COUNT);
#endif
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool AudioVoxClass::Process(CCINIClass &ini)
{
    static char const * const DEFAULTS = "Defaults";
    static char const * const SOUNDLIST = "SpeechList";

    char buffer[32];

    //Clear();

    /**
     *  x
     */
    if (ini.Is_Present(DEFAULTS)) {
        DefaultPriority = ini.Get_Int(DEFAULTS, "Priority", DefaultPriority);
        DefaultDelay = ini.Get_Float(DEFAULTS, "Delay", DefaultDelay);
        DefaultVolume = ini.Get_Float_Clamp(DEFAULTS, "Volume", AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX, DefaultVolume);
        DefaultMinVolume = ini.Get_Float_Clamp(DEFAULTS, "MinVolume", AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX, DefaultMinVolume);
        //DefaultMaxVolume = ini.Get_Float_Clamp(DEFAULTS, "MaxVolume", AUDIO_VOLUME_MIN, AUDIO_VOLUME_MAX, DefaultMaxVolume); // Not to be loaded from the ini database.
    }

    if (ini.Is_Present(SOUNDLIST)) {

        int counter = ini.Entry_Count(SOUNDLIST);

        for (int index = 0; index < counter; ++index) {

            if (ini.Get_String(SOUNDLIST, ini.Get_Entry(SOUNDLIST, index), buffer, sizeof(buffer)-1)) {
                VoxType vox = From_Name(buffer);

                AudioVoxClass *voxptr = nullptr;
                if (vox == VOX_NONE) {
                    voxptr = new AudioVoxClass(buffer);
                    AUDIO_DEBUG_INFO("Vox::Process: Creating new Vox %s.\n", voxptr->Name.Peek_Buffer());
                    Voxs.Add(voxptr);

                } else {
                    voxptr = Voxs[vox];
                    AUDIO_DEBUG_INFO("Vox::Process: Found exiting Vox %s.\n", voxptr->Name.Peek_Buffer());
                }
                voxptr->Read_INI(ini);
            }

        }

    }

#ifndef NDEBUG
    // To help find duplicate loading errors.
    ASSERT_FATAL_PRINT(Voxs.Count() == VOX_COUNT, "Voxs.Count() == %d, VOX_COUNT == %d.", Voxs.Count(), VOX_COUNT);
#endif

    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioVoxClass::Scan()
{
    for (int index = 0; index < Voxs.Count(); ++index) {

        AudioVoxClass *voxptr = Voxs[index];

        /**
         *  x
         */
        Wstring name = voxptr->Name;
        if (voxptr->Sound.Is_Not_Empty()) {
            name = voxptr->Sound;
        }

        /**
         *  x
         */
        AudioManager.Audio_Set_Data(name, voxptr->Available, voxptr->FileType, voxptr->FileName);
    }

#if 0//#ifndef NDEBUG
    AUDIO_DEBUG_WARNING("Vox dump...\n");
    for (int index = 0; index < Voxs.Count(); ++index) {
        AUDIO_DEBUG_WARNING("  %03d  %s\n", index, ((AudioVoxClass *)Voxs[index])->Name.Peek_Buffer());
    }
#endif
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioVoxClass::Clear()
{
    while (Voxs.Count() > 0) {
        int index = Voxs.Count()-1;
        delete Voxs[index];
        Voxs.Delete(index);
    }

#ifndef NDEBUG
    // To help find duplicate loading errors.
    ASSERT_FATAL_PRINT(Voxs.Count() == 0, "Voxs.Count() == %d.", Voxs.Count());
#endif
}


/**
 *  EVA speaks to the player.  
 * 
 *  @author: CCHyper
 */
void AudioVoxClass::Speak(VoxType voice, bool force)
{
    //ASSERT(voice != VOX_NONE);    // Removed, triggers when some superweapons enable.
    ASSERT(voice < VOX_COUNT);

    if (!AudioManager.Is_Available() || Debug_Quiet) {
        return;
    }

    if (AudioManager.Get_Group_Volume(AUDIO_GROUP_SPEECH) <= 0.0f) {
        return;
    }

    if (voice == VOX_NONE || voice == SpeakQueue || voice == CurrentVoice) {
        return;
    }

    if (SpeakQueue != VOX_NONE) {
        return;
    }

    if (!IsSpeechAllowed) {
        AUDIO_DEBUG_WARNING("Vox::Speak - Speech is disabled!\n");
        return;
    }

    AudioVoxClass *voxptr = Voxs[voice];
    if (!voxptr) {
        AUDIO_DEBUG_ERROR("Vox::Speak - voxptr is null!\n");
        return;
    }

    SpeakQueue = voice;

    if (force) {
        SpeakTimer = 0;
        AI();
        return;
    }

    if (SpeakTimer.Expired()) {
        AI();
        return;
    }

    SpeakTimer = 60;
}


/**
 *  Handles starting the EVA voices.
 * 
 *  @author: CCHyper
 */
void AudioVoxClass::AI()
{
    if (!AudioManager.Is_Available() || Debug_Quiet) {
        return;
    }

    if (AudioManager.Get_Group_Volume(AUDIO_GROUP_SPEECH) <= 0.0f) {
        return;
    }

    if (!SpeakTimer.Expired()) {
        return;
    }

    if (CurrentVoice != VOX_NONE && (CurrentSpeechHandle && !CurrentSpeechHandle->Is_Playing())) {
        CurrentVoice = VOX_NONE;
        return;
    }

    if (SpeakQueue != VOX_NONE && (CurrentSpeechHandle && CurrentSpeechHandle->Is_Playing())) {
        return;
    }

    if (SpeakQueue == VOX_NONE) {
        return;
    }

    CurrentVoice = VOX_NONE;

    AudioVoxClass *voxptr = Voxs[SpeakQueue];
    if (!voxptr) {
        AUDIO_DEBUG_ERROR("Vox::AI - voxptr is null!\n");
        return;
    }

    Wstring name = voxptr->Name;
    Wstring filename = voxptr->FileName;
    
    // Removed, fails to play NOD speech.
#if 1
    /// Urgh, this is a mega bug...
    /**
     *  Only allow EVA speeches to be played by the speech handler. This has been
     *  expanded compared to the original code so up to 99 sides are supported, based
     *  on the currently set SpeechSide of the mission.
     */
    if (name[3] == 'I') {

        bool is_valid = false;
        int match_index = -1;
        char buffer[4];

        Wstring _name = name;
        Wstring _filename = filename;

        SideType side = Scen->SpeechSide;
        if (side == SIDE_NONE) {
            side = PlayerPtr->Class->Side;
        }

        AUDIO_DEBUG_INFO("Vox::Speak - Remapping %s to side %d (%s).\n", _name.Peek_Buffer(), side, SideClass::Name_From(side));

        std::snprintf(buffer, sizeof(buffer), "%02d-", side);
        Wstring _buff = buffer;

        _name.Trim_Range(0, 3);
        _filename.Trim_Range(0, 3);

        _name = _buff + _name;
        _filename = _buff + _filename;

        if (CCFileClass(_filename.Peek_Buffer()).Is_Available()) {

            name = _name;
            filename = _filename;

        } else {

            AUDIO_DEBUG_WARNING("Vox::Speak - Vox \"%s\" does not exist, restoring original name \"%s\"!\n", _name.Peek_Buffer(), name.Peek_Buffer());

        }

#if 0
        for (int index = 0; index < 99; ++index) {
            std::snprintf(buffer, sizeof(buffer), "%02d-", index);
            if (voxptr->Name.Contains(buffer)) {
                match_index = index;
                is_valid = true;
                break;
            }
        }

        if (!is_valid || match_index == -1) {
            AUDIO_DEBUG_WARNING("Vox::Speak - Vox \"%s\" is not a valid speech file!\n", _name.Peek_Buffer());
            return;
        }

        if (match_index != side) {
            AUDIO_DEBUG_WARNING("Vox::Speak - Vox \"%s\" does not match the player side!\n", _name.Peek_Buffer());
        }
#endif

    }

#endif

    /**
     *  Speech file was found, now play it.
     */
    VoxType vox = SpeakQueue;
    float vol = std::clamp(voxptr->Volume, voxptr->MinVolume, voxptr->MaxVolume);
    float pitch = voxptr->FrequencyShift;
    float delay = voxptr->Delay;
    AudioPriorityType priority = AudioManager.Priority_To_AudioPriority(voxptr->Priority);
    AudioSoundType type = voxptr->Type;
    AudioControlType control = voxptr->Control;

    AUDIO_DEBUG_INFO("Vox::AI - About to call AudioManager.Play with \"%s\".\n", filename.Peek_Buffer());
    AudioHandleClass *handle = AudioManager.Play(filename,
                                                    AUDIO_GROUP_SPEECH,
                                                    vol,
                                                    pitch,
                                                    0.0f,
                                                    priority,
                                                    0.0f,
                                                    delay);

    if (!handle) {
        AUDIO_DEBUG_ERROR("Vox::AI - Failed to play \"%s\"!\n", name.Peek_Buffer());
        SpeakQueue = VOX_NONE;
        return;
    }

    if (CurrentSpeechHandle && CurrentSpeechHandle->Is_Playing()) {
        CurrentSpeechHandle->Stop(0.5f, true);
    }

    CurrentSpeechHandle = handle;
    CurrentVoice = SpeakQueue;

    SpeakQueue = VOX_NONE;
}


/**
 *  Forces the EVA voice to stop talking.
 * 
 *  @author: CCHyper
 */
void AudioVoxClass::Stop_Speaking()
{
    SpeakQueue = VOX_NONE;

    if (CurrentSpeechHandle) {
        CurrentSpeechHandle->Stop(0.5f, true); // sounds better when it fades out.
    }
}


/**
 *  Checks to see if the eva voice is still playing.
 * 
 *  @author: CCHyper
 */
bool AudioVoxClass::Is_Speaking()
{
    Speak_AI();

    if (!AudioManager.Is_Available() || Debug_Quiet) {
        return false;
    }

    if (AudioManager.Get_Group_Volume(AUDIO_GROUP_SPEECH) <= 0.0f) {
        return false;
    }

    return SpeakQueue != VOX_NONE && (CurrentSpeechHandle && CurrentSpeechHandle->Is_Playing());
}


/**
 *  Sets the global speech volume to that specified.
 * 
 *  @author: CCHyper
 */
void AudioVoxClass::Set_Speech_Volume(int vol)
{
    float volf = std::clamp(float(vol/255.0f), 0.0f, 1.0f);
    AudioManager.Set_Group_Volume(AUDIO_GROUP_SPEECH, volf);
}


#ifndef NDEBUG
/**
 *  Writes out the default speech file.
 *
 *  @author: CCHyper
 */
bool AudioVoxClass::Write_Default_Speech_INI(CCINIClass &ini)
{
    static char const * const DEFAULTS = "Defaults";
    static char const * const SPEECHLIST = "SpeechList";

    char buffer[256];

    /**
     *  Clear out all existing base data from the ini file.
     */
    ini.Clear(DEFAULTS);
    ini.Clear(SPEECHLIST);

    ini.Clear();

    /**
     *  Save the default sound values.
     */
    //ini.Put_Int(DEFAULTS, "Priority", DefaultPriority);
    ini.Put_Float(DEFAULTS, "Delay", DefaultDelay);
    ini.Put_Float(DEFAULTS, "Volume", DefaultVolume);
    ini.Put_Float(DEFAULTS, "MinVolume", DefaultMinVolume);
    ini.Put_Float(DEFAULTS, "MaxVolume", DefaultMaxVolume);

    /**
     *  x
     */
    for (VoxType vox = VOX_FIRST; vox < ARRAY_SIZE(Speech); ++vox) {

        const char * vox_name = Speech[vox];

        /**
         *  x
         */
        char entrybuff[8];
        std::snprintf(entrybuff, sizeof(entrybuff), "%d", vox);

        ini.Put_String(SPEECHLIST, entrybuff, vox_name);

        /**
         *  Now write the keys for its section.
         */
        //ini.Put_Int(vox_name, "Priority", DefaultPriority);

    }

    return true;
}
#endif


/**
 *  x
 *
 *  @author: CCHyper
 */
VoxType AudioVoxClass::From_Name(const char *name)
{
    ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return VOX_NONE;
    }

    if (name != nullptr) {
        for (VoxType index = VOX_FIRST; index < Voxs.Count(); ++index) {
            AudioVoxClass *vocptr = Voxs[index];
            if (vocptr->Name == name) {
                return index;
            }
        }
    }

    return VOX_NONE;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
const char *AudioVoxClass::Name_From(VoxType type)
{
    return (type != VOX_NONE && type < Voxs.Count() ? (Voxs[type])->Name.Peek_Buffer() : "<none>");
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
void AudioVoxClass::Set_Speech_Allowed(bool set)
{
    IsSpeechAllowed = set;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool AudioVoxClass::Is_Speech_Allowed()
{
    return IsSpeechAllowed;
}

#endif
