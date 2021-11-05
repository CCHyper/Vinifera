/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MOUSEEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended MouseClass.
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
#include "mouseext_hooks.h"
#include "tibsun_globals.h"
#include "vinifera_globals.h"
#include "housetype.h"
#include "unittype.h"
#include "infantrytype.h"
#include "aircrafttype.h"
#include "buildingtype.h"
#include "foot.h"
#include "mouse.h"
#include "session.h"
#include "list.h"
#include "textbtn.h"
#include "txtlabel.h"
#include "wwmouse.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Constants and globals for the spawn object popup.
 */
#define SpawnObjectPopup_XPos       (16+10)
#define SpawnObjectPopup_YPos       (10)
#define SpawnObjectPopup_Width      (100)
#define SpawnObjectPopup_Height     (300)

#define SpawnObjectPopup_Rect       Rect(SpawnObjectPopup_XPos, SpawnObjectPopup_YPos, SpawnObjectPopup_Width, SpawnObjectPopup_Height)

#define Padding 5
#define ButtonCount 4

#define HouseList_XPos              (SpawnObjectPopup_XPos+Padding)
#define HouseList_YPos              (SpawnObjectPopup_Width+Padding+25)
#define HouseList_Width             (SpawnObjectPopup_Width-(Padding*2))
#define HouseList_Height            (SpawnObjectPopup_YPos+25)

#define ObjectList_XPos             (SpawnObjectPopup_XPos+Padding)
#define ObjectList_YPos             (SpawnObjectPopup_YPos+Padding+50)
#define ObjectList_Width            (SpawnObjectPopup_Width-(Padding*2))
#define ObjectList_Height           (SpawnObjectPopup_YPos+75)

#define TextButton_Width            ((SpawnObjectPopup_Width/ButtonCount)-(Padding*2))
#define TextButton_Height           14

/**
 *  These are the button ID's for the pop-up object-editing gizmos.
 */
#define POPUP_HOUSELIST             500     // House selection list.
#define POPUP_OBJECTLIST            501     // Object selection list.
#define POPUP_UNITSBUTTON           502     // Units button list.
#define POPUP_INFANTRYBUTTON        503     // Infantry button list.
#define POPUP_AIRCRAFTBUTTON        504     // Aircraft button list.
#define POPUP_BUILDINGSBUTTON       505     // Buildings button list.

/**
 *  Font styles.
 */
#define TPF_POPUPFONT             TPF_METAL12 //TPF_EFNT
#define TPF_POPUPFONT_NOSHADOW    TextPrintType(TPF_POPUPFONT|TPF_NOSHADOW)
#define TPF_POPUPFONT_DROPSHADOW  TextPrintType(TPF_POPUPFONT|TPF_DROPSHADOW)
#define TPF_POPUPFONT_FULLSHADOW  TextPrintType(TPF_POPUPFONT|TPF_FULLSHADOW)
#define TPF_POPUPBUTTON           TextPrintType(TPF_CENTER|TPF_POPUPFONT)

/**
 *  Text string constants.
 */
#define TXT_HOUSES      "Houses"
#define TXT_OBJECTS     "Objects"
#define TXT_UNITS       "Units"
#define TXT_INFANTRY    "Infantry"
#define TXT_AIRCRAFT    "Aircraft"
#define TXT_BUILDINGS   "Buildings"


static ListClass *SpawnObject_HouseList = nullptr;
static ListClass *SpawnObject_ObjectList = nullptr;

static TextButtonClass *SpawnObject_UnitsButton = nullptr;
static TextButtonClass *SpawnObject_InfantryButton = nullptr;
static TextButtonClass *SpawnObject_AircraftButton = nullptr;
static TextButtonClass *SpawnObject_BuildingsButton = nullptr;

static HousesType SpawnObject_House = HOUSE_NONE;
static int SpawnObject_Index = -1;  // NONE
static RTTIType SpawnObject_Type = RTTI_NONE;




