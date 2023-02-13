/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          LUASCRIPT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         x
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
#include "luascript.h"
#include "ccfile.h"
#include "debughandler.h"
#include "asserthandler.h"

#if defined(LUA_ENABLED)


LuaScriptClass::LuaScriptClass() :
    State(nullptr)
{
    /**
     *  Create a new lua thread.
     */
    State = luaL_newstate();
    ASSERT(State != nullptr);

    /**
     *  Give this lua context access to the standard lua libraries.
     */
    luaL_openlibs(State);
}

LuaScriptClass::~LuaScriptClass()
{
    lua_close(State);
    State = nullptr;
}

bool LuaScriptClass::Initalize(Wstring script_name, bool (*register_func)(LuaScriptClass *lua_script))
{
    ASSERT(State != nullptr);
    ASSERT(register_func != nullptr);

    CCFileClass scriptfile(script_name.Peek_Buffer());

    if (!scriptfile.Is_Available()) {
        DEBUG_ERROR("Lua: Unable to find %s!\n", script_name.Peek_Buffer());
        return false;
    }

    DEBUG_INFO("Lua: About to load %s...\n", script_name.Peek_Buffer());

    /**
     *  workaround
     */
    Buffer lua_buffer(scriptfile.Size());
    scriptfile.Read(lua_buffer.Get_Buffer(), scriptfile.Size());

    luaL_loadbuffer(State, (const char *)lua_buffer.Get_Buffer(), scriptfile.Size(), nullptr);

    /**
     *  x
     */
    if (lua_pcall(State, 0, 0, 0) != 0) {
        DEBUG_ERROR("Lua: lua_pcall failed with %s!\n", lua_tostring(State, -1));
        return false;
    }

    DEBUG_INFO("Lua: About to register functions for %s...\n", script_name.Peek_Buffer());

    /**
     *  Register the functions available for this script.
     */
    if (!register_func || !register_func(this)) {
        return false;
    }

    DEBUG_INFO("Lua: Script %s added.\n", script_name.Peek_Buffer());

    return true;
}

void LuaScriptClass::Deinitialize()
{
}

/**
 *  Calls a given function within this script.
 * 
 *  @author: CCHyper
 */
bool LuaScriptClass::Call_Function(const char *func_name, int *return_value)
{
    ASSERT(State != nullptr);
    ASSERT(func_name != nullptr);

    /**
     *  Push function.
     */
    if (lua_getglobal(State, func_name) != 0) {
        DEBUG_ERROR("Lua: lua_getglobal failed with %s!\n", lua_tostring(State, 1));
        return false;
    }

    int number_of_args = 0;
    int number_of_returns = 0;

    if (return_value) {
        number_of_returns = 1;
    }

    /**
     *  x
     * 
     *  Format:
     *    state, number_of_args, number_of_returns, errfunc_idx.
     */
    if (lua_pcall(State, number_of_args, number_of_returns, 0) != 0) {
        DEBUG_ERROR("Lua: lua_pcall failed with %s!\n", lua_tostring(State, 1));
        DEV_DEBUG_INFO("Lua: Function `%s' must return a value!", func_name);
        return false;
    }

    /**
     *  Retrieve result value and pop it from the stack.
     */
    if (return_value) {
        *return_value = lua_tonumber(State, -1);
        lua_pop(State, 1);
    }

    return true;
}

/**
 *  x
 *
 *  @author: CCHyper
 */
bool LuaScriptClass::Register_Function(int (*lua_func) (lua_State *state), const char *func_name, const char *prefix_group)
{
    ASSERT(State != nullptr);

    Wstring funcname = func_name;

    if (prefix_group) {
        funcname += "_";
        funcname += prefix_group;
    }

    lua_register(State, funcname.Peek_Buffer(), lua_func);

    DEV_DEBUG_INFO("Lua::Register_Function - %s : %s.\n", FileName.Peek_Buffer(), funcname.Peek_Buffer());

    return true;
}

#endif
