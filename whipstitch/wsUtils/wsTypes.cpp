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
