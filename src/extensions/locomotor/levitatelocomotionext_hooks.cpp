/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          LEVITATELOCOMOTIONEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended LevitateLocomotionClass.
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
#include "levitatelocomotionext_hooks.h"
#include "levitatelocomotion.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "rules.h"
#include "foot.h"
#include "techno.h"
#include "technotypeext.h"
#include "voc.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Not advised to do this, but this is for functions that have static
 *  variables that we need access to.
 */
#include "tspp.h"
int &_Levitate_Process_Index = Make_Global<int>(0x00804A5C);


#if 0
// Drag
#endif

#if 0
// MaxVelocityWhenHappy
#endif

#if 0
// MaxVelocityWhenFollowing
#endif

#if 0
// MaxVelocityWhenPissedOff
#endif

#if 0
// AccelerationProbability
#endif

#if 1
// AccelerationDuration
DECLARE_PATCH(_LevitateLocomotionClass_AccelerationDuration_Patch)
{
    GET_REGISTER_STATIC(LevitateLocomotionClass *, this_ptr, ecx);
    static TechnoTypeClassExtension *exttype_ptr;
    static const TechnoTypeClass *technotype;
    static FootClass *linked_to;
    static int acceleration_duration;

    linked_to = this_ptr->Linked_To();
    technotype = linked_to->Techno_Type_Class();

    /**
     *  Fetch the extension type data.
     */
    exttype_ptr = TechnoTypeClassExtensions.find(technotype);
    if (exttype_ptr) {

        /**
         *  Original code fallback.
         */
        acceleration_duration = exttype_ptr->LevitationAccelerationDuration;

    } else {

        /**
         *  Original code fallback.
         */
        acceleration_duration = Levitation_AccelerationDuration;

    }

    _asm { mov ecx, acceleration_duration }
    JMP_REG(edi, 0x004FFA54);
}
#endif

#if 0
// Acceleration
#endif

#if 0
// InitialBoost
#endif

#if 1
// MaxBlockCount
DECLARE_PATCH(_LevitateLocomotionClass_MaxBlockCount_1_Patch)
{
    GET_REGISTER_STATIC(LevitateLocomotionClass *, this_ptr, esi);
    static TechnoTypeClassExtension *exttype_ptr;
    static const TechnoTypeClass *technotype;
    static FootClass *linked_to;
    static int acceleration_duration;

    linked_to = this_ptr->Linked_To();
    technotype = linked_to->Techno_Type_Class();

    /**
     *  Fetch the extension type data.
     */
    exttype_ptr = TechnoTypeClassExtensions.find(technotype);
    if (exttype_ptr) {

        /**
         *  
         */
        this_ptr->field_44 = exttype_ptr->LevitationMaxBlockCount;

    } else {

        /**
         *  Original code fallback.
         */
        this_ptr->field_44 = Levitation_AccelerationDuration;

    }

    JMP_REG(ecx, 0x005003E3);
}

DECLARE_PATCH(_LevitateLocomotionClass_MaxBlockCount_2_Patch)
{
    GET_REGISTER_STATIC(LevitateLocomotionClass *, this_ptr, esi);
    static TechnoTypeClassExtension *exttype_ptr;
    static const TechnoTypeClass *technotype;
    static FootClass *linked_to;
    static int acceleration_duration;

    linked_to = this_ptr->Linked_To();
    technotype = linked_to->Techno_Type_Class();

    /**
     *  Fetch the extension type data.
     */
    exttype_ptr = TechnoTypeClassExtensions.find(technotype);
    if (exttype_ptr) {

        /**
         *  
         */
        this_ptr->field_44 = exttype_ptr->LevitationMaxBlockCount;

    } else {

        /**
         *  Original code fallback.
         */
        this_ptr->field_44 = Levitation_AccelerationDuration;

    }

    /**
     *  
     */
    _asm { mov ecx, [esi+8] }

    JMP_REG(ecx, 0x0050048B);
}
#endif

#if 0
// IntentionalDeacceleration
#endif

