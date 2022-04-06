/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          PIXELFX.CPP
 *
 *  @author        CCHyper, tomsons26
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
#include "pixelfx.h"
#include "tibsun_globals.h"
#include "options.h"
#include "tactical.h"
#include "cell.h"
#include "overlay.h"
#include "ccini.h"
#include "random.h"
#include "debughandler.h"
#include "asserthandler.h"


#define BITMASK_8192 8192
#define BITMASK_4096 4096


// DONE
PixelFXClass::PixelFXClass(/*PixelFXType fx_type*/) :
    //FXType(/*fx_type*/),
    CurrentColor(0,0,0),
    BitMask(0),
    BitMaskOffset(0),
    MaxRGB(0,0,0),
    WorkingRGB(0,0,0),
    Offset(0,0),
    CurrentTime(0)
{
    Init();
}


// DONE
PixelFXClass::~PixelFXClass()
{
}


void PixelFXClass::Init(/*PixelFXType fx_type*/)
{
    if (FXType == FX_NONE) {
        return;
    }

    PixelFXTypeStruct &type = PixelEffectTypes[FXType];

    int rand_pos_offset = NonCriticalRandomNumber();
    Offset.X = (rand_pos_offset & 63) - 31;
    Offset.Y = ((rand_pos_offset / 32) & 31) - 15;

    MaxRGB = type.Max;

    if (type.field_18) {
        unsigned mask = (1 <<  type.field_18) - 1;

        unsigned randval1 = NonCriticalRandomNumber();
        MaxRGB.Red -= randval1 & mask;

        unsigned bitval = randval1 >> type.field_18;
        MaxRGB.Green = MaxRGB.Green - (bitval & mask);
        MaxRGB.Blue -= mask & (bitval >> type.field_18);
    }

    WorkingRGB = type.Min;
    CurrentColor = type.Min;

    BitMask = 0;

    int v13 = type.field_20 - type.field_24 + 1;
    BitMaskOffset = type.field_24 + NonCriticalRandomNumber() % v13;

    CurrentTime = type.field_1C & NonCriticalRandomNumber();
}


void PixelFXClass::Update(int time)
{
    BitMask += time * BitMaskOffset;
    if (BitMask > BITMASK_8192) {
        BitMask = BITMASK_8192;
    }

    int mask = BitMask % BITMASK_4096;
    if ((BitMask & BITMASK_4096) != 0) {
        mask = BITMASK_4096 - mask;
    }

    CurrentColor.Red   = (mask * MaxRGB.Red   + (BITMASK_4096 - mask) * WorkingRGB.Red)   / BITMASK_4096;
    CurrentColor.Green = (MaxRGB.Green + (BITMASK_4096 - mask) * WorkingRGB.Green) / BITMASK_4096;
    CurrentColor.Blue  = (mask * MaxRGB.Blue  + (BITMASK_4096 - mask) * WorkingRGB.Blue)  / BITMASK_4096;
}


// DONE
bool PixelFXClass::Is_To_Update(int time)
{
    CurrentTime -= time;
    if (CurrentTime > 0) {
        return false;
    }
    CurrentTime = 0;
    return true;
}


void PixelFXClass::Process(CCINIClass &ini)
{
    /**
     *  Default definition of the info for each pixel effect type.
     */
    static PixelFXTypeStruct PixelEffectTypes[FX_COUNT] = {
        PixelFXTypeStruct { RGBClass{158,158,224}, RGBClass{40,40,80}, 5, 4095, 12, 3 }, // Water
        //PixelFXTypeStruct { RGBClass{255,255,240}, RGBClass{176,144,0}, 0, 4095, 30, 15 } // RA2 Ore
        PixelFXTypeStruct { RGBClass{158,224,158}, RGBClass{40,80,40}, 0, 4095, 30, 15 } // Tiberium
        PixelFXTypeStruct { RGBClass{158,224,158}, RGBClass{40,80,40}, 0, 4095, 30, 15 } // Tiberium
        PixelFXTypeStruct { RGBClass{158,224,158}, RGBClass{40,80,40}, 0, 4095, 30, 15 } // Tiberium
        PixelFXTypeStruct { RGBClass{158,224,158}, RGBClass{40,80,40}, 0, 4095, 30, 15 } // Tiberium
    };

    static const char * const PIXELEFFECTS = "PixelEffects";
    
    char buf[128];	// Working string staging buffer.

    int len = ini.Entry_Count(INI_Name());
    for (int index = 0; index < len; index++) {
        char const * entry = ini.Get_Entry(INI_Name(), index);
        SmudgeType	smudge;		// Smudge type.

        ini.Get_String(INI_Name(), entry, NULL, buf, sizeof(buf));
        smudge = SmudgeTypeClass::From_Name(strtok(buf, ","));
        if (smudge != SMUDGE_NONE) {
            char * ptr = strtok(NULL, ",");
            if (ptr != NULL) {
                int data = 0;
                CELL cell = atoi(ptr);
                ptr = strtok(NULL, ",");
                if (ptr != NULL) data = atoi(ptr);
                new SmudgeClass(smudge, Cell_Coord(cell));
                if (Map[cell].Smudge == smudge && data != 0) {
                    Map[cell].SmudgeData = data;
                }
            }
        }
    }
}


