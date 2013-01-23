#version 120
//  wsUnlit.fsh
//  D. Scott Nettleton
//  1/22/2013
//  Initial Fragment Shader for the Whipstitch Game Engine
//  This shader integrates with the Whipstitch Game Engine's
//  new deferred rendering paradigm.

//  Copyright D. Scott Nettleton, 2013
//  This software is released under the terms of the
//  Lesser GNU Public License (LGPL).

varying vec3 vertPos;
varying vec3 vertNorm;

varying vec2 texCoords;

uniform sampler2D colorMap;

/*****  WS_RENDER_MODE_CEL  ******/

vec4 directionalLight_cel(const in int lightNumber, const in int celShadingLevels, const in vec4 matDiffuse) {
  float levelScaling = 1.0 / celShadingLevels;
  vec3 vertLightNorm = normalize( gl_LightSource[lightNumber].position.xyz - vertPos );
  float cosine = max( 0.0, dot(vertLightNorm, vertNorm) );
  return (gl_LightSource[lightNumber].ambient*matDiffuse) + matDiffuse * ceil( cosine*celShadingLevels ) * levelScaling;
}

vec4 positionalLight_cel(const in int lightNumber, const in int celShadingLevels, const in vec4 matDiffuse) {
  float levelScaling = 1.0 / celShadingLevels;
  vec3 vertLightNorm = normalize( gl_LightSource[lightNumber].position.xyz - vertPos );
  float cosine = max( 0.0, dot(vertLightNorm, vertNorm) );
  return (gl_LightSource[lightNumber].ambient*matDiffuse) + matDiffuse * ceil( cosine*celShadingLevels ) * levelScaling;
}

vec4 spotLight_cel(const in int lightNumber, const in int celShadingLevels, const in vec4 matDiffuse) {
  float levelScaling = 1.0 / celShadingLevels;
  vec3 vertLightNorm = normalize( gl_LightSource[lightNumber].position.xyz - vertPos );
  float cosine = max( 0.0, dot(vertLightNorm, vertNorm) );
  vec4 diffuse = matDiffuse * ceil( cosine*celShadingLevels ) * levelScaling;
  float spotAngle = dot(-vertLightNorm, normalize(gl_LightSource[lightNumber].spotDirection));
  float spotCosCutoff = cos(gl_LightSource[lightNumber].spotCutoff*0.017453); //  times pi/180
  if (spotAngle < spotCosCutoff) {  //  The position is outside the spotLight
    return vec4(0.0);
  }
  return (gl_LightSource[lightNumber].ambient*matDiffuse) + diffuse;
}

vec4 getColor_cel() {
  const int CEL_SHADING_LEVELS = 3;
  vec4 matAmbient;
  vec4 matDiffuse = gl_FrontMaterial.diffuse * texture2D(colorMap, texCoords);
  vec4 accumulation = vec4(0.0);
  for (int i = 0; i < 1; ++i) {
    if (gl_LightSource[i].position.w == 0.0) {  //  Directional Light
      accumulation += directionalLight_cel(i, CEL_SHADING_LEVELS, matDiffuse);
    }
    else if (gl_LightSource[i].spotCutoff == 180.0) {  //  Positional Light
      accumulation += positionalLight_cel(i, CEL_SHADING_LEVELS, matDiffuse);
    }
    else {  //  Spotlight
      accumulation += spotLight_cel(i, CEL_SHADING_LEVELS, matDiffuse);
    }
  }
  return clamp(accumulation+gl_FrontMaterial.emission, 0.0, 1.0);
}

void main() {
  vec4 texColor = texture2D(colorMap, texCoords);
  
  gl_FragData[0] = vec4( vertPos, 0.0);
  gl_FragData[1] = vec4( vertNorm, 0.0);
  gl_FragData[2] = texColor;
  gl_FragData[3] = getColor_cel();
}
