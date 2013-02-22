#version 120//wsAntialiasing.fsh//D. Scott Nettleton//4/20/2013//Post-processing Fragment Shader for the Whipstitch Game Engine//This shader integrates with the Whipstitch Game Engine's//new deferred rendering paradigm.//Copyright D. Scott Nettleton, 2013//This software is released under the terms of the//Lesser GNU Public License (LGPL).//Antialiasing utilizes DLAA algorithm

uniform sampler2D colorMap;
uniform float screenWidth;
uniform float screenHeight;

varying vec2 texCoords;//uniform sampler2D depthMap;

const vec3 oneThird = vec3(1.0/3.0, 1.0/3.0, 1.0/3.0);
const float SAMPLE_OFFSET = 1.0;
const float NUM_SAMPLES = 3.0;
const float EDGE_TOLERANCE = 0.1;

const int METHOD_DLAA = 1;
const int METHOD_SSAA = 2;

const int method = METHOD_SSAA;

float avg(vec3 value) {
  return dot(value, oneThird);
}

float lumRGB(vec3 v) {
  return dot(v, vec3(0.299, 0.587, 0.114)); //  Perceieved luminosity
}

vec4 dlaa() {
  //short edges
  vec4 thisFrag = texture2D(colorMap, texCoords);
  vec4 sumHoriz = texture2D(colorMap, texCoords + vec2(-SAMPLE_OFFSET/screenWidth, 0.0)) +
  texture2D(colorMap, texCoords + vec2(SAMPLE_OFFSET/screenWidth, 0.0));
  vec4 sumVert = texture2D(colorMap, texCoords + vec2(0.0, -SAMPLE_OFFSET/screenHeight)) +
  texture2D(colorMap, texCoords + vec2(0.0, SAMPLE_OFFSET/screenHeight));

  vec4 diffToCenterHoriz = abs(sumHoriz - (2.0*thisFrag))/2.0;  
  vec4 diffToCenterVert = abs(sumVert - (2.0*thisFrag))/2.0;

  float valueEdgeHoriz = avg(diffToCenterHoriz.xyz);
  float valueEdgeVert = avg(diffToCenterVert.xyz);
  
  float edgeDetectHoriz = clamp((NUM_SAMPLES*valueEdgeHoriz) - 0.1, 0.0, 1.0);
  float edgeDetectVert = clamp((NUM_SAMPLES*valueEdgeVert)  - 0.1, 0.0, 1.0);

  vec4 avgHoriz = (sumHoriz + thisFrag)/NUM_SAMPLES;
  vec4 avgVert = (sumVert + thisFrag)/NUM_SAMPLES;

  float valueHoriz = avg(avgHoriz.xyz);
  float valueVert = avg(avgVert.xyz);

  float blurAmountHoriz = clamp(edgeDetectHoriz / valueHoriz, 0.0, 1.0);
  float blurAmountVert = clamp(edgeDetectVert / valueVert, 0.0, 1.0);

  vec4 aaResult = mix(thisFrag, avgHoriz, blurAmountHoriz);
  aaResult = mix(aaResult, avgVert, blurAmountVert);//long edges
  /*
    vec4 sampleVertNeg1 = texture2D(colorMap, texCoords + vec2(0.0, -3.5/screenHeight)); 
    vec4 sampleVertNeg2 = texture2D(colorMap, texCoords + vec2(0.0, -7.5/screenHeight));
    vec4 sampleVertPos1 = texture2D(colorMap, texCoords + vec2(0.0, 3.5/screenHeight)); 
    vec4 sampleVertPos2 = texture2D(colorMap, texCoords + vec2(0.0, 7.5/screenHeight)); 

    vec4 sampleHorizNeg1 = texture2D(colorMap, texCoords + vec2(-3.5/screenWidth, 0.0)); 
    vec4 sampleHorizNeg2 = texture2D(colorMap, texCoords + vec2(-7.5/screenWidth, 0.0));
    vec4 sampleHorizPos1 = texture2D(colorMap, texCoords + vec2(3.5/screenWidth, 0.0)); 
    vec4 sampleHorizPos2 = texture2D(colorMap, texCoords + vec2(7.5/screenWidth, 0.0)); 

    float pass1EdgeAvgHoriz = (sampleHorizNeg2.a + sampleHorizNeg1.a + thisFrag.a + sampleHorizPos1.a + sampleHorizPos2.a)/5.0;
    float pass1EdgeAvgVert = (sampleVertNeg2.a + sampleVertNeg1.a + thisFrag.a + sampleVertPos1.a + sampleVertPos2.a )/5.0;
    pass1EdgeAvgHoriz = clamp(pass1EdgeAvgHoriz*2.0 - 1.0, 0.0, 1.0);
    pass1EdgeAvgVert = clamp(pass1EdgeAvgVert*2.0 - 1.0, 0.0, 1.0);
    float longEdge = max(pass1EdgeAvgHoriz, pass1EdgeAvgVert);

    if (longEdge > 1.0) {
      vec4 avgHorizLong = (sampleHorizNeg2 + sampleHorizNeg1 + thisFrag + sampleHorizPos1 + sampleHorizPos2)/5.0;
      vec4 avgVertLong = (sampleVertNeg2  + sampleVertNeg1  + thisFrag + sampleVertPos1  + sampleVertPos2 )/5.0;
      float valueHorizLong = avg(avgHorizLong.xyz);
      float valueVertLong = avg(avgVertLong.xyz);

      vec4 sampleLeft = texture2D(colorMap, texCoords + vec2(-1.0/screenWidth, 0.0));
      vec4 sampleRight = texture2D(colorMap, texCoords + vec2(1.0/screenWidth, 0.0));
      vec4 sampleUp = texture2D(colorMap, texCoords + vec2(0.0, -1.0/screenHeight));
      vec4 sampleDown = texture2D(colorMap, texCoords + vec2(0.0, 1.0/screenHeight));

      float valueCenter = avg(thisFrag.xyz);
      float valueLeft = avg(sampleLeft.xyz);
      float valueRight = avg(sampleRight.xyz);
      float valueTop = avg(sampleUp.xyz);
      float valueBottom = avg(sampleDown.xyz);

      vec4 diffToCenter = valueCenter - vec4(valueLeft, valueTop, valueRight, valueBottom);    
      float blurAmountLeft = clamp(0.0 + (valueVertLong  - valueLeft )/diffToCenter.x, 0.0, 1.0);
      float blurAmountUp = clamp(0.0 + (valueHorizLong - valueTop   )/diffToCenter.y, 0.0, 1.0);
      float blurAmountRight = clamp(1.0 + (valueVertLong  - valueCenter)/diffToCenter.z, 0.0, 1.0);
      float blurAmountDown = clamp(1.0 + (valueHorizLong - valueCenter)/diffToCenter.w, 0.0, 1.0);    

      vec4 blurAmounts = vec4(blurAmountLeft, blurAmountRight, blurAmountUp, blurAmountDown);
      blurAmounts = (blurAmounts == vec4(0.0, 0.0, 0.0, 0.0)) ? vec4(1.0, 1.0, 1.0, 1.0) : blurAmounts;

      vec4 longBlurHoriz = mix(sampleLeft, thisFrag, blurAmounts.x);
      longBlurHoriz = mix(sampleRight, longBlurHoriz, blurAmounts.y);
      vec4 longBlurVert = mix(sampleUp, thisFrag, blurAmounts.z);
      longBlurVert = mix(sampleDown, longBlurVert, blurAmounts.w);

      aaResult = mix(aaResult, longBlurHoriz, pass1EdgeAvgVert);
      aaResult = mix(aaResult, longBlurVert, pass1EdgeAvgHoriz);
    }
  //*/
  return vec4(aaResult.rgb, 1.0);
}

