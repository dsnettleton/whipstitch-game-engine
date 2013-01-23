/**
 *  wsCamera.h
 *  Jan 20, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsCamera, which provides an interface for
 *  positioning, aiming, and rotating the graphics display viewport in
 *  the Whipstitch Game Engine. A camera object is given positional
 *  and directional vectors, rendering methods and distances, and a frame
 *  providing its onscreen position and dimensions. The camera projection
 *  can be orthogonal or perspective, optionally drawing a grid for
 *  orthogonal projections.
 *
 *  Onscreen coordinates in the Whipstitch Game Engine are given using
 *  a scale 1600 units wide by 900 units tall. This is for consistency
 *  across many window sizes, and may or may not translate directly into
 *  pixels, depending on the resolution. If the window ratio is not 16:9
 *  (widescreen), the shortened dimension (either height or width) is
 *  clipped from the right or top. The windowspace origin (0,0) is at
 *  the lower left corner of the screen, in order to be sensible.
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
#ifndef WS_CAMERA_H_
#define WS_CAMERA_H_

#include "../wsUtils.h"
// #include "wsShader.h"

// #define WS_RENDER_MODE_LIT          1 //  Per-fragment lighting
// #define WS_RENDER_MODE_CEL          2 //  Cel shading
// #define WS_RENDER_MODE_INV          3 //  Invisible (refracted) shading
// #define WS_RENDER_MODE_CEL_OUTLINED 4 //  Cel Shading with black outline
// #define WS_RENDER_MODE_WIRE         5 //  Wireframe

#define WS_CAMERA_MODE_INACTIVE     0 //  Don't draw
#define WS_CAMERA_MODE_PERSP        1 //  Perspective Mode
#define WS_CAMERA_MODE_ORTHO        2 //  Orthographic Mode

// #ifndef WS_DEFAULT_RENDER_MODE
//   #define WS_DEFAULT_RENDER_MODE   WS_RENDER_MODE_LIT
// #endif

#define WS_DEFAULT_FOV            60.0f
#define WS_DEFAULT_ASPECT_RATIO   (16.0 / 9.0)
#define WS_DEFAULT_Z_NEAR         0.01f
#define WS_DEFAULT_Z_FAR          1000.0f

class wsCamera {
  private:
    //  Private Data Members
    vec4 pos;
    vec4 dir;   //  Normal vector storing the direction which the camera is facing
    vec4 upDir; //  Normal vector storing the direction which is considered "up" by the camera.
    vec4 rightDir;  //  Normal vector storing the direction to the right of the camera.
    vec4 screenCoords;  //  Using rectX, rectY, rectW, rectH
    const char* name;
    u32 renderMode; //  Smooth/cel shaded, wireframe, etc.
    u32 cameraMode; //  Perspective vs. Orthographic Projection
    f32 fov;  //  Field of view in degrees (perspective) or in units (ortho)
    f32 aspectRatio; //  The camera's aspect ratio
    f32 zNear;  //  Closest depth to render
    f32 zFar;   //  Furthest depth to render
  public:
    //  Constructors and Deconstructors
    wsCamera(const char* myName, u32 myCameraMode = WS_CAMERA_MODE_PERSP);
    wsCamera(const char* myName, const vec4& myPos, const vec4& myDir, const vec4& myUpDir, const vec4& myScreenCoords,
     const u32 myCameraMode, const f32 myFov, const f32 myAspectRatio, const f32 myZNear, const f32 myZFar);
    //  Setters and Getters
    const vec4& getPos() const { return pos; }
    const vec4& getDir() { return dir; }
    const vec4& getUpDir() { return upDir; }
    const vec4& getRightDir() { return rightDir; }
    u32 getRenderMode() const { return renderMode; }
    u32 getCameraMode() const { return cameraMode; }
    const char* getName() const { return name; }
    f32 getX() const { return pos.x; }
    f32 getY() const { return pos.y; }
    f32 getZ() const { return pos.z; }
    f32 getFov() const { return fov; }
    f32 getAspectRatio() const { return aspectRatio; }
    f32 getZNear() const { return zNear; }
    f32 getZFar() const { return zFar; }
    const vec4& getScreenCoords() const { return screenCoords; }
    f32 getScreenX() const { return screenCoords.rectX; }
    f32 getScreenY() const { return screenCoords.rectY; }
    f32 getScreenW() const { return screenCoords.rectW; }
    f32 getScreenH() const { return screenCoords.rectH; }
    void setScreenPos(f32 x, f32 y) { screenCoords.rectX = x; screenCoords.rectY = y; }
    void setScreenSize(f32 w, f32 h) { screenCoords.rectW = w; screenCoords.rectH = h; }
    void setCameraMode(u32 myCameraMode) { cameraMode = myCameraMode; }
    void setRenderMode(u32 myRenderMode) { renderMode = myRenderMode; }
    void setPos(const vec4& myPos) { pos = myPos; }
    void setDir(const vec4& myDir) { dir = myDir; }
    void setUpDir(const vec4& myUpDir) { upDir = myUpDir; }
    void setRightDir(const vec4& myRightDir) { rightDir = myRightDir; }
    void setFov(f32 my) { fov = my; }
    void setScreenCoords(const vec4& rect) { screenCoords = rect; aspectRatio = rect.rectW/rect.rectH; }
    void setRange(f32 near, f32 far) { zNear = near; zFar = far; }
    //  Operational Methods
    void draw();      //  Orient and draw the camera in OpenGL
    const vec4 getWorldCoords(const f32 myX, const f32 myY) const; //  Returns game-world positional coordinates translated from the given screen coordinates
    bool isInFrame(const f32 x, const f32 y) const;   //  Checks to see whether the given coordinates fall within the camera's viewport.
    void lookAt(const vec4& focal);  //  Direct the camera to a focal point (positional vector)
    void move(const vec4& vec);  //  Move the camera along the directional vector
    void moveBackward(const f32 amount);    //  Move the camera backward by the specified amount
    void moveDown(const f32 amount);    //  Move the camera down by the specified amount
    void moveForward(const f32 amount);     //  Move the camera forward by the specified amount
    void moveLeft(const f32 amount);        //  Strafe the camera left by the specified amount
    void moveRight(const f32 amount);       //  Strafe the camera right by the specified amount
    void moveUp(const f32 amount);      //  Move the camera up by the specified amount
    void orbit(const vec4& focal, const vec4& axis, const f32 angle);  //  Orbit the camera around a focal point (positional vector) by the provided angle
    void pan(const f32 angle);  //  Pan the camera left or right
    void roll(const f32 angle); //  Roll the camera on its directional axis
    void setScreenCoords(const f32 x, const f32 y, const f32 w, const f32 h);  //  Sets the size and width of the camera's onscreen display.
    void tilt(const f32 angle); //  Tilt the camera up or down
    void updateRightDir();
};

#endif //  WS_CAMERA_H_
