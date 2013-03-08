/*
 * wsMesh.h
 *
 *  Created on: Sep 19, 2012
 *    Author: dsnettleton
 *
 *    This file declares the class wsMesh, which extends the abstract base
 *    type wsAsset. A wsMesh is only part of the more complete type,
 *    wsModel, but the mesh can be used independently if so desired.
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

#ifndef WS_MESH_H_
#define WS_MESH_H_

#include "wsAsset.h"

#define WS_TEXTURE_MAP_COLOR    0x0001
#define WS_TEXTURE_MAP_NORMAL   0x0002

#define WS_MAX_JOINT_INFLUENCES   8

struct wsVert {
  vec4 pos; //  (BUFFER OFFSET = 0)
  vec4 norm;  //  (BUFFER OFFSET = 16)
  f32 tex[2]; //  UV texture coordinates   (BUFFER OFFSET = 32)
  //  Joint Indices and Weights for animation
  i32 numWeights; //    (BUFFER OFFSET = 40)
  i32 jointIndex[WS_MAX_JOINT_INFLUENCES];  //    (BUFFER OFFSET = 44, 60)
  f32 influence[WS_MAX_JOINT_INFLUENCES];   //    (BUFFER OFFSET = 76, 92)
};

struct wsTriangle {
  u32 vertIndices[3];
};

struct wsMaterial {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 emissive;
  wsTriangle* tris;
  wsHashMap<f32> *properties;
  u32 shininess;
  u32 colorMap;
  u32 normalMap;
  u32 numTriangles;
  u32 numProperties;
};

struct wsJoint {
  vec4 start;
  vec4 startRel;
  vec4 end;
  vec4 endRel;
  quat rot;
  i32 parent;
};

struct wsBounds {
  f32 minX;
  f32 maxX;
  f32 minY;
  f32 maxY;
  f32 minZ;
  f32 maxZ;
  wsBounds() : minX(0.0f), maxX(0.0f), minY(0.0f), maxY(0.0f), minZ(0.0f), maxZ(0.0f) {}
};

class wsMesh: public wsAsset {
  private:
    wsVert* verts;
    wsMaterial* mats;
    wsJoint* joints;
    vec4* jointLocations;
    quat* jointRotations;
    wsHashMap<u32>* jointIndices;
    wsBounds bounds;
    u32 numVerts;
    u32 numMaterials;
    u32 numJoints;
  public:
    //  Constructor
    wsMesh(const char* filepath);
    ~wsMesh();
    //  Getters
    const wsBounds* getBounds() const { return &bounds; }
    const wsJoint* getJoint(const char* jointName) { return &joints[jointIndices->retrieve(wsHash(jointName))]; }
    const wsJoint* getJoints() const { return joints; }
    const u32 getJointIndex(const char* jointName) { return jointIndices->retrieve(wsHash(jointName)); }
    const vec4* getJointLocations() { return jointLocations; }
    const quat* getJointRotations() { return jointRotations; }
    const wsMaterial* getMats() const { return mats; }
    const wsVert* getVerts() const { return verts; }
    u32 getNumJoints() const { return numJoints; }
    u32 getNumMaterials() const { return numMaterials; }
    u32 getNumVerts() const { return numVerts; }
    //  Operational Methods
    void errorCheck(const i32 my);
};

#endif /* WS_MESH_H_ */
