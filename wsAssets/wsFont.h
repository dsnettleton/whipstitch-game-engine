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
 *  Copyright D. Scott Nettleton, 2013
 *  This software is released under the terms of the
 *  Lesser GNU Public License (LGPL).
 *  This file is part of the Whipstitch Game Engine.
 *  The Whipstitch Game Engine is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  The Whipstitch Game Engine is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *  You should have received a copy of the GNU Lesser Public License
 *  along with The Whipstitch Game Engine.
 *  If not, see <http://www.gnu.org/licenses/>.
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
