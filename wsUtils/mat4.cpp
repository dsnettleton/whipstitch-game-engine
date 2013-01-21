/*
 * mat4.cpp
 *
 *  Created on: Jul 2, 2012
 *    Author: dsnettleton
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

#include "mat4.h"
#include "vec4.h"
#include "quat.h"
#include "wsTrig.h"
#include "wsProfiling.h"

const mat4 MAT4_IDENTITY(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

#if WS_SUPPORTS_SSE4 == WS_TRUE

  f32 mat4::determinant() const {
    //  Longhand clearly wouldn't be preferable for human readability, but it is
    //  computationally faster.
    f32 my(
            data[3] * data[6] * data[9] * data[12]
                    - data[2] * data[7] * data[9] * data[12]
                    - data[3] * data[5] * data[10] * data[12]
                    + data[1] * data[7] * data[10] * data[12]
                    + data[2] * data[5] * data[11] * data[12]
                    - data[1] * data[6] * data[11] * data[12]
                    - data[3] * data[6] * data[8] * data[13]
                    + data[2] * data[7] * data[8] * data[13]
                    + data[3] * data[4] * data[10] * data[13]
                    - data[0] * data[7] * data[10] * data[13]
                    - data[2] * data[4] * data[11] * data[13]
                    + data[0] * data[6] * data[11] * data[13]
                    + data[3] * data[5] * data[8] * data[14]
                    - data[1] * data[7] * data[8] * data[14]
                    - data[3] * data[4] * data[9] * data[14]
                    + data[0] * data[7] * data[9] * data[14]
                    + data[1] * data[4] * data[11] * data[14]
                    - data[0] * data[5] * data[11] * data[14]
                    - data[2] * data[5] * data[8] * data[15]
                    + data[1] * data[6] * data[8] * data[15]
                    + data[2] * data[4] * data[9] * data[15]
                    - data[0] * data[6] * data[9] * data[15]
                    - data[1] * data[4] * data[10] * data[15]
                    + data[0] * data[5] * data[10] * data[15]);
    return my;
    //  (whew!)
  }

  vec4 mat4::getRotation() const {
    vec4 vSin, vCos;
    vSin.y = -data[2];
    if (vSin.y >= 1.0f) {
      vCos.y = 0.0f;
    }
    else {
      vCos.y = wsSqrt(1.0f - vSin.y * vSin.y);
    }
    if (vCos.y) {
      vSin.x = data[6] / vCos.y;
      vCos.x = data[10] / vCos.y;
      vSin.z = data[1] / vCos.y;
      vCos.z = data[0] / vCos.y;
    }
    else {
      vSin.x = -data[9];
      vCos.x = data[5];
      vSin.z = 0.0f;
      vCos.z = 1.0f;
    }
    vec4 my(atan2(vSin.x, vCos.x), atan2(vSin.y, vCos.y), atan2(vSin.z, vCos.z));
    return my;
  }

  quat mat4::getRotationQuat() const {
    f32 my = 1.0f + data[0] + data[5] + data[10];
    if (my > 0.0f) {
      my = 2.0f * wsSqrt(my);
      return quat((data[9] - data[6]) / my, (data[2] - data[8]) / my,
              (data[4] - data[1]) / my, my / 4.0f);
    }
    if (data[0] > data[5] && data[0] > data[10]) {
      my = 2.0f * wsSqrt(1.0f + data[0] - data[5] - data[10]);
      return quat(my / 4.0f, (data[4] + data[1]) / my, (data[2] + data[8]) / my,
              (data[9] - data[6]) / my);
    }
    if (data[5] > data[10]) {
      my = 2.0f * wsSqrt(1.0f + data[5] - data[0] - data[10]);
      return quat((data[4] + data[1]) / my, my / 4.0f, (data[9] + data[6]) / my,
              (data[2] - data[8]) / my);
    }
    my = 2.0f * wsSqrt(1.0f + data[10] - data[0] - data[5]);
    quat myQuat((data[2] + data[8]) / my, (data[9] + data[6]) / my, my / 4.0f,
            (data[4] - data[1]) / my);
    return myQuat;
  }

  vec4 mat4::getTranslation() const {
    return vec4(mReg3);
  }

  mat4 mat4::inverse() const {
    mat4 trans = getTranspose();
    sseAlign(__m128 tmp);
    sseAlign(__m128 cofactor0);
    sseAlign(__m128 cofactor1);
    sseAlign(__m128 cofactor2);
    sseAlign(__m128 cofactor3);

    tmp = _mm_mul_ps(trans.mReg2, trans.mReg3);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    cofactor0 = _mm_mul_ps(trans.mReg1, tmp);
    cofactor1 = _mm_mul_ps(trans.mReg0, tmp);
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor0 = _mm_sub_ps(_mm_mul_ps(trans.mReg1, tmp), cofactor0);
    cofactor1 = _mm_sub_ps(_mm_mul_ps(trans.mReg0, tmp), cofactor1);
    cofactor1 = ws_sseShuffle(cofactor1, 2, 3, 0, 1);
    tmp = _mm_mul_ps(trans.mReg1, trans.mReg2);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    cofactor0 = _mm_add_ps(_mm_mul_ps(trans.mReg3, tmp), cofactor0);
    cofactor3 = _mm_mul_ps(trans.mReg0, tmp);
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor0 = _mm_sub_ps(cofactor0, _mm_mul_ps(trans.mReg3, tmp));
    cofactor3 = _mm_sub_ps(_mm_mul_ps(trans.mReg0, tmp), cofactor3);
    cofactor3 = ws_sseShuffle(cofactor3, 2, 3, 0, 1);
    tmp = _mm_mul_ps(ws_sseShuffle(trans.mReg1, 2, 3, 0, 1), trans.mReg3);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    ws_sseShuffle(trans.mReg2, 2, 3, 0, 1);
    cofactor0 = _mm_add_ps(_mm_mul_ps(trans.mReg2, tmp), cofactor0);
    cofactor2 = _mm_mul_ps(trans.mReg0, tmp);
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor0 = _mm_sub_ps(cofactor0, _mm_mul_ps(trans.mReg2, tmp));
    cofactor2 = _mm_sub_ps(_mm_mul_ps(trans.mReg0, tmp), cofactor2);
    cofactor2 = ws_sseShuffle(cofactor2, 2, 3, 0, 1);
    tmp = _mm_mul_ps(trans.mReg0, trans.mReg1);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    cofactor2 = _mm_add_ps(_mm_mul_ps(trans.mReg3, tmp), cofactor2);
    cofactor3 = _mm_sub_ps(_mm_mul_ps(trans.mReg2, tmp), cofactor3);
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor2 = _mm_sub_ps(_mm_mul_ps(trans.mReg3, tmp), cofactor2);
    cofactor3 = _mm_sub_ps(cofactor3, _mm_mul_ps(trans.mReg2, tmp));
    tmp = _mm_mul_ps(trans.mReg0, trans.mReg3);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    cofactor1 = _mm_sub_ps(cofactor1, _mm_mul_ps(trans.mReg2, tmp));
    cofactor2 = _mm_add_ps(_mm_mul_ps(trans.mReg1, tmp), cofactor2);
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor1 = _mm_add_ps(_mm_mul_ps(trans.mReg2, tmp), cofactor1);
    cofactor2 = _mm_sub_ps(cofactor2, _mm_mul_ps(trans.mReg1, tmp));
    tmp = _mm_mul_ps(trans.mReg0, trans.mReg2);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    cofactor1 = _mm_add_ps(_mm_mul_ps(trans.mReg3, tmp), cofactor1);
    cofactor3 = _mm_sub_ps(cofactor3, _mm_mul_ps(trans.mReg1, tmp));
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor1 = _mm_sub_ps(cofactor1, _mm_mul_ps(trans.mReg3, tmp));
    cofactor3 = _mm_add_ps(_mm_mul_ps(trans.mReg1, tmp), cofactor3);

    sseAlign(__m128 determinant);
    determinant = _mm_mul_ps(trans.mReg0, cofactor0);
    determinant = _mm_add_ps(ws_sseShuffle(determinant, 2, 3, 0, 1), determinant);
    determinant = _mm_add_ss(ws_sseShuffle(determinant, 1, 0, 3, 2), determinant);
    tmp = _mm_rcp_ss(determinant);
    determinant = _mm_sub_ss(_mm_add_ss(tmp, tmp),
            _mm_mul_ss(determinant, _mm_mul_ss(tmp, tmp)));
    determinant = ws_sseDistributeX(determinant);
    cofactor0 = _mm_mul_ps(determinant, cofactor0);
    cofactor1 = _mm_mul_ps(determinant, cofactor1);
    cofactor2 = _mm_mul_ps(determinant, cofactor2);
    cofactor3 = _mm_mul_ps(determinant, cofactor3);
    mat4 my(cofactor0, cofactor1, cofactor2, cofactor3);
    return my;
  }

  mat4& mat4::invert() {
    mat4 trans = getTranspose();
    sseAlign(__m128 tmp);
    sseAlign(__m128 cofactor0);
    sseAlign(__m128 cofactor1);
    sseAlign(__m128 cofactor2);
    sseAlign(__m128 cofactor3);

    tmp = _mm_mul_ps(trans.mReg2, trans.mReg3);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    cofactor0 = _mm_mul_ps(trans.mReg1, tmp);
    cofactor1 = _mm_mul_ps(trans.mReg0, tmp);
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor0 = _mm_sub_ps(_mm_mul_ps(trans.mReg1, tmp), cofactor0);
    cofactor1 = _mm_sub_ps(_mm_mul_ps(trans.mReg0, tmp), cofactor1);
    cofactor1 = ws_sseShuffle(cofactor1, 2, 3, 0, 1);
    tmp = _mm_mul_ps(trans.mReg1, trans.mReg2);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    cofactor0 = _mm_add_ps(_mm_mul_ps(trans.mReg3, tmp), cofactor0);
    cofactor3 = _mm_mul_ps(trans.mReg0, tmp);
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor0 = _mm_sub_ps(cofactor0, _mm_mul_ps(trans.mReg3, tmp));
    cofactor3 = _mm_sub_ps(_mm_mul_ps(trans.mReg0, tmp), cofactor3);
    cofactor3 = ws_sseShuffle(cofactor3, 2, 3, 0, 1);
    tmp = _mm_mul_ps(ws_sseShuffle(trans.mReg1, 2, 3, 0, 1), trans.mReg3);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    ws_sseShuffle(trans.mReg2, 2, 3, 0, 1);
    cofactor0 = _mm_add_ps(_mm_mul_ps(trans.mReg2, tmp), cofactor0);
    cofactor2 = _mm_mul_ps(trans.mReg0, tmp);
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor0 = _mm_sub_ps(cofactor0, _mm_mul_ps(trans.mReg2, tmp));
    cofactor2 = _mm_sub_ps(_mm_mul_ps(trans.mReg0, tmp), cofactor2);
    cofactor2 = ws_sseShuffle(cofactor2, 2, 3, 0, 1);
    tmp = _mm_mul_ps(trans.mReg0, trans.mReg1);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    cofactor2 = _mm_add_ps(_mm_mul_ps(trans.mReg3, tmp), cofactor2);
    cofactor3 = _mm_sub_ps(_mm_mul_ps(trans.mReg2, tmp), cofactor3);
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor2 = _mm_sub_ps(_mm_mul_ps(trans.mReg3, tmp), cofactor2);
    cofactor3 = _mm_sub_ps(cofactor3, _mm_mul_ps(trans.mReg2, tmp));
    tmp = _mm_mul_ps(trans.mReg0, trans.mReg3);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    cofactor1 = _mm_sub_ps(cofactor1, _mm_mul_ps(trans.mReg2, tmp));
    cofactor2 = _mm_add_ps(_mm_mul_ps(trans.mReg1, tmp), cofactor2);
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor1 = _mm_add_ps(_mm_mul_ps(trans.mReg2, tmp), cofactor1);
    cofactor2 = _mm_sub_ps(cofactor2, _mm_mul_ps(trans.mReg1, tmp));
    tmp = _mm_mul_ps(trans.mReg0, trans.mReg2);
    tmp = ws_sseShuffle(tmp, 1, 0, 3, 2);
    cofactor1 = _mm_add_ps(_mm_mul_ps(trans.mReg3, tmp), cofactor1);
    cofactor3 = _mm_sub_ps(cofactor3, _mm_mul_ps(trans.mReg1, tmp));
    tmp = ws_sseShuffle(tmp, 2, 3, 0, 1);
    cofactor1 = _mm_sub_ps(cofactor1, _mm_mul_ps(trans.mReg3, tmp));
    cofactor3 = _mm_add_ps(_mm_mul_ps(trans.mReg1, tmp), cofactor3);

    sseAlign(__m128 determinant);
    determinant = _mm_mul_ps(trans.mReg0, cofactor0);
    determinant = _mm_add_ps(ws_sseShuffle(determinant, 2, 3, 0, 1), determinant);
    determinant = _mm_add_ss(ws_sseShuffle(determinant, 1, 0, 3, 2), determinant);
    tmp = _mm_rcp_ss(determinant);
    determinant = _mm_sub_ss(_mm_add_ss(tmp, tmp),
            _mm_mul_ss(determinant, _mm_mul_ss(tmp, tmp)));
    determinant = ws_sseDistributeX(determinant);
    mReg0 = _mm_mul_ps(determinant, cofactor0);
    mReg1 = _mm_mul_ps(determinant, cofactor1);
    mReg2 = _mm_mul_ps(determinant, cofactor2);
    mReg3 = _mm_mul_ps(determinant, cofactor3);
    return *this;
  }

  bool mat4::isIdentity() const {
    return (
      data[0] == 1.0f && data[1] == 0.0f && data[2] == 0.0f && data[3] == 0.0f
      && data[4] == 0.0f && data[5] == 1.0f && data[6] == 0.0f
      && data[7] == 0.0f && data[8] == 0.0f && data[9] == 0.0f
      && data[10] == 1.0f && data[11] == 0.0f
      && data[12] == 0.0f && data[13] == 0.0f
      && data[14] == 0.0f && data[15] == 1.0f);
  }

  mat4& mat4::loadIdentity() {
    mReg0 = MAT4_IDENTITY.mReg0;
    mReg1 = MAT4_IDENTITY.mReg1;
    mReg2 = MAT4_IDENTITY.mReg2;
    mReg3 = MAT4_IDENTITY.mReg3;
    return *this;
  }

  void mat4::print(u16 printLog) const {
    wsLog(printLog, "  [ %f %f %f %f ]\n"
            "    [ %f %f %f %f ]\n"
            "    [ %f %f %f %f ]\n"
            "    [ %f %f %f %f ]\n", data[0], data[1], data[2], data[3], data[4],
            data[5], data[6], data[7], data[8], data[9], data[10], data[11],
            data[12], data[13], data[14], data[15]);
  }

  mat4& mat4::setRotation(const quat& quaternion) {
    f32 xx = quaternion.x * quaternion.x;
    f32 xy = quaternion.x * quaternion.y;
    f32 xz = quaternion.x * quaternion.z;
    f32 xw = quaternion.x * quaternion.w;
    f32 yy = quaternion.y * quaternion.y;
    f32 yz = quaternion.y * quaternion.z;
    f32 yw = quaternion.y * quaternion.w;
    f32 zz = quaternion.z * quaternion.z;
    f32 zw = quaternion.z * quaternion.w;
    mat4 rotationMatrix(1 - 2 * (yy + zz), 2 * (xy + zw), 2 * (xz - yw), 0.0f,
              2 * (xy - zw), 1 - 2 * (xx + zz), 2 * (yz + xw), 0.0f,
              2 * (xz + yw), 2 * (yz - xw), 1 - 2 * (xx + yy), 0.0f);
    *this = rotationMatrix * *this;
    return *this;
  }

  mat4& mat4::setRotation(const vec4& axis, f32 angle) {
    quat quaternion(axis, angle);
    setRotation(quaternion);
    return *this;
  }

  mat4& mat4::setRotation(f32 axisX, f32 axisY, f32 axisZ, f32 angle) {
    setRotation(vec4(axisX, axisY, axisZ), angle);
    return *this;
  }

  mat4& mat4::setRotation(const vec4& euler) {
    vec4 vSin(wsSin(euler.x), wsSin(euler.y), wsSin(euler.z));
    vec4 vCos(wsCos(euler.x), wsCos(euler.y), wsCos(euler.z));
    data[0] = vCos.y * vCos.z;
    data[1] = vCos.y * vSin.z;
    data[2] = -vSin.y;
    data[4] = vSin.x * vSin.y * vCos.z - vCos.x * vSin.z;
    data[5] = vSin.x * vSin.y * vSin.z + vCos.x * vCos.z;
    data[6] = vSin.x * vCos.y;
    data[8] = vCos.x * vSin.y * vCos.z + vSin.x * vSin.z;
    data[9] = vCos.x * vSin.y * vSin.z - vSin.x * vCos.z;
    data[10] = vCos.x * vCos.y;
    return *this;
  }

  mat4& mat4::setScale(f32 scalar) {
    data[0] = scalar;
    data[5] = scalar;
    data[10] = scalar;
    return *this;
  }

  mat4& mat4::setScale(const vec4& scalar) {
    data[0] = scalar.x;
    data[5] = scalar.y;
    data[10] = scalar.z;
    return *this;
  }

  mat4& mat4::setScale(f32 scaleX, f32 scaleY, f32 scaleZ) {
    data[0] = scaleX;
    data[5] = scaleY;
    data[10] = scaleZ;
    return *this;
  }

  mat4& mat4::setTranslation(const vec4& trans) {
    data[12] = trans.x;
    data[13] = trans.y;
    data[14] = trans.z;
    return *this;
  }

  mat4& mat4::setTranslation(f32 transX, f32 transY, f32 transZ) {
    data[12] = transX;
    data[13] = transY;
    data[14] = transZ;
    return *this;
  }

  mat4& mat4::transpose() {
    *this = mat4(data[0], data[4], data[8], data[12], data[1], data[5], data[9], data[13],
            data[2], data[6], data[10], data[14], data[3], data[7], data[11],
            data[15]);
    return *this;
  }

  mat4 mat4::getTranspose() const {
    mat4 my(data[0], data[4], data[8], data[12], data[1], data[5], data[9], data[13],
            data[2], data[6], data[10], data[14], data[3], data[7], data[11],
            data[15]);
    return my;
  }

  mat4& mat4::operator+=(const mat4& other) {
    mReg0 = _mm_add_ps(mReg0, other.mReg0);
    mReg1 = _mm_add_ps(mReg1, other.mReg1);
    mReg2 = _mm_add_ps(mReg2, other.mReg2);
    mReg3 = _mm_add_ps(mReg3, other.mReg3);
    return *this;
  }

  mat4& mat4::operator-=(const mat4& other) {
    mReg0 = _mm_sub_ps(mReg0, other.mReg0);
    mReg1 = _mm_sub_ps(mReg1, other.mReg1);
    mReg2 = _mm_sub_ps(mReg2, other.mReg2);
    mReg3 = _mm_sub_ps(mReg3, other.mReg3);
    return *this;
  }

  mat4& mat4::operator*=(const mat4& other) {
    sseAlign(__m128 tmp);
    tmp = _mm_mul_ps(ws_sseDistributeX(mReg0), other.mReg0);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg0), other.mReg1, tmp);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg0), other.mReg2, tmp);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg0), other.mReg3, tmp);
    mReg0 = tmp;
    tmp = _mm_mul_ps(ws_sseDistributeX(mReg1), other.mReg0);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg1), other.mReg1, tmp);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg1), other.mReg2, tmp);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg1), other.mReg3, tmp);
    mReg1 = tmp;
    tmp = _mm_mul_ps(ws_sseDistributeX(mReg2), other.mReg0);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg2), other.mReg1, tmp);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg2), other.mReg2, tmp);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg2), other.mReg3, tmp);
    mReg2 = tmp;
    tmp = _mm_mul_ps(ws_sseDistributeX(mReg3), other.mReg0);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg3), other.mReg1, tmp);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg3), other.mReg2, tmp);
    tmp = ws_sseMadd(ws_sseDistributeY(mReg3), other.mReg3, tmp);
    mReg3 = tmp;
    return *this;
  }

  mat4& mat4::operator*=(f32 value) {
    sseAlign(__m128 scalar);
    scalar = _mm_set1_ps(value);
    mReg0 = _mm_mul_ps(mReg0, scalar);
    mReg1 = _mm_mul_ps(mReg1, scalar);
    mReg2 = _mm_mul_ps(mReg2, scalar);
    mReg3 = _mm_mul_ps(mReg3, scalar);
    return *this;
  }

  mat4& mat4::operator/=(f32 value) {
    sseAlign(__m128 scalar);
    scalar = _mm_set1_ps(value);
    mReg0 = _mm_div_ps(mReg0, scalar);
    mReg1 = _mm_div_ps(mReg1, scalar);
    mReg2 = _mm_div_ps(mReg2, scalar);
    mReg3 = _mm_div_ps(mReg3, scalar);
    return *this;
  }

  mat4 mat4::operator+(const mat4& other) const {
    mat4 my(_mm_add_ps(mReg0, other.mReg0), _mm_add_ps(mReg1, other.mReg1),
            _mm_add_ps(mReg2, other.mReg2), _mm_add_ps(mReg3, other.mReg3));
    return my;
  }

  mat4 mat4::operator-(const mat4& other) const {
    mat4 my(_mm_sub_ps(mReg0, other.mReg0), _mm_sub_ps(mReg1, other.mReg1),
            _mm_sub_ps(mReg2, other.mReg2), _mm_sub_ps(mReg3, other.mReg3));
    return my;
  }

  mat4 mat4::operator*(const mat4& other) const {
    sseAlign(__m128 tmp1);
    sseAlign(__m128 tmp2);
    sseAlign(__m128 tmp3);
    sseAlign(__m128 tmp4);
    tmp1 = _mm_mul_ps(ws_sseDistributeX(mReg0), other.mReg0);
    tmp1 = ws_sseMadd(ws_sseDistributeY(mReg0), other.mReg1, tmp1);
    tmp1 = ws_sseMadd(ws_sseDistributeY(mReg0), other.mReg2, tmp1);
    tmp1 = ws_sseMadd(ws_sseDistributeY(mReg0), other.mReg3, tmp1);
    tmp2 = _mm_mul_ps(ws_sseDistributeX(mReg1), other.mReg0);
    tmp2 = ws_sseMadd(ws_sseDistributeY(mReg1), other.mReg1, tmp2);
    tmp2 = ws_sseMadd(ws_sseDistributeY(mReg1), other.mReg2, tmp2);
    tmp2 = ws_sseMadd(ws_sseDistributeY(mReg1), other.mReg3, tmp2);
    tmp3 = _mm_mul_ps(ws_sseDistributeX(mReg2), other.mReg0);
    tmp3 = ws_sseMadd(ws_sseDistributeY(mReg2), other.mReg1, tmp3);
    tmp3 = ws_sseMadd(ws_sseDistributeY(mReg2), other.mReg2, tmp3);
    tmp3 = ws_sseMadd(ws_sseDistributeY(mReg2), other.mReg3, tmp3);
    tmp4 = _mm_mul_ps(ws_sseDistributeX(mReg3), other.mReg0);
    tmp4 = ws_sseMadd(ws_sseDistributeY(mReg3), other.mReg1, tmp4);
    tmp4 = ws_sseMadd(ws_sseDistributeY(mReg3), other.mReg2, tmp4);
    tmp4 = ws_sseMadd(ws_sseDistributeY(mReg3), other.mReg3, tmp4);
    mat4 my(tmp1, tmp2, tmp3, tmp4);
    return my;
  }

  mat4 mat4::operator*(f32 value) const {
    sseAlign(__m128 scalar);
    scalar = _mm_set1_ps(value);
    mat4 my(_mm_mul_ps(mReg0, scalar), _mm_mul_ps(mReg1, scalar), _mm_mul_ps(mReg2, scalar),
            _mm_mul_ps(mReg3, scalar));
    return my;
  }

  mat4 mat4::operator/(f32 value) const {
    sseAlign(__m128 scalar);
    scalar = _mm_set1_ps(value);
    mat4 my(_mm_div_ps(mReg0, scalar), _mm_div_ps(mReg1, scalar), _mm_div_ps(mReg2, scalar),
            _mm_div_ps(mReg3, scalar));
    return my;
  }

  mat4& mat4::operator=(const mat4& other) {
    mReg0 = other.mReg0;
    mReg1 = other.mReg1;
    mReg2 = other.mReg2;
    mReg3 = other.mReg3;
    return *this;
  }

  bool mat4::operator==(const mat4& other) const {
    return (
      data[0] == other.data[0] && data[1] == other.data[1]
      && data[2] == other.data[2] && data[3] == other.data[3]
      && data[4] == other.data[4] && data[5] == other.data[5]
      && data[6] == other.data[6] && data[7] == other.data[7]
      && data[8] == other.data[8] && data[9] == other.data[9]
      && data[10] == other.data[10]
      && data[11] == other.data[11]
      && data[12] == other.data[12]
      && data[13] == other.data[13]
      && data[14] == other.data[14]
      && data[15] == other.data[15]  );
  }

  bool mat4::operator!=(const mat4& other) const {
    return (
      data[0] != other.data[0] || data[1] != other.data[1]
      || data[2] != other.data[2] || data[3] != other.data[3]
      || data[4] != other.data[4] || data[5] != other.data[5]
      || data[6] != other.data[6] || data[7] != other.data[7]
      || data[8] != other.data[8] || data[9] != other.data[9]
      || data[10] != other.data[10]
      || data[11] != other.data[11]
      || data[12] != other.data[12]
      || data[13] != other.data[13]
      || data[14] != other.data[14]
      || data[15] != other.data[15]  );
  }

  vec4 mat4::getRow(u32 rowNum) const {
    u32 rowOffset = rowNum * 4;
    vec4 my(data[rowOffset], data[rowOffset + 1], data[rowOffset + 2], data[rowOffset + 3]);
    return my;
  }

  vec4 mat4::getCol(u32 colNum) const {
    vec4 my(data[colNum], data[colNum + 4], data[colNum + 8], data[colNum + 12]);
    return my;
  }

#else   //  If SSE4 is not supported...
mat4::mat4(f32 val0_0, f32 val0_1, f32 val0_2, f32 val0_3, f32 val1_0, f32 val1_1,
        f32 val1_2, f32 val1_3, f32 val2_0, f32 val2_1, f32 val2_2, f32 val2_3,
        f32 val3_0, f32 val3_1, f32 val3_2, f32 val3_3) {
  //  Set matrix values
  data[0] = val0_0;
  data[1] = val0_1;
  data[2] = val0_2;
  data[3] = val0_3;
  data[4] = val1_0;
  data[5] = val1_1;
  data[6] = val1_2;
  data[7] = val1_3;
  data[8] = val2_0;
  data[9] = val2_1;
  data[10] = val2_2;
  data[11] = val2_3;
  data[12] = val3_0;
  data[13] = val3_1;
  data[14] = val3_2;
  data[15] = val3_3;

}

mat4::mat4(const mat4& other) {
  //  Set matrix values
  //  Because the library is built for speed, for loops are averted.
  data[0] = other.data[0];
  data[1] = other.data[1];
  data[2] = other.data[2];
  data[3] = other.data[3];
  data[4] = other.data[4];
  data[5] = other.data[5];
  data[6] = other.data[6];
  data[7] = other.data[7];
  data[8] = other.data[8];
  data[9] = other.data[9];
  data[10] = other.data[10];
  data[11] = other.data[11];
  data[12] = other.data[12];
  data[13] = other.data[13];
  data[14] = other.data[14];
  data[15] = other.data[15];

}

f32 mat4::determinant() const {
  //  Longhand clearly wouldn't be preferable for human readability, but it is
  //  computationally faster.
  f32 my(data[3] * data[6] * data[9] * data[12] - data[2] * data[7]
          * data[9] * data[12] - data[3] * data[5] * data[10]
          * data[12] + data[1] * data[7] * data[10] * data[12]
          + data[2] * data[5] * data[11] * data[12] - data[1]
          * data[6] * data[11] * data[12] - data[3] * data[6]
          * data[8] * data[13] + data[2] * data[7] * data[8]
          * data[13] + data[3] * data[4] * data[10] * data[13]
          - data[0] * data[7] * data[10] * data[13] - data[2]
          * data[4] * data[11] * data[13] + data[0] * data[6]
          * data[11] * data[13] + data[3] * data[5] * data[8]
          * data[14] - data[1] * data[7] * data[8] * data[14]
          - data[3] * data[4] * data[9] * data[14] + data[0]
          * data[7] * data[9] * data[14] + data[1] * data[4]
          * data[11] * data[14] - data[0] * data[5] * data[11]
          * data[14] - data[2] * data[5] * data[8] * data[15]
          + data[1] * data[6] * data[8] * data[15] + data[2]
          * data[4] * data[9] * data[15] - data[0] * data[6]
          * data[9] * data[15] - data[1] * data[4] * data[10]
          * data[15] + data[0] * data[5] * data[10] * data[15]);

  return my;
  //  (whew!)
}

vec4 mat4::getRotation() const {
  vec4 vSin, vCos;
  vSin.y = -data[2];
  if (vSin.y >= 1.0f) {
    vCos.y = 0.0f;
  }
  else {
    vCos.y = wsSqrt(1.0f - vSin.y * vSin.y);
  }
  if (vCos.y) {
    vSin.x = data[6] / vCos.y;
    vCos.x = data[10] / vCos.y;
    vSin.z = data[1] / vCos.y;
    vCos.z = data[0] / vCos.y;
  }
  else {
    vSin.x = -data[9];
    vCos.x = data[5];
    vSin.z = 0.0f;
    vCos.z = 1.0f;
  }
  vec4 my(atan2(vSin.x, vCos.x), atan2(vSin.y, vCos.y), atan2(vSin.z, vCos.z));

  return my;
}

quat mat4::getRotationQuat() const {
  f32 my = 1.0f + data[0] + data[5] + data[10];
  if (my > 0.0f) {
    my = 2.0f * wsSqrt(my);
    return quat((data[9] - data[6]) / my, (data[2] - data[8]) / my,
            (data[4] - data[1]) / my, my / 4.0f);
  }
  if (data[0] > data[5] && data[0] > data[10]) {
    my = 2.0f * wsSqrt(1.0f + data[0] - data[5] - data[10]);
    return quat(my / 4.0f, (data[4] + data[1]) / my,
            (data[2] + data[8]) / my, (data[9] - data[6]) / my);
  }
  if (data[5] > data[10]) {
    my = 2.0f * wsSqrt(1.0f + data[5] - data[0] - data[10]);
    return quat((data[4] + data[1]) / my, my / 4.0f,
            (data[9] + data[6]) / my, (data[2] - data[8]) / my);
  }
  my = 2.0f * wsSqrt(1.0f + data[10] - data[0] - data[5]);
  quat myQ((data[2] + data[8]) / my, (data[9] + data[6]) / my, my / 4.0f,
          (data[4] - data[1]) / my);

  return myQ;
}

vec4 mat4::getTranslation() const {
  vec4 my(data[12], data[13], data[14]);

  return my;
}

mat4& mat4::invert() {
  //  Using Cramer's rule
  f32 cofact[24];//  cofactors
  mat4 trans = getTranspose();
  /* calculate pairs for first 8 elements (cofactors) */
  cofact[0] = trans.data[10] * trans.data[15];
  cofact[1] = trans.data[11] * trans.data[14];
  cofact[2] = trans.data[9] * trans.data[15];
  cofact[3] = trans.data[11] * trans.data[13];
  cofact[4] = trans.data[9] * trans.data[14];
  cofact[5] = trans.data[10] * trans.data[13];
  cofact[6] = trans.data[8] * trans.data[15];
  cofact[7] = trans.data[11] * trans.data[12];
  cofact[8] = trans.data[8] * trans.data[14];
  cofact[9] = trans.data[10] * trans.data[12];
  cofact[10] = trans.data[8] * trans.data[13];
  cofact[11] = trans.data[9] * trans.data[12];
  cofact[12] = trans.data[2] * trans.data[7];
  cofact[13] = trans.data[3] * trans.data[6];
  cofact[14] = trans.data[1] * trans.data[7];
  cofact[15] = trans.data[3] * trans.data[5];
  cofact[16] = trans.data[1] * trans.data[6];
  cofact[17] = trans.data[2] * trans.data[5];
  cofact[18] = trans.data[0] * trans.data[7];
  cofact[19] = trans.data[3] * trans.data[4];
  cofact[20] = trans.data[0] * trans.data[6];
  cofact[21] = trans.data[2] * trans.data[4];
  cofact[22] = trans.data[0] * trans.data[5];
  cofact[23] = trans.data[1] * trans.data[4];
  /* calculate first 8 elements (cofactors) */
  data[0]=cofact[0]*trans.data[5] + cofact[3]*trans.data[6] + cofact[4]*trans.data[7] -
  cofact[1]*trans.data[5] - cofact[2]*trans.data[6] - cofact[5]*trans.data[7];
  data[1]=cofact[1]*trans.data[4] + cofact[6]*trans.data[6] + cofact[9]*trans.data[7] -
  cofact[0]*trans.data[4] - cofact[7]*trans.data[6] - cofact[8]*trans.data[7];
  data[2]=cofact[2]*trans.data[4] + cofact[7]*trans.data[5] + cofact[10]*trans.data[7] -
  cofact[3]*trans.data[4] - cofact[6]*trans.data[5] - cofact[11]*trans.data[7];
  data[3]=cofact[5]*trans.data[4] + cofact[8]*trans.data[5] + cofact[11]*trans.data[6] -
  cofact[4]*trans.data[4] - cofact[9]*trans.data[5] - cofact[10]*trans.data[6];
  data[4]=cofact[1]*trans.data[1] + cofact[2]*trans.data[2] + cofact[5]*trans.data[3] -
  cofact[0]*trans.data[1] - cofact[3]*trans.data[2] - cofact[4]*trans.data[3];
  data[5]=cofact[0]*trans.data[0] + cofact[7]*trans.data[2] + cofact[8]*trans.data[3] -
  cofact[1]*trans.data[0] - cofact[6]*trans.data[2] - cofact[9]*trans.data[3];
  data[6]=cofact[3]*trans.data[0] + cofact[6]*trans.data[1] + cofact[11]*trans.data[3] -
  cofact[2]*trans.data[0] - cofact[7]*trans.data[1] - cofact[10]*trans.data[3];
  data[7]=cofact[4]*trans.data[0] + cofact[9]*trans.data[1] + cofact[10]*trans.data[2] -
  cofact[5]*trans.data[0] - cofact[8]*trans.data[1] - cofact[11]*trans.data[2];
  data[8]=cofact[12]*trans.data[13]+cofact[15]*trans.data[14]+cofact[16]*trans.data[15] -
  cofact[13]*trans.data[13]-cofact[14]*trans.data[14]-cofact[17]*trans.data[15];
  data[9]=cofact[13]*trans.data[12]+cofact[18]*trans.data[14]+cofact[21]*trans.data[15] -
  cofact[12]*trans.data[12]-cofact[19]*trans.data[14]-cofact[20]*trans.data[15];
  data[10]=cofact[14]*trans.data[12]+cofact[19]*trans.data[13]+cofact[22]*trans.data[15]-
  cofact[15]*trans.data[12]-cofact[18]*trans.data[13]-cofact[23]*trans.data[15];
  data[11]=cofact[17]*trans.data[12]+cofact[20]*trans.data[13]+cofact[23]*trans.data[14]-
  cofact[16]*trans.data[12]-cofact[21]*trans.data[13]-cofact[22]*trans.data[14];
  data[12]=cofact[14]*trans.data[10]+cofact[17]*trans.data[11]+cofact[13]*trans.data[9]-
  cofact[16]*trans.data[11]-cofact[12]*trans.data[9]-cofact[15]*trans.data[10];
  data[13]=cofact[20]*trans.data[11]+cofact[12]*trans.data[8]+cofact[19]*trans.data[10]-
  cofact[18]*trans.data[10]-cofact[21]*trans.data[11]-cofact[13]*trans.data[8];
  data[14]=cofact[18]*trans.data[9]+cofact[23]*trans.data[11]+cofact[15]*trans.data[8]-
  cofact[22]*trans.data[11]-cofact[14]*trans.data[8]-cofact[19]*trans.data[9];
  data[15]=cofact[22]*trans.data[10]+cofact[16]*trans.data[8]+cofact[21]*trans.data[9]-
  cofact[20]*trans.data[9]-cofact[23]*trans.data[10]-cofact[17]*trans.data[8];

  f32 determinant = trans.data[0]*data[0] + trans.data[1]*data[1] +
  trans.data[2]*data[2] + trans.data[3]*data[3];

  determinant = 1.0f / determinant;  //  Get the reciprocal of the determinant
  for (u32 i = 0; i < 16; ++i) {
    data[i] *= determinant;
  }

  return *this;
}

