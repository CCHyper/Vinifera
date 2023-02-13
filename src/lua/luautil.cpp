/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          LUAUTIL.CPP
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
#include "luautil.h"


void Lua_PrintError(lua_State *state, int run_status)
{
    switch (run_status)
    {
        case LUA_ERRRUN:
        {
            printf("LUA: Runtime Error: %s\n", lua_tostring(state, -1));
            break;
        }
        case LUA_ERRSYNTAX:
        {
            printf("LUA: Syntax Error: %s\n", lua_tostring(state, -1));
            break;
        }
        case LUA_ERRMEM:
        {
            printf("LUA: Memory Alloc Error: %s\n", lua_tostring(state, -1));
            break;
        }
        case LUA_ERRERR:
        {
            printf("LUA: Error returning Error: %s\n", lua_tostring(state, -1));
            break;
        }
        default:
        {
            printf("LUA: Unknown Error: %s\n", lua_tostring(state, -1));
            break;
        }
    }
}