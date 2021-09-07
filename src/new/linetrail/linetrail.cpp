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
 * 
 *  @author: CCHyper
 */
LineTrailClass::LineTrailClass(RGBClass &color, int dec, TPoint2D<int> &pos) :
    Color(color),
    Decrement(dec)
{
    /**
     *  If the lowest detail level is set, line trails fades out much quicker.
     */
    if (!Options.DetailLevel) {
        Decrement = (2 * dec);
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
LineTrailClass::~LineTrailClass()
{
}


#if 0
/**
 *  
 * 
 *  @author: CCHyper
 */
void LineTrailClass::Set_Color(RGBClass &color)
{
    Color = color;
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void LineTrailClass::Set_Color_Decrement(int dec)
{
    /**
     *  If the lowest detail level is set, line trails fades out much quicker.
     */
    if (!Options.DetailLevel) {
        dec = (2 * dec);
    }

    Decrement = dec;
}
#endif


/**
 *  
 * 
 *  @author: CCHyper
 */
void LineTrailClass::Draw_It()
{
    v1 = this->__index + 1;

    if ( v1 >= 32 )
    {
        v1 = 0;
    }

    v2 = &this->Trails[this->__index];
    if ( v2->coord.X != LineTrailDefaultCoord.X || v2->coord.Y != LineTrailDefaultCoord.Y || v2->coord.Z != LineTrailDefaultCoord.Z )
    {
        v3 = v1 + 1;
        v4 = &this->Trails[this->__index];
        v5 = (this + 16 * v3);
        if ( v2->mult_dwfield_C )
        {
            do
            {
                if ( v5->coord.X == LineTrailDefaultCoord.X && v5->coord.Y == LineTrailDefaultCoord.Y && v5->coord.Z == LineTrailDefaultCoord.Z )
                {
                    break;
                }
                TacticalMap->Coord_To_Pixel(&v4->coord, &pixel1);
                TacticalMap->Coord_To_Pixel(&v5->coord, &pixel2);
                v6 = -2 - ZDepth_Adjust_For_Height(v4->coord.Z);
                v7 = ZDepth_Adjust_For_Height(v5->coord.Z);
                DSurface_4BEAC0_blit(CompositeSurface, &TacticalRect, &pixel1, &pixel2, &this->rgb, v4->mult_dwfield_C, v6, -2 - v7);
                v8 = v3;
                if ( v3 >= 32 )
                {
                    v8 = 0;
                }
                if ( v8 == this->__index )
                {
                    break;
                }
                v3 = v8 + 1;
                v4 = v5;
                v9 = v8 + 1;
                v10 = v5->mult_dwfield_C;
                v5 = (this + 16 * v9);
            }
            while ( v10 );
        }
    }
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void LineTrailClass::Update()
{
}


/**
 *  
 * 
 *  @author: CCHyper
 */
bool LineTrailClass::Is_Something() const
{
    return !Next && !field_1C[field_C];
}


/**
 *  Draws all active line bolts to the screen.
 * 
 *  @author: CCHyper
 */
void LineTrailClass::Draw_All()
{
    for (int i = 0; i < LineTrails.Count(); ++i) {
        LineTrailClass *linetrail = LineTrails[i];
        linetrail->Update();
        if (!linetrail->Is_Something()) {
            linetrail->Draw_It();

        } else if (true) {
            LineTrails.Delete(linetrail);
            delete linetrail;
        }
    }
}


/**
 *  Removes all line trails from the game world.
 * 
 *  @author: CCHyper
 */
void LineTrailClass::Clear_All()
{
    for (int i = 0; i < LineTrails.Count(); ++i) {
        LineTrailClass *linetrail = LineTrails[i];
        LineTrails.Delete(linetrail);
        delete linetrail;
    }
    LineTrails.Clear();
}