mat4 mat4::inverse() const {
  //  Using Cramer's rule
  f32 cofact[24];//  cofactors
  mat4 trans = getTranspose();
  /* calculate pairs for first 8 elements (cofactors) */
  cofact[0] = trans.data[10] * trans.data[15];
  cofact[1] = trans.data[11] * trans.data[14];
  cofact[2] = trans.data[9] * trans.data[15];
  cofact[3] = trans.data[11] * trans.data[13];
  cofact[4] = trans.data[9] * trans.data[14];
  cofact[5] = trans.data[10] * trans.data[13];
  cofact[6] = trans.data[8] * trans.data[15];
  cofact[7] = trans.data[11] * trans.data[12];
  cofact[8] = trans.data[8] * trans.data[14];
  cofact[9] = trans.data[10] * trans.data[12];
  cofact[10] = trans.data[8] * trans.data[13];
  cofact[11] = trans.data[9] * trans.data[12];
  cofact[12] = trans.data[2] * trans.data[7];
  cofact[13] = trans.data[3] * trans.data[6];
  cofact[14] = trans.data[1] * trans.data[7];
  cofact[15] = trans.data[3] * trans.data[5];
  cofact[16] = trans.data[1] * trans.data[6];
  cofact[17] = trans.data[2] * trans.data[5];
  cofact[18] = trans.data[0] * trans.data[7];
  cofact[19] = trans.data[3] * trans.data[4];
  cofact[20] = trans.data[0] * trans.data[6];
  cofact[21] = trans.data[2] * trans.data[4];
  cofact[22] = trans.data[0] * trans.data[5];
  cofact[23] = trans.data[1] * trans.data[4];
  /* calculate first 8 elements (cofactors) */
  mat4 my(cofact[0]*trans.data[5] + cofact[3]*trans.data[6] + cofact[4]*trans.data[7] -
          cofact[1]*trans.data[5] - cofact[2]*trans.data[6] - cofact[5]*trans.data[7],

          cofact[1]*trans.data[4] + cofact[6]*trans.data[6] + cofact[9]*trans.data[7] -
          cofact[0]*trans.data[4] - cofact[7]*trans.data[6] - cofact[8]*trans.data[7],

          cofact[2]*trans.data[4] + cofact[7]*trans.data[5] + cofact[10]*trans.data[7] -
          cofact[3]*trans.data[4] - cofact[6]*trans.data[5] - cofact[11]*trans.data[7],

          cofact[5]*trans.data[4] + cofact[8]*trans.data[5] + cofact[11]*trans.data[6] -
          cofact[4]*trans.data[4] - cofact[9]*trans.data[5] - cofact[10]*trans.data[6],

          cofact[1]*trans.data[1] + cofact[2]*trans.data[2] + cofact[5]*trans.data[3] -
          cofact[0]*trans.data[1] - cofact[3]*trans.data[2] - cofact[4]*trans.data[3],

          cofact[0]*trans.data[0] + cofact[7]*trans.data[2] + cofact[8]*trans.data[3] -
          cofact[1]*trans.data[0] - cofact[6]*trans.data[2] - cofact[9]*trans.data[3],

          cofact[3]*trans.data[0] + cofact[6]*trans.data[1] + cofact[11]*trans.data[3] -
          cofact[2]*trans.data[0] - cofact[7]*trans.data[1] - cofact[10]*trans.data[3],

          cofact[4]*trans.data[0] + cofact[9]*trans.data[1] + cofact[10]*trans.data[2] -
          cofact[5]*trans.data[0] - cofact[8]*trans.data[1] - cofact[11]*trans.data[2],

          cofact[12]*trans.data[13]+cofact[15]*trans.data[14]+cofact[16]*trans.data[15] -
          cofact[13]*trans.data[13]-cofact[14]*trans.data[14]-cofact[17]*trans.data[15],

          cofact[13]*trans.data[12]+cofact[18]*trans.data[14]+cofact[21]*trans.data[15] -
          cofact[12]*trans.data[12]-cofact[19]*trans.data[14]-cofact[20]*trans.data[15],

          cofact[14]*trans.data[12]+cofact[19]*trans.data[13]+cofact[22]*trans.data[15] -
          cofact[15]*trans.data[12]-cofact[18]*trans.data[13]-cofact[23]*trans.data[15],

          cofact[17]*trans.data[12]+cofact[20]*trans.data[13]+cofact[23]*trans.data[14] -
          cofact[16]*trans.data[12]-cofact[21]*trans.data[13]-cofact[22]*trans.data[14],

          cofact[14]*trans.data[10]+cofact[17]*trans.data[11]+cofact[13]*trans.data[9] -
          cofact[16]*trans.data[11]-cofact[12]*trans.data[9]-cofact[15]*trans.data[10],

          cofact[20]*trans.data[11]+cofact[12]*trans.data[8]+cofact[19]*trans.data[10] -
          cofact[18]*trans.data[10]-cofact[21]*trans.data[11]-cofact[13]*trans.data[8],

          cofact[18]*trans.data[9]+cofact[23]*trans.data[11]+cofact[15]*trans.data[8] -
          cofact[22]*trans.data[11]-cofact[14]*trans.data[8]-cofact[19]*trans.data[9],

          cofact[22]*trans.data[10]+cofact[16]*trans.data[8]+cofact[21]*trans.data[9] -
          cofact[20]*trans.data[9]-cofact[23]*trans.data[10]-cofact[17]*trans.data[8] );

  f32 determinant = trans.data[0]*my.data[0] + trans.data[1]*my.data[1] +
  trans.data[2]*my.data[2] + trans.data[3]*my.data[3];

  determinant = 1.0f / determinant;  //  Get the reciprocal of the determinant
  for (u32 i = 0; i < 16; ++i) {
    my.data[i] *= determinant;
  }

  return my;
}

