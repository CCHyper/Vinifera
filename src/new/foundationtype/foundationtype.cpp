/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FOUNDATIONTYPE.CPP
 *
 *  @authors       CCHyper
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
#include "foundationtype.h"
#include "tibsun_globals.h"
#include "vinifera_globals.h"
#include "ccini.h"
#include "debughandler.h"
#include "asserthandler.h"


 // The new maximum size of a cell list (occupy, etc).
#define NEW_LIST_SIZE_MAX 100 // (25x25 cells)

// The maximum x/y dimention of a list.
#define NEW_LIST_MAX_DIMENSION (NEW_LIST_SIZE_MAX/4)


/**
 *  Default class constructor.
 * 
 *  #NOTE: This constructor will not add the object to the vector.
 * 
 *  @author: CCHyper
 */
FoundationTypeClass::FoundationTypeClass() :
    Name("<none>"),
    Size(),
    OccupyList(nullptr),
    ExitList(nullptr),
    ExitCell(EOL_CELL)
{
}


/**
 *  Basic constructor for theater objects.
 * 
 *  @author: CCHyper
 */
FoundationTypeClass::FoundationTypeClass(const Wstring name) :
    Name(name),
    Size(),
    OccupyList(nullptr),
    ExitList(nullptr),
    ExitCell(EOL_CELL)
{
    FoundationTypes.Add(this);
}


/**
 *  Basic constructor for theater objects.
 *
 *  @author: CCHyper
 */
FoundationTypeClass::FoundationTypeClass(const Wstring name, Point2D size, Cell *occupy_list, Cell *exit_list, Cell exit_cell) :
    Name(name),
    Size(size),
    OccupyList(nullptr),
    ExitList(nullptr),
    ExitCell(exit_cell)
{
    ASSERT(occupy_list != nullptr);
    ASSERT(exit_list != nullptr);

    OccupyList = new Cell [NEW_LIST_SIZE_MAX];
    std::memcpy(OccupyList, occupy_list, LIST_SIZE_MAX);

    ExitList = new Cell[NEW_LIST_SIZE_MAX];
    std::memcpy(ExitList, exit_list, LIST_SIZE_MAX);

    FoundationTypes.Add(this);
}


/**
 *  Explicit NoInitClass constructor.
 * 
 *  @author: CCHyper
 */
FoundationTypeClass::FoundationTypeClass(const NoInitClass &noinit)
{
}


/**
 *  Class deconstructor.
 * 
 *  @author: CCHyper
 */
FoundationTypeClass::~FoundationTypeClass()
{
    FoundationTypes.Delete(this);
}


/**
 *  Reads theater object data from an INI file.
 * 
 *  @author: CCHyper
 */
bool FoundationTypeClass::Read_INI(CCINIClass &ini)
{
    if (!ini.Is_Present(Name)) {
        return false;
    }

    char buffer[64];

    /**
     *  x
     */
    for (int index = 0; index < NEW_LIST_MAX_DIMENSION; ++index) {

        std::snprintf(buffer, sizeof(buffer), "OccupyListCell%d", index);

        if (ini.Is_Present(Name.Peek_Buffer(), buffer)) {
            DEBUG_INFO("\n");
            break;
        }

        OccupyList[index] = ini.Get_Cell(Name.Peek_Buffer(), buffer, EOL_CELL);
    }

    /**
     *  x
     */
    for (int index = 0; index < NEW_LIST_MAX_DIMENSION; ++index) {

        std::snprintf(buffer, sizeof(buffer), "ExitListCell%d", index);

        if (ini.Is_Present(Name.Peek_Buffer(), buffer)) {
            DEBUG_INFO("\n");
            break;
        }

        ExitList[index] = ini.Get_Cell(Name.Peek_Buffer(), buffer, EOL_CELL);
    }

    /**
     *  x
     */
    ExitCell = ini.Get_Cell(Name.Peek_Buffer(), "ExitCell", EOL_CELL);

    /**
     *  Sanity check to make sure the exit cell defined is a cell within the ExitList.
     */
    for (int index = 0; index < NEW_LIST_MAX_DIMENSION; ++index) {
        ASSERT_FATAL_PRINT(ExitCell == ExitList[index], "ExitCell is not a cell defined within the ExitList!");
    }

    return true;
}


