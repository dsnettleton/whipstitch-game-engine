/*
 * wsRenderSystem.cpp
 *
 *  Created on: Sep 10, 2012
 *    Author: dsnettleton
 *
 *    This file declares the class wsRenderSystem, which loads the OpenGL context using
 *    GLEW (GL Extension Wrangler) to initialize all available extensions on the given
 *    platform.
 *
 *    wsRenderSystem is an engine subsytem, and must be initialized via the startUp()
 *    function before it may be used. This is done through the engine startup command
 *    wsInit().
 *
 *  Copyright D. Scott Nettleton, 2013
 *  This software is released under the terms of the
 *  Lesser GNU Public License (LGPL).
 *  This file is part of the Whipstitch Game Engine.
 *  The Whipstitch Game Engine is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  The Whipstitch Game Engine is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *  You should have received a copy of the GNU Lesser Public License
 *  along with The Whipstitch Game Engine.
 *  If not, see <http://www.gnu.org/licenses/>.
*/
 
#include "wsRenderSystem.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <sstream>
#include "wsScreenManager.h"
#include "wsColors.h"
#include "../wsGameFlow/wsInputManager.h"

wsRenderSystem wsRenderer;

#if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
  #include "GL/glew.h"
  #include "SOIL/SOIL.h"
#endif

//  Indices for framebuffer object array
#define WS_NUM_FRAMEBUFFERS   3
#define WS_FBO_PRIMARY        0
#define WS_FBO_POST_A         1
#define WS_FBO_POST_B         2

#define WS_NUM_SHADERS        6
#define WS_SHADER_INITIAL     0
#define WS_SHADER_FINAL       1
#define WS_SHADER_POST        2
#define WS_SHADER_HUD         3
#define WS_SHADER_OUTLINE     4
#define WS_SHADER_ANTIALIAS   5

#define WS_NUM_FBO_TEX        7
#define WS_FBO_TEX_POS        0
#define WS_FBO_TEX_NORM       1
#define WS_FBO_TEX_TEXTURE    2
#define WS_FBO_TEX_MAT        3
#define WS_FBO_TEX_DEPTH      4
#define WS_FBO_TEX_FINAL_A    5
#define WS_FBO_TEX_FINAL_B    6

wsMeshContainer::wsMeshContainer(const wsMesh* my) {
  mesh = my;
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    numIndexArrays = mesh->getNumMaterials();
    indexArrays = wsNewArray(wsIndexArray, numIndexArrays);
    glGenBuffers(1, &vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexArray);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wsVert)*mesh->getNumVerts(), mesh->getVerts(), GL_DYNAMIC_DRAW);
    //  Unbind the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  #endif
}

wsMeshContainer::~wsMeshContainer() {
  for (u32 i = 0; i < numIndexArrays; ++i) {
    glDeleteBuffers(1, &indexArrays[i].handle);
  }
  glDeleteBuffers(1, &vertexArray);
}