bool mat4::isIdentity() const {
  bool my(data[0] == 1.0f && data[1] == 0.0f && data[2] == 0.0f && data[3]
          == 0.0f && data[4] == 0.0f && data[5] == 1.0f && data[6]
          == 0.0f && data[7] == 0.0f && data[8] == 0.0f && data[9]
          == 0.0f && data[10] == 1.0f && data[11] == 0.0f && data[12]
          == 0.0f && data[13] == 0.0f && data[14] == 0.0f && data[15]
          == 1.0f);

  return my;
}

mat4& mat4::loadIdentity() {
  data[0] = 1.0f;
  data[1] = 0.0f;
  data[2] = 0.0f;
  data[3] = 0.0f;
  data[4] = 0.0f;
  data[5] = 1.0f;
  data[6] = 0.0f;
  data[7] = 0.0f;
  data[8] = 0.0f;
  data[9] = 0.0f;
  data[10] = 1.0f;
  data[11] = 0.0f;
  data[12] = 0.0f;
  data[13] = 0.0f;
  data[14] = 0.0f;
  data[15] = 1.0f;

  return *this;
}

void mat4::print(u16 printLog) const {
  wsLog( printLog,
          "  [ %f %f %f %f ]\n"
          "    [ %f %f %f %f ]\n"
          "    [ %f %f %f %f ]\n"
          "    [ %f %f %f %f ]\n",
          data[0], data[1], data[2], data[3],
          data[4], data[5], data[6], data[7],
          data[8], data[9], data[10], data[11],
          data[12], data[13], data[14], data[15] );
}

