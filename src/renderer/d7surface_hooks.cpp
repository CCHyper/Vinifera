/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          D7SURFACE_HOOKS.CPP
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
#include "d7surface_hooks.h"
#include "ddraw7_util.h"
#include "ddraw7_support.h"
#include "tibsun_globals.h"
#include "d7surface.h"
#include "dsurface.h"
#include "rawfile.h"
#include "fatal.h"
#include "debughandler.h"
#include "asserthandler.h"

#include "hooker.h"
#include "hooker_macros.h"


#if 0
/**
 *  Wrappers for hooking virtual functions.
 */
static class D7SurfaceHookingClass
{
    public:
        D7Surface *D7Surface_Hook_Ctor1() { return new (this) D7Surface(); }
        D7Surface *D7Surface_Hook_Ctor2(LPDIRECTDRAWSURFACE7 surface) { return new (this) D7Surface(surface); }
        D7Surface *D7Surface_Hook_Ctor3(int width, int height, bool system_mem) { return new (this) D7Surface(width, height, system_mem); }
        void D7Surface_Hook_Dtor() { reinterpret_cast<D7Surface *>(this)->D7Surface::~D7Surface(); }
        bool D7Surface_Hook_Copy_From2(Rect &dest, Surface &src, Rect &a3, bool trans_blit, bool a5) { return reinterpret_cast<D7Surface *>(this)->D7Surface::Copy_From(dest, src, a3, trans_blit, a5); }
        bool D7Surface_Hook_Copy_From3(Surface &src, bool trans_blit, bool a3) { return reinterpret_cast<D7Surface *>(this)->D7Surface::Copy_From(src, trans_blit, a3); }
        bool D7Surface_Hook_Fill_Rect1(Rect &size, unsigned color) { return reinterpret_cast<D7Surface *>(this)->D7Surface::Fill_Rect(size, color); }
        bool D7Surface_Hook_Copy_From1(Rect &toarea, Rect &torect, Surface &fromsurface, Rect &fromarea, Rect &fromrect, bool trans_blit, bool a7) { return reinterpret_cast<D7Surface *>(this)->D7Surface::Copy_From(toarea, torect, fromsurface, fromarea, fromrect, trans_blit, a7); }
        bool D7Surface_Hook_Fill_Rect2(Rect &area, Rect &rect, unsigned color) { return reinterpret_cast<D7Surface *>(this)->D7Surface::Fill_Rect(area, rect, color); }
};
#endif


static class DSurfaceFake : public DSurface
{
    public:
        HDC _Get_DC()
        {
            if (!DirectDraw7PrimarySurface) {
                DEBUG_WARNING("DSurface::Get_DC() - DirectDraw7PrimarySurface is null!\n");
                return nullptr;
            }

            if (Is_Locked()) {
                return nullptr;
            }

            HDC hdc = nullptr;
            HRESULT ddrval = DirectDraw7PrimarySurface->GetDC(&hdc);
            if (FAILED(ddrval)) {
                DEBUG_INFO("DSurface::Get_DC() - Failed to create DC!\n");
                DDRAW_ERROR_MSGBOX("DSurface::Get_DC()\n\nLock failed with error code %08X\n", ddrval);
                return nullptr;
            }
            ++LockLevel;
            return hdc;
        }

        bool _Release_DC(HDC hdc)
        {
            HRESULT ddrval = DirectDraw7PrimarySurface->ReleaseDC(hdc);
            if (FAILED(ddrval)) {
                DEBUG_INFO("DSurface::Release_DC() - Failed to create DC!\n");
                DDRAW_ERROR_MSGBOX("DSurface::Release_DC()\n\nLock failed with error code %08X\n", ddrval);
                return false;
            }
            if (LockLevel > 0) {
                --LockLevel;
            }
            return true;
        }

