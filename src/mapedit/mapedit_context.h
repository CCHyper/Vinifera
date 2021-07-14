#pragma once

#include "wwkeyboard.h"
#include "textprint.h"


class DSurface;
class Rect;


extern KeyNumType UnknownKey;

extern bool MenuUpdate;
extern int MenuSkip;

extern DSurface *MenuSurface;


/**********************************************************************
**    This structure is used to control the box relief style drawn by
**    the Draw_Box() function.
*/
typedef struct BoxStyleType {
    int Filler;     // Center box fill color.
    int Shadow;     // Shadow color (darker).
    int Highlight;  // Highlight color (lighter).
    int Corner;     // Corner color (transition).
} BoxStyleType;

typedef enum BoxStyleEnum : unsigned char {
    BOXSTYLE_DOWN,       // Typical depressed edge border.
    BOXSTYLE_RAISED,     // Typical raised edge border.
    BOXSTYLE_DIS_DOWN,   // Disabled but depressed.
    BOXSTYLE_DIS_RAISED, // Disabled but raised.
    BOXSTYLE_BOX,        // list box.
    BOXSTYLE_BORDER,     // main dialog box.

    BOXSTYLE_COUNT
} BoxStyleEnum;

/*
**	This is the menu control structures.
*/
typedef enum MenuIndexType {
    MENUX,
    MENUY,
    ITEMWIDTH,
    ITEMSHIGH,
    MSELECTED,
    NORMCOL,
    HILITE,
} MenuIndexType;


void Draw_Box(Rect &window, DSurface *surface, BoxStyleEnum up, bool filled);
void Dialog_Box(Rect &window);
void Window_Box(Rect &window, DSurface *surface, BoxStyleEnum style);
int Do_Context_Menu(Rect &window, DSurface *surface, const char **strings, TextPrintType font_style);