mat4& mat4::setRotation(const quat& quaternion) {
  f32 xx = quaternion.x * quaternion.x;
  f32 xy = quaternion.x * quaternion.y;
  f32 xz = quaternion.x * quaternion.z;
  f32 xw = quaternion.x * quaternion.w;
  f32 yy = quaternion.y * quaternion.y;
  f32 yz = quaternion.y * quaternion.z;
  f32 yw = quaternion.y * quaternion.w;
  f32 zz = quaternion.z * quaternion.z;
  f32 zw = quaternion.z * quaternion.w;
  mat4 rotationMatrix(1 - 2 * (yy + zz), 2 * (xy + zw), 2 * (xz - yw), 0.0f,
            2 * (xy - zw), 1 - 2 * (xx + zz), 2 * (yz + xw), 0.0f,
            2 * (xz + yw), 2 * (yz - xw), 1 - 2 * (xx + yy), 0.0f);
  *this = rotationMatrix * *this;
  return *this;
}

mat4& mat4::setRotation(const vec4& axis, f32 angle) {
  quat quaternion(axis, angle);
  setRotation(quaternion);

  return *this;
}

mat4& mat4::setRotation(f32 axisX, f32 axisY, f32 axisZ, f32 angle) {
  setRotation(vec4(axisX, axisY, axisZ), angle);

  return *this;
}

