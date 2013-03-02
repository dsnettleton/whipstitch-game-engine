/**
 *  wsSound.h
 *  Jan 19, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the struct wsSound, which is used to store
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
