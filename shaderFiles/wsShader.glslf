#version 130
//  wsShader.glslf
//  D. Scott Nettleton
//  10/04/2012
//  Primary Fragment Shader for the Whipstitch Game Engine
//	Copyright D. Scott Nettleton, 2013
//	This software is released under the terms of the
//	Lesser GNU Public License (LGPL).

varying vec3 vertPos;
varying vec3 vertNorm;
varying vec2 texCoords;

uniform float ambientComponent;
uniform vec3 eyePos;

uniform int lightingEnabled;
uniform int renderMode;
uniform int usingTexture;

const int WS_RENDER_MODE_LIT = 1;    //  Per-fragment lighting
const int WS_RENDER_MODE_CEL = 2;    //  Cel shading
const int WS_RENDER_MODE_INV = 3;    //  Invisible (refracted) shading

const vec4 RED = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 GREEN = vec4(0.0, 1.0, 0.0, 1.0);
const vec4 BLUE = vec4(0.0, 0.0, 1.0, 1.0);
const vec4 MAGENTA = vec4(1.0, 0.0, 1.0, 1.0);

uniform sampler2D tex;

/*****  WS_RENDER_MODE_LIT  ******/

void directionalLight_lit(const in int lightNumber, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular) {
	vec3 vertLight = gl_LightSource[lightNumber].position.xyz - vertPos;
	float vertDist = length(vertLight);
	float normDotLightDir = max(0.0, dot(vertNorm, normalize(vertLight)));
	float attenuation = 1.0 / (gl_LightSource[lightNumber].constantAttenuation + gl_LightSource[lightNumber].linearAttenuation*vertDist + gl_LightSource[lightNumber].quadraticAttenuation*vertDist*vertDist);
  float specPower;
	if (normDotLightDir == 0.0) {
		specPower = 0.0;
  }
	else {
		float normDotHalfVec = max(0.0, dot(vertNorm, gl_LightSource[lightNumber].halfVector.xyz));
		specPower = pow(normDotHalfVec, gl_FrontMaterial.shininess);
	}
	ambient += gl_LightSource[lightNumber].ambient*attenuation;
	diffuse += vec4(vec3(gl_LightSource[lightNumber].diffuse.rgb*normDotLightDir), gl_LightSource[lightNumber].diffuse.a);
	specular += gl_LightSource[lightNumber].specular*specPower;
}

void positionalLight_lit(const in int lightNumber, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular) {
  float specPower;
	vec3 vertLight = vec3(gl_LightSource[lightNumber].position) - vertPos;
	float vertDist = length(vertLight);
	float normDotLightDir = max(0.0, dot(vertNorm, normalize(vertLight)));
	float attenuation = 1.0 / (gl_LightSource[lightNumber].constantAttenuation + gl_LightSource[lightNumber].linearAttenuation*vertDist + gl_LightSource[lightNumber].quadraticAttenuation*vertDist*vertDist);
	//	Compute Specular
	if (normDotLightDir == 0.0) {
		specPower = 0.0;
	}
	else {
		float normDotHalfVec = max(0.0, dot(vertNorm, normalize(vertLight+eyePos)));
		specPower = max(pow(normDotHalfVec, gl_FrontMaterial.shininess), 0.0);
	}
	ambient += gl_LightSource[lightNumber].ambient*attenuation;
	diffuse += gl_LightSource[lightNumber].diffuse*attenuation*normDotLightDir;
	specular += gl_LightSource[lightNumber].specular*specPower;
}

void spotLight_lit(const in int lightNumber, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular) {
  float specPower;
	vec3 vertLight = vec3(gl_LightSource[lightNumber].position) - vertPos;
	float vertDist = length(vertLight);
	float attenuation = 1.0 / (gl_LightSource[lightNumber].constantAttenuation + gl_LightSource[lightNumber].linearAttenuation*vertDist + gl_LightSource[lightNumber].quadraticAttenuation*vertDist*vertDist);
	float spotAttenuation;
	float spotAngle = dot(-normalize(vertLight), normalize(gl_LightSource[lightNumber].spotDirection));
	float spotCosCutoff = cos(gl_LightSource[lightNumber].spotCutoff*0.017453); //  times pi/180
	float normDotLightDir = max(0.0, dot(vertNorm, normalize(vertLight)));
	vec3 halfVector = normalize(vertLight+eyePos);
	if (spotAngle < spotCosCutoff) {	//	The position is outside the spotLight
		spotAttenuation = 0.0;
	}
	else {	//	The position is within the spotlight
		spotAttenuation = pow(spotAngle, gl_LightSource[lightNumber].spotExponent);
		if (spotCosCutoff != 1.0) {
			spotAttenuation *= 1.0-((1.0-spotAngle)/(1.0-spotCosCutoff));
		}
	}
	attenuation *= spotAttenuation;
	normDotLightDir = max(0.0, dot(vertNorm, vertLight));
	//	Compute Specular
	if (normDotLightDir == 0.0) {
		specPower = 0.0;
	}
	else {
		float normDotHalfVec = max(0.0, dot(vertNorm, halfVector));
		specPower = max(pow(normDotHalfVec, gl_FrontMaterial.shininess), 0.0);
	}
	ambient += gl_LightSource[lightNumber].ambient*attenuation;
	diffuse += gl_LightSource[lightNumber].diffuse*attenuation*normDotLightDir;
	specular += gl_LightSource[lightNumber].specular*specPower;
}

