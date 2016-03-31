//
//  Peach3DMesh.h
//  Peach3DLib
//
//  Created by singoon he on 12-6-7.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_MESH_H
#define PEACH3D_MESH_H

#include "Peach3DCompile.h"
#include "Peach3DIObject.h"

namespace Peach3D
{
    class PEACH3D_DLL Mesh
    {
    public:
        const char* getName() { return mMeshName.c_str(); }
        ObjectPtr getObjectByName(const char* name);
        /* Create a object with name. If name is nullptr, system will named it. */
        ObjectPtr createObject(const char* name = nullptr);
        /**
         * Traverse objects, will auto call lambda func.
         * This func will used to copy object materials to node.
         */
        void tranverseObjects(std::function<void(const char*, const ObjectPtr&)> callFunc);
        /* Return any object vertex type. */
        uint getAnyVertexType();
        
    protected:
        Mesh(const char* name) : mMeshName(name) {}
        ~Mesh();

    protected:
        std::string     mMeshName;      // mesh name
        std::map<std::string, ObjectPtr> mObjectMap; // all object list
        friend class ResourceManager;   // only ResourceManager can create Mesh
    };
}

#endif // PEACH3D_MESH_H
