#version 120
//  wsUnlit.fsh
//  D. Scott Nettleton
//  1/22/2013
//  Cel-shading Fragment Shader for the Whipstitch Game Engine
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//  Copyright D. Scott Nettleton, 2013
//  This software is released under the terms of the
//  Lesser GNU Public License (LGPL).

const float OUTLINE_THRESHOLD = 0.2;
const float OUTLINE_THICKNESS = 0.00075;
const int CEL_SHADING_LEVELS = 3;

uniform sampler2D colorMap;
uniform sampler2D depthMap;

uniform float zNear;
uniform float zFar;

varying vec2 texCoords;

// Standard depth function.
float getDepth(vec2 coord) {
    return 2.0 * zNear * zFar / (zFar + zNear - (2.0 * texture2D(depthMap, coord).x - 1.0) * (zFar - zNear));
}

float getCelShaderFactor(vec2 coord) {
  float depthVal = getDepth(coord);
  vec3 norm = normalize(vec3(getDepth(coord+vec2(OUTLINE_THICKNESS,0.0))-depthVal, getDepth(coord+vec2(0.0,OUTLINE_THICKNESS))-depthVal, OUTLINE_THRESHOLD));
  return norm.z;
}

void main() {
  float levelScaling = 1.0 / CEL_SHADING_LEVELS;
  vec4 color = texture2D(colorMap, texCoords);
  color.rgb *= getCelShaderFactor(texCoords);

  gl_FragData[0] = color;
}
