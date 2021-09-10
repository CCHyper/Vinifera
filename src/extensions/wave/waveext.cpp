/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          WAVEEXT.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         Extended WaveClass class.
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
#include "waveext.h"
#include "wave.h"
#include "weapontypeext.h"
#include "weapontype.h"
#include "tibsun_inline.h"
#include "foot.h"
#include "wwcrc.h"
#include "wwmath.h"
#include "asserthandler.h"
#include "debughandler.h"


/**
 *  Provides the map for all WaveClass extension instances.
 */
ExtensionMap<WaveClass, WaveClassExtension> WaveClassExtensions;


/**
 *  This is a temporary pointer to the weapon type that the wave object belongs to.
 */
WeaponTypeClass *Wave_TempWeaponTypePtr = nullptr;


/**
 *  Class constructor.
 *  
 *  @author: CCHyper
 */
WaveClassExtension::WaveClassExtension(WaveClass *this_ptr) :
    Extension(this_ptr),
    WeaponTypePtr(nullptr),
    SonicBeamColor{0,0,0},
    SonicBeamIsClear(false),
    SonicBeamAlpha(0.5),
    SonicBeamDuration(0.125),
    SonicBeamAmplitude(12.0),
    SonicBeamOffset(0.49),
    SonicBeamSurfacePattern(SURFACE_PATTERN_CIRCLE),
    SonicBeamSinePattern(SINE_PATTERN_CIRCLE),
    SonicBeamStartPinLeft(-30.0, -100.0, 0.0),
    SonicBeamStartPinRight(-30.0, 100.0, 0.0),
    SonicBeamEndPinLeft(30.0, -100.0, 0.0),
    SonicBeamEndPinRight(30.0, 100.0, 0.0),
    field_CD(false),         
    field_D4(0),
    SonicBeamReversed(false),
    SonicBeamTablesCalculated(false),
    SonicBeamSineTable(),
    SonicBeamSurfacePatternTable(),
    SonicBeamIntensityTable()
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("WaveClassExtension constructor - 0x%08X\n", (uintptr_t)(ThisPtr));
    //DEV_DEBUG_WARNING("WaveClassExtension constructor - 0x%08X\n", (uintptr_t)(ThisPtr));

    IsInitialized = true;
}


/**
 *  Class no-init constructor.
 *  
 *  @author: CCHyper
 */
WaveClassExtension::WaveClassExtension(const NoInitClass &noinit) :
    Extension(noinit)
{
    IsInitialized = false;
}


/**
 *  Class deconstructor.
 *  
 *  @author: CCHyper
 */
WaveClassExtension::~WaveClassExtension()
{
    //DEV_DEBUG_TRACE("WaveClassExtension deconstructor - 0x%08X\n", (uintptr_t)(ThisPtr));
    //DEV_DEBUG_WARNING("WaveClassExtension deconstructor - 0x%08X\n", (uintptr_t)(ThisPtr));

    IsInitialized = false;
}


/**
 *  Initializes an object from the stream where it was saved previously.
 *  
 *  @author: CCHyper
 */
HRESULT WaveClassExtension::Load(IStream *pStm)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("WaveClassExtension::Load - 0x%08X\n", (uintptr_t)(ThisPtr));

    HRESULT hr = Extension::Load(pStm);
    if (FAILED(hr)) {
        return E_FAIL;
    }

    new (this) WaveClassExtension(NoInitClass());
    
    return hr;
}


/**
 *  Saves an object to the specified stream.
 *  
 *  @author: CCHyper
 */
HRESULT WaveClassExtension::Save(IStream *pStm, BOOL fClearDirty)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("WaveClassExtension::Save - 0x%08X\n", (uintptr_t)(ThisPtr));

    HRESULT hr = Extension::Save(pStm, fClearDirty);
    if (FAILED(hr)) {
        return hr;
    }

    return hr;
}


/**
 *  Return the raw size of class data for save/load purposes.
 *  
 *  @author: CCHyper
 */
int WaveClassExtension::Size_Of() const
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("WaveClassExtension::Size_Of - 0x%08X\n", (uintptr_t)(ThisPtr));

    return sizeof(*this);
}


/**
 *  Removes the specified target from any targeting and reference trackers.
 *  
 *  @author: CCHyper
 */
void WaveClassExtension::Detach(TARGET target, bool all)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("WaveClassExtension::Detach - 0x%08X\n", (uintptr_t)(ThisPtr));
}


/**
 *  Compute a unique crc value for this instance.
 *  
 *  @author: CCHyper
 */
