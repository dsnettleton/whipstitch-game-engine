/*
 * wsAnimation.cpp
 *
 *  Created on: Nov 25, 2012
 *      Author: dsnettleton
 *
 *      This file defines the class wsAnimation, which extends the abstract base
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
 
#include "wsAnimation.h"
#include <stdio.h>
#include "../wsGraphics.h"

wsAnimation::wsAnimation(const char* filepath) {
    assetType = WS_ASSET_TYPE_ANIM;
    wsLog(WS_LOG_GRAPHICS, "Loading Animation from file \"%s\"\n", filepath);
    FILE* pFile;
    wsAssert( (pFile = fopen(filepath, "r")), "Error Loading file." );
    //  Scan past the header
    errorCheck( fscanf( pFile,  "//  Whipstitch Animation File\n//  This Animation is for use with the Whipstitch Game Engine\n"
                                "//  For more information, email dsnettleton@whipstitchgames.com\n\n") );
    f32 versionNumber;
    errorCheck( fscanf( pFile, "versionNumber %f\n", &versionNumber) );
    errorCheck( fscanf( pFile, "animationType %u\n", &animType) );
    char nameBuffer[255];
    errorCheck( fscanf( pFile, "animationName %[^\t\n]\n", nameBuffer) );
    errorCheck( fscanf( pFile, "framesPerSecond %f\n\n", &framesPerSecond) );
    errorCheck( fscanf( pFile, "numJoints %u\n", &numJoints) );
    errorCheck( fscanf( pFile, "numKeyFrames %u\n\n", &numKeyframes) );
    //  Generate object arrays and place them on the current stack
    joints = wsNewArray(wsAnimJoint, numJoints);
    keyframes = wsNewArray(wsKeyframe, numKeyframes);
    strcpy(name, nameBuffer);
    errorCheck( fscanf( pFile, "//  Animation files store a collection of joint rotations; matrices are computed on loading\n\n") );
    errorCheck( fscanf( pFile, "joints {\n") );
    u32 jointIndex = 0;
    for (u32 j = 0; j < numJoints; ++j) {
        errorCheck( fscanf( pFile, "  joint %u {\n", &jointIndex) );
        wsAssert( (jointIndex == j), "Current index does not relate to current joint.");
        errorCheck( fscanf( pFile, "    jointName %[^\t\n]\n", joints[j].name) );
        errorCheck( fscanf( pFile, "    parent %d\n", &joints[j].parent) );
        errorCheck( fscanf( pFile, "    pos_start { %f %f %f }\n", &joints[j].start.x, &joints[j].start.y, &joints[j].start.z) );
        errorCheck( fscanf( pFile, "    rotation { %f %f %f %f }\n", &joints[j].rot.x, &joints[j].rot.y, &joints[j].rot.z, &joints[j].rot.w) );
        errorCheck( fscanf( pFile, "  }\n") );
    }// End for each joint
    errorCheck( fscanf( pFile, "}\n\n") );
    errorCheck( fscanf( pFile, "keyframes {\n") );
    u32 frameIndex = 0;
    for (u32 k = 0; k < numKeyframes; ++k) {
        errorCheck( fscanf( pFile, "  keyframe %u {\n", &frameIndex) );
        wsAssert( (frameIndex == k), "Current index does not relate to current keyframe.");
        errorCheck( fscanf( pFile, "    frameNumber %f\n", &keyframes[k].frameIndex) );
        errorCheck( fscanf( pFile, "    jointsModified %u\n", &keyframes[k].numJointsModified) );
        keyframes[k].mods = wsNewArray(wsJointMod, keyframes[k].numJointsModified);
        for (u32 j = 0; j < keyframes[k].numJointsModified; ++j) {
            errorCheck( fscanf( pFile, "    joint %u {\n", &keyframes[k].mods[j].jointIndex) );
            errorCheck( fscanf( pFile, "      jointTranslation { %f %f %f }\n",
                            &keyframes[k].mods[j].location.x,
                            &keyframes[k].mods[j].location.y,
                            &keyframes[k].mods[j].location.z) );
            keyframes[k].mods[j].location.w = 1.0f;
            errorCheck( fscanf( pFile, "      jointRotation { %f %f %f %f }\n",
                            &keyframes[k].mods[j].rotation.x,
                            &keyframes[k].mods[j].rotation.y,
                            &keyframes[k].mods[j].rotation.z,
                            &keyframes[k].mods[j].rotation.w) );
            errorCheck( fscanf( pFile, "    }\n") );
        }// End for each joint modifier
        errorCheck( fscanf( pFile, "  }\n") );
    }
    errorCheck( fscanf( pFile, "}\n") );
    if (numKeyframes) { animLength = keyframes[numKeyframes-1].frameIndex / framesPerSecond; }
    if (fclose(pFile) == EOF) {
      wsLog(WS_LOG_ERROR, "Failed to close animation file \"%s\"", filepath);
    }
}// End wsAnimation constructor

wsAnimation::~wsAnimation() {
    //  Nothing to do here.
}

void wsAnimation::errorCheck(const i32 my) {
  if (my == EOF) {
    wsLog(WS_LOG_ERROR, "Error: premature end of file.");
  }
}
