/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TECHNOEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended TechnoClass.
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
#include "technoext_hooks.h"
#include "technoext_init.h"
#include "technoext.h"
#include "techno.h"
#include "technotype.h"
#include "technotypeext.h"
#include "tibsun_inline.h"
#include "weapontype.h"
#include "weapontypeext.h"
#include "warheadtype.h"
#include "warheadtypeext.h"
#include "house.h"
#include "housetype.h"
#include "rules.h"
#include "voc.h"
#include "laserdraw.h"
#include "laserdrawext.h"
#include "particlesys.h"
#include "fatal.h"
#include "vinifera_util.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"




// https://github.com/Phobos-developers/Phobos/blob/ccd8954cf23720a79faeb5c6099b911967dd29b7/src/Misc/Hooks.LaserDraw.cpp


/**
 *  Creates a new laser draw object.
 *  
 *  @author: CCHyper
 */
static void New_LaserDrawClass_Object(TechnoClass *this_ptr, Coordinate &start_coord, Coordinate &end_coord, int z_adjust, WeaponTypeClass *weapon)
{
    if (!this_ptr || !weapon) {
        return;
    }

    LaserDrawClassExtension *laserdrawext;
    WeaponTypeClassExtension *weapontypeext;

    LaserDrawClass *laserdraw = nullptr;

    bool lightning_bolt = false;

    bool blinks = false;
    bool fades = true;
    float start_intensity = 0.0f;
    float end_intensity = 1.0f;
    bool a9 = true;             // TODO, what is this?



    //
    // TODO, investigate why the lasers look different (darker?) with this patch.
    //



    /**
     *  Fetch the laser overrides from the weapon.
     */
    weapontypeext = WeaponTypeClassExtensions.find(weapon);
    if (weapontypeext) {
        lightning_bolt = weapontypeext->IsLightningBolt;
        blinks = weapontypeext->IsLaserBlinks;
        fades = weapontypeext->IsLaserFades;
        start_intensity = weapontypeext->LaserStartIntensity;
        end_intensity = weapontypeext->LaserFinishIntensity;
    }


    /**
     *  Create the lightning bolt laser object.
     */
    if (lightning_bolt) {

        for (int i = 0; i < 200; ++i) {


            ? += Random_Pick(-128, 128);
            ? += Random_Pick(-128, 128);
            ? = Random_Pick(0, 3 * ?) + ?;


            laserdraw = new LaserDrawClass(
                start_coord,
                end_coord,
                z_adjust,
                a9,
                weapon->LaserInnerColor,
                weapon->LaserOuterColor,
                weapon->LaserOuterSpread,
                weapon->LaserDuration,
                blinks,
                fades,
                start_intensity,
                end_intensity);


            laserdrawext = LaserDrawClassExtensions.find(laserdraw);

            //weapontypeext = WeaponTypeClassExtensions.find(weapon);
            if (weapontypeext) {

                /**
                 *  Fetch extended laser overrides from the weapon.
                 */
                laserdrawext = LaserDrawClassExtensions.find(laserdraw);
                if (laserdrawext) {
                    laserdrawext->Thickness = weapontypeext->LaserThickness;
                }
            }

            /**
             *  Store extended laserdraw properties.
             */
            if (laserdrawext) {
                laserdrawext->Source = this_ptr;
            }



        }

        /**
         *  Spawn a spark particle at the destination of the lightning bolt.
         */
        ParticleSystemClass *particlesys = new ParticleSystemClass(Rule->DefaultSparkSystem, end_coord);
        ASSERT(particlesys != nullptr);


    /**
     *  Create the standard laser object.
     */
    } else {
    
        laserdraw = new LaserDrawClass(
            start_coord,
            end_coord,
            z_adjust,
            a9,
            weapon->LaserInnerColor,
            weapon->LaserOuterColor,
            weapon->LaserOuterSpread,
            weapon->LaserDuration,
            blinks,
            fades,
            start_intensity,
            end_intensity);

        ASSERT(laserdraw != nullptr);

        laserdrawext = LaserDrawClassExtensions.find(laserdraw);

        //weapontypeext = WeaponTypeClassExtensions.find(weapon);
        if (weapontypeext) {

            /**
             *  Fetch extended laser overrides from the weapon.
             */
            laserdrawext = LaserDrawClassExtensions.find(laserdraw);
            if (laserdrawext) {
                laserdrawext->Thickness = weapontypeext->LaserThickness;
            }
        }

        /**
         *  Store extended laserdraw properties.
         */
        if (laserdrawext) {
            laserdrawext->Source = this_ptr;
        }
    
    }
}


