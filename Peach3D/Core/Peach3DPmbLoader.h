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

namespace Peach3D
{
    class PmbLoader
    {
    public:
        /* *.pmb is Peach3D Mesh Binary file. */
        static bool pmbMeshDataParse(uchar* orignData, ulong length, const std::string& dir, const MeshPtr& dMesh);
    };
}

#endif /* PEACH3D_PMBLOADER_H */
