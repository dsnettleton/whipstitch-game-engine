#version 120
//  wsDeferred.vsh
//  D. Scott Nettleton
//  1/22/2013
//  Deferred Vertex Shader for the Whipstitch Game Engine
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//  This shader takes the information stored in gl_FragData[]
//  by the initial shader program (wsShader.vsh, wsShader.fsh)
//  and evaluates lighting.

//  Copyright D. Scott Nettleton, 2013
//  This software is released under the terms of the
//  Lesser GNU Public License (LGPL).

varying vec2 texCoords;

void main() {
  
  gl_Position.xy = gl_Vertex.xy * 2.0 - 1.0;
  gl_Position.zw = gl_Vertex.zw;
  texCoords = gl_Vertex.xy;
}
