/*
 * wsRenderSystem.h
 *
 *  Created on: Sep 10, 2012
 *  Author: dsnettleton
 *
 *  This file declares the class wsRenderSystem, which loads the OpenGL context using
 *  GLEW (GL Extension Wrangler) to initialize all available extensions on the given
 *  platform.
 *
 *  wsRenderSystem is an engine subsytem, and must be initialized via the startUp()
 *  function before it may be used. This is done through the engine startup command
 *  wsInit().
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2013
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef WS_RENDER_SYSTEM_H_
#define WS_RENDER_SYSTEM_H_

#include "wsShader.h"
#include "../wsUtils.h"
#include "../wsAssets.h"
#include "wsCamera.h"
 
#ifndef WS_GLEW_INCLUDED_
  #include "GL/glew.h"
  #define WS_GLEW_INCLUDED_
#endif

#define WS_BACKEND_OPENGL   0x10
#define WS_BACKEND_SDL  0x20  //  For potential future use
#define WS_BACKEND_DIRECTX  0x40  //  For potential future use

//  Default (preferred) backend is OpenGL
#define WS_GRAPHICS_BACKEND WS_BACKEND_OPENGL

//  Drawing Features
#define WS_DRAW_CULL_FACE 0x00000001
#define WS_DRAW_LIGHTING  0x00000002
#define WS_DRAW_DEPTH     0x00000004
#define WS_DRAW_BONES     0x00000008
#define WS_DRAW_CEL       0x00000010
#define WS_DRAW_AXES      0x00000020
#define WS_DRAW_TEXTURES  0x00000040
#define WS_DRAW_OUTLINE   0x00000080
#define WS_DRAW_CURSOR    0x00000100  //  If enabled, default system cursor is shown
#define WS_DRAW_ANTIALIAS 0x00000200

#ifndef WS_MAX_TEXTURES
  #define WS_MAX_TEXTURES 256
#endif
#ifndef WS_MAX_JOINTS
  #define WS_MAX_JOINTS   64
#endif

struct wsMeshContainer {
  const wsMesh* mesh;
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    wsIndexArray* indexArrays;
    u32 numIndexArrays;
    u32 vertexArray;
  #endif
  wsMeshContainer(const wsMesh* my);
  ~wsMeshContainer();
};

class wsRenderSystem {
  private:
    i32 versionMajor;
    i32 versionMinor;
    i32 numExtensions;
    u32 drawFeatures;
    //  Shader Variables
    wsShader** shaders;
    u32* shaderBuffers;
    u32* frameBufferObjects;
    u32* frameBufferTextures;
    wsCamera* hudCam;
    u32 renderMode, finalFramebuffer;
    i32 shaderWidth, shaderHeight;
    //  Drawing components
    wsHashMap<wsCamera*>* cameras;  //  Change to ordered hashmap
    wsHashMap<u32>* imageIndices;
    wsHashMap<wsModel*>* models;
    wsHashMap<wsMeshContainer*>* meshes;
    wsOrderedHashMap<wsPanel*>* panels;
    u32 currentPostBuffer;
    u32 currentFBO;
    //  True only when the startUp function has been called
    bool _mInitialized;
    //  Private Methods
    void drawMesh(u32 meshIndex);
    void drawModel(u32 modelIndex);
    // void drawPanel(wsPanel* pan);
    void initializeShaders(u32 width, u32 height);
  public:
    /*  Default Constructor and Deconstructor */
    //  As an engine subsystem, the renderer takes no action until explicitly
    //  initialized via the startUp(...) function.
    //  uninitialized via the shutDown() function.
    wsRenderSystem() : _mInitialized(false) {}
    ~wsRenderSystem() {}
    /*  Setters and Getters */
    bool isEnabled(u32 features) { return ((drawFeatures & features) == features); }
    u32 getRenderMode() { return renderMode; }
    void setRenderMode(const u32 my) { renderMode = my; }
    wsCamera* getCamera(const char* cameraName) { return cameras->retrieve(wsHash(cameraName)); }
    wsModel* getModel(const char* modelName) { return models->retrieve(wsHash(modelName)); }
    /*  Operational Methods */
    void addAnimation(wsAnimation* myAnim, const char* modelName);
    u32 addCamera(wsCamera* myCam);
    u32 addMesh(const char* filepath);  //  Adds a mesh and return the mesh's index
    u32 addModel(const char* filepath); //  Adds a 3D model and returns its index
    u32 addModel(const char* modelName, wsMesh* myMesh, u32 maxAnimations = 7);
    u32 addModel(wsModel* myModel);
    u32 addPanel(const char* panelName, wsPanel* myPanel);
    void beginAnimation(const char* modelName, const char* animName);
    void checkExtensions();
    void clearScreen();
    void continueAnimation(const char* modelName);
    void continueAnimations();
    void disable(u32 renderingFeatures);
    void drawMeshes();
    void drawModels();
    void drawPanels();
    void drawPost();    //  Post-processing effects
    void drawScene();
    void enable(u32 renderingFeatures);
    void loadIdentity();
    void loadTexture(u32* index, const char* filename, bool autoSmooth = true);
    void modelviewMatrix();
    void nextRenderMode();
    void pauseAnimation(const char* modelName);
    void pauseAnimations();
    void projectionMatrix();
    void scanHUD(); //  Use mouse position from wsInputs to test ui panels for events, etc.
    void setCameraMode(const char* cameraName, u32 cameraMode);
    void setClearColor(const vec4& clearColor);
    void setClearColor(f32 r, f32 g, f32 b, f32 a);
    void setMaterial(const wsMaterial& mat);
    void setPos(const char* modelName, const vec4& pos);
    void setRotation(const char* modelName, const quat& rot);
    void setScale(const char* modelName, const f32 scale);
    void swapBuffers();
    void swapPostBuffer();
    void updateAnimation(const char* modelName, t32 increment);
    void updateAnimations(t32 increment);
    //  Uninitializes the renderer
    void shutDown();
    //  Initializes the renderer
    void startUp();
};

extern wsRenderSystem wsRenderer;

#endif /* WS_RENDER_SYSTEM_H_ */
