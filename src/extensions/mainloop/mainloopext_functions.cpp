/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MAINLOOPEXT_FUNCTIONS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         x
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
#include "mainloopext_functions.h"
#include "tibsun_globals.h"
#include "tibsun_functions.h"
#include "session.h"
#include "tactical.h"
#include "iomap.h"
#include "object.h"
#include "event.h"
#include "target.h"
#include "house.h"
#include "housetype.h"
#include "wwmouse.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


// https://github.com/mvdhout1992/ts-patches/commit/40e31c3ebf472f73854e991bdb9c1925e0bdb212


bool SuperRecord = false;


/**
 *  x
 */
#ifndef NDEBUG
#define DEBUG_RECORD_PLAYBACK 1
#endif


/**
 *  Handles saving/loading map pos & current object.
 */
void Vinifera_Do_Record_Playback()
{
    /**
     *  Record a game.
     */
    if (Session.Record) {

#ifdef DEBUG_RECORD_PLAYBACK
        DEBUG_INFO("Record -  Frame: %d\n", Frame);
#endif

        /**
         *  For 'SuperRecord', we'll open & close the file with every entry.
         */
        if (SuperRecord) {
            Session.RecordFile.Open(FILE_ACCESS_READ_WRITE);
            Session.RecordFile.Seek(0, FILE_SEEK_END);
        }

        /**
         *  Save the mouse's location.
         */
        Session.RecordFile.Write(&WWMouse->Get_Mouse_XY(),
            sizeof(Point2D));

        /**
         *  Save the map's location.
         */
        Session.RecordFile.Write(&TacticalMap->Get_Desired_Tactical_Position(),
            sizeof(Point2D));

        /**
         *  Save the current object list count.
         */
        int count = CurrentObjects.Count();
        Session.RecordFile.Write(&count, sizeof(count));

        /**
         *  Save a CRC of the selected-object list.
         */
        unsigned long sum = 0;
        for (int i = 0; i < count; ++i) {
            sum += TargetClass(CurrentObjects[i]).Value();
        }
        Session.RecordFile.Write(&sum, sizeof(sum));

        /**
         *  Save all selected objects.
         */
        for (int i = 0; i < count; ++i) {
            unsigned long tgt = TargetClass(CurrentObjects[i]).Pack();
            Session.RecordFile.Write(&tgt, sizeof(tgt));
        }

        /**
         *  Save team-selection and formation events.
         */
        Session.RecordFile.Write(&TeamEvent, sizeof(TeamEvent));
        Session.RecordFile.Write(&TeamNumber, sizeof(TeamNumber));
        TeamEvent = 0;
        TeamNumber = 0;

        /**
         *  If 'SuperRecord', close the file now.
         */
        if (SuperRecord) {
            Session.RecordFile.Close();
        }
    }

    /**
     *  Play back a game ("attract" mode).
     */
    if (Session.Play) {

#ifdef DEBUG_RECORD_PLAYBACK
        DEBUG_INFO("Play -  Frame: %d\n", Frame);
#endif

        /**
         *  Read & set the mouse's position.
         */
        Point2D mousepos;
        if (Session.RecordFile.Read(&mousepos, sizeof(mousepos)) == sizeof(mousepos)) {
            // Nop
        }

        /**
         *  Read & set the map's location.
         */
        Coordinate coord;
        if (Session.RecordFile.Read(&coord, sizeof(coord)) == sizeof(coord)) {
            TacticalMap->Set_Desired_Tactical_Position(coord);
        }

        int count = 0;
        if (Session.RecordFile.Read(&count, sizeof(count)) == sizeof(count)) {
            /**
             *  Compute a CRC of the current object-selection list.
             */
            unsigned long sum = 0;
            for (int i = 0; i < CurrentObjects.Count(); ++i) {
                sum += TargetClass(CurrentObjects[i]).Pack();
            }

            /**
             *  Load the CRC of the objects on disk; if it doesn't match, select
             *  all objects as they're loaded.
             */
            unsigned long sum2 = 0;
            Session.RecordFile.Read(&sum2, sizeof(sum2));
            if (sum2 != sum) {
                Unselect_All();
            }

            AllowVoice = true;

            for (int i = 0; i < count; ++i) {
                unsigned long tgt = 0;
                if (Session.RecordFile.Read(&tgt, sizeof(tgt)) == sizeof(tgt)) {
                    xTargetClass t;
                    t.Unpack(tgt);
                    ObjectClass *obj = t.As_Object();
                    if (obj && (sum2 != sum)) {
                        obj->Select();
                        AllowVoice = false;
                    }
                }
            }

            AllowVoice = true;
        }

        /**
         *  Save team-selection and formation events.
         */
        Session.RecordFile.Read(&TeamEvent, sizeof(TeamEvent));
        Session.RecordFile.Read(&TeamNumber, sizeof(TeamNumber));
        //if (TeamEvent) {
        //    Handle_Team(TeamNumber, TeamEvent - 1);
        //}

        /**
         *  The map isn't drawn in playback mode, so draw it here.
         */
        Map.Flag_To_Redraw();
        Map.Render();
    }
}


/**
 *  Saves multiplayer-specific recording values.
 */