/**
 *  #issue-
 * 
 *  Patch to intercept and replace the creation of a LaserDraw object when
 *  a unit fires a laser weapon. This allows us to fully customise the creation
 *  and properties of the object.
 *  
 *  @author: CCHyper
 */
DECLARE_PATCH(_TechnoClass_Laser_Zap_Customise_Patch)
{
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, edi);
    GET_REGISTER_STATIC(WeaponTypeClass *, weapon, esi);
    LEA_STACK_STATIC(Coordinate *, from_coord, esp, 0x24);
    LEA_STACK_STATIC(Coordinate *, to_coord, esp, 0x30);
    GET_REGISTER_STATIC(int, z_adjust, ebx);

    /**
     *  Create a new laser object.
     */
    New_LaserDrawClass_Object(this_ptr, *from_coord, *to_coord, z_adjust, weapon);

    JMP(0x006301C7);
}






/**
 *  #issue-411
 * 
 *  Implements IsAffectsAllies for WarheadTypes.
 * 
 *  @note: This patch does not replace "stolen" code as per our implementation
 *         rules, this is because the call to ObjectClass::Take_Damage that follows
 *         is too much of a risk to not have correctly implemented.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TechnoClass_Take_Damage_IsAffectsAllies_Patch)
{
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, esi);
    GET_STACK_STATIC(int *, damage, esp, 0xEC);
    GET_STACK_STATIC(int, distance, esp, 0xF0);
    GET_STACK_STATIC(const WarheadTypeClass *, warhead, esp, 0xF4);
    GET_STACK_STATIC(TechnoClass *, source, esp, 0xF8);
    GET_STACK_STATIC8(bool, forced, esp, 0xFC);
    GET_STACK_STATIC(int, a6, esp, 0x100);
    static WarheadTypeClassExtension *warheadtypeext;
    static ResultType result;

    if (warhead) {

        /**
         *  Is the warhead that hit us one that affects units allied with its firing owner?
         */
        warheadtypeext = WarheadTypeClassExtensions.find(warhead);
        if (warheadtypeext && !warheadtypeext->IsAffectsAllies) {

            /**
             *  If the source of the damage is an ally of ours, then reset
             *  the damage amount and return that we took no damage.
             */
            if (source && source->House->Is_Ally(this_ptr->House)) {
                *damage = 0;
                goto return_RESULT_NONE;
            }

        }

    }

    /**
     *  Stolen bytes/code.
     */
    _asm { mov ecx, a6 }

    /**
     *   Restore a few registers to be safe.
     */
    _asm { mov ebx, source }
    //_asm { mov edi, damage }
    JMP_REG(edx, 0x006328E5);

    /**
     *  Function returns RESULT_NONE.
     */
return_RESULT_NONE:
    JMP_REG(edi, 0x00632882);
}


/**
 *  #issue-404
 * 
 *  A object with "CloakStop" set has no effect on the cloaking behavior.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TechnoClass_Is_Ready_To_Uncloak_Cloak_Stop_BugFix_Patch)
{
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, esi);
    GET_REGISTER_STATIC(bool, cloaked_by_house, al);

    /**
     *  Is this object unable to recloak or is it disabled by an EMP?
     */
    //if (!this_ptr->Is_Allowed_To_Recloak() && !this_ptr->IsCloakable || this_ptr->entry_2A4()) { // Original code.
    if (!this_ptr->Is_Allowed_To_Recloak() || !this_ptr->IsCloakable || this_ptr->entry_2A4()) {
        goto continue_check;
    }

    /**
     *  Object is not allowed to un-cloak at this time.
     */
return_false:
    JMP_REG(ecx, 0x0062F746);

    /**
     *  Continue checks.
     */
continue_check:
    _asm { mov bl, cloaked_by_house }
    JMP_REG(ecx, 0x0062F6DD);
}


/**
 *  #issue-391
 * 
 *  Extends the firing animation effect to support more facings.
 * 
 *  @author: CCHyper
 */
static DirStruct &Techno_Fire_Direction(TechnoClass *this_ptr)
{
    return this_ptr->Fire_Direction();
}

