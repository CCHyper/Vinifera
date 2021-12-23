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
 *  @brief         Contains the hooks for the extended VQAClass.
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
#include "vqa.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"



/**
 *  Patch to update the volume of the VQA AHandle during playback.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_AHandle_Timer_Callback_Adjust_Volume_Patch)
{
    /**
     *  Stolen bytes/code.
     */
    _asm { sub eax, edi }
    _asm { mov [esi+0x20], eax }

    GET_REGISTER_STATIC(AHandle *, this_ptr, esi);
    GET_STACK_STATIC(VQAHandle *, vqhandle, esp, 0x1C);
    static int vqhandle_config_volume;

    /**
     *  We need to do this as we do not currently have a full mapping for VQAHandle.
     */
    _asm { mov ecx, [esp+0x0C] } // VQAHandle
    _asm { mov eax, [ecx+0x0A4] } // VQAHandle.Config.Volume
    _asm { mov [vqhandle_config_volume], eax }

    /**
     *  If the volume has been changed, update the buffer.
     */
    if (this_ptr->Volume != vqhandle_config_volume) {
        if (this_ptr->BufferPtr) {
            static int dsvol;
            dsvol = Convert_HMI_To_Direct_Sound_Volume(vqhandle_config_volume);
            this_ptr->BufferPtr->SetVolume(dsvol);
            this_ptr->Volume = vqhandle_config_volume & 255;
        }
    }

    _asm { mov eax, [esi+0x20] }
    _asm { pop ebp }
    _asm { pop ebx }
    _asm { pop edi }
    _asm { pop esi }
    _asm { add esp, 0x10 }
    _asm { ret }
}


/**
 *  Handle input during VQA playback.
 * 
 *  @return: 0 == Continue input loop.
 *           1 == Breakout of playback.
 * 
 *  @author: CCHyper
 */
#include "textprint.h"
#include "wwfont.h"
static void VQA_Debug_Overlay(VQAClass *vqa, XSurface *surface)
{
    char buffer[1024];

    ColorType fcolor = COLOR_WHITE;
    ColorType bcolor = COLOR_BLACK;
    TextPrintType style = TPF_METAL12; 
    int xpos = 5;
    int ypos = 5;
    int row_height = Font_Ptr(style)->Get_Char_Height();

    /**
     *  Print VQAClass
     */

    std::snprintf(buffer, sizeof(buffer), "%s", vqa->Filename);
    Simple_Text_Print(buffer, surface, &surface->Get_Rect(), &Point2D(xpos,ypos),
                      NormalDrawer, fcolor, bcolor, style);

    ypos += row_height;

    std::snprintf(buffer, sizeof(buffer), "%d / %d", vqa->CurrentFrame, vqa->TotalFrames);
    Simple_Text_Print(buffer, surface, &surface->Get_Rect(), &Point2D(xpos,ypos),
                      NormalDrawer, fcolor, bcolor, style);

    ypos += row_height;

    std::snprintf(buffer, sizeof(buffer), "%d x %d x 16", vqa->Width, vqa->Height, 16);
    Simple_Text_Print(buffer, surface, &surface->Get_Rect(), &Point2D(xpos,ypos),
                      NormalDrawer, fcolor, bcolor, style);

    ypos += row_height;


    /**
     *  Print VQAConfig
     */

    if (vqa->Config.ImageWidth != -1 || vqa->Config.ImageHeight != -1 || vqa->Config.X1 != 0 || vqa->Config.Y1 != 0) {
        std::snprintf(buffer, sizeof(buffer), "%d x %d x %d x %d", vqa->Config.ImageWidth, vqa->Config.ImageHeight, vqa->Config.X1, vqa->Config.Y1);
        Simple_Text_Print(buffer, surface, &surface->Get_Rect(), &Point2D(xpos,ypos),
                          NormalDrawer, fcolor, bcolor, style);

        ypos += row_height;
    }
    
#if 0
    std::snprintf(buffer, sizeof(buffer), "%d x %d x %d", vqa->Config.FrameRate, vqa->Config.DrawRate, vqa->Config.RefreshRate);
    Simple_Text_Print(buffer, surface, &surface->Get_Rect(), &Point2D(xpos,ypos),
                      NormalDrawer, fcolor, bcolor, style);

    ypos += row_height;
#else
    std::snprintf(buffer, sizeof(buffer), "%d", vqa->Config.RefreshRate);
    Simple_Text_Print(buffer, surface, &surface->Get_Rect(), &Point2D(xpos,ypos),
                      NormalDrawer, fcolor, bcolor, style);

    ypos += row_height;
#endif

#if 0
    std::snprintf(buffer, sizeof(buffer), "%d", vqa->Config.NumFrameBufs);
    Simple_Text_Print(buffer, surface, &surface->Get_Rect(), &Point2D(xpos,ypos),
                      NormalDrawer, fcolor, bcolor, style);

    ypos += row_height;
#endif

#if 0
    std::snprintf(buffer, sizeof(buffer), "%d", vqa->Config.NumCBBufs);
    Simple_Text_Print(buffer, surface, &surface->Get_Rect(), &Point2D(xpos,ypos),
                      NormalDrawer, fcolor, bcolor, style);

    ypos += row_height;
#endif

#if 0
    std::snprintf(buffer, sizeof(buffer), "%d", vqa->Config.AudioRate);
    Simple_Text_Print(buffer, surface, &surface->Get_Rect(), &Point2D(xpos,ypos),
                      NormalDrawer, fcolor, bcolor, style);

    ypos += row_height;
#endif

}


