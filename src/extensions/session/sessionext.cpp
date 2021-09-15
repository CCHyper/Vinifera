/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SESSIONEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended SessionClass class.
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
#include "sessionext.h"
#include "tibsun_globals.h"
#include "noinit.h"
#include "session.h"
#include "ccini.h"
#include "rawfile.h"
#include "language.h"
#include "asserthandler.h"
#include "debughandler.h"


SessionClassExtension *SessionExtension = nullptr;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
SessionClassExtension::SessionClassExtension(SessionClass *this_ptr) :
    Extension(this_ptr)
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("SessionClassExtension constructor - 0x%08X\n", (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("SessionClassExtension constructor - 0x%08X\n", (uintptr_t)(ThisPtr));

    /**
     *  Initialise the default Skirmish options.
     */
    std::strcpy(SkirmishOptions.Handle, Text_String(TXT_NONAME));
    SkirmishOptions.Side = HOUSE_FIRST;
    SkirmishOptions.Color = PCOLOR_FIRST;
    SkirmishOptions.Credits = 10000;
    SkirmishOptions.GameSpeed = 3;
    SkirmishOptions.UnitCount = 10;
    SkirmishOptions.AIPlayers = 1;
    SkirmishOptions.AIDifficulty = 1;
    SkirmishOptions.Bases = true;
    SkirmishOptions.BridgeDestruction = true;
    SkirmishOptions.Goodies = true;
    SkirmishOptions.ShortGame = false;
    SkirmishOptions.CrapEngineers = false;
    SkirmishOptions.FogOfWar = false;
    SkirmishOptions.RedeployMCV = false;
    SkirmishOptions.ScenarioIndex = 0;

    IsInitialized = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
SessionClassExtension::SessionClassExtension(const NoInitClass &noinit) :
    Extension(noinit)
{
    IsInitialized = false;
}


/**
 *  Class deconstructor.
 *  
 *  @author: CCHyper
 */
SessionClassExtension::~SessionClassExtension()
{
    //EXT_DEBUG_TRACE("SessionClassExtension deconstructor - 0x%08X\n", (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("SessionClassExtension deconstructor - 0x%08X\n", (uintptr_t)(ThisPtr));

    IsInitialized = false;
}


/**
 *  Return the raw size of class data for save/load purposes.
 *  
 *  @author: CCHyper
 */
int SessionClassExtension::Size_Of() const
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("SessionClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(ThisPtr));

    return sizeof(*this);
}


/**
 *  Fetches the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
void SessionClassExtension::Read_MultiPlayer_Settings()
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("SessionClassExtension::Read_MultiPlayer_Settings - 0x%08X\n", (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("SessionClassExtension::Read_MultiPlayer_Settings - 0x%08X\n", (uintptr_t)(ThisPtr));

    static char const * const SKIRMISH = "Skirmish";

    /**
     *  Copy the current settings to the Skirmish options. This ensures
     *  all values are initialised if they are not found in the INI.
     */
    std::strcpy(SkirmishOptions.Handle, ThisPtr->Handle);
    SkirmishOptions.Side = HOUSE_FIRST;
    SkirmishOptions.Color = PCOLOR_FIRST;
    SkirmishOptions.Credits = ThisPtr->Options.Credits;
    SkirmishOptions.GameSpeed = ThisPtr->Options.GameSpeed;
    SkirmishOptions.UnitCount = ThisPtr->Options.UnitCount;
    SkirmishOptions.AIPlayers = ThisPtr->Options.AIPlayers;
    SkirmishOptions.AIDifficulty = ThisPtr->Options.AIDifficulty;
    SkirmishOptions.Bases = ThisPtr->Options.Bases;
    SkirmishOptions.BridgeDestruction = ThisPtr->Options.BridgeDestruction;
    SkirmishOptions.Goodies = ThisPtr->Options.Goodies;
    SkirmishOptions.ShortGame = ThisPtr->Options.ShortGame;
    SkirmishOptions.CrapEngineers = ThisPtr->Options.CrapEngineers;
    SkirmishOptions.FogOfWar = ThisPtr->Options.FogOfWar;
    SkirmishOptions.RedeployMCV = ThisPtr->Options.RedeployMCV;
    SkirmishOptions.ScenarioIndex = ThisPtr->Options.ScenarioIndex;

    /**
     *  Read the skirmish settings.
     */
    ConfigINI.Get_String(SKIRMISH, "Handle", SkirmishOptions.Handle, sizeof(SkirmishOptions.Handle));
    ConfigINI.Get_HousesType(SKIRMISH, "Side", SkirmishOptions.Side);
    ConfigINI.Get_Int(SKIRMISH, "Color", SkirmishOptions.Color);
    ConfigINI.Get_Int(SKIRMISH, "Credits", SkirmishOptions.Credits);
    ConfigINI.Get_Int(SKIRMISH, "GameSpeed", SkirmishOptions.GameSpeed);
    ConfigINI.Get_Int(SKIRMISH, "UnitCount", SkirmishOptions.UnitCount);
    ConfigINI.Get_Int(SKIRMISH, "AIPlayers", SkirmishOptions.AIPlayers);
    ConfigINI.Get_Int(SKIRMISH, "AIDifficulty", SkirmishOptions.AIDifficulty);
    ConfigINI.Get_Bool(SKIRMISH, "Bases", SkirmishOptions.Bases);
    ConfigINI.Get_Bool(SKIRMISH, "BridgeDestruction", SkirmishOptions.BridgeDestruction);
    ConfigINI.Get_Bool(SKIRMISH, "Goodies", SkirmishOptions.Goodies);
    ConfigINI.Get_Bool(SKIRMISH, "ShortGame", SkirmishOptions.ShortGame);
    ConfigINI.Get_Bool(SKIRMISH, "MultiEngineer", SkirmishOptions.CrapEngineers);
    ConfigINI.Get_Bool(SKIRMISH, "FogOfWar", SkirmishOptions.FogOfWar);
    ConfigINI.Get_Bool(SKIRMISH, "RedeployMCV", SkirmishOptions.RedeployMCV);
    ConfigINI.Get_Int(SKIRMISH, "ScenarioIndex", SkirmishOptions.ScenarioIndex);

    SkirmishOptions.AIPlayers = std::clamp(SkirmishOptions.AIPlayers, 1, (MAX_PLAYERS-1));

    ThisPtr->Options.Credits = SkirmishOptions.Credits;
    ThisPtr->Options.GameSpeed = SkirmishOptions.GameSpeed;
    ThisPtr->Options.UnitCount = SkirmishOptions.UnitCount;
    ThisPtr->Options.AIPlayers = SkirmishOptions.AIPlayers;
    ThisPtr->Options.AIDifficulty = SkirmishOptions.AIDifficulty;
    ThisPtr->Options.Bases = SkirmishOptions.Bases;
    ThisPtr->Options.BridgeDestruction = SkirmishOptions.BridgeDestruction;
    ThisPtr->Options.Goodies = SkirmishOptions.Goodies;
    ThisPtr->Options.ShortGame = SkirmishOptions.ShortGame;
    ThisPtr->Options.CrapEngineers = SkirmishOptions.CrapEngineers;
    ThisPtr->Options.FogOfWar = SkirmishOptions.FogOfWar;
    ThisPtr->Options.RedeployMCV = SkirmishOptions.RedeployMCV;
    ThisPtr->Options.ScenarioIndex = SkirmishOptions.ScenarioIndex;
}


/**
 *  Saves the extension data from the INI database.  
 *  
 *  @author: CCHyper
 */
void SessionClassExtension::Write_MultiPlayer_Settings()
{
    ASSERT(ThisPtr != nullptr);
    //EXT_DEBUG_TRACE("SessionClassExtension::Write_MultiPlayer_Settings - 0x%08X\n", (uintptr_t)(ThisPtr));
    //EXT_DEBUG_WARNING("SessionClassExtension::Write_MultiPlayer_Settings - 0x%08X\n", (uintptr_t)(ThisPtr));

    static char const * const SKIRMISH = "Skirmish";

    RawFileClass file("SUN.INI");

    SkirmishOptions.AIPlayers = std::clamp(SkirmishOptions.AIPlayers, 1, (MAX_PLAYERS-1));

    /**
     *  Write the skirmish settings.
     */
    ConfigINI.Put_String(SKIRMISH, "Handle", SkirmishOptions.Handle);
    ConfigINI.Put_HousesType(SKIRMISH, "Side", SkirmishOptions.Side);
    ConfigINI.Put_Int(SKIRMISH, "Color", SkirmishOptions.Color);
    ConfigINI.Put_Int(SKIRMISH, "Credits", SkirmishOptions.Credits);
    ConfigINI.Put_Int(SKIRMISH, "GameSpeed", SkirmishOptions.GameSpeed);
    ConfigINI.Put_Int(SKIRMISH, "UnitCount", SkirmishOptions.UnitCount);
    ConfigINI.Put_Int(SKIRMISH, "AIPlayers", SkirmishOptions.AIPlayers);
    ConfigINI.Put_Int(SKIRMISH, "AIDifficulty", SkirmishOptions.AIDifficulty);
    ConfigINI.Put_Bool(SKIRMISH, "Bases", SkirmishOptions.Bases);
    ConfigINI.Put_Bool(SKIRMISH, "BridgeDestruction", SkirmishOptions.BridgeDestruction);
    ConfigINI.Put_Bool(SKIRMISH, "Goodies", SkirmishOptions.Goodies);
    ConfigINI.Put_Bool(SKIRMISH, "ShortGame", SkirmishOptions.ShortGame);
    ConfigINI.Put_Bool(SKIRMISH, "MultiEngineer", SkirmishOptions.CrapEngineers);
    ConfigINI.Put_Bool(SKIRMISH, "FogOfWar", SkirmishOptions.FogOfWar);
    ConfigINI.Put_Bool(SKIRMISH, "RedeployMCV", SkirmishOptions.RedeployMCV);
    ConfigINI.Put_Int(SKIRMISH, "ScenarioIndex", SkirmishOptions.ScenarioIndex);

    ConfigINI.Save(file, false);
}
