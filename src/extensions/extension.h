/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          EXTENSION.H
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
#pragma once

#include "always.h"
#include "vinifera_defines.h"
#include "extension_globals.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <objidl.h> // for IStream.


/**
 *  Do not call these directly! Use the template functions below.
 */
AbstractClassExtension *Make_Extension_Internal(const AbstractClass *abstract);
AbstractClassExtension *Fetch_Extension_Internal(const AbstractClass *abstract);
bool Destroy_Extension_Internal(const AbstractClass *abstract);


/**
 *  Query if Vinifera supports this class with an extension.
 */
bool Is_Extension_Support_Enabled(RTTIType rtti);
bool Is_Extension_Support_Enabled(const AbstractClass *abstract);


/**
 *  Save and load interface.
 */
bool Save_Extensions(IStream *pStm);
bool Load_Extensions(IStream *pStm);

/**
 *  Save and load interface.
 */
bool Request_Extension_Pointer_Remap();

bool Register_Extension_Class_Factories();

void Clear_Extension_Lists();

unsigned Get_Extension_Save_Version_Number();


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
T *Fetch_Extension(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    return (T *)Fetch_Extension_Internal(abstract);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
T *Make_Extension(const AbstractClass *abstract)
{
    return (T *)Make_Extension_Internal(abstract);
}


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
void Destroy_Extension(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    Destroy_Extension_Internal(abstract);
}