void WaveClassExtension::Compute_CRC(WWCRCEngine &crc) const
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("WaveClassExtension::Compute_CRC - 0x%08X\n", (uintptr_t)(ThisPtr));
}


/**
 *  Copies any data required into the newly created wave class extension.
 *  
 *  @author: CCHyper
 */
void WaveClassExtension::Init()
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("WaveClassExtension::Init - 0x%08X\n", (uintptr_t)(ThisPtr));

    /**
     *  Fetch the sonic beam overrides from the weapon.
     */
    if (WeaponTypePtr) {

        WeaponTypeClassExtension *weapontypeext;
        weapontypeext = WeaponTypeClassExtensions.find(WeaponTypePtr);
    
        if (weapontypeext && ThisPtr->Type == WAVE_SONIC) {
            SonicBeamDuration = weapontypeext->SonicBeamDuration;
            SonicBeamAlpha = weapontypeext->SonicBeamAlpha;
            SonicBeamAmplitude = weapontypeext->SonicBeamAmplitude;
            SonicBeamOffset = weapontypeext->SonicBeamOffset;
            SonicBeamSurfacePattern = weapontypeext->SonicBeamSurfacePattern;
            SonicBeamSinePattern = weapontypeext->SonicBeamSinePattern;
            SonicBeamColor = weapontypeext->SonicBeamColor;
            SonicBeamIsClear = weapontypeext->SonicBeamIsClear;
            SonicBeamStartPinLeft = weapontypeext->SonicBeamStartPinLeft;
            SonicBeamStartPinRight = weapontypeext->SonicBeamStartPinRight;
            SonicBeamEndPinLeft = weapontypeext->SonicBeamEndPinLeft;
            SonicBeamEndPinRight = weapontypeext->SonicBeamEndPinRight;
        }
    }
}


/**
 *  Generate the tables for the sonic beam.
 *  
 *  @authors: CCHyper, tomsons26 (additional research by askhati and Morton)
 */
bool WaveClassExtension::Calculate_Sonic_Beam_Tables()
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("WaveClassExtension::Calculate_Sonic_Beam_Tables - 0x%08X\n", (uintptr_t)(ThisPtr));

    if (!SonicBeamTablesCalculated) {

        /**
         *  Generate the sonic beam surface pattern table.
         */
        for (int x = 0; x < 300; ++x) {
            for (int y = 0; y < 300; ++y) {
                switch (SonicBeamSurfacePattern) {
                    default:
                    case SURFACE_PATTERN_CIRCLE:
                        SonicBeamSurfacePatternTable[x][y] = WWMath::Sqrt(x * x + y * y);
                        break;
                    case SURFACE_PATTERN_ELLIPSE:
                    {
                        double x_stretch_coeff = 2.0;
                        double y_stretch_coeff = 1.0;
                        SonicBeamSurfacePatternTable[x][y] = WWMath::Sqrt((x * 2) / x_stretch_coeff + (y * 2) / y_stretch_coeff);
                        break;
                    }
                    case SURFACE_PATTERN_RHOMBUS:
                        SonicBeamSurfacePatternTable[x][y] = WWMath::Sqrt(x + y);
                        break;
                    case SURFACE_PATTERN_SQUARE:
                        SonicBeamSurfacePatternTable[x][y] = WWMath::Sqrt(std::max(std::abs(x), std::abs(y)));
                        break;
                };
            }
        }

        /**
         *  Generate the sonic beam sine pattern table.
         */
        for (int i = 0; i < ARRAY_SIZE(SonicBeamSineTable); ++i) {
            switch (SonicBeamSinePattern) {
                default:
                case SINE_PATTERN_CIRCLE:
                    SonicBeamSineTable[i] = (short)(WWMath::Sin(i * SonicBeamDuration) * SonicBeamAmplitude + SonicBeamOffset);
                    break;
                case SINE_PATTERN_SQUARE:
                    SonicBeamSineTable[i] = (short)(WWMath::Signum(WWMath::Sin(i)));
                    break;
                case SINE_PATTERN_SAWTOOTH:
                    SonicBeamSineTable[i] = (short)(2 * (i/SonicBeamDuration - WWMath::Floor(i/SonicBeamDuration + 1/2)));
                    break;
                case SINE_PATTERN_TRIANGLE:
                {
                    double sawtooth = 2 * (i/SonicBeamDuration - WWMath::Floor(i/SonicBeamDuration + 1/2));
                    SonicBeamSineTable[i] = (short)(2 * std::abs(sawtooth) - 1);
                    break;
                }
            };
        }

        /**
         *  The pixel offset of the wave.
         */
        for (int i = 0; i < ARRAY_SIZE(SonicBeamIntensityTable); ++i) {
            SonicBeamIntensityTable[i] = 110 + (i * 8);
        }

        SonicBeamTablesCalculated = true;
    }

    return true;
}


