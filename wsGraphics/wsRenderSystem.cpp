/*
 * wsRenderSystem.cpp
 *
 *  Created on: Sep 10, 2012
 *      Author: dsnettleton
 *
 *      This file declares the class wsRenderSystem, which loads the OpenGL context using
 *      GLEW (GL Extension Wrangler) to initialize all available extensions on the given
 *      platform.
 *
 *      wsRenderSystem is an engine subsytem, and must be initialized via the startUp()
 *      function before it may be used. This is done through the engine startup command
 *      wsInit().
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
#include "wsScreen.h"
#include "wsColors.h"

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

void wsRenderSystem::startUp() {
    _mInitialized = true;
    drawBones = false;
#if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        wsLog(WS_LOG_ERROR | WS_LOG_GRAPHICS, "Could not initialize GLEW: %s\n", glewGetErrorString(glewStatus));
    }
    wsLog(WS_LOG_MAIN | WS_LOG_GRAPHICS,    "GLEW Initialized\n"
                                            "  Vendor: %s\n"
                                            "  Renderer: %s\n"
                                            "  OpenGL Version: %s\n"
                                            "  GLSL Version: %s\n",
                                            (char*)glGetString(GL_VENDOR),
                                            (char*)glGetString(GL_RENDERER),
                                            (char*)glGetString(GL_VERSION),
                                            (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    checkExtensions();
    glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
    glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    wsLog(WS_LOG_GRAPHICS, "Supported OpenGL Extensions:");
    for (int i = 0; i < numExtensions; ++i) {
        wsLog(WS_LOG_GRAPHICS, "    %s\n", (char*)glGetStringi(GL_EXTENSIONS, i));
    }
    wsAssert(versionMajor >= 3, "The Whipstitch Engine only supports OpenGL versions 3.0 and up.");

    //  Create and link the main shader program
    wsLog(WS_LOG_GRAPHICS, "Initializing Primary Shader.\n");
    //  Compile Primary Vertex Shader
    std::ifstream vertFile;
    //vertFile.open("shaderFiles/perFragmentLighting.glslv");
    vertFile.open("shaderFiles/wsShader.glslv");
    //vertFile.open("shaderFiles/brick.glslv");
    wsAssert(vertFile, "Primary Vertex Shader file could not be found");
    std::stringstream vertStream;
    vertStream << vertFile.rdbuf();
    vertFile.close();
    std::string vertFileContents;
    const char* vertString;
    vertFileContents = vertStream.str() + "\0";
    vertString = vertFileContents.c_str();
    i32 status;
    primaryVert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(primaryVert, 1, &vertString, NULL);
    glCompileShader(primaryVert);
    glGetShaderiv(primaryVert, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        i32 logSizeV, charsWrittenV;
        glGetShaderiv(primaryVert, GL_INFO_LOG_LENGTH, &logSizeV);
        char* vertLog = new char[logSizeV];
        glGetShaderInfoLog(primaryVert, logSizeV, &charsWrittenV, vertLog);
        wsLog(WS_LOG_ERROR | WS_LOG_GRAPHICS, "Vertex Shader did not compile:\n    >%s<\n", vertLog);
        delete[] vertLog;
        wsAssert(false, "Vertex Shader did not compile.");
    }

    //  Compile Primary Fragment Shader
    std::ifstream fragFile;
    //fragFile.open("shaderFiles/perFragmentLighting.glslf");
    fragFile.open("shaderFiles/wsShader.glslf");
    //fragFile.open("shaderFiles/brick.glslf");
    wsAssert(fragFile, "Primary Fragment Shader file could not be found");
    std::stringstream fragStream;
    fragStream << fragFile.rdbuf();
    fragFile.close();
    std::string fragFileContents;
    const char* fragString;
    fragFileContents = fragStream.str() + "\0";
    fragString = fragFileContents.c_str();
    primaryFrag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(primaryFrag, 1, &fragString, NULL);
    glCompileShader(primaryFrag);
    glGetShaderiv(primaryFrag, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        i32 logSizeF, charsWrittenF;
        glGetShaderiv(primaryFrag, GL_INFO_LOG_LENGTH, &logSizeF);
        char* fragLog = new char[logSizeF];    //malloc(logSize);
        glGetShaderInfoLog(primaryFrag, logSizeF, &charsWrittenF, fragLog);
        wsLog(WS_LOG_ERROR | WS_LOG_GRAPHICS, "Fragment Shader did not compile:\n    >%s<\n", fragLog);
        delete[] fragLog;
        wsAssert(false, "Frag Shader did not compile.");
    }

    //  Attach shaders to the program object
    primaryShader = glCreateProgram();
    wsAssert(primaryShader, "Error creating primary shader program object");
    glAttachShader(primaryShader, primaryVert);
    glAttachShader(primaryShader, primaryFrag);
    //  Link the primary shader program
    glLinkProgram(primaryShader);
    glGetProgramiv(primaryShader, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        i32 logSize, charsWritten;
        glGetProgramiv(primaryShader, GL_INFO_LOG_LENGTH, &logSize);
        char* shaderLog = new char[logSize];    //malloc(logSize);
        glGetProgramInfoLog(primaryShader, logSize, &charsWritten, shaderLog);
        wsLog(WS_LOG_ERROR | WS_LOG_GRAPHICS, "Primary shader could not be linked:\n     %s\n", shaderLog);
        delete[] shaderLog;
        //wsAssert(false, "Problem linking primary shader program");
    }
    //  Set the Primary Shader as the current shader program
    glUseProgram(primaryShader);
    //  Set uniform variables
    lightingEnabled = true;
    //renderMode = WS_RENDER_MODE_LIT;
    renderMode = WS_RENDER_MODE_CEL;
    glUniform1i(glGetUniformLocation(primaryShader, "lightingEnabled"), lightingEnabled?1:0);
    glUniform1i(glGetUniformLocation(primaryShader, "tex"), 0); //  Texture channel 0 reserved for color maps
    glUniform1i(glGetUniformLocation(primaryShader, "usingTexture"), 1);
    glUniform1i(glGetUniformLocation(primaryShader, "renderMode"), renderMode);
#endif
    meshes = wsNew(wsHashMap<wsMeshContainer*>, wsHashMap<wsMeshContainer*>(wsNextPrime(100)));
    models = wsNew(wsHashMap<wsModel*>, wsHashMap<wsModel*>(wsNextPrime(100)));
}

void wsRenderSystem::addAnimation(wsAnimation* myAnim, const char* modelName) {
    models->retrieve(wsHash(modelName))->addAnimation(myAnim);
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
        it.next();
    }
}

void wsRenderSystem::disable(u32 renderingFeatures) {
    wsAssert(_mInitialized, "Must initialize the rendering system first.");
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
        if ((renderingFeatures & WS_RENDER_CULL_FACE) != 0) {
            glDisable(GL_CULL_FACE);
        }
        if ((renderingFeatures & WS_RENDER_LIGHTING) != 0) {
            glDisable(GL_LIGHTING);
            lightingEnabled = false;
            glUniform1i(glGetUniformLocation(primaryShader, "lightingEnabled"), 0);
        }
        if ((renderingFeatures & WS_RENDER_DEPTH) != 0) {
            glDisable(GL_DEPTH_TEST);
        }
        if ((renderingFeatures & WS_RENDER_SHADER) != 0) {
            glUseProgram(0);
        }
    #endif
    if ((renderingFeatures & WS_RENDER_BONES) != 0) {
        drawBones = false;
    }
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
        if (drawBones) {
            const wsJoint* joints = my->mesh->getJoints();
            disable(WS_RENDER_SHADER);
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
            enable(WS_RENDER_SHADER);
        }
        //*/
        /*
        //  Draw Tags
        const wsTag* tags = my->mesh->getTags();
        glDisable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        vec4* green = wsNew( vec4, vec4(0.0f, 1.0f, 0.0f, 1.0f) );
        vec4* red = wsNew( vec4, vec4(1.0f, 0.0f, 0.0f, 1.0f) );
        glBegin(GL_LINES);
        for (u32 i = 0; i < my->getNumTags(); ++i) {
            glColor4fv((GLfloat*)green);
            glVertex3fv((GLfloat*)&tags[i].pos);
            glColor4fv((GLfloat*)red);
            vec4* arrowPos = wsNew( vec4, vec4(tags[i].pos) );
            arrowPos->y += 3.0f;
            arrowPos->rotate(tags[i].rot);
            glVertex3fv((GLfloat*)arrowPos);
        }
        glEnd();
        glDisable(GL_COLOR_MATERIAL);
        //*/
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
        it.next();
    }
    //  Draw Axes
    disable(WS_RENDER_SHADER);
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
    enable(WS_RENDER_SHADER);
}

