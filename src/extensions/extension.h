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
#include "abstract.h"
#include "abstractext.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <unknwn.h> // for IStream
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

}; // namespace "Extension::Private".

namespace Utility
{

/**
 *  Erase First Occurrence of given substring from main string.
 */
inline void Erase_Sub_String(std::string &str, const std::string &erase)
{
    /**
     *  Search for the substring in string.
     */
    size_t pos = str.find(erase);
    if (pos != std::string::npos) {

        /**
         *  If found then erase it from string.
         */
        str.erase(pos, erase.length());
    }
}

/**
 *  Wrapper for "typeid(T).name()", removes the "class" or "struct" prefix on the string.
 */
template<typename T>
std::string Get_TypeID_Name()
{
    std::string str = typeid(T).name();
    Erase_Sub_String(str, "class ");
    Erase_Sub_String(str, "struct ");
    return str;
}

static std::string Get_TypeID_Name(const AbstractClass *abstract)
{
    std::string str = typeid(*abstract).name();
    str.erase(0, 6);
    return str;
}

static std::string Get_TypeID_Name(const AbstractClassExtension *abstract_ext)
{
    std::string str = typeid(*abstract_ext).name();
    str.erase(0, 6);
    return str;
}

}; // namespace "Extension::Utility"

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

    EXT_DEBUG_INFO("Created \"%s\" extension.\n", Extension::Utility::Get_TypeID_Name<BASE_CLASS>().c_str());

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

    EXT_DEBUG_INFO("Destroyed \"%s\" extension.\n", Extension::Utility::Get_TypeID_Name<BASE_CLASS>().c_str());
}

}; // namespace "Extension::Singleton".

namespace List
{

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
EXT_CLASS *Fetch(const BASE_CLASS *base, DynamicVectorClass<EXT_CLASS *> &list)
{
    ASSERT(base != nullptr);

    for (int index = 0; index < list.Count(); ++index) {
        EXT_CLASS * ext = list[index];
        if (list[index]->This() == base) {
            EXT_DEBUG_INFO("Found \"%s\" extension.\n", Extension::Utility::Get_TypeID_Name<BASE_CLASS>().c_str());
            return ext;
        }
    }

    return nullptr;
}

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
EXT_CLASS *Make(const BASE_CLASS *base, DynamicVectorClass<EXT_CLASS *> &list)
{
    ASSERT(base != nullptr);

    EXT_CLASS *ext_ptr = new EXT_CLASS(base);
    ASSERT(ext_ptr != nullptr);

    EXT_DEBUG_INFO("Created \"%s\" extension.\n", Extension::Utility::Get_TypeID_Name<BASE_CLASS>().c_str());

    list.Add(ext_ptr);

    return ext_ptr;
}

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class BASE_CLASS, class EXT_CLASS>
void Destroy(const BASE_CLASS *base, DynamicVectorClass<EXT_CLASS *> &list)
{
    ASSERT(base != nullptr);

    for (int index = 0; index < list.Count(); ++index) {
        EXT_CLASS * ext = list[index].This();
        if (ext->This() == base) {
            EXT_DEBUG_INFO("Found \"%s\" extension.\n", Extension::Utility::Get_TypeID_Name<BASE_CLASS>().c_str());
            delete ext;
            return;
        }
    }

    EXT_DEBUG_INFO("Destroyed \"%s\" extension.\n", Extension::Utility::Get_TypeID_Name<BASE_CLASS>().c_str());
}

}; // namespace "Extension::List".

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class EXT_CLASS>
EXT_CLASS *Fetch(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    return (EXT_CLASS *)Extension::Private::Fetch_Internal(abstract);
}

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class EXT_CLASS>
EXT_CLASS *Make(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    return (EXT_CLASS *)Extension::Private::Make_Internal(abstract);
}

/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class EXT_CLASS>
void Destroy(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    Extension::Private::Destroy_Internal(abstract);
}

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
void Free_Heaps();
void Print_CRCs(FILE *fp, EventClass *ev);

/**
 *  
 */
bool Register_Class_Factories();

}; // namespace "Extension".
