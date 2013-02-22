#version 120
//  wsOutline.fsh
//  D. Scott Nettleton
//  2/20/2013
//  Post-processing Fragment Shader for the Whipstitch Game Engine
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//  Copyright D. Scott Nettleton, 2013
//  This software is released under the terms of the
//  Lesser GNU Public License (LGPL).

uniform vec3 outlineColor;

uniform sampler2D colorMap;
uniform sampler2D depthMap;

uniform float zNear;
uniform float zFar;

varying vec2 texCoords;

const float OUTLINE_THRESHOLD = 0.2;
const float OUTLINE_THICKNESS = 0.002;

float getDepth(vec2 coord) {
    return 2.0 * zNear * zFar / (zFar + zNear - (2.0 * texture2D(depthMap, coord).x - 1.0) * (zFar - zNear));
}

float getOutlines(vec2 coord, float thickness) {
  float depthVal = getDepth(coord);
  vec3 norm = normalize(
                vec3(
                  getDepth(coord+vec2(thickness,0.0)) - depthVal,
                  getDepth(coord+vec2(0.0,thickness)) - depthVal,
                  OUTLINE_THRESHOLD
                )
              );
  return norm.z;
}

void main() {
  vec4 color = texture2D(colorMap, texCoords);
  float outlines = getOutlines(texCoords, OUTLINE_THICKNESS);
  if (outlines <= OUTLINE_THRESHOLD) {
    color.rgb = outlineColor;
  }
  gl_FragData[0] = color;
}
