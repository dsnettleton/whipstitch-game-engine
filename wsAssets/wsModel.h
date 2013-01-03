/*
 * wsModel.h
 *
 *  Created on: Oct 2, 2012
 *      Author: dsnettleton
 *
 *      This file declares the class wsModel, which extends the abstract base
 *      type wsAsset. A wsModel is a complete type used for combining various
 *      assets such as meshes and animations into a cohesive whole.
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

#ifndef WSMODEL_H_
#define WSMODEL_H_

#include "wsMesh.h"
#include "wsAnimation.h"
#include "../wsUtils.h"

#if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
struct wsIndexArray {
    u32* indices;
    u32 numIndices;
    u32 handle;
};
#endif

class wsModel: public wsAsset {
    private:
        wsMesh* mesh;
        const char* name;
        #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
            wsIndexArray* indexArrays;
            u32 numIndexArrays;
            u32 vertexArray;
        #endif
        t64 animTime;
        wsHashMap<wsAnimation*> *animations;
        wsAnimation* currentAnimation;
        f32 timeScale;
        u16 defaultAnimation;
        bool looping;
        bool animPaused;
    public:
        /// Constructor and Destructor
        wsModel(const char* filepath);
        wsModel(const char* myName, wsMesh* myMesh, const u32 myMaxAnimations);
        ~wsModel();
        /// Setters and Getters
        wsIndexArray* getIndexArrays() { return indexArrays; }
        u16 getMaxAnimations() { return animations->getMaxElements(); }
        wsMesh* getMesh() { return mesh; }
        const char* getName() { return name; }
        const char* getCurrentAnimation() { if (currentAnimation == NULL) { return ""; } return currentAnimation->getName(); }
        u16 getNumAnimations() { return animations->getLength(); }
        u32 getNumIndexArrays() { return numIndexArrays; }
        f32 getTimeScale() { return timeScale; }
        u32 getVertexArray() { return vertexArray; }
        void setFrame(f32 newFrame) {
            wsAssert( (currentAnimation != NULL), "Cannot set frame for NULL animation.");
            while (newFrame > currentAnimation->getLength()) {
                newFrame -= currentAnimation->getLength();
            }
            animTime = (t64)newFrame / (t64)currentAnimation->getFramesPerSecond();
        }
        void setMesh(wsMesh* my) { mesh = my; }
        void setTimeScale(f32 my) { timeScale = my; }
        /// Operational Methods
        void addAnimation(wsAnimation* anim);
        void applyAnimation();  //  Applies the current animation to the mesh vertices
        void beginAnimation(const char* animName);
        //u32 computeJointMod(const wsJointMod* myMod, const u32 jointIndex); //  Applies the mod recursively until no parent exists
        void continueAnimation();// Continues a paused animation
        void draw();
        void incrementAnimationTime(t32 increment);
        void pauseAnimation();
        void updateVbo();
};

#endif /* WSMODEL_H_ */