DECLARE_PATCH(_TechnoClass_Fire_At_Weapon_Anim_Patch)
{
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, esi);
    GET_REGISTER_STATIC(WeaponTypeClass *, weapon, ebx);
    GET_REGISTER_STATIC(AnimTypeClass *, anim, edi);
    static DirStruct *dir;
    static int anim_count;
    static int index;

    anim_count = weapon->Anim.Count();
    dir = &Techno_Fire_Direction(this_ptr);

    if (anim_count == 8) {

        index = Dir_To_8(*dir);
        anim = weapon->Anim[index % FACING_COUNT];

    } else if (anim_count == 16) {

        index = Dir_To_16(*dir);
        anim = weapon->Anim[index % 16];

    } else if (anim_count == 32) {

        index = Dir_To_32(*dir);
        anim = weapon->Anim[index % 32];

    } else if (anim_count == 64) {

        index = Dir_To_64(*dir);
        anim = weapon->Anim[index % 64];

    } else {

        index = 0;
        anim = nullptr;

    }

    _asm { mov edx, anim }
    JMP(0x006310A6);
}


/**
 *  #issue-356
 * 
 *  Custom cloaking sound for TechnoTypes.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TechnoClass_Do_Cloak_Cloak_Sound_Patch)
{
    GET_REGISTER_STATIC(Coordinate *, coord, eax);
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, esi);
    static TechnoTypeClass *technotype;
    static TechnoTypeClassExtension *technotypeext;
    static VocType voc;

    technotype = this_ptr->Techno_Type_Class();

    /**
     *  Fetch the default cloaking sound.
     */
    voc = Rule->CloakSound;

    /**
     *  Fetch the class extension if it exists.
     */
    technotypeext = TechnoTypeClassExtensions.find(technotype);
    if (technotypeext) {

        /**
         *  Does this object have a custom cloaking sound? If so, use it.
         */
        if (technotypeext->CloakSound != VOC_NONE) {
            voc = technotypeext->CloakSound;
        }
    }

    /**
     *  Play the sound effect at the objects location.
     */
    Sound_Effect(voc, *coord);

    JMP(0x00633C8B);
}


/**
 *  #issue-356
 * 
 *  Custom uncloaking sound for TechnoTypes.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TechnoClass_Do_Uncloak_Uncloak_Sound_Patch)
{
    GET_REGISTER_STATIC(Coordinate *, coord, eax);
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, esi);
    static TechnoTypeClass *technotype;
    static TechnoTypeClassExtension *technotypeext;
    static VocType voc;

    technotype = this_ptr->Techno_Type_Class();

    /**
     *  Fetch the default cloaking sound.
     */
    voc = Rule->CloakSound;

    /**
     *  Fetch the class extension if it exists.
     */
    technotypeext = TechnoTypeClassExtensions.find(technotype);
    if (technotypeext) {

        /**
         *  Does this object have a custom decloaking sound? If so, use it.
         */
        if (technotypeext->UncloakSound != VOC_NONE) {
            voc = technotypeext->UncloakSound;
        }
    }

    /**
     *  Play the sound effect at the objects location.
     */
    Sound_Effect(voc, *coord);

    JMP(0x00633BE7);
}


/**
 *  A patch that adds debug logging on null house pointers in TechnoClass::Owner().
 * 
 *  This is a common crash observed by mod developers and map creators, and
 *  aims to assist tracking down the offending object.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TechnoClass_Null_House_Warning_Patch)
{
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, ecx);
    static HouseClass *house;
    static int id;
    
    house = this_ptr->House;
    if (!house) {
        DEBUG_WARNING("Techno \"%s\" has an invalid house!", this_ptr->Name());
        Vinifera_DeveloperMode_Warning_WWMessageBox("Techno \"%s\" has an invalid house!", this_ptr->Name());
        Fatal("Null house pointer in TechnoClass::Owner!\n");

    } else {
        id = house->ID;
    }
    
    _asm { mov eax, id }
    _asm { ret }
}


/**
 *  Main function for patching the hooks.
 */
void TechnoClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    TechnoClassExtension_Init();

    Patch_Jump(0x00633C78, &_TechnoClass_Do_Cloak_Cloak_Sound_Patch);
    Patch_Jump(0x00633BD4, &_TechnoClass_Do_Uncloak_Uncloak_Sound_Patch);
    Patch_Jump(0x0063105C, &_TechnoClass_Fire_At_Weapon_Anim_Patch);
    Patch_Jump(0x0062F6B7, &_TechnoClass_Is_Ready_To_Uncloak_Cloak_Stop_BugFix_Patch);
    Patch_Jump(0x0062E6F0, &_TechnoClass_Null_House_Warning_Patch);
    Patch_Jump(0x006328DE, &_TechnoClass_Take_Damage_IsAffectsAllies_Patch);
    Patch_Jump(0x0063012B, &_TechnoClass_Laser_Zap_Customise_Patch);
}
