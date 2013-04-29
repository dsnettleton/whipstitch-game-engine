/**
 *  wsPanel.cpp
 *  Feb 18, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the class wsPanel, which is an image drawn
 *  directly to the screen (i.e., a HUD element). Panels can contain
 *  many other interface elements, such as buttons, lists, progressbars,
 *  textboxes, onscreen text, etc.
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

#include "wsPanel.h"
#include "wsButton.h"
#include "../wsGraphics/wsRenderSystem.h"
#include "../wsGameFlow/wsEventManager.h"

/*  Constructor */
wsPanel::wsPanel(vec4 myRectangle, u32 myLayer, const char* myColorMap, u32 myProperties) :
                wsPanelElement(myRectangle, myLayer, myColorMap, myProperties) {
  type = WS_ELEMENT_PANEL;
  elements = wsNew(wsOrderedHashMap<wsPanelElement*>, wsOrderedHashMap<wsPanelElement*>(WS_MAX_PANEL_ELEMENTS));
}

/*  Operational Methods   */
void wsPanel::addElement(wsPanelElement* myElement) {
  wsAssert(myElement != WS_NULL, "Cannot add a NULL element to this panel.");
  elements->insert(elements->getLength(), myElement, myElement->getLayer());
}

void wsPanel::checkMouse(f32 mouseX, f32 mouseY, bool buttonDown) {
  if ( (properties & WS_HUD_VISIBLE) && !(properties & WS_HUD_PASSABLE)) {
    f32 relX = mouseX - rectangle.rectX;
    f32 relY = WS_HUD_HEIGHT - mouseY - rectangle.rectY;
    if (relX >= 0 && relY >= 0 && relX <= rectangle.rectW && relY <= rectangle.rectH) {
      // wsEcho("Checking Mouse: %f, %f, %u\n", relX, relY, (buttonDown)?1:0);
      //  Check buttons and other inputs
      for (wsOrderedHashMap<wsPanelElement*>::iterator it = elements->begin(); it.get() != WS_NULL; ++it) {
        // wsEcho("Ping");
        if (it.get()->getType() == WS_ELEMENT_BUTTON) {
          wsButton* my = (wsButton*)it.get();
          switch (my->getState()) {
            case WS_BUTTON_STATE_ACTIVE:
              if (relX >= my->getX() && relY >= my->getY() && relX <= my->getX() + my->getWidth() &&
                  relY <= my->getY() + my->getHeight()) {
                if (buttonDown) {
                  my->setState(WS_BUTTON_STATE_CLICKED);
                  // wsEvents.push( wsEvent(WS_EVENT_HUD_BUTTON, WS_PRESS, wsHash(my->getName())) );
                }
                else {
                  my->setState(WS_BUTTON_STATE_HOVER);
                  wsEvents.push( wsEvent(WS_EVENT_HUD_BUTTON, WS_HOVER, wsHash(my->getName())) );
                }
              }
              break;
            case WS_BUTTON_STATE_HOVER:
              if (relX >= my->getX() && relY >= my->getY() && relX <= my->getX() + my->getWidth() &&
                  relY <= my->getY() + my->getHeight()) {
                if (buttonDown) {
                  my->setState(WS_BUTTON_STATE_CLICKED);
                  wsEvents.push( wsEvent(WS_EVENT_HUD_BUTTON, WS_PRESS, wsHash(my->getName())) );
                }
              }
              else {
                my->setState(WS_BUTTON_STATE_ACTIVE);
                wsEvents.push( wsEvent(WS_EVENT_HUD_BUTTON, WS_UNHOVER, wsHash(my->getName())) );
              }
              break;
            case WS_BUTTON_STATE_CLICKED:
              if (relX >= my->getX() && relY >= my->getY() &&
                  relX <= my->getX() + my->getWidth() && relY <= my->getY() + my->getHeight()) {
                if (!buttonDown) {
                  //  Activate Button Event
                  my->setState(WS_BUTTON_STATE_HOVER);
                  wsEvents.push( wsEvent(WS_EVENT_HUD_BUTTON, WS_RELEASE, wsHash(my->getName())) );
                }
              }
              else {
                my->setState(WS_BUTTON_STATE_ACTIVE);
              }
              break;
            default:
            case WS_BUTTON_STATE_INACTIVE:
              wsEcho("INACTIVE");
              break;
          }
        }
      }// End for each panel element
    }// End if the mouse is within the panel's borders
  }// End if this panel is available
}// End checkMouse method

void wsPanel::draw() {
  if (properties & WS_HUD_VISIBLE) {
    f32 widthRatio = wsScreenWidth / WS_HUD_WIDTH;
    f32 heightRatio = wsScreenHeight / WS_HUD_HEIGHT;
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      glEnable(GL_SCISSOR_TEST);
      glScissor(rectangle.x*widthRatio, rectangle.y*heightRatio, rectangle.rectW*widthRatio, rectangle.rectH*heightRatio);
      glBindTexture(GL_TEXTURE_2D, colorMap);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glPushMatrix();
        glTranslatef(rectangle.x, rectangle.y, 0.0f);
        glBegin(GL_TRIANGLE_FAN);
          glTexCoord2f(0.0f, 0.0f);
          glVertex2f(0.0f, 0.0f);
          glTexCoord2f(1.0f, 0.0f);
          glVertex2f(rectangle.rectW, 0.0f);
          glTexCoord2f(1.0f, 1.0f);
          glVertex2f(rectangle.rectW, rectangle.rectH);
          glTexCoord2f(0.0f, 1.0f);
          glVertex2f(0.0f, rectangle.rectH);
        glEnd();

        for (wsOrderedHashMap<wsPanelElement*>::iterator it = elements->begin(); it.get() != WS_NULL; ++it) {
          it.get()->draw();
        }
      glPopMatrix();
      glDisable(GL_BLEND);
      glDisable(GL_SCISSOR_TEST);
    #endif
  }
}
