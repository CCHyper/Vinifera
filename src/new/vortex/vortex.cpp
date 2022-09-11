/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          VORTEX.CPP
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
#include "vortex.h"
#include "object.h"
#include "building.h"
#include "buff.h"
#include "bsurface.h"
#include "tibsun_inline.h"
#include "tibsun_globals.h"
#include "iomap.h"
#include "tactical.h"
#include "voc.h"
#include "layer.h"
#include <algorithm>


/**
 *  Instance of chronal vortex class. This must be the only instance.
 */
ChronalVortexClass ChronalVortex;


/***********************************************************************************************
 * CVC::ChronalVortexClass -- vortex class constructor                                         *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:25PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
ChronalVortexClass::ChronalVortexClass() :
    Position(),
    AnimateDir(0),
    AnimateFrame(-1),
    Animate(0),
    State(STATE_INITIAL),
    VortexRemapTables(),
    LightningRemap(),
    Active(false),
    StartShutdown(false),
    StartHiding(false),
    Hidden(false),
    Theater(THEATER_NONE),
    LastAttackFrame(-1),
    ZapFrame(-1),
    TargetObject(nullptr),
    TargetDistance(0),
    HiddenFrame(-1),
    XDir(0),
    YDir(0),
    DesiredXDir(0),
    DesiredYDir(0),
    Range(10),
    Speed(10),
    Damage(200),
    RenderBuffer(nullptr),
    FiringBuilding(nullptr)
{
}


/***********************************************************************************************
 * CVC::~ChronalVortexClass -- vortex class destructor                                         *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:25PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
ChronalVortexClass::~ChronalVortexClass()
{
    delete RenderBuffer;
    RenderBuffer = nullptr;

    Active = false;
}


/***********************************************************************************************
 * CVC::Appear -- Makes a chronal vortex appear at the given coordinate.                       *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Coordinate that vortex should appear at.                                          *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: This member does nothing if the vortex is already active                          *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:27PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  Makes the vortex appear at the specified coordinate.
 * 
 *  x
 */
void ChronalVortexClass::Appear(Coordinate coordinate)
{
    if (Active) {
        return;
    }

    /*
    ** Adjust the given coordinate so the vortex appears in a central position
    */
    int x = Lepton_To_Pixel(coordinate.X);
    int y = Lepton_To_Pixel(coordinate.Y);

    x -= 32;
    y -= 32;

    LEPTON lx = Pixel_To_Lepton (x);
    LEPTON ly = Pixel_To_Lepton (y);

    Position = XY_Coord (lx, ly);

    /*
    ** Initialise the vortex variables.
    */
    AnimateDir = 1;
    AnimateFrame = 0;
    State = STATE_GROW;
    Active = true;
    Animate = 0;
    StartShutdown = false;
    LastAttackFrame = Frame;
    TargetObject = nullptr;
    ZapFrame = 0;
    Hidden = false;
    StartHiding = false;
    XDir = 0;
    YDir = 0;

    /*
    ** Vortex starts off in a random direction.
    */
    DesiredXDir = Random_Pick(-Speed, Speed);
    DesiredYDir = Random_Pick(-Speed, Speed);

}


/***********************************************************************************************
 * CVC::Disappear -- Makes the chronal vortex go away.                                         *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:30PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  Makes the vortex go away.
 * 
 *  x
 */
void ChronalVortexClass::Disappear()
{
    if (Hidden) {
        Active = false;
    } else {
        StartShutdown = true;
    }
}


/***********************************************************************************************
 * CVC::Hide -- Makes the vortex hide. It might come back later.                               *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: This doesnt deactivate the vortex. Use Disappear to get rid of it permanently.    *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:30PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
void ChronalVortexClass::Hide()
{
    if (!StartShutdown) {
        StartHiding = true;
    }
}


/***********************************************************************************************
 * CVC::Show -- Makes a hidden vortex visible again.                                           *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:31PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
void ChronalVortexClass::Show()
{
    /*
    ** Dont do anything if vortx is dying.
    */
    if (!StartShutdown) {

        /*
        ** If the vortex is hidden then show it again.
        */
        if (Hidden) {
            Hidden = false;
            StartHiding = false;
            AnimateFrame = 0;
            State = STATE_GROW;
            XDir = 0;
            YDir = 0;
        } else {
            /*
            ** If the vortex is in the process of hiding then reverse it.
            */
            StartHiding = false;
            if (State == STATE_SHRINK) {
                State = STATE_GROW;
                AnimateFrame = VORTEX_FRAMES - AnimateFrame;
            }
        }
    }
}


