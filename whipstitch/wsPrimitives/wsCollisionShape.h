/**
 *  wsCollisionShape.h
 *  April 24, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the abstract base class wsCollisionShape,
 *  which defines a primitive shape type to be accessed by the
 *  physics implementation.
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
#ifndef WS_COLLISION_SHAPE_H_
#define WS_COLLISION_SHAPE_H_

#include "../wsConfig.h"
#include "../wsUtils.h"

class wsCollisionShape {
  protected:
    //  Private Data Members
    f32 dimensions[4];
    u32 type;
  public:
    //  Setters and Getters
    f32 getDim0() { return dimensions[0]; }
    f32 getDim1() { return dimensions[1]; }
    f32 getDim2() { return dimensions[0]; }
    f32 getDim3() { return dimensions[1]; }
    u32 getType() { return type; }
};

#endif //  WS_COLLISION_SHAPE_H_
