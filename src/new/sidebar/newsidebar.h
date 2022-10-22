/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : SIDEBAR.H                                                    *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : October 20, 1994                                             *
 *                                                                                             *
 *                  Last Update : October 20, 1994   [JLB]                                     *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#pragma once

#include "always.h"
#include "power.h"
#include "factory.h"
#include "gcntrl.h"
#include "shapebtn.h"


class Straw;
class Pipe;
struct ShapeFileStruct;
class FactoryClass;


#define SIDEBAR_WID 999


class NewSidebarClass : public PowerClass
{
    public:
        /*
        **    These constants are used to control the sidebar rendering. They are instantiated
        **    as enumerations since C++ cannot use "const" in this context.
        */
        enum SideBarClassEnums {
            BUTTON_ACTIVATOR=100,            // Button ID for the activator.
            SIDE_X=320-80,                    // The X position of sidebar upper left corner.
            SIDE_Y=7+70,    // The Y position of sidebar upper left corner.
            SIDE_WIDTH=SIDEBAR_WID,            // Width of the entire sidebar (in pixels).
            SIDE_HEIGHT=200-(7+70),            // Height of the entire sidebar (in pixels).
            TOP_HEIGHT=13,                        // Height of top section (with repair/sell buttons).
            COLUMN_ONE_X=(320-80)+8,            // Sidestrip upper left coordinates...
            COLUMN_ONE_Y=int(SIDE_Y)+int(TOP_HEIGHT),
            COLUMN_TWO_X=(320-80)+8+((80-16)/2)+3,
            COLUMN_TWO_Y=7+70+13,

//BGA: changes to all buttons
            BUTTON_ONE_WIDTH=32,                // Button width.
            BUTTON_TWO_WIDTH=20,                // Button width.
            BUTTON_THREE_WIDTH=20,            // Button width.
            BUTTON_HEIGHT=9,                    // Button height.
            BUTTON_ONE_X=(int)SIDE_X+2,            // Left button X coordinate.
            BUTTON_ONE_Y=(int)SIDE_Y+2,            // Left button Y coordinate.
            BUTTON_TWO_X=(int)SIDE_X+36,            // Right button X coordinate.
            BUTTON_TWO_Y=(int)SIDE_Y+2,            // Right button Y coordinate.
            BUTTON_THREE_X=(int)SIDE_X+58,            // Right button X coordinate.
            BUTTON_THREE_Y=(int)SIDE_Y+2,            // Right button Y coordinate.

            COLUMNS=2                            // Number of side strips on sidebar.
        };

    private:
        /*
        **    Each side strip is managed by this class. It handles all strip specific
        **    actions.
        */
        class StripClass : public StageClass
        {
            class SelectClass : public ControlClass
            {
                public:
                    SelectClass();
                    SelectClass(NoInitClass const & x) : ControlClass(x) {};

                    void Set_Owner(StripClass & strip, int index);

                    StripClass * Strip;
                    int Index;

                protected:
                    virtual bool Action(unsigned flags, KeyNumType & key);
            };

            public:
                StripClass() {}
                StripClass(InitClass const & x);
                StripClass(NoInitClass const & x) {}

                bool Add(RTTIType type, int ID);
                bool Abandon_Production(FactoryClass * factory);
                bool Scroll(bool up);
                bool AI(KeyNumType & input, Point2D & xy);
                void Draw_It(bool complete);
                void One_Time(int id);
                void Init_Clear();
                void Init_IO(int id);
                bool Recalc();
                void Activate();
                void Deactivate();
                void Flag_To_Redraw();
                bool Factory_Link(FactoryClass * factory, RTTIType type, int id);
                const ShapeFileStruct * Get_Special_Cameo(SpecialWeaponType type);

                /*
                **    File I/O.
                */
                bool Load(Straw & file);
                bool Save(Pipe & file) const;

