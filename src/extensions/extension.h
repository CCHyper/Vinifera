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


class EventClass;


/**
 *  Do not call these directly! Use the template functions below.
 */
namespace ExtensionPrivate
{

    AbstractClassExtension *Make_Internal(const AbstractClass *abstract);
    bool Destroy_Internal(const AbstractClass *abstract);
    AbstractClassExtension *Fetch_Internal(const AbstractClass *abstract);

}; // namespace "ExtensionPrivate".


namespace Extension
{

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
T *Fetch(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    return (T *)ExtensionPrivate::Fetch_Internal(abstract);
}

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
T *Make(const AbstractClass *abstract)
{
    return (T *)ExtensionPrivate::Make_Internal(abstract);
}

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
void Destroy(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    ExtensionPrivate::Destroy_Internal(abstract);
}

/**
 *  Query if Vinifera supports this class with an extension.
 */
bool Is_Supported(RTTIType rtti);
bool Is_Supported(const AbstractClass *abstract);

/**
 *  Save and load interface.
 */
bool Save(IStream *pStm);
bool Load(IStream *pStm);
bool Request_Pointer_Remap();
unsigned Get_Save_Version_Number();

/**
 *  
 */
void Detach_This_From_All(TARGET target, bool all = true);
void Clear_Vectors();
void Print_CRCs(FILE *fp, EventClass *ev);

/**
 *  
 */
bool Register_Class_Factories();

}; // namespace "Extension".
