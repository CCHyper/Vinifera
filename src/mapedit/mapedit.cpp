#include "mapedit.h"
#include "mapedit_functions.h"
#include "tibsun_defines.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "tibsun_util.h"
#include "vinifera_globals.h"
#include "rules.h"
#include "house.h"
#include "housetype.h"
#include "trigger.h"
#include "triggertype.h"
#include "tag.h"
#include "tagtype.h"
#include "object.h"
#include "techno.h"
#include "technotype.h"
#include "foot.h"
#include "infantry.h"
#include "infantrytype.h"
#include "unit.h"
#include "unittype.h"
#include "building.h"
#include "buildingtype.h"
#include "aircraft.h"
#include "aircrafttype.h"
#include "overlay.h"
#include "overlaytype.h"
#include "smudge.h"
#include "smudgetype.h"
#include "terrain.h"
#include "terraintype.h"
#include "isotile.h"
#include "isotiletype.h"
#include "scenarioini.h"
#include "msgbox.h"
#include "iomap.h"
#include "session.h"
#include "tactical.h"
#include "textprint.h"
#include "control.h"
#include "gauge.h"
#include "dial8.h"
#include "textbtn.h"
#include "txtlabel.h"
#include "list.h"
#include "wwfont.h"
#include "bsurface.h"
#include "dsurface.h"
#include "drawshape.h"
#include "wwkeyboard.h"
#include "wwmouse.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"


ControlClass *MapEditClass::MapArea = nullptr;
ListClass *MapEditClass::HouseList = nullptr;
ListClass *MapEditClass::MissionList = nullptr;
TriColorGaugeClass *MapEditClass::HealthGauge = nullptr;
Dial8Class *MapEditClass::FacingDial = nullptr;
TextLabelClass *MapEditClass::HealthText = nullptr;
TextButtonClass *MapEditClass::Sellable = nullptr;
TextButtonClass *MapEditClass::Rebuildable = nullptr;
TextButtonClass *MapEditClass::Nominal = nullptr;
TextButtonClass *MapEditClass::Repairable = nullptr;
GaugeClass *MapEditClass::BaseGauge = nullptr;
TextLabelClass *MapEditClass::BaseLabel = nullptr;
Cell MapEditClass::CurrentCell = Cell(-1,-1);
//TriggerClass *MapEditClass::CurTrigger = nullptr;
TagClass *MapEditClass::CurTag = nullptr;
ObjectClass *MapEditClass::GrabbedObject = nullptr;
Cell MapEditClass::GrabOffset = Cell(-1,-1);
DynamicVectorClass<const ObjectTypeClass * > MapEditClass::AvailableObjects;
int MapEditClass::NumType[8 /*NUM_EDIT_CLASSES*/];
int MapEditClass::TypeOffset[8 /*NUM_EDIT_CLASSES*/];
int MapEditClass::LastChoice = 0;
HousesType MapEditClass::LastHouse = HOUSE_NONE;
unsigned MapEditClass::LastClickTime;
bool MapEditClass::IsLeftMouseDown = false;
bool MapEditClass::IsMapChanged = false;
bool MapEditClass::IsBaseBuilding = false;
bool MapEditClass::IsMarbleMadnessEnabled = false;
bool MapEditClass::IsShowHelp = false;
bool MapEditClass::IsShowOccupiers = false;
bool MapEditClass::IsShowWaypoints = true;
bool MapEditClass::IsShowCellTags = true;

static char HealthBuf[20];

/**
 *  Array of all missions supported by the map editor.
 */
static MissionType MapEditMissions[] = {
    MISSION_SLEEP,
    //MISSION_ATTACK,
    //MISSION_MOVE,
    //MISSION_QMOVE,
    //MISSION_RETREAT,
    MISSION_GUARD,
    MISSION_STICKY,
    //MISSION_ENTER,
    //MISSION_CAPTURE,
    MISSION_HARVEST,
    MISSION_GUARD_AREA,
    MISSION_RETURN, 
    //MISSION_STOP,
    MISSION_AMBUSH,
    MISSION_HUNT,
    //MISSION_UNLOAD,
    //MISSION_SABOTAGE,
    //MISSION_CONSTRUCTION,
    //MISSION_DECONSTRUCTION,
    //MISSION_REPAIR,
    //MISSION_RESCUE,
    //MISSION_MISSILE,
    MISSION_HARMLESS,
    //MISSION_OPEN,
    //MISSION_PATROL,
};
#define NUM_EDIT_MISSIONS (sizeof(MapEditMissions) / sizeof(MapEditMissions[0]))


/**
 *  Text string constants.
 */
#define TXT_SELLABLE "Sellable"
#define TXT_REBUILD "Rebuild"
#define TXT_REPAIRABLE "Repairable"
#define TXT_NOMINAL "Nominal"


/**
 *  Editor font styles.
 */
#define TPF_EFONT             TPF_METAL12 //TPF_EFNT
#define TPF_EFONT_NOSHADOW    TextPrintType(TPF_EFONT|TPF_NOSHADOW)
#define TPF_EFONT_DROPSHADOW  TextPrintType(TPF_EFONT|TPF_DROPSHADOW)
#define TPF_EFONT_FULLSHADOW  TextPrintType(TPF_EFONT|TPF_FULLSHADOW)
#define TPF_EBUTTON           TextPrintType(TPF_CENTER|TPF_EFONT)


/**
 *  These are the button ID's for the pop-up object-editing gizmos.
 * 
 *  The house button ID's must be sequential, with a 1-to-1 correspondence to
 *  the HousesType values.
 */
enum MapEditButtonIDEnum
{
    POPUP_HOUSE = 500,          // House buttons

    POPUP_HOUSELIST = 600,      // House selection list.
    POPUP_SELLABLE,             // Allowed to sell.
    POPUP_REBUILDABLE,          // Allowed to rebuild.
    POPUP_NOMINAL,              // 
    POPUP_REPAIRABLE,           // Allowed to repair.
    POPUP_MISSIONLIST,          // List box for missions
    POPUP_HEALTHGAUGE,          // Health of object
    POPUP_FACINGDIAL,           // Object's facing
    POPUP_BASEPERCENT,          // Base's percent-built slider

    MAP_AREA,                   // Map as a click-able thingy
};

enum MapEditPositionEnum
{
    POPUP_HOUSE_X = 10,
    POPUP_HOUSE_Y = 100,
    POPUP_HOUSE_W = 60,
    POPUP_HOUSE_H = (190-100),

    POPUP_MISSION_W = 160,
    POPUP_MISSION_H = 80,
    POPUP_MISSION_X = 140,
    POPUP_MISSION_Y = 300,

    POPUP_FACEBOX_W = 60,
    POPUP_FACEBOX_H = 60,
    POPUP_FACEBOX_X = 320,
    POPUP_FACEBOX_Y = 320,

    POPUP_HEALTH_W = 100,
    POPUP_HEALTH_H = 20,
    POPUP_HEALTH_X = 400,
    POPUP_HEALTH_Y = 340,

    POPUP_BASE_W = 100,
    POPUP_BASE_H = 16,
    POPUP_BASE_X = 600-POPUP_BASE_W,
    POPUP_BASE_Y = 0,

    POPUP_BUILDING_OPTION_X = 600,
    POPUP_BUILDING_OPTION_W = 85,
    POPUP_BUILDING_OPTION_H = 10,
};


static int Scale_Float_To_Int(float value, int scale)
{
    value = std::clamp(value, 0.0f, 1.0f);
    return (value * (float)scale);
}


void MapEditClass::_One_Time()
{
    MouseClass::One_Time();

    /**
     *  The map: A single large "button".
     */
    MapArea = new ControlClass(MAP_AREA,
        0, 16, Options.ScreenWidth, Options.ScreenHeight-16,
        GadgetClass::LEFTPRESS|GadgetClass::LEFTRELEASE, false);

    /**
     *  House buttons.
     */
    HouseList = new ListClass(POPUP_HOUSELIST, POPUP_HOUSE_X,
        MapArea->Height-POPUP_HOUSE_H-10, POPUP_HOUSE_W, POPUP_HOUSE_H,
        TPF_EFONT,
        MFCC::Retrieve("EBTN-UP.SHP"),
        MFCC::Retrieve("EBTN-DN.SHP"));

    /**
     *  The mission list box.
     */
    MissionList = new ListClass(POPUP_MISSIONLIST,
        POPUP_MISSION_X, MapArea->Height-POPUP_MISSION_H-10, POPUP_MISSION_W, POPUP_MISSION_H,
        TPF_EFONT,
        MFCC::Retrieve("EBTN-UP.SHP"),
        MFCC::Retrieve("EBTN-DN.SHP"));
    for (int i = 0; i < NUM_EDIT_MISSIONS; ++i) {
        MissionList->Add_Item(MissionClass::Mission_Name(MapEditMissions[i]));
    }

    /**
     *  The object health bar.
     */
    HealthGauge = new TriColorGaugeClass(POPUP_HEALTHGAUGE,
        POPUP_HEALTH_X, MapArea->Height-POPUP_HEALTH_H-19, POPUP_HEALTH_W, POPUP_HEALTH_H);
    HealthGauge->Use_Thumb(true);
    HealthGauge->Set_Maximum(256);
    HealthGauge->Set_Yellow_Limit(127 - 1);
    HealthGauge->Set_Red_Limit(63 - 1);

    /**
     *  The health text label.
     */
    HealthBuf[0] = '\0';
    HealthText = new TextLabelClass(HealthBuf,
        HealthGauge->X + HealthGauge->Width / 2,
        HealthGauge->Y + HealthGauge->Height + 1,
        GadgetClass::Get_Color_Scheme(),
        TPF_CENTER|TPF_EFONT_DROPSHADOW);

    /**
     *  Building attribute buttons.
     */
    Sellable = new TextButtonClass(POPUP_SELLABLE, TXT_SELLABLE, TPF_EBUTTON,
        POPUP_BUILDING_OPTION_X, MapArea->Height-66, POPUP_BUILDING_OPTION_W, POPUP_BUILDING_OPTION_H, false, false);
    Rebuildable = new TextButtonClass(POPUP_REBUILDABLE, TXT_REBUILD, TPF_EBUTTON,
        POPUP_BUILDING_OPTION_X, MapArea->Height-54, POPUP_BUILDING_OPTION_W, POPUP_BUILDING_OPTION_H, false, false);
    Repairable = new TextButtonClass(POPUP_REPAIRABLE, TXT_REPAIRABLE, TPF_EBUTTON,
        POPUP_BUILDING_OPTION_X, MapArea->Height-42, POPUP_BUILDING_OPTION_W, POPUP_BUILDING_OPTION_H, false, false);
    Nominal = new TextButtonClass(POPUP_NOMINAL, TXT_NOMINAL, TPF_EBUTTON,
        POPUP_BUILDING_OPTION_X, MapArea->Height-30, POPUP_BUILDING_OPTION_W, POPUP_BUILDING_OPTION_H, false, false);

    /**
     *  The unit facing dial.
     */
    FacingDial = new Dial8Class(POPUP_FACINGDIAL, POPUP_FACEBOX_X,
        MapArea->Height-POPUP_FACEBOX_H-10, POPUP_FACEBOX_W, POPUP_FACEBOX_H, DIR_N);

    /**
     *  The base percent-built slider & its label
     */
    BaseGauge = new GaugeClass(POPUP_BASEPERCENT, POPUP_BASE_X, MapArea->Width-POPUP_BASE_W, POPUP_BASE_W, POPUP_BASE_H);
    BaseLabel = new TextLabelClass("Base:", POPUP_BASE_X - 3, MapArea->Width-POPUP_BASE_W, GadgetClass::Get_Color_Scheme(),
        TPF_RIGHT|TPF_EFONT_FULLSHADOW);
    BaseGauge->Set_Maximum(100);
    BaseGauge->Set_Value(Scen->Percent);
}


void MapEditClass::_Init_IO()
{
    /**
     *  For normal game mode, jump to the parent's Init routine.
     */
    if (!Debug_Map) {

        //GadgetClass::Set_Color_Scheme("Grey");

        MouseClass::Init_IO();

    } else {

        //GadgetClass::Set_Color_Scheme("LightBlue");

        /**
         *  For editor mode, add the map area to the button input list.
         */
        Buttons = nullptr;
        Add_A_Button(*BaseGauge);
        Add_A_Button(*BaseLabel);
        Add_A_Button(*MapArea);

        /**
         *  Populate the House list.
         */
        HouseList->List.Clear();
        for (HousesType house = HOUSE_FIRST; house < HouseTypes.Count(); ++house) {
            HouseList->Add_Item(HouseTypeClass::As_Reference(house).Name());
        }

        /**
         *  Set health bar limits.
         */
        HealthGauge->Set_Maximum(Scale_Float_To_Int(Rule->ConditionGreen, 256));
        HealthGauge->Set_Yellow_Limit(Scale_Float_To_Int(Rule->ConditionYellow, 256)-1);
        HealthGauge->Set_Red_Limit(Scale_Float_To_Int(Rule->ConditionRed, 256)-1);
    }
}


