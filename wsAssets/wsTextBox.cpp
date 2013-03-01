/**
 *  wsTextBox.cpp
 *  Feb 27, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the class wsTextBox, which inherits the abstract
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

#include "wsTextBox.h"

#include "../wsGraphics/wsRenderSystem.h"

wsTextBox::wsTextBox(const char* myName, vec4 myRectangle, f32 imageWidth, f32 myEndLength, u32 myMaxChars, u32 myLayer, const char* myColorMap, u32 myProperties) :
                wsPanelElement(myRectangle, myLayer, myColorMap, myProperties) {
  type = WS_ELEMENT_TEXT_BOX;
  name = myName;
  endLength = myEndLength;
  endRatio = endLength / imageWidth;
  maxChars = myMaxChars;
  // text = wsNew(wsText, wsText(myRectangle, ));
}

void wsTextBox::draw() {
  if (properties & WS_HUD_VISIBLE) {
    f32 elEnd = 1.0f - endRatio;
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      glBindTexture(GL_TEXTURE_2D, colorMap);
      glPushMatrix();
        glTranslatef(rectangle.x, rectangle.y, 0.0f);
        glBegin(GL_TRIANGLE_STRIP);
          glTexCoord2f(0.0f, 1.0f);
          glVertex2f(0.0f, rectangle.rectH);
          glTexCoord2f(0.0f, 0.0f);
          glVertex2f(0.0f, 0.0f);
          glTexCoord2f(endRatio, 1.0f);
          glVertex2f(endLength, rectangle.rectH);
          glTexCoord2f(endRatio, 0.0f);
          glVertex2f(endLength, 0.0f);
          glTexCoord2f(elEnd, 1.0f);
          glVertex2f(rectangle.rectW - endLength, rectangle.rectH);
          glTexCoord2f(elEnd, 0.0f);
          glVertex2f(rectangle.rectW - endLength, 0.0f);
          glTexCoord2f(1.0f, 1.0f);
          glVertex2f(rectangle.rectW, rectangle.rectH);
          glTexCoord2f(1.0f, 0.0f);
          glVertex2f(rectangle.rectW, 0.0f);
        glEnd();
      glPopMatrix();
    #endif
  }
}
