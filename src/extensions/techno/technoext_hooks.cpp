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
#include "technotype.h"
#include "techno.h"
#include "tibsun_globals.h"
#include "tactical.h"
#include "dsurface.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"
#include "vinifera_util.h"


DECLARE_PATCH(_)
{
}


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or deconstructor.
 * 
 *  @note: All functions must not be virtual and must also be prefixed
 *         with "_" to prevent accidental virtualization.
 */
static class FakeTechnoClass : public TechnoClass
{
    public:
        bool _Render(Rect &rect, bool force = false, bool a3 = false);
};


/**
 *  Patch for implementing Render() into 
 * 
 *  @warning: Do not touch this unless you know what you are doing!
 * 
 *  @author: CCHyper
 */
#include "wwfont.h"
bool FakeTechnoClass::_Render(Rect &rect, bool force, bool a3)
{
    //DEBUG_INFO("TechnoClass::Render()\n");



    Point3D lept = Class_Of()->Lepton_Dimensions();
    Point3D lept_center = Point3D(lept.X/2, lept.Y/2, lept.Z/2);

    Point3D pix = Class_Of()->Pixel_Dimensions();
    Point3D pixel_center = Point3D(pix.X/2, pix.Y/2, pix.Z/2);

    //Coordinate coord = Get_Coord();
    Coordinate coord = Center_Coord();
    //coord.X = ;
    //coord.Y = ;
    //coord.Z = ;

    Point2D screen = TacticalMap->func_60F150(coord);

    /**
     *  ????
     */
    screen.X -= TacticalMap->field_5C.X;
    screen.Y -= TacticalMap->field_5C.Y;

    /**
     *  Adjust draw position relative to the viewable tactical area.
     */
    screen.X += TacticalRect.X;
    screen.Y += TacticalRect.Y;

    TempSurface->Fill_Rect(rect, Rect(screen.X, screen.Y, 2, 2), DSurface::RGBA_To_Pixel(255,0,0));

    //screen.X += dim.X/2;
    //screen.Y -= dim.Y;


    TextPrintType style = TPF_CENTER|TPF_FULLSHADOW|TPF_6POINT;
    WWFontClass *font = Font_Ptr(style);

    screen.Y -= font->Get_Char_Height()/2;





    Simple_Text_Print("TEST STRING", TempSurface, &rect, &screen, ColorScheme::As_Pointer("White"), style);

    return ObjectClass::Render(rect, force, a3);
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

    /**
     *  Patches for implementing Render() into TechnoClass.
     */
    Change_Virtual_Address(0x006D7D48, Get_Func_Address(&FakeTechnoClass::_Render)); // TechnoClass
    Change_Virtual_Address(0x006D04C8, Get_Func_Address(&FakeTechnoClass::_Render)); // FootClass
    Change_Virtual_Address(0x006CAEE8, Get_Func_Address(&FakeTechnoClass::_Render)); // AircraftClass
    Change_Virtual_Address(0x006D220C, Get_Func_Address(&FakeTechnoClass::_Render)); // InfantryClass
    Patch_Call(0x00651F39, &FakeTechnoClass::_Render); // UnitClass
}