mat4& mat4::setRotation(const vec4& euler) {
  vec4 vSin(wsSin(euler.x), wsSin(euler.y), wsSin(euler.z));
  vec4 vCos(wsCos(euler.x), wsCos(euler.y), wsCos(euler.z));
  data[0] = vCos.y * vCos.z;
  data[1] = vCos.y * vSin.z;
  data[2] = -vSin.y;
  data[4] = vSin.x * vSin.y * vCos.z - vCos.x * vSin.z;
  data[5] = vSin.x * vSin.y * vSin.z + vCos.x * vCos.z;
  data[6] = vSin.x * vCos.y;
  data[8] = vCos.x * vSin.y * vCos.z + vSin.x * vSin.z;
  data[9] = vCos.x * vSin.y * vSin.z - vSin.x * vCos.z;
  data[10] = vCos.x * vCos.y;

  return *this;
}

mat4& mat4::setScale(f32 scalar) {
  data[0] = scalar;
  data[5] = scalar;
  data[10] = scalar;

  return *this;
}

mat4& mat4::setScale(const vec4& scalar) {
  data[0] = scalar.x;
  data[5] = scalar.y;
  data[10] = scalar.z;

  return *this;
}

mat4& mat4::setScale(f32 scaleX, f32 scaleY, f32 scaleZ) {
  data[0] = scaleX;
  data[5] = scaleY;
  data[10] = scaleZ;

  return *this;
}

