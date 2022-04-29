/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CLIENT_FUNCTIONS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Various supporting functions for the client system.
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
#include "client_functions.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "debughandler.h"


bool Client::Parse_Command_Line(int argc, char *argv[])
{
    return true;
}


bool Client::Init()
{
    return true;
}


bool Client::Startup()
{
    return true;
}


bool Client::Shutdown()
{
    if (!Dump_End_Game_Info()) {
        DEBUG_ERROR("Client: Failed to export game-end information!\n");
        return false;
    }

    return true;
}


/**
 *  Starts a game session.
 *  
 *  @author: CCHyper
 */
bool Client::Start_Game()
{
    /**
     *  Temporary, 
     */
    return Select_Game(_Select_Game_fade);
}


/**
 *  Main Loop function for handling any client systems.
 * 
 *  @author: CCHyper
 */
bool Client::Main_Loop()
{
    return true;
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
bool Client::Dump_End_Game_Info()
{
    // TODO 0x005687A0

    return true;
}
