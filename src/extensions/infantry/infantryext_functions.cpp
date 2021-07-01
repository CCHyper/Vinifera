/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          INFANTRYEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended InfantryClass.
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
#include "infantryext_functions.h"
#include "infantry.h"
#include "infantrytype.h"
#include "infantrytypeext.h"
#include "jumpjetlocomotion.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Queries if the current infantry locomotor is moving.
 * 
 *  @author: CCHyper
 */
bool Infantry_Is_Moving(InfantryClass *this_ptr)
{
    if (!this_ptr) {
        return false;
    }

    return this_ptr->Locomotion->Is_Moving_Now();
}


/**
 *  Does this infantry have the jumpjet locomotor assigned?
 * 
 *  @author: CCHyper
 */
bool Infantry_Locomotor_Is_Jumpjet(InfantryClass *this_ptr)
{
    CLSID clsid;
    HRESULT hr;

    ILocomotionPtr *iloco;

    hr = this_ptr->Locomotion->QueryInterface(__uuidof(IPersist), (void **)&iloco);
    if (FAILED(hr)) {
        return hr;
    }

    iloco->GetClassID(clsid);

    return clsid == __uuidof(JumpjetLocomotionClass);
}
