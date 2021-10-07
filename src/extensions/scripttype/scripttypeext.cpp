/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SCRIPTTYPEEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended ScriptType class.
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
#include "scripttypeext.h"
#include "scripttype.h"
#include "vinifera_defines.h"


const char *ScriptMissionClassExtension::Mission_Name(int mission)
{
    if (mission < SMISSION_COUNT) {
        return ScriptMissionClass::Mission_Name(ScriptMissionType(mission));
    }

    switch (mission) {

        case SMISSION_GATHER_AT_ENEMY:
            return "Gather at Enemy";

        default:
            return "<invalid>";
    };
}


const char *ScriptMissionClassExtension::Mission_Description(int mission)
{
    if (mission < SMISSION_COUNT) {
        return ScriptMissionClass::Mission_Name(ScriptMissionType(mission));
    }

    switch (mission) {

        case SMISSION_GATHER_AT_ENEMY:
            return "Uses AISafeDistance to find a spot close to enemy's base to gather close.";

        default:
            return "<invalid>";
    };
}
