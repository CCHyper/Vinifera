#include "mapedit_context.h"
#include "tibsun_functions.h"
#include "dsurface.h"
#include "iomap.h"
#include "textprint.h"
#include "wwfont.h"
#include "ttimer.h"
#include "stimer.h"
#include "language.h"
#include "gadget.h"
#include "colorscheme.h"
#include "wwmouse.h"


KeyNumType UnknownKey = KN_NONE;

bool MenuUpdate = 1;
int MenuSkip = 0;

DSurface *MenuSurface;


static int MenuList[][8]={
    { 1 /* MENUX */,
      3 /* MENUY */,
      12 /* ITEMWIDTH */,
      3 /* ITEMSHIGH */,
      0 /* MSELECTED */,
      COLOR_WHITE /* NORMCOL */,
      COLOR_PINK /* HILITE */,
      0 },
};


void Delay(int duration)
{
    unsigned long count;
    TTimerClass<SystemTimerClass> timer;

    while (duration--) {
        count = timer.Value() + 1L;
        while (count >= (unsigned)timer.Value()) {
            ;
        }
    }
}


/**
 *  Draws a caption on a dialog box.
 */
void Draw_Caption(const char *text, Point2D &pos, int w)
{
    #define TPF_TEXT (TPF_6PT_GRAD|TPF_NOSHADOW)

    /*
    **	Draw the caption.
    */
    if (text != nullptr && text[0] != '\0') {
        if (Debug_Map) {
            Fancy_Text_Print(text, TempSurface, &TempSurface->Get_Rect(), &Point2D(w/2 + pos.X, (4) + pos.Y),
                ColorSchemes[GadgetClass::Get_Color_Scheme()], COLOR_TBLACK, TPF_CENTER|TPF_EFNT|TPF_USE_GRAD_PAL|TPF_NOSHADOW);
        } else {
            Fancy_Text_Print(text, TempSurface, &TempSurface->Get_Rect(), &Point2D(w/2 + pos.X, (16) + pos.Y),
                ColorSchemes[GadgetClass::Get_Color_Scheme()], COLOR_TBLACK, TPF_CENTER|TPF_TEXT);
            WWFontClass *font = Font_Ptr(TPF_TEXT);
            int length = font->String_Pixel_Width(text);
            TempSurface->Draw_Line(
                Point2D((pos.X + (w/2)) - (length/2), pos.Y + font->Get_Char_Height() + font->Get_Font_Height() + (16)),
                Point2D((pos.X + (w/2)) + (length/2), pos.Y + font->Get_Char_Height() + font->Get_Font_Height() + (16)),
                ColorSchemes[GadgetClass::Get_Color_Scheme()]->Box);
        }
    }
}

void Draw_Caption(int text, Point2D &pos, int w)
{
    Draw_Caption(Text_String(text), pos, w);
}


/**
 *  Displays a highlighted box.
 */
