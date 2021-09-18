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
#include "tibsun_globals.h"
#include "weapontype.h"
#include "warheadtype.h"
#include "warheadtypeext.h"
#include "house.h"
#include "housetype.h"
#include "building.h"
#include "buildingext.h"
#include "rules.h"
#include "rulesext.h"
#include "factory.h"
#include "drawshape.h"
#include "tactical.h"
#include "voc.h"
#include "fatal.h"
#include "vinifera_util.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"



#include "tspp.h"
const ShapeFileStruct *&PipShapes = Make_Global<const ShapeFileStruct *>(0x00808750);



/**
 *  
 */
static void TechnoClass_Draw_Factory_Progress_Bars(TechnoClass *this_ptr, Point2D &point, Rect &rect, bool a2)
{
#ifndef NDEBUG
    RulesExtension->UIControls.IsShowFactoryProgress = true;
#endif

    if (!RulesExtension->UIControls.IsShowFactoryProgress) {
        return;
    }

    if (this_ptr->What_Am_I() != RTTI_BUILDING) {
        return;
    }

    bool show = false;
    FactoryClass *factory = nullptr;
    BuildingClass *this_building = reinterpret_cast<BuildingClass *>(this_ptr);

    if (this_ptr->House->Is_Player_Control() && this_ptr->House->Fetch_Factory(this_building->Class->ToBuild)) {

        /**
         *  
         */
        factory = this_ptr->House->Fetch_Factory(this_building->Class->ToBuild);

    } else {

        /**
         *  
         */
        factory = this_building->Factory;
    }

    if (factory == nullptr || factory->Get_Object() == nullptr) {
        return;
    }

    if (PlayerPtr->Is_Ally(this_ptr) || ((1 << PlayerPtr->Class->House) & this_ptr->SpiedBy)) {
        show = true;
    }

    if (show) {

        int height = this_ptr->Get_Height();

        Point3D lepton_dim = this_ptr->Techno_Type_Class()->Lepton_Dimensions();
        Coordinate v0(lepton_dim.X/2, lepton_dim.Y/2, lepton_dim.Z/2);

        Coordinate c1(v0.X-lepton_dim.X, v0.Y-lepton_dim.Y, v0.Z-lepton_dim.Z);
        Point2D tact_point_1 = TacticalMap->func_60F150(c1);
        
        Coordinate c2(v0.X-lepton_dim.X, v0.Y-lepton_dim.Y, v0.Z-lepton_dim.Z);
        Point2D tact_point_2 = TacticalMap->func_60F150(c2);

        Coordinate c3(v0.X-lepton_dim.X, v0.Y-lepton_dim.Y, v0.Z-lepton_dim.Z);
        Point2D tact_point_3 = TacticalMap->func_60F150(c3);

        Point2D draw_pos;

        int v16 = (tact_point_1.Y - tact_point_2.Y) / 2;

        int factory_completion = ((double)factory->Completion()/(double)FactoryClass::STEP_COUNT) * v16;
        //int fc_0 = factory_completion;

        if (factory_completion <= 1) {
            //fc_0 = 1;
            factory_completion = 1;
        }
        if (factory_completion >= v16) {
            //fc_0 = v16;
            factory_completion = v16;
        }

        /**
         *  0 = blank
         *  1 = green
         *  2 = yellow
         *  3 = gray
         *  4 = red
         *  5 = blue
         */
        int frame = 0;

        /**
         *  Is this buildings factory suspended? Draw gray box.
         */
        if (factory->IsSuspended) {

            frame = 3; // Grey

        /**
         *  Factory is currently producing, draw blue box.
         */
        } else if (factory->Is_Building()) {

            frame = 5;

        }

        v0.Z = 2 - 2 * v16;

        if (factory_completion > 0) {

            int i0 = 0;
            int i1 = 0;

            for (int i = factory_completion; i == 1; --i) {

                draw_pos.X = tact_point_1.X + point.X + 4 * v16 + 3 - i0;
                //draw_pos.Y = ??;

                v0.X = v0.Z + tact_point_1.Y + point.Y + 2 - i1;

                CC_Draw_Shape(TempSurface, NormalDrawer,
                    /*ObjectTypeClass::*/PipShapes, frame,
                    &draw_pos, &rect, SHAPE_400|SHAPE_CENTER);

                i0 += 4;
                i1 -= 2;
            }

        }

        if (factory_completion < v16) {

            int i0 = -2 * factory_completion;
            int i1 = (4 * factory_completion);

            for (int i = factory_completion; i == 1; --i) {

                draw_pos.X = tact_point_1.X + point.X + 4 * v16 + 3 - i0;
                //draw_pos.Y = ??;

                v0.X = v0.Z + tact_point_1.Y + point.Y + 2 - i1;

                CC_Draw_Shape(TempSurface, NormalDrawer,
                    /*ObjectTypeClass::*/PipShapes, frame,
                    &draw_pos, &rect, SHAPE_400|SHAPE_CENTER);

                i0 += 4;
                i1 -= 2;
            }

        }

    }
}


