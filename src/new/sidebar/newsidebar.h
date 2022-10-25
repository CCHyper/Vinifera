/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          NEWSIDEBAR.H
 *
 *  @author        CCHyper
 *
 *  @brief         
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
#include "sidebar.h"
#include "factory.h"
#include "gcntrl.h"
#include "shapebtn.h"


class Straw;
class Pipe;
struct ShapeFileStruct;
class FactoryClass;


class NewSidebarClass : public PowerClass
{
    public:
        /**
         *  These constants are used to control the sidebar rendering.
         */
        enum SideBarClassEnums {
            BUTTON_ACTIVATOR=100,               // Button ID for the activator.

            SIDE_X=320-80,                      // The X position of sidebar upper left corner.
            SIDE_Y=7+70,                        // The Y position of sidebar upper left corner.
            SIDE_WIDTH=168,                     // Width of the entire sidebar (in pixels).
            SIDE_HEIGHT=200-(7+70),             // Height of the entire sidebar (in pixels).
            TOP_HEIGHT=13,                      // Height of top section (with repair/sell buttons).
            COLUMN_ONE_X=(320-80)+8,            // Sidestrip upper left coordinates...
            COLUMN_ONE_Y=SIDE_Y+TOP_HEIGHT,
            COLUMN_TWO_X=(320-80)+8+((80-16)/2)+3,
            COLUMN_TWO_Y=7+70+13,

            BUTTON_ONE_WIDTH=32,                // Button width.
            BUTTON_TWO_WIDTH=20,                // Button width.
            BUTTON_THREE_WIDTH=20,              // Button width.
            BUTTON_HEIGHT=9,                    // Button height.
            BUTTON_ONE_X=SIDE_X+2,              // Left button X coordinate.
            BUTTON_ONE_Y=SIDE_Y+2,              // Left button Y coordinate.
            BUTTON_TWO_X=SIDE_X+36,             // Right button X coordinate.
            BUTTON_TWO_Y=SIDE_Y+2,              // Right button Y coordinate.
            BUTTON_THREE_X=SIDE_X+58,           // Right button X coordinate.
            BUTTON_THREE_Y=SIDE_Y+2,            // Right button Y coordinate.

            COLUMNS=2                           // Number of side strips on sidebar.
        };

        typedef enum ButtonNumberType {
            BUTTON_RADAR = 100,
            BUTTON_REPAIR,
            BUTTON_POWER,
            BUTTON_DEMOLISH,
            BUTTON_UPGRADE,
            BUTTON_WAYPOINT,
            BUTTON_SELECT,
            BUTTON_ZOOM
        } ButtonNumberType;

    public:
        /**
         *  Each side strip is managed by this class. It handles all strip specific actions.
         */
        class StripClass : public StageClass
        {
            public:
                /**
                 *  Working numbers used when rendering and processing the side strip.
                 */
                enum StripClassEnums {
                    BUTTON_UP = 200,
                    BUTTON_DOWN = 210,
                    BUTTON_SELECT = 220,

                    MAX_BUILDABLES = 75,             // Maximum number of object types in sidebar.
                    OBJECT_HEIGHT = 51,              // Pixel height of each buildable object.
                    OBJECT_WIDTH = 64,               // Pixel width of each buildable object.
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

            public:
                class SelectClass : public ControlClass
                {
                    public:
                        SelectClass();
                        SelectClass(NoInitClass const & x) : ControlClass(x) {};
                        virtual ~SelectClass() {}

                        void Set_Owner(StripClass & strip, int index);

                    private:
                        StripClass * Strip;
                        int Index;

                    public:
                        virtual bool Action(unsigned flags, KeyNumType & key) override;
                };

            public:
                StripClass() {}
                StripClass(const InitClass & x);
                StripClass(const NoInitClass & x) : StageClass() {}
                ~StripClass() {}

                bool Add(RTTIType type, int ID);
                bool Abandon_Production(FactoryClass * factory);
                bool Scroll(bool up);
                bool Scroll_Page(bool up);
                bool AI(KeyNumType & input, Point2D & xy);
                void Draw_It(bool complete);
                void One_Time(int id);
                void Init_Clear();
                void Init_IO(int id);
                void Init_For_House(int id);
                const char *Help_Text(int gadget_id);
                bool Recalc();
                void Activate();
                void Deactivate();
                void Flag_To_Redraw();
                bool Factory_Link(FactoryClass * factory, RTTIType type, int id);
                const ShapeFileStruct * Get_Special_Cameo(SpecialWeaponType type);
                inline void Set_Position(int x, int y) { X = x; Y = y; }

                bool Is_On_Sidebar(RTTIType type, int id) const
                {
                    for (int i = 0; i < BuildableCount; ++i) {
                        BuildType build = Buildables[i];
                        if (build.BuildableType == type && build.BuildableID == id) {
                            return true;
                        }
                    }
                    return false;
                }

            public:
                static int Max_Visible();

            public:                
                /**
                 *  This is the coordinate of the upper left corner that this side strip
                 *  uses for rendering.
                 */

                int X;
                int Y;
                
                /**
                 *  new in TS
                 */
                Rect field_20;
                
                /**
                 *  
                 */
                /*
                **    This is a unique identifier for the sidebar strip. Using this identifier,
                **    it is possible to differentiate the button messages that arrive from the
                **    common input button list.  It >MUST< be equal to the strip's index into
                ** the Column[] array, because the strip uses it to access the stripclass
                ** buttons.
                */
                int ID;
                
                /**
                 *  
                 */
                /*
                **    Shape numbers for the shapes in the STRIP.SHP file.
                */
                enum SideBarStipShapeEnums {
                    SB_BLANK,            // The blank rectangle to use if there are no objects present.
                    SB_FRAME
                };
                
