/*
 * wsTypes.h
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file declares the various types to be used by the whipstitch
 *      game engine. Because memory management is so important to reliable
 *      engine performance, the data types are built to be the same size
 *      on any platform. It is the intention that these data types are used
 *      for all operations within the engine.
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

#ifndef WS_TYPES_H_
#define WS_TYPES_H_

#ifndef NULL
#define NULL 0
#endif

#include <stdint.h>//  Custom reliably-sized data types
#include "boost/filesystem.hpp"

#define wsKB    1024
#define wsMB    1048576
#define wsGB    1073741824

typedef int8_t    i8;   //  8-bit signed integer
typedef uint8_t   u8;   //  8-bit unsigned integer
typedef int16_t   i16;  //  16-bit signed integer
typedef uint16_t  u16;  //  16-bit unsigned integer
typedef int32_t   i32;  //  32-bit signed integer
typedef uint32_t  u32;  //  32-bit unsigned integer
typedef int64_t   i64;  //  64-bit signed integer
typedef uint64_t  u64;  //  64-bit unsigned integer
typedef float     f32;  //  32-bit floating point value
typedef double    f64;  //  64-bit floating point value

struct wsIndexArray {
  u32* indices;
  u32 numIndices;
  u32 handle;
};

//  The following is a hybrid 32-bit int/float format, useful
//  for some lightning-fast floating point operations
union i_f_hybrid {
    i32 i;
    f32 f;
    i_f_hybrid(f32 val = 0.0f) : f(val) {}
    i_f_hybrid(i32 val) : i(val) {}
};
union u_f_hybrid {
    u32 u;
    f32 f;
    u_f_hybrid(f32 val = 0.0f) : f(val) {}
    u_f_hybrid(u32 val) : u(val) {}
};
union i_f_hybrid64 {
    i64 i;
    f64 f;
    i_f_hybrid64(f64 val = 0.0f) : f(val) {}
    i_f_hybrid64(i64 val) : i(val) {}
};
union u_f_hybrid64 {
    u64 u;
    f64 f;
    u_f_hybrid64(f64 val = 0.0f) : f(val) {}
    u_f_hybrid64(u64 val) : u(val) {}
};

extern const i_f_hybrid BIAS_POS;  //  1 * 2^23
extern const i_f_hybrid BIAS_NEG;  //  1.5 * 2^23

//  Converts a floating point value to an integer with
//  greater efficiency than a standard typecast
i32 toInt(f32 myFloat);
i32 toInt_positive(f32 myFloat); //  Optimized for known positive values
i32 toInt_negative(f32 myFloat); //  Optimized for known negative values

//  Typedefs using Boost::filesystem libraries
namespace wsFile = boost::filesystem;
typedef boost::filesystem::path wsPath;

extern wsPath ws_path_cwd;
extern wsPath ws_path_home;
extern wsPath ws_path_log_dir;

#endif