void MapEditClass::_AI(KeyNumType &input, Point2D &xy)
{
    /**
     *  Trap 'F2' regardless of whether we're in game or editor mode.
     */
    if (Vinifera_DeveloperMode && Session.Singleplayer_Game()) {

#ifndef NDEBUG
        if (input == KN_F2) {
#else
        if (input == (KN_F2|KN_CTRL_BIT)) {
#endif
            ScenarioInit = 0;

            /*
            ** If Changed is set, prompt for saving changes.
            */
            if (Debug_Map && IsMapChanged) {
                int rc = WWMessageBox().Process("Save Changes?", 0, TXT_YES, TXT_NO);
                HiddenSurface->Clear();
                Flag_To_Redraw(true);
                Render();

                /**
                 *  User wants to save.
                 */
                if (rc == 0) {

                    /**
                     *  If save cancelled, abort game.
                     */
                    if (Save_Scenario()) {
                        input = KN_NONE;

                    } else {
                        IsMapChanged = false;
                        Go_Editor(!Debug_Map);
                    }

                } else {

                    /**
                     *  User doesn't want to save.
                     */
                    Go_Editor(!Debug_Map);
                }

            } else {

                /**
                 *  If we're in game mode, set IsMapChanged to false (so if we didn't save
                 *  our changes above, they won't keep coming back to haunt us with continual
                 *  "Save Changes?" prompts!)
                 */
                if (!Debug_Map) {
                    IsMapChanged = false;
                }
                BaseGauge->Set_Value(Scen->Percent);
                Go_Editor(!Debug_Map);
            }
        }
    }

    /**
     *  For normal game mode, jump to the parent's AI routine.
     */
    if (!Debug_Map || !Session.Singleplayer_Game()) {
        MouseClass::AI(input, xy);
        return;
    }

    /**
     *  Do special mouse processing if the mouse is over the map.
     */
    if (WWMouse->Get_Mouse_X() >= TacticalRect.X && WWMouse->Get_Mouse_X() <=
        TacticalRect.X + TacticalRect.Width &&
        WWMouse->Get_Mouse_Y() >= TacticalRect.Y && WWMouse->Get_Mouse_Y() <=
        TacticalRect.Y + TacticalRect.Height) {

        /**
         *  When the mouse moves over a scrolling edge, ScrollClass changes its
         *  shape to the appropriate arrow or NO symbol; it's our job to change it
         *  back to normal (or whatever the shape is set to by Set_Default_Mouse())
         *  when it re-enters the map area.
         */
        if (CurTag) {
            Override_Mouse_Shape(MOUSE_CAN_MOVE);
        } else {
            Override_Mouse_Shape(MOUSE_NORMAL);
        }
    }

    /**
     *  Set 'ZoneCell' to track the mouse cursor around over the map. Do this
     *  even if the map is scrolling.
     */
    if (WWMouse->Get_Mouse_X() >= TacticalRect.X && WWMouse->Get_Mouse_X() <=
        TacticalRect.X + TacticalRect.Width &&
        WWMouse->Get_Mouse_Y() >= TacticalRect.Y && WWMouse->Get_Mouse_Y() <=
        TacticalRect.Y + TacticalRect.Height) {

        Cell cell = Click_Cell_Calc(WWMouse->Get_Mouse_XY());
        if (cell) {
            Set_Cursor_Pos(cell);
            if (PendingObject) {
                Flag_To_Redraw(true);
            }
        }
    }

    /**
     *  Check for mouse motion while left button is down.
     */
    if (IsLeftMouseDown && Mouse_Moved()) {

        /**
         *  "Paint" mode: place current object, and restart placement.
         */
        if (PendingObject) {
            Flag_To_Redraw(true);
            if (Place_Object()) {
                IsMapChanged = true;
                Start_Placement();
            }
        } else {

            /**
             *  Move the currently-grabbed object.
             */
            if (GrabbedObject) {
                GrabbedObject->Mark(MARK_CHANGE);
                if (Move_Grabbed_Object()) {
                    IsMapChanged = true;
                }
            }
        }
    }

    /**
     *  Increment game frame.
     */
    ++Frame;
    
    /**
     *  Trap special editing keys; if one is detected, set 'input' to 0 to
     *  prevent a conflict with parent's AI().
     */
    switch (input) {

        /**
         *  F1:
         * 
         *  Show help.
         */
        case KN_F1:
            IsShowHelp = !IsShowHelp;
            HiddenSurface->Clear();
            Flag_To_Redraw(true);
            input = KN_NONE;
            break;

        /**
         *  F2:
         * 
         *  
         */
        //case KN_F2:
        //    break;

        /**
         *  F3:
         * 
         *  
         */
        case KN_F3:
            IsShowOccupiers = !IsShowOccupiers;
            HiddenSurface->Clear();
            Flag_To_Redraw(true);
            input = KN_NONE;
            break;

        /**
         *  F4:
         * 
         *  
         */
        case KN_F4:
            IsShowWaypoints = !IsShowWaypoints;
            HiddenSurface->Clear();
            Flag_To_Redraw(true);
            input = KN_NONE;
            break;

        /**
         *  F5:
         * 
         *  
         */
        case KN_F5:
            IsShowCellTags = !IsShowCellTags;
            HiddenSurface->Clear();
            Flag_To_Redraw(true);
            input = KN_NONE;
            break;

        /**
         *  F6:
         * 
         *  Toggle passable/impassable display.
         */
        case KN_F6:
            Debug_Passable = !Debug_Passable;
            HiddenSurface->Clear();
            Flag_To_Redraw(true);
            input = KN_NONE;
            break;

        /**
         *  F7:
         * 
         *  
         */
        case KN_F7:
            if (Placement_Dialog()) {           // TEMP
                Start_Placement();
            }
            break;

        /**
         *  F8:
         * 
         *  
         */
        case KN_F8:
            break;

        /**
         *  F9:
         * 
         *  
         */
        case KN_F9:
            break;

        /**
         *  F10:
         * 
         *  
         */
        case KN_F10:
            break;

        /**
         *  F11:
         * 
         *  
         */
        case KN_F11:
            break;

        /**
         *  F12:
         * 
         *  
         */
        case KN_F12:
            break;

        /**
         *  ESCAPE:
         * 
         *  Exit placement mode or exit to desktop.
         */
        case KN_ESC:
        {
            /**
             *  Exit object placement mode.
             */
            if (PendingObject) {
                if (IsBaseBuilding) {
                    Cancel_Base_Building();
                } else {
                    Cancel_Placement();
                }
                input = KN_NONE;
                break;
            }

            /**
             *  Exit tag placement mode.
             */
            if (CurTag) {
                Stop_Tag_Placement();
                input = KN_NONE;
                break;
            }

            int rc = WWMessageBox().Process("Exit Scenario Editor?", 0, TXT_YES, TXT_NO);
            HiddenSurface->Clear();
            Flag_To_Redraw(true);
            Render();

            /**
             *  User doesn't want to exit; return to editor.
             */
            if (rc == 1) {
                input = KN_NONE;
                break;
            }

            /**
             *  If changed, prompt for saving.
             */
            if (IsMapChanged) {
                int rc = WWMessageBox().Process("Save Changes?", 0, TXT_YES, TXT_NO);
                HiddenSurface->Clear();
                Flag_To_Redraw(true);
                Render();

                /**
                 *  User wants to save.
                 */
                if (rc == 0) {

                    /**
                     *  If save cancelled, abort exit.
                     */
                    if (Save_Scenario()) {
                        input = KN_NONE;
//                      break;
                    } else {
                        IsMapChanged = false;
                    }
                }
            }

            /**
             *  Quit to the desktop.
             */
            //Fatal(EXIT_SUCCESS, "Map Editor");
            GameActive = false;
            break;
        }

        /**
         *  DELETE:
         *
         *  Delete currently-selected object.
         */
        case KN_DELETE:
        {
            /**
             *  Delete currently-selected object's trigger, or the object.
             */
            if (CurrentObjects.Count()) {

                /**
                 *  Delete trigger.
                 */
                if (CurrentObjects.Fetch_Head()->Tag) {
                    CurrentObjects.Fetch_Head()->Tag = nullptr;

                } else {
                    /**
                     *  If the current object is part of the AI's Base, remove it
                     *  from the Base's Node list.
                     */
                    //if (CurrentObjects.Fetch_Head()->What_Am_I() == RTTI_BUILDING &&
                    //    Base.Is_Node((BuildingClass *)CurrentObjects.Fetch_Head())) {
                    //    node = Base.Get_Node((BuildingClass *)CurrentObjects.Fetch_Head());
                    //    Base.Nodes.Delete(*node);
                    //}

                    /**
                     *  Delete current object.
                     */
                    delete CurrentObjects.Fetch_Head();

                    /**
                     *  Hide the popup controls.
                     */
                    Popup_Controls();
                }

                /**
                 *  Force a redraw.
                 */
                HiddenSurface->Clear();
                Flag_To_Redraw(true);

                IsMapChanged = true;

            } else {

                /**
                 *  Remove trigger from current cell.
                 */
                if (CurrentCell) {
                    if (Map[CurrentCell].CellTag) {
                        Map[CurrentCell].CellTag = nullptr;

                        /**
                         *  Force a redraw.
                         */
                        HiddenSurface->Clear();
                        Flag_To_Redraw(true);
                        IsMapChanged = true;
                    }
                }
            }

            input = KN_NONE;
            break;
        }

        /**
         *  H
         *
         *  Toggle current placement object's house.
         */
        case KN_H:
            if (PendingObject) {
                Toggle_House();
                HouseList->Set_Selected_Index(PendingHouse);
            }
            input = KN_NONE;
            break;

        /**
         *  CTRL+ALT+M:
         * 
         *  Toggle marble madness mode.
         */
        case KN_CTRL_BIT|KN_ALT_BIT|KN_M:
        case KN_M:
            DEBUG_INFO("Toggle marble madness mode.\n");
            IsMarbleMadnessEnabled = !IsMarbleMadnessEnabled;
            break;

        /*
        **    HOME = jump to first placement object, or go to Home Cell
        */
        case KN_HOME:
            if (PendingObject) {
                Place_Home();
            } else {

                /*
                **    Set map position
                */
                ScenarioInit++;
                Set_Tactical_Position(Cell_Coord(Scen->Waypoint[WAYPT_HOME]));
                ScenarioInit--;

                /*
                **    Force map to redraw
                */
                HiddenSurface->Clear();
                Flag_To_Redraw(true);
                Render();
            }
            input = KN_NONE;
            break;

        /*
        **    SHIFT-HOME: Set new Home Cell position
        */
        case KN_HOME|KN_SHIFT_BIT:
        {
            if (!CurrentCell) {
                break;
            }

            /*
            ** Unflag the old Home Cell, if there are no other waypoints
            ** pointing to it
            */
            Cell cell = Scen->Waypoint[WAYPT_REINF];

            if (cell) {
                bool found = false;
                for (int i = 0; i < WAYPT_COUNT; i++) {
                    if (i != WAYPT_REINF && Scen->Waypoint[i] == cell) {
                        found = true;
                    }
                }

                if (!found) {
                    (*this)[cell].IsWaypoint = false;
                }
            }

            /*
            ** Now set the new Home cell
            */
            Scen->Waypoint[WAYPT_HOME] = CurrentCell;
            (*this)[CurrentCell].IsWaypoint = true;

            IsMapChanged = true;
            input = KN_NONE;
            break;
        }

        /*
        **    SHIFT-R: Set new Reinforcement Cell position.  Don't allow setting
        **    the Reinf. Cell to the same as the Home Cell (for display purposes.)
        */
        case KN_R|KN_SHIFT_BIT:
        {
            if (!CurrentCell || CurrentCell == Scen->Waypoint[WAYPT_HOME]) {
                break;
            }

            /*
            ** Unflag the old Reinforcement Cell, if there are no other waypoints
            ** pointing to it.
            */
            Cell cell = Scen->Waypoint[WAYPT_REINF];

            if (cell) {
                bool found = false;
                for (int i = 0; i < WAYPT_COUNT; i++) {
                    if (i != WAYPT_REINF && Scen->Waypoint[i] == cell) {
                        found = true;
                    }
                }

                if (!found) {
                    (*this)[cell].IsWaypoint = false;
                }
            }

            /*
            ** Now set the new Reinforcement cell
            */
            Scen->Waypoint[WAYPT_REINF] = CurrentCell;
            (*this)[CurrentCell].IsWaypoint = true;
            IsMapChanged = true;
            input = KN_NONE;
            break;
        }

        /**
         *    ALT+Letter(A-Z):
         *
         *  Label a waypoint cell
         */
        case KN_A|KN_ALT_BIT:
        case KN_B|KN_ALT_BIT:
        case KN_C|KN_ALT_BIT:
        case KN_D|KN_ALT_BIT:
        case KN_E|KN_ALT_BIT:
        case KN_F|KN_ALT_BIT:
        case KN_G|KN_ALT_BIT:
        case KN_H|KN_ALT_BIT:
        case KN_I|KN_ALT_BIT:
        case KN_J|KN_ALT_BIT:
        case KN_K|KN_ALT_BIT:
        case KN_L|KN_ALT_BIT:
        case KN_M|KN_ALT_BIT:
        case KN_N|KN_ALT_BIT:
        case KN_O|KN_ALT_BIT:
        case KN_P|KN_ALT_BIT:
        case KN_Q|KN_ALT_BIT:
        case KN_R|KN_ALT_BIT:
        case KN_S|KN_ALT_BIT:
        case KN_T|KN_ALT_BIT:
        case KN_U|KN_ALT_BIT:
        case KN_V|KN_ALT_BIT:
        case KN_W|KN_ALT_BIT:
        case KN_X|KN_ALT_BIT:
        case KN_Y|KN_ALT_BIT:
        case KN_Z|KN_ALT_BIT:
        {
            if (CurrentCell) {
                int waypt_idx = (input & ~KN_ALT_BIT) - KN_A;
                Update_Waypoint(waypt_idx);
            }
            input = KN_NONE;
            break;
        }

        /*
        ** ALT-. : Designate an extended (2-letter) waypoint name
        */
#if 0
        case KN_PERIOD:
        case ((int)KN_PERIOD | (int)KN_ALT_BIT):
            if (CurrentCell && Get_Waypoint_Name(wayname)) {
                int waynm = 0;
                if (std::strlen(wayname)) {
                    wayname[0] = toupper(wayname[0]);
                    wayname[1] = toupper(wayname[1]);
                    if (wayname[0] >= 'A' && wayname[0] <= 'Z') {
                        waynm = wayname[0] - 'A';
                        if (wayname[1] >= 'A' && wayname[1] <= 'Z') {
                            waynm = (waynm+1)*26 + (wayname[1] - 'A');
                        }
                        if (waynm < WAYPT_HOME) Update_Waypoint(waynm);
                    }
                }
            }
            input = KN_NONE;
            break;
#endif

        /**
         *  ALT+SPACE:
         *
         *  Remove a waypoint designation.
         */
        case KN_SPACE|KN_ALT_BIT:
            if (CurrentCell) {

                /*
                **    Loop through letter waypoints; if this cell is one of them,
                **    clear that waypoint.
                */
                for (int i = 0 ; i < WAYPT_HOME; i++) {
                    if (Scen->Waypoint[i]==CurrentCell)
                        Scen->Waypoint[i] = -1;
                }

                /*
                **    Loop through flag home values; if this cell is one of them, clear
                **    that waypoint.
                */
#if 0
                for (int i = 0; i < MAX_PLAYERS; i++) {
                    house = (HousesType)(HOUSE_MULTI1 + i);
                    if (HouseClass::As_Pointer(house) &&
                        CurrentCell == HouseClass::As_Pointer(house)->FlagHome)
                    HouseClass::As_Pointer(house)->Flag_Remove(As_Target(CurrentCell), true);
                }
#endif

                /*
                **    If there are no more waypoints on this cell, clear the cell's
                **    waypoint designation.
                */
                if (Scen->Waypoint[WAYPT_HOME]!=CurrentCell &&
                    Scen->Waypoint[WAYPT_REINF]!=CurrentCell)
                    (*this)[CurrentCell].IsWaypoint = false;
                IsMapChanged = true;
            }
            input = KN_NONE;
            break;

        /**
         *  SHIFT+ALT+<arrow key>:
         *
         *  Move the current object.
         */
        case (KN_UP|KN_ALT_BIT|KN_SHIFT_BIT):
        case (KN_DOWN|KN_ALT_BIT|KN_SHIFT_BIT):
        case (KN_LEFT|KN_ALT_BIT|KN_SHIFT_BIT):
        case (KN_RIGHT|KN_ALT_BIT|KN_SHIFT_BIT):
            if (CurrentObjects.Count()) {
                CurrentObjects.Fetch_Head()->Move(KN_To_Facing(input)-1);
                IsMapChanged = true;
            }
            input = KN_NONE;
            break;

        /**
         *  TAB:
         * 
         *  Select next object on the map.
         */
        case KN_TAB:
            Select_Next();
            input = KN_NONE;
            break;

        /**
         *  CTRL+TAB:
         * 
         *  Select previous object on the map.
         */
        case KN_CTRL_BIT|KN_TAB:
            Select_Prev();
            input = KN_NONE;
            break;

        /**
         *  RIGHT:
         *
         *  Go to previous placement object.
         */
        case KN_LEFT:
            if (PendingObject) {
                Place_Prev();
            }
            input = KN_NONE;
            break;

        /**
         *  RIGHT:
         *
         *  Go to next placement object.
         */
        case KN_RIGHT:
            if (PendingObject) {
                Place_Next();
            }
            input = KN_NONE;
            break;
  
        /**
         *  PGUP:
         *
         *  Go to previous placement category
         */
        case KN_PGUP:
            if (PendingObject) {
                Place_Prev_Category();
            }
            input = KN_NONE;
            break;

        /**
         *  PGDN:
         *
         *  Go to next placement category.
         */
        case KN_PGDN:
            if (PendingObject) {
                Place_Next_Category();
            }
            input = KN_NONE;
            break;


        /**
         *  RMOUSE:
         *
         *  Pop up context menu.
         */
        case KN_RMOUSE:

            /*
            **	Turn off placement mode
            */
            if (PendingObject) {
                if (IsBaseBuilding) {
                    Cancel_Base_Building();
                } else {
                    Cancel_Placement();
                }
            }

            /*
            **	Turn off trigger placement mode
            */
            if (CurTag) {
                Stop_Tag_Placement();
            }

            /*
            **	Unselect object & hide popup controls
            */
            Unselect_All();
            Popup_Controls();

            /*
            **  
            */
            //if (WWKeyboard->Down(KN_RMOUSE)) {
            //
            //    Remove_A_Button(*HouseList);
            //    Remove_A_Button(*MissionList);
            //    Remove_A_Button(*HealthGauge);
            //    Remove_A_Button(*HealthText);
            //    Remove_A_Button(*FacingDial);
            //    Remove_A_Button(*BaseGauge);
            //    Remove_A_Button(*BaseLabel);
            //    Remove_A_Button(*MapArea);
            //
            //    Remove_A_Button(*Sellable);
            //    Remove_A_Button(*Rebuildable);
            //    Remove_A_Button(*Repairable);
            //    Remove_A_Button(*Nominal);
            //
            //    Add_A_Button(*BaseGauge);
            //    Add_A_Button(*BaseLabel);
            //    Add_A_Button(*MapArea);
            //
            //    IsLeftMouseDown = false;
            //    GrabbedObject = nullptr;
            //}

            Context_Menu(WWMouse->Get_Mouse_XY());

            input = KN_NONE;

            break;


        /**
         *  Editor tactical gadget.
         * 
         *  Left-mouse click:
         *    Button DOWN:
         *    - Toggle LMouseDown
         *    - If we're in placement mode, try to place the current object
         *    - If success, re-enter placement mode
         *    - Otherwise, try to select an object, and "grab" it if there is one
         *    - If no object, then select that cell as the "current" cell
         * 
         *    Button UP:
         *    - Toggle LMouseDown
         *    - release any grabbed object
         */
        case MAP_AREA|KN_BUTTON:
        {
            /*
            **  Left Button: DOWN
            */
            if (WWKeyboard->Down(KN_LMOUSE)) {

                IsLeftMouseDown = true;

                /*
                **  Placement mode: place an object
                */
                if (PendingObject) {
                    if (Place_Object()) {
                        IsMapChanged = true;
                        Start_Placement();
                    }

                } else {

                    /*
                    **  Place a trigger
                    */
                    if (CurTag) {
                        Place_Tag();
                        IsMapChanged = true;

                    } else {

                        /**
                         *  Select an object or a cell
                         *  Check for double-click
                         */
                        if (CurrentObjects.Count() &&
                            ((TickCount - LastClickTime) < 15)) {
                            ;    // stub

                        } else {
                            /**
                             *  Single-click: select object
                             */
                            if (Select_Object()) {
                                CurrentCell = Cell(0,0);
                                Grab_Object();

                            } else {

                                /**
                                 *  No object: select the cell.
                                 */
                                CurrentCell = Click_Cell_Calc(Point2D(WWKeyboard->MouseQX, WWKeyboard->MouseQY));
                                HiddenSurface->Clear();
                                Flag_To_Redraw(true);
                                Render();
                            }
                        }
                    }
                }
                LastClickTime = TickCount();
                input = KN_NONE;

            /**
             *  Left Button: UP
             */
            } else {
                IsLeftMouseDown = false;
                GrabbedObject = nullptr;
                input = KN_NONE;
            }
            break;
        }


        /**
         * 
         *  Button events.
         * 
         */

        /**
         *  Object-Editing button: Nominal
         */
        case POPUP_NOMINAL|KN_BUTTON:
        {
            if (CurrentObjects.Count() && CurrentObjects.Fetch_Head()->What_Am_I() == RTTI_BUILDING) {
                BuildingClass *building = ((BuildingClass *)CurrentObjects.Fetch_Head());

                if (building->Class->TechLevel != -1) {
                    building->IsNominal = (building->IsNominal == false);
                    building->Mark(MARK_CHANGE);
                }
                if (building->IsNominal) {
                    Nominal->Turn_On();
                } else {
                    Nominal->Turn_Off();
                }
            }
            break;
        }
 
        /**
         *  Object-Editing button: Repairable
         */
        case POPUP_REPAIRABLE|KN_BUTTON:
        {
            if (CurrentObjects.Count() && CurrentObjects.Fetch_Head()->What_Am_I() == RTTI_BUILDING) {
                BuildingClass *building = ((BuildingClass *)CurrentObjects.Fetch_Head());

                if (building->Class->TechLevel != -1) {
                    building->IsToRepair = (building->IsToRepair == false);
                    building->Mark(MARK_CHANGE);
                }
                if (building->IsToRepair) {
                    Repairable->Turn_On();
                } else {
                    Repairable->Turn_Off();
                }
            }
            break;
        }

        /**
         *  Object-Editing button: Sellable
         */
        case POPUP_SELLABLE|KN_BUTTON:
        {
            if (CurrentObjects.Count() && CurrentObjects.Fetch_Head()->What_Am_I() == RTTI_BUILDING) {
                BuildingClass *building = ((BuildingClass *)CurrentObjects.Fetch_Head());

                if (building->Class->TechLevel != -1) {
                    building->IsAllowedToSell = (building->IsAllowedToSell == false);
                    building->Mark(MARK_CHANGE);
                }
                if (building->IsAllowedToSell) {
                    Sellable->Turn_On();
                } else {
                    Sellable->Turn_Off();
                }
            }
            break;
        }
        
        /**
         *  Object-Editing button: Rebuildable
         */
        case POPUP_REBUILDABLE|KN_BUTTON:
        {
            if (CurrentObjects.Count() && CurrentObjects.Fetch_Head()->What_Am_I() == RTTI_BUILDING) {
                BuildingClass *building = ((BuildingClass *)CurrentObjects.Fetch_Head());

                if (building->Class->TechLevel != -1) {
                    building->IsToRebuild = (building->IsToRebuild == false);
                    building->Mark(MARK_CHANGE);
                }
                if (building->IsToRebuild) {
                    Rebuildable->Turn_On();
                } else {
                    Rebuildable->Turn_Off();
                }
            }
            break;
        }

        /**
         *    Object-Editing button: House Button
         */
        case POPUP_HOUSELIST|KN_BUTTON:
        {
            /**
             *  Determine the house desired by examining the currently
             *  selected index in the house list gadget.
             */
            HousesType house = HousesType(((ListClass *)Buttons->Extract_Gadget(POPUP_HOUSELIST))->Current_Index());

            /**
             *  If that house doesn't own this object, try to transfer it
             */
            if (CurrentObjects.Count() && CurrentObjects.Fetch_Head()->Owner() != house) {
                if (Change_House(house)) {
                    IsMapChanged = true;
                }
            }

            HiddenSurface->Clear();
            Buttons->Flag_List_To_Redraw();
            Flag_To_Redraw(true);
            input = KN_NONE;
            break;
        }

        /**
         *  Object-Editing button: Mission
         */
        case POPUP_MISSIONLIST|KN_BUTTON:
        {
            if (CurrentObjects.Count() && CurrentObjects.Fetch_Head()->Is_Techno()) {

                TechnoClass *techno = ((TechnoClass *)CurrentObjects.Fetch_Head());

                /**
                 *  Set new mission.
                 */
                MissionType mission = MapEditMissions[MissionList->Current_Index()];
                if (techno->Get_Mission() != mission) {
                    techno->Set_Mission(mission);
                    IsMapChanged = true;
                    Buttons->Flag_List_To_Redraw();
                    Flag_To_Redraw(true);
                }
            }
            input = KN_NONE;
            break;
        }

        /**
         *    Object-Editing Button: Health
         */
        case POPUP_HEALTHGAUGE|KN_BUTTON:
        {
            if (CurrentObjects.Count() && CurrentObjects.Fetch_Head()->Is_Techno()) {

                TechnoClass *techno = ((TechnoClass *)CurrentObjects.Fetch_Head());

                /**
                 *  Derive strength from current gauge reading.
                 */
                int strength = (techno->Class_Of()->MaxStrength * HealthGauge->Get_Value()) / 256;

                /**
                 *  Clip to 1
                 */
                if (strength <= 0) {
                    strength = 1;
                }

                /**
                 *  Set new strength.
                 */
                if (strength != techno->Strength) {
                    techno->Strength = strength;
                    HiddenSurface->Clear();
                    Flag_To_Redraw(true);
                    IsMapChanged = true;
                }

                /**
                 *  Update text label.
                 */
                std::snprintf(HealthBuf, sizeof(HealthBuf), "%d", strength);
            }
            input = KN_NONE;
            break;
        }

        /**
         *  Object-Editing Button: Facing
         */
        case POPUP_FACINGDIAL|KN_BUTTON:
        {
            if (CurrentObjects.Count() && CurrentObjects.Fetch_Head()->Is_Techno()) {

                TechnoClass *techno = ((TechnoClass *)CurrentObjects.Fetch_Head());

                /**
                 *  Set new facing.
                 */
                if (FacingDial->Get_Direction() != techno->PrimaryFacing.Current().Get_Dir()) {

                    /**
                     *  Adjust the facing to the isometric projection.
                     */
                    DirType adj_facing = FacingDial->Get_Direction();
                    adj_facing -= 16;

                    /**
                     *  Set body's facing.
                     */
                    techno->PrimaryFacing.Set(DirStruct(adj_facing));

                    /**
                     *  Set turret facing, if there is one.
                     */
                    if (techno->Techno_Type_Class()->IsTurretEquipped) {
                        techno->SecondaryFacing.Set(DirStruct(adj_facing));
                    }

                    HiddenSurface->Clear();
                    Flag_To_Redraw(true);
                    IsMapChanged = true;
                }
            }
            input = KN_NONE;
            break;
        }

        /**
         *  Object-Editing Button: Facing
         */
        case POPUP_BASEPERCENT|KN_BUTTON:
            if (BaseGauge->Get_Value() != Scen->Percent) {
                Scen->Percent = BaseGauge->Get_Value();
                Build_Base_To(Scen->Percent);
                HiddenSurface->Clear();
                Flag_To_Redraw(true);
            }
            input = KN_NONE;
            break;

        default:
            break;

    };
}


void MapEditClass::_Draw_It(bool forced)
{
    MouseClass::Draw_It(forced);

    if (Debug_Map) {
        if (forced) {
        }
    }
}


void MapEditClass::_Detach(TARGET target, bool all)
{
    MouseClass::Detach(target, all);

    if (Debug_Map) {
        if (GrabbedObject == target) {
            GrabbedObject = nullptr;
        }
    }
}


bool MapEditClass::_Scroll_Map(FacingType facing, int &distance, bool really)
{
    if (Debug_Map) {

        /**
         *  The popup gadgets require the entire map to be redrawn if we scroll.
         */
        if (really) {
            Flag_To_Redraw(true);
            Render();
        }
    }

    return MouseClass::Scroll_Map(facing, distance, really);
}


bool MapEditClass::New_Scenario()
{
#if 0
    int scen_num;
    ScenarioPlayerType player;
    ScenarioDirType dir;
    ScenarioVarType var;
    Disect_Scenario_Name(Scen.ScenarioName, scen_num, player, dir, var);

    int rc;
    HousesType house;

    /*
    **	Force the house save value to match the player house.
    */
    if (PlayerPtr) {
        switch (PlayerPtr->Class->House) {
            case HOUSE_SPAIN:
                player = SCEN_PLAYER_SPAIN;
                break;

            case HOUSE_GREECE:
                player = SCEN_PLAYER_GREECE;
                break;

            default:
            case HOUSE_USSR:
                player = SCEN_PLAYER_USSR;
                break;
        }
    }

    /*
    **	Prompt for scenario info
    */
    rc = Pick_Scenario("New Scenario", scen_num, player, dir, var);
    if (rc != 0) {
        return false;
    }

    ScenarioInit++;

    /*
    **	Blow away everything
    */
    Clear_Scenario();

    /*
    **	Set parameters
    */
//	Scen.Scenario = scen_num;
//	Scen.ScenPlayer = player;
//	Scen.ScenDir = dir;
//	Scen.ScenVar = var;
    Scen.Set_Scenario_Name(scen_num, player, dir, var);

    /*
    **	Create houses
    */
    for (house = HOUSE_FIRST; house < HOUSE_COUNT; house++) {
        new HouseClass(house);
    }

    switch (player) {
        case SCEN_PLAYER_MPLAYER:
            PlayerPtr = HouseClass::As_Pointer(HOUSE_MULTI1);
            PlayerPtr->IsHuman = true;
            LastHouse = HOUSE_MULTI1;
            break;

        case SCEN_PLAYER_USSR:
            PlayerPtr = HouseClass::As_Pointer(HOUSE_USSR);
            PlayerPtr->IsHuman = true;
            Base.House = HOUSE_SPAIN;
            LastHouse = HOUSE_GOOD;
            break;

        case SCEN_PLAYER_SPAIN:
            PlayerPtr = HouseClass::As_Pointer(HOUSE_SPAIN);
            PlayerPtr->IsHuman = true;
            Base.House = HOUSE_USSR;
            LastHouse = HOUSE_GOOD;
            break;

        case SCEN_PLAYER_GREECE:
            PlayerPtr = HouseClass::As_Pointer(HOUSE_GREECE);
            PlayerPtr->IsHuman = true;
            Base.House = HOUSE_USSR;
            LastHouse = HOUSE_GOOD;
            break;
    }

    /*
    **	Init the entire map
    */
    Fill_In_Data();

    /*
    **	Prompt for map size
    */
    Size_Map(-1, -1, 30, 30);

    /*
    **	Set the Home & Reinforcement Cells to the center of the map
    */
    Scen.Waypoint[WAYPT_REINF] = XY_Cell(MapCellX + MapCellWidth / 2, MapCellY + MapCellHeight / 2);
    Scen.Waypoint[WAYPT_HOME] = XY_Cell(MapCellX + MapCellWidth / 2, MapCellY + MapCellHeight / 2);
    (*this)[TacticalCoord].IsWaypoint = 1;
    Flag_Cell(Coord_Cell(TacticalCoord));

    Set_Tactical_Position(Cell_Coord(Scen.Waypoint[WAYPT_HOME] - (MAP_CELL_W * 4 * RESFACTOR) - (5 * RESFACTOR)));
#endif

    ScenarioInit--;

    return true;
}


bool MapEditClass::Load_Scenario(const char *filename)
{
    return false;
}


/**
 *  Saves current scenario to an INI file.
 */
bool MapEditClass::Save_Scenario(bool emergency)
{
    RawFileClass file(Scen->ScenarioName);

    if (file.Is_Available()) {
        file.Close();
        int rc = WWMessageBox().Process("Scenario file already exists. Replace?", 0, Text_String(TXT_YES), Text_String(TXT_NO));
        HiddenSurface->Clear();
        Map.Flag_To_Redraw(true);
        Map.Render();
        if (rc == 1) {
            return false;
        }
    }

    char buffer[128];
    if (emergency) {
        std::snprintf(buffer, sizeof(buffer), "RECOVERED_%s", Scen->ScenarioName);
    } else {
        std::snprintf(buffer, sizeof(buffer), "%s", Scen->ScenarioName);
    }

    Write_Scenario_INI(buffer, true);

    return true;
}


bool MapEditClass::Size_Map(int x, int y, int w, int h)
{
    return false;
}


/**
 *  Checks for mouse motion.
 */
bool MapEditClass::Mouse_Moved()
{
    static int old_mx = 0;
    static int old_my = 0;
    static Cell old_zonecell = 0;
    ObjectTypeClass *objtype = nullptr;
    bool retcode = false;

    /**
     *  Return if no motion.
     */
    if (old_mx == WWMouse->Get_Mouse_X() && old_my == WWMouse->Get_Mouse_Y()) {
        return false;
    }

    /**
     *  Get a ptr to ObjectTypeClass.
     */
    if (PendingObject) {
        objtype = PendingObject;

    } else {
        if (GrabbedObject) {
            objtype = GrabbedObject->Class_Of();
        } else {
            old_mx = WWMouse->Get_Mouse_X();
            old_my = WWMouse->Get_Mouse_Y();
            old_zonecell = ZoneCell;
            return false;
        }
    }

    /**
     *  Infantry: Mouse moved if any motion at all.
     */
    if (objtype->What_Am_I() == RTTI_INFANTRYTYPE) {
        retcode = true;

    } else {
        /**
         *  Other types: Mouse moved only if cell changed.
         */
        if (old_zonecell!=ZoneCell) {
            retcode = true;
        } else {
            retcode = false;
        }
    }

    old_mx = WWMouse->Get_Mouse_X();
    old_my = WWMouse->Get_Mouse_Y();
    old_zonecell = ZoneCell;

    return retcode;
}


/**
 *  Moves the grabbed object.
 */
bool MapEditClass::Move_Grabbed_Object()
{
    Coordinate new_coord = 0;
    int retval = false;

    /**
     *  Lift up the object.
     */
    GrabbedObject->Mark(MARK_UP);

    /*------------------------------------------------------------------------
    If infantry, use a free spot in this cell
    ------------------------------------------------------------------------*/
    if (GrabbedObject->Is_Infantry()) {

        Coordinate mouse_coord = Get_Coord_Under_Mouse();
        if (Is_Spot_Free(mouse_coord)) {
            new_coord = Closest_Free_Spot(mouse_coord);
            /*..................................................................
            Clear the occupied bit in this infantry's cell.
            ..................................................................*/
            ((InfantryClass *)GrabbedObject)->Clear_Occupy_Bit(GrabbedObject->Coord);
        } else {
            new_coord = Coordinate(0,0,0);
        }

    } else {

        /*------------------------------------------------------------------------
        Non-infantry: use cell's center coordinate
        ------------------------------------------------------------------------*/
        new_coord = Cell_Coord(ZoneCell + GrabOffset);

        if (GrabbedObject->What_Am_I() == RTTI_BUILDING ||
            GrabbedObject->What_Am_I() == RTTI_TERRAIN) {

            new_coord = Coord_Snap(new_coord);
        }

        /*
        ................ Try to place object at new coordinate ................
        */
        if (GrabbedObject->Can_Enter_Cell(&(*this)[new_coord]) != MOVE_OK) {
            new_coord = Coordinate(0,0,0);
        }
    }
    if (new_coord) {

        /**
         *  
         */
        new_coord.Z = Map.Get_Cell_Height(new_coord);

        /*
        ** If this object is part of the AI's Base list, change the coordinate
        ** in the Base's Node list.
        */
        if (GrabbedObject->What_Am_I() == RTTI_BUILDING) {
            BuildingClass *grabbed_building = (BuildingClass *)GrabbedObject;
            if (grabbed_building->House->Base.Get_Node(grabbed_building)) {
                grabbed_building->House->Base.Get_Node(grabbed_building)->Where = Coord_Cell(new_coord);
            }
        }

        /*------------------------------------------------------------------------
        For infantry, set the bit in its new cell marking that spot as occupied.
        ------------------------------------------------------------------------*/
        if (GrabbedObject->Is_Infantry()) {
            ((InfantryClass *)GrabbedObject)->Set_Occupy_Bit(new_coord);
        }

        GrabbedObject->Coord = new_coord;
        retval = true;
    }
    GrabbedObject->Mark(MARK_DOWN);

    /*------------------------------------------------------------------------
    Re-select the object, and reset the mouse pointer
    ------------------------------------------------------------------------*/
    Set_Default_Mouse(MOUSE_NORMAL);
    Override_Mouse_Shape(MOUSE_NORMAL);

    Flag_To_Redraw(true);

    return retval;
}


/**
 *  Grabs the current object.
 */
void MapEditClass::Grab_Object()
{
    Cell cell;

    if (CurrentObjects.Count()) {
        GrabbedObject = CurrentObjects.Fetch_Head();

        /**
         *  Find out which cell 'ZoneCell' is in relation to the object's current cell.
         */
        cell = Coord_Cell(GrabbedObject->Coord);
        GrabOffset = cell - ZoneCell;
    }
}


/**
 *  Attempts to place the current object.
 */
bool MapEditClass::Place_Object()
{
    Cell template_cell;                        // cell being checked for template
    Coordinate obj_coord;                            // coord of occupier object
    int okflag;                                    // OK to place a template?
    short const * occupy;                        // ptr into template's OccupyList
    ObjectClass * occupier;                    // occupying object
//    TemplateType save_ttype;                // for saving cell's TType
    unsigned char save_ticon;                // for saving cell's TIcon
//    BaseNodeClass node;                        // for adding to an AI Base

#if 0
    /*
     *  Placing a template:
     *  - first lift up any objects in the cell
     *  - place the template, and try to replace the objects; if they won't go
     *    back, the template can't go there
    */
    //ScenarioInit++;

    if (PendingObject->What_Am_I() == RTTI_TEMPLATETYPE) {

        /*
         *  Loop through all cells this template will occupy
        */
        okflag = true;
        occupy = PendingObject->Occupy_List();
        while ((*occupy) != REFRESH_EOL) {

            /*
             *  Check this cell for an occupier
            */
            template_cell = (ZoneCell+ZoneOffset) + (*occupy);
            if ((*this)[template_cell].Cell_Occupier()) {
                occupier = (*this)[template_cell].Cell_Occupier();

                /*
                 *  Save object's coordinates
                */
                obj_coord = occupier->Coord;

                /*
                 *  Place the object in limbo
                */
                occupier->Mark(MARK_UP);

                /*
                 *  Set the cell's template values
                */
                save_ttype = (*this)[template_cell].TType;
                save_ticon = (*this)[template_cell].TIcon;
                (*this)[template_cell].TType =
                    ((TemplateTypeClass *)PendingObject)->Type;
                (*this)[template_cell].TIcon = Cell_X(*occupy) + Cell_Y(*occupy) *
                    ((TemplateTypeClass *)PendingObject)->Width;
                (*this)[template_cell].Recalc_Attributes();

                /*
                 *  Try to put the object back down
                */
                if (occupier->Can_Enter_Cell(Coord_Cell(obj_coord)) != MOVE_OK) {
                    okflag = false;
                }

                /*
                 *  Put everything back the way it was
                */
                (*this)[template_cell].TType = save_ttype;
                (*this)[template_cell].TIcon = save_ticon;
                (*this)[template_cell].Recalc_Attributes();

                /*
                 *  Major error if can't replace the object now
                */
                occupier->Mark(MARK_DOWN);
            }
            occupy++;
        }

        /*
         *  If it's still OK after ALL THAT, place the template
        */
        if (okflag) {
            if (PendingObjectPtr->Unlimbo(Cell_Coord(ZoneCell + ZoneOffset))) {

                /*
                 *  Loop through all cells occupied by this template, and clear the
                 *  smudge & overlay.
                */
                occupy = PendingObject->Occupy_List();
                while ((*occupy) != REFRESH_EOL) {

                    /*
                     *  Get cell for this occupy item
                    */
                    template_cell = (ZoneCell+ZoneOffset) + (*occupy);

                    /*
                     *  Clear smudge & overlay
                    */
                    (*this)[template_cell].Overlay = OVERLAY_NONE;
                    (*this)[template_cell].OverlayData = 0;
                    (*this)[template_cell].Smudge = SMUDGE_NONE;

                    /*
                     *  make adjacent cells recalc attrib's
                    */
                    (*this)[template_cell].Recalc_Attributes();
                    (*this)[template_cell].Wall_Update();
                    (*this)[template_cell].Concrete_Calc();

                    occupy++;
                }

                /*
                 *  Set flags etc
                */
                PendingObjectPtr = 0;
                PendingObject = 0;
                PendingHouse = HOUSE_NONE;
                Set_Cursor_Shape(0);
                //ScenarioInit--;
                TotalValue = Overpass();
                Flag_To_Redraw(false);
                return true;
            }

            /*
             *  Failure to deploy results in a returned failure code.
            */
            //ScenarioInit--;
            return false;
        }

        /*
         *  Not OK; return error
        */
        //ScenarioInit--;
        return false;
    }
#endif

    /*
     *  Placing infantry: Infantry can go into cell sub-positions, so find the
     *  sub-position closest to the mouse & put him there
    */
    if (PendingObject->What_Am_I() == RTTI_INFANTRYTYPE) {

#if 0
        /*
         *  Find cell sub-position
        */
        if (Is_Spot_Free(Pixel_To_Coord(Get_Mouse_X(), Get_Mouse_Y()))) {
            obj_coord = Closest_Free_Spot(Pixel_To_Coord(Get_Mouse_X(), Get_Mouse_Y()));
        } else {
            obj_coord = Coordinate(0,0,0);
        }
#endif

        /*
         *  No free spots; don't place the object
        */
        if (!obj_coord) {
            //ScenarioInit--;
            return false;
        }

        /*
         *  Unlimbo the object
        */
        if (PendingObjectPtr->Unlimbo(obj_coord)) {
            ((InfantryClass *)PendingObjectPtr)->Set_Occupy_Bit(obj_coord);
            PendingObjectPtr = nullptr;
            PendingObject = nullptr;
            PendingHouse = HOUSE_NONE;
            Set_Cursor_Shape(nullptr);
            //ScenarioInit--;
            return true;
        }

        //ScenarioInit--;
        return false;
    }

    /*
     *  Placing an object
    */
    if (PendingObjectPtr->Unlimbo(Cell_Coord(ZoneCell + ZoneOffset))) {

        /*
        ** Update the Tiberium computation if we're placing an overlay
        */
        if (PendingObject->What_Am_I() == RTTI_OVERLAYTYPE &&
            ((OverlayTypeClass *)PendingObject)->IsTiberium) {
            TotalValue = Overpass();
            Flag_To_Redraw(false);
        }

#if 0
        /*
        ** If we're building a base, add this building to the base's Node list.
        */
        if (IsBaseBuilding && PendingObject->What_Am_I() == RTTI_BUILDINGTYPE) {
            Base.Nodes.Add(BaseNodeClass(((BuildingTypeClass *)PendingObject)->Type, Coord_Cell(PendingObjectPtr->Coord)));
        }
#endif

        PendingObjectPtr = nullptr;
        PendingObject = nullptr;
        PendingHouse = HOUSE_NONE;
        Set_Cursor_Shape(nullptr);
        //ScenarioInit--;
        return true;
    }

    return false;
}


/**
 *  Homes the placement object.
 */
void MapEditClass::Place_Home()
{
    delete PendingObjectPtr;
    PendingObjectPtr = nullptr;
    PendingObject = nullptr;

    /*
    ** Don't allow this command if we're building a base; the only valid
    ** category for base-building is buildings.
    */
    if (IsBaseBuilding) {
        return;
    }

    /*
    ------------------ Loop until we create a valid object -------------------
    */
    LastChoice = 0;
    while (!PendingObjectPtr) {
        /*
        ................... Get house for this object type ....................
        */
        if (!Verify_House(LastHouse,AvailableObjects[LastChoice])) {
            LastHouse = Cycle_House(LastHouse,AvailableObjects[LastChoice]);
        }

        /*
        ....................... Create placement object .......................
        */
        PendingObject = (ObjectTypeClass *)AvailableObjects[LastChoice];
        PendingHouse = LastHouse;
        PendingObjectPtr = PendingObject->Create_One_Of(HouseClass::As_Pointer(PendingHouse));

        /*
        .................. If this one failed, try the next ...................
        */
        if (!PendingObjectPtr) {
            PendingObject = nullptr;
            LastChoice++;
            if (LastChoice == AvailableObjects.Count()) {
                LastChoice = 0;
            }
        }
    }

    /*
    ------------------------ Set the new cursor shape ------------------------
    */
    Set_Cursor_Pos();
    Set_Cursor_Shape(0);
    Set_Cursor_Shape(PendingObject->Occupy_List());

    /*
    ----------------- Redraw the map to erase old leftovers ------------------
    */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
    Render();
}


/**
 *  Enters placement mode.
 */
void MapEditClass::Start_Placement()
{
    /**
     *  Initialize addable objects list; we must do this every time in case one
     *  of the object pools has become exhausted; that object won't be available
     *  for adding. These must be added in the same order expected by the
     *  object selection dialog (same as button order).
     */
    Clear_List();
    //IsometricTileTypeClass::Prep_For_Add();
    //OverlayTypeClass::Prep_For_Add();
    //SmudgeTypeClass::Prep_For_Add();
    //TerrainTypeClass::Prep_For_Add();
    //UnitTypeClass::Prep_For_Add();
    //InfantryTypeClass::Prep_For_Add();
    //BuildingTypeClass::Prep_For_Add();
    //AircraftTypeClass::Prep_For_Add();

    /*
     *  Compute offset of each class type in the Objects array
    */
    TypeOffset[0] = 0;
    for (int i = 1; i < 8/*NUM_EDIT_CLASSES*/; i++) {
        TypeOffset[i] = TypeOffset[i-1] + NumType[i-1];
    }

    /*
     *  Create the placement object:
     *  - For normal placement mode, use the last-used index into Objects
     *    (LastChoice), and the last-used house (LastHouse).
     *  - For base-building mode, force the object to be a building, and use the
     *    House specified in the Base object
    */
    if (!IsBaseBuilding) {
        if (LastChoice >= AvailableObjects.Count()) {
            LastChoice = AvailableObjects.Count() - 1;
        }
        PendingObject = (ObjectTypeClass *)Objects[LastChoice];
        PendingHouse = LastHouse;
        PendingObjectPtr = PendingObject->Create_One_Of(HouseClass::As_Pointer(LastHouse));
    } else {
        if (LastChoice < TypeOffset[7]) {
            LastChoice = TypeOffset[7];
        }
        if (LastChoice >= AvailableObjects.Count()) {
            LastChoice = AvailableObjects.Count() - 1;
        }
        PendingObject = (ObjectTypeClass *)Objects[LastChoice];
//        PendingHouse = LastHouse = Base.House;
        PendingObjectPtr = PendingObject->Create_One_Of(HouseClass::As_Pointer(LastHouse));
    }

    /*
     *  Error if no more objects available
    */
    if (!PendingObjectPtr) {
        WWMessageBox().Process("No more objects of this type available.", 0);
        HiddenSurface->Clear();
        Flag_To_Redraw(true);
        Render();
        PendingObject = nullptr;
        if (IsBaseBuilding) {
            Cancel_Base_Building();
        }
        return;
    }

    /*
     *  Set the placement cursor
    */
    Set_Cursor_Pos();
    Set_Cursor_Shape(PendingObject->Occupy_List());
}


/**
 *  While placing object, goes to next.
 */
void MapEditClass::Place_Next()
{
    delete PendingObjectPtr;
    PendingObjectPtr = nullptr;
    PendingObject = nullptr;

    /*
    ------------------ Loop until we create a valid object -------------------
    */
    while (!PendingObjectPtr) {
        /*
        ................. Go to next object in Objects list ...................
        */
        LastChoice++;
        if (LastChoice == AvailableObjects.Count()) {
            /*
            ** If we're in normal placement mode, wrap to the 1st object;
            ** if we're in base-building mode, wrap to the 1st building
            */
            if (!IsBaseBuilding) {
                LastChoice = 0;
            } else {
                LastChoice = TypeOffset[7];
            }
        }

        /*
        ................... Get house for this object type ....................
        */
        if (!Verify_House(LastHouse,AvailableObjects[LastChoice])) {
            /*
            ** If we're in normal placement mode, change the current
            ** placement house to the one that can own this object.
            ** If we're building a base, skip ahead to the next object if the
            ** base's house can't own this one.
            */
            if (!IsBaseBuilding) {
                LastHouse = Cycle_House(LastHouse,AvailableObjects[LastChoice]);
            } else {
                continue;
            }
        }

        /*
        ....................... Create placement object .......................
        */
        PendingObject = (ObjectTypeClass *)AvailableObjects[LastChoice];
        PendingHouse = LastHouse;
        PendingObjectPtr = PendingObject->Create_One_Of(HouseClass::As_Pointer(PendingHouse));
        if (!PendingObjectPtr) {
            PendingObject = nullptr;
        }
    }

    /*
    ------------------------ Set the new cursor shape ------------------------
    */
    Set_Cursor_Pos();
    Set_Cursor_Shape(nullptr);
    Set_Cursor_Shape(PendingObject->Occupy_List());

    /*
    ----------------- Redraw the map to erase old leftovers ------------------
    */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
    Render();
}


/**
 *  While placing object, goes to previous.
 */
void MapEditClass::Place_Prev()
{
    delete PendingObjectPtr;
    PendingObjectPtr = nullptr;
    PendingObject = nullptr;

    /*
    ------------------ Loop until we create a valid object -------------------
    */
    while (!PendingObjectPtr) {

        /*
        ................. Go to prev object in Objects list ..................
        */
        LastChoice--;
        /*
        ** If we're in normal placement mode, wrap at the 1st object.
        ** If we're building a base, wrap at the 1st building.
        */
        if (!IsBaseBuilding) {
            if (LastChoice < 0) {
                LastChoice = AvailableObjects.Count() - 1;
            }
        } else {
            if (LastChoice < TypeOffset[7]) {
                LastChoice = AvailableObjects.Count() - 1;
            }
        }

        /*
        ................... Get house for this object type ....................
        */
        if (!Verify_House(LastHouse,AvailableObjects[LastChoice])) {
            /*
            ** If we're in normal placement mode, change the current
            ** placement house to the one that can own this object.
            ** If we're building a base, skip ahead to the next object if the
            ** base's house can't own this one.
            */
            if (!IsBaseBuilding) {
                LastHouse = Cycle_House(LastHouse,AvailableObjects[LastChoice]);
            } else {
                continue;
            }
        }

        /*
        ....................... Create placement object .......................
        */
        PendingObject = (ObjectTypeClass *)AvailableObjects[LastChoice];
        PendingHouse = LastHouse;
        PendingObjectPtr = PendingObject->Create_One_Of(HouseClass::As_Pointer(PendingHouse));
        if (!PendingObjectPtr) {
            PendingObject = nullptr;
        }
    }

    /*
    ------------------------ Set the new cursor shape ------------------------
    */
    Set_Cursor_Pos();
    Set_Cursor_Shape(nullptr);
    Set_Cursor_Shape(PendingObject->Occupy_List());

    /*
    ----------------- Redraw the map to erase old leftovers ------------------
    */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
    Render();
}


/**
 *  Places next category of object.
 */
void MapEditClass::Place_Next_Category()
{
    int i;

    /*
    ** Don't allow this command if we're building a base; the only valid
    ** category for base-building is buildings.
    */
    if (IsBaseBuilding) {
        return;
    }

    delete PendingObjectPtr;
    PendingObjectPtr = nullptr;
    PendingObject = nullptr;

    /*
    ------------------ Go to next category in Objects list -------------------
    */
    i = LastChoice;
    while (Objects[i]->What_Am_I() == Objects[LastChoice]->What_Am_I()) {
        i++;
        if (i == AvailableObjects.Count()) {
            i = 0;
        }
    }
    LastChoice = i;

    /*
    ------------------ Loop until we create a valid object -------------------
    */
    while (!PendingObjectPtr) {

        /*
        ................... Get house for this object type ....................
        */
        if (!Verify_House(LastHouse,AvailableObjects[LastChoice])) {
            LastHouse = Cycle_House(LastHouse,AvailableObjects[LastChoice]);
        }

        /*
        ....................... Create placement object .......................
        */
        PendingObject = (ObjectTypeClass *)Objects[LastChoice];
        PendingHouse = LastHouse;
        PendingObjectPtr = PendingObject->Create_One_Of(HouseClass::As_Pointer(PendingHouse));

        /*
        .................. If this one failed, try the next ...................
        */
        if (!PendingObjectPtr) {
            PendingObject = nullptr;
            LastChoice++;
            if (LastChoice == AvailableObjects.Count()) {
                LastChoice = 0;
            }
        }
    }

    /*
    ------------------------ Set the new cursor shape ------------------------
    */
    Set_Cursor_Pos();
    Set_Cursor_Shape(nullptr);
    Set_Cursor_Shape(PendingObject->Occupy_List());

    /*
    ----------------- Redraw the map to erase old leftovers ------------------
    */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
    Render();
}


/**
 *  Places previous category of object.
 */
void MapEditClass::Place_Prev_Category()
{
    int i;

    /*
    ** Don't allow this command if we're building a base; the only valid
    ** category for base-building is buildings.
    */
    if (IsBaseBuilding) {
        return;
    }

    delete PendingObjectPtr;
    PendingObjectPtr = nullptr;
    PendingObject = nullptr;

    /*
    ------------------ Go to prev category in Objects list -------------------
    */
    i = LastChoice;
    /*
    ..................... Scan for the previous category .....................
    */
    while (Objects[i]->What_Am_I() == Objects[LastChoice]->What_Am_I()) {
        i--;
        if (i < 0) {
            i = AvailableObjects.Count() - 1;
        }
    }
    /*
    .................... Scan for start of this category .....................
    */
    LastChoice = i;
    while (Objects[i]->What_Am_I() == Objects[LastChoice]->What_Am_I()) {
        LastChoice = i;
        i--;
        if (i < 0) {
            i = AvailableObjects.Count() - 1;
        }
    }

    /*
    ------------------ Loop until we create a valid object -------------------
    */
    while (!PendingObjectPtr) {
        /*
        ................... Get house for this object type ....................
        */
        if (!Verify_House(LastHouse,AvailableObjects[LastChoice])) {
            LastHouse = Cycle_House(LastHouse,AvailableObjects[LastChoice]);
        }

        /*
        ....................... Create placement object .......................
        */
        PendingObject = (ObjectTypeClass *)Objects[LastChoice];
        PendingHouse = LastHouse;
        PendingObjectPtr = PendingObject->Create_One_Of(HouseClass::As_Pointer(PendingHouse));

        /*
        .................. If this one failed, try the next ...................
        */
        if (!PendingObjectPtr) {
            PendingObject = nullptr;
            LastChoice--;
            if (LastChoice < 0) {
                LastChoice = AvailableObjects.Count() - 1;
            }
        }
    }

    /*
    ------------------------ Set the new cursor shape ------------------------
    */
    Set_Cursor_Pos();
    Set_Cursor_Shape(nullptr);
    Set_Cursor_Shape(PendingObject->Occupy_List());

    /*
    ----------------- Redraw the map to erase old leftovers ------------------
    */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
    Render();
}


void MapEditClass::Start_Tag_Placement()
{
    Set_Default_Mouse(MOUSE_CAN_MOVE);
    Override_Mouse_Shape(MOUSE_CAN_MOVE);
}


void MapEditClass::Stop_Tag_Placement()
{
    CurTag = nullptr;

    Set_Default_Mouse(MOUSE_NORMAL);
    Override_Mouse_Shape(MOUSE_NORMAL);
}



/***************************************************************************
 * MapEditClass::Place_Trigger -- assigns trigger to object or cell        *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      none.                                                              *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   12/01/1994 BR : Created.                                              *
 *=========================================================================*/
void MapEditClass::Place_Tag()
{
    ObjectClass * object = nullptr; // Generic object clicked on.
    int x,y;
    Cell cell; // Cell that was selected.

    /*
    **	See if an object was clicked on
    */
    x = WWKeyboard->MouseQX;
    y = WWKeyboard->MouseQY;

    /*
    **	Get cell for x,y
    */
    cell = Click_Cell_Calc(Point2D(x, y));

    /*
    **	Convert x,y to offset from cell upper-left
    */
    x = (x-TacticalRect.X) % CELL_PIXEL_W;
    y = (y-TacticalRect.Y) % CELL_PIXEL_H;

    /*
    **	Get object at that x,y
    */
    object = Cell_Object(cell, Point2D(x, y));

#if 0
    /*
    **	Assign trigger to an object
    */
    AttachType a1 = CurTag->Attaches_To();
    if (object && (a1 & ATTACH_OBJECT) != 0) {
        if (CurTag) {
            TagClass * ct = TagTypeClass::Find_Or_Make(CurTag);
            if (ct) {
                object->Tag = ct;
            }
        }
    } else {

        /*
        **	Assign trigger to a cell
        */
        if ((a1 & ATTACH_CELL) != 0) {
            if (CurTag) {
                TagClass * ct = TagTypeClass::Find_Or_Make(CurTag);
                Map[cell].CellTag = ct;
            }
//			CellTriggers[cell] = CurTag;
        }
    }
#endif

    /*
    **	Force map to redraw
    */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
}


void MapEditClass::Cancel_Placement(void)
{
    /*
    **	Delete the placement object
    */
    delete PendingObjectPtr;
    PendingObject = 0;
    PendingObjectPtr = 0;
    PendingHouse = HOUSE_NONE;

    /*
    **	Restore cursor shape
    */
    Set_Cursor_Shape(0);

    /*
    **	Redraw the map to erase old leftovers
    */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
    Render();
}


void MapEditClass::Cancel_Base_Building(void)
{
    /*
    ** Build the base to the proper amount
    */
    Build_Base_To(Scen->Percent);

    /*
    ** Cancel placement mode
    */
    Cancel_Placement();
    IsBaseBuilding = false;

    /*
    ** Force map to redraw
    */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
}


void MapEditClass::Build_Base_To(int percent)
{
    int i;
    int num_buildings;
    BuildingTypeClass const * objtype;
    BuildingClass * obj;

    //ScenarioInit++;

#if 0

    /*
    ** Completely dismantle the base, so we start at a known point
    */
    for (i = 0; i < Base.Nodes.Count(); i++) {
        if (Base.Is_Built(i)) {
            obj = Base.Get_Building(i);
            delete obj;
        }
    }

    /*
    ** Compute number of buildings to build
    */
    num_buildings = (Base.Nodes.Count() * percent) / 100;

    /*
    ** Build the base to the desired amount
    */
    for (i = 0; i < num_buildings; i++) {
        /*
        ** Get a ptr to the type of building to build, create one, and unlimbo it.
        */
        objtype = &BuildingTypeClass::As_Reference(Base.Nodes[i].Type);
        obj = (BuildingClass *)objtype->Create_One_Of(HouseClass::As_Pointer(Base.House));

        /*
        ** If unlimbo fails, error out
        */
        ScenarioInit++;
        if (!obj->Unlimbo(Cell_Coord(Base.Nodes[i].Cell))) {
            delete obj;
            WWMessageBox().Process("Unable to build base!");
            ScenarioInit--;
            return;
        }
        ScenarioInit--;
    }

    //ScenarioInit--;
#endif
}



bool MapEditClass::Select_Object()
{
    ObjectClass *object = nullptr; // Generic object clicked on.
    int x,y;
    Cell cell; // Cell that was selected.
    int rc = true;

    /*
    **    See if an object was clicked on
    */
    x = WWKeyboard->MouseQX;
    y = WWKeyboard->MouseQY;

    /*
    **    Get cell for x,y
    */
    cell = Click_Cell_Calc(Point2D(x,y));

    /*
    **    Convert x,y to offset from cell upper-left
    */
    x = (x-TacticalRect.X) % CELL_PIXEL_W;
    y = (y-TacticalRect.Y) % CELL_PIXEL_H;

    /*
    **    Get object at that x,y
    */
    object = Cell_Object(cell, Point2D(x,y));

    /*
    **    If no object, unselect the current one
    */
    if (!object) {
        if (CurrentObjects.Count()) {

            /*
            **    Unselect all current objects
            */
            Unselect_All();

            /*
            **    Turn off object controls
            */
            Popup_Controls();
        }
        rc = false;
    } else {

        /*
        **    Select object only if it's different
        */
        if (!CurrentObjects.Count() || (CurrentObjects.Count() && object != CurrentObjects.Fetch_Head())) {

            /*
            **    Unselect all current objects
            */
            Unselect_All();
            object->Select();

            /*
            **    Set mouse shape back to normal
            */
            Set_Default_Mouse(MOUSE_NORMAL);
            Override_Mouse_Shape(MOUSE_NORMAL);

            /*
            **    Show the popup controls
            */
            Popup_Controls();
        }
    }

    /*
    **    Force map to redraw
    */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);

    return rc;
}


