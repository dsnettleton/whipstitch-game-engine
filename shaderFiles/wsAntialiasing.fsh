#version 120//wsAntialiasing.fsh//D. Scott Nettleton//4/20/2013//Post-processing Fragment Shader for the Whipstitch Game Engine//This shader integrates with the Whipstitch Game Engine's//new deferred rendering paradigm.//Copyright D. Scott Nettleton, 2013//This software is released under the terms of the//Lesser GNU Public License (LGPL).//Antialiasing utilizes DLAA algorithm

uniform sampler2D colorMap;
uniform float screenWidth;
uniform float screenHeight;

varying vec2 texCoords;//uniform sampler2D depthMap;

const vec3 oneFifth = vec3(1.0/5.0, 1.0/5.0, 1.0/5.0);
const float SAMPLE_OFFSET = 1.5;
const float EDGE_TOLERANCE = 0.3;
const float BLUR_BLEND_FACTOR = 0.5;

float lumRGB(vec3 v) {
  return dot(v, vec3(0.299, 0.587, 0.114)); //  Perceieved luminosity
}

vec4 dlaa() {
  //short edges
  vec4 thisFrag = texture2D(colorMap, texCoords);
  vec4 leftFrag = texture2D(colorMap, texCoords + vec2(-SAMPLE_OFFSET/screenWidth, 0.0));
  vec4 rightFrag = texture2D(colorMap, texCoords + vec2(SAMPLE_OFFSET/screenWidth, 0.0));
  vec4 topFrag = texture2D(colorMap, texCoords + vec2(0.0, SAMPLE_OFFSET/screenHeight));
  vec4 bottomFrag = texture2D(colorMap, texCoords +  vec2(0.0, -SAMPLE_OFFSET/screenHeight));
  vec4 uberLeftFrag = texture2D(colorMap, texCoords + vec2(-2.0*SAMPLE_OFFSET/screenWidth, 0.0));
  vec4 uberRightFrag = texture2D(colorMap, texCoords + vec2(2.0*SAMPLE_OFFSET/screenWidth, 0.0));
  vec4 uberTopFrag = texture2D(colorMap, texCoords + vec2(0.0, 2.0*SAMPLE_OFFSET/screenHeight));
  vec4 uberBottomFrag = texture2D(colorMap, texCoords +  vec2(0.0, -2.0*SAMPLE_OFFSET/screenHeight));

  vec4 hiPassHoriz = (4.0*thisFrag - topFrag - bottomFrag - uberTopFrag - uberBottomFrag);
  float valueEdgeHoriz = lumRGB(hiPassHoriz.xyz);
  vec4 hiPassVert = (4.0*thisFrag - leftFrag - rightFrag - uberLeftFrag - uberRightFrag);
  float valueEdgeVert = lumRGB(hiPassVert.xyz);

  vec4 blur = thisFrag;
  if (abs(valueEdgeHoriz) > EDGE_TOLERANCE && abs(valueEdgeVert) <= EDGE_TOLERANCE) {
    blur.xyz += leftFrag.xyz + rightFrag.xyz + uberLeftFrag.xyz + uberRightFrag.xyz;
    blur.xyz *= oneFifth;
  }
  else if (abs(valueEdgeVert) > EDGE_TOLERANCE) {
    blur.xyz += topFrag.xyz + bottomFrag.xyz + uberTopFrag.xyz + uberBottomFrag.xyz;
    blur.xyz *= oneFifth;
  }
  vec4 avg = mix(thisFrag, blur, BLUR_BLEND_FACTOR);
  return vec4(avg.xyz, 1.0);
}

vec4 ssaa() {
  vec2 offset = vec2(-(lumRGB(texture2D(colorMap, texCoords + vec2(0.0, -SAMPLE_OFFSET/screenHeight)).xyz) -
                      lumRGB(texture2D(colorMap, texCoords + vec2(0.0, SAMPLE_OFFSET/screenHeight)).xyz)),
                      lumRGB(texture2D(colorMap, texCoords + vec2(SAMPLE_OFFSET/screenWidth, 0.0)).xyz) -
                      lumRGB(texture2D(colorMap, texCoords + vec2(-SAMPLE_OFFSET/screenWidth, 0.0)).xyz));
  float magnitude = length(offset);

  if  (magnitude < EDGE_TOLERANCE) {
    return texture2D(colorMap, texCoords);
  }
  else {
    offset *= vec2(1.0/screenWidth, 1.0/screenHeight) / magnitude;
    offset *= 1.5;
    vec4 thisFrag = texture2D(colorMap, texCoords);
    vec4 sample1 = texture2D(colorMap, texCoords + offset * 0.5) * 0.9;
    vec4 sample2 = texture2D(colorMap, texCoords - offset * 0.5) * 0.9;
    vec4 sample3 = texture2D(colorMap, texCoords + offset) * 0.75;
    vec4 sample4 = texture2D(colorMap, texCoords - offset) * 0.75;

    return (thisFrag + sample1 + sample2 + sample3 + sample4) / 4.3;
  }
}

void  main() {
  gl_FragData[0] = dlaa();
}
