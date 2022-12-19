/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ACTIONTYPE.H
 *
 *  @author        CCHyper
 *
 *  @brief         Mouse cursor controls and overrides.
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
#include "tibsun_defines.h"
#include "wstring.h"


class CCINIClass;
class NoInitClass;


class ActionTypeClass
{
    public:
        ActionTypeClass(Wstring name);
        ActionTypeClass(const NoInitClass &noinit);
        ~ActionTypeClass();

        static void One_Time();
        static bool Read_INI(CCINIClass &ini);

        static ActionType From_Name(Wstring &name);
        static Wstring Name_From(ActionType type);
        static const ActionTypeClass *Find_Or_Make(Wstring &name);

    private:
        static Wstring Internal_Name(Wstring name);

    public:
        /**
         *  The ini name of this action.
         */
        Wstring Name;

    public:
        static Wstring ActionName[ACTION_COUNT];
};
