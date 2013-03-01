/**
 *  wsMusic.cpp
 *  Jan 19, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the struct wsMusic, which stores the necessary
 *  functions to be operated by the sound manager in order to load,
 *  stream, and play music files in the ogg/vorbis format.
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

#include "wsMusic.h"

f32 wsMusic::gain = 1.0f;

wsMusic::wsMusic(const char* filePath, u32 fileType) {
  pos.w = 1.0f;
  pitch = 1.0f;
  rolloff = 0.0f;
  refDistance = 50.0f;
  looping = true;
  relative = false;
  fileLoaded = false;
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    alGenSources(1, &soundSource);
    update();
    alGenBuffers(2, buffers);
  #endif
  if (fileType == WS_MUSIC_FILE_OGG) {
    loadOGG(filePath);
  }
}

wsMusic::~wsMusic() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    alSourceStop(soundSource);
    errorCheck();
    clear();
    alDeleteSources(1, &soundSource);
    errorCheck();
    alDeleteBuffers(2, buffers);
    errorCheck();
    if (fileLoaded) {
      ov_clear(&oggStream);
      fclose(oggFile);
    }
  #endif
}

/// Operational Member Functions

void wsMusic::clear() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    i32 queued;
    alGetSourcei(soundSource, AL_BUFFERS_QUEUED, &queued);
    while(queued--) {
      u32 buffer;
      alSourceUnqueueBuffers(soundSource, 1, &buffer);
    }
  #endif
}

void wsMusic::displayInfo() {
  wsLog(WS_LOG_SOUND, "Version          %d\n"
                      "Channels         %d\n"
                      "Rate (Hz)        %ld\n"
                      "Nominal Bitrate  %ld\n",
                      vorbisInfo->version, vorbisInfo->channels, vorbisInfo->rate, vorbisInfo->bitrate_nominal);
}

void wsMusic::errorCheck() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    i32 error = alGetError();
    if (error != AL_NO_ERROR) {
      wsLog(WS_LOG_SOUND | WS_LOG_ERROR, "OpenAL error occurred.\n");
    }
  #endif
}

bool wsMusic::isPlaying() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    i32 state;
    alGetSourcei(soundSource, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
  #endif
}

void wsMusic::loadOGG(const char* filePath) {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    i32 result;
    oggFile = fopen(filePath, "rb");
    wsAssert(oggFile, "Could not open Ogg file.");
    if((result = ov_open(oggFile, &oggStream, NULL, 0)) < 0)     {
      fclose(oggFile);
      wsAssert(false, "Could not open Ogg stream. ");
    }
    vorbisInfo = ov_info(&oggStream, -1);
    vorbisComment = ov_comment(&oggStream, -1);
    if(vorbisInfo->channels == 1) { format = AL_FORMAT_MONO16; }
    else { format = AL_FORMAT_STEREO16;}
    update();
    wsLog(WS_LOG_SOUND, "Opened music file %s\n", filePath);
    //displayInfo();
  #endif
}

bool wsMusic::play() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    if(isPlaying()) {
      return true;
    }
    if(!streamBuffer(buffers[0])) {
      return false;
    }
    if(!streamBuffer(buffers[1])) {
      return false;
    }
    alSourceQueueBuffers(soundSource, 2, buffers);
    alSourcePlay(soundSource);
    return true;
  #endif
}

bool wsMusic::streamBuffer(u32 bufferIndex) {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    char pcm[WS_MUSIC_BUFFER_SIZE];
    i32 size = 0, section, result;
    while(size < WS_MUSIC_BUFFER_SIZE) {
      result = ov_read(&oggStream, pcm + size, WS_MUSIC_BUFFER_SIZE - size, 0, 2, 1, &section);
      if(result > 0) { size += result;}
      else if (result < 0) { return false; }
      else { break; }
    }
    if(size == 0) {
      return false;
    }
    alBufferData(bufferIndex, format, pcm, size, vorbisInfo->rate);
    return true;
  #endif
}

void wsMusic::update() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    alSourcef(soundSource, AL_PITCH, pitch);
    alSourcef(soundSource, AL_GAIN, pos.w*gain);
    alSource3f(soundSource, AL_POSITION, pos.x, pos.y, pos.z);
    alSource3f(soundSource, AL_VELOCITY, vel.x, vel.y, vel.z);
    alSourcef(soundSource, AL_ROLLOFF_FACTOR, rolloff);
    alSourcef(soundSource, AL_REFERENCE_DISTANCE, refDistance);
    alSourcei(soundSource, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
  #endif
}

bool wsMusic::updateStream() {
  #if WS_SOUND_BACKEND == WS_BACKEND_OPENAL
    i32 currentState, processed;
    alGetSourcei(soundSource, AL_SOURCE_STATE, &currentState);
    alGetSourcei(soundSource, AL_BUFFERS_PROCESSED, &processed);
    bool active = true;
    while (processed > 0) {
      u32 buffer;
      alSourceUnqueueBuffers(soundSource, 1, &buffer);
      active = streamBuffer(buffer);
      if (active) {
        alSourceQueueBuffers(soundSource, 1, &buffer);
      }
      --processed;
    }
    if (currentState == AL_STOPPED) {
      ov_time_seek(&oggStream, 0.0);
      if (looping) {
        play();
      }
      else {
        active = false;
      }
    }
    update();
    return active;
  #endif
}
