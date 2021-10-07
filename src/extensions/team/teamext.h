/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TEAMEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended TeamClass class.
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

#include "extension.h"
#include "container.h"
#include "tibsun_defines.h"


class TeamClass;
class ScriptMissionClass;


class TeamClassExtension final
{
    public:
        static void Switch(TeamClass *this_ptr, ScriptMissionClass *scriptmission, bool a3);

    protected:
        static void TMission_Gather_At_Enemy(TeamClass *this_ptr, ScriptMissionClass *mission, bool a3);
};