                /**
                 *  
                 */
                /*
                **    If this particular side strip needs to be redrawn, then this flag
                **    will be true.
                */
                bool IsToRedraw;
                
                /**
                 *  
                 */
                /*
                **    If construction is in progress (no other objects in this strip can
                **    be started), then this flag will be true. It will be cleared when
                **    the strip is free to start production again.
                */
                bool IsBuilding;
                
                /**
                 *  
                 */
                /*
                **    This controls the sidebar slide direction. If this is true, then the sidebar
                **    will scroll downward -- revealing previous objects.
                */
                bool IsScrollingDown;
                
                /**
                 *  
                 */
                /*
                **    If the sidebar is scrolling, then this flag is true. Otherwise it is false.
                */
                bool IsScrolling;
                
                /**
                 *  
                 */
                /*
                **    This is the object (sidebar slot) that is flashing. Only one slot can be flashing
                **    at any one instant. This is usually the result of a click on the slot and construction
                **    has commenced.
                */
                int Flasher;
                
                /**
                 *  
                 */
                /*
                **    As the sidebar scrolls up and down, this variable holds the index for the topmost
                **    visible sidebar slot.
                */
                int TopIndex;
                
                /**
                 *  
                 */
                /*
                **    This is the queued scroll direction and amount. The sidebar
                **    will scroll the number of slots indicated by this value. This
                **    value is set according to the scroll buttons.
                */
                int Scroller;
                
                /**
                 *  
                 */
                /*
                **    The sidebar has smooth scrolling. This is the number of pixels the sidebar
                **    has slide down. Thus, if this value were 5, then there would be 5 pixels of
                **    the TopIndex-1 sidebar object visible. When the Slid value reaches 24, then
                **    the value resets to zero and the TopIndex is decremented. For sliding in the
                **    opposite direction, change the IsScrollingDown flag.
                */
                int Slid;
                
                /**
                 *  
                 */
                /*
                ** The value of Slid the last time we rendered the sidebar.
                */
                int LastSlid;
                
                /**
                 *  
                 */
                /*
                **    This is the count of the number of sidebar slots that are active.
                */
                int BuildableCount;
                
                /**
                 *  
                 */
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
                //DynamicVectorClass<BuildType> Buildables;
                
            private:
                /**
                 *  
                 */
                static const ShapeFileStruct * DarkenShape;
                
            public:
                static ShapeButtonClass UpButton[COLUMNS];
                static ShapeButtonClass DownButton[COLUMNS];
                static SelectClass SelectButton[COLUMNS][MAX_VISIBLE];

        } Column[COLUMNS];
        //DynamicVectorClass<StripClass> Column;

    public:
        class SBGadgetClass: public GadgetClass
        {
            public:
                SBGadgetClass() : GadgetClass(0, 0, 1, 1, LEFTUP) {}
                virtual ~SBGadgetClass() {}

            public:
                virtual bool Action(unsigned flags, KeyNumType & key) override;
        };

    public:
        NewSidebarClass();
        NewSidebarClass(const NoInitClass & x);
        virtual ~NewSidebarClass() {}

        /**
         *  GScreenClass
         */
        virtual void One_Time() override;
        virtual void Init_Clear() override;
        virtual void Init_IO() override;
        virtual void AI(KeyNumType & input, Point2D & xy) override;
        virtual void Draw_It(bool complete) override;

        /**
         *  DisplayClass
         */
        virtual HRESULT Load(IStream *pStm) override;
        virtual HRESULT Save(IStream *pStm) override;
        virtual const char *Help_Text(int gadget_id) override;
        virtual void entry_84() override;

        /**
         *  RadarClass
         */
        virtual void Init_For_House() override;

        bool Abandon_Production(RTTIType type, FactoryClass * factory);
        bool Activate(int control);
        bool Add(RTTIType type, int ID);
        void Recalc();
        bool Factory_Link(FactoryClass * factory, RTTIType type, int id);
        bool Scroll(bool up, int column);
        bool Scroll_Page(bool up, int column);
        bool Activate_Repair(int control);
        bool Activate_Upgrade(int control);
        bool Activate_Demolish(int control);
        int Which_Column(RTTIType type);
        void Zoom_Mode_Control();
        void Set_Cameo_Text(bool enable);

        bool Is_On_Sidebar(RTTIType type, int id)
        {
            int column = Which_Column(type);
            return Column[column].Is_On_Sidebar(type, id);
        }

        void Redraw_Sidebar() { IsToRedraw = true; Flag_To_Redraw(); }

    private:
        /**
         *  new in TS.
         */
        bool field_1CD4;

        /**
         *  If the sidebar is active then this flag is true.
         */
        bool IsSidebarActive;

        /**
         *  This flag tells the rendering system that the sidebar needs to be redrawn.
         */
        bool IsToRedraw;
        
        /**
         *  new in TS.
         */
        bool field_1CD7;

        /**
         *  new in TS.
         */
        bool field_1CD8;
        
        /**
         *  x
         */
        bool IsRepairActive;
        bool IsUpgradeActive;
        bool IsDemolishActive;

    private:
        /**
         *  This points to the animation sequence of frames used to mark the passage of time
         *  as an object is undergoing construction.
         */
        static const ShapeFileStruct * ClockShape;
        static const ShapeFileStruct * RechargeClockShape;

        static const ShapeFileStruct * SidebarShape;
        static const ShapeFileStruct * SidebarMiddleShape;
        static const ShapeFileStruct * SidebarBottomShape;
        static const ShapeFileStruct * SidebarAddonShape;

        static ShapeButtonClass Waypoint;
        static ShapeButtonClass Upgrade;
        static ShapeButtonClass Power;
        static ShapeButtonClass Repair;
        static SBGadgetClass Background;
};
