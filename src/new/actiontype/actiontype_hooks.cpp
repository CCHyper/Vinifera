/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ACTIONTYPE_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for action type class.
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
#include "actiontype_hooks.h"
#include "actiontype.h"
#include "ccini.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"#


/**
 *  A fake class for implementing new member functions which allow
 *  access to the "this" pointer of the intended class.
 *
 *  @note: This must not contain a constructor or destructor.
 *
 *  @note: All functions must not be virtual and must also be prefixed
 *         with "_" to prevent accidental virtualization.
 */
static class CCINIClassExt : public CCINIClass
{
    public:
        ActionType _Get_ActionType(const char *section, const char *entry, const ActionType defvalue);
};



/**
 *  x
 * 
 *  @author: CCHyper
 */
ActionType CCINIClassExt::_Get_ActionType(const char *section, const char *entry, const ActionType defvalue)
{
    Wstring buffer = ActionTypeClass::Name_From(defvalue);

    if (INIClass::Get_String(section, entry, buffer) > 0) {
        return ActionTypeClass::From_Name(buffer);
    }

    return defvalue;
}


void ActionTypeClass_Hooks()
{
    Patch_Call(0x0060D3A6, &CCINIClassExt::_Get_ActionType);
}
