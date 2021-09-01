/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          EXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for implementing all the extended classes.
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
#include "ext_hooks.h"
#include "saveload_hooks.h"
#include "iomap.h"

/**
 *  Extended classes here.
 */
#include "initext_hooks.h"
#include "mainloopext_hooks.h"
#include "newmenuext_hooks.h"

#include "tacticalext_hooks.h"
#include "scenarioext_hooks.h"
#include "displayext_hooks.h"
#include "tooltipext_hooks.h"
#include "commandext_hooks.h"
#include "msglistext_hooks.h"

#include "objecttypeext_hooks.h"
#include "technotypeext_hooks.h"
#include "buildingtypeext_hooks.h"
#include "unittypeext_hooks.h"
#include "infantrytypeext_hooks.h"
#include "aircrafttypeext_hooks.h"
#include "warheadtypeext_hooks.h"
#include "weapontypeext_hooks.h"
#include "bullettypeext_hooks.h"
#include "supertypeext_hooks.h"
#include "voxelanimtypeext_hooks.h"
#include "animtypeext_hooks.h"
#include "particletypeext_hooks.h"
#include "particlesystypeext_hooks.h"
#include "isotiletypeext_hooks.h"
#include "overlaytypeext_hooks.h"
#include "smudgetypeext_hooks.h"
#include "terraintypeext_hooks.h"
#include "housetypeext_hooks.h"
#include "sideext_hooks.h"
#include "campaignext_hooks.h"
#include "tiberiumext_hooks.h"
//#include "taskforceext_hooks.h"
//#include "aitrigtypeext_hooks.h"
//#include "scripttypeext_hooks.h"
//#include "tagtypeext_hooks.h"
//#include "triggertypeext_hooks.h"

#include "technoext_hooks.h"
#include "footext_hooks.h"

#include "unitext_hooks.h"
#include "buildingext_hooks.h"
#include "aircraftext_hooks.h"
#include "infantryext_hooks.h"
#include "cellext_hooks.h"
#include "houseext_hooks.h"
#include "teamext_hooks.h"
#include "factoryext_hooks.h"
#include "animext_hooks.h"
#include "bulletext_hooks.h"
#include "terrainext_hooks.h"

#include "combatext_hooks.h"

#include "empulseext_hooks.h"

#include "dropshipext_hooks.h"

#include "cciniext_hooks.h"

#include "skirmishdlg_hooks.h"

#include "hooker.h"
#include "hooker_macros.h"





#if 0
    GET_REGISTER_STATIC(BlitTransXlatAlphaZRead *, this_ptr, ebp);
    GET_REGISTER_STATIC(unsigned, src_pixel, eax);
    GET_REGISTER_STATIC(short *, alpha_buff, ebp);
    GET_REGISTER_STATIC(short *, alpha_ptr, esi);

    static unsigned tint_color;
    static unsigned pixel;

    tint_color = DSurface::RGBA_To_Pixel(63, 0, 0);

    static short *buff;
    static short alpha_pixel;
    buff = this_ptr->Buffer;
    alpha_pixel = alpha_buff[*alpha_ptr];
    pixel = buff[src_pixel | alpha_pixel];

    if (tint_color) {
        pixel |= tint_color;
    }

    _asm { mov ecx, pixel }

    JMP_REG(ecx, 0x00469AD9);
#endif

#include "blitter.h"
#include "rgb.h"
class BlitTransXlatAlphaZRead : public Blitter
{
    public:
        short *Buffer;
        short *Alpha;
};
DECLARE_PATCH(_BlitTransXlatAlphaZRead_Copy_Tint_Patch)
{
    _asm { 
        mov ax, [eax+ebx*2]
        mov bp, 1111100000000000b   ; red
        test bp, bp
        jz set_pixel

        or bp, ax     ; OR pixel and tint

set_pixel:
        mov [edi], ax
    }
    JMP_REG(ebp, 0x0046912C);
}

DECLARE_PATCH(_BlitTransXlatAlphaZReadWrite_Copy_Tint_Patch)
{
    _asm { 
        mov cx, [eax+ecx*2]
        mov ax, 1111100000000000b   ; red
        test ax, ax
        jz set_pixel

        or cx, ax     ; OR pixel and tint

set_pixel:
        mov [edi], cx
    }
    JMP_REG(ecx, 0x00469AD9);
}

