#version 120
//  wsDeferred.fsh
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

uniform sampler2D normalTex;
uniform sampler2D positionTex;
uniform sampler2D materialTex;

void main() {
    //directional light, coming from over the right shoulder in eye-space
  vec3 light =  vec3( 0.57735);
  
  vec4 normal = texture2D( normalTex, texCoord);
  vec3 position = texture2D( positionTex, texCoord).xyz;
  vec4 mat = texture2D( materialTex, texCoord);
  vec3 bumpNormal = normal.xyz * 2.0 - 1.0;
  float factor = length(bumpNormal);
  
  float d = dot( light, bumpNormal) * 1.0;
  
  d = max( 0.0, d) + 0.2;
  d = min( normal.w, d);
  
  vec3 spec = vec3(0.0);
  
  if ( mat.w > 0.0f) {
  
    vec3 refl = reflect( normalize(position), bumpNormal);
    float s = pow( max( 0.0, dot( refl, light)), mat.w);
    spec = s*mix( mat.rgb, vec3(1.0), 0.2);
    d *= 0.0125f; //reduce diffuse to make it look more metalic

  }
  
  gl_FragColor = vec4( d*mat.rgb + spec, 1.0);
}