/**
 *  Selects next object on the map.
 */
void MapEditClass::Select_Next()
{
#if 0
    ObjectClass * obj;
    CELL obj_cell;
    int smap_w; // screen map width in icons
    int smap_h; // screen map height in icons
    int cell_x; // cell-x of next object
    int cell_y; // cell-y of next object
    int tcell_x; // cell-x of TacticalCell
    int tcell_y; // cell-y of TacticalCell

    /*
    ----------------------- Get next object on the map -----------------------
    */
    obj = Next_Object(CurrentObjects.Fetch_Head());

    if (obj) {
        /*
        ............... Unselect current object if there is one ...............
        */
        Unselect_All();

         /*
        ......................... Select this object ..........................
        */
        obj->Select();
    }

    /*
    --------------------- Restore mouse shape to normal ----------------------
    */
    Set_Default_Mouse(MOUSE_NORMAL);
    Override_Mouse_Shape(MOUSE_NORMAL);

    /*
    -------------------------- Show pop-up controls --------------------------
    */
    Popup_Controls();

    /*
    ---------------- Make sure object is shown on the screen -----------------
    */
    /*
    ..................... compute screen map dimensions ......................
    */
    smap_w = Lepton_To_Cell(TacLeptonWidth);
    smap_h = Lepton_To_Cell(TacLeptonHeight);

    /*
    ...................... compute x,y of object's cell ......................
    */
    obj_cell = Coord_Cell(CurrentObjects.Fetch_Head()->Coord);
    cell_x = Cell_X(obj_cell);
    cell_y = Cell_Y(obj_cell);
    tcell_x = Coord_XCell(TacticalCoord);
    tcell_y = Coord_YCell(TacticalCoord);

    /*
    ................... If object is off-screen, move map ....................
    */
    if (cell_x < tcell_x) {
        tcell_x = cell_x;
    } else {
        if (cell_x >= tcell_x + smap_w) {
            tcell_x = cell_x - smap_w + 1;
        }
    }

    if (cell_y < tcell_y) {
        tcell_y = cell_y;
    } else {
        if (cell_y >= tcell_y + smap_h) {
            tcell_y = cell_y - smap_h + 1;
        }
    }

    ScenarioInit++;
    Set_Tactical_Position(XY_Coord(Cell_To_Lepton(tcell_x), Cell_To_Lepton(tcell_y)));
    ScenarioInit--;
#endif

    /**
     *  Force map to redraw.
     */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
}


