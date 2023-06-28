/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          GSCREENEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended GScreenClass.
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
#include "gscreenext_hooks.h"
#include "vinifera_globals.h"
#include "iomap.h"
#include "bsurface.h"
#include "dsurface.h"
#include "abuffer.h"
#include "zbuffer.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  Working surfaces.
 */
static BSurface *DepthSurface = nullptr;
static BSurface *AlphaSurface = nullptr;


/**
 *  
 * 
 *  @author: tomsons26, DarthJane.
 */
static void Linearize(unsigned short *src, unsigned short *dest, unsigned size)
{
    if (dest == nullptr) {
        dest = src;
    }

    unsigned short min = 0xFFFF;
    unsigned short max = 0;
    for (int idx = 0; idx < size; ++idx) {
        if (src[idx] > max) {
            max = src[idx];
        }
        if (src[idx] < min) {
            min = src[idx];
        }
    }

    for (int idx = 0; idx < size; ++idx) {
        float pixel = src[idx];
        dest[idx] = (unsigned short)((pixel - min) / (max - min) * 0xFFFF);
    }
}


/**
 *  
 * 
 *  @author: tomsons26
 */
static void Blit_Depth_Buffer(int x = 0, int y = 0)
{
    BSurface *buffsurf = DepthBuffer->Get_Surface();
    int width = buffsurf->Get_Width();
    int height = buffsurf->Get_Height();

    if (buffsurf->Lock()) {
            
        /**
         *  Copy over to a working buffer.
         */
        //DepthBuffer->Copy_To(DepthSurface, x, y, width, height);

        unsigned short *buffptr = (unsigned short *)DepthSurface->Lock();

        /**
         *  Linearize the 16 bit grey-scale image.
         */
        Linearize(buffptr, nullptr, width * height);

        /**
         *  Convert the buffer to a proper color format.
         */
        for (int i = 0; i < width * height; ++i) {
            int v = buffptr[i] >> 8;
            buffptr[i] = DSurface::RGB_To_Pixel(v, v, v);
        }

        DepthSurface->Unlock();
        buffsurf->Unlock();
    }
}


/**
 *  
 * 
 *  @author: tomsons26
 */
static void Blit_Alpha_Buffer(int x = 0, int y = 0)
{
    BSurface *buffsurf = AlphaBuffer->Get_Surface();
    int width = buffsurf->Get_Width();
    int height = buffsurf->Get_Height();

    if (buffsurf->Lock()) {

        /**
         *  Copy over to a working buffer.
         */
        //AlphaBuffer->Copy_To(AlphaSurface, x, y, width, height);

        unsigned short *buffptr = (unsigned short *)AlphaSurface->Lock();

        /**
         *  Convert the buffer to a proper color format.
         */
        for (int i = 0; i < width * height; ++i) {
            int v = buffptr[i];
            buffptr[i] = DSurface::RGB_To_Pixel(v, v, v);
        }

        AlphaSurface->Unlock();
        buffsurf->Unlock();
    }
}


/**
 *  Create the working surfaces for the alpha and depth drawing.
 * 
 *  @author: CCHyper
 */
static void Create_Alpha_Buffer_Surface()
{
    if (AlphaSurface) {
        delete AlphaSurface;
        AlphaSurface = nullptr;
    }

    AlphaSurface = new BSurface(AlphaBuffer->Get_Surface()->Width, AlphaBuffer->Get_Surface()->Height, 2);
}

static void Create_Depth_Buffer_Surface()
{
    if (DepthSurface) {
        delete DepthSurface;
        DepthSurface = nullptr;
    }

    DepthSurface = new BSurface(DepthBuffer->Get_Surface()->Width, DepthBuffer->Get_Surface()->Height, 2);
}


/**
 *  Intercepts the final blit process to PrimarySurface.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_GScreenClass_Blit_Final_Blit_Intercept_Patch)
{
    LEA_STACK_STATIC(Rect *, torect, esp, 0x3C);
    LEA_STACK_STATIC(Rect *, fromrect, esp, 0x4C);
    GET_REGISTER_STATIC(DSurface *, in_surface, esi);
    GET_REGISTER_STATIC(int, dialog_val, ebp);
    GET_REGISTER_STATIC(bool, draw_mouse, bl);
    static Surface *from_surface;
    static bool arg5;
    
    /**
     *  Allocate the depth and alpha surfaces. If the buffer size has changed
     *  the re-allocate the destination surfaces.
     */
    if (AlphaBuffer) {
        if (!AlphaSurface || AlphaBuffer->Get_Surface()->Width != AlphaSurface->Width) {
            Create_Alpha_Buffer_Surface();
        }
    }
    if (DepthBuffer) {
        if (!DepthSurface || DepthBuffer->Get_Surface()->Width != DepthSurface->Width) {
            Create_Depth_Buffer_Surface();
        }
    }
    
    /**
     *  Blit the alpha buffer.
     */
    if (AlphaSurface && Vinifera_Developer_RenderAlphaBuffer) {
        AlphaSurface->Fill(0x041F);  // TEMP
        Blit_Alpha_Buffer(0, 16);
        from_surface = AlphaSurface;
        torect->X = 0;
        torect->Y = 16;
        fromrect->Width = AlphaSurface->Width;
        fromrect->Height = AlphaSurface->Height;

    /**
     *  Blit the depth buffer.
     */
    } else if (DepthSurface && Vinifera_Developer_RenderDepthBuffer) {
        DepthSurface->Fill(0x041F);  // TEMP
        Blit_Depth_Buffer(0, 16);
        from_surface = DepthSurface;
        torect->X = 0;
        torect->Y = 16;
        fromrect->Width = DepthSurface->Width;
        fromrect->Height = DepthSurface->Height;

    /**
     *  Normal blit process.
     */
    } else {
        from_surface = in_surface;
    }

    /**
     *  Copy surface data to the primary surface.
     */
    if (dialog_val) { arg5 = true; } else { arg5 = false; }
    PrimarySurface->Copy_From(*torect, *from_surface, *fromrect, false, arg5);

    _asm { mov bl, draw_mouse }
    JMP_REG(edx, 0x004B9A42);
}


/**
 *  Main function for patching the hooks.
 */
void GScreenClassExtension_Hooks()
{
    Patch_Jump(0x004B9A24, &_GScreenClass_Blit_Final_Blit_Intercept_Patch);
}
