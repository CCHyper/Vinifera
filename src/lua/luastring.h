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
#include "luascript.h"
#include "luascriptvariable.h"
#include "wstring.h"



/**
 *  x
 */
class LuaString : public LuaUserVar
{
    public:
        LuaString(const char *str);
        LuaString(Wstring str);

        LuaTable *Lua_Append(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Assign(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_At(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Capacity(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Compare(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Copy(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Str(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Data(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Empty(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Erase(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Find(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Find_First_Not_Of(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Find_First_Of(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Find_Last_Not_Of(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Find_Last_Of(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Insert(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Length(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Max_Size(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_RFind(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Replace(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Reserve(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Resize(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Size(LuaScriptClass *script, LuaTable *params);
        LuaTable *Lua_Substr(LuaScriptClass *script, LuaTable *params);

        void To_String(std::string &outstr);
        bool Is_Equal(const LuaVar *var) const;
        LuaVar *Map_Into_Other_Script(LuaScriptClass *);
        size_t Hash_Function(void);

        Wstring Get_WString() const { return Value; }

    private:
        Wstring Value;
};
