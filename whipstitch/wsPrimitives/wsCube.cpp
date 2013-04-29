/**
 *  wsCube.cpp
 *  March 17, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the class wsCube, which inherits the abstract
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

#include "wsCube.h"
#include "../wsGraphics/wsRenderSystem.h"

wsCube::wsCube(const f32 width, const f32 height, const f32 length, const vec4& myPos, const quat& myRot, const u32 myProperties,
              const u64 myCollisionClass, const f32 textureScaleS, const f32 textureScaleT, const char* myColorMap, const char* myNormalMap) {
  properties = myProperties;
  primType = WS_PRIM_TYPE_CUBE;

  dimensions.x = width;
  dimensions.y = height;
  dimensions.z = length;
  pos = myPos;
  rot = myRot;
  collisionClass = myCollisionClass;

  if (strcmp(myColorMap, "")) {
    wsRenderer.loadTexture(&mat.colorMap, myColorMap, true, true);
  }
  else {
    mat.colorMap = WS_NULL;
  }
  if (strcmp(myNormalMap, "")) {
    wsRenderer.loadTexture(&mat.normalMap, myNormalMap, true, true);
  }
  else {
    mat.normalMap = WS_NULL;
  }// @todo testings

  vec4 vertPositions[8];
  vec4 vertNorms[8];
  vertPositions[0].set(-width/2.0f, -height/2.0f, -length/2.0f);
  vertPositions[1].set(width/2.0f, -height/2.0f, -length/2.0f);
  vertPositions[2].set(width/2.0f, height/2.0f, -length/2.0f);
  vertPositions[3].set(-width/2.0f, height/2.0f, -length/2.0f);
  vertPositions[4].set(-width/2.0f, -height/2.0f, length/2.0f);
  vertPositions[5].set(width/2.0f, -height/2.0f, length/2.0f);
  vertPositions[6].set(width/2.0f, height/2.0f, length/2.0f);
  vertPositions[7].set(-width/2.0f, height/2.0f, length/2.0f);
  for (u32 i = 0; i < 8; ++i) {
    vertPositions[i].rotate(rot);
    vertNorms[i] = vertPositions[i].normal();
    vertPositions[i] += pos;
  }

  #if (WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL)
    //  Front face (Z = -1)
      verts[0].pos = vertPositions[0];
      verts[0].norm = vertNorms[0];
      verts[0].tex[0] = 0.0f; verts[0].tex[1] = 0.0f;
      verts[1].pos = vertPositions[1];
      verts[1].norm = vertNorms[1];
      verts[1].tex[0] = textureScaleS; verts[1].tex[1] = 0.0f;
      verts[2].pos = vertPositions[2];
      verts[2].norm = vertNorms[2];
      verts[2].tex[0] = textureScaleS; verts[2].tex[1] = textureScaleT;
      verts[3].pos = vertPositions[3];
      verts[3].norm = vertNorms[3];
      verts[3].tex[0] = 0.0f; verts[3].tex[1] = textureScaleT;
    //  Right face (X = 1)
      verts[4].pos = vertPositions[1];
      verts[4].norm = vertNorms[1];
      verts[4].tex[0] = 0.0f; verts[4].tex[1] = 0.0f;
      verts[5].pos = vertPositions[5];
      verts[5].norm = vertNorms[5];
      verts[5].tex[0] = textureScaleS; verts[5].tex[1] = 0.0f;
      verts[6].pos = vertPositions[6];
      verts[6].norm = vertNorms[6];
      verts[6].tex[0] = textureScaleS; verts[6].tex[1] = textureScaleT;
      verts[7].pos = vertPositions[2];
      verts[7].norm = vertNorms[2];
      verts[7].tex[0] = 0.0f; verts[7].tex[1] = textureScaleT;
    //  Back face (Z = 1)
      verts[8].pos = vertPositions[5];
      verts[8].norm = vertNorms[5];
      verts[8].tex[0] = 0.0f; verts[8].tex[1] = 0.0f;
      verts[9].pos = vertPositions[4];
      verts[9].norm = vertNorms[4];
      verts[9].tex[0] = textureScaleS; verts[9].tex[1] = 0.0f;
      verts[10].pos = vertPositions[7];
      verts[10].norm = vertNorms[7];
      verts[10].tex[0] = textureScaleS; verts[10].tex[1] = textureScaleT;
      verts[11].pos = vertPositions[6];
      verts[11].norm = vertNorms[6];
      verts[11].tex[0] = 0.0f; verts[11].tex[1] = textureScaleT;
    //  Left face (X = -1)
      verts[12].pos = vertPositions[4];
      verts[12].norm = vertNorms[4];
      verts[12].tex[0] = 0.0f; verts[12].tex[1] = 0.0f;
      verts[13].pos = vertPositions[0];
      verts[13].norm = vertNorms[0];
      verts[13].tex[0] = textureScaleS; verts[13].tex[1] = 0.0f;
      verts[14].pos = vertPositions[3];
      verts[14].norm = vertNorms[3];
      verts[14].tex[0] = textureScaleS; verts[14].tex[1] = textureScaleT;
      verts[15].pos = vertPositions[7];
      verts[15].norm = vertNorms[7];
      verts[15].tex[0] = 0.0f; verts[15].tex[1] = textureScaleT;
    //  Top face (Y = 1)
      verts[16].pos = vertPositions[3];
      verts[16].norm = vertNorms[3];
      verts[16].tex[0] = 0.0f; verts[16].tex[1] = 0.0f;
      verts[17].pos = vertPositions[2];
      verts[17].norm = vertNorms[2];
      verts[17].tex[0] = textureScaleS; verts[17].tex[1] = 0.0f;
      verts[18].pos = vertPositions[6];
      verts[18].norm = vertNorms[6];
      verts[18].tex[0] = textureScaleS; verts[18].tex[1] = textureScaleT;
      verts[19].pos = vertPositions[7];
      verts[19].norm = vertNorms[7];
      verts[19].tex[0] = 0.0f; verts[19].tex[1] = textureScaleT;
    //  Bottom face (Y = -1)
      verts[20].pos = vertPositions[1];
      verts[20].norm = vertNorms[1];
      verts[20].tex[0] = 0.0f; verts[20].tex[1] = 0.0f;
      verts[21].pos = vertPositions[0];
      verts[21].norm = vertNorms[0];
      verts[21].tex[0] = textureScaleS; verts[21].tex[1] = 0.0f;
      verts[22].pos = vertPositions[4];
      verts[22].norm = vertNorms[4];
      verts[22].tex[0] = textureScaleS; verts[22].tex[1] = textureScaleT;
      verts[23].pos = vertPositions[5];
      verts[23].norm = vertNorms[5];
      verts[23].tex[0] = 0.0f; verts[23].tex[1] = textureScaleT;

    //  Index Array
      indexArray.numIndices = 36;
      indexArray.indices = wsNewArray(u32, indexArray.numIndices);
      indexArray.indices[0] = 0;
      indexArray.indices[1] = 2;
      indexArray.indices[2] = 1;
      indexArray.indices[3] = 0;
      indexArray.indices[4] = 3;
      indexArray.indices[5] = 2;
      indexArray.indices[6] = 4;
      indexArray.indices[7] = 6;
      indexArray.indices[8] = 5;
      indexArray.indices[9] = 4;
      indexArray.indices[10] = 7;
      indexArray.indices[11] = 6;
      indexArray.indices[12] = 8;
      indexArray.indices[13] = 10;
      indexArray.indices[14] = 9;
      indexArray.indices[15] = 8;
      indexArray.indices[16] = 11;
      indexArray.indices[17] = 10;
      indexArray.indices[18] = 12;
      indexArray.indices[19] = 14;
      indexArray.indices[20] = 13;
      indexArray.indices[21] = 12;
      indexArray.indices[22] = 15;
      indexArray.indices[23] = 14;
      indexArray.indices[24] = 16;
      indexArray.indices[25] = 18;
      indexArray.indices[26] = 17;
      indexArray.indices[27] = 16;
      indexArray.indices[28] = 19;
      indexArray.indices[29] = 18;
      indexArray.indices[30] = 20;
      indexArray.indices[31] = 22;
      indexArray.indices[32] = 21;
      indexArray.indices[33] = 20;
      indexArray.indices[34] = 23;
      indexArray.indices[35] = 22;

    glGenBuffers(1, &vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wsPrimVert)*24, verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &indexArray.handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArray.handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32)*indexArray.numIndices, indexArray.indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  #endif
}

void wsCube::draw() {
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glBindBuffer(GL_ARRAY_BUFFER, vertexArray);
    glVertexAttribPointer(WS_VERT_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(wsPrimVert), WS_BUFFER_OFFSET(0));
    glVertexAttribPointer(WS_VERT_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(wsPrimVert), WS_BUFFER_OFFSET(16));
    glVertexAttribPointer(WS_VERT_ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(wsPrimVert), WS_BUFFER_OFFSET(32));
    wsRenderer.setPrimMaterial(mat);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArray.handle);
    glDrawElements(GL_TRIANGLES, indexArray.numIndices, GL_UNSIGNED_INT, WS_BUFFER_OFFSET(0));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  #endif
}


void wsCube::drawBounds() {
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glBindBuffer(GL_ARRAY_BUFFER, vertexArray);
    glVertexAttribPointer(WS_VERT_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(wsPrimVert), WS_BUFFER_OFFSET(0));
    wsRenderer.setPrimMaterial(mat);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArray.handle);
    glDrawElements(GL_TRIANGLES, indexArray.numIndices, GL_UNSIGNED_INT, WS_BUFFER_OFFSET(0));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  #endif
}
