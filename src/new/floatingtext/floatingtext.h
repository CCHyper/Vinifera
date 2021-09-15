#pragma once

#include "always.h"
#include "textprint.h"
#include "tibsun_defines.h"
#include "ttimer.h"
#include "ftimer.h"
#include "wstring.h"


class BSurface;
class WWFontClass;


/**
 *  
 */
class FloatingTextClass
{
    private:
        FloatingTextClass();
        ~FloatingTextClass();
        
    public:
        //void Flag_To_Delete() { Lifetime = 0; }
        void Attached_To(TARGET target) { AttachedTo = target; }

        static void Draw_All();
        static void Clear_All();

        static FloatingTextClass *Create(const char *string, ColorSchemeType color, TextPrintType style, Point3D &pos, int frame_time_out, int frame_count, int rate);

    private:
        bool Draw_It();

    private:
        /**
         *  The text string to display.
         */
        Wstring String;

        /**
         *  The object this floating text object is attached to. This is used to
         *  fetch the current world coord [optional].
         */
        TARGET AttachedTo;

        /**
         *  Text color scheme.
         */
        ColorSchemeType Color;

        /**
         *  Text print style (font etc).
         */
        TextPrintType Style;

        /**
         *  Surface to draw the text to.
         */
        BSurface *TextSurface;

        /**
         *  Pointer to the style font used for drawing the text.
         */
        WWFontClass *Font;


        CDTimerClass<FrameTimerClass> Timer;




        /**
         *  Starting location.
         */
        //Point3D Position;

        /**
         *  ????
         */
        //int FrameTimeOut;

        /**
         *  The number of frames this string lasts for.
         */
        int FrameCount;

        /**
         *  The rate at which this string moves.
         */
        //int Rate;

        /**
         *  Is the rate normalised to the game speed?
         */
        //bool IsNormalized;

        /**
         *  Has this string expired and ready is to delete?
         */
        //bool HasExpired;
};
