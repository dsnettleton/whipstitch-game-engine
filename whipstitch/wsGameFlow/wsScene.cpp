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

wsScene::wsScene(vec4 myGravity) {
  gravity = myGravity;
  cameras = wsNew(wsHashMap<wsCamera*>, wsHashMap<wsCamera*>(WS_MAX_CAMERAS));
  models = wsNew(wsHashMap<wsModel*>, wsHashMap<wsModel*>(WS_MAX_MODELS));
  primitives = wsNewArray(wsPrimitive*, WS_MAX_PRIMITIVES);
  //  Instantiate physics engine
  #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
    rigidBodies = wsNew(wsHashMap<btRigidBody*>, wsHashMap<btRigidBody*>(WS_MAX_MODELS));
    broadphase = wsNew(btDbvtBroadphase,  btDbvtBroadphase());
    collisionConfig = wsNew(btDefaultCollisionConfiguration, btDefaultCollisionConfiguration());
    dispatcher = wsNew(btCollisionDispatcher, btCollisionDispatcher(collisionConfig));
    solver = wsNew(btSequentialImpulseConstraintSolver, btSequentialImpulseConstraintSolver());
    physicsWorld = wsNew(btDiscreteDynamicsWorld, btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig));
    physicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
  #endif
  numPrimitives = 0;
}

void wsScene::addAnimation(wsAnimation* myAnim, const char* modelName) {
  wsModel* myModel = models->retrieve(wsHash(modelName));
  myModel->addAnimation(myAnim);
  #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
    char animName[512];
    strcpy(animName, modelName);
    strcat(animName, myAnim->getName());
    u32 boundsHash = wsHash(animName);
    const vec4 dimensions = myAnim->getBounds();
    btCollisionShape* boundsShape;
    if (myModel->getCollisionShape() == WS_NULL) {
      boundsShape = wsNew(btBoxShape, btBoxShape(btVector3(dimensions.x, dimensions.y, dimensions.z)));
    }
    else {
      wsCollisionShape* shape = myModel->getCollisionShape();
      switch (shape->getType()) {
        default:
          boundsShape = wsNew(btBoxShape, btBoxShape(btVector3(dimensions.x, dimensions.y, dimensions.z)));
          break;
        case WS_SHAPE_CAPSULE:
          boundsShape = wsNew(btCapsuleShape, btCapsuleShape(shape->getDim0(), shape->getDim1()));
          break;
        case WS_SHAPE_CUBE:
          boundsShape = wsNew(btBoxShape, btBoxShape(btVector3(shape->getDim0(), shape->getDim1(), shape->getDim2())));
          break;
        case WS_SHAPE_CYLINDER:
          boundsShape = wsNew(btCylinderShape,
            btCylinderShape(btVector3(shape->getDim0(), shape->getDim1()/2.0f, shape->getDim0())));
          break;
        case WS_SHAPE_SPHERE:
          boundsShape = wsNew(btSphereShape, btSphereShape(shape->getDim0()));
          break;
      }
    }
    f32 mass = myModel->getMass();
    btVector3 myInertia(0.0f, 0.0f, 0.0f);
    boundsShape->calculateLocalInertia(mass, myInertia);
    btRigidBody::btRigidBodyConstructionInfo animRigidBodyCI(mass, myModel->getTransformp(), boundsShape, myInertia);
    btRigidBody* animRigidBody = wsNew(btRigidBody, btRigidBody(animRigidBodyCI));
    if (myModel->getProperties() & WS_MODEL_LOCK_HORIZ_ROTATIONS) {
      animRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
    }
    rigidBodies->insert(boundsHash, animRigidBody);
  #endif
}// End addAnimation method

u32 wsScene::addCamera(wsCamera* myCam) {
  u32 camHash = wsHash(myCam->getName());
  if (cameras->insert(camHash, myCam) == WS_SUCCESS) {
    return camHash;
  }
  return WS_NULL;
}// End addCamera method

