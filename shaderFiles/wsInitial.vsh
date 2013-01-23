#version 120
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

invariant gl_Position;

void main() {
  vertPos = vec3(gl_ModelViewMatrix*gl_Vertex);
  vertNorm = gl_NormalMatrix * gl_Normal;
  texCoords = gl_MultiTexCoord0.st;
  gl_Position = ftransform();
}
