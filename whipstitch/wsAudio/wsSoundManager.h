/**
 *    wsSoundManager.h
 *    Jan 19, 2013
 *    D. Scott Nettleton
 *
 *    This file declares the class wsSoundManager, which is an engine
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
#ifndef WS_SOUND_MANAGER_H_
#define WS_SOUND_MANAGER_H_

#include "../wsUtils.h"
#include "wsMusic.h"
#include "wsSound.h"

#define WS_DEFAULT_MAX_MUSIC  67
#define WS_DEFAULT_MAX_SOUNDS 97

class wsSoundManager {
  private:
    //  Private Data Members
    vec4 listenerPosition;
    wsHashMap<wsSound*>* sounds;
    wsHashMap<wsMusic*>* music;
    f32 masterVolume;
    bool _mInitialized;
  public:
    /*  Default Constructor and Deconstructor */
    //  As an engine subsystem, the sound manager takes no action until explicitly
    //  initialized via the startUp(...) function.
    //  uninitialized via the shutDown() function.
    wsSoundManager() : _mInitialized(false) {}
    ~wsSoundManager() {}
    void startUp();
    void shutDown();
    //  Setters and Getters
    void setMasterVolume(const f32 volume);
    void setMusicVolume(const f32 volume) { wsMusic::gain = volume; }
    void setSoundVolume(const f32 volume) { wsSound::gain = volume; }
    //  Operational Methods
    u32 addMusic(const char* name, const char* filePath);
    u32 addMusic(const char* name, wsMusic* myMusic);
    u32 addSound(const char* name, const char* filePath);
    u32 addSound(const char* name, wsSound* mySound);
    void playSound(const char* name);
    void playMusic(const char* name);
    void updateStreams();
};

extern wsSoundManager wsSounds;

#endif //  WS_SOUND_MANAGER_H_
