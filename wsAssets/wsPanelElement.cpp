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
