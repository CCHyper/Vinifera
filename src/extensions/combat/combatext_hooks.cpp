/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          COMBATEXT_HOOKS.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Contains the hooks for the extended combat functions.
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
#include "tibsun_inline.h"
#include "buildingext_hooks.h"
#include "combat.h"
#include "cell.h"
#include "techno.h"
#include "overlaytype.h"
#include "rulesext.h"
#include "scenarioext.h"
#include "warheadtype.h"
#include "warheadtypeext.h"
#include "tibsun_inline.h"
#include "extension.h"
#include "fatal.h"
#include "asserthandler.h"
#include "debughandler.h"

#include "hooker.h"
#include "hooker_macros.h"


/**
 *  convert a float to integer with the desired scale.
 * 
 *  @author CCHyper
 */
static int Scale_Float_To_Int(float value, int scale)
{
    value = std::clamp(value, 0.0f, 1.0f);
    return (value * scale);
}


#if 0
/**
 *  #issue-410
 * 
 *  Implements IsWallAbsoluteDestroyer for WarheadTypes.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Explosion_Damage_IsWallAbsoluteDestroyer_Patch)
{
    GET_REGISTER_STATIC(OverlayTypeClass *, overlay, esi);
    GET_REGISTER_STATIC(const WarheadTypeClass *, warhead, ebx);
    GET_REGISTER_STATIC(CellClass *, cellptr, edi);
    GET_STACK_STATIC(int, strength, esp, 0x54);
    static const WarheadTypeClassExtension *warheadtypeext;

    /**
     *  Check to make sure that the warhead is of the kind that can destroy walls.
     */
    if (overlay->IsWall) {

        /**
         *  Is this warhead capable of instantly destroying the wall regardless
         *  of damage? If so, then pass -1 into Reduce_Wall to remove the wall
         *  section from the cell.
         */
        warheadtypeext = Extension::Fetch<WarheadTypeClassExtension>(warhead);
        if (warheadtypeext->IsWallAbsoluteDestroyer) {
            cellptr->Reduce_Wall(-1);

        /**
         *  Original check.
         */
        } else if (warhead->IsWallDestroyer || warhead->IsWoodDestroyer || overlay->Armor == ARMOR_WOOD) {
            cellptr->Reduce_Wall(strength);
        }

    }

    JMP_REG(ecx, 0x0045FAD0);
}
#endif


#if 0
/**
 *  #issue-897
 *
 *  Implements IsIceDestruction scenario option for preventing destruction of ice,
 *  and IceStrength for configuring the chance for ice getting destroyed.
 *
 *  @author: Rampastring
 */
DECLARE_PATCH(_Explosion_Damage_IsIceDestruction_Patch)
{
    GET_REGISTER_STATIC(const WarheadTypeClass *, warhead, edi);
    GET_STACK_STATIC(int, strength, esp, 0x54);

    if (!ScenExtension->IsIceDestruction) {
        goto no_ice_destruction;
    }

    /**
     *  Stolen bytes/code here.
     */
    if (warhead->IsWallDestroyer || warhead->IsConventional) {

        /**
         *  Allow destroying ice if the strength of ice is 0 or the random number check allows it.
         */
        if (RuleExtension->IceStrength <= 0 || Random_Pick(0, RuleExtension->IceStrength) < strength) {
            goto allow_ice_destruction;
        }
    }

    /**
     *  Don't allow destroying ice, continue execution after ice-destruction logic.
     */
no_ice_destruction:
    JMP_REG(ecx, 0x004602DF);

    /**
     *  Allow destroying any potential ice on the cell.
     */
allow_ice_destruction:
    JMP_REG(ecx, 0x0046025C);
}
#endif


/**
 *  #issue-412
 * 
 *  Implements CombatLightSize for WarheadTypes.
 * 
 *  @author: CCHyper
 */
DECLARE_PATCH(_Do_Flash_CombatLightSize_Patch)
{
    GET_REGISTER_STATIC(int, damage, ecx);
    GET_REGISTER_STATIC(const WarheadTypeClass *, warhead, edx);
    static const WarheadTypeClassExtension *warheadtypeext;
    static float light_size;
    static int flash_size;

    /**
     *  Fetch the extension instance.
     */
    warheadtypeext = Extension::Fetch<WarheadTypeClassExtension>(warhead);

    /**
     *  If no custom light size has been set, then just use the default code.
     *  This sets the size of the light based on the damage dealt by the Warhead.
     */
    if (warheadtypeext->CombatLightSize <= 0.0f) {

        /**
         *  Original code.
         */
        flash_size = (damage / 4);
        if (flash_size < 63) {
            if (flash_size <= 21) {
                flash_size = 21;
            }
        } else {
            flash_size = 63;
        }
    }


    /**
     *  Has a custom light size been set on the warhead?
     */
    if (warheadtypeext->CombatLightSize > 0.0f) {

        /**
         *  Clamp the light size and scale to expected size range.
         */
        light_size = warheadtypeext->CombatLightSize;
        if (light_size > 1.0f) {
            light_size = 1.0f;
        }
        flash_size = Scale_Float_To_Int(light_size, 63);
    }

    /**
     *  Set the desired flash size.
     */
    _asm { mov esi, flash_size }

    JMP(0x00460495);
}