/**
 *  Selects previous object on the map.
 */
void MapEditClass::Select_Prev()
{

    /**
     *  Force map to redraw.
     */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
}


/**
 *  Is this objtype ownable by this house?
 */
bool MapEditClass::Verify_House(HousesType house, const ObjectTypeClass *objtype)
{
    /**
     *  Verify that new house can own this object.
     */
    return (objtype->Get_Ownable() & (1 << house)) != 0;
}


void MapEditClass::Toggle_House()
{
    TechnoClass *tp;

    /*
    ** Don't allow this command if we're building a base; the only valid
    ** house for base-building is the one assigned to the base.
    */
    if (IsBaseBuilding) {
        return;
    }

    /*------------------------------------------------------------------------
    Only techno objects can be owned by a house; return if not a techno
    ------------------------------------------------------------------------*/
    if (!PendingObjectPtr->Is_Techno()) {
        return;
    }

    /*------------------------------------------------------------------------
    Select the house that will own this object
    ------------------------------------------------------------------------*/
    LastHouse = Cycle_House((HousesType)PendingObjectPtr->Owner(), PendingObject);

    /*------------------------------------------------------------------------
    Change the house
    ------------------------------------------------------------------------*/
    tp = (TechnoClass *)PendingObjectPtr;
    tp->House = HouseClass::As_Pointer(LastHouse);

    /*------------------------------------------------------------------------
    Set house variables to new house
    ------------------------------------------------------------------------*/
    PendingHouse = LastHouse;
}


