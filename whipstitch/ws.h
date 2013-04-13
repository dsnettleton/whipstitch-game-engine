/*
 * ws.h
 *
 *  Created on: Jul 12, 2012
 *      Author: dsnettleton
 *
 *      Includes all necessary functions, definitions, and subheaders for the
 *      Whipstitch Game Engine.
 *
 *      Linking requires the necessary library options (g++):
 *          -lgomp
 *          -lboost_system
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

#ifndef WS_H_
#define WS_H_

#include "wsUtils.h"
#include "wsGraphics.h"
#include "wsAssets.h"
#include "wsGameFlow.h"
#include "wsAudio.h"

void wsInit(const char* title, const i32 width, const i32 height, bool fullscreen,
                u64 mainMem, u32 frameStackMem);
void wsQuit();

#endif /* WS_H_ */