void Draw_Box(Rect &window, DSurface *surface, BoxStyleEnum up, bool filled)
{
    ColorScheme *scheme = ColorSchemes[GadgetClass::Get_Color_Scheme()];

    /**
     *  Filler, Shadow, Hilite, Corner colors.
     */
    BoxStyleType const ButtonColors[BOXSTYLE_COUNT] = {
        { scheme->Background, scheme->Highlight, scheme->Shadow, scheme->Corners}, // Down
        { scheme->Background, scheme->Shadow, scheme->Highlight, scheme->Corners}, // Raised
        { COLOR_DKGREY, COLOR_WHITE, COLOR_BLACK, COLOR_DKGREY}, // Disabled down
        { COLOR_DKGREY, COLOR_BLACK, COLOR_LTGREY, COLOR_DKGREY}, // Disabled up
        { COLOR_BLACK, scheme->Box, scheme->Box, COLOR_BLACK}, // List box
        { COLOR_BLACK, scheme->Box, scheme->Box, COLOR_BLACK}, // Dialog box
    };

    int x = window.X;
    int y = window.Y;
    int w = window.Width;
    int h = window.Height;

    w--;
    h--;

    const BoxStyleType &style = ButtonColors[up];

    if (filled) {
        surface->Fill_Rect(Rect(x, y, x+w, y+h), style.Filler);
    }

    switch (up) {
        case BOXSTYLE_BOX:
            surface->Draw_Rect(Rect(x, y, x+w, y+h), style.Highlight);
            break;

        case BOXSTYLE_BORDER:
            surface->Draw_Rect(Rect(x+1, y+1, x+w-1, y+h-1), style.Highlight);
            break;

        default:
            surface->Draw_Line(Point2D(x, y+h), Point2D(x+w, y+h), style.Shadow);
            surface->Draw_Line(Point2D(x+w, y), Point2D(x+w, y+h), style.Shadow);

            surface->Draw_Line(Point2D(x, y), Point2D(x+w, y), style.Highlight);
            surface->Draw_Line(Point2D(x, y), Point2D(x, y+h), style.Highlight);

            surface->Put_Pixel(Point2D(x, y+h), style.Corner);
            surface->Put_Pixel(Point2D(x+w, y), style.Corner);
            break;
    }
}



/***********************************************************************************************
 * Dialog_Box -- draws a dialog background box                                                 *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      x,y,w,h      the usual                                                                 *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/26/1995 BR : Created.                                                                  *
 *=============================================================================================*/
void Dialog_Box(Rect &window)
{
    Draw_Box(window, TempSurface, BOXSTYLE_BORDER, true);
}


/***********************************************************************************************
 * Window_Box -- Draws a fancy box over the specified window.                                  *
 *                                                                                             *
 *    This routine will draw a fancy (shaded) box over the specified                           *
 *    window. This is the effect used to give the polished look to                             *
 *    screen rectangles without having to use art.                                             *
 *                                                                                             *
 * INPUT:   window   -- Specified window to fill and border.                                   *
 *                                                                                             *
 *          style    -- The style to render the window.                                        *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   The rendering is done to the LogicPage.                                         *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/03/1992 JLB : Created.                                                                 *
 *   07/31/1992 JLB : Cool raised border effect.                                               *
 *   06/08/1994 JLB : Takes appropriate enumeration parameters.                                *
 *=============================================================================================*/
void Window_Box(Rect &window, DSurface *surface, BoxStyleEnum style)
{
    Draw_Box(window, surface, style, true);
}


/*=========================================================================*/
/*    SELECT_TO_ENTRY:                                                                            */
/*                                                                                                    */
/*        This routine converts a selection to the correct string entry. It       */
/*    does this by search through a long bitfield starting at position index    */
/*    until it finds the correct conversion to entries.                                */
/*                                                                                                    */
/*    INPUTS:    int selection from menu, long the bit field to search, int        */
/*                the starting index within the bit field.                                */
/*    RETURNS:    int the index into the table of entries                                */
/*=========================================================================*/
int Select_To_Entry(int select, unsigned long bitfield, int index)
{
    /**
     *  If all bits are set then it as is.
     */
    if (bitfield == 0xFFFFFFFFL) {
        return select;
    }

    int placement = 0;                                                /* current pos zero        */
    while (select) {                                            /* while still ones        */
        if (bitfield & (1L << (placement+index))) {            /* if this flagged then    */
            select--;                                            /* decrement counter        */
        }
        placement++;                                            /* and we moved a place    */
    }
    while (!(bitfield & (1L << (placement+index)))) {
        placement++;
    }

    /**
     *  Return the position.
     */
    return placement;                                        /* return the position    */
}


/**
 *  Flash the line at the desired location for the menu routine.
 */