mat4& mat4::setTranslation(const vec4& trans) {
  data[12] = trans.x;
  data[13] = trans.y;
  data[14] = trans.z;

  return *this;
}

mat4& mat4::setTranslation(f32 transX, f32 transY, f32 transZ) {
  data[12] = transX;
  data[13] = transY;
  data[14] = transZ;

  return *this;
}

mat4& mat4::transpose() {
  *this = mat4(data[0], data[4], data[8], data[12], data[1], data[5],
          data[9], data[13], data[2], data[6], data[10], data[14],
          data[3], data[7], data[11], data[15]);

  return *this;
}

mat4 mat4::getTranspose() const {
  return mat4(data[0], data[4], data[8], data[12], data[1], data[5],
          data[9], data[13], data[2], data[6], data[10], data[14],
          data[3], data[7], data[11], data[15]);

  return *this;
}

mat4& mat4::operator+=(const mat4& other) {
  data[0] += other.data[0];
  data[1] += other.data[1];
  data[2] += other.data[2];
  data[3] += other.data[3];
  data[4] += other.data[4];
  data[5] += other.data[5];
  data[6] += other.data[6];
  data[7] += other.data[7];
  data[8] += other.data[8];
  data[9] += other.data[9];
  data[10] += other.data[10];
  data[11] += other.data[11];
  data[12] += other.data[12];
  data[13] += other.data[13];
  data[14] += other.data[14];
  data[15] += other.data[15];

  return *this;
}