void wsRenderSystem::startUp() {
  _mInitialized = true;
  drawFeatures = WS_DRAW_LIGHTING | WS_DRAW_DEPTH | WS_DRAW_CULL_FACE | WS_DRAW_CEL | WS_DRAW_CURSOR | WS_DRAW_OUTLINE | WS_DRAW_ANTIALIAS;
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
      wsLog(WS_LOG_ERROR | WS_LOG_GRAPHICS, "Could not initialize GLEW: %s\n", glewGetErrorString(glewStatus));
    }
    i32 maxColorAttachments;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
    wsLog(WS_LOG_MAIN | WS_LOG_GRAPHICS,  "GLEW Initialized\n"
                                          "  Vendor: %s\n"
                                          "  Renderer: %s\n"
                                          "  OpenGL Version: %s\n"
                                          "  GLSL Version: %s\n"
                                          "  Max Color Attachments: %d\n",
                                          (char*)glGetString(GL_VENDOR),
                                          (char*)glGetString(GL_RENDERER),
                                          (char*)glGetString(GL_VERSION),
                                          (char*)glGetString(GL_SHADING_LANGUAGE_VERSION),
                                          maxColorAttachments);
    checkExtensions();
    glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
    glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    wsLog(WS_LOG_GRAPHICS, "Supported OpenGL Extensions:");
    for (int i = 0; i < numExtensions; ++i) {
      wsLog(WS_LOG_GRAPHICS, "  %s\n", (char*)glGetStringi(GL_EXTENSIONS, i));
    }
    wsAssert(versionMajor >= 3, "The Whipstitch Engine only supports OpenGL versions 3.0 and up.");

  #endif
  currentPostBuffer = renderMode = WS_FBO_TEX_FINAL_A;
  currentFBO = WS_FBO_POST_A;
  frameBufferObjects = NULL;
  frameBufferTextures = NULL;
  shaderWidth = 0;
  shaderHeight = 0;
  initializeShaders(wsScreenWidth, wsScreenHeight);
  hudCam = wsNew(wsCamera, wsCamera("Hud Camera", vec4(0.0f, 0.0f, 1.0f), vec4(0.0f, 0.0f, -1.0f), vec4(0.0f, 1.0f),
    vec4(0.0f, 0.0f, wsScreenWidth, wsScreenHeight), WS_CAMERA_MODE_ORTHO, WS_DEFAULT_FOV, wsScreenWidth/wsScreenHeight, WS_DEFAULT_Z_NEAR,
    WS_DEFAULT_Z_FAR));
  cameras = wsNew(wsHashMap<wsCamera*>, wsHashMap<wsCamera*>(101));
  meshes = wsNew(wsHashMap<wsMeshContainer*>, wsHashMap<wsMeshContainer*>(101));
  models = wsNew(wsHashMap<wsModel*>, wsHashMap<wsModel*>(101));
  panels = wsNew(wsOrderedHashMap<wsPanel*>, wsOrderedHashMap<wsPanel*>(101));
  // wsCamera* defaultCam = wsNew(wsCamera, wsCamera("wsDefault", vec4(9.0f, 17.0f, 9.0f), vec4(0.0f, 0.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f),
  //     vec4(0.0f, 0.0f, 1280.0f, 720.0f), WS_RENDER_MODE_CEL, WS_CAMERA_MODE_PERSP, 60.0f, 16.0f/9.0f, 0.01f, 100.0f));
  if (!(drawFeatures & WS_DRAW_CURSOR)) {
    #if WS_SCREEN_BACKEND == WS_BACKEND_GLFW
      glfwDisable(GLFW_MOUSE_CURSOR);
    #endif
  }
  enable(drawFeatures);
}

void wsRenderSystem::initializeShaders(u32 width, u32 height) {
  shaderWidth = width;
  shaderHeight = height;
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    //  Create Framebuffer Objects
    frameBufferObjects = wsNewArray(u32, WS_NUM_FRAMEBUFFERS);
    frameBufferTextures = wsNewArray(u32, WS_NUM_FBO_TEX);
    glGenFramebuffers(WS_NUM_FRAMEBUFFERS, frameBufferObjects);
    glGenTextures(WS_NUM_FBO_TEX, frameBufferTextures);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjects[WS_FBO_PRIMARY]);
      glBindTexture(GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_POS]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, shaderWidth, shaderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_POS], 0);
      glBindTexture(GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_NORM]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB10_A2, shaderWidth, shaderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_2D);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_NORM], 0);
      glBindTexture(GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_TEXTURE]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, shaderWidth, shaderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_TEXTURE], 0);
      glBindTexture(GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_MAT]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, shaderWidth, shaderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_MAT], 0);
      glBindTexture(GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_DEPTH]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shaderWidth, shaderHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_DEPTH], 0);
      //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_DEPTH], 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    wsAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Problem configuring WS_FBO_PRIMARY.");

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjects[WS_FBO_POST_A]);
      glBindTexture(GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_FINAL_A]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, shaderWidth, shaderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_FINAL_A], 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    wsAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Problem configuring WS_FBO_POST_A.");

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjects[WS_FBO_POST_B]);
      glBindTexture(GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_FINAL_B]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, shaderWidth, shaderHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_FINAL_B], 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    wsAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Problem configuring WS_FBO_POST_B.");

    //  Create shader objects
    shaders = wsNewArray(wsShader*, WS_NUM_SHADERS);
    shaders[WS_SHADER_INITIAL] = wsNew(wsShader, wsShader("shaderFiles/wsInitial.vsh", "shaderFiles/wsInitial.fsh"));
    shaders[WS_SHADER_FINAL] = wsNew(wsShader, wsShader("shaderFiles/wsFullscreen.vsh", "shaderFiles/wsFinal.fsh"));
    shaders[WS_SHADER_POST] = wsNew(wsShader, wsShader("shaderFiles/wsFullscreen.vsh", "shaderFiles/wsPost.fsh"));
    shaders[WS_SHADER_HUD] = wsNew(wsShader, wsShader("shaderFiles/wsHud.vsh", "shaderFiles/wsHud.fsh"));
    shaders[WS_SHADER_OUTLINE] = wsNew(wsShader, wsShader("shaderFiles/wsFullscreen.vsh", "shaderFiles/wsOutline.fsh"));
    shaders[WS_SHADER_ANTIALIAS] = wsNew(wsShader, wsShader("shaderFiles/wsFullscreen.vsh", "shaderFiles/wsAntialiasing.fsh"));
    //  Set uniform variables
    shaders[WS_SHADER_INITIAL]->setUniformInt("colorMap", 0);
    shaders[WS_SHADER_FINAL]->setUniformInt("finalTexture", 0);
    shaders[WS_SHADER_POST]->setUniformInt("colorMap", 0);
    shaders[WS_SHADER_POST]->setUniformInt("materialMap", 1);
    shaders[WS_SHADER_HUD]->setUniformInt("colorMap", 0);
    shaders[WS_SHADER_OUTLINE]->setUniformVec3("outlineColor", 0.0f, 0.0f, 0.0f);
    shaders[WS_SHADER_OUTLINE]->setUniform("zNear", 0.01f);
    shaders[WS_SHADER_OUTLINE]->setUniform("zFar", 100.0f);
    shaders[WS_SHADER_OUTLINE]->setUniformInt("colorMap", 0);
    shaders[WS_SHADER_OUTLINE]->setUniformInt("depthMap", 1);
    shaders[WS_SHADER_ANTIALIAS]->setUniformInt("colorMap", 0);
    shaders[WS_SHADER_ANTIALIAS]->setUniform("screenWidth", shaderWidth);
    shaders[WS_SHADER_ANTIALIAS]->setUniform("screenHeight", shaderHeight);

    shaderBuffers = wsNewArray(u32, 4);
    shaderBuffers[0] = GL_COLOR_ATTACHMENT0;
    shaderBuffers[1] = GL_COLOR_ATTACHMENT1;
    shaderBuffers[2] = GL_COLOR_ATTACHMENT2;
    shaderBuffers[3] = GL_COLOR_ATTACHMENT3;
  #endif
}

