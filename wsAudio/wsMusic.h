/**
 *  wsMusic.h
 *  Jan 19, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the struct wsMusic, which stores the necessary
 *  functions to be operated by the sound manager in order to load,
 *  stream, and play music files in the ogg/vorbis format.
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
