/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          INTEGRITYCHECKER.H
 *
 *  @author        CCHyper
 *
 *  @brief         
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
#include "integritychecker.h"
#include "vinifera_gitinfo.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "house.h"
#include "event.h"
#include "wwcrc.h"
#include "ccini.h"


// TODO, when client support is added, store the CRC in the player tracker.

bool IntegrityChecker::Check()
{
    return true;
}


/**
 *  
 */
bool IntegrityChecker::Update()
{
    WWCRCEngine crc;

    /**
     *  Reset integrity check values.
     */
    Vinifera_IntegrityCheckFailed = false;
    Vinifera_IntegrityCheckCRC = 0LL;

    Vinifera_IntegrityCheckCRC += RuleINI->Get_Unique_ID();
    Vinifera_IntegrityCheckCRC += FSRuleINI.Get_Unique_ID();
    Vinifera_IntegrityCheckCRC += ArtINI.Get_Unique_ID();
    Vinifera_IntegrityCheckCRC += AIINI.Get_Unique_ID();
    Vinifera_IntegrityCheckCRC += FSAIINI.Get_Unique_ID();

    const char *git_hash = Vinifera_Git_Hash();
    Vinifera_IntegrityCheckCRC += crc(git_hash, std::strlen(git_hash));

    // TODO: Add my CRC to the client player tracker.

    return true;
}


/**
 *  
 */
bool IntegrityChecker::Send_Event()
{
    return OutList.Add(EventClass(PlayerPtr->ID, NewEventType::INTEGRITYCHECK), Vinifera_IntegrityCheckCRC);
}
