/**
 *  wsPanelElement.h
 *  Jan 30, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the abstract base class wsPanelElement.
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
#ifndef WS_PANEL_ELEMENT_H_
#define WS_PANEL_ELEMENT_H_

#include "../wsUtils.h"

#define WS_HUD_WIDTH   1600.0f
#define WS_HUD_HEIGHT   900.0f

#define WS_HUD_VISIBLE      0x00000001  //  Whether to draw this element
#define WS_HUD_PASSABLE     0x00000002  //  Whether this element is interactive
#define WS_HUD_FOCUS        0x00000004  //  Whether this element is the active input element
#define WS_HUD_MOUSE_OVER   0x00000008  //  Whether the mouse pointer is currently over this element
#define WS_HUD_INACTIVE     0x00000010  //  This (interactive) element cannot be used
#define WS_HUD_CLICKED      0x00000020  //  Whether the pointer is being clicked over this element

#define WS_ELEMENT_PANEL    0x0001
#define WS_ELEMENT_TEXT     0x0002
#define WS_ELEMENT_BUTTON   0x0003
#define WS_ELEMENT_TEXT_BOX 0x0004

class wsPanelElement {
  protected:
    //  Protected Data Members
    vec4 rectangle;
    u32 layer;  //  Panels with higher layers are drawn on top of other panels
    u32 properties; //  HUD properties
    u32 colorMap; //  Texture id
    u32 type;
  public:
    //  Constructors and Deconstructors
    wsPanelElement(vec4 myRectangle, u32 myLayer, const char* myColorMap, u32 myProperties = WS_NULL);
    //  Setters and Getters
    void disable(u32 property) { property ^= properties; properties &= property; }
    void enable(u32 property) { properties |= property; }
    u32 getColorMap() { return colorMap; }
    f32 getHeight() { return rectangle.rectH; }
    u32 getLayer() { return layer; }
    u32 getProperties() { return properties; }
    const vec4& getRect() { return rectangle; }
    u32 getType() { return type; }
    f32 getWidth() { return rectangle.rectW; }
    f32 getX() { return rectangle.x; }
    f32 getY() { return rectangle.y; }
    bool hasProperty(u32 property) { return (properties & property); }
    void setHeight(f32 my) { rectangle.rectH = my; }
    void setLayer(u32 my) { layer = my; }
    void setPos(f32 myX, f32 myY) { rectangle.x = myX; rectangle.y = myY; }
    void setRect(const vec4& myRect) { rectangle = myRect; }
    void setWidth(f32 my) { rectangle.rectW = my; }
    void setX(f32 my) { rectangle.x = my; }
    void setY(f32 my) { rectangle.y = my; }
    //  Operational Methods
    virtual void draw() = 0;
};

#endif //  WS_PANEL_ELEMENT_H_
