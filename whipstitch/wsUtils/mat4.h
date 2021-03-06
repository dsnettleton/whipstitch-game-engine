/*
 * mat4.h
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file declares the struct mat4, which is a 4x4 matrix of 32-bit floating
 *      point variables. The matrix struct is designed to interact with the vec4
 *      and quat structs (homogeneous vectors and quaternions, respectively).
 *
 *      All important matrix operations, including multiplication by other matrices,
 *      retrieval of the determinant, conversion to a quaternion, transposition,
 *      and inversion are included.
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

#ifndef WS_MAT4_H_
#define WS_MAT4_H_

#include "wsOperations.h"

struct vec4;
struct quat;

struct mat4 {
#if WS_SUPPORTS_SSE4 == WS_TRUE
    sseAlign(
        union {
            struct {
                __m128  mReg0;
                __m128  mReg1;
                __m128  mReg2;
                __m128  mReg3;
            };
            f32 data[16];
        }
    );
    //  default identity matrix
    mat4(   f32 val0_0 = 1.0f, f32 val0_1 = 0.0f, f32 val0_2 = 0.0f, f32 val0_3 = 0.0f,
            f32 val1_0 = 0.0f, f32 val1_1 = 1.0f, f32 val1_2 = 0.0f, f32 val1_3 = 0.0f,
            f32 val2_0 = 0.0f, f32 val2_1 = 0.0f, f32 val2_2 = 1.0f, f32 val2_3 = 0.0f,
            f32 val3_0 = 0.0f, f32 val3_1 = 0.0f, f32 val3_2 = 0.0f, f32 val3_3 = 1.0f  ) :
            mReg0(_mm_set_ps(val0_3, val0_2, val0_1, val0_0)),
            mReg1(_mm_set_ps(val1_3, val1_2, val1_1, val1_0)),
            mReg2(_mm_set_ps(val2_3, val2_2, val2_1, val2_0)),
            mReg3(_mm_set_ps(val3_3, val3_2, val3_1, val3_0)) {}
    //  copy constructor
    mat4(const mat4& other) :
        mReg0(other.mReg0), mReg1(other.mReg1), mReg2(other.mReg2), mReg3(other.mReg3) {}
    mat4(const __m128& reg0, const __m128& reg1, const __m128& reg2, const __m128& reg3):
        mReg0(reg0), mReg1(reg1), mReg2(reg2), mReg3(reg3) {}
    f32* getData() { return data; }
    f32 getData(u32 row, u32 col) { return data[row*4+col]; }
#else
    f32 data[16];
    //  default identity matrix
    mat4( f32 val0_0 = 1.0f, f32 val0_1 = 0.0f, f32 val0_2 = 0.0f, f32 val0_3 = 0.0f,
        f32 val1_0 = 0.0f, f32 val1_1 = 1.0f, f32 val1_2 = 0.0f, f32 val1_3 = 0.0f,
        f32 val2_0 = 0.0f, f32 val2_1 = 0.0f, f32 val2_2 = 1.0f, f32 val2_3 = 0.0f,
        f32 val3_0 = 0.0f, f32 val3_1 = 0.0f, f32 val3_2 = 0.0f, f32 val3_3 = 1.0f  );
    mat4(const mat4& other);  //  copy constructor
    f32* getData() { return data; }
    f32 getData(u32 row, u32 col) { return data[row*4+col]; }
#endif  /*  WS_SUPPORTS_SSE4    */
    vec4 getRow(u32 rowNum) const;
    vec4 getCol(u32 colNum) const;
    f32 determinant() const;
    vec4 getRotation() const;
    quat getRotationQuat() const;
    vec4 getTranslation() const;
    mat4& invert();
    mat4 inverse() const;
    bool isIdentity() const;
    mat4& loadIdentity();
    void print(u16 printLog = WS_LOG_MAIN) const;
    mat4& setRotation(const quat& quaternion);
    mat4& setRotation(const vec4& axis, f32 angle);
    mat4& setRotation(f32 axisX, f32 axisY, f32 axisZ, f32 angle);
    mat4& setRotation(const vec4& euler);
    mat4& setScale(f32 scalar);
    mat4& setScale(const vec4& scalar);
    mat4& setScale(f32 scaleX, f32 scaleY, f32 scaleZ);
    mat4& setTranslation(const vec4& trans);
    mat4& setTranslation(f32 transX, f32 transY, f32 transZ);
    mat4& translate(const vec4& location);
    mat4& transpose();
    mat4 getTranspose() const;
    mat4& operator+=(const mat4& other);
    mat4& operator-=(const mat4& other);
    mat4& operator*=(const mat4& other);
    mat4& operator*=(f32 value);
    mat4& operator/=(f32 value);
    mat4 operator+(const mat4& other) const;
    mat4 operator-(const mat4& other) const;
    mat4 operator*(const mat4& other) const;
    mat4 operator*(f32 value) const;
    mat4 operator/(f32 value) const;
    mat4& operator=(const mat4& other);
    bool operator==(const mat4& other) const;
    bool operator!=(const mat4& other) const;
};

extern const mat4 MAT4_IDENTITY;

#endif /* WS_MAT4_H_ */
