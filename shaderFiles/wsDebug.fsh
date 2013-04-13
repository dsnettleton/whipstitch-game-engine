#version 130
//  wsDebug.fsh
//  D. Scott Nettleton
//  3/14/2013
//  Debug Vertex Shader for the Whipstitch Game Engine
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//  Copyright D. Scott Nettleton, 2013
//  This software is released under the terms of the
//  Lesser GNU Public License (LGPL).

in vec4 vertColor;

void main() {
  gl_FragData[2] = vertColor;
  gl_FragData[3] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
