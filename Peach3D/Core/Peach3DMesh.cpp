//
//  Peach3DIMesh.cpp
//  TestPeach3D
//
//  Created by singoon he on 12-6-7.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DMesh.h"
#include "Peach3DIPlatform.h"

namespace Peach3D
{
    IObject* Mesh::createObject(const char* name)
    {
        const char* newName = name;
        if (!newName)
        {
            // generate mesh name if name==nullptr
            char pName[100] = { 0 };
            static uint meshAutoCount = 0;
            sprintf(pName, "pd_Object%d", meshAutoCount++);
            newName = pName;
        }
        // add Object to mesh
        IObject* childObject = IRender::getSingletonPtr()->createObject(newName);
        mObjectMap[newName] = childObject;
        return childObject;
    }
    
    IObject* Mesh::getObjectByName(const char* name)
    {
        if (mObjectMap.find(name) != mObjectMap.end())
        {
            return mObjectMap[name];
        }
        return nullptr;
    }
    
    void Mesh::tranverseObjects(std::function<void(const char*, IObject*)> callFunc)
    {
        for (auto iter=mObjectMap.begin(); iter!=mObjectMap.end(); ++iter) {
            // tranverse all child with param func
            callFunc(iter->first.c_str(), iter->second);
        }
    }
    
    IObject* Mesh::getRayIntersectObjectWithTranslation(const Matrix4& translate, const Ray& ray)
    {
        for (auto iter=mObjectMap.begin(); iter!=mObjectMap.end(); ++iter) {
            AABB oAABB = iter->second->getAABB(translate);
            if (oAABB.isValid() && oAABB.isRayIntersect(ray)) {
                return iter->second;
            }
        }
        return nullptr;
    }
        
    Mesh::~Mesh()
    {
        IRender* render = IRender::getSingletonPtr();
        for (auto iter=mObjectMap.begin(); iter!=mObjectMap.end(); ++iter)
        {
            // tranverse all child with param func
            render->deleteObject(iter->second);
        }
        mObjectMap.clear();
    }
}
