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
#include "Peach3DAABB.h"

namespace Peach3D
{
    class SceneNode;
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
        void tranverseObjects(std::function<void(const char*, IObject*)> callFunc)
        {
            for (auto iter=mObjectMap.begin(); iter!=mObjectMap.end(); ++iter)
            {
                // tranverse all child with param func
                callFunc(iter->first.c_str(), iter->second);
            }
        }
        /**
         * @brief Get Object intersect ray.
         * @params translate SceneNode translate matrix.
         * @return First Object will return, nullptr will return if no Object intersect to ray.
         */
        IObject* getRayIntersectObjectWithTranslation(const Matrix4& translate, const Ray& ray);
        // render mesh, render all objects
        void render(const std::vector<SceneNode*>& renderList);
        
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
