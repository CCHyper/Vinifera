#include "mapedit.h"
#include "mapedit_functions.h"
#include "tibsun_defines.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "tibsun_util.h"
#include "vinifera_globals.h"
#include "house.h"
#include "housetype.h"
#include "trigger.h"
#include "triggertype.h"
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
#include "wwfont.h"
#include "dsurface.h"
#include "wwkeyboard.h"
#include "wwmouse.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"


ControlClass *MapEditClass::MapArea = nullptr;
Cell MapEditClass::CurrentCell;
TriggerClass *MapEditClass::CurTrigger = nullptr;
TagClass *MapEditClass::CurTag = nullptr;
ObjectClass *MapEditClass::GrabbedObject = nullptr;
Cell MapEditClass::GrabOffset;
unsigned MapEditClass::LastClickTime;
bool MapEditClass::IsLeftMouseDown = false;
bool MapEditClass::IsMapChanged = false;
bool MapEditClass::IsBaseBuilding = false;
bool MapEditClass::IsMarbleMadnessAvailable = false;


#define MAP_AREA 500


void MapEditClass::_One_Time()
{
    MouseClass::One_Time();

    /**
     *  The map: a single large "button".
     */
    MapArea = new ControlClass(MAP_AREA,
        TacticalRect.X, TacticalRect.Y+16, TacticalRect.Width, TacticalRect.Height-16,
        GadgetClass::LEFTPRESS|GadgetClass::LEFTRELEASE, false);
}


void MapEditClass::_Init_IO()
{
    /**
     *  For normal game mode, jump to the parent's Init routine.
     */
    if (!Debug_Map) {

        MouseClass::Init_IO();

    } else {

        /**
         *  For editor mode, add the map area to the button input list.
         */
        //Buttons = nullptr;
        //Add_A_Button(*BaseGauge);
        //Add_A_Button(*BaseLabel);
        //Add_A_Button(*MapArea);
    }
}


void MapEditClass::_AI(KeyNumType &input, Point2D &xy)
{
    int rc;

    /**
     *  Trap 'F2' regardless of whether we're in game or editor mode.
     */
    if (Vinifera_DeveloperMode) {

        if (input == (KN_F2|KN_CTRL_BIT)) {
            ScenarioInit = 0;

            /*
            ** If Changed is set, prompt for saving changes.
            */
            if (IsMapChanged) {
                rc = WWMessageBox().Process("Save Changes?", TXT_YES, TXT_NO);
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
                 *  If we're in game mode, set Changed to 0 (so if we didn't save our
                 *  changes above, they won't keep coming back to haunt us with continual
                 *  Save Changes? prompts!)
                 */
                if (!Debug_Map) {
                    IsMapChanged = false;
                }
                //BaseGauge->Set_Value(Scen->Percent);
                Go_Editor(!Debug_Map);
            }
        }
    }

    /**
     *  For normal game mode, jump to the parent's AI routine.
     */
    if (!Debug_Map) {
        MouseClass::AI(input, xy);
        return;
    }