/**********************************************************************
**    This structure is used to control the box relief style drawn by
**    the Draw_Box() function.
*/
typedef struct BoxStyleType {
    int Filler;     // Center box fill color.
    int Shadow;     // Shadow color (darker).
    int Highlight;  // Highlight color (lighter).
    int Corner;     // Corner color (transition).
} BoxStyleType;

typedef enum BoxStyleEnum : unsigned char {
    BOXSTYLE_DOWN,       // Typical depressed edge border.
    BOXSTYLE_RAISED,     // Typical raised edge border.
    BOXSTYLE_DIS_DOWN,   // Disabled but depressed.
    BOXSTYLE_DIS_RAISED, // Disabled but raised.
    BOXSTYLE_BOX,        // list box.
    BOXSTYLE_BORDER,     // main dialog box.

    BOXSTYLE_COUNT
} BoxStyleEnum;

/**
 *  Displays a highlighted box.
 */
void Draw_Box(Rect &window, DSurface *surface, BoxStyleEnum up, bool filled)
{
    ColorScheme *scheme = ColorSchemes[GadgetClass::Get_Color_Scheme()];

    /**
     *  Filler, Shadow, Hilite, Corner colors.
     */
    BoxStyleType const ButtonColors[BOXSTYLE_COUNT] = {
        { scheme->Background, scheme->Highlight, scheme->Shadow, scheme->Corners}, // Down
        { scheme->Background, scheme->Shadow, scheme->Highlight, scheme->Corners}, // Raised
        { COLOR_DKGREY, COLOR_WHITE, COLOR_BLACK, COLOR_DKGREY}, // Disabled down
        { COLOR_DKGREY, COLOR_BLACK, COLOR_LTGREY, COLOR_DKGREY}, // Disabled up
        { COLOR_BLACK, scheme->Box, scheme->Box, COLOR_BLACK}, // List box
        { COLOR_BLACK, scheme->Box, scheme->Box, COLOR_BLACK}, // Dialog box
    };

    int x = window.X;
    int y = window.Y;
    int w = window.Width;
    int h = window.Height;

    w--;
    h--;

    const BoxStyleType &style = ButtonColors[up];

    if (filled) {
        surface->Fill_Rect(Rect(x, y, x+w, y+h), style.Filler);
    }

    switch (up) {
        case BOXSTYLE_BOX:
            surface->Draw_Rect(Rect(x, y, x+w, y+h), style.Highlight);
            break;

        case BOXSTYLE_BORDER:
            surface->Draw_Rect(Rect(x+1, y+1, x+w-1, y+h-1), style.Highlight);
            break;

        default:
            surface->Draw_Line(Point2D(x, y+h), Point2D(x+w, y+h), style.Shadow);
            surface->Draw_Line(Point2D(x+w, y), Point2D(x+w, y+h), style.Shadow);

            surface->Draw_Line(Point2D(x, y), Point2D(x+w, y), style.Highlight);
            surface->Draw_Line(Point2D(x, y), Point2D(x, y+h), style.Highlight);

            surface->Put_Pixel(Point2D(x, y+h), style.Corner);
            surface->Put_Pixel(Point2D(x+w, y), style.Corner);
            break;
    }
}



/***********************************************************************************************
 * Dialog_Box -- draws a dialog background box                                                 *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      x,y,w,h      the usual                                                                 *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/26/1995 BR : Created.                                                                  *
 *=============================================================================================*/
void Dialog_Box(Rect &window)
{
    Draw_Box(window, TempSurface, BOXSTYLE_BORDER, true);
}






/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or deconstructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
class MouseClassFake final : public MouseClass
{
    public:
        void _One_Time();
        void _Init_IO();
        void _AI(KeyNumType &input, Point2D &xy);
        void _Draw_It(bool forced = false);
        void _Detach(TARGET target, bool all = true);
};