/**
 *  Changes CurrentObject's house.
 */
bool MapEditClass::Change_House(HousesType newhouse)
{
    /*
    **	Return if no current object
    */
    if (!CurrentObjects.Count()) {
        return false;
    }

    /*
    **	Only techno objects can be owned by a house; return if not a techno
    */
    if (!CurrentObjects.Fetch_Head()->Is_Techno()) {
        return false;
    }

    TechnoClass *current_techno = (TechnoClass *)CurrentObjects.Fetch_Head();

    /*
    **	You can't change the house if the object is part of the AI's Base.
    */
    if (current_techno->What_Am_I() == RTTI_BUILDING && current_techno->House->Base.Is_Node((BuildingClass *)current_techno)) {
        return false;
    }

    /*
    **	Verify that the target house exists
    */
    if (HouseClass::As_Pointer(newhouse) == nullptr) {
        return false;
    }

    /*
    **	Verify that this is a valid owner
    */
    if (!Verify_House(newhouse, current_techno->Class_Of())) {
        return false;
    }

    /*
    **	Change the house
    */
    TechnoClass *tp = (TechnoClass *)CurrentObjects.Fetch_Head();
    tp->House = HouseClass::As_Pointer(newhouse);

    tp->IsOwnedByPlayer = false;
    if (tp->House == PlayerPtr) {
        tp->IsOwnedByPlayer = true;
    }

    return true;
}


/**
 *  Finds next valid house for object type.
 */