void wsRenderSystem::addAnimation(wsAnimation* myAnim, const char* modelName) {
  wsAssert(_mInitialized, "Must initialize the rendering system before adding an animation.");
  models->retrieve(wsHash(modelName))->addAnimation(myAnim);
}

u32 wsRenderSystem::addCamera(wsCamera* myCam) {
  wsAssert(_mInitialized, "Must initialize the rendering system before adding a camera.");
  u32 camHash = wsHash(myCam->getName());
  if (cameras->insert(camHash, myCam) == WS_SUCCESS) {
    return camHash;
  }
  return WS_NULL;
}

u32 wsRenderSystem::addMesh(const char* filepath) {
  wsAssert(_mInitialized, "Must initialize the rendering system before adding a mesh.");
  wsMesh* myMesh = wsNew(wsMesh, wsMesh(filepath));
  wsMeshContainer* container = wsNew(wsMeshContainer, wsMeshContainer(myMesh));
  u32 meshHash = wsHash(filepath);
  if (meshes->insert(meshHash, container) == WS_SUCCESS) {
    const wsMaterial* mats = myMesh->getMats();
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      //  Generate vertex buffer objects for this mesh
      for (u32 i = 0; i < container->numIndexArrays; ++i) {
        const wsTriangle* tris = mats[i].tris;
        //  Create index arrays
        container->indexArrays[i].numIndices = mats[i].numTriangles*3;
        container->indexArrays[i].indices = wsNewArray(u32, container->indexArrays[i].numIndices);
        for (u32 t = 0; t < mats[i].numTriangles; ++t) {
          container->indexArrays[i].indices[t*3] = tris[t].vertIndices[0];
          container->indexArrays[i].indices[t*3+1] = tris[t].vertIndices[1];
          container->indexArrays[i].indices[t*3+2] = tris[t].vertIndices[2];
        }
        //  Generate opengl index arrays
        glGenBuffers(1, &container->indexArrays[i].handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, container->indexArrays[i].handle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32)*container->indexArrays[i].numIndices,
                container->indexArrays[i].indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      }
      //  Unbind the current index array buffer
    #endif
    return meshHash;
  }
  return WS_NULL;
}