/***********************************************************************************************
 * CVC::Stop -- Stops the vortex without going through the hide animation                      *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:32PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  Stops the vortex without going through the hide animation.
 * 
 *  x
 */
void ChronalVortexClass::Stop()
{
    if (Active) {
        Active = false;
    }
}


/***********************************************************************************************
 * CVC::Load -- Loads the chronal vortex from a savegame file.                                 *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    ptr to file                                                                       *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:32PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
void ChronalVortexClass::Load(Straw &file)
{
    /*
    ** Delete the render buffer as we are going to lose the pointer anyway.
    ** It will be re-allocated when needed.
    */
    delete RenderBuffer;
    RenderBuffer = nullptr;

    file.Get(this, sizeof (ChronalVortexClass));
}


/***********************************************************************************************
 * CVC::Save -- Saves the vortex class data to a savegame file                                 *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    file                                                                              *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:33PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
void ChronalVortexClass::Save(Pipe &file)
{
    Surface *save_ptr = nullptr;

    if (RenderBuffer){
        /*
        ** Save the ptr to the render buffer so we can null it for the save
        */
        save_ptr = RenderBuffer;
        RenderBuffer = nullptr;
    }

    file.Put(this, sizeof(ChronalVortexClass));

    /*
    ** Restore the render buffer ptr
    */
    if (save_ptr){
        RenderBuffer = save_ptr;
    }
}


/***********************************************************************************************
 * CVC::AI -- AI for the vortex. Includes movement and firing.                                 *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:34PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  Call this every frame.
 * 
 *  x
 */
void ChronalVortexClass::AI()
{

    int chance;

    /*
    ** No AI if vortex isnt active
    */
    if (Active) {

        /*
        ** Do the movement AI
        */
        Movement();

        /*
        ** Do the attack AI
        */
        Zap_Target();


        if (Hidden && (Frame - HiddenFrame > 50) ) {
            /*
            ** Vortex is hidden. Chance of it showing itself increases the longer its stays hidden.
            */
            chance = Random_Pick(0, 2000);
            if (chance <= Frame - HiddenFrame) {
                Show();
            }

        } else {

            if (Animate == 0) {

                /*
                ** Its time to animate the vortex.
                */
                AnimateFrame += AnimateDir;

                if (AnimateFrame > VORTEX_FRAMES) {
                    /*
                    ** State changes can only occur on final animation frames.
                    */
                    AnimateFrame = 1;

                    if (StartShutdown) {

                        /*
                        ** Vortex is in the process of dying.
                        */
                        if (State == STATE_SHRINK) {
                            Set_Redraw();
                            Active = false;
                            AnimateFrame = 0;
                        } else {
                            Attack();
                            State = STATE_SHRINK;
                        }
                    } else {

                        if (StartHiding) {
                            /*
                            ** Vortex wants to hide.
                            */
                            if (State == STATE_SHRINK) {
                                /*
                                ** Hide the vortex now.
                                */
                                Set_Redraw();
                                StartHiding = false;
                                Hidden = true;
                                HiddenFrame = Frame;
                                if (Random_Pick(0,4) == 4) {
                                    Disappear();
                                }
                            } else {
                                /*
                                ** Start hiding the vortex.
                                */
                                Attack();
                                State = STATE_SHRINK;
                            }
                        } else {

                            Attack();
                            if (State == STATE_GROW) {
                                State = STATE_ROTATE;
                            } else {
                                //Attack();
                            }
                        }
                    }
                } else {
                    if (AnimateFrame == VORTEX_FRAMES / 2) Attack();
                }
            }
            Animate++;
            Animate &= 1;
        }
    }
}