mat4& mat4::operator-=(const mat4& other) {
  data[0] -= other.data[0];
  data[1] -= other.data[1];
  data[2] -= other.data[2];
  data[3] -= other.data[3];
  data[4] -= other.data[4];
  data[5] -= other.data[5];
  data[6] -= other.data[6];
  data[7] -= other.data[7];
  data[8] -= other.data[8];
  data[9] -= other.data[9];
  data[10] -= other.data[10];
  data[11] -= other.data[11];
  data[12] -= other.data[12];
  data[13] -= other.data[13];
  data[14] -= other.data[14];
  data[15] -= other.data[15];

  return *this;
}

mat4& mat4::operator*=(const mat4& other) {
  *this = mat4(
          data[0] * other.data[0] + data[1] * other.data[4]
          + data[2] * other.data[8] + data[3]
          * other.data[12],

          data[0] * other.data[1] + data[1] * other.data[5]
          + data[2] * other.data[9] + data[3]
          * other.data[13],

          data[0] * other.data[2] + data[1] * other.data[6]
          + data[2] * other.data[10] + data[3]
          * other.data[14],

          data[0] * other.data[3] + data[1] * other.data[7]
          + data[3] * other.data[11] + data[3]
          * other.data[15],

          data[4] * other.data[0] + data[5] * other.data[4]
          + data[6] * other.data[8] + data[7]
          * other.data[12],

          data[4] * other.data[1] + data[5] * other.data[5]
          + data[6] * other.data[9] + data[7]
          * other.data[13],

          data[4] * other.data[2] + data[5] * other.data[6]
          + data[6] * other.data[10] + data[7]
          * other.data[14],

          data[4] * other.data[3] + data[5] * other.data[7]
          + data[7] * other.data[11] + data[7]
          * other.data[15],

          data[8] * other.data[0] + data[9] * other.data[4]
          + data[10] * other.data[8] + data[11]
          * other.data[12],

          data[8] * other.data[1] + data[9] * other.data[5]
          + data[10] * other.data[9] + data[11]
          * other.data[13],

          data[8] * other.data[2] + data[9] * other.data[6]
          + data[10] * other.data[10] + data[11]
          * other.data[14],

          data[8] * other.data[3] + data[9] * other.data[7]
          + data[11] * other.data[11] + data[11]
          * other.data[15],

          data[12] * other.data[0] + data[13] * other.data[4]
          + data[14] * other.data[8] + data[15]
          * other.data[12],

          data[12] * other.data[1] + data[13] * other.data[5]
          + data[14] * other.data[9] + data[15]
          * other.data[13],

          data[12] * other.data[2] + data[13] * other.data[6]
          + data[14] * other.data[10] + data[15]
          * other.data[14],

          data[12] * other.data[3] + data[13] * other.data[7]
          + data[15] * other.data[11] + data[15]
          * other.data[15]);

  return *this;
}

