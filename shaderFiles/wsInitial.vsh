#version 130
//  wsInitial.vsh
//  D. Scott Nettleton
//  1/22/2013
//  Initial Vertex Shader for the Whipstitch Game Engine
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//	Copyright D. Scott Nettleton, 2013
//	This software is released under the terms of the
//	Lesser GNU Public License (LGPL).

varying vec3 vertPos;
varying vec3 vertNorm;
varying vec2 texCoords;

uniform vec4 baseBoneLocs[64];
uniform vec4 baseBoneRots[64];
uniform vec4 boneLocs[64];
uniform vec4 boneRots[64];

attribute int numWeights;
attribute vec4 jointIndex;
attribute vec4 jointIndex2; //  For numWeights > 4
attribute vec4 influence;
attribute vec4 influence2;  //  For numWeights > 4

invariant gl_Position;

vec4 quatInvert(vec4 quat) {
  quat.xyz *= -1;
  return quat;
}

vec4 quatMultiply(vec4 quat1, vec4 quat2) {
  return vec4(quat1.w*quat2.x + quat1.x*quat2.w + quat1.y*quat2.z - quat1.z*quat2.y,
              quat1.w*quat2.y - quat1.x*quat2.z + quat1.y*quat2.w + quat1.z*quat2.x,
              quat1.w*quat2.z + quat1.x*quat2.y - quat1.y*quat2.x + quat1.z*quat2.w,
              quat1.w*quat2.w - quat1.x*quat2.x - quat1.y*quat2.y - quat1.z*quat2.z);
}

vec4 rotateByQuat(vec4 vector, vec4 quat) {
  float tmpW = vector.w;
  vector.w = 0;
  vector = quatMultiply( quatMultiply(quat, vector), quatInvert(quat) );
  vector.w = tmpW;
  return vector;
}

void main() {
  //  Animate!
  int index;
  vec4 position;
  vec3 normal;
  vec3 posSum = vec3(0.0f, 0.0f, 0.0f);
  vec3 normSum = vec3(0.0f, 0.0f, 0.0f);
  vec4 myPos;
  vec4 myNorm;
  if (numWeights > 0) {
    index = int(jointIndex.x);
    myPos = gl_Vertex - baseBoneLocs[index];
    myPos = rotateByQuat(myPos, quatInvert(baseBoneRots[index]));
    myPos = rotateByQuat(myPos, boneRots[index]);
    myPos += boneLocs[index];
    posSum += myPos.xyz * influence.x;
    myNorm = vec4(gl_Normal, 0.0);
    myNorm = rotateByQuat(myNorm, quatInvert(baseBoneRots[index]));
    myNorm = rotateByQuat(myNorm, boneRots[index]);
    normSum += myNorm.xyz * influence.x;
  }
  else {
    posSum = gl_Vertex.xyz;
    normSum = gl_Normal;
  }
  if (numWeights > 1) {
    index = int(jointIndex.y);
    myPos = gl_Vertex - baseBoneLocs[index];
    myPos = rotateByQuat(myPos, quatInvert(baseBoneRots[index]));
    myPos = rotateByQuat(myPos, boneRots[index]);
    myPos += boneLocs[index];
    posSum += myPos.xyz * influence.y;
    myNorm = vec4(gl_Normal, 0.0);
    myNorm = rotateByQuat(myNorm, quatInvert(baseBoneRots[index]));
    myNorm = rotateByQuat(myNorm, boneRots[index]);
    normSum += myNorm.xyz * influence.y;
  }
  if (numWeights > 2) {
    index = int(jointIndex.z);
    myPos = gl_Vertex - baseBoneLocs[index];
    myPos = rotateByQuat(myPos, quatInvert(baseBoneRots[index]));
    myPos = rotateByQuat(myPos, boneRots[index]);
    myPos += boneLocs[index];
    posSum += myPos.xyz * influence.z;
    myNorm = vec4(gl_Normal, 0.0);
    myNorm = rotateByQuat(myNorm, quatInvert(baseBoneRots[index]));
    myNorm = rotateByQuat(myNorm, boneRots[index]);
    normSum += myNorm.xyz * influence.z;
  }
  if (numWeights > 3) {
    index = int(jointIndex.w);
    myPos = gl_Vertex - baseBoneLocs[index];
    myPos = rotateByQuat(myPos, quatInvert(baseBoneRots[index]));
    myPos = rotateByQuat(myPos, boneRots[index]);
    myPos += boneLocs[index];
    posSum += myPos.xyz * influence.w;
    myNorm = vec4(gl_Normal, 0.0);
    myNorm = rotateByQuat(myNorm, quatInvert(baseBoneRots[index]));
    myNorm = rotateByQuat(myNorm, boneRots[index]);
    normSum += myNorm.xyz * influence.w;
  }
  if (numWeights > 4) {
    index = int(jointIndex2.x);
    myPos = gl_Vertex - baseBoneLocs[index];
    myPos = rotateByQuat(myPos, quatInvert(baseBoneRots[index]));
    myPos = rotateByQuat(myPos, boneRots[index]);
    myPos += boneLocs[index];
    posSum += myPos.xyz * influence2.x;
    myNorm = vec4(gl_Normal, 0.0);
    myNorm = rotateByQuat(myNorm, quatInvert(baseBoneRots[index]));
    myNorm = rotateByQuat(myNorm, boneRots[index]);
    normSum += myNorm.xyz * influence2.x;
  }
  if (numWeights > 5) {
    index = int(jointIndex2.y);
    myPos = gl_Vertex - baseBoneLocs[index];
    myPos = rotateByQuat(myPos, quatInvert(baseBoneRots[index]));
    myPos = rotateByQuat(myPos, boneRots[index]);
    myPos += boneLocs[index];
    posSum += myPos.xyz * influence2.y;
    myNorm = vec4(gl_Normal, 0.0);
    myNorm = rotateByQuat(myNorm, quatInvert(baseBoneRots[index]));
    myNorm = rotateByQuat(myNorm, boneRots[index]);
    normSum += myNorm.xyz * influence2.y;
  }
  if (numWeights > 6) {
    index = int(jointIndex2.z);
    myPos = gl_Vertex - baseBoneLocs[index];
    myPos = rotateByQuat(myPos, quatInvert(baseBoneRots[index]));
    myPos = rotateByQuat(myPos, boneRots[index]);
    myPos += boneLocs[index];
    posSum += myPos.xyz * influence2.z;
    myNorm = vec4(gl_Normal, 0.0);
    myNorm = rotateByQuat(myNorm, quatInvert(baseBoneRots[index]));
    myNorm = rotateByQuat(myNorm, boneRots[index]);
    normSum += myNorm.xyz * influence2.z;
  }
  if (numWeights > 7) {
    index = int(jointIndex2.w);
    myPos = gl_Vertex - baseBoneLocs[index];
    myPos = rotateByQuat(myPos, quatInvert(baseBoneRots[index]));
    myPos = rotateByQuat(myPos, boneRots[index]);
    myPos += boneLocs[index];
    posSum += myPos.xyz * influence2.w;
    myNorm = vec4(gl_Normal, 0.0);
    myNorm = rotateByQuat(myNorm, quatInvert(baseBoneRots[index]));
    myNorm = rotateByQuat(myNorm, boneRots[index]);
    normSum += myNorm.xyz * influence2.w;
  }
  position = vec4(posSum, 1.0);
  normal = normSum;
  vertNorm = gl_NormalMatrix*normal;
  vertPos = vec3(gl_ModelViewMatrix*position);
  texCoords = gl_MultiTexCoord0.st;
  gl_Position = gl_ModelViewProjectionMatrix * position;
}