u32 wsRenderSystem::addModel(const char* filepath) {
  wsAssert(_mInitialized, "Must initialize the rendering system before adding a model.");
  wsModel* myModel = wsNew(wsModel, wsModel(filepath));
  u32 modelHash = wsHash(filepath);
  if (models->insert(modelHash, myModel) == WS_SUCCESS) {
    return modelHash;
  }
  return WS_NULL;
}// End addModel(filepath) method

u32 wsRenderSystem::addModel(const char* modelName, wsMesh* myMesh, const u32 maxAnimations) {
  wsAssert(_mInitialized, "Must initialize the rendering system before adding a model.");
  wsModel* myModel = wsNew(wsModel, wsModel(modelName, myMesh, maxAnimations));
  u32 modelHash = wsHash(myModel->getName());
  if (models->insert(modelHash, myModel) == WS_SUCCESS) {
    return modelHash;
  }
  return WS_NULL;
}// End addModel(mesh, maxAnimations) method

u32 wsRenderSystem::addModel(wsModel* myModel) {
  wsAssert(_mInitialized, "Must initialize the rendering system before adding a model.");
  u32 modelHash = wsHash(myModel->getName());
  if (models->insert(modelHash, myModel) == WS_SUCCESS) {
    return modelHash;
  }
  return WS_NULL;
}// End addModel(wsModel*) method

u32 wsRenderSystem::addPanel(const char* panelName, wsPanel* myPanel) {
  wsAssert(_mInitialized, "Must initialize the rendering system before adding a panel.");
  u32 panelHash = wsHash(panelName);
  if (panels->insert(panelHash, myPanel, myPanel->getLayer()) == WS_SUCCESS) {
    return panelHash;
  }
  return WS_NULL;
}

void wsRenderSystem::beginAnimation(const char* modelName, const char* animName) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  wsLog(WS_LOG_GRAPHICS, "Model %s - Beginning Animation: %s\n", modelName, animName);
  models->retrieve(wsHash(modelName))->beginAnimation(animName);
}

void wsRenderSystem::checkExtensions() {
  wsAssert(_mInitialized, "Must initialize the rendering system before checking extensions.");
  if (GLEW_EXT_framebuffer_object) {
    wsLog(WS_LOG_GRAPHICS, "Framebuffer Supported.\n");
  }
  else {
    wsLog(WS_LOG_GRAPHICS, "Frambuffer Not Supported.\n");
  }
  if (GLEW_EXT_vertex_array) {
    wsLog(WS_LOG_GRAPHICS, "Vertex Arrays Supported.\n");
  }
  else {
    wsLog(WS_LOG_GRAPHICS, "Vertex Arrays Not Supported.\n");
  }
  if (GLEW_ARB_vertex_buffer_object) {
    wsLog(WS_LOG_GRAPHICS, "Vertex Buffer Objects Supported.\n");
  }
  else {
    wsLog(WS_LOG_GRAPHICS, "Vertex Buffer Objects Not Supported.\n");
  }
  if (GLEW_ARB_vertex_shader) {
    wsLog(WS_LOG_GRAPHICS, "Vertex Shaders supported.\n");
  }
  else {
    wsLog(WS_LOG_GRAPHICS, "Vertex shaders not supported. :(\n");
  }
  if (GLEW_ARB_fragment_shader) {
    wsLog(WS_LOG_GRAPHICS, "Fragment Shaders supported.\n");
  }
  else {
    wsLog(WS_LOG_GRAPHICS, "Fragment shaders not supported. :(\n");
  }
}

void wsRenderSystem::clearScreen() {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  #endif
}

void wsRenderSystem::continueAnimation(const char* modelName) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  models->retrieve(wsHash(modelName))->continueAnimation();
}

void wsRenderSystem::continueAnimations() {
  wsHashMap<wsModel*>::iterator it = models->begin();
  for (u32 i = 0; i < models->getLength(); ++i) {
    models->getArrayItem(it.mCurrentElement)->continueAnimation();
    ++it;
  }
}

void wsRenderSystem::disable(u32 renderingFeatures) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    if (renderingFeatures & WS_DRAW_CULL_FACE) {
      glDisable(GL_CULL_FACE);
    }
    if (renderingFeatures & WS_DRAW_LIGHTING) {
      glDisable(GL_LIGHTING);
      // shader->setUniformInt("lightingEnabled", 0);
    }
    if (renderingFeatures & WS_DRAW_DEPTH) {
      glDisable(GL_DEPTH_TEST);
    }
    if (renderingFeatures & WS_DRAW_TEXTURES) {
      glDisable(GL_TEXTURE_2D);
    }
    if (renderingFeatures & WS_DRAW_CURSOR) {
      #if WS_SCREEN_BACKEND == WS_BACKEND_GLFW
        glfwDisable(GLFW_MOUSE_CURSOR);
      #endif
    }
  #endif
  renderingFeatures ^= drawFeatures;
  drawFeatures &= renderingFeatures;
}

