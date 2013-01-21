/**
 *    wsSoundManager.h
 *    Jan 19, 2013
 *    D. Scott Nettleton
 *
 *    This file declares the class wsSoundManager, which is an engine
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
