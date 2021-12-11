/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VQAEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks related to VQA playback engine.
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
#include "vqaext_hooks.h"
#include "tibsun_functions.h"
#include "tibsun_globals.h"
#include "subtitlemanager.h"
#include "textprint.h"
#include "wwfont.h"
#include "movie.h"
#include "vqa.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"
#include <algorithm>

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Subtitles manager, reused for each movie playback.
 */
static SubTitleManager *SubTitlesMgr = nullptr;


static void Delete_SubTitleManager()
{
    delete SubTitlesMgr;
    SubTitlesMgr = nullptr;
}


/**
 *  Patch to delete the subtitles after the movie has finished.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_MovieClass_Destroy_Delete_SubTitles_Patch)
{
    /**
     *  Delete the existing subtitle manager instance.
     */
    Delete_SubTitleManager();

    /**
     *  Stolen bytes/code.
     */
    _asm { pop esi }
    _asm { ret }
}


/**
 *  Patch to create the subtitles for this movie.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_MovieClass_Create_Init_SubTitles_Patch)
{
    GET_REGISTER_STATIC(MovieClass *, movie, esi);

    /**
     *  Delete the existing subtitle manager instance to be safe.
     */
    Delete_SubTitleManager();
    
    /**
     *  Create a new instance of the subtitle manager for this movie.
     */
    SubTitlesMgr = SubTitleManager::Create_Subtitles(movie->VQMovie->Filename);

    /**
     *  Stolen bytes/code.
     */
    _asm { mov byte ptr [esi+45h], 1 }
    _asm { mov eax, esi }

    _asm { pop edi }
    _asm { pop esi }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { add esp, 0x0A8 }
    _asm { ret 0x28 }
}


/**
 *  Draws the subtitles for this frame.
 * 
 *  @author: CCHyper
 */
static void VQA_Draw_SubTitles(VQAClass *vqa, XSurface *surface)
{
    SubTitlesMgr->Update(VQA_GetTime(vqa->Handle));
}


/**
 *  Patch for intercepting the drawing of the VQA frame.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_VQAClass_Play_Draw_Intercept)
{
    GET_REGISTER_STATIC(VQAClass *, this_ptr, esi);

    /**
     *  Draw the subtitles for this VQA.
     */
    VQA_Draw_SubTitles(this_ptr, TempSurface);

    /**
     *  Stolen bytes/code
     */
    _asm {
        mov eax, [esi+0x53C]
        xor ebx, ebx
        cmp eax, ebx
        jz no_blit
        
        call eax

    no_blit:
    }

    JMP(0x0066BB0A);
}


/**
 *  Main function for patching the hooks.
 */
void VQAExtension_Hooks()
{
    Patch_Jump(0x0066BAFE, &_VQAClass_Play_Draw_Intercept);
    Patch_Jump(0x0056445A, &_MovieClass_Create_Init_SubTitles_Patch);
    Patch_Jump(0x005644A4, &_MovieClass_Destroy_Delete_SubTitles_Patch);
}