/**
 *  Reimplementation of Explosion_Damage.
 *
 *  @author: CCHyper
 */
static void Vinifera_Explosion_Damage(const Coordinate &coord, int strength, TechnoClass *source, const WarheadTypeClass *warhead, bool a5 = false)
{
    //CELL cell; // Cell number under explosion.
    ObjectClass* object; // Working object pointer.
    ObjectClass* objects[32]; // Maximum number of objects that can be damaged.
    int distance; // Distance to unit.
    int range; // Damage effect radius.
    int count; // Number of vehicle IDs in list.

    if (!strength || Special.IsInert || warhead == nullptr) return;

    //WarheadTypeClass const* whead = WarheadTypeClass::As_Pointer(warhead);
    //  WarheadTypeClass const * whead = &Warheads[warhead];
    //  range = ICON_LEPTON_W*2;
    range = ICON_LEPTON_W + (ICON_LEPTON_W >> 1);
    cell = Coord_Cell(coord);
    if ((unsigned)cell >= MAP_CELL_TOTAL) return;

    CellClass *cellptr = &Map[cell];
    ObjectClass *impacto = cellptr->Cell_Occupier();

    /*
    **  Fill the list of unit IDs that will have damage
    **  assessed upon them. The units can be lifted from
    **  the cell data directly.
    */
    count = 0;
    for (FacingType i = FACING_NONE; i < FACING_COUNT; i++) {
        /*
        **  Fetch a pointer to the cell to examine. This is either
        **  an adjacent cell or the center cell. Damage never spills
        **  further than one cell away.
        */
        if (i != FACING_NONE) {
            cellptr = Map[cell].Adjacent_Cell(i);
            if (!cellptr) continue;
        }

        /*
        **  Add all objects in this cell to the list of objects to possibly apply
        ** damage to. The list stops building when the object pointer list becomes
        ** full.  Do not include overlapping objects; selection state can affect
        ** the overlappers, and this causes multiplayer games to go out of sync.
        */
        object = cellptr->Cell_Occupier();
        while (object) {
            if (!object->IsToDamage && object != source) {
                object->IsToDamage = true;
                objects[count++] = object;
                if (count >= ARRAY_SIZE(objects)) break;
            }
            object = object->Next;
        }
        if (count >= ARRAY_SIZE(objects)) break;
    }

    /*
    **  Sweep through the units to be damaged and damage them. When damaging
    **  buildings, consider a hit on any cell the building occupies as if it
    **  were a direct hit on the building's center.
    */
    for (int index = 0; index < count; index++) {
        object = objects[index];

        object->IsToDamage = false;
        if (object->IsActive) {
            if (object->What_Am_I() == RTTI_BUILDING && impacto == object) {
                distance = 0;
            }
            else {
                distance = Distance(coord, object->Center_Coord());
            }
            if (object->IsDown && !object->IsInLimbo && distance < range) {
                int damage = strength;
                object->Take_Damage(damage, distance, warhead, source);
            }
        }
    }

    /*
    **  If there is a wall present at this location, it may be destroyed. Check to
    **  make sure that the warhead is of the kind that can destroy walls.
    */
    cellptr = &Map[cell];
    if (cellptr->Overlay != OVERLAY_NONE) {
        OverlayTypeClass const *optr = &OverlayTypeClass::As_Reference(cellptr->Overlay);

        if (optr->IsTiberium && warhead->IsTiberiumDestroyer) {
            cellptr->Reduce_Tiberium(strength / 10);
        }
        if (optr->IsWall) {
            if (warhead->IsWallDestroyer || (warhead->IsWoodDestroyer && optr->Armor == ARMOR_WOOD)) {
                Map[cell].Reduce_Wall(strength);
            }
        }
    }

    /*
    **  If there is a bridge at this location, then it may be destroyed by the
    **  combat damage.
    */
    if (cellptr->TType == TEMPLATE_BRIDGE1 || cellptr->TType == TEMPLATE_BRIDGE2 ||
        cellptr->TType == TEMPLATE_BRIDGE1H || cellptr->TType == TEMPLATE_BRIDGE2H ||
        cellptr->TType == TEMPLATE_BRIDGE_1A || cellptr->TType == TEMPLATE_BRIDGE_1B ||
        cellptr->TType == TEMPLATE_BRIDGE_2A || cellptr->TType == TEMPLATE_BRIDGE_2B ||
        cellptr->TType == TEMPLATE_BRIDGE_3A || cellptr->TType == TEMPLATE_BRIDGE_3B) {

        if (((warhead == WARHEAD_AP || warhead == WARHEAD_HE) && Random_Pick(1, Rule.BridgeStrength) < strength)) {
            Map.Destroy_Bridge_At(cell);
        }
    }
}


/**
 *  Main function for patching the hooks.
 */
void CombatExtension_Hooks()
{
    //Patch_Jump(0x0045FAA0, &_Explosion_Damage_IsWallAbsoluteDestroyer_Patch);
    //Patch_Jump(0x00460244, &_Explosion_Damage_IsIceDestruction_Patch);

    Patch_Call(0x0045EEB0, &Vinifera_Explosion_Damage);

    Patch_Jump(0x00460477, &_Do_Flash_CombatLightSize_Patch);
}