HousesType MapEditClass::Cycle_House(HousesType curhouse, const ObjectTypeClass *objtype)
{
    HousesType count; // prevents an infinite loop.

    /*
    **    Loop through all house types, starting with the one after 'curhouse';
    **    return the first one that's valid
    */
    count = HOUSE_NONE;
    while (true) {

        /*
        **    Go to next house
        */
        curhouse++;
        if (curhouse == HouseTypes.Count()) {
            curhouse = HOUSE_FIRST;
        }

        /*
        **    Count # iterations; don't go forever
        */
        count++;
        if (count == HouseTypes.Count()) {
            curhouse = HOUSE_NONE;
            break;
        }

        /*
        **    Break if this is a valid house
        */
//        if (HouseClass::As_Pointer(curhouse) && Verify_House(curhouse, objtype)) {
            break;
//        }
    }

    return curhouse;
}


bool MapEditClass::Get_Waypoint_Name(char wayptname[])
{
    /*
    **	Dialog & button dimensions
    */
    enum {
        D_DIALOG_W = 100,											// dialog width
        D_DIALOG_H = 56,											// dialog height
        D_DIALOG_X = ((320 - D_DIALOG_W) / 2),				// centered x-coord
        D_DIALOG_Y = ((200 - D_DIALOG_H) / 2),				// centered y-coord
        D_DIALOG_CX = D_DIALOG_X + (D_DIALOG_W / 2),		// coord of x-center

        D_TXT8_H = 11,												// ht of 8-pt text
        D_MARGIN = 7,												// margin width/height

        D_EDIT_W = D_DIALOG_W - (D_MARGIN * 2),
        D_EDIT_H = 13,
        D_EDIT_X = D_DIALOG_X + D_MARGIN,
        D_EDIT_Y = D_DIALOG_Y + 20,

        D_BUTTON_X = D_DIALOG_X + D_MARGIN,
        D_BUTTON_Y = D_DIALOG_Y + 40,
        D_BUTTON_W = 40,
        D_BUTTON_H = 13,

        D_CANCEL_X = D_DIALOG_X + 53,
        D_CANCEL_Y = D_DIALOG_Y + 40,
        D_CANCEL_W = 40,
        D_CANCEL_H = 13,

    };

    /*
    **	Button enumerations
    */
    enum {
        BUTTON_OK = 100,
        BUTTON_CANCEL,
        BUTTON_EDIT,
    };

    /*
    **	Dialog variables
    */
    bool cancel = false; // true = user cancels
    wayptname[0] = '\0';

#if 0
    /*
    **	Buttons
    */
    ControlClass * commands = nullptr;		// the button list

    TextButtonClass button (BUTTON_OK, TXT_OK, TPF_EBUTTON, D_BUTTON_X, D_BUTTON_Y, D_BUTTON_W);
    TextButtonClass cancelbtn (BUTTON_CANCEL, TXT_CANCEL, TPF_EBUTTON, D_CANCEL_X, D_CANCEL_Y, D_CANCEL_W);
    EditClass editbtn (BUTTON_EDIT, wayptname, 3, TPF_EFNT|TPF_NOSHADOW, D_EDIT_X, D_EDIT_Y, D_EDIT_W, -1, EditClass::ALPHANUMERIC);

    /*
    **	Initialize.
    */
    Set_Logic_Page(SeenBuff);

    /*
    **	Create the button list.
    */
    commands = &button;
    cancelbtn.Add_Tail(*commands);
    editbtn.Add_Tail(*commands);
    editbtn.Set_Focus();

    /*
    **	Main Processing Loop.
    */
    bool firsttime = true;
    bool display = true;
    bool process = true;
    while (process) {

        /*
        **	Invoke game callback.
        */
        if (Session.Type == GAME_NORMAL) {
            Call_Back();
        } else if (Main_Loop()) {
            process = false;
            cancel = true;
        }

        /*
        **	Refresh display if needed.
        */
        if (display) {

            /*
            **	Display the dialog box.
            */
            Hide_Mouse();
            if (display) {
                Dialog_Box(D_DIALOG_X, D_DIALOG_Y, D_DIALOG_W, D_DIALOG_H);
//				Draw_Caption(caption, D_DIALOG_X, D_DIALOG_Y, D_DIALOG_W);

            }

            /*
            **	Redraw the buttons.
            */
            if (display) {
                commands->Flag_List_To_Redraw();
            }
            Show_Mouse();
            display = false;
        }

        /*
        **	Get user input.
        */
        KeyNumType input = commands->Input();

        /*
        **	The first time through the processing loop, set the edit
        **	gadget to have the focus. The
        **	focus must be set here since the gadget list has changed
        **	and this change will cause any previous focus setting to be
        **	cleared by the input processing routine.
        */
        if (firsttime) {
            firsttime = false;
            editbtn.Set_Focus();
            editbtn.Flag_To_Redraw();
        }

        /*
        **	If the <RETURN> key was pressed, then default to the appropriate
        **	action button according to the style of this dialog box.
        */
        if (input == KN_RETURN) {
            input = (KeyNumType)(BUTTON_OK|KN_BUTTON);
        }

        /*
        **	Process input.
        */
        switch (input) {
            /*
            ** Load: if load fails, present a message, and stay in the dialog
            ** to allow the user to try another game
            */
            case (BUTTON_OK | KN_BUTTON):
                Hide_Mouse();
                SeenPage.Clear();
                GamePalette.Set();
                Show_Mouse();
                process = false;
                cancel = false;
                break;

            /*
            ** ESC/Cancel: break
            */
            case (KN_ESC):
            case (BUTTON_CANCEL | KN_BUTTON):
                Hide_Mouse();
                SeenPage.Clear();
                GamePalette.Set();
                Show_Mouse();
                cancel = true;
                process = false;
                break;

            default:
                break;
        }
    }
#endif

    Flag_To_Redraw(true);
    if (cancel) {
        return false;
    }

    return true;
}


void MapEditClass::Update_Waypoint(int waypt_idx)
{
    /*
    **    Unflag cell for this waypoint if there is one
    */
    Cell cell = Scen->Waypoint[waypt_idx];
    if (cell) {
        if (Scen->Waypoint[WAYPT_HOME] != cell && Scen->Waypoint[WAYPT_REINF] != cell) {
            (*this)[cell].IsWaypoint = false;
        }
    }
    Scen->Waypoint[waypt_idx] = CurrentCell;
    (*this)[CurrentCell].IsWaypoint = true;
    IsMapChanged = true;

    Flag_To_Redraw(false);
    IsToRedraw = true;
}


void MapEditClass::Object_Display(const ObjectTypeClass *objtype, Rect &window, Point2D &xy, HousesType house)
{
    switch (((ObjectTypeClass *)objtype)->What_Am_I())
    {
#if 0
        case RTTI_UNITTYPE:
        case RTTI_INFANTRYTYPE:
        case RTTI_AIRCRAFTTYPE:
        {
            int shape = 0;
            const ShapeFileStruct *ptr = objtype->Get_Cameo_Data();
            if (ptr) {
                ptr = objtype->Get_Image_Data();
                shape = 2;
            }
            CC_Draw_Shape(ptr, shape, x, y, window, SHAPE_NORMAL|SHAPE_CENTER|SHAPE_WIN_REL);
            break;
        }
#endif
        
#if 0
        case RTTI_BUILDINGTYPE:
        {
            const ShapeFileStruct *ptr = objtype->Get_Cameo_Data();
            if (ptr) {
                ptr = objtype->Get_Image_Data();
            }
            CC_Draw_Shape(ptr, 0, x, y, window, SHAPE_CENTER|SHAPE_WIN_REL);
        }
#endif
        
#if 0
        case RTTI_OVERLAYTYPE:
        {
            const ShapeFileStruct *ptr = objtype->Get_Image_Data();
            if (ptr) {
                int frame = 0;

                const OverlayTypeClass *overlaytype = (const OverlayTypeClass *)objtype;
                if (overlaytype->IsTiberium) {
                    frame = 7;
                }
                /*if (overlaytype->Type == OVERLAY_GEMS1 || overlaytype->Type == OVERLAY_GEMS2
                 || overlaytype->Type == OVERLAY_GEMS3 || overlaytype->Type == OVERLAY_GEMS4) {
                    frame = 2;
                }*/
                CC_Draw_Shape(objtype->Get_Image_Data(), frame, x, y, window, SHAPE_NORMAL|SHAPE_CENTER|SHAPE_WIN_REL);
            }
        }
#endif
        
#if 0
        case RTTI_SMUDGETYPE:
        {
            void const * ptr = objtype->Get_Image_Data();
            if (ptr != NULL) {
                for (int w = 0; w < Width; w++) {
                    for (int h = 0; h < Height; h++) {
                        CC_Draw_Shape(ptr, w + (h*Width), x + w*ICON_PIXEL_W, y + h*ICON_PIXEL_H, WINDOW_TACTICAL, SHAPE_WIN_REL);
                    }
                }
            }
        }
#endif

        case RTTI_TERRAINTYPE:
        {
            CC_Draw_Shape(TempSurface, NormalDrawer, objtype->Get_Image_Data(),
                0, &xy, &window, SHAPE_NORMAL|SHAPE_CENTER);
        }

#if 0
        case RTTI_ISOTILETYPE:
            int	w,h;
            int	index;
            bool	scale;		// Should the template be half sized?

            w = Bound(Width, 1, 13);
            h = Bound(Height, 1, 8);
            scale = (w > 3 || h > 3);
            if (scale) {
                x -= (w * ICON_PIXEL_W) / 4;
                y -= (h * ICON_PIXEL_H) / 4;
            } else {
                x -= (w * ICON_PIXEL_W) / 2;
                y -= (h * ICON_PIXEL_H) / 2;
            }
            x += WindowList[window][WINDOWX];
            y += WindowList[window][WINDOWY];

            IconsetClass const * iconset = (IconsetClass const *)Get_Image_Data();
            unsigned char const * map = iconset->Map_Data();

            for (index = 0; index < w*h; index++) {
                if (map[index] != 0xFF) {
                    HidPage.Draw_Stamp(iconset, index, 0, 0, NULL, WINDOW_MAIN);
                    if (scale) {

                        HidPage.Scale((*LogicPage), 0, 0,
                            x + ((index % w)*(ICON_PIXEL_W/2)),
                            y + ((index / w)*(ICON_PIXEL_H/2)),
                            ICON_PIXEL_W, ICON_PIXEL_H,
                            ICON_PIXEL_W/2, ICON_PIXEL_H/2, (char *)NULL);

                    } else {
                        HidPage.Blit((*LogicPage), 0, 0, x + ((index % w)*(ICON_PIXEL_W)),
                            y + ((index / w)*(ICON_PIXEL_H)), ICON_PIXEL_W, ICON_PIXEL_H);
                    }
                }
            }
#endif
    };
}


/***************************************************************************
 * MapEditClass::Popup_Controls -- shows/hides the pop-up object controls  *
 *                                                                         *
 * Call this routine whenever the CurrentObject changes. The routine will  *
 * selectively enable or disable the popup controls based on whether       *
 * CurrentObject is NULL, or if it's a Techno object, or what type of      *
 * Techno object it is.                                                    *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      none.                                                              *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   11/22/1994 BR : Created.                                              *
 *   04/30/1996 JLB : Revamped for new buttons and stuff.                  *
 *=========================================================================*/
void MapEditClass::Popup_Controls(void)
{
    TechnoTypeClass *objtype = nullptr;
    HousesType owner;      // object's current owner
    int mission_index;     // object's current mission
    int strength;          // object's 0-255 strength value
    int i;

    /*
    **	Remove all buttons from GScreen's button list (so none of them provide
    **	input any more); then, destroy the list by Zapping each button.  Then,
    **	we'll have to add at least the MapArea button back to the Input button
    **	list before we return, plus any other buttons to process input for.  We
    **	always must add MapArea LAST in the list, so it doesn't intercept the
    **	other buttons' input.
    */
    Remove_A_Button(*HouseList);
    Remove_A_Button(*MissionList);
    Remove_A_Button(*HealthGauge);
    Remove_A_Button(*HealthText);
    Remove_A_Button(*FacingDial);
    Remove_A_Button(*BaseGauge);
    Remove_A_Button(*BaseLabel);
    Remove_A_Button(*MapArea);

    Remove_A_Button(*Sellable);
    Remove_A_Button(*Rebuildable);
    Remove_A_Button(*Repairable);
    Remove_A_Button(*Nominal);

    /*
    **	If no current object, hide the list
    */
    if (!CurrentObjects.Count()) {
        Add_A_Button(*BaseGauge);
        Add_A_Button(*BaseLabel);
        Add_A_Button(*MapArea);
        return;
    }

    /*
    **	If not Techno, no need for editing buttons
    */
    if (!CurrentObjects.Fetch_Head()->Is_Techno()) {
        Add_A_Button(*BaseGauge);
        Add_A_Button(*BaseLabel);
        Add_A_Button(*MapArea);
        return;
    }

    objtype = CurrentObjects.Fetch_Head()->Techno_Type_Class();

    /*
    **	Get object's current values
    */
    owner = (HousesType)CurrentObjects.Fetch_Head()->Owner();
    mission_index = 0;
    for (i = 0; i < NUM_EDIT_MISSIONS; i++) {
        if (CurrentObjects.Fetch_Head()->Get_Mission() == MapEditMissions[i]) {
            mission_index = i;
        }
    }
    strength = CurrentObjects.Fetch_Head()->Health_Ratio()*256;

    switch (objtype->What_Am_I()) {
        case RTTI_UNITTYPE:
        case RTTI_INFANTRYTYPE:
        case RTTI_AIRCRAFTTYPE:
        {
            TechnoClass *techno = (BuildingClass *)CurrentObjects.Fetch_Head();

            MissionList->Set_Selected_Index(mission_index);
            HealthGauge->Set_Value(strength);
            std::snprintf(HealthBuf, sizeof(HealthBuf), "%d", CurrentObjects.Fetch_Head()->Strength);
            FacingDial->Set_Direction(techno->PrimaryFacing.Current().Get_Dir());

            /*
            **	Make the list.
            */
            Add_A_Button(*HealthGauge);
            Add_A_Button(*HouseList);
            HouseList->Set_Selected_Index(owner);
            Add_A_Button(*MissionList);
            Add_A_Button(*HealthText);
            Add_A_Button(*FacingDial);
            break;
        }

        case RTTI_BUILDINGTYPE:
        {
            BuildingClass *building = (BuildingClass *)CurrentObjects.Fetch_Head();

            HealthGauge->Set_Value(strength);
            std::snprintf(HealthBuf, sizeof(HealthBuf), "%d", building->Strength);
            Add_A_Button(*HealthGauge);
            Add_A_Button(*HouseList);
            HouseList->Set_Selected_Index(owner);
            Add_A_Button(*HealthText);

            Add_A_Button(*Sellable);
            if (building->IsAllowedToSell) {
                Sellable->Turn_On();
            } else {
                Sellable->Turn_Off();
            }
            Add_A_Button(*Rebuildable);
            if (building->IsToRebuild) {
                Rebuildable->Turn_On();
            } else {
                Rebuildable->Turn_Off();
            }
            Add_A_Button(*Repairable);
            if (building->IsToRepair) {
                Repairable->Turn_On();
            } else {
                Repairable->Turn_Off();
            }
            Add_A_Button(*Nominal);
            if (building->IsNominal) {
                Nominal->Turn_On();
            } else {
                Nominal->Turn_Off();
            }
            if (objtype->IsTurretEquipped) {
                FacingDial->Set_Direction(building->PrimaryFacing.Current().Get_Dir());
                Add_A_Button(*FacingDial);
            }
            break;
        }
    }

    /*
    **	Add the map area last, so it's "underneath" the other buttons, and won't
    **	intercept input for those buttons.
    */
    Add_A_Button(*BaseGauge);
    Add_A_Button(*BaseLabel);
    Add_A_Button(*MapArea);
}


#include "mapedit_context.h"

