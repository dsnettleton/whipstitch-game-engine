/*
 * quat.cpp
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file implements the quat structure, which is used for
 *      quaternion operations in conjunction with the vec4 vector struct. Quaternions
 *      are used primarily for rotations in 3D space. They are essentially a normalized
 *      4D vector, containing an x,y,z axis value, and a w rotation value. They can
 *      be conveniently modified and interpolated with each other, and do not suffer
 *      from gimbal lock.
 *
 *      One version is provided using standard C operations, the other
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

#include "quat.h"
#include "vec4.h"

#if WS_SUPPORTS_SSE4 == WS_TRUE

    #define WS_FLIP_QUAT(q) \
        q = _mm_xor_ps(q, _mm_setr_ps(0, 0, 0, 0x80000000))

    //  Constructors

    quat::quat(vec4 axis, f32 angle) {
        WS_PROFILE();
        axis.normalize();
        mReg = _mm_mul_ps( _mm_set_ps(0.0f, axis.z, axis.y, axis.x),
                        _mm_set1_ps(wsSin(angle/2.0f)) );
        w = wsCos(angle/2.0f);

    }
    quat::quat(const vec4& euler) {
        WS_PROFILE();
        quat rot_x(vec4(1.0f, 0.0f, 0.0f), euler.x);
        quat rot_y(vec4(0.0f, 1.0f, 0.0f), euler.y);
        quat rot_z(vec4(0.0f, 0.0f, 1.0f), euler.z);
        rot_z *= rot_y;
        rot_z *= rot_x;
        *this = rot_z;

    }

    //  Operational Member Functions

    quat& quat::set(f32 myX, f32 myY, f32 myZ, f32 myW) {
        WS_PROFILE();
        mReg = _mm_set_ps(myW, myZ, myY, myX);

        return *this;
    }

    quat quat::getConjugate() const {
        WS_PROFILE();
        quat my(-x, -y, -z, w);

        return my;
    }

    quat& quat::conjugate() {
        WS_PROFILE();
        mReg = _mm_mul_ps(mReg, _mm_set_ps(1.0f, -1.0f, -1.0f, -1.0f));

        return *this;
    }

    //  For our purposes (using normalized quaternions), the inverse and
    //  conjugate are the same.
    quat quat::getInverse() const {
        WS_PROFILE();
        quat my(-x, -y, -z, w);

        return my;
    }

    quat& quat::invert() {
        WS_PROFILE();
        mReg = _mm_mul_ps(mReg, _mm_set_ps(1.0f, -1.0f, -1.0f, -1.0f));

        return *this;
    }

    //  Returns the angle between the two quaternions
    f32 quat::getAngle(const quat& other) const {
        WS_PROFILE();
        f32 my = wsArcCos( x*other.x + y*other.y + z*other.z + w*other.w );

        return my;
    }

    //  Uses Slerp, returns the blended quaternion
    quat quat::blend(const quat& other, f32 blendFactor) const {
        WS_PROFILE();
        f32 theta = getAngle(other);
        f32 blendoA = wsSin((1.0f-blendFactor)*theta) / wsSin(theta);
        f32 blendoB = wsSin(blendFactor*theta) / wsSin(theta);
        quat my(_mm_add_ps( _mm_mul_ps( _mm_set1_ps(blendoA), mReg ),
                            _mm_mul_ps( _mm_set1_ps(blendoB), other.mReg ) ) );

        return my;
    }

    //Uses slerp, sets this to blended quaternion
    quat& quat::toBlend(const quat& other, f32 blendFactor) {
        WS_PROFILE();
        f32 theta = getAngle(other);
        f32 blendoA = wsSin((1.0f-blendFactor)*theta) / wsSin(theta);
        f32 blendoB = wsSin(blendFactor*theta) / wsSin(theta);
        mReg =  _mm_add_ps( _mm_mul_ps( _mm_set1_ps(blendoA), mReg ),
                            _mm_mul_ps( _mm_set1_ps(blendoB), other.mReg ) );

        return *this;
    }

    quat& quat::setRotation(vec4 axis, f32 angle) {
        WS_PROFILE();
        axis.normalize();
        mReg = _mm_mul_ps( _mm_set_ps(0.0f, axis.z, axis.y, axis.x),
                        _mm_set1_ps(wsSin(angle/2.0f)) );
        w = wsCos(angle/2.0f);

        return *this;
    }

    quat& quat::setRotation(f32 axisX, f32 axisY, f32 axisZ, f32 angle) {
        WS_PROFILE();
        setRotation(vec4(axisX, axisY, axisZ), angle);

        return *this;
    }

    quat& quat::setRotationZXY(const vec4& euler) {
        WS_PROFILE();
        quat rot_x(vec4(1.0f, 0.0f, 0.0f), euler.x);
        quat rot_y(vec4(0.0f, 1.0f, 0.0f), euler.y);
        quat rot_z(vec4(0.0f, 0.0f, 1.0f), euler.z);
        rot_z *= rot_y;
        rot_z *= rot_x;
        *this = rot_z;

        return *this;
    }

    quat quat::operator*(const quat& other) const {
        WS_PROFILE();
        /*
        __m128 opOne = _mm_mul_ps( mReg, ws_sseDistributeW(other.mReg));
        __m128 opTwo = _mm_mul_ps( ws_sseShuffle(mReg, 2, 0, 1, 0),
                        ws_sseShuffle(other.mReg, 1, 2, 0, 0));
        __m128 opThree = _mm_mul_ps(ws_sseShuffle(mReg, 3, 3, 3, 1),
                        ws_sseShuffle(other.mReg, 0, 1, 2, 1));
        __m128 opFour = _mm_mul_ps( ws_sseShuffle(mReg, 1, 2, 0, 2),
                        ws_sseShuffle(other.mReg, 2, 0, 1, 2));
        WS_FLIP_QUAT(opThree);
        WS_FLIP_QUAT(opFour);
        return quat(_mm_add_ps(_mm_sub_ps(opOne, opTwo), _mm_add_ps(opThree, opFour)));
        /*/
        quat my(w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y + y * other.w + z * other.x - x * other.z,
                w * other.z + z * other.w + x * other.y - y * other.x,
                w * other.w - x * other.x - y * other.y - z * other.x);

        return my;
        //*/
    }

    quat& quat::operator*=(const quat& other) {
        WS_PROFILE();
        /*
        __m128 opOne = _mm_mul_ps( mReg, ws_sseShuffle(other.mReg, 3, 3, 3, 3));
        __m128 opTwo = _mm_mul_ps( ws_sseShuffle(mReg, 2, 0, 1, 0),
                        ws_sseShuffle(other.mReg, 1, 2, 0, 0));
        __m128 opThree = _mm_mul_ps(ws_sseShuffle(mReg, 3, 3, 3, 1),
                        ws_sseShuffle(other.mReg, 0, 1, 2, 1));
        __m128 opFour = _mm_mul_ps( ws_sseShuffle(mReg, 1, 2, 0, 2),
                        ws_sseShuffle(other.mReg, 2, 0, 1, 2));
        WS_FLIP_QUAT(opThree);
        WS_FLIP_QUAT(opFour);
        mReg = _mm_add_ps(_mm_sub_ps(opOne, opTwo), _mm_add_ps(opThree, opFour));
        return *this;
        /*/
        set(w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y + y * other.w + z * other.x - x * other.z,
            w * other.z + z * other.w + x * other.y - y * other.x,
            w * other.w - x * other.x - y * other.y - z * other.x);

        return *this;
        //*/
    }

    quat& quat::operator=(const quat& other) {
        WS_PROFILE();
        mReg = other.mReg;

        return *this;
    }

    void quat::print(u16 printLog) const {
        wsLog(printLog, "{ %f, %f, %f, %f }\n", x, y, z, w);
    }

