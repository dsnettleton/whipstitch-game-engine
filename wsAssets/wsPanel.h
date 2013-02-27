/**
 *  wsPanel.h
 *  Jan 30, 2013
 *  D. Scott Nettleton
 *
 *  This file defines the class wsPanel, which is an image drawn
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
#ifndef WS_PANEL_H_
#define WS_PANEL_H_

#include "wsPanelElement.h"

#ifndef WS_MAX_PANEL_ELEMENTS
  #define WS_MAX_PANEL_ELEMENTS 32
#endif

class wsPanel : public wsPanelElement {
  private:
    //  Private Data Members
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      wsIndexArray* indexArrays;
      u32 numIndexArrays;
      u32 vertexArray;
    #endif
    wsOrderedHashMap<wsPanelElement*>* elements;
  public:
    //  Constructors and Deconstructors
    wsPanel(vec4 myRectangle, u32 myLayer, const char* myColorMap, u32 myProperties = WS_NULL);
    //  Setters and Getters
    u32 getNumElements() { return elements->getLength(); }
    //  Operational Methods
    void addElement(wsPanelElement* myElement);
    void checkMouse(f32 mouseX, f32 mouseY, bool buttonDown);
    void draw();
};

#endif //  WS_PANEL_H_
