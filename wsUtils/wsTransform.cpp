/*
 * wsTransform.cpp
 *
 *  Created on: Jul 11, 2012
 *      Author: dsnettleton
 *
 *      This file implements the class wsTransform, which is a form of SQT transform.
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

#include "wsTransform.h"
#include "vec4.h"
#include "quat.h"

//  Constructors

wsTransform::wsTransform(f32 myScale, const quat& myRotation, const vec4& myTranslation) {
    WS_PROFILE();
    rotation = myRotation;
    scale = myScale;
    translationX = myTranslation.x;
    translationY = myTranslation.y;
    translationZ = myTranslation.z;

}

wsTransform::wsTransform(   f32 myScale, const quat& myRotation,
                            f32 myTranslationX, f32 myTranslationY, f32 myTranslationZ) {
    WS_PROFILE();
    rotation = myRotation;
    scale = myScale;
    translationX = myTranslationX;
    translationY = myTranslationY;
    translationZ = myTranslationZ;

}

//  Operational member functions

vec4 wsTransform::getTranslation() const {
    WS_PROFILE();
    vec4 my(translationX, translationY, translationZ);

    return my;
}

wsTransform& wsTransform::setTranslation(const vec4& trans) {
    WS_PROFILE();
    translationX = trans.x; translationY = trans.y; translationZ = trans.z;

    return *this;
}

wsTransform wsTransform::getInverse() const {
    WS_PROFILE();
    wsTransform my(-scale, rotation.getInverse(), -translationX, -translationY, -translationZ);

    return my;
}

wsTransform& wsTransform::invert() {
    WS_PROFILE();
    rotation.invert();
    scale *= -1.0f;
    translationX *= -1.0f;
    translationY *= -1.0f;
    translationZ *= -1.0f;

    return *this;
}

wsTransform& wsTransform::operator*=(const wsTransform& other) {
    WS_PROFILE();
    rotation *= other.rotation;
    scale *= other.scale;
    translationX += other.translationX;
    translationY += other.translationY;
    translationZ += other.translationZ;

    return *this;
}

wsTransform wsTransform::operator*(const wsTransform& other) const {
    WS_PROFILE();
    wsTransform my(scale*other.scale, rotation*other.rotation, translationX+other.translationX, translationY+other.translationY, translationZ+other.translationZ);

    return my;
}

//  Returns the blended SQT transform
wsTransform wsTransform::blend(const wsTransform& other, f32 blendFactor) const {
    WS_PROFILE();
    wsTransform my( wsLerp(scale, other.scale, blendFactor),
                    rotation.blend(other.rotation, blendFactor),
                    wsLerp(translationX, other.translationX, blendFactor),
                    wsLerp(translationY, other.translationY, blendFactor),
                    wsLerp(translationZ, other.translationZ, blendFactor)   );

    return my;
}
//  Sets this to the blended SQT transform
wsTransform& wsTransform::toBlend(const wsTransform& other, f32 blendFactor) {
    WS_PROFILE();
    rotation= rotation.blend(other.rotation, blendFactor);
    scale = wsLerp(scale, other.scale, blendFactor);
    translationX = wsLerp(translationX, other.translationX, blendFactor);
    translationY = wsLerp(translationY, other.translationY, blendFactor);
    translationZ = wsLerp(translationZ, other.translationZ, blendFactor);

    return *this;
}

mat4 wsTransform::toMatrix() const {
    WS_PROFILE();
    mat4 my;
    my.data[0] = scale;
    my.data[5] = scale;
    my.data[10] = scale;
    my.setRotation(rotation);
    my.setTranslation(translationX, translationY, translationZ);

    return my;
}

void wsTransform::print(u16 printLog) const {
    wsLog(  printLog,
                "Transform Values:\n"
                "    Scale = %f\n"
                "    Quaternion = { %f, %f, %f, %f }\n"
                "    Translation = { %f, %f, %f }\n",
                scale, rotation.x, rotation.y, rotation.z, rotation.w, translationX, translationY, translationZ);
}

