/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VORTEX.H
 *
 *  @author        CCHyper
 *
 *  @brief         Definition of ChronalVortexClass. The Chronal vortex sometimes appears when the chronosphere is used.
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
#include "tibsun_defines.h"


class BSurface;
class ObjectClass;
class Pipe;
class Straw;


#define MAX_REMAP_SHADES 16  // Number of lookup tables required for vortex shading.
#define VORTEX_FRAMES 16     // Number of frames in one complete rotation of the vortex.
#define ZAP_COUNT 1


class ChronalVortexClass
{
    public:
        ChronalVortexClass();
        ~ChronalVortexClass();

        void Detach(TARGET target);
        void Appear(Coordinate coordinate);
        void Disappear();
        void AI();
        void Render();
        void Set_Redraw();
        void Setup_Remap_Tables(TheaterType theater);
        void Set_Target(ObjectClass *target);
        void Stop();

        void Load(Straw &file);
        void Save(Pipe &file);

        /**
         *  Returns true of vortex is currently active.
         */
        bool Is_Active() const { return Active; }

        Coordinate Get_Position() const { return Position; }
        int Get_Range() const { return Range; }
        int Get_Speed() const { return Speed; }
        int Get_Damage() const { return Damage; }

        void Set_Range(int range) { Range = range; }
        void Set_Speed(int speed) { Speed = speed; }
        void Set_Damage(int damage) { Damage = damage; }
        void Set_Firing_Building(BuildingTypeClass *building) { FiringBuilding = building; }

    private:
        void Build_Fading_Table(PaletteClass const &palette, void *dest, int color, int frac);
        void Coordinate_Remap(BSurface *inbuffer, int x, int y, int width, int height, unsigned char *remap_table);

        void Attack();
        void Zap_Target();
        void Movement();
        void Hide();
        void Show();

    private:
        /**
         *  Position of the top left of the vortex.
         */
        Coordinate Position;

        /**
         *  Direction of rotation.
         */
        int AnimateDir;

        /**
         *  Current frame of animation.
         */
        int AnimateFrame;

        /**
         *  Animation flag. When 0 vortex will animate 1 frame.
         */
        int Animate;

        /**
         *  Possible states the vortex can be in.
         */
        typedef enum AnimStateType {
            STATE_INITIAL,  // Newly created Vortex.
            STATE_GROW,     // Vortex has just appeared and is growing larger
            STATE_ROTATE,   // Vortex is rotating
            STATE_SHRINK    // Vortex is shrinking and about to disappear
        } AnimStateType;

        /**
         *  State of vortex.
         */
        AnimStateType State;

        /**
         *  Color lookup tables for shading on vortex.
         */
        unsigned char VortexRemapTables[MAX_REMAP_SHADES][256];

        /**
         *  Color lookup table to make the blue lightning orange.
         */
        unsigned char LightningRemap[256];

        /**
         *  Is vortex currently active?
         */
        bool Active;

        /**
         *  Is the vortex winding down?
         */
        bool StartShutdown;

        /**
         *  Is the vortex about to hide from view?
         */
        bool StartHiding;

        /**
         *  Is the vortex active but hidden?
         */
        bool Hidden;

        /**
         *  Theater that lookup table is good for.
         */
        TheaterType Theater;

        /**
         *  Last frame that vortex attacked on.
         */
        int LastAttackFrame;

        /**
         *  How many times lightning has zapped on this attack.
         */
        int ZapFrame;

        /**
         *  Ptr to object that the vortex is zapping.
         */
        TARGET TargetObject;

        /**
         *  Distance to the target object.
         */
        int TargetDistance;

        /**
         *  Game frame that vortex hid on.
         */
        int HiddenFrame;

        /**
         *  Direction vortex is going in.
         */
        int XDir;
        int YDir;

        /**
         *  Direction vortex should be going in.
         */
        int DesiredXDir;
        int DesiredYDir;

        /**
         *  Range in cells of the vortex lightning.
         */
        int Range;

        /**
         *  Max speed in leptons per frame of the vortex.
         */
        int Speed;

        /**
         *  Damge of vortex lightning zap.
         */
        int Damage;

        /**
         *  Offscreen buffer to render vortex into. This is needed so we can handle clipping.
         */
        BSurface *RenderBuffer;
        
        /**
         *  
         */
        BuildingTypeClass *FiringBuilding;
};

extern ChronalVortexClass ChronalVortex;
