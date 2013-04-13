/**
 *  wsScene.h
 *  Mar 3, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsScene, which contains all the models
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
#ifndef WS_SCENE_H_
#define WS_SCENE_H_

#include "../wsAssets.h"
#include "../wsGraphics/wsCamera.h"
#include "../wsPrimitives.h"

#define WS_BACKEND_BULLET     0 //  Bullet physics engine

#ifndef WS_PHYSICS_BACKEND
 #define WS_PHYSICS_BACKEND WS_BACKEND_BULLET
#endif

#if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
  #include "btBulletDynamicsCommon.h"
  #include "btBulletCollisionCommon.h"
#endif

#ifndef WS_MAX_MODELS
  #define WS_MAX_MODELS 101
#endif

#ifndef WS_MAX_CAMERAS
  #define WS_MAX_CAMERAS 17
#endif

#ifndef WS_MAX_PRIMITIVES
  #define WS_MAX_PRIMITIVES 101
#endif

class wsScene {
  private:
    //  Private Data Members
    vec4 gravity;
    wsHashMap<wsCamera*>* cameras;
    wsHashMap<wsModel*>* models;
    wsPrimitive** primitives;
    #if WS_PHYSICS_BACKEND == WS_BACKEND_BULLET
      wsHashMap<btRigidBody*>* rigidBodies;
      btBroadphaseInterface* broadphase;  //  Eliminates object pairs that should not collide
      btDefaultCollisionConfiguration* collisionConfig; //  fine-tunes collision algorithms
      btCollisionDispatcher* dispatcher;
      btSequentialImpulseConstraintSolver* solver;  //  solves physics interface
      btDiscreteDynamicsWorld* physicsWorld;
    #endif
    u32 numPrimitives;
  public:
    //  Constructors and Deconstructors
    wsScene(vec4 myGravity = vec4(0.0f, 0.0f, 0.0f, 0.0f));
    //  Setters and Getters
    wsCamera* getCamera(const char* cameraName) { return cameras->retrieve(wsHash(cameraName)); }
    wsHashMap<wsCamera*>* getCameras() { return cameras; }
    wsModel* getModel(const char* modelName) { return models->retrieve(wsHash(modelName)); }
    wsHashMap<wsModel*>* getModels() { return models; }
    u32 getNumPrimitives() { return numPrimitives; }
    wsPrimitive** getPrimitives() { return primitives; }
    //  Operational Methods
    void addAnimation(wsAnimation* myAnim, const char* modelName);
    u32 addCamera(wsCamera* myCam);
    u32 addModel(const char* filepath); //  Adds a 3D model and returns its index
    u32 addModel(const char* modelName, wsMesh* myMesh, u32 maxAnimations = 7);
    u32 addModel(wsModel* myModel);
    u32 addPrimitive(wsPrimitive* myPrimitive);
    void beginAnimation(const char* modelName, const char* animName);
    void continueAnimation(const char* modelName);
    void continueAnimations();
    void pauseAnimation(const char* modelName);
    void pauseAnimations();
    void setCameraMode(const char* cameraName, u32 cameraMode);
    void setPos(const char* modelName, const vec4& pos);
    void setRotation(const char* modelName, const quat& rot);
    void setScale(const char* modelName, const f32 scale);
    void updateAnimation(const char* modelName, t32 increment);
    void updateAnimations(t32 increment);
    void updatePhysics(const t32 increment);
};

#endif //  WS_SCENE_H_
