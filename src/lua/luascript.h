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
#pragma once

#if defined(LUA_ENABLED)

#include "always.h"
#include "wstring.h"
#include "vector.h"
#include "luautil.h"


/**
 *  x
 */
class LuaScriptClass
{
    public:
        LuaScriptClass();
        ~LuaScriptClass();

        bool Initalize(Wstring script_name, bool (*register_func)(LuaScriptClass *lua_script));
        void Deinitialize();

        bool Call_Function(const char *func_name, int *return_value = nullptr);
        // TODO: work out a way of handling variable arguments and the return type of the function call.

        bool Register_Function(int (*lua_func) (lua_State *state), const char *func_name, const char *prefix_group = nullptr);

        /**
         *  Fetch the Lua state context
         */
        lua_State *Get_State() const { return State; }

    private:
        /**
         *  x
         */
        lua_State *State;

        /**
         *  x
         */
        Wstring Name;

        /**
         *  x
         */
        Wstring FileName;
};

#endif
