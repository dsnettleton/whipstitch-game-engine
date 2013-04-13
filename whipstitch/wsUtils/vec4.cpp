/*
 * vec4.cpp
 *
 *  Created on: Jul 3, 2012
 *    Author: dsnettleton
 *
 *    This file implements the struct vec4, which is intended to be used as a 3d vector
 *    in homogeneous coordinates. Vectors are important objects for 3D operations,
 *    as they allow many operations to be greatly simplified, as well as performed
 *    efficiently.
 *
 *    Because of the mathematical nature of the operations, many or all functions
 *    have been recommended for inlining to the compiler.
 *
 *    One version is provided using standard C operations, the other
 *    utilizing SSE4 on platforms which support it.
 *
 *    SSE4 is a version of the SSE (Streaming SIMD Extensions) set, which allows
 *    a single instruction to be performed on multiple data values simultaneously.
 *    The SSE register is 128 bits, allowing four 32-bit data values to be operated on.
 *    In this case, those data values are floating point decimal values.
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

#include "vec4.h"
#include "mat4.h"
#include "quat.h"

const vec4 WS_X_AXIS(1.0f, 0.0f, 0.0f, 1.0f);
const vec4 WS_Y_AXIS(0.0f, 1.0f, 0.0f, 1.0f);
const vec4 WS_Z_AXIS(0.0f, 0.0f, 1.0f, 1.0f);

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
    return vec4(_mm_mul_ps(mReg, _mm_rsqrt_ps(_mm_dp_ps(mReg, mReg, 0x77))));
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
    return vec4(quaternion*vecQuat*quaternion.getInverse());
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
  vec4 vec4::operator-() const {
    return vec4(-x, -y, -z, -w);
  }

  vec4 vec4::operator+(const vec4& other) const {
    return vec4(x + other.x, y + other.y, z + other.z, w + other.w);
  }

  vec4 vec4::operator-(const vec4& other) const {
    return vec4(x - other.x, y - other.y, z - other.z, w - other.w);
  }

  vec4 vec4::operator*(const vec4& other) const {
    return vec4(x * other.x, y * other.y, z * other.z, w * other.w);
  }

  vec4 vec4::operator/(const vec4& other) const {
    return vec4(x / other.x, y / other.y, z / other.z, w / other.w);
  }

  vec4 vec4::operator+(f32 scalar) const {
    return vec4(x + scalar, y + scalar, z + scalar, w + scalar);
  }

  vec4 vec4::operator-(f32 scalar) const {
    return vec4(x - scalar, y - scalar, z - scalar, w - scalar);
  }

  vec4 vec4::operator*(f32 scalar) const {
    return vec4(x * scalar, y * scalar, z * scalar, w * scalar);
  }

  vec4 vec4::operator/(f32 scalar) const {
    return vec4(x / scalar, y / scalar, z / scalar, w / scalar);
  }

  vec4 vec4::operator*(const mat4& mat) const {
    return vec4(  dotProduct(mat.getCol(0)),
            dotProduct(mat.getCol(1)),
            dotProduct(mat.getCol(2)),
            dotProduct(mat.getCol(3))  );
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
            dotProduct(mat.getCol(3))  );

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
    vec4 my(  wsLerp(x, other.x, blendFactor),
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

  f32 vec4::distance(const vec4& other) const {
    return (*this - other).magnitude();
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

  vec4 vec4::getRotate(const vec4& axis, f32 degrees) const {
    return getRotate(quat(axis, degrees));
  }

  vec4& vec4::rotate(const vec4& axis, f32 degrees) {
    return rotate(quat(axis, degrees));
  }

  vec4& vec4::rotatePlane(const vec4& dir, vec4 upDir) { //  Rotates X and Y coordinates to the specified 3d view
    vec4 vec = dir.crossProduct(upDir) * x; //  adjusts the X value to the normal perpendicular to dir and updir
    upDir *= y;  //  adjusts the Y value to the upDir normal vector
    vec += upDir;
    *this = vec;
    return *this;   //  Combines the two vectors and returns them.
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

#endif  /*  WS_SUPPORTS_SSE4  */