void wsRenderSystem::drawMesh(u32 meshIndex) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  const wsMeshContainer* my = meshes->getArrayItem(meshIndex);
  wsAssert(my != NULL, "Cannot draw mesh; empty reference.");
  const wsMaterial* mats = my->mesh->getMats();
  wsAssert(mats != NULL, "Cannot use material; empty reference.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
    glPointSize(3.0f);
    glDisable(GL_CULL_FACE);
    glBindBuffer(GL_ARRAY_BUFFER, my->vertexArray);
    glTexCoordPointer(2, GL_FLOAT, sizeof(wsVert), WS_BUFFER_OFFSET(32));
    glNormalPointer(GL_FLOAT, sizeof(wsVert), WS_BUFFER_OFFSET(16));
    glVertexPointer(3, GL_FLOAT, sizeof(wsVert), WS_BUFFER_OFFSET(0));
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    for (u32 m = 0; m < my->mesh->getNumMaterials(); ++m) {
      setMaterial(mats[m]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my->indexArrays[m].handle);
      glDrawElements(GL_TRIANGLES, my->indexArrays[m].numIndices, GL_UNSIGNED_INT, WS_BUFFER_OFFSET(0));
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    if ((drawFeatures & WS_DRAW_BONES) && my->mesh->getNumJoints()) {
      const wsJoint* joints = my->mesh->getJoints();
      glDisable(GL_LIGHTING);
      glEnable(GL_COLOR_MATERIAL);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_DEPTH_TEST);
      vec4 startPos, endPos;
      glLineWidth(3.0f);
      //  Draw Joints
      glBegin(GL_LINES);
      for (u32 j = 0; j < my->mesh->getNumJoints(); ++j) {
        startPos = joints[j].start;
        endPos = joints[j].end;
        if (joints[j].parent >= 0) {
          startPos.rotate(joints[0].rot);
          endPos.rotate(joints[0].rot);
          startPos += joints[0].end;
          endPos += joints[0].end;
        }
        if (j) {
          glColor4fv((GLfloat*)&YELLOW);
          glVertex3fv((GLfloat*)&startPos);
          glColor4fv((GLfloat*)&ORANGE);
          glVertex3fv((GLfloat*)&endPos);
        }
      }
      glEnd();
      glEnable(GL_LIGHTING);
      glDisable(GL_COLOR_MATERIAL);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_DEPTH_TEST);
    }
  #endif
}

void wsRenderSystem::drawMeshes() {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  vec4 lightPos(10.0f, 20.0f, 10.0f, 1.0f);
  vec4 lightCol(1.0f, 1.0f, 1.0f, 1.0f);
  vec4 lightAmb(0.1f, 0.1f, 0.1f, 1.0f);
  glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat*)&lightPos);
  glLightfv(GL_LIGHT0, GL_AMBIENT, (GLfloat*)&lightAmb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, (GLfloat*)&lightCol);
  glLightfv(GL_LIGHT0, GL_SPECULAR, (GLfloat*)&lightCol);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  //  Actual Drawing of Meshes goes here
  glShadeModel(GL_SMOOTH);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  wsHashMap<wsMeshContainer*>::iterator it = meshes->begin();
  for (u32 i = 0; i < meshes->getLength(); ++i) {
    drawMesh(it.mCurrentElement);
    ++it;
  }
  //  Draw Axes
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glLineWidth(1.0f);
  glBegin(GL_LINES);
  glColor4fv((GLfloat*)&RED);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(10.0f, 0.0f, 0.0f);
  glColor4fv((GLfloat*)&GREEN);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 10.0f, 0.0f);
  glColor4fv((GLfloat*)&BLUE);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 10.0f);
  glEnd();
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
}

