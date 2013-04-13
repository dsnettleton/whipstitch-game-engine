/*
 * wsMesh.cpp
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

#include "wsMesh.h"
#include <stdio.h>
#include "../wsGraphics.h"

wsMesh::wsMesh(const char* filepath) {
  u32 hasSkeleton = 0;
  assetType = WS_ASSET_TYPE_MESH;
  wsLog(WS_LOG_GRAPHICS, "Loading Mesh from file \"%s\"\n", filepath);
  FILE* pFile;
  pFile = fopen(filepath, "r");
  wsAssert( pFile, "Error Loading file." );
  //  Scan past the header
  errorCheck( fscanf( pFile,  "//  Whipstitch Mesh File\n//  This mesh is for use with the Whipstitch Game Engine\n"
                "//  For more information, email dsnettleton@whipstitchgames.com\n\n") );
  f32 versionNumber;
  errorCheck( fscanf( pFile, "versionNumber %f\n", &versionNumber) );
  char nameBuffer[255];
  errorCheck( fscanf( pFile, "meshName %[^\t\n]\n", nameBuffer) );
  errorCheck( fscanf( pFile, "numVertices %u\n", &numVerts) );
  errorCheck( fscanf( pFile, "numMaterials %u\n", &numMaterials) );
  errorCheck( fscanf( pFile, "defaultPos { %f %f %f }\n", &defaultPos.x, &defaultPos.y, &defaultPos.z) );
  errorCheck( fscanf( pFile, "hasSkeleton %u\n\n", &hasSkeleton) );
  wsLog(WS_LOG_GRAPHICS, "meshName = %s, numVertices = %u, numMaterials = %u, hasSkeleton = %u\n", nameBuffer, numVerts, numMaterials, hasSkeleton);
  //  Generate object arrays and place them on the current stack
  mats = wsNewArray(wsMaterial, numMaterials);
  verts = wsNewArray(wsVert, numVerts);
  if (hasSkeleton > 0) {
    errorCheck( fscanf( pFile, "skeleton {\n" ) );
    errorCheck( fscanf( pFile, "  numJoints %u\n", &numJoints) );
    //  Initialize joint array for this skeleton
    joints = wsNewArray(wsJoint, numJoints);
    jointLocations = wsNewArray(vec4, numJoints);
    jointRotations = wsNewArray(quat, numJoints);
    jointIndices = wsNew(wsHashMap<u32>, wsHashMap<u32>(numJoints));
    u32 currentJ;
    for (u32 j = 0; j < numJoints; ++j) {
      errorCheck( fscanf( pFile, "  joint %u {\n", &currentJ) );
      wsAssert( (currentJ == j), "Current index does not relate to current joint.");
      errorCheck( fscanf( pFile, "    name %[^\t\n]\n", nameBuffer) );
      errorCheck( fscanf( pFile, "    parent %d\n", &joints[j].parent) );
      errorCheck( fscanf( pFile, "    pos_start { %f %f %f }\n",
              &joints[j].start.x,
              &joints[j].start.y,
              &joints[j].start.z ) );
      errorCheck( fscanf( pFile, "    pos_end { %f %f %f }\n",
              &joints[j].end.x,
              &joints[j].end.y,
              &joints[j].end.z ) );
      errorCheck( fscanf( pFile, "    rotation { %f %f %f %f }\n",
              &joints[j].rot.x,
              &joints[j].rot.y,
              &joints[j].rot.z,
              &joints[j].rot.w ) );
      errorCheck( fscanf( pFile, "  }\n") );
      wsLog(WS_LOG_GRAPHICS, "jointName = %s\n", nameBuffer);
      jointIndices->insert(wsHash(nameBuffer), j);
      if (j >= 0) {
        //  Align to model rotation/location
        jointLocations[j] = joints[j].start;
        jointRotations[j] = joints[j].rot;
        joints[j].end -= joints[j].start;
        if (joints[j].parent >= 0) {
          joints[j].startRel = joints[j].start - joints[joints[j].parent].start;
          joints[j].startRel.rotate(joints[joints[j].parent].rot.getInverse());
        }
        else {
          joints[j].startRel = joints[j].start;
        }
        joints[j].end.rotate(joints[j].rot.getInverse());
      }
    }
    errorCheck( fscanf( pFile, "}\n\n") );
  }// End if (hasSkeleton)
  else {
    joints = WS_NULL;
    jointLocations = WS_NULL;
    jointRotations = WS_NULL;
    jointIndices = WS_NULL;
  }
  errorCheck( fscanf( pFile, "vertices {\n") );
  errorCheck( fscanf( pFile, "  bounds { %f %f %f }\n", &bounds.x, &bounds.y, &bounds.z) );
  // f32 minX, minY, minZ, maxX, maxY, maxZ;
  // errorCheck( fscanf( pFile, "    min { %f %f %f }\n", &minX, &minY, &minZ) );
  // errorCheck( fscanf( pFile, "    max { %f %f %f }\n", &maxX, &maxY, &maxZ) );
  // errorCheck( fscanf( pFile, "  }\n") );
  // bounds.x = (minX+maxX)/4.0f;
  // bounds.y = (minY+maxY)/4.0f;
  // bounds.z = (minZ+maxZ)/4.0f;
  u32 currentIndex;
  for (u32 v = 0; v < numVerts; ++v) {
    errorCheck( fscanf( pFile, "  vert %u {\n", &currentIndex) );
    wsAssert( (currentIndex == v), "Current index does not relate to current vertex.");
    errorCheck( fscanf( pFile, "    pos { %f %f %f }\n", &verts[v].pos.x, &verts[v].pos.y, &verts[v].pos.z) );
    errorCheck( fscanf( pFile, "    norm { %f %f %f }\n", &verts[v].norm.x, &verts[v].norm.y, &verts[v].norm.z) );
    verts[v].pos.w = 1.0f;
    verts[v].norm.w = 1.0f;
    errorCheck( fscanf( pFile, "    tex { %f %f }\n", &verts[v].tex[0], &verts[v].tex[1]) );
    errorCheck( fscanf( pFile, "    weights {\n") );
    errorCheck( fscanf( pFile, "      numWeights %u\n", &verts[v].numWeights) );
    for (i32 w = 0; w < verts[v].numWeights; ++w) {
      if (w < WS_MAX_JOINT_INFLUENCES) {
        errorCheck( fscanf( pFile, "      joint { %d %f }\n", &verts[v].jointIndex[w], &verts[v].influence[w]) );
      }
      else {
        errorCheck( fscanf( pFile, "      joint { %d %f }\n",
          &verts[v].jointIndex[WS_MAX_JOINT_INFLUENCES-1],
          &verts[v].influence[WS_MAX_JOINT_INFLUENCES-1]) );
      }
    }
    if (verts[v].numWeights > WS_MAX_JOINT_INFLUENCES) { verts[v].numWeights = WS_MAX_JOINT_INFLUENCES; }
    for (i32 w = verts[v].numWeights; w < WS_MAX_JOINT_INFLUENCES; ++w) { //  Initialize the rest to null
      verts[v].jointIndex[w] = 0;
      verts[v].influence[w] = 0.0f;
    }
    errorCheck( fscanf( pFile, "    }\n") );
    errorCheck( fscanf( pFile, "  }\n") );
  }
  errorCheck( fscanf( pFile, "}\n\nmaterials {\n") );
  for (u32 m = 0; m < numMaterials; ++m) {
    wsLog("Loading Mat %u\n", m);
    errorCheck( fscanf( pFile, "  mat %u {\n", &currentIndex) );
    wsAssert( (currentIndex == m), "Current index does not relate to current material.");
    errorCheck( fscanf( pFile, "    name %s\n", nameBuffer) );
    errorCheck( fscanf( pFile, "    shine %u\n", &mats[m].shininess) );
    errorCheck( fscanf( pFile, "    ambient { %f %f %f %f }\n",
            &mats[m].ambient.r,
            &mats[m].ambient.g,
            &mats[m].ambient.b,
            &mats[m].ambient.a ) );
    errorCheck( fscanf( pFile, "    diffuse { %f %f %f %f }\n",
            &mats[m].diffuse.r,
            &mats[m].diffuse.g,
            &mats[m].diffuse.b,
            &mats[m].diffuse.a ) );
    errorCheck( fscanf( pFile, "    specular { %f %f %f %f }\n",
            &mats[m].specular.r,
            &mats[m].specular.g,
            &mats[m].specular.b,
            &mats[m].specular.a ) );
    errorCheck( fscanf( pFile, "    emissive { %f %f %f %f }\n",
            &mats[m].emissive.r,
            &mats[m].emissive.g,
            &mats[m].emissive.b,
            &mats[m].emissive.a ) );
    errorCheck( fscanf( pFile, "    maps {\n" ) );
    u32 mapsBitflag;
    errorCheck( fscanf( pFile, "      bitFlag %u\n", &mapsBitflag) );
    if (mapsBitflag & WS_TEXTURE_MAP_COLOR) {
      errorCheck( fscanf( pFile, "      colorMap %[^\t\n]\n", nameBuffer) );
      char filepath[264] = { "textures/" };
      strcat(filepath, nameBuffer);
      wsRenderer.loadTexture(&mats[m].colorMap, filepath);
    }
    if (mapsBitflag & WS_TEXTURE_MAP_NORMAL) {
      errorCheck( fscanf( pFile, "      normalMap %[^\t\n]\n", nameBuffer) );
      char filepath[264] = { "textures/" };
      strcat(filepath, nameBuffer);
      wsRenderer.loadTexture(&mats[m].normalMap, filepath);
    }
    errorCheck( fscanf( pFile, "    }\n") );
    //  Triangles using this material
    errorCheck( fscanf( pFile, "    numTriangles %u\n", &mats[m].numTriangles) );
    mats[m].tris = wsNewArray( wsTriangle, mats[m].numTriangles );
    errorCheck( fscanf( pFile, "    triangles {\n") );
    for (u32 t = 0; t < mats[m].numTriangles; ++t) {
      errorCheck( fscanf( pFile, "      tri %u {\n", &currentIndex) );
      wsAssert( (currentIndex == t), "Current index does not relate to current triangle.");
      errorCheck( fscanf( pFile,  "        verts {\n"
                                  "          indices { %u %u %u }\n",
                                  &mats[m].tris[t].vertIndices[0],
                                  &mats[m].tris[t].vertIndices[1],
                                  &mats[m].tris[t].vertIndices[2] ) );
      errorCheck( fscanf( pFile, "        }\n") );
      errorCheck( fscanf( pFile, "      }\n") );
    }
    errorCheck( fscanf( pFile, "    }\n" ) );
    errorCheck( fscanf( pFile, "    properties {\n") );
    errorCheck( fscanf( pFile, "      numProperties %u\n", &mats[m].numProperties) );
    if (mats[m].numProperties > 0) {
      mats[m].properties = wsNew(wsHashMap<f32>, wsHashMap<f32>(wsNextPrime(mats[m].numProperties)));
      f32 value = 0.0f;
      u32 propNum = 0;
      for (u32 p = 0; p < mats[m].numProperties; ++p) {
        errorCheck( fscanf( pFile, "    property %u {\n", &propNum) );
        wsAssert( (propNum == p), "Current Index does not relate to current property.");
        errorCheck( fscanf( pFile, "      name %[^\t\n]\n", nameBuffer) );
        errorCheck( fscanf( pFile, "    value %f\n", &value) );
        errorCheck( fscanf( pFile, "    }\n") );
        mats[m].properties->insert(wsHash(nameBuffer), value);
      }
    }
    else {
      mats[m].properties = NULL;
    }
    errorCheck( fscanf( pFile, "    }\n") );
    errorCheck( fscanf( pFile, "  }\n") );
    wsLog(WS_LOG_GRAPHICS, "Mesh Loaded.");
  }
  errorCheck( fscanf( pFile, "}\n") );
  if (fclose(pFile) == EOF) {
    wsLog(WS_LOG_ERROR, "Failed to close mesh file");
  }
}

wsMesh::~wsMesh() {
  //  Nothing to do here.
}

void wsMesh::errorCheck(const i32 my) {
  if (my == EOF) {
    wsLog(WS_LOG_ERROR, "Error: premature end of file.");
  }
}


