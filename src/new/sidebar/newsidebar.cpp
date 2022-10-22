/***********************************************************************************************
 ***            C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : SIDEBAR.CPP                                                  *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : October 20, 1994                                             *
 *                                                                                             *
 *                  Last Update : October 9, 1996 [JLB]                                        *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   NewSidebarClass::AI -- Handles player clicking on sidebar area.                              *
 *   NewSidebarClass::Abandon_Production -- Stops production of the object specified.             *
 *   NewSidebarClass::Activate -- Controls the sidebar activation.                                *
 *   NewSidebarClass::Activate_Demolish -- Controls the demolish button on the sidebar.           *
 *   NewSidebarClass::Activate_Repair -- Controls the repair button on the sidebar.               *
 *   NewSidebarClass::Activate_Upgrade -- Controls the upgrade button on the sidebar.             *
 *   NewSidebarClass::Add -- Adds a game object to the sidebar list.                              *
 *   NewSidebarClass::Draw_It -- Renders the sidebar display.                                     *
 *   NewSidebarClass::Factory_Link -- Links a factory to a sidebar strip.                         *
 *   NewSidebarClass::Init_Clear -- Sets sidebar to a known (and deactivated) state               *
 *   NewSidebarClass::Init_IO -- Adds buttons to the button list                                  *
 *   NewSidebarClass::Init_Theater -- Performs theater-specific initialization                    *
 *   NewSidebarClass::One_Time -- Handles the one time game initializations.                      *
 *   NewSidebarClass::One_Time -- Handles the one time game initializations.                      *
 *   NewSidebarClass::Recalc -- Examines the sidebar data and updates it as necessary.            *
 *   NewSidebarClass::Refresh_Cells -- Intercepts the refresh, looking for sidebar controls.      *
 *   NewSidebarClass::SBGadgetClass::Action -- Special function that controls the mouse over the s*
 *   NewSidebarClass::Scroll -- Handles scrolling the sidebar object strip.                       *
 *   NewSidebarClass::Set_Current -- Sets a specified object that controls the sidebar display.   *
 *   NewSidebarClass::NewSidebarClass -- Default constructor for the sidebar.                        *
 *   NewSidebarClass::NewSidebarClass -- This is the no initialization constructor for the sidebar.  *
 *   NewSidebarClass::StripClass::AI -- Input and AI processing for the side strip.               *
 *   NewSidebarClass::StripClass::Abandon_Produ -- Abandons production associated with sidebar.   *
 *   NewSidebarClass::StripClass::Activate -- Adds the strip buttons to the input system.         *
 *   NewSidebarClass::StripClass::Add -- Add an object to the side strip.                         *
 *   NewSidebarClass::StripClass::Deactivate -- Removes the side strip buttons from the input syst*
 *   NewSidebarClass::StripClass::Draw_It -- Render the sidebar display.                          *
 *   NewSidebarClass::StripClass::Factory_Link -- Links a factory to a sidebar button.            *
 *   NewSidebarClass::StripClass::Flag_To_Redra -- Flags the sidebar strip to be redrawn.         *
 *   NewSidebarClass::StripClass::Get_Special_Cameo -- Fetches the special event cameo shape.     *
 *   NewSidebarClass::StripClass::Init_Clear -- Sets sidebar to a known (and deactivated) state   *
 *   NewSidebarClass::StripClass::Init_IO -- Adds buttons to the button list                      *
 *   NewSidebarClass::StripClass::Init_Theater -- Performs theater-specific initialization        *
 *   NewSidebarClass::StripClass::One_Time -- Performs one time actions necessary for the side str*
 *   NewSidebarClass::StripClass::Recalc -- Revalidates the current sidebar list of objects.      *
 *   NewSidebarClass::StripClass::Scroll -- Causes the side strip to scroll.                      *
 *   NewSidebarClass::StripClass::SelectClass:: -- Action function when buildable cameo is selecte*
 *   NewSidebarClass::StripClass::SelectClass:: -- Assigns special values to a buildable select bu*
 *   NewSidebarClass::StripClass::SelectClass::SelectClass -- Default constructor.                *
 *   NewSidebarClass::StripClass::StripClass -- Default constructor for the side strip class.     *
 *   NewSidebarClass::Which_Column -- Determines which column a given type should appear.         *
 *   NewSidebarClass::Zoom_Mode_Control -- Handles the zoom mode toggle operation.                *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#include "newsidebar.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "house.h"
#include "session.h"
#include "vox.h"
#include "voc.h"
#include "iomap.h"
#include "drawshape.h"
#include "techno.h"
#include "technotype.h"
#include "event.h"
#include "super.h"
#include "supertype.h"
#include "rules.h"
#include "asserthandler.h"


ShapeFileStruct * NewSidebarClass::SidebarShape = nullptr;
ShapeFileStruct * NewSidebarClass::SidebarMiddleShape = nullptr;
ShapeFileStruct * NewSidebarClass::SidebarBottomShape = nullptr;


typedef enum ButtonNumberType {
    BUTTON_RADAR = 100,
    BUTTON_REPAIR,
    BUTTON_POWER,
    BUTTON_WAYPOINT,
    BUTTON_DEMOLISH,
    BUTTON_UPGRADE,
    BUTTON_SELECT,
    BUTTON_ZOOM
} ButtonNumberType;

/*
** Sidebar buttons
*/
NewSidebarClass::SBGadgetClass NewSidebarClass::Background;
ShapeButtonClass NewSidebarClass::Waypoint;
ShapeButtonClass NewSidebarClass::Demolish;
ShapeButtonClass NewSidebarClass::Power;
ShapeButtonClass NewSidebarClass::Repair;
ShapeButtonClass NewSidebarClass::StripClass::UpButton[COLUMNS];
ShapeButtonClass NewSidebarClass::StripClass::DownButton[COLUMNS];
NewSidebarClass::StripClass::SelectClass NewSidebarClass::StripClass::SelectButton[COLUMNS][MAX_VISIBLE];

/*
** Shape data pointers
*/
ShapeFileStruct * NewSidebarClass::StripClass::LogoShapes = nullptr;
ShapeFileStruct const * NewSidebarClass::StripClass::ClockShapes;
ShapeFileStruct const * NewSidebarClass::StripClass::DarkenShape;
ShapeFileStruct const * NewSidebarClass::StripClass::SpecialShapes[SPECIAL_COUNT];


/***********************************************************************************************
 * NewSidebarClass::NewSidebarClass -- Default constructor for the sidebar.                          *
 *                                                                                             *
 *    Constructor for the sidebar handler. It basically sets up the sidebar to the empty       *
 *    condition.                                                                               *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/17/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
NewSidebarClass::NewSidebarClass() :
    IsSidebarActive(false),
    IsToRedraw(true),
    IsRepairActive(false),
    IsUpgradeActive(false),
    IsDemolishActive(false)
{
    /*
    **  This sets up the clipping window. This window is used by the shape drawing
    **  code so that as the sidebar buildable buttons scroll, they get properly
    **  clipped at the top and bottom edges.
    */
    //WindowList[WINDOW_SIDEBAR][WINDOWX] = (SIDE_X+8);
    //WindowList[WINDOW_SIDEBAR][WINDOWY] = SIDE_Y + 1 + TOP_HEIGHT;
    //WindowList[WINDOW_SIDEBAR][WINDOWWIDTH] = SIDE_WIDTH;
    //WindowList[WINDOW_SIDEBAR][WINDOWHEIGHT] = StripClass::MAX_VISIBLE * StripClass::OBJECT_HEIGHT;
//    WindowList[WINDOW_SIDEBAR][WINDOWHEIGHT] = StripClass::MAX_VISIBLE * StripClass::OBJECT_HEIGHT-1;

    /*
    **  Set up the coordinates for the sidebar strips. These coordinates are for
    **  the upper left corner.
    */
    new (&Column[0]) StripClass(InitClass());
    new (&Column[1]) StripClass(InitClass());

    Column[0].X = COLUMN_ONE_X;
    Column[0].Y = COLUMN_ONE_Y;
    Column[1].X = COLUMN_TWO_X;
    Column[1].Y = COLUMN_TWO_Y;
}


