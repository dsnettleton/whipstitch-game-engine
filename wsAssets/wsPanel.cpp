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
      // wsLog("Checking Mouse: %f, %f, %u\n", relX, relY, (buttonDown)?1:0);
      //  Check buttons and other inputs
      for (wsOrderedHashMap<wsPanelElement*>::iterator it = elements->begin(); it.get() != WS_NULL; ++it) {
        // wsLog("Ping");
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
              wsLog("INACTIVE");
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