void wsRenderSystem::drawModel(u32 modelIndex) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  wsModel* my = models->getArrayItem(modelIndex);
  wsAssert(my != NULL, "Cannot draw mesh; empty reference.");
  const wsMaterial* mats = my->getMesh()->getMats();
  wsAssert(mats != NULL, "Cannot use material; empty reference.");
  mat4 transform = my->getTransform().toMatrix();
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glPushMatrix();
    glMultMatrixf((GLfloat*)&transform);
    if (my->getAttachment() != WS_NULL) {   //  This is attached to another model
      transform.loadIdentity();
      transform.setRotation(my->getAttachment()->rot);
      transform.setTranslation(my->getAttachment()->start);
      glMultMatrixf((GLfloat*)&transform);
    }

    //enable(WS_DRAW_TEXTURES);
    glDisable(GL_COLOR_MATERIAL);
    glPointSize(3.0f);
    //enable(WS_DRAW_CULL_FACE);
    glBindBuffer(GL_ARRAY_BUFFER, my->getVertexArray());
    glTexCoordPointer(2, GL_FLOAT, sizeof(wsVert), WS_BUFFER_OFFSET(32));
    glNormalPointer(GL_FLOAT, sizeof(wsVert), WS_BUFFER_OFFSET(16));
    glVertexPointer(3, GL_FLOAT, sizeof(wsVert), WS_BUFFER_OFFSET(0));
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    for (u32 m = 0; m < my->getMesh()->getNumMaterials(); ++m) {
      setMaterial(mats[m]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my->getIndexArrays()[m].handle);
      glDrawElements(GL_TRIANGLES, my->getIndexArrays()[m].numIndices, GL_UNSIGNED_INT, WS_BUFFER_OFFSET(0));
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    if ((drawFeatures & WS_DRAW_BONES) && my->getMesh()->getNumJoints()) {
      const wsJoint* joints = my->getMesh()->getJoints();
      glEnable(GL_COLOR_MATERIAL);
      disable(WS_DRAW_TEXTURES | WS_DRAW_DEPTH | WS_DRAW_LIGHTING);
      vec4 endPos;
      quat rotation;
      glLineWidth(3.0f);
      //  Draw Joints
      glBegin(GL_LINES);
      for (u32 j = 0; j < my->getMesh()->getNumJoints(); ++j) {
        endPos = joints[j].end;
        endPos.rotate(joints[j].rot);
        endPos += joints[j].start;

        glColor4fv((GLfloat*)&YELLOW);
        glVertex3fv((GLfloat*)&joints[j].start);
        glColor4fv((GLfloat*)&RED);
        glVertex3fv((GLfloat*)&endPos);
      }
      glEnd();
      glDisable(GL_COLOR_MATERIAL);
      glEnable(GL_LIGHTING);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_DEPTH_TEST);
      enable(WS_DRAW_TEXTURES | WS_DRAW_DEPTH | WS_DRAW_LIGHTING);
    }
    glPopMatrix();
  #endif
}

void wsRenderSystem::drawModels() {
  for (wsHashMap<wsModel*>::iterator mod = models->begin(); mod.get() != WS_NULL; ++mod) {
    drawModel(mod.mCurrentElement);
  }
}

void wsRenderSystem::drawPanels() {
  shaders[WS_SHADER_HUD]->use();
  hudCam->draw();
  disable(WS_DRAW_DEPTH | WS_DRAW_LIGHTING);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  for (wsOrderedHashMap<wsPanel*>::iterator pan = panels->begin(); pan.get() != WS_NULL; ++pan) {
    pan.get()->draw();
  }
  enable(WS_DRAW_DEPTH | WS_DRAW_LIGHTING | WS_DRAW_TEXTURES);
}