void Flash_Line(const char *text, Point2D &pix, ColorSchemeType nfgc, ColorSchemeType hfgc, unsigned bgc)
{
    for (int loop = 0; loop < 3; ++loop) {
        WWMouse->Hide_Mouse();
        Fancy_Text_Print(text, MenuSurface, &MenuSurface->Get_Rect(), &pix, ColorSchemes[hfgc], bgc, TPF_8POINT|TPF_DROPSHADOW);
        Delay(2);
        Fancy_Text_Print(text, MenuSurface, &MenuSurface->Get_Rect(), &pix, ColorSchemes[nfgc], bgc, TPF_8POINT|TPF_DROPSHADOW);
        WWMouse->Show_Mouse();
        Delay(2);
    }
}


/**
 *  Test to see if a given pair of coordinates are within the given rectangular region. 
 */
int Coordinates_In_Region(Point2D &xy, Point2D &in1, Point2D &in2)
{
    return ((xy.X >= in1.X) && (xy.X <= in2.X) && (xy.Y >= in1.Y) && (xy.Y <= in1.Y));
}


/*=========================================================================*/
/*    FIND_MENU_ITEMS:                                                                            */
/*                                                                                                    */
/*        This routine finds the real total items in a menu when certain items    */
/*    may be disabled by bit fields and the like. This is done by looping        */
/*    through the fields, starting at the position passed in index and             */
/*    counting the number of bits that are set.                                            */
/*                                                                                                    */
/*    INPUTS:    int the maximum number of items possible on the menu, long         */
/*                the bit field of enabled and disabled items, char the index        */
/*                point to start at within the list.                                        */
/*    RETURNS:    int the total number of items in the menu                            */
/*=========================================================================*/
int Find_Menu_Items(int maxitems, unsigned long field, char index)
{
    if (field==0xFFFFFFFFL) {                                         /* if all bits are set    */
        return maxitems;                                        /* then maxitems set        */
    }
    
    int ctr = 0;
    for (int loop; loop < maxitems; ++loop) {            /* loop through items    */
        if (field & (1L << (loop + index))) {                /* if the bit is set        */
            ctr++;                                                /*        count the item        */
        }
    }
    return ctr;
}


/**
 *  Setup the basic menu.
 */
void Setup_Menu(Rect &window, const char *text[], unsigned long field, int index, int skip, TextPrintType font_style)
{
    WWFontClass *font = Font_Ptr(TextPrintType(font_style & 0xF));

    int *menuptr = &MenuList[0][0]; // get pointer to menu
    int menuy = window.Y + menuptr[MENUY]; // get the absolute
    int menux = (window.X + menuptr[MENUX]) << 3; // coords of menu
    int item=  Select_To_Entry(menuptr[MSELECTED], field, index);
    int num = menuptr[ITEMSHIGH];

    WWMouse->Hide_Mouse();

    for (int lp = 0 ; lp < num; ++lp) {

        int idx = Select_To_Entry(lp, field, index);

        int drawy = menuy + (lp * font->Get_Font_Height()) + (lp * skip);

        Fancy_Text_Print(text[idx], MenuSurface, &MenuSurface->Get_Rect(), &Point2D(menux, drawy),
            ColorSchemes[menuptr[((idx == item) && (MenuUpdate)) ? HILITE : NORMCOL]], COLOR_TBLACK, font_style|TPF_DROPSHADOW);
    }

    MenuSkip = skip;

    WWMouse->Show_Mouse();
    WWKeyboard->Clear();
}


