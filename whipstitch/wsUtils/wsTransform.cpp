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
  return wsTransform(scale*other.scale, rotation*other.rotation,
    translationX+other.translationX, translationY+other.translationY, translationZ+other.translationZ);
}

wsTransform& wsTransform::operator+=(const vec4& other) {
  translationX += other.x;
  translationY += other.y;
  translationZ += other.z;
  return *this;
}

wsTransform wsTransform::operator+(const vec4& other) const {
  return wsTransform(scale, rotation, translationX+other.x, translationY+other.y, translationZ+other.z);
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
  wsEcho(  printLog,
              "Transform Values:\n"
              "    Scale = %f\n"
              "    Quaternion = { %f, %f, %f, %f }\n"
              "    Translation = { %f, %f, %f }\n",
              scale, rotation.x, rotation.y, rotation.z, rotation.w, translationX, translationY, translationZ);
}

