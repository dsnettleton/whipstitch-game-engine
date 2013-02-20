#version 120
//  wsHud.vsh
//  D. Scott Nettleton
//  2/18/2013
//  HUD Vertex Shader for the Whipstitch Game Engine
//  This shader sets up a Heads Up Display for drawing
//  Textured quads directly onto the screen.

//  Copyright D. Scott Nettleton, 2013
//  This software is released under the terms of the
//  Lesser GNU Public License (LGPL).

varying vec2 texCoords;
varying vec4 matColor;

invariant gl_Position;

void main() {
  matColor = gl_Color;
  texCoords = gl_MultiTexCoord0.st;
  gl_Position = ftransform();
}
