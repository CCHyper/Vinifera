/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          RENDERER_HOOKS.CPP
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
#include "renderer_hooks.h"
#include "renderer.h"
#include "tibsun_globals.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"



#include "ingame_overlay.h"



/**
 *  Adds a flip call to the end of GScreenClass::Blit.
 */
DECLARE_PATCH(_GScreenClass_Blit_ImGui)
{
    GET_REGISTER_STATIC(XSurface *, surface, esi);

    if (Vinifera_DeveloperMode) {
        //Renderer_ImGui::Render_Loop(surface);
        //InGameOverlay::Process();
        //InGameOverlay::Render(surface);
        //InGameOverlay::Render_To_Surface(surface);
    }

    _asm { mov ebp, [0x007E492C] }
    JMP_REG(ecx, 0x004B9A24);

    //_asm { pop esi }
    //_asm { add esp, 0x5C }
    //_asm { ret 4 }
}




void Renderer_Hooks()
{
#if 0
    RawFileClass file("SUN.INI");
    CCINIClass ini;

    ini.Load(file, false);

    /**
     *  Read the renderer settings before game init to ensure we patch
     *  in the correct renderer support.
     */
    if (Renderer::Read_INI(ini)) {

        /**
         *  Main hooks for the D7Surface and DirectDraw7 system.
         */
        if (Renderer::UseDirectDraw7) {
            DEBUG_INFO("Using DirectDraw7 as the renderer.\n");
            DDraw7_Hooks();
            D7Surface_Hooks();
        }
    }

    /**
     *  Restores ddraw.dll replacements to their original name.
     */
    if (!Renderer::UseDirectDraw7 && RawFileClass("DDRAW.BAK").Is_Available()) {
        DEBUG_INFO("Renaming DDRAW.BAK to DDRAW.DLL.\n");
        std::rename("DDRAW.BAK", "DDRAW.DLL");
    }
#endif

    /**
     *  Patch in the renderer flipper.
     */
    Patch_Jump(0x004B9A1E, &_GScreenClass_Blit_ImGui);
}