/**
 *  Draw the pixel effect over cells.
 * 
 *  @authors: CCHyper
 */
void PixelFXClass::Draw_It()
{
    if (Options.DetailLevel == 0) {
        return;
    }

    if (TempSurface->Get_Bytes_Per_Pixel() != 2) {
        return;
    }

    unsigned short *buffptr = (unsigned short *)TempSurface->Lock();
    if (!buffptr) {
        return;
    }

    static unsigned long _previous_time = -1;

    Rect *tactical_rect = &TacticalRect;
    Rect *effect_rect = &TacticalRect;

    int surface_pitch = TempSurface->Get_Pitch();

    int curr_time = timeGetTime();
    int time_diff = curr_time - _previous_time;
    _previous_time = curr_time;
    int timing = time_diff;
    if (time_diff > 1000) {
        timing = 1000;
    }

    int v11 = tactical_rect->Height;
    int v12 = tactical_rect->Width;

    int v7 = TacticalMap->TacticalPos.X;
    int v8 = TacticalMap->TacticalPos.Y;

    v9 = v7 + tactical_rect->X;
    v10 = tactical_rect->Y;
    LODWORD(a1.X) = v9 - TacticalRect.X;
    LODWORD(a1.Y) = v10 + v8 - TacticalRect.Y;
    a3.X = (v9 - TacticalRect.X);

    a3.Z = 0.0;
    a3.Y = SLODWORD(a1.Y);
    a3 = a3 * TacticalMap->IsometricMatrix;
    HIWORD(a1.X) = a3.Y / 256;
    LODWORD(v13) = v11 / 15 + 17;
    v33 = v13;
    v15 = v12 / 60 + 4;
    LOWORD(a1.X) = a3.X / 256 - 2;
    v32 = v15;

    int v14 = 0;
    while (v14 < SLODWORD(v13)) {

        v16 = v14 / 2;
        ++v14;
        LOWORD(a3.X) = v16;
        v31 = v14;
        HIWORD(a3.X) = v14 / 2;
        LOWORD(xdimb) = LOWORD(a1.X) + v16;
        HIWORD(xdimb) = HIWORD(a3.X) + HIWORD(a1.X);
        a2 = xdimb;

        if (v15 > 0) {

            v28 = v15;

            do {

                if (Map.func_510FB0(a2)) {

                    CellClass &cell = Map[a2];

                    /**
                     *  Query the cell attributes.
                     */
                    bool is_water = cell.Land_Type() == LAND_WATER;
                    bool has_tiberium = cell.Get_Tiberium_Value() > 0;

                    TiberiumType tib = TIBERIUM_NONE;
                    if (has_tiberium) {
                        tib = OverlayClass::To_TiberiumType(cell.Overlay);
                        // TODO - color override?
                    }

                    if ((cell.Bitfield1 & 0x10) != 0    // IsVisible?
                      && !cell.CellClass_45D4A0(cell)
                      && (is_water || has_tiberium)
                      && !cell.Cell_Occupier() // We don't want to draw on cells with an occupier.
                      && (cell.Bitfield2 & 4096) == 0) {   // Bit4_4?

                        PixelFXClass *pixelfx = Fetch_Extension(EX_RTTI_CELL, &cell)->PixelEffect;
                        PixelFXType fx_type = is_water ? FX_WATER : FX_TIBERIUM;

                        /**
                         *  Update existing cell pixel effect.
                         */
                        if (pixelfx) {
                            if (pixelfx->Get_BitMask() >= BITMASK_8192) {
                                pixelfx->Init(/*fx_type*/);
                            }

                        /**
                         *  Create new effect for the cell.
                         */
                        } else {

                            ColorScheme *tib_color = nullptr;

                            // TODO, fetch cells tiberium then scheme color for the pixel fx?
                            if (has_tiberium) {
                                TiberiumType tib = OverlayClass::To_TiberiumType(cell.Overlay);
                                if (tib != TIBERIUM_NONE) {
                                    tib_color = ColorSchemes[Tiberiums[tib]->Color];
                                }
                            }

                            cell.PixelFX = nullptr;

                            pixelfx = new PixelFXClass(fx_type);
                            if (pixelfx) {
                                cell.PixelFX = pixelfx;
                                //cell.PixelFX->Set_Color( RGBClass(tib_color->field_308) );

                            }

                        }

                        if (pixelfx) {
                            if (pixelfx->Is_To_Update(timing) ) {

                                Rect cell_rect = cell.Get_Rect();
                                int x = pixelfx->Get_Offset().X + cell_rect.X + cell_rect.Width / 2;
                                int y = pixelfx->Get_Offset().Y + cell_rect.Y + cell_rect.Height / 2;

                                if (effect_rect->Is_Within(Point2D(x, y))) {

                                    pixelfx->Update(timing);

                                    unsigned char *bptr = (unsigned char *)buffptr;
                                    unsigned short *pixel_ptr = (unsigned short *)bptr[x + y * surface_pitch];
                                    *pixel_ptr = (unsigned short)DSurface::RGB_To_Pixel(pixelfx->Get_Color().Red,
                                                                                        pixelfx->Get_Color().Green,
                                                                                        pixelfx->Get_Color().Blue);
                                }
                            }
                        }
                    }
                }

                ++a2.X;
                --a2.Y;
                --v28;

            } while (v28);

            v14 = v31;
            v15 = v32;
            v13 = v33;
        }
    }

    TempSurface->Unlock();
}


