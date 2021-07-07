/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          CLIENT_FUNCTIONS.H
 *
 *  @author        CCHyper
 *
 *  @brief         Various globals for the client front-end system.
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
#include "client_globals.h"
#include "cncnet5_globals.h"


namespace Client
{

bool Parse_Command_Line(int argc, char *argv[]);

bool Read_Client_Startup_Settings();
bool Read_Client_Settings(CCINIClass &ini);
bool Read_CnCNet_Settings(CCINIClass &ini);
bool Read_Game_Settings(CCINIClass &ini);

bool Init();
bool Startup();
bool Shutdown();

bool Start_Game();
bool Main_Loop();

bool Create_Player_Nodes();
bool Assign_Houses();

bool Dump_End_Game_Info();

const char *Name_From_Team(ClientTeamType team);

/**
 *  Helpful functions that hide the casts.
 */
bool Add_Client_Node_Tag(ClientNodeNameType *node);
bool Remove_Client_Node_Tag(ClientNodeNameType *node);
ClientNodeNameType *Get_Client_Node_Tag(int index);

void Draw_Observer_Overlay();

}; // namespace Client
