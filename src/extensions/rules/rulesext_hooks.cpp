/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          RULESEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended RulesClass.
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
#include "rulesext_hooks.h"
#include "rulesext_init.h"
#include "rulesext.h"
#include "rules.h"
#include "tiberium.h"
#include "weapontype.h"
#include "ccini.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or deconstructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
class RulesClassFake final : public RulesClass
{
    public:
        void _Process(CCINIClass &ini);

        bool Weapons(CCINIClass &ini);
};


/**
 *  Fetch all the weapon characteristic values.
 * 
 *  @author: CCHyper
 */
bool RulesClassFake::Weapons(CCINIClass &ini)
{
    static const char * const WEAPONS = "Weapons";

    char buf[128];
    const WeaponTypeClass *weapontype;

    int counter = ini.Entry_Count(WEAPONS);
    for (int index = 0; index < counter; ++index) {
        const char *entry = ini.Get_Entry(WEAPONS, index);

        /**
         *  Get a weapon entry.
         */
        if (ini.Get_String(WEAPONS, entry, buf, sizeof(buf))) {

            /**
             *  Find or create a weapon of the name specified.
             */
            weapontype = WeaponTypeClass::Find_Or_Make(buf);
            if (weapontype) {
                DEV_DEBUG_INFO("Rules: Found WeaponType \"%s\".\n", buf);
            } else {
                DEV_DEBUG_WARNING("Rules: Error processing WeaponType \"%s\"!\n", buf);
            }

        }

    }

    return counter > 0;
}


/**
 *  Fetch the bulk of the rule data from the control file.
 * 
 *  @author: CCHyper
 */
void RulesClassFake::_Process(CCINIClass &ini)
{
    Colors(ini);
    Houses(ini);
    Sides(ini);
    Overlays(ini);

    /**
     *  #issue-117
     * 
     *  Add reading of Weapons list from RULES.INI
     * 
     *  @author: CCHyper
     */
    Weapons(ini);

    SuperWeapons(ini);
    Warheads(ini);
    Smudges(ini);
    Terrains(ini);
    Buildings(ini);
    Vehicles(ini);
    Aircraft(ini);
    Infantry(ini);
    Animations(ini);
    VoxelAnims(ini);
    Particles(ini);
    ParticleSystems(ini);
    JumpjetControls(ini);
    MPlayer(ini);
    AI(ini);
    Powerups(ini);
    Land_Types(ini);
    IQ(ini);
    General(ini);
    Objects(ini);
    Difficulty(ini);
    CrateRules(ini);
    CombatDamage(ini);
    AudioVisual(ini);
    SpecialWeapons(ini);
    TiberiumClass::Process(ini);

    /**
     *  Read the extended RulesClass data.
     */
    if (RulesExtension) {
        RulesExtension->Process(ini);
    }
}


/**
 *  Main function for patching the hooks.
 */
void RulesClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    RulesClassExtension_Init();

    Patch_Jump(0x005C6710, &RulesClassFake::_Process);
}
