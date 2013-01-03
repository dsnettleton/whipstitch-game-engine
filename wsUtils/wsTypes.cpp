/*
 * wsTypes.c
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file implements the various types to be used by the whipstitch
 *      game engine. Because memory management is so important to reliable
 *      engine performance, the data types are built to be the same size
 *      on any platform. It is the intention that these data types are used
 *      for all operations within the engine.
 *
 *      This implementation allows faster switching between 32-bit
 *      integer and floating point values.
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

#include "wsTypes.h"
#include "wsOperations.h"

wsPath ws_path_cwd(wsFile::current_path());
wsPath ws_path_home(getenv("HOME"));
wsPath ws_path_log_dir(ws_path_cwd.string() + "/.logs/");

const i_f_hybrid BIAS_POS((23 + 127) << 23); //  1 * 2^23
const i_f_hybrid BIAS_NEG(((23 + 127) << 23) + (1 << 22)); //  1.5 * 2^23

i32 toInt(f32 myFloat) { //  sometimes faster than simple type casting
    i_f_hybrid my(myFloat);
    if (my.i) {
        if (my.i > 0) {
            my.f += BIAS_POS.f; //  Add the bias as a floating point
            my.i -= BIAS_POS.i; //  subtract the bias as an integer
            return my.i;
        }
        my.f += BIAS_NEG.f; //  Add the bias as a floating point
        my.i -= BIAS_NEG.i; //  subtract the bias as an integer
        return my.i;
    }
    return 0;
}

i32 toInt_positive(f32 myFloat) { //  Optimized for known positive values
    i_f_hybrid my(myFloat);
    my.f += BIAS_POS.f; //  Add the bias as a floating point
    my.i -= BIAS_POS.i; //  subtract the bias as an integer
    return my.i;
}

i32 toInt_negative(f32 myFloat) { //  Optimized for known negative values
    i_f_hybrid my(myFloat);
    my.f += BIAS_NEG.f; //  Add the bias as a floating point
    my.i -= BIAS_NEG.i; //  subtract the bias as an integer
    return my.i;
}