u32 wsScene::addModel(wsModel* myModel) {
  u32 modelHash = wsHash(myModel->getName());
  if (models->insert(modelHash, myModel) == WS_SUCCESS) {
    #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
      const vec4 dimensions = myModel->getBounds();
      btCollisionShape* boundsShape;
      if (myModel->getCollisionShape() == WS_NULL) {
        boundsShape = wsNew(btBoxShape, btBoxShape(btVector3(dimensions.x, dimensions.y, dimensions.z)));
      }
      else {
        wsCollisionShape* shape = myModel->getCollisionShape();
        switch (shape->getType()) {
          default:
            boundsShape = wsNew(btBoxShape, btBoxShape(btVector3(dimensions.x, dimensions.y, dimensions.z)));
            break;
          case WS_SHAPE_CAPSULE:
            boundsShape = wsNew(btCapsuleShape, btCapsuleShape(shape->getDim0(), shape->getDim1()));
            break;
          case WS_SHAPE_CUBE:
            boundsShape = wsNew(btBoxShape, btBoxShape(btVector3(shape->getDim0(), shape->getDim1(), shape->getDim2())));
            break;
          case WS_SHAPE_CYLINDER:
            boundsShape = wsNew(btCylinderShape,
              btCylinderShape(btVector3(shape->getDim0(), shape->getDim1()/2.0f, shape->getDim0())));
            break;
          case WS_SHAPE_SPHERE:
            boundsShape = wsNew(btSphereShape, btSphereShape(shape->getDim0()));
            break;
        }
      }
      f32 mass = myModel->getMass();
      btVector3 myInertia(0.0f, 0.0f, 0.0f);
      boundsShape->calculateLocalInertia(mass, myInertia);
      btRigidBody::btRigidBodyConstructionInfo modelRigidBodyCI(mass, myModel->getTransformp(), boundsShape, myInertia);
      btRigidBody* modelRigidBody = wsNew(btRigidBody, btRigidBody(modelRigidBodyCI));
      if (myModel->getProperties() & WS_MODEL_LOCK_HORIZ_ROTATIONS) {
        modelRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
      }
      modelRigidBody->setActivationState(DISABLE_DEACTIVATION);
      rigidBodies->insert(modelHash, modelRigidBody);
      physicsWorld->addRigidBody(modelRigidBody, myModel->getCollisionClass(),
        collisionClasses[(u32)wsLog2(myModel->getCollisionClass())]);
    #endif
    return modelHash;
  }
  return WS_NULL;
}// End addModel(wsModel*) method

u32 wsScene::addPrimitive(wsPrimitive* myPrimitive) {
  wsAssert(numPrimitives < WS_MAX_PRIMITIVES, "Cannot add another primitive to the scene. Maximum already reached.");
  primitives[numPrimitives] = myPrimitive;
  #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
    switch (myPrimitive->getType()) {
      case WS_PRIM_TYPE_PLANE:
        {
          wsPlane* plane = (wsPlane*)myPrimitive;
          vec4 data = plane->getPosData();
          btCollisionShape* planeShape = wsNew(btStaticPlaneShape, btStaticPlaneShape(btVector3(data.x, data.y, data.z), data.w));
          btDefaultMotionState* planeState = wsNew(btDefaultMotionState, btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, 0.0f, 0.0f))));
          btRigidBody::btRigidBodyConstructionInfo planeRigidBodyCI(0, planeState, planeShape, btVector3(0,0,0));
          btRigidBody* groundRigidBody = wsNew(btRigidBody, btRigidBody(planeRigidBodyCI));
          physicsWorld->addRigidBody(groundRigidBody, myPrimitive->getCollisionClass(),
            collisionClasses[(u32)wsLog2(myPrimitive->getCollisionClass())]);
        }
        break;
      case WS_PRIM_TYPE_CUBE:
        {
          wsCube* cube = (wsCube*)myPrimitive;
          vec4 dimensions = cube->getDimensions();
          vec4 pos = cube->getPos();
          quat rot = cube->getRot();
          btCollisionShape* cubeShape = wsNew(btBoxShape, btBoxShape(btVector3(dimensions.x/2.0f, dimensions.y/2.0f, dimensions.z/2.0f)));
          btDefaultMotionState* cubeState = wsNew(btDefaultMotionState, btDefaultMotionState(btTransform(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z))));
          btRigidBody::btRigidBodyConstructionInfo cubeRigidBodyCI(0, cubeState, cubeShape, btVector3(0, 0, 0));
          btRigidBody* cubeRigidBody = wsNew(btRigidBody, btRigidBody(cubeRigidBodyCI));
          physicsWorld->addRigidBody(cubeRigidBody, myPrimitive->getCollisionClass(),
            collisionClasses[(u32)wsLog2(myPrimitive->getCollisionClass())]);
        }
        break;
      default:
        break;
    }
  #endif
  return numPrimitives++;
}

void wsScene::attachModel(const char* modelSource, const char* modelDest, const char* jointName) {
  models->retrieve(wsHash(modelDest))->attachModel(models->retrieve(wsHash(modelSource)), jointName);
}