/***********************************************************************************************
 * CVC::Movement -- Movement AI for the vortex.                                                *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:39PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
void ChronalVortexClass::Movement()
{
    bool newpick = true;

    /*
    ** Update the vortex position by applying the x and y direction variables
    */
    LEPTON x = Position.X;
    LEPTON y = Position.Y;

    x += XDir;
    y += YDir;

    Position = XY_Coord(x,y);

    /*
    ** Reverse the direction of the vortex if its drifting off the map.
    */
    if (x > CELL_LEPTON_W * (Map.MapCellX + Map.MapCellWidth - 4)) {
        newpick = false;
        if (DesiredXDir > 0 ) DesiredXDir = -DesiredXDir;
    }

    if (y > CELL_LEPTON_H * (Map.MapCellY + Map.MapCellHeight - 4)) {
        newpick = false;
        if (DesiredYDir > 0 ) DesiredYDir = -DesiredYDir;
    }

    if (x < CELL_LEPTON_W * Map.MapCellX + 2 * CELL_LEPTON_W) {
        newpick = false;
        if (DesiredXDir < 0 ) DesiredXDir = -DesiredXDir;
    }

    if (y < CELL_LEPTON_H * Map.MapCellY + 2 * CELL_LEPTON_W) {
        newpick = false;
        if (DesiredYDir < 0 ) DesiredYDir = -DesiredYDir;
    }

    /*
    ** Vortex direction tends towards the desired direction unless the vortex is shutting down or
    **    appearing in which case the direction tends towards 0.
    */
    if (State == STATE_ROTATE || Hidden) {
        if (XDir < DesiredXDir) XDir++;
        if (XDir > DesiredXDir) XDir--;
        if (YDir < DesiredYDir) YDir++;
        if (YDir > DesiredYDir) YDir--;
    } else {
        if (XDir > 0) XDir -= Speed/8;
        if (XDir < 0) XDir += Speed/8;
        if (YDir > 0) YDir -= Speed/8;
        if (YDir < 0) YDir += Speed/8;
    }

    /*
    ** Occasionally change the direction of the vortex.
    */
    if (newpick && Random_Pick (0, 100) == 100) {
        DesiredXDir = Random_Pick (-Speed, Speed);
        DesiredYDir = Random_Pick (-Speed, Speed);
    }
}


/***********************************************************************************************
 * CVC::Set_Target -- Make the vortex zap a particular object.                                 *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    ptr to object to zap                                                              *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:42PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  Makes the vortex attack the specified target. Target must be in range of the vortex.
 * 
 *  x
 */
void ChronalVortexClass::Set_Target(ObjectClass *target)
{
    if (Active) {
        ZapFrame = 0;
        TargetObject = nullptr;
        if (target != nullptr) {
            TargetObject = target;
        }
        LastAttackFrame = Frame;
        TargetDistance = (target != nullptr) ? Distance (target->Center_Coord(), Position) : 0;
    }
}


/***********************************************************************************************
 * CVC::Attack -- look for objects to attack                                                   *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:42PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
void ChronalVortexClass::Attack()
{
    /*
    ** Calculate the position of the center of the vortex.
    */
    int x = Lepton_To_Pixel(Position.X);
    int y = Lepton_To_Pixel(Position.Y);

    x += 32;
    y += 12;

    LEPTON lx = Pixel_To_Lepton(x);
    LEPTON ly = Pixel_To_Lepton(y);

    Coordinate here = XY_Coord(lx, ly);

    /*
    ** Scan through the ground layer objects and see who we should attack
    */

    /*
    ** First scan - find any object directly above the vortex.
    */
    for (int i = 0; i < Map.Layer[LAYER_GROUND].Count(); i++) {
        ObjectClass * obj = Map.Layer[LAYER_GROUND][i];

        if ( obj->Is_Techno() && obj->Strength > 0 ) {

            int distance = Distance (obj->Center_Coord(), here);

            if (distance <= CELL_LEPTON_W*2) {
                Set_Target (obj);
                break;
            }
        }
    }

    /*
    ** If we found something to attack then just return
    */
    if (!Target_Legal(TargetObject)) {
        return;
    }


    /*
    ** Scan through all ground level objects.
    **
    ** Objects within range have a chance of being selected based on their distance from the vortex.
    */

    int chance = Random_Pick (0, 1000);
    if (chance > Frame - LastAttackFrame) return;

    for (int i = 0; i < Map.Layer[LAYER_GROUND].Count(); i++) {
        ObjectClass * obj = Map.Layer[LAYER_GROUND][i];

        if ( obj && obj->Is_Techno() ) {

            int distance = Distance (obj->Center_Coord(), Position);

            if (distance < CELL_LEPTON_W * Range) {
                chance = Random_Pick (0, distance);
                if (chance < CELL_LEPTON_W) {
                    Set_Target (obj);
                    break;
                }
            }
        }
    }
}


