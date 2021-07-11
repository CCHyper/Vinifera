/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SESSIONEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended SessionClass.
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
#include "sessionext_hooks.h"
#include "sessionext_init.h"
#include "sessionext_functions.h"
#include "sessionext.h"
#include "session.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or deconstructor!
 *  @note: All functions must be prefixed with "_" to prevent accidental virtualization.
 */
class SessionClassFake final : public SessionClass
{
    public:
        void _Read_Scenario_Descriptions();
};


/**
 *  Implementation of Read_Scenario_Descriptions() for SessionClass.
 */
void SessionClassFake::_Read_Scenario_Descriptions()
{
    Session_Read_Scenario_Descriptions((SessionClass *)this);
}


/**
 *  Main function for patching the hooks.
 */
void SessionClassExtension_Hooks()
{
    /**
     *  Initialises the extended class.
     */
    SessionClassExtension_Init();

    Patch_Jump(0x005EE7D0, &SessionClassFake::_Read_Scenario_Descriptions);
}