/***********************************************************************************************
 * NewSidebarClass::NewSidebarClass -- This is the no initialization constructor for the sidebar.    *
 *                                                                                             *
 *    Unlike the normal constructor, this one doesn't do any initialization. There is one      *
 *    exception to this. The stip classes can't call an explicit NoInitClass constructor       *
 *    since they are an array. Since the default constructor is called for these strips, we    *
 *    must reset the X and Y location to what we know they should be.                          *
 *                                                                                             *
 * INPUT:   flag to indicate that this is a no initialization constructor.                     *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/06/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
NewSidebarClass::NewSidebarClass(const NoInitClass &x)
{
}


/***********************************************************************************************
 * NewSidebarClass::One_Time -- Handles the one time game initializations.                        *
 *                                                                                             *
 *    This routine is used to load the graphic data that is needed by the sidebar display. It  *
 *    should only be called ONCE.                                                              *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Only call this routine once when the game first starts.                         *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/28/94   JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::One_Time()
{
    PowerClass::One_Time();

    /*
    **  This sets up the clipping window. This window is used by the shape drawing
    **  code so that as the sidebar buildable buttons scroll, they get properly
    **  clipped at the top and bottom edges.
    */
    //WindowList[WINDOW_SIDEBAR][WINDOWX] = ((SIDE_X+8));
    //WindowList[WINDOW_SIDEBAR][WINDOWY] = (SIDE_Y + 1 + TOP_HEIGHT);
    //WindowList[WINDOW_SIDEBAR][WINDOWWIDTH] = (SIDE_WIDTH);
    //WindowList[WINDOW_SIDEBAR][WINDOWHEIGHT] = (StripClass::MAX_VISIBLE * StripClass::OBJECT_HEIGHT);
//    WindowList[WINDOW_SIDEBAR][WINDOWHEIGHT] = (StripClass::MAX_VISIBLE * StripClass::OBJECT_HEIGHT-1);

    /*
    ** Top of the window seems to be wrong for the new sidebar. ST - 5/2/96 2:49AM
    */
    //WindowList[WINDOW_SIDEBAR][WINDOWY] -= 1;

    /*
    **  Set up the coordinates for the sidebar strips. These coordinates are for
    **  the upper left corner.
    */
//    Column[0].X = COLUMN_ONE_X;
//    Column[0].Y = COLUMN_ONE_Y;
//    Column[1].X = COLUMN_TWO_X;
//    Column[1].Y = COLUMN_TWO_Y;
    Column[0].One_Time(0);
    Column[1].One_Time(1);

    /*
    **  Load the sidebar shape in at this time. (Hi-Res sidebar is theater dependant)
    */
    if (SidebarShape == nullptr) {
        SidebarShape = (ShapeFileStruct *)MFCC::Retrieve("SIDEBAR.SHP");
    }
}


/***********************************************************************************************
 * NewSidebarClass::Init_Clear -- Sets sidebar to a known (and deactivated) state                 *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::Init_Clear()
{
    PowerClass::Init_Clear();

    IsToRedraw = true;
    IsRepairActive = false;
    IsUpgradeActive = false;
    IsDemolishActive = false;

    Column[0].Init_Clear();
    Column[1].Init_Clear();

    Activate(false);
}


/***********************************************************************************************
 * NewSidebarClass::Init_IO -- Adds buttons to the button list                                    *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::Init_IO()
{
    PowerClass::Init_IO();

    /*
    ** Add the sidebar's buttons only if we're not in editor mode.
    */
    if (!Debug_Map) {

        Repair.IsSticky = true;
        Repair.ID = BUTTON_REPAIR;
        Repair.X = (498/2);
        Repair.Y = (150/2);
        Repair.IsPressed = false;
        Repair.IsToggleType = true;
        Repair.ReflectButtonState = true;
        Repair.Set_Shape((ShapeFileStruct *)MFCC::Retrieve("REPAIR.SHP"));

        //Upgrade.IsSticky = true;
        //Upgrade.ID = BUTTON_UPGRADE;
        //Upgrade.X = 0x21f;
        //Upgrade.Y = (0x96/2);
        //Upgrade.IsPressed = false;
        //Upgrade.IsToggleType = true;
        //Upgrade.ReflectButtonState = true;
        //Upgrade.Set_Shape((ShapeFileStruct *)MFCC::Retrieve("SELL.SHP"));

        //Zoom.IsSticky = true;
        //Zoom.ID = BUTTON_ZOOM;
        //Zoom.X = (0x24c/2);
        //Zoom.Y = (0x96/2);
        //Zoom.IsPressed = false;
        //Zoom.Set_Shape((ShapeFileStruct *)MFCC::Retrieve("MAP.SHP"));

        //if ((IsRadarActive && Is_Zoomable()) || Session.Type != GAME_NORMAL) {
        //    Zoom.Enable();
        //} else {
        //    Zoom.Disable();
        //}
        Column[0].Init_IO(0);
        Column[1].Init_IO(1);

        /*
        ** If a game was loaded & the sidebar was enabled, pop it up now
        */
        if (IsSidebarActive) {
            IsSidebarActive = false;
            Activate(1);
//            Background.Zap();
//            Add_A_Button(Background);
        }
    }
}


