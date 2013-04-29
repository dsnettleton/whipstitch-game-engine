/**
 *  wsPrimitive.h
 *  March 17, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsPrimitive, which is an abstract
 *  base class for objects contained in a Scene. Primitives can
 *  be cubes, spheres, cylinders, tetrahedrons, etc., but they
 *  all must be static objects with bounding boxes.
 *
 *  Primitives may also act as trigger areas, raising an event
 *  when an object enters or leaves the area.
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
#ifndef WS_PRIMITIVE_H_
#define WS_PRIMITIVE_H_

#include "../wsUtils.h"
#include "../wsConfig.h"

//  Potential types of primitives; not all have
//  been implemented.
#define WS_PRIM_TYPE_PLANE          0x0001
#define WS_PRIM_TYPE_SPHERE         0x0002
#define WS_PRIM_TYPE_CUBE           0x0003
#define WS_PRIM_TYPE_CYLINDER       0x0004
#define WS_PRIM_TYPE_TETRAHEDRON    0x0005
#define WS_PRIM_TYPE_WEDGE          0x0006
#define WS_PRIM_TYPE_MESH           0X0007

//  Define properties for primitives bitflag
#define WS_PRIMITIVE_PASSABLE   0x0001
#define WS_PRIMITIVE_VISIBLE    0x0002
#define WS_PRIMITIVE_TRIGGER    0x0004


//  vertex structure for primitive objects in the Whipstitch game engine.
//  static positions/normals which are not modified by skeletons
struct wsPrimVert {
  vec4 pos;   //  (BUFFER OFFSET = 0)
  vec4 norm;  //  (BUFFER OFFSET = 16)
  f32 tex[2]; //  (BUFFER OFFSET = 32)
  // i32 numWeights; //    (BUFFER OFFSET = 40)
  // wsPrimVert() : numWeights(0) {}
};

//  material structure for primitive objects in the Whipstitch game engine.
//  Same material properties as models, but without triangle arrays
struct wsPrimMaterial {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 emissive;
  wsHashMap<f32> *properties;
  u32 shininess;
  u32 colorMap;
  u32 normalMap;
  u32 numProperties;
  wsPrimMaterial() {
    //  Default settings
    ambient.set(0.1f, 0.1f, 0.1f);
    diffuse.set(1.0f, 1.0f, 1.0f);
    specular.set(1.0f, 1.0f, 1.0f);
    emissive.set(0.0f, 0.0f, 0.0f);
    properties = wsNew(wsHashMap<f32>, wsHashMap<f32>(WS_MAX_PRIM_MATERIAL_PROPERTIES));
    shininess = 12;
    colorMap = normalMap = numProperties = 0;
  }
};

class wsPrimitive {
  protected:
    //  Private Data Members
    wsPrimMaterial mat;
    u64 collisionClass;
    u32 primType;
    u32 properties;
  public:
    //  Setters and getters
    const u64 getCollisionClass() { return collisionClass; }
    u32 getType() { return primType; }
    bool hasProperty(const u32 myProp) { return (properties & myProp); }
    //  Purely Virtual Methods
    virtual void draw() = 0;
    virtual void drawBounds() = 0;
};

#endif //  WS_PRIMITIVE_H_
