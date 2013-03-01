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
