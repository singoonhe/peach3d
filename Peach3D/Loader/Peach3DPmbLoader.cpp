//
//  Peach3DPmbLoader.cpp
//  Peach3DLib
//
//  Created by singoon.he on 2/16/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DPmbLoader.h"

namespace Peach3D
{
    void* PmbLoader::pmbMeshDataParse(const ResourceLoaderInput& input)
    {
        return input.handler;
    }
}