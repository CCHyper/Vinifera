/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          EBOLT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Graphical electric bolts.
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


class TechnoClass;


class EBoltClass
{
    public:
        EBoltClass();
        ~EBoltClass();

EBolt__Remove_Owner	.text	004C1E50	00000013	00000000	00000000	R	.	.	.	.	.	T	.
EBolt__Draw_Line	.text	004C1E70	000000AB	00000018	00000024	R	.	.	.	.	.	T	.
EBolt__Draw_Blot	.text	004C1F20	0000086F	0000044C	0000001C	R	.	.	.	.	B	T	.
EBolt__Draw	.text	004C2790	0000009B	00000018	00000000	R	.	.	.	.	.	T	.
EBolt__Draw_All	.text	004C2830	000001A9	0000003C	00000000	R	.	.	.	.	.	T	.
Destory_EBoltDVC	.text	004C29E0	00000073	00000008	00000000	R	.	.	.	.	.	T	.
EBolt__Fire	.text	004C2A60	000000D8	00000004	0000001C	R	.	.	.	.	.	T	.
EBolt__Owner_Coords	.text	004C2B40	00000081	0000001C	00000004	R	.	.	.	.	.	T	.
EBolt__Set_Owner_And_Weapon	.text	004C2BD0	0000003B	00000008	00000008	R	.	.	.	.	.	T	.

        static void Draw_All();
        static void Clear_All();

    private:
        Coordinate coord1;
        Coordinate coord2;
        int _ZAdjust;
        int Random;
        TechnoClass *Owner;
        int Weapon;
        int Lifetime;
        bool UseAlternateColor;
};
