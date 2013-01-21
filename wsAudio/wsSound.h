/**
 *  wsSound.h
 *  Jan 19, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the struct wsSound, which is used to store
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
#ifndef WS_SOUND_H_
#define WS_SOUND_H_

#include "../wsUtils.h"

#define WS_BACKEND_OPENAL 1

//  Default (preferred) backend is OpenAL
#define WS_SOUND_BACKEND WS_BACKEND_OPENAL
#include <AL/alut.h>

struct wsSound {
  vec4 pos; //  w value indicates volume
  vec4 vel; //  velocity of the sound source
  f32 refDistance;  //  Distance at which the sound becomes half as loud
  f32 rolloff;  //  factor by which the sound is attenuated over distance
  f32 pitch;
  static f32 gain;
  u32 soundBuffer;
  u32 soundSource;
  bool looping;
  bool relative;
  wsSound(const char* filePath);
  ~wsSound();
  void play();
  void update();
};

#endif //  WS_SOUND_H_
