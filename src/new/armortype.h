/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          THEATERTYPE.H
 *
 *  @authors       CCHyper
 *
 *  @brief         Map armor type class.
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
#include "vector.h"
#include "tibsun_defines.h"


class CCINIClass;


class ArmorTypeClass
{
    public:
        ArmorTypeClass();
        ArmorTypeClass(const char *name);
        ~ArmorTypeClass();

        bool Read_INI(CCINIClass &ini);

        static bool One_Time();
        
        static const ArmorTypeClass &As_Reference(ArmorType type);
        static const ArmorTypeClass *As_Pointer(ArmorType type);
        static const ArmorTypeClass &As_Reference(const char *name);
        static const ArmorTypeClass *As_Pointer(const char *name);
        static ArmorType From_Name(const char *name);
        static const char *Name_From(ArmorType type);

        static const ArmorTypeClass *Find_Or_Make(const char *name);

        const char *Get_Name() const { return Name; }
    
    private:
        /**
         *  The name of this armor. This is the name as defined in the INI
         *  and is also used for identifying the armor on object.
         */
        char Name[16];
};
