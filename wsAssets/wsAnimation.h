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
 
#ifndef WS_ANIMATION_H_
#define WS_ANIMATION_H_

#include "wsAsset.h"

#define WS_ANIM_TYPE_KEYFRAME 1

#define WS_MAX_JOINTS 128

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
        char name[255];
        u32 animType;
        wsAnimJoint* joints;
        wsKeyframe* keyframes;
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

