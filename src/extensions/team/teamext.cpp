/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          TEAMEXT.CPP
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
#include "teamext.h"
#include "team.h"
#include "teamtype.h"
#include "vinifera_defines.h"
#include "tibsun_globals.h"
#include "tibsun_inline.h"
#include "script.h"
#include "scripttype.h"
#include "house.h"
#include "housetype.h"
#include "object.h"
#include "foot.h"
#include "technotype.h"
#include "target.h"
#include "rules.h"
#include "rulesext.h"
#include "iomap.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Executes the new team scripts.
 * 
 *  @author: CCHyper
 */
void TeamClassExtension::Switch(TeamClass *this_ptr, ScriptMissionClass *scriptmission, bool a3)
{
    switch (scriptmission->Mission) {

        case SMISSION_GATHER_AT_ENEMY:
            TMission_Gather_At_Enemy(this_ptr, scriptmission, a3);
            break;

        /**
         *  Unexpected ScriptMissionType.
         */
        default:
            DEV_DEBUG_WARNING("Invalid team script mission type!\n");
            break;
    };
}


/**
 *  Helper info for writing new team scripts.
 * 
 *  TActionClass::Data                  = First Param (PARAM1)
 *  TActionClass::Bounds.X              = Second Para (PARAM2)
 *  TActionClass::Bounds.Y              = Third Para (PARAM3)
 *  TActionClass::Bounds.W              = Fourth Para (PARAM4)
 *  TActionClass::Bounds.H              = Fifth Para (PARAM5)
 * 
 *  (PARAM6) (OPTIONAL)
 *  if TActionFormatType == 4
 *    TActionClass::Data (overwrites PARAM1)
 *  else
 *    TActionClass::Location
 * 
 *  
 *  Example action line from a scenario file;
 * 
 *  [Actions]
 *  NAME = [Action Count], [TActionType], [TActionFormatType], [PARAM1], [PARAM2], [PARAM3], [PARAM4], [PARAM5], [PARAM6:OPTIONAL]
 *  
 *  To allow the use of TActionClass::Data (PARAM1), you must have the TActionFormatType set
 *  to "0", otherwise this param is ignored!
 * 
 * 
 *  For producing FinalSun [Action] entries;
 *  NOTE: For available ParamTypes, see the [ParamTypes] section in FSData.INI.
 *  NOTE: "DEF_PARAM1_VALUE" if negative (-ve), PARAM1 will be set to the absolute value of this number.
 * 
 *  [Actions]
 *  TActionType = [Name], [DEF_PARAM1_VALUE], [PARAM1_TYPE], [PARAM2_TYPE], [PARAM3_TYPE], [PARAM4_TYPE], [PARAM5_TYPE], [PARAM6_TYPE], [USE_WP], [USE_TAG], [Description], 1, 0, [TActionType]
 */


/**
 *  #issue-150
 * 
 *  Gather At Enemy team script.
 * 
 *  Expected scenario INI format;
 *  NAME = [Action Count], 106, 0, [PARAM1 = House ID], [PARAM2 = Credits], 0, 0, 0
 * 
 *  For FinalSun;
 *  106=Give credits to house... [Requires Vinifera],0,2,5,0,0,0,0,0,0,Gives credits to the specified house.,1,0,106
 * 
 *  @author: CCHyper
 */
void TeamClassExtension::TMission_Gather_At_Enemy(TeamClass *this_ptr, ScriptMissionClass *mission, bool a3)
{
    if (a3) {

        /**
         *  Pick a team leader.
         */
        const FootClass *leader = this_ptr->Fetch_A_Leader();
        if (!leader) {
            this_ptr->IsNextMission = true;
            return;
        }
        
        /**
         *  ?, then consider the mission
         *  complete since this script can never complete otherwise.
         */
        HousesType enemy = leader->House->Enemy;
        if (enemy == HOUSE_NONE || Houses[enemy] == nullptr) {
            this_ptr->IsNextMission = true;
            return;
        }

        HouseClass *enemy_hptr = Houses[enemy];

        /**
         *  If the enemy house center or the teams house center is invalid, then
         *  consider the mission complete since this script can never complete otherwise.
         */
        Coordinate enemy_base_center = enemy_hptr->Center;
        Coordinate team_base_center = leader->House->Center;
        if (enemy_base_center || !team_base_center) {
            this_ptr->IsNextMission = true;
            return;
        }

        /**
         *  Find the distance from the enemy base to we should gather at.
         */
        LEPTON safe_distance = Cell_To_Lepton(8);
        if (RulesExtension) {
            safe_distance = Cell_To_Lepton(RulesExtension->AISafeDistance);
        }

        /**
         *  Adjust the enemy base center
         */
        Coordinate movecoord = enemy_base_center;
        Move_Coord(movecoord, Direction256(team_base_center, enemy_base_center), safe_distance);

        Cell movecell = Coord_Cell(movecoord);

        /**
         *  
         */
        Cell gatherat = Map.Nearby_Location(movecell, leader->Techno_Type_Class()->Speed, -1, MZONE_NORMAL, false, 3, 3);
        CellClass *gatherat_cell = &Map[gatherat];

        /**
         *  Set the teams destination.
         */
        this_ptr->Assign_Mission_Target(As_Target(gatherat_cell));

        DEBUG_INFO("A \"%s\" Team has chosen (%d, %d) for its GatherAtEnemy cell.\n", this_ptr->Class->Name(), gatherat.X, gatherat.Y);
    }

    /**
     *  If the teams target has been set, perform movement to the destination.
     */
    this_ptr->Coordinate_Move();
}
