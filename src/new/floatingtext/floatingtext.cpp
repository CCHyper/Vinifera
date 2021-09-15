#include "floatingtext.h"
#include "vinifera_globals.h"
#include "tibsun_globals.h"
#include "tactical.h"
#include "object.h"
#include "objecttype.h"
#include "target.h"
#include "textprint.h"
#include "wwfont.h"
#include "bsurface.h"
#include "dsurface.h"
#include "colorscheme.h"
#include "spritecollection.h"
#include "debughandler.h"
#include "asserthandler.h"


#define SURFACE_WIDTH 60
#define SURFACE_HEIGHT 20

#define MASK_STAGES 30          // Larger = Smoother

static BSurface *_masking_surface = nullptr;
//ColorScheme *color_white = nullptr;
//ColorScheme *color_black = nullptr;
//ColorScheme *color_grey = nullptr;
//ColorScheme *color_lightgrey = nullptr;
//ColorScheme *color_green = nullptr;
//ColorScheme *color_red = nullptr;


static void Floating_Text_One_Time()
{
    /**
     *  Perform a once time initialisation of the masking surface. This fills
     *  the surface with a gradient from solid black to solid white.
     */
    //if (!_masking_surface) {
        //_masking_surface = new BSurface(SURFACE_WIDTH, SURFACE_HEIGHT*MASK_STAGES, 1);
        _masking_surface = new BSurface(SURFACE_WIDTH, SURFACE_HEIGHT, 1);
         
#if 0
         int tint_increment = 255/MASK_STAGES;

         // TODO: does not produce final white fill.

        /**
         *  Fill in the fading steps.
         */
        for (int i = 0; i < MASK_STAGES; ++i) {
            
            int value = tint_increment*i;
            unsigned color = DSurface::RGBA_To_Pixel(value, value, value);

            Rect fill_area(0, SURFACE_HEIGHT*i, SURFACE_WIDTH, SURFACE_HEIGHT);

            _masking_surface->Fill_Rect(fill_area, color);
        }
#endif
    //}

    //color_white = ColorScheme::As_Pointer("White");
    //color_black = ColorScheme::As_Pointer("Black");
    //color_grey = ColorScheme::As_Pointer("Grey");
    //color_lightgrey = ColorScheme::As_Pointer("LightGrey");
    //color_green = ColorScheme::As_Pointer("Green");
    //color_red = ColorScheme::As_Pointer("Red");
}


FloatingTextClass::FloatingTextClass() :
    String(),
    AttachedTo(nullptr),
    Color(COLORSCHEME_NONE),
    Style(TPF_NONE),
    TextSurface(nullptr),
    Font(nullptr),
    //Position(),
    //FrameTimeOut(TICKS_PER_SECOND/3),
    FrameCount(0),
    Timer()
    //Rate(0),
    //IsNormalized(true),
    //HasExpired(false)
{
    //TextSurface = new BSurface(SURFACE_WIDTH, SURFACE_HEIGHT*MASK_STAGES, 2);
    TextSurface = new BSurface(SURFACE_WIDTH, SURFACE_HEIGHT, 2);
}


FloatingTextClass::~FloatingTextClass()
{
    delete TextSurface;
    TextSurface = nullptr;
}