void MouseClassFake::_One_Time()
{
    MouseClass::One_Time();

    if (Vinifera_DeveloperMode) {

        /**
         *  The House list box.
         */
        SpawnObject_HouseList = new ListClass(POPUP_HOUSELIST,
            HouseList_XPos, HouseList_YPos, HouseList_Width, HouseList_Height,
            TPF_POPUPFONT,
            MFCC::Retrieve("EBTN-UP.SHP"),
            MFCC::Retrieve("EBTN-DN.SHP"));

        /**
         *  The Object list box.
         */
        SpawnObject_ObjectList = new ListClass(POPUP_OBJECTLIST,
            ObjectList_XPos, ObjectList_YPos, ObjectList_Width, ObjectList_Height,
            TPF_POPUPFONT,
            MFCC::Retrieve("EBTN-UP.SHP"),
            MFCC::Retrieve("EBTN-DN.SHP"));

        /**
         *  Object text buttons.
         */
        SpawnObject_UnitsButton = new TextButtonClass(POPUP_UNITSBUTTON, TXT_UNITS, TPF_POPUPBUTTON,
            0, 16, TextButton_Width, TextButton_Height, false, false);
    }
}


void MouseClassFake::_Init_IO()
{
    MouseClass::Init_IO();

    if (Vinifera_DeveloperMode) {

        Add_A_Button(*SpawnObject_HouseList);
        Add_A_Button(*SpawnObject_ObjectList);
        Add_A_Button(*SpawnObject_UnitsButton);

        /**
         *  Populate the House list.
         */
        SpawnObject_HouseList->List.Clear();
        for (HousesType house = HOUSE_FIRST; house < HouseTypes.Count(); ++house) {
            SpawnObject_HouseList->Add_Item(HouseTypeClass::As_Reference(house).Name());
        }

        /**
         *  Populate the initial Object list with units.
         */
        SpawnObject_ObjectList->List.Clear();
        for (UnitType unit = UNIT_FIRST; unit < UnitTypes.Count(); ++unit) {
            SpawnObject_ObjectList->Add_Item(UnitTypeClass::As_Reference(unit).Name());
        }

    }
}