/***********************************************************************************************
 * NewSidebarClass::Which_Column -- Determines which column a given type should appear.           *
 *                                                                                             *
 *    Use this function to resolve what column the specified object type should be placed      *
 *    into.                                                                                    *
 *                                                                                             *
 * INPUT:   otype -- Pointer to the object type class of the object in question.               *
 *                                                                                             *
 * OUTPUT:  Returns with the column number that the object should be placed in.                *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/01/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int NewSidebarClass::Which_Column(RTTIType type) // DONE
{
    if (type == RTTI_BUILDINGTYPE || type == RTTI_BUILDING) {
        return 0;
    }
    return 1;
}


/***********************************************************************************************
 * NewSidebarClass::Factory_Link -- Links a factory to a sidebar strip.                           *
 *                                                                                             *
 *    This routine will link the specified factory to the sidebar strip. A factory must be     *
 *    linked to the sidebar so that as the factory production progresses, the sidebar will     *
 *    show the production progress.                                                            *
 *                                                                                             *
 * INPUT:   factory  -- The factory number to attach.                                          *
 *                                                                                             *
 *          type     -- The object type number.                                                *
 *                                                                                             *
 *          id       -- The object sub-type number.                                            *
 *                                                                                             *
 * OUTPUT:  Was the factory successfully attached to the sidebar strip?                        *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::Factory_Link(FactoryClass * factory, RTTIType type, int id) // DONE
{
    ASSERT(type < RTTI_COUNT);
    ASSERT(id >= 0);

    return Column[Which_Column(type)].Factory_Link(factory, type, id);
}


/***********************************************************************************************
 * NewSidebarClass::Activate_Repair -- Controls the repair button on the sidebar.                 *
 *                                                                                             *
 *    Use this routine to turn the repair sidebar button on and off. Typically, the button     *
 *    is enabled when the currently selected structure is friendly and damaged.                *
 *                                                                                             *
 * INPUT:   control  -- The controls how the button is to be activated or deactivated;         *
 *                      0  -- Turn button off.                                                 *
 *                      1  -- Turn button on.                                                  *
 *                      -1 -- Toggle button state.                                             *
 *                                                                                             *
 * OUTPUT:  bool; Was the button previously activated?                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::Activate_Repair(int control) // DONE
{
    bool old = IsRepairActive;

    if (control == -1) {
        control = IsRepairActive ? 0 : 1;
    }
    switch (control) {
        case 1:
            IsRepairActive = true;
            break;

        default:
        case 0:
            IsRepairActive = false;
            break;
    }
    if (old != IsRepairActive) {
        Flag_To_Redraw();
        IsToRedraw = true;

        if (!IsRepairActive) {
            Set_Default_Mouse(MOUSE_NORMAL, false);
        }
    }
    return old;
}


/***********************************************************************************************
 * NewSidebarClass::Activate_Upgrade -- Controls the upgrade button on the sidebar.               *
 *                                                                                             *
 *    Use this routine to turn the upgrade sidebar button on and off. Typically, the button    *
 *    is enabled when the currently selected structure can be upgraded and disabled otherwise. *
 *                                                                                             *
 * INPUT:   control  -- The controls how the button is to be activated or deactivated;         *
 *                      0  -- Turn button off.                                                 *
 *                      1  -- Turn button on.                                                  *
 *                      -1 -- Toggle button state.                                             *
 *                                                                                             *
 * OUTPUT:  bool; Was the button previously activated?                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::Activate_Upgrade(int control) // DONE
{
    bool old = IsUpgradeActive;
    if (control == -1) {
        control = IsUpgradeActive ? 0 : 1;
    }
    switch (control) {
        case 1:
            IsUpgradeActive = true;
            break;

        default:
        case 0:
            IsUpgradeActive = false;
            break;
    }
    if (old != IsUpgradeActive) {
        Flag_To_Redraw();
        IsToRedraw = true;
        if (!IsUpgradeActive) {
            Set_Default_Mouse(MOUSE_NORMAL, false);
        }
    }
    return old;
}


/***********************************************************************************************
 * NewSidebarClass::Activate_Demolish -- Controls the demolish button on the sidebar.             *
 *                                                                                             *
 *    Use this routine to turn the demolish/dismantle sidebar button on and off. Typically,    *
 *    the button is enabled when a friendly building is selected and disabled otherwise.       *
 *                                                                                             *
 * INPUT:   control  -- The controls how the button is to be activated or deactivated;         *
 *                      0  -- Turn button off.                                                 *
 *                      1  -- Turn button on.                                                  *
 *                      -1 -- Toggle button state.                                             *
 *                                                                                             *
 * OUTPUT:  bool; Was the button previously activated?                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::Activate_Demolish(int control) // DONE
{
    bool old = IsDemolishActive;

    if (control == -1) {
        control = IsDemolishActive ? 0 : 1;
    }
    switch (control) {
        case 1:
            IsDemolishActive = true;
            break;

        default:
        case 0:
            IsDemolishActive = false;
            break;
    }
    if (old != IsDemolishActive) {
        Flag_To_Redraw();
        IsToRedraw = true;
        if (!IsDemolishActive) {
            Set_Default_Mouse(MOUSE_NORMAL, false);
        }
    }
    return old;
}


/***********************************************************************************************
 * NewSidebarClass::Add -- Adds a game object to the sidebar list.                                *
 *                                                                                             *
 *    This routine is used to add a game object to the sidebar. Call this routine when a       *
 *    factory type building is created. It handles the case of adding an item that has already *
 *    been added -- it just ignores it.                                                        *
 *                                                                                             *
 * INPUT:   object   -- Pointer to the object that is being added.                             *
 *                                                                                             *
 * OUTPUT:  bool; Was the object added to the sidebar?                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/17/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::Add(RTTIType type, int id) // DONE
{
    ASSERT(type < RTTI_COUNT);

    /*
    ** Add the sidebar only if we're not in editor mode.
    */
    if (!Debug_Map) {
        int column = Which_Column(type);

        if (Column[column].Add(type, id)) {
            Activate(1);
            IsToRedraw = true;
            Flag_To_Redraw();
            return true;
        }
        return false;
    }

    return false;
}


/***********************************************************************************************
 * NewSidebarClass::Scroll -- Handles scrolling the sidebar object strip.                         *
 *                                                                                             *
 *    This routine is used to scroll the sidebar strip of objects. The strip appears whenever  *
 *    a building is selected that can produce units. If the number of units to produce is      *
 *    greater than what the sidebar can hold, this routine is used to scroll the other object  *
 *    into view so they can be selected.                                                       *
 *                                                                                             *
 * INPUT:   up -- Should the scroll be upwards? Upward scrolling reveals object that are       *
 *                later in the list of objects.                                                *
 *                                                                                             *
 * OUTPUT:  bool; Did scrolling occur?                                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/28/94   JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::Scroll(bool up, int column)
{
    if (column == -1) {
        bool scr = false;
        scr |= Column[0].Scroll(up);
        scr |= Column[1].Scroll(up);
        if (!scr) {
            Sound_Effect(Rule->ScoldSound);
        }
        if (scr) {
            IsToRedraw = true;
            Flag_To_Redraw();
            return true;
        }
        return false;
    }

    if (Column[column].Scroll(up)) {
        // No need to redraw the whole sidebar juss because we scrolled a strip is there? ST - 10/15/96 7:29PM
        //IsToRedraw = true;
        Flag_To_Redraw();
        return true;
    }
    return false;
}


/***********************************************************************************************
 * NewSidebarClass::Draw_It -- Renders the sidebar display.                                       *
 *                                                                                             *
 *    This routine performs the actual drawing of the sidebar display.                         *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  bool; Was the sidebar imagery changed at all?                                      *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/28/94   JLB : Created.                                                                 *
 *   12/31/1994 JLB : Split rendering off into the sidebar strip class.                        *
 *=============================================================================================*/
void NewSidebarClass::Draw_It(bool complete)
{
    PowerClass::Draw_It(complete);

    //BStart(BENCH_SIDEBAR);

    if (IsSidebarActive && (IsToRedraw || complete) && !Debug_Map) {
        IsToRedraw = false;

        //if (LogicPage->Lock()) {
        //   /*
        //   **  Draw the outline box around the sidebar buttons.
        //   */
        //   int shape = complete ? 0 : 1;
        //
        //   /*
        //   ** The sidebar shape is too big in 640x400 so it needs to be drawn in three chunks.
        //   */
        //   CC_Draw_Shape(SidebarShape, 0, SIDE_X, 8, WINDOW_MAIN, SHAPE_WIN_REL);
        //   CC_Draw_Shape(SidebarMiddleShape, shape, SIDE_X, (8+80), WINDOW_MAIN, SHAPE_WIN_REL);
        //   CC_Draw_Shape(SidebarBottomShape, shape, SIDE_X, (8+80+50), WINDOW_MAIN, SHAPE_WIN_REL);
        //
        //   Repair.Draw_Me true;
        //   Upgrade.Draw_Me true;
        //   Zoom.Draw_Me true;
        //   LogicPage->Unlock();
        //}
    }

    /*
    **  Draw the side strip elements by calling their respective draw functions.
    */
    if (IsSidebarActive) {
        Column[0].Draw_It(complete);
        Column[1].Draw_It(complete);

        if (complete || IsToRedraw) {
            Repair.Draw_Me(true);
            //Upgrade.Draw_Me(true);
            //Zoom.Draw_Me(true);
        }
    }
    IsToRedraw = false;

    //BEnd(BENCH_SIDEBAR);
}


/***********************************************************************************************
 * NewSidebarClass::AI -- Handles player clicking on sidebar area.                                *
 *                                                                                             *
 *    This routine handles the processing necessary when the player clicks on the sidebar.     *
 *    Typically, this is selection of the item to build.                                       *
 *                                                                                             *
 * INPUT:   input -- Reference to the keyboard input value.                                    *
 *                                                                                             *
 *          x,y   -- Mouse coordinates at time of input.                                       *
 *                                                                                             *
 * OUTPUT:  bool; Was the click handled?                                                       *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/28/94   JLB : Created.                                                                 *
 *   11/11/1994 JLB : Processes input directly.                                                *
 *   12/26/1994 JLB : Uses factory manager class for construction handling.                    *
 *   12/31/1994 JLB : Simplified to use the sidebar strip class handlers.                      *
 *   12/31/1994 JLB : Uses mouse coordinate parameters.                                        *
 *   06/27/1995 JLB : <TAB> key toggles sidebar.                                               *
 *=============================================================================================*/
