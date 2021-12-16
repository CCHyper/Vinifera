/*
The MIT License (MIT)

Copyright (c) 2016 Andre Leiradella

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

// Define these here or somewhere else to add fonts to your imgui app.
#define IMGUIAL_FONTS_COUSINE_REGULAR
#define IMGUIAL_FONTS_DROID_SANS
#define IMGUIAL_FONTS_KARLA_REGULAR
#define IMGUIAL_FONTS_PROGGY_TINY
#define IMGUIAL_FONTS_FONT_AWESOME_5
#define IMGUIAL_FONTS_MATERIAL_DESIGN
#define IMGUIAL_FONTS_KENNEY_ICONS

#ifdef IMGUIAL_FONTS_FONT_AWESOME_5
#include "IconsFontAwesome5.h"
#endif

#ifdef IMGUIAL_FONTS_MATERIAL_DESIGN
#include "IconsMaterialDesign.h"
#endif

#ifdef IMGUIAL_FONTS_KENNEY_ICONS
#include "IconsKenney.h"
#endif

namespace ImGuiAl
{
  namespace Fonts
  {
    enum Font
    {
      kCousineRegular,
      kDroidSans,
      kKarlaRegular,
      kProggyTiny,
      kFontAwesome5Regular,
      kFontAwesome5Solid,
      kMaterialDesign,
      kKenneyIcons
    };
    
    const void* GetCompressedData( Font font, int* size );
  }
}
