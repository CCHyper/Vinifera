/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          JUMPJETLOCOMOTIONEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended JumpjetLocomotionClass.
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
#include "houseext_hooks.h"
#include "jumpjetlocomotion.h"
#include "foot.h"
#include "technotype.h"
#include "technotypeext.h"
#include "tibsun_globals.h"
#include "rules.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"



//int JumpjetTurnRate; // 0x1FC [jj loco 004F95C7]
//int JumpjetSpeed; // 0x200 [jj loco 004FA4EC, 004FA553, 004F9EDC, 004FA1BA, 004FA1FD, 004FA218]
//double JumpjetClimb; // 0x208 [jj loco 004FA76A, 004FA790]
//int JumpjetCruiseHeight; // 0x210 [jj loco 004F9765, 004F9ABE, 004F9DC1, 004F9E34, 004FA1D6, 004FA226]
//double JumpjetAcceleration; // 0x218 [jj loco 004FA4DA, 004FA51D]
//double JumpjetWobblesPerSecond; // 0x220 [jj loco 004FA5D0]
//int JumpjetWobbleDeviation; // 0x228 [jj loco 004FA5F7]
//int JumpjetCloakDetectionRadius; // 0xF6C [jj loco 004FA918]





// JumpJet on TechnoTypes
// 0041D82F - astar
// 0045F0F6 - explosion
// 004A321F - FootClass
// 004A8930 - FootClass
// 0062DF4A - TechnoClass
// 0054CE75 jj loco - YR                for jumpjetturn?

// new patches based on RA2 code.
// 0043D0CF BuildingClass - YR
// 0043D363 BuildingClass - YR
// 00444CB7 BuildingClass - YR
// 00467C52 BulletClass - YR
// 00487A5E CellClass - YR
// 004DE30E FootClass - YR
// Added Assign_Destination check
// 0051C9F9 InfantryClass - YR
// 006FC4A8 TechnoClass - YR
// 006FE5C3 TechnoClass - YR


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or deconstructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
class JumpjetLocomotionClassFake final : public JumpjetLocomotionClass
{
    public:
        HRESULT __stdcall _Link_To_Object(void *object);
};


/**
 *  Implementation of Link_To_Object() for JumpjetLocomotionClass.
 */
HRESULT JumpjetLocomotionClassFake::_Link_To_Object(void *object)
{
    TechnoTypeClassExtension *technotypeext;
    TechnoTypeClass *technotype;
    FootClass *footobject;

    /**
     *  Fetch the unit linked to this locomotor.
     */
    footobject = reinterpret_cast<FootClass *>(object);
    technotype = footobject->Techno_Type_Class();

    /**
     *  Fetch the global value as the default.
     */
    int turnrate = Rule->JumpjetTurnRate;

    /**
     *  Fetch the value from extended techno type class instance if it exists.
     */
    technotypeext = TechnoTypeClassExtensions.find(technotype);
    if (technotypeext) {
        turnrate = technotypeext->JumpjetTurnRate;
    }

    Facing.Set_ROT(turnrate);

    LocomotionClass::Link_To_Object(object);

    return S_OK;
}

//int JumpjetTurnRate; // 0x1FC [jj loco 004F95C7]
//DECLARE_PATCH()
//{
//}

//int JumpjetSpeed; // 0x200 [jj loco 004FA4EC, 004FA553, 004F9EDC, 004FA1BA, 004FA1FD, 004FA218]
DECLARE_PATCH(_)
{
}

//double JumpjetClimb; // 0x208 [jj loco 004FA76A, 004FA790]
DECLARE_PATCH(_JumpjetLocomotionClass_Process_Movement_AI_1_Patch)
{
}

DECLARE_PATCH(_JumpjetLocomotionClass_Process_Movement_AI_2_Patch)
{
}

//int JumpjetCruiseHeight; // 0x210 [jj loco 004F9765, 004F9ABE, 004F9DC1, 004F9E34, 004FA1D6, 004FA226]
DECLARE_PATCH(_JumpjetLocomotionClass_Process_JumpjetCruiseHeight_Patch)
{
}

DECLARE_PATCH(_JumpjetLocomotionClass_Move_To_JumpjetCruiseHeight_Patch)
{
}

DECLARE_PATCH(_JumpjetLocomotionClass_Move_To_In_Which_Layer_Patch)
{
}

//double JumpjetAcceleration; // 0x218 [jj loco 004FA4DA, 004FA51D]
DECLARE_PATCH(_JumpjetLocomotionClass_Movement_AI_JumpjetAcceleration_1_Patch)
{
}

DECLARE_PATCH(_JumpjetLocomotionClass_Movement_AI_JumpjetAcceleration_2_Patch)
{
}

