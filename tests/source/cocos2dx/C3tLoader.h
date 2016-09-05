//
//  C3tLoader.h
//  test
//
//  Created by singoon.he on 8/9/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef C3T_LOADER_H
#define C3T_LOADER_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DVector3.h"
#include "Peach3DMesh.h"
#include "Peach3DSkeleton.h"
#include "Peach3DTypes.h"

using namespace Peach3D;
class C3tLoader
{
public:
    /* *.pst is Peach3D Skeleton Text file. */
    static void* c3tMeshDataParse(const ResourceLoaderInput& input);
};

#endif /* C3T_LOADER_H */