void wsRenderSystem::drawPost() { //  Post-processing effects
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObjects[currentFBO]);
  // glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT); // Push our glEnable and glViewport states
  glViewport(0, 0, shaderWidth, shaderHeight);
  glDrawBuffers(1, shaderBuffers);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_TEXTURE]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_MAT]);

  // clearScreen();
  // glLoadIdentity();

  shaders[WS_SHADER_POST]->use();
  glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(0.0f, 1.0f);
  glEnd();
  // glPopAttrib();
  if (drawFeatures & WS_DRAW_OUTLINE) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frameBufferTextures[currentPostBuffer]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, frameBufferTextures[WS_FBO_TEX_DEPTH]);
    swapPostBuffer();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObjects[currentFBO]);
    // glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT); // Push our glEnable and glViewport states
    glDrawBuffers(1, shaderBuffers);
    // clearScreen();
    // glLoadIdentity();
    shaders[WS_SHADER_OUTLINE]->use();
    glBegin(GL_QUADS);
      glVertex2f(0.0f, 0.0f);
      glVertex2f(1.0f, 0.0f);
      glVertex2f(1.0f, 1.0f);
      glVertex2f(0.0f, 1.0f);
    glEnd();
    // glPopAttrib();
  }

  if (drawFeatures & WS_DRAW_ANTIALIAS) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frameBufferTextures[currentPostBuffer]);
    swapPostBuffer();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObjects[currentFBO]);
    // glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT); // Push our glEnable and glViewport states
    glDrawBuffers(1, shaderBuffers);
    // clearScreen();
    // glLoadIdentity();
    shaders[WS_SHADER_ANTIALIAS]->use();
    glBegin(GL_QUADS);
      glVertex2f(0.0f, 0.0f);
      glVertex2f(1.0f, 0.0f);
      glVertex2f(1.0f, 1.0f);
      glVertex2f(0.0f, 1.0f);
    glEnd();
    // glPopAttrib();
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void wsRenderSystem::drawScene() {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  vec4 lightPos(10.0f, 20.0f, 10.0f, 1.0f);
  vec4 lightCol(1.0f, 1.0f, 1.0f, 1.0f);
  vec4 lightAmb(0.1f, 0.1f, 0.1f, 1.0f);
  glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat*)&lightPos);
  glLightfv(GL_LIGHT0, GL_AMBIENT, (GLfloat*)&lightAmb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, (GLfloat*)&lightCol);
  glLightfv(GL_LIGHT0, GL_SPECULAR, (GLfloat*)&lightCol);
  glEnable(GL_LIGHT0);

  shaders[WS_SHADER_INITIAL]->use();
  shaders[WS_SHADER_INITIAL]->setUniformInt("celShaded", (drawFeatures & WS_DRAW_CEL)? 1:0 );
  shaders[WS_SHADER_INITIAL]->setUniformInt("lightingEnabled", (drawFeatures & WS_DRAW_LIGHTING)? 1:0 );

  glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjects[WS_FBO_PRIMARY]);
  glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT); // Push our glEnable and glViewport states
  glViewport(0, 0, shaderWidth, shaderHeight);
  glDrawBuffers(4, shaderBuffers);
  clearScreen();
  glLoadIdentity();
  
  for (wsHashMap<wsCamera*>::iterator cam = cameras->begin(); cam.get() != WS_NULL; ++cam) {
    cam.get()->draw();
    shaders[WS_SHADER_INITIAL]->setUniformVec3("eyePos", cam.get()->getPos());

    drawModels();
  }

  if (drawFeatures & WS_DRAW_AXES) {
    //  Draw Axes
    disable(WS_DRAW_TEXTURES | WS_DRAW_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glColor4fv((GLfloat*)&RED);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(10.0f, 0.0f, 0.0f);
    glColor4fv((GLfloat*)&GREEN);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 10.0f, 0.0f);
    glColor4fv((GLfloat*)&BLUE);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 10.0f);
    glEnd();
    glDisable(GL_COLOR_MATERIAL);
    enable(WS_DRAW_TEXTURES | WS_DRAW_LIGHTING);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  drawPost();

  clearScreen();
  glViewport(0, 0, shaderWidth, shaderHeight);
  glBindTexture(GL_TEXTURE_2D, frameBufferTextures[renderMode]);

  shaders[WS_SHADER_FINAL]->use();

  glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(0.0f, 1.0f);
  glEnd();

  glPopAttrib();

  drawPanels();
  wsShader::end();
  
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  swapBuffers();
}

void wsRenderSystem::enable(u32 renderingFeatures) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  drawFeatures |= renderingFeatures;
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    if (renderingFeatures & WS_DRAW_CULL_FACE) {
      glEnable(GL_CULL_FACE);
    }
    if (renderingFeatures & WS_DRAW_LIGHTING) {
      glEnable(GL_LIGHTING);
      // shader->setUniformInt("lightingEnabled", 1);
    }
    if (renderingFeatures & WS_DRAW_DEPTH) {
      glEnable(GL_DEPTH_TEST);
    }
    if (renderingFeatures & WS_DRAW_TEXTURES) {
      glEnable(GL_TEXTURE_2D);
    }
    if (renderingFeatures & WS_DRAW_CURSOR) {
      #if WS_SCREEN_BACKEND == WS_BACKEND_GLFW
        glfwEnable(GLFW_MOUSE_CURSOR);
      #endif
    }
  #endif
}

void wsRenderSystem::loadIdentity() {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glLoadIdentity();
  #endif
}

