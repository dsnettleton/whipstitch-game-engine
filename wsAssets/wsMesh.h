/*
 * wsMesh.h
 *
 *  Created on: Sep 19, 2012
 *      Author: dsnettleton
 *
 *      This file declares the class wsMesh, which extends the abstract base
 *      type wsAsset. A wsMesh is only part of the more complete type,
 *      wsModel, but the mesh can be used independently if so desired.
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

#ifndef WS_MESH_H_
#define WS_MESH_H_

#include "wsAsset.h"

struct wsWeight {
    i32 jointIndex;
    f32 influence;
};

struct wsVert {
    vec4 pos;
    vec4 norm;
    f32 tex[2]; //  UV texture coordinates
    vec4 originalPos;
    wsWeight* weights;
    u32 numWeights;
};

struct wsTriangle {
    u32 vertIndices[3];
};

struct wsMaterial {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emissive;
    wsTriangle* tris;
    u32 shininess;
    u32 colorMap;
    u32 numTriangles;
};

struct wsJoint {
    vec4 start;
    vec4 startRel;
    vec4 end;
    vec4 endRel;
    quat rot;
    i32 parent;
};

struct wsTag {
    vec4 pos;
    quat rot;
};

class wsMesh: public wsAsset {
    private:
        wsVert* verts;
        wsMaterial* mats;
        wsJoint* joints;
        wsJoint* baseSkel;
        wsTag* tags;
        u32 numVerts;
        u32 numMaterials;
        u32 numJoints;
        u32 numTags;
    public:
        //  Constructor
        wsMesh(const char* filepath);
        ~wsMesh();
        //  Getters
        const wsVert* getVerts() const { return verts; }
        const wsMaterial* getMats() const { return mats; }
        const wsJoint* getJoints() const { return joints; }
        const wsJoint* getBaseSkel() const { return baseSkel; }
        const wsTag* getTags() const { return tags; }
        u32 getNumVerts() const { return numVerts; }
        u32 getNumMaterials() const { return numMaterials; }
        u32 getNumJoints() const { return numJoints; }
        u32 getNumTags() const { return numTags; }
        //  Operational Methods
        void errorCheck(const i32 my);
};

#endif /* WS_MESH_H_ */