void MapEditClass::Context_Menu(Point2D &mouse_xy)
{
    #define CONTEXT_MENU_ITEM_COUNT 8
    #define CONTEXT_MENU_FONT TPF_8POINT

    const char *_menus[CONTEXT_MENU_ITEM_COUNT + 1];
    int selection; // option the user picks.
    bool process; // menu stays up while true.
    int rc;

    /**
     *  Populate menu items.
     */
    _menus[0] = "New Scenario";
    _menus[1] = "Load Scenario";
    _menus[2] = "Save Scenario";
    _menus[3] = "Size Map";
    _menus[4] = "Add Game Object";
    _menus[5] = "Scenario Options";
    _menus[6] = "AI Options";
    _menus[7] = "Play Scenario";
    _menus[8] = nullptr;

    /**
     *  Display default mouse cursor.
     */
    Override_Mouse_Shape(MOUSE_NORMAL);

    /**
     *  Context menu loop.
     */
    process = true;
    while (process) {

        /**
         *  Invoke game callback, to update music.
         */
        Call_Back();

        /**
         *  Invoke the context menu. Do_Menu assumes the mouse is already
         *  hidden so lets do that first.
         */
        WWMouse->Hide_Mouse();

        Rect menu_rect(
            mouse_xy,
            50,
            Font_Ptr(CONTEXT_MENU_FONT)->Get_Char_Height() * CONTEXT_MENU_ITEM_COUNT
        );

        selection = Do_Context_Menu(menu_rect, TempSurface, &_menus[0], CONTEXT_MENU_FONT);

        WWMouse->Show_Mouse();
        if (UnknownKey == KN_ESC || UnknownKey == KN_LMOUSE || UnknownKey == KN_RMOUSE) {
            //process = false;
            selection = -1;
        }

        /**
         *  Process menu selection.
         */
        switch (selection) {

            /**
             * New scenario
             */
            case 0:
                if (IsMapChanged) {
                    rc = WWMessageBox().Process("Save Changes?", 0, TXT_YES, TXT_NO);
                    HiddenSurface->Clear();
                    Flag_To_Redraw(true);
                    Render();
                    if (rc==0) {
                        if (!Save_Scenario()) {
                            break;
                        } else {
                            IsMapChanged = false;
                        }
                    }
                }
                if (New_Scenario()) {
                    Scen->CarryOverCap = 0;
                    Scen->CarryOverPercent = 0;
                    IsMapChanged = true;
                }
                process = false;
                break;

            /*
            .......................... Load scenario ...........................
            */
            case 1:
                if (IsMapChanged) {
                    rc = WWMessageBox().Process("Save Changes?", 0, TXT_YES, TXT_NO);
                    HiddenSurface->Clear();
                    Flag_To_Redraw(true);
                    Render();
                    if (rc==0) {
                        if (!Save_Scenario()) {
                            break;
                        } else {
                            IsMapChanged = false;
                        }
                    }
                }
                if (Load_Scenario()) {
                    Scen->CarryOverCap = 0;
                    Scen->CarryOverPercent = 0;
                    IsMapChanged = false;
                }
                process = false;
                break;

            /*
            .......................... Save scenario ...........................
            */
            case 2:
                if (Save_Scenario()) {
                    IsMapChanged = 0;
                }
                process = false;
                break;

            /*
            .......................... Edit map size ...........................
            */
            case 3:
                if (Size_Map(MapCellX, MapCellY, MapCellWidth, MapCellHeight)) {
                    process = false;
                    IsMapChanged = true;
                }
                break;

            /*
            .......................... Add an object ...........................
            */
            case 4:
                if (Placement_Dialog()) {
                    Start_Placement();
                    process = false;
                }
                break;

            /*
            ......................... Scenario options .........................
            */
            case 5:
                //if (Scenario_Dialog()) {
                //	IsMapChanged = 1;
                //	process = false;
                //}
                break;

            /*
            .......................... Other options ...........................
            */
            case 6:
                //AI_Menu();
                process = false;
                break;

            /*
            ...................... Test-drive this scenario ....................
            */
            case 7:
                if (IsMapChanged) {
                    rc = WWMessageBox().Process("Save Changes?", 0, TXT_YES, TXT_NO);
                    HiddenSurface->Clear();
                    Flag_To_Redraw(true);
                    Render();
                    if (rc==0) {
                        if (!Save_Scenario()) {
                            break;
                        } else {
                            IsMapChanged = true;
                        }
                    }
                }
                IsMapChanged = false;
                Debug_Map = false;
                Start_Scenario(Scen->ScenarioName, false);
                return;

            /**
             *  ESCAPE key pressed.
             */
            default:
            case -1:
                process = false;
                break;
        }

        Sleep(10);
    }

    /*------------------------------------------------------------------------
    Restore the display:
    - Clear HIDPAGE to erase any spurious drawing done by the menu system
    - Invoke Flag_To_Redraw to tell DisplayClass to re-render the whole screen
    - Invoke Redraw() to update the display
    ------------------------------------------------------------------------*/
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
    Render();
}


/***************************************************************************
 * MapEditClass::Clear_List -- clears the internal choosable object list   *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      none.                                                              *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   10/20/1994 BR : Created.                                              *
 *=========================================================================*/
void MapEditClass::Clear_List()
{
    /*------------------------------------------------------------------------
    Set # object type ptrs to 0, set NumType for each type to 0
    ------------------------------------------------------------------------*/
    AvailableObjects.Clear();
    for (int i = 0; i < 8/*NUM_EDIT_CLASSES*/; i++) {
        NumType[i] = 0;
    }
}


/***************************************************************************
 * MapEditClass::Add_To_List -- adds a TypeClass to the choosable list     *
 *                                                                         *
 * Use this routine to add an object to the game object selection list.    *
 * This list is used by the Add_Object function. All items located in the  *
 * list will appear and be chooseable by that function. Make sure to       *
 * clear the list before adding a sequence of items to it. Clearing        *
 * the list is accomplished by the Clear_List() function.                  *
 *                                                                         *
 * INPUT:                                                                  *
 *      object      ptr to ObjectTypeClass to add                          *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      bool: was the object added to the list?  A failure could occur if  *
 *      NULL were passed in or the list is full.                           *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   06/04/1994 JLB : Created.                                             *
 *=========================================================================*/
bool MapEditClass::Add_To_List(const ObjectTypeClass *object)
{
    /*
    **	Add the object if there's room.
    */
    if (object/* && AvailableObjects.Count() < MAX_EDIT_OBJECTS*/) {
        AvailableObjects.Add(object);
        //Objects[NumObjects++] = object;

        /*
        **	Update type counters.
        */
        switch (((ObjectTypeClass *)object)->What_Am_I()) {
            case RTTI_ISOTILETYPE:
                NumType[0]++;
                break;

            case RTTI_OVERLAYTYPE:
                NumType[1]++;
                break;

            case RTTI_SMUDGETYPE:
                NumType[2]++;
                break;

            case RTTI_TERRAINTYPE:
                NumType[3]++;
                break;

            case RTTI_UNITTYPE:
                NumType[4]++;
                break;

            case RTTI_INFANTRYTYPE:
                NumType[5]++;
                break;

            case RTTI_AIRCRAFTTYPE:
                NumType[6]++;
                break;

            case RTTI_BUILDINGTYPE:
                NumType[7]++;
                break;
        }
        //DEV_DEBUG_INFO("Editor: Added obejct \"%s\" to the list.\n", object->Name());
        return true;
    }

    return false;
}


/**
 *  Adds an object to the scenario.
 */
