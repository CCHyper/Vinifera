/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TECHNOEXT_FUNCTIONS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the supporting functions for the extended TechnoClass.
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
#include "technoext_functions.h"
#include "technoext.h"
#include "techno.h"
#include "tibsun_inline.h"
#include "rules.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  #issue-
 * 
 *  
 * 
 *  @author: CCHyper
 */
bool Techno_Is_Iron_Curtain_Active(TechnoClass *this_ptr) // mAbstractClass_IsIronCurtained
{
	if (this_ptr) {
		return false;
	}

    TechnoClassExtension *technoext;

    /**
     *  Fetch the class extension if it exists.
     */
    technoext = TechnoClassExtensions.find(this_ptr);
    if (!technoext) {
        return false;
    }

    return technoext->IronCurtainCountDown.Is_Active();
}


/**
 *  #issue-
 * 
 *  
 * 
 *  @author: CCHyper
 */
void Techno_Set_Iron_Curtain_Timer(TechnoClass *this_ptr, int value)
{
	if (this_ptr) {
		return;
	}

    TechnoClassExtension *technoext;

    /**
     *  Fetch the class extension if it exists.
     */
    technoext = TechnoClassExtensions.find(this_ptr);
    if (!technoext) {
        return;
    }

    technoext->IronCurtainCountDown = value;
}


// YR 0070E380


/**
 *  #issue-
 * 
 *  
 * 
 *  @author: CCHyper
 */
// YR 0070E5A0
void Techno_Iron_Curtain_AI(TechnoClass *this_ptr)
{
	if (this_ptr) {
		return;
	}

    TechnoClassExtension *technoext;

    /**
     *  Fetch the class extension if it exists.
     */
    technoext = TechnoClassExtensions.find(this_ptr);
    if (!technoext) {
        return;
    }

    if (Techno_Is_Iron_Curtain_Active(this_ptr)) {

        switch (technoext->IronCurtainTintStage) {
            
            case 0:
                technoext->IronCurtainTintStage = 1;
                technoext->IronCurtainCountDown = 6;
                break;

            case 1:
                if (technoext->IronCurtainCountDown.Expired()) {
                    technoext->IronCurtainTintStage = 2;
                    technoext->IronCurtainCountDown = 4;
                }
                break;

            case 2:
                if (technoext->IronCurtainCountDown.Expired()) {
                    technoext->IronCurtainTintStage = 3;
                    technoext->IronCurtainCountDown = Random_Pick(-5, 5) + 20;
                }
                break;

            case 3:
                if (technoext->IronCurtainCountDown.Expired()) {
                    technoext->IronCurtainTintStage = 4;
                    technoext->IronCurtainCountDown = 8;
                }
                break;

            case 4:
                if (technoext->IronCurtainCountDown.Expired()) {
                    technoext->IronCurtainTintStage = 5;
                    technoext->IronCurtainCountDown = 16;
                }
                break;

            case 5:
                if (technoext->IronCurtainCountDown.Expired()) {
                    if (technoext->IronCurtainCountDown >= 54) {
                        technoext->IronCurtainTintStage = 4;
                        technoext->IronCurtainCountDown = 8;
                    } else {
                        technoext->IronCurtainTintStage = 6;
                    }
                }
                break;

            case 6:
                if (technoext->IronCurtainCountDown <= 30) {
                    technoext->IronCurtainTintStage = 7;
                    technoext->IronCurtainCountDown = 6;
                }
                break;

            case 7: // TODO
                technoext->IronCurtainTintStage = 8;
                break;

            case 8: // TODO
                technoext->IronCurtainTintStage = 9;
                break;

            case 9:
                if (technoext->IronCurtainCountDown.Expired()) {
                    technoext->IronCurtainTintStage = 10;
                }
                break;

            default:
                technoext->IronCurtainTintStage = 10;
                break;

        };

    } else {
    
        technoext->IronCurtainTintStage = 0;
    }
}





#if 0
RA2; -------------------------------------------------------------

IronCurtain					- ActionType

//IronCurtainColor			- RulesClass::AudioVisual
//IronCurtainDuration			- RulesClass::CombatDamage
//IronCurtainInvokeAnim		- RulesClass::General

