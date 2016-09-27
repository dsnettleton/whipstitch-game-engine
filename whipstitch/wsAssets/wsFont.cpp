/**
 *  wsFont.cpp
 *  Feb 18, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the class wsFont, which utilizes the freetype
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
 *  Copyright (c) D. Scott Nettleton, 2013-2016
*/

#include "wsFont.h"
#include FT_GLYPH_H

#include "../wsGraphics/wsRenderSystem.h"

wsFont::wsFont(const char* filePath, f32 fontHeight) {
  wsEcho(WS_LOG_UTIL, "Loading Font %s\n", filePath);
  height = fontHeight;
  assetType = WS_ASSET_TYPE_FONT;

  FT_Library fontLibrary;
  FT_Face typeFace;
  i32 error;

  error = FT_Init_FreeType(&fontLibrary); //  Intiialize the freetype library
  wsAssert(error == 0, "Problem initializing the Freetype Font library.");

  error = FT_New_Face(fontLibrary, filePath, 0, &typeFace); //  The zero indicates that we are loading the first face index (not bold, italic, etc.)
  if (error) {
    wsEcho(WS_LOG_ERROR, "Problem loading file: %s\n", filePath);
  }

  //  Assume a screen resolution of 96 dpi.
  FT_Set_Char_Size(typeFace, 0, height*64, 96, 96); //  A width of zero means 'same as character height'

  u32 numTextures = WS_NUM_FONT_TEXTURES;
  charWidths = wsNewArray(f32, numTextures);

  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    charTextures = wsNewArray(u32, numTextures);
    displayLists = glGenLists(numTextures);
    glGenTextures(numTextures, charTextures);
  #endif

  for(unsigned char i = 0; i < numTextures; ++i) {  //  Generate the actual textures for the display lists
    generateTexture(typeFace, i);
  }

  FT_Done_Face(typeFace);
  FT_Done_FreeType(fontLibrary);
}// End Constructor

wsFont::~wsFont() {
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glDeleteLists(displayLists, WS_NUM_FONT_TEXTURES);
    glDeleteTextures(WS_NUM_FONT_TEXTURES, charTextures);
  #endif
}// End Destructor

void wsFont::generateTexture(FT_Face typeFace, u8 index) {
  FT_Glyph glyph;
  i32 error;

  error = FT_Load_Glyph(typeFace, FT_Get_Char_Index(typeFace, index), FT_LOAD_RENDER );
  if (error) { wsEcho(WS_LOG_ERROR, "Error loading character: %c\n", index); }
  error = FT_Get_Glyph(typeFace->glyph, &glyph);
  if (error) { wsEcho(WS_LOG_ERROR, "Could not load glyph for character: %c\n", index); }

  FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
  FT_BitmapGlyph bmpGlyph = (FT_BitmapGlyph)glyph;

  i32 bmpW = 1;
  i32 bmpH = 1;
  while (bmpW < bmpGlyph->bitmap.width*1.5) {
    bmpW *= 2;
  }
  while (bmpH < bmpGlyph->bitmap.rows*1.5) {
    bmpH *= 2;
  }

  char* bmpData = wsNewArrayTmp(char, 2*bmpW*bmpH);

  for(i32 i = 0; i < bmpH; ++i) {
    for(i32 j = 0; j < bmpW; ++j) {
      if (j >= (i32)bmpGlyph->bitmap.width || i >= (i32)bmpGlyph->bitmap.rows) {
        bmpData[2 * (j + i * bmpW)] = 0;
        bmpData[2 * (j + i * bmpW)+1] = 0;
      }
      else {
        bmpData[2 * (j + i * bmpW)] = bmpGlyph->bitmap.buffer[j + bmpGlyph->bitmap.width * i];
        bmpData[2 * (j + i * bmpW) + 1] = bmpGlyph->bitmap.buffer[j + bmpGlyph->bitmap.width * i];
      }
    }
  }

  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glBindTexture(GL_TEXTURE_2D, charTextures[index]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmpW, bmpH, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, bmpData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glNewList(displayLists + index, GL_COMPILE);
      f32 w, h, u, v;
      w = bmpGlyph->bitmap.width;  //  Inflate the characters a bit.
      h = bmpGlyph->bitmap.rows;
      u = w / (f32)bmpW;
      v = h / (f32)bmpH;
      glPushMatrix();
        glTranslatef(0.0f, bmpGlyph->top-h, 0.0f);
        glBindTexture(GL_TEXTURE_2D, charTextures[index]);
        glBegin(GL_TRIANGLE_FAN);
          glTexCoord2f(0.0f, v);
          glVertex2f(0.0f, 0.0f);
          glTexCoord2f(u, v);
          glVertex2f(w, 0.0f);
          glTexCoord2f(u, 0.0f);
          glVertex2f(w, h);
          glTexCoord2f(0.0f, 0.0f);
          glVertex2f(0.0f, h);
        glEnd();
      glPopMatrix();
      if (w) {
        glTranslatef(w, 0.0f, 0.0f);
      }
      else {
        glTranslatef(height/3.0f, 0.0f, 0.0f);  //  Blank characters become spaces.
      }
    glEndList();
  #endif

  charWidths[index] = w ? w : height/3.0f;  //  pushes the width of the character in pixels, or one half the height if the width is zero
}// End private method generateTexture
