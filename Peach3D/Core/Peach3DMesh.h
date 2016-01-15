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
        // return mesh name
        const char* getName() { return mMeshName.c_str(); }
        // create a object with name. If name is nullptr, system will named it
        IObject* createObject(const char* name = nullptr);
        // get object by name
        IObject* getObjectByName(const char* name);
        /**
         * Traverse objects, will auto call lambda func.
         * This func will used to copy object materials to node.
         */
        void tranverseObjects(std::function<void(const char*, IObject*)> callFunc);
        
    protected:
        Mesh(const char* name) : mMeshName(name) {}
        ~Mesh();

    protected:
        std::string     mMeshName;      // mesh name
        std::map<std::string, IObject*> mObjectMap; // all object list
        friend class ResourceManager;   // only ResourceManager can create Mesh
    };
}

#endif // PEACH3D_MESH_H