bool MapEditClass::Placement_Dialog()
{
    /*
    **	Dialog & button dimensions
    */
    enum {
        SCALE = 2,

        D_DIALOG_W = 400 * SCALE,
        D_DIALOG_H = 180 * SCALE,
        D_DIALOG_X = 800/2,
        D_DIALOG_Y = 600/2,
        D_DIALOG_CX = D_DIALOG_X + (D_DIALOG_W / 2) * SCALE,

        D_TXT8_H = 11 * SCALE,
        D_MARGIN = 7 * SCALE,

        D_PICTURE_W = 152 * SCALE,					// must be divisible by 8!
        D_PICTURE_H = 105 * SCALE,
        D_PICTURE_X = D_DIALOG_X + 35 * SCALE,		// must start on a byte boundary!
        D_PICTURE_Y = D_DIALOG_Y + D_MARGIN + D_TXT8_H + D_MARGIN * SCALE,
        D_PICTURE_CX = D_PICTURE_X + D_PICTURE_W / 2 * SCALE,

        D_GDI_W = 65 * SCALE,
        D_GDI_H = 9 * SCALE,
        D_GDI_X = D_PICTURE_X+D_PICTURE_W+5 * SCALE,
        D_GDI_Y = D_PICTURE_Y * SCALE,

        D_LEFT_W = 45 * SCALE,
        D_LEFT_H = 9 * SCALE,
        D_LEFT_X = D_PICTURE_CX - 5 - D_LEFT_W * SCALE,
        D_LEFT_Y = D_PICTURE_Y + D_PICTURE_H + D_MARGIN * SCALE,

        D_RIGHT_W = 45 * SCALE,
        D_RIGHT_H = 9 * SCALE,
        D_RIGHT_X = D_PICTURE_CX + 5 * SCALE,
        D_RIGHT_Y = D_PICTURE_Y + D_PICTURE_H + D_MARGIN * SCALE,

        D_TEMPLATE_W = 70 * SCALE,
        D_TEMPLATE_H = 9 * SCALE,
        D_TEMPLATE_X = D_DIALOG_X + D_DIALOG_W - D_MARGIN - D_TEMPLATE_W - 30 * SCALE,
        D_TEMPLATE_Y = D_PICTURE_Y * SCALE,

        D_OVERLAY_W = 70 * SCALE,
        D_OVERLAY_H = 9 * SCALE,
        D_OVERLAY_X = D_DIALOG_X + D_DIALOG_W - D_MARGIN - D_OVERLAY_W - 30 * SCALE,
        D_OVERLAY_Y = D_TEMPLATE_Y + D_TEMPLATE_H * SCALE,

        D_SMUDGE_W = 70 * SCALE,
        D_SMUDGE_H = 9 * SCALE,
        D_SMUDGE_X = D_DIALOG_X + D_DIALOG_W - D_MARGIN - D_SMUDGE_W - 30 * SCALE,
        D_SMUDGE_Y = D_OVERLAY_Y + D_OVERLAY_H * SCALE,

        D_TERRAIN_W = 70 * SCALE,
        D_TERRAIN_H = 9 * SCALE,
        D_TERRAIN_X = D_DIALOG_X + D_DIALOG_W - D_MARGIN - D_TERRAIN_W - 30 * SCALE,
        D_TERRAIN_Y = D_SMUDGE_Y + D_SMUDGE_H * SCALE,

        D_UNIT_W = 70 * SCALE,
        D_UNIT_H = 9 * SCALE,
        D_UNIT_X = D_DIALOG_X + D_DIALOG_W - D_MARGIN - D_UNIT_W - 30 * SCALE,
        D_UNIT_Y = D_TERRAIN_Y + D_TERRAIN_H * SCALE,

        D_INFANTRY_W = 70 * SCALE,
        D_INFANTRY_H = 9 * SCALE,
        D_INFANTRY_X = D_DIALOG_X + D_DIALOG_W - D_MARGIN - D_INFANTRY_W - 30 * SCALE,
        D_INFANTRY_Y = D_UNIT_Y + D_UNIT_H * SCALE,

        D_AIRCRAFT_W = 70 * SCALE,
        D_AIRCRAFT_H = 9 * SCALE,
        D_AIRCRAFT_X = D_DIALOG_X + D_DIALOG_W - D_MARGIN - D_AIRCRAFT_W - 30 * SCALE,
        D_AIRCRAFT_Y = D_INFANTRY_Y + D_INFANTRY_H * SCALE,

        D_BUILDING_W = 70 * SCALE,
        D_BUILDING_H = 9 * SCALE,
        D_BUILDING_X = D_DIALOG_X + D_DIALOG_W - D_MARGIN - D_BUILDING_W - 30 * SCALE,
        D_BUILDING_Y = D_AIRCRAFT_Y + D_AIRCRAFT_H * SCALE,

        //D_AIR_W = 70 * SCALE,
        //D_AIR_H = 9 * SCALE,
        //D_AIR_X = D_DIALOG_X + D_DIALOG_W - D_MARGIN - D_AIR_W - 30 * SCALE,
        //D_AIR_Y = D_BUILDING_Y + D_BUILDING_H * SCALE,

        D_OK_W = 45 * SCALE,
        D_OK_H = 9 * SCALE,
        D_OK_X = D_PICTURE_CX - D_OK_W - 5 * SCALE,
        D_OK_Y = D_DIALOG_Y + D_DIALOG_H - D_OK_H - D_MARGIN - 15 * SCALE,

        D_CANCEL_W = 45 * SCALE,
        D_CANCEL_H = 9 * SCALE,
        D_CANCEL_X = D_PICTURE_CX + 5 * SCALE,
        D_CANCEL_Y = D_DIALOG_Y + D_DIALOG_H - D_CANCEL_H - D_MARGIN - 15 * SCALE,

        GRIDSIZE = 10 * SCALE,
        GRIDBLOCK_W = 3 * SCALE,
        GRIDBLOCK_H = 3 * SCALE,
        D_GRID_X = D_DIALOG_X + D_DIALOG_W - (GRIDSIZE * GRIDBLOCK_W) - D_MARGIN - 35 * SCALE,
        D_GRID_Y = D_DIALOG_Y + D_DIALOG_H - (GRIDSIZE * GRIDBLOCK_H) - D_MARGIN - 35 * SCALE,
    };

    /*
    **	Button enumerations:
    */
    enum {
        BUTTON_GDI=100,
        BUTTON_HOUSE,
        BUTTON_NEXT,
        BUTTON_PREV,
        BUTTON_OK,
        BUTTON_CANCEL,
        BUTTON_ISOTILE/*BUTTON_TEMPLATE*/,
        BUTTON_OVERLAY,
        BUTTON_SMUDGE,
        BUTTON_TERRAIN,
        BUTTON_UNIT,
        BUTTON_INFANTRY,
        BUTTON_AIRCRAFT,
        BUTTON_BUILDING,
        //BUTTON_AIR,
    };

    HousesType house;
    ColorScheme * scheme = ColorSchemes[GadgetClass::Get_Color_Scheme()];

    /*
    **	Dialog variables
    */
    bool cancel = false;							// true = user cancels
    const ObjectTypeClass * curobj;			// Working object pointer.
    int x,y;											// for drawing the grid
    KeyNumType input;								// user input
    const Cell * occupy;							// ptr into object's OccupyList
    int cell;										// cell index for parsing OccupyList
    int i;
    int typeindex;									// index of class type

    /*
    **	Buttons
    */
    ControlClass * commands;

    ListClass housebtn(BUTTON_HOUSE,
        D_GDI_X, D_GDI_Y, 60, 8*16,
        TPF_EFNT | TPF_NOSHADOW,
        MFCC::Retrieve("EBTN-UP.SHP"),
        MFCC::Retrieve("EBTN-DN.SHP"));
    for (HousesType house = HOUSE_FIRST; house < HouseTypes.Count(); ++house) {
        housebtn.Add_Item(HouseTypeClass::As_Reference(house).Name());
    }

    house = HOUSE_FIRST;

    TextButtonClass nextbtn(BUTTON_NEXT, ">>", TPF_EBUTTON, D_RIGHT_X, D_RIGHT_Y, D_RIGHT_W, D_RIGHT_H);
    TextButtonClass prevbtn(BUTTON_PREV, "<<", TPF_EBUTTON, D_LEFT_X, D_LEFT_Y, D_LEFT_W, D_LEFT_H);
    TextButtonClass okbtn(BUTTON_OK, "OK", TPF_EBUTTON, D_OK_X, D_OK_Y, D_OK_W, D_OK_H);
    TextButtonClass cancelbtn(BUTTON_CANCEL, "Cancel", TPF_EBUTTON, D_CANCEL_X, D_CANCEL_Y, D_CANCEL_W, D_CANCEL_H);
    TextButtonClass templatebtn(BUTTON_ISOTILE, "Isometric Tile", TPF_EBUTTON, D_TEMPLATE_X, D_TEMPLATE_Y, D_TEMPLATE_W, D_TEMPLATE_H);
    TextButtonClass overlaybtn(BUTTON_OVERLAY, "Overlay", TPF_EBUTTON, D_OVERLAY_X, D_OVERLAY_Y, D_OVERLAY_W, D_OVERLAY_H);
    TextButtonClass smudgebtn(BUTTON_SMUDGE, "Smudge", TPF_EBUTTON, D_SMUDGE_X, D_SMUDGE_Y, D_SMUDGE_W, D_SMUDGE_H);
    TextButtonClass terrainbtn(BUTTON_TERRAIN, "Terrain", TPF_EBUTTON, D_TERRAIN_X, D_TERRAIN_Y, D_TERRAIN_W, D_TERRAIN_H);
    TextButtonClass unitbtn(BUTTON_UNIT, "Unit", TPF_EBUTTON, D_UNIT_X, D_UNIT_Y, D_UNIT_W, D_UNIT_H);
    TextButtonClass infantrybtn(BUTTON_INFANTRY, "Infantry", TPF_EBUTTON, D_INFANTRY_X, D_INFANTRY_Y, D_INFANTRY_W, D_INFANTRY_H);
    TextButtonClass aircraftbtn(BUTTON_AIRCRAFT, "Aircraft", TPF_EBUTTON, D_AIRCRAFT_X, D_AIRCRAFT_Y, D_AIRCRAFT_W, D_AIRCRAFT_H);
    TextButtonClass buildingbtn(BUTTON_BUILDING, "Building", TPF_EBUTTON, D_BUILDING_X, D_BUILDING_Y, D_BUILDING_W, D_BUILDING_H);
    //TextButtonClass airbtn(BUTTON_AIR, "Aircraft", TPF_EBUTTON, D_AIR_X, D_AIR_Y, D_AIR_W, D_AIR_H);

    /*
    **	Initialize addable objects list; we must do this every time in case one
    **	of the object pools has become exhausted; that object won't be available
    **	for adding.  (Skip aircraft, since they won't be used in the editor.)
    */
    Clear_List();
    IsometricTileType_Prep_For_Add();
    OverlayType_Prep_For_Add();
    SmudgeType_Prep_For_Add();
    TerrainType_Prep_For_Add();
    UnitType_Prep_For_Add();
    InfantryType_Prep_For_Add();
    BuildingType_Prep_For_Add();
    AircraftType_Prep_For_Add();

    /*
    **	Compute offset of each class type in the Objects array
    */
    TypeOffset[0] = 0;
    for (i = 1; i < 8/*NUM_EDIT_CLASSES*/; i++) {
    	TypeOffset[i] = TypeOffset[i-1] + NumType[i-1];
    }

    /*
    **	Return if no objects to place
    */
    if (!AvailableObjects.Count())  {
    	return -1;
    }

    /*
    **	Initialize
    */
    if (LastChoice >= AvailableObjects.Count()) {
    	LastChoice = 0;
    }
    curobj = (ObjectTypeClass *)Objects[LastChoice];		// current object to choose

    commands = &nextbtn;
    housebtn.Add_Tail(*commands);
    prevbtn.Add_Tail(*commands);
    okbtn.Add_Tail(*commands);
    cancelbtn.Add_Tail(*commands);
    templatebtn.Add_Tail(*commands);
    overlaybtn.Add_Tail(*commands);
    smudgebtn.Add_Tail(*commands);
    terrainbtn.Add_Tail(*commands);
    unitbtn.Add_Tail(*commands);
    infantrybtn.Add_Tail(*commands);
    aircraftbtn.Add_Tail(*commands);
    buildingbtn.Add_Tail(*commands);
    //airbtn.Add_Tail(*commands);

    /*
    **	Make sure the recorded house selection matches the house list
    **	box selection.
    */
    LastHouse = HousesType(housebtn.Current_Index());

    /*
    **	Main processing loop
    */
    bool display = true;
    bool process = true;
    while (process) {

        /*
        **	Invoke game callback
        */
        Call_Back();

        DSurface *temp_prev = TempSurface;
        TempSurface = HiddenSurface;            // BUGFIX?

        /*
        **	Refresh display if needed
        */
        if (display) {

            /*
            **	Display the dialog box
            */
            WWMouse->Hide_Mouse();
            Draw_Box(Rect(D_DIALOG_X, D_DIALOG_Y, D_DIALOG_W, D_DIALOG_H), TempSurface, BOXSTYLE_BORDER, true);

            //Draw_Caption(TXT_PLACE_OBJECT, D_DIALOG_X, D_DIALOG_Y, D_DIALOG_W);
            Fancy_Text_Print("Place Object", TempSurface, &TempSurface->Get_Rect(),
                &Point2D(D_DIALOG_X+(D_DIALOG_W/2), D_DIALOG_Y), scheme, COLOR_TBLACK,
                TPF_CENTER|TPF_EFONT_NOSHADOW);

            /*
            **	Display the current object:
            **	- save the current window dimensions
            **	- adjust the window size to the actual drawable area
            **	- draw the shape
            **	- reset the window dimensions
            */
            Draw_Box(Rect(D_PICTURE_X, D_PICTURE_Y, D_PICTURE_W, D_PICTURE_H), TempSurface, BOXSTYLE_DOWN, false);
            Object_Display(curobj, TempSurface->Get_Rect(), TempSurface->Get_Rect().Center_Point(), LastHouse);

            /*
            **	Erase the grid
            */
            //TempSurface->Fill_Rect(D_GRID_X - GRIDBLOCK_W * 2, D_GRID_Y,
            //	D_GRID_X + GRIDSIZE * GRIDBLOCK_W,
            //	D_GRID_Y + GRIDSIZE * GRIDBLOCK_H, COLOR_BLACK);

            /*
            **	Draw a box for every cell occupied
            */
            if (curobj) {
                occupy = curobj->Occupy_List();
//INF LOOP      //while (*occupy && occupy->X != REFRESH_EOL || occupy->Y != REFRESH_EOL) {
                //    cell = (*occupy);
                //    occupy++;
                //    x = D_GRID_X + ((cell % MAP_CELL_W) * GRIDBLOCK_W);
                //    y = D_GRID_Y + ((cell / MAP_CELL_W) * GRIDBLOCK_H);
                //    TempSurface->Fill_Rect(TempSurface->Get_Rect(),
                //        Rect(x, y, x + GRIDBLOCK_W - 1, y + GRIDBLOCK_H - 1), scheme->Bright);
                //}
            }

            /*
            **	Draw the grid itself
            */
            for (y = 0; y <= GRIDSIZE; y++) {
                for (x = 0; x <= GRIDSIZE; x++) {
                    TempSurface->Draw_Line(TempSurface->Get_Rect(),
                        Point2D(D_GRID_X + x * GRIDBLOCK_W, D_GRID_Y),
                        Point2D(D_GRID_X + x * GRIDBLOCK_W, D_GRID_Y + GRIDSIZE * GRIDBLOCK_H),
                        scheme->Shadow);
                }
                TempSurface->Draw_Line(TempSurface->Get_Rect(),
                    Point2D(D_GRID_X, D_GRID_Y + y * GRIDBLOCK_H),
                    Point2D(D_GRID_X + GRIDSIZE * GRIDBLOCK_W, D_GRID_Y + y * GRIDBLOCK_H),
                    scheme->Shadow);
            }

            /*
            **	Print the object's label from the class's Full_Name().
            **	Warning: Text_String returns an EMS pointer, so standard string
            **	functions won't work!
            */
            if (curobj) {
                Fancy_Text_Print(curobj->Full_Name(), TempSurface, &TempSurface->Get_Rect(),
                    &Point2D(D_PICTURE_CX, D_PICTURE_Y + D_MARGIN), scheme, COLOR_TBLACK,
                    TPF_CENTER|TPF_EFONT_NOSHADOW);
            }

            /*
            **	Redraw buttons
            **	Figure out which class category we're in & highlight that button
            **	This updates 'typeindex', which is used below, and it also updates
            **	the category button states.
            */
            int i = 0;
            for (typeindex = 0; typeindex < 8/*NUM_EDIT_CLASSES*/; typeindex++) {
            	i += NumType[typeindex];
            	if (LastChoice < i) break;
            }
            templatebtn.Turn_Off();
            overlaybtn.Turn_Off();
            smudgebtn.Turn_Off();
            terrainbtn.Turn_Off();
            unitbtn.Turn_Off();
            infantrybtn.Turn_Off();
            aircraftbtn.Turn_Off();
            //airbtn.Turn_Off();
            buildingbtn.Turn_Off();
            switch (typeindex + BUTTON_ISOTILE) {
                case BUTTON_ISOTILE:
                    templatebtn.Turn_On();
                    break;

                case BUTTON_OVERLAY:
                    overlaybtn.Turn_On();
                    break;

                case BUTTON_SMUDGE:
                    smudgebtn.Turn_On();
                    break;

                case BUTTON_TERRAIN:
                    terrainbtn.Turn_On();
                    break;

                case BUTTON_UNIT:
                    unitbtn.Turn_On();
                    break;

                case BUTTON_INFANTRY:
                    infantrybtn.Turn_On();
                    break;

                case BUTTON_AIRCRAFT:
                    aircraftbtn.Turn_On();
                    break;

                //case BUTTON_AIR:
                //    airbtn.Turn_On();
                //    break;

                case BUTTON_BUILDING:
                    buildingbtn.Turn_On();
                    break;
            }

            /*
            **	Redraw buttons
            */
            commands->Draw_All();
            WWMouse->Show_Mouse();
            display = false;

        }

        /*
        **	Get user input
        */
        input = commands->Input();

        /*
        **	Process user input
        */
        switch (input) {

            /*
            **	GDI House
            */
            case KN_BUTTON|BUTTON_HOUSE:
                house = HousesType(housebtn.Current_Index());

                /*
                **	Set flags & buttons
                */
                LastHouse = house;
                display = true;
                break;

            /*
            **	Next in list
            */
            case KN_RIGHT:
            case KN_BUTTON|BUTTON_NEXT:
                /*
                **	Increment to next obj
                */
                LastChoice++;
                if (LastChoice == AvailableObjects.Count()) {
                	LastChoice = 0;
                }
                curobj = AvailableObjects[LastChoice];

                nextbtn.Turn_Off();
                display = true;
                break;

            /*
            **	Previous in list
            */
            case KN_LEFT:
            case KN_BUTTON|BUTTON_PREV:

                /*
                **	Decrement to prev obj
                */
                LastChoice--;
                if (LastChoice < 0) {
                	LastChoice = AvailableObjects.Count()-1;
                }
                curobj = AvailableObjects[LastChoice];
                prevbtn.Turn_Off();
                display = true;
                break;

            /*
            **	Select a class type
            */
            case KN_BUTTON|BUTTON_ISOTILE:
            case KN_BUTTON|BUTTON_OVERLAY:
            case KN_BUTTON|BUTTON_SMUDGE:
            case KN_BUTTON|BUTTON_TERRAIN:
            case KN_BUTTON|BUTTON_UNIT:
            case KN_BUTTON|BUTTON_INFANTRY:
            case KN_BUTTON|BUTTON_AIRCRAFT:
            case KN_BUTTON|BUTTON_BUILDING:
            //case KN_BUTTON|BUTTON_AIR:

                /*
                **	Find index of class
                */
                typeindex = input - (KN_BUTTON|BUTTON_ISOTILE);

                /*
                **	If no objects of that type, do nothing
                */
                if (NumType[typeindex] == 0) {
                	display = true;
                	break;
                }

                /*
                **	Set current object
                */
                LastChoice = TypeOffset[typeindex];
                curobj = (ObjectTypeClass *)Objects[LastChoice];
                display = true;
                break;

            /*
            **	Next category
            */
            case KN_PGDN:
                typeindex++;
                if (typeindex == 8/*NUM_EDIT_CLASSES*/) {
                	typeindex = 0;
                }

                /*
                **	Set current object
                */
                LastChoice = TypeOffset[typeindex];
                curobj = (ObjectTypeClass *)Objects[LastChoice];
                display = true;
                break;

            /*
            **	Previous category
            */
            case KN_PGUP:
                typeindex--;
                if (typeindex < 0) {
                	typeindex = 8/*NUM_EDIT_CLASSES*/ - 1;
                }

                /*
                **	Set current object
                */
                LastChoice = TypeOffset[typeindex];
                curobj = (ObjectTypeClass *)Objects[LastChoice];
                display = true;
                break;

            /*
            **	Jump to 1st choice
            */
            case KN_HOME:
                LastChoice = 0;

                /*
                **	Set current object
                */
                curobj = (ObjectTypeClass *)Objects[LastChoice];
                display = true;
                break;

            /*
            **	OK
            */
            case KN_RETURN:
            case KN_BUTTON|BUTTON_OK:
                cancel = false;
                process = false;
                break;

            /*
            **	Cancel
            */
            case KN_ESC:
            case KN_BUTTON|BUTTON_CANCEL:
                cancel = true;
                process = false;
                break;

            default:
                break;
        }

    }

    /*
    **	Redraw the display
    */
    HiddenSurface->Clear();
    Flag_To_Redraw(true);
    Render();

    if (cancel) {
        return false;
    }

    return true;
}


/***********************************************************************************************
 * TerrainTypeClass::Prep_For_Add -- Prepares to add terrain object.                           *
 *                                                                                             *
 *    Submits all of the valid terrain objects to the scenario editor for possible selection   *
 *    and subsequent placement on the map. All terrain objects, that have a valid shape        *
 *    file available, are added.                                                               *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/23/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void MapEditClass::IsometricTileType_Prep_For_Add()
{
    for (IsometricTileType index = ISOTILE_FIRST; index < IsoTileTypes.Count(); ++index) {
        if (IsometricTileTypeClass::As_Reference(index).Get_Tile_Data() != nullptr) {
            MapEditClass::Add_To_List(&IsometricTileTypeClass::As_Reference(index));
        }
    }
}


void MapEditClass::TerrainType_Prep_For_Add()
{
    for (TerrainType index = TERRAIN_FIRST; index < TerrainTypes.Count(); ++index) {
        if (TerrainTypeClass::As_Reference(index).Get_Image_Data() != nullptr) {
            MapEditClass::Add_To_List(&TerrainTypeClass::As_Reference(index));
        }
    }
}


void MapEditClass::OverlayType_Prep_For_Add()
{
    for (OverlayType index = OVERLAY_FIRST; index < OverlayTypes.Count(); ++index) {
        if (OverlayTypeClass::As_Reference(index).Get_Image_Data() != nullptr) {
            MapEditClass::Add_To_List(&OverlayTypeClass::As_Reference(index));
        }
    }
}


void MapEditClass::SmudgeType_Prep_For_Add()
{
    for (SmudgeType index = SMUDGE_FIRST; index < SmudgeTypes.Count(); ++index) {
        if (SmudgeTypeClass::As_Reference(index).Get_Image_Data() != nullptr) {
            MapEditClass::Add_To_List(&SmudgeTypeClass::As_Reference(index));
        }
    }
}


void MapEditClass::UnitType_Prep_For_Add()
{
    for (UnitType index = UNIT_FIRST; index < UnitTypes.Count(); ++index) {
        if (UnitTypeClass::As_Reference(index).Get_Image_Data() != nullptr) {
            MapEditClass::Add_To_List(&UnitTypeClass::As_Reference(index));
        }
    }
}


void MapEditClass::InfantryType_Prep_For_Add()
{
    for (InfantryType index = INFANTRY_FIRST; index < InfantryTypes.Count(); ++index) {
        if (InfantryTypeClass::As_Reference(index).Get_Image_Data() != nullptr) {
            MapEditClass::Add_To_List(&InfantryTypeClass::As_Reference(index));
        }
    }
}


void MapEditClass::BuildingType_Prep_For_Add()
{
    for (BuildingType index = BUILDING_FIRST; index < BuildingTypes.Count(); ++index) {
        if (BuildingTypeClass::As_Reference(index).Get_Image_Data() != nullptr) {
            MapEditClass::Add_To_List(&BuildingTypeClass::As_Reference(index));
        }
    }
}


void MapEditClass::AircraftType_Prep_For_Add()
{
    for (AircraftType index = AIRCRAFT_FIRST; index < AircraftTypes.Count(); ++index) {
        if (AircraftTypeClass::As_Reference(index).Get_Image_Data() != nullptr) {
            MapEditClass::Add_To_List(&AircraftTypeClass::As_Reference(index));
        }
    }
}
