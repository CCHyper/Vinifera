/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          LINETRAIL.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Graphical line trails for Objects.
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
#include "linetrail.h"
#include "tibsun_globals.h"
#include "vinifera_globals.h"
#include "options.h"


/**
 *  
 */
LineTrail::LineTrail() :
    Color(128,128,128),
    Next(nullptr),
    ColorDecrement(16),
    field_C(),
    field_10(),
    field_14(),
    field_18(),
    field_1C()
{
}


/**
 *  
 */
LineTrail::~LineTrail()
{
    if (Next) {
        //Next->~LineTrail;
        //Next = nullptr;
    }
}


void LineTrail::Set_Color(RGBClass &color)
{
    Color = color;
}


void LineTrail::Set_Color_Decrement(int dec)
{
    if (Options.DetailLevel == 0) {
        dec = 2 * dec;
    }
    ColorDecrement = dec;
}


void LineTrail::Draw_It()
{
}


void LineTrail::Update()
{
}


bool LineTrail::Is_Something() const
{
    return !Next && !field_1C[field_C];
}


void LineTrail::Draw_All()
{
    for (int i = 0; i < LineTrails.Count(); ++i) {
        LineTrail *line = LineTrails[i];
        line->Update();
        if (!line->Is_Something()) {
            line->Draw_It();
        } else if (true) {
        }
    }
}


void LineTrail::Clear_All()
{
}
