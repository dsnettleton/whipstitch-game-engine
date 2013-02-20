/*
 * wsModel.cpp
 *
 *  Created on: Oct 2, 2012
 *    Author: dsnettleton
 *
 *    This file defines the class wsModel, which extends the abstract base
 *    type wsAsset. A wsModel is a complete type used for combining various
 *    assets such as meshes and animations into a cohesive whole.
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

#include "wsModel.h"
#include "../wsGraphics/wsRenderSystem.h"
#include "../wsGameFlow/wsThreadPool.h"

wsModel::wsModel(const char* filepath) {
  //Function stub
  assetType = WS_ASSET_TYPE_MODEL;
  mesh = NULL;
  defaultAnimation = 0;
  animTime = 0.0;
  currentAnimation = NULL;
  animations = wsNew(wsHashMap<wsAnimation*>, wsHashMap<wsAnimation*>(WS_DEFAULT_MAX_ANIMATIONS));
  looping = false;
  timeScale = 1.0f;
  animPaused = false;
}

wsModel::wsModel(const char* myName, wsMesh* myMesh, const u32 myMaxAnimations = WS_DEFAULT_MAX_ANIMATIONS) {
  assetType = WS_ASSET_TYPE_MODEL;
  mesh = myMesh;
  name = myName;
  defaultAnimation = 0;
  animTime = 0.0;
  currentAnimation = NULL;
  if (myMaxAnimations) {
    animations = wsNew(wsHashMap<wsAnimation*>, wsHashMap<wsAnimation*>(wsNextPrime(myMaxAnimations)));
  }
  else {
    animations = NULL;
  }
  attachment = WS_NULL;
  looping = false;
  timeScale = 1.0f;
  animPaused = false;
  //  Initialize Drawing variables
  const wsMaterial* mats = myMesh->getMats();
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    numIndexArrays = myMesh->getNumMaterials();
    indexArrays = wsNewArray(wsIndexArray, numIndexArrays);
    glGenBuffers(1, &vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wsVert)*myMesh->getNumVerts(), myMesh->getVerts(), GL_DYNAMIC_DRAW);
    //  Unbind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //  Generate vertex buffer objects for this mesh
    for (u32 i = 0; i < numIndexArrays; ++i) {
      const wsTriangle* tris = mats[i].tris;
      //  Create index arrays
      indexArrays[i].numIndices = mats[i].numTriangles*3;
      indexArrays[i].indices = wsNewArray(u32, indexArrays[i].numIndices);
      for (u32 t = 0; t < mats[i].numTriangles; ++t) {
        indexArrays[i].indices[t*3] = tris[t].vertIndices[0];
        indexArrays[i].indices[t*3+1] = tris[t].vertIndices[1];
        indexArrays[i].indices[t*3+2] = tris[t].vertIndices[2];
      }
      //  Generate opengl index arrays
      glGenBuffers(1, &indexArrays[i].handle);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArrays[i].handle);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32)*indexArrays[i].numIndices, indexArrays[i].indices,
              (myMaxAnimations) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
  #endif
}

wsModel::~wsModel() {
  //  Nothing to see here...
}

void wsModel::addAnimation(wsAnimation* anim) {
  wsAssert( (anim != NULL), "Cannot add a null animation to the model.");
  wsLog("Adding animation %s to model %s - animHash = %u\n", anim->getName(), name, wsHash(anim->getName()));
  animations->insert(wsHash(anim->getName()), anim);
}

void wsModel::applyAnimation() {
  if (currentAnimation == NULL) { return; }   //  Error checking
  if (mesh->getNumJoints() == 0) { return; }
  const wsKeyframe* frames = currentAnimation->getKeyframes();
  while (animTime > currentAnimation->getAnimLength()) {
    animTime -= currentAnimation->getAnimLength();
  }
  u32 numKeyframes = currentAnimation->getNumKeyframes();
  f32 fps = currentAnimation->getFramesPerSecond();
  f32 frameNum = animTime * fps;
  u32 prevKeyframe = 0;
  u32 nextKeyframe = 0;
  //  Compute the next and previous keyframes for the current animation
  for (u32 i = 0; i < numKeyframes; ++i) {
    if (frames[i].frameIndex > frameNum) {
      nextKeyframe = i;
      if (i) {  //  If i != 0
        prevKeyframe = i-1;
      }
      break;
    }
  }
  f32 blendFactor = wsBlendFactor(frames[prevKeyframe].frameIndex, frameNum, frames[nextKeyframe].frameIndex);
  wsLog(WS_LOG_GRAPHICS, "animation keyframe = %f, blendFactor = %f\n", frameNum, blendFactor);
  wsJoint* joints = (wsJoint*)mesh->getJoints();
  //  Place this on the frame stack (it will be cleared next frame)
  wsJointMod* mods = wsNewArrayTmp(wsJointMod, mesh->getNumJoints());
  for (u32 i = 0; i < mesh->getNumJoints(); ++i) {
    mods[i].location = frames[prevKeyframe].mods[i].location.blend(frames[nextKeyframe].mods[i].location,
            blendFactor);
    mods[i].rotation = frames[prevKeyframe].mods[i].rotation.blend(frames[nextKeyframe].mods[i].rotation,
            blendFactor);
  }
  //  Applying animation
  for (u32 i = 0; i < mesh->getNumJoints(); ++i) {
    joints[i].rot = mods[i].rotation;
    joints[i].start = joints[i].startRel;
    if (joints[i].parent >= 0) {
      joints[i].start.rotate(joints[joints[i].parent].rot);
      joints[i].start += joints[joints[i].parent].start;
    }
    joints[i].start += mods[i].location;
  }
  quat rot;
  vec4 pos;
  vec4 norm;
  vec4 posSum;
  vec4 normSum;
  wsVert* verts = (wsVert*)mesh->getVerts();
  //  Animate Vertices
  for (u32 v = 0; v < mesh->getNumVerts(); ++v) {
    posSum.set(0, 0, 0, 1);
    normSum.set(0, 0, 0, 1);
    for (u32 w = 0; w < verts[v].numWeights; ++w) {
      pos = verts[v].originalPos - mesh->getBaseSkel()[verts[v].weights[w].jointIndex].start;
      pos.rotate(mesh->getBaseSkel()[verts[v].weights[w].jointIndex].rot.getInverse());
      pos.rotate(joints[verts[v].weights[w].jointIndex].rot);
      pos += joints[verts[v].weights[w].jointIndex].start;
      posSum += pos * verts[v].weights[w].influence;
      //  Handle Normals
      norm = verts[v].originalNorm;
      norm.rotate(mesh->getBaseSkel()[verts[v].weights[w].jointIndex].rot.getInverse());
      norm.rotate(joints[verts[v].weights[w].jointIndex].rot);
      normSum += norm;
    }
    verts[v].pos = posSum;
    verts[v].norm = normSum / verts[v].numWeights;
  }
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glBindBuffer(GL_ARRAY_BUFFER, vertexArray);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(wsVert)*mesh->getNumVerts(), mesh->getVerts());
  #endif
}

void wsModel::attachModel(wsModel* myModel, const char* jointName) {
  wsAssert(myModel != NULL, "Cannot attach a null model");
  //u32 nameHash = wsHash(jointName);
  myModel->attachment = (wsJoint*)mesh->getJoint(jointName);
}

void wsModel::beginAnimation(const char* animName) {
  currentAnimation = animations->retrieve(wsHash(animName));
  animTime = 0.0;
  applyAnimation();
}

void wsModel::continueAnimation() {
  wsLog("continuing animation");
  animPaused = false;
}

void wsModel::incrementAnimationTime(t32 increment) {
  if (animPaused) { return; }
  increment *= timeScale;
  animTime += increment;
  applyAnimation();
}

void wsModel::pauseAnimation() {
  wsLog("pausing animation");
  animPaused = true;
}

void wsModel::setFrame(f32 newFrame) {
  wsAssert( (currentAnimation != NULL), "Cannot set frame for NULL animation.");
  while (newFrame > currentAnimation->getLength()) {
    newFrame -= currentAnimation->getLength();
  }
  animTime = (t64)newFrame / (t64)currentAnimation->getFramesPerSecond();
}
