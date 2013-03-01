/**
 *  wsText.h
 *  Feb 18, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsText, which is a line of text
 *  printed directly to the screen (i.e., a HUD element). The text
 *  is contained within a wsPanel object.
 *
 *  HUD coordinates assume a screen size of 1600x900 for convenience.
 *  When the game is run, if the display is not widescreen, coordinates
 *  are clipped from both ends. So to support, for example, a 4:3 screen
 *  ratio, it's best either to keep all elements within the range x=[200, 1200],
 *  or to rearrange the elements when the screen is resized.
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
#ifndef WS_TEXT_H_
#define WS_TEXT_H_

#include "wsPanelElement.h"
#include "wsFont.h"

class wsText : public wsPanelElement {
  private:
    //  Private Data Members
    vec4 color;
    char* text;
    wsFont* font;
    u32 length;
  public:
    //  Constructors and Deconstructors
    wsText(vec4 myRectangle, const char* myText, wsFont* myFont, u32 myLayer, u32 myProperties);
    //  Setters and Getters
    vec4 getColor() { return color; }
    f32 getHeight() { if (font == WS_NULL) { return 0; } return font->getHeight(); }
    i32 getIntegerValue() { return atoi(text); }
    f32 getFloatValue() { return atof(text); }
    u32 getLength() { return length; }
    const char* getText() { return text; }
    void set(const char* myText, wsFont* myFont);
    void setColor(const vec4& myColor) { color = myColor; }
    void setFont(wsFont* myFont) { font = myFont; }
    wsFont* getFont() { return font; }
    void setText(const char* myText);
    void setText(i32 myVar);
    void setText(f32 myVar);
    //  Operational Methods
    void draw();
    //  calculates and returns the width (in pixels) of the current text string using the current font
    f32 getWidth();
    //  calculates and returns the width (in pixels) of the current text string using the current font, from the first index to the second
    f32 getTextDist(u32 start, u32 end);
    //  returns an index for the position in the text nearest the x coordinate pos. Used primarily for locating cursor in text boxes
    u32 getTextIndex(f32 myDistance);
};

#endif //  WS_TEXT_H_
