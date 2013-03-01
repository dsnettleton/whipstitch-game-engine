/*
 * quat.h
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file declares the quat structure, which is used for
 *      quaternion operations in conjunction with the vec4 vector struct. Quaternions
 *      are used primarily for rotations in 3D space. They are essentially a normalized
 *      4D vector, containing an x,y,z axis value, and a w rotation value. They can
 *      be conveniently modified and interpolated with each other, and do not suffer
 *      from gimbal lock.
 *
 *      One implementation is provided using standard C operations, the other
 *      utilizing SSE4 on platforms which support it.
 *
 *      SSE4 is a version of the SSE (Streaming SIMD Extensions) set, which allows
 *      a single instruction to be performed on multiple data values simultaneously.
 *      The SSE register is 128 bits, allowing four 32-bit data values to be operated on.
 *      In this case, those data values are floating point decimal values.
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

#ifndef WS_QUAT_H_
#define WS_QUAT_H_

#include "wsTrig.h"
#include "wsOperations.h"
#include <stdio.h>

struct vec4;

struct quat {
#if WS_SUPPORTS_SSE4 == WS_TRUE
    //  Data members
    sseAlign(
        union {
            __m128 mReg;
            struct {
                f32 x;
                f32 y;
                f32 z;
                f32 w;
            };
        };
    )
    //  Default constructor
    quat(f32 myX = 0.0f, f32 myY = 0.0f, f32 myZ = 0.0f, f32 myW = 1.0f) :
        mReg(_mm_set_ps(myW, myZ, myY, myX)) {}
    quat(const quat& other) : mReg(other.mReg) {}
    quat(__m128 reg) : mReg(reg) {}
#else   //  If SSE4 is not supported
    //  Data members
    f32 x, y, z, w;
    //  Default constructor
    quat(f32 myX = 0.0f, f32 myY = 0.0f, f32 myZ = 0.0f, f32 myW = 1.0f) :
        x(myX), y(myY), z(myZ), w(myW) {}
    quat(const quat& other);
#endif  /*  WS_SUPPORTS_SSE4    */
    quat(vec4 axis, f32 angle);
    quat(const vec4& euler);
    //  Operational Member Functions
    quat& set(f32 myX = 0.0f, f32 myY = 0.0f, f32 myZ = 0.0f, f32 myW = 1.0f);
    quat getConjugate() const;
    quat& conjugate();
    //  For our purposes (using normalized quaternions), the inverse and
    //  conjugate are the same.
    quat getInverse() const;
    quat& invert();
    //  Returns angle (in degrees) between the 2 quats
    f32 getAngle(const quat& other) const;
    f32 dotProduct(const quat& other) const;
    //  Uses Slerp, returns blended quat
    quat blend(const quat& other, f32 blendFactor) const;
    //Uses slerp, sets this to the blended quat
    quat& toBlend(const quat& other, f32 blendFactor);
    quat& setRotation(vec4 axis, f32 angle);
    quat& setRotation(f32 axisX, f32 axisY, f32 axisZ, f32 angle);
    quat& setRotationZXY(const vec4& euler);
    quat operator-() const;
    quat operator*(const quat& other) const;
    quat& operator*=(const quat& other);
    quat& operator=(const quat& other);
    quat normal() const;
    quat& normalize();
    void print(u16 printLog = WS_LOG_MAIN) const;
};

#endif /* WS_QUAT_H_ */
