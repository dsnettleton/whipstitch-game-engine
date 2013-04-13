#version 130
//  wsSolidColor.fsh
//  D. Scott Nettleton
//  3/25/2013
//  Simple Vertex Shader for drawing a model's polygons as
//  a solid color.
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//  Copyright D. Scott Nettleton, 2013
//  This software is released under the terms of the
//  Lesser GNU Public License (LGPL).

in vec4 vertColor;

void main() {
  gl_FragData[3] = vertColor;
}
