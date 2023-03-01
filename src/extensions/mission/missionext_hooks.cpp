/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          MISSIONEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended MissionClass.
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
#include "missionext_hooks.h"
#include "mission.h"
#include "missionext.h"
#include "extension.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 * 
 *  @note: This must not contain a constructor or destructor.
 * 
 *  @note: All functions must not be virtual and must also be prefixed
 *         with "_" to prevent accidental virtualization.
 */
class MissionClassExt final : public MissionClass
{
    public:
        void _AI();
};


/**
 *  Reimplementation of MissionClass::AI.
 *
 *  @author: CCHyper
 */
void MissionClassExt::_AI()
{
    MissionClassExtension *missionext = Extension::Fetch<MissionClassExtension>(this);
    missionext->MissionClassExtension::AI(); // Specificy the namespace to ensure we call the correct function in the hierarchy.
}


/**
 *  Main function for patching the hooks.
 */
void MissionClassExtension_Hooks()
{
    Patch_Call(0x0062E9D1, &MissionClassExt::_AI);
    Change_Virtual_Address(0x006D44B0, Get_Func_Address(&MissionClassExt::_AI)); // Of MissionClass.
    Change_Virtual_Address(0x006D5BE0, Get_Func_Address(&MissionClassExt::_AI)); // Of RadioClass.
}
