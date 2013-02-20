/**
 *  wsText.cpp
 *  Feb 18, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the implements wsText, which is a line of text
 *  printed directly to the screen (i.e., a HUD element). The text
 *  is contained within a wsPanel object.
 *
 *  HUD coordinates assume a screen size of 1600x900 for convenience.
 *  When the game is run, if the display is not widescreen, coordinates
 *  are clipped from both ends. So to support, for example, a 4:3 screen
 *  ratio, it's best either to keep all elements within the range x=[200, 1200],
 *  or to rearrange the elements when the screen is resized.
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

#include "wsText.h"
#include "../wsGraphics/wsRenderSystem.h"

wsText::wsText(vec4 myRectangle, const char* myText, wsFont* myFont, u32 myLayer, u32 myProperties) :
    wsPanelElement(myRectangle, myLayer, WS_NULL, myProperties), color(1.0f, 1.0f, 1.0f, 1.0f), font(myFont) {
  length = strlen(myText);
  text = wsNewArray(char, length+1);//  Add 1 for null-terminating character (\0)
  strcpy(text, myText);
}

void wsText::draw() {
  if (properties & WS_HUD_VISIBLE) {
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      glColor4fv((GLfloat*)&color);
      u32 myFont = font->getDisplayLists();
      glListBase(myFont);
      glPushMatrix();
        glTranslatef(rectangle.rectX, rectangle.rectY-font->getHeight(), 0.0f);
        glCallLists(length, GL_UNSIGNED_BYTE, text);
      glPopMatrix();
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    #endif
  }
}

void wsText::set(const char* myText, wsFont* myFont) {
  setText(myText);
  font = myFont;
}

void wsText::setText(const char* myText) {
  u32 newLength = strlen(myText);
  if (length < newLength) {
    length = newLength;
    text = wsNewArray(char, newLength+1);// Add 1 for null-terminating character (\0)
    strcpy(text, myText);
  }
  else {
    strcpy(text, myText);
  }
}

void wsText::setText(i32 myVar) {
  sprintf(text, "%d", myVar);
}

void wsText::setText(f32 myVar) {
  sprintf(text, "%f", myVar);
}

f32 wsText::getWidth() {
  f32 totalW = 0.0f;
  for (u32 i = 0; i < length; ++i) {
    totalW += font->getCharWidth(text[i]);
  }
  return totalW;
}

f32 wsText::getTextDist(u32 start, u32 end) {
  f32 totalW = 0.0f;
  if (end >= length) { end = length; }
  for (u32 i = start; i < end; ++i) {
    totalW += font->getCharWidth(text[i]);
  }
  return totalW;
}

u32 wsText::getTextIndex(f32 myDistance) {
  //  Many ways the opposite of getTextDist, this function returns an index for
  //  the position in the text nearest the x coordinate pos.
  //  Its primary purpose is to grab the desired position of a cursor when
  //  clicking in a text box.
  f32 gap = 0.0f;
  myDistance -= rectangle.rectX;
  for (u32 i = 0; i < length; ++i) {
    if (myDistance < gap + font->getCharWidth(text[i])/2) {
      return i;
    }
    gap += font->getCharWidth(text[i]);
  }
  return length;
}
