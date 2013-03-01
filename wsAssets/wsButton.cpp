/**
 *  wsButton.cpp
 *  Feb 23, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the class wsButton, which inherits the abstract
 *  base class wsPanelElement. wsButtons are attached to a panel and
 *  drawn directly onto the screen. Their positions are given relative
 *  to their parent panel's coordinates, using the standard Whipstitch
 *  screen coordinate system.
 *
 *  A button utilizes a single colormap image, which is divided into
 *  four quadrants. The upper left quadrant is displayed when the button
 *  is in the "active" state: able to be clicked, but at rest. The upper
 *  right quadrant is displayed when a pointer is hovered over the button,
 *  or the button has focus (selected via keyboard/controller interface).
 *  The lower left quadrant is displayed while the pointer is clicking
 *  on the button. The lower right quadrant is displayed when the button
 *  is in its "inactive" state. This can be invisible, greyed out, etc.
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

#include "wsButton.h"

#include "../wsGraphics/wsRenderSystem.h"

wsButton::wsButton(const char* myName, vec4 myRectangle, u32 myLayer, const char* myColorMap, u32 myProperties) :
                wsPanelElement(myRectangle, myLayer, myColorMap, myProperties) {
  type = WS_ELEMENT_BUTTON;
  name = myName;
}

u32 wsButton::getState() {
  if (properties & WS_HUD_INACTIVE) {
    return WS_BUTTON_STATE_INACTIVE;
  }
  else if (properties & WS_HUD_MOUSE_OVER) {
    return WS_BUTTON_STATE_HOVER;
  }
  else if (properties & WS_HUD_CLICKED) {
    return WS_BUTTON_STATE_CLICKED;
  }
  return WS_BUTTON_STATE_ACTIVE;
}

void wsButton::setState(u32 myState) {
  switch (myState) {
    case WS_BUTTON_STATE_ACTIVE:
      myState = (WS_HUD_INACTIVE | WS_HUD_MOUSE_OVER | WS_HUD_CLICKED) ^ properties;
      properties &= myState;
      break;
    case WS_BUTTON_STATE_HOVER:
      myState = (WS_HUD_INACTIVE | WS_HUD_CLICKED) ^ properties;
      properties &= myState;
      properties |= WS_HUD_MOUSE_OVER;
      break;
    case WS_BUTTON_STATE_CLICKED:
      myState = (WS_HUD_INACTIVE | WS_HUD_MOUSE_OVER) ^ properties;
      properties &= myState;
      properties |= WS_HUD_CLICKED;
      break;
    case WS_BUTTON_STATE_INACTIVE:
      myState = (WS_HUD_MOUSE_OVER | WS_HUD_CLICKED) ^ properties;
      properties &= myState;
      properties |= WS_HUD_INACTIVE;
      break;
    default:
      break;
  }
}

void wsButton::draw() {
  if (properties & WS_HUD_VISIBLE) {
    vec4 texRect(0.0f, 0.5f, 0.5f, 1.0f);
    switch (getState()) {
      default:
      case WS_BUTTON_STATE_ACTIVE:
        break;
      case WS_BUTTON_STATE_HOVER:
        texRect.rectX = 0.5f;
        texRect.rectW = 1.0f;
        break;
      case WS_BUTTON_STATE_CLICKED:
        texRect.rectY = 0.0f;
        texRect.rectH = 0.5f;
        break;
      case WS_BUTTON_STATE_INACTIVE:
        texRect.rectX = 0.5f;
        texRect.rectW = 1.0f;
        texRect.rectY = 0.0f;
        texRect.rectH = 0.5f;
        break;
    }
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      glBindTexture(GL_TEXTURE_2D, colorMap);
      glPushMatrix();
        glTranslatef(rectangle.x, rectangle.y, 0.0f);
        glBegin(GL_TRIANGLE_FAN);
          glTexCoord2f(texRect.rectX, texRect.rectY);
          glVertex2f(0.0f, 0.0f);
          glTexCoord2f(texRect.rectW, texRect.rectY);
          glVertex2f(rectangle.rectW, 0.0f);
          glTexCoord2f(texRect.rectW, texRect.rectH);
          glVertex2f(rectangle.rectW, rectangle.rectH);
          glTexCoord2f(texRect.rectX, texRect.rectH);
          glVertex2f(0.0f, rectangle.rectH);
        glEnd();

      glPopMatrix();
    #endif
  }
}
