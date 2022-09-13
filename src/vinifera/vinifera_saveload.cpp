/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VINIFERA_SAVELOAD.CPP
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
#include "vinifera_saveload.h"
#include "wstring.h"
#include "debughandler.h"


void Vinifera_Swizzle_Reset(const char *file, const int line, const char *func)
{
#ifdef VINIFERA_USE_NEW_SWIZZLE_MANAGER
    DEBUG_INFO("SwizzleManager::Reset() - %s.\n", func);
    ((ViniferaSwizzleManagerClass &)SwizzleManager).ViniferaSwizzleManagerClass::Reset();
#else
    SwizzleManager.SwizzleManagerClass::Reset();
#endif
}


void Vinifera_Swizzle_Request_Pointer_Remap(void **pointer, const char *file, const int line, const char *func, const char *var)
{
#ifdef VINIFERA_USE_NEW_SWIZZLE_MANAGER
    Wstring funcname = func;
    funcname += "()";
    ((ViniferaSwizzleManagerClass &)SwizzleManager).ViniferaSwizzleManagerClass::Swizzle_Dbg(pointer, file, line, funcname.Peek_Buffer(), var);
#else
    SwizzleManager.SwizzleManagerClass::Swizzle(pointer);
#endif
}


void Vinifera_Swizzle_Fetch_Swizzle_ID(void *pointer, LONG *id, const char *file, const int line, const char *func, const char *var)
{
#ifdef VINIFERA_USE_NEW_SWIZZLE_MANAGER
    ((ViniferaSwizzleManagerClass &)SwizzleManager).ViniferaSwizzleManagerClass::Fetch_Swizzle_ID_Dbg(pointer, id, file, line, func, var);
#else
    SwizzleManager.SwizzleManagerClass::Fetch_Swizzle_ID(pointer, id);
#endif
}


void Vinifera_Swizzle_Here_I_Am(LONG id, void *pointer, const char *file, const int line, const char *func, const char *var)
{
#ifdef VINIFERA_USE_NEW_SWIZZLE_MANAGER
    ((ViniferaSwizzleManagerClass &)SwizzleManager).ViniferaSwizzleManagerClass::Here_I_Am_Dbg(id, pointer, file, line, func, var);
#else
    SwizzleManager.SwizzleManagerClass::Here_I_Am(id, pointer);
#endif
}