/***********************************************************************************************
 * CVC::Zap_Target -- If the vortex has a target object then zap it with lightning.            *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:45PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
void ChronalVortexClass::Zap_Target()
{
    if (!Hidden && Target_Legal(TargetObject) && ZapFrame < ZAP_COUNT) {

        /*
        ** Get the center of the vortex.
        */
        int x = Lepton_To_Pixel(Position.X);
        int y = Lepton_To_Pixel(Position.Y);

        x += 32;
        y += 12;

        LEPTON lx = Pixel_To_Lepton(x);
        LEPTON ly = Pixel_To_Lepton(y);

        Coordinate here = XY_Coord(lx, ly);

        /*
        ** Create a temporary techno object se we can access the lightning ability of the tesla.
        */
        TechnoClass *temptech = new BuildingClass(FiringBuilding);
        if (temptech != nullptr) {
            temptech->Coord = here;
            ObjectClass *obj = Target_As_Object(TargetObject);
            Sound_Effect(VOC_TESLA_ZAP, TargetObject->Center_Coord());
            temptech->Fire_At(TargetObject);
            delete temptech;

            /*
            ** Flag the whole map to redraw to cover the lightning.
            */
            Map.Flag_To_Redraw(true);

            /*
            ** Zap the target 3 times but only do damage on the last frame.
            */
            ZapFrame++;

            if (ZapFrame == ZAP_COUNT) {
                ZapFrame = 0;
                int damage = Damage;
                obj->Take_Damage(damage, TargetDistance, WARHEAD_TESLA, nullptr, true);
                TargetObject = nullptr;
            }
        }

        /*
        ** Vortex might pretend to go away after zapping the target.
        */
        if (Random_Pick (0,2) == 2) {
            Hide();
        }
    }
}


/***********************************************************************************************
 * CVC::Coordinate_Remap -- Draws the vortex                                                   *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    ptr to view port to draw the vortex into                                          *
 *           x offset                                                                          *
 *           y offset                                                                          *
 *           width of vortex                                                                   *
 *           height of vortex                                                                  *
 *           ptr to shading remap tables                                                       *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:48PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
void ChronalVortexClass::Coordinate_Remap(BSurface *inbuffer, int x, int y, int width, int height, unsigned char *remap_table)
{
    unsigned char getx, gety, remap_color, pixel_color;

    Buffer destbuf(width * height);

    unsigned char *destptr = (unsigned char *)destbuf.Get_Buffer();

    int destx = x;
    int desty = y;

    int dest_width = width;
    int dest_height = height;

    if (inbuffer->Lock()) {

        /*
        ** Get a pointer to the section of buffer we are going to work on.
        */
        unsigned char *bufptr = (unsigned char *)inbuffer->Get_Buffer_Ptr(destx, desty);

        int modulo = inbuffer->Get_Pitch() + inbuffer->Get_Width();


        for (int yy = desty ; yy < desty+dest_height ; yy++) {
            for (int xx = destx ; xx < destx+dest_width ; xx++) {

                /*
                ** Get the coordinates of the pixel to draw
                */
                getx = *(remap_table++);
                gety = *(remap_table++);
                remap_color = *(remap_table++);

                pixel_color = * (bufptr + getx + (gety * modulo) );

                *(destptr++) = VortexRemapTables[remap_color][pixel_color];
            }

            remap_table += 3 * (width - dest_width);
            destptr += width - dest_width;

        }

        destbuf.To_Page(destx, desty, dest_width, dest_height, *inbuffer);

        inbuffer->Unlock();
    }
}


/***********************************************************************************************
 * CVC::Render -- Renders the vortex at its current position.                                  *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:49PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  Renders the vortex at its current position.
 * 
 *  x
 */
