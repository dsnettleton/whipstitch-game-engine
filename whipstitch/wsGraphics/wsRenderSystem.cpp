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
 
#include "wsRenderSystem.h"
#include "wsScreenManager.h"
#include "wsColors.h"
#include "../wsGameFlow/wsInputManager.h"

wsRenderSystem wsRenderer;

#if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
  #include "GL/glew.h"
  #include "SOIL/SOIL.h"
#endif

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

#if (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
  void GLFWCALL wsResizeCallback(i32 width, i32 height) {
    wsRenderer.resize(width, height);
  }
#endif

void wsRenderSystem::startUp() {
  _mInitialized = true;
  drawFeatures =  WS_DRAW_LIGHTING |
                  WS_DRAW_DEPTH |
                  WS_DRAW_CULL_FACE |
                  WS_DRAW_CEL |
                  WS_DRAW_CURSOR |
                  WS_DRAW_NORM_MAPS |
                  WS_DRAW_ANTIALIAS;
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
  imageIndices = wsNew(wsHashMap<u32>, wsHashMap<u32>(WS_MAX_TEXTURES));
  meshes = wsNew(wsHashMap<wsMeshContainer*>, wsHashMap<wsMeshContainer*>(101));
  panels = wsNew(wsOrderedHashMap<wsPanel*>, wsOrderedHashMap<wsPanel*>(101));
  if (!(drawFeatures & WS_DRAW_CURSOR)) {
    #if WS_SCREEN_BACKEND == WS_BACKEND_GLFW
      glfwDisable(GLFW_MOUSE_CURSOR);
    #endif
  }
  #if (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
    glfwSetWindowSizeCallback(wsResizeCallback);
  #endif
  enable(drawFeatures);
}