void MouseClassFake::_AI(KeyNumType &input, Point2D &xy)
{
    static bool _enabled = false;

    bool input_handled = false;

    if (!input_handled) {
        MouseClass::AI(input, xy);
    }

    if (Vinifera_DeveloperMode && Session.Singleplayer_Game() && Vinifera_Developer_SpawnObject) {

        Dialog_Box(SpawnObjectPopup_Rect);


        if (!_enabled) {
            SpawnObject_HouseList->Enable();
            SpawnObject_ObjectList->Enable();
            SpawnObject_UnitsButton->Enable();
            //SpawnObject_InfantryButton->Enable();
            //SpawnObject_AircraftButton->Enable();
            //SpawnObject_BuildingsButton->Enable();
            _enabled = true;
        }

        /**
         *  Only handle input in the area of the spawn object popup.
         */
        Rect area(
            TacticalRect.X+SpawnObjectPopup_XPos,
            TacticalRect.X+SpawnObjectPopup_XPos+SpawnObjectPopup_Width,
            SpawnObjectPopup_Width,
            SpawnObjectPopup_Height
        );
        if (area.Is_Within(WWMouse->Get_Mouse_XY())) {

            switch (input) {

                /**
                 *  Object-Editing button: Units button
                 */
                case POPUP_UNITSBUTTON|KN_BUTTON:
                {
                    /**
                     *  Populate the Object list with units.
                     */
                    ListClass *list = ((ListClass *)Buttons->Extract_Gadget(POPUP_BUILDINGSBUTTON));
                    list->List.Clear();
                    for (UnitType unit = UNIT_FIRST; unit < UnitTypes.Count(); ++unit) {
                        list->Add_Item(UnitTypeClass::As_Reference(unit).Name());
                    }

                    SpawnObject_Type = RTTI_UNITTYPE;

                    break;
                }

                /**
                 *  Object-Editing button: Infantry button
                 */
                case POPUP_INFANTRYBUTTON|KN_BUTTON:
                {
                    /**
                     *  Populate the Object list with infantry.
                     */
                    ListClass *list = ((ListClass *)Buttons->Extract_Gadget(POPUP_BUILDINGSBUTTON));
                    list->List.Clear();
                    for (InfantryType infantry = INFANTRY_FIRST; infantry < InfantryTypes.Count(); ++infantry) {
                        list->Add_Item(InfantryTypeClass::As_Reference(infantry).Name());
                    }

                    SpawnObject_Type = RTTI_INFANTRYTYPE;

                    break;
                }

                /**
                 *  Object-Editing button: Aircraft button
                 */
                case POPUP_AIRCRAFTBUTTON|KN_BUTTON:
                {
                    /**
                     *  Populate the Object list with aircraft.
                     */
                    ListClass *list = ((ListClass *)Buttons->Extract_Gadget(POPUP_BUILDINGSBUTTON));
                    list->List.Clear();
                    for (AircraftType aircraft = AIRCRAFT_FIRST; aircraft < AircraftTypes.Count(); ++aircraft) {
                        list->Add_Item(AircraftTypeClass::As_Reference(aircraft).Name());
                    }

                    SpawnObject_Type = RTTI_AIRCRAFTTYPE;

                    break;
                }

                /**
                 *  Object-Editing button: Building button
                 */
                case POPUP_BUILDINGSBUTTON|KN_BUTTON:
                {
                    /**
                     *  Populate the Object list with buildings.
                     */
                    ListClass *list = ((ListClass *)Buttons->Extract_Gadget(POPUP_BUILDINGSBUTTON));
                    list->List.Clear();
                    for (BuildingType buildings = BUILDING_FIRST; buildings < BuildingTypes.Count(); ++buildings) {
                        list->Add_Item(BuildingTypeClass::As_Reference(buildings).Name());
                    }

                    SpawnObject_Type = RTTI_BUILDINGTYPE;

                    break;
                }

                /**
                 *  House List.
                 */
                case POPUP_HOUSELIST|KN_BUTTON:
                {
                    /**
                     *  Determine the house desired by examining the currently
                     *  selected index in the house list gadget.
                     */
                    SpawnObject_House = HousesType(((ListClass *)Buttons->Extract_Gadget(POPUP_HOUSELIST))->Current_Index());

                    break;
                }

                /**
                 *  Objects List.
                 */
                case POPUP_OBJECTLIST|KN_BUTTON:
                {
                    /**
                     *  Determine the house desired by examining the currently
                     *  selected index in the house list gadget.
                     */
                    SpawnObject_Index = (((ListClass *)Buttons->Extract_Gadget(POPUP_OBJECTLIST))->Current_Index());

                    break;
                }

            };

            Buttons->Flag_List_To_Redraw();
            
            /**
             *  Force a redraw of the tactical map.
             */
            Flag_To_Redraw(2);
            
            input = KN_NONE;
            input_handled = true;
        }

    } else {

        SpawnObject_HouseList->Disable();
        SpawnObject_ObjectList->Disable();

        _enabled = false;

    }
}


void MouseClassFake::_Draw_It(bool forced)
{
    MouseClass::Draw_It(forced);
}


void MouseClassFake::_Detach(TARGET target, bool all)
{
    MouseClass::Detach(target, all);
}


/**
 *  Main function for patching the hooks.
 */
void MouseClassExtension_Hooks()
{
    Change_Virtual_Address(0x006CA768, Get_Func_Address(&MouseClassFake::_One_Time));
    Patch_Call(0x004E4838, &MouseClassFake::_One_Time);
    Change_Virtual_Address(0x006CA774, Get_Func_Address(&MouseClassFake::_Init_IO));
    Change_Virtual_Address(0x006CA77C, Get_Func_Address(&MouseClassFake::_AI));
    Change_Virtual_Address(0x006CA790, Get_Func_Address(&MouseClassFake::_Draw_It));
    Change_Virtual_Address(0x006CA7B4, Get_Func_Address(&MouseClassFake::_Detach));
}
