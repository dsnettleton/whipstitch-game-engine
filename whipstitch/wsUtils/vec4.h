/*
 * vec4.h
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file declares the struct vec4, which is intended to be used as a 3d vector
 *      in homogeneous coordinates. Vectors are important objects for 3D operations,
 *      as they allow many operations to be greatly simplified, as well as performed
 *      efficiently.
 *
 *      Because of the mathematical nature of the operations, many or all functions
 *      have been recommended for inlining to the compiler.
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

#ifndef WS_VEC4_H_
#define WS_VEC4_H_

#include "wsOperations.h"
#include "mat4.h"
#include "wsTransform.h"
#include <stdio.h>

struct vec4 {
#if WS_SUPPORTS_SSE4 == WS_TRUE
    sseAlign(
        union {
            __m128 mReg;
            struct {
                f32 x;
                f32 y;
                f32 z;
                f32 w;
            };
            struct {
                f32 r;
                f32 g;
                f32 b;
                f32 a;
            };
            struct {
                f32 rectX;
                f32 rectY;
                f32 rectW;
                f32 rectH;
            };
        }
    );
    //  Default constructor uses SSE to initialize all values to zero
    vec4() : mReg(_mm_setzero_ps()) { w = 1.0f; }
    //  Secondary constructor allows explicit setting of values
    vec4(f32 myX, f32 myY = 0.0f, f32 myZ = 0.0f, f32 myW = 1.0f) :
                    mReg(_mm_set_ps(myW, myZ, myY, myX)) {}
    vec4(const vec4& other) : mReg(other.mReg) {}
    vec4(__m128 reg) : mReg(reg) {}
    vec4(const quat& quaternion) : mReg(quaternion.mReg) {}
#else   //  if SSE4 is not supported
    union {
        struct { f32 x, y, z, w; };
        struct { f32 r, g, b, a; };
        struct { f32 rectX, rectY, rectW, rectH; };
    };
    //  Default constructor allows explicit setting of values
    vec4(f32 myX = 0.0f, f32 myY = 0.0f, f32 myZ = 0.0f, f32 myW = 1.0f) :
        x(myX), y(myY), z(myZ), w(myW) {}
    vec4(const vec4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
    vec4(const quat& quaternion) :
                    x(quaternion.x), y(quaternion.y), z(quaternion.z), w(quaternion.w) {}
#endif  /*  WS_SUPPORTS_SSE4    */
    //  Set variables
    vec4& set(f32 myX = 0.0f, f32 myY = 0.0f, f32 myZ = 0.0f, f32 myW = 1.0f);
    vec4& set3(const f32 data[], f32 myW = 1.0f);
    vec4& set4(const f32 data[]);
    vec4& set(const quat& quaternion);
    // Arithmetic Operators
    vec4 operator-() const;
    vec4 operator+(const vec4& other) const;// {return _mm_add_ps(mReg, other.mReg);}
    vec4 operator-(const vec4& other) const;// {return _mm_sub_ps(mReg, other.mReg);}
    vec4 operator*(const vec4& other) const;// {return _mm_mul_ps(mReg, other.mReg);}
    vec4 operator/(const vec4& other) const;// {return _mm_div_ps(mReg, other.mReg);}
    vec4 operator+(f32 scalar) const;
    vec4 operator-(f32 scalar) const;
    vec4 operator*(f32 scalar) const;
    vec4 operator/(f32 scalar) const;
    vec4 operator*(const mat4& mat) const;
    vec4& operator+=(const vec4& other);
    vec4& operator-=(const vec4& other);
    vec4& operator*=(const vec4& other);
    vec4& operator/=(const vec4& other);
    vec4& operator+=(f32 scalar);
    vec4& operator-=(f32 scalar);
    vec4& operator*=(f32 scalar);
    vec4& operator/=(f32 scalar);
    vec4& operator*=(const mat4& mat);
    vec4& operator=(const vec4& other);
    bool operator==(const vec4& other) const;
    bool operator!=(const vec4& other) const;
    vec4 crossProduct(const vec4& other) const; //  Returns the cross-product
    vec4& toCrossProduct(const vec4& other); //  Sets this vector to the cross-product
    vec4 blend(const vec4& other, f32 blendFactor); //  Linear interpolation
    vec4& toBlend(const vec4& other, f32 blendFactor);   //  Sets this to the blended vector
    bool isZero() const;
    f32 distance(const vec4& other) const;
    f32 dotProduct(const vec4& other) const;
    f32 magnitude() const;
    f32 magnitudeSquared() const;
    // Returns the normalized unit vector
    vec4 normal() const;
    //  Returns the address of the value array
    const f32* array() const { return &x; }
    // Scales the vector to normal length
    vec4& normalize();
    bool perpendicular(const vec4& other) const;
    vec4 getRotate(const quat& quaternion) const;
    vec4& rotate(const quat& quaternion);
    vec4 getRotate(const vec4& axis, f32 degrees) const;
    vec4& rotate(const vec4& axis, f32 degrees);
    vec4& rotatePlane(const vec4& dir, vec4 upDir); //  Rotates X and Y coordinates to the specified 3d view
    bool isPositional();
    bool isDirectional();
    void print(u16 printLog = WS_LOG_MAIN) const;
};

extern const vec4 WS_X_AXIS;
extern const vec4 WS_Y_AXIS;
extern const vec4 WS_Z_AXIS;

#endif /* WS_VEC4_H_ */
