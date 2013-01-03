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

#ifndef WS_H_
#define WS_H_

#include "wsUtils.h"
#include "wsGraphics.h"
#include "wsAssets.h"
#include "wsGameFlow.h"

void wsInit(const char* title, const i32 width, const i32 height, bool fullscreen,
                u64 mainMem, u32 frameStackMem);
void wsQuit();

#endif /* WS_H_ */
