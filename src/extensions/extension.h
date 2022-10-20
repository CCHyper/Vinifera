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
#include <string>
#include <typeinfo>


class EventClass;


namespace Extension
{

/**
 *  Do not call these directly! Use the template functions below.
 */
namespace Private
{

AbstractClassExtension *Make_Internal(const AbstractClass *abstract);
bool Destroy_Internal(const AbstractClass *abstract);
AbstractClassExtension *Fetch_Internal(const AbstractClass *abstract);

/**
 *  Wrapper for "typeid(T).name()", removes the "class" prefix on the string.
 */
template<typename T>
std::string Get_TypeID_Name()
{
    std::string str = typeid(T).name();
    str.erase(0, 6);
    return str;
}

}; // namespace "Extension::Private".

namespace Singleton
{

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
EXT_CLASS *Make(const BASE_CLASS *base)
{
    ASSERT(base != nullptr);

    EXT_CLASS *ext_ptr = new EXT_CLASS(base);
    ASSERT(ext_ptr != nullptr);

    EXT_DEBUG_INFO("Created \"%s\" extension.\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());

    return ext_ptr;
}

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
void Destroy(const EXT_CLASS *ext)
{
    ASSERT(ext != nullptr);

    delete ext;

    EXT_DEBUG_INFO("Destroyed \"%s\" extension.\n", Extension::Private::Get_TypeID_Name<BASE_CLASS>().c_str());
}

}; // namespace "Extension::Singleton".

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
T *Fetch(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    return (T *)Extension::Private::Fetch_Internal(abstract);
}

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
T *Make(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    return (T *)Extension::Private::Make_Internal(abstract);
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

    Extension::Private::Destroy_Internal(abstract);
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
