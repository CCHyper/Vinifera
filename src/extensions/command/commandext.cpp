/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          COMMANDEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended hotkey command class.
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
#include "commandext.h"
#include "tibsun_globals.h"
#include "dsurface.h"
#include "wwmouse.h"
#include "unit.h"
#include "unittype.h"
#include "infantry.h"
#include "infantrytype.h"
#include "building.h"
#include "buildingtype.h"
#include "aircraft.h"
#include "aircrafttype.h"
#include "session.h"
#include "wwcrc.h"
#include "filepcx.h"
#include "filepng.h"
#include "minidump.h"
#include "winutil.h"
#include "miscutil.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  Handy defines for handling any adjustments.
 */
#define CATEGORY_DEVELOPER "Developer"


/**
 *  #issue-167
 * 
 *  Writes a PNG screenshot of the current screen buffer.
 * 
 *  @author: CCHyper
 */
const char *PNGScreenCaptureCommandClass::Get_Name() const
{
    return "ScreenCapture";
}

const char *PNGScreenCaptureCommandClass::Get_UI_Name() const
{
    return "Screen Capture";
}

const char *PNGScreenCaptureCommandClass::Get_Category() const
{
    return "Interface";
}

const char *PNGScreenCaptureCommandClass::Get_Description() const
{
    return "Takes a snapshot of the game screen (Saved as 'SCRN_<date-time>.PNG.)";
}

bool PNGScreenCaptureCommandClass::Process()
{
    if (!IsWindow(MainWindow)) {
        return false;
    }

    RECT crect;
    if (!GetClientRect(MainWindow, &crect)) {
        return false;
    }

    POINT tl_point;
    tl_point.x = crect.left;
    tl_point.y = crect.top;
    if (!ClientToScreen(MainWindow, &tl_point)) {
        return false;
    }

    POINT br_point;
    br_point.x = crect.right;
    br_point.y = crect.bottom;
    if (!ClientToScreen(MainWindow, &br_point)) {
        return false;
    }

    int w = std::min((int)crect.right+1, HiddenSurface->Get_Width());
    int h = std::min((int)crect.bottom+1, HiddenSurface->Get_Height());

    Rect src(tl_point.x, tl_point.y, w, h);
    Rect dest(0, 0, HiddenSurface->Get_Width(), HiddenSurface->Get_Height());

    /**
     *  We don't want the mouse to appear in screenshots!
     */
    WWMouse->Hide_Mouse();

    /**
     *  Blit primary surface to the hidden.
     */
    bool blit = HiddenSurface->Copy_From(dest, *PrimarySurface, src);
    ASSERT(blit);

    /**
     *  Now show the mouse again.
     */
    WWMouse->Show_Mouse();

    char buffer[256];

#if 0
    /**
     *  Find a free filename slot.
     */
    for (unsigned i = 0; i <= 9999; ++i) {
        std::snprintf(buffer, sizeof(buffer), "SCRN%04d.PNG", i);
        if (!RawFileClass(buffer).Is_Available()) {
            break;
        }
    }
#endif

    /**
     *  Generate a unique filename with the current timestamp.
     */
    int day = 0;
    int month = 0;
    int year = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
    Get_Full_Time(day, month, year, hour, min, sec);
    std::snprintf(buffer, sizeof(buffer), "SCRN_%02u-%02u-%04u_%02u-%02u-%02u.PNG", day, month, year, hour, min, sec);

    /**
     *  We found a free filename, now write the buffer to a PNG file.
     */
    bool success = Write_PNG_File(&RawFileClass(buffer), *HiddenSurface, &GamePalette);

    if (success) {
        DEBUG_INFO("PNG screenshot \"%s\" written sucessfully.\n", buffer);
    } else {
        DEBUG_ERROR("Failed to write PNG screenshot \"%s\"!\n", buffer);
    }

    return success;
}


/**
 *  Produces a memory dump on request.
 * 
 *  @author: CCHyper
 */
const char *MemoryDumpCommandClass::Get_Name() const
{
    return "MemoryDump";
}

const char *MemoryDumpCommandClass::Get_UI_Name() const
{
    return "Memory Dump";
}

const char *MemoryDumpCommandClass::Get_Category() const
{
    return CATEGORY_DEVELOPER;
}

const char *MemoryDumpCommandClass::Get_Description() const
{
    return "Produces a mini-dump of the memory for analysis.";
}

bool MemoryDumpCommandClass::Process()
{
    if (!Session.Singleplayer_Game()) {
        return false;
    }

    DEBUG_INFO("About to produce memory dump...\n");

    GenerateFullCrashDump = false; // We don't need a full memory dump.
    NonFatalMinidump = true;
    MinidumpUseCurrentTime = true;

    Create_Mini_Dump(nullptr, Get_Module_File_Name());

    return true;
}


/**
 *  Dumps all the current game objects as CRCs to the log output.
 * 
 *  @author: CCHyper
 */
const char *DumpHeapCRCCommandClass::Get_Name() const
{
    return "DumpHeapCRC";
}

const char *DumpHeapCRCCommandClass::Get_UI_Name() const
{
    return "Dump Heap CRCs";
}

const char *DumpHeapCRCCommandClass::Get_Category() const
{
    return CATEGORY_DEVELOPER;
}

const char *DumpHeapCRCCommandClass::Get_Description() const
{
    return "Dumps all the current game objects as CRCs to the log output.";
}

/**
 *  Handy macro for defining the logging the heaps CRCs.
 * 
 *  @author: CCHyper
 */
#define LOG_CRC(class_name, heap_name) \
    { \
        DEBUG_INFO(#class_name ":\n"); \
        if (!heap_name.Count()) { \
            DEBUG_INFO("  EMPTY\n"); \
        } else { \
            WWCRCEngine crc; \
            for (unsigned i = 0; i < heap_name.Count(); ++i) { \
                class_name *ptr = heap_name[i]; \
                if (ptr != nullptr) { \
                    ptr->Compute_CRC(crc); \
                    DEBUG_INFO("  %04d\tCRC: 0x%08X\n", i, crc.CRC_Value()); \
                } else { \
                    DEBUG_INFO("  %04d\tFAILED!\n", i); \
                } \
            } \
        } \
        DEBUG_INFO("\n"); \
    }

bool DumpHeapCRCCommandClass::Process()
{
    if (!Session.Singleplayer_Game()) {
        return false;
    }

    DEBUG_INFO("\nAbout to dump heap CRC's...\n\n");

    LOG_CRC(UnitClass, Units);
    LOG_CRC(InfantryClass, Infantry);
    LOG_CRC(BuildingClass, Buildings);
    LOG_CRC(AircraftClass, Aircrafts);

    LOG_CRC(UnitTypeClass, UnitTypes);
    LOG_CRC(InfantryTypeClass, InfantryTypes);
    LOG_CRC(BuildingTypeClass, BuildingTypes);
    LOG_CRC(AircraftTypeClass, AircraftTypes);
    
    DEBUG_INFO("\nFinished!\n\n");

    return true;
}