vec4 getColor_lit() {
	vec4 myColor = vec4(1.0);
  if (lightingEnabled != 0) {
	  vec4 debugColor;
		vec4 ambient = vec4(0.0);
  	vec4 diffuse = vec4(0.0);
		vec4 specular = vec4(0.0);
    for (int i = 0; i < 1; ++i) {
			vec3 vertLight = vec3(gl_LightSource[i].position) - vertPos;
			//	Compute diffuse and ambient
			float vertDist = length(vertLight);
			vec3 vertLightNorm = normalize(vertLight);
			vec3 halfVector = normalize(vertLight+eyePos);
			float specPower;
			float normDotLightDir;
			float normDotHalfVec;
	  	if (gl_LightSource[i].position.w == 0.0) {	//	Directional Light
	  	  directionalLight_lit(i, ambient, diffuse, specular);
	  	}
	  	else if (gl_LightSource[i].spotCutoff == 180.0) {	//	Positional Light
	  	  positionalLight_lit(i, ambient, diffuse, specular);
			}
			else {	//	Spotlight
			  spotLight_lit(i, ambient, diffuse, specular);
			}
    }
    vec4 color = clamp(gl_FrontMaterial.diffuse*diffuse + gl_FrontMaterial.ambient*ambient + gl_FrontMaterial.specular*specular + gl_FrontMaterial.ambient*ambientComponent, 0.0, 1.0);
    if (usingTexture != 0) {
 	  	myColor = color*texture2D(tex, texCoords);
 	  }
 	  else {
 	  	myColor = color;
 	  }
  }
  else {
  	if (usingTexture != 0) {
	    myColor = gl_FrontMaterial.diffuse * texture2D(tex, texCoords.st);
	  }
	  else {
	  	myColor = gl_FrontMaterial.diffuse;
	  }
  }
  return myColor;
}

/*****  WS_RENDER_MODE_CEL  ******/

vec4 directionalLight_cel(const in int lightNumber, const in int celShadingLevels, const in vec4 matDiffuse) {
  float levelScaling = 1.0 / celShadingLevels;
  vec3 vertLightNorm = normalize( gl_LightSource[lightNumber].position.xyz - vertPos );
  float cosine = max( 0.0, dot(vertLightNorm, vertNorm) );
  vec4 diffuse = matDiffuse * ceil( cosine*celShadingLevels ) * levelScaling;
  return (gl_LightSource[lightNumber].ambient*matDiffuse) + diffuse;
}

vec4 positionalLight_cel(const in int lightNumber, const in int celShadingLevels, const in vec4 matDiffuse) {
  float levelScaling = 1.0 / celShadingLevels;
  vec3 vertLightNorm = normalize( gl_LightSource[lightNumber].position.xyz - vertPos );
  float cosine = max( 0.0, dot(vertLightNorm, vertNorm) );
  vec4 diffuse = matDiffuse * ceil( cosine*celShadingLevels ) * levelScaling;
  return (gl_LightSource[lightNumber].ambient*matDiffuse) + diffuse;
}

vec4 spotLight_cel(const in int lightNumber, const in int celShadingLevels, const in vec4 matDiffuse) {
  float levelScaling = 1.0 / celShadingLevels;
  vec3 vertLightNorm = normalize( gl_LightSource[lightNumber].position.xyz - vertPos );
  float cosine = max( 0.0, dot(vertLightNorm, vertNorm) );
  vec4 diffuse = matDiffuse * ceil( cosine*celShadingLevels ) * levelScaling;
	float spotAngle = dot(-vertLightNorm, normalize(gl_LightSource[lightNumber].spotDirection));
	float spotCosCutoff = cos(gl_LightSource[lightNumber].spotCutoff*0.017453); //  times pi/180
	if (spotAngle < spotCosCutoff) {	//	The position is outside the spotLight
	  return vec4(0.0);
  }
  return (gl_LightSource[lightNumber].ambient*matDiffuse) + diffuse;
}

vec4 getColor_cel() {
  const int CEL_SHADING_LEVELS = 3;
  vec4 matAmbient;
  vec4 matDiffuse;
  if (usingTexture != 0) {
    matDiffuse = gl_FrontMaterial.diffuse * texture2D(tex, texCoords);
  }
  else {
    matDiffuse = gl_FrontMaterial.diffuse;
  }
  vec4 accumulation = vec4(0.0);
  for (int i = 0; i < 1; ++i) {
  	if (gl_LightSource[i].position.w == 0.0) {	//	Directional Light
      accumulation += directionalLight_cel(i, CEL_SHADING_LEVELS, matDiffuse);
  	}
  	else if (gl_LightSource[i].spotCutoff == 180.0) {	//	Positional Light
      accumulation += positionalLight_cel(i, CEL_SHADING_LEVELS, matDiffuse);
		}
		else {	//	Spotlight
      accumulation += spotLight_cel(i, CEL_SHADING_LEVELS, matDiffuse);
		}
  }
  return clamp(accumulation, 0.0, 1.0);
}

void main() {
  if (renderMode == WS_RENDER_MODE_LIT) {
    gl_FragColor = getColor_lit();
    //gl_FragColor = RED;
  }
  else if (renderMode == WS_RENDER_MODE_CEL) {
    gl_FragColor = getColor_cel();
    //gl_FragColor = GREEN;
  }
  else if (renderMode == WS_RENDER_MODE_INV) {
    gl_FragColor = vec4(0.0, 1.0, 1.0, 0.4);
  }
  else {
    gl_FragColor = MAGENTA;
  }
}