        bool _Restore_Check()
        {
            if (!Debug_Windowed && !GameInFocus) {
                return false;
            }

            if (!DirectDraw7PrimarySurface) {
                return true;
            }

            DWORD ddrval = 0;
        
            if (DirectDraw7_Is_Surface_Lost()) {

                if (DirectDraw7PrimarySurface->Restore() == DD_FALSE || DirectDraw7PrimarySurface->IsLost() == DD_FALSE) {
                    return false;
                }

                int prev_locklevel = LockLevel;
                if (LockLevel > 0) {
                    LockLevel = 0;
                    Lock();
                    ++LockLevel;
                    Unlock();
                    LockLevel = prev_locklevel;
                }
            }

            return true;
        }
};





void D7Surface_Hooks()
{
    // TODO: OwnerDraw uses AlternateSurface, this might not be enough...
    Patch_Jump(0x0048B2E0, &DSurfaceFake::_Get_DC);
    Patch_Jump(0x0048B320, &DSurfaceFake::_Release_DC);

    Patch_Jump(0x0048B510, &DSurfaceFake::_Restore_Check);





#if 0
        /**
         *  Hooks for the new DirectDraw7 surface.
         */
    //    Patch_Jump(0x0048AD10, &D7SurfaceHookingClass::D7Surface_Hook_Ctor1);
    //    Patch_Jump(0x0048B250, &D7SurfaceHookingClass::D7Surface_Hook_Ctor2);
    //    Patch_Jump(0x0048ABB0, &D7SurfaceHookingClass::D7Surface_Hook_Ctor3);
    //    Patch_Jump(0x0048ACA0, &D7SurfaceHookingClass::D7Surface_Hook_Dtor);
//        Patch_Jump(0x0048AD60, &Create_Primary_Intercept);
    //    Patch_Jump(0x0048B2E0, &D7Surface::Get_DC);
    //    Patch_Jump(0x0048B320, &D7Surface::Release_DC);
    //    Hook_Virtual(0x0048B350, D7Surface::Get_Bytes_Per_Pixel);
    //    Hook_Virtual(0x0048B360, D7Surface::Get_Pitch);
    //    Hook_Virtual(0x0048B370, D7Surface::Lock);
    //    Hook_Virtual(0x0048B450, D7Surface::Can_Lock);
    //    Hook_Virtual(0x0048B4B0, D7Surface::Can_Blit);
    //    Hook_Virtual(0x0048B4D0, D7Surface::Unlock);
    //    Patch_Jump(0x0048B510, &D7Surface::Restore_Check);
    //    Hook_Virtual(0x0048B590, D7SurfaceHookingClass::D7Surface_Hook_Copy_From2);
    //    Hook_Virtual(0x0048B5E0, D7SurfaceHookingClass::D7Surface_Hook_Copy_From1);
    //    Hook_Virtual(0x0048BB00, D7SurfaceHookingClass::D7Surface_Hook_Fill_Rect1);
    //    Hook_Virtual(0x0048BB30, D7SurfaceHookingClass::D7Surface_Hook_Fill_Rect2);
    //    Hook_Virtual(0x0048BCE0, D7Surface::Fill_Rect_Trans);
        //Patch_Jump(0x0048BFB0, );
        //Patch_Jump(0x0048C0E0, &D7Surface::Get_Red_Left);
        //Patch_Jump(0x0048C0F0, &D7Surface::Get_Red_Right);
        //Patch_Jump(0x0048C100, &D7Surface::Get_Green_Left);
        //Patch_Jump(0x0048C110, &D7Surface::Get_Green_Right);
        //Patch_Jump(0x0048C120, &D7Surface::Get_Blue_Left);
        //Patch_Jump(0x0048C130, &D7Surface::Get_Blue_Right);
        //Patch_Jump(0x048C140, );
        //Hook_Virtual(0x0048C150, );
        //Hook_Virtual(0x0048CC00, );
        //Patch_Jump(0x0048E3B0, );
        //Hook_Virtual(0x0048E4B0, );
        //Hook_Virtual(0x0048EA90, );
        //Hook_Virtual(0x0048F4B0, );
        //Hook_Virtual(0x0048FB90, );
    //    Hook_Virtual(0x004901A0, D7Surface_Hook_Copy_From3);
    //    Hook_Virtual(0x004901C0, D7Surface::entry_84);
#endif

}
