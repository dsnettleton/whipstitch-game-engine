/*
 * wsTransform.h
 *
 *  Created on: Jul 11, 2012
 *      Author: dsnettleton
 *
 *      This file declares the struct wsTransform, which is a form of SQT transform.
 *      The SQT scale factor is uniform, allowing all the necessary information to be
 *      stored in only eight floats, including a quaternion. The quaternion is placed first
 *      in the transform, giving the data structure a 16-byte alignment.
 *      Transforms are performed on vectors by scaling, then rotating, then translating.
 *      The translation and scale are interpolated using LERP, and the quaternion using
 *      SLERP, by default, or LERP for speed.
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

#ifndef WS_TRANSFORM_H_
#define WS_TRANSFORM_H_

#include "wsOperations.h"
#include "quat.h"
#include "mat4.h"

struct wsTransform {
    //  Member variables
    quat rotation;
    f32 translationX;
    f32 translationY;
    f32 translationZ;
    f32 scale;
    //  Constructors
    wsTransform() : translationX(0.0f), translationY(0.0f), translationZ(0.0f), scale(1.0f) {}
    wsTransform(f32 myScale, const quat& myRotation, const vec4& translation);
    wsTransform(f32 myScale, const quat& myRotation, f32 myTranslationX, f32 myTranslationY, f32 myTranslationZ);
    //  Setters and Getters
    vec4 getTranslation() const;
    wsTransform& setTranslation(f32 x, f32 y, f32 z) {
        translationX = x; translationY = y; translationZ = z;
        return *this;
    }
    wsTransform& setTranslation(const vec4& trans);
    //  Returns the inverse of this SQT transform
    wsTransform getInverse() const;
    //  Sets this SQT transform to its inverse
    wsTransform& invert();
    wsTransform& operator*=(const wsTransform& other);
    wsTransform operator*(const wsTransform& other) const;
    //  Returns the blended SQT transform
    wsTransform blend(const wsTransform& other, f32 blendFactor) const;
    //  Sets this to the blended SQT transform
    wsTransform& toBlend(const wsTransform& other, f32 blendFactor);
    //  Returns a 4x4 Matrix with the transform's properties
    mat4 toMatrix() const;
    void print(u16 printLog = WS_LOG_MAIN) const;
};


#endif /* WS_TRANSFORM_H_ */
