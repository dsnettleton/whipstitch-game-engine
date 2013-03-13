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
