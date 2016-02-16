//
//  Peach3DPmtLoader.h
//  Peach3DLib
//
//  Created by singoon.he on 2/16/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PMTLOADER_H
#define PEACH3D_PMTLOADER_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DVector3.h"
#include "Peach3DMesh.h"
#include "tinyxml2/tinyxml2.h"

using namespace::tinyxml2;
namespace Peach3D
{
    class PmtLoader
    {
    public:
        /* *.pmb is Peach3D Mesh Text file. */
        static bool pmtMeshDataParse(uchar* orignData, ulong length, const std::string& dir, Mesh* dMesh);
        
    private:
        /* Read object vertex and index data. */
        static void objVertexDataParse(const XMLElement* objEle, Mesh* dMesh);
        /* Read object material data. */
        static void objMaterialDataParse(const XMLElement* parentEle, IObject* obj);
    };
}

#endif /* PEACH3D_PMTLOADER_H */
