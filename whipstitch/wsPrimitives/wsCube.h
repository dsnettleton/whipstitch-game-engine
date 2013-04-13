/**
 *  wsCube.h
 *  March 17, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsCube, which inherits the abstract
 *  base class wsPrimitive. A cube in this context may actually be
 *  a rectangular prism, but it is a static piece of scenery which
 *  contains a bounding box.
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
#ifndef WS_CUBE_H_
#define WS_CUBE_H_

#include "wsPrimitive.h"

class wsCube : public wsPrimitive {
  private:
    //  Private Data Members
    vec4 dimensions;
    vec4 pos;
    quat rot;
    wsPrimVert verts[24]; //  vertices are repeated once per cube face for normal/uv purposes
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      wsIndexArray indexArray;
      u32 numIndexArrays;
      u32 vertexArray;
    #endif
  public:
    //  Constructors and Deconstructors
    wsCube(const f32 width, const f32 height, const f32 length, const vec4& myPos = vec4(0.0f, 0.0f, 0.0f, 1.0f),
      const quat& myRot = quat(0.0f, 0.0f, 0.0f, 1.0f), const u32 myProperties = WS_PRIMITIVE_VISIBLE,
      const f32 textureScale = 1.0f, const char* myColorMap = "", const char* myNormalMap = "");
    //  Setters and Getters
    vec4 getDimensions() { return dimensions; }
    vec4 getPos() { return pos; }
    quat getRot() { return rot; }
    //  Operational Methods
    void draw();
    void drawBounds();
};

#endif //  WS_CUBE_H_
