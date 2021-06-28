#include "ebolt.h"
/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          EBOLT.CPP
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
#include "ebolt.h"


EBoltClass::EBoltClass() :
	coord1(),
	coord2(),
	_ZAdjust(),
	Random(),
	Owner(nullptr),
	Weapon(),
	Lifetime(),
	AlternateColor(false)
{
}


EBoltClass::~EBoltClass()
{
	if (Owner) {
		Owner->EBolt = nullptr;
		Owner = nullptr;
	}
}