/**
 *  
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_VQAClass_Play_Draw_Debug_Overlay)
{
    GET_REGISTER_STATIC(VQAClass *, this_ptr, esi);

    if (Vinifera_DeveloperMode) {
        VQA_Debug_Overlay(this_ptr, TempSurface);
        //VQA_Debug_Overlay(AlternateSurface);
        //VQA_Debug_Overlay(HiddenSurface);
        //VQA_Debug_Overlay(PrimarySurface);
    }

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
 *  Handle input during VQA playback.
 * 
 *  @return: 0 == Continue input loop.
 *           1 == Breakout of playback.
 * 
 *  @author: CCHyper
 */
static int VQA_Input_Handler(VQAClass *vqa)
{
    if (WWKeyboard->Check() == KN_NONE) {
        return 0;
    }

    int retcode = 0;

    /**
     *  Fetch the pressed key, only when released.
     */
    switch (WWKeyboard->Get()) {

        case KN_ESC|KN_RLSE_BIT:
            DEV_DEBUG_INFO("VQA: Breakout.\n");
            retcode = 1;
            break;

        case KN_UP|KN_RLSE_BIT:
        {
            OptionsExtension->MovieVolume = std::clamp((OptionsExtension->MovieVolume + 0.10f), 0.0f, 1.0f);
            OptionsExtension->MovieVolume = WWMath::Round_Ceil_Multiple(OptionsExtension->MovieVolume, 0.10f);
            vqa->Set_Volume(OptionsExtension->MovieVolume * 255);
            
            /**
             *  We need to do this as we do not currently have a full mapping for VQAHandle.
             */
            void *vqhandle = vqa->Handle;

            // VQAHandle.Config.Volume = OptionsExtension->MovieVolume * 255
            int vol_int = OptionsExtension->MovieVolume * 255;
            _asm { mov eax, [vqhandle]  }
            _asm { mov ecx, [vol_int] }
            _asm { mov [eax+0x0A4], ecx }

            DEV_DEBUG_INFO("VQA: Adjusted volume to '%f'.\n", OptionsExtension->MovieVolume);

            retcode = 0;

            break;
        }

        case KN_DOWN|KN_RLSE_BIT:
        {
            OptionsExtension->MovieVolume = std::clamp((OptionsExtension->MovieVolume - 0.10f), 0.0f, 1.0f);
            OptionsExtension->MovieVolume = WWMath::Round_Ceil_Multiple(OptionsExtension->MovieVolume, 0.10f);
            vqa->Set_Volume(OptionsExtension->MovieVolume * 255);

            /**
             *  We need to do this as we do not currently have a full mapping for VQAHandle.
             */
            void *vqhandle = vqa->Handle;

            // VQAHandle.Config.Volume = OptionsExtension->MovieVolume * 255
            int vol_int = OptionsExtension->MovieVolume * 255;
            _asm { mov eax, [vqhandle]  }
            _asm { mov ecx, [vol_int] }
            _asm { mov [eax+0x0A4], ecx }

            DEV_DEBUG_INFO("VQA: Adjusted volume to '%f'.\n", OptionsExtension->MovieVolume);

            retcode = 0;

            break;
        }

    };

    return retcode;
}


/**
 *  Patch to intercept the main loop of the VQA playback.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_VQAClass_Play_Check_Input_Patch)
{
    GET_REGISTER_STATIC(VQAClass *, this_ptr, esi);

    static int retcode;
    retcode = VQA_Input_Handler(this_ptr);

    if (retcode == 1) {
        goto breakout;
    }

continue_loop:
    JMP(0x0066BA30);

breakout:
    JMP(0x0066BB8A);
}


/**
 *  #issue-87
 * 
 *  Patch to use CCFileClass instead of MixFileClass when loading VQA files. This
 *  allows VQA files to be loaded from the games root directory.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_VQA_Mix_File_Handler_Use_CCFileClass_Patch)
{
    GET_REGISTER_STATIC(VQAClass *, this_ptr, esi);
    GET_STACK_STATIC(char *, filename, esp, 0xC);

    static int error;

    /**
     *  Original code used MixFileClass::Offset to find the file, this limited
     *  the VQA file streamer to only be able to load files from mix files.
     */
#if 0
    static MFCC *mixfile;
    static long offset;
    if (!MFCC::Offset(this_ptr->Filename, nullptr, &mixfile, &offset)) {
        error = 1;
    } else {
        this_ptr->field_64 = this_ptr->File.Open(mixfile->Filename, FILE_ACCESS_READ);
        error = (this_ptr->File.Seek(offset, FILE_SEEK_CURRENT) == 0);
    }
#endif

    /**
     *  ...Now we use CCFileClass, which does use MixFileClass to search for
     *  the file, but also scans for the file locally first.
     */
    this_ptr->File.Set_Name(filename);

    // #REMOVED: This fails as CDFileClass does not implement Is_Available to search the paths.
    //if (this_ptr->File.Is_Available()) {
    //    error = 1;
    //    goto exit_label;
    //}

    this_ptr->field_64 = this_ptr->File.Open(FILE_ACCESS_READ);

    error = !this_ptr->field_64;

exit_label:
    _asm { xor eax, eax }
    _asm { cmp error, 0 }
    _asm { setnz al }
    _asm { pop esi }
    _asm { ret 0x0C }
}


/**
 *  Main function for patching the hooks.
 */
void VQAExtension_Hooks()
{
    Patch_Jump(0x0066C0FD, _VQA_Mix_File_Handler_Use_CCFileClass_Patch);
    Patch_Jump(0x0066BB61, &_VQAClass_Play_Check_Input_Patch);
    Patch_Jump(0x0066BAFE, &_VQAClass_Play_Draw_Debug_Overlay);
    Patch_Jump(0x004073F9, &_AHandle_Timer_Callback_Adjust_Volume_Patch);
}