void NewSidebarClass::AI(KeyNumType & input, Point2D & xy)
{
    bool redraw = false;

#if 0
    /*
    **  Toggle the sidebar in and out with the <TAB> key.
    */
    if (input == KN_TAB) {
        Activate(-1);
    }
#endif

    if (!Debug_Map) {
        Activate(1);    // Force the sidebar always on.
        Column[0].AI(input, xy);
        Column[1].AI(input, xy);
    }

    if (IsSidebarActive) {

        /*
        **  If there are any buildings in the payer's inventory, then allow the repair
        **  option.
        */
        if (PlayerPtr->CurBuildings > 0) {
            Activate_Repair(true);
        } else {
            Activate_Repair(false);
        }

        if (input == (BUTTON_REPAIR|KN_BUTTON)) {
            Repair_Mode_Control(-1);
        }

        if (input == (BUTTON_POWER|KN_BUTTON)) {
            Power_Mode_Control(-1);
        }

        if (input == (BUTTON_WAYPOINT|KN_BUTTON)) {
            Waypoint_Mode_Control(-1, false);
        }

        if (input == (BUTTON_DEMOLISH|KN_BUTTON)) {
            Sell_Mode_Control(-1);
        }

        //if (redraw) {
        //    //IsToRedraw = true;
        //    Column[0].Flag_To_Redraw();
        //    Column[1].Flag_To_Redraw();
        //
        //    Flag_To_Redraw();
        //}
    }

    if (!IsRepairMode && Repair.IsOn) {
        Repair.Turn_Off();
    }

    if (!IsSellMode && Demolish.IsOn) {
        Demolish.Turn_Off();
    }

    if (!IsSellMode && Power.IsOn) {
        Power.Turn_Off();
    }

    if (!IsSellMode && Waypoint.IsOn) {
        Waypoint.Turn_Off();
    }

    PowerClass::AI(input, xy);
}


/***********************************************************************************************
 * NewSidebarClass::Recalc -- Examines the sidebar data and updates it as necessary.              *
 *                                                                                             *
 *    Occasionally a factory gets destroyed. This routine must be called in such a case        *
 *    because it might be possible that sidebar object need to be removed. This routine will   *
 *    examine all existing objects in the sidebar class and if no possible factory can         *
 *    produce it, then it will be removed.                                                     *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   This routine is exhaustive and thus time consuming. Only call it when really    *
 *             necessary. Such as when a factory is destroyed rather than when a non-factory   *
 *             is destroyed.                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/30/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::Recalc() // DONE
{
    bool redraw = false;

    redraw |= Column[0].Recalc();
    redraw |= Column[1].Recalc();

    if (redraw) {
        IsToRedraw = true;
        Flag_To_Redraw();
    }
}


/***********************************************************************************************
 * NewSidebarClass::Activate -- Controls the sidebar activation.                                  *
 *                                                                                             *
 *    Use this routine to turn the sidebar on or off. This routine handles updating the        *
 *    necessary flags.                                                                         *
 *                                                                                             *
 * INPUT:   control  -- Tells what to do with the sidebar according to the following:          *
 *                         0 = Turn sidebar off.                                               *
 *                         1 = Turn sidebar on.                                                *
 *                         -1= Toggle sidebar on or off.                                       *
 *                                                                                             *
 * OUTPUT:  bool; Was the sidebar already on?                                                  *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/09/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::Activate(int control)
{
    bool old = IsSidebarActive;

    if (Session.Play)
        return old;

    /*
    **  Determine the new state of the sidebar.
    */
    switch (control) {
        case -1:
            IsSidebarActive = IsSidebarActive == false;
            break;

        case 1:
            IsSidebarActive = true;
            break;

        default:
        case 0:
            IsSidebarActive = false;
            break;
    }

    /*
    **  Only if there is a change in the state of the sidebar will anything
    **  be done to change it.
    */
    if (IsSidebarActive != old) {

        /*
        **  If the sidebar is activated but was on the right side of the screen, then
        **  activate it on the left side of the screen.
        */
        if (IsSidebarActive /*&& X*/) {
            //Set_View_Dimensions(0, 8, ((320-SIDE_WIDTH)/ICON_PIXEL_W));
            IsToRedraw = true;
            //Help_Text(TXT_NONE);
            Repair.Zap();
            Add_A_Button(Repair);
            //Upgrade.Zap();
            //Add_A_Button(Upgrade);
            //Zoom.Zap();
            //Add_A_Button(Zoom);
            Column[0].Activate();
            Column[1].Activate();
            Background.Zap();
            Add_A_Button(Background);
            Map.RadarButton.Zap();
            Add_A_Button(Map.RadarButton);
            //Map.PowerButton.Zap();
            //Add_A_Button(Map.PowerButton);
        } else  {
            //Help_Text(TXT_NONE);
            //Set_View_Dimensions(0, 8);
            Remove_A_Button(Repair);
            //Remove_A_Button(Upgrade);
            //Remove_A_Button(Zoom);
            Remove_A_Button(Background);
            Column[0].Deactivate();
            Column[1].Deactivate();
            Remove_A_Button(Map.RadarButton);
            //Remove_A_Button(Map.PowerButton);
        }

        /*
        **  Since the sidebar status has changed, update the map so that the graphics
        **  will be rendered correctly.
        */
        Flag_To_Redraw(true);
    }

    return old;
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::StripClass -- Default constructor for the side strip class.       *
 *                                                                                             *
 *    This constructor is used to reset the side strip to default empty state.                 *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