                /*
                **    Working numbers used when rendering and processing the side strip.
                */
                enum SideBarGeneralEnums {
                    BUTTON_UP = 200,
                    BUTTON_DOWN = 210,
                    BUTTON_SELECT = 220,
                    MAX_BUILDABLES = 75,             // Maximum number of object types in sidebar.
                    OBJECT_HEIGHT = 51,             // Pixel height of each buildable object.
		            OBJECT_WIDTH = 64,             // Pixel width of each buildable object.
                    STRIP_WIDTH = 35,                // Width of strip (not counting border lines).
                    MAX_VISIBLE = 4,                 // Number of object slots visible at any one time.
                    SCROLL_RATE = 12,                // The pixel jump while scrolling (larger is faster).
                    UP_X_OFFSET = 2,                 // Scroll up arrow coordinates.
                    UP_Y_OFFSET = MAX_VISIBLE * OBJECT_HEIGHT + 1,
                    DOWN_X_OFFSET = 18,              // Scroll down arrow coordinates.
                    DOWN_Y_OFFSET = UP_Y_OFFSET,     //int(MAX_VISIBLE)*int(OBJECT_HEIGHT)+1,
                    SBUTTON_WIDTH = 16,              // Width of the mini-scroll button.
                    SBUTTON_HEIGHT = 12,             // Height of the mini-scroll button.
                    LEFT_EDGE_OFFSET = 2,            // Offset from left edge for building shapes.
                    TEXT_X_OFFSET = 18,              // X offset to print "ready" text.
                    TEXT_Y_OFFSET = 15,              // Y offset to print "ready" text.
                };

                /*
                **    This is the coordinate of the upper left corner that this side strip
                **    uses for rendering.
                */
                int X;
                int Y;

                Rect field_20; // new in TS

                /*
                **    This is a unique identifier for the sidebar strip. Using this identifier,
                **    it is possible to differentiate the button messages that arrive from the
                **    common input button list.  It >MUST< be equal to the strip's index into
                ** the Column[] array, because the strip uses it to access the stripclass
                ** buttons.
                */
                int ID;

                /*
                **    Shape numbers for the shapes in the STRIP.SHP file.
                */
                enum SideBarStipShapeEnums {
                    SB_BLANK,            // The blank rectangle to use if there are no objects present.
                    SB_FRAME
                };

                /*
                **    If this particular side strip needs to be redrawn, then this flag
                **    will be true.
                */
                bool IsToRedraw;

                /*
                **    If construction is in progress (no other objects in this strip can
                **    be started), then this flag will be true. It will be cleared when
                **    the strip is free to start production again.
                */
                bool IsBuilding;

                /*
                **    This controls the sidebar slide direction. If this is true, then the sidebar
                **    will scroll downward -- revealing previous objects.
                */
                bool IsScrollingDown;

                /*
                **    If the sidebar is scrolling, then this flag is true. Otherwise it is false.
                */
                bool IsScrolling;

                /*
                **    This is the object (sidebar slot) that is flashing. Only one slot can be flashing
                **    at any one instant. This is usually the result of a click on the slot and construction
                **    has commenced.
                */
                int Flasher;

                /*
                **    As the sidebar scrolls up and down, this variable holds the index for the topmost
                **    visible sidebar slot.
                */
                int TopIndex;

                /*
                **    This is the queued scroll direction and amount. The sidebar
                **    will scroll the number of slots indicated by this value. This
                **    value is set according to the scroll buttons.
                */
                int Scroller;

                /*
                **    The sidebar has smooth scrolling. This is the number of pixels the sidebar
                **    has slide down. Thus, if this value were 5, then there would be 5 pixels of
                **    the TopIndex-1 sidebar object visible. When the Slid value reaches 24, then
                **    the value resets to zero and the TopIndex is decremented. For sliding in the
                **    opposite direction, change the IsScrollingDown flag.
                */
                int Slid;

                /*
                ** The value of Slid the last time we rendered the sidebar.
                */
                int LastSlid;