void ChronalVortexClass::Render ()
{
    if (Active && !Hidden) {
        char fname[80];

        int frame;

        /*
        ** Calculate which coordinate lookup table we should be using for this frame.
        */
        switch (State) {
            case STATE_GROW:
                frame = 0;
                break;

            case STATE_ROTATE:
                frame = VORTEX_FRAMES;
                break;

            case STATE_SHRINK:
                frame = VORTEX_FRAMES * 2;
                break;
        }

        frame += AnimateFrame;

        std::snprintf(fname, sizeof(fname), "HOLE%04d.LUT", frame);

        void const *lut_ptr = MFCC::Retrieve(fname);
        if (lut_ptr) {

            /*
            ** Build a representation of the area of the screen where the vortex will be
            ** in an off-screen buffer.
            ** This is necessary for clipping as we cant remap pixels from off screen if we build
            ** the image from the hidpage.
            */
            if (!RenderBuffer) {
                RenderBuffer = new BSurface(CELL_PIXEL_W * 4, CELL_PIXEL_H * 4);
            }
            Cell xc = Coord_XCell (Position);
            Cell yc = Coord_YCell (Position);
            CellClass *cellptr;
            Cell cell;
            TemplateTypeClass const * ttype = 0;
            int icon;        // The icon number to use from the template set.

            Surface * oldpage = Set_Logic_Page(RenderBuffer);

            /*
            ** Temporarily modify the tactical window so it works with our offscreen buffer
            */
            int wx = WindowList[WINDOW_TACTICAL][WINDOWX];
            int wy = WindowList[WINDOW_TACTICAL][WINDOWY];
            int ww = WindowList[WINDOW_TACTICAL][WINDOWWIDTH];
            int wh = WindowList[WINDOW_TACTICAL][WINDOWHEIGHT];

            WindowList[WINDOW_TACTICAL][WINDOWX] = 0;
            WindowList[WINDOW_TACTICAL][WINDOWY] = 0;
            WindowList[WINDOW_TACTICAL][WINDOWWIDTH] = RenderBuffer->Get_Width();
            WindowList[WINDOW_TACTICAL][WINDOWHEIGHT] = RenderBuffer->Get_Height();

            /*
            ** Loop through all the cells that the vortex overlaps and render the template, smudge
            ** and overlay for each cell.
            */
            for (int y = 0 ; y<4; y++) {
                for (int x = 0 ; x<4; x++) {

                    cell = XY_Cell (xc+x, yc+y);
                    if (!cell) {

                        //cellptr = &Map[ Coord_Whole (CELL_Coord(cell)) ];
                        cellptr = &Map[cell];

                        /*
                        **    Fetch a pointer to the template type associated with this cell.
                        */
                        if (cellptr->TType != TEMPLATE_NONE && cellptr->TType != TEMPLATE_CLEAR1 && cellptr->TType != 255) {
                            ttype = &TemplateTypeClass::As_Reference(cellptr->TType);
                            icon = cellptr->TIcon;
                        } else {
                            ttype = &TemplateTypeClass::As_Reference(TEMPLATE_CLEAR1);
                            icon = cellptr->Clear_Icon();
                        }

                        /*
                        ** Draw the template
                        */
                        if (ttype->Get_Image_Data()) {
                            RenderBuffer->Draw_Stamp(ttype->Get_Image_Data(), icon, x*CELL_PIXEL_W, y*CELL_PIXEL_H, nullptr, WINDOW_MAIN);
                        }

                        /*
                        **    Draw any smudge.
                        */
                        if (cellptr->Smudge != SMUDGE_NONE) {
                            SmudgeTypeClass::As_Reference(cellptr->Smudge).Draw_It(x*CELL_PIXEL_W, y*CELL_PIXEL_H, cellptr->SmudgeData);
                        }

                        /*
                        **    Draw the overlay object.
                        */
                        if (cellptr->Overlay != OVERLAY_NONE) {
                            OverlayTypeClass const & otype = OverlayTypeClass::As_Reference(cellptr->Overlay);
                            IsTheaterShape = (bool)otype.IsTheater;    //Tell Build_Frame if this overlay is theater specific
                            CC_Draw_Shape(otype.Get_Image_Data(),
                                                cellptr->OverlayData,
                                                x*CELL_PIXEL_W + (CELL_PIXEL_W >> 1),
                                                y*CELL_PIXEL_H + (CELL_PIXEL_H >> 1),
                                                WINDOW_TACTICAL,
                                                SHAPE_CENTER|SHAPE_WIN_REL|SHAPE_GHOST,
                                                nullptr,
                                                DisplayClass::UnitShadow);

                            IsTheaterShape = false;
                        }

                        /*
                        **    Draw the flag if there is one located at this cell.
                        */
                        if (cellptr->IsFlagged) {
                            void const * flag_remap = HouseClass::As_Pointer(cellptr->Owner)->Remap_Table(false, REMAP_NORMAL);
                            CC_Draw_Shape(MFCC::Retrieve("FLAGFLY.SHP"), Frame % 14, x+(ICON_PIXEL_W/2), y+(ICON_PIXEL_H/2), WINDOW_TACTICAL, SHAPE_CENTER|SHAPE_GHOST|SHAPE_FADING, flag_remap, DisplayClass::UnitShadow);
                        }
                    }
                }
            }


            Set_Logic_Page(oldpage);

            /*
            ** Restore the tactical window to its correct value
            */
            WindowList[WINDOW_TACTICAL][WINDOWX] = wx;
            WindowList[WINDOW_TACTICAL][WINDOWY] = wy;
            WindowList[WINDOW_TACTICAL][WINDOWWIDTH] = ww;
            WindowList[WINDOW_TACTICAL][WINDOWHEIGHT] = wh;

            /*
            ** Render the vortex over the cells we just rendered to our buffer
            */
            Coordinate_Remap(RenderBuffer, Lepton_To_Pixel((Coord_Fraction(Position)).X),
                                           Lepton_To_Pixel((Coord_Fraction(Position)).Y),
                                           64,
                                           64,
                                           (unsigned char *)lut_ptr);


            /*
            ** Calculate the pixel position of our fresh block of cells on the tactical map so
            ** we can blit it to the hid page.
            */
            Coordinate render_pos = XY_Coord(xc * CELL_LEPTON_W, yc * CELL_LEPTON_H);    // Coord_Whole(Position);

            Point2D xy;
            TacticalMap->Coord_To_Pixel(render_pos, xy);

            /*
            ** Create a view port to blit to
            */
            BSurface target(LogicPage->Get_Graphic_Buffer(),
                            0,
                            LogicPage->Get_YPos(),
                            Lepton_To_Pixel(Map.TacLeptonWidth),
                            Lepton_To_Pixel(Map.TacLeptonHeight));

            /*
            ** Do some clipping since the library clipping gets it wrong.
            */
            int diff;

            int source_x = 0;
            int source_y = 0;
            int source_width = CELL_PIXEL_W*4;
            int source_height = CELL_PIXEL_H*4;

            int dest_x = x;
            int dest_y = y;
            int dest_width = source_width;
            int dest_height = source_height;

            if (dest_x < 0) {
                source_width += dest_x;
                dest_width += dest_x;
                source_x -= dest_x;
                dest_x = 0;
            }

            if (dest_y <0) {
                source_height += dest_y;
                dest_height += dest_y;
                source_y -= dest_y;
                dest_y = 0;
            }

            if (dest_x + dest_width > target.Get_Width() ) {
                diff = dest_x + dest_width - target.Get_Width();
                dest_width -= diff;
                source_width -= diff;
            }

            if (dest_y + dest_height > target.Get_Height() ) {
                diff = dest_y + dest_height - target.Get_Height();
                dest_height -= diff;
                source_height -= diff;
            }


            /*
            ** Blit our freshly draw cells and vortex into their correct position on the hidpage
            */
            if (dest_width > 0 && dest_height > 0) {
                RenderBuffer->Copy_From(target, source_x, source_y, dest_x, dest_y, dest_width, dest_height, false);
            }

        }
    }
}


