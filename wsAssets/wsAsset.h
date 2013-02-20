/*
 * wsAsset.h
 *
 *  Created on: Sep 19, 2012
 *      Author: dsnettleton
 *
 *      This file declares the abstract base type wsAsset, which creates and initializes
 *      new gameplay objects onto the stack.
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
