/**
 *  wsCamera.cpp
 *  Jan 20, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the class wsCamera, which provides an interface for
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

#include "wsCamera.h"
#include "wsRenderSystem.h"

wsCamera::wsCamera(const char* myName, u32 myCameraMode) :
  pos(0.0f, 0.0f, 10.0f),
  dir(0.0f, 0.0f, 1.0f),
  upDir(0.0f, 1.0f, 0.0f),
  rightDir(1.0f, 0.0f, 0.0f),
  screenCoords(0.0f, 0.0f, WS_HUD_WIDTH, WS_HUD_HEIGHT),
  cameraMode(myCameraMode),
  fov(WS_DEFAULT_FOV),
  aspectRatio(WS_DEFAULT_ASPECT_RATIO),
  zNear(WS_DEFAULT_Z_NEAR),
  zFar(WS_DEFAULT_Z_FAR) {
  name = myName;
}

wsCamera::wsCamera(const char* myName, const vec4& myPos, const vec4& myDir, const vec4& myUpDir, const vec4& myScreenCoords,
  const u32 myCameraMode, const f32 myFov, const f32 myAspectRatio, const f32 myZNear, const f32 myZFar) :
  pos(myPos),
  dir(myDir),
  upDir(myUpDir),
  screenCoords(myScreenCoords),
  cameraMode(myCameraMode),
  fov(myFov),
  aspectRatio(myAspectRatio),
  zNear(myZNear),
  zFar(myZFar) {
  name = myName;
  updateRightDir();
}

void wsCamera::draw() {
  #if WS_GRAPHICS_BACKEND == WS_BACKEND_OPENGL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(screenCoords.rectX, screenCoords.rectY, screenCoords.rectW, screenCoords.rectH);
    switch (cameraMode) {
      default:
      case WS_CAMERA_MODE_INACTIVE:
        break;
      case WS_CAMERA_MODE_PERSP:
        gluPerspective(fov, aspectRatio, zNear, zFar);
        gluLookAt(pos.x, pos.y, pos.z,
          pos.x+dir.x, pos.y+dir.y, pos.z+dir.z,
          upDir.x, upDir.y, upDir.z);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        break;
      case WS_CAMERA_MODE_ORTHO:
        gluOrtho2D(0.0f, WS_HUD_WIDTH, 0.0f, WS_HUD_HEIGHT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        break;
    }
  #endif
  //shader->setUniformVec3("eyePos", pos);
}

const vec4 wsCamera::getWorldCoords(const f32 myX, const f32 myY) const {
  if (cameraMode == WS_CAMERA_MODE_PERSP) { return vec4(); }
  vec4 coords;
  coords.x = (myX - screenCoords.rectX - (screenCoords.rectW/2))/((screenCoords.rectW/2)/(fov*aspectRatio));
  coords.y = (screenCoords.rectY - (screenCoords.rectH/2) - myY)/((screenCoords.rectH/2)/fov);
  coords.rotatePlane(dir, upDir);
  coords += pos;

  if (dir.x == 1 || dir.x == -1) {
    coords.x = 0;
  }
  if (dir.y == 1 || dir.y == -1) {
    coords.y = 0;
  }
  if (dir.z == 1 || dir.z == -1) {
    coords.z = 0;
  }
  return coords;
}

bool wsCamera::isInFrame(f32 x, f32 y) const {
  return (x >= screenCoords.rectX && y >= screenCoords.rectY &&
      x <= screenCoords.rectX+screenCoords.rectW && y <= screenCoords.rectY+screenCoords.rectH);
}

void wsCamera::lookAt(const vec4& focal) {
  if (cameraMode == WS_CAMERA_MODE_ORTHO) { return; }
  dir = (focal - pos).normal();
  if (!dir.perpendicular(upDir)) {     //  A camera initialized looking straight down might have this problem.
    vec4 normal = dir.crossProduct(upDir);
    upDir = normal.crossProduct(dir);
  }
  else {
    if (upDir != vec4(0.0f, 0.99f, 0.01f)) {
      upDir.set(0.0f, 0.99f, 0.01f);  //  Use a slight tilt to the Z-axis to define our up direction
    }
    else {
      upDir.set(0.0f, 1.0f, 0.0f);
      lookAt(focal);
    }
  }
  updateRightDir();
}

void wsCamera::move(const vec4& vec) {
  pos += vec;
}

void wsCamera::moveBackward(const f32 amount) {
  switch (cameraMode) {
    case WS_CAMERA_MODE_PERSP:
      pos -= dir*amount;
      break;
    case WS_CAMERA_MODE_ORTHO:
      fov += amount;
      break;
    default:
      break;
  }
}

void wsCamera::moveDown(const f32 amount) {
  pos -= upDir*amount;
}

void wsCamera::moveForward(const f32 amount) {
  switch (cameraMode) {
    case WS_CAMERA_MODE_PERSP:
      pos += dir*amount;
      break;
    case WS_CAMERA_MODE_ORTHO:
      fov -= amount;
      break;
    default:
      break;
  }
}

void wsCamera::moveLeft(const f32 amount) {
  pos -= rightDir*amount;
}

void wsCamera::moveRight(const f32 amount) {
  pos += rightDir*amount;
}

void wsCamera::moveUp(const f32 amount) {
  pos += upDir*amount;
}

void wsCamera::orbit(const vec4& focal, const vec4& axis, const f32 angle) {
  if (cameraMode != WS_CAMERA_MODE_PERSP) { return; }
  vec4 diff = pos - focal;
  diff.rotate(axis, angle);
  pos = diff + focal;
  dir = -diff.normal();
  upDir.rotate(axis, angle);
  upDir.normalize();
  updateRightDir();
}

void wsCamera::pan(const f32 angle) {
  dir.rotate(upDir, -angle);
  updateRightDir();
  lookAt(pos+dir);   //  Correct bad directional vectors
}

void wsCamera::roll(const f32 angle) {
  upDir.rotate(dir, -angle);
  updateRightDir();
  lookAt(pos+dir);   //  Correct bad directional vectors
}

void wsCamera::setScreenCoords(const f32 x, const f32 y, const f32 w, const f32 h) {
  screenCoords.rectX = x;
  screenCoords.rectY = y;
  screenCoords.rectW = w;
  screenCoords.rectH = h;
  aspectRatio = w/h;
}

void wsCamera::tilt(const f32 angle) {
  dir.rotate(rightDir, -angle);
  upDir.rotate(rightDir, -angle);
  lookAt(pos+dir);   //  Correct bad directional vectors
}

void wsCamera::updateRightDir() {
  rightDir = dir.crossProduct(upDir).normal();
}
