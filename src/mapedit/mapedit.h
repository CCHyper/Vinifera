#pragma once

#include "iomap.h"


class TriggerClass;
class Point2D;
class TextLabelClass;
class TextButtonClass;
class TriColorGaugeClass;
class Dial8Class;
class ListClass;
class GaugeClass;


/**
 *  This class is derived from the normal display map class. It exists
 *  only to allow editing and adding items to the map.
 * 
 *  #NOTE:
 *  Even though this class inherits from the io map stack, you must access
 *  the map layer with the "Map" global! The inheritance is just for creation
 *  of the virtuals so we can hook them back into the table.
 */
class MapEditClass : private MouseClass
{
    public:
        MapEditClass() {}
        virtual ~MapEditClass() {}

        /**
         *  The intended virtual overrides must 
         */
        void _One_Time();
        void _Init_IO();
        void _AI(KeyNumType &input, Point2D &xy);
        void _Draw_It(bool forced = false);
        void _Detach(TARGET target, bool all = true);
        bool _Scroll_Map(FacingType facing, int &distance, bool really = true);
        
        static bool New_Scenario();
        static bool Load_Scenario(const char *filename = nullptr);
        static bool Save_Scenario(bool emergency = false);
        static bool Size_Map(int x, int y, int w, int h);

    protected:
        bool Mouse_Moved();
        bool Move_Grabbed_Object();
        void Grab_Object();
        bool Place_Object();
        void Place_Home();
        void Place_Next();
        void Place_Prev();
        void Place_Next_Category();
        void Place_Prev_Category();
        void Start_Placement();
        void Start_Tag_Placement();
        void Stop_Tag_Placement();
        void Place_Tag();
        void Cancel_Placement();
        void Cancel_Base_Building();
        void Build_Base_To(int percent);
        bool Select_Object();
        void Select_Next();
        void Select_Prev();
        bool Verify_House(HousesType house, const ObjectTypeClass *objtype);
        void Toggle_House();
        bool Change_House(HousesType newhouse);
        HousesType Cycle_House(HousesType curhouse, const ObjectTypeClass *objtype);
        bool Get_Waypoint_Name(char wayptname[]);
        void Update_Waypoint(int waypt_idx);
        void Object_Display(const ObjectTypeClass *objtype, Rect &window, Point2D &xy, HousesType house);
        void Popup_Controls();
        void Context_Menu(Point2D &mouse_xy);
        bool Placement_Dialog();

    public:
        static void Clear_List();
        static bool Add_To_List(const ObjectTypeClass *object);

    private:
        void IsometricTileType_Prep_For_Add();
        void TerrainType_Prep_For_Add();
        void OverlayType_Prep_For_Add();
        void SmudgeType_Prep_For_Add();
        void UnitType_Prep_For_Add();
        void InfantryType_Prep_For_Add();
        void BuildingType_Prep_For_Add();
        void AircraftType_Prep_For_Add();

    public:

        /**
         *  All class members must be static!
         */

        /**
         *  Variables for supporting the object-editing controls at screen bottom.
         */
        static ControlClass *MapArea;
        static ListClass *HouseList;
        static ListClass *MissionList;
        static TriColorGaugeClass *HealthGauge;
        static Dial8Class *FacingDial;
        static TextLabelClass *HealthText;
        static TextButtonClass *Sellable;
        static TextButtonClass *Rebuildable;
        static TextButtonClass *Nominal;
        static TextButtonClass *Repairable;
        static GaugeClass *BaseGauge;
        static TextLabelClass *BaseLabel;

        /**
         *  The currently-selected cell for the Scenario Editor.
         */
        static Cell CurrentCell;

        /**
         *  
         */
        //static TriggerClass *CurTrigger;
        static TagClass *CurTag;

        /**
         *  
         */
        static TeamClass *CurTeam;

        /**
         *  Object "grabbed" with mouse.
         */
        static ObjectClass *GrabbedObject;

        /**
         *  Offset to grabbed obj's upper-left.
         */
        static Cell GrabOffset;

        /**
         *  Array of all TypeClasses the user can add to the map.
         */
        //static const ObjectTypeClass * Objects[MAX_EDIT_OBJECTS];
        static DynamicVectorClass<const ObjectTypeClass * > AvailableObjects; // was Objects

        /**
         *  # of objects in the Objects array.
         */
        //static int ObjCount;

        /*.....................................................................
        Number of each type of object in Objects, so we can switch categories
        .....................................................................*/
        static int NumType[8 /*NUM_EDIT_CLASSES*/];		// # of each type of class:
                                                        // 0 = IsoTile
                                                        // 1 = Overlay
                                                        // 2 = Smudge
                                                        // 3 = Terrain
                                                        // 4 = Unit
                                                        // 5 = Infantry
                                                        // 6 = Aircraft
                                                        // 7 = Building

        /*.....................................................................
        The offset of each type of object within the Objects[] array
        .....................................................................*/
        static int TypeOffset[8 /*NUM_EDIT_CLASSES*/];	// offsets within Objects[]

        /**
         *  Index of item user last-selected to place.
         */
        static int LastChoice;

        /**
         *  Last-selected house of object.
         */
        static HousesType LastHouse;

        /**
         *  Time of last LEFT-MOUSE click.
         */
        static unsigned LastClickTime;

        /**
         *  
         */
        static bool IsLeftMouseDown;

        /**
         *  Are there unsaved changes to the scenario?
         */
        static bool IsMapChanged;

        /**
         *  
         */
        static bool IsBaseBuilding;

        /**
         *  
         */
        static bool IsMarbleMadnessEnabled;
        
        /**
         *  
         */
        static bool IsShowHelp;

        /**
         *  
         */
        static bool IsShowOccupiers;
        static bool IsShowWaypoints;
        static bool IsShowCellTags;
};


extern MapEditClass MapEdit;