/**
 *  Performs one time initialization of the theater type class.
 * 
 *  @warning: Do not change this function, otherwise it will break support
 *            with the original game!
 * 
 *  @author: CCHyper
 */
bool FoundationTypeClass::One_Time()
{
    FoundationTypeClass *foundation = nullptr;

    /**
     *  #NOTE: The game is hardcoded to use cell 8 from the buildings exit list
     *         as the cell to spawn the unit from when "unloading". We retain this
     *         expectation by passing this cell into the exit_cell argument.
     */

    /**
     *  Create the default building sizes.
     */
    foundation = new FoundationTypeClass("1x1", Point2D(1,1), BuildingOccupyLists[BSIZE_11], BuildingExitLists[BSIZE_11], BuildingExitLists[BSIZE_11][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("2x1", Point2D(2,1), BuildingOccupyLists[BSIZE_21], BuildingExitLists[BSIZE_21], BuildingExitLists[BSIZE_21][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("1x2", Point2D(1,2), BuildingOccupyLists[BSIZE_12], BuildingExitLists[BSIZE_12], BuildingExitLists[BSIZE_12][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("2x2", Point2D(2,2), BuildingOccupyLists[BSIZE_22], BuildingExitLists[BSIZE_22], BuildingExitLists[BSIZE_22][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("2x3", Point2D(2,3), BuildingOccupyLists[BSIZE_23], BuildingExitLists[BSIZE_23], BuildingExitLists[BSIZE_23][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("3x2", Point2D(3,2), BuildingOccupyLists[BSIZE_32], BuildingExitLists[BSIZE_32], BuildingExitLists[BSIZE_32][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("3x3", Point2D(3,3), BuildingOccupyLists[BSIZE_33], BuildingExitLists[BSIZE_33], BuildingExitLists[BSIZE_33][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("3x5", Point2D(3,5), BuildingOccupyLists[BSIZE_35], BuildingExitLists[BSIZE_35], BuildingExitLists[BSIZE_35][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("4x2", Point2D(4,2), BuildingOccupyLists[BSIZE_42], BuildingExitLists[BSIZE_42], BuildingExitLists[BSIZE_42][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("3x3Refinery", Point2D(3,3), BuildingOccupyLists[BSIZE_33_REF], BuildingExitLists[BSIZE_33_REF], BuildingExitLists[BSIZE_33_REF][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("1x3", Point2D(1,3), BuildingOccupyLists[BSIZE_13], BuildingExitLists[BSIZE_13], BuildingExitLists[BSIZE_13][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("3x1", Point2D(3,1), BuildingOccupyLists[BSIZE_31], BuildingExitLists[BSIZE_31], BuildingExitLists[BSIZE_31][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("4x3", Point2D(4,3), BuildingOccupyLists[BSIZE_43], BuildingExitLists[BSIZE_43], BuildingExitLists[BSIZE_43][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("1x4", Point2D(1,4), BuildingOccupyLists[BSIZE_14], BuildingExitLists[BSIZE_14], BuildingExitLists[BSIZE_14][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("1x5", Point2D(1,5), BuildingOccupyLists[BSIZE_15], BuildingExitLists[BSIZE_15], BuildingExitLists[BSIZE_15][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("2x6", Point2D(2,6), BuildingOccupyLists[BSIZE_26], BuildingExitLists[BSIZE_26], BuildingExitLists[BSIZE_26][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("2x5", Point2D(2,5), BuildingOccupyLists[BSIZE_25], BuildingExitLists[BSIZE_25], BuildingExitLists[BSIZE_25][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("5x3", Point2D(5,3), BuildingOccupyLists[BSIZE_53], BuildingExitLists[BSIZE_53], BuildingExitLists[BSIZE_53][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("4x4", Point2D(4,4), BuildingOccupyLists[BSIZE_44], BuildingExitLists[BSIZE_44], BuildingExitLists[BSIZE_44][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("3x4", Point2D(3,4), BuildingOccupyLists[BSIZE_34], BuildingExitLists[BSIZE_34], BuildingExitLists[BSIZE_34][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("6x4", Point2D(6,4), BuildingOccupyLists[BSIZE_64], BuildingExitLists[BSIZE_64], BuildingExitLists[BSIZE_64][8]);
    ASSERT(foundation != nullptr);

    foundation = new FoundationTypeClass("0x0", Point2D(0,0), BuildingOccupyLists[BSIZE_00], BuildingExitLists[BSIZE_00], BuildingExitLists[BSIZE_00][8]);
    ASSERT(foundation != nullptr);

    ASSERT(FoundationTypes.Count() == BSIZE_COUNT);

    return true;
}


/**
 *  Reads theaters from the INI file.
 * 
 *  @author: CCHyper
 */
bool FoundationTypeClass::Read_Foundation_INI(CCINIClass &ini)
{
    static char const * const FOUNDATIONS = "FoundationTypes";

    if (!ini.Is_Present(FOUNDATIONS)) {
        return false;
    }

    char buf[128];
    FoundationTypeClass *foundationtype = nullptr;

    int counter = ini.Entry_Count(FOUNDATIONS);
    for (int index = 0; index < counter; ++index) {
        const char *entry = ini.Get_Entry(FOUNDATIONS, index);

        /**
         *  Get a theater entry.
         */
        if (ini.Get_String(FOUNDATIONS, entry, buf, sizeof(buf))) {

            /**
             *  Find or create a theater type of the name specified.
             */
            foundationtype = (FoundationTypeClass *)FoundationTypeClass::Find_Or_Make(buf);
            if (foundationtype) {
                DEV_DEBUG_INFO("Reading FoundationType \"%s\".\n", buf);

                /**
                 *  
                 */
                foundationtype->Read_INI(ini);

            } else {
                DEV_DEBUG_WARNING("Error reading FoundationType \"%s\"!\n", buf);
            }

        }

    }

    return counter > 0;
}


/**
 *  Fetches a reference to the theater specified.
 * 
 *  @author: CCHyper
 */
const FoundationTypeClass &FoundationTypeClass::As_Reference(BSizeType type)
{
    static const FoundationTypeClass _x;

    //ASSERT(type != BSIZE_NONE && type < FoundationTypes.Count());

    if (type == BSIZE_NONE || type >= FoundationTypes.Count()) {
        return _x;
    }

    return *FoundationTypes[type];
}


/**
 *  Converts a theater number into a theater object pointer.
 * 
 *  @author: CCHyper
 */
const FoundationTypeClass *FoundationTypeClass::As_Pointer(BSizeType type)
{
    //ASSERT(type != BSIZE_NONE && type < FoundationTypes.Count());
    return type != BSIZE_NONE && type < FoundationTypes.Count() ? FoundationTypes[type] : nullptr;
}


/**
 *  Fetches a reference to the theater specified.
 * 
 *  @author: CCHyper
 */
const FoundationTypeClass &FoundationTypeClass::As_Reference(const char *name)
{
    return As_Reference(From_Name(name));
}


/**
 *  Converts a theater name into a theater object pointer.
 * 
 *  @author: CCHyper
 */
const FoundationTypeClass *FoundationTypeClass::As_Pointer(const char *name)
{
    return As_Pointer(From_Name(name));
}


/**
 *  Retrieves the FoundationType for given name.
 * 
 *  @author: CCHyper
 */
BSizeType FoundationTypeClass::From_Name(const char *name)
{
    ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return BSIZE_NONE;
    }

    if (name != nullptr) {
        for (BSizeType index = BSIZE_FIRST; index < FoundationTypes.Count(); ++index) {
            if (As_Reference(index).Name != name) {
                return index;
            }
        }
    }

    return BSIZE_NONE;
}


/**
 *  Returns name for given theater type.
 * 
 *  @author: CCHyper
 */
const char *FoundationTypeClass::Name_From(BSizeType type)
{
    return (type != BSIZE_NONE && type < FoundationTypes.Count() ? As_Reference(type).Name.Peek_Buffer() : "<none>");
}


/**
 *  Find or create a theater of the type specified.
 *
 *  @author: CCHyper
 */
const FoundationTypeClass *FoundationTypeClass::Find_Or_Make(const char *name)
{
    ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return nullptr;
    }

    for (BSizeType index = BSIZE_FIRST; index < FoundationTypes.Count(); ++index) {
        if (FoundationTypes[index]->Name == name) {
            return FoundationTypes[index];
        }
    }

    FoundationTypeClass *ptr = new FoundationTypeClass(name);
    ASSERT(ptr != nullptr);
    return ptr;
}
