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

#include "wsModel.h"
#include "../wsGraphics/wsRenderSystem.h"
#include "../wsGameFlow/wsThreadPool.h"

wsModel::wsModel(const char* myName, wsMesh* myMesh, const u32 myMaxAnimations, const f32 myMass,
    const u64 myCollisionClass, const u16 myProperties, wsCollisionShape* myCollisionShape) {
  assetType = WS_ASSET_TYPE_MODEL;
  mesh = myMesh;
  name = myName;
  bounds = *mesh->getBounds();
  defaultAnimation = 0;
  collisionClass = myCollisionClass;
  animTime = 0.0;
  currentAnimation = NULL;
  properties = myProperties;
  if (myMaxAnimations) {
    animations = wsNew(wsHashMap<wsAnimation*>, wsHashMap<wsAnimation*>(wsNextPrime(myMaxAnimations)));
  }
  else {
    animations = NULL;
  }
  jointLocations = wsNewArray(vec4, myMesh->getNumJoints());
  jointRotations = wsNewArray(quat, myMesh->getNumJoints());
  if (myMesh->getNumJoints()) {
    applyStaticAnimation();
  }
  attachmentModel = WS_NULL;
  attachmentTransform = WS_NULL;
  attachmentLoc = WS_NULL;
  attachmentRot = WS_NULL;
  timeScale = 1.0f;
  mass = myMass;
  collisionShape = myCollisionShape;
  transform.setTranslation(myMesh->getDefaultPos());
  //  Initialize Drawing variables
  const wsMaterial* mats = myMesh->getMats();
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    numIndexArrays = myMesh->getNumMaterials();
    indexArrays = wsNewArray(wsIndexArray, numIndexArrays);
    glGenBuffers(1, &vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wsVert)*myMesh->getNumVerts(), myMesh->getVerts(), GL_STATIC_DRAW);
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
  wsEcho("Adding animation %s to model %s - animHash = %u\n", anim->getName(), name, wsHash(anim->getName()));
  animations->insert(wsHash(anim->getName()), anim);
}

void wsModel::applyAnimation() {
  if (mesh->getNumJoints() == 0) { return; }  //  Error checking
  if (currentAnimation == NULL) {
    applyStaticAnimation();
    return;
  }
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
  wsEcho(WS_LOG_GRAPHICS, "animation keyframe = %f, blendFactor = %f\n", frameNum, blendFactor);
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
    jointRotations[i] = joints[i].rot;
    jointLocations[i] = joints[i].start;
  }
}

void wsModel::applyStaticAnimation() {
  if (mesh->getNumJoints() == 0) { return; }  //  Error checking
  wsEcho(WS_LOG_GRAPHICS, "Applying Static Animation");
  vec4* meshJointLocations = (vec4*)mesh->getJointLocations();
  quat* meshJointRotations = (quat*)mesh->getJointRotations();
  for (u32 i = 0; i < mesh->getNumJoints(); ++i) {
    jointLocations[i] = meshJointLocations[i];
    jointRotations[i] = meshJointRotations[i];
  }
}

void wsModel::attachModel(wsModel* myModel, const char* jointName) {
  wsAssert(myModel != NULL, "Cannot attach a null model");
  u32 index = mesh->getJointIndex(jointName);
  myModel->attachmentModel = this;
  myModel->attachmentTransform = &transform;
  myModel->attachmentLoc = &jointLocations[index];
  myModel->attachmentRot = &jointRotations[index];
  myModel->properties &= WS_MODEL_ATTACHED;
}

void wsModel::beginAnimation(const char* animName) {
  currentAnimation = animations->retrieve(wsHash(animName));
  animTime = 0.0;
  //  Update bounding box
  bounds = currentAnimation->getBounds();
  applyAnimation();
}

void wsModel::continueAnimation() {
  wsEcho("continuing animation");
  u16 animPaused = WS_MODEL_ANIM_PAUSED;
  animPaused ^= properties;
  properties &= animPaused;
}

void wsModel::incrementAnimationTime(t32 increment) {
  if (properties & WS_MODEL_ANIM_PAUSED) { return; }
  increment *= timeScale;
  animTime += increment;
  applyAnimation();
}

void wsModel::pauseAnimation() {
  wsEcho("pausing animation");
  properties &= WS_MODEL_ANIM_PAUSED;
}

void wsModel::setFrame(f32 newFrame) {
  wsAssert( (currentAnimation != NULL), "Cannot set frame for NULL animation.");
  while (newFrame > currentAnimation->getLength()) {
    newFrame -= currentAnimation->getLength();
  }
  animTime = (t64)newFrame / (t64)currentAnimation->getFramesPerSecond();
}
