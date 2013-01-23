#version 120
//  wsFullscreen.vsh
//  D. Scott Nettleton
//  1/22/2013
//  Fullscreen Vertex Shader for the Whipstitch Game Engine
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//  This shader is used for fullscreen applications, meant to
//  stretch a single textured quad across the screen. Vertex
//  position means little, so only the texture coordinates
//  are dealt with.

//  Copyright D. Scott Nettleton, 2013
//  This software is released under the terms of the
//  Lesser GNU Public License (LGPL).

varying vec2 texCoords;

void main() {
  gl_Position = vec4(2.0*gl_Vertex.x - 1.0, 2.0*gl_Vertex.y - 1.0, -1.0, 1.0);
  texCoords = vec2(gl_Vertex);
}
