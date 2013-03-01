/**
 *  wsSound.cpp
 *  Jan 19, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the struct wsSound, which is used to store
 *  information for a single sound object.
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
