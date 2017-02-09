//
//  Peach3DPttLoader.h
//  Peach3DLib
//
//  Created by singoon.he on 09/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PTTLOADER_H
#define PEACH3D_PTTLOADER_H

#include "Peach3DCompile.h"
#include "Peach3DTerrain.h"
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;
namespace Peach3D
{
    class PttLoader
    {
    public:
        /* *.ptt is Peach3D Terrain Text file. */
        static Terrain* pttTerrainParse(const char* file);
    };
}

#endif /* PEACH3D_PTTLOADER_H */
