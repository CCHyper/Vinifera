/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          SCENARIOEXT.H
 *
 *  @author        CCHyper
 *
 *  @brief         Extended ScenarioClass class.
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
#include "extension.h"
#include "scenario.h"


class LuaScriptClass;


class ScenarioClassExtension final : public GlobalExtensionClass<ScenarioClass>
{
    public:
        IFACEMETHOD(Load)(IStream *pStm);
        IFACEMETHOD(Save)(IStream *pStm, BOOL fClearDirty);

    public:
        ScenarioClassExtension(const ScenarioClass *this_ptr);
        ScenarioClassExtension(const NoInitClass &noinit);
        virtual ~ScenarioClassExtension();

        virtual int Size_Of() const override;
        virtual void Detach(TARGET target, bool all = true) override;
        virtual void Compute_CRC(WWCRCEngine &crc) const override;

        virtual const char *Name() const override { return "Scenario"; }
        virtual const char *Full_Name() const override { return "Scenario"; }

        void Init_Clear();
        bool Read_INI(CCINIClass &ini);

        bool Read_Tutorial_INI(CCINIClass &ini, bool log = false);

        static void Assign_Houses();
        static void Create_Units(bool official);

        /**
         *  x
         */
        LuaScriptClass *Get_Lua_Script() const { return LuaScript; }

        bool Lua_Scenario_Start();
        bool Lua_Frame_Tick(long frame);

    private:
        static bool Lua_Register_Functions(LuaScriptClass *lua_script);

    public:

    private:
        /**
         *  The Lua script file associated with this scenario file.
         */
        LuaScriptClass *LuaScript;

        /**
         *  x
         */
        bool IsSeenScenarioStart;
};