EVA_IronCurtainDetected		- BuildingClass::Grand_Opening
EVA_IronCurtainReady		- SuperClass::AI
							- SuperClass::Forced_Charge
							- SuperClass::Place
							
EVA_IronCurtainActivated	- SuperClass::Place

//IronCurtainCountDown		- TechnoClass

__IronTintTimer				- TechnoClass	- StageClass???
__IronTintStage				- TechnoClass


SuperClass::Place			- Firing code.


TechnoClass::Take_Damage	- Calls new virtual to do flash


--- __IronTintStage;
//.text:0070E2D2	TechnoClass::Set_Iron_Tint_Timer	mov     [ecx+1A4h], eax
//.text:0070E5B7	TechnoClass::Iron_Timer_AI	mov     eax, [esi+1A4h] (many more)
//.text:0070E3A3	TechnoClass_irontinttimer	mov     ecx, [ecx+1A4h]
//.text:006F2C67	??0TechnoClass@@QAE@PAVHouseClass@@@Z	mov     dword ptr [esi+1A4h], 0Ah
//.text:00512257	?Compute_CRC@HouseTypeClass@@UBEXAAVCRCEngine@@@Z	mov     dl, [esi+1A4h]


--- IronCurtainCountDown;
//.text:0041BF40	TechnoClass_IronCurtainCountDown_41BF40	mov     edx, [ecx+18Ch]
//.text:006F2C4F	??0TechnoClass@@QAE@PAVHouseClass@@@Z	mov     [esi+18Ch], ecx
//.text:0070E2C3	TechnoClass::Set_Iron_Tint_Timer	mov     [ecx+18Ch], eax
//.text:0070E2BD	TechnoClass::Set_Iron_Tint_Timer	lea     esi, [ecx+18Ch]; Load Effective Address
//.text:0070E789	TechnoClass::Iron_Timer_AI	lea     ecx, [esi+18Ch]; this
//.text:0070E7CF	TechnoClass::Iron_Timer_AI	lea     ecx, [esi+18Ch]; jumptable 0070E5C6 case 6


--- __IronTintTimer;
//.text:0070E387	TechnoClass_irontinttimer	mov     esi, [ecx+198h]
//.text:0070E75C	TechnoClass::Iron_Timer_AI	mov     edx, [esi+198h]; jumptable 0070E5C6 case 5
//.text:0070E708	TechnoClass::Iron_Timer_AI	mov     edi, [esi+198h]; jumptable 0070E5C6 case 4
//.text:006F2C5B	??0TechnoClass@@QAE@PAVHouseClass@@@Z	mov     [esi+198h], edx
//.text:0070E768	TechnoClass::Iron_Timer_AI	lea     edi, [esi+198h]; Load Effective Address


--- IronCurtainColor;
.text:0073BFBF	UnitClass::Draw_Voxel	mov     eax, [esi+18A8h]

--- IronCurtainDuration;
.text:006CD016	?Place@SuperClass@@QAEXPAUCellStruct@@_N@Z	mov     eax, [ecx+0FE8h]
//.text:0066C64B	?CombatDamage@RulesClass@@AAE_NAAVCCINIClass@@@Z	mov     [esi+RulesClass.IronCurtainDuration], eax

--- IronCurtainInvokeAnim;
.text:006CCF00	?Place@SuperClass@@QAEXPAUCellStruct@@_N@Z	mov     edx, [ecx+348h]





RA1; -------------------------------------------------------------


IronCurtain					- RulesClass::Recharge

IronCurtain (duration)		- RulesClass::General

TechnoClass					- IronCurtainCountDown

CellClass::Goodie_Check		- checks IronCurtainCountDown
InfantryClass::Per_Cell_Process		- checks IronCurtainCountDown
TeamClass::TMission_Invulnerable		- checks IronCurtainCountDown
TechnoClass::Take_Damage		- checks IronCurtainCountDown, nullifys damage
TechnoClass::Techno_Draw_Object		- checks IronCurtainCountDown, adds ember/red effect
UnitClass::Mission_Unload		- checks IronCurtainCountDown

VOX_IRON_CHARGING			- IRONCHG1
VOX_IRON_READY				- IRONRDY1

HouseClass::Super_Weapon_Handler	- 	Sidebar code?
HouseClass::Place_Special_Blast		-   Firing code.
#endif