int Check_Menu(Rect &window, const char *text[], char *selection, long field, int index, TextPrintType font_style)
{
    font_style = TextPrintType(font_style & 0xF);
    WWFontClass *font = Font_Ptr(font_style);

    //selection++;                                                /* get rid of warning    */

    int *menuptr = &MenuList[0][0];                            /* get pointer to menu    */
    int maxitem = menuptr[ITEMSHIGH] - 1;                            /* find max items            */
    int item = menuptr[MSELECTED] % (maxitem + 1);    /* find selected             */
    int newitem = menuptr[MSELECTED] % (maxitem + 1);    /* find selected             */
    int select = -1;                                                    /* no selection made        */
    int menuskip = font->Get_Font_Height() + MenuSkip;                            /* calc new font height    */
    int halfskip = MenuSkip >> 1;                                    /* adjustment for menus    */

    int menuy = window.Y + menuptr[MENUY];                                /* get the absolute         */
    int menux = (window.X + menuptr[MENUX]) << 3;                        /*        coords of menu        */
    ColorSchemeType normcol = ColorScheme::From_Name("White"); //menuptr[NORMCOL];
    ColorSchemeType litcol = ColorScheme::From_Name("Pink"); //menuptr[HILITE];
    ColorScheme *normcolptr = ColorSchemes[normcol];
    ColorScheme *litcolptr = ColorSchemes[litcol];

    /*
    **    Fetch a pending keystroke from the buffer if there is a keystroke
    **    present. If no keystroke is pending then simple mouse tracking will
    **    be done.
    */
    KeyNumType key = KN_NONE;
    UnknownKey = KN_NONE;
    if (WWKeyboard->Check()) {
        key = KeyNumType((WWKeyboard->Get() & 0x18FF));            /* mask off all but release bit    */
    }

    /*
    **  if we are using the mouse and it is installed, then find the mouse
    **  coordinates of the menu and if we are not somewhere on the menu get
    **  the heck outta here. If we are somewhere on the menu, then figure
    **  out the new selected item, and continue forward.
    */
    int mx1 = (window.X << 3) + (menuptr[MENUX] * font->Get_Font_Width());        /* get menu coords        */
    int my1 = (window.Y) + (menuptr[MENUY]) - halfskip;            /*        from the menu        */
    int mx2 = mx1 + (menuptr[ITEMWIDTH] * font->Get_Font_Width()) - 1;        /*        structure as        */
    int my2 = my1 + (menuptr[ITEMSHIGH] * menuskip) - 1;        /*        necessary            */

    int tempy = WWMouse->Get_Mouse_Y();
    if (Coordinates_In_Region(Point2D(WWMouse->Get_Mouse_X(), tempy), Point2D(mx1, my1), Point2D(mx2, my2)) && MenuUpdate) {
        newitem = (tempy - my1) / menuskip;
    }

    switch (key) {

        case KN_UP:                                                /* if the key moves up    */
            newitem--;                                            /*     new item up one    */
            if (newitem < 0) {                                    /* if invalid new item    */
                newitem = maxitem;                                /* put at list bottom    */
            }
            break;
        case KN_DOWN:                                            /* if key moves down        */
            newitem++;                                            /*        new item down one    */
            if (newitem > maxitem) {                             /* if new item past         */
                newitem = 0;                                        /*        list end, clear    */
            }
            break;
        case KN_HOME:                                            /* if top of list key     */
        case KN_PGUP:                                            /*        is selected then    */
            newitem = 0;                                            /*        new item = top        */
            break;
        case KN_END:                                            /* if bottom of list is    */
        case KN_PGDN:                                            /*        selected then        */
            newitem = maxitem;                                    /*        new item = bottom    */
            break;

        /*
        **    Handle mouse button press. Set selection and then fall into the
        **    normal menu item select logic.
        */
        case KN_RMOUSE:
        case KN_LMOUSE:
            if (Coordinates_In_Region(Point2D(WWKeyboard->MouseQX, WWKeyboard->MouseQY), Point2D(mx1, my1), Point2D(mx2, my2))) {
                newitem = (WWKeyboard->MouseQY - my1) / menuskip;
            } else {
                UnknownKey = key;            //    Pass the unprocessed button click back.
                break;
            }

        /*
        **    Normal menu item select logic. Will flash line and exit with menu
        **    selection number.
        */
        case KN_RETURN:                                        /* if a selection is     */
        case KN_SPACE:                                            /*        made with key        */
        case KN_CENTER:
            select = newitem;                                    /*        flag it made.        */
            break;

        case 0:
            break;

        /*
        **    When no key was pressed or an unknown key was pressed, set the
        **    global record of the key and exit normally.
        **    EXCEPTION:    If the key matches the first letter of any of the
        **                    menu entries, then presume it as a selection of
        **                    that entry.
        */
        default:
            for (int idx = 0; idx < menuptr[ITEMSHIGH]; idx++) {
                if (toupper(*(text[Select_To_Entry(idx, field, index)])) == toupper(WWKeyboard->To_ASCII((KeyNumType)(key & 0x0FF)))) {
                    newitem = select = idx;
                    break;
                }
            }
            UnknownKey = key;
            break;
    }

    if (newitem != item) {

        WWMouse->Hide_Mouse();

        int idx = Select_To_Entry(item, field, index);
        int drawy = menuy + (item * menuskip);
        Fancy_Text_Print(text[idx], MenuSurface, &MenuSurface->Get_Rect(), &Point2D(menux, drawy), normcolptr, COLOR_TBLACK, font_style|TPF_DROPSHADOW);

        idx = Select_To_Entry(newitem, field, index);
        drawy = menuy + (newitem * menuskip);
        Fancy_Text_Print(text[idx], MenuSurface, &MenuSurface->Get_Rect(), &Point2D(menux, drawy), litcolptr, COLOR_TBLACK, font_style|TPF_DROPSHADOW);

        WWMouse->Show_Mouse();                                                /* resurrect the mouse    */
    }

    if (select != -1) {
        WWMouse->Hide_Mouse();                                                /* get rid of the mouse    */

        int idx = Select_To_Entry(select, field, index);
        int drawy = menuy + (newitem * menuskip);
        Flash_Line(text[idx], Point2D(menux, drawy), normcol, litcol, COLOR_TBLACK);

        WWMouse->Show_Mouse();
        select = idx;
    }

    menuptr[MSELECTED] = newitem;                            /* update menu select    */

    return(select);
}


