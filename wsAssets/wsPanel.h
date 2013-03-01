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
