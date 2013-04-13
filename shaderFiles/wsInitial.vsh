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

out vec3 vertPos;
out vec3 vertNorm;
out vec2 texCoords;

uniform vec4 baseBoneLocs[64];
uniform vec4 baseBoneRots[64];
uniform vec4 boneLocs[64];
uniform vec4 boneRots[64];

in vec4 vert_position;
in vec3 vert_normal;
in vec2 vert_texCoords;
in int numWeights;
in vec4 jointIndex;
in vec4 jointIndex2; //  For numWeights > 4
in vec4 influence;
in vec4 influence2;  //  For numWeights > 4

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
  vec3 posSum = vec3(0.0f, 0.0f, 0.0f);
  vec3 normSum = vec3(0.0f, 0.0f, 0.0f);
  vec4 tmpPos;
  vec4 tmpNorm;
  if (numWeights > 0) {
    index = int(jointIndex.x);
    tmpPos = vert_position - baseBoneLocs[index];
    tmpPos = rotateByQuat(tmpPos, quatInvert(baseBoneRots[index]));
    tmpPos = rotateByQuat(tmpPos, boneRots[index]);
    tmpPos += boneLocs[index];
    posSum += tmpPos.xyz * influence.x;
    tmpNorm = vec4(vert_normal, 0.0);
    tmpNorm = rotateByQuat(tmpNorm, quatInvert(baseBoneRots[index]));
    tmpNorm = rotateByQuat(tmpNorm, boneRots[index]);
    normSum += tmpNorm.xyz * influence.x;
  }
  else {
    posSum = vert_position.xyz;
    normSum = vert_normal;
  }
  if (numWeights > 1) {
    index = int(jointIndex.y);
    tmpPos = vert_position - baseBoneLocs[index];
    tmpPos = rotateByQuat(tmpPos, quatInvert(baseBoneRots[index]));
    tmpPos = rotateByQuat(tmpPos, boneRots[index]);
    tmpPos += boneLocs[index];
    posSum += tmpPos.xyz * influence.y;
    tmpNorm = vec4(vert_normal, 0.0);
    tmpNorm = rotateByQuat(tmpNorm, quatInvert(baseBoneRots[index]));
    tmpNorm = rotateByQuat(tmpNorm, boneRots[index]);
    normSum += tmpNorm.xyz * influence.y;
  }
  if (numWeights > 2) {
    index = int(jointIndex.z);
    tmpPos = vert_position - baseBoneLocs[index];
    tmpPos = rotateByQuat(tmpPos, quatInvert(baseBoneRots[index]));
    tmpPos = rotateByQuat(tmpPos, boneRots[index]);
    tmpPos += boneLocs[index];
    posSum += tmpPos.xyz * influence.z;
    tmpNorm = vec4(vert_normal, 0.0);
    tmpNorm = rotateByQuat(tmpNorm, quatInvert(baseBoneRots[index]));
    tmpNorm = rotateByQuat(tmpNorm, boneRots[index]);
    normSum += tmpNorm.xyz * influence.z;
  }
  if (numWeights > 3) {
    index = int(jointIndex.w);
    tmpPos = vert_position - baseBoneLocs[index];
    tmpPos = rotateByQuat(tmpPos, quatInvert(baseBoneRots[index]));
    tmpPos = rotateByQuat(tmpPos, boneRots[index]);
    tmpPos += boneLocs[index];
    posSum += tmpPos.xyz * influence.w;
    tmpNorm = vec4(vert_normal, 0.0);
    tmpNorm = rotateByQuat(tmpNorm, quatInvert(baseBoneRots[index]));
    tmpNorm = rotateByQuat(tmpNorm, boneRots[index]);
    normSum += tmpNorm.xyz * influence.w;
  }
  if (numWeights > 4) {
    index = int(jointIndex2.x);
    tmpPos = vert_position - baseBoneLocs[index];
    tmpPos = rotateByQuat(tmpPos, quatInvert(baseBoneRots[index]));
    tmpPos = rotateByQuat(tmpPos, boneRots[index]);
    tmpPos += boneLocs[index];
    posSum += tmpPos.xyz * influence2.x;
    tmpNorm = vec4(vert_normal, 0.0);
    tmpNorm = rotateByQuat(tmpNorm, quatInvert(baseBoneRots[index]));
    tmpNorm = rotateByQuat(tmpNorm, boneRots[index]);
    normSum += tmpNorm.xyz * influence2.x;
  }
  if (numWeights > 5) {
    index = int(jointIndex2.y);
    tmpPos = vert_position - baseBoneLocs[index];
    tmpPos = rotateByQuat(tmpPos, quatInvert(baseBoneRots[index]));
    tmpPos = rotateByQuat(tmpPos, boneRots[index]);
    tmpPos += boneLocs[index];
    posSum += tmpPos.xyz * influence2.y;
    tmpNorm = vec4(vert_normal, 0.0);
    tmpNorm = rotateByQuat(tmpNorm, quatInvert(baseBoneRots[index]));
    tmpNorm = rotateByQuat(tmpNorm, boneRots[index]);
    normSum += tmpNorm.xyz * influence2.y;
  }
  if (numWeights > 6) {
    index = int(jointIndex2.z);
    tmpPos = vert_position - baseBoneLocs[index];
    tmpPos = rotateByQuat(tmpPos, quatInvert(baseBoneRots[index]));
    tmpPos = rotateByQuat(tmpPos, boneRots[index]);
    tmpPos += boneLocs[index];
    posSum += tmpPos.xyz * influence2.z;
    tmpNorm = vec4(vert_normal, 0.0);
    tmpNorm = rotateByQuat(tmpNorm, quatInvert(baseBoneRots[index]));
    tmpNorm = rotateByQuat(tmpNorm, boneRots[index]);
    normSum += tmpNorm.xyz * influence2.z;
  }
  if (numWeights > 7) {
    index = int(jointIndex2.w);
    tmpPos = vert_position - baseBoneLocs[index];
    tmpPos = rotateByQuat(tmpPos, quatInvert(baseBoneRots[index]));
    tmpPos = rotateByQuat(tmpPos, boneRots[index]);
    tmpPos += boneLocs[index];
    posSum += tmpPos.xyz * influence2.w;
    tmpNorm = vec4(vert_normal, 0.0);
    tmpNorm = rotateByQuat(tmpNorm, quatInvert(baseBoneRots[index]));
    tmpNorm = rotateByQuat(tmpNorm, boneRots[index]);
    normSum += tmpNorm.xyz * influence2.w;
  }
  vertNorm = gl_NormalMatrix*normSum;
  /*
  vec3 tanCrossZ = cross(normSum, vec3(0.0f, 0.0f, 1.0f));
  vec3 tanCrossY = cross(normSum, vec3(0.0f, 1.0f, 0.0f));
  if (length(tanCrossZ) > length(tanCrossY)) {
    vertTan = gl_NormalMatrix*normalize(tanCrossZ);
  }
  else {
    vertTan = gl_NormalMatrix*normalize(tanCrossY);
  }
  */

  vertPos = vec3(gl_ModelViewMatrix*vec4(posSum,1.0));
  texCoords = vert_texCoords;
  gl_Position = gl_ModelViewProjectionMatrix * vec4(posSum,1.0);
}
