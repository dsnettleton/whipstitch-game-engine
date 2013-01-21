/**
 *    wsSoundManager.cpp
 *    Jan 20, 2013
 *    D. Scott Nettleton
 *
 *    This file implements the class wsSoundManager, which is an engine
 *    subsystem for the Whipstitch Game Engine. It handles audio files
 *    including streaming, buffering, and playing.
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

#include "wsSoundManager.h"

wsSoundManager wsSounds;

void wsSoundManager::startUp() {
  _mInitialized = true;
  masterVolume = 1.0f;
  music = wsNew(wsHashMap<wsMusic*>, wsHashMap<wsMusic*>(WS_DEFAULT_MAX_MUSIC));
  sounds = wsNew(wsHashMap<wsMusic*>, wsHashMap<wsSound*>(WS_DEFAULT_MAX_SOUNDS));
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    alListenerf(AL_GAIN, masterVolume);
    alutInit(NULL, NULL);
  #endif
}

u32 wsSoundManager::addMusic(const char* name, const char* filePath) {
  wsMusic* myMusic = wsNew(wsMusic, wsMusic(filePath));
  return music->insert(wsHash(name), myMusic);
}

u32 wsSoundManager::addMusic(const char* name, wsMusic* myMusic) {
  return music->insert(wsHash(name), myMusic);
}

u32 wsSoundManager::addSound(const char* name, const char* filePath) {
  wsSound* mySound = wsNew(wsSound, wsSound(filePath));
  return sounds->insert(wsHash(name), mySound);
}

u32 wsSoundManager::addSound(const char* name, wsSound* mySound) {
  return sounds->insert(wsHash(name), mySound);
}

void wsSoundManager::setMasterVolume(const f32 volume) {
  wsAssert(_mInitialized, "Must initialize the sound manager first.");
  masterVolume = volume;
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    alListenerf(AL_GAIN, masterVolume);
  #endif
}

void wsSoundManager::playSound(const char* name) {
  sounds->retrieve(wsHash(name))->play();
}

void wsSoundManager::playMusic(const char* name) {
  music->retrieve(wsHash(name))->play();
}

void wsSoundManager::updateStreams() {
  for (wsHashMap<wsMusic*>::iterator it = music->begin(); it.get() != WS_NULL; ++it) {
    it.get()->updateStream();
  }
}

void wsSoundManager::shutDown() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    alutExit();
  #endif
}
