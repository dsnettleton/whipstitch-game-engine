/**
 *    wsSoundManager.cpp
 *    Jan 20, 2013
 *    D. Scott Nettleton
 *
 *    This file implements the class wsSoundManager, which is an engine
 *    subsystem for the Whipstitch Game Engine. It handles audio files
 *    including streaming, buffering, and playing.
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
