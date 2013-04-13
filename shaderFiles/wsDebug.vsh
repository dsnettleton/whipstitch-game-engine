#version 130
//  wsDebug.vsh
//  D. Scott Nettleton
//  3/14/2013
//  Debug Vertex Shader for the Whipstitch Game Engine
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//	Copyright D. Scott Nettleton, 2013
//	This software is released under the terms of the
//	Lesser GNU Public License (LGPL).

out vec4 vertColor;

invariant gl_Position;

void main() {
  vertColor = gl_Color;
  gl_Position = ftransform();
}