bool FloatingTextClass::Draw_It()
{
    TextPrintType style = Style;
    Point2D text_pos(SURFACE_WIDTH/2, Font->Get_Char_Height()/2);

#if 0
    /**
     *  Perform a once time initialisation of the text surface.
     */
    if (!TextSurface) {
        //TextSurface = new BSurface(SURFACE_WIDTH, SURFACE_HEIGHT*MASK_STAGES, 2);
        TextSurface = new BSurface(SURFACE_WIDTH, SURFACE_HEIGHT, 2);

        //TextSurface->Clear();

        /**
         *  Copy the text in the rows steps.
         */
//        for (int i = 0; i < MASK_STAGES; ++i) {

            Simple_Text_Print(String.Peek_Buffer(), TextSurface, &TextSurface->Get_Rect(),
                &text_pos, color_green, COLOR_TBLACK, style);

//            text_pos.Y += SURFACE_HEIGHT;
//        }

    }
#endif


    Simple_Text_Print(String.Peek_Buffer(), TextSurface, &TextSurface->Get_Rect(),
        &text_pos, ColorSchemes[Color], COLOR_TBLACK, style);



#if 0 //#ifndef NDEBUG
    /**
     *  Debug Only: Show output mask on screen.
     */
    CompositeSurface->Copy_From(TacticalRect, *_masking_surface, TextSurface->Get_Rect(), true);
    CompositeSurface->Copy_From(TacticalRect, *TextSurface, TextSurface->Get_Rect(), true);
#endif


    static int inc = 0;




    int value = 255-(MASK_STAGES*inc);
    unsigned color = DSurface::RGB_To_Pixel(value, value, value);
    _masking_surface->Fill(color);



    if ((Frame % 20) == 0) {
        ++inc;
    }
    if (inc >= MASK_STAGES) {
        inc = 0;
    }



    Point2D screen;



    /**
     *  Attached to a object in the game world.
     */
    if (AttachedTo) {

        ObjectClass *objptr = Target_As_Object(AttachedTo);
        if (!objptr) {
            return false;
        }

        Coordinate coord = objptr->Center_Coord();

        screen = TacticalMap->func_60F150(coord);

        screen.X -= TacticalMap->field_5C.X;
        screen.Y -= TacticalMap->field_5C.Y;
        
        if (Is_Target_Building(AttachedTo)) {
            BuildingClass *bptr = Target_As_Building(AttachedTo);
            screen.Y -= objptr->Class_Of()->Pixel_Dimensions().Y;
        }

    } else {

        // TODO

    }

    /**
     *  Adjust draw position relative to the viewable tactical area.
     */
    screen.X += TacticalRect.X;
    screen.Y += TacticalRect.Y;


    // center it
    screen.X -= TextSurface->Get_Width()/2;


    // TEMP to make it move.
    screen.Y -= FrameCount - Timer.Value();



    /**
     *  Determine if the cell draw rect is within the viewport.
     */
    Rect intersect = Intersect(Rect(screen.X, screen.Y, TextSurface->Get_Width(), TextSurface->Get_Height()), TacticalRect);
    if (!intersect.Is_Valid()) {
        return false;
    }

    Rect draw_rect(screen, Point2D(TextSurface->Get_Width(), TextSurface->Get_Height()));

    /**
     *  Draw text to the screen with the alpha mask applied.
     */
    //SpriteCollection.Draw_Alpha(draw_rect, *TempSurface, *TextSurface, *_masking_surface);
    TempSurface->Copy_From(draw_rect, *TextSurface, TextSurface->Get_Rect(), true);

    return true;
}


/**
 *  Draws all active electric bolts to the screen.
 * 
 *  @author: CCHyper
 */
void FloatingTextClass::Draw_All()
{
    /**
     *  Perform a one time initialisation.
     */
    static bool _mask_one_time = false;
    if (!_mask_one_time) {
        Floating_Text_One_Time();
        _mask_one_time = true;
    }

    for (int i = FloatingText.Count()-1; i >= 0; --i) {
        FloatingTextClass *ftext = FloatingText[i];
        if (!ftext) {
            DEV_DEBUG_WARNING("Invalid FloatingText!\n");
            continue;
        }

        /**
         *  Is the source object has left the game world, remove this floating text.
         */
        if (ftext->AttachedTo) {
            ObjectClass *objptr = Target_As_Object(ftext->AttachedTo);
            if (objptr && (!objptr->IsActive || objptr->IsInLimbo)) {
                FloatingText.Delete(ftext);
                delete ftext;
                continue;
            }
        }

        /**
         *  Draw the current floating text.
         */
        ftext->Draw_It();

        /**
         *  Floating text has expired, delete it.
         */
        if (ftext->Timer.Expired()) {
            FloatingText.Delete(ftext);
            delete ftext;
        }

    }
}


/**
 *  Removes all floating text from the game world.
 * 
 *  @author: CCHyper
 */
void FloatingTextClass::Clear_All()
{
    for (int i = 0; i < FloatingText.Count(); ++i) {
        delete FloatingText[i];
    }
    FloatingText.Clear();
}


/**
 *  Add a new floating string.
 * 
 *  @author: CCHyper
 */
FloatingTextClass *FloatingTextClass::Create(const char *string, ColorSchemeType color, TextPrintType style,
                               Point3D &pos, int frame_time_out, int frame_count, int rate)
{
    FloatingTextClass *floating_string = new FloatingTextClass;
    if (!floating_string) {
        return false;
    }

    floating_string->String = nullptr;
    floating_string->Color = color;
    floating_string->Style = style;
    //floating_string->Position = pos;
    //floating_string->FrameTimeOut = frame_time_out;
    floating_string->FrameCount = frame_count;
    floating_string->Timer = frame_count;
    //floating_string->Rate = rate;
    //floating_string->IsNormalized = false;
    floating_string->Font = Font_Ptr(style);
    //floating_string->HasExpired = false;
    floating_string->String = string;

    FloatingText.Add(floating_string);

    return floating_string;
}
