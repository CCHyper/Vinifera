/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SHIPLOCOMOTION.H
 *
 *  @authors       CCHyper
 *
 *  @brief         Ship locomotion.
 *
 *  @license       Vinifera is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 Vinifera is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY     without even the implied
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
//#include "locomotion.h"
#include "drivelocomotion.h"
#include "vinifera_defines.h"


class DECLSPEC_UUID(CLSID_SHIP_LOCOMOTOR)
//ShipLocomotionClass : public LocomotionClass
ShipLocomotionClass : public DriveLocomotionClass
{
    public:
        /**
         *  IUnknown
         */
        IFACEMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObj);
        IFACEMETHOD_(ULONG, AddRef)();
        IFACEMETHOD_(ULONG, Release)();

        /**
         *  IPersist
         */
        IFACEMETHOD(GetClassID)(CLSID *pClassID);
        
        /**
         *  IPersistStream
         */
        IFACEMETHOD(Load)(IStream *pStm);
        IFACEMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
        IFACEMETHOD_(LONG, GetSizeMax)(ULARGE_INTEGER *pcbSize);

        /**
         *  ILocomotion
         */
        IFACEMETHOD(Link_To_Object)(void *object);
        //IFACEMETHOD_(bool, Is_Moving)();
        IFACEMETHOD_(Coordinate, Destination)();
        IFACEMETHOD_(Coordinate, Head_To_Coord)();
        //IFACEMETHOD_(Matrix3D, Draw_Matrix)(int *key);
        IFACEMETHOD_(Matrix3D, Shadow_Matrix)(int *key);
        IFACEMETHOD_(int, Z_Adjust)();
        IFACEMETHOD_(ZGradientType, Z_Gradient)();
        //IFACEMETHOD_(bool, Process)();
        IFACEMETHOD_(void, Move_To)(Coordinate to);
        IFACEMETHOD_(void, Stop_Moving)();
        IFACEMETHOD_(void, Do_Turn)(DirStruct coord);
        IFACEMETHOD_(void, Unlimbo)();
        //IFACEMETHOD_(void, Force_Track)(int track, Coordinate coord);
        IFACEMETHOD_(LayerType, In_Which_Layer)();
        IFACEMETHOD_(void, Force_New_Slope)(TileRampType ramp);
        IFACEMETHOD_(bool, Is_Moving_Now)();
        IFACEMETHOD_(void, Mark_All_Occupation_Bits)(MarkType mark);
        IFACEMETHOD_(bool, Is_Moving_Here)(Coordinate to);
        //IFACEMETHOD_(bool, Will_Jump_Tracks)();
        IFACEMETHOD_(void, Lock)();
        IFACEMETHOD_(void, Unlock)();
        IFACEMETHOD_(int, Get_Track_Number)();
        IFACEMETHOD_(int, Get_Track_Index)();
        IFACEMETHOD_(int, Get_Speed_Accum)();

    public:
        virtual int Size_Of(bool firestorm = false) const override { return sizeof(*this); }

    public:
        ShipLocomotionClass();
        ~ShipLocomotionClass();

    private:
        Coordinate Smooth_Turn(Coordinate adj, DirType &dir);

    private:
        /**
         *  These enumerations are used as working constants that exist only
         *  in the ShipClass namespace.
         */
        enum ShipClassEnum {
            BACKUP_INTO_REFINERY = 64,     // Track to backup into refinery.
            OUT_OF_REFINERY,               // Track to leave refinery.
            OUT_OF_WEAPON_FACTORY          // Track to leave weapons factory.
        };

        /**
         *  Smooth turning tracks are controlled by this structure and these
         *  processing bits.
         */
        typedef enum TrackControlType {
            F_ = 0x00,        // No translation necessary?
            F_T = 0x01,       // Transpose X and Y components?
            F_X = 0x02,       // Reverse X component sign?
            F_Y = 0x04,       // Reverse Y component sign?
            F_D = 0x08        // Two cell consumption?
        } TrackControlType;

        typedef struct {
            char Track;                 // Which track to use.
            char StartTrack;            // Track when starting from stand-still.
            char field_2;
            char field_3;
            DirType Facing;             // Facing when track has been completed.
            TrackControlType Flag;      // List processing flag bits.
        } TurnTrackType;

        typedef struct {
            Coordinate Offset;          // Offset to origin coordinate.
            DirType Facing;             // Facing (primary track).
        } TrackType;

        typedef struct {
            const TrackType * Track;    // Pointer to track list.
            int Jump;                   // Index where track jumping is allowed.
            int Entry;                  // Entry point if jumping to this track.
            int Cell;                   // Per cell process should occur at this index.
        } RawTrackType;
        
        static TurnTrackType const TrackControl[67]; // less than drive.
        static RawTrackType const RawTracks[13]; // 2 less than drive.
        static TrackType const Track13[];
        static TrackType const Track12[];
        static TrackType const Track11[];
        static TrackType const Track10[];
        static TrackType const Track9[];
        static TrackType const Track8[];
        static TrackType const Track7[];
        static TrackType const Track6[];
        static TrackType const Track5[];
        static TrackType const Track4[];
        static TrackType const Track3[];
        static TrackType const Track2[];
        static TrackType const Track1[24];

#if 0
    protected:

        TileRampType CurrentRamp;
        TileRampType PreviousRamp;
        CDRateTimerClass<FrameTimerClass> RampTransitionTimer;

        /**
         *  This is the desired destination of the unit. The unit will attempt
         *  to head toward this target (avoiding intervening obstacles).
         */
        TARGET DestinationCoord;

        /**
         *  This is the coordinate that the unit is heading to
         *  as an immediate destination. This coordinate is never further
         *  than once cell (or track) from the unit's location. When this coordinate
         *  is reached, then the next location in the path list becomes the
         *  next HeadTo coordinate.
         */
        Coordinate HeadToCoord;

        /**
         *  These speed values are used to accumulate movement and then
         *  convert them into pixel "steps" that are then translated through
         *  the currently running track so that the unit will move.
         */
        int SpeedAccum;

        double field_50; // slope related, speed adjuster/multiplier?

        /**
         *  This the track control logic (used for ground vehicles only). The 'Track'
         *  variable holds the track being followed (0 == not following track). The
         *  'TrackIndex' variable holds the current index into the specified track
         *  (starts at 0).
         */
        int TrackNumber;
        int TrackIndex;

        /**
         *  This vehicle could be processing a "short track". A short track is one that
         *  doesn't actually go anywhere. Kind of like turning in place.
         */
        bool IsOnShortTrack;

        /**
         *  Some units must have their turret locked down to face their body direction.
         *  When this flag is set, this condition is in effect. This flag is a more
         *  accurate check than examining the TrackNumber since the turret may be
         *  rotating into position so that a pending track may start. During this process
         *  the track number does not indicate anything.
         */
        bool IsTurretLockedDown;

        // TODO, this is RA's FootClass IsRotating?
        bool IsRotating;

        // TODO, this is RA's FootClass IsDriving?
        bool IsDriving;

        bool IsRocking;

        bool IsLocked;

        ILocomotionPtr Piggybacker;
#endif

    private:
        // copy and assignment not implemented; prevent their use by declaring as private.
        ShipLocomotionClass(const ShipLocomotionClass &) = delete;
        ShipLocomotionClass &operator=(const ShipLocomotionClass &) = delete;
};