#if 0
    /**
     *  Do special mouse processing if the mouse is over the map.
     */
    if (WWMouse->Get_Mouse_X() > TacPixelX && WWMouse->Get_Mouse_X() <
        TacticalRect.X + Lepton_To_Pixel(TacLeptonWidth) &&
        WWMouse->Get_Mouse_Y() > TacPixelY && WWMouse->Get_Mouse_Y() <
        TacticalRect.Y + Lepton_To_Pixel(TacLeptonHeight)) {

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
#endif

#if 0
    /**
     *  Set 'ZoneCell' to track the mouse cursor around over the map. Do this
     *  even if the map is scrolling.
     */
    if (WWMouse->Get_Mouse_X() >= TacPixelX && WWMouse->Get_Mouse_X() <=
        TacPixelX + Lepton_To_Pixel(TacLeptonWidth) &&
        WWMouse->Get_Mouse_Y() >= TacPixelY && WWMouse->Get_Mouse_Y() <=
        TacPixelY + Lepton_To_Pixel(TacLeptonHeight)) {

        cell = Click_Cell_Calc(WWMouse->Get_Mouse_X(), WWMouse->Get_Mouse_Y());
        if (cell != -1) {
            Set_Cursor_Pos(cell);
            if (PendingObject) {
                Flag_To_Redraw(true);
            }
        }
    }
#endif

    /**
     *  Check for mouse motion while left button is down.
     */
    if (IsLeftMouseDown && Mouse_Moved()) {

        /**
         *  "Paint" mode: place current object, and restart placement.
         */
        if (PendingObject) {
            Flag_To_Redraw(true);
            if (Place_Object() == 0) {
                IsMapChanged = true;
                Start_Placement();
            }
        } else {

            /**
             *  Move the currently-grabbed object.
             */
            if (GrabbedObject) {
                GrabbedObject->Mark(MARK_CHANGE);
                if (Move_Grabbed_Object() == 0) {
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
            break;

        /**
         *  F2:
         * 
         *  
         */
        case KN_F2:
            break;

        /**
         *  F3:
         * 
         *  
         */
        case KN_F3:
            break;

        /**
         *  F4:
         * 
         *  
         */
        case KN_F4:
            break;

        /**
         *  F5:
         * 
         *  
         */
        case KN_F5:
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
                    //Cancel_Base_Building();
                } else {
                    //Cancel_Placement();
                }
                input = KN_NONE;
                break;
            }

            /**
             *  Exit trigger placement mode.
             */
            if (CurTrigger) {
                //Stop_Trigger_Placement();
                input = KN_NONE;
                break;
            }

            /**
             *  Exit tag placement mode.
             */
            if (CurTag) {
                //Stop_Tag_Placement();
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
                rc = WWMessageBox().Process("Save Changes?", 0, TXT_YES, TXT_NO);
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
            Fatal(EXIT_SUCCESS, "Map Editor");
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
                    //Popup_Controls();
                }

                /**
                 *  Force a redraw.
                 */
                HiddenSurface->Clear();
                Map.Flag_To_Redraw(true);

                IsMapChanged = true;

            } else {

                /**
                 *  Remove trigger from current cell.
                 */
                if (CurrentCell) {
                    if (Map[CurrentCell].CellTag) {
                        Map[CurrentCell].CellTag = nullptr;
//                        CellTriggers[CurrentCell] = nullptr;

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
         *  CTRL+ALT+M:
         * 
         *  Toggle marble madness framework mode.
         */
        case KN_CTRL_BIT|KN_ALT_BIT|KN_M:
            if (IsMarbleMadnessAvailable) {
                DEBUG_INFO("Toggle marble madness mode.\n");
            }
            break;

    };

}


void MapEditClass::_Draw_It(bool forced)
{
}


void MapEditClass::_Detach(TARGET target, bool all)
{
    MouseClass::Detach(target, all);

//    if (GrabbedObject == target) {
//        GrabbedObject = nullptr;
//    }
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
//            Map[Coord_Cell(GrabbedObject->Coord)].Flag.Composite &=
//                ~(1 << CellClass::Spot_Index(GrabbedObject->Coord));
        } else {
            new_coord = Coordinate(0,0,0);
        }

    } else {

#if 0
        /*------------------------------------------------------------------------
        Non-infantry: use cell's center coordinate
        ------------------------------------------------------------------------*/
        new_coord = Cell_Coord(ZoneCell + GrabOffset);

        if (GrabbedObject->What_Am_I() == RTTI_BUILDING ||
            GrabbedObject->What_Am_I() == RTTI_TERRAIN) {

            new_coord &= 0xFF00FF00L;
        }
#endif

        /*
        ................ Try to place object at new coordinate ................
        */
        if (GrabbedObject->Can_Enter_Cell(&(*this)[new_coord]) != MOVE_OK) {
            new_coord = Coordinate(0,0,0);
        }
    }
    if (new_coord) {
        /*
        ** If this object is part of the AI's Base list, change the coordinate
        ** in the Base's Node list.
        */
        //if (GrabbedObject->What_Am_I()==RTTI_BUILDING && Base.Get_Node((BuildingClass *)GrabbedObject)) {
        //    Base.Get_Node((BuildingClass *)GrabbedObject)->Coord = new_coord;
        //}

        GrabbedObject->Coord = new_coord;
        retval = true;
    }
    GrabbedObject->Mark(MARK_DOWN);

    /*------------------------------------------------------------------------
    For infantry, set the bit in its new cell marking that spot as occupied.
    ------------------------------------------------------------------------*/
    if (GrabbedObject->Is_Infantry()) {
        ((InfantryClass *)GrabbedObject)->Set_Occupy_Bit(new_coord);
//        Map[Coord_Cell(new_coord)].Flag.Composite |=
//            (1 << CellClass::Spot_Index(new_coord));
    }

    /*------------------------------------------------------------------------
    Re-select the object, and reset the mouse pointer
    ------------------------------------------------------------------------*/
    Set_Default_Mouse(MOUSE_NORMAL);
    Override_Mouse_Shape(MOUSE_NORMAL);

    Flag_To_Redraw(true);

    return retval;
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
    **    Placing a template:
    **    - first lift up any objects in the cell
    **    - place the template, and try to replace the objects; if they won't go
    **      back, the template can't go there
    */
    //ScenarioInit++;
    if (PendingObject->What_Am_I() == RTTI_TEMPLATETYPE) {

        /*
        **    Loop through all cells this template will occupy
        */
        okflag = true;
        occupy = PendingObject->Occupy_List();
        while ((*occupy) != REFRESH_EOL) {

            /*
            **    Check this cell for an occupier
            */
            template_cell = (ZoneCell+ZoneOffset) + (*occupy);
            if ((*this)[template_cell].Cell_Occupier()) {
                occupier = (*this)[template_cell].Cell_Occupier();

                /*
                **    Save object's coordinates
                */
                obj_coord = occupier->Coord;

                /*
                **    Place the object in limbo
                */
                occupier->Mark(MARK_UP);

                /*
                **    Set the cell's template values
                */
                save_ttype = (*this)[template_cell].TType;
                save_ticon = (*this)[template_cell].TIcon;
                (*this)[template_cell].TType =
                    ((TemplateTypeClass *)PendingObject)->Type;
                (*this)[template_cell].TIcon = Cell_X(*occupy) + Cell_Y(*occupy) *
                    ((TemplateTypeClass *)PendingObject)->Width;
                (*this)[template_cell].Recalc_Attributes();

                /*
                **    Try to put the object back down
                */
                if (occupier->Can_Enter_Cell(Coord_Cell(obj_coord)) != MOVE_OK) {
                    okflag = false;
                }

                /*
                **    Put everything back the way it was
                */
                (*this)[template_cell].TType = save_ttype;
                (*this)[template_cell].TIcon = save_ticon;
                (*this)[template_cell].Recalc_Attributes();

                /*
                **    Major error if can't replace the object now
                */
                occupier->Mark(MARK_DOWN);
            }
            occupy++;
        }

        /*
        **    If it's still OK after ALL THAT, place the template
        */
        if (okflag) {
            if (PendingObjectPtr->Unlimbo(Cell_Coord(ZoneCell + ZoneOffset))) {

                /*
                **    Loop through all cells occupied by this template, and clear the
                **    smudge & overlay.
                */
                occupy = PendingObject->Occupy_List();
                while ((*occupy) != REFRESH_EOL) {

                    /*
                    **    Get cell for this occupy item
                    */
                    template_cell = (ZoneCell+ZoneOffset) + (*occupy);

                    /*
                    **    Clear smudge & overlay
                    */
                    (*this)[template_cell].Overlay = OVERLAY_NONE;
                    (*this)[template_cell].OverlayData = 0;
                    (*this)[template_cell].Smudge = SMUDGE_NONE;

                    /*
                    **    make adjacent cells recalc attrib's
                    */
                    (*this)[template_cell].Recalc_Attributes();
                    (*this)[template_cell].Wall_Update();
                    (*this)[template_cell].Concrete_Calc();

                    occupy++;
                }

                /*
                **    Set flags etc
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
            **    Failure to deploy results in a returned failure code.
            */
            //ScenarioInit--;
            return false;
        }

        /*
        **    Not OK; return error
        */
        //ScenarioInit--;
        return false;
    }
#endif

    /*
    **    Placing infantry: Infantry can go into cell sub-positions, so find the
    **    sub-position closest to the mouse & put him there
    */
    if (PendingObject->What_Am_I() == RTTI_INFANTRYTYPE) {

#if 0
        /*
        **    Find cell sub-position
        */
        if (Is_Spot_Free(Pixel_To_Coord(Get_Mouse_X(), Get_Mouse_Y()))) {
            obj_coord = Closest_Free_Spot(Pixel_To_Coord(Get_Mouse_X(), Get_Mouse_Y()));
        } else {
            obj_coord = Coordinate(0,0,0);
        }
#endif

        /*
        **    No free spots; don't place the object
        */
        if (!obj_coord) {
            //ScenarioInit--;
            return false;
        }

        /*
        **    Unlimbo the object
        */
        if (PendingObjectPtr->Unlimbo(obj_coord)) {
            ((InfantryClass *)PendingObjectPtr)->Set_Occupy_Bit(obj_coord);
//            Map[obj_coord].Flag.Composite |=
//                (1 << CellClass::Spot_Index(obj_coord));
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
    **    Placing an object
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
//            node.Type = ((BuildingTypeClass *)PendingObject)->Type;
//            node.Cell = Coord_Cell(PendingObjectPtr->Coord);
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
 *  Enters placement mode.
 */
void MapEditClass::Start_Placement()
{
#if 0
    /**
     *  Initialize addable objects list; we must do this every time in case one
     *  of the object pools has become exhausted; that object won't be available
     *  for adding. These must be added in the same order expected by the
     *  object selection dialog (same as button order).
     */
    Clear_List();
    IsometricTileTypeClass::Prep_For_Add();
    OverlayTypeClass::Prep_For_Add();
    SmudgeTypeClass::Prep_For_Add();
    TerrainTypeClass::Prep_For_Add();
    UnitTypeClass::Prep_For_Add();
    InfantryTypeClass::Prep_For_Add();
    BuildingTypeClass::Prep_For_Add();
    AircraftTypeClass::Prep_For_Add();

    /*
    **    Compute offset of each class type in the Objects array
    */
    TypeOffset[0] = 0;
    for (int i = 1; i < NUM_EDIT_CLASSES; i++) {
        TypeOffset[i] = TypeOffset[i-1] + NumType[i-1];
    }

    /*
    **    Create the placement object:
    **    - For normal placement mode, use the last-used index into Objects
    **      (LastChoice), and the last-used house (LastHouse).
    **    - For base-building mode, force the object to be a building, and use the
    **      House specified in the Base object
    */
    if (!BaseBuilding) {
        if (LastChoice >= ObjCount) {
            LastChoice = ObjCount - 1;
        }
        PendingObject = Objects[LastChoice];
        PendingHouse = LastHouse;
        PendingObjectPtr = PendingObject->Create_One_Of(HouseClass::As_Pointer(LastHouse));
    } else {
        if (LastChoice < TypeOffset[7]) {
            LastChoice = TypeOffset[7];
        }
        if (LastChoice >= ObjCount) {
            LastChoice = ObjCount - 1;
        }
        PendingObject = Objects[LastChoice];
        PendingHouse = LastHouse = Base.House;
        PendingObjectPtr = PendingObject->Create_One_Of(HouseClass::As_Pointer(LastHouse));
    }

    /*
    **    Error if no more objects available
    */
    if (!PendingObjectPtr) {
        WWMessageBox().Process("No more objects of this type available.");
        HidPage.Clear();
        Flag_To_Redraw(true);
        Render();
        PendingObject = NULL;
        if (BaseBuilding) {
            Cancel_Base_Building();
        }
        return;
    }
#endif

    /*
    **    Set the placement cursor
    */
    Set_Cursor_Pos();
    Set_Cursor_Shape(PendingObject->Occupy_List());
}




#if 0
/*...........................................................................
These are the button ID's for the pop-up object-editing gizmos.
The house button ID's must be sequential, with a 1-to-1 correspondence to
the HousesType values.
...........................................................................*/
enum MapEditButtonIDEnum
{
    //POPUP_GDI = 500,       // GDI house button
    //POPUP_NOD,             // NOD house button
    //POPUP_NEUTRAL,         // Neutral house button
    //POPUP_HOUSE_JP,        // not used
    //POPUP_MULTI1,          // Multiplayer 1 house button
    //POPUP_MULTI2,          // Multiplayer 2 house button
    //POPUP_MULTI3,          // Multiplayer 3 house button
    //POPUP_MULTI4,          // Multiplayer 4 house button
    //POPUP_MULTI5,          // Multiplayer 4 house button
    //POPUP_MULTI6,          // Multiplayer 4 house button
    //POPUP_MISSIONLIST,     // list box for missions
    POPUP_HEALTHGAUGE,     // health of object
    POPUP_FACINGDIAL,      // object's facing
    //POPUP_BASEPERCENT,     // Base's percent-built slider
    MAP_AREA,              // map as a click-able thingy
    BUTTON_FLAG = 0x8000
};

enum MapEditPositionEnum
{
//    POPUP_GDI_W = 100,
//    POPUP_GDI_H = 18,
//    POPUP_GDI_X = 20,
//    POPUP_GDI_Y = 320,

//    POPUP_NOD_W = 100,
//    POPUP_NOD_H = 18,
//    POPUP_NOD_X = 20,
//    POPUP_NOD_Y = 338,

//    POPUP_NEUTRAL_W = 100,
//    POPUP_NEUTRAL_H = 18,
//    POPUP_NEUTRAL_X = 20,
//    POPUP_NEUTRAL_Y = 356,

//    POPUP_MULTI1_W = 50,
//    POPUP_MULTI1_H = 18,
//    POPUP_MULTI1_X = 20,
//    POPUP_MULTI1_Y = 320,

//    POPUP_MULTI2_W = 50,
//    POPUP_MULTI2_H = 18,
//    POPUP_MULTI2_X = 70,
//    POPUP_MULTI2_Y = 320,

//    POPUP_MULTI3_W = 50,
//    POPUP_MULTI3_H = 18,
//    POPUP_MULTI3_X = 20,
//    POPUP_MULTI3_Y = 330,

//    POPUP_MULTI4_W = 50,
//    POPUP_MULTI4_H = 18,
//    POPUP_MULTI4_X = 70,
//    POPUP_MULTI4_Y = 338,

//    POPUP_MISSION_W = 160,
//    POPUP_MISSION_H = 80,
//    POPUP_MISSION_X = 140,
//    POPUP_MISSION_Y = 300,

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
    POPUP_BASE_X = 600 - POPUP_BASE_W,
    POPUP_BASE_Y = 0,
};

static char HealthBuf[20];



void MapEditClass::One_Time()
{
    /**
     *  The map: a single large "button".
     */
    MapArea = new ControlClass(MAP_AREA,
        TacticalRect.X, TacticalRect.Y+16, TacticalRect.Width, TacticalRect.Height-16,
        GadgetClass::LEFTPRESS|GadgetClass::LEFTRELEASE, false);

    /*
    **    House buttons
    */
#if 0
    HouseList = new ListClass(POPUP_HOUSELIST, POPUP_HOUSE_X, POPUP_HOUSE_Y, POPUP_HOUSE_W, POPUP_HOUSE_H,
        TPF_EFNT|TPF_NOSHADOW,
        MFCD::Retrieve("EBTN-UP.SHP"),
        MFCD::Retrieve("EBTN-DN.SHP"));
    for (HousesType house = HOUSE_FIRST; house < HOUSE_COUNT; house++) {
        HouseList->Add_Item(HouseTypeClass::As_Reference(house).IniName);
    }
#endif

    /*
    **    The mission list box
    */
#if 0
    MissionList = new ListClass(POPUP_MISSIONLIST,
        POPUP_MISSION_X, POPUP_MISSION_Y, POPUP_MISSION_W, POPUP_MISSION_H,
        TPF_EFNT|TPF_NOSHADOW,
        MFCD::Retrieve("EBTN-UP.SHP"),
        MFCD::Retrieve("EBTN-DN.SHP"));
    for (int i = 0; i < NUM_EDIT_MISSIONS; i++) {
        MissionList->Add_Item(MissionClass::Mission_Name(MapEditMissions[i]));
    }
#endif

    /**
     *  The object health bar.
     */
    HealthGauge = new TriColorGaugeClass(POPUP_HEALTHGAUGE,
        POPUP_HEALTH_X, POPUP_HEALTH_Y, POPUP_HEALTH_W, POPUP_HEALTH_H);
    HealthGauge->Use_Thumb(true);
    HealthGauge->Set_Maximum(256);
    HealthGauge->Set_Red_Limit(63 - 1);
    HealthGauge->Set_Yellow_Limit(127 - 1);

    /*
    **    The health text label
    */
#if 0
    HealthBuf[0] = 0;
    HealthText = new TextLabelClass(HealthBuf,
        POPUP_HEALTH_X + POPUP_HEALTH_W / 2,
        POPUP_HEALTH_Y + POPUP_HEALTH_H + 1,
        GadgetClass::Get_Color_Scheme(),
        TPF_CENTER | TPF_FULLSHADOW | TPF_EFNT);
#endif

    /*
    **    Building attribute buttons.
    */
#if 0
    Sellable = new TextButtonClass(POPUP_SELLABLE, TXT_SELLABLE, TPF_EBUTTON, 320-65, 200-25, 60);
    Rebuildable = new TextButtonClass(POPUP_REBUILDABLE, TXT_REBUILD, TPF_EBUTTON, 320-65, 200-15, 60);
#endif

    /**
     *  The unit facing dial.
     */
    FacingDial = new Dial8Class(POPUP_FACINGDIAL, POPUP_FACEBOX_X,
        POPUP_FACEBOX_Y, POPUP_FACEBOX_W, POPUP_FACEBOX_H, DIR_N);

    /*
    **    The base percent-built slider & its label
    */
#if 0
    BaseGauge = new GaugeClass(POPUP_BASEPERCENT, POPUP_BASE_X, POPUP_BASE_Y, POPUP_BASE_W, POPUP_BASE_H);
    BaseLabel = new TextLabelClass ("Base:", POPUP_BASE_X - 3, POPUP_BASE_Y, GadgetClass::Get_Color_Scheme(),
        TPF_RIGHT | TPF_NOSHADOW | TPF_EFNT);
    BaseGauge->Set_Maximum(100);
    BaseGauge->Set_Value(Scen->Percent);
#endif
}


/**
 *  
 * 
 *  @author: CCHyper
 */
void MapEditClass::Input(KeyNumType &input, Point2D &xy)
{
    if (!Debug_Map) {
        return;
    }



#if 0
    /*
    **    Trap 'F2' regardless of whether we're in game or editor mode
    */
    if ((input == KN_F2 && Session.Type == GAME_NORMAL) || input == (KN_F2 | KN_CTRL_BIT)) {
        ScenarioInit = 0;

        /*
        ** If Changed is set, prompt for saving changes.
        */
        if (Changed) {
            rc = WWMessageBox().Process("Save Changes?", TXT_YES, TXT_NO);
            HiddenSurface->Clear();
            Flag_To_Redraw(true);
            Render();

            /*
            **    User wants to save
            */
            if (rc == 0) {

                /*
                **    If save cancelled, abort game
                */
                if (Save_Scenario()!=0) {
                    input = KN_NONE;
                } else {
                    Changed = false;
                    Go_Editor(!Debug_Map);
                }
            } else {

                /*
                **    User doesn't want to save
                */
                Go_Editor(!Debug_Map);
            }
        } else {
            /*
            ** If we're in game mode, set Changed to 0 (so if we didn't save our
            ** changes above, they won't keep coming back to haunt us with continual
            ** Save Changes? prompts!)
            */
            if (!Debug_Map) {
                Changed = false;
            }
            BaseGauge->Set_Value(Scen.Percent);
            Go_Editor(!Debug_Map);
        }
    }


    /*
    **    Do special mouse processing if the mouse is over the map
    */
    if (Get_Mouse_X() > TacPixelX && Get_Mouse_X() <
        TacPixelX + Lepton_To_Pixel(TacLeptonWidth) &&
        Get_Mouse_Y() > TacPixelY && Get_Mouse_Y() <
        TacPixelY + Lepton_To_Pixel(TacLeptonHeight)) {

        /*
        **    When the mouse moves over a scrolling edge, ScrollClass changes its
        **    shape to the appropriate arrow or NO symbol; it's our job to change it
        **    back to normal (or whatever the shape is set to by Set_Default_Mouse())
        **    when it re-enters the map area.
        */
        if (CellTag) {
            Override_Mouse_Shape(MOUSE_CAN_MOVE);
        } else {
            Override_Mouse_Shape(MOUSE_NORMAL);
        }
    }



    /*
    **    Set 'ZoneCell' to track the mouse cursor around over the map.  Do this
    **    even if the map is scrolling.
    */
    if (Get_Mouse_X() >= TacPixelX && Get_Mouse_X() <=
        TacPixelX + Lepton_To_Pixel(TacLeptonWidth) &&
        Get_Mouse_Y() >= TacPixelY && Get_Mouse_Y() <=
        TacPixelY + Lepton_To_Pixel(TacLeptonHeight)) {

        cell = Click_Cell_Calc(Get_Mouse_X(), Get_Mouse_Y());
        if (cell != -1) {
            Set_Cursor_Pos(cell);
            if (PendingObject) {
                Flag_To_Redraw(true);
            }
        }
    }



    /*
    **    Check for mouse motion while left button is down.
    */
    rc = Mouse_Moved();
    if (LMouseDown && rc) {

        /*
        **    "Paint" mode: place current object, and restart placement
        */
        if (PendingObject) {
            Flag_To_Redraw(true);
            if (Place_Object() == 0) {
                Changed = true;
                Start_Placement();
            }
        } else {

            /*
            **    Move the currently-grabbed object
            */
            if (GrabbedObject) {
                GrabbedObject->Mark(MARK_CHANGE);
                if (Move_Grabbed_Object() == 0) {
                    Changed = true;
                }
            }
        }
    }
#endif







    /*
    **    Trap special editing keys; if one is detected, set 'input' to 0 to
    **    prevent a conflict with parent's AI().
    */
    switch (input) {

        /**
         *  F1
         * 
         *  Show help.
         */
        case KN_F1:
            break;


#if 0
        /*
        ** F2/RMOUSE = pop up main menu
        */
        case KN_RMOUSE:

            /*
            **    Turn off placement mode
            */
            if (PendingObject) {
                if (BaseBuilding) {
                    Cancel_Base_Building();
                } else {
                    Cancel_Placement();
                }
            }

            /*
            **    Turn off trigger placement mode
            */
            if (CurTrigger) {
                Stop_Trigger_Placement();
            }

            /*
            **    Unselect object & hide popup controls
            */
            if (CurrentObject.Count()) {
                CurrentObject[0]->Unselect();
                Popup_Controls();
            }
            Main_Menu();
            input = KN_NONE;
            break;
#endif


        /*
        **    F6 = toggle passable/impassable display
        */
        case KN_F6:
            Debug_Passable = !Debug_Passable;
            HiddenSurface->Clear();
            Map.Flag_To_Redraw(true);
            input = KN_NONE;
            break;


        /*
        **    INSERT = go into object-placement mode
        */
        case KN_INSERT:
        {
            if (!Map.PendingObject) {

                /*
                **    Unselect current object, hide popup controls
                */
                if (CurrentObjects.Count()) {
                    CurrentObjects.Fetch_Head()->Unselect();
                    //Popup_Controls();
                }

                /*
                **    Go into placement mode
                */
                Start_Placement();
            }
            input = KN_NONE;
            break;
        }


        /*
        **    ESC = exit placement mode, or exit to DOS
        */
        case KN_ESC:
        {
            /*
            **    Exit object placement mode
            */
            if (Map.PendingObject) {
                //if (BaseBuilding) {
                //    Cancel_Base_Building();
                //} else {
                    Cancel_Placement();
                //}
                input = KN_NONE;
                break;
            } else {

                /*
                **    Exit trigger placement mode
                */
                //if (CurTrigger) {
                //    Stop_Trigger_Placement();
                //    input = KN_NONE;
                //    break;
                //} else {
                    int rc = WWMessageBox().Process("Exit Scenario Editor?", 0, TXT_YES, TXT_NO);
                    HiddenSurface->Clear();
                    Map.Flag_To_Redraw(true);
                    Map.Render();

                    /*
                    **    User doesn't want to exit; return to editor
                    */
                    if (rc == 1) {
                        input = KN_NONE;
                        break;
                    }

                    /*
                    **    If changed, prompt for saving
                    */
                    if (Changed) {
                        rc = WWMessageBox().Process("Save Changes?", 0, TXT_YES, TXT_NO);
                        HiddenSurface->Clear();
                        Map.Flag_To_Redraw(true);
                        Map.Render();

                        /*
                        **    User wants to save
                        */
                        if (rc == 0) {

                            /*
                            **    If save cancelled, abort exit
                            */
                            if (Save_Scenario()) {
                                input = KN_NONE;
                                break;
                            } else {
                                Changed = false;
                            }
                        }
                    }
                //}
            }
            //Prog_End();
            //Emergency_Exit(0);
            break;
        }


        /*
        **    LEFT = go to previous placement object
        */
        case KN_LEFT:
            if (Map.PendingObject) {
                Place_Prev();
            }
            input = KN_NONE;
            break;


        /*
        **    RIGHT = go to next placement object
        */
        case KN_RIGHT:
            if (Map.PendingObject) {
                Place_Next();
            }
            input = KN_NONE;
            break;


        /*
        **    PGUP = go to previous placement category
        */
        case KN_PGUP:
            if (Map.PendingObject) {
                Place_Prev_Category();
            }
            input = KN_NONE;
            break;


        /*
        **    PGDN = go to next placement category
        */
        case KN_PGDN:
            if (Map.PendingObject) {
                Place_Next_Category();
            }
            input = KN_NONE;
            break;


#if 0
        /*
        **    ALT-Space: Remove a waypoint designation
        */
        case ((int)KN_SPACE | (int)KN_ALT_BIT):
            if (CurrentCell != 0) {

                /*
                **    Loop through letter waypoints; if this cell is one of them,
                **    clear that waypoint.
                */
                for (i = 0 ; i < WAYPT_HOME; i++) {
                    if (Scen.Waypoint[i]==CurrentCell)
                        Scen.Waypoint[i] = -1;
                }

                /*
                **    Loop through flag home values; if this cell is one of them, clear
                **    that waypoint.
                */
                for (i = 0; i < MAX_PLAYERS; i++) {
                    house = (HousesType)(HOUSE_MULTI1 + i);
                    if (HouseClass::As_Pointer(house) &&
                        CurrentCell == HouseClass::As_Pointer(house)->FlagHome)
                    HouseClass::As_Pointer(house)->Flag_Remove(As_Target(CurrentCell), true);
                }

                /*
                **    If there are no more waypoints on this cell, clear the cell's
                **    waypoint designation.
                */
                if (Scen.Waypoint[WAYPT_HOME]!=CurrentCell &&
                    Scen.Waypoint[WAYPT_REINF]!=CurrentCell)
                    Map[CurrentCell].IsWaypoint = 0;
                Changed = true;
                Flag_Cell(CurrentCell);
            }
            input = KN_NONE;
            break;
#endif


        /*
        **    'H' = toggle current placement object's house
        */
        case KN_H:
        case ((int)KN_H | (int)KN_SHIFT_BIT):
            if (Map.PendingObject) {
                //Toggle_House();
            }
            input = KN_NONE;
            break;


        /**
         *    SHIFT-ALT-Arrow:
         *
         *  Move the current object.
         */
        case (KN_UP|KN_ALT_BIT|KN_SHIFT_BIT):
        case (KN_DOWN|KN_ALT_BIT|KN_SHIFT_BIT):
        case (KN_LEFT|KN_ALT_BIT|KN_SHIFT_BIT):
        case (KN_RIGHT|KN_ALT_BIT|KN_SHIFT_BIT):
            if (CurrentObjects.Count()) {
                CurrentObjects.Fetch_Head()->Move(KN_To_Facing(input)-1);
                Changed = true;
            }
            input = KN_NONE;
            break;


        /**
         *    DELETE:
         *
         *  Delete currently-selected object.
         */
        case KN_DELETE:
        {
            /*
            **    Delete currently-selected object's trigger, or the object
            */
            if (CurrentObjects.Count()) {

                /*
                **    Delete trigger
                */
                if (CurrentObjects.Fetch_Head()->Tag) {
                    CurrentObjects.Fetch_Head()->Tag = nullptr;

                } else {
                    /*
                    ** If the current object is part of the AI's Base, remove it
                    ** from the Base's Node list.
                    */
#if 0
                    if (CurrentObjects.Fetch_Head()->What_Am_I() == RTTI_BUILDING &&
                        Base.Is_Node((BuildingClass *)CurrentObjects.Fetch_Head())) {
                        node = Base.Get_Node((BuildingClass *)CurrentObjects.Fetch_Head());
                        Base.Nodes.Delete(*node);
                    }
#endif

                    /*
                    **    Delete current object
                    */
                    delete CurrentObjects.Fetch_Head();

                    /*
                    **    Hide the popup controls
                    */
                    //Popup_Controls();
                }

                /*
                **    Force a redraw
                */
                HiddenSurface->Clear();
                Map.Flag_To_Redraw(true);

                Changed = true;

            } else {

#if 0
                /*
                **    Remove trigger from current cell
                */
                if (CurrentCell) {
                    if (Map[CurrentCell].Trigger.Is_Valid()) {
                        Map[CurrentCell].Trigger = nullptr;
//                        CellTriggers[CurrentCell] = nullptr;

                        /*
                        **    Force a redraw
                        */
                        HiddenSurface->Clear();
                        Flag_To_Redraw(true);
                        Changed = true;
                    }
                }
#endif
            }
            input = KN_NONE;
            break;
        }


#if 0
        /*
        **    TAB: select next object on the map
        */
        case KN_TAB:
            Select_Next();
            input = KN_NONE;
            break;
#endif

    };

}




#endif

