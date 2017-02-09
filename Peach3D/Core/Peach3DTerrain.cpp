//
//  Peach3DTerrain.cpp
//  Peach3DLib
//
//  Created by singoon.he on 09/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#include "Peach3DTerrain.h"

namespace Peach3D
{
    Terrain::~Terrain()
    {
        if (mHighData) {
            delete mHighData;
            mHighData = nullptr;
        }
    }
    
    bool Terrain::buildTerrain()
    {
        return true;
    }
    
    float Terrain::getCurrentHeight(const Vector3& pos)
    {
        return 0.f;
    }
}