/***********************************************************************************************
 * CVC::Set_Redraw -- Flags the cells under to vortex to redraw.                               *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:50PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  Flags the cells under to vortex to redraw.
 * 
 *  x
 */
void ChronalVortexClass::Set_Redraw()
{
#if 0
    if (Active) {

        Cell xc = Coord_XCell(Position);
        Cell yc = Coord_YCell(Position);

        for (int y = std::max(0, yc - 1) ; y< yc + 4; y++) {
            for (int x = std::max(0, xc - 1) ; x< xc + 4; x++) {
                Cell cell = XY_Cell(x,y);
                if (!cell) {
                    Map[cell].Redraw_Objects();
                }
            }
        }
    }
#endif
}


/**
 *  Initialises the color remap tables based on theater.
 * 
 *  x
 */
void ChronalVortexClass::Setup_Remap_Tables(TheaterType theater)
{
#if 0
    /*
    ** The names of the remap files for each theater
    */
    static char _remaps[3][13] ={
        "TEMP_VTX.PAL",
        "SNOW_VTX.PAL",
        "INTR_VTX.PAL"
    };

    int i;

    /*
    ** If the theater has changed then load the remap tables from disk if they exist or create
    ** them if they dont.
    */
    if (theater != Theater) {

        Theater = theater;

        CCFileClass file (_remaps[(int)Theater]);

        if (file.Is_Available()) {
            file.Read (VortexRemapTables, MAX_REMAP_SHADES*256);
        } else {

            for (i=0 ; i<MAX_REMAP_SHADES ; i++) {
                Build_Fading_Table ( GamePalette, &VortexRemapTables[i][0], 0, 240- ((i*256)/MAX_REMAP_SHADES) );
            }

            file.Write (VortexRemapTables, MAX_REMAP_SHADES*256);
        }
    }

    /*
    ** Set up the remap table for the lightning
    */
    for (i=0 ; i<256 ; i++) {
        LightningRemap[i] = i;
    }
    LightningRemap[192] = 208;
    LightningRemap[193] = 209;
    LightningRemap[194] = 210;
    LightningRemap[195] = 211;
    LightningRemap[196] = 212;
    LightningRemap[197] = 213;
    LightningRemap[198] = 214;
    LightningRemap[199] = 215;
#endif
}


