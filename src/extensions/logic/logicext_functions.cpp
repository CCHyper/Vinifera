/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          BUILDINGEXT_FUNCTIONS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the supporting functions for the extended BuildingClass.
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
#include "logicext_functions.h"
#include "logic.h"
#include "voc.h"
#include "rules.h"
#include "rulesext.h"
#include "tibsun_inline.h"
#include "tibsun_globals.h"
#include "vinifera_globals.h"
#include "iomap.h"
#include "debughandler.h"
#include "asserthandler.h"


void Logic_Time_Quake_AI(LogicClass *this_ptr)
{
    if (!this_ptr) {
        return;
    }

    /*
    ** If there's a time quake, handle it here.
    */
    if (TimeQuake) {

        Sound_Effect(VOC_KABOOM15);

        if (RulesExtension) {

            /**
             *  If this warhead has screen shake values defined, then set the blitter
             *  offset values. GScreenClass::Blit will handle the rest for us.
             */
            if (RulesExtension->MTankShakePixelXLo > 0 || RulesExtension->MTankShakePixelXHi > 0) {
                Map.ScreenX = Sim_Random_Pick(RulesExtension->MTankShakePixelXLo, RulesExtension->MTankShakePixelXHi);
            }
            if (RulesExtension->MTankShakePixelYLo > 0 || RulesExtension->MTankShakePixelYHi > 0) {
                Map.ScreenY = Sim_Random_Pick(RulesExtension->MTankShakePixelYLo, RulesExtension->MTankShakePixelYHi);
            }

        } else {
            Map.ScreenX = 0;
            Map.ScreenY = 8;
        }
    }

    /*
    **	AI for all sentient objects is processed.
    */
    for (index = 0; index < Count(); index++) {
        ObjectClass * obj = (*this)[index];
        int count = Count();

        BStart(BENCH_AI);
        obj->AI();
        BEnd(BENCH_AI);

        if (TimeQuake && obj != NULL && obj->IsActive && !obj->IsInLimbo && obj->Strength) {
            int damage = (int)obj->Class_Of().MaxStrength * Rule.QuakeDamagePercent;
#ifdef FIXIT_CSII	//	checked - ajw 9/28/98
            if (TimeQuakeCenter) {
                if(::Distance(obj->As_Target(),TimeQuakeCenter)/256 < MTankDistance) {
                    switch(obj->What_Am_I()) {
                        case RTTI_INFANTRY:
                            damage = QuakeInfantryDamage;
                            break;
                        case RTTI_BUILDING:
                            damage = QuakeBuildingDamage * (int)obj->Class_Of().MaxStrength;
                            break;
                        default:
                            damage = QuakeUnitDamage * (int)obj->Class_Of().MaxStrength;
                            break;
                    }
                    if (damage) {
                        obj->Clicked_As_Target(HOUSE_COUNT); // 2019/09/20 JAS - Added record of who clicked on the object, HOUSE_COUNT is used to mark for all houses
                        new AnimClass(ANIM_MINE_EXP1, obj->Center_Coord());
                    }
                    obj->Take_Damage(damage, 0, WARHEAD_AP, 0, true);
                }
            } else {
                obj->Take_Damage(damage, 0, WARHEAD_AP, 0, true);
            }
#else
            obj->Take_Damage(damage, 0, WARHEAD_AP, 0, true);
#endif
        }
        /*
        **	If the object was destroyed in the process of performing its AI, then
        **	adjust the index so that no object gets skipped.
        */
        int count_diff = Count() - count;
        if (count_diff < 0) {
            index += count_diff;
        }
    }
}