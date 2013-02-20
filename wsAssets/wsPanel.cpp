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

#include "../wsGraphics/wsRenderSystem.h"

/*  Constructor */
wsPanel::wsPanel(vec4 myRectangle, u32 myLayer, const char* myColorMap, u32 myProperties) :
                wsPanelElement(myRectangle, myLayer, myColorMap, myProperties) {
  elements = wsNew(wsOrderedHashMap<wsPanelElement*>, wsOrderedHashMap<wsPanelElement*>(WS_MAX_PANEL_ELEMENTS));
}

/*  Operational Methods   */
void wsPanel::addElement(wsPanelElement* myElement) {
  wsAssert(myElement != WS_NULL, "Cannot add a NULL element to this panel.");
  elements->insert(elements->getLength(), myElement, myElement->getLayer());
}

void wsPanel::draw() {
  if (properties & WS_HUD_VISIBLE) {
    f32 widthRatio = (f32)wsScreenWidth / 1600.0f;
    f32 heightRatio = (f32)wsScreenHeight / 900.0f;
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