/**
 *  Re-implementation of Draw_Sonic_Beam_Pixel() for WaveClassExtension.
 * 
 *  @authors: CCHyper, tomsons26 (additional research by askhati and Morton)
 */
void WaveClassExtension::Draw_Sonic_Beam_Pixel(int a1, int a2, int a3, unsigned short *buffer)
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("WaveClassExtension::Draw_Sonic_Beam_Pixel - 0x%08X\n", (uintptr_t)(ThisPtr));

    /**
     *  One-time generate the required sonic beam effect tables.
     */
    if (!SonicBeamTablesCalculated) {
        Calculate_Sonic_Beam_Tables();
    }

    /**
     *  Calculate the pixel offset position based on the surface pattern.
     */
    int wave_pos = ThisPtr->SonicEC + (unsigned short)SonicBeamSurfacePatternTable[a3][std::abs(a1 - ThisPtr->field_6C.X - a2)];
    int pos = std::abs(SonicBeamSineTable[wave_pos]);

    /**
     *  #issue-540
     * 
     *  Possible bug-fix for the common crash, back-ported from Red Alert 2.
     */
    if (pos > 9) {
        pos = 9;
    }

    /**
     *  Get the intensity adjustment value based on the pattern position.
     */
    int intensity = SonicBeamIntensityTable[pos];

    /**
     *  Fetch the offset pixel from the buffer.
     */
    unsigned short pixel = buffer[ThisPtr->field_188[pos]];

    /**
     *  Re-color the buffer pixel.
     */
    int r = 0;
    int g = 0;
    int b = 0;

    /**
     *  Clear beam, not color change.
     */
    if (SonicBeamIsClear) {

        r = (unsigned char)((unsigned char)(pixel >> DSurface::RedLeft) << DSurface::RedRight);
        g = (unsigned char)((unsigned char)(pixel >> DSurface::GreenLeft) << DSurface::GreenRight);
        b = (unsigned char)((unsigned char)(pixel >> DSurface::BlueLeft) << DSurface::BlueRight);


    /**
     *  Custom color has been set.
     */
    } else if (SonicBeamColor.R != 0 && SonicBeamColor.G != 0 && SonicBeamColor.B != 0) {

        int color_r = SonicBeamColor.R;
        int color_g = SonicBeamColor.G;
        int color_b = SonicBeamColor.B;

        /**
         *  Calculate the alpha of the beam.
         */
        int alpha = float(intensity) * 1.0;
        color_r += float((unsigned char)((unsigned char)(pixel >> DSurface::RedLeft) << DSurface::RedRight)) * SonicBeamAlpha;
        color_g += float((unsigned char)((unsigned char)(pixel >> DSurface::GreenLeft) << DSurface::GreenRight)) * SonicBeamAlpha;
        color_b += float((unsigned char)((unsigned char)(pixel >> DSurface::BlueLeft) << DSurface::BlueRight)) * SonicBeamAlpha;
        
        r = color_r + ((alpha * (unsigned char)((unsigned char)(pixel >> DSurface::RedLeft) << DSurface::RedRight)) / 256);
        g = color_g + ((alpha * (unsigned char)((unsigned char)(pixel >> DSurface::GreenLeft) << DSurface::GreenRight)) / 256);
        b = color_b + ((alpha * (unsigned char)((unsigned char)(pixel >> DSurface::BlueLeft) << DSurface::BlueRight)) / 256);

    
    /**
     *  Original sonic beam color code.
     */
    } else {

        r = (unsigned char)((unsigned char)(pixel >> DSurface::RedLeft) << DSurface::RedRight);

        g = ((intensity * (unsigned char)((unsigned char)(pixel >> DSurface::GreenLeft) << DSurface::GreenRight)) / 256)
                        + (unsigned char)((unsigned char)(pixel >> DSurface::GreenLeft) << DSurface::GreenRight);

        b = ((intensity * (unsigned char)((unsigned char)(pixel >> DSurface::BlueLeft) << DSurface::BlueRight)) / 256)
                        + (unsigned char)((unsigned char)(pixel >> DSurface::BlueLeft) << DSurface::BlueRight);
    
    }

    /**
     *  Clamp the color within expected ranges and put the new pixel
     *  back into the buffer.
     */
    r = std::min<int>(r, 255);
    g = std::min<int>(g, 255);
    b = std::min<int>(b, 255);
    *buffer = DSurface::RGBA_To_Pixel(r, g, b);
}


