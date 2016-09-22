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

#include "../wsConfig.h"
#include "wsShader.h"
#include "../wsUtils.h"
#include "../wsAssets.h"
#include "../wsPrimitives.h"
#include "../wsGameFlow/wsScene.h"

#ifndef WS_GLEW_INCLUDED_
  #include "GL/glew.h"
  #define WS_GLEW_INCLUDED_
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
    wsHashMap<u32>* imageIndices;
    wsHashMap<wsMeshContainer*>* meshes;
    wsOrderedHashMap<wsPanel*>* panels;
    u32 currentPostBuffer;
    u32 currentFBO;
    //  True only when the startUp function has been called
    bool _mInitialized;
    //  Private Methods
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
    /*  Operational Methods */
    u32 addMesh(const char* filepath);  //  Adds a mesh and return the mesh's index
    u32 addPanel(const char* panelName, wsPanel* myPanel);
    void checkExtensions();
    void clearScreen();
    void disable(u32 renderingFeatures);
    void drawModels(wsHashMap<wsModel*>* models);
    void drawPanels();
    void drawPost();    //  Post-processing effects
    void drawScene(wsScene* myScene);
    void enable(u32 renderingFeatures);
    void loadIdentity();
    void loadTexture(u32* index, const char* filename, bool autoSmooth = true, bool tiling = false);
    void modelviewMatrix();
    void nextRenderMode();
    void projectionMatrix();
    void resize(const u32 newWidth, const u32 newHeight);
    void scanHUD(); //  Use mouse position from wsInputs to test ui panels for events, etc.
    void setClearColor(const vec4& clearColor);
    void setClearColor(f32 r, f32 g, f32 b, f32 a);
    void setMaterial(const wsMaterial& mat);
    void setPrimMaterial(const wsPrimMaterial& mat);
    void swapBuffers();
    void swapPostBuffer();
    //  Uninitializes the renderer
    void shutDown();
    //  Initializes the renderer
    void startUp();
};

extern wsRenderSystem wsRenderer;

#endif /* WS_RENDER_SYSTEM_H_ */
