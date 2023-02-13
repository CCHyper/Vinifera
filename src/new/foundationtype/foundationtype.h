/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          FOUNDATIONTYPE.H
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
#pragma once 

#include "always.h"
#include "wstring.h"
#include "tibsun_defines.h"


class CCINIClass;


class FoundationTypeClass
{
    private:
        FoundationTypeClass();
        FoundationTypeClass(const Wstring name, Point2D size, Cell *occupy_list, Cell *exit_list, Cell exit_cell);

    public:
        FoundationTypeClass(const Wstring name);
        FoundationTypeClass(const NoInitClass &noinit);
        ~FoundationTypeClass();

        bool Read_INI(CCINIClass &ini);

        Point2D Get_Size() const { return Size; }
        Cell *Occupy_List() const { return OccupyList; }
        Cell *Exit_List() const { return ExitList; }
        Cell Exit_Cell() const { return ExitCell; }

        static bool One_Time();

        static bool Read_Foundation_INI(CCINIClass &ini);
        
        static const FoundationTypeClass &As_Reference(BSizeType type);
        static const FoundationTypeClass *As_Pointer(BSizeType type);
        static const FoundationTypeClass &As_Reference(const char *name);
        static const FoundationTypeClass *As_Pointer(const char *name);
        static BSizeType From_Name(const char *name);
        static const char *Name_From(BSizeType type);

    private:
        static const FoundationTypeClass *Find_Or_Make(const char *name);
    
    private:
        /**
         *  x
         */
        Wstring Name;

        /**
         *  x
         */
        Point2D Size;

        /**
         *  x
         */
        Cell *OccupyList;

        /**
         *  x
         */
        Cell *ExitList;

        /**
         *  x
         */
        Cell ExitCell;
};
