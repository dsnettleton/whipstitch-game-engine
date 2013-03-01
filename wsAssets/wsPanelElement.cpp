/**
 *  wsPanelElement.cpp
 *  Feb 15, 2013
 *  D. Scott Nettleton
 *
 *  This file implements methods for the class wsPanelElement.
 *  A panel element is a HUD interface object which is drawn onto
 *  a wsPanel object. A wsPanel itself is a Panel Element, so one
 *  panel may contain many.
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

#include "wsPanelElement.h"

#include "../wsGraphics/wsRenderSystem.h"

wsPanelElement::wsPanelElement(vec4 myRectangle, u32 myLayer, const char* myColorMap, u32 myProperties) :
    rectangle(myRectangle), layer(myLayer), properties(myProperties) {
  if (myColorMap != WS_NULL) {
    char filepath[264] = { "textures/" };
    strcat(filepath, myColorMap);
    wsRenderer.loadTexture(&colorMap, filepath, false);
  }
}
