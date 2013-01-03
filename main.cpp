//  main.cpp
//  D. Scott Nettleton
/*
 *  This is the test file (driver) for the wsMath functions; it will
 *  also be used to generate a file containing tables for quick trig
 *  lookups.
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

#include "ws.h"
#include <stdio.h>
#include "GL/glu.h"

int main(int argc, char** argv) {
#ifdef _PROFILE
    wsActiveLogs = (WS_LOG_PROFILING | WS_LOG_MAIN | WS_LOG_UTIL | WS_LOG_ERROR);
#else
    //wsActiveLogs = (WS_LOG_MAIN | WS_LOG_MEMORY | WS_LOG_ERROR);
    wsActiveLogs = (WS_LOG_DEBUG | WS_LOG_ERROR | WS_LOG_THREADS);
    //wsActiveLogs = WS_LOG_ALL;
#endif
    wsInit("Whipstitch Game Engine", 1280, 720, false, 134217728, 32768);    //  128MB, 32KB
    glClearColor(0.4f, 0.6f, 0.4f, 1.0f);
    glViewport(0, 0, 1280, 720);
    wsRenderer.projectionMatrix();
    wsRenderer.loadIdentity();
    gluPerspective(60.0f, (1280.0f / 720.0f), 2.0f, 100.0f);
    gluLookAt(9.0f, 17.0f, 9.0f, 0.0f, 9.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    wsRenderer.modelviewMatrix();
    wsRenderer.enable(WS_RENDER_DEPTH);
    wsRenderer.disable(WS_RENDER_CULL_FACE);
    //wsRenderer.setRenderMode(WS_RENDER_MODE_LIT);
    wsMesh* griswald = wsNew(wsMesh, wsMesh("/home/dsnettleton/Documents/Programming/LoreArcana/models/Griswald.wsMesh"));
    wsRenderer.addModel("Griswald", griswald, 7);
    wsAnimation* anim_walk = wsNew(wsAnimation, wsAnimation("/home/dsnettleton/Documents/Programming/LoreArcana/models/Walk.wsAnim"));
    wsAnimation* anim_idle = wsNew(wsAnimation, wsAnimation("/home/dsnettleton/Documents/Programming/LoreArcana/models/Idle.wsAnim"));
    wsAnimation* anim_jump = wsNew(wsAnimation, wsAnimation("/home/dsnettleton/Documents/Programming/LoreArcana/models/Jump.wsAnim"));
    wsRenderer.addAnimation(anim_walk, "Griswald");
    wsRenderer.addAnimation(anim_idle, "Griswald");
    wsRenderer.addAnimation(anim_jump, "Griswald");
    wsLog("Animation Name = \"%s\"", anim_idle->getName());
    wsRenderer.beginAnimation("Griswald", "Idle");
    //wsRenderer.getModel("Griswald")->setTimeScale(0.5f);
    wsTask_test testTask[64];
    for (u32 i = 0; i < 64; ++i) {
        testTask[i].iVal = i;
        wsThreads.pushTask(&testTask[i]);
    }

    wsGame.beginLoop();

    wsQuit();
    return 0;
}
