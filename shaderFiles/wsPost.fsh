#version 120
//  wsPost.fsh
//  D. Scott Nettleton
//  1/22/2013
//  Post-processing Fragment Shader for the Whipstitch Game Engine
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//  Copyright D. Scott Nettleton, 2013
//  This software is released under the terms of the
//  Lesser GNU Public License (LGPL).

// uniform int antialias;

uniform sampler2D colorMap;
uniform sampler2D materialMap;

varying vec2 texCoords;

void main() {
  gl_FragData[0] = texture2D(colorMap, texCoords) * texture2D(materialMap, texCoords);
}
