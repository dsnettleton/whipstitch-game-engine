/**
 *  wsScene.cpp
 *  Mar 3, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the class wsScene, which contains all the models
 *  and objects necessary to define a scene in the Whipstitch Game Engine.
 *  A scene can be an entire level, and contains cameras, models (entities),
 *  lights, terrain, and static world blocks.
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
#include "wsScene.h"

wsScene::wsScene() {
  cameras = wsNew(wsHashMap<wsCamera*>, wsHashMap<wsCamera*>(101));
  models = wsNew(wsHashMap<wsModel*>, wsHashMap<wsModel*>(101));
}

void wsScene::addAnimation(wsAnimation* myAnim, const char* modelName) {
  models->retrieve(wsHash(modelName))->addAnimation(myAnim);
}

u32 wsScene::addCamera(wsCamera* myCam) {
  u32 camHash = wsHash(myCam->getName());
  if (cameras->insert(camHash, myCam) == WS_SUCCESS) {
    return camHash;
  }
  return WS_NULL;
}

u32 wsScene::addModel(const char* filepath) {
  wsModel* myModel = wsNew(wsModel, wsModel(filepath));
  u32 modelHash = wsHash(filepath);
  if (models->insert(modelHash, myModel) == WS_SUCCESS) {
    return modelHash;
  }
  return WS_NULL;
}// End addModel(filepath) method

u32 wsScene::addModel(const char* modelName, wsMesh* myMesh, const u32 maxAnimations) {
  wsModel* myModel = wsNew(wsModel, wsModel(modelName, myMesh, maxAnimations));
  u32 modelHash = wsHash(myModel->getName());
  if (models->insert(modelHash, myModel) == WS_SUCCESS) {
    return modelHash;
  }
  return WS_NULL;
}// End addModel(mesh, maxAnimations) method

u32 wsScene::addModel(wsModel* myModel) {
  u32 modelHash = wsHash(myModel->getName());
  if (models->insert(modelHash, myModel) == WS_SUCCESS) {
    return modelHash;
  }
  return WS_NULL;
}// End addModel(wsModel*) method

void wsScene::beginAnimation(const char* modelName, const char* animName) {
  wsLog(WS_LOG_GRAPHICS, "Model %s - Beginning Animation: %s\n", modelName, animName);
  models->retrieve(wsHash(modelName))->beginAnimation(animName);
}

void wsScene::continueAnimation(const char* modelName) {
  models->retrieve(wsHash(modelName))->continueAnimation();
}

void wsScene::continueAnimations() {
  wsHashMap<wsModel*>::iterator it = models->begin();
  for (u32 i = 0; i < models->getLength(); ++i) {
    models->getArrayItem(it.mCurrentElement)->continueAnimation();
    ++it;
  }
}

void wsScene::pauseAnimation(const char* modelName) {
  models->retrieve(wsHash(modelName))->pauseAnimation();
}

void wsScene::pauseAnimations() {
  wsHashMap<wsModel*>::iterator it = models->begin();
  for (u32 i = 0; i < models->getLength(); ++i) {
    models->getArrayItem(it.mCurrentElement)->pauseAnimation();
    ++it;
  }
}

void wsScene::setCameraMode(const char* cameraName, u32 cameraMode) {
  cameras->retrieve(wsHash(cameraName))->setCameraMode(cameraMode);
}

void wsScene::setPos(const char* modelName, const vec4& pos) {
  models->retrieve(wsHash(modelName))->setPos(pos);
}

void wsScene::setRotation(const char* modelName, const quat& rot) {
  models->retrieve(wsHash(modelName))->setRotation(rot);
}

void wsScene::setScale(const char* modelName, const f32 scale) {
  models->retrieve(wsHash(modelName))->setScale(scale);
}

void wsScene::updateAnimation(const char* modelName, t32 increment) {
  models->retrieve(wsHash(modelName))->incrementAnimationTime(increment);
}

void wsScene::updateAnimations(t32 increment) {
  wsHashMap<wsModel*>::iterator it = models->begin();
  for (u32 i = 0; i < models->getLength(); ++i) {
    models->getArrayItem(it.mCurrentElement)->incrementAnimationTime(increment);
    ++it;
  }
}