void wsScene::beginAnimation(const char* modelName, const char* animName) {
  wsEcho(WS_LOG_GRAPHICS, "Model %s - Beginning Animation: %s\n", modelName, animName);
  wsModel* myModel = models->retrieve(wsHash(modelName));
  #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
    char myAnim[512];
    if (myModel->getCurrentAnimation() == NULL) {
      physicsWorld->removeRigidBody(rigidBodies->retrieve(wsHash(modelName)));
    }
    else {
      strcpy(myAnim, modelName);
      strcat(myAnim, myModel->getCurrentAnimationName());
      physicsWorld->removeRigidBody(rigidBodies->retrieve(wsHash(myAnim)));
    }
    strcpy(myAnim, modelName);
    strcat(myAnim, animName);
    btRigidBody* myRigidBody = rigidBodies->retrieve(wsHash(myAnim));
    myRigidBody->setMotionState(myModel->getTransformp());
    // myRigidBody->setCollisionFlags(myRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    myRigidBody->setActivationState(DISABLE_DEACTIVATION);
    physicsWorld->addRigidBody(myRigidBody, myModel->getCollisionClass(),
      collisionClasses[(u32)wsLog2(myModel->getCollisionClass())]);
  #endif
  myModel->beginAnimation(animName);
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

void wsScene::moveModel(const char* modelName, const vec4& dist) {
  wsModel* myModel = models->retrieve(wsHash(modelName));
  if (myModel == WS_NULL) { return; }
  myModel->move(dist);
  #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
    btRigidBody* myRigidBody;
    char myAnim[512];
    if (myModel->getCurrentAnimation() == NULL) {
      myRigidBody = rigidBodies->retrieve(wsHash(modelName));
    }
    else {
      strcpy(myAnim, modelName);
      strcat(myAnim, myModel->getCurrentAnimationName());
      myRigidBody = rigidBodies->retrieve(wsHash(myAnim));
    }
    if (myRigidBody == WS_NULL) { return; }
    myRigidBody->setMotionState(myModel->getTransformp());
  #endif
}

vec4 wsScene::moveModelBackward(const char* modelName, const f32 dist) {
  wsModel* myModel = models->retrieve(wsHash(modelName));
  if (myModel == WS_NULL) { return vec4(); }
  vec4 myTranslation = myModel->moveBackward(dist);
  #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
    btRigidBody* myRigidBody;
    char myAnim[512];
    if (myModel->getCurrentAnimation() == NULL) {
      myRigidBody = rigidBodies->retrieve(wsHash(modelName));
    }
    else {
      strcpy(myAnim, modelName);
      strcat(myAnim, myModel->getCurrentAnimationName());
      myRigidBody = rigidBodies->retrieve(wsHash(myAnim));
    }
    if (myRigidBody == WS_NULL) { return vec4(); }
    myRigidBody->setMotionState(myModel->getTransformp());
  #endif
  return myTranslation;
}

vec4 wsScene::moveModelForward(const char* modelName, const f32 dist) {
  wsModel* myModel = models->retrieve(wsHash(modelName));
  if (myModel == WS_NULL) { return vec4(); }
  vec4 myTranslation = myModel->moveForward(dist);
  #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
    btRigidBody* myRigidBody;
    char myAnim[512];
    if (myModel->getCurrentAnimation() == NULL) {
      myRigidBody = rigidBodies->retrieve(wsHash(modelName));
    }
    else {
      strcpy(myAnim, modelName);
      strcat(myAnim, myModel->getCurrentAnimationName());
      myRigidBody = rigidBodies->retrieve(wsHash(myAnim));
    }
    if (myRigidBody == WS_NULL) { return vec4(); }
    myRigidBody->setMotionState(myModel->getTransformp());
  #endif
  return myTranslation;
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

void wsScene::rotateModel(const char* modelName, const vec4& axis, const f32 angle) {
  wsModel* myModel = models->retrieve(wsHash(modelName));
  if (myModel == WS_NULL) { return; }
  myModel->rotate(axis, angle);
  #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
    btRigidBody* myRigidBody;
    char myAnim[512];
    if (myModel->getCurrentAnimation() == NULL) {
      myRigidBody = rigidBodies->retrieve(wsHash(modelName));
    }
    else {
      strcpy(myAnim, modelName);
      strcat(myAnim, myModel->getCurrentAnimationName());
      myRigidBody = rigidBodies->retrieve(wsHash(myAnim));
    }
    if (myRigidBody == WS_NULL) { return; }
    myRigidBody->setMotionState(myModel->getTransformp());
  #endif
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

void wsScene::updatePhysics(t32 increment) {
  #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
  //*
    physicsWorld->stepSimulation(increment, 3);
    /*
    btTransform transform;
    wsModel* myModel;
    btRigidBody* myBody;
    for (wsHashMap<wsModel*>::iterator mod = models->begin(); mod.get() != WS_NULL; ++mod) {
      myModel = models->getArrayItem(mod.mCurrentElement);
      myBody = rigidBodies->getArrayItem(mod.mCurrentElement);
      myBody->getMotionState()->getWorldTransform(transform);
      myModel->setPos(transform.getOrigin().getX(), transform.getOrigin().getY(), transform.getOrigin().getZ());
      myModel->setRotation(transform.getRotation().getX(), transform.getRotation().getY(),
        transform.getRotation().getZ(), transform.getRotation().getW());
    }
  //*/
  #endif
}