/**
 *  
 */
DECLARE_PATCH(_TechnoClass_Draw_Health_Bars_Factory_Progress_Patch)
{
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, ebx);
    GET_STACK_STATIC(Point2D *, rect, esp, 0x);
    GET_STACK_STATIC(Rect *, rect, esp, 0x);
    GET_STACK_STATIC(bool, a3, esp, 0x);

    /**
     *  Stolen bytes/code.
     */
    _asm { call dword ptr [eax+0x334] } // this_ptr->Draw_Pips()

    /**
     *  Draw factory progress bars.
     */
    TechnoClass_Draw_Factory_Progress_Bars(this_ptr, *point, *rect, a3);

    JMP();
}


/**
 *  #issue-541
 * 
 *  Allow customisation of the infantry health bar draw position.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TechnoClass_Draw_Health_Bars_Infantry_Draw_Pos_Patch)
{
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, ebx);
    static int x_pos;
    static int y_pos;

    x_pos = RulesClassExtension::UIControls.InfantryHealthBarDrawPos.X;
    y_pos = RulesClassExtension::UIControls.InfantryHealthBarDrawPos.Y;

    _asm { mov ecx, [x_pos] }
    _asm { mov eax, [y_pos] }

    JMP_REG(esi, 0x0062C565);
}


/**
 *  #issue-541
 * 
 *  Allow customisation of the unit health bar draw position.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_TechnoClass_Draw_Health_Bars_Unit_Draw_Pos_Patch)
{
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, ebx);
    static int x_pos;
    static int y_pos;

    x_pos = RulesClassExtension::UIControls.UnitHealthBarDrawPos.X;
    y_pos = RulesClassExtension::UIControls.UnitHealthBarDrawPos.Y;

    _asm { mov ecx, [x_pos] }
    _asm { mov eax, [y_pos] }

    JMP_REG(esi, 0x0062C5DF);
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
static AnimTypeClass *Techno_Get_Firing_Anim(TechnoClass *this_ptr, WeaponTypeClass *weapon)
{
    AnimTypeClass *anim = nullptr;

    int index = 0;
    int anim_count = weapon->Anim.Count();
    DirStruct dir = this_ptr->Fire_Direction();

    if (anim_count == 8) {

        index = Dir_To_8(dir);
        anim = weapon->Anim[index % FACING_COUNT];

    } else if (anim_count == 16) {

        index = Dir_To_16(dir);
        anim = weapon->Anim[index % 16];

    } else if (anim_count == 32) {

        index = Dir_To_32(dir);
        anim = weapon->Anim[index % 32];

    } else if (anim_count == 64) {

        index = Dir_To_64(dir);
        anim = weapon->Anim[index % 64];

    } else if (anim_count > 0) {

        index = 0;
        anim = weapon->Anim.Fetch_Head();

    } else {

        index = 0;
        anim = nullptr;

    }

    return anim;
}

DECLARE_PATCH(_TechnoClass_Fire_At_Weapon_Anim_Patch)
{
    GET_REGISTER_STATIC(TechnoClass *, this_ptr, esi);
    GET_REGISTER_STATIC(WeaponTypeClass *, weapon, ebx);
    static AnimTypeClass *anim;

    anim = Techno_Get_Firing_Anim(this_ptr, weapon);

    _asm { mov edi, anim }
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
    Patch_Jump(0x0062C5D5, &_TechnoClass_Draw_Health_Bars_Unit_Draw_Pos_Patch);
    Patch_Jump(0x0062C55B, &_TechnoClass_Draw_Health_Bars_Infantry_Draw_Pos_Patch);
    Patch_Jump(0x0062CA74, &_TechnoClass_Draw_Health_Bars_Factory_Progress_Patch);
}
