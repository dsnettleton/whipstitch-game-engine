/**
 *  wsCollisionCube.h
 *  April 24, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsCollisionCube, which inherits
 *  the abstract base class wsCollisionShape. A wsCollisionCube
 *  represents a simple bounding box.
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
#ifndef WS_COLLISION_CUBE_H_
#define WS_COLLISION_CUBE_H_

class wsCollisionCube : public wsCollisionShape {
  public:
    //  Constructors and Deconstructors
    wsCollisionCube(const f32 width, const f32 height, const f32 length) {
      type = WS_SHAPE_CUBE;
      dimensions[0] = width;
      dimensions[1] = height;
      dimensions[2] = length;
      dimensions[3] = 0.0f;
    }
    //  Setters and Getters
    f32 getWidth() { return dimensions[0]; }
    f32 getHeight() { return dimensions[1]; }
    f32 getLength() { return dimensions[2]; }
};

#endif //  WS_COLLISION_CUBE_H_
