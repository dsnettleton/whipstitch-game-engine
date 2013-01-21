/**
 *  wsSound.cpp
 *  Jan 19, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the struct wsSound, which is used to store
 *  information for a single sound object.
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

#include "wsSound.h"

f32 wsSound::gain = 1.0f;

wsSound::wsSound(const char* filePath) {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    alGenSources(1, &soundSource);
    pitch = 1.0f;
    rolloff = 1.0f;
    refDistance = 50.0f;
    looping = false;
    relative = true;
    update();
    alGenBuffers(1, &soundBuffer);
    i32 errorType;
    soundBuffer = alutCreateBufferFromFile(filePath);
    errorType = alutGetError();
    if (errorType) {
      wsLog(WS_LOG_SOUND | WS_LOG_ERROR, "Error Loading Sound file \"%s\": %s\n", filePath, alutGetErrorString(errorType));
    }
    alSourcei(soundSource, AL_BUFFER, soundBuffer);
  #endif
}

wsSound::~wsSound() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    alDeleteSources(1, &soundSource);
    int error = alGetError();
    if(error != AL_NO_ERROR) { wsLog(WS_LOG_SOUND | WS_LOG_ERROR, "OpenAL error was raised."); }
    alDeleteBuffers(1, &soundBuffer);
    error = alGetError();
    if(error != AL_NO_ERROR) { wsLog(WS_LOG_SOUND | WS_LOG_ERROR, "OpenAL error was raised."); }
  #endif
}

void wsSound::play() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    wsLog(WS_LOG_SOUND, "Playing Sound.");
    update();
    alSourcePlay(soundSource);
  #endif
}

void wsSound::update() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    alSourcef(soundSource, AL_PITCH, pitch);
    alSourcef(soundSource, AL_GAIN, pos.w*gain);
    alSourcei(soundSource, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
    alSource3f(soundSource, AL_POSITION, pos.x, pos.y, pos.z);
    alSource3f(soundSource, AL_VELOCITY, vel.x, vel.y, vel.z);
    alSourcef(soundSource, AL_ROLLOFF_FACTOR, rolloff);
    alSourcef(soundSource, AL_REFERENCE_DISTANCE, refDistance);
    alSourcei(soundSource, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
  #endif
}