                /*
                **    This is the count of the number of sidebar slots that are active.
                */
                int BuildableCount;

                /*
                **    This is the array of buildable object types. This array is sorted in the order
                **    that it is to be displayed. This array keeps track of which objects are building
                **    and ready to be placed. The very nature of this method precludes simultaneous
                **    construction of the same object type.
                */
                typedef struct BuildType {
                    int BuildableID;
                    RTTIType BuildableType;
                    FactoryClass * Factory;                                // Production manager.
                } BuildType;
                BuildType Buildables[MAX_BUILDABLES];

                /*
                **    Pointer to the shape data for small versions of the logos. These are used as
                **    placeholder pieces on the side bar.
                */
                static ShapeFileStruct * LogoShapes;

                /*
                **    This points to the animation sequence of frames used to mark the passage of time
                **    as an object is undergoing construction.
                */
                static const ShapeFileStruct * ClockShapes;

                static const ShapeFileStruct * DarkenShape;

                /*
                ** This points to the animation sequence which deals with special
                ** shapes which handle non-production based icons.
                */
                static const ShapeFileStruct * SpecialShapes[SPECIAL_COUNT];

                /*
                **    This is the last theater that the special palette remap table was loaded
                **    for. If the current theater differs from this recorded value, then the
                **    remap tables are reloaded.
                */
//                static TheaterType LastTheater;

                static ShapeButtonClass UpButton[COLUMNS];
                static ShapeButtonClass DownButton[COLUMNS];
                static SelectClass SelectButton[COLUMNS][MAX_VISIBLE];

                /*
                **    This points to the shapes that are used for the clock overlay. This displays
                **    progress of construction.
                */
                static char ClockTranslucentTable[(1+1)*256];

        } Column[COLUMNS];

    private:
        class SBGadgetClass: public GadgetClass {
            public:
                SBGadgetClass() : GadgetClass(0, 0, 1, 1, LEFTUP) {}

            protected:
                virtual bool Action(unsigned flags, KeyNumType & key);
        };

    public:
        NewSidebarClass();
        NewSidebarClass(const NoInitClass & x);

        /*
        ** Initialization
        */
        virtual void One_Time();                            // One-time inits
        virtual void Init_Clear();                        // Clears all to known state
        virtual void Init_IO();                            // Inits button list

        virtual void AI(KeyNumType & input, Point2D & xy);
        virtual void Draw_It(bool complete);

        bool Abandon_Production(RTTIType type, FactoryClass * factory);
        bool Activate(int control);
        bool Add(RTTIType type, int ID);
        bool Sidebar_Click(KeyNumType & input, Point2D & xy);
        void Recalc();
        bool Factory_Link(FactoryClass * factory, RTTIType type, int id);
        bool Scroll(bool up, int column);
        static int Max_Visible();

        bool field_1CD4; // new in TS.

        /*
        **    If the sidebar is active then this flag is true.
        */
        bool IsSidebarActive;

        /*
        **    This flag tells the rendering system that the sidebar needs to be redrawn.
        */
        bool IsToRedraw;

        bool field_1CD7; // new in TS.
        bool field_1CD8; // new in TS.

        bool IsRepairActive;
        bool IsUpgradeActive;
        bool IsDemolishActive;

    private:
        bool Activate_Repair(int control);
        bool Activate_Upgrade(int control);
        bool Activate_Demolish(int control);
        int Which_Column(RTTIType type);

        static ShapeFileStruct * SidebarShape;
        static ShapeFileStruct * SidebarMiddleShape;
        static ShapeFileStruct * SidebarBottomShape;

        /*
        **    This is the button that is used to collapse and expand the sidebar.
        ** These buttons must be available to derived classes, for Save/Load.
        */
        static ShapeButtonClass Waypoint;
        static ShapeButtonClass Demolish;
        static ShapeButtonClass Power;
        static ShapeButtonClass Repair;
        static SBGadgetClass Background;
};