DECLARE_PATCH(_RLEBlitTransXlatAlphaZReadWrite_Copy_Tint_Patch)
{
    _asm { 
        mov dx, [edx+esi*2]
        mov esi, [esp+28h]
        mov cx, 1111100000000000b   ; red
        test cx, cx
        jz set_pixel

        or dx, cx     ; OR pixel and tint

set_pixel:
        mov [ebp-2], dx
    }
    JMP_REG(ecx, 0x0046C7E6);
}

DECLARE_PATCH(_RLEBlitTransXlatAlphaZRead_Copy_Tint_Patch)
{
    _asm { 
        mov dx, [ecx+edx*2]
        mov cx, 1111100000000000b   ; red
        test cx, cx
        jz set_pixel

        or dx, cx     ; OR pixel and tint

set_pixel:
        mov ecx, [esp+1Ch]
        mov [ebp-2], dx
    }
    JMP_REG(edx, 0x0046BA28);
}




void Extension_Hooks()
{
    Patch_Jump(0x00469125, &_BlitTransXlatAlphaZRead_Copy_Tint_Patch);
    Patch_Jump(0x00469AD2, &_BlitTransXlatAlphaZReadWrite_Copy_Tint_Patch);
    Patch_Jump(0x0046C7DA, &_RLEBlitTransXlatAlphaZReadWrite_Copy_Tint_Patch);
    Patch_Jump(0x0046BA1C, &_RLEBlitTransXlatAlphaZRead_Copy_Tint_Patch);

    /**
     *  Hook the new save and load system in.
     */
    SaveLoad_Hooks();

    /**
     *  Various functions.
     */
    GameInit_Hooks();
    MainLoop_Hooks();
    NewMenuExtension_Hooks();

    /**
     *  All class extensions here.
     */
    TacticalExtension_Hooks();
    ScenarioClassExtension_Hooks();
    DisplayClassExtension_Hooks();
    ToolTipManagerExtension_Hooks();
    CommandExtension_Hooks();
    MessageListClassExtension_Hooks();

    /**
     *  All type class extensions here.
     */
    ObjectTypeClassExtension_Hooks();
    TechnoTypeClassExtension_Hooks();
    BuildingTypeClassExtension_Hooks();
    UnitTypeClassExtension_Hooks();
    InfantryTypeClassExtension_Hooks();
    AircraftTypeClassExtension_Hooks();
    WarheadTypeClassExtension_Hooks();
    WeaponTypeClassExtension_Hooks();
    BulletTypeClassExtension_Hooks();
    SuperWeaponTypeClassExtension_Hooks();
    VoxelAnimTypeClassExtension_Hooks();
    AnimTypeClassExtension_Hooks();
    ParticleTypeClassExtension_Hooks();
    ParticleSystemTypeClassExtension_Hooks();
    IsometricTileTypeClassExtension_Hooks();
    OverlayTypeClassExtension_Hooks();
    SmudgeTypeClassExtension_Hooks();
    TerrainTypeClassExtension_Hooks();
    HouseTypeClassExtension_Hooks();
    SideClassExtension_Hooks();
    CampaignClassExtension_Hooks();
    TiberiumClassExtension_Hooks();
    //TaskForceClassExtension_Hooks();
    //AITriggerTypeClassExtension_Hooks();
    //ScriptTypeClassExtension_Hooks();
    //TagTypeClassExtension_Hooks();
    //TriggerTypeClassExtension_Hooks();

    TechnoClassExtension_Hooks();
    UnitClassExtension_Hooks();
    AircraftClassExtension_Hooks();
    InfantryClassExtension_Hooks();
    BuildingClassExtension_Hooks();
    CellClassExtension_Hooks();
    HouseClassExtension_Hooks();
    TeamClassExtension_Hooks();
    FactoryClassExtension_Hooks();
    TechnoClassExtension_Hooks();
    FootClassExtension_Hooks();
    AnimClassExtension_Hooks();
    BulletClassExtension_Hooks();
    TerrainClassExtension_Hooks();

    CombatExtension_Hooks();

    EMPulseClassExtension_Hooks();

    DropshipExtension_Hooks();

    CCINIClassExtension_Hooks();

    /**
     *  Dialogs and associated code.
     */
    SkirmishDialog_Hooks();
}