vec4 ssaa() {
  float t = lumRGB(texture2D(colorMap, texCoords + vec2(0.0, -SAMPLE_OFFSET/screenHeight)).xyz),
        l = lumRGB(texture2D(colorMap, texCoords + vec2(-SAMPLE_OFFSET/screenWidth, 0.0)).xyz),
        r = lumRGB(texture2D(colorMap, texCoords + vec2(SAMPLE_OFFSET/screenWidth, 0.0)).xyz),
        b = lumRGB(texture2D(colorMap, texCoords + vec2(0.0, SAMPLE_OFFSET/screenHeight)).xyz);

  vec2    n = vec2(-(t - b), r - l);
  float   nl = length(n);

  if  (nl < EDGE_TOLERANCE) {
    return texture2D(colorMap, texCoords);
  }
  else {
    n *= vec2(1.0/screenWidth, 1.0/screenHeight) / nl;
    n *= 1.5;
    vec4  o = texture2D(colorMap, texCoords),
          t0 = texture2D(colorMap, texCoords + n * 0.5) * 0.9,
          t1 = texture2D(colorMap, texCoords - n * 0.5) * 0.9,
          t2 = texture2D(colorMap, texCoords + n) * 0.75,
          t3 = texture2D(colorMap, texCoords - n) * 0.75;

    return (o + t0 + t1 + t2 + t3) / 4.3;
    // return vec4(1.0, 0.0, 0.0, 1.0);
  }
}

void  main() {
  if (method == METHOD_DLAA) {
    gl_FragData[0] = dlaa();
  }
  else if (method == METHOD_SSAA) {
    gl_FragData[0] = ssaa();
    // gl_FragData[0] = vec4(vec3(1.0,1.0,1.0) * lumRGB(texture2D(colorMap, texCoords).xyz), 1.0);
  }
}
