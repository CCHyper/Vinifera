/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          LUASCRIPT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Replacement pointer swizzling interface for debugging save load issues.
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
 *  @note          This file is inspired by the Lua engine observed in the GlyphX
 *                 source code released by Petroglyph Games. Source:
 *                 https://github.com/PetroglyphGames/GlyphX-Reference
 *
 ******************************************************************************/
#pragma once

#include "always.h"
#include "ref_ptr.h"
#include "smartptr.h"
#include "tibsun_util.h"


 /**
  *  Macro to aid in registering a Member function to Lua. Takes the type,
  *  Lua name, and MemberFunction pointer and creates a Wrapper object that
  *  maps the function call to the Member.
  */
#define LUA_REGISTER_MEMBER_FUNCTION(type, name, func) \
    Register_Member(name, new LuaMemberFunctionWrapper<type>(this, func))


/**
 *  Enumerations.
 */
enum LuaVarType
{
    LUA_VAR_TYPE_INVALID = 0,

    LUA_VAR_TYPE_MAP,
    LUA_VAR_TYPE_TABLE,
    LUA_VAR_TYPE_VOID,
    LUA_VAR_TYPE_NUMBER,
    LUA_VAR_TYPE_BOOL,
    LUA_VAR_TYPE_STRING,
    LUA_VAR_TYPE_THREAD,
    LUA_VAR_TYPE_FUNCTION,
    LUA_VAR_TYPE_USER_VAR,
    LUA_VAR_TYPE_POINTER,
};


/**
 *  Forward Declarations.
 */
class LuaVar;
class LuaUserVar;
class LuaScriptClass;
template <class T, LuaVarType _VarType> class LuaValue;


/**
 *  Lua Typedefs
 */
typedef void (*lua_function_t)();
typedef void (*lua_thread_t)(int);

typedef std::map<SmartPtr<LuaVar>, SmartPtr<LuaVar>, LuaHashCompare> LuaMapType;
typedef LuaValue<LuaMapType, LUA_VAR_TYPE_MAP>                       LuaMap;
typedef LuaValue<std::vector<SmartPtr<LuaVar> >, LUA_VAR_TYPE_TABLE> LuaTable;
typedef LuaValue<void *, LUA_VAR_TYPE_VOID>                          LuaVoid;
typedef LuaValue<float, LUA_VAR_TYPE_NUMBER>                         LuaNumber;
typedef LuaValue<bool, LUA_VAR_TYPE_BOOL>                            LuaBool;
typedef LuaValue<std::string, LUA_VAR_TYPE_STRING>                   LuaString;
typedef LuaValue<lua_thread_t, LUA_VAR_TYPE_THREAD>                  LuaThread;
typedef LuaValue<lua_function_t, LUA_VAR_TYPE_FUNCTION>              LuaFunction;
typedef LuaValue<void *, LUA_VAR_TYPE_POINTER>                       LuaPointer;


/**
 *  Safe Cast Macro for LuaVar objects. Wraps the cast in a SmartPtr so
 *  that a cast from the result of an object factory doesn't leak.
 */
typedef SmartPtr<T> LuaCasted<typename T>;
#define LUA_SAFE_CAST(type, var) (CC_Dynamic_Cast<type>(LuaCasted<LuaVar>(var)))


/**
 * Base class representation of a Lua Variable.
 */
class LuaVar : public RefCountClass
{
    public:
        typedef SmartPtr<LuaVar> Pointer;

        virtual ~LuaVar() {}

        virtual LuaVarType Get_Var_Type() = 0;
};


/**
 *  x
 */
class LuaUserVar
{

};
