/**
 *  wsMusic.h
 *  Jan 19, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the struct wsMusic, which stores the necessary
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
#ifndef WS_MUSIC_H_
#define WS_MUSIC_H_

#include "wsSound.h"
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#ifndef WS_MUSIC_BUFFER_SIZE
  #define WS_MUSIC_BUFFER_SIZE 262144  //  256 kilobyte buffer
#endif

#define WS_MUSIC_FILE_OGG 0x0001

struct wsMusic {
  vec4 pos; //  w value indicates volume
  vec4 vel; //  velocity of the sound source
  u32 buffers[2]; //  Sound is double-buffered for audio streaming
  vorbis_info* vorbisInfo;
  vorbis_comment* vorbisComment;
  FILE* oggFile;
  OggVorbis_File oggStream;
  f32 refDistance;  //  Distance at which the sound becomes half as loud
  f32 rolloff;  //  factor by which the sound is attenuated over distance
  f32 pitch;
  static f32 gain;
  u32 soundBuffer;
  u32 soundSource;
  i32 format;
  bool looping;
  bool relative;
  bool fileLoaded;
  //  Constructors and Deconstructors
  wsMusic(const char* filePath, u32 fileType = WS_MUSIC_FILE_OGG);
  ~wsMusic();
  //  Operational Methods
  void clear();
  void displayInfo();
  void errorCheck();
  bool isPlaying();
  void loadOGG(const char* filePath);
  bool play();
  bool streamBuffer(u32 bufferIndex);
  void update();
  bool updateStream();
};

#endif //  WS_MUSIC_H_
