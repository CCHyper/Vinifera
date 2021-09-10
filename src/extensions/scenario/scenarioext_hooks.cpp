/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SCENARIOEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended ScenarioClass.
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
#include "scenarioext_hooks.h"
#include "scenarioext_init.h"
#include "scenarioext_functions.h"
#include "scenarioext.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "house.h"
#include "unit.h"
#include "unittype.h"
#include "unittypeext.h"
#include "rules.h"
#include "rulesext.h"
#include "multiscore.h"
#include "scenario.h"
#include "session.h"
#include "rules.h"
#include "ccfile.h"
#include "ccini.h"
#include "addon.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Wrapper function for creating a UnitClass instance.
 * 
 *  @author: CCHyper
 */
static UnitClass *New_UnitClass(UnitTypeClass *classof, HouseClass *house)
{
    return new UnitClass(classof, house);
}


/**
 *  #issue-177
 * 
 *  Various patches to add support for more than one entry defined on BaseUnit.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Create_Units_Calculate_Total_Cost_Of_Units_Patch)
{
    GET_REGISTER_STATIC(UnitTypeClass *, current_unittype, esi);
    GET_REGISTER_STATIC(int, tot_cost, ebp);
    GET_REGISTER_STATIC(int, tot_count, ebx);
    static UnitTypeClass *unittype;
    static int i;

    unittype = nullptr;

    /**
     *  Fetch the extended rules class instance and make sure value
     *  entries have been loaded.
     */
    if (RulesExtension && RulesExtension->BaseUnit.Count() > 0) {

        /**
         *  Iterate over all defined BaseUnits, if any match the unit
         *  we are currently processing then skip it.
         */
        for (i = 0; i < RulesExtension->BaseUnit.Count(); ++i) {
            if (RulesExtension->BaseUnit[i] == current_unittype) {
                goto skip_unit;
            }
        }

    /**
     *  Fallback to the original code.
     */
    } else if (Rule->BaseUnit == current_unittype) {
        goto skip_unit;
    }

    DEBUG_INFO("Calc: %s\n", current_unittype->Name());

    /**
     *  Add the unit to the available units vector.
     */
add_cost:
    _asm { mov esi, current_unittype }
    _asm { mov ebp, tot_cost }
    _asm { mov ebx, tot_count }
    JMP_REG(edx, 0x005DE663);

    /**
     *  Don't add the unit to the vector.
     */
skip_unit:
    JMP(0x005DE670);
}

DECLARE_PATCH(_Create_Units_Populate_Available_Units_Vector_Patch)
{
    GET_REGISTER_STATIC(UnitTypeClass *, current_unittype, esi);
    static UnitTypeClass *unittype;
    static int i;

    unittype = nullptr;

    /**
     *  Fetch the extended rules class instance and make sure value
     *  entries have been loaded.
     */
    if (RulesExtension && RulesExtension->BaseUnit.Count() > 0) {

        /**
         *  Iterate over all defined BaseUnits, if any match the unit
         *  we are currently processing then skip it.
         */
        for (i = 0; i < RulesExtension->BaseUnit.Count(); ++i) {
            if (RulesExtension->BaseUnit[i] == current_unittype) {
                goto dont_add_to_vector;
            }
        }

    /**
     *  Fallback to the original code.
     */
    } else if (Rule->BaseUnit == current_unittype) {
        goto dont_add_to_vector;
    }

    DEBUG_INFO("Pop: %s\n", current_unittype->Name());

    /**
     *  Add the unit to the available units vector.
     */
add_to_vector:
    _asm { mov esi, current_unittype }
    JMP(0x005DE9DC);

    /**
     *  Don't add the unit to the vector.
     */
dont_add_to_vector:
    JMP(0x005DEA22);
}

