/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          PREREQUSITETYPE.H
 *
 *  @authors       CCHyper
 *
 *  @brief         Building prerequisite group type class.
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
#include "wstring.h"


class CCINIClass;


class PrerequisiteGroupTypeClass
{
    public:
        PrerequisiteGroupTypeClass(Wstring name);
        PrerequisiteGroupTypeClass(const NoInitClass &noinit);
        ~PrerequisiteGroupTypeClass();

        bool Read_INI(CCINIClass &ini);
        
        static const PrerequisiteGroupTypeClass &As_Reference(TheaterType type);
        static const PrerequisiteGroupTypeClass *As_Pointer(TheaterType type);
        static const PrerequisiteGroupTypeClass &As_Reference(const char *name);
        static const PrerequisiteGroupTypeClass *As_Pointer(const char *name);
        static TheaterType From_Name(const char *name);
        static const char *Name_From(TheaterType type);
    
    private:
        /**
         *  The name of this group type. This is the name as defined in the INI.
         */
        char Name[32];

        /**
         *  x
         */
        DynamicVectorClass<Wstring> Prerequisites;
};
