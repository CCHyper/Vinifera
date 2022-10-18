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
#include "debughandler.h"
#include "asserthandler.h"

#include <objidl.h> // for IStream.


class AbstractClass;
class AbstractClassExtension;


/**
 *  For printing out extension debug info.
 */
#ifndef NDEBUG
#define EXT_DEBUG_SAY(x, ...) DEV_DEBUG_SAY(x, ##__VA_ARGS__)
#define EXT_DEBUG_INFO(x, ...) DEV_DEBUG_INFO(x, ##__VA_ARGS__)
#define EXT_DEBUG_WARNING(x, ...) DEV_DEBUG_WARNING(x, ##__VA_ARGS__)
#define EXT_DEBUG_ERROR(x, ...) DEV_DEBUG_ERROR(x, ##__VA_ARGS__)
#define EXT_DEBUG_FATAL(x, ...) DEV_DEBUG_FATAL(x, ##__VA_ARGS__)
#define EXT_DEBUG_TRACE(x, ...) DEV_DEBUG_TRACE(x, ##__VA_ARGS__)
#else
#define EXT_DEBUG_SAY(x, ...) ((void)0)
#define EXT_DEBUG_INFO(x, ...) ((void)0)
#define EXT_DEBUG_WARNING(x, ...) ((void)0)
#define EXT_DEBUG_ERROR(x, ...) ((void)0)
#define EXT_DEBUG_FATAL(x, ...) ((void)0)
#define EXT_DEBUG_TRACE(x, ...) ((void)0)
#endif


/**
 *  Do not call these directly! Use the template functions below.
 */
AbstractClassExtension *Find_Or_Make_Extension_Internal(const AbstractClass *abstract, bool allow_make = true);
AbstractClassExtension *Fetch_Extension_Internal(const AbstractClass *abstract);
bool Destroy_Extension_Internal(const AbstractClass *abstract);


/**
 *  Save and load interface.
 */
bool Is_Extension_Support_Enabled(ExtensionRTTIType rtti);

bool Save_Extensions(IStream *pStm);
bool Load_Extensions(IStream *pStm);

bool Request_Extension_Pointer_Remap();

bool Register_Extension_Class_Factories();

void Clear_Extension_Lists();


/**
 *  x
 * 
 *  @author: CCHyper
 */
template<class T>
T *Find_Or_Make_Extension(const AbstractClass *abstract, bool allow_make = true)
{
    return (T *)Find_Or_Make_Extension_Internal(abstract, allow_make);
}


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
bool Destroy_Extension(const AbstractClass *abstract)
{
    ASSERT(abstract != nullptr);

    return Destroy_Extension_Internal(abstract);
}