void wsRenderSystem::loadTexture(u32* index, const char* filename, bool autoSmooth) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    *index = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, *index, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    if (!(*index)) {
      wsLog(WS_LOG_ERROR, "Could not locate texture file: \"%s\"\n  SOIL error: %s\n", filename, SOIL_last_result());
    }
    else {
      wsLog(WS_LOG_GRAPHICS, "Loaded texture file: \"%s\"\n", filename);
      if (autoSmooth) {
        glBindTexture(GL_TEXTURE_2D, *index);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
      }
      else {
        glBindTexture(GL_TEXTURE_2D, *index);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
      }
    }
  #endif
}

void wsRenderSystem::modelviewMatrix() {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glMatrixMode(GL_MODELVIEW);
  #endif
}

void wsRenderSystem::nextRenderMode() {
  if (renderMode == WS_FBO_TEX_FINAL_A) { ++renderMode; }
  renderMode = (renderMode + 1) % WS_NUM_FBO_TEX;
  if (renderMode == WS_FBO_TEX_DEPTH) { ++renderMode; }
}

void wsRenderSystem::pauseAnimation(const char* modelName) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  models->retrieve(wsHash(modelName))->pauseAnimation();
}

void wsRenderSystem::pauseAnimations() {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  wsHashMap<wsModel*>::iterator it = models->begin();
  for (u32 i = 0; i < models->getLength(); ++i) {
    models->getArrayItem(it.mCurrentElement)->pauseAnimation();
    ++it;
  }
}

void wsRenderSystem::projectionMatrix() {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glMatrixMode(GL_PROJECTION);
  #endif
}

void wsRenderSystem::scanHUD() {
  f32 mouseX = wsInputs.getMouseX();
  f32 mouseY = wsInputs.getMouseY();
  //  Convert to screen space coordinates (1600x900 grid)
  mouseX *= WS_HUD_WIDTH / wsScreenWidth;
  mouseY *= WS_HUD_HEIGHT / wsScreenHeight;
  for (wsOrderedHashMap<wsPanel*>::iterator pan = panels->begin(); pan.get() != WS_NULL; ++pan) {
    pan.get()->checkMouse(mouseX, mouseY, wsInputs.getMouseDown());
  }
}

void wsRenderSystem::setCameraMode(const char* cameraName, u32 cameraMode) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  cameras->retrieve(wsHash(cameraName))->setCameraMode(cameraMode);
}

void wsRenderSystem::setClearColor(const vec4& clearColor) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
  #endif
}

void wsRenderSystem::setClearColor(f32 r, f32 g, f32 b, f32 a) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glClearColor(r, g, b, a);
  #endif
}

void wsRenderSystem::setMaterial(const wsMaterial& mat) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat*)&mat.ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat*)&mat.diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat*)&mat.specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat*)&mat.emissive);
    glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);
    glBindTexture(GL_TEXTURE_2D, mat.colorMap);
  #endif
}

void wsRenderSystem::setPos(const char* modelName, const vec4& pos) {
  models->retrieve(wsHash(modelName))->setPos(pos);
}

void wsRenderSystem::setRotation(const char* modelName, const quat& rot) {
  models->retrieve(wsHash(modelName))->setRotation(rot);
}

void wsRenderSystem::setScale(const char* modelName, const f32 scale) {
  models->retrieve(wsHash(modelName))->setScale(scale);
}

void wsRenderSystem::swapBuffers() {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  wsScreens.swapBuffers();
}

void wsRenderSystem::swapPostBuffer() {
  if (currentPostBuffer == WS_FBO_TEX_FINAL_A) {
    currentPostBuffer = WS_FBO_TEX_FINAL_B;
    currentFBO = WS_FBO_POST_B;
  }
  else {
    currentPostBuffer = WS_FBO_TEX_FINAL_A;
    currentFBO = WS_FBO_POST_A;
  }
  if (renderMode == WS_FBO_TEX_FINAL_A || renderMode == WS_FBO_TEX_FINAL_B) {
    renderMode = currentPostBuffer;
  }
}

void wsRenderSystem::updateAnimation(const char* modelName, t32 increment) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  models->retrieve(wsHash(modelName))->incrementAnimationTime(increment);
}

void wsRenderSystem::updateAnimations(t32 increment) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  wsHashMap<wsModel*>::iterator it = models->begin();
  for (u32 i = 0; i < models->getLength(); ++i) {
    models->getArrayItem(it.mCurrentElement)->incrementAnimationTime(increment);
    ++it;
  }
}

void wsRenderSystem::shutDown() {
  //  Not much going on here...
}