/***********************************************************************************************
 * CVC::Build_Fading_Table -- Builds a fading color lookup table.                              *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    ptr to palette to base tables on                                                  *
 *           ptr to buffer to put clut in.                                                     *
 *           color to bias colors to                                                           *
 *           percentage of bias                                                                *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: Based on Conquer_Build_Fading_Table                                               *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    8/29/96 4:53PM ST : Created                                                              *
 *=============================================================================================*/
/**
 *  x
 * 
 *  x
 */
void ChronalVortexClass::Build_Fading_Table(PaletteClass const & palette, void * dest, int color, int frac)
{
#if 0
    if (dest) {
        unsigned char * ptr = (unsigned char *)dest;

        /*
        **    Find an appropriate remap color index for every color in the palette.
        **    There are certain exceptions to this, but they are trapped within the
        **    loop.
        */
        for (int index = 0; index < PaletteClass::COLOR_COUNT; index++) {

            /*
            **    If this color should not be remapped, then it will be stored as a remap
            **    to itself. This is effectively no remap.
            */
            if (index == 0 ||
                 (index >= CYCLE_COLOR_START && index < (CYCLE_COLOR_START + CYCLE_COLOR_COUNT)) ||
                 index == CC_PULSE_COLOR ||
                 index == CC_EMBER_COLOR) {
                *ptr++ = index;
            } else {

                /*
                **    Find the color that, ideally, the working color should be remapped
                **    to in the special remap range.
                */
                RGBClass trycolor = palette[index];
                trycolor.Adjust(frac, palette[color]);            // Try to match this color.

                /*
                **    Search through the remap range to find the color that should be remapped
                **    to.
                */
                int best = -1;
                int bvalue = 0;
                for (int id = 0; id < PaletteClass::COLOR_COUNT; id++) {

                    if (id != 0 &&
                        (id < CYCLE_COLOR_START || id >= (CYCLE_COLOR_START + CYCLE_COLOR_COUNT)) &&
                         id != CC_PULSE_COLOR &&
                         id != CC_EMBER_COLOR) {

                        int diff = palette[id].Difference(trycolor);
                        if (best == -1 || diff < bvalue) {
                            best = id;
                            bvalue = diff;
                        }
                    }
                }
                *ptr++ = best;
            }
        }
    }
#endif
}


/**
 *  x
 * 
 *  x
 */
void ChronalVortexClass::Detach(TARGET target)
{
    if (Target_Legal(target) && target == TargetObject) {
        TargetObject = nullptr;
    }
}
