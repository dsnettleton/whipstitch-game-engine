/*
 * wsRenderSystem.h
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

#ifndef WS_RENDER_SYSTEM_H_
#define WS_RENDER_SYSTEM_H_

#include "wsShader.h"
#include "../wsUtils.h"
#include "../wsAssets.h"

#define WS_BACKEND_OPENGL   0x10
#define WS_BACKEND_SDL      0x20    //  For potential future use
#define WS_BACKEND_DIRECTX  0x40    //  For potential future use

//  Default (preferred) backend is OpenGL
#define WS_GRAPHICS_BACKEND WS_BACKEND_OPENGL

//  Rendering Features
#define WS_RENDER_CULL_FACE 0x00000001
#define WS_RENDER_LIGHTING  0x00000002
#define WS_RENDER_DEPTH     0x00000004
#define WS_RENDER_BONES     0x00000008
#define WS_RENDER_SHADER    0x00000010

#define WS_RENDER_MODE_LIT  1    //  Per-fragment lighting
#define WS_RENDER_MODE_CEL  2    //  Cel shading
#define WS_RENDER_MODE_INV  3    //  Invisible (refracted) shading

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
        //  Shader Variables
        u32 primaryShader;  //  Handle for the primary shader (wsShader.glslv, wsShader.glslf)
        u32 primaryVert;
        u32 primaryFrag;
        u32 renderMode;
        bool lightingEnabled;
        bool drawBones;
        //  Drawing components
        wsHashMap<wsModel*>* models;
        wsHashMap<wsMeshContainer*>* meshes;
        //  True only when the startUp function has been called
        bool _mInitialized;
        //  Private Methods
        void drawMesh(u32 meshIndex);
        void drawModel(u32 modelIndex);
    public:
        /*  Default Constructor and Deconstructor */
        //  As an engine subsystem, the renderer takes no action until explicitly
        //  initialized via the startUp(...) function.
        //  uninitialized via the shutDown() function.
        wsRenderSystem() : _mInitialized(false) {}
        ~wsRenderSystem() {}
        /*  Setters and Getters */
        bool getDrawBones() { return drawBones; }
        u32 getRenderMode() { return renderMode; }
        void setRenderMode(const u32 my) {
            renderMode = my;
            glUniform1i(glGetUniformLocation(primaryShader, "renderMode"), renderMode);
        }
        wsModel* getModel(const char* modelName) { return models->retrieve(wsHash(modelName)); }
        /*  Operational Methods */
        void addAnimation(wsAnimation* myAnim, const char* modelName);
        u32 addMesh(const char* filepath);  //  Adds a mesh and return the mesh's index
        u32 addModel(const char* filepath); //  Adds a 3D model and returns its index
        u32 addModel(const char* modelName, wsMesh* myMesh, u32 maxAnimations = 7);
        void beginAnimation(const char* modelName, const char* animName);
        void checkExtensions();
        void clearScreen();
        void continueAnimation(const char* modelName);
        void continueAnimations();
        void disable(u32 renderingFeatures);
        void drawMeshes();
        void drawModels();
        void enable(u32 renderingFeatures);
        void loadIdentity();
        void loadTexture(u32* index, const char* filename);
        void modelviewMatrix();
        void pauseAnimation(const char* modelName);
        void pauseAnimations();
        void projectionMatrix();
        void setClearColor(const vec4& clearColor);
        void setClearColor(f32 r, f32 g, f32 b, f32 a);
        void setMaterial(const wsMaterial& mat);
        void swapBuffers();
        void updateAnimation(const char* modelName, t32 increment);
        void updateAnimations(t32 increment);
        //  Uninitializes the renderer
        void shutDown();
        //  Initializes the renderer
        void startUp();
};

extern wsRenderSystem wsRenderer;

#endif /* WS_RENDER_SYSTEM_H_ */
