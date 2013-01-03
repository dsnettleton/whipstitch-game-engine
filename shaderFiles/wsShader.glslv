#version 130
//  wsShader.glslv
//  D. Scott Nettleton
//  10/04/2012
//  Primary Vertex Shader for the Whipstitch Game Engine
//	Copyright D. Scott Nettleton, 2013
//	This software is released under the terms of the
//	Lesser GNU Public License (LGPL).

varying vec3 vertPos;
varying vec3 vertNorm;
varying vec2 texCoords;

uniform int lightingEnabled;
uniform int renderMode;

const int WS_RENDER_MODE_LIT = 1;    //  Per-fragment lighting
const int WS_RENDER_MODE_CEL = 2;    //  Cel shading
const int WS_RENDER_MODE_INV = 3;    //  Invisible (refracted) shading

void main() {
  vec4 eyeCoordPosition = gl_ModelViewMatrix*gl_Vertex;
  vertPos = vec3(eyeCoordPosition) / eyeCoordPosition.w;
  vertNorm = normalize(gl_NormalMatrix * gl_Normal);
  texCoords = gl_MultiTexCoord0.st;
  gl_Position = ftransform();
}