#if 0
// IntentionalDriftVelocity
#endif

#if 0
// ProximityDistance
#endif

#if 1
// PropulsionSoundEffect
DECLARE_PATCH(_LevitateLocomotionClass_PropulsionSoundEffect_Patch)
{
    GET_REGISTER_STATIC(LevitateLocomotionClass *, this_ptr, ecx);
    static TechnoTypeClassExtension *exttype_ptr;
    static const TechnoTypeClass *technotype;
    static FootClass *linked_to;
    static VocType voc;

    linked_to = this_ptr->Linked_To();
    technotype = linked_to->Techno_Type_Class();

    /**
     *  Fetch the extension type data.
     */
    exttype_ptr = TechnoTypeClassExtensions.find(technotype);
    if (exttype_ptr && exttype_ptr->LevitationPropulsionSoundEffect.Count() > 0 && (_Levitate_Process_Index++ & 3) == 0) {

        /**
         *  
         */
        voc = exttype_ptr->LevitationPropulsionSoundEffect[NonCriticalRandomNumber() % exttype_ptr->LevitationPropulsionSoundEffect.Count()];
        Sound_Effect(voc);

    } else if (Levitation_PropulsionSoundEffect.Count() > 0 && (_Levitate_Process_Index++ & 3) == 0) {

        /**
         *  Original code fallback.
         */
        voc = Levitation_PropulsionSoundEffect[NonCriticalRandomNumber() % Levitation_PropulsionSoundEffect.Count()];
        Sound_Effect(voc);

    }

continue_function:
    /**
     *  We need to set ESI as we patch over its prolog assignment.
     */
    _asm { mov esi, this_ptr }

    JMP_REG(edi, 0x004FFA54);
}
#endif


/**
 *  Main function for patching the hooks.
 */
void LevitateLocomotionClassExtension_Hooks()
{
#if 0
// Drag
// 004FE2F5
// 004FE3CA
// 004FE8CA
// 004FEAEF
// 004FEEDB
// 004FF227
// 004FF521
// 004FF6C8
// 004FFB7C
// 00500453
// 00500ABF
// 00500B3D
#endif

#if 0
// MaxVelocityWhenHappy
// 004FEB4F
#endif

#if 0
// MaxVelocityWhenFollowing
// 004FEA66
#endif

#if 0
// MaxVelocityWhenPissedOff
// 004FE9AE
#endif

#if 0
// AccelerationProbability
// 004FE62E
// 004FEB80
#endif

#if 1
// AccelerationDuration
Patch_Jump(0x004FFA4E, &_LevitateLocomotionClass_AccelerationDuration_Patch);
#endif

#if 0
// Acceleration
// 004FFA78
// 004FFA86
#endif

#if 0
// InitialBoost
// 004FFA95
// 004FFAA5
#endif

#if 1
// MaxBlockCount
Patch_Jump(0x005003DA, &_LevitateLocomotionClass_MaxBlockCount_1_Patch);
Patch_Jump(0x00500480, &_LevitateLocomotionClass_MaxBlockCount_2_Patch);
#endif

#if 0
// IntentionalDeacceleration
// 004FE23A
// 004FE2D4
// 004FE807
// 004FE8AA
// 004FEA49
// 004FFE1E
// 00500A4F
// 00500A9D
#endif

#if 0
// IntentionalDriftVelocity
// 004FF82D
// 004FF845
// 004FFCEA
// 004FFD06
// 004FFDD2
// 004FFDEE
#endif

#if 0
// ProximityDistance
// 004FE2AD
// 004FE7E0
// 004FE887
// 004FEA26
// 004FED49
// 004FEE6C
// 004FF095
// 004FF1B8
// 004FF3B6
// 004FFE8E
// 0050002E
#endif

#if 1
// PropulsionSoundEffect
// 004FFA0C
Patch_Jump(0x004FFA0C, &_LevitateLocomotionClass_PropulsionSoundEffect_Patch);
#endif
}
