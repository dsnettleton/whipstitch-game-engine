/**
 *  wsFont.h
 *  Feb 18, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsFont, which utilizes the freetype
 *  font library to load *.ttf (Truetype) font files into glyphs to
 *  be used by in-game text objects.
 *
 *  Fonts are initialized with a font height, which translates to onscreen
 *  pixels. Screen coordinates are given in the range [0,1600] for X
 *  coordinates and [0,900] for Y coordinates. The origin is at the bottom
 *  left. This is true regardless of window resolution. The exception
 *  is that the ranges are clipped to accomodate non-widescreen aspect ratios.
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2013
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef WS_FONT_H_
#define WS_FONT_H_

#include "wsAsset.h"

#include <ft2build.h>
#include FT_FREETYPE_H  //  Header macros have replaced the older directory-based inclusion from previous freetype implementations

#ifndef WS_DEFAULT_FONT_HEIGHT
  #define WS_DEFAULT_FONT_HEIGHT  12
#endif
#define WS_NUM_FONT_TEXTURES 127

class wsFont : public wsAsset {
  private:
    f32* charWidths;
    f32 height;
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      u32* charTextures;
      u32 displayLists;
    #endif
    /// Private Member Functions
    void generateTexture(FT_Face typeFace, u8 index);
  public:
    /// Constructors and Deconstructor
    wsFont(const char* filePath, f32 fontHeight = WS_DEFAULT_FONT_HEIGHT);
    ~wsFont();
    /// Setters and Getters
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      u32 getDisplayLists() { return displayLists; }
      u32 getCharTexture(u8 index) { return charTextures[index]; }
    #endif
    f32 getCharWidth(u8 index) { return charWidths[index]; }
    f32 getHeight() { return height; }
};

#endif //  WS_FONT_H_