bool PixelFXClass::Read_INI(CCINIClass &ini)
{
    const char *name = Name.Peek_Buffer();

    if (!ini.Is_Present(name)) {
        return false;
    }

    Min = ini.Get_RGB(name, "ColorMin", Min);
    Max = ini.Get_RGB(name, "ColorMax", Max);
    field_18 = ini.Get_Int(name, "field_18", field_18);
    field_1C = ini.Get_Int(name, "field_1C", field_1C);
    field_20 = ini.Get_Int(name, "field_20", field_20);
    field_24 = ini.Get_Int(name, "field_24", field_24);

    return true;
}


/**
 *  Draw the pixel effects over each cell.
 * 
 *  @authors: CCHyper
 */
void PixelFXClass::Draw_All()
{
    for (int i = PixelEffects.Count()-1; i >= 0; --i) {
        EBoltClass *pixelfx = PixelEffects[i];
        if (!pixelfx) {
            DEV_DEBUG_WARNING("Invalid PixelFX!\n");
            continue;
        }

#if 0
        /**
         *  Is the source object has left the game world, remove this bolt.
         */
        if (pixelfx->Source && (!pixelfx->Source->IsActive || pixelfx->Source->IsInLimbo)) {
            PixelEffects.Delete(pixelfx);
            delete pixelfx;
            continue;
        }

        /**
         *  Update the source coord.
         */
        Coordinate coord = pixelfx->Source_Coord();
        if (coord) {
            pixelfx->StartCoord = coord;
        }
#endif

        /**
         *  Draw the current pixel effect.
         */
        pixelfx->Draw_It();

#if 0
        /**
         *  Electric bolt has expired, delete it.
         */
        if (pixelfx->Lifetime <= 0) {
            PixelEffects.Delete(pixelfx);
            delete pixelfx;
        }
#endif
    }
}


/**
 *  Removes all pixel effects from the game world.
 * 
 *  @author: CCHyper
 */
void PixelFXClass::Clear_All()
{
    for (int i = 0; i < EBolts.Count(); ++i) {
        delete EBolts[i];
    }
    EBolts.Clear();
}
