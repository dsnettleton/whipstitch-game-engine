/*
 * wsAsset.h
 *
 *  Created on: Sep 19, 2012
 *      Author: dsnettleton
 *
 *      This file declares the abstract base type wsAsset, which creates and initializes
 *      new gameplay objects onto the stack.
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

#ifndef WS_ASSET_H_
#define WS_ASSET_H_

#include "../wsUtils.h"

#define WS_ASSET_TYPE_MESH  0x0001
#define WS_ASSET_TYPE_ANIM  0x0002
#define WS_ASSET_TYPE_MODEL 0x0003
#define WS_ASSET_TYPE_FONT  0x0004

class wsAsset {
    private:
        static u32 assetIdCounter;
    protected:
        u32 assetId; //  Globally unique identifier for the asset
        u32 assetType;
    public:
        //  Constructor
        wsAsset() {
            assetId = ++assetIdCounter;
            assetType = WS_NULL;
        }
        //  Getter
        u32 getId() { return assetId; }
        u32 getType() { return assetType; }
};

//u32 wsAsset::assetIdCounter = 1;

#endif /* WS_ASSET_H_ */
