/**
 *  wsTextBox.h
 *  Feb 27, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsTextBox, which inherits the abstract
 *  base class wsPanelElement. A TextBox may be added to a panel for
 *  user-interface purposes. It utilizes a single image file, with predefined
 *  end lengths. The endLength is the width of the image on either side which
 *  is used cap the ends of the textBox. The middle of the textbox is drawn
 *  by stretching the remainder of the image across the remainder of the
 *  textBox's width.
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
#ifndef WS_TEXT_BOX_H_
#define WS_TEXT_BOX_H_

#include "wsPanelElement.h"
#include "wsText.h"

class wsTextBox : public wsPanelElement {
  private:
    //  Private Data Members
    const char* name;
    wsText* text;
    f32 endLength;
    f32 endRatio;
    u32 maxChars;
  public:
    //  Constructors and Deconstructors
    wsTextBox(const char* myName, vec4 myRectangle, f32 imageWidth, f32 myEndLength, u32 myMaxChars, u32 myLayer, const char* myColorMap, u32 myProperties = WS_NULL);
    //  Setters and Getters
    //  @TODO:  Add method for additional colormap (textbox has focus)
    //  Operational Methods
    void draw();
};

#endif //  WS_TEXT_BOX_H_