DECLARE_PATCH(_JumpjetLocomotionClass_Movement_AI_JumpjetWobblesPerSecond_Patch)
{
    GET_REGISTER_STATIC(JumpjetLocomotionClass *, this_ptr, esi);
    static TechnoTypeClassExtension *technotypeext;
    static TechnoTypeClass *technotype;
    static FootClass *linkedto;
    static double wobblespersec;

    /**
     *  Fetch the unit linked to this locomotor.
     */
    linkedto = this_ptr->Linked_To();
    technotype = linkedto->Techno_Type_Class();

    /**
     *  Fetch the global value as the default.
     */
    wobblespersec = Rule->JumpjetWobblesPerSecond;

    /**
     *  Fetch the value from extended techno type class instance if it exists.
     */
    technotypeext = TechnoTypeClassExtensions.find(technotype);
    if (technotypeext) {
        wobblespersec = technotypeext->JumpjetWobblesPerSecond;
    }

    /**
     *  
     */
    _asm { fld qword ptr ss : 0x006D36B8 } // 15.0
    _asm { fdiv qword ptr ss : wobblespersec }
    _asm { fdivr qword ptr ss : 0x006CC190 } // 6.283185307179586
    _asm { fadd qword ptr [esi+0x58] } // this->field_58
    _asm { fstp qword ptr [esi+0x58] }

    JMP_REG(ecx, 0x004FA5E2);
}

DECLARE_PATCH(_JumpjetLocomotionClass_Movement_AI_JumpjetWobbleDeviation_Patch)
{
    GET_REGISTER_STATIC(JumpjetLocomotionClass *, this_ptr, esi);
    static TechnoTypeClassExtension *technotypeext;
    static TechnoTypeClass *technotype;
    static FootClass *linkedto;
    static int deviation;

    /**
     *  Stolen bytes/code.
     */
    _asm { add esp, 0x8 }

    /**
     *  Fetch the unit linked to this locomotor.
     */
    linkedto = this_ptr->Linked_To();
    technotype = linkedto->Techno_Type_Class();

    /**
     *  Fetch the global value as the default.
     */
    deviation = Rule->JumpjetWobbleDeviation;

    /**
     *  Fetch the value from extended techno type class instance if it exists.
     */
    technotypeext = TechnoTypeClassExtensions.find(technotype);
    if (technotypeext) {
        deviation = technotypeext->JumpjetWobbleDeviation;
    }

    /**
     *  Calculate the deviation on the coord.
     */
    _asm { fimul dword ptr ss : deviation }
    _asm { fiadd dword ptr [esi+0x50] } // this->field_50
    _asm { mov eax, 0x006B2330 } // _fotl

    JMP_REG(ecx, 0x004FA605);
}

DECLARE_PATCH(_JumpjetLocomotionClass_Movement_AI_JumpjetCloakDetectionRadius_Patch)
{
    GET_REGISTER_STATIC(JumpjetLocomotionClass *, this_ptr, esi);
    static TechnoTypeClassExtension *technotypeext;
    static TechnoTypeClass *technotype;
    static FootClass *linkedto;
    static int detect_radius;

    /**
     *  Fetch the unit linked to this locomotor.
     */
    linkedto = this_ptr->Linked_To();
    technotype = linkedto->Techno_Type_Class();

    /**
     *  Fetch the global value as the default.
     */
    detect_radius = Rule->JumpjetCloakDetectionRadius;

    /**
     *  Fetch the value from extended techno type class instance if it exists.
     */
    technotypeext = TechnoTypeClassExtensions.find(technotype);
    if (technotypeext) {
        detect_radius = technotypeext->JumpjetCloakDetectionRadius;
    }

    _asm { mov ecx, detect_radius }
    JMP(0x004FA91E);
}


/**
 *  Main function for patching the hooks.
 */
void JumpjetLocomotionClassExtension_Hooks()
{
    Change_Virtual_Address(0x006D35C8, Get_Func_Address(& JumpjetLocomotionClassFake::_Link_To_Object));
    //Patch_Jump(0x, &);
    //Patch_Jump(0x, &);
    //Patch_Jump(0x, &);
    //Patch_Jump(0x, &);
    //Patch_Jump(0x, &);
    //Patch_Jump(0x, &);
    //Patch_Jump(0x, &);
    //Patch_Jump(0x, &);
    //Patch_Jump(0x, &_JumpjetLocomotionClass_Movement_AI_JumpjetAcceleration_1_Patch);
    //Patch_Jump(0x, &_JumpjetLocomotionClass_Movement_AI_JumpjetAcceleration_2_Patch);
    //Patch_Jump(0x004FA5C5, &_JumpjetLocomotionClass_Movement_AI_JumpjetWobblesPerSecond_Patch);
    //Patch_Jump(0x004FA5EF, &_JumpjetLocomotionClass_Movement_AI_JumpjetWobbleDeviation_Patch);
    //Patch_Jump(0x004FA912, &_JumpjetLocomotionClass_Movement_AI_JumpjetCloakDetectionRadius_Patch);
}
