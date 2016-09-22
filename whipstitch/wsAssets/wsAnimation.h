/*
 * wsAnimation.h
 *
 *  Created on: Nov 25, 2012
 *      Author: dsnettleton
 *
 *      This file declares the class wsAnimation, which extends the abstract base
 *      type wsAsset. A wsAnimation is only part of the more complete type,
 *      wsModel, though the same animation can be applied to multiple wsMesh objects.
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

#ifndef WS_ANIMATION_H_
#define WS_ANIMATION_H_

#include "../wsConfig.h"
#include "wsAsset.h"

struct wsAnimJoint {
    char name[127];
    i32 parent;
    vec4 start;
    quat rot;
};

struct wsJointMod {
    u32 jointIndex;
    vec4 location;
    quat rotation;
};

struct wsKeyframe {
    f32 frameIndex;
    u32 numJointsModified;
    wsJointMod* mods;
};

class wsAnimation: public wsAsset {
    private:
        char name[256];
        vec4 bounds;
        wsAnimJoint* joints;
        wsKeyframe* keyframes;
        u32 animType;
        u32 numJoints;
        u32 numKeyframes;
        f32 framesPerSecond;
        t32 animLength;
    public:
        //  Constructor
        wsAnimation(const char* filepath);
        ~wsAnimation();
        //  Getters
        const t32 getAnimLength() { return animLength; }
        const vec4& getBounds() { return bounds; }
        const f32 getFramesPerSecond() { return framesPerSecond; }
        const wsAnimJoint* getJoints() { return joints; }
        const wsKeyframe* getKeyframes() { return keyframes; }
        const u32 getLength() const { return keyframes[numKeyframes-1].frameIndex; }
        const char* getName() const { return name; }
        const u32 getNumJoints() const { return numJoints; }
        const u32 getNumKeyframes() const { return numKeyframes; }
        //  Operational Methods
        void errorCheck(const i32 my);
};

#endif