/**
 *  Generic context menu processor.
 */
int Do_Context_Menu(Rect &window, DSurface *surface, const char **strings, TextPrintType font_style)
{
    int selection = -1;     // Selection from user.

    if (!strings) {
        return selection;
    }

    WWKeyboard->Clear();

    MenuSurface = surface;

    font_style = TextPrintType(font_style & 0xF);
    WWFontClass *font = Font_Ptr(font_style);

    /**
     *  Determine the number of entries in this string.
     */
    const char **ptr = strings;     // Working menu text pointer.
    int count = 0;                  // Number of entries in this menu.
    while (*ptr++) {
        count++;
    }
    MenuList[0][ITEMSHIGH] = count;

    /**
     *  Determine the width of the menu by finding the length of the
     *  longest menu entry.
     */
    int length = 0;                 // The width of the menu (in pixels).
    ptr = strings;
    while (*ptr) {
        length = std::max(length, (int)font->String_Pixel_Width(*ptr));
        ptr++;
    }
    length += 7;
    MenuList[0][ITEMWIDTH] = length >> 3;

    WWMouse->Show_Mouse();

    /**
     *  Display the menu.
     */
    Window_Box(window, MenuSurface, BOXSTYLE_RAISED);
    Setup_Menu(window, strings, -1, 0, 0, font_style);

    WWKeyboard->Clear();

    UnknownKey = KN_NONE;

    while (selection == -1) {
        Call_Back();
        selection = Check_Menu(window, strings, nullptr, 0xFFL, 0, font_style);
        if (UnknownKey != KN_NONE || UnknownKey == KN_ESC
         || UnknownKey == KN_LMOUSE || UnknownKey == KN_RMOUSE) {
            break;
        }
    }

    WWKeyboard->Clear();
    WWMouse->Hide_Mouse();

    Map.Flag_To_Redraw(true);

    return selection;
}

