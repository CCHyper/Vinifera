#pragma once

#include "iomap.h"


class TriggerClass;
class Point2D;


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
        static bool Load_Scenario();
        static bool Save_Scenario(bool emergency = false);
        static bool Size_Map(int x, int y, int w, int h);

    protected:
        bool Mouse_Moved();
        bool Move_Grabbed_Object();
        bool Place_Object();
        void Start_Placement();

    protected:

        /**
         *  All class members must be static!
         */

        /**
         *  
         */
        static ControlClass *MapArea;

        /**
         *  The currently-selected cell for the Scenario Editor.
         */
        static Cell CurrentCell;

        /**
         *  
         */
        static TriggerClass *CurTrigger;
        static TagClass *CurTag;

        /**
         *  Object "grabbed" with mouse.
         */
        static ObjectClass *GrabbedObject;

        /**
         *  Offset to grabbed obj's upper-left.
         */
        static Cell GrabOffset;

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
        static bool IsMarbleMadnessAvailable;
};


extern MapEditClass MapEdit;
