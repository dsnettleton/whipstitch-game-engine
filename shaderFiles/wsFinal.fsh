#version 120
//  wsFullscreen.vsh
//  D. Scott Nettleton
//  1/22/2013
//  Fullscreen Vertex Shader for the Whipstitch Game Engine
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//  This shader is used to print a final shader texture onto
//  the screen.

//  Copyright D. Scott Nettleton, 2013
//  This software is released under the terms of the
//  Lesser GNU Public License (LGPL).

varying vec2 texCoords;

uniform sampler2D finalTexture;

void main() {  
  gl_FragColor = texture2D(finalTexture, texCoords);
}
