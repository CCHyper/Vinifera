/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_SAVELOAD.H
 *
 *  @authors       CCHyper
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
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "swizzle.h"
#include "newswizzle.h"


void Vinifera_Swizzle_Reset(const char *file, const int line, const char *func);
void Vinifera_Swizzle_Request_Pointer_Remap(void **pointer, const char *file, const int line, const char *func, const char *var);

template<typename T>
void Vinifera_Swizzle_Request_Pointer_Remap_Vector(DynamicVectorClass<T> &vector, const char *file, const int line, const char *func, const char *var)
{
#ifdef VINIFERA_USE_NEW_SWIZZLE_MANAGER
    for (int i = 0; i < vector.Count(); ++i) {
        ((ViniferaSwizzleManagerClass &)SwizzleManager).Swizzle_Dbg((void **)&vector[i], file, line, func, var);
    }
#else
    for (int i = 0; i < vector.Count(); ++i) {
        SwizzleManager.Swizzle((void **)&vector[i]);
    }
#endif
}

void Vinifera_Swizzle_Fetch_Swizzle_ID(void *pointer, LONG *id, const char *file, const int line, const char *func, const char *var);
void Vinifera_Swizzle_Here_I_Am(LONG id, void *pointer, const char *file, const int line, const char *func, const char *var);


/**
 *  Wrappers for the new swizzle manager for providing debug information.
 */
#define VINIFERA_SWIZZLE_RESET(func)                                      Vinifera_Swizzle_Reset(__FILE__, __LINE__, __FUNCTION__);
#define VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP(pointer, variable)         Vinifera_Swizzle_Request_Pointer_Remap((void **)&pointer, __FILE__, __LINE__, __FUNCTION__, variable);
#define VINIFERA_SWIZZLE_REQUEST_POINTER_REMAP_VECTOR(vector, variable)   Vinifera_Swizzle_Request_Pointer_Remap_Vector(vector, __FILE__, __LINE__, __FUNCTION__, variable);
#define VINIFERA_SWIZZLE_FETCH_SWIZZLE_ID(pointer, id, variable)          Vinifera_Swizzle_Fetch_Swizzle_ID((void *)pointer, &id, __FILE__, __LINE__, __FUNCTION__, variable);
#define VINIFERA_SWIZZLE_REGISTER_POINTER(id, pointer, variable)          Vinifera_Swizzle_Here_I_Am(id, (void *)pointer, __FILE__, __LINE__, __FUNCTION__, variable);
