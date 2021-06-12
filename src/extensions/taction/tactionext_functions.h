/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TACTIONEXT_FUNCTIONS.H
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the supporting functions for the extended TActionClass.
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
#pragma once

#include "always.h"
#include "tibsun_defines.h"


class TActionClass;
class HouseClass;
class ObjectClass;
class TriggerClass;


bool TAction_Give_Credits(TActionClass *taction, HouseClass *house, ObjectClass *object, TriggerClass *trigger, Cell *cell);
bool TAction_Give_Credits_To_House(TActionClass *taction, HouseClass *house, ObjectClass *object, TriggerClass *trigger, Cell *cell);
bool TAction_Enable_Short_Game(TActionClass *taction, HouseClass *house, ObjectClass *object, TriggerClass *trigger, Cell *cell);
bool TAction_Disable_Short_Game(TActionClass *taction, HouseClass *house, ObjectClass *object, TriggerClass *trigger, Cell *cell);