mat4& mat4::operator*=(f32 value) {
  data[0] *= value;
  data[1] *= value;
  data[2] *= value;
  data[3] *= value;
  data[4] *= value;
  data[5] *= value;
  data[6] *= value;
  data[7] *= value;
  data[8] *= value;
  data[9] *= value;
  data[10] *= value;
  data[11] *= value;
  data[12] *= value;
  data[13] *= value;
  data[14] *= value;
  data[15] *= value;

  return *this;
}

mat4& mat4::operator/=(f32 value) {
  data[0] /= value;
  data[1] /= value;
  data[2] /= value;
  data[3] /= value;
  data[4] /= value;
  data[5] /= value;
  data[6] /= value;
  data[7] /= value;
  data[8] /= value;
  data[9] /= value;
  data[10] /= value;
  data[11] /= value;
  data[12] /= value;
  data[13] /= value;
  data[14] /= value;
  data[15] /= value;

  return *this;
}

mat4 mat4::operator+(const mat4& other) const {
  mat4 my(data[0] + other.data[0], data[1] + other.data[1],
          data[2] + other.data[2], data[3] + other.data[3],
          data[4] + other.data[4], data[5] + other.data[5],
          data[6] + other.data[6], data[7] + other.data[7],
          data[8] + other.data[8], data[9] + other.data[9],
          data[10] + other.data[10], data[11] + other.data[11],
          data[12] + other.data[12], data[13] + other.data[13],
          data[14] + other.data[14], data[15] + other.data[15]);

  return my;
}

mat4 mat4::operator-(const mat4& other) const {
  mat4 my(data[0] - other.data[0], data[1] - other.data[1],
          data[2] - other.data[2], data[3] - other.data[3],
          data[4] - other.data[4], data[5] - other.data[5],
          data[6] - other.data[6], data[7] - other.data[7],
          data[8] - other.data[8], data[9] - other.data[9],
          data[10] - other.data[10], data[11] - other.data[11],
          data[12] - other.data[12], data[13] - other.data[13],
          data[14] - other.data[14], data[15] - other.data[15]);

  return my;
}

mat4 mat4::operator*(const mat4& other) const {
  mat4 my(
          data[0] * other.data[0] + data[1] * other.data[4]
          + data[2] * other.data[8] + data[3]
          * other.data[12],

          data[0] * other.data[1] + data[1] * other.data[5]
          + data[2] * other.data[9] + data[3]
          * other.data[13],

          data[0] * other.data[2] + data[1] * other.data[6]
          + data[2] * other.data[10] + data[3]
          * other.data[14],

          data[0] * other.data[3] + data[1] * other.data[7]
          + data[3] * other.data[11] + data[3]
          * other.data[15],

          data[4] * other.data[0] + data[5] * other.data[4]
          + data[6] * other.data[8] + data[7]
          * other.data[12],

          data[4] * other.data[1] + data[5] * other.data[5]
          + data[6] * other.data[9] + data[7]
          * other.data[13],

          data[4] * other.data[2] + data[5] * other.data[6]
          + data[6] * other.data[10] + data[7]
          * other.data[14],

          data[4] * other.data[3] + data[5] * other.data[7]
          + data[7] * other.data[11] + data[7]
          * other.data[15],

          data[8] * other.data[0] + data[9] * other.data[4]
          + data[10] * other.data[8] + data[11]
          * other.data[12],

          data[8] * other.data[1] + data[9] * other.data[5]
          + data[10] * other.data[9] + data[11]
          * other.data[13],

          data[8] * other.data[2] + data[9] * other.data[6]
          + data[10] * other.data[10] + data[11]
          * other.data[14],

          data[8] * other.data[3] + data[9] * other.data[7]
          + data[11] * other.data[11] + data[11]
          * other.data[15],

          data[12] * other.data[0] + data[13] * other.data[4]
          + data[14] * other.data[8] + data[15]
          * other.data[12],

          data[12] * other.data[1] + data[13] * other.data[5]
          + data[14] * other.data[9] + data[15]
          * other.data[13],

          data[12] * other.data[2] + data[13] * other.data[6]
          + data[14] * other.data[10] + data[15]
          * other.data[14],

          data[12] * other.data[3] + data[13] * other.data[7]
          + data[15] * other.data[11] + data[15]
          * other.data[15]);

  return my;
}

mat4 mat4::operator*(f32 value) const {
  mat4 my(data[0] * value, data[1] * value, data[2] * value,
          data[3] * value, data[4] * value, data[5] * value,
          data[6] * value, data[7] * value, data[8] * value,
          data[9] * value, data[10] * value, data[11] * value,
          data[12] * value, data[13] * value, data[14] * value,
          data[15] * value);

  return my;
}

mat4 mat4::operator/(f32 value) const {
  mat4 my(data[0] / value, data[1] / value, data[2] / value,
          data[3] / value, data[4] / value, data[5] / value,
          data[6] / value, data[7] / value, data[8] / value,
          data[9] / value, data[10] / value, data[11] / value,
          data[12] / value, data[13] / value, data[14] / value,
          data[15] / value);

  return my;
}

mat4& mat4::operator=(const mat4& other) {
  data[0] = other.data[0];
  data[1] = other.data[1];
  data[2] = other.data[2];
  data[3] = other.data[3];
  data[4] = other.data[4];
  data[5] = other.data[5];
  data[6] = other.data[6];
  data[7] = other.data[7];
  data[8] = other.data[8];
  data[9] = other.data[9];
  data[10] = other.data[10];
  data[11] = other.data[11];
  data[12] = other.data[12];
  data[13] = other.data[13];
  data[14] = other.data[14];
  data[15] = other.data[15];

  return *this;
}

bool mat4::operator==(const mat4& other) const {
  bool my(data[0] == other.data[0] && data[1] == other.data[1] && data[2]
          == other.data[2] && data[3] == other.data[3] && data[4]
          == other.data[4] && data[5] == other.data[5] && data[6]
          == other.data[6] && data[7] == other.data[7] && data[8]
          == other.data[8] && data[9] == other.data[9] && data[10]
          == other.data[10] && data[11] == other.data[11] && data[12]
          == other.data[12] && data[13] == other.data[13] && data[14]
          == other.data[14] && data[15] == other.data[15]);

  return my;
}

bool mat4::operator!=(const mat4& other) const {
  bool my(data[0] != other.data[0] || data[1] != other.data[1] || data[2]
          != other.data[2] || data[3] != other.data[3] || data[4]
          != other.data[4] || data[5] != other.data[5] || data[6]
          != other.data[6] || data[7] != other.data[7] || data[8]
          != other.data[8] || data[9] != other.data[9] || data[10]
          != other.data[10] || data[11] != other.data[11] || data[12]
          != other.data[12] || data[13] != other.data[13] || data[14]
          != other.data[14] || data[15] != other.data[15]);

  return my;
}

vec4 mat4::getRow(u32 rowNum) const {
  u32 rowOffset = rowNum*4;
  vec4 my( data[rowOffset],
          data[rowOffset+1],
          data[rowOffset+2],
          data[rowOffset+3] );

  return my;
}

vec4 mat4::getCol(u32 colNum) const {
  vec4 my( data[colNum],
          data[colNum+4],
          data[colNum+8],
          data[colNum+12] );

  return my;
}

#endif  /*  WS_SUPPORTS_SSE4  */

