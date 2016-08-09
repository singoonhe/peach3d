//
//  Peach3DPmbLoader.h
//  Peach3DLib
//
//  Created by singoon.he on 2/16/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PMBLOADER_H
#define PEACH3D_PMBLOADER_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DVector3.h"
#include "Peach3DMesh.h"
#include "Peach3DTypes.h"

namespace Peach3D
{
    class PmbLoader
    {
    public:
        /* *.pmb is Peach3D Mesh Binary file. */
        static void* pmbMeshDataParse(const ResourceLoaderInput& input);
    };
}

#endif /* PEACH3D_PMBLOADER_H */
