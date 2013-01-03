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

wsTransform::wsTransform(f32 scale, const quat& rotation, const vec4& translation) {
    WS_PROFILE();
    q = rotation;
    s = scale;
    tx = translation.x;
    ty = translation.y;
    tz = translation.z;

}

wsTransform::wsTransform(   f32 scale, const quat& rotation,
                            f32 transX, f32 transY, f32 transZ) {
    WS_PROFILE();
    q = rotation;
    s = scale;
    tx = transX;
    ty = transY;
    tz = transZ;

}

//  Operational member functions

vec4 wsTransform::getTranslation() const {
    WS_PROFILE();
    vec4 my(tx, ty, tz);

    return my;
}

wsTransform& wsTransform::setTranslation(const vec4& trans) {
    WS_PROFILE();
    tx = trans.x; ty = trans.y; tz = trans.z;

    return *this;
}

wsTransform wsTransform::getInverse() const {
    WS_PROFILE();
    wsTransform my(-s, q.getInverse(), -tx, -ty, -tz);

    return my;
}

wsTransform& wsTransform::invert() {
    WS_PROFILE();
    q.invert();
    s *= -1.0f;
    tx *= -1.0f;
    ty *= -1.0f;
    tz *= -1.0f;

    return *this;
}

wsTransform& wsTransform::operator*=(const wsTransform& other) {
    WS_PROFILE();
    q *= other.q;
    s *= other.s;
    tx += other.tx;
    ty += other.ty;
    tz += other.tz;

    return *this;
}

wsTransform wsTransform::operator*(const wsTransform& other) const {
    WS_PROFILE();
    wsTransform my(s*other.s, q*other.q, tx+other.tx, ty+other.ty, tz+other.tz);

    return my;
}

//  Returns the blended SQT transform
wsTransform wsTransform::blend(const wsTransform& other, f32 blendFactor) const {
    WS_PROFILE();
    wsTransform my( wsLerp(s, other.s, blendFactor),
                    q.blend(other.q, blendFactor),
                    wsLerp(tx, other.tx, blendFactor),
                    wsLerp(ty, other.ty, blendFactor),
                    wsLerp(tz, other.tz, blendFactor)   );

    return my;
}
//  Sets this to the blended SQT transform
wsTransform& wsTransform::toBlend(const wsTransform& other, f32 blendFactor) {
    WS_PROFILE();
    q = q.blend(other.q, blendFactor);
    s = wsLerp(s, other.s, blendFactor);
    tx = wsLerp(tx, other.tx, blendFactor);
    ty = wsLerp(ty, other.ty, blendFactor);
    tz = wsLerp(tz, other.tz, blendFactor);

    return *this;
}

mat4 wsTransform::toMatrix() const {
    WS_PROFILE();
    mat4 my;
    my.setRotation(q);
    my.data[0] += s;
    my.data[5] += s;
    my.data[10] += s;
    my.setTranslation(tx, ty, tz);

    return my;
}

void wsTransform::print(u16 printLog) const {
    wsLog(  printLog,
                "Transform Values:\n"
                "    Scale = %f\n"
                "    Quaternion = { %f, %f, %f, %f }\n"
                "    Translation = { %f, %f, %f }\n",
                s, q.x, q.y, q.z, q.w, tx, ty, tz);
}

