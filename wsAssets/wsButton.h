/**
 *  wsButton.h
 *  Feb 23, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsButton, which inherits the abstract
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
#ifndef WS_BUTTON_H_
#define WS_BUTTON_H_

#include "wsPanelElement.h"

#define WS_BUTTON_STATE_ACTIVE    0x01
#define WS_BUTTON_STATE_HOVER     0x02
#define WS_BUTTON_STATE_CLICKED   0x03
#define WS_BUTTON_STATE_INACTIVE  0x04

class wsButton : public wsPanelElement {
  private:
    //  Private Data Members
    const char* name;
  public:
    //  Constructors and Deconstructors
    wsButton(const char* myName, vec4 myRectangle, u32 myLayer, const char* myColorMap, u32 myProperties = WS_NULL);
    //  Setters and Getters
    const char* getName() { return name; }
    u32 getState();
    void giveFocus() { properties |= WS_HUD_FOCUS; }
    bool hasFocus() { return (properties & WS_HUD_FOCUS || properties & WS_HUD_MOUSE_OVER); }
    bool isActive() { return !(properties & WS_HUD_INACTIVE); }
    void setState(u32 myState);
    void removeFocus() { u32 mask = WS_HUD_FOCUS ^ properties; properties &= mask; }
    //  Operational Methods
    void draw();
};

#endif //  WS_BUTTON_H_
