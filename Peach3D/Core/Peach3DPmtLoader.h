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

using namespace tinyxml2;
namespace Peach3D
{
    class PmtLoader
    {
    public:
        /* *.pmb is Peach3D Mesh Text file. */
        static bool pmtMeshDataParse(uchar* orignData, ulong length, const char* dir, const MeshPtr& dMesh);
        
    private:
        /* Read object data, include vertex/index/material. */
        static void objDataParse(const XMLElement* objEle, const char* dir, const MeshPtr& dMesh);
        /* Read object vertex data. */
        static const XMLElement* objVertexDataParse(const XMLElement* prevEle, uint verType, const ObjectPtr& obj);
        /* Read object index data. */
        static const XMLElement* objIndexDataParse(const XMLElement* prevEle, const ObjectPtr& obj);
        /* Read object material data. */
        static const XMLElement* objMaterialDataParse(const XMLElement* prevEle, const char* dir, const ObjectPtr& obj);
    };
}

#endif /* PEACH3D_PMTLOADER_H */
