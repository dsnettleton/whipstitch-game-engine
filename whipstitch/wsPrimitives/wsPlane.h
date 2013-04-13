/**
 *  wsPlane.h
 *  April 4, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsPlane, which inherits the abstract
 *  base class wsPrimitive. A plane in this context is a static piece
 *  of scenery which creates a barrier endlessly in all directions.
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
#ifndef WS_PLANE_H_
#define WS_PLANE_H_

#include "wsPrimitive.h"

class wsPlane : public wsPrimitive {
  private:
    //  Private Data Members
    vec4 posData; //  Normal vector and distance from origin
  public:
    //  Constructors and Deconstructors
    wsPlane(const vec4& normal = vec4(0.0f, 1.0f, 0.0f, 0.0f), const f32 distFromOrigin = 0.0f,
      const u32 myProperties = WS_PRIMITIVE_VISIBLE);
    //  Getters and Setters
    f32 getDistFromOrigin() { return posData.w; }
    vec4 getNormal() { return vec4(posData.x, posData.y, posData.z, 0.0f); }
    vec4 getPosData() { return posData; }
    //  Operational Methods
    void draw();
    void drawBounds();
};

#endif //  WS_PLANE_H_