/**
 *  Re-implementation of Wave_AI() for WaveClassExtension.
 * 
 *  @authors: tomsons26, CCHyper
 */
void WaveClassExtension::Wave_AI()
{
    ASSERT(ThisPtr != nullptr);
    //DEV_DEBUG_TRACE("WaveClassExtension::Wave_AI - 0x%08X\n", (uintptr_t)(ThisPtr));

    //#define WAVE_MAG_BEAM 3

    static double dbl_B45D80 = WWMath::Sqrt((WWMath::Pow(256.0, 2.0) * 2));

    //
    // TODO: Need to confirm the TS code compared to YR changes so we can make the
    //       dolphin wave optional.
    //

#if 0
    if (ThisPtr->field_D8 - ThisPtr->field_E0 > 0.1 /*&& ThisPtr->Type == WAVE_SONIC*/) {
        field_CD = true;
    }
    if (ThisPtr->SonicEC == (unsigned int)(1.0 / 0.05) && ThisPtr->Type == WAVE_MAG_BEAM) {
        ThisPtr->SonicEC = 64;
        ++field_D4;
    }
#else
    if (ThisPtr->field_D8 - ThisPtr->field_E0 > 0.1 && !SonicBeamReversed) {
        field_CD = true;
    }
    if (ThisPtr->SonicEC == (unsigned int)(1.0 / 0.05) && SonicBeamReversed) {
        ThisPtr->SonicEC = 64;
        ++field_D4;
    }
#endif

    TechnoClass *object_techno = (TechnoClass *)ThisPtr->xObject;
    TechnoClass *target_techno = (TechnoClass *)ThisPtr->xTarget;

    if (!target_techno || !ThisPtr->xObject || ThisPtr->SonicEC == (unsigned int)(signed __int64)(1.0 / 0.05) || object_techno->TarCom != target_techno) {
        ThisPtr->field_CC = false;
        field_CD = true;
    }

#if 0
    if (ThisPtr->Type == WAVE_MAG_BEAM) {
        ThisPtr->field_CC = false;
        field_CD = true;
    } else {
        if (dbl_B45D80 * 6.0 < (double)Distance(object_techno->Coord, target_techno->entry_5C())) {
            ThisPtr->field_CC = false;
            field_CD = true;
        }
    }
#else
    if (SonicBeamReversed) {
        ThisPtr->field_CC = false;
        field_CD = true;
    } else {
        if (dbl_B45D80 * 6.0 < (double)Distance(object_techno->Coord, target_techno->entry_5C())) {
            ThisPtr->field_CC = false;
            field_CD = true;
        }
    }
#endif

    if (ThisPtr->field_CC) {
#if 0
        if (ThisPtr->Type == WAVE_MAG_BEAM) {
            if (target_techno && target_techno->Kind_Of() == RTTI_UNIT) {
                //WaveClass::Init_Magnetic(target_techno->entry_5C(), object_techno->Fire_Coord());
            } else {
                //WaveClass::Init_Magnetic(object_techno->Fire_Coord(), target_techno->entry_5C());
            }
        } else {
            ThisPtr->func_672160(object_techno->Fire_Coord(), target_techno->entry_5C());
        }
#else
        if (SonicBeamReversed && target_techno && target_techno->Kind_Of() == RTTI_UNIT) {
            ThisPtr->func_672160(target_techno->entry_5C(), object_techno->Fire_Coord());

        } else {
            ThisPtr->func_672160(object_techno->Fire_Coord(), target_techno->entry_5C());
        }
#endif
    }

    double v23 = ThisPtr->field_D8;
    //if (ThisPtr->Type == WAVE_MAG_BEAM) {
    if (SonicBeamReversed) {
        if (v23 < 1.0 && ThisPtr->field_CC) {
            ThisPtr->field_D8 = ThisPtr->field_D8 + 0.05;
            if (ThisPtr->field_D8 > 0.98) {
                ThisPtr->field_D8 = 1.0;
            }
            double v25 = ThisPtr->field_D8;
            Point2D *v27 = ThisPtr->field_E8.Points;

            v27[0].X = (signed __int64)((double)ThisPtr->field_7C.X * v25 + (double)ThisPtr->field_8C.X * (1.0 - v25));
            v27[0].Y = (signed __int64)((double)ThisPtr->field_7C.Y * v25 + (double)ThisPtr->field_8C.Y * (1.0 - v25));

            v27[1].X = (signed __int64)((double)ThisPtr->field_84.X * v25 + (double)ThisPtr->field_94.X * (1.0 - v25));
            v27[1].Y = (signed __int64)((double)ThisPtr->field_84.Y * v25 + (double)ThisPtr->field_94.Y * (1.0 - v25));
        }

        if (field_CD && 0.05 * 0.5 + ThisPtr->field_D8 >= ThisPtr->field_E0) {
            ThisPtr->field_E0 = ThisPtr->field_E0 + 0.05;
            if (ThisPtr->field_D8 <= ThisPtr->field_E0) {
                //ida misses this
                ThisPtr->entry_E4();
                return;
            }

            Point2D *target_techno5 = ThisPtr->field_E8.Points;
            double target_techno6 = ThisPtr->field_E0;

            target_techno5[2].X = (signed __int64)((double)ThisPtr->field_84.X * target_techno6 + (double)ThisPtr->field_94.X * (1.0 - target_techno6));
            target_techno5[2].Y = (signed __int64)((double)ThisPtr->field_84.Y * target_techno6 + (double)ThisPtr->field_94.Y * (1.0 - target_techno6));

            target_techno5[3].X = (signed __int64)((double)ThisPtr->field_7C.X * target_techno6 + (double)ThisPtr->field_8C.X * (1.0 - target_techno6));
            target_techno5[3].Y = (signed __int64)((double)ThisPtr->field_7C.Y * target_techno6 + (double)ThisPtr->field_8C.Y * (1.0 - target_techno6));
        }

    } else {
        if (v23 < 1.0 && ThisPtr->field_CC) {
            ThisPtr->field_D8 = ThisPtr->field_D8 + 0.05;
            if (ThisPtr->field_D8 > 0.98) {
                ThisPtr->field_D8 = 1.0;
            }
            Point2D *object_techno1 = ThisPtr->field_E8.Points;

            double target_techno9 = ThisPtr->field_D8;
            object_techno1->X = (signed __int64)((double)ThisPtr->field_7C.X * target_techno9 + (double)ThisPtr->field_8C.X * (1.0 - target_techno9));
            object_techno1->Y = (signed __int64)((double)ThisPtr->field_7C.Y * target_techno9 + (double)ThisPtr->field_8C.Y * (1.0 - target_techno9));

            object_techno1[1].X = (signed __int64)((double)ThisPtr->field_74.X * target_techno9 + (double)ThisPtr->field_6C.X * (1.0 - target_techno9));
            object_techno1[1].Y = (signed __int64)((double)ThisPtr->field_74.Y * target_techno9 + (double)ThisPtr->field_6C.Y * (1.0 - target_techno9));

            object_techno1[2].X = (signed __int64)((double)ThisPtr->field_84.X * target_techno9 + (double)ThisPtr->field_94.X * (1.0 - target_techno9));
            object_techno1[2].Y = (signed __int64)((double)ThisPtr->field_84.Y * target_techno9 + (double)ThisPtr->field_94.Y * (1.0 - target_techno9));
        }

        if (field_CD && 0.05 * 0.5 + ThisPtr->field_D8 >= ThisPtr->field_E0) {
            ThisPtr->field_E0 = ThisPtr->field_E0 + 0.05;
            if (ThisPtr->field_D8 <= ThisPtr->field_E0) { //ida misses this
                ThisPtr->entry_E4();
                return;
            }
            Point2D *v52 = ThisPtr->field_E8.Points;
            double v53 = ThisPtr->field_E0;

            v52[3].X = (signed __int64)((double)ThisPtr->field_84.X * v53 + (double)ThisPtr->field_94.X * (1.0 - v53));
            v52[3].Y = (signed __int64)((double)ThisPtr->field_84.Y * v53 + (double)ThisPtr->field_94.Y * (1.0 - v53));

            v52[4].X = (signed __int64)((double)ThisPtr->field_74.X * v53 + (double)ThisPtr->field_6C.X * (1.0 - v53));
            v52[4].Y = (signed __int64)((double)ThisPtr->field_74.Y * v53 + (double)ThisPtr->field_6C.Y * (1.0 - v53));

            v52[5].X = (signed __int64)((double)ThisPtr->field_7C.X * v53 + (double)ThisPtr->field_8C.X * (1.0 - v53));
            v52[5].Y = (signed __int64)((double)ThisPtr->field_7C.Y * v53 + (double)ThisPtr->field_8C.Y * (1.0 - v53));
        }
    }
    ThisPtr->func_671C40();
}