bool Vinifera_Save_Recording_Values(CCFileClass &file)
{
    DEBUG_INFO("Saving recording values for scenario : %s\n", Scen->ScenarioName);

    file.Write(&BuildLevel, sizeof(BuildLevel));
    file.Write(&Debug_Unshroud, sizeof(Debug_Unshroud));
    file.Write(&Seed, sizeof(Seed));
    file.Write(&Scen->Scenario, sizeof(Scen->Scenario));
    file.Write(Scen->ScenarioName, sizeof(Scen->ScenarioName));
    file.Write(&Whom, sizeof(Whom));
    file.Write(&Special, sizeof(SpecialClass));
    file.Write(&Options, sizeof(OptionsClass));

    return true;
}


/**
 *  Loads multiplayer-specific recording values.
 */
bool Vinifera_Load_Recording_Values(CCFileClass &file)
{
    file.Read(&BuildLevel, sizeof(BuildLevel));
    file.Read(&Debug_Unshroud, sizeof(Debug_Unshroud));
    file.Read(&Seed, sizeof(Seed));
    file.Read(&Scen->Scenario, sizeof(Scen->Scenario));
    file.Read(Scen->ScenarioName, sizeof(Scen->ScenarioName));
    file.Read(&Whom, sizeof(Whom));
    file.Read(&Special, sizeof(SpecialClass));
    file.Read(&Options, sizeof(OptionsClass));

    DEBUG_INFO("Loaded recording values for scenario : %s\n", Scen->ScenarioName);

    return true;
}


/**
 *  Records the DoList to disk.
 */
void Vinifera_Queue_Record()
{
    /**
     *  Compute # of events to save this frame.
     */
    int j = 0;
    for (int i = 0; i < DoList.Count; ++i) {
        if (Frame == DoList[i].Frame && !DoList[i].IsExecuted) {
            j++;
        }
    }

    /**
     *  Save the # of events, then all events.
     */
    Session.RecordFile.Write(&j, sizeof(j));
    for (int i = 0; i < DoList.Count; ++i) {
        if (Frame == DoList[i].Frame && !DoList[i].IsExecuted) {
            Session.RecordFile.Write(&DoList[i], sizeof(EventClass));
            j--;
        }
    }

}


/**
 *  Plays back queue entries from a record file.
 */
void Vinifera_Queue_Playback()
{
    static int mx = 0;
    static int my = 0;

    /**
     *  If the user hits ESC, stop the playback.
     */
    if (WWKeyboard->Check()) {
        unsigned short key = WWKeyboard->Get();
        if (key == KA_ESC || Session.Attract) {
            GameActive = false;
            return;
        }
    }

    /**
     *  If we're in "Attract" mode, and the user moves the mouse, stop the playback.
     */
    if (Session.Attract && Frame > 0 && (mx != WWMouse->Get_Mouse_X() || my != WWMouse->Get_Mouse_Y())) {
        GameActive = false;
        return;
    }
    mx = WWMouse->Get_Mouse_X();
    my = WWMouse->Get_Mouse_Y();

    /**
     *  Compute the Game's CRC.
     */
    Compute_Game_CRC();
    CRC[Frame & 0x001f] = GameCRC;

    /**
     *  If we've reached the CRC print frame, do so & exit.
     */
    if (Frame >= Session.TrapPrintCRC) {
        Print_CRCs(nullptr);
        //Prog_End("Queue_Playback reached CRC print frame", true);
        Emergency_Exit(0);
    }

    /**
     *  Don't read anything the first time through (since the Queue_AI_Network
     *  routine didn't write anything the first time through); do this after the
     *  CRC is computed, since we'll still need a CRC for Frame 0.
     */
    if (Frame == 0 && Session.Type != GAME_NORMAL) {
        return;
    }

    /**
     *  Only process every 'FrameSendRate' frames.
     */
    int testframe = ((Frame + (Session.FrameSendRate - 1)) / Session.FrameSendRate) * Session.FrameSendRate;
    if ((Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) && Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
        if (Frame != testframe) {
            return;
        }
    }

    /**
     *  Read the DoList from disk.
     */
    bool ok = true;
    int numevents = 0;
    if (Session.RecordFile.Read(&numevents, sizeof(numevents)) == sizeof(numevents)) {
        for (int i = 0; i < numevents; i++) {
            EventClass event;
            if (Session.RecordFile.Read(&event, sizeof(EventClass)) == sizeof(EventClass)) {
                event.IsExecuted = false;
                DoList.Add(event);
            } else {
                ok = false;
                break;
            }
        }
    } else {
        ok = false;
    }

    if (!ok) {
        GameActive = false;
        return;
    }

    /**
     *  Execute the DoList; if an error occurs, bail out.
     */
    int max_houses = 0;
    HousesType base_house = HOUSE_NONE;
    if (Session.Type == GAME_NORMAL) {
        max_houses = 1;
        base_house = PlayerPtr->Class->House;
    } else {
        max_houses = Session.MaxPlayers;
        base_house = HOUSE_FIRST;
    }
    if (!Execute_DoList(max_houses, base_house, nullptr, nullptr, nullptr)) {
        GameActive = false;
        return;
    }

    /**
     *  Clean out the DoList.
     */
    Clean_DoList(nullptr);
}