void wsRenderSystem::initializeShaders(u32 width, u32 height) {
  shaderWidth = (f32)width*1.5f;
  shaderHeight = (f32)height*1.5f;
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
    shaders[WS_SHADER_INITIAL] = wsNew(wsShader, wsShader("shaderFiles/wsInitial.vsh", "shaderFiles/wsInitial.fsh", true));
      shaders[WS_SHADER_INITIAL]->setVertexAttribute("vert_position", WS_VERT_ATTRIB_POSITION);
      shaders[WS_SHADER_INITIAL]->setVertexAttribute("vert_normal", WS_VERT_ATTRIB_NORMAL);
      shaders[WS_SHADER_INITIAL]->setVertexAttribute("vert_texCoords", WS_VERT_ATTRIB_TEX_COORDS);
      shaders[WS_SHADER_INITIAL]->setVertexAttribute("numWeights", WS_VERT_ATTRIB_NUM_WEIGHTS);
      shaders[WS_SHADER_INITIAL]->setVertexAttribute("jointIndex", WS_VERT_ATTRIB_JOINT_INDEX);
      shaders[WS_SHADER_INITIAL]->setVertexAttribute("influence", WS_VERT_ATTRIB_INFLUENCE);
      shaders[WS_SHADER_INITIAL]->setVertexAttribute("jointIndex2", WS_VERT_ATTRIB_JOINT_INDEX_2);
      shaders[WS_SHADER_INITIAL]->setVertexAttribute("influence2", WS_VERT_ATTRIB_INFLUENCE_2);
    wsAssert((shaders[WS_SHADER_INITIAL]->install()), "Could not install initial shader.");
    shaders[WS_SHADER_FINAL] = wsNew(wsShader, wsShader("shaderFiles/wsFullscreen.vsh", "shaderFiles/wsFinal.fsh"));
    shaders[WS_SHADER_POST] = wsNew(wsShader, wsShader("shaderFiles/wsFullscreen.vsh", "shaderFiles/wsPost.fsh"));
    shaders[WS_SHADER_HUD] = wsNew(wsShader, wsShader("shaderFiles/wsHud.vsh", "shaderFiles/wsHud.fsh"));
    shaders[WS_SHADER_OUTLINE] = wsNew(wsShader, wsShader("shaderFiles/wsFullscreen.vsh", "shaderFiles/wsOutline.fsh"));
    shaders[WS_SHADER_ANTIALIAS] = wsNew(wsShader, wsShader("shaderFiles/wsFullscreen.vsh", "shaderFiles/wsAntialiasing.fsh"));
    shaders[WS_SHADER_DEBUG] = wsNew(wsShader, wsShader("shaderFiles/wsDebug.vsh", "shaderFiles/wsDebug.fsh"));
    //  Set uniform variables
    shaders[WS_SHADER_INITIAL]->setUniformInt("colorMap", 0);
    shaders[WS_SHADER_INITIAL]->setUniformInt("normalMap", 1);
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

u32 wsRenderSystem::addPanel(const char* panelName, wsPanel* myPanel) {
  wsAssert(_mInitialized, "Must initialize the rendering system before adding a panel.");
  u32 panelHash = wsHash(panelName);
  if (panels->insert(panelHash, myPanel, myPanel->getLayer()) == WS_SUCCESS) {
    return panelHash;
  }
  return WS_NULL;
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

void wsRenderSystem::drawModels(wsHashMap<wsModel*>* models) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  wsModel* my;
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glDisable(GL_COLOR_MATERIAL);
    // glPointSize(3.0f);
    glEnableVertexAttribArray(WS_VERT_ATTRIB_NUM_WEIGHTS);
  #endif
  for (wsHashMap<wsModel*>::iterator mod = models->begin(); mod.get() != WS_NULL; ++mod) {
    my = models->getArrayItem(mod.mCurrentElement);
    wsAssert(my != NULL, "Cannot draw mesh; empty reference.");
    const wsMaterial* mats = my->getMesh()->getMats();
    wsAssert(mats != NULL, "Cannot use material; empty reference.");
    mat4 transform = my->getTransform().toMatrix();
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
      wsAssert((my->getNumJoints() <= WS_MAX_JOINTS), "Cannot have more than the max number of joints in a skeleton.");
      shaders[WS_SHADER_INITIAL]->setUniformVec4Array("baseBoneLocs", my->getMesh()->getJointLocations(), my->getNumJoints());
      shaders[WS_SHADER_INITIAL]->setUniformVec4Array("baseBoneRots", (vec4*)my->getMesh()->getJointRotations(), my->getNumJoints());
      shaders[WS_SHADER_INITIAL]->setUniformVec4Array("boneLocs", my->getJointLocations(), my->getNumJoints());
      shaders[WS_SHADER_INITIAL]->setUniformVec4Array("boneRots", (vec4*)my->getJointRotations(), my->getNumJoints());

      glPushMatrix();
        glMultMatrixf((GLfloat*)&transform);
        if (my->getAttachmentLoc() != WS_NULL && my->getAttachmentRot() != WS_NULL) {   //  This is attached to another model
          transform.loadIdentity();
          transform.setRotation(*my->getAttachmentRot());
          transform.setTranslation(*my->getAttachmentLoc());
          glMultMatrixf((GLfloat*)&transform);
        }
        glBindBuffer(GL_ARRAY_BUFFER, my->getVertexArray());
        glVertexAttribPointer(WS_VERT_ATTRIB_POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(wsVert), WS_BUFFER_OFFSET(0));
        glVertexAttribPointer(WS_VERT_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(wsVert), WS_BUFFER_OFFSET(16));
        glVertexAttribPointer(WS_VERT_ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(wsVert), WS_BUFFER_OFFSET(32));
        glVertexAttribPointer(WS_VERT_ATTRIB_NUM_WEIGHTS, 1, GL_INT, GL_FALSE, sizeof(wsVert), WS_BUFFER_OFFSET(40));
        if (my->getMesh()->getNumJoints()) {
          glVertexAttribPointer(WS_VERT_ATTRIB_JOINT_INDEX, 4, GL_INT, GL_FALSE, sizeof(wsVert), WS_BUFFER_OFFSET(44));
          glVertexAttribPointer(WS_VERT_ATTRIB_JOINT_INDEX_2, 4, GL_INT, GL_FALSE, sizeof(wsVert), WS_BUFFER_OFFSET(60));
          glVertexAttribPointer(WS_VERT_ATTRIB_INFLUENCE, 4, GL_FLOAT, GL_FALSE, sizeof(wsVert), WS_BUFFER_OFFSET(76));
          glVertexAttribPointer(WS_VERT_ATTRIB_INFLUENCE_2, 4, GL_FLOAT, GL_FALSE, sizeof(wsVert), WS_BUFFER_OFFSET(92));
          glEnableVertexAttribArray(WS_VERT_ATTRIB_JOINT_INDEX);
          glEnableVertexAttribArray(WS_VERT_ATTRIB_INFLUENCE);
          glEnableVertexAttribArray(WS_VERT_ATTRIB_JOINT_INDEX_2);
          glEnableVertexAttribArray(WS_VERT_ATTRIB_INFLUENCE_2);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDepthFunc(GL_LESS);
        glCullFace(GL_BACK);
        for (u32 m = 0; m < my->getMesh()->getNumMaterials(); ++m) {
          setMaterial(mats[m]);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my->getIndexArrays()[m].handle);
          glDrawElements(GL_TRIANGLES, my->getIndexArrays()[m].numIndices, GL_UNSIGNED_INT, WS_BUFFER_OFFSET(0));
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        if (my->getMesh()->getNumJoints()) {
          glDisableVertexAttribArray(WS_VERT_ATTRIB_JOINT_INDEX);
          glDisableVertexAttribArray(WS_VERT_ATTRIB_INFLUENCE);
          glDisableVertexAttribArray(WS_VERT_ATTRIB_JOINT_INDEX_2);
          glDisableVertexAttribArray(WS_VERT_ATTRIB_INFLUENCE_2);
        }
        if ((drawFeatures & WS_DRAW_BONES) && my->getMesh()->getNumJoints()) {
          //*  Rewrite and draw bones on initial shader XD
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
          enable(WS_DRAW_TEXTURES | WS_DRAW_DEPTH | WS_DRAW_LIGHTING);
          //*/
        }
        if (drawFeatures & WS_DRAW_BOUNDS) {
          shaders[WS_SHADER_DEBUG]->use();
          glEnable(GL_COLOR_MATERIAL);
          disable(WS_DRAW_TEXTURES | WS_DRAW_LIGHTING);
          glLineWidth(2.0f);
          //  Add bounds drawing
          const vec4 myBounds = my->getBounds();
          glPushMatrix();
            glTranslatef(my->getMesh()->getDefaultPos().x, my->getMesh()->getDefaultPos().y, my->getMesh()->getDefaultPos().z);
            glColor4fv((GLfloat*)&RED);
            glBegin(GL_LINE_STRIP);
              glVertex3f(-myBounds.x, -myBounds.y, -myBounds.z);
              glVertex3f(myBounds.x, -myBounds.y, -myBounds.z);
              glVertex3f(myBounds.x, myBounds.y, -myBounds.z);
              glVertex3f(-myBounds.x, myBounds.y, -myBounds.z);
              glVertex3f(-myBounds.x, -myBounds.y, -myBounds.z);
              glVertex3f(-myBounds.x, -myBounds.y, myBounds.z);
              glVertex3f(myBounds.x, -myBounds.y, myBounds.z);
              glVertex3f(myBounds.x, myBounds.y, myBounds.z);
              glVertex3f(-myBounds.x, myBounds.y, myBounds.z);
              glVertex3f(-myBounds.x, -myBounds.y, myBounds.z);
            glEnd();
            glBegin(GL_LINES);
              glVertex3f(myBounds.x, -myBounds.y, -myBounds.z);
              glVertex3f(myBounds.x, -myBounds.y, myBounds.z);
              glVertex3f(myBounds.x, myBounds.y, -myBounds.z);
              glVertex3f(myBounds.x, myBounds.y, myBounds.z);
              glVertex3f(-myBounds.x, myBounds.y, -myBounds.z);
              glVertex3f(-myBounds.x, myBounds.y, myBounds.z);
            glEnd();
          glPopMatrix();
          glDisable(GL_COLOR_MATERIAL);
          enable(WS_DRAW_TEXTURES | WS_DRAW_LIGHTING);
          shaders[WS_SHADER_INITIAL]->use();
        }
      glPopMatrix();
    #endif
  }
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glDisableVertexAttribArray(WS_VERT_ATTRIB_NUM_WEIGHTS);
    glDisableVertexAttribArray(WS_VERT_ATTRIB_TEX_COORDS);
    glDisableVertexAttribArray(WS_VERT_ATTRIB_NORMAL);
    glDisableVertexAttribArray(WS_VERT_ATTRIB_POSITION);
  #endif
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
    glDrawBuffers(1, shaderBuffers);
    shaders[WS_SHADER_OUTLINE]->use();
    glBegin(GL_QUADS);
      glVertex2f(0.0f, 0.0f);
      glVertex2f(1.0f, 0.0f);
      glVertex2f(1.0f, 1.0f);
      glVertex2f(0.0f, 1.0f);
    glEnd();
  }

  if (drawFeatures & WS_DRAW_ANTIALIAS) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frameBufferTextures[currentPostBuffer]);
    swapPostBuffer();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObjects[currentFBO]);
    glDrawBuffers(1, shaderBuffers);
    shaders[WS_SHADER_ANTIALIAS]->use();
    glBegin(GL_QUADS);
      glVertex2f(0.0f, 0.0f);
      glVertex2f(1.0f, 0.0f);
      glVertex2f(1.0f, 1.0f);
      glVertex2f(0.0f, 1.0f);
    glEnd();
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void wsRenderSystem::drawScene(wsScene* myScene) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  vec4 lightPos(10.0f, 20.0f, 10.0f, 1.0f);
  vec4 lightCol(1.0f, 1.0f, 1.0f, 1.0f);
  vec4 lightAmb(0.1f, 0.1f, 0.1f, 1.0f);
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat*)&lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, (GLfloat*)&lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, (GLfloat*)&lightCol);
    glLightfv(GL_LIGHT0, GL_SPECULAR, (GLfloat*)&lightCol);
    glEnable(GL_LIGHT0);
    if (drawFeatures & WS_DRAW_BOUNDS) {
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(1.0f, 1.0f);
    }

    shaders[WS_SHADER_INITIAL]->use();
    shaders[WS_SHADER_INITIAL]->setUniformInt("celShaded", (drawFeatures & WS_DRAW_CEL)? 1:0 );
    shaders[WS_SHADER_INITIAL]->setUniformInt("lightingEnabled", (drawFeatures & WS_DRAW_LIGHTING)? 1:0 );

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjects[WS_FBO_PRIMARY]);
    glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT); // Push our glEnable and glViewport states
    glViewport(0, 0, shaderWidth, shaderHeight);
    glDrawBuffers(4, shaderBuffers);
    clearScreen();
    glLoadIdentity();

    u32 numPrims = myScene->getNumPrimitives();
    wsPrimitive** prims = myScene->getPrimitives();
    
    for (wsHashMap<wsCamera*>::iterator cam = myScene->getCameras()->begin(); cam.get() != WS_NULL; ++cam) {
      cam.get()->draw();
      shaders[WS_SHADER_INITIAL]->setUniformVec3("eyePos", cam.get()->getPos());

      glEnableVertexAttribArray(WS_VERT_ATTRIB_TEX_COORDS);
      glEnableVertexAttribArray(WS_VERT_ATTRIB_NORMAL);
      glEnableVertexAttribArray(WS_VERT_ATTRIB_POSITION);
      for (u32 p = 0; p < numPrims; ++p) {
        //  Drawing is done by the primitive object, since methods vary
        //  greatly by type of primitive.
        prims[p]->draw();
      }
      if (drawFeatures & WS_DRAW_BOUNDS) {
        for (u32 p = 0; p < numPrims; ++p) {
          shaders[WS_SHADER_DEBUG]->use();
          glEnable(GL_COLOR_MATERIAL);
          disable(WS_DRAW_TEXTURES | WS_DRAW_LIGHTING);
          glLineWidth(2.0f);
          //  Add bounds drawing
          prims[p]->drawBounds();
          glDisable(GL_COLOR_MATERIAL);
          enable(WS_DRAW_TEXTURES | WS_DRAW_LIGHTING);
          shaders[WS_SHADER_INITIAL]->use();
        }
      }

      glEnableVertexAttribArray(WS_VERT_ATTRIB_NUM_WEIGHTS);
      drawModels(myScene->getModels());
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
  #endif

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

void wsRenderSystem::loadTexture(u32* index, const char* filename, bool autoSmooth, bool tiling) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    u32 fileHash = wsHash(filename);
    if (!imageIndices->contains(fileHash)) {
      *index = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, *index, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
      if (!(*index)) {
        wsLog(WS_LOG_ERROR, "Could not locate texture file: \"%s\"\n  SOIL error: %s\n", filename, SOIL_last_result());
        if (strcmp(filename, "textures/wsDefaultTexture.png")) {
          loadTexture(index, "textures/wsDefaultTexture.png", autoSmooth);
        }
      }
      else {
        wsLog(WS_LOG_GRAPHICS, "Loaded texture file: \"%s\"\n", filename);
        imageIndices->insert(fileHash, *index);
        if (autoSmooth) {
          glBindTexture(GL_TEXTURE_2D, *index);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            if (tiling) {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }
            else {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
          glBindTexture(GL_TEXTURE_2D, 0);
        }
        else {
          glBindTexture(GL_TEXTURE_2D, *index);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            if (tiling) {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }
            else {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
          glBindTexture(GL_TEXTURE_2D, 0);
        }
      }
    }// End if this index is not in the hashmap
    else {
      wsLog(WS_LOG_GRAPHICS, "Texture file \"%s\" has already been loaded.\n", filename);
      *index = imageIndices->retrieve(fileHash);
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

void wsRenderSystem::projectionMatrix() {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glMatrixMode(GL_PROJECTION);
  #endif
}

void wsRenderSystem::resize(const u32 newWidth, const u32 newHeight) {
  wsScreenWidth = newWidth;
  wsScreenHeight = newHeight;
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mat.colorMap);
    if (drawFeatures & WS_DRAW_NORM_MAPS) {
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, mat.normalMap);
      shaders[WS_SHADER_INITIAL]->setUniformInt("hasNormalMap", mat.normalMap);
    }
    else {
      shaders[WS_SHADER_INITIAL]->setUniformInt("hasNormalMap", 0);
    }
  #endif
}

void wsRenderSystem::setPrimMaterial(const wsPrimMaterial& mat) {
  wsAssert(_mInitialized, "Must initialize the rendering system first.");
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat*)&mat.ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat*)&mat.diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat*)&mat.specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat*)&mat.emissive);
    glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mat.colorMap);
    if (drawFeatures & WS_DRAW_NORM_MAPS) {
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, mat.normalMap);
      shaders[WS_SHADER_INITIAL]->setUniformInt("hasNormalMap", mat.normalMap);
    }
    else {
      shaders[WS_SHADER_INITIAL]->setUniformInt("hasNormalMap", 0);
    }
  #endif
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

void wsRenderSystem::shutDown() {
  //  Not much going on here...
}