void wsRenderSystem::drawModel(u32 modelIndex) {
    wsAssert(_mInitialized, "Must initialize the rendering system first.");
    wsModel* my = models->getArrayItem(modelIndex);
    wsAssert(my != NULL, "Cannot draw mesh; empty reference.");
    const wsMaterial* mats = my->getMesh()->getMats();
    wsAssert(mats != NULL, "Cannot use material; empty reference.");
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_COLOR_MATERIAL);
        glPointSize(3.0f);
        glDisable(GL_CULL_FACE);
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
        if (drawBones) {
            const wsJoint* joints = my->getMesh()->getJoints();
            disable(WS_RENDER_SHADER);
            glDisable(GL_LIGHTING);
            glEnable(GL_COLOR_MATERIAL);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
            vec4 startPos, endPos;
            quat rotation;
            glLineWidth(3.0f);
            //  Draw Joints
            glBegin(GL_LINES);
            for (u32 j = 0; j < my->getMesh()->getNumJoints(); ++j) {
                startPos = joints[j].start;
                endPos = joints[j].end;
                rotation = joints[j].rot;
                //parent = joints[j].parent;
                endPos.rotate(rotation);
                endPos += startPos;
                if (j > 1)
                    glColor4fv((GLfloat*)&YELLOW);
                else if (j > 0)
                    glColor4fv((GLfloat*)&GREEN);
                else
                    glColor4fv((GLfloat*)&CYAN);
                glVertex3fv((GLfloat*)&startPos);
                if (j > 1)
                    glColor4fv((GLfloat*)&RED);
                else if (j > 0)
                    glColor4fv((GLfloat*)&DARK_GREEN);
                else
                    glColor4fv((GLfloat*)&BLUE);
                glVertex3fv((GLfloat*)&endPos);
            }
            glEnd();
            glEnable(GL_LIGHTING);
            glDisable(GL_COLOR_MATERIAL);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_DEPTH_TEST);
            enable(WS_RENDER_SHADER);
        }
        //*/
        /*
        //  Draw Tags
        const wsTag* tags = my->my->getMesh()->getTags();
        glDisable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        vec4* green = wsNew( vec4, vec4(0.0f, 1.0f, 0.0f, 1.0f) );
        vec4* red = wsNew( vec4, vec4(1.0f, 0.0f, 0.0f, 1.0f) );
        glBegin(GL_LINES);
        for (u32 i = 0; i < my->my->getMesh()->getNumTags(); ++i) {
            glColor4fv((GLfloat*)green);
            glVertex3fv((GLfloat*)&tags[i].pos);
            glColor4fv((GLfloat*)red);
            vec4* arrowPos = wsNew( vec4, vec4(tags[i].pos) );
            arrowPos->y += 3.0f;
            arrowPos->rotate(tags[i].rot);
            glVertex3fv((GLfloat*)arrowPos);
        }
        glEnd();
        glDisable(GL_COLOR_MATERIAL);
        //*/
    #endif
}