#else   //  If this does not support SSE4

    //  Constructors

    quat& quat::set(f32 myX, f32 myY, f32 myZ, f32 myW) {
        WS_PROFILE();
        x = myX;
        y = myY;
        z = myZ;
        w = myW;

        return *this;
    }

    quat::quat(vec4 axis, f32 angle) {
        WS_PROFILE();
        axis.normalize();
        f32 sineage = wsSin(angle/2.0f);
        x = axis.x * sineage;
        y = axis.y * sineage;
        z = axis.z * sineage;
        w = wsCos(angle/2.0f);

    }

    quat::quat(const vec4& euler) {
        WS_PROFILE();
        quat rot_x(vec4(1.0f, 0.0f, 0.0f, 1.0f), euler.x);
        quat rot_y(vec4(0.0f, 1.0f, 0.0f, 1.0f), euler.y);
        quat rot_z(vec4(0.0f, 0.0f, 1.0f, 1.0f), euler.z);
        rot_z *= rot_y;
        rot_z *= rot_x;
        *this = rot_z;

    }

    quat::quat(const quat& other) {
        WS_PROFILE();
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;

    }

    //  Operational Member Functions
    quat quat::getConjugate() const {
        WS_PROFILE();
        quat my(-x, -y, -z, w);

        return my;
    }

    quat& quat::conjugate() {
        WS_PROFILE();
        x *= -1.0f;
        y *= -1.0f;
        z *= -1.0f;

        return *this;
    }

    //  For our purposes (using normalized quaternions), the inverse and
    //  conjugate are the same.
    quat quat::getInverse() const {
        WS_PROFILE();
        return quat(-x, -y, -z, w);
    }

    quat& quat::invert() {
        WS_PROFILE();
        x *= -1.0f;
        y *= -1.0f;
        z *= -1.0f;
        return *this;
    }

    //  Returns the angle between the two quaternions
    f32 quat::getAngle(const quat& other) const {
        WS_PROFILE();
        return wsArcCos( x*other.x + y*other.y + z*other.z + w*other.w );
    }

    f32 quat::dotProduct(const quat& other) const {
        WS_PROFILE();
        return f32(x*other.x + y*other.y + z*other.z + w*other.w);
    }

    //  Uses Slerp, returns the blended quaternion
    quat quat::blend(const quat& other, f32 blendFactor) const {
        WS_PROFILE();
        /*  //  TOGGLE - SLERP
        f32 theta = getAngle(other);
        f32 blendoA = wsSin((1.0f-blendFactor)*theta);
        f32 blendoB = wsSin(blendFactor*theta);
        return quat(    (blendoA*x + blendoB*other.x) / wsSin(theta),
                        (blendoA*y + blendoB*other.y) / wsSin(theta),
                        (blendoA*z + blendoB*other.z) / wsSin(theta),
                        (blendoA*w + blendoB*other.w) / wsSin(theta) );
        /*/ //  TOGGLE - LERP
        return quat(wsLerp(x, other.x, blendFactor),
                    wsLerp(y, other.y, blendFactor),
                    wsLerp(z, other.z, blendFactor),
                    wsLerp(w, other.w, blendFactor) );
        //*///! spherical linear interpolation
        /*
        f32 dot = dotProduct(other);
        quat third(other);
        if (dot < 0) {
            dot *= -1;
            third *= -1;
        }
        f32 theta = wsArcCos(dot);
        return (*this * wsSin(angle*(1.0-blendFactor)) + third*wsSin(angle*blendFactor))/wsSin(angle);//*/
    }

    //Uses slerp, sets this to blended quaternion
    quat& quat::toBlend(const quat& other, f32 blendFactor) {
        WS_PROFILE();
        /*  //  TOGGLE - SLERP
        f32 theta = getAngle(other);
        f32 blendoA = wsSin((1.0f-blendFactor)*theta) / wsSin(theta);
        f32 blendoB = wsSin(blendFactor*theta) / wsSin(theta);
        x =  blendoA*x + blendoB*other.x;
        y =  blendoA*y + blendoB*other.y;
        z =  blendoA*z + blendoB*other.z;
        w =  blendoA*w + blendoB*other.w;
        /*/ //  TOGGLE - LERP
        x = wsLerp(x, other.x, blendFactor);
        y = wsLerp(y, other.y, blendFactor);
        z = wsLerp(z, other.z, blendFactor);
        w = wsLerp(w, other.w, blendFactor);
        //*/

        return *this;
    }

    quat& quat::setRotation(vec4 axis, f32 angle) {
        WS_PROFILE();
        axis.normalize();
        f32 sineage = wsSin(angle / 2.0f);
        x = axis.x * sineage;
        y = axis.y * sineage;
        z = axis.z * sineage;
        w = wsCos(angle / 2.0f);

        return *this;
    }

    quat& quat::setRotation(f32 axisX, f32 axisY, f32 axisZ, f32 angle) {
        WS_PROFILE();
        setRotation(vec4(axisX, axisY, axisZ), angle);

        return *this;
    }

    quat& quat::setRotationZXY(const vec4& euler) {
        WS_PROFILE();
        quat rot_x(vec4(1.0f, 0.0f, 0.0f, 1.0f), euler.x);
        quat rot_y(vec4(0.0f, 1.0f, 0.0f, 1.0f), euler.y);
        quat rot_z(vec4(0.0f, 0.0f, 1.0f, 1.0f), euler.z);
        rot_z *= rot_y;
        rot_z *= rot_x;
        *this = rot_z;

        return *this;
    }

    quat quat::operator-() const {
        WS_PROFILE();
        return quat(-x, -y, -z, w);
    }

    quat quat::operator*(const quat& other) const {
        WS_PROFILE();
        return quat(w*other.x + x*other.w + y*other.z - z*other.y,
                    w*other.y - x*other.z + y*other.w + z*other.x,
                    w*other.z + x*other.y - y*other.x + z*other.w,
                    w*other.w - x*other.x - y*other.y - z*other.z);
    }

    quat& quat::operator*=(const quat& other) {
        WS_PROFILE();
        *this =  quat(  w*other.x + x*other.w + y*other.z - z*other.y,
                        w*other.y - x*other.z + y*other.w + z*other.x,
                        w*other.z + x*other.y - y*other.x + z*other.w,
                        w*other.w - x*other.x - y*other.y - z*other.z);
        return *this;
    }

    quat& quat::operator=(const quat& other) {
        WS_PROFILE();
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;

        return *this;
    }

    quat quat::normal() const {
        f32 magSquared = x*x + y*y + z*z + w*w;
        quat my(*this);
        if (magSquared) {
            f32 mag = wsSqrt(magSquared);
            my.x /= mag;
            my.y /= mag;
            my.z /= mag;
            my.w /= mag;
        }

        return my;
    }

    quat& quat::normalize() {
        f32 magSquared = x*x + y*y + z*z + w*w;
        if (magSquared) {
            f32 mag = wsSqrt(magSquared);
            x /= mag;
            y /= mag;
            z /= mag;
            w /= mag;
        }

        return *this;
    }

    void quat::print(u16 printLog) const {
        wsLog(printLog, "{ %f, %f, %f, %f }\n", x, y, z, w);
    }

#endif /*   WS_SUPPORTS_SSE4    */
