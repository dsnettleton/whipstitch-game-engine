/*
 * wsModel.h
 *
 *  Created on: Oct 2, 2012
 *    Author: dsnettleton
 *
 *    This file declares the class wsModel, which extends the abstract base
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

#ifndef WSMODEL_H_
#define WSMODEL_H_

#include "wsMesh.h"
#include "wsAnimation.h"
#include "../wsUtils.h"

#define WS_DEFAULT_MAX_ANIMATIONS   11

class wsModel: public wsAsset {
  private:
    wsMesh* mesh;
    const char* name;
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      wsIndexArray* indexArrays;
      u32 numIndexArrays;
      u32 vertexArray;
    #endif
    wsHashMap<wsAnimation*> *animations;
    wsAnimation* currentAnimation;
    vec4* jointLocations;
    quat* jointRotations;
    vec4* attachmentLoc;  //  Joint, if any, that this model is attached to
    quat* attachmentRot;  //  Joint, if any, that this model is attached to
    vec4 bounds; //  Model's current bounding box
    wsTransform transform;  //  Position, direction, and scale
    f32 mass;
    t64 animTime;
    f32 timeScale;
    u16 defaultAnimation;
    bool looping;
    bool animPaused;
  public:
    /// Constructor and Destructor
    wsModel(const char* filepath);
    wsModel(const char* myName, wsMesh* myMesh, const u32 myMaxAnimations = WS_DEFAULT_MAX_ANIMATIONS, const f32 myMass = 0.0f);
    ~wsModel();
    /// Setters and Getters
    vec4* getAttachmentLoc() { return attachmentLoc; }
    quat* getAttachmentRot() { return attachmentRot; }
    const vec4& getBounds() { return bounds; }
    wsIndexArray* getIndexArrays() { return indexArrays; }
    vec4* getJointLocations() { return jointLocations; }
    quat* getJointRotations() { return jointRotations; }
    f32 getMass() { return mass; }
    u16 getMaxAnimations() { return animations->getMaxElements(); }
    wsMesh* getMesh() { return mesh; }
    const char* getName() { return name; }
    wsAnimation* getCurrentAnimation() { return currentAnimation; }
    const char* getCurrentAnimationName() { if (currentAnimation == NULL) { return ""; } return currentAnimation->getName(); }
    u16 getNumAnimations() { return animations->getLength(); }
    u32 getNumIndexArrays() { return numIndexArrays; }
    u32 getNumJoints() { return mesh->getNumJoints(); }
    const vec4 getPos() { return vec4(transform.translationX, transform.translationY, transform.translationZ); }
    const quat& getRot() { return transform.rotation; }
    f32 getTimeScale() { return timeScale; }
    const wsTransform& getTransform() { return transform; }
    u32 getVertexArray() { return vertexArray; }
    void setFrame(f32 newFrame);
    void setMesh(wsMesh* my) { mesh = my; }
    void setTimeScale(f32 my) { timeScale = my; }
    void setScale(f32 my) { transform.scale = my; }
    void setPos(const vec4& my) { transform.setTranslation(my); }
    void setPos(const f32 myX, const f32 myY, const f32 myZ) { transform.setTranslation(myX, myY, myZ); }
    void setRotation(const quat& my) { transform.rotation = my; }
    void setRotation(const f32 myX, const f32 myY, const f32 myZ, const f32 myW) { transform.setRotation(myX, myY, myZ, myW); }
    /// Operational Methods
    void addAnimation(wsAnimation* anim);
    void applyAnimation();  //  Applies the current animation to the mesh vertices
    void attachModel(wsModel* myModel, const char* jointName);
    void beginAnimation(const char* animName);
    void continueAnimation();// Continues a paused animation
    void draw();
    void incrementAnimationTime(t32 increment);
    void pauseAnimation();
    void updateVbo();
};

#endif /* WSMODEL_H_ */