void wsRenderSystem::drawModels() {
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
    wsHashMap<wsModel*>::iterator it = models->begin();
    for (u32 i = 0; i < models->getLength(); ++i) {
        drawModel(it.mCurrentElement);
        it.next();
    }
    //  Draw Axes
    disable(WS_RENDER_SHADER);
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
    enable(WS_RENDER_SHADER);
}

void wsRenderSystem::enable(u32 renderingFeatures) {
    wsAssert(_mInitialized, "Must initialize the rendering system first.");
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
        if ((renderingFeatures & WS_RENDER_CULL_FACE) != 0) {
            glEnable(GL_CULL_FACE);
        }
        if ((renderingFeatures & WS_RENDER_LIGHTING) != 0) {
            glEnable(GL_LIGHTING);
            lightingEnabled = true;
            glUniform1i(glGetUniformLocation(primaryShader, "lightingEnabled"), 1);
        }
        if ((renderingFeatures & WS_RENDER_DEPTH) != 0) {
            glEnable(GL_DEPTH_TEST);
        }
        if ((renderingFeatures & WS_RENDER_SHADER) != 0) {
            glUseProgram(primaryShader);
        }
    #endif
    if ((renderingFeatures & WS_RENDER_BONES) != 0) {
        drawBones = true;
    }
}

void wsRenderSystem::loadIdentity() {
    wsAssert(_mInitialized, "Must initialize the rendering system first.");
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
        glLoadIdentity();
    #endif
}

void wsRenderSystem::loadTexture(u32* index, const char* filename) {
    wsAssert(_mInitialized, "Must initialize the rendering system first.");
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
        *index = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, *index, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        if (!(*index)) {
            wsLog(WS_LOG_ERROR, "Could not locate texture file: \"%s\"\n    SOIL error: %s\n", filename, SOIL_last_result());
        }
        else {
            wsLog(WS_LOG_GRAPHICS, "Loaded texture file: \"%s\"\n", filename);
        }
    #endif
}

void wsRenderSystem::modelviewMatrix() {
    wsAssert(_mInitialized, "Must initialize the rendering system first.");
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
        glMatrixMode(GL_MODELVIEW);
    #endif
}

void wsRenderSystem::pauseAnimation(const char* modelName) {
    wsAssert(_mInitialized, "Must initialize the rendering system first.");
    models->retrieve(wsHash(modelName))->pauseAnimation();
}

void wsRenderSystem::pauseAnimations() {
    wsHashMap<wsModel*>::iterator it = models->begin();
    for (u32 i = 0; i < models->getLength(); ++i) {
        models->getArrayItem(it.mCurrentElement)->pauseAnimation();
        it.next();
    }
}

void wsRenderSystem::projectionMatrix() {
    wsAssert(_mInitialized, "Must initialize the rendering system first.");
    #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
        glMatrixMode(GL_PROJECTION);
    #endif
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

void wsRenderSystem::swapBuffers() {
    wsAssert(_mInitialized, "Must initialize the rendering system first.");
    #if WS_SCREEN_BACKEND == WS_BACKEND_GLFW
        glfwSwapBuffers();
    #endif
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
        it.next();
    }
}

void wsRenderSystem::shutDown() {
    //  Not much going on here...
}