DECLARE_PATCH(_Create_Units_Place_BaseUnit_Patch)
{
    GET_REGISTER_STATIC(HouseClass *, hptr, edi);
    static UnitClass *unit;
    static UnitTypeClass *unittype;

    unit = nullptr;

    /**
     *  Fetch the extended rules class instance and make sure value
     *  entries have been loaded.
     */
    if (RulesExtension && RulesExtension->BaseUnit.Count() > 0) {

        /**
         *  Fetch the first buildable base unit from the new base unit entry
         *  and get the current count of that unit that this can house own.
         */
        unittype = hptr->Get_First_Ownable(RulesExtension->BaseUnit);

    /**
     *  Fallback to the original code.
     */
    } else {
        unittype = Rule->BaseUnit;
    }

    if (unittype) {
        unit = New_UnitClass(unittype, hptr);
    } else {
        DEBUG_WARNING("Invalid or no available BaseUnit found in Create_Units()!\n");
        goto failed_to_create;
    }

    /**
     *  Unlimbo the base unit.
     */
unlimbo_baseunit:
    _asm { mov esi, unit }
    JMP(0x005DEC33);

    /**
     *  Failed to create the unit instance.
     */
failed_to_create:
    JMP(0x005DECC2);
}


/**
 *  Process additions to the Rules data from the input file.
 * 
 *  @author: CCHyper
 */
static bool Rule_Addition(const char *fname, bool with_digest = false)
{
    CCFileClass file(fname);
    if (!file.Is_Available()) {
        return false;
    }

    CCINIClass ini;
    if (!ini.Load(file, with_digest)) {
        return false;
    }

    DEBUG_INFO("Calling Rule->Addition() with \"%s\" overrides.\n", fname);

    Rule->Addition(ini);

    return true;
}


/**
 *  #issue-#671
 * 
 *  Add loading of MPLAYER.INI to override Rules data for multiplayer games.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Read_Scenario_INI_MPlayer_INI_Patch)
{
    if (Session.Type != GAME_NORMAL && Session.Type != GAME_WDT) {

        /**
         *  Process the multiplayer ini overrides.
         */
        Rule_Addition("MPLAYER.INI");
        if (Addon_Enabled(ADDON_FIRESTORM)) { 
            Rule_Addition("MPLAYERFS.INI");
        }

    }

    /**
     *  Update the progress screen bars.
     */
    Session.Loading_Callback(42);

    /**
     *  Stolen bytes/code.
     */
    Call_Back();

    JMP(0x005DD8DA);
}


/**
 *  #issue-522
 * 
 *  These patches make the multiplayer score screen to honour the value of
 *  "IsSkipScore" from ScenarioClass.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Do_Win_Skip_MPlayer_Score_Screen_Patch)
{
    /**
     *  Stolen bytes/code.
     */
    ++Session.GamesPlayed;

    if (!Scen->IsSkipScore) {
        MultiScore::Presentation();
    }

    JMP(0x005DC9DF);
}

DECLARE_PATCH(_Do_Lose_Skip_MPlayer_Score_Screen_Patch)
{
    /**
     *  Stolen bytes/code.
     */
    ++Session.GamesPlayed;

    if (!Scen->IsSkipScore) {
        MultiScore::Presentation();
    }

    JMP(0x005DCD9D);
}


/**
 *  Main function for patching the hooks.
 */
void ScenarioClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    ScenarioClassExtension_Init();

    /**
     *  For compatibility with the TS Client we need to remove
     *  these two reimplementations as they conflict with the spawner.
     */
#if !defined(TS_CLIENT)
    /**
     *  Hooks in the new Assign_Houses() function.
     * 
     *  @author: CCHyper
     */
    Patch_Call(0x005E08E3, &Vinifera_Assign_Houses);

    /**
     *  #issue-338
     * 
     *  Hooks in the new Create_Units() function.
     * 
     *  @author: CCHyper
     */
    Patch_Call(0x005DD320, &Vinifera_Create_Units);
#endif

    Patch_Jump(0x005DC9D4, &_Do_Win_Skip_MPlayer_Score_Screen_Patch);
    Patch_Jump(0x005DCD92, &_Do_Lose_Skip_MPlayer_Score_Screen_Patch);
    Patch_Jump(0x005DD8D5, &_Read_Scenario_INI_MPlayer_INI_Patch);

    Patch_Jump(0x005DE637, &_Create_Units_Calculate_Total_Cost_Of_Units_Patch);
    Patch_Jump(0x005DE9AF, &_Create_Units_Populate_Available_Units_Vector_Patch);
    Patch_Jump(0x005DEC07, &_Create_Units_Place_BaseUnit_Patch);
}
