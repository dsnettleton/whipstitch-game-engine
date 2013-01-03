/*
 * vec4.cpp
 *
 *  Created on: Jul 3, 2012
 *      Author: dsnettleton
 *
 *      This file implements the struct vec4, which is intended to be used as a 3d vector
 *      in homogeneous coordinates. Vectors are important objects for 3D operations,
 *      as they allow many operations to be greatly simplified, as well as performed
 *      efficiently.
 *
 *      Because of the mathematical nature of the operations, many or all functions
 *      have been recommended for inlining to the compiler.
 *
 *      One version is provided using standard C operations, the other
 *      utilizing SSE4 on platforms which support it.
 *
 *      SSE4 is a version of the SSE (Streaming SIMD Extensions) set, which allows
 *      a single instruction to be performed on multiple data values simultaneously.
 *      The SSE register is 128 bits, allowing four 32-bit data values to be operated on.
 *      In this case, those data values are floating point decimal values.
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

#include "vec4.h"
#include "mat4.h"
#include "quat.h"

const vec4 X_AXIS(1.0f, 0.0f, 0.0f, 1.0f);
const vec4 Y_AXIS(0.0f, 1.0f, 0.0f, 1.0f);
const vec4 Z_AXIS(0.0f, 0.0f, 1.0f, 1.0f);

#if WS_SUPPORTS_SSE4 == WS_TRUE

//  Set variables
vec4& vec4::set(f32 myX, f32 myY, f32 myZ, f32 myW) {
    mReg = _mm_set_ps(myW, myZ, myY, myX);

    return *this;
}

vec4& vec4::set3(const f32 data[], f32 myW) {
    mReg = _mm_set_ps(myW, data[2], data[1], data[0]);

    return *this;
}

vec4& vec4::set4(const f32 data[]) {
    mReg = _mm_set_ps(data[3], data[2], data[1], data[0]);

    return *this;
}

vec4& vec4::set(const quat& quaternion) {
    mReg = quaternion.mReg;

    return *this;
}

// Arithmetic Operators
vec4 vec4::operator+(const vec4& other) const {
    vec4 my = _mm_add_ps(mReg, other.mReg);

    return my;
}

vec4 vec4::operator-(const vec4& other) const {
    vec4 my = _mm_sub_ps(mReg, other.mReg);

    return my;
}

vec4 vec4::operator*(const vec4& other) const {
    vec4 my = _mm_mul_ps(mReg, other.mReg);

    return my;
}

vec4 vec4::operator/(const vec4& other) const {
    vec4 my = _mm_div_ps(mReg, other.mReg);

    return my;
}

vec4 vec4::operator+(f32 scalar) const {
    vec4 my = _mm_add_ps(mReg, _mm_set1_ps(scalar));

    return my;
}

vec4 vec4::operator-(f32 scalar) const {
    vec4 my = _mm_sub_ps(mReg, _mm_set1_ps(scalar));

    return my;
}

vec4 vec4::operator*(f32 scalar) const {
    vec4 my = _mm_mul_ps(mReg, _mm_set1_ps(scalar));

    return my;
}

vec4 vec4::operator/(f32 scalar) const {
    vec4 my = _mm_div_ps(mReg, _mm_set1_ps(scalar));

    return my;
}

vec4 vec4::operator*(const mat4& mat) const {
    sseAlign(__m128 result);
    result = _mm_mul_ps(ws_sseDistributeX(mReg), mat.mReg0);
    result = ws_sseMadd(ws_sseDistributeY(mReg), mat.mReg1, result);
    result = ws_sseMadd(ws_sseDistributeZ(mReg), mat.mReg2, result);
    result = ws_sseMadd(ws_sseDistributeW(mReg), mat.mReg3, result);

    return result;
}

vec4& vec4::operator+=(const vec4& other) {
    mReg = _mm_add_ps(mReg, other.mReg);

    return *this;
}

vec4& vec4::operator-=(const vec4& other) {
    mReg = _mm_sub_ps(mReg, other.mReg);

    return *this;
}

vec4& vec4::operator*=(const vec4& other) {
    mReg = _mm_mul_ps(mReg, other.mReg);

    return *this;
}

vec4& vec4::operator/=(const vec4& other) {
    mReg = _mm_div_ps(mReg, other.mReg);

    return *this;
}

vec4& vec4::operator+=(f32 scalar) {
    mReg = _mm_add_ps(mReg, _mm_set1_ps(scalar));

    return *this;
}

vec4& vec4::operator-=(f32 scalar) {
    mReg = _mm_sub_ps(mReg, _mm_set1_ps(scalar));

    return *this;
}

vec4& vec4::operator*=(f32 scalar) {
    mReg = _mm_mul_ps(mReg, _mm_set1_ps(scalar));

    return *this;
}

vec4& vec4::operator/=(f32 scalar) {
    mReg = _mm_div_ps(mReg, _mm_set1_ps(scalar));

    return *this;
}

vec4& vec4::operator*=(const mat4& mat) {
    sseAlign(__m128 result);
    result = _mm_mul_ps(ws_sseDistributeX(mReg), mat.mReg0);
    result = ws_sseMadd(ws_sseDistributeY(mReg), mat.mReg1, result);
    result = ws_sseMadd(ws_sseDistributeZ(mReg), mat.mReg2, result);
    result = ws_sseMadd(ws_sseDistributeW(mReg), mat.mReg3, result);
    mReg = result;

    return *this;
}

vec4& vec4::operator=(const vec4& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;

    return *this;
}

bool vec4::operator==(const vec4& other) const {
    return (x == other.x && y == other.y && z == other.z && w == other.w);
}

bool vec4::operator!=(const vec4& other) const {
    return (x != other.x || y != other.y || z != other.z || w != other.w);
}

vec4 vec4::crossProduct(const vec4& other) const {
    vec4 my = _mm_sub_ps(
                _mm_mul_ps(_mm_shuffle_ps(mReg, mReg, _MM_SHUFFLE(3, 0, 2, 1)),
                _mm_shuffle_ps(other.mReg, other.mReg, _MM_SHUFFLE(3, 1, 0, 2))),
                _mm_mul_ps(_mm_shuffle_ps(mReg, mReg, _MM_SHUFFLE(3, 1, 0, 2)),
                _mm_shuffle_ps(other.mReg, other.mReg, _MM_SHUFFLE(3, 0, 2, 1)))
                    );

    return my;
}

vec4& vec4::toCrossProduct(const vec4& other) {
    mReg = _mm_sub_ps(
                    _mm_mul_ps(_mm_shuffle_ps(mReg, mReg, _MM_SHUFFLE(3, 0, 2, 1)),
                    _mm_shuffle_ps(other.mReg, other.mReg, _MM_SHUFFLE(3, 1, 0, 2))),
                    _mm_mul_ps(_mm_shuffle_ps(mReg, mReg, _MM_SHUFFLE(3, 1, 0, 2)),
                    _mm_shuffle_ps(other.mReg, other.mReg, _MM_SHUFFLE(3, 0, 2, 1)))
                        );

    return *this;
}

vec4 vec4::blend(const vec4& other, f32 blendFactor) {
    vec4 my( _mm_add_ps(_mm_mul_ps( _mm_set1_ps(1.0f-blendFactor), mReg),
                        _mm_mul_ps( _mm_set1_ps(blendFactor), other.mReg) ) );

    return my;
}

vec4& vec4::toBlend(const vec4& other, f32 blendFactor) {
    mReg = _mm_add_ps(  _mm_mul_ps( _mm_set1_ps(1.0f-blendFactor), mReg),
                        _mm_mul_ps( _mm_set1_ps(blendFactor), other.mReg) );

    return *this;
}

bool vec4::isZero() const {
    //return _mm_cmpeq_ps(mReg, _mm_setzero_ps());
    return (x == 0.0f && y == 0.0f && z == 0.0f); //  Exclude w value
}

f32 vec4::dotProduct(const vec4& other) const {
    f32 my = _mm_cvtss_f32(_mm_dp_ps(mReg, other.mReg, 0x71));

    return my;
}

f32 vec4::magnitude() const {
    f32 my = _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(mReg, mReg, 0x71)));

    return my;
}

f32 vec4::magnitudeSquared() const {
    f32 my = _mm_cvtss_f32(_mm_dp_ps(mReg, mReg, 0x71));

    return my;
}

// Returns the normalized unit vector
vec4 vec4::normal() const {
    vec4 my(_mm_mul_ps(mReg, _mm_rsqrt_ps(_mm_dp_ps(mReg, mReg, 0x77))));

    return my;
}

// Scales the vector to normal length
vec4& vec4::normalize() {
    mReg = _mm_mul_ps(mReg, _mm_rsqrt_ps(_mm_dp_ps(mReg, mReg, 0x77)));

    return *this;
}

bool vec4::perpendicular(const vec4& other) const {
    return (dotProduct(other) == 0.0f);
}

vec4 vec4::getRotate(const quat& quaternion) const {
    quat vecQuat(mReg);
    vec4 my(quaternion*vecQuat*quaternion.getInverse());

    return my;
}

vec4& vec4::rotate(const quat& quaternion) {
    quat vecQuat(x, y, z, w);
    set(quaternion*vecQuat*quaternion.getInverse());

    return *this;
}

bool vec4::isPositional() {
    return (w == 1.0f);
}

bool vec4::isDirectional() {
    return (w == 0.0f);
}

void vec4::print(u16 printLog) const {
    wsLog(printLog, "{ %f, %f, %f, %f }\n", x, y, z, w);
}

#else   //  If SSE4 is not supported

//  Set variables
vec4& vec4::set(f32 myX, f32 myY, f32 myZ, f32 myW) {
    x = myX;
    y = myY;
    z = myZ;
    w = myW;

    return *this;
}

vec4& vec4::set3(const f32 data[], f32 myW) {
    x = data[0];
    y = data[1];
    z = data[2];
    w = myW;

    return *this;
}

vec4& vec4::set4(const f32 data[]) {
    x = data[0];
    y = data[1];
    z = data[2];
    w = data[3];

    return *this;
}

vec4& vec4::set(const quat& quaternion) {
    x = quaternion.x;
    y = quaternion.y;
    z = quaternion.z;
    w = quaternion.w;

    return *this;
}

// Arithmetic Operators
vec4 vec4::operator+(const vec4& other) const {
    vec4 my(x + other.x, y + other.y, z + other.z, w + other.w);

    return my;
}

vec4 vec4::operator-(const vec4& other) const {
    vec4 my(x - other.x, y - other.y, z - other.z, w - other.w);

    return my;
}

vec4 vec4::operator*(const vec4& other) const {
    return vec4(x * other.x, y * other.y, z * other.z, w * other.w);
}

vec4 vec4::operator/(const vec4& other) const {
    vec4 my(x / other.x, y / other.y, z / other.z, w / other.w);

    return my;
}

vec4 vec4::operator+(f32 scalar) const {
    vec4 my(x + scalar, y + scalar, z + scalar, w + scalar);

    return my;
}

vec4 vec4::operator-(f32 scalar) const {
    vec4 my(x - scalar, y - scalar, z - scalar, w - scalar);

    return my;
}

vec4 vec4::operator*(f32 scalar) const {
    vec4 my(x * scalar, y * scalar, z * scalar, w * scalar);

    return my;
}

vec4 vec4::operator/(f32 scalar) const {
    vec4 my(x / scalar, y / scalar, z / scalar, w / scalar);

    return my;
}

vec4 vec4::operator*(const mat4& mat) const {
    vec4 my(    dotProduct(mat.getCol(0)),
                dotProduct(mat.getCol(1)),
                dotProduct(mat.getCol(2)),
                dotProduct(mat.getCol(3))    );

    return my;
}

vec4& vec4::operator+=(const vec4& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

vec4& vec4::operator-=(const vec4& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
}

vec4& vec4::operator*=(const vec4& other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;
    w *= other.w;

    return *this;
}

vec4& vec4::operator/=(const vec4& other) {
    x /= other.x;
    y /= other.y;
    z /= other.z;
    w /= other.w;

    return *this;
}

vec4& vec4::operator+=(f32 scalar) {
    x += scalar;
    y += scalar;
    z += scalar;
    w += scalar;

    return *this;
}

vec4& vec4::operator-=(f32 scalar) {
    x -= scalar;
    y -= scalar;
    z -= scalar;
    w -= scalar;

    return *this;
}

vec4& vec4::operator*=(f32 scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;

    return *this;
}

vec4& vec4::operator/=(f32 scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;

    return *this;
}

vec4& vec4::operator*=(const mat4& mat) {
    *this =  vec4(  dotProduct(mat.getCol(0)),
                    dotProduct(mat.getCol(1)),
                    dotProduct(mat.getCol(2)),
                    dotProduct(mat.getCol(3))    );

    return *this;
}

vec4& vec4::operator=(const vec4& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;

    return *this;
}

bool vec4::operator==(const vec4& other) const {
    return (x == other.x && y == other.y && z == other.z && w == other.w);
}

bool vec4::operator!=(const vec4& other) const {
    return (x != other.x || y != other.y || z != other.z || w != other.w);
}

vec4 vec4::crossProduct(const vec4& other) const {
    return vec4(y * other.z - z * other.y, z * other.x - x * other.z,
            x * other.y - y * other.x, w);
}

vec4& vec4::toCrossProduct(const vec4& other) {
    vec4 tmp(y * other.z - z * other.y, z * other.x - x * other.z,
                    x * other.y - y * other.x, w);
    x = tmp.x;
    y = tmp.y;
    z = tmp.z;
    w = tmp.w;

    return *this;
}

vec4 vec4::blend(const vec4& other, f32 blendFactor) {
    vec4 my(    wsLerp(x, other.x, blendFactor),
                wsLerp(y, other.y, blendFactor),
                wsLerp(z, other.z, blendFactor),
                wsLerp(w, other.w, blendFactor) );

    return my;
}

vec4& vec4::toBlend(const vec4& other, f32 blendFactor) {
    x = wsLerp(x, other.x, blendFactor);
    y = wsLerp(y, other.y, blendFactor);
    z = wsLerp(z, other.z, blendFactor);
    w = wsLerp(w, other.w, blendFactor);

    return *this;
}

bool vec4::isZero() const {
    return (x == 0.0f && y == 0.0f && z == 0.0f); //  Exclude w value
}

f32 vec4::dotProduct(const vec4& other) const {
    f32 my(x * other.x + y * other.y + z * other.z);

    return my;
}

f32 vec4::magnitude() const {
    f32 my = wsSqrt(x * x + y * y + z * z);

    return my;
}

f32 vec4::magnitudeSquared() const {
    f32 my(x * x + y * y + z * z);

    return my;
}

// Returns the normalized unit vector
vec4 vec4::normal() const {
    f32 mag = magnitudeSquared();
    if (mag) {
        return *this / wsSqrt(mag);
    }

    return *this;
}

// Scales the vector to normal length
vec4& vec4::normalize() {
    f32 mag = magnitudeSquared();
    if (mag) {
        *this /= wsSqrt(mag);
    }

    return *this;
}

bool vec4::perpendicular(const vec4& other) const {
    return (dotProduct(other) == 0.0f);
}

vec4 vec4::getRotate(const quat& quaternion) const {
    quat vecQuat(x, y, z, 0);
    vec4 my(quaternion * vecQuat * quaternion.getInverse());
    my.w = w;

    return my;
}

vec4& vec4::rotate(const quat& quaternion) {
    f32 tmpW = w;
    //  Perform rotation
    quat vecQuat(x, y, z, 0);
    set(quaternion * vecQuat * quaternion.getInverse());
    w = tmpW;

    return *this;
}

bool vec4::isPositional() {
    return (w == 1.0f);
}

bool vec4::isDirectional() {
    return (w == 0.0f);
}

void vec4::print(u16 printLog) const {
    wsLog(printLog, "{ %f, %f, %f, %f }\n", x, y, z, w);
}

#endif  /*  WS_SUPPORTS_SSE4    */

