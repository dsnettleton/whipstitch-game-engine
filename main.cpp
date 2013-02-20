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
    wsActiveLogs = (WS_LOG_DEBUG | WS_LOG_ERROR | WS_LOG_UTIL);
    //wsActiveLogs = WS_LOG_ALL;
    //sActiveLogs = WS_LOG_MAIN;
  #endif
  wsInit("Whipstitch Game Engine", 1280, 720, false, 512*wsMB, 32*wsMB);  //  512MB, 32MB
  wsRenderer.setClearColor(0.4f, 0.6f, 0.4f, 1.0f);
  wsMesh* griswald = wsNew(wsMesh, wsMesh("/home/dsnettleton/Documents/Programming/LoreArcana/models/Griswald.wsMesh"));
  wsMesh* bladeWand = wsNew(wsMesh, wsMesh("/home/dsnettleton/Documents/Programming/LoreArcana/models/bladeWand.wsMesh"));
  wsModel* Griswald = wsNew(wsModel, wsModel("Griswald", griswald, 7));
  wsModel* BladeWand = wsNew(wsModel, wsModel("BladeWand", bladeWand, 0));
  wsSound* Click = wsNew(wsSound, wsSound("/home/dsnettleton/Documents/Programming/LoreArcana/sounds/btnClick.wav"));
  wsMusic* Resistors = wsNew(wsMusic, wsMusic("/home/dsnettleton/Documents/Programming/LoreArcana/sounds/music/07. We're the Resistors.ogg"));
  wsSounds.addSound("Click", Click);
  wsSounds.addMusic("Resistors", Resistors);
  wsRenderer.addModel(Griswald);
  wsRenderer.addModel(BladeWand);
  wsAnimation* anim_walk = wsNew(wsAnimation, wsAnimation("/home/dsnettleton/Documents/Programming/LoreArcana/models/Walk.wsAnim"));
  wsAnimation* anim_idle = wsNew(wsAnimation, wsAnimation("/home/dsnettleton/Documents/Programming/LoreArcana/models/Idle.wsAnim"));
  wsAnimation* anim_jump = wsNew(wsAnimation, wsAnimation("/home/dsnettleton/Documents/Programming/LoreArcana/models/Jump.wsAnim"));
  wsRenderer.addAnimation(anim_walk, "Griswald");
  wsRenderer.addAnimation(anim_idle, "Griswald");
  wsRenderer.addAnimation(anim_jump, "Griswald");
  wsLog("Animation Name = \"%s\"", anim_idle->getName());
  wsRenderer.beginAnimation("Griswald", "Idle");
  // Griswald->setTimeScale(0.5f);
  wsFont* fntUbuntu = wsNew(wsFont, wsFont("/home/dsnettleton/Documents/Programming/Eclipse/workspace/Florin/fonts/Ubuntu-B.ttf", 30));
  wsText* txtHello = wsNew(wsText, wsText(vec4(16, 240, 1024, 256), "Hello World!", fntUbuntu, 0, WS_HUD_VISIBLE));
  wsText* txtLine2 = wsNew(wsText, wsText(vec4(16, 204, 1024, 256), "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789", fntUbuntu, 1, WS_HUD_VISIBLE));
  wsText* txtLine3 = wsNew(wsText, wsText(vec4(16, 168, 1024, 256), "abcdefghijklmnopqrstuvwxyz", fntUbuntu, 2, WS_HUD_VISIBLE));
  txtHello->setColor(vec4(0.0f, 1.0f, 1.0f, 1.0f));
  txtLine2->setColor(vec4(0.945f, 0.69f, 0.114f, 1.0f));
  txtLine3->setColor(txtLine2->getColor());

  //wsRenderer.setScale("Griswald", 2.0f);
  //wsRenderer.setScale("BladeWand", 2.0f);
  // BladeWand->setPos( vec4(0.0f, 8.0f, 2.0f) );
  // BladeWand->setRotation( quat(0.0f, 0.0f, 0.707f, 0.707f) );
  Griswald->attachModel(BladeWand,"tag_Hand.r");

  wsPanel* testPanel = wsNew(wsPanel, wsPanel(vec4(10,10,800,450), 1, "test2.png"));
  wsPanel* testPanel2 = wsNew(wsPanel, wsPanel(vec4(30, 260, 800,450), 0, "test.png"));
  wsPanel* textBlock = wsNew(wsPanel, wsPanel(vec4(288, 64, 1024, 256), 5, "textBlock.png", WS_HUD_VISIBLE));
  textBlock->addElement(txtHello);
  textBlock->addElement(txtLine2);
  textBlock->addElement(txtLine3);

  wsRenderer.addPanel("TestPanel", testPanel);
  wsRenderer.addPanel("TestPanel2", testPanel2);
  wsRenderer.addPanel("Text Block", textBlock);

  wsGame.beginLoop();

  wsQuit();
  return 0;
}