NewSidebarClass::StripClass::StripClass(InitClass const & x) : // DONE
    StageClass(),
    X(0),
    Y(0),
    field_20(),
    ID(0),
    IsToRedraw(true),
    IsBuilding(false),
    IsScrollingDown(false),
    IsScrolling(false),
    Flasher(-1),
    TopIndex(0),
    Scroller(0),
    Slid(0),
    BuildableCount(0)
{
    for (int index = 0; index < MAX_BUILDABLES; ++index) {
        Buildables[index].BuildableID = 0;
        Buildables[index].BuildableType = RTTI_NONE;
        Buildables[index].Factory = nullptr;
    }
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::One_Time -- Performs one time actions necessary for the side stri *
 *                                                                                             *
 *    Call this routine ONCE at the beginning of the game. It handles retrieving pointers to   *
 *    the shape files it needs for rendering.                                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::StripClass::One_Time(int id) // DONE
{
    DarkenShape = (ShapeFileStruct *)MFCC::Retrieve("DARKEN.SHP");
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Get_Special_Cameo -- Fetches the special event cameo shape.       *
 *                                                                                             *
 *    This routine will return with a pointer to the cameo data for the special objects that   *
 *    can appear on the sidebar (e.g., nuclear bomb).                                          *
 *                                                                                             *
 * INPUT:   type  -- The special type to fetch the cameo imagery for.                          *
 *                                                                                             *
 * OUTPUT:  Returns with a pointer to the cameo imagery for the specified special object.      *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/19/1995 JLB : commented                                                                *
 *=============================================================================================*/
const ShapeFileStruct * NewSidebarClass::StripClass::Get_Special_Cameo(SpecialWeaponType type) // DONE
{
    if (type < SuperWeaponTypes.Count()) {
        return SuperWeaponTypes[type]->SidebarIcon;
    }
    return nullptr;
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Init_Clear -- Sets sidebar to a known (and deactivated) state     *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::StripClass::Init_Clear() // DONE
{
    IsScrollingDown = false;
    IsScrolling = false;
    IsBuilding = false;
    Flasher = -1;
    TopIndex = 0;
    Slid = 0;
    BuildableCount = 0;

    /*
    ** Since we're resetting the strips, clear out all the buildables & factory pointers.
    */
    for (int index = 0; index < MAX_BUILDABLES; ++index) {
        Buildables[index].BuildableID = 0;
        Buildables[index].BuildableType = RTTI_NONE;
        Buildables[index].Factory = nullptr;
    }
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Init_IO -- Initializes the strip's buttons                        *
 *                                                                                             *
 * This routine doesn't actually add any buttons to the list.                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::StripClass::Init_IO(int id) // DONE
{
    ID = id;

    UpButton[ID].IsSticky = true;
    UpButton[ID].ID = BUTTON_UP+id;
    UpButton[ID].field_3C = true;
    UpButton[ID].ShapeDrawer = SidebarDrawer;
    UpButton[ID].Flags = GadgetClass::RIGHTPRESS|GadgetClass::RIGHTRELEASE|GadgetClass::LEFTPRESS|GadgetClass::LEFTRELEASE;
    UpButton[ID].IsSticky = true;

    DownButton[ID].IsSticky = true;
    DownButton[ID].ID = BUTTON_DOWN+id;
    DownButton[ID].field_3C = true;
    DownButton[ID].ShapeDrawer = SidebarDrawer;
    DownButton[ID].Flags = GadgetClass::RIGHTPRESS|GadgetClass::RIGHTRELEASE|GadgetClass::LEFTPRESS|GadgetClass::LEFTRELEASE;
    DownButton[ID].IsSticky = true;

    for (int index = 0; index < MAX_VISIBLE; ++index) {
        SelectClass & g = SelectButton[ID][index];
        g.ID = BUTTON_SELECT;
        g.X = SidebarRect.X + X;
        g.Y = SidebarRect.Y + Y + index;
        g.Width = OBJECT_WIDTH;
        g.Height = OBJECT_HEIGHT;
        g.Set_Owner(*this, index);
    }

}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Activate -- Adds the strip buttons to the input system.           *
 *                                                                                             *
 *    This routine will add the side strip buttons to the map's input system. This routine     *
 *    should be called once when the sidebar activates.                                        *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Never call this routine a second time without first calling Deactivate().       *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::StripClass::Activate()
{
    UpButton[ID].Zap();
    Map.Add_A_Button(UpButton[ID]);

    DownButton[ID].Zap();
    Map.Add_A_Button(DownButton[ID]);

    for (int index = 0; index < MAX_VISIBLE; ++index) {
        SelectButton[ID][index].Zap();
        Map.Add_A_Button(SelectButton[ID][index]);
    }
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Deactivate -- Removes the side strip buttons from the input syste *
 *                                                                                             *
 *    Call this routine to remove all the buttons on the side strip from the map's input       *
 *    system.                                                                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Never call this routine unless the Activate() function was previously called.   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::StripClass::Deactivate()
{
    Map.Remove_A_Button(UpButton[ID]);
    Map.Remove_A_Button(DownButton[ID]);
    for (int index = 0; index < MAX_VISIBLE; ++index) {
        Map.Remove_A_Button(SelectButton[ID][index]);
    }
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Add -- Add an object to the side strip.                           *
 *                                                                                             *
 *    Use this routine to add a buildable object to the side strip.                            *
 *                                                                                             *
 * INPUT:   object   -- Pointer to the object type that can be built and is to be added to     *
 *                      the side strip.                                                        *
 *                                                                                             *
 * OUTPUT:  bool; Was the object successfully added to the side strip? Failure could be the    *
 *                result of running out of room in the side strip array or the object might    *
 *                already be in the list.                                                      *
 *                                                                                             *
 * WARNINGS:   none.                                                                           *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::StripClass::Add(RTTIType type, int id)
{
    if (BuildableCount <= MAX_BUILDABLES) {
        for (int index = 0; index < BuildableCount; ++index) {
            if (Buildables[index].BuildableType == type && Buildables[index].BuildableID == id) {
                return false;
            }
        }
        if (!ScenarioInit && type != RTTI_SPECIAL) {
            Speak(VOX_NEW_CONSTRUCT);
        }
        Buildables[BuildableCount].BuildableType = type;
        Buildables[BuildableCount].BuildableID = id;
        BuildableCount++;
        IsToRedraw = true;
        return true;
    }
    return false;
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Scroll -- Causes the side strip to scroll.                        *
 *                                                                                             *
 *    Use this routine to flag the side strip to scroll. The direction scrolled is controlled  *
 *    by the parameter. Scrolling is merely initiated by this routine. Subsequent calls to     *
 *    the AI function and the Draw_It function are required to properly give the appearance    *
 *    of scrolling.                                                                            *
 *                                                                                             *
 * INPUT:   bool; Should the side strip scroll UP? If it is to scroll down then pass false.    *
 *                                                                                             *
 * OUTPUT:  bool; Was the side strip started to scroll in the desired direction?               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *   07/29/1995 JLB : Simplified scrolling logic.                                              *
 *=============================================================================================*/
bool NewSidebarClass::StripClass::Scroll(bool up)
{
    if (up) {
        if (!TopIndex) return false;
        Scroller--;
    } else {
        if (TopIndex+MAX_VISIBLE >= BuildableCount) return false;
        Scroller++;
    }
    return true;
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Flag_To_Redra -- Flags the sidebar strip to be redrawn.           *
 *                                                                                             *
 *    This utility routine is called when something changes on the sidebar and it must be      *
 *    reflected the next time drawing is performed.                                            *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/18/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::StripClass::Flag_To_Redraw()
{
    IsToRedraw = true;
    //Map.NewSidebarClass::IsToRedraw = true;
    Map.Flag_To_Redraw();
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::AI -- Input and AI processing for the side strip.                 *
 *                                                                                             *
 *    The side strip AI processing is performed by this function. This function not only       *
 *    checks for player input, but also handles any graphic logic updating necessary as a      *
 *    result of flashing or construction animation.                                            *
 *                                                                                             *
 * INPUT:   input -- The player input code.                                                    *
 *                                                                                             *
 *          x,y   -- Mouse coordinate to use.                                                  *
 *                                                                                             *
 * OUTPUT:  bool; Did the AI detect that it will need a rendering change? If this routine      *
 *                returns true, then the Draw_It function should be called at the              *
 *                earliest opportunity.                                                        *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *   12/31/1994 JLB : Uses mouse coordinate parameters.                                        *
 *=============================================================================================*/
bool NewSidebarClass::StripClass::AI(KeyNumType & input, Point2D & xy)
{
    bool redraw = false;

    /*
    **  If this is scroll button for this side strip, then scroll the strip as
    **  indicated.
    */
    if (input == (UpButton[ID].ID|KN_BUTTON)) {    // && !IsScrolling
        UpButton[ID].IsPressed = false;
        if (!Scroll(true)) {
            Sound_Effect(Rule->ScoldSound);
        }
    }
    if (input == (DownButton[ID].ID|KN_BUTTON)) {    // && !IsScrolling
        DownButton[ID].IsPressed = false;
        if (!Scroll(false)) {
            Sound_Effect(Rule->ScoldSound);
        }
    }

    /*
    **  Reflect the scroll desired direction/value into the scroll
    **  logic handler. This might result in up or down scrolling.
    */
    if (!IsScrolling && Scroller) {
        if (BuildableCount <= MAX_VISIBLE) {
            Scroller = 0;
        } else {

            /*
            **  Top of list is moving toward lower ordered entries in the object list. It looks like
            **  the "window" to the object list is moving up even though the actual object images are
            **  scrolling downward.
            */
            if (Scroller < 0) {
                if (!TopIndex) {
                    Scroller = 0;
                } else {
                    Scroller++;
                    IsScrollingDown = false;
                    IsScrolling = true;
                    TopIndex--;
                    Slid = 0;
                }

            } else {
                if (TopIndex+MAX_VISIBLE >= BuildableCount) {
                    Scroller = 0;
                } else {
                    Scroller--;
                    Slid = OBJECT_HEIGHT;
                    IsScrollingDown = true;
                    IsScrolling = true;
                }
            }
        }
    }

    /*
    **  Scroll logic is handled here.
    */
    if (IsScrolling) {
        if (IsScrollingDown) {
            Slid -= SCROLL_RATE;
            if (Slid <= 0) {
                IsScrolling = false;
                Slid = 0;
                TopIndex++;
            }
        } else {
            Slid += SCROLL_RATE;
            if (Slid >= OBJECT_HEIGHT) {
                IsScrolling = false;
                Slid = 0;
            }
        }
        redraw = true;
    }

    /*
    **  Handle any flashing logic. Flashing occurs when the player selects an object
    **  and provides the visual feedback of a recognized and legal selection.
    */
    if (Flasher != -1) {
        if (Graphic_Logic()) {
            redraw = true;
            if (Fetch_Stage() >= 7) {
                Set_Rate(0);
                Set_Stage(0);
                Flasher = -1;
            }
        }
    }

    /*
    **  Handle any building clock animation logic.
    */
    if (IsBuilding) {
        for (int index = 0; index < BuildableCount; ++index) {

            if (Buildables[index].Factory) {
                FactoryClass * factory = Buildables[index].Factory;

                if (factory && factory->Has_Changed()) {
                    redraw = true;
                    if (factory->Has_Completed()) {

                        /*
                        **  Construction has been completed. Announce this fact to the player and
                        **  try to get the object to automatically leave the factory. Buildings are
                        **  the main exception to the ability to leave the factory under their own
                        **  power.
                        */
                        TechnoClass * pending = factory->Get_Object();
                        if (pending != nullptr) {
                            switch (pending->What_Am_I()) {
                                //case RTTI_VESSEL:
                                case RTTI_UNIT:
                                case RTTI_AIRCRAFT:
                                    OutList.Add(EventClass(PlayerPtr->ID, PLACE, (RTTIType)pending->What_Am_I(), -1));
                                    Speak(VOX_UNIT_READY);
                                    break;

                                case RTTI_BUILDING:
                                    Speak(VOX_CONSTRUCTION);
                                    break;

                                case RTTI_INFANTRY:
                                    OutList.Add(EventClass(PlayerPtr->ID, PLACE, (RTTIType)pending->What_Am_I(), -1));
                                    Speak(VOX_UNIT_READY);
                                    break;
                            }
                        }
                    }
                }
            }
        }
    }

    /*
    **  If any of the logic determined that this side strip needs to be redrawn, then
    **  set the redraw flag for this side strip.
    */
    if (redraw) {
        Flag_To_Redraw();
    }
    return redraw;
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Draw_It -- Render the sidebar display.                            *
 *                                                                                             *
 *    Use this routine to render the sidebar display. It checks to see if it needs to be       *
 *    redrawn and only redraw if necessary. If the "complete" parameter is true, then it       *
 *    will force redraw the entire strip.                                                      *
 *                                                                                             *
 * INPUT:   complete -- Should the redraw be forced? A force redraw will ignore the redraw     *
 *                      flag.                                                                  *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *   08/06/1995 JLB : Handles multi factory tracking in same strip.                            *
 *=============================================================================================*/
void NewSidebarClass::StripClass::Draw_It(bool complete)
{
    if (IsToRedraw || complete) {
        IsToRedraw = false;

        //SidebarRedraws++;

        /*
        **  Fills the background to the side strip. We shouldnt need to do this if the strip
        **  has a full complement of icons.
        */
        /*
        ** New sidebar needs to be drawn not filled
        */
        if (BuildableCount < MAX_VISIBLE) {
            //CC_Draw_Shape(LogoShapes, ID,    X+(2),    Y,    WINDOW_MAIN, SHAPE_WIN_REL|SHAPE_NORMAL,    0);
        }

        /*
        **  Redraw the scroll buttons.
        */
        UpButton[ID].Draw_Me(true);
        DownButton[ID].Draw_Me(true);

        /*
        **  Loop through all the buildable objects that are visible in the strip and render
        **  them. Their Y offset may be adjusted if the strip is in the process of scrolling.
        */
        for (int i = 0; i < MAX_VISIBLE + (IsScrolling ? 1 : 0); i++) {
            bool production;
            bool completed;
            int  stage;
            bool darken = false;
            void const * shapefile = 0;
            int shapenum = 0;
            void const * remapper = 0;
            FactoryClass * factory = 0;
            int index = i+TopIndex;
            int x = X;
            int y = Y + (i*OBJECT_HEIGHT);

            /*
            **  If the strip is scrolling, then the offset is adjusted accordingly.
            */
            if (IsScrolling) {
                y -= (OBJECT_HEIGHT - Slid);
//                y -= OBJECT_HEIGHT - Slid;
            }

            /*
            **  Fetch the shape number for the object type located at this current working
            **  slot. This shape pointer is used to draw the underlying graphic there.
            */
            if (index < BuildableCount) {
                ObjectTypeClass const * obj = nullptr;
                SpecialWeaponType spc = SPECIAL_NONE;

                if (Buildables[index].BuildableType != RTTI_SPECIAL) {

                    obj = Fetch_Techno_Type(Buildables[index].BuildableType, Buildables[index].BuildableID);
                    if (obj != nullptr) {

                        /*
                        **  Fetch the remap table that is appropriate for this object
                        **  type.
                        */
                        //remapper = PlayerPtr->Remap_Table(false, ((TechnoTypeClass const *)obj)->Remap);

                        /*
                        **  If there is already a factory producing this kind of object, then all
                        **  objects of this type are displays in a disabled state.
                        */
                        bool isbusy = (PlayerPtr->Fetch_Factory(Buildables[index].BuildableType) != nullptr);
                        //if (!isbusy && PlayerPtr->Is_Hack_Prevented(Buildables[index].BuildableType, Buildables[index].BuildableID)) {
                        //    isbusy = true;
                        //}

                        /*
                        **  Infantry don't get remapped in the sidebar (special case).
                        */
                        if (Buildables[index].BuildableType == RTTI_INFANTRYTYPE) {
                            remapper = 0;
                        }

                        shapefile = obj->Get_Cameo_Data();
                        shapenum  = 0;
                        if (Buildables[index].Factory) {
                            production = true;
                            completed = Buildables[index].Factory->Has_Completed();
                            stage = Buildables[index].Factory->Completion();
                            darken = false;
                        } else {
                            production = false;
//                            darken = IsBuilding;

                            /*
                            **  Darken the imagery if a factory of a matching type is
                            **  already busy.
                            */
                            darken = isbusy;
                        }
                    } else {
                        //darken = PlayerPtr->Is_Hack_Prevented(Buildables[index].BuildableType, Buildables[index].BuildableID);
                    }

                } else  {

                    spc = SpecialWeaponType(Buildables[index].BuildableID);
                    shapefile = Get_Special_Cameo(spc);
                    shapenum = 0;

                    production = true;
                    //completed = PlayerPtr->SuperWeapon[spc].Is_Ready();
                    //stage = PlayerPtr->SuperWeapon[spc].Anim_Stage();
                    darken = false;
                }

                if (obj != nullptr || spc != SPECIAL_NONE) {
                    /*
                    ** If this item is flashing then take care of it.
                    **
                    */
                    if (Flasher == index && (Fetch_Stage() & 0x01)) {
                        //remapper = Map.FadingLight;
                    }

                } else {
                    shapefile = LogoShapes;
                    if (!darken) {
                        shapenum = SB_BLANK;
                    }
                }
            } else {
                shapefile = LogoShapes;
                shapenum = SB_BLANK;
                production = false;
            }

            remapper = 0;
            /*
            **  Now that the shape of the object at the current working slot has been found,
            **  draw it and any graphic overlays as necessary.
            **
            ** Don't draw blank shapes over the new 640x400 sidebar art - ST 5/1/96 6:01PM
            */
            if (shapenum != SB_BLANK || shapefile != LogoShapes) {
                //CC_Draw_Shape(shapefile, shapenum,
                //    x-(WindowList[WINDOW_SIDEBAR][WINDOWX])+(LEFT_EDGE_OFFSET),
                //    y-WindowList[WINDOW_SIDEBAR][WINDOWY],
                //    WINDOW_SIDEBAR,
                //    SHAPE_NORMAL|SHAPE_WIN_REL| (remapper ? SHAPE_FADING : SHAPE_NORMAL),
                //    remapper);
                //
                ///*
                //**  Darken this object because it cannot be produced or is otherwise
                //**  unavailable.
                //*/
                //if (darken) {
                //    CC_Draw_Shape(ClockShapes, 0,
                //            x-(WindowList[WINDOW_SIDEBAR][WINDOWX])+(LEFT_EDGE_OFFSET),
                //            y-WindowList[WINDOW_SIDEBAR][WINDOWY],
                //            WINDOW_SIDEBAR,
                //            SHAPE_NORMAL|SHAPE_WIN_REL|SHAPE_GHOST,
                //            nullptr, ClockTranslucentTable);
                //}
            }

            /*
            **  Draw the overlapping clock shape if this is object is being constructed.
            **  If the object is completed, then display "Ready" with no clock shape.
            */
            if (production) {
                //if (completed) {
                //
                //    /*
                //    **  Display text showing that the object is ready to place.
                //    */
                //    CC_Draw_Shape(ObjectTypeClass::PipShapes, PIP_READY,
                //    (x-(WindowList[WINDOW_SIDEBAR][WINDOWX]))+(LEFT_EDGE_OFFSET+15),
                //    (y-WindowList[WINDOW_SIDEBAR][WINDOWY])+(4),
                //    WINDOW_SIDEBAR, SHAPE_CENTER);
                //} else {
                //
                //    CC_Draw_Shape(ClockShapes, stage+1,
                //            x-(WindowList[WINDOW_SIDEBAR][WINDOWX])+(LEFT_EDGE_OFFSET),
                //            y-WindowList[WINDOW_SIDEBAR][WINDOWY],
                //            WINDOW_SIDEBAR,
                //            SHAPE_NORMAL|SHAPE_WIN_REL|SHAPE_GHOST,
                //            nullptr, ClockTranslucentTable);
                //
                //    /*
                //    **  Display text showing that the construction is temporarily on hold.
                //    */
                //    if (factory && !factory->Is_Building()) {
                //        CC_Draw_Shape(ObjectTypeClass::PipShapes, PIP_HOLDING,
                //        (x-(WindowList[WINDOW_SIDEBAR][WINDOWX])) + ((LEFT_EDGE_OFFSET+15)),
                //        (y-WindowList[WINDOW_SIDEBAR][WINDOWY])+(4),
                //        WINDOW_SIDEBAR, SHAPE_CENTER);
                //    }
                //}
            }

        }

        LastSlid = Slid;
    }
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Recalc -- Revalidates the current sidebar list of objects.        *
 *                                                                                             *
 *    This routine will revalidate all the buildable objects in the sidebar. This routine      *
 *    comes in handy when a factory has been destroyed, and the sidebar needs to reflect any   *
 *    change that this requires. It checks every object to see if there is a factory available *
 *    that could produce it. If none can be found, then the object is removed from the         *
 *    sidebar.                                                                                 *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  bool; The sidebar has changed as a result of this call?                            *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *   06/26/1995 JLB : Doesn't collapse sidebar when buildables removed.                        *
 *=============================================================================================*/
bool NewSidebarClass::StripClass::Recalc()
{
    int ok;

    if (Debug_Map || !BuildableCount) {
        return false;
    }

    /*
    **  Sweep through all objects listed in the sidebar. If any of those object can
    **  not be created -- even in theory -- then they must be removed form the sidebar and
    **  any current production must be abandoned.
    */
    bool redraw = false;
    for (int index = 0; index < BuildableCount; ++index) {
        TechnoTypeClass const * tech = Fetch_Techno_Type(Buildables[index].BuildableType, Buildables[index].BuildableID);
        if (tech != nullptr) {
            //ok = tech->Who_Can_Build_Me(true, ?, false, PlayerPtr->Class->House) != nullptr;
        } else {
            if (Buildables[index].BuildableID < SPECIAL_COUNT) {
                ok = PlayerPtr->SuperWeapon[Buildables[index].BuildableID].Is_Present();
            } else {
                ok = false;
            }
        }

        if (!ok) {

            /*
            **  Removes this entry from the list.
            */
            if (BuildableCount > 1 && index < BuildableCount-1) {
                memcpy(&Buildables[index], &Buildables[index+1], sizeof(Buildables[0])*((BuildableCount-index)-1));
            }
            TopIndex = 0;
            IsToRedraw = true;
            redraw = true;
            BuildableCount--;
            index--;
        }
    }

#ifdef NEVER
    /*
    **  If there are no more buildable objects to display, make the sidebar go away.
    */
    if (!BuildableCount) {
        Map.NewSidebarClass::Activate(0);
    }
#endif
    return redraw;
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::SelectClass::SelectClass -- Default constructor.                  *
 *                                                                                             *
 *    This is the default constructor for the button that controls the buildable cameos on     *
 *    the sidebar strip.                                                                       *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   The coordinates are set to zero by this routine. They must be set to the        *
 *             correct values before this button will function.                                *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
NewSidebarClass::StripClass::SelectClass::SelectClass() :
    ControlClass(0, 0, 0, (OBJECT_WIDTH-1), OBJECT_HEIGHT, LEFTPRESS|RIGHTPRESS|LEFTUP),
    Strip(0),
    Index(0)
{
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::SelectClass:: -- Assigns special values to a buildable select but *
 *                                                                                             *
 *    Use this routine to set custom buildable vars for this particular select button. It      *
 *    uses this information to properly know what buildable object to start or stop production *
 *    on.                                                                                      *
 *                                                                                             *
 * INPUT:   strip    -- Reference to the strip that owns this buildable button.                *
 *                                                                                             *
 *          index    -- The index (0 .. MAX_VISIBLE-1) of this button. This is used to let     *
 *                      the owning strip know what index this button refers to.                *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void NewSidebarClass::StripClass::SelectClass::Set_Owner(StripClass & strip, int index) // DONE
{
    Strip = &strip;
    Index = index;
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::SelectClass:: -- Action function when buildable cameo is selected *
 *                                                                                             *
 *    This function is called when the buildable icon (cameo) is clicked on. It handles        *
 *    starting and stopping production as indicated.                                           *
 *                                                                                             *
 * INPUT:   flags -- The input event that triggered the call.                                  *
 *                                                                                             *
 *          key   -- The keyboard value at the time of the input.                              *
 *                                                                                             *
 * OUTPUT:  Returns with whether the input list should be scanned further.                     *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *   10/09/1996 JLB : Sonar pulse converted to regular event type.                             *
 *=============================================================================================*/
bool NewSidebarClass::StripClass::SelectClass::Action(unsigned flags, KeyNumType & key)
{
    int index = Strip->TopIndex + Index;
    RTTIType otype = Strip->Buildables[index].BuildableType;
    int oid = Strip->Buildables[index].BuildableID;
    FactoryClass * fnumber = Strip->Buildables[index].Factory;
    //RemapControlType * scheme = GadgetClass::Get_Color_Scheme();

    ObjectTypeClass const * choice = nullptr;
    SpecialWeaponType spc = SPECIAL_NONE;

    /*
    **  Determine the factory number that would apply to objects of the type
    **  the mouse is currently addressing. This doesn't mean that the factory number
    **  fetched is actually producing the indicated object, merely that that particular
    **  kind of factory is specified by the "genfactory" value. This can be used to see
    **  if the factory type is currently busy or not.
    */
    FactoryClass * factory = PlayerPtr->Fetch_Factory(otype);

    Map.Override_Mouse_Shape(MOUSE_NORMAL);

    if (index < Strip->BuildableCount) {
        if (otype != RTTI_SPECIAL) {
            choice = Fetch_Techno_Type(otype, oid);
        } else {
            spc = SpecialWeaponType(oid);
        }

        if (fnumber) {
            factory = fnumber;
        }

      } else {
          //Map.Help_Text(TXT_NONE);
      }

    if (spc != SPECIAL_NONE) {

        /*
        **  Display the help text if the mouse is over the button.
        */
        if (flags & LEFTUP) {
            //Map.Help_Text(SpecialWeaponHelp[spc], X, Y, scheme->Color, true);
            flags &= ~LEFTUP;
        }

        /*
        **  A right mouse button signals "cancel".  If we are in targeting
        ** mode then we don't want to be any more.
        */
        if (flags & RIGHTPRESS) {
            Map.TargettingType = SPECIAL_NONE;
        }
        /*
        **  A left mouse press signal "activate".  If our weapon type is
        ** available then we should activate it.
        */
        if (flags & LEFTPRESS) {

            if (spc < SPECIAL_COUNT) {
                if (PlayerPtr->SuperWeapon[spc].Is_Ready()) {
                    //if (spc != SPECIAL_SONAR_PULSE) {
                    //    Map.TargettingType = spc;
                    //    Unselect_All();
                    //    Speak(VOX_SELECT_TARGET);
                    //} else {
                    //    OutList.Add(EventClass(PlayerPtr->ID, SPECIAL_PLACE, SPECIAL_SONAR_PULSE, 0));
                    //}
                } else {
                    PlayerPtr->SuperWeapon[spc].Impatient_Click();
                }
            }
        }

    } else {

        if (choice != nullptr) {

            /*
            **  Display the help text if the mouse is over the button.
            */
            if (flags & LEFTUP) {
                //Map.Help_Text(choice->Full_Name(), X, Y, scheme->Color, true);
                //Map.Set_Cost(choice->Cost_Of() * PlayerPtr->CostBias);
                flags &= ~LEFTUP;
            }

            /*
            **  A right mouse button signals "cancel".
            */
            if (flags & RIGHTPRESS) {

                /*
                **  If production is in progress, put it on hold. If production is already
                **  on hold, then abandon it. Money will be refunded, the factory
                **  manager deleted, and the object under construction is returned to
                **  the free pool.
                */
                if (factory != nullptr) {

                    /*
                    **  Cancels placement mode if the sidebar factory is abandoned or
                    **  suspended.
                    */
                    if (Map.PendingObjectPtr && Map.PendingObjectPtr->Is_Techno()) {
                        Map.PendingObjectPtr = 0;
                        Map.PendingObject = 0;
                        Map.PendingHouse = HOUSE_NONE;
                        Map.Set_Cursor_Shape(0);
                    }

                    if (!factory->Is_Building()) {
                        Speak(VOX_CANCELED);
                        OutList.Add(EventClass(ABANDON, otype, oid));
                    } else {
                        Speak(VOX_SUSPENDED);
                        OutList.Add(EventClass(SUSPEND, otype, oid));
                        Map.Column[0].IsToRedraw = true;
                        Map.Column[1].IsToRedraw = true;
                    }
                }
            }

            if (flags & LEFTPRESS) {

                /*
                **  If there is already a factory attached to this strip but the player didn't click
                **  on the icon that has the attached factory, then say that the factory is busy and
                **  ignore the click.
                */
                if (fnumber == nullptr && factory != nullptr) {
                    Speak(VOX_NO_FACTORY);
                    ControlClass::Action(flags, key);
                    return true;
                }

                if (factory != nullptr) {

                    /*
                    **  If this object is currently being built, then give a scold sound and text and then
                    **  bail.
                    */
                    if (factory->Is_Building()) {
                        Speak(VOX_NO_FACTORY);
                    } else {

                        /*
                        **  If production has completed, then attempt to have the object exit
                        **  the factory or go into placement mode.
                        */
                        if (factory->Has_Completed()) {

                            TechnoClass * pending = factory->Get_Object();
                            if (!pending && factory->Get_Special_Item()) {
                                Map.TargettingType = (SpecialWeaponType)1; //SPECIAL_ANY;
                            } else {
                                BuildingClass * builder = pending->Who_Can_Build_Me(false, false);
                                if (!builder) {
                                    OutList.Add(EventClass(ABANDON, otype, oid));
                                    Speak(VOX_NO_FACTORY);
                                } else {

                                    /*
                                    **  If the completed object is a building, then change the
                                    **  game state into building placement mode. This fact is
                                    **  not transmitted to any linked computers until the moment
                                    **  the building is actually placed down.
                                    */
                                    if (pending->What_Am_I() == RTTI_BUILDING) {
                                         PlayerPtr->Manual_Place(builder, (BuildingClass *)pending);
                                    } else {

                                        /*
                                        **  For objects that can leave the factory under their own
                                        **  power, queue this event and process through normal house
                                        **  production channels.
                                        */
                                        OutList.Add(EventClass(PLACE, otype, -1));
                                    }
                                }
                            }
                        } else {

                            /*
                            **  The factory must have been in a suspended state. Resume construction
                            **  normally.
                            */
                            if (otype == RTTI_INFANTRYTYPE) {
                                Speak(VOX_TRAINING);
                            } else {
                                Speak(VOX_BUILDING);
                            }
                            OutList.Add(EventClass(PRODUCE, Strip->Buildables[index].BuildableType, Strip->Buildables[index].BuildableID));
                        }
                    }

                } else {

                    /*
                    **  If this side strip is already busy with production, then ignore the
                    **  input and announce this fact.
                    */
                    if (otype == RTTI_INFANTRYTYPE) {
                        Speak(VOX_TRAINING);
                    } else {
                        Speak(VOX_BUILDING);
                    }
                    OutList.Add(EventClass(PRODUCE, Strip->Buildables[index].BuildableType, Strip->Buildables[index].BuildableID));
                }
            }
        } else {
            flags = 0;
        }
    }

    ControlClass::Action(flags, key);
    return true;
}


/***********************************************************************************************
 * NewSidebarClass::SBGadgetClass::Action -- Special function that controls the mouse over the si *
 *                                                                                             *
 *    This routine is called whenever the mouse is over the sidebar. It makes sure that the    *
 *    mouse is always the normal shape while over the sidebar.                                 *
 *                                                                                             *
 * INPUT:   flags -- The event flags that resulted in this routine being called.               *
 *                                                                                             *
 *          key   -- Reference the keyboard code that may be present.                          *
 *                                                                                             *
 * OUTPUT:  Returns that no further keyboard processing is necessary.                          *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::SBGadgetClass::Action(unsigned flags, KeyNumType & key)
{
    //Map.Help_Text(TXT_NONE);
    Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
    return true;
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Factory_Link -- Links a factory to a sidebar button.              *
 *                                                                                             *
 *    This routine will link the specified factory to this sidebar strip. The exact button to  *
 *    link to is determined from the object type and id specified. A linked button is one that *
 *    will show appropriate construction animation (clock shape) that matches the state of     *
 *    the factory.                                                                             *
 *                                                                                             *
 * INPUT:   factory  -- The factory number to link to the sidebar.                             *
 *                                                                                             *
 *          type     -- The object type that this factory refers to.                           *
 *                                                                                             *
 *          id       -- The object sub-type that this factory refers to.                       *
 *                                                                                             *
 * OUTPUT:  Was the factory successfully attached? Failure would indicate that there is no     *
 *          object of the specified type and sub-type in the sidebar list.                     *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/18/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::StripClass::Factory_Link(FactoryClass * factory, RTTIType type, int id) // DONE
{
    for (int index = 0; index < BuildableCount; ++index) {
        if (Buildables[index].BuildableType == type && Buildables[index].BuildableID == id) {
            Buildables[index].Factory = factory;
            IsBuilding = true;
            /*
            ** Flag that all the icons on this strip need to be redrawn
            */
            Flag_To_Redraw();
            return true;
        }
    }
    return false;
}


/***********************************************************************************************
 * NewSidebarClass::Abandon_Production -- Stops production of the object specified.               *
 *                                                                                             *
 *    This routine is used to abandon production of the object specified. The factory will     *
 *    be completely disabled by this call.                                                     *
 *                                                                                             *
 * INPUT:   type     -- The object type that is to be abandoned. The sub-type is not needed    *
 *                      since it is presumed there can be only one type in production at any   *
 *                      one time.                                                              *
 *                                                                                             *
 *          factory  -- The factory number that is doing the production.                       *
 *                                                                                             *
 * OUTPUT:  Was the factory successfully abandoned?                                            *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/18/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool NewSidebarClass::Abandon_Production(RTTIType type, FactoryClass * factory) // DONE
{
    return Column[Which_Column(type)].Abandon_Production(factory);
}


/***********************************************************************************************
 * NewSidebarClass::StripClass::Abandon_Produ -- Abandons production associated with sidebar.     *
 *                                                                                             *
 *    Production of the object associated with this sidebar is abandoned when this routine is  *
 *    called.                                                                                  *
 *                                                                                             *
 * INPUT:   factory  -- The factory index that is to be suspended.                             *
 *                                                                                             *
 * OUTPUT:  Was the production abandonment successful?                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/18/1995 JLB : Created.                                                                 *
 *   08/06/1995 JLB : More intelligent abandon logic for multiple factories.                   *
 *=============================================================================================*/
bool NewSidebarClass::StripClass::Abandon_Production(FactoryClass * factory)
{
    bool noprod = true;
    bool abandon = false;
    for (int index = 0; index < BuildableCount; ++index) {
        if (Buildables[index].Factory == factory) {
            Buildables[index].Factory->Abandon();
            Buildables[index].Factory = nullptr;
            abandon = true;
        } else {
            if (Buildables[index].Factory != nullptr) {
                noprod = false;
            }
        }
    }

    /*
    **  If there was a change to the strip, then flag the strip to be redrawn.
    */
    if (abandon) {
        Flag_To_Redraw();
    }

    /*
    **  If there is no production whatsoever on this strip, then flag it so.
    */
    if (noprod) {
        IsBuilding = false;
    }
    return abandon;
}
